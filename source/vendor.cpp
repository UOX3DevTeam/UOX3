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
	UI32 pay = 0, give = v->GetHoldG(), t = 0;
	if( mChar == v->GetOwnerObj() )
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
			Items->SpawnItem( mSock, mChar, give, "#", 1, 0x0EED, 0, true, true);
		
		npcTalk( mSock, v, 1328, false, v->GetHoldG(), pay, give );
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
	int itemtotal;
	UI32 playergoldtotal, goldtotal = 0;
	bool soldout = false, clear = false;
	CChar *mChar = mSock->CurrcharObj();
	CItem *p = getPack( mChar );
	if( p == NULL ) 
		return;

	std::vector< int > bitems, amount, layer;

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
	bool useBank = (goldtotal >= static_cast<UI32>(cwmWorldState->ServerData()->GetBuyThreshold() ));
	if( useBank )
		playergoldtotal = GetBankCount( mChar, 0x0EED );
	else
		playergoldtotal = GetAmount( mChar, 0x0EED );
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
				npcTalkAll( npc, 1337, false, mChar->GetName() );
			else
			{
				if( goldtotal == 1 )
					npcTalkAll( npc, 1338, false, mChar->GetName(), goldtotal );
				else
					npcTalkAll( npc, 1339, false, mChar->GetName(), goldtotal );

				goldSound( mSock, goldtotal );
			}
			
			clear = true;
			if( !mChar->IsGM() ) 
			{
				if( useBank )
					DeleteBankItem( mChar, 0x0EED, 0, goldtotal );
				else
					DeleteQuantity( mChar, 0x0EED, goldtotal );
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
							items[biTemp].SetCont( p );
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

							items[biTemp].SetCont( p );
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
		CPBuyItem clrSend;
		clrSend.Serial( mSock->GetDWord( 3 ) );
		mSock->Send( &clrSend );
	}
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
	cBaseObject *getCont;
	CItem *ci;
	int a;

	for( ITEM i = 0; i < itemcount; i++ )
	{
		if( items[i].GetRestock() && items[i].GetCont( 1 ) >= 0x40 )
		{
			getCont = items[i].GetCont();
			if( getCont != NULL )
			{
				ci = (CItem *)getCont;
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
				StoreItemRandomValue( &items[i], 0xFF );
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
	UI32 value;
	int z;
	char ciname[MAX_NAME];
	char cinam2[MAX_NAME];
	char itemname[MAX_NAME];
	
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
				value = calcValue( i, (UI32)q->GetValue() );
				if( cwmWorldState->ServerData()->GetTradeSystemStatus() )
					value = calcGoodValue( npc, q, value, true );
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
	UI32 value;
	UI08 m1[2048];
	char m2[2];
	char ciname[256];
	char cinam2[256];
	
	CItem *vendorPack = i->GetItemAtLayer( 0x1C );	// find the acceptable sell layer
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
						value = calcValue( j, (UI32)q->GetValue() );
						if( cwmWorldState->ServerData()->GetTradeSystemStatus() )
							value = calcGoodValue( i, j, value, true );
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
	if( mSock->GetByte( 8 ) != 0 )
	{
		CChar *mChar = mSock->CurrcharObj();
		CChar *n			= calcCharObjFromSer( mSock->GetDWord( 3 ) );
		if( n == NULL || mChar == NULL )
			return;
		CItem *sellPack		= n->GetItemAtLayer( 0x1A );
		CItem *boughtPack	= n->GetItemAtLayer( 0x1B );
		CItem *buyPack		= n->GetItemAtLayer( 0x1C );
		CItem *j, *k;
		UI16 i, amt, maxsell = 0;
		UI32 totgold = 0, value = 0;
		for( i = 0; i < mSock->GetByte( 8 ); i++ )
		{
			j = calcItemObjFromSer( mSock->GetDWord( 9 + (6*i) ) );
			amt = mSock->GetWord( 13 + (6*i) );
			maxsell += amt;
		}

		if( maxsell > cwmWorldState->ServerData()->GetSellMaxItemsStatus() )
		{
			npcTalkAll( n, 1342, false, mChar->GetName(), cwmWorldState->ServerData()->GetSellMaxItemsStatus() );
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
							value = calcValue( j, (UI32)k->GetValue() );
					}
				}
				if( join != NULL )
				{
					join->SetAmount( join->GetAmount() + amt );
					join->SetRestock( join->GetRestock() - amt );
					totgold += ( amt * value );
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
					j->SetCont( boughtPack );
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
		goldSound( mSock, totgold );
	}
	
	CPBuyItem clrSend;
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
		CItem *ci = i->GetItemAtLayer( 0x1A );
		if( ci != NULL )
		{
			for( CItem *c = ci->FirstItemObj(); !ci->FinishedItems(); c = ci->NextItemObj() )
			{
				if( c != NULL )
				{
					if( c->GetRestock() )
					{
						UI16 stockAmt = min( c->GetRestock(), ( c->GetRestock() / 2 ) + 1 );
						c->SetAmount( c->GetAmount() + stockAmt );
						c->SetRestock( c->GetRestock() - stockAmt );
					}
					if( cwmWorldState->ServerData()->GetTradeSystemStatus() ) 
						StoreItemRandomValue( c, calcRegionFromXY( i->GetX(), i->GetY(), i->WorldNumber() ) );
				}
			}
		}
	}
}
