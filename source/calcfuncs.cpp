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

//o---------------------------------------------------------------------------o
//|	Function	-	UOXSOCKET calcSocketFromChar( CHARACTER i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the socket based on the calling character
//o---------------------------------------------------------------------------o
UOXSOCKET calcSocketFromChar( CHARACTER i )
{
	if( i == INVALIDSERIAL || i >= cmem )
		return -1;
	if( chars[i].IsNpc() )
		return -1;
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock->Currchar() == i )
		{
			Network->PopConn();
			return calcSocketFromSockObj( tSock );
		}
	}
	Network->PopConn();
	return -1;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cSocket *calcSocketObjFromChar( CHARACTER i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the socket object based on the calling character
//o---------------------------------------------------------------------------o
cSocket *calcSocketObjFromChar( CHARACTER i )
{
	if( i == INVALIDSERIAL || i >= cmem )
		return NULL;
	return calcSocketObjFromChar( &chars[i] );
}

//o---------------------------------------------------------------------------o
//|	Function	-	cSocket *calcSocketObjFromChar( CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the socket object based on the calling character
//o---------------------------------------------------------------------------o
cSocket *calcSocketObjFromChar( CChar *i )
{
	if( i == NULL )
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
//|	Function	-	UOXSOCKET calcSocketFromChar( CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the socket based on the calling character
//o---------------------------------------------------------------------------o
UOXSOCKET calcSocketFromChar( CChar *i )
{
	if( i == NULL || i->IsNpc() ) 
		return -1;

	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock->CurrcharObj() == i )
		{
			Network->PopConn();
			return calcSocketFromSockObj( tSock );
		}
	}
	Network->PopConn();
	return -1;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CHARACTER calcCharFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the character based on the calling serial
//o---------------------------------------------------------------------------o
CHARACTER calcCharFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 )
{
	SERIAL serial = calcserial( ser1, ser2, ser3, ser4 );
	return calcCharFromSer( serial );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CHARACTER calcCharFromSer( SERIAL serial )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the character based on the calling serial
//o---------------------------------------------------------------------------o
CHARACTER calcCharFromSer( SERIAL serial )
{
	if( serial == INVALIDSERIAL )
		return INVALIDSERIAL;
	return ncharsp.FindBySerial( serial );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CChar *calcCharObjFromSer( SERIAL targSerial )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the character object based on the calling serial
//o---------------------------------------------------------------------------o
CChar *calcCharObjFromSer( SERIAL targSerial )
{
	if( targSerial == INVALIDSERIAL )
		return NULL;
	CHARACTER targChar = calcCharFromSer( targSerial );
	if( targChar != INVALIDSERIAL )
		return &chars[targChar];
	else
		return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	ITEM calcItemFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the item based on the calling serial
//o---------------------------------------------------------------------------o
ITEM calcItemFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 )
{
	SERIAL serial = calcserial( ser1, ser2, ser3, ser4 );
	return calcItemFromSer( serial );
}

//o---------------------------------------------------------------------------o
//|	Function	-	ITEM calcItemFromSer( SERIAL ser )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the item based on the calling serial
//o---------------------------------------------------------------------------o
ITEM calcItemFromSer( SERIAL ser )
{
	if( ser == INVALIDSERIAL )
		return INVALIDSERIAL;
	return nitemsp.FindBySerial( ser );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *calcItemObjFromSer( SERIAL targSerial )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the item object based on the calling serial
//o---------------------------------------------------------------------------o
CItem *calcItemObjFromSer( SERIAL targSerial )
{
	if( targSerial == INVALIDSERIAL )
		return NULL;
	ITEM targItem = calcItemFromSer( targSerial );
	if( targItem != INVALIDSERIAL )
		return &items[targItem];
	else
		return NULL;
}

//o--------------------------------------------------------------------------
//|	Function		-	cSocket *calcSocketObjFromSock( UOXSOCKET s )
//|	Date			-	05 July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a cSocket * from the socket handle
//o--------------------------------------------------------------------------
cSocket *calcSocketObjFromSock( UOXSOCKET s )
{
	return Network->GetSockPtr( s );
}

//o--------------------------------------------------------------------------
//|	Function		-	UOXSOCKET calcSocketFromSockObj( cSocket *s )
//|	Date			-	05 July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a socket handle from the socket pointer
//o--------------------------------------------------------------------------
UOXSOCKET calcSocketFromSockObj( cSocket *s )
{
	if( s == NULL )
		return -1;
	return Network->FindNetworkPtr( s );
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber )
//|	Date			-	Unknown
//|	Programmer		-	Unknown
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Find what region x and y are in
//o--------------------------------------------------------------------------
UI08 calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber )
{
	for( int i = 0; i < locationcount; i++ )
	{
		if( location[i].x1 <= x && location[i].y1 <= y && location[i].x2 >= x && location[i].y2 >= y )
		{
			if( region[location[i].region]->WorldNumber() == worldNumber )
				return location[i].region;
		}
	}
	return 0xFF;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 calcValue( CItem *i, UI32 value )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the value of an item
//o---------------------------------------------------------------------------o
UI32 calcValue( CItem *i, UI32 value)
{
	UI32 mod = 10;
	
	if( i->GetType() == 19 )
	{
		if( i->GetMoreX() > 500 )
			mod++;
		if( i->GetMoreX() > 900 )
			mod++;
		if( i->GetMoreX() > 1000 )
			mod++;
		if( i->GetMoreZ() > 1 ) 
			mod += (3*(i->GetMoreZ()-1));
		value=(value*mod)/10;
	}
	
	if( i->GetRank() > 0 && i->GetRank() < 10 && cwmWorldState->ServerData()->GetRankSystemStatus() )
		value = (UI32)( i->GetRank() * value ) / 10;
	if( value < 1 ) 
		value = 1;
	
	if( !i->GetRndValueRate() ) 
		i->SetRndValueRate( 0 );
	if( i->GetRndValueRate() != 0 && cwmWorldState->ServerData()->GetTradeSystemStatus() )
		value += (UI32)(value*i->GetRndValueRate())/1000;
	if( value < 1 ) 
		value = 1;
	return value;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 calcGoodValue( CChar *npcnum2, CItem *i, UI32 value, bool isSelling )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the value of a good
//o---------------------------------------------------------------------------o
UI32 calcGoodValue( CChar *npcnum2, CItem *i, UI32 value, bool isSelling)
{
	UI08 actreg = calcRegionFromXY( npcnum2->GetX(), npcnum2->GetY(), npcnum2->WorldNumber() );
	SI32 good = i->GetGood();
	SI32 regvalue = 0;
	
	if( good <= -1 )
		return value;
	
	if(isSelling)
		regvalue = region[actreg]->GetGoodSell( static_cast<UI08>(i->GetGood()) );
	else
		regvalue = region[actreg]->GetGoodBuy( static_cast<UI08>(i->GetGood()) );
	
	UI32 x = (UI32)( value * abs( regvalue ) ) / 1000;
	
	if( regvalue < 0 )
		value -= x;
	else
		value += x;
	
	if( value <= 0 )
		value = 1;
	
	return value;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 calcGoldInPack( CItem *item )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate total gold in a pack
//o---------------------------------------------------------------------------o
UI32 calcGoldInPack( CItem *item )
{
	UI32 total = 0;
	
	for( CItem *i = item->FirstItemObj(); !item->FinishedItems(); i = item->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == 0x0EED )
				total += i->GetAmount();
			if( i->IsContType() )
				total += calcGoldInPack( i );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 calcGold( CChar *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate total gold on a character
//o---------------------------------------------------------------------------o
UI32 calcGold( CChar *p )
{
	CItem *i = getPack( p );
	if( i != NULL )
		return calcGoldInPack( i );
	return 0;
}
