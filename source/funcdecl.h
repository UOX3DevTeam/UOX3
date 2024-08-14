//o------------------------------------------------------------------------------------------------o
//| File		-	funcdecl.h
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	This header file contains all of our globally declared functions
//o------------------------------------------------------------------------------------------------o
//| Notes		-	Version History
//|						2.0		10/16/2003
//|						Many functions removed, overall reorganization and updated to match
//|						the rest of UOX3.
//o------------------------------------------------------------------------------------------------o
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
#include "osunique.hpp"

extern CConsole Console;
extern CWorldMain   *cwmWorldState;
extern std::mt19937 generator;

//o------------------------------------------------------------------------------------------------o
// Range check functions
//o------------------------------------------------------------------------------------------------o
bool	ObjInRange( CSocket *mSock, CBaseObject *obj, UI16 distance );
bool	ObjInRange( CBaseObject *a, CBaseObject *b, UI16 distance );
bool	ObjInRangeSquare( CBaseObject *a, CBaseObject *b, UI16 distance );
bool	ObjInOldRange( CBaseObject *a, CBaseObject *b, UI16 distance );
bool	ObjInOldRangeSquare( CBaseObject *a, CBaseObject *b, UI16 distance );
bool	CharInRange( CChar *a, CChar *b );
UI16	GetDist( CBaseObject *a, CBaseObject *b );
UI16	GetDist( Point3_st a, Point3_st b );
UI16	GetOldDist( CBaseObject *a, CBaseObject *b );
UI16	GetDist3D( CBaseObject *a, CBaseObject *b );
UI16	GetDist3D( Point3_st a, Point3_st b );
auto	FindPlayersInVisrange( CBaseObject *myObj ) -> std::vector<CSocket *>;
auto	FindPlayersInOldVisrange( CBaseObject *myObj ) -> std::vector<CSocket *>;
auto	FindNearbyPlayers( SI16 x, SI16 y, SI08 z, UI16 distance ) -> std::vector<CSocket *>;
auto	FindNearbyPlayers( CBaseObject *myObj, UI16 distance ) -> std::vector<CSocket *>;
auto	FindNearbyPlayers( CBaseObject *myObj ) -> std::vector<CSocket *>;
auto	FindNearbyPlayers( CChar *mChar ) -> std::vector<CSocket *>;

//o------------------------------------------------------------------------------------------------o
// Multi functions
//o------------------------------------------------------------------------------------------------o
CMultiObj *	FindMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceId );
CMultiObj *	FindMulti( CBaseObject *i );

//o------------------------------------------------------------------------------------------------o
// Item functions
//o------------------------------------------------------------------------------------------------o
CItem *GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceId );
CItem *FindItemNearXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 id, UI16 instanceId );

//o------------------------------------------------------------------------------------------------o
// Calculation functions (socket, char, item and so forth)
//o------------------------------------------------------------------------------------------------o
CItem *		CalcItemObjFromSer( SERIAL targSerial );
CChar *		CalcCharObjFromSer( SERIAL targSerial );
CMultiObj *	CalcMultiFromSer( SERIAL targSerial );
inline UI32 CalcSerial( UI08 a1, UI08 a2, UI08 a3, UI08 a4 )
{
	return (( a1 << 24 ) + ( a2 << 16 ) + ( a3 << 8 ) + a4 );
}

//o------------------------------------------------------------------------------------------------o
// Socket stuff
//o------------------------------------------------------------------------------------------------o
auto 	SendVecsAsGump( CSocket *sock, std::vector<std::string>& one, std::vector<std::string>& two, UI32 type, SERIAL serial ) -> void;
void	SendMapChange( UI08 worldNumber, CSocket *sock, bool initialLogin = false );
bool	IsOnline( CChar& mChar );

//o------------------------------------------------------------------------------------------------o
// Light related functions
//o------------------------------------------------------------------------------------------------o
void	DoLight( CSocket *s, UI08 level );
void	DoLight( CChar *mChar, UI08 level );
void	DoLight( CItem *mItem, UI08 level );

//o------------------------------------------------------------------------------------------------o
// Poison related functions
//o------------------------------------------------------------------------------------------------o
TIMERVAL GetPoisonDuration( UI08 poisonStrength );
TIMERVAL GetPoisonTickTime( UI08 poisonStrength );

//o------------------------------------------------------------------------------------------------o
// Amount related
//o------------------------------------------------------------------------------------------------o
UI32	GetItemAmount( CChar *s, UI16 realId, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0, bool colorCheck = false, bool moreCheck = false, std::string sectionId = "" );
UI32	GetTotalItemCount( CItem *objCont );
UI32	DeleteItemAmount( CChar *s, UI32 amount, UI16 realId, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0, bool colorCheck = false, bool moreCheck = false, std::string sectionId = "" );
UI32	DeleteSubItemAmount( CItem *p, UI32 amount, UI16 realId, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0, bool colorCheck = false, bool moreCheck = false, std::string sectionId = "" );
UI32	GetBankCount( CChar *p, UI16 itemId, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0 );
UI32	DeleteBankItem( CChar *p, UI32 amt, UI16 itemId, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0 );
UI32    DeleteQuiverItemAmount( CChar *s, UI32 amount, UI16 realId, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0, bool colorCheck = false, bool moreCheck = false, std::string sectionId = "" );

//o------------------------------------------------------------------------------------------------o
// Region related
//o------------------------------------------------------------------------------------------------o
CTownRegion *CalcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, CBaseObject *mObj = nullptr );
void CheckCharInsideBuilding( CChar *c, CSocket *mSock, bool doWeatherStuff );

//o------------------------------------------------------------------------------------------------o
// Find functions
//o------------------------------------------------------------------------------------------------o
CChar *			FindItemOwner( CItem *p );
CBaseObject *	FindItemOwner( CItem *i, ObjectType &objType );
CItem *			FindRootContainer( CItem *i );
CItem *			FindItemOfType( CChar *toFind, ItemTypes type );
CItem *			FindItemOfSectionId( CChar *toFind, std::string sectionId );
CItem *			FindItem( CChar *toFind, UI16 itemId );

//o------------------------------------------------------------------------------------------------o
// Reputation Stuff
//o------------------------------------------------------------------------------------------------o
void	Karma( CChar *nCharId, CChar *nKilledId, const SI16 nKarma );
void	Fame( CChar *nCharId, const SI16 nFame );
void	UpdateFlag( CChar *mChar );

//o------------------------------------------------------------------------------------------------o
// Combat Stuff
//o------------------------------------------------------------------------------------------------o
void	MakeCriminal( CChar *c );
void	FlagForStealing( CChar *c );
bool	WillResultInCriminal( CChar *mChar, CChar *targ );
void	CallGuards( CChar *mChar, CChar *targChar );
void	CallGuards( CChar *mChar );

//o------------------------------------------------------------------------------------------------o
// Time Functions
//o------------------------------------------------------------------------------------------------o
inline TIMERVAL BuildTimeValue( R32 timeFromNow )
{
	return static_cast<TIMERVAL>( cwmWorldState->GetUICurrentTime() + ( static_cast<R32>( 1000 ) * timeFromNow ));
}

UI32	GetClock( void );
inline char *	RealTime( char *time_str )
{
	auto timet = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	struct tm curtime;
	lcltime( timet, curtime );
	strftime( time_str, 256, "%B %d %I:%M:%S %p", &curtime );
	return time_str;
}
inline char *	RealTime24( char *time_str )
{
	auto timet = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	struct tm curtime;
	lcltime( timet, curtime );
	strftime( time_str, 256, "%B %d %H:%M:%S", &curtime );
	return time_str;
}
inline char *	RealTimeDate( char *time_str )
{
	auto timet = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	struct tm curtime;
	lcltime( timet, curtime );
	strftime( time_str, 256, "%B %d", &curtime );
	return time_str;
}

#if P_TIMESTAMP
inline std::string TimeStamp( void )
{
	auto stamp = []()
	{
		auto milli = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		auto hours = ((( milli / 1000 ) / 60 ) / 60 );
		milli = milli - ( hours * 60 * 60 * 1000 );
		auto minutes = (( milli / 1000 ) / 60 );
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
inline void StartMilliTimer( UI32 &Seconds, UI32 &Milliseconds )
{
	auto timenow	= std::chrono::system_clock::now().time_since_epoch();
	auto sec		= std::chrono::duration_cast<std::chrono::seconds>( timenow ).count();
	auto milli		= std::chrono::duration_cast<std::chrono::milliseconds>( timenow ).count() - ( 1000 * sec );
	Seconds			= static_cast<UI32>( sec );
	Milliseconds	= static_cast<UI32>( milli );
}
inline UI32 CheckMilliTimer( UI32 &Seconds, UI32 &Milliseconds )
{
	auto timenow	= std::chrono::system_clock::now().time_since_epoch();
	auto sec		= std::chrono::duration_cast<std::chrono::seconds>( timenow ).count();
	auto milli		= std::chrono::duration_cast<std::chrono::milliseconds>( timenow ).count() - ( 1000 * sec );
	return static_cast<UI32>(( 1000 *( sec - Seconds )) + ( milli - Milliseconds ));
}

inline UI32 GetMinutesSinceEpoch()
{
	return static_cast<UI32>( std::chrono::duration_cast<std::chrono::minutes>( std::chrono::system_clock::now().time_since_epoch() ).count() );
}

//o------------------------------------------------------------------------------------------------o
// Misc Functions
//o------------------------------------------------------------------------------------------------o
bool		IsNumber( const std::string& str );
bool		FileExists( const std::string& filepath );
void		DismountCreature( CChar *s );
size_t		GetTileName( CItem& mItem, std::string& itemname );
std::string	GetNpcDictName( CChar *mChar, CSocket *tSock = nullptr, UI08 requestSource = 0 );
std::string	GetNpcDictTitle( CChar *mChar, CSocket *tSock = nullptr );
bool		LineOfSight( CSocket *s, CChar *mChar, SI16 x2, SI16 y2, SI08 z2, UI08 checkfor, bool useSurfaceZ, SI08 z2Top = 0, bool checkDistance = true );
bool		CheckItemLineOfSight( CChar *mChar, CItem *i );
void		Shutdown( SI32 retCode );
void		HandleDeath( CChar *mChar, CChar *attacker );
void		NpcResurrectTarget( CChar *s );

inline bool ValidateObject( const CBaseObject *toValidate )
{
	bool rValue = true;
	try
	{
		if( toValidate == nullptr )
		{
			rValue = false;
		}
		else if( toValidate->IsDeleted() )
		{
			rValue = false;
		}
	}
	catch( ... )
	{
		rValue = false;
		Console.Error( oldstrutil::format( "Invalid Object found: 0x%X", (UI64)toValidate ));
	}
	return rValue;
}

template<class T>
inline T RandomNum( T nLowNum, T nHighNum )
{
	auto high = std::max<T>( nLowNum, nHighNum );
	auto low = std::min<T>( nLowNum, nHighNum );
	if( std::is_floating_point<T>::value )
	{
		auto distribution = std::uniform_real_distribution<double>( low, high );
		return distribution( generator );
	}
	auto distribution = std::uniform_int_distribution<T>( low, high );
	return distribution( generator );
}

template<class T>
inline T HalfRandomNum( T HighRange )
{
	T halfSize = static_cast<T>( HighRange / 2 );
	return RandomNum( halfSize, HighRange );
}

#endif

