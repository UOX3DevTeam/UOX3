#include "uox3.h"

#undef DBGFILE
#define DBGFILE "vendor.cpp"

//o---------------------------------------------------------------------------o
//|   Function    :  void PlVGetgold( cSocket *mSock, CChar *v )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get gold when player buys an item off a player vendor
//o---------------------------------------------------------------------------o
void PlVGetgold( cSocket *mSock, CChar *v )
{
	CChar *mChar = mSock->CurrcharObj();
	char temp[1024];
	UI32 pay = 0, give = v->GetHoldG(), t = 0;
	if( mChar->GetSerial() == v->GetOwner() )
	{
		if( v->GetHoldG() <= 0 )
		{
			npcTalk( mSock, v, 1326, false );
			v->SetHoldG( 0 );
			return;
		} 
		else if( v->GetHoldG() > 0 && v->GetHoldG() <= MAX_STACK )
		{
			if( v->GetHoldG() > 9 )
			{
				pay = (int)( v->GetHoldG() / 10 );
				give = v->GetHoldG() - pay;
			} 
			else 
			{
				pay = v->GetHoldG();
				give = 0;
			}
			v->SetHoldG( 0 );
		}
		else 
		{
			t = v->GetHoldG() - MAX_STACK;	// yank of 65 grand, then do calculations
			v->SetHoldG( t );
			pay = 6554;
			give = 58981;
			if( t > 0 )
				npcTalk( mSock, v, 1327, false );
		}
		if( give ) 
			Items->SpawnItem( mSock, give, "#", 1, 0x0EED, 0, true, true);
		
		sprintf( temp, Dictionary->GetEntry( 1328 ), v->GetHoldG(), pay, give );
		npcTalk( mSock, v, temp, false );
	} 
	else 
		npcTalk( mSock, v, 1329, false );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void buyItem( cSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player buys an item from a vendor
//o---------------------------------------------------------------------------o
void buyItem( cSocket *mSock )
{
	int i, j;
	int playergoldtotal, goldtotal = 0, itemtotal;
	bool soldout = false, clear = false;
	CChar *mChar = mSock->CurrcharObj();
	CItem *p = getPack( mChar );
	if( p == NULL ) 
		return;

	char temp[1024];

	vector< int > bitems, amount, layer;

	CChar *npc = calcCharObjFromSer( mSock->GetDWord( 3 ) );
	
	itemtotal = (mSock->GetWord( 1 ) - 8) / 7;
	if( itemtotal > 511 ) 
		return;
	bitems.resize( itemtotal );
	amount.resize( itemtotal );
	layer.resize( itemtotal );
	int baseOffset = 0;
	for( i = 0; i < itemtotal; i++ )
	{
		baseOffset	= 7 * i;
		layer[i]	= mSock->GetByte( 8 + baseOffset );
		bitems[i]	= calcItemFromSer( mSock->GetDWord( 9 + baseOffset ) );
		amount[i]	= mSock->GetWord( 13 + baseOffset );
		goldtotal	+= ( amount[i] * ( items[bitems[i]].GetValue() ) );
	}
	bool useBank = (goldtotal >= cwmWorldState->ServerData()->GetBuyThreshold() );
	if( useBank )
		playergoldtotal = getBankCount( mChar, 0x0EED );
	else
		playergoldtotal = getAmount( mChar, 0x0EED );
	if( playergoldtotal >= goldtotal || mChar->IsGM() )
	{
		for( i = 0; i < itemtotal; i++ )
		{
			if( items[bitems[i]].GetAmount() < amount[i] )
				soldout = true;
		}
		if( soldout )
		{
			npcTalk( mSock, npc, 1336, false );
			clear = true;
		}
		else
		{
			if( mChar->IsGM() )
				sprintf( temp, Dictionary->GetEntry( 1337 ), mChar->GetName() );
			else
			{
				if( goldtotal == 1 )
					sprintf( temp, Dictionary->GetEntry( 1338 ), mChar->GetName(), goldtotal );
				else
					sprintf( temp, Dictionary->GetEntry( 1339 ), mChar->GetName(), goldtotal );

				goldsfx( mSock, goldtotal );
			}
			npcTalkAll( npc, temp, false );
			
			clear = true;
			if( !mChar->IsGM() ) 
			{
				if( useBank )
					deleBankItem( mChar, 0x0EED, 0, goldtotal );
				else
					deleQuan( mChar, 0x0EED, goldtotal );
			}
			int biTemp;
			CItem *iMade = NULL;
			for( i = 0; i < itemtotal; i++ )
			{
				biTemp = bitems[i];
				iMade = NULL;
				if( items[biTemp].GetAmount() > amount[i] )
				{
					if( items[biTemp].isPileable() )
					{
						iMade = Commands->DupeItem( mSock, &items[biTemp], amount[i] );
						if( iMade != NULL )
						{
							iMade->SetX( (UI16)( 20 + ( RandomNum( 0, 49 ) ) ) );
							iMade->SetY( (UI16)( 85 + ( RandomNum( 0, 75 ) ) ) );
							iMade->SetCont( p );
							RefreshItem( iMade );
						}
					}
					else
					{
						for( j = 0; j < amount[i]; j++ )
						{
							iMade = Commands->DupeItem( mSock, &items[biTemp], 1 );
							if( iMade != NULL )
							{
								iMade->SetX( (UI16)( 20 + ( RandomNum( 0, 49 ) ) ) );
								iMade->SetY( (UI16)( 85 + ( RandomNum( 0, 75 ) ) ) );
								iMade->SetCont( p );
								RefreshItem( iMade );
							}
						}
					}
					items[biTemp].SetAmount( items[biTemp].GetAmount() - amount[i] );
					items[biTemp].SetRestock( items[biTemp].GetRestock() + amount[i] );
				}
				else
				{
					switch( layer[i] )
					{
					case 0x1A: // Buy Container
						if( items[biTemp].isPileable() )
						{
							iMade = Commands->DupeItem( mSock, &items[biTemp], amount[i] );
							if( iMade != NULL )
							{
								iMade->SetX( (UI16)( 20 + ( RandomNum( 0, 49 ) ) ) );
								iMade->SetY( (UI16)( 85 + ( RandomNum( 0, 75 ) ) ) );
								iMade->SetCont( p );
								RefreshItem( iMade );
							}
						}
						else
						{
							for( j = 0; j < amount[i]; j++ )
							{
								iMade = Commands->DupeItem( mSock, &items[biTemp], 1 );
								if( iMade != NULL )
								{
									iMade->SetX( (UI16)( 20 + ( RandomNum( 0, 49 ) ) ) );
									iMade->SetY( (UI16)( 85 + ( RandomNum( 0, 75 ) ) ) );
									iMade->SetCont( p );
									RefreshItem( iMade );
								}
							}
							iMade = NULL;
						}
						items[biTemp].SetAmount( items[biTemp].GetAmount() - amount[i] );
						items[biTemp].SetRestock( items[biTemp].GetRestock() + amount[i] );
						break;
					case 0x1B: // Bought Container
						if( items[biTemp].isPileable() )
						{
							items[biTemp].SetCont( p->GetSerial() );
							RefreshItem( &items[biTemp] );
						}
						else
						{
							for( j = 0; j < amount[i]-1; j++ )
							{
								iMade = Commands->DupeItem( mSock, &items[biTemp], 1 );
								if( iMade != NULL )
								{
									iMade->SetX( (UI16)( 20 + ( RandomNum( 0, 49 ) ) ) );
									iMade->SetY( (UI16)( 85 + ( RandomNum( 0, 75 ) ) ) );
									iMade->SetCont( p );
									RefreshItem( iMade );
								}
							}

							items[biTemp].SetCont( p->GetSerial() );
							items[biTemp].SetAmount( 1 );
							RefreshItem( &items[biTemp] );
						}
						break;
					default:
						Console.Error( 2, " Fallout of switch statement without default. vendor.cpp, buyItem()" );
					}
				}
				/*if( iMade != NULL )
				{
					iMade->SetX( (UI16)( 20 + ( RandomNum( 0, 49 ) ) ) );
					iMade->SetY( (UI16)( 85 + ( RandomNum( 0, 75 ) ) ) );
					iMade->SetCont( p );
					RefreshItem( iMade );
				}*/
			}
		}
	}
	else
		npcTalkAll( npc, 1340, false );
	
	if( clear )
	{
		CPClearMsg clrSend;
		clrSend.Serial( mSock->GetDWord( 3 ) );
		mSock->Send( &clrSend );
	}
	Weight->calcWeight( mChar );
	statwindow( mSock, mChar );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void restock( bool stockAll )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Restock NPC Vendors buy and sell containers
//o---------------------------------------------------------------------------o
void restock( bool stockAll )
{
	SERIAL serial;
	CItem *ci;
	int a;

	for( ITEM i = 0; i < itemcount; i++ )
	{
		if( items[i].GetRestock() && items[i].GetCont( 1 ) >= 0x40 )
		{
			serial = items[i].GetCont();
			if( serial != INVALIDSERIAL )
			{
				ci = calcItemObjFromSer( serial );
				if( ci != NULL )
				{
					if( ( ci->GetLayer() == 0x1A ) )
					{
						if( stockAll )
						{
							items[i].SetAmount( items[i].GetAmount() + items[i].GetRestock() );
							items[i].SetRestock( 0 );
						}
						else
						{
							if( items[i].GetRestock() > 0 )
							{
								a = min( items[i].GetRestock(), (items[i].GetRestock() / 2 ) + 1 );
								items[i].SetAmount( items[i].GetAmount() + a );
								items[i].SetRestock( items[i].GetRestock() - a );
							}
						}
					}
				}
			}
			if( cwmWorldState->ServerData()->GetTradeSystemStatus() )
				StoreItemRandomValue( &items[i], -1 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void sendSellSubItem( CChar *npc, CItem *p, CItem *q, UI08 *m1, int &m1t)
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Send available sell items to client for Sell list
//o---------------------------------------------------------------------------o
void sendSellSubItem( CChar *npc, CItem *p, CItem *q, UI08 *m1, int &m1t)
{
	int value,z;
	char ciname[256];
	char cinam2[256];
	char itemname[256];
	
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			strcpy( ciname, i->GetName() );
			strcpy( cinam2, q->GetName() );
			strupr( ciname );
			strupr( cinam2 );
			
			if( i->GetType() == 1 )
				sendSellSubItem( npc, i, q, m1, m1t );
			else if( i->GetID() == q->GetID() && i->GetType() == q->GetType() &&
				m1[8] < 60 && ( !cwmWorldState->ServerData()->GetSellByNameStatus() ||
				( cwmWorldState->ServerData()->GetSellByNameStatus() && !strcmp( ciname, cinam2 ) ) ) )
			{
				m1[m1t+0] = i->GetSerial( 1 );
				m1[m1t+1] = i->GetSerial( 2 );
				m1[m1t+2] = i->GetSerial( 3 );
				m1[m1t+3] = i->GetSerial( 4 );
				m1[m1t+4] = i->GetID( 1 );
				m1[m1t+5] = i->GetID( 2 );
				m1[m1t+6] = i->GetColour( 1 );
				m1[m1t+7] = i->GetColour( 2 );
				m1[m1t+8] = (UI08)(i->GetAmount()>>8);
				m1[m1t+9] = (UI08)(i->GetAmount()%256);
				value = calcValue( i, q->GetValue() );
				if( cwmWorldState->ServerData()->GetTradeSystemStatus() )
					value = calcGoodValue( npc, q, value, 1 );
				m1[m1t+10] = (UI08)(value>>8);
				m1[m1t+11] = (UI08)(value%256);
				m1[m1t+12] = 0;// Unknown... 2nd length byte for string?
				m1[m1t+13] = (UI08)(getTileName( i, itemname ));
				m1t += 14;
				for( z = 0; z <m1[m1t-1]; z++ )
					m1[m1t+z] = itemname[z];

				m1t += m1[m1t-1];
				m1[8]++;
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool sendSellStuff( cSocket *s, CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Send Vendor Sell List to client
//o---------------------------------------------------------------------------o
bool sendSellStuff( cSocket *s, CChar *i )
{
	char itemname[256];
	int value;
	UI08 m1[2048];
	char m2[2];
	char ciname[256];
	char cinam2[256];
	
	CItem *vendorPack = FindItemOnLayer( i, 0x1C );	// find the acceptable sell layer
	if( vendorPack == NULL ) 
		return false;	// no layer
	
	m2[0] = 0x33;
	m2[1] = 0x01;
	CChar *mChar = s->CurrcharObj();
	s->Send( m2, 2 );
	
	CItem *pack = getPack( mChar );				// no pack for the player
	if( pack == NULL ) 
		return false;
	
	m1[0] = 0x9E; // Header
	m1[1] = 0; // Size
	m1[2] = 0; // Size
	m1[3] = i->GetSerial( 1 );
	m1[4] = i->GetSerial( 2 );
	m1[5] = i->GetSerial( 3 );
	m1[6] = i->GetSerial( 4 );
	m1[7] = 0; // Num items
	m1[8] = 0; // Num items
	int m1t = 9;

	for( CItem *q = vendorPack->FirstItemObj(); !vendorPack->FinishedItems(); q = vendorPack->NextItemObj() )
	{
		if( q != NULL )
		{
			for( CItem *j = pack->FirstItemObj(); !pack->FinishedItems(); j = pack->NextItemObj() )
			{
				if( j != NULL )
				{
					strcpy( ciname, j->GetName() );
					strcpy( cinam2, q->GetName() );
					strupr( ciname );
					strupr( cinam2 );
					if( j->GetType() == 1 )
						sendSellSubItem( i, j, q, m1, m1t );
					else if( ( j->GetID() == q->GetID() && j->GetType() == q->GetType() &&
						m1[8] < 60 && !cwmWorldState->ServerData()->GetSellByNameStatus() ) || ( cwmWorldState->ServerData()->GetSellByNameStatus() && 
						!strcmp( ciname, cinam2 ) ) )
					{
						m1[m1t+0] = j->GetSerial( 1 );
						m1[m1t+1] = j->GetSerial( 2 );
						m1[m1t+2] = j->GetSerial( 3 );
						m1[m1t+3] = j->GetSerial( 4 );
						m1[m1t+4] = j->GetID( 1 );
						m1[m1t+5] = j->GetID( 2 );
						m1[m1t+6] = j->GetColour( 1 );
						m1[m1t+7] = j->GetColour( 2 );
						m1[m1t+8] = (UI08)(j->GetAmount()>>8);
						m1[m1t+9] = (UI08)(j->GetAmount()%256);
						value = calcValue( j, q->GetValue() );
						if( cwmWorldState->ServerData()->GetTradeSystemStatus() )
							value = calcGoodValue( i, j, value, 1 );
						m1[m1t+10] = (UI08)(value>>8);
						m1[m1t+11] = (UI08)(value%256);
						m1[m1t+12] = 0;// Unknown... 2nd length byte for string?
						m1[m1t+13] = (UI08)(getTileName( j, itemname ));
						m1t=m1t+14;
						for( int z = 0; z < m1[m1t-1]; z++ )
							m1[m1t+z]=itemname[z];

						m1t=m1t+m1[m1t-1];
						m1[8]++;
					}
				}
			}
		}
	}
	
	m1[1] = (UI08)(m1t>>8);
	m1[2] = (UI08)(m1t%256);
	if( m1[8] != 0 )
		s->Send( m1, m1t );
	else
		npcTalkAll( i, 1341, false );
	m2[0] = 0x33;
	m2[1] = 0x00;
	s->Send( m2, 2 );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void sellItem( cSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Player sells an item to the vendor
//o---------------------------------------------------------------------------o
void sellItem( cSocket *mSock )
{
	int i, amt, value=0, totgold=0;
	CItem *j, *k;
	int maxsell;
	char tmpmsg[256];
	*tmpmsg = '\0';

	CChar *mChar = mSock->CurrcharObj();
	if( mSock->GetByte( 8 ) != 0 )
	{
		CChar *n = calcCharObjFromSer( mSock->GetDWord( 3 ) );
		CItem *sellPack = FindItemOnLayer( n, 0x1A );
		CItem *boughtPack = FindItemOnLayer( n, 0x1B );
		CItem *buyPack = FindItemOnLayer( n, 0x1C );
		// Pre Calculate Total Amount of selling items to STOPS if the items if greater than SELLMAXITEM - Magius(CHE)
		maxsell = 0;
		for( i = 0; i < mSock->GetByte( 8 ); i++ )
		{
			j = calcItemObjFromSer( mSock->GetDWord( 9 + (6*i) ) );
			amt = mSock->GetWord( 13 + (6*i) );
			maxsell += amt;
		}

		if( maxsell > cwmWorldState->ServerData()->GetSellMaxItemsStatus() )
		{
			sprintf( tmpmsg, Dictionary->GetEntry( 1342 ), mChar->GetName(), cwmWorldState->ServerData()->GetSellMaxItemsStatus() );
			npcTalkAll( n, tmpmsg, false );
			return;
		}

		for( i = 0; i < mSock->GetByte( 8 ); i++ )
		{
			j = calcItemObjFromSer( mSock->GetDWord( 9 + (6*i) ) );
			amt = mSock->GetWord( 13 + (6*i) );
			if( j != NULL )
			{
				if( j->GetAmount() < amt )
				{
					npcTalkAll( n, 1343, false );
					return;
				}
				CItem *join = NULL;
				for( k = sellPack->FirstItemObj(); !sellPack->FinishedItems(); k = sellPack->NextItemObj() )
				{
					if( k != NULL )
					{
						if( k->GetID() == j->GetID() && j->GetType() == k->GetType() )
							join = k;
					}
				}
				for( k = buyPack->FirstItemObj(); !buyPack->FinishedItems(); k = buyPack->NextItemObj() )
				{
					if( k != NULL )
					{
						if( k->GetID() == j->GetID() && j->GetType() == k->GetType() )
							value = calcValue( j, k->GetValue() );
					}
				}
				if( join != NULL )
				{
					join->SetAmount( join->GetAmount() + amt );
					join->SetRestock( join->GetRestock() - amt );
					totgold = totgold+(amt*value);
					if( j->GetAmount() == amt )
						Items->DeleItem( j );
					else
					{
						j->SetAmount( j->GetAmount() - amt );
						RefreshItem( j );
					}
				}
				else
				{
					totgold += ( amt * value );
					j->SetCont( boughtPack->GetSerial() );
					//remove from shopkeeps inventory lookup tauriel

					CPRemoveItem toRemove = (*j);
					Network->PushConn();
					for( cSocket *zSock = Network->FirstSocket(); !Network->FinishedSockets(); zSock = Network->NextSocket() )
						zSock->Send( &toRemove );
					Network->PopConn();

					if( j->GetAmount() != amt ) 
						Commands->DupeItem( mSock, j, j->GetAmount() - amt );
				}
			} 
		}
		addgold( mSock, totgold );
		goldsfx( mSock, totgold );
	}
	
	CPClearMsg clrSend;
	clrSend.Serial( mSock->GetDWord( 3 ) );
	mSock->Send( &clrSend );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void restockNPC( CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Restock NPC Vendors
//o---------------------------------------------------------------------------o
void restockNPC( CChar *i )
{
	if( i == NULL || !i->IsShop() )
		return;	// if we aren't a shopkeeper, why bother?
	if( shoprestocktime <= uiCurrentTime || overflow )
	{
		CItem *ci = FindItemOnLayer( i, 0x1A );
		if( ci != NULL )
		{
			for( CItem *c = ci->FirstItemObj(); !ci->FinishedItems(); c = ci->NextItemObj() )
			{
				if( c != NULL )
				{
					if( c->GetRestock() )
					{
						int tmp = min( c->GetRestock(), ( c->GetRestock() / 2 ) + 1 );
						c->SetAmount( c->GetAmount() + tmp );
						c->SetRestock( c->GetRestock() - tmp );
					}
					if( cwmWorldState->ServerData()->GetTradeSystemStatus() ) 
						StoreItemRandomValue( c, calcRegionFromXY( i->GetX(), i->GetY(), i->WorldNumber() ) );
				}
			}
		}
	}
}
