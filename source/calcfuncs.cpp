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
//|	Function	-	cSocket *calcSocketObjFromChar( CChar *i )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the socket object based on the calling character
//|					In the future (think after a CPlayer class) perhaps we should
//|					store the socket on the char temporarily to remove the need of
//|					looping through all online sockets to find it.
//o---------------------------------------------------------------------------o
cSocket *calcSocketObjFromChar( CChar *i )
{
	if( !ValidateObject( i ) )
		return NULL;
	if( i->IsNpc() )
		return NULL;
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock->CurrcharObj() == i )
		{
			Network->PopConn();
			return tSock;
		}
	}
	Network->PopConn();
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CChar *calcCharObjFromSer( SERIAL targSerial )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the character object based on the calling serial
//o---------------------------------------------------------------------------o
CChar *calcCharObjFromSer( SERIAL targSerial )
{
	cBaseObject *findItem = ObjectFactory::getSingleton().FindObject( targSerial );
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
	cBaseObject *findItem = ObjectFactory::getSingleton().FindObject( targSerial );
	CItem *toRet = NULL;
	if( findItem != NULL )
	{
		if( findItem->CanBeObjType( OT_ITEM ) )
			toRet = static_cast<CItem *>(findItem);
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
cTownRegion *calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber )
{
	cTownRegion *tReg		= NULL;
	std::vector< cTownRegion * >::iterator i;
	const regLocs *getLoc	= NULL;
	for( i = regions.begin(); i != regions.end(); ++i )
	{
		tReg = (*i);
		if( tReg == NULL || tReg->WorldNumber() != worldNumber )
			continue;

		for( size_t j = 0; j < tReg->GetNumLocations(); ++j )
		{
			getLoc = tReg->GetLocation( j );
			if( getLoc != NULL )
			{
				if( getLoc->x1 <= x && getLoc->y1 <= y && getLoc->x2 >= x && getLoc->y2 >= y )
					return tReg;
			}
		}
	}
	return regions.back();
}

}

