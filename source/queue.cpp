//o---------------------------------------------------------------------------o
//| Queue.cpp
//o---------------------------------------------------------------------------o
//| Implementation (vector based) of the GM and counselor queue class
//o---------------------------------------------------------------------------o
//| Date:	March 15th, 2000
//| Coder:	Abaddon
//o---------------------------------------------------------------------------o

#include "uox3.h"
#include "PageVector.h"
#include "gump.h"

namespace UOX
{

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

bool PageVector::AtEnd( void ) const
{
	return ( currentPos == Queue.end() );
}

PageVector::PageVector() : maxID( 0 )
{
	title = "";
	Queue.resize( 0 );
	currentPos = Queue.end();
}

PageVector::~PageVector()
{
	KillQueue();
}

SERIAL PageVector::Add( HelpRequest *toAdd )
{
	HelpRequest *adding = new HelpRequest;
#if defined( UOX_DEBUG_MODE )
	if( adding == NULL )
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

bool PageVector::Remove( void )
{
	if( AtEnd() )
		return false;
	delete (*currentPos);
	Queue.erase( currentPos );
	return true;
}

HelpRequest *PageVector::First( void )
{
	currentPos = Queue.begin();
	if( AtEnd() )
		return NULL;	// empty queue!
	return (*currentPos);
}

HelpRequest *PageVector::Next( void )
{
	++currentPos;
	if( AtEnd() )
		return NULL;	// at end, return NULL!
	return (*currentPos);
}

size_t PageVector::NumEntries( void ) const
{
	return Queue.size();
}

void PageVector::KillQueue( void )
{
	for( size_t counter = 0; counter < Queue.size(); ++counter )
	{
		delete Queue[counter];
	}
	Queue.clear();
}

void PageVector::SendAsGump( CSocket *toSendTo )
{
	GumpDisplay GQueue( toSendTo, 320, 340 );
	GQueue.SetTitle( title );

	std::vector< HelpRequest * >::iterator qIter;
	for( qIter = Queue.begin(); qIter != Queue.end(); ++qIter )
	{
		if( (*qIter) == NULL )
			continue;
		if( !(*qIter)->IsHandled() )
		{
			CChar *mChar = calcCharObjFromSer( (*qIter)->WhoPaging() );
			if( ValidateObject( mChar ) )
				GQueue.AddData( mChar->GetName(), (*qIter)->TimeOfPage() );
		}
	}
	GQueue.Send( 4, false, INVALIDSERIAL );
}

void PageVector::SetTitle( const std::string &newTitle )
{
	title = newTitle;
}

PageVector::PageVector( const std::string &newTitle )
{
	SetTitle( newTitle );
	Queue.resize( 0 );
	currentPos = Queue.end();
}

bool PageVector::GotoPos( SI32 pos )
{
	if( pos < 0 || static_cast<UI32>(pos) >= Queue.size() )
		return false;
	currentPos = (Queue.begin() + pos);
	return true;
}

SI32 PageVector::CurrentPos( void ) const
{
	return (currentPos - Queue.begin());
}

SERIAL PageVector::GetCallNum( void ) const
{
	if( AtEnd() )
		return INVALIDSERIAL;
	return (*currentPos)->RequestID();
}

SI32 PageVector::FindCallNum( SERIAL callNum )
// returns position of page with call number callNum
{ 
	for( size_t counter = 0; counter < Queue.size(); ++counter )
	{
		if( Queue[counter]->RequestID() == callNum )
			return counter;
	}
	return -1;
}

HelpRequest *PageVector::Current( void )
{
	if( !AtEnd() )
		return (*currentPos);
	else
		return NULL;
}

bool PageVector::AnswerNextCall( CSocket *mSock, CChar *mChar )
{
	bool retVal		= false;
	CChar *isPaging = NULL;
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
				QNext.AddData( "Paged at: ", tempPage->TimeOfPage() );
				QNext.Send( 4, false, INVALIDSERIAL );
				tempPage->IsHandled( true );
				mChar->SetLocation( isPaging );
				mChar->SetCallNum( static_cast<SI16>(tempPage->RequestID() ));
				retVal = true;
				break;
			}
		}
	}
	return retVal;
}



//o---------------------------------------------------------------------------o
//|	Class		:	~HelpRequest()
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Cleans up anything allocated
//o---------------------------------------------------------------------------o
HelpRequest::~HelpRequest()
{
}
	
//o---------------------------------------------------------------------------o
//|	Class		:	SERIAL WhoPaging( void )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Returns the serial of the player who paged
//o---------------------------------------------------------------------------o
SERIAL HelpRequest::WhoPaging( void ) const
{
	return playerPaging;
}

//o---------------------------------------------------------------------------o
//|	Class		:	SERIAL WhoHandling( void )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Returns the serial of the player who is handling the request
//o---------------------------------------------------------------------------o
SERIAL HelpRequest::WhoHandling( void ) const
{
	return playerHandling;
}

//o---------------------------------------------------------------------------o
//|	Class		:	SI08 Priority( void )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Returns the priority of the request
//o---------------------------------------------------------------------------o
SI08 HelpRequest::Priority( void ) const
{
	return priority;
}

//o---------------------------------------------------------------------------o
//|	Class		:	bool IsHandled( void )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Returns true if the request is being handled
//o---------------------------------------------------------------------------o
bool HelpRequest::IsHandled( void ) const
{
	return handled;
}

//o---------------------------------------------------------------------------o
//|	Class		:	time_t TimeOfPage( void )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Returns the number of seconds since Jan 1, 1970 that the
//|					page was recorded
//o---------------------------------------------------------------------------o
time_t HelpRequest::TimeOfPage( void ) const
{
	return timeOfPage;
}

//o---------------------------------------------------------------------------o
//|	Class		:	std::string Reason( void )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Returns the reason that the request was made
//o---------------------------------------------------------------------------o
std::string HelpRequest::Reason( void ) const
{
	return reason;
}

//o---------------------------------------------------------------------------o
//|	Class		:	SERIAL RequestID( void )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Returns the ID # of the request
//o---------------------------------------------------------------------------o
SERIAL HelpRequest::RequestID( void ) const
{
	return helpReqID;
}

//o---------------------------------------------------------------------------o
//|	Class		:	WhoPaging( SERIAL pPaging )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Sets the serial of the player paging
//o---------------------------------------------------------------------------o
void HelpRequest::WhoPaging( SERIAL pPaging )
{
	playerPaging = pPaging;
}

//o---------------------------------------------------------------------------o
//|	Class		:	WhoHandling( SERIAL pHandling )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Sets the serial of the player handling the page
//o---------------------------------------------------------------------------o
void HelpRequest::WhoHandling( SERIAL pHandling )
{
	playerHandling = pHandling;
}

//o---------------------------------------------------------------------------o
//|	Class		:	Priority( SI08 pPriority )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Sets the priority of the page
//o---------------------------------------------------------------------------o
void HelpRequest::Priority( SI08 pPriority )
{
	priority = pPriority;
}

//o---------------------------------------------------------------------------o
//|	Class		:	IsHandled( bool pHandled )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Sets whether the page has been handled
//o---------------------------------------------------------------------------o
void HelpRequest::IsHandled( bool pHandled )
{
	handled = pHandled;
}

//o---------------------------------------------------------------------------o
//|	Class		:	TimeOfPage( time_t pTime )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Sets the time the page was recorded
//o---------------------------------------------------------------------------o
void HelpRequest::TimeOfPage( time_t pTime )
{
	timeOfPage = pTime;
}

//o---------------------------------------------------------------------------o
//|	Class		:	Reason( std::string pReason )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Sets the reason for the page
//o---------------------------------------------------------------------------o
void HelpRequest::Reason( const std::string &pReason )
{
	reason = pReason;
}

//o---------------------------------------------------------------------------o
//|	Class		:	RequestID( SERIAL hrid )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Sets the unique ID of the page
//o---------------------------------------------------------------------------o
void HelpRequest::RequestID( SERIAL hrid )
{
	helpReqID = hrid;
}

}
