#ifndef __Boats_h
#define __Boats_h

enum ShipOffsets
{
	PORT_PLANK	= 0,
	STARB_PLANK,
	HOLD,
	TILLER
};

enum ShipItems
{
	PORT_P_C	= 0,// Port Plank Closed
	PORT_P_O,		// Port Plank Opened
	STAR_P_C,		// Starboard Plank Closed
	STAR_P_O,		// Starb Plank Open
	HOLDID,			// Hold
	TILLERID		// Tiller
};

class cBoat 
{
	private:
		void	TurnBoat( CItem *b, bool rightTurn );
		void	LeaveBoat( cSocket *s, CItem *p );
		bool	BlockBoat( CItem *b, SI16 xmove, SI16 ymove, UI08 dir );
		void	TurnStuff( CItem *b, CItem *i, bool rightTurn );
		void	TurnStuff( CItem *b, CChar *i, bool rightTurn );

	public:
				cBoat();
		virtual ~cBoat();
		CItem *	GetBoat( cSocket *s );
		void	Speech( cSocket *mSock, char *talk );
		void	OpenPlank( CItem *p );
		void	PlankStuff( cSocket *, CItem *p );
		bool	CreateBoat( cSocket *s, CItem *b, char id2, int boattype );
		void	MoveBoat( cSocket *s, UI08 dir, CItem *boat);
		void	ModelBoat( cSocket *s, CItem *i );
};
#endif


