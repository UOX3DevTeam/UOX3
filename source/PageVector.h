
//o---------------------------------------------------------------------------o
//| PageVector.h
//o---------------------------------------------------------------------------o
//| HelpRequest and PageVector classes. For use in the GM and Couselor queues
//o---------------------------------------------------------------------------o

#ifndef __PAGEVECTOR_H__
#define __PAGEVECTOR_H__

class HelpRequest
{
private:
	SERIAL			helpReqID;
	SERIAL			playerPaging;		// player who is paging
	SERIAL			playerHandling;		// player who is handling it
	SI08			priority;			// priority of the page
	bool			handled;			// has it been handled?
	time_t			timeOfPage;			// when was the page reported?
	char			reason[80];			// reason for the page

public:
					HelpRequest() : helpReqID( INVALIDSERIAL ), playerPaging( INVALIDSERIAL ), playerHandling( INVALIDSERIAL ), priority( 0 ), handled( false ), timeOfPage( 0 ) { reason[0] = 0; }
	virtual			~HelpRequest();
	
	SERIAL			WhoPaging( void ) const;
	SERIAL			WhoHandling( void ) const;
	SI08			Priority( void ) const;
	bool			IsHandled( void ) const;
	time_t			TimeOfPage( void ) const;
	const char *	Reason( void ) const;
	SERIAL			RequestID( void ) const;

	void			WhoPaging( SERIAL pPaging );
	void			WhoHandling( SERIAL pHandling );
	void			Priority( SI08 pPriority );
	void			IsHandled( bool pHandled );
	void			TimeOfPage( time_t pTime );
	void			Reason( const char *pReason );
	void			RequestID( SERIAL hrid );
};

class PageVector
{
private:
	std::vector< HelpRequest * >	Queue;
	SI32					currentPos; 
	char					title[25];	// GM/Counselor Queue

	R32						avgEntryTime, maxEntryTime, minEntryTime;
	SERIAL					maxID;

	void					KillQueue( void );
public:
	SERIAL			Add( HelpRequest *toAdd );
	bool			Remove( void );
	HelpRequest *	First( void );
	HelpRequest *	Next( void );
	HelpRequest *	Current( void );
	bool			AtEnd( void ) const;
	UI32			NumEntries( void ) const;
					PageVector();
					PageVector( const char *newTitle );
	virtual			~PageVector();
	void			SendAsGump( cSocket *toSendTo );
	void			SetTitle( const char *newTitle );
	bool			GotoPos( SI32 pos );
	SI32			CurrentPos( void ) const;
	SERIAL			GetCallNum( void ) const;
	SI32			FindCallNum( SERIAL callNum );
};


#endif

