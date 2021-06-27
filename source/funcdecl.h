//o-----------------------------------------------------------------------------------------------o
//| File		-	funcdecl.h
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	This header file contains all of our globally declared functions
//o-----------------------------------------------------------------------------------------------o
//| Notes		-	Version History
//|						2.0		10/16/2003
//|						Many functions removed, overall reorganization and updated to match
//|						the rest of UOX3.
//o-----------------------------------------------------------------------------------------------o
#ifndef __FUNCDECLS_H__
#define __FUNCDECLS_H__
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include "uoxstruct.h"
#include "StringUtility.hpp"
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <sstream>

#include "cBaseObject.h"
#include "cConsole.h"
#include "worldmain.h"
extern CConsole Console;
extern CWorldMain   *cwmWorldState;
extern std::mt19937 generator;

//o-----------------------------------------------------------------------------------------------o
// Range check functions
//o-----------------------------------------------------------------------------------------------o
bool	objInRange( CSocket *mSock, CBaseObject *obj, UI16 distance );
bool	objInRange( CBaseObject *a, CBaseObject *b, UI16 distance );
bool	objInRangeSquare( CBaseObject *a, CBaseObject *b, UI16 distance );
bool	objInOldRange( CBaseObject *a, CBaseObject *b, UI16 distance );
bool	objInOldRangeSquare( CBaseObject *a, CBaseObject *b, UI16 distance );
bool	charInRange( CChar *a, CChar *b );
UI16	getDist( CBaseObject *a, CBaseObject *b );
UI16	getDist( point3 a, point3 b );
UI16	getOldDist( CBaseObject *a, CBaseObject *b );
UI16	getDist3D( CBaseObject *a, CBaseObject *b );
UI16	getDist3D( point3 a, point3 b );
SOCKLIST	FindPlayersInVisrange( CBaseObject *myObj );
SOCKLIST	FindPlayersInOldVisrange( CBaseObject *myObj );
SOCKLIST	FindNearbyPlayers( CBaseObject *myObj, UI16 distance );
SOCKLIST	FindNearbyPlayers( CBaseObject *myObj );
SOCKLIST	FindNearbyPlayers( CChar *mChar );
//o-----------------------------------------------------------------------------------------------o
// Multi functions
//o-----------------------------------------------------------------------------------------------o
CMultiObj *	findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID );
CMultiObj *	findMulti( CBaseObject *i );

//o-----------------------------------------------------------------------------------------------o
// Item functions
//o-----------------------------------------------------------------------------------------------o
CItem *GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID );
CItem *FindItemNearXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 id, UI16 instanceID );

//o-----------------------------------------------------------------------------------------------o
// Calculation functions (socket, char, item and so forth)
//o-----------------------------------------------------------------------------------------------o
CItem *		calcItemObjFromSer( SERIAL targSerial );
CChar *		calcCharObjFromSer( SERIAL targSerial );
CMultiObj *	calcMultiFromSer( SERIAL targSerial );
inline UI32 calcserial( UI08 a1, UI08 a2, UI08 a3, UI08 a4 )
{
	return ( (a1<<24) + (a2<<16) + (a3<<8) + a4 );
}

//o-----------------------------------------------------------------------------------------------o
// Socket stuff
//o-----------------------------------------------------------------------------------------------o
void	SendVecsAsGump( CSocket *sock, STRINGLIST& one, STRINGLIST& two, UI32 type, SERIAL serial );
void	SendMapChange( UI08 worldNumber, CSocket *sock, bool initialLogin = false );
bool	isOnline( CChar& mChar );

//o-----------------------------------------------------------------------------------------------o
// Light related functions
//o-----------------------------------------------------------------------------------------------o
void	doLight( CSocket *s, UI08 level );
void	doLight( CChar *mChar, UI08 level );

//o-----------------------------------------------------------------------------------------------o
// Amount related
//o-----------------------------------------------------------------------------------------------o
UI32	GetItemAmount( CChar *s, UI16 realID, UI16 realColour = 0x0000 );
UI32	GetTotalItemCount( CItem *objCont );
UI32	DeleteItemAmount( CChar *s, UI32 amount, UI16 realID, UI16 realColour = 0x0000 );
UI32	DeleteSubItemAmount( CItem *p, UI32 amount, UI16 realID, UI16 realColour = 0x0000 );
UI32	GetBankCount( CChar *p, UI16 itemID, UI16 realColour = 0x0000 );
UI32	DeleteBankItem( CChar *p, UI32 amt, UI16 itemID, UI16 realColour = 0x0000 );

//o-----------------------------------------------------------------------------------------------o
// Region related
//o-----------------------------------------------------------------------------------------------o
CTownRegion *calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID );
void CheckCharInsideBuilding( CChar *c, CSocket *mSock, bool doWeatherStuff );

//o-----------------------------------------------------------------------------------------------o
// Find functions
//o-----------------------------------------------------------------------------------------------o
CChar *			FindItemOwner( CItem *p );
CBaseObject *	FindItemOwner( CItem *i, ObjectType &objType );
CItem *			FindRootContainer( CItem *i );
CItem *			FindItemOfType( CChar *toFind, ItemTypes type );
CItem *			FindItem( CChar *toFind, UI16 itemID );

//o-----------------------------------------------------------------------------------------------o
// Reputation Stuff
//o-----------------------------------------------------------------------------------------------o
void	Karma( CChar *nCharID, CChar *nKilledID, const SI16 nKarma );
void	Fame( CChar *nCharID, const SI16 nFame );
void	UpdateFlag( CChar *mChar );

//o-----------------------------------------------------------------------------------------------o
// Combat Stuff
//o-----------------------------------------------------------------------------------------------o
void	criminal( CChar *c );
bool	WillResultInCriminal( CChar *mChar, CChar *targ );
void	callGuards( CChar *mChar, CChar *targChar );
void	callGuards( CChar *mChar );

//o-----------------------------------------------------------------------------------------------o
// Time Functions
//o-----------------------------------------------------------------------------------------------o
inline TIMERVAL BuildTimeValue( R32 timeFromNow )
{
	return static_cast<TIMERVAL>( cwmWorldState->GetUICurrentTime() + ( static_cast<R32>(1000) * timeFromNow ) );
}

UI32	getclock( void );
inline char *	RealTime( char *time_str )
{
	auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto curtime = std::localtime(&timet);
	strftime( time_str, 256, "%B %d %I:%M:%S %p", curtime );
	return time_str;
}
inline char *	RealTime24( char *time_str )
{
	auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto curtime = std::localtime(&timet);
	strftime( time_str, 256, "%B %d %H:%M:%S", curtime );
	return time_str;
}
inline char *	RealTimeDate( char *time_str )
{
	auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto curtime = std::localtime(&timet);
	strftime( time_str, 256, "%B %d", curtime );
	return time_str;
}

#if P_TIMESTAMP
inline std::string TimeStamp( void )
{
	auto stamp = []() {  
		auto milli = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		auto hours = ( ( ( milli / 1000 ) / 60 ) / 60 );
		milli = milli - ( hours * 60 * 60 * 1000 );
		auto minutes = ( ( milli / 1000 ) / 60 );
		milli = milli - ( minutes * 1000 * 60 );
		auto seconds = milli / 1000;
		milli = milli - ( seconds * 1000 );
		std::stringstream value;
		char timeStamp[512];
		RealTime( timeStamp );
		value << " [" << timeStamp << "][" << milli << "]";
		return value.str();
	};

	return stamp();
}
#else
inline std::string TimeStamp( void )
{
	return "";
}
#endif
inline void StartMilliTimer( std::uint32_t &Seconds, std::uint32_t &Milliseconds )
{
	auto timenow = std::chrono::system_clock::now().time_since_epoch() ;
	auto sec = std::chrono::duration_cast<std::chrono::seconds>(timenow).count() ;
	auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(timenow).count() - (1000*sec);
	Seconds            =  static_cast<std::uint32_t>(sec) ;
	Milliseconds    =  static_cast<std::uint32_t>(milli);
}
inline std::uint32_t CheckMilliTimer( std::uint32_t &Seconds, std::uint32_t &Milliseconds )
{
	auto timenow = std::chrono::system_clock::now().time_since_epoch() ;
	auto sec = std::chrono::duration_cast<std::chrono::seconds>(timenow).count() ;
	auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(timenow).count() - (1000*sec);
	return static_cast<std::uint32_t>((1000 *(sec - Seconds)) + (milli - Milliseconds));
}

inline std::uint32_t GetMinutesSinceEpoch()
{
	return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::minutes>(std::chrono::system_clock::now().time_since_epoch()).count());
}

//o-----------------------------------------------------------------------------------------------o
// Misc Functions
//o-----------------------------------------------------------------------------------------------o
R32		roundNumber( R32 toRound);
bool	FileExists( const std::string& filepath );
void	DismountCreature( CChar *s );
size_t	getTileName( CItem& mItem, std::string& itemname );
bool	LineOfSight( CSocket *s, CChar *mChar, SI16 x2, SI16 y2, SI08 z2, UI08 checkfor, bool useSurfaceZ );
bool	checkItemLineOfSight( CChar *mChar, CItem *i );
void	Shutdown( SI32 retCode );
void	HandleDeath( CChar *mChar, CChar *attacker );
void	NpcResurrectTarget( CChar *s );

inline bool ValidateObject( const CBaseObject *toValidate )
{
	bool rvalue = true;
	try
	{
		if( toValidate == nullptr )
			rvalue = false;
		else if( toValidate->isDeleted() )
			rvalue = false;
	}
	catch( ... )
	{
		rvalue = false;
		Console.error( strutil::format("Invalid Object found: 0x%X", (UI64)toValidate) );
	}
	return rvalue;
}

template< class T >
inline T RandomNum( T nLowNum, T nHighNum )
{
	auto high = std::max<T>(nLowNum,nHighNum);
	auto low = std::min<T>(nLowNum,nHighNum);
	if (std::is_floating_point<T>::value){
		auto distribution = std::uniform_real_distribution<double>(low, high);
		return distribution(generator);

	}
	auto distribution = std::uniform_int_distribution<T>(low, high);
	return distribution(generator);
}


template< class T >
inline T HalfRandomNum( T HighRange )
{
	T halfSize = static_cast< T >(HighRange / 2);
	return RandomNum( halfSize, HighRange );
}

#endif

