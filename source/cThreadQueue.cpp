#include "cThreadQueue.h"

CThreadQueue::CThreadQueue()
{
}

CThreadQueue &CThreadQueue::operator<<( MessageType newMessage )
{
	NewMessage( newMessage, NULL );
	return (*this);
}
CThreadQueue &CThreadQueue::operator <<( char *toPush )
{
	NewMessage( MSG_PRINT, toPush );
	return (*this);
}
bool CThreadQueue::Empty( void )
{
	MutexOn();
	bool retVal = internalQueue.empty();
	MutexOff();
	return retVal;
}
MessagePassed CThreadQueue::GrabMessage( void )
{ 
	MutexOn();	
	MessagePassed toReturn = internalQueue.front();	
	internalQueue.pop();	
	MutexOff();	
	return toReturn;	
}
void CThreadQueue::NewMessage( MessageType toAdd, char *data )
{ 
	MutexOn();
	MessagePassed adding;
	adding.actualMessage = toAdd;
	if( data == NULL )
		adding.data[0] = 0;
	else
		strncpy( adding.data, data, 128 );
	internalQueue.push( adding );
	MutexOff();	
}
