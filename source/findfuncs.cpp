#include "uox3.h"
#include "regions.h"
#include "network.h"
#include "cRaces.h"
#include "classes.h"

namespace UOX
{

SOCKLIST FindPlayersInOldVisrange( CBaseObject *myObj )
{
	SOCKLIST nearbyChars;
	Network->PushConn();
	for( CSocket *mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ) )
		{
			if( objInOldRange( myObj, mChar, static_cast<UI16>(mSock->Range() + Races->VisRange( mChar->GetRace() )) ) )
				nearbyChars.push_back( mSock );
		}
	}
	Network->PopConn();
	return nearbyChars;
}

SOCKLIST FindPlayersInVisrange( CBaseObject *myObj )
{
	SOCKLIST nearbyChars;
	Network->PushConn();
	for( CSocket *mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ) )
		{
			if( objInRange( mChar, myObj, static_cast<UI16>(mSock->Range() + Races->VisRange( mChar->GetRace() )) ) )
				nearbyChars.push_back( mSock );
		}
	}
	Network->PopConn();
	return nearbyChars;
}

SOCKLIST FindNearbyPlayers( CBaseObject *myObj, UI16 distance )
{
	SOCKLIST nearbyChars;
	Network->PushConn();
	for( CSocket *mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
	{
		if( objInRange( mSock->CurrcharObj(), myObj, distance ) )
			nearbyChars.push_back( mSock );
	}
	Network->PopConn();
	return nearbyChars;
}
SOCKLIST FindNearbyPlayers( CChar *mChar )
{
	UI16 visRange = static_cast<UI16>(MAX_VISRANGE + Races->VisRange( mChar->GetRace() ));
	return FindNearbyPlayers( mChar, visRange );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CBaseObject *FindItemOwner( CItem *i, ObjectType &objType )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Finds the root container object, returns it, and sets objType
//|					to the objects type
//o---------------------------------------------------------------------------o
CBaseObject *FindItemOwner( CItem *i, ObjectType &objType )
{
	if( !ValidateObject( i ) || i->GetCont() == NULL )	// Item has no containing item
		return NULL;

	while( i->GetCont() != NULL )
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

//o---------------------------------------------------------------------------o
//|	Function	-	CChar *FindItemOwner( CItem *p )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns the character who owns the item (if any)
//o---------------------------------------------------------------------------o
CChar *FindItemOwner( CItem *p )
{
	if( !ValidateObject( p ) || p->GetCont() == NULL )
		return NULL;

	ObjectType oType = OT_CBO;
	CBaseObject *iOwner = FindItemOwner( p, oType );
	if( oType == OT_CHAR )
		return static_cast<CChar *>(iOwner);
	return NULL;
}


//o---------------------------------------------------------------------------o
//|	Function	-	CItem *SearchSubPackForItem( CItem *toSearch, UI16 itemID )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Search character's subpacks for items of specific ID
//o---------------------------------------------------------------------------o
CItem *SearchSubPackForItem( CItem *toSearch, UI16 itemID )
{
	CDataList< CItem * > *tsCont = toSearch->GetContainsList();
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
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *FindItem( CChar *toFind, UI16 itemID )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Look for items of a certain ID in character's pack
//o---------------------------------------------------------------------------o
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
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *SearchSubPackForItem( CItem *toSearch, ItemTypes type )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Search character's subpacks for items of specific type
//o---------------------------------------------------------------------------o
CItem *SearchSubPackForItemOfType( CItem *toSearch, ItemTypes type )
{
	CDataList< CItem * > *tsCont = toSearch->GetContainsList();
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
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *FindItem( CChar *toFind, ItemTypes type )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Look for items of a certain type in character's pack
//o---------------------------------------------------------------------------o
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
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool inMulti( SI16 x, SI16 y, SI08 z, CItem *m )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if item is in a multi
//o---------------------------------------------------------------------------o
bool inMulti( SI16 x, SI16 y, SI08 z, CMultiObj *m )
{
	if( !ValidateObject( m ) )
		return false;
	const UI16 multiID = static_cast<UI16>(m->GetID() - 0x4000);
	SI32 length = Map->SeekMulti( multiID );
	if( length == -1 || length >= 17000000 )
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

	UI08 zOff = 1;
	if( m->CanBeObjType( OT_BOAT ) )
		zOff = 3;

	const SI16 baseX = m->GetX();
	const SI16 baseY = m->GetY();
	const SI08 baseZ = m->GetZ();
	for( SI32 j = 0; j < length; ++j )
	{
		Multi_st& multi = Map->SeekIntoMulti( multiID, j );

		if( (baseX + multi.x) == x && (baseY + multi.y) == y )
		{
			const SI08 multiZ = (baseZ + multi.z + Map->TileHeight( multi.tile ) );
			if( abs( multiZ - z ) <= zOff )
				return true;
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CMultiObj *findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find a multi at x,y,z
//o---------------------------------------------------------------------------o
CMultiObj *findMulti( CBaseObject *i )
{
	if( !ValidateObject( i ) )
		return NULL;
	return findMulti( i->GetX(), i->GetY(), i->GetZ(), i->WorldNumber() );
}

template< class T >
inline T hypotenuse( T sideA, T sideB )
{
	T sumSquares	= (sideA * sideA) + (sideB * sideB);
	T retVal		= sqrt( (R64)sumSquares );
	return retVal;
}

CMultiObj *findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber )
{
	SI32 lastdist = 30;
	CMultiObj *multi = NULL;
	SI32 ret, dx, dy;

	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *toCheck = (*rIter);
		if( toCheck == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = toCheck->GetItemList();
		regItems->Push();
		for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
		{
			if( !ValidateObject( itemCheck ) )
				continue;
			if( itemCheck->GetID( 1 ) >= 0x40 && itemCheck->CanBeObjType( OT_MULTI ) )
			{
				dx = abs( x - itemCheck->GetX() );
				dy = abs( y - itemCheck->GetY() );
				ret = (SI32)( hypotenuse( dx, dy ) );
				if( ret <= lastdist )
				{
					lastdist = ret;
					multi = static_cast<CMultiObj *>(itemCheck);
					if( inMulti( x, y, z, multi ) )
					{
						regItems->Pop();
						return multi;
					}
					else
						multi = NULL;
				}
			}
		}
		regItems->Pop();
	}
	return multi;
}

CItem *GetItemAtXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber )
{
	CMapRegion *toCheck = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( toCheck != NULL )	// no valid region
	{
		CDataList< CItem * > *regItems = toCheck->GetItemList();
		regItems->Push();
		for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
		{
			if( !ValidateObject( itemCheck ) )
				continue;
			if( itemCheck->GetX() == x && itemCheck->GetY() == y && itemCheck->GetZ() == z )
			{
				regItems->Pop();
				return itemCheck;
			}
		}
		regItems->Pop();
	}
	return NULL;
}

CItem *FindItemNearXYZ( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 id )
{
	UI16 oldDist		= DIST_OUTOFRANGE;
	UI16 currDist;
	CItem *currItem		= NULL;
	point3 targLocation = point3( x, y, z );
	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *toCheck = (*rIter);
		if( toCheck == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = toCheck->GetItemList();
		regItems->Push();
		for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
		{
			if( !ValidateObject( itemCheck ) )
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
}
