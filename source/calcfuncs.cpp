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
//|	Function	-	CTownRegion *calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, CBaseObject *mObj )
//o--------------------------------------------------------------------------
//|	Purpose		-	Find what region x and y are in. This should be done for all items on world load, and when any of the
//|					item's location and/or container properties change
//o--------------------------------------------------------------------------
CTownRegion *calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, CBaseObject *mObj )
{
	// Reset sub region of character
	if( ValidateObject( mObj ) )
	{
		mObj->SetSubRegion( 0 );

		// Check if object is an item, and if it's in a container
		if( mObj->GetObjType() == OT_ITEM && static_cast<CItem *>(mObj)->GetContSerial() != INVALIDSERIAL )
		{
			// Item is in a container, so find the root container (whether character or item) and use its coordinates to calculate region
			CBaseObject *rootContainer = FindItemOwner( static_cast<CItem *>(mObj) );
			if( ValidateObject( rootContainer ))
			{
				x = rootContainer->GetX();
				y = rootContainer->GetY();
				worldNumber = rootContainer->WorldNumber();
				instanceID = rootContainer->GetInstanceID();
			}
		}
	}

	const regLocs *getLoc	= nullptr;
	for (auto [twnnum,myReg]:cwmWorldState->townRegions){
		if( myReg != nullptr && myReg->WorldNumber() == worldNumber && myReg->GetInstanceID() == instanceID )
		{
			for( size_t j = 0; j < myReg->GetNumLocations(); ++j )
			{
				getLoc = myReg->GetLocation( j );
				if( getLoc != nullptr )
				{
					// Calculate the region/subregion of the object
					if( getLoc->x1 <= x && getLoc->y1 <= y && getLoc->x2 >= x && getLoc->y2 >= y )
					{
						// If object is in a subregion, store a reference to it
						if( myReg->IsSubRegion() && ValidateObject( mObj ))
						{
							mObj->SetSubRegion( myReg->GetRegionNum() );
							continue;
						}
						return myReg;
					}
				}
			}
		}
	}
	return cwmWorldState->townRegions[0xFF];
}


