// All funcs in this file are used for item/char distance related situations
// There's a chance that a number of these could become inline
#include "uox3.h"
#include "cRaces.h"
#include "regions.h"

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckItemRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if an item is within reach
//o------------------------------------------------------------------------------------------------o
bool CheckItemRange( CChar *mChar, CItem *i )
{
	if( mChar->IsGM() || mChar->IsCounselor() )
		return true;

	CBaseObject *itemOwner	= i;
	bool checkRange			= false;

	if( i->GetCont() != nullptr ) // It's inside another container, we need root container to calculate distance
	{
		ObjectType objType	= OT_CBO;
		CBaseObject *iOwner = FindItemOwner( i, objType );
		if( iOwner != nullptr )
		{
			itemOwner = iOwner;
		}
	}
	if( itemOwner == mChar )
	{
		checkRange = true;
	}
	else
	{
		if( ValidateObject( itemOwner ))
		{
			if( mChar->GetInstanceId() != itemOwner->GetInstanceId() || mChar->WorldNumber() != itemOwner->WorldNumber() )
				return false;
		}
		else
		{
			if( mChar->GetInstanceId() != i->GetInstanceId() || mChar->WorldNumber() != i->WorldNumber() )
				return false;
		}

		checkRange = ObjInRange( mChar, itemOwner, DIST_NEARBY );
	}

	return checkRange;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjInRange()
//|	Date		-	2/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if BaseObject obj is within a certain distance
//o------------------------------------------------------------------------------------------------o
bool ObjInRange( CSocket *mSock, CBaseObject *obj, UI16 distance )
{
	CChar *mChar = mSock->CurrcharObj();
	return ObjInRange( mChar, obj, distance );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjInRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if an object is within a certain distance of another object
//o------------------------------------------------------------------------------------------------o
bool ObjInRange( CBaseObject *a, CBaseObject *b, UI16 distance )
{
	return ( GetDist( a, b ) <= distance );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjInRangeSquare()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if an object's location is within a certain distance of another
//|					object, but checking using a square instead of a radius
//o------------------------------------------------------------------------------------------------o
bool ObjInRangeSquare( CBaseObject *a, CBaseObject *b, UI16 distance )
{
	if( !ValidateObject( a ) || !ValidateObject( b ))
		return false;

	if( a == b )
		return true;

	if( a->WorldNumber() != b->WorldNumber() || a->GetInstanceId() != b->GetInstanceId() )
		return false;

	auto aX = a->GetX();
	auto aY = a->GetY();
	auto bX = b->GetX();
	auto bY = b->GetY();
	return ( aX >= ( bX - distance ) && aX <= ( bX + distance )
		&& aY >= ( bY - distance ) && aY <= ( bY + distance ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjInOldRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if an object is within a certain distance of another object
//o------------------------------------------------------------------------------------------------o
bool ObjInOldRange( CBaseObject *a, CBaseObject *b, UI16 distance )
{
	return ( GetOldDist( a, b ) <= distance );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjInOldRangeSquare()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if an object's old location is within a certain distance of another
//|					object, but checking using a square instead of a radius
//o------------------------------------------------------------------------------------------------o
bool ObjInOldRangeSquare( CBaseObject *a, CBaseObject *b, UI16 distance )
{
	if( !ValidateObject( a ) || !ValidateObject( b ))
		return false;

	if( a == b )
		return true;

	if( a->WorldNumber() != b->WorldNumber() || a->GetInstanceId() != b->GetInstanceId() )
		return false;

	Point3_st aOldLoc = a->GetOldLocation();
	auto aX = aOldLoc.x;
	auto aY = aOldLoc.y;
	auto bX = b->GetX();
	auto bY = b->GetY();
	return ( aX >= ( bX - distance ) && aX <= ( bX + distance )
		&& aY >= ( bY - distance ) && aY <= ( bY + distance ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CharInRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if characters a and b are in visual range
//o------------------------------------------------------------------------------------------------o
bool CharInRange( CChar *a, CChar *b )
{
	if( !ValidateObject( a ))
		return false;

	SI16 visRange = MAX_VISRANGE;
	if( a->GetSocket() != nullptr )
	{
		visRange = a->GetSocket()->Range() + Races->VisRange( a->GetRace() );
	}
	else
	{
		visRange += Races->VisRange( a->GetRace() );
	}
	return ObjInRangeSquare( a, b, static_cast<UI16>( visRange ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetDist()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the distance between two objects
//o------------------------------------------------------------------------------------------------o
UI16 GetDist( CBaseObject *a, CBaseObject *b )
{
	if( !ValidateObject( a ) || !ValidateObject( b ))
		return DIST_OUTOFRANGE;

	if( a == b )
		return DIST_SAMETILE;

	if( a->WorldNumber() != b->WorldNumber() || a->GetInstanceId() != b->GetInstanceId() )
		return DIST_OUTOFRANGE;

	return GetDist( a->GetLocation(), b->GetLocation() );
}

UI16 GetDist( Point3_st a, Point3_st b )
{
	Point3_st difference = a - b;
	return static_cast<UI16>( difference.Mag() );
}

UI16 GetDist3D( Point3_st a, Point3_st b )
{
	Point3_st difference = a - b;
	return static_cast<UI16>( difference.Mag3D() );
}

UI16 GetOldDist( CBaseObject *a, CBaseObject *b )
{
	if( !ValidateObject( a ) || !ValidateObject( b ))
		return DIST_OUTOFRANGE;

	if( a == b )
		return DIST_SAMETILE;

	if( a->WorldNumber() != b->WorldNumber() || a->GetInstanceId() != b->GetInstanceId() )
		return DIST_OUTOFRANGE;

	Point3_st distA;
	Point3_st distB;
	distA = a->GetOldLocation();
	distB = b->GetLocation();
	Point3_st difference = distA - distB;
	return static_cast<UI16>( difference.Mag() );
}

UI16 GetDist3D( CBaseObject *a, CBaseObject *b )
{
	if( !ValidateObject( a ) || !ValidateObject( b ))
		return DIST_OUTOFRANGE;

	if( a == b )
		return DIST_SAMETILE;

	if( a->WorldNumber() != b->WorldNumber() || a->GetInstanceId() != b->GetInstanceId() )
		return DIST_OUTOFRANGE;

	Point3_st difference = a->GetLocation() - b->GetLocation();
	return static_cast<UI16>( difference.Mag3D() );
}
