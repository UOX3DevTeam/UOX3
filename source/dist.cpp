// All funcs in this file are used for item/char distance related situations
// There's a chance that a number of these could become inline
#include "uox3.h"
#include "cRaces.h"
#include "regions.h"

namespace UOX
{
	
//o---------------------------------------------------------------------------o
//|	Function	-	bool checkItemRange( CChar *mChar, CItem *i, UI16 distance )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Checks if an item is within reach
//o---------------------------------------------------------------------------o
bool checkItemRange( CChar *mChar, CItem *i )
{
	if( mChar->IsGM() || mChar->IsCounselor() )
		return true;

	CBaseObject *itemOwner	= i;
	bool checkRange			= false;

	if( i->GetCont() != NULL ) // It's inside another container, we need root container to calculate distance
	{
		ObjectType objType	= OT_CBO;
		CBaseObject *iOwner = FindItemOwner( i, objType );
		if( iOwner != NULL )
			itemOwner = iOwner;
	}
	if( itemOwner == mChar )
		checkRange = true;
	else
		checkRange = objInRange( mChar, itemOwner, DIST_NEARBY );

	return checkRange;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool objInRange( CSocket *mSock, CBaseObject *obj, UI16 distance )
//|	Date		-	2/12/2003
//| Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if BaseObject obj is within a certain distance
//o---------------------------------------------------------------------------o
bool objInRange( CSocket *mSock, CBaseObject *obj, UI16 distance )
{
	CChar *mChar = mSock->CurrcharObj();
	return objInRange( mChar, obj, distance );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool objInRange( CBaseObject *a, CBaseObject *b, UI16 distance )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if an object is within a certain distance of another
//|					object
//o---------------------------------------------------------------------------o
bool objInRange( CBaseObject *a, CBaseObject *b, UI16 distance )
{
	return ( getDist( a, b ) <= distance );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool objInRange( CBaseObject *a, CBaseObject *b, UI16 distance )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if an object is within a certain distance of another
//|					object
//o---------------------------------------------------------------------------o
bool objInOldRange( CBaseObject *a, CBaseObject *b, UI16 distance )
{
	return ( getOldDist( a, b ) <= distance );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool charInRange( CChar *a, CChar *b )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if characters a and b are in visual range
//o---------------------------------------------------------------------------o
bool charInRange( CChar *a, CChar *b )
{
	if( !ValidateObject( a ) )
		return false;
	SI16 visRange		= MAX_VISRANGE + Races->VisRange( a->GetRace() );
	return objInRange( a, b, static_cast<UI16>(visRange) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 getDist( CBaseObject *a, CBaseObject *b )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the distance between two objects
//o---------------------------------------------------------------------------o
UI16 getDist( CBaseObject *a, CBaseObject *b )
{
	if( !ValidateObject( a ) || !ValidateObject( b ) ) 
		return DIST_OUTOFRANGE;
	if( a == b )
		return DIST_SAMETILE;
	if( a->WorldNumber() != b->WorldNumber() )
		return DIST_OUTOFRANGE;
	point3 difference = a->GetLocation() - b->GetLocation();
	return static_cast<UI16>(difference.Mag());
}

UI16 getOldDist( CBaseObject *a, CBaseObject *b )
{
	if( !ValidateObject( a ) || !ValidateObject( b ) ) 
		return DIST_OUTOFRANGE;
	if( a == b )
		return DIST_SAMETILE;
	if( a->WorldNumber() != b->WorldNumber() )
		return DIST_OUTOFRANGE;
	point3 distA;
	point3 distB;
	if( a->CanBeObjType( OT_CHAR ) )
		distA = ((CChar *)a)->GetOldLocation();
	else
		distA = a->GetLocation();
	if( b->CanBeObjType( OT_CHAR ) )
		distB = ((CChar *)b)->GetOldLocation();
	else
		distB = b->GetLocation();
	point3 difference = distA - distB;
	return static_cast<UI16>(difference.Mag());
}

UI16 getDist3D( CBaseObject *a, CBaseObject *b )
{
	if( !ValidateObject( a ) || !ValidateObject( b ) )
		return DIST_OUTOFRANGE;
	if( a == b )
		return DIST_SAMETILE;
	if( a->WorldNumber() != b->WorldNumber() )
		return DIST_OUTOFRANGE;
	point3 difference = a->GetLocation() - b->GetLocation();
	return static_cast<UI16>(difference.Mag3D());
}

}
