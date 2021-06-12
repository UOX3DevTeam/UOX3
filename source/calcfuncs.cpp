//o-----------------------------------------------------------------------------------------------o
//|	File		-	calcfuncs.cpp
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Various functions to calculate objects from serials
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "townregion.h"
#include "network.h"
#include "ObjectFactory.h"


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CChar *calcCharObjFromSer( SERIAL targSerial )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the character object based on the calling serial
//o-----------------------------------------------------------------------------------------------o
CChar *calcCharObjFromSer( SERIAL targSerial )
{
	CBaseObject *findItem = ObjectFactory::getSingleton().FindObject( targSerial );
	CChar *toRet = nullptr;
	if( findItem != nullptr )
	{
		if( findItem->CanBeObjType( OT_CHAR ) )
			toRet = static_cast<CChar *>(findItem);
	}
	return toRet;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *calcItemObjFromSer( SERIAL targSerial )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the item object based on the calling serial
//o-----------------------------------------------------------------------------------------------o
CItem *calcItemObjFromSer( SERIAL targSerial )
{
	CBaseObject *findItem = ObjectFactory::getSingleton().FindObject( targSerial );
	CItem *toRet = nullptr;
	if( findItem != nullptr )
	{
		if( findItem->CanBeObjType( OT_ITEM ) )
			toRet = static_cast<CItem *>(findItem);
	}
	return toRet;
}

CMultiObj *calcMultiFromSer( SERIAL targSerial )
{
	CBaseObject *findMulti = ObjectFactory::getSingleton().FindObject( targSerial );
	CMultiObj *toRet = nullptr;
	if( findMulti != nullptr )
	{
		if( findMulti->CanBeObjType( OT_MULTI ) )
			toRet = static_cast<CMultiObj *>(findMulti);
	}
	return toRet;
}

//o--------------------------------------------------------------------------
//|	Function	-	CTownRegion *calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber )
//o--------------------------------------------------------------------------
//|	Purpose		-	Find what region x and y are in
//o--------------------------------------------------------------------------
CTownRegion *calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID )
{
	const regLocs *getLoc	= nullptr;
	TOWNMAP_CITERATOR tIter	= cwmWorldState->townRegions.begin();
	TOWNMAP_CITERATOR tEnd	= cwmWorldState->townRegions.end();
	while( tIter != tEnd )
	{
		CTownRegion *myReg = tIter->second;
		if( myReg != nullptr && myReg->WorldNumber() == worldNumber && myReg->GetInstanceID() == instanceID )
		{
			for( size_t j = 0; j < myReg->GetNumLocations(); ++j )
			{
				getLoc = myReg->GetLocation( j );
				if( getLoc != nullptr )
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

