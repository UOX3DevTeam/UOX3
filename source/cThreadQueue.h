#ifndef __CTHREADQUEUE_H__
#define __CTHREADQUEUE_H__

#include <string>
#include <queue>
#include <mutex>

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
	std::string			data;
};

class CThreadQueue
{
private:
	std::queue< MessagePassed >	internalQueue;
	std::mutex queuelock;
public:
	CThreadQueue();
	void			NewMessage( MessageType toAdd, const std::string& data="" );
	MessagePassed	GrabMessage( void );
	bool			Empty( void );
	CThreadQueue &	operator<<( MessageType newMessage );
	CThreadQueue &	operator<<( char *toPush );
	CThreadQueue &  operator<<( const char *toPush );
	CThreadQueue &  operator<<( const std::string& toPush );
};

extern CThreadQueue							messageLoop;

#endif

