#ifndef __XTAltWorker_h
#define __XTAltWorker_h

#include <pcl/ByteArray.h>
#include <pcl/ExternalProcess.h>
#include <pcl/Image.h>

#include <atomic>
#include <memory>
#include <mutex>

namespace pcl
{

class XTAltInstance;

class XTAltWorker
{
public:
   static XTAltWorker& Instance();

   void Process( Image&, const XTAltInstance&, Image* starsOutput );
   void Abort();
   void Shutdown();

private:
   XTAltWorker() = default;
   ~XTAltWorker();
   XTAltWorker( const XTAltWorker& ) = delete;
   XTAltWorker& operator =( const XTAltWorker& ) = delete;

   void EnsureStarted();
   void StopProcess( bool force );
   String FindInstallationRoot() const;
   IsoString BuildRequestJson( const Image&, const XTAltInstance& ) const;
   void CheckCancellation( uint64 requestId );
   void DrainOutput( ByteArray&, ByteArray& );

   std::mutex m_requestMutex;
   std::mutex m_processMutex;
   std::unique_ptr<ExternalProcess> m_process;
   std::atomic<uint64> m_generation { 0 };
   bool m_requestActive = false; // guarded by m_processMutex
};

} // pcl

#endif
