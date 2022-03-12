//o-----------------------------------------------------------------------------------------------o
//| File		-	Queue.cpp
//| Date		-	March 15th, 2000
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Implementation (vector based) of the GM and counselor queue class
//o-----------------------------------------------------------------------------------------------o

#include "uox3.h"
#include "PageVector.h"
#include "gump.h"

PageVector *GMQueue;
PageVector *CounselorQueue;

inline bool operator==( const HelpRequest& x, const HelpRequest& y )
{
	return ( x.Priority() == y.Priority() );
}

inline bool operator<( const HelpRequest& x, const HelpRequest& y )
{
	return ( x.Priority() < y.Priority() );
}

inline bool operator>( const HelpRequest& x, const HelpRequest& y )
{
	return ( x.Priority() > y.Priority() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AtEnd( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether position has reached end of queue
//o-----------------------------------------------------------------------------------------------o
bool PageVector::AtEnd( void ) const
{
	return ( currentPos == Queue.end() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	PageVector()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for PageVector class - initializes an empty queue
//o-----------------------------------------------------------------------------------------------o
PageVector::PageVector() : maxID( 0 )
{
	title = "";
	Queue.resize( 0 );
	currentPos = Queue.end();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	~PageVector()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Deconstructor for PageVector class - kills entire queue
//o-----------------------------------------------------------------------------------------------o
PageVector::~PageVector()
{
	KillQueue();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL Add( HelpRequest *toAdd )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds help request from player to page queue
//o-----------------------------------------------------------------------------------------------o
SERIAL PageVector::Add( HelpRequest *toAdd )
{
	HelpRequest *adding = new HelpRequest;
#if defined( UOX_DEBUG_MODE )
	if( adding == nullptr )
		return INVALIDSERIAL;
#endif
#pragma note( "Bad idea to use memcpy to copy one class object to another (especially if the class contains an std::string)?" )
	memcpy( adding, toAdd, sizeof( HelpRequest ) );
	//*adding = *toAdd; // Maybe we should do this instead?
	Queue.push_back( adding );
	adding->RequestID( ++maxID );
	std::sort( Queue.begin(), Queue.end() );
	return adding->RequestID();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Remove( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes help request from player from queue
//o-----------------------------------------------------------------------------------------------o
bool PageVector::Remove( void )
{
	if( AtEnd() )
		return false;
	delete (*currentPos);
	Queue.erase( currentPos );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	HelpRequest *First( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets first help request in queue, if any exists
//o-----------------------------------------------------------------------------------------------o
HelpRequest *PageVector::First( void )
{
	currentPos = Queue.begin();
	if( AtEnd() )
		return nullptr;	// empty queue!
	return (*currentPos);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	HelpRequest *Next( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets next help request in queue, if any exists
//o-----------------------------------------------------------------------------------------------o
HelpRequest *PageVector::Next( void )
{
	++currentPos;
	if( AtEnd() )
		return nullptr;	// at end, return nullptr!
	return (*currentPos);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t NumEntries( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of help requests in queue
//o-----------------------------------------------------------------------------------------------o
size_t PageVector::NumEntries( void ) const
{
	return Queue.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void KillQueue( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears all help requests from queue
//o-----------------------------------------------------------------------------------------------o
void PageVector::KillQueue( void )
{
	for( size_t counter = 0; counter < Queue.size(); ++counter )
	{
		delete Queue[counter];
	}
	Queue.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendAsGump( CSocket *toSendTo )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a list of help requests in queue to client and displays it in a Gump
//o-----------------------------------------------------------------------------------------------o
void PageVector::SendAsGump( CSocket *toSendTo )
{
	GumpDisplay GQueue( toSendTo, 320, 340 );
	GQueue.SetTitle( title );

	std::vector< HelpRequest * >::iterator qIter;
	for( qIter = Queue.begin(); qIter != Queue.end(); ++qIter )
	{
		if( (*qIter) == nullptr )
			continue;
		if( !(*qIter)->IsHandled() )
		{
			CChar *mChar = calcCharObjFromSer( (*qIter)->WhoPaging() );
			if( ValidateObject( mChar ) )
				GQueue.AddData( mChar->GetName(), static_cast<UI32>((*qIter)->TimeOfPage()) );
		}
	}
	GQueue.Send( 4, false, INVALIDSERIAL );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetTitle( const std::string &newTitle )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets title displayed for help request queue in client
//o-----------------------------------------------------------------------------------------------o
void PageVector::SetTitle( const std::string &newTitle )
{
	title = newTitle;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	PageVector( const std::string &newTitle )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializer for PageVector class
//o-----------------------------------------------------------------------------------------------o
PageVector::PageVector( const std::string &newTitle )
{
	SetTitle( newTitle );
	Queue.resize( 0 );
	currentPos = Queue.end();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GotoPos( SI32 pos )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets queue position to specified value
//o-----------------------------------------------------------------------------------------------o
bool PageVector::GotoPos( SI32 pos )
{
	if( pos < 0 || static_cast<UI32>(pos) >= Queue.size() )
		return false;
	currentPos = (Queue.begin() + pos);
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 CurrentPos( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets current queue position
//o-----------------------------------------------------------------------------------------------o
SI32 PageVector::CurrentPos( void ) const
{
	return static_cast<SI32>((currentPos - Queue.begin()));
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetCallNum( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets call number/help request ID for current position in queue
//o-----------------------------------------------------------------------------------------------o
SERIAL PageVector::GetCallNum( void ) const
{
	if( AtEnd() )
		return INVALIDSERIAL;
	return (*currentPos)->RequestID();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 FindCallNum( SERIAL callNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets position of page with specified call number/help request ID
//o-----------------------------------------------------------------------------------------------o
SI32 PageVector::FindCallNum( SERIAL callNum )
{
	for( size_t counter = 0; counter < Queue.size(); ++counter )
	{
		if( Queue[counter]->RequestID() == callNum )
			return static_cast<SI32>(counter);
	}
	return -1;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	HelpRequest *Current( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets current position in queue
//o-----------------------------------------------------------------------------------------------o
HelpRequest *PageVector::Current( void )
{
	if( !AtEnd() )
		return (*currentPos);
	else
		return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AnswerNextCall( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Marks next help request in queue as handled and sends it to GM/Counselor
//o-----------------------------------------------------------------------------------------------o
bool PageVector::AnswerNextCall( CSocket *mSock, CChar *mChar )
{
	bool retVal		= false;
	CChar *isPaging = nullptr;
	for( HelpRequest *tempPage = First(); !AtEnd(); tempPage = Next() )
	{
		if( !tempPage->IsHandled() )
		{
			isPaging = calcCharObjFromSer( tempPage->WhoPaging() );
			if( ValidateObject( isPaging ) )
			{
				GumpDisplay QNext( mSock, 300, 200 );
				QNext.AddData( "Pager: ", isPaging->GetName() );
				QNext.AddData( "Problem: ", tempPage->Reason() );
				QNext.AddData( "Serial number ", tempPage->WhoPaging(), 3 );
				QNext.AddData( "Paged at: ", static_cast<UI32>(tempPage->TimeOfPage()) );
				QNext.Send( 4, false, INVALIDSERIAL );
				tempPage->IsHandled( true );
				if( mChar->WorldNumber() != isPaging->WorldNumber() )
				{
					mChar->SetLocation( isPaging );
					SendMapChange( mChar->WorldNumber(), mSock );
				}
				else
					mChar->SetLocation( isPaging );
				mChar->SetCallNum( static_cast<SI16>(tempPage->RequestID() ));
				retVal = true;
				break;
			}
		}
	}
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	~HelpRequest()
//|	Date		-	10 September 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Deconstructor for HelpRequest class - cleans up anything allocated
//o-----------------------------------------------------------------------------------------------o
HelpRequest::~HelpRequest()
{
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL WhoPaging( void ) const
//|					void WhoPaging( SERIAL pPaging )
//|	Date		-	10 September 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the serial of the player who paged
//o-----------------------------------------------------------------------------------------------o
SERIAL HelpRequest::WhoPaging( void ) const
{
	return playerPaging;
}
void HelpRequest::WhoPaging( SERIAL pPaging )
{
	playerPaging = pPaging;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL WhoHandling( void ) const
//|					void WhoHandling( SERIAL pHandling )
//|	Date		-	10 September 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the serial of the player who is handling the request
//o-----------------------------------------------------------------------------------------------o
SERIAL HelpRequest::WhoHandling( void ) const
{
	return playerHandling;
}
void HelpRequest::WhoHandling( SERIAL pHandling )
{
	playerHandling = pHandling;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 Priority( void ) const
//|					void Priority( SI08 pPriority )
//|	Date		-	10 September 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the priority of the request
//o-----------------------------------------------------------------------------------------------o
SI08 HelpRequest::Priority( void ) const
{
	return priority;
}
void HelpRequest::Priority( SI08 pPriority )
{
	priority = pPriority;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsHandled( void ) const
//|					void IsHandled( bool pHandled )
//|	Date		-	10 September 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets true if the request is being handled
//o-----------------------------------------------------------------------------------------------o
bool HelpRequest::IsHandled( void ) const
{
	return handled;
}
void HelpRequest::IsHandled( bool pHandled )
{
	handled = pHandled;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	time_t TimeOfPage( void ) const
//|					void TimeOfPage( time_t pTime )
//|	Date		-	10 September 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the number of seconds since Jan 1, 1970 that the
//|					page was recorded
//o-----------------------------------------------------------------------------------------------o
time_t HelpRequest::TimeOfPage( void ) const
{
	return timeOfPage;
}
void HelpRequest::TimeOfPage( time_t pTime )
{
	timeOfPage = pTime;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string Reason( void )
//|					void Reason( const std::string &pReason )
//|	Date		-	10 September 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the reason that the request was made
//o-----------------------------------------------------------------------------------------------o
std::string HelpRequest::Reason( void ) const
{
	return reason;
}
void HelpRequest::Reason( const std::string &pReason )
{
	reason = pReason;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL RequestID( void ) const
//|					void RequestID( SERIAL hrid )
//|	Date		-	10 September 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the ID # of the request
//o-----------------------------------------------------------------------------------------------o
SERIAL HelpRequest::RequestID( void ) const
{
	return helpReqID;
}
void HelpRequest::RequestID( SERIAL hrid )
{
	helpReqID = hrid;
}
