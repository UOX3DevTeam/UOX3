#include "cThreadQueue.h"
#include <algorithm>

//==============================================================================
// We instantiate this right at the start 
CThreadQueue messageLoop;

//=============================================================
auto CThreadQueue::operator << ( MessageType newMessage ) -> CThreadQueue &
{
	NewMessage( newMessage, "" );
	return ( *this );
}
//=============================================================
auto CThreadQueue::operator << ( char *toPush ) -> CThreadQueue &
{
	NewMessage( MSG_PRINT, toPush );
	return ( *this );
}
//=============================================================
auto CThreadQueue::operator << ( const char *toPush ) -> CThreadQueue &
{
	NewMessage( MSG_PRINT, toPush );
	return ( *this );
}
//=============================================================
auto CThreadQueue::operator << ( const std::string& toPush ) -> CThreadQueue &
{
	NewMessage( MSG_PRINT, toPush.c_str() );
	return ( *this );
}
//=============================================================
auto CThreadQueue::Empty() -> bool
{
	std::scoped_lock lock( queuelock );
	bool retVal = internalQueue.empty();
	return retVal;
}
//=============================================================
auto CThreadQueue::GrabMessage() -> MessagePassed_st
{
	std::scoped_lock lock( queuelock );
	MessagePassed_st toReturn = internalQueue.front();
	internalQueue.pop();

	return toReturn;
}
//=============================================================
auto CThreadQueue::NewMessage( MessageType toAdd, const std::string& data ) -> void
{
	MessagePassed_st adding;
	adding.actualMessage = toAdd;
	adding.data = data;
	std::scoped_lock lock( queuelock );
	internalQueue.push( adding );
}
//=============================================================
auto CThreadQueue::BulkData() -> std::queue<MessagePassed_st>
{
	std::queue<MessagePassed_st> returnQueue;
	std::scoped_lock lock( queuelock );
	std::swap( returnQueue, internalQueue );
	return returnQueue;
}
