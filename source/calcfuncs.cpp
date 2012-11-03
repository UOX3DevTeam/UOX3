//o--------------------------------------------------------------------------o
//|	File					-	calcfuncs.cpp
//|	Date					-	
//|	Developers		-	
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "townregion.h"
#include "network.h"
#include "ObjectFactory.h"

namespace UOX
{

//o---------------------------------------------------------------------------o
//|	Function	-	CChar *calcCharObjFromSer( SERIAL targSerial )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the character object based on the calling serial
//o---------------------------------------------------------------------------o
CChar *calcCharObjFromSer( SERIAL targSerial )
{
	CBaseObject *findItem = ObjectFactory::getSingleton().FindObject( targSerial );
	CChar *toRet = NULL;
	if( findItem != NULL )
	{
		if( findItem->CanBeObjType( OT_CHAR ) )
			toRet = static_cast<CChar *>(findItem);
	}
	return toRet;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *calcItemObjFromSer( SERIAL targSerial )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the item object based on the calling serial
//o---------------------------------------------------------------------------o
CItem *calcItemObjFromSer( SERIAL targSerial )
{
	CBaseObject *findItem = ObjectFactory::getSingleton().FindObject( targSerial );
	CItem *toRet = NULL;
	if( findItem != NULL )
	{
		if( findItem->CanBeObjType( OT_ITEM ) )
			toRet = static_cast<CItem *>(findItem);
	}
	return toRet;
}

CMultiObj *calcMultiFromSer( SERIAL targSerial )
{
	CBaseObject *findMulti = ObjectFactory::getSingleton().FindObject( targSerial );
	CMultiObj *toRet = NULL;
	if( findMulti != NULL )
	{
		if( findMulti->CanBeObjType( OT_MULTI ) )
			toRet = static_cast<CMultiObj *>(findMulti);
	}
	return toRet;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber )
//|	Date			-	Unknown
//|	Programmer		-	UOX3 DevTeam
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Find what region x and y are in
//o--------------------------------------------------------------------------
CTownRegion *calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber )
{
	const regLocs *getLoc	= NULL;
	TOWNMAP_CITERATOR tIter	= cwmWorldState->townRegions.begin();
	TOWNMAP_CITERATOR tEnd	= cwmWorldState->townRegions.end();
	while( tIter != tEnd )
	{
		CTownRegion *myReg = tIter->second;
		if( myReg != NULL && myReg->WorldNumber() == worldNumber )
		{
			for( size_t j = 0; j < myReg->GetNumLocations(); ++j )
			{
				getLoc = myReg->GetLocation( j );
				if( getLoc != NULL )
				{
					if( getLoc->x1 <= x && getLoc->y1 <= y && getLoc->x2 >= x && getLoc->y2 >= y )
						return myReg;
				}
			}
		}
		++tIter;
	}
	return cwmWorldState->townRegions[0xFF];
}

}

