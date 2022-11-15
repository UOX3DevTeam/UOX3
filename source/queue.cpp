//o------------------------------------------------------------------------------------------------o
//| File		-	Queue.cpp
//| Date		-	March 15th, 2000
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Implementation (vector based) of the GM and counselor queue class
//o------------------------------------------------------------------------------------------------o

#include "uox3.h"
#include "PageVector.h"
#include "gump.h"

PageVector *GMQueue;
PageVector *CounselorQueue;

inline bool operator==( const CHelpRequest& x, const CHelpRequest& y )
{
	return ( x.Priority() == y.Priority() );
}

inline bool operator<( const CHelpRequest& x, const CHelpRequest& y )
{
	return ( x.Priority() < y.Priority() );
}

inline bool operator>( const CHelpRequest& x, const CHelpRequest& y )
{
	return ( x.Priority() > y.Priority() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::AtEnd()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether position has reached end of queue
//o------------------------------------------------------------------------------------------------o
bool PageVector::AtEnd( void ) const
{
	return ( currentPos == requestQueue.end() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::PageVector()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for PageVector class - initializes an empty queue
//o------------------------------------------------------------------------------------------------o
PageVector::PageVector() : maxId( 0 )
{
	title = "";
	requestQueue.resize( 0 );
	currentPos = requestQueue.end();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::~PageVector()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deconstructor for PageVector class - kills entire queue
//o------------------------------------------------------------------------------------------------o
PageVector::~PageVector()
{
	KillQueue();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::Add()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds help request from player to page queue
//o------------------------------------------------------------------------------------------------o
SERIAL PageVector::Add( CHelpRequest *toAdd )
{
	CHelpRequest *adding = new CHelpRequest;
#if defined( UOX_DEBUG_MODE )
	if( adding == nullptr )
		return INVALIDSERIAL;
#endif
	// Note: Bad idea to use memcpy to copy one class object to another (especially if the class contains an std::string)?"
	//memcpy( adding, toAdd, sizeof( CHelpRequest ));
	*adding = *toAdd; // Maybe we should do this instead?
	requestQueue.push_back( adding );
	adding->RequestId( ++maxId );
	std::sort( requestQueue.begin(), requestQueue.end() );
	return adding->RequestId();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::Remove()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes help request from player from queue
//o------------------------------------------------------------------------------------------------o
bool PageVector::Remove( void )
{
	if( AtEnd() )
		return false;

	delete ( *currentPos );
	requestQueue.erase( currentPos );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::First()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets first help request in queue, if any exists
//o------------------------------------------------------------------------------------------------o
CHelpRequest *PageVector::First( void )
{
	currentPos = requestQueue.begin();
	if( AtEnd() )
		return nullptr;	// empty queue!

	return ( *currentPos );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::Next()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets next help request in queue, if any exists
//o------------------------------------------------------------------------------------------------o
CHelpRequest *PageVector::Next( void )
{
	++currentPos;
	if( AtEnd() )
		return nullptr;	// at end, return nullptr!

	return ( *currentPos );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::NumEntries()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of help requests in queue
//o------------------------------------------------------------------------------------------------o
size_t PageVector::NumEntries( void ) const
{
	return requestQueue.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::KillQueue()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears all help requests from queue
//o------------------------------------------------------------------------------------------------o
void PageVector::KillQueue( void )
{
	for( size_t counter = 0; counter < requestQueue.size(); ++counter )
	{
		delete requestQueue[counter];
	}
	requestQueue.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::SendAsGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a list of help requests in queue to client and displays it in a Gump
//o------------------------------------------------------------------------------------------------o
void PageVector::SendAsGump( CSocket *toSendTo )
{
	CGumpDisplay GQueue( toSendTo, 320, 340 );
	GQueue.SetTitle( title );

	std::vector<CHelpRequest *>::iterator qIter;
	for( qIter = requestQueue.begin(); qIter != requestQueue.end(); ++qIter )
	{
		if(( *qIter ) == nullptr )
			continue;

		if( !( *qIter )->IsHandled() )
		{
			CChar *mChar = CalcCharObjFromSer(( *qIter )->WhoPaging() );
			if( ValidateObject( mChar ))
			{
				GQueue.AddData( mChar->GetName(), static_cast<UI32>(( *qIter )->TimeOfPage() ));
			}
		}
	}
	GQueue.Send( 4, false, INVALIDSERIAL );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::SetTitle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets title displayed for help request queue in client
//o------------------------------------------------------------------------------------------------o
void PageVector::SetTitle( const std::string &newTitle )
{
	title = newTitle;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::PageVector()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializer for PageVector class
//o------------------------------------------------------------------------------------------------o
PageVector::PageVector( const std::string &newTitle )
{
	SetTitle( newTitle );
	requestQueue.resize( 0 );
	currentPos = requestQueue.end();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::GotoPos()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets queue position to specified value
//o------------------------------------------------------------------------------------------------o
bool PageVector::GotoPos( SI32 pos )
{
	if( pos < 0 || static_cast<UI32>( pos ) >= requestQueue.size() )
		return false;

	currentPos = ( requestQueue.begin() + pos );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::CurrentPos()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets current queue position
//o------------------------------------------------------------------------------------------------o
SI32 PageVector::CurrentPos( void ) const
{
	return static_cast<SI32>(( currentPos - requestQueue.begin() ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::GetCallNum()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets call number/help request ID for current position in queue
//o------------------------------------------------------------------------------------------------o
SERIAL PageVector::GetCallNum( void ) const
{
	if( AtEnd() )
		return INVALIDSERIAL;

	return ( *currentPos )->RequestId();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::FindCallNum()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets position of page with specified call number/help request ID
//o------------------------------------------------------------------------------------------------o
SI32 PageVector::FindCallNum( SERIAL callNum )
{
	for( size_t counter = 0; counter < requestQueue.size(); ++counter )
	{
		if( requestQueue[counter]->RequestId() == callNum )
			return static_cast<SI32>( counter );
	}
	return -1;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::Current()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets current position in queue
//o------------------------------------------------------------------------------------------------o
CHelpRequest *PageVector::Current( void )
{
	if( !AtEnd() )
		return ( *currentPos );
	else
		return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PageVector::AnswerNextCall()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Marks next help request in queue as handled and sends it to GM/Counselor
//o------------------------------------------------------------------------------------------------o
bool PageVector::AnswerNextCall( CSocket *mSock, CChar *mChar )
{
	bool retVal		= false;
	CChar *isPaging = nullptr;
	for( CHelpRequest *tempPage = First(); !AtEnd(); tempPage = Next() )
	{
		if( !tempPage->IsHandled() )
		{
			isPaging = CalcCharObjFromSer( tempPage->WhoPaging() );
			if( ValidateObject( isPaging ))
			{
				CGumpDisplay QNext( mSock, 300, 200 );
				QNext.AddData( "Pager: ", isPaging->GetName() );
				QNext.AddData( "Problem: ", tempPage->Reason() );
				QNext.AddData( "Serial number ", tempPage->WhoPaging(), 3 );
				QNext.AddData( "Paged at: ", static_cast<UI32>( tempPage->TimeOfPage() ));
				QNext.Send( 4, false, INVALIDSERIAL );
				tempPage->IsHandled( true );
				if( mChar->WorldNumber() != isPaging->WorldNumber() )
				{
					mChar->SetLocation( isPaging );
					SendMapChange( mChar->WorldNumber(), mSock );
				}
				else
				{
					mChar->SetLocation( isPaging );
				}
				mChar->SetCallNum( static_cast<SI16>( tempPage->RequestId() ));
				retVal = true;
				break;
			}
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHelpRequest::~CHelpRequest()
//|	Date		-	10 September 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Deconstructor for CHelpRequest class - cleans up anything allocated
//o------------------------------------------------------------------------------------------------o
CHelpRequest::~CHelpRequest()
{
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHelpRequest::WhoPaging()
//|					CHelpRequest::WhoPaging()
//|	Date		-	10 September 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the serial of the player who paged
//o------------------------------------------------------------------------------------------------o
SERIAL CHelpRequest::WhoPaging( void ) const
{
	return playerPaging;
}
void CHelpRequest::WhoPaging( SERIAL pPaging )
{
	playerPaging = pPaging;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHelpRequest::WhoHandling()
//|	Date		-	10 September 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the serial of the player who is handling the request
//o------------------------------------------------------------------------------------------------o
SERIAL CHelpRequest::WhoHandling( void ) const
{
	return playerHandling;
}
void CHelpRequest::WhoHandling( SERIAL pHandling )
{
	playerHandling = pHandling;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHelpRequest::Priority()
//|	Date		-	10 September 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the priority of the request
//o------------------------------------------------------------------------------------------------o
SI08 CHelpRequest::Priority( void ) const
{
	return priority;
}
void CHelpRequest::Priority( SI08 pPriority )
{
	priority = pPriority;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHelpRequest::IsHandled()
//|	Date		-	10 September 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets true if the request is being handled
//o------------------------------------------------------------------------------------------------o
bool CHelpRequest::IsHandled( void ) const
{
	return handled;
}
void CHelpRequest::IsHandled( bool pHandled )
{
	handled = pHandled;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHelpRequest::TimeOfPage()
//|	Date		-	10 September 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the number of seconds since Jan 1, 1970 that the
//|					page was recorded
//o------------------------------------------------------------------------------------------------o
time_t CHelpRequest::TimeOfPage( void ) const
{
	return timeOfPage;
}
void CHelpRequest::TimeOfPage( time_t pTime )
{
	timeOfPage = pTime;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHelpRequest::Reason()
//|	Date		-	10 September 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the reason that the request was made
//o------------------------------------------------------------------------------------------------o
std::string CHelpRequest::Reason( void ) const
{
	return reason;
}
void CHelpRequest::Reason( const std::string &pReason )
{
	reason = pReason;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHelpRequest::RequestId()
//|	Date		-	10 September 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the ID # of the request
//o------------------------------------------------------------------------------------------------o
SERIAL CHelpRequest::RequestId( void ) const
{
	return helpReqId;
}
void CHelpRequest::RequestId( SERIAL hrid )
{
	helpReqId = hrid;
}
