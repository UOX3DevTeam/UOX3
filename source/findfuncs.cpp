#include "uox3.h"
#include "regions.h"
#include "network.h"
#include "cRaces.h"
#include "classes.h"

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SOCKLIST FindPlayersInOldVisrange( CBaseObject *myObj )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players who previously were in visual range of an object
//o-----------------------------------------------------------------------------------------------o
SOCKLIST FindPlayersInOldVisrange( CBaseObject *myObj )
{
	SOCKLIST nearbyChars;
	//std::scoped_lock lock(Network->internallock);
	Network->pushConn();
	for( CSocket *mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ) )
		{
			if( myObj->GetObjType() == OT_MULTI )
			{
				if( objInOldRangeSquare( myObj, mChar, static_cast<UI16>(DIST_BUILDRANGE) ) )
				{
					nearbyChars.push_back( mSock );
				}
			}
			else
			{
				UI16 visRange = static_cast<UI16>(mSock->Range() + Races->VisRange( mChar->GetRace() ));
				if( objInOldRangeSquare( myObj, mChar, visRange ) )
				{
					nearbyChars.push_back( mSock );
				}
			}
		}
	}
	Network->popConn();
	return nearbyChars;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SOCKLIST FindPlayersInVisrange( CBaseObject *myObj )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players in visual range of an object
//o-----------------------------------------------------------------------------------------------o
SOCKLIST FindPlayersInVisrange( CBaseObject *myObj )
{
	SOCKLIST nearbyChars;
	//std::scoped_lock lock(Network->internallock);
	Network->pushConn();
	for( CSocket *mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ))
		{
			UI16 visRange = static_cast<UI16>(mSock->Range() + Races->VisRange( mChar->GetRace() ));
			if( objInRangeSquare( myObj, mChar, visRange ) )
			{
				nearbyChars.push_back( mSock );
			}
		}
	}
	Network->popConn();
	return nearbyChars;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SOCKLIST FindNearbyPlayers( CBaseObject *myObj, UI16 distance )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find players who within a certain distance of an object
//o-----------------------------------------------------------------------------------------------o
SOCKLIST FindNearbyPlayers( CBaseObject *myObj, UI16 distance )
{
	SOCKLIST nearbyChars;
	//std::scoped_lock lock(Network->internallock);
	Network->pushConn();
	for( CSocket *mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ))
		{
			if( objInRange( mChar, myObj, distance ) )
				nearbyChars.push_back( mSock );
		}
	}
	Network->popConn();
	return nearbyChars;
}
SOCKLIST FindNearbyPlayers( CChar *mChar )
{
	UI16 visRange = static_cast<UI16>(MAX_VISRANGE + Races->VisRange( mChar->GetRace() ));
	return FindNearbyPlayers( mChar, visRange );
}

SOCKLIST FindNearbyPlayers( CBaseObject *mObj )
{
	UI16 visRange = static_cast<UI16>(MAX_VISRANGE + Races->VisRange( mObj->GetRace() ));
	return FindNearbyPlayers( mObj, visRange );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CHARLIST findNearbyChars( CChar *mChar, distLocs distance )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of characters (PC or NPC) that are within a certain distance
//o-----------------------------------------------------------------------------------------------o
CHARLIST findNearbyChars( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance )
{
	CHARLIST ourChars;
	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *CellResponse = (*rIter);
		if( CellResponse == nullptr )
			continue;

		GenericList< CChar * > *regChars = CellResponse->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) || tempChar->GetInstanceID() != instanceID )
				continue;
			if( tempChar->GetX() <= x + distance || tempChar->GetX() >= x - distance || tempChar->GetY() <= y + distance || tempChar->GetY() >= y - distance )
				ourChars.push_back( tempChar );
		}
		regChars->Pop();
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
CItem *SearchSubPackForItem( CItem *toSearch, UI16 itemID )
{
	GenericList< CItem * > *tsCont = toSearch->GetContainsList();
	for( CItem *toCheck = tsCont->First(); !tsCont->Finished(); toCheck = tsCont->Next() )
	{
		if( ValidateObject( toCheck ) )
		{
			if( toCheck->GetID() == itemID )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( toCheck->GetType() == IT_CONTAINER || toCheck->GetType() == IT_LOCKEDCONTAINER )	// search any subpacks, specifically pack and locked containers
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
CItem *SearchSubPackForItemOfType( CItem *toSearch, ItemTypes type )
{
	GenericList< CItem * > *tsCont = toSearch->GetContainsList();
	for( CItem *toCheck = tsCont->First(); !tsCont->Finished(); toCheck = tsCont->Next() )
	{
		if( ValidateObject( toCheck ) )
		{
			if( toCheck->GetType() == type )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( toCheck->GetType() == IT_CONTAINER || toCheck->GetType() == IT_LOCKEDCONTAINER )	// search any subpacks, specifically pack and locked containers
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
		
		for( auto &multi : Map->SeekMulti( multiID ).allItems() )
		{
			// Ignore signs and signposts sticking out of buildings
			if((( multi.tileid >= 0x0b95 ) && ( multi.tileid <= 0x0c0e )) || (( multi.tileid == 0x1f28 ) || ( multi.tileid == 0x1f29 )))
				continue;
			
			if( (baseX + multi.xoffset) == x && (baseY + multi.yoffset) == y )
			{
				// Find the top Z level of the multi section being examined
				const SI08 multiZ = (baseZ + multi.zoffset + Map->TileHeight( multi.tileid ) );
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
CMultiObj *findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )
{
	SI32 lastdist = 30;
	CMultiObj *multi = nullptr;
	SI32 ret, dx, dy;

	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *toCheck = (*rIter);
		if( toCheck == nullptr )	// no valid region
			continue;
		GenericList< CItem * > *regItems = toCheck->GetItemList();
		regItems->Push();
		for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
		{
			if( !ValidateObject( itemCheck ) || itemCheck->GetInstanceID() != instanceID )
				continue;
			if( itemCheck->GetID( 1 ) >= 0x40 && itemCheck->CanBeObjType( OT_MULTI ) )
			{
				dx = abs( x - itemCheck->GetX() );
				dy = abs( y - itemCheck->GetY() );
				ret = (SI32)( hypotenuse( dx, dy ) );
				if( ret <= lastdist )
				{
					lastdist = ret;
					multi = static_cast<CMultiObj *>( itemCheck );
					if( inMulti( x, y, z, multi ) )
					{
						regItems->Pop();
						return multi;
					}
					else
						multi = nullptr;
				}
			}
		}
		regItems->Pop();
	}
	return multi;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find items at specified location
//o-----------------------------------------------------------------------------------------------o
CItem *GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )
{
	CMapRegion *toCheck = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( toCheck != nullptr )	// no valid region
	{
		GenericList< CItem * > *regItems = toCheck->GetItemList();
		regItems->Push();
		for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
		{
			if( !ValidateObject( itemCheck ) || itemCheck->GetInstanceID() != instanceID )
				continue;
			if( itemCheck->GetX() == x && itemCheck->GetY() == y && itemCheck->GetZ() == z )
			{
				regItems->Pop();
				return itemCheck;
			}
		}
		regItems->Pop();
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
	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *toCheck = (*rIter);
		if( toCheck == nullptr )	// no valid region
			continue;
		GenericList< CItem * > *regItems = toCheck->GetItemList();
		regItems->Push();
		for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
		{
			if( !ValidateObject( itemCheck ) || itemCheck->GetInstanceID() != instanceID )
				continue;
			if( itemCheck->GetID() == id && itemCheck->GetZ() == z )
			{
				point3 difference	= itemCheck->GetLocation() - targLocation;
				currDist			= static_cast<UI16>(difference.Mag());
				if( currDist < oldDist)
				{
					oldDist = currDist;
					currItem = itemCheck;
				}
			}
		}
		regItems->Pop();
	}
	return currItem;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ITEMLIST findNearbyItems( CBaseObject *mObj, distLocs distance )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of Items that are within a certain distance of a given object
//o-----------------------------------------------------------------------------------------------o
ITEMLIST findNearbyItems( CBaseObject *mObj, distLocs distance )
{
	ITEMLIST ourItems;
	REGIONLIST nearbyRegions = MapRegion->PopulateList( mObj );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *CellResponse = (*rIter);
		if( CellResponse == nullptr )
			continue;

		GenericList< CItem * > *regItems = CellResponse->GetItemList();
		regItems->Push();
		for( CItem *Item = regItems->First(); !regItems->Finished(); Item = regItems->Next() )
		{
			if( !ValidateObject( Item ) || Item->GetInstanceID() != mObj->GetInstanceID() )
				continue;
			if( objInRange( mObj, Item, distance ) )
				ourItems.push_back( Item );
		}
		regItems->Pop();
	}
	return ourItems;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ITEMLIST findNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of Items that are within a certain distance of a location
//o-----------------------------------------------------------------------------------------------o
ITEMLIST findNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance )
{
	ITEMLIST ourItems;
	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *CellResponse = (*rIter);
		if( CellResponse == nullptr )
			continue;

		GenericList< CItem * > *regItems = CellResponse->GetItemList();
		regItems->Push();
		for( CItem *Item = regItems->First(); !regItems->Finished(); Item = regItems->Next() )
		{
			if( !ValidateObject( Item ) || Item->GetInstanceID() != instanceID )
				continue;
			if( getDist( Item->GetLocation(), point3( x, y, Item->GetZ() )) <= distance )
				ourItems.push_back( Item );
		}
		regItems->Pop();
	}
	return ourItems;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	BASOBJECTLIST findNearbyObjects( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance  )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of BaseObjects that are within a certain distance of a location
//o-----------------------------------------------------------------------------------------------o
BASEOBJECTLIST findNearbyObjects( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance )
{
	BASEOBJECTLIST ourObjects;
	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *CellResponse = (*rIter);
		if( CellResponse == nullptr )
			continue;

		GenericList< CItem * > *regItems = CellResponse->GetItemList();
		regItems->Push();
		for( CItem *Item = regItems->First(); !regItems->Finished(); Item = regItems->Next() )
		{
			if( !ValidateObject( Item ) || Item->GetInstanceID() != instanceID )
				continue;
			if( getDist( Item->GetLocation(), point3( x, y, Item->GetZ() )) <= distance )
				ourObjects.push_back( Item );
		}
		regItems->Pop();

		GenericList< CChar * > *regChars = CellResponse->GetCharList();
		regItems->Push();
		for( CChar *Character = regChars->First(); !regChars->Finished(); Character = regChars->Next() )
		{
			if( !ValidateObject( Character ) || Character->GetInstanceID() != instanceID )
				continue;
			if( getDist( Character->GetLocation(), point3( x, y, Character->GetZ() )) <= distance )
				ourObjects.push_back( Character );
		}
		regChars->Pop();
	}
	return ourObjects;
}
