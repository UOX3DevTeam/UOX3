#include "threadsafeobject.h"

namespace UOX
{

ThreadSafeObject::ThreadSafeObject()
{
#if UOX_PLATFORM == PLATFORM_WIN32
	d_mutex = CreateMutex( NULL, false, NULL );
#else
	//d_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init( &mutexAttr );

	pthread_mutexattr_settype( &mutexAttr, PTHREAD_MUTEX_RECURSIVE );

	pthread_mutex_init(&d_mutex,&mutexAttr);

	pthread_mutexattr_destroy( &mutexAttr );
#endif
}
ThreadSafeObject::~ThreadSafeObject()
{
#if UOX_PLATFORM == PLATFORM_WIN32
	if( d_mutex != NULL )
	{
		CloseHandle( d_mutex );
		d_mutex = NULL;
	}
#else
	pthread_mutex_unlock(&d_mutex);
	pthread_mutex_destroy(&d_mutex);
#endif
}

void ThreadSafeObject::MutexOn( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	WaitForSingleObject( d_mutex, INFINITE );
#else
	pthread_mutex_lock( &d_mutex );
#endif
}
void ThreadSafeObject::MutexOff( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	ReleaseMutex( d_mutex );
#else
	pthread_mutex_unlock( &d_mutex );
#endif
}

}
