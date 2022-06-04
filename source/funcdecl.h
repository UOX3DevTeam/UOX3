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
#include "uox3.h"
#include "cBaseObject.h"
#include "cConsole.h"
#include "osunique.hpp"
#include "StringUtility.hpp"
#include "worldmain.h"


#include <random>
#include <type_traits>
#include <chrono>
#include <sstream>




class CChar;		// cChar.h
class CMultiObj;		//cItem.h
class CItem;		//cItem.h
class CTownRegion;	//townregion.h

extern CConsole Console;
extern CWorldMain   *cwmWorldState;
extern std::mt19937 generator;

//o-----------------------------------------------------------------------------------------------o
// Range check functions
//o-----------------------------------------------------------------------------------------------o
auto	objInRange( CSocket *mSock, CBaseObject *obj, UI16 distance ) ->bool ;
auto	objInRange( CBaseObject *a, CBaseObject *b, UI16 distance )->bool ;
auto	objInRangeSquare( CBaseObject *a, CBaseObject *b, UI16 distance )->bool ;
auto	objInOldRange( CBaseObject *a, CBaseObject *b, UI16 distance )->bool ;
auto	objInOldRangeSquare( CBaseObject *a, CBaseObject *b, UI16 distance )->bool ;
auto	charInRange( CChar *a, CChar *b )->bool ;
auto	getDist( CBaseObject *a, CBaseObject *b )->std::uint16_t ;
auto	getDist( point3 a, point3 b )->std::uint16_t;
auto	getOldDist( CBaseObject *a, CBaseObject *b )->std::uint16_t;
auto	getDist3D( CBaseObject *a, CBaseObject *b )->std::uint16_t;
auto	getDist3D( point3 a, point3 b )->std::uint16_t;
auto	FindPlayersInVisrange( CBaseObject *myObj )->std::vector< CSocket * >;
auto	FindPlayersInOldVisrange( CBaseObject *myObj )->std::vector< CSocket * >;
auto	FindNearbyPlayers( SI16 x, SI16 y, SI08 z, UI16 distance )->std::vector< CSocket * >;
auto	FindNearbyPlayers( CBaseObject *myObj, UI16 distance )->std::vector< CSocket * >;
auto	FindNearbyPlayers( CBaseObject *myObj )->std::vector< CSocket * >;
auto	FindNearbyPlayers( CChar *mChar )->std::vector< CSocket * >;
//o-----------------------------------------------------------------------------------------------o
// Multi functions
//o-----------------------------------------------------------------------------------------------o
auto findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )->CMultiObj *	;
auto findMulti( CBaseObject *i ) ->CMultiObj *	;

//o-----------------------------------------------------------------------------------------------o
// Item functions
//o-----------------------------------------------------------------------------------------------o
auto GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )->CItem *;
auto FindItemNearXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 id, UI16 instanceID )->CItem *;

//o-----------------------------------------------------------------------------------------------o
// Calculation functions (socket, char, item and so forth)
//o-----------------------------------------------------------------------------------------------o
auto calcItemObjFromSer( SERIAL targSerial )->CItem *	;
auto calcCharObjFromSer( SERIAL targSerial )->CChar *	;
auto calcMultiFromSer( SERIAL targSerial ) ->CMultiObj * ;

inline auto calcserial( UI08 a1, UI08 a2, UI08 a3, UI08 a4 ) ->SERIAL {
	return ( (a1<<24) + (a2<<16) + (a3<<8) + a4 );
}

//o-----------------------------------------------------------------------------------------------o
// Socket stuff
//o-----------------------------------------------------------------------------------------------o
auto SendVecsAsGump( CSocket *sock, std::vector<std::string>& one, std::vector<std::string>& two, UI32 type, SERIAL serial ) ->void;
auto SendMapChange( UI08 worldNumber, CSocket *sock, bool initialLogin = false ) ->void ;
auto isOnline( CChar& mChar ) ->bool;

//o-----------------------------------------------------------------------------------------------o
// Light related functions
//o-----------------------------------------------------------------------------------------------o
auto doLight( CSocket *s, UI08 level ) ->void;
auto doLight( CChar *mChar, UI08 level ) ->void ;
auto doLight( CItem *mItem, UI08 level ) ->void ;

//o-----------------------------------------------------------------------------------------------o
// Poison related functions
//o-----------------------------------------------------------------------------------------------o
auto getPoisonDuration( UI08 poisonStrength ) ->TIMERVAL;
auto getPoisonTickTime( UI08 poisonStrength ) ->TIMERVAL;

//o-----------------------------------------------------------------------------------------------o
// Amount related
//o-----------------------------------------------------------------------------------------------o
auto	GetItemAmount( CChar *s, UI16 realID, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0, bool colorCheck = false ) ->UI32 ;
auto	GetTotalItemCount( CItem *objCont ) ->UI32 ;
auto	DeleteItemAmount( CChar *s, UI32 amount, UI16 realID, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0 ) ->UI32 ;
auto	DeleteSubItemAmount( CItem *p, UI32 amount, UI16 realID, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0 )->UI32 ;
auto	GetBankCount( CChar *p, UI16 itemID, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0 ) ->UI32 ;
auto	DeleteBankItem( CChar *p, UI32 amt, UI16 itemID, UI16 realColour = 0x0000, UI32 realMoreVal = 0x0 ) ->UI32 ;

//o-----------------------------------------------------------------------------------------------o
// Region related
//o-----------------------------------------------------------------------------------------------o
auto calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, CBaseObject *mObj = nullptr ) ->CTownRegion *;
auto CheckCharInsideBuilding( CChar *c, CSocket *mSock, bool doWeatherStuff ) ->void;

//o-----------------------------------------------------------------------------------------------o
// Find functions
//o-----------------------------------------------------------------------------------------------o
auto FindItemOwner( CItem *p )->CChar * ;
auto FindItemOwner( CItem *i, ObjectType &objType ) ->CBaseObject *;
auto FindRootContainer( CItem *i ) ->CItem * ;
auto FindItemOfType( CChar *toFind, ItemTypes type ) ->CItem * ;
auto FindItem( CChar *toFind, UI16 itemID ) ->CItem * ;

//o-----------------------------------------------------------------------------------------------o
// Reputation Stuff
//o-----------------------------------------------------------------------------------------------o
auto Karma( CChar *nCharID, CChar *nKilledID, const SI16 nKarma ) ->void ;
auto Fame( CChar *nCharID, const SI16 nFame ) ->void ;
auto UpdateFlag( CChar *mChar ) ->void ;

//o-----------------------------------------------------------------------------------------------o
// Combat Stuff
//o-----------------------------------------------------------------------------------------------o
auto criminal( CChar *c ) ->void ;
auto WillResultInCriminal( CChar *mChar, CChar *targ ) ->bool ;
auto callGuards( CChar *mChar, CChar *targChar ) ->void ;
auto callGuards( CChar *mChar ) ->void ;

//o-----------------------------------------------------------------------------------------------o
// Time Functions
//o-----------------------------------------------------------------------------------------------o
inline auto BuildTimeValue( R32 timeFromNow ) -> TIMERVAL {
	return static_cast<TIMERVAL>( cwmWorldState->GetUICurrentTime() + ( static_cast<R32>(1000) * timeFromNow ) );
}

auto getclock() ->UI32 ;

inline auto RealTime( char *time_str ) ->char* {
	auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm curtime;
	lcltime(timet,curtime);
	strftime( time_str, 256, "%B %d %I:%M:%S %p", &curtime );
	return time_str;
}
inline auto RealTime24( char *time_str ) ->char* {
	auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm curtime;
	lcltime(timet, curtime);
	strftime( time_str, 256, "%B %d %H:%M:%S", &curtime );
	return time_str;
}
inline auto RealTimeDate( char *time_str ) ->char* {
	auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm curtime;
	lcltime(timet, curtime);
	strftime( time_str, 256, "%B %d", &curtime );
	return time_str;
}

#if P_TIMESTAMP
inline auto TimeStamp( void ) ->std::string {
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
inline auto TimeStamp( void ) ->std::string {
	return "";
}
#endif
inline auto StartMilliTimer( std::uint32_t &Seconds, std::uint32_t &Milliseconds ) ->void {
	auto timenow = std::chrono::system_clock::now().time_since_epoch() ;
	auto sec = std::chrono::duration_cast<std::chrono::seconds>(timenow).count() ;
	auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(timenow).count() - (1000*sec);
	Seconds            =  static_cast<std::uint32_t>(sec) ;
	Milliseconds    =  static_cast<std::uint32_t>(milli);
}
inline auto CheckMilliTimer( std::uint32_t &Seconds, std::uint32_t &Milliseconds ) ->std::uint32_t {
	auto timenow = std::chrono::system_clock::now().time_since_epoch() ;
	auto sec = std::chrono::duration_cast<std::chrono::seconds>(timenow).count() ;
	auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(timenow).count() - (1000*sec);
	return static_cast<std::uint32_t>((1000 *(sec - Seconds)) + (milli - Milliseconds));
}

inline auto GetMinutesSinceEpoch()->std::uint32_t {
	return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::minutes>(std::chrono::system_clock::now().time_since_epoch()).count());
}

//o-----------------------------------------------------------------------------------------------o
// Misc Functions
//o-----------------------------------------------------------------------------------------------o
auto roundNumber( R32 toRound) ->R32 ;
auto isNumber( const std::string& str ) ->bool ;
auto FileExists( const std::string& filepath ) ->bool ;
auto DismountCreature( CChar *s ) ->void ;
auto getTileName( CItem& mItem, std::string& itemname ) ->size_t ;
auto getNpcDictName( CChar *mChar, CSocket *tSock = nullptr ) ->std::string ;
auto getNpcDictTitle( CChar *mChar, CSocket *tSock = nullptr ) ->std::string ;
auto LineOfSight( CSocket *s, CChar *mChar, SI16 x2, SI16 y2, SI08 z2, UI08 checkfor, bool useSurfaceZ, SI08 z2Top = 0, bool checkDistance = true ) ->bool ;
auto checkItemLineOfSight( CChar *mChar, CItem *i ) ->bool ;
auto Shutdown( SI32 retCode ) ->void ;
auto HandleDeath( CChar *mChar, CChar *attacker ) ->void ;
auto NpcResurrectTarget( CChar *s ) ->void ;

inline auto ValidateObject( const CBaseObject *toValidate )->bool {
	bool rvalue = true;
	try {
		if( toValidate == nullptr )
			rvalue = false;
		else if( toValidate->isDeleted() )
			rvalue = false;
	}
	catch( ... ) {
		rvalue = false;
		Console.error( oldstrutil::format("Invalid Object found: 0x%X", (UI64)toValidate) );
	}
	return rvalue;
}

template< class T >
inline auto RandomNum( T nLowNum, T nHighNum ) ->T {
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
inline auto HalfRandomNum( T HighRange ) ->T {
	T halfSize = static_cast< T >(HighRange / 2);
	return RandomNum( halfSize, HighRange );
}

#endif

