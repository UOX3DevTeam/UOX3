// All funcs in this file are used for item/char distance related situations
// There's a chance that a number of these could become inline
#include "uox3.h"

//o---------------------------------------------------------------------------o
//|	Function	-	bool checkItemRange( CChar *mChar, CItem *i, UI16 distance )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Checks if an item is within a certain distance (based on
//|					its container and location)
//o---------------------------------------------------------------------------o
bool checkItemRange( CChar *mChar, CItem *i, UI16 distance )
{
	CChar *itemOwner = NULL;

	bool checkRange = false;

	if( i->GetCont() != INVALIDSERIAL ) // It's inside another container, we need root container to calculate distance
	{
		itemOwner = getPackOwner( i );
		if( itemOwner == NULL ) // In a pack on the ground!
		{
			CItem *rootContainer = getRootPack( i );
			if( rootContainer == NULL )
				Console.Error( 2, "Pack with serial %li has an invalid container chain!", i->GetSerial() );
			else
				checkRange = itemInRange( mChar, rootContainer, distance );
		}
	}
	else  // It's on the ground
		checkRange = itemInRange( mChar, i, distance );

	bool isGM = mChar->IsGM();
	bool isCounselor = mChar->IsCounselor();
	bool isPackOwner = ( itemOwner == mChar );
	bool isPetOwner = ( itemOwner != NULL && itemOwner != mChar && (CChar *)(itemOwner->GetOwnerObj()) == mChar );

	if( isGM || isCounselor || isPackOwner || isPetOwner )
		return true;
	else
		return checkRange;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool objInRange( cBaseObject *a, cBaseObject *b, UI16 distance )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if an object is within a certain distance of another
//|					object
//o---------------------------------------------------------------------------o
bool objInRange( cBaseObject *a, cBaseObject *b, UI16 distance )
{
	if( a == b )
		return true;
	if( a->GetObjType() == OT_CHAR )
	{
		if( static_cast<CChar *>(a)->IsGM() )
			return true;
	}
	point3 difference = a->GetLocation() - b->GetLocation();
	return ( difference.MagSquared() <= ( distance * distance ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool itemInRange( cSocket *s, CItem *i, UI16 distance )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if item i is within a certain distance
//o---------------------------------------------------------------------------o
bool itemInRange( CChar *mChar, CItem *i, UI16 distance )
{
	if( mChar == NULL || i == NULL )
		return false;
	if( mChar->IsGM() )
		return true;
	if( mChar->WorldNumber() != i->WorldNumber() )
		return false;
	point3 difference = mChar->GetLocation() - i->GetLocation();
	return ( difference.MagSquared() <= ( distance * distance ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool itemInRange( cSocket *s, CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if item i in visual range
//o---------------------------------------------------------------------------o
bool itemInRange( CChar *mChar, CItem *i )
{
	if( mChar == NULL || i == NULL )
		return true;
	SI08 vr;
	if( i->GetID( 1 ) >= 0x40 ) 
		vr = BUILDRANGE;
	else
		vr = MAXVISRANGE + Races->VisRange( mChar->GetRace() );
	point3 difference = mChar->GetLocation() - i->GetLocation();
	if( abs( difference.x ) > vr )
		return false;
	if( abs( difference.y ) > vr )
		return false;
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool charInRange( cSocket *a, cSocket *b )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if characters connected to socket a and socket b are
//|					in visual range
//o---------------------------------------------------------------------------o
bool charInRange( cSocket *a, cSocket *b )
{
	if( a == b ) 
		return false;
	CChar *charA = a->CurrcharObj();
	CChar *charB = b->CurrcharObj();
	if( charA == false || charB == false )
		return false;
	if( charA->WorldNumber() != charB->WorldNumber() )
		return false;
	SI16 visRange = a->Range() + Races->VisRange( charA->GetRace() ) + 1;
	point3 difference = charA->GetLocation() - charB->GetLocation();
	return ( difference.MagSquared() <= ( visRange * visRange ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool charInRange( CChar *a, CChar *b )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if characters a and b are in visual range
//o---------------------------------------------------------------------------o
bool charInRange( CChar *a, CChar *b )
{
	if( a == NULL || b == NULL )
		return false;
	if( a == b )
		return true;
	if( a->WorldNumber() != b->WorldNumber() )
		return false;
	SI16 visRange = MAXVISRANGE + Races->VisRange( a->GetRace() );
	point3 difference = a->GetLocation() - b->GetLocation();
	return ( difference.MagSquared() <= ( visRange * visRange ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool charInRange( cSocket *s, CChar *i, UI16 distance )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if character i is within a certain distance of the
//|					character attached to socket s
//o---------------------------------------------------------------------------o
bool charInRange( cSocket *s, CChar *i, UI16 distance )
{
	if( i == NULL || s == NULL )
		return true;
	CChar *mChar = s->CurrcharObj();
	if( mChar->IsGM() )	
		return true;
	if( mChar == i )
		return true;
	if( mChar->WorldNumber() != i->WorldNumber() )
		return false;
	point3 difference = mChar->GetLocation() - i->GetLocation();
	return( difference.MagSquared() <= ( distance * distance ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 getCharDist( CChar *a, CChar *b )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the distance between characters a and b
//o---------------------------------------------------------------------------o
UI16 getCharDist( CChar *a, CChar *b )
{
	if( a == NULL || b == NULL) 
		return 30;
	if( a == b )
		return 0;
	if( a->WorldNumber() != b->WorldNumber() )
		return 0xFFFF;
	point3 difference = a->GetLocation() - b->GetLocation();
	return (UI16)(difference.Mag());
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 getItemDist( CChar *mChar, CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the distance between mChar item i
//o---------------------------------------------------------------------------o
UI16 getItemDist( CChar *mChar, CItem *i )
{
	if( mChar == NULL || i == NULL ) 
		return 30;
	if( mChar->WorldNumber() != i->WorldNumber() )
		return 0xFFFF;
	point3 difference = mChar->GetLocation() - i->GetLocation();
	return (UI16)(difference.Mag());
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 getDist( cBaseObject *a, cBaseObject *b )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the distance between two objects
//o---------------------------------------------------------------------------o
UI16 getDist( cBaseObject *a, cBaseObject *b )
{
	if( a == NULL || b == NULL ) 
		return 30;
	if( a->WorldNumber() != b->WorldNumber() )
		return 0xFFFF;
	if( a->GetObjType() == OT_CHAR )
	{
		if( static_cast<CChar *>(a)->IsGM() )
			return 0;
	}
	point3 difference = a->GetLocation() - b->GetLocation();
	return (SI16)difference.Mag();
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool inBankRange( CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Checks if player is in the range of a banker
//o---------------------------------------------------------------------------o
bool inBankRange( CChar *i )
{
	if( i == NULL ) 
		return false;
	SI16 x = i->GetX(), y = i->GetY();

	UI08 worldNumber = i->WorldNumber();
	SubRegion *Cell = MapRegion->GetCell( x, y, worldNumber );
	if( Cell != NULL )
	{
		Cell->PushChar();
		for( CChar *bankCheck = Cell->FirstChar(); !Cell->FinishedChars(); bankCheck = Cell->GetNextChar() )
		{
			if( bankCheck->GetNPCAiType() == 0x08 )
			{
				if( getCharDist( i, bankCheck ) <= 6 )
				{
					Cell->PopChar();
					return true;
				}
			}
		}
		Cell->PopChar();
	}
	return false;
}

