#include "uox3.h"
#include "classes.h"
#include "CPacketSend.h"

#undef DBGFILE
#define DBGFILE "trade.cpp"

#include "ObjectFactory.h"

namespace UOX
{

CItem *startTrade( CSocket *mSock, CChar *i )
{
	if( mSock == NULL || !ValidateObject( i ) )
		return NULL;

	CChar *mChar	= mSock->CurrcharObj();
	CItem *bps		= mChar->GetPackItem();
	CItem *bpi		= i->GetPackItem();
	CSocket *nSock	= calcSocketObjFromChar( i );

	if( nSock == NULL )
		return NULL;

	if( !ValidateObject( bps ) )
	{
		mSock->sysmessage( 773 );
		nSock->sysmessage( 1357, mChar->GetName().c_str() );
		return NULL;
	}
	
	if( !ValidateObject( bpi ) )
	{
		nSock->sysmessage( 773 );
		mSock->sysmessage( 1357, i->GetName().c_str() );
		return NULL;
	}

	CItem *ps = Items->CreateItem( NULL, i, 0x1E5E, 1, 0, OT_ITEM );
	if( ps == NULL || !ps->SetCont( mChar ) ) 
		return NULL;
	ps->SetX( 26 );
	ps->SetY( 0 );
	ps->SetZ( 0 );
	ps->SetLayer( IL_NONE );
	ps->SetType( IT_CONTAINER );
	ps->SetDye( false );
	ps->SendPackItemToSocket( mSock );
	ps->SendPackItemToSocket( nSock );
	CItem *pi = Items->CreateItem( NULL, i, 0x1E5E, 1, 0, OT_ITEM );
	if( pi == NULL || !pi->SetCont( i ) ) 
		return NULL;
	pi->SetX( 26 );
	pi->SetY( 0 );
	pi->SetZ( 0 );
	pi->SetLayer( IL_NONE );
	pi->SetType( IT_CONTAINER );
	pi->SetDye( false );
	pi->SendPackItemToSocket( mSock );
	pi->SendPackItemToSocket( nSock );
	
	pi->SetTempVar( CITV_MOREX, ps->GetSerial() );
	ps->SetTempVar( CITV_MOREX, pi->GetSerial() );
	pi->SetTempVar( CITV_MOREZ, 0 );
	ps->SetTempVar( CITV_MOREZ, 0 );
	
	CPSecureTrading cpstOne( (*i), (*ps), (*pi) );
	cpstOne.Name( i->GetName() );
	mSock->Send( &cpstOne );
	
	CPSecureTrading cpstTwo( (*mChar), (*pi), (*ps) );
	cpstTwo.Name( mChar->GetName() );
	nSock->Send( &cpstTwo );

	return ps;
}

bool clearTradesFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) && a->CanBeObjType( OT_ITEM ) )
	{
		// Body of the functor goes here
		CItem *i = static_cast< CItem * >(a);
		if( i->GetType() == IT_CONTAINER && i->GetX() == 26 && i->GetY() == 0 && i->GetZ() == 0 && i->GetID() == 0x1E5E )
		{
			CChar *k = (CChar *)i->GetCont();
			CItem *p = k->GetPackItem();
			if( ValidateObject( p ) )	// can we move this check to outside the for loop?? I should think so!
			{
				CDataList< CItem * > *iCont = i->GetContainsList();
				for( CItem *j = iCont->First(); !iCont->Finished(); j = iCont->Next() )
				{
					if( ValidateObject( j ) )
						j->SetCont( p );
				}
			}
			i->Delete();
			++b;	// let's track how many we cleared
		}
	}
	return retVal;
}

void clearTrades( void )
{
	UI32 b = 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &clearTradesFunctor );
}

void endTrade( SERIAL targSerial )
{
	CItem *cont1 = calcItemObjFromSer( targSerial );
	if( !ValidateObject( cont1 ) )
		return;

	CItem *cont2 = calcItemObjFromSer( cont1->GetTempVar( CITV_MOREX ) );
	if( !ValidateObject( cont2 ) )
		return;

	CChar *p1 = (CChar *)cont1->GetCont();
	CChar *p2 = (CChar *)cont2->GetCont();

	CItem *bp1 = p1->GetPackItem();
	if( !ValidateObject( bp1 ) ) 
		return;

	CItem *bp2 = p2->GetPackItem();
	if( !ValidateObject( bp2 ) ) 
		return;

	CSocket *mSock = calcSocketObjFromChar( p1 );
	CSocket *nSock = calcSocketObjFromChar( p2 );
	
	if( mSock != NULL ) 
	{
		CPSecureTrading cpstOne( (*cont1), 0, 0 );
		cpstOne.Action( 1 );
		mSock->Send( &cpstOne );
	}
	
	if( nSock != NULL ) 
	{
		CPSecureTrading cpstTwo( (*cont2), 0, 0 );
		cpstTwo.Action( 1 );
		nSock->Send( &cpstTwo );
	}
	CItem *i = NULL, *j = NULL;
	CDataList< CItem * > *c1Cont = cont1->GetContainsList();
	for( i = c1Cont->First(); !c1Cont->Finished(); i = c1Cont->Next() )
	{
		if( ValidateObject( i ) )
		{
			i->SetCont( bp1 );
			i->PlaceInPack();
			if( i->GetGlow() != INVALIDSERIAL )
			{
				j = calcItemObjFromSer( i->GetGlow() );
				if( ValidateObject( j ) )
				{
					j->SetCont( bp1 );
					j->SetX( i->GetX() );
					j->SetY( i->GetY() );
					j->SetZ( i->GetZ() );
				}
			}
		}
	}
	CDataList< CItem * > *c2Cont = cont2->GetContainsList();
	for( i = c2Cont->First(); !c2Cont->Finished(); i = c2Cont->Next() )
	{
		if( ValidateObject( i ) )
		{
			i->SetCont( bp2 );  
			i->PlaceInPack();
			if( i->GetGlow() != INVALIDSERIAL )
			{
				j = calcItemObjFromSer( i->GetGlow() );
				if( ValidateObject( j ) )
				{
					j->SetCont( bp2 );
					j->SetX( i->GetX() );
					j->SetY( i->GetY() );
					j->SetZ( i->GetZ() );
				}
			}
		}
	}
	cont1->Delete();
	cont2->Delete();
}

void doTrade( CItem *cont1, CItem *cont2 )
{
	CChar *p1 = (CChar *)cont1->GetCont();
	if( !ValidateObject( p1 ) ) 
		return;

	CChar *p2 = (CChar *)cont2->GetCont();
	if( !ValidateObject( p2 ) ) 
		return;

	CItem *bp1 = p1->GetPackItem();
	if( !ValidateObject( bp1 ) ) 
		return;

	CItem *bp2 = p2->GetPackItem();
	if( !ValidateObject( bp2 ) ) 
		return;
	
	CItem *i = NULL, *j = NULL;
	CDataList< CItem * > *c1Cont = cont1->GetContainsList();
	for( i = c1Cont->First(); !c1Cont->Finished(); i = c1Cont->Next() )
	{
		if( ValidateObject( i ) )
		{
			i->SetCont( bp2 );
			i->PlaceInPack();
			if( i->GetGlow() != INVALIDSERIAL )
			{
				j = calcItemObjFromSer( i->GetGlow() );
				if( ValidateObject( j ) )
				{
					j->SetCont( bp2 );
					j->SetX( i->GetX() );
					j->SetY( i->GetY() );
					j->SetZ( i->GetZ() );
				}
			}

		}
	}
	CDataList< CItem * > *c2Cont = cont2->GetContainsList();
	for( i = c2Cont->First(); !c2Cont->Finished(); i = c2Cont->Next() )
	{
		if( ValidateObject( i ) )
		{
			i->SetCont( bp1 );
			i->PlaceInPack();
			if( i->GetGlow() != INVALIDSERIAL )
			{
				j = calcItemObjFromSer( i->GetGlow() );
				if( ValidateObject( j ) )
				{
					j->SetCont( bp1 );
					j->SetX( i->GetX() );
					j->SetY( i->GetY() );
					j->SetZ( i->GetZ() );
				}
			}
		}
	}
}

void sendTradeStatus( CItem *cont1, CItem *cont2 )
{
	CChar *p1 = (CChar *)cont1->GetCont();
	CChar *p2 = (CChar *)cont2->GetCont();
	CSocket *s1 = calcSocketObjFromChar( p1 );
	CSocket *s2 = calcSocketObjFromChar( p2 );

	if( s1 != NULL )
	{
		CPSecureTrading cpstOne( (*cont1), (cont1->GetTempVar( CITV_MOREZ ) % 256), (cont2->GetTempVar( CITV_MOREZ ) % 256) );
		cpstOne.Action( 2 );
		s1->Send( &cpstOne );
	}

	if( s2 != NULL ) 
	{
		CPSecureTrading cpstTwo( (*cont2), (cont2->GetTempVar( CITV_MOREZ ) % 256), (cont1->GetTempVar( CITV_MOREZ ) % 256) );
		cpstTwo.Action( 2 );
		s2->Send( &cpstTwo );
	}
}

bool CPITradeMessage::Handle( void )
{
	CItem *cont1 = NULL, *cont2 = NULL;
	switch( tSock->GetByte( 3 ) )
	{
		case 0://Start trade - Never happens, sent out by the server only.
			break;
		case 2://Change check marks.  Possibly conclude trade
			cont1 = calcItemObjFromSer( tSock->GetDWord( 4 ) );
			if( ValidateObject( cont1 ) )
				cont2 = calcItemObjFromSer( cont1->GetTempVar( CITV_MOREX ) );
			if( ValidateObject( cont2 ) )
			{
				cont1->SetTempVar( CITV_MOREZ, tSock->GetByte( 11 ) );
				sendTradeStatus( cont1, cont2 );
				if( cont1->GetTempVar( CITV_MOREZ ) && cont2->GetTempVar( CITV_MOREZ ) )
				{
					doTrade( cont1, cont2 );
					endTrade( tSock->GetDWord( 4 ) );
				}
			}
			break;
		case 1://Cancel trade.  Send each person cancel messages, move items.
			endTrade( tSock->GetDWord( 4 ) );
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. trade.cpp, trademsg()" );
			break;
	}
	return true;
}

void killTrades( CChar& i )
{
	for( CItem *j = i.FirstItem(); !i.FinishedItems(); j = i.NextItem() )
	{
		if( ValidateObject( j ) )
		{
			if( j->GetType() == IT_CONTAINER && j->GetX() == 26 && j->GetY() == 0 && j->GetZ() == 0 && j->GetID() == 0x1E5E )
				endTrade( j->GetSerial() );
		}
	}
}

}
