#ifndef __Boats_h
#define __Boats_h

//For iSizeShipOffsets->

const UI32 PORT_PLANK	= 0;
const UI32 STARB_PLANK	= 1;
const UI32 HOLD			= 2;
const UI32 TILLER		= 3;

//For iShipItems->
const UI32 PORT_P_C	= 0;	// Port Plank Closed
const UI32 PORT_P_O	= 1;	// Port Plank Opened
const UI32 STAR_P_C	= 2;	// Starboard Plank Closed
const UI32 STAR_P_O	= 3;	// Starb Plank Open
const UI32 TILLERID	= 5;	// Tiller
const UI32 HOLDID	= 4;	// Hold

class cBoat 
{
	private:
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
		void	TurnBoat( CItem *b, bool rightTurn );
		void	ModelBoat( cSocket *s, CItem *i );
};
#endif


