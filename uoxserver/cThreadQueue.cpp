#include "cThreadQueue.h"

CThreadQueue					messageLoop;

CThreadQueue::CThreadQueue()
{
}

CThreadQueue &CThreadQueue::operator<<( MessageType newMessage )
{
	NewMessage( newMessage, "" );
	return (*this);
}
CThreadQueue &CThreadQueue::operator <<( char *toPush )
{
	NewMessage( MSG_PRINT, toPush );
	return (*this);
}
CThreadQueue &CThreadQueue::operator <<( const char *toPush )
{
	NewMessage( MSG_PRINT, toPush );
	return ( *this );
}
CThreadQueue &CThreadQueue::operator <<( const std::string& toPush )
{
	NewMessage( MSG_PRINT, toPush.c_str() );
	return ( *this );
}
bool CThreadQueue::Empty( void )
{
	std::scoped_lock lock(queuelock);
	bool retVal = internalQueue.empty();
	return retVal;
}
MessagePassed CThreadQueue::GrabMessage( void )
{
	std::scoped_lock lock(queuelock);
	MessagePassed toReturn = internalQueue.front();
	internalQueue.pop();

	return toReturn;
}
void CThreadQueue::NewMessage( MessageType toAdd, const std::string& data )
{
	std::scoped_lock lock(queuelock);
	MessagePassed adding;
	adding.actualMessage = toAdd;
	adding.data = data ;
	internalQueue.push( adding );

}
