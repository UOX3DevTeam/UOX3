#ifndef __CTHREADQUEUE_H__
#define __CTHREADQUEUE_H__

#include "threadsafeobject.h"

namespace UOX
{

enum MessageType
{
	MSG_WORLDSAVE = 0,
	MSG_SHUTDOWN,
	MSG_UNKNOWN,
	MSG_PRINT,
	MSG_RELOADJS,
	MSG_CONSOLEBCAST,
	MSG_PRINTDONE,
	MSG_PRINTFAILED,
	MSG_SECTIONBEGIN,
	MSG_RELOAD,
	MSG_COUNT
};

struct MessagePassed
{
	MessageType		actualMessage;
	char			data[128];
};

class CThreadQueue : public ThreadSafeObject
{
private:
	std::queue< MessagePassed >	internalQueue;
public:
					CThreadQueue();
	void			NewMessage( MessageType toAdd, const char *data = NULL );
	MessagePassed	GrabMessage( void );
	bool			Empty( void );
	CThreadQueue &	operator<<( MessageType newMessage );
	CThreadQueue &	operator<<( char *toPush );
};

extern CThreadQueue							messageLoop;

}

#endif

