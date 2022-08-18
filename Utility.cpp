#include "Utility.h"

#ifdef _MSC_VER

	#if _MSC_VER > 1000
		#pragma once
	#endif // _MSC_VER > 1000

#endif

namespace DBL
{
	bool thread_spawn(THREAD_START_ROUTINE func, void* var, thread_id& thread)
	{
	#ifdef _MSC_VER
			thread_id result = 0;
			unsigned int id = 0;
			result = _beginthreadex(NULL, 0, func, var, 0, &id);
			if (result == 0) return false;
	#else
		thread_id result = 0;
		if (pthread_create(&result, 0, func, var) != 0) return false;
	#endif
		thread = result;
		return true;
	}

	bool thread_spawn(THREAD_START_ROUTINE func, void* var)
	{
		thread_id thread = 0;
		return thread_spawn(func, var, thread);
	}

	void thread_join(thread_id thread)
	{
	#ifdef _MSC_VER
			WaitForSingleObject((void*)thread, INFINITE);
			CloseHandle((HANDLE)thread);
	#else
		pthread_join((pthread_t)thread, 0);
#endif
	}

	void thread_detach(thread_id thread)
	{
#ifdef _MSC_VER
		CloseHandle((HANDLE)thread);
#else
		pthread_t t = thread;
		pthread_detach(t);
#endif
	}

	thread_id thread_self()
	{
#ifdef _MSC_VER
		return (unsigned)GetCurrentThread();
#else
		return pthread_self();
#endif
	}

	void process_sleep(double s)
	{
#ifdef _MSC_VER
		Sleep((long)(s * 1000));
#else
		timespec time, remainder;
		double intpart;
		time.tv_nsec = (long)(modf(s, &intpart) * 1e9);
		time.tv_sec = (int)intpart;
		while (nanosleep(&time, &remainder) == -1)
			time = remainder;
#endif
	}
}