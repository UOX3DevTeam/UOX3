#ifndef __THREAD_SAFE_OBJ__
#define __THREAD_SAFE_OBJ__

namespace UOX
{

#if UOX_PLATFORM == PLATFORM_WIN32
	#include <windows.h>
#else
	#define _GNU_SOURCE 1	// this is a hack
				// it should've been defined by default
				// but it wasn't
	#include <pthread.h>
#endif

class ThreadSafeObject
{
public:
			ThreadSafeObject();
	virtual	~ThreadSafeObject();

	void On( void )
	{
		MutexOn();
	}
	void Off( void )
	{
		MutexOff();
	}
protected:
#if UOX_PLATFORM == PLATFORM_WIN32
	HANDLE d_mutex;
#else
	pthread_mutex_t d_mutex;
#endif
	void MutexOn( void );
	void MutexOff( void );
};

}

#endif

