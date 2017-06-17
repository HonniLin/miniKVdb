#ifndef OSSUTIL_HPP__
#define OSSUTIL_HPP__
#include "core.hpp"
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/xtime.hpp>
#if defined _WINDOWS
#include <TlHelp32.h>
#endif

#if defined _WINDOWS
inline void ossSleepmicros ( unsigned int s )
{
   boost::xtime xt ;
   boost::xtime_get ( &xt, boost::TIME_UTC_ ) ;
   xt.sec += (int)(s/1000000) ;
   xt.nsec += (int)((s%1000000)*1000) ;
   if ( xt.nsec >= 1000000000 )
   {
      xt.nsec -= 1000000000 ;
      xt.sec ++ ;
   }
   boost::thread::sleep(xt) ;
}
#else
inline void ossSleepmicros ( unsigned int s ) //等待微秒
{
   struct timespec t ;
   t.tv_sec = (time_t) (s/1000000) ;
   t.tv_nsec = 1000 * (s % 1000000 ) ;
   while ( nanosleep ( &t, &t )==-1 && errno==EINTR ) ;
}
#endif
inline void ossSleepmillis ( unsigned int s ) //等待毫秒
{
   ossSleepmicros ( s * 1000 ) ;
}

#if defined _WINDOWS
typedef DWORD     OSSPID ;
typedef DWORD     OSSTID ;
#else
typedef pid_t     OSSPID ; //进程id
typedef pthread_t OSSTID ; //线程id
#endif
inline OSSPID ossGetParentProcessID () //得到父进程id
{
#if defined _WINDOWS
   OSSPID pid = -1 ;
   OSSPID ppid = -1 ;
   HANDLE hSnapshot = 0 ;
   PROCESSENTRY32 pe = {0} ;
   pe.dwSize = sizeof(PROCESSENTRY32) ;
   hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ;
   if ( hSnapshot == 0 )
	   goto error ;
   pid = GetCurrentProcessId () ;
   if ( Process32First ( hSnapshot, &pe ) )
   {
	   do
	   {
		   if ( pe.th32ProcessID == pid )
		   {
			   ppid = pe.th32ParentProcessID ;
			   goto done ;
		   }
	   } while ( Process32Next ( hSnapshot, &pe ) ) ;
   }
done:
   return ppid ;
error :
   goto done ;
#else
   return getppid () ;
#endif
}

inline OSSPID ossGetCurrentProcessID ()
{
#if defined _WINDOWS
   return GetCurrentProcessId () ;
#else
   return getpid () ;
#endif
}

inline OSSTID ossGetCurrentThreadID ()
{
#if defined _WINDOWS
   return GetCurrentThreadId () ;
#else
   return syscall(SYS_gettid) ;
#endif
}

#endif
