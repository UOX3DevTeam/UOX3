#include "uox3.h"

#undef DBGFILE
#define DBGFILE "trade.cpp"

CItem *startTrade( cSocket *mSock, CChar *i )
{
	char msg[90];

	CChar *mChar = mSock->CurrcharObj();

	CItem *bps = getPack( mChar );
	CItem *bpi = getPack( i );
	cSocket *nSock = calcSocketObjFromChar( i );
	
	if( bps == NULL )
	{
		sysmessage( mSock, 773 );
		sysmessage( nSock, 1357, mChar->GetName() );
		return NULL;
	}
	
	if( bpi == NULL )
	{
		sysmessage( nSock, 773 );
		sysmessage( mSock, 1357, i->GetName() );
		return NULL;
	}

	CItem *ps = Items->SpawnItem( NULL, i, 1, "#", false, 0x1E5E, 0, false, false );
	if( ps == NULL ) 
		return NULL;
	ps->SetX( 26 );
	ps->SetY( 0 );
	ps->SetZ( 0 );
	ps->SetLayer( 0 );
	if( !ps->SetCont( mChar ) )
		return NULL;
	ps->SetType( 1 );
	ps->SetDye( false );
	sendPackItem( mSock, ps );
	if( nSock != NULL ) 
		sendPackItem( nSock, ps );
	CItem *pi = Items->SpawnItem( NULL, i, 1, "#", false, 0x1E5E, 0, false, false );
	if( pi == NULL ) 
		return NULL;
	pi->SetX( 26 );
	pi->SetY( 0 );
	pi->SetZ( 0 );
	pi->SetLayer( 0 );
	if( !pi->SetCont( i ) )
		return NULL;
	pi->SetType( 1 );
	pi->SetDye( false );
	sendPackItem( mSock, pi );
	if( nSock != NULL ) 
		sendPackItem( nSock, pi );
	
	pi->SetMoreB( ps->GetSerial() );
	ps->SetMoreB( pi->GetSerial() );
	pi->SetMoreZ( 0 );
	ps->SetMoreZ( 0 );
	
	msg[0] = 0x6F; // Header Byte
	msg[1] = 0; // Size
	msg[2] = 47; // Size
	msg[3] = 0; // Initiate
	msg[4] = i->GetSerial( 1 );
	msg[5] = i->GetSerial( 2 );
	msg[6] = i->GetSerial( 3 );
	msg[7] = i->GetSerial( 4 );
	msg[8] = ps->GetSerial( 1 );
	msg[9] = ps->GetSerial( 2 );
	msg[10] = ps->GetSerial( 3 );
	msg[11] = ps->GetSerial( 4 );
	msg[12] = pi->GetSerial( 1 );
	msg[13] = pi->GetSerial( 2 );
	msg[14] = pi->GetSerial( 3 );
	msg[15] = pi->GetSerial( 4 );
	msg[16] = 1;
	strcpy(&(msg[17]), i->GetName());
	mSock->Send( msg, 47 );
	
	msg[0] = 0x6F; // Header Byte
	msg[1] = 0; // Size
	msg[2] = 47; // Size
	msg[3] = 0; // Initiate
	msg[4] = mChar->GetSerial( 1 );
	msg[5] = mChar->GetSerial( 2 );
	msg[6] = mChar->GetSerial( 3 );
	msg[7] = mChar->GetSerial( 4 );
	msg[8] = pi->GetSerial( 1 );
	msg[9] = pi->GetSerial( 2 );
	msg[10] = pi->GetSerial( 3 );
	msg[11] = pi->GetSerial( 4 );
	msg[12] = ps->GetSerial( 1 );
	msg[13] = ps->GetSerial( 2 );
	msg[14] = ps->GetSerial( 3 );
	msg[15] = ps->GetSerial( 4 );
	msg[16] = 1;
	strcpy(&(msg[17]), mChar->GetName());
	if( nSock != NULL ) 
		nSock->Send( msg, 47 );
	
	return ps;
}

void clearTrades( void )
{
	for( ITEM i = 0; i < cwmWorldState->GetItemCount(); i++ )
	{
		if( items[i].GetType() == 1 && items[i].GetX() == 26 && items[i].GetY() == 0 && items[i].GetZ() == 0 && items[i].GetID() == 0x1E5E )
		{
			CChar *k = (CChar *)items[i].GetCont();
			CItem *p = getPack( k );
			if( p != NULL )	// can we move this check to outside the for loop?? I should think so!
			{
				for( CItem *j = items[i].FirstItemObj(); !items[i].FinishedItems(); j = items[i].NextItemObj() )
				{
					if( j != NULL )
						j->SetCont( p );
				}
			}
			Items->DeleItem( &items[i] );
		}
	}
}

void doTradeMsg( cSocket *mSock )
{
	CItem *cont1 = NULL, *cont2 = NULL;
	switch( mSock->GetByte( 3 ) )
	{
	case 0://Start trade - Never happens, sent out by the server only.
		break;
	case 2://Change check marks.  Possibly conclude trade
		cont1 = calcItemObjFromSer( mSock->GetDWord( 4 ) );
		if( cont1 != NULL )
			cont2 = calcItemObjFromSer( cont1->GetMoreB() );
		if( cont2 != NULL )
		{
			cont1->SetMoreZ( mSock->GetByte( 11 ) );
			sendTradeStatus( cont1, cont2 );
			if( cont1->GetMoreZ() && cont2->GetMoreZ() )
			{
				doTrade( cont1, cont2 );
				endTrade( mSock->GetDWord( 4 ) );
			}
		}
		break;
	case 1://Cancel trade.  Send each person cancel messages, move items.
		endTrade( mSock->GetDWord( 4 ) );
		break;
	default:
		Console.Error( 2, " Fallout of switch statement without default. trade.cpp, trademsg()" );
	}
}

void sendTradeStatus( CItem *cont1, CItem *cont2 )
{
	char msg[30];
	
	CChar *p1 = (CChar *)cont1->GetCont();
	CChar *p2 = (CChar *)cont2->GetCont();
	cSocket *s1 = calcSocketObjFromChar( p1 );
	cSocket *s2 = calcSocketObjFromChar( p2 );
	
	msg[0] = 0x6F;//Header
	msg[1] = 0x00;//Size
	msg[2] = 0x11;//Size
	msg[3] = 0x02;//State
	msg[4] = cont1->GetSerial( 1 );
	msg[5] = cont1->GetSerial( 2 );
	msg[6] = cont1->GetSerial( 3 );
	msg[7] = cont1->GetSerial( 4 );
	msg[8] = 0;
	msg[9] = 0;
	msg[10] = 0;
	msg[11] = (UI08)(cont1->GetMoreZ()%256);
	msg[12] = 0;
	msg[13] = 0;
	msg[14] = 0;
	msg[15] = (UI08)(cont2->GetMoreZ()%256);
	msg[16] = 0; // No name in this message
	if( s1 != NULL ) 
		s1->Send( msg, 17 );
	
	msg[4] = cont2->GetSerial( 1 );
	msg[5] = cont2->GetSerial( 2 );
	msg[6] = cont2->GetSerial( 3 );
	msg[7] = cont2->GetSerial( 4 );
	msg[11] = (UI08)(cont2->GetMoreZ()%256);
	msg[15] = (UI08)(cont1->GetMoreZ()%256);
	if( s2 != NULL ) 
		s2->Send( msg, 17 );
}

void endTrade( SERIAL targSerial )
{
	char msg[30];
	
	CItem *cont1 = calcItemObjFromSer( targSerial );
	if( cont1 == NULL )
		return;

	CItem *cont2 = calcItemObjFromSer( cont1->GetMoreB() );
	if( cont2 == NULL )
		return;

	CChar *p1 = (CChar *)cont1->GetCont();
	CChar *p2 = (CChar *)cont2->GetCont();

	CItem *bp1 = getPack( p1 );
	if( bp1 == NULL ) 
		return;

	CItem *bp2 = getPack( p2 );
	if( bp2 == NULL ) 
		return;

	cSocket *mSock = calcSocketObjFromChar( p1 );
	cSocket *nSock = calcSocketObjFromChar( p2 );
	
	msg[0] = 0x6F;//Header Byte
	msg[1] = 0x00;//Size
	msg[2] = 0x11;//Size
	msg[3] = 0x01;//State byte
	msg[4] = cont1->GetSerial( 1 );
	msg[5] = cont1->GetSerial( 2 );
	msg[6] = cont1->GetSerial( 3 );
	msg[7] = cont1->GetSerial( 4 );
	msg[8] = 0;
	msg[9] = 0;
	msg[10] = 0;
	msg[11] = 0;
	msg[12] = 0;
	msg[13] = 0;
	msg[14] = 0;
	msg[15] = 0;
	msg[16] = 0;
	if( mSock != NULL ) 
		mSock->Send( msg, 17 );
	
	msg[0] = 0x6F;//Header Byte
	msg[1] = 0x00;//Size
	msg[2] = 0x11;//Size
	msg[3] = 0x01;//State byte
	msg[4] = cont2->GetSerial( 1 );
	msg[5] = cont2->GetSerial( 2 );
	msg[6] = cont2->GetSerial( 3 );
	msg[7] = cont2->GetSerial( 4 );
	msg[8] = 0;
	msg[9] = 0;
	msg[10] = 0;
	msg[11] = 0;
	msg[12] = 0;
	msg[13] = 0;
	msg[14] = 0;
	msg[15] = 0;
	msg[16] = 0;
	if( nSock != NULL ) 
		nSock->Send( msg, 17 );
	
	CItem *i = NULL, *j = NULL;
	for( i = cont1->FirstItemObj(); !cont1->FinishedItems(); i = cont1->NextItemObj() )
	{
		if( i != NULL )
		{
			i->SetCont( bp1 );
			i->SetX( 50 + ( RandomNum( 0, 79 ) ) );
			i->SetY( 50 + ( RandomNum( 0, 79 ) ) );
			i->SetZ( 9 );
			if( i->GetGlow() > 0 )
			{
				j = calcItemObjFromSer( i->GetGlow() );
				if( j != NULL )
				{
					j->SetCont( bp1 );
					j->SetX( i->GetX() );
					j->SetY( i->GetY() );
					j->SetZ( i->GetZ() );
					RefreshItem( j );
				}
			}
			if( mSock != NULL ) 
				RefreshItem( i );
		}
	}
	for( i = cont2->FirstItemObj(); !cont2->FinishedItems(); i = cont2->NextItemObj() )
	{
		if( i != NULL )
		{
			i->SetCont( bp2 );  
			i->SetX( 50 + ( RandomNum( 0, 79 ) ) );
			i->SetY( 50 + ( RandomNum( 0, 79 ) ) );
			i->SetZ( 9 );
			if( i->GetGlow() > 0 )
			{
				j = calcItemObjFromSer( i->GetGlow() );
				if( j != NULL )
				{
					j->SetCont( bp2 );
					j->SetX( i->GetX() );
					j->SetY( i->GetY() );
					j->SetZ( i->GetZ() );
					RefreshItem( j );
				}
			}
			if( nSock != NULL ) 
				RefreshItem( i );
		}
	}
	Items->DeleItem( cont1 );
	Items->DeleItem( cont2 );
}

void doTrade( CItem *cont1, CItem *cont2 )
{
	CChar *p1 = (CChar *)cont1->GetCont();
	if( p1 == NULL ) 
		return;

	CChar *p2 = (CChar *)cont2->GetCont();
	if( p2 == NULL ) 
		return;

	CItem *bp1 = getPack( p1 );
	if( bp1 == NULL ) 
		return;

	CItem *bp2 = getPack( p2 );
	if( bp2 == NULL ) 
		return;

	cSocket *mSock = calcSocketObjFromChar( p1 );
	cSocket *nSock = calcSocketObjFromChar( p2 );
	
	CItem *i = NULL, *j = NULL;
	for( i = cont1->FirstItemObj(); !cont1->FinishedItems(); i = cont1->NextItemObj() )
	{
		if( i != NULL )
		{
			i->SetCont( bp2 );
			i->SetX( 50 + ( RandomNum( 0, 79 ) ) );
			i->SetY( 50 + ( RandomNum( 0, 79 ) ) );
			i->SetZ( 9 );
			if( i->GetGlow() > 0 )
			{
				j = calcItemObjFromSer( i->GetGlow() );
				if( j != NULL )
				{
					j->SetCont( bp2 );
					j->SetX( i->GetX() );
					j->SetY( i->GetY() );
					j->SetZ( i->GetZ() );
					RefreshItem( j );
				}
			}

			if( mSock != NULL || nSock != NULL ) 
				RefreshItem( i );
		}
	}
	for( i = cont2->FirstItemObj(); !cont2->FinishedItems(); i = cont2->NextItemObj() )
	{
		if( i != NULL )
		{
			i->SetCont( bp1 );
			i->SetX( 50 + ( RandomNum( 0, 79 ) ) );
			i->SetY( 50 + ( RandomNum( 0, 79 ) ) );
			i->SetZ( 9 );
			if( i->GetGlow() > 0 )
			{
				j = calcItemObjFromSer( i->GetGlow() );
				if( j != NULL )
				{
					j->SetCont( bp1 );
					j->SetX( i->GetX() );
					j->SetY( i->GetY() );
					j->SetZ( i->GetZ() );
					RefreshItem( j );
				}
			}
			if( mSock != NULL || nSock != NULL )
				RefreshItem( i );
		}
	}
}

void killTrades( CChar *i )
{
	for( CItem *j = i->FirstItem(); !i->FinishedItems(); j = i->NextItem() )
	{
		if( j != NULL )
		{
			if( j->GetType() == 1 && j->GetX() == 26 && j->GetY() == 0 && j->GetZ() == 0 && j->GetID() == 0x1E5E )
				endTrade( j->GetSerial() );
		}
	}
}
