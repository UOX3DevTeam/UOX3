#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include "mapclasses.h"

// Maximum Return Value: Number of steps to return (Replaces PATHNUM)
// NOTE: P_PF_MRV CANNOT EXCEED THE VALUE OF PATHNUM FOR THE TIME BEING
constexpr auto P_PF_MRV = 22;
// Minimum Flee Distance: MFD
constexpr auto P_PF_MFD	= 15;

struct PfNode_st
{
	UI16	hCost;
	UI08	gCost;
	size_t	parent;
	SI08	z;
	PfNode_st() : hCost( 0 ), gCost( 0 ), parent( 0 ), z( 0 )
	{
	}
	PfNode_st( UI16 nHC, UI08 nGC, UI32 nPS, SI08 nZ ) : hCost( nHC ), gCost( nGC ), parent( nPS ), z( nZ )
	{
	}
};

struct NodeFCost_st
{
	UI32 xySer;
	UI16 fCost;
	NodeFCost_st() : xySer( 0 ), fCost( 0 )
	{
	}
	NodeFCost_st( UI16 nFC, UI32 nS ) : xySer( nS ), fCost( nFC )
	{
	}
};

class CMovement
{
	// Function declarations
public:
	bool	AdvancedPathfinding( CChar *mChar, UI16 targX, UI16 targY, bool willRun = false, UI16 maxSteps = 0 );
	void	Walking( CSocket *mSock, CChar *s, UI08 dir, SI16 sequence );
	void	CombatWalk( CChar *i );
	void	NpcMovement( CChar& mChar );
	void	PathFind( CChar *c, SI16 gx, SI16 gy, bool willRun = false, UI08 pathLen = P_PF_MRV );
	UI08	Direction( CChar *c, SI16 x, SI16 y );
	bool	CheckForCharacterAtXYZ( CChar *c, SI16 cx, SI16 cy, SI08 cz );
	SI08	CalcWalk( CChar *c, SI16 x, SI16 y, SI16 oldx, SI16 oldy, SI08 oldz, bool justask, bool waterWalk = false );
private:
	bool	PFGrabNodes( CChar *mChar, UI16 targX, UI16 targY, UI16 curX, UI16 curY, SI08 curZ, UI32 parentSer, std::map<UI32, PfNode_st>& openList, std::map<UI32, UI32>& closedList, std::deque<NodeFCost_st>& fCostList );
	bool	CalcMove( CChar *c, SI16 x, SI16 y, SI08 &z, UI08 dir );

	bool	HandleNPCWander( CChar& mChar );
	bool	IsValidDirection( UI08 dir );
	bool	IsFrozen( CChar *c, CSocket *mSock, SI16 sequence );
	bool	IsOverloaded( CChar *c, CSocket *mSock, SI16 sequence );

	bool	IsOk( std::vector<Tile_st> &xyblock, UI16 &xycount, UI08 world, SI08 ourZ, SI08 ourTop, SI16 x, SI16 y, UI16 instanceId, bool ignoreDoor, bool waterWalk );
	void	GetAverageZ( UI08 nm, SI16 x, SI16 y, SI08& z, SI08& avg, SI08& top );
	void	GetStartZ( UI08 world, CChar *c, SI16 x, SI16 y, SI08 z, SI08& zlow, SI08& ztop, UI16 instanceId, bool waterWalk );

	void	GetBlockingStatics( SI16 x, SI16 y, std::vector<Tile_st> &xyblock, UI16 &xycount, UI08 worldNumber );
	void	GetBlockingDynamics( SI16 x, SI16 y, std::vector<Tile_st> &xyblock, UI16 &xycount, UI08 worldNumber, UI16 instanceId );

	UI08	Direction( SI16 sx, SI16 sy, SI16 dx, SI16 dy );

	void	NpcWalk( CChar *i, UI08 j, SI08 getWander );
	SI16	GetXfromDir( UI08 dir, SI16 x );
	SI16	GetYfromDir( UI08 dir, SI16 y );

	void	BoundingBoxTeleport( CChar *c, UI16 fx2Actual, UI16 fy2Actual, SI16 newz, SI16 newy );

	bool	VerifySequence( CChar *c, CSocket *mSock, SI16 sequence);
	bool	CheckForRunning( CChar *c, UI08 dir );
	bool	CheckForStealth( CChar *c );
	bool	CheckForHouseBan( CChar *c, CSocket *mSock );
	void	MoveCharForDirection( CChar *c, SI16 newX, SI16 newY, SI08 newZ );
	void	SendWalkToPlayer( CChar *c, CSocket *mSock, SI16 sequence );
	void	SendWalkToOtherPlayers( CChar *c, UI08 dir, SI16 oldx, SI16 oldy );
	void	OutputShoveMessage( CChar *c, CSocket *mSock );
	void	HandleItemCollision( CChar *c, CSocket *mSock, SI16 oldx, SI16 oldy );
	bool	IsGMBody( CChar *c );

	void	DenyMovement( CSocket *mSock, CChar *s, SI16 sequence );
};

extern CMovement *Movement;

#endif

