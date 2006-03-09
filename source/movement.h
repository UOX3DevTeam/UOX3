#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

namespace UOX
{

// Maximum Search Depth: Iterations to calculate
#define P_PF_MSD		5
// Maximum Return Value: Number of steps to return (Replaces PATHNUM)
// NOTE: P_PF_MRV CANNOT EXCEED THE VALUE OF PATHNUM FOR THE TIME BEING
#define P_PF_MRV		2
// Maximum Influence Range: Tiles to left/right of path to account for
#define P_PF_MIR		5
// Maximum Blocked Range: MIR to use if character is stuck
#define P_PF_MBR		10
// Minimum Flee Distance: MFD
#define P_PF_MFD		15

struct pfNode
{
	UI16 hCost;
	UI08 gCost;
	size_t parent;
	pfNode() : hCost( 0 ), gCost( 0 ), parent( 0 )
	{
	}
	pfNode( UI16 nHC, UI08 nGC, UI32 nPS ) : hCost( nHC ), gCost( nGC ), parent( nPS )
	{
	}
};

struct nodeFCost
{
	UI32 xySer;
	UI16 fCost;
	nodeFCost() : xySer( 0 ), fCost( 0 )
	{
	}
	nodeFCost( UI16 nFC, UI32 nS ) : fCost( nFC ), xySer( nS )
	{
	}
};

class cMovement
{
	// Function declarations
public:
	void	AdvancedPathfinding( CChar *mChar, UI16 targX, UI16 targY, bool willRun = false );
	void	Walking( CSocket *mSock, CChar *s, UI08 dir, SI16 sequence );
	void	CombatWalk( CChar *i );
	bool	validNPCMove( SI16 x, SI16 y, SI08 z, CChar *s );
	void	NpcMovement( CChar& mChar );
	void	PathFind( CChar *c, SI16 gx, SI16 gy, bool willRun = false, UI08 pathLen = P_PF_MRV );
	UI08	Direction( CChar *c, SI16 x, SI16 y );
private:
	bool	PFGrabNodes( CChar *mChar, UI16 targX, UI16 targY, UI16 &curX, UI16 &curY, UI32 parentSer, std::map< UI32, pfNode >& openList, std::map< UI32, UI32 >& closedList, std::deque< nodeFCost >& fCostList, std::map< UI32, bool >& blockList );
	SI08	calc_walk( CChar *c, SI16 x, SI16 y, SI16 oldx, SI16 oldy, bool justask );
	SI08	calc_WaterWalk( CChar *c, SI16 x, SI16 y, SI16 oldx, SI16 oldy, bool justask );
	bool	calc_move( CChar *c, SI16 x, SI16 y, SI08 &z, UI08 dir );

	bool	isValidDirection( UI08 dir );
	bool	isFrozen( CChar *c, CSocket *mSock, SI16 sequence );
	bool	isOverloaded( CChar *c, CSocket *mSock, SI16 sequence );

/*
//	Currently all these functions are unreferenced (used only of themselves, never called by anything)
//	Thus I am commenting them out until we decide if they do hold some use for the future - giwo
	bool	MoveHeightAdjustment( int MoveType, CTileUni *thisblock, int &ontype, SI32 &nItemTop, SI32 &nNewZ );
	SI16	CheckMovementType( CChar *c );
	bool	CanGMWalk( CTileUni &xyb );
	bool	CanPlayerWalk( CTileUni &xyb );
	bool	CanNPCWalk( CTileUni &xyb );
	bool	CanFishWalk( CTileUni &xyb );
	bool	CanBirdWalk( CTileUni &xyb );
	*/

	void	GetBlockingMap( SI16 x, SI16 y, CTileUni *xyblock, UI16 &xycount, SI16 oldx, SI16 oldy, UI08 worldNumber );
	void	GetBlockingStatics( SI16 x, SI16 y, CTileUni *xyblock, UI16 &xycount, UI08 worldNumber );
	void	GetBlockingDynamics( SI16 x, SI16 y, CTileUni *xyblock, UI16 &xycount, UI08 worldNumber );

	SI16	Distance( SI16 sx, SI16 sy, SI16 dx, SI16 dy );
	UI08	Direction( SI16 sx, SI16 sy, SI16 dx, SI16 dy );

	bool	CheckForCharacterAtXYZ( CChar *c, SI16 cx, SI16 cy, SI08 cz );
	void	NpcWalk( CChar *i, UI08 j, SI08 getWander );
	SI16	GetXfromDir( UI08 dir, SI16 x );
	SI16	GetYfromDir( UI08 dir, SI16 y );

	bool	VerifySequence( CChar *c, CSocket *mSock, SI16 sequence);
	bool	CheckForRunning( CChar *c, UI08 dir );
	bool	CheckForStealth( CChar *c );
	bool	CheckForHouseBan( CChar *c, CSocket *mSock );
	void	MoveCharForDirection( CChar *c, SI08 myz, UI08 dir );
	void	HandleRegionStuffAfterMove( CChar *c, SI16 oldx, SI16 oldy );
	void	SendWalkToPlayer( CChar *c, CSocket *mSock, SI16 sequence );
	void	SendWalkToOtherPlayers( CChar *c, UI08 dir, SI16 oldx, SI16 oldy );
	void	OutputShoveMessage( CChar *c, CSocket *mSock );
	void	HandleItemCollision( CChar *c, CSocket *mSock, SI16 oldx, SI16 oldy );
	bool	IsGMBody( CChar *c );

	void	deny( CSocket *mSock, CChar *s, SI16 sequence );
};

extern cMovement *Movement;

}

#endif

