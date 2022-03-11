
//o-----------------------------------------------------------------------------------------------o
//| File		-	PageVector.h
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	HelpRequest and PageVector classes. For use in the GM and Couselor queues
//o-----------------------------------------------------------------------------------------------o

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
	std::string		reason;				// reason for the page

public:
	HelpRequest() : helpReqID( INVALIDSERIAL ), playerPaging( INVALIDSERIAL ), playerHandling( INVALIDSERIAL ), priority( 0 ), handled( false ), timeOfPage( 0 )
	{
		reason= "";
	}
	~HelpRequest();

	SERIAL			WhoPaging( void ) const;
	SERIAL			WhoHandling( void ) const;
	SI08			Priority( void ) const;
	bool			IsHandled( void ) const;
	time_t			TimeOfPage( void ) const;
	std::string		Reason( void ) const;
	SERIAL			RequestID( void ) const;

	void			WhoPaging( SERIAL pPaging );
	void			WhoHandling( SERIAL pHandling );
	void			Priority( SI08 pPriority );
	void			IsHandled( bool pHandled );
	void			TimeOfPage( time_t pTime );
	void			Reason( const std::string &pReason );
	void			RequestID( SERIAL hrid );
};

class PageVector
{
private:
	std::vector< HelpRequest * >			Queue;
	std::vector< HelpRequest * >::iterator	currentPos;
	std::string								title;	// GM/Counselor Queue

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
	size_t			NumEntries( void ) const;
	PageVector();
	PageVector( const std::string &newTitle );
	~PageVector();
	void			SendAsGump( CSocket *toSendTo );
	void			SetTitle( const std::string &newTitle );
	bool			GotoPos( SI32 pos );
	SI32			CurrentPos( void ) const;
	SERIAL			GetCallNum( void ) const;
	SI32			FindCallNum( SERIAL callNum );
	bool			AnswerNextCall( CSocket *mSock, CChar *mChar );
};

extern PageVector	*GMQueue;
extern PageVector	*CounselorQueue;

#endif

