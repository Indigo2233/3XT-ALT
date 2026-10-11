#ifndef __XTAltProcess_h
#define __XTAltProcess_h

#include <pcl/Image.h>
#include <pcl/MetaParameter.h>
#include <pcl/MetaProcess.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

enum class XTProduct
{
   BXT,
   SXT,
   NXT
};

IsoString XTProductProcessId( XTProduct );
String XTProductTitle( XTProduct );
IsoString XTProductWorkerId( XTProduct );

class XTAltRealParameter : public MetaDouble
{
public:
   XTAltRealParameter( MetaProcess*, const IsoString&, double, double, double, int );
   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;

private:
   IsoString m_id;
   double m_minimum;
   double m_maximum;
   double m_default;
   int m_precision;
};

class XTAltUIntParameter : public MetaUInt32
{
public:
   XTAltUIntParameter( MetaProcess*, const IsoString&, uint32, uint32, uint32 );
   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;

private:
   IsoString m_id;
   uint32 m_minimum;
   uint32 m_maximum;
   uint32 m_default;
};

class XTAltBoolParameter : public MetaBoolean
{
public:
   XTAltBoolParameter( MetaProcess*, const IsoString&, bool );
   IsoString Id() const override;
   bool DefaultValue() const override;

private:
   IsoString m_id;
   bool m_default;
};

class XTAltDeviceParameter : public MetaEnumeration
{
public:
   enum { Auto, DirectML, CPU, NumberOfItems, Default = Auto };

   explicit XTAltDeviceParameter( MetaProcess* );
   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

struct XTAltParameterSet
{
   explicit XTAltParameterSet( MetaProcess*, XTProduct );

   XTAltDeviceParameter* device = nullptr;
   XTAltRealParameter* overlap = nullptr;

   XTAltRealParameter* ss = nullptr;
   XTAltRealParameter* ash = nullptr;
   XTAltBoolParameter* ansp = nullptr;
   XTAltRealParameter* nsd = nullptr;
   XTAltRealParameter* sn = nullptr;
   XTAltBoolParameter* correctOnly = nullptr;
   XTAltBoolParameter* lunarPlanetary = nullptr;

   XTAltBoolParameter* stars = nullptr;
   XTAltBoolParameter* unscreen = nullptr;

   XTAltRealParameter* dn = nullptr;
   XTAltUIntParameter* iterations = nullptr;
   XTAltBoolParameter* csep = nullptr;
   XTAltBoolParameter* fsep = nullptr;
   XTAltRealParameter* di = nullptr;
   XTAltRealParameter* dc = nullptr;
   XTAltRealParameter* dhf = nullptr;
   XTAltRealParameter* dlf = nullptr;
   XTAltRealParameter* dihf = nullptr;
   XTAltRealParameter* dilf = nullptr;
   XTAltRealParameter* dchf = nullptr;
   XTAltRealParameter* dclf = nullptr;
   XTAltRealParameter* fs = nullptr;
};

class XTAltProcess : public MetaProcess
{
public:
   explicit XTAltProcess( XTProduct );

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool NeedsValidation() const override;

   XTProduct Product() const { return m_product; }
   const XTAltParameterSet& Parameters() const { return m_parameters; }

private:
   XTProduct m_product;
   XTAltParameterSet m_parameters;
};

class XTAltInstance : public ProcessImplementation
{
public:
   explicit XTAltInstance( const XTAltProcess* );
   XTAltInstance( const XTAltInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, String& ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type ) override;

   XTProduct Product() const { return m_product; }
   const XTAltProcess* ProcessMeta() const;
   void NormalizeParameters();
   void ApplyTo( Image&, Image* starsOutput = nullptr ) const;

private:
   XTProduct m_product;

   pcl_enum p_device = XTAltDeviceParameter::Default;
   double p_overlap = 0.2;

   double p_ss = 0.5;
   double p_ash = 0;
   pcl_bool p_ansp = true;
   double p_nsd = 0;
   double p_sn = 0.5;
   pcl_bool p_correctOnly = false;
   pcl_bool p_lunarPlanetary = false;

   pcl_bool p_stars = false;
   pcl_bool p_unscreen = false;

   double p_dn = 0.9;
   uint32 p_iterations = 2;
   pcl_bool p_csep = false;
   pcl_bool p_fsep = false;
   double p_di = 0.9;
   double p_dc = 0.9;
   double p_dhf = 0.9;
   double p_dlf = 0.9;
   double p_dihf = 0.9;
   double p_dilf = 0.9;
   double p_dchf = 0.9;
   double p_dclf = 0.9;
   double p_fs = 5;

   friend class XTAltWorker;
   friend class XTAltInterface;
};

extern XTAltProcess* TheBXTAltProcess;
extern XTAltProcess* TheSXTAltProcess;
extern XTAltProcess* TheNXTAltProcess;

} // pcl

#endif
