#ifndef __CTHREADQUEUE_H__
#define __CTHREADQUEUE_H__

#include <string>
#include <queue>
#include <mutex>

//o------------------------------------------------------------------------------------------------o
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

struct MessagePassed_st
{
	MessageType	 actualMessage;
	std::string	 data;
};

//o------------------------------------------------------------------------------------------------o
class CThreadQueue
{
private:
	std::queue<MessagePassed_st> internalQueue;
	std::mutex queuelock;
public:
	CThreadQueue() = default;
	auto NewMessage( MessageType toAdd, const std::string& data = "" ) -> void;
	auto GrabMessage()->MessagePassed_st;
	auto Empty() -> bool;
	auto operator<<( MessageType newMessage ) -> CThreadQueue &;
	auto operator<<( char *toPush ) -> CThreadQueue &;
	auto operator<<( const char *toPush ) -> CThreadQueue &;
	auto operator<<( const std::string& toPush ) -> CThreadQueue &;
	auto BulkData() -> std::queue<MessagePassed_st>;
};

extern CThreadQueue messageLoop;

#endif

