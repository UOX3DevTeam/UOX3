#include "uox3.h"
#include "regions.h"
#include "network.h"
#include "cRaces.h"
#include "classes.h"

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	auto FindPlayersInOldVisrange( CBaseObject *myObj )->std::vector< CSocket * >
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players who previously were in visual range of an object
//o-----------------------------------------------------------------------------------------------o
auto FindPlayersInOldVisrange( CBaseObject *myObj ) ->std::vector< CSocket * >{

	std::vector< CSocket * > nearbyChars;
	for (auto &mSock : Network->connClients){
		auto mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ) ){
			if( myObj->GetObjType() == OT_MULTI ){
				if( objInOldRangeSquare( myObj, mChar, static_cast<UI16>(DIST_BUILDRANGE) ) ){
					nearbyChars.push_back( mSock );
				}
			}
			else {
				auto visRange = static_cast<UI16>(mSock->Range() + Races->VisRange( mChar->GetRace() ));
				if( objInOldRangeSquare( myObj, mChar, visRange ) ){
					nearbyChars.push_back( mSock );
				}
			}
		}
	}
	return nearbyChars;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	auto FindPlayersInVisrange( CBaseObject *myObj ) ->std::vector< CSocket * >
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players in visual range of an object
//o-----------------------------------------------------------------------------------------------o
auto FindPlayersInVisrange( CBaseObject *myObj )->std::vector< CSocket * >{

	std::vector< CSocket * > nearbyChars;
	for (auto &mSock : Network->connClients){
		auto mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar )) {
			auto visRange = static_cast<UI16>(mSock->Range() + Races->VisRange( mChar->GetRace() ));
			if( objInRangeSquare( myObj, mChar, visRange ) ){
				nearbyChars.push_back( mSock );
			}
		}
	}
	return nearbyChars;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	auto FindNearbyPlayers( CBaseObject *myObj, UI16 distance )->std::vector< CSocket * >
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players who within a certain distance of an object
//o-----------------------------------------------------------------------------------------------o
auto FindNearbyPlayers( CBaseObject *myObj, UI16 distance ) ->std::vector< CSocket * >{

	std::vector< CSocket * > nearbyChars;
	for (auto &mSock : Network->connClients){
		auto mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar )){
			if( objInRange( mChar, myObj, distance ) ){
				nearbyChars.push_back( mSock );
			}
		}
	}
	return nearbyChars;
}
auto FindNearbyPlayers( CChar *mChar ) ->std::vector< CSocket * >{

	UI16 visRange = MAX_VISRANGE;
	if( mChar->GetSocket() != nullptr )
		visRange = static_cast<UI16>(mChar->GetSocket()->Range() + Races->VisRange( mChar->GetRace() ));
	else
		visRange += static_cast<UI16>(Races->VisRange( mChar->GetRace() ));
	return FindNearbyPlayers( mChar, visRange );
}

auto FindNearbyPlayers( CBaseObject *mObj ) ->std::vector< CSocket * >{

	UI16 visRange = static_cast<UI16>(MAX_VISRANGE + Races->VisRange( mObj->GetRace() ));
	return FindNearbyPlayers( mObj, visRange );
}

auto FindNearbyPlayers( SI16 x, SI16 y, SI08 z, UI16 distance )->std::vector< CSocket * > {
	std::vector< CSocket * > nearbyChars;
	for (auto &mSock : Network->connClients){
		auto mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar )){
			if( getDist( point3( mChar->GetX(), mChar->GetY(), mChar->GetZ() ), point3( x, y, z ) ) <= distance ){
				nearbyChars.push_back( mSock );
			}
		}
	}
	return nearbyChars;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	auto findNearbyChars( CChar *mChar, distLocs distance ) -> std::vector< CChar* >
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of characters (PC or NPC) that are within a certain distance
//o-----------------------------------------------------------------------------------------------o
auto findNearbyChars( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance ) ->std::vector< CChar* >{

	std::vector< CChar* > ourChars;
	for (auto &CellResponse : MapRegion->PopulateList( x, y, worldNumber )){
		if( CellResponse){
			auto regChars = CellResponse->GetCharList();
			for (auto &tempChar:regChars->collection()){
				if( ValidateObject( tempChar ) && tempChar->GetInstanceID() == instanceID ){
					if( tempChar->GetX() <= x + distance || tempChar->GetX() >= x - distance || tempChar->GetY() <= y + distance || tempChar->GetY() >= y - distance ){
						ourChars.push_back( tempChar );
					}
				}
			}
			
		}
	}
	return ourChars;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject *FindItemOwner( CItem *i, ObjectType &objType )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the root container object, returns it, and sets objType
//|					to the objects type
//o-----------------------------------------------------------------------------------------------o
CBaseObject *FindItemOwner( CItem *i, ObjectType &objType )
{
	if( !ValidateObject( i ) || i->GetCont() == nullptr )	// Item has no containing item
		return nullptr;

	while( i->GetCont() != nullptr )
	{
		if( i->GetContSerial() < BASEITEMSERIAL )
		{
			objType = OT_CHAR;
			return i->GetCont();
		}
		else
			i = static_cast<CItem *>(i->GetCont());
	}
	objType = OT_ITEM;
	return i;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *FindRootContainer( CItem *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the root container item and returns it
//o-----------------------------------------------------------------------------------------------o
CItem *FindRootContainer( CItem *i )
{
	if( !ValidateObject( i ) || i->GetCont() == nullptr )
		return nullptr;

	while( i->GetCont() != nullptr )
	{
		if( i->GetContSerial() < BASEITEMSERIAL )
			break;
		else
			i = static_cast<CItem *>(i->GetCont());
	}
	return i;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CChar *FindItemOwner( CItem *p )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the character who owns the item (if any)
//o-----------------------------------------------------------------------------------------------o
CChar *FindItemOwner( CItem *p )
{
	if( !ValidateObject( p ) || p->GetCont() == nullptr )
		return nullptr;

	ObjectType oType = OT_CBO;
	CBaseObject *iOwner = FindItemOwner( p, oType );
	if( oType == OT_CHAR )
		return static_cast<CChar *>(iOwner);
	return nullptr;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *SearchSubPackForItem( CItem *toSearch, UI16 itemID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Search character's subpacks for items of specific ID
//o-----------------------------------------------------------------------------------------------o
auto SearchSubPackForItem( CItem *toSearch, UI16 itemID ) ->CItem *{
	auto tsCont = toSearch->GetContainsList();
	for (auto &toCheck:tsCont->collection()){
		if( ValidateObject( toCheck ) ) {
			if( toCheck->GetID() == itemID ){	// it's in our hand
				return toCheck;
			}				// we've found the first occurance on the person!
			else if( toCheck->GetType() == IT_CONTAINER || toCheck->GetType() == IT_LOCKEDCONTAINER )	{// search any subpacks, specifically pack and locked containers
				auto packSearchResult = SearchSubPackForItem( toCheck, itemID );
				if( ValidateObject( packSearchResult ) ){
					return packSearchResult;
				}
			}
		}
	}
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *FindItem( CChar *toFind, UI16 itemID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Look for items of a certain ID in character's pack
//o-----------------------------------------------------------------------------------------------o
CItem *FindItem( CChar *toFind, UI16 itemID )
{
	for( CItem *toCheck = toFind->FirstItem(); !toFind->FinishedItems(); toCheck = toFind->NextItem() )
	{
		if( ValidateObject( toCheck ) )
		{
			if( toCheck->GetID() == itemID )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( toCheck->GetLayer() == IL_PACKITEM )	// could use packitem, but we're already in the same type of loop, so we'll check it ourselves
			{
				CItem *packSearchResult = SearchSubPackForItem( toCheck, itemID );
				if( ValidateObject( packSearchResult ) )
					return packSearchResult;
			}
		}
	}
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *SearchSubPackForItem( CItem *toSearch, ItemTypes type )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Search character's subpacks for items of specific type
//o-----------------------------------------------------------------------------------------------o
auto SearchSubPackForItemOfType( CItem *toSearch, ItemTypes type )->CItem *{
	auto tsCont = toSearch->GetContainsList();
	for (auto &toCheck : tsCont->collection()){
		if( ValidateObject( toCheck ) ) {
			if( toCheck->GetType() == type ){
				// it's in our hand
				return toCheck;
			}				// we've found the first occurance on the person!
			else if( toCheck->GetType() == IT_CONTAINER || toCheck->GetType() == IT_LOCKEDCONTAINER )	{
				// search any subpacks, specifically pack and locked containers
				auto packSearchResult = SearchSubPackForItemOfType( toCheck, type );
				if( ValidateObject( packSearchResult ) )
					return packSearchResult;
			}
		}
	}
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *FindItemOfType( CChar *toFind, ItemTypes type )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Look for items of a certain type in character's pack
//o-----------------------------------------------------------------------------------------------o
CItem *FindItemOfType( CChar *toFind, ItemTypes type )
{
	for( CItem *toCheck = toFind->FirstItem(); !toFind->FinishedItems(); toCheck = toFind->NextItem() )
	{
		if( ValidateObject( toCheck ) )
		{
			if( toCheck->GetType() == type )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( toCheck->GetLayer() == IL_PACKITEM )	// could use packitem, but we're already in the same type of loop, so we'll check it ourselves
			{
				CItem *packSearchResult = SearchSubPackForItemOfType( toCheck, type );
				if( ValidateObject( packSearchResult ) )
					return packSearchResult;
			}
		}
	}
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool inMulti( SI16 x, SI16 y, SI08 z, CMultiObj *m )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if object is in a multi
//o-----------------------------------------------------------------------------------------------o
bool inMulti( SI16 x, SI16 y, SI08 z, CMultiObj *m )
{
	if( !ValidateObject( m ) )
		return false;
	const UI16 multiID = static_cast<UI16>(m->GetID() - 0x4000);
	SI32 length = 0;
	
	if( !Map->multiExists( multiID ) )
	{
		// the length associated with the multi means one thing
		// the multi it's trying to reference is NOT in the multis.mul file
		// so as a measure... if it's wet, we'll make it a boat
		// if it's dry, we'll make it a house
		Console << "inmulti() - Bad length in multi file, avoiding stall. Item Name: " << m->GetName() << " " << m->GetSerial() << myendl;
		length = 0;
		
		const map_st map1 = Map->SeekMap( m->GetX(), m->GetY(), m->WorldNumber() );
		CLand& land = Map->SeekLand( map1.id );
		if( land.CheckFlag( TF_WET ) ) // is it water?
		{
			// NOTE: We have an intrinsic issue here: It is of type CMultiObj, not CBoat
			// So either: 1) Let the user fix it in the worldfile once its saved
			// 2) Destroy the CMultiObj, create a new CBoatObj, and set to the same serial
			m->SetID( 0x4001 );
		}
		else
			m->SetID( 0x4064 );
	}
	else
	{
		UI08 zOff = m->CanBeObjType( OT_BOAT ) ? 3 : 20;
		const SI16 baseX = m->GetX();
		const SI16 baseY = m->GetY();
		const SI08 baseZ = m->GetZ();
		
		for( auto &multi : Map->SeekMulti( multiID ).items )
		{
			// Ignore signs and signposts sticking out of buildings
			if((( multi.tileid >= 0x0b95 ) && ( multi.tileid <= 0x0c0e )) || (( multi.tileid == 0x1f28 ) || ( multi.tileid == 0x1f29 )))
				continue;
			
			if( (baseX + multi.offsetx) == x && (baseY + multi.offsety) == y )
			{
				// Find the top Z level of the multi section being examined
				const SI08 multiZ = (baseZ + multi.altitude + Map->TileHeight( multi.tileid ) );
				if( m->GetObjType() == OT_BOAT )
				{
					// We're on a boat!
					if( abs( multiZ - z ) <= zOff )
						return true;
				}
				else
				{
					if( z >= multiZ || abs( multiZ - z ) <= zOff )
						return true;
				}
			}
		}
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj *findMulti( CBaseObject *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find multi at an object's location
//o-----------------------------------------------------------------------------------------------o
CMultiObj *findMulti( CBaseObject *i )
{
	if( !ValidateObject( i ) )
		return nullptr;
	return findMulti( i->GetX(), i->GetY(), i->GetZ(), i->WorldNumber(), i->GetInstanceID() );
}

template< class T >
inline T hypotenuse( T sideA, T sideB )
{
	T sumSquares	= (sideA * sideA) + (sideB * sideB);
	T retVal		= static_cast<T>(sqrt( (R64)sumSquares ));
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj *findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )
//|	Changes		-	(06/07/2020) Added instanceID support
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find multis at specified location
//o-----------------------------------------------------------------------------------------------o
auto findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )->CMultiObj *{
	SI32 lastdist = 30;
	CMultiObj *multi = nullptr;
	SI32 ret, dx, dy;

	for (auto &toCheck : MapRegion->PopulateList( x, y, worldNumber )){
		if( toCheck){
			auto regItems = toCheck->GetItemList();
			for (auto &itemCheck : regItems->collection()){
				if( ValidateObject( itemCheck )&& itemCheck->GetInstanceID() == instanceID ){
					if( itemCheck->GetID( 1 ) >= 0x40 && itemCheck->CanBeObjType( OT_MULTI ) ) {
						dx = abs( x - itemCheck->GetX() );
						dy = abs( y - itemCheck->GetY() );
						ret = (SI32)( hypotenuse( dx, dy ) );
						if( ret <= lastdist ) {
							lastdist = ret;
							multi = static_cast<CMultiObj *>( itemCheck );
							if( inMulti( x, y, z, multi ) ) {
								return multi;
							}
							else{
								multi = nullptr;
							}
						}
					}
				}
			}
		}
	}
	return multi;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find items at specified location
//o-----------------------------------------------------------------------------------------------o
auto GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID ) -> CItem *{
	auto toCheck = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( toCheck ){	// no valid region
		auto regItems = toCheck->GetItemList();
		for (auto &itemCheck:regItems->collection()){
			if( ValidateObject( itemCheck ) && itemCheck->GetInstanceID() == instanceID ){
				if( itemCheck->GetX() == x && itemCheck->GetY() == y && itemCheck->GetZ() == z ) {
					return itemCheck;
				}
			}
		}
	}
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *FindItemNearXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 id, UI16 instanceID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find items near specified location
//o-----------------------------------------------------------------------------------------------o
CItem *FindItemNearXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 id, UI16 instanceID )
{
	UI16 oldDist		= DIST_OUTOFRANGE;
	UI16 currDist;
	CItem *currItem		= nullptr;
	point3 targLocation = point3( x, y, z );
	for (auto &toCheck : MapRegion->PopulateList( x, y, worldNumber )){
		if( toCheck){
			auto regItems = toCheck->GetItemList();
			for (auto &itemCheck: regItems->collection()){
				if( ValidateObject( itemCheck ) && itemCheck->GetInstanceID() == instanceID ){
					
					if( itemCheck->GetID() == id && itemCheck->GetZ() == z ) {
						point3 difference	= itemCheck->GetLocation() - targLocation;
						currDist			= static_cast<UI16>(difference.Mag());
						if( currDist < oldDist) {
							oldDist = currDist;
							currItem = itemCheck;
						}
					}
				}
			}
			
		}
	}
	return currItem;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	auto findNearbyItems( CBaseObject *mObj, distLocs distance )->std::vector< CItem* >
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of Items that are within a certain distance of a given object
//o-----------------------------------------------------------------------------------------------o
auto findNearbyItems( CBaseObject *mObj, distLocs distance )->std::vector< CItem* >{

	std::vector< CItem* > ourItems;
	for (auto &CellResponse : MapRegion->PopulateList( mObj)){
		if(CellResponse){
			auto regItems = CellResponse->GetItemList();
			for( CItem *Item = regItems->First(); !regItems->Finished(); Item = regItems->Next() ) {
				if( ValidateObject( Item ) && Item->GetInstanceID() == mObj->GetInstanceID() ) {
					if( objInRange( mObj, Item, distance ) ){
						ourItems.push_back( Item );
					}
				}
			}
		}
	}
	return ourItems;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	auto findNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance ) ->std::vector< CItem* >
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of Items that are within a certain distance of a location
//o-----------------------------------------------------------------------------------------------o
auto findNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance ) ->std::vector< CItem* >{

	std::vector< CItem* > ourItems;
	for (auto &CellResponse : MapRegion->PopulateList( x, y, worldNumber )){
		if(CellResponse){
			
			auto regItems = CellResponse->GetItemList();
			for (auto &Item : regItems->collection()){
				if( ValidateObject( Item ) && Item->GetInstanceID() == instanceID ){
					if( getDist( Item->GetLocation(), point3( x, y, Item->GetZ() )) <= distance ){
						ourItems.push_back( Item );
					}
				}
			}
		}
	}
	return ourItems;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	auto findNearbyObjects( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance  )->std::vector< CBaseObject* >
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of BaseObjects that are within a certain distance of a location
//o-----------------------------------------------------------------------------------------------o
auto findNearbyObjects( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance ) ->std::vector< CBaseObject* >{
	std::vector< CBaseObject* >	 ourObjects;
	for (auto &CellResponse : MapRegion->PopulateList( x, y, worldNumber )){
		if(CellResponse){
			
			auto regItems = CellResponse->GetItemList();
			for (auto &Item:regItems->collection()){
				if( ValidateObject( Item ) && Item->GetInstanceID() == instanceID ){
					if( getDist( Item->GetLocation(), point3( x, y, Item->GetZ() )) <= distance ){
						ourObjects.push_back( Item );
					}
				}
			}
			
			auto regChars = CellResponse->GetCharList();
			for (auto &Character : regChars->collection()){
				if( ValidateObject( Character ) && Character->GetInstanceID() == instanceID ){
					if( getDist( Character->GetLocation(), point3( x, y, Character->GetZ() )) <= distance ){
						ourObjects.push_back( Character );
					}
				}
			}
		}
	}
	return ourObjects;
}
