#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  1
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#include "XTAltModule.h"
#include "XTAltInterface.h"
#include "XTAltProcess.h"
#include "XTAltWorker.h"

#ifdef __PCL_WINDOWS
#include <Windows.h>
#else
#include <cstdio>
#include <cstdlib>
#endif

namespace
{

void XTAltDiagnostic( const char* message )
{
#ifdef __PCL_WINDOWS
   wchar_t path[32768];
   DWORD length = ::GetEnvironmentVariableW( L"XTALT_DIAGNOSTIC_LOG", path, DWORD( sizeof( path )/sizeof( path[0] ) ) );
   if ( length == 0 || length >= sizeof( path )/sizeof( path[0] ) )
      return;
   HANDLE file = ::CreateFileW( path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
   if ( file != INVALID_HANDLE_VALUE )
   {
      DWORD written = 0;
      ::WriteFile( file, message, DWORD( strlen( message ) ), &written, nullptr );
      ::WriteFile( file, "\r\n", 2, &written, nullptr );
      ::CloseHandle( file );
   }
#else
   const char* path = std::getenv( "XTALT_DIAGNOSTIC_LOG" );
   if ( path != nullptr && *path != '\0' )
      if ( FILE* file = std::fopen( path, "a" ) )
      {
         std::fprintf( file, "%s\n", message );
         std::fclose( file );
      }
#endif
}

} // namespace

namespace pcl
{

const char* XTAltModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString XTAltModule::Name() const
{
   return "XTAlt";
}

String XTAltModule::Description() const
{
   return "BXT-ALT, SXT-ALT and NXT-ALT native PixInsight processes";
}

String XTAltModule::Company() const
{
   return "Local";
}

String XTAltModule::Author() const
{
   return "XT-ALT";
}

String XTAltModule::Copyright() const
{
   return "Copyright (c) 2026";
}

String XTAltModule::TradeMarks() const
{
   return String();
}

String XTAltModule::OriginalFileName() const
{
#ifdef __PCL_MACOSX
   return "XTAlt-pxm.dylib";
#else
   return "XTAlt-pxm.dll";
#endif
}

void XTAltModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year = 2026;
   month = 10;
   day = 11;
}

void XTAltModule::OnUnload()
{
   XTAltWorker::Instance().Shutdown();
}

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   XTAltDiagnostic( "InstallPixInsightModule entered" );
   new pcl::XTAltModule;
   XTAltDiagnostic( "XTAltModule registered" );

   if ( mode == pcl::InstallMode::FullInstall )
   {
      XTAltDiagnostic( "FullInstall entered" );
      pcl::XTAltProcess* bxt = new pcl::XTAltProcess( pcl::XTProduct::BXT );
      XTAltDiagnostic( "BXTAlt process registered" );
      pcl::XTAltProcess* sxt = new pcl::XTAltProcess( pcl::XTProduct::SXT );
      XTAltDiagnostic( "SXTAlt process registered" );
      pcl::XTAltProcess* nxt = new pcl::XTAltProcess( pcl::XTProduct::NXT );
      XTAltDiagnostic( "NXTAlt process registered" );

      new pcl::XTAltInterface( pcl::XTProduct::BXT, bxt );
      new pcl::XTAltInterface( pcl::XTProduct::SXT, sxt );
      new pcl::XTAltInterface( pcl::XTProduct::NXT, nxt );
      XTAltDiagnostic( "Interfaces registered" );
   }

   XTAltDiagnostic( "InstallPixInsightModule completed" );
   return 0;
}
