#include "uox3.h"
#include "regions.h"
#include "network.h"
#include "cRaces.h"
#include "classes.h"

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindPlayersInOldVisrange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players who previously were in visual range of an object
//o------------------------------------------------------------------------------------------------o
auto FindPlayersInOldVisrange( CBaseObject *myObj ) -> std::vector<CSocket *>
{
	std::vector<CSocket *> nearbyChars;
	for( auto &mSock : Network->connClients )
	{
		auto mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ))
		{
			if( myObj->GetObjType() == OT_MULTI )
			{
				if( ObjInOldRangeSquare( myObj, mChar, static_cast<UI16>( DIST_BUILDRANGE )))
				{
					nearbyChars.push_back( mSock );
				}
			}
			else
			{
				auto visRange = static_cast<UI16>( mSock->Range() + Races->VisRange( mChar->GetRace() ));
				if( ObjInOldRangeSquare( myObj, mChar, visRange ))
				{
					nearbyChars.push_back( mSock );
				}
			}
		}
	}
	return nearbyChars;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindPlayersInVisrange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players in visual range of an object
//o------------------------------------------------------------------------------------------------o
auto FindPlayersInVisrange( CBaseObject *myObj ) -> std::vector<CSocket *>
{
	std::vector<CSocket *> nearbyChars;
	for( auto &mSock : Network->connClients )
	{
		auto mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ))
		{
			auto visRange = static_cast<UI16>( mSock->Range() + Races->VisRange( mChar->GetRace() ));
			if( ObjInRangeSquare( myObj, mChar, visRange ))
			{
				nearbyChars.push_back( mSock );
			}
		}
	}
	return nearbyChars;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindNearbyPlayers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players who within a certain distance of an object
//o------------------------------------------------------------------------------------------------o
auto FindNearbyPlayers( CBaseObject *myObj, UI16 distance ) -> std::vector<CSocket *>
{
	std::vector<CSocket *> nearbyChars;
	for( auto &mSock : Network->connClients )
	{
		auto mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ))
		{
			if( ObjInRange( mChar, myObj, distance ))
			{
				nearbyChars.push_back( mSock );
			}
		}
	}
	return nearbyChars;
}
auto FindNearbyPlayers( CChar *mChar ) -> std::vector<CSocket *>
{
	UI16 visRange = MAX_VISRANGE;
	if( mChar->GetSocket() != nullptr )
	{
		visRange = static_cast<UI16>( mChar->GetSocket()->Range() + Races->VisRange( mChar->GetRace() ));
	}
	else
	{
		visRange += static_cast<UI16>( Races->VisRange( mChar->GetRace() ));
	}
	return FindNearbyPlayers( mChar, visRange );
}

auto FindNearbyPlayers( CBaseObject *mObj ) -> std::vector<CSocket *>
{
	UI16 visRange = static_cast<UI16>( MAX_VISRANGE + Races->VisRange( mObj->GetRace() ));
	return FindNearbyPlayers( mObj, visRange );
}

auto FindNearbyPlayers( SI16 x, SI16 y, SI08 z, UI16 distance ) -> std::vector<CSocket *>
{
	std::vector<CSocket *> nearbyChars;
	for( auto &mSock : Network->connClients )
	{
		auto mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ))
		{
			if( GetDist( Point3_st( mChar->GetX(), mChar->GetY(), mChar->GetZ() ), Point3_st( x, y, z )) <= distance )
			{
				nearbyChars.push_back( mSock );
			}
		}
	}
	return nearbyChars;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindNearbyChars()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of characters (PC or NPC) that are within a certain distance
//o------------------------------------------------------------------------------------------------o
auto FindNearbyChars( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CChar *>
{
	std::vector<CChar*> ourChars;
	for( auto &CellResponse : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( CellResponse )
		{
			auto regChars = CellResponse->GetCharList();
			for( auto &tempChar : regChars->collection() )
			{
				if( ValidateObject( tempChar ) && tempChar->GetInstanceId() == instanceId )
				{
					if( tempChar->GetX() <= x + distance || tempChar->GetX() >= x - distance || tempChar->GetY() <= y + distance || tempChar->GetY() >= y - distance )
					{
						ourChars.push_back( tempChar );
					}
				}
			}
		}
	}
	return ourChars;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindItemOwner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the root container object, returns it, and sets objType
//|					to the objects type
//o------------------------------------------------------------------------------------------------o
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
		{
			i = static_cast<CItem *>( i->GetCont() );
		}
	}
	objType = OT_ITEM;
	return i;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindRootContainer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the root container item and returns it
//o------------------------------------------------------------------------------------------------o
CItem *FindRootContainer( CItem *i )
{
	if( !ValidateObject( i ) || i->GetCont() == nullptr )
		return nullptr;

	while( i->GetCont() != nullptr )
	{
		if( i->GetContSerial() < BASEITEMSERIAL )
		{
			break;
		}
		else
		{
			i = static_cast<CItem *>( i->GetCont() );
		}
	}
	return i;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindItemOwner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the character who owns the item (if any)
//o------------------------------------------------------------------------------------------------o
CChar *FindItemOwner( CItem *p )
{
	if( !ValidateObject( p ) || p->GetCont() == nullptr )
		return nullptr;

	ObjectType oType = OT_CBO;
	CBaseObject *iOwner = FindItemOwner( p, oType );
	if( oType == OT_CHAR )
	{
		return static_cast<CChar *>( iOwner );
	}
	return nullptr;
}


//o------------------------------------------------------------------------------------------------o
//|	Function	-	SearchSubPackForItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Search character's subpacks for items of specific ID
//o------------------------------------------------------------------------------------------------o
auto SearchSubPackForItem( CItem *toSearch, UI16 itemId ) ->CItem *
{
	auto tsCont = toSearch->GetContainsList();
	for( const auto &toCheck : tsCont->collection() )
	{
		if( ValidateObject( toCheck ))
		{
			if( toCheck->GetId() == itemId ) // it's in our hand
			{
				return toCheck; // we've found the first occurance on the person!
			}
			else if( toCheck->GetType() == IT_CONTAINER || toCheck->GetType() == IT_LOCKEDCONTAINER )
			{
				// search any subpacks, specifically pack and locked containers
				auto packSearchResult = SearchSubPackForItem( toCheck, itemId );
				if( ValidateObject( packSearchResult ))
				{
					return packSearchResult;
				}
			}
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Look for items of a certain ID in character's pack
//o------------------------------------------------------------------------------------------------o
CItem *FindItem( CChar *toFind, UI16 itemId )
{
	for( CItem *toCheck = toFind->FirstItem(); !toFind->FinishedItems(); toCheck = toFind->NextItem() )
	{
		if( ValidateObject( toCheck ))
		{
			if( toCheck->GetId() == itemId ) // it's in our hand
			{
				return toCheck; // we've found the first occurance on the person!
			}
			else if( toCheck->GetLayer() == IL_PACKITEM ) // could use packItem, but we're already in the same type of loop, so we'll check it ourselves
			{
				CItem *packSearchResult = SearchSubPackForItem( toCheck, itemId );
				if( ValidateObject( packSearchResult ))
				{
					return packSearchResult;
				}
			}
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SearchSubPackForItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Search character's subpacks for items of specific type
//o------------------------------------------------------------------------------------------------o
auto SearchSubPackForItemOfType( CItem *toSearch, ItemTypes type )->CItem *
{
	auto tsCont = toSearch->GetContainsList();
	for( const auto &toCheck : tsCont->collection() )
	{
		if( ValidateObject( toCheck ))
		{
			if( toCheck->GetType() == type ) // it's in our hand
			{
				return toCheck; // we've found the first occurance on the person!
			}
			else if( toCheck->GetType() == IT_CONTAINER || toCheck->GetType() == IT_LOCKEDCONTAINER )
			{
				// search any subpacks, specifically pack and locked containers
				auto packSearchResult = SearchSubPackForItemOfType( toCheck, type );
				if( ValidateObject( packSearchResult ))
				{
					return packSearchResult;
				}
			}
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindItemOfType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Look for items of a certain type in character's pack
//o------------------------------------------------------------------------------------------------o
CItem *FindItemOfType( CChar *toFind, ItemTypes type )
{
	for( CItem *toCheck = toFind->FirstItem(); !toFind->FinishedItems(); toCheck = toFind->NextItem() )
	{
		if( ValidateObject( toCheck ))
		{
			if( toCheck->GetType() == type ) // it's in our hand
			{
				return toCheck;	// we've found the first occurance on the person!
			}
			else if( toCheck->GetLayer() == IL_PACKITEM ) // could use packItem, but we're already in the same type of loop, so we'll check it ourselves
			{
				CItem *packSearchResult = SearchSubPackForItemOfType( toCheck, type );
				if( ValidateObject( packSearchResult ))
				{
					return packSearchResult;
				}
			}
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	InMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if object is in a multi
//o------------------------------------------------------------------------------------------------o
bool InMulti( SI16 x, SI16 y, SI08 z, CMultiObj *m )
{
	if( !ValidateObject( m ))
		return false;

	const UI16 multiId = static_cast<UI16>( m->GetId() - 0x4000 );
	[[maybe_unused]] SI32 length = 0;

	if( !Map->MultiExists( multiId ))
	{
		// the length associated with the multi means one thing
		// the multi it's trying to reference is NOT in the multis.mul file
		// so as a measure... if it's wet, we'll make it a boat
		// if it's dry, we'll make it a house
		Console << "inmulti() - Bad length in multi file, avoiding stall. Item Name: " << m->GetName() << " " << m->GetSerial() << myendl;
		length = 0;

		auto map1 = Map->SeekMap( m->GetX(), m->GetY(), m->WorldNumber() );
		if( map1.CheckFlag( TF_WET )) // is it water?
		{
			// NOTE: We have an intrinsic issue here: It is of type CMultiObj, not CBoat
			// So either: 1) Let the user fix it in the worldfile once its saved
			// 2) Destroy the CMultiObj, create a new CBoatObj, and set to the same serial
			m->SetId( 0x4001 );
		}
		else
		{
			m->SetId( 0x4064 );
		}
	}
	else
	{
		UI08 zOff = m->CanBeObjType( OT_BOAT ) ? 3 : 20;
		const SI16 baseX = m->GetX();
		const SI16 baseY = m->GetY();
		const SI08 baseZ = m->GetZ();

		for( auto &multi : Map->SeekMulti( multiId ).items )
		{
			// Ignore signs and signposts sticking out of buildings
			if((( multi.tileId >= 0x0b95 ) && ( multi.tileId <= 0x0c0e )) || (( multi.tileId == 0x1f28 ) || ( multi.tileId == 0x1f29 )))
				continue;

			if(( baseX + multi.offsetX ) == x && ( baseY + multi.offsetY ) == y )
			{
				// Find the top Z level of the multi section being examined
				const SI08 multiZ = ( baseZ + multi.altitude + Map->TileHeight( multi.tileId ));
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindMulti( CBaseObject *i )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find multi at an object's location
//o------------------------------------------------------------------------------------------------o
CMultiObj *FindMulti( CBaseObject *i )
{
	if( !ValidateObject( i ))
		return nullptr;

	return FindMulti( i->GetX(), i->GetY(), i->GetZ(), i->WorldNumber(), i->GetInstanceId() );
}

template< class T >
inline T hypotenuse( T sideA, T sideB )
{
	T sumSquares	= ( sideA * sideA ) + ( sideB * sideB );
	T retVal		= static_cast<T>( sqrt( static_cast<R64>( sumSquares )));
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceId )
//|	Changes		-	(06/07/2020) Added instanceId support
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find multis at specified location
//o------------------------------------------------------------------------------------------------o
auto FindMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceId )->CMultiObj *
{
	SI32 lastdist = 30;
	CMultiObj *multi = nullptr;
	SI32 ret, dx, dy;

	for( auto &toCheck : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( toCheck == nullptr )
			continue;

		auto regItems = toCheck->GetItemList();
		for( const auto &itemCheck : regItems->collection() )
		{
			if( !ValidateObject( itemCheck ) || itemCheck->GetInstanceId() != instanceId )
				continue;

			if( itemCheck->GetId( 1 ) >= 0x40 && itemCheck->CanBeObjType( OT_MULTI ))
			{
				dx = abs( x - itemCheck->GetX() );
				dy = abs( y - itemCheck->GetY() );
				ret = static_cast<SI32>( hypotenuse( dx, dy ));
				if( ret <= lastdist )
				{
					lastdist = ret;
					multi = static_cast<CMultiObj *>( itemCheck );
					if( InMulti( x, y, z, multi ))
					{
						return multi;
					}
					else
					{
						multi = nullptr;
					}
				}
			}
		}
	}
	return multi;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetItemAtXYZ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find items at specified location
//o------------------------------------------------------------------------------------------------o
auto GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceId ) -> CItem *
{
	auto toCheck = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( toCheck ) // no valid region
	{
		auto regItems = toCheck->GetItemList();
		for( const auto &itemCheck : regItems->collection() )
		{
			if( ValidateObject( itemCheck ) && itemCheck->GetInstanceId() == instanceId )
			{
				if( itemCheck->GetX() == x && itemCheck->GetY() == y && itemCheck->GetZ() == z )
				{
					return itemCheck;
				}
			}
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindItemNearXYZ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find items near specified location
//o------------------------------------------------------------------------------------------------o
CItem *FindItemNearXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 id, UI16 instanceId )
{
	UI16 oldDist		= DIST_OUTOFRANGE;
	UI16 currDist;
	CItem *currItem		= nullptr;
	Point3_st targLocation = Point3_st( x, y, z );
	for( auto &toCheck : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( toCheck == nullptr ) // no valid region
			continue;

		auto regItems = toCheck->GetItemList();
		for( const auto &itemCheck: regItems->collection() )
		{
			if( !ValidateObject( itemCheck ) || itemCheck->GetInstanceId() != instanceId )
				continue;

			if( itemCheck->GetId() == id && itemCheck->GetZ() == z )
			{
				Point3_st difference	= itemCheck->GetLocation() - targLocation;
				currDist = static_cast<UI16>( difference.Mag() );
				if( currDist < oldDist )
				{
					oldDist = currDist;
					currItem = itemCheck;
				}
			}
		}
	}
	return currItem;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindNearbyItems( CBaseObject *mObj, distLocs distance )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of Items that are within a certain distance of a given object
//o------------------------------------------------------------------------------------------------o
auto FindNearbyItems( CBaseObject *mObj, distLocs distance ) -> std::vector<CItem *>
{
	std::vector<CItem*> ourItems;
	for( auto &CellResponse : MapRegion->PopulateList( mObj ))
	{
		if( CellResponse == nullptr )
			continue;

		auto regItems = CellResponse->GetItemList();
		for( CItem *Item = regItems->First(); !regItems->Finished(); Item = regItems->Next() )
		{
			if( !ValidateObject( Item ) || Item->GetInstanceId() != mObj->GetInstanceId() )
				continue;

			if( ObjInRange( mObj, Item, distance ))
			{
				ourItems.push_back( Item );
			}
		}
	}
	return ourItems;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of Items that are within a certain distance of a location
//o------------------------------------------------------------------------------------------------o
auto FindNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CItem *>
{
	std::vector<CItem*> ourItems;
	for( auto &cellResponse : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( cellResponse == nullptr )
			continue;

		auto regItems = cellResponse->GetItemList();
		for( const auto &Item : regItems->collection() )
		{
			if( !ValidateObject( Item ) || Item->GetInstanceId() != instanceId )
				continue;

			if( GetDist( Item->GetLocation(), Point3_st( x, y, Item->GetZ() )) <= distance )
			{
				ourItems.push_back( Item );
			}
		}
	}
	return ourItems;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindNearbyObjects()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of BaseObjects that are within a certain distance of a location
//o------------------------------------------------------------------------------------------------o
auto FindNearbyObjects( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CBaseObject *>
{
	std::vector<CBaseObject *>	 ourObjects;
	for( auto &CellResponse : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( CellResponse == nullptr )
			continue;

		auto regItems = CellResponse->GetItemList();
		for( const auto &item : regItems->collection() )
		{
			if( !ValidateObject( item ) || item->GetInstanceId() != instanceId )
				continue;

			if( GetDist( item->GetLocation(), Point3_st( x, y, item->GetZ() )) <= distance )
			{
				ourObjects.push_back( item );
			}
		}

		auto regChars = CellResponse->GetCharList();
		for( const auto &character : regChars->collection() )
		{
			if( !ValidateObject( character ) || character->GetInstanceId() != instanceId )
				continue;

			if( GetDist( character->GetLocation(), Point3_st( x, y, character->GetZ() )) <= distance )
			{
				ourObjects.push_back( character );
			}
		}
	}
	return ourObjects;
}
