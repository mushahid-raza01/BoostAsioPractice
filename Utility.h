#ifndef FIX_UTILITY_H

#define FIX_UTILITY_H

#ifdef _MSC_VER
	#pragma warning( disable : 4503 4355 4786 4290 )
#endif

#ifndef _MSC_VER
	#include "Allocator.h"
#endif


#ifdef __cpp_noexcept_function_type
	#define NOEXCEPT noexcept
	#define EXCEPT(...) noexcept(false)
#else
	#define NOEXCEPT throw()
	#define EXCEPT(...) throw(__VA_ARGS__)
#endif


#ifdef HAVE_STLPORT
	#define ALLOCATOR std::allocator
#elif ENABLE_DEBUG_ALLOCATOR
	#include <ext/debug_allocator.h>
	#define ALLOCATOR __gnu_cxx::debug_allocator
#elif ENABLE_NEW_ALLOCATOR
	#include <ext/new_allocator.h>
	#define ALLOCATOR __gnu_cxx::new_allocator
#elif ENABLE_BOOST_FAST_POOL_ALLOCATOR
	#include <boost/pool/pool_alloc.hpp>
	#define ALLOCATOR boost::fast_pool_allocator
#elif ENABLE_MT_ALLOCATOR
	#include <ext/mt_allocator.h>
	#define ALLOCATOR __gnu_cxx::__mt_alloc
#elif ENABLE_BOOST_POOL_ALLOCATOR
	#include <boost/pool/pool_alloc.hpp>
	#define ALLOCATOR boost::pool_allocator
#elif ENABLE_POOL_ALLOCATOR
	#include <ext/pool_allocator.h>
	#define ALLOCATOR __gnu_cxx::__pool_alloc
#elif ENABLE_BITMAP_ALLOCATOR
	#include <ext/bitmap_allocator.h>
	#define ALLOCATOR __gnu_cxx::bitmap_allocator
#elif ENABLE_TBB_ALLOCATOR
	#include <tbb/scalable_allocator.h>
	#define ALLOCATOR tbb::scalable_allocator
#else
	#define ALLOCATOR std::allocator
#endif

#ifdef _MSC_VER
///////////////////////////////////////////////
#include <Windows.h>
#include <process.h>
//////#include <Winsock2.h>
//#include <direct.h>
//#include <time.h>
//
///////////////////////////////////////////////
#else
/////////////////////////////////////////////
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#if defined(__SUNPRO_CC)
#include <sys/filio.h>
#endif
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
/////////////////////////////////////////////
#endif

namespace DBL
{

#if(_MSC_VER >= 1900)
	typedef _beginthreadex_proc_type THREAD_START_ROUTINE;
	#define THREAD_PROC unsigned int _stdcall


#elif(_MSC_VER > 0)
	typedef unsigned int(_stdcall* THREAD_START_ROUTINE)(void*);
	#define  THREAD_PROC unsigned int _stdcall


#else
	extern "C" { typedef void* (THREAD_START_ROUTINE)(void*); }
	#define THREAD_PROC void *
#endif

#ifdef _MSC_VER
	typedef unsigned thread_id;
#else
	typedef pthread_t thread_id;
#endif

	bool thread_spawn(THREAD_START_ROUTINE func, void* var, thread_id& thread);
	bool thread_spawn(THREAD_START_ROUTINE func, void* var);
	void thread_join(thread_id thread);
	void thread_detach(thread_id thread);
	thread_id thread_self();

	void process_sleep(double s);

}

#endif
