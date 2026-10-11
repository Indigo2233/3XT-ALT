#include "XTAltProcess.h"
#include "XTAltInterface.h"
#include "XTAltWorker.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ImageVariant.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>

namespace pcl
{

XTAltProcess* TheBXTAltProcess = nullptr;
XTAltProcess* TheSXTAltProcess = nullptr;
XTAltProcess* TheNXTAltProcess = nullptr;

IsoString XTProductProcessId( XTProduct product )
{
   switch ( product )
   {
   case XTProduct::BXT: return "BXTAlt";
   case XTProduct::SXT: return "SXTAlt";
   default:             return "NXTAlt";
   }
}

String XTProductTitle( XTProduct product )
{
   switch ( product )
   {
   case XTProduct::BXT: return "BXT-ALT";
   case XTProduct::SXT: return "SXT-ALT";
   default:             return "NXT-ALT";
   }
}

IsoString XTProductWorkerId( XTProduct product )
{
   switch ( product )
   {
   case XTProduct::BXT: return "bxt";
   case XTProduct::SXT: return "sxt";
   default:             return "nxt";
   }
}

XTAltRealParameter::XTAltRealParameter( MetaProcess* process, const IsoString& id,
                                       double minimum, double maximum, double defaultValue,
                                       int precision )
   : MetaDouble( process )
   , m_id( id )
   , m_minimum( minimum )
   , m_maximum( maximum )
   , m_default( defaultValue )
   , m_precision( precision )
{
}

IsoString XTAltRealParameter::Id() const { return m_id; }
int XTAltRealParameter::Precision() const { return m_precision; }
double XTAltRealParameter::MinimumValue() const { return m_minimum; }
double XTAltRealParameter::MaximumValue() const { return m_maximum; }
double XTAltRealParameter::DefaultValue() const { return m_default; }

XTAltUIntParameter::XTAltUIntParameter( MetaProcess* process, const IsoString& id,
                                       uint32 minimum, uint32 maximum, uint32 defaultValue )
   : MetaUInt32( process )
   , m_id( id )
   , m_minimum( minimum )
   , m_maximum( maximum )
   , m_default( defaultValue )
{
}

IsoString XTAltUIntParameter::Id() const { return m_id; }
double XTAltUIntParameter::MinimumValue() const { return m_minimum; }
double XTAltUIntParameter::MaximumValue() const { return m_maximum; }
double XTAltUIntParameter::DefaultValue() const { return m_default; }

XTAltBoolParameter::XTAltBoolParameter( MetaProcess* process, const IsoString& id, bool defaultValue )
   : MetaBoolean( process )
   , m_id( id )
   , m_default( defaultValue )
{
}

IsoString XTAltBoolParameter::Id() const { return m_id; }
bool XTAltBoolParameter::DefaultValue() const { return m_default; }

XTAltDeviceParameter::XTAltDeviceParameter( MetaProcess* process )
   : MetaEnumeration( process )
{
}

IsoString XTAltDeviceParameter::Id() const { return "device"; }
size_type XTAltDeviceParameter::NumberOfElements() const { return NumberOfItems; }

IsoString XTAltDeviceParameter::ElementId( size_type index ) const
{
   switch ( index )
   {
   case Auto:     return "Auto";
   case DirectML: return "DirectML";
   default:       return "CPU";
   }
}

int XTAltDeviceParameter::ElementValue( size_type index ) const
{
   return int( index );
}

size_type XTAltDeviceParameter::DefaultValueIndex() const
{
   return Default;
}

XTAltParameterSet::XTAltParameterSet( MetaProcess* process, XTProduct product )
{
   device = new XTAltDeviceParameter( process );
   overlap = new XTAltRealParameter( process, "overlap", 0, 0.5, 0.2, 2 );

   if ( product == XTProduct::BXT )
   {
      ss = new XTAltRealParameter( process, "sharpenStars", 0, 0.7, 0.5, 2 );
      ash = new XTAltRealParameter( process, "adjustStarHalos", -0.5, 0.5, 0, 2 );
      ansp = new XTAltBoolParameter( process, "automaticPSF", true );
      nsd = new XTAltRealParameter( process, "psfDiameter", 0, 8, 0, 1 );
      sn = new XTAltRealParameter( process, "sharpenNonstellar", 0, 1, 0.5, 2 );
      correctOnly = new XTAltBoolParameter( process, "correctOnly", false );
      lunarPlanetary = new XTAltBoolParameter( process, "lunarPlanetary", false );
   }
   else if ( product == XTProduct::SXT )
   {
      stars = new XTAltBoolParameter( process, "generateStars", false );
      unscreen = new XTAltBoolParameter( process, "unscreenStars", false );
   }
   else
   {
      dn = new XTAltRealParameter( process, "denoise", 0, 1, 0.9, 2 );
      iterations = new XTAltUIntParameter( process, "iterations", 1, 5, 2 );
      csep = new XTAltBoolParameter( process, "intensityColorSeparation", false );
      fsep = new XTAltBoolParameter( process, "frequencySeparation", false );
      di = new XTAltRealParameter( process, "denoiseIntensity", 0, 1, 0.9, 2 );
      dc = new XTAltRealParameter( process, "denoiseColor", 0, 1, 0.9, 2 );
      dhf = new XTAltRealParameter( process, "denoiseHF", 0, 1, 0.9, 2 );
      dlf = new XTAltRealParameter( process, "denoiseLF", 0, 1, 0.9, 2 );
      dihf = new XTAltRealParameter( process, "denoiseHFIntensity", 0, 1, 0.9, 2 );
      dilf = new XTAltRealParameter( process, "denoiseLFIntensity", 0, 1, 0.9, 2 );
      dchf = new XTAltRealParameter( process, "denoiseHFColor", 0, 1, 0.9, 2 );
      dclf = new XTAltRealParameter( process, "denoiseLFColor", 0, 1, 0.9, 2 );
      fs = new XTAltRealParameter( process, "frequencyScale", 1, 100, 5, 1 );
   }
}

XTAltProcess::XTAltProcess( XTProduct product )
   : m_product( product )
   , m_parameters( this, product )
{
   switch ( product )
   {
   case XTProduct::BXT: TheBXTAltProcess = this; break;
   case XTProduct::SXT: TheSXTAltProcess = this; break;
   case XTProduct::NXT: TheNXTAltProcess = this; break;
   }
}

IsoString XTAltProcess::Id() const { return XTProductProcessId( m_product ); }
IsoString XTAltProcess::Category() const { return "XTAlt"; }
uint32 XTAltProcess::Version() const { return 0x100; }

String XTAltProcess::Description() const
{
   return XTProductTitle( m_product ) + " local ONNX image processing";
}

ProcessInterface* XTAltProcess::DefaultInterface() const
{
   switch ( m_product )
   {
   case XTProduct::BXT: return TheBXTAltInterface;
   case XTProduct::SXT: return TheSXTAltInterface;
   default:             return TheNXTAltInterface;
   }
}

ProcessImplementation* XTAltProcess::Create() const
{
   return new XTAltInstance( this );
}

ProcessImplementation* XTAltProcess::Clone( const ProcessImplementation& implementation ) const
{
   const XTAltInstance* instance = dynamic_cast<const XTAltInstance*>( &implementation );
   return instance != nullptr && instance->Product() == m_product ? new XTAltInstance( *instance ) : nullptr;
}

bool XTAltProcess::NeedsValidation() const
{
   return false;
}

XTAltInstance::XTAltInstance( const XTAltProcess* process )
   : ProcessImplementation( process )
   , m_product( process->Product() )
{
}

XTAltInstance::XTAltInstance( const XTAltInstance& source )
   : ProcessImplementation( source )
   , m_product( source.m_product )
{
   Assign( source );
}

void XTAltInstance::Assign( const ProcessImplementation& implementation )
{
   const XTAltInstance* source = dynamic_cast<const XTAltInstance*>( &implementation );
   if ( source == nullptr || source->m_product != m_product )
      return;

   p_device = source->p_device;
   p_overlap = source->p_overlap;
   p_ss = source->p_ss;
   p_ash = source->p_ash;
   p_ansp = source->p_ansp;
   p_nsd = source->p_nsd;
   p_sn = source->p_sn;
   p_correctOnly = source->p_correctOnly;
   p_lunarPlanetary = source->p_lunarPlanetary;
   p_stars = source->p_stars;
   p_unscreen = source->p_unscreen;
   p_dn = source->p_dn;
   p_iterations = source->p_iterations;
   p_csep = source->p_csep;
   p_fsep = source->p_fsep;
   p_di = source->p_di;
   p_dc = source->p_dc;
   p_dhf = source->p_dhf;
   p_dlf = source->p_dlf;
   p_dihf = source->p_dihf;
   p_dilf = source->p_dilf;
   p_dchf = source->p_dchf;
   p_dclf = source->p_dclf;
   p_fs = source->p_fs;
   NormalizeParameters();
}

UndoFlags XTAltInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

bool XTAltInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
   {
      whyNot = XTProductTitle( m_product ) + " does not support complex images.";
      return false;
   }
   int channels = image.NumberOfNominalChannels();
   if ( channels != 1 && channels != 3 )
   {
      whyNot = XTProductTitle( m_product ) + " requires a grayscale or RGB image.";
      return false;
   }
   return true;
}

void XTAltInstance::NormalizeParameters()
{
   if ( p_correctOnly )
      p_lunarPlanetary = false;
   if ( !p_stars )
      p_unscreen = false;
}

const XTAltProcess* XTAltInstance::ProcessMeta() const
{
   return static_cast<const XTAltProcess*>( Meta() );
}

void XTAltInstance::ApplyTo( Image& image, Image* starsOutput ) const
{
   XTAltInstance normalized( *this );
   normalized.NormalizeParameters();
   XTAltWorker::Instance().Process( image, normalized, starsOutput );
}

bool XTAltInstance::ExecuteOn( View& view )
{
   String whyNot;
   if ( !CanExecuteOn( view, whyNot ) )
      throw Error( whyNot );

   AutoViewLock lock( view );
   Console console;
   console.EnableAbort();
   console.WriteLn( "<end><cbr><br>" + XTProductTitle( m_product ) + ": " + view.FullId().UTF8ToUTF16() );

   Image result;
   ImageVariant resultVariant( &result );
   resultVariant.AssignImage( view.Image() );

   Image stars;
   ApplyTo( result, m_product == XTProduct::SXT && p_stars ? &stars : nullptr );

   ImageVariant target = view.Image();
   target.AssignImage( resultVariant );

   if ( !stars.IsEmpty() )
   {
      IsoString id = view.Id() + "_stars_alt";
      ImageWindow output( stars.Width(), stars.Height(), stars.NumberOfChannels(),
                          32, true, stars.IsColor(), true, id );
      ImageVariant outputImage = output.MainView().Image();
      ImageVariant starsVariant( &stars );
      outputImage.AssignImage( starsVariant );
      output.Show();
      output.ZoomToFit();
   }

   console.WriteLn( "<end><cbr>Completed." );
   return true;
}

void* XTAltInstance::LockParameter( const MetaParameter* parameter, size_type )
{
   const XTAltParameterSet& p = ProcessMeta()->Parameters();
   if ( parameter == p.device ) return &p_device;
   if ( parameter == p.overlap ) return &p_overlap;
   if ( parameter == p.ss ) return &p_ss;
   if ( parameter == p.ash ) return &p_ash;
   if ( parameter == p.ansp ) return &p_ansp;
   if ( parameter == p.nsd ) return &p_nsd;
   if ( parameter == p.sn ) return &p_sn;
   if ( parameter == p.correctOnly ) return &p_correctOnly;
   if ( parameter == p.lunarPlanetary ) return &p_lunarPlanetary;
   if ( parameter == p.stars ) return &p_stars;
   if ( parameter == p.unscreen ) return &p_unscreen;
   if ( parameter == p.dn ) return &p_dn;
   if ( parameter == p.iterations ) return &p_iterations;
   if ( parameter == p.csep ) return &p_csep;
   if ( parameter == p.fsep ) return &p_fsep;
   if ( parameter == p.di ) return &p_di;
   if ( parameter == p.dc ) return &p_dc;
   if ( parameter == p.dhf ) return &p_dhf;
   if ( parameter == p.dlf ) return &p_dlf;
   if ( parameter == p.dihf ) return &p_dihf;
   if ( parameter == p.dilf ) return &p_dilf;
   if ( parameter == p.dchf ) return &p_dchf;
   if ( parameter == p.dclf ) return &p_dclf;
   if ( parameter == p.fs ) return &p_fs;
   return nullptr;
}

} // pcl
