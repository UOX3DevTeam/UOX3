
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
#include <algorithm>

inline bool operator==( const HelpRequest& x, const HelpRequest& y )
{
    return ( x.Priority() == y.Priority() );
}

inline bool operator<( const HelpRequest& x, const HelpRequest& y )
{
	return( x.Priority() < y.Priority() );
}

inline bool operator>( const HelpRequest& x, const HelpRequest& y )
{
	return( x.Priority() > y.Priority() );
}

bool PageVector::AtEnd( void ) const
{
	return( currentPos < 0 || static_cast<UI32>(currentPos) >= Queue.size() );
}

PageVector::PageVector() : currentPos( -1 ), maxID( 0 )
{
	title[0] = 0;
	Queue.resize( 0 );
}

PageVector::~PageVector()
{
	KillQueue();
}

SERIAL PageVector::Add( HelpRequest *toAdd )
{
	HelpRequest *adding = new HelpRequest;
#ifdef DEBUG
	if( adding == NULL )
		return INVALIDSERIAL;
#endif
	memcpy( adding, toAdd, sizeof( HelpRequest ) );
	Queue.push_back( adding );
	adding->RequestID( ++maxID );
	std::sort( Queue.begin(), Queue.end() );
	return adding->RequestID();
}

bool PageVector::Remove( void )
{
	if( AtEnd() )
		return false;
	delete Queue[currentPos];
	for( UI32 index = static_cast<UI32>(currentPos); index < Queue.size() - 1; index++ )
		Queue[index] = Queue[index+1];
	Queue.resize( Queue.size() - 1 );
	return true;
}

HelpRequest *PageVector::First( void )
{
	currentPos = 0;
	if( AtEnd() )
		return NULL;	// empty queue!
	return Queue[currentPos];
}

HelpRequest *PageVector::Next( void )
{
	currentPos++;
	if( AtEnd() )
		return NULL;	// at end, return NULL!
	return Queue[currentPos];
}

UI32 PageVector::NumEntries( void ) const
{
	return Queue.size();
}

void PageVector::KillQueue( void )
{
	for( UI32 counter = 0; counter < Queue.size(); counter++ )
	{
		delete Queue[counter];
	}
	Queue.resize( 0 );
}

void PageVector::SendAsGump( cSocket *toSendTo )
{
	GumpDisplay GQueue( toSendTo, 320, 340 );
	GQueue.SetTitle( title );

	for( UI32 counter = 0; counter < Queue.size(); counter++ )
	{
		if( !Queue[counter]->IsHandled() )
		{
			CChar *mChar = calcCharObjFromSer( Queue[counter]->WhoPaging() );
			if( mChar != NULL )
				GQueue.AddData( mChar->GetName(), Queue[counter]->TimeOfPage() );
		}
	}
	GQueue.Send( 4, false, INVALIDSERIAL );
}

void PageVector::SetTitle( const char *newTitle )
{
	strncpy( title, newTitle, 25 );
}

PageVector::PageVector( const char *newTitle ) : currentPos( -1 )
{
	SetTitle( newTitle );
	Queue.resize( 0 );
}

bool PageVector::GotoPos( SI32 pos )
{
	if( pos < 0 || static_cast<UI32>(pos) >= Queue.size() )
		return false;
	currentPos = pos;
	return true;
}

SI32 PageVector::CurrentPos( void ) const
{
	return currentPos;
}

SERIAL PageVector::GetCallNum( void ) const
{
	if( AtEnd() )
		return INVALIDSERIAL;
	return Queue[currentPos]->RequestID();
}

SI32 PageVector::FindCallNum( SERIAL callNum )
// returns position of page with call number callNum
{ 
	for( UI32 counter = 0; counter < Queue.size(); counter++ )
	{
		if( Queue[counter]->RequestID() == callNum )
			return counter;
	}
	return -1;
}

HelpRequest *PageVector::Current( void )
{
	if( !AtEnd() )
		return Queue[currentPos];
	else
		return NULL;
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
//|	Class		:	const char *Reason( void )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Returns the reason that the request was made
//o---------------------------------------------------------------------------o
const char * HelpRequest::Reason( void ) const
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
//|	Class		:	Reason( const char *pReason )
//|	Date		:	10 September 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Sets the reason for the page
//o---------------------------------------------------------------------------o
void HelpRequest::Reason( const char *pReason )
{
	strcpy( reason, pReason );
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
