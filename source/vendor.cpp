#include "uox3.h"
#include "commands.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "townregion.h"
#include "CJSMapping.h"
#include "cScript.h"

#undef DBGFILE
#define DBGFILE "vendor.cpp"

#include "ObjectFactory.h"

namespace UOX
{

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 calcValue( CItem *i, UI32 value )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the value of an item
//o---------------------------------------------------------------------------o
UI32 calcValue( CItem *i, UI32 value )
{
	if( i->GetType() == IT_POTION )
	{
		UI32 mod = 10;
		if( i->GetTempVar( CITV_MOREX ) > 500 )
			++mod;
		if( i->GetTempVar( CITV_MOREX ) > 900 )
			++mod;
		if( i->GetTempVar( CITV_MOREX ) > 1000 )
			++mod;
		if( i->GetTempVar( CITV_MOREZ ) > 1 )
			mod += (3*(i->GetTempVar( CITV_MOREZ )-1));
		value=(value*mod)/10;
	}

	if( i->GetRank() > 0 && i->GetRank() < 10 && cwmWorldState->ServerData()->RankSystemStatus() )
		value = (UI32)( i->GetRank() * value ) / 10;
	if( value < 1 )
		value = 1;

	if( !i->GetRndValueRate() )
		i->SetRndValueRate( 0 );
	if( i->GetRndValueRate() != 0 && cwmWorldState->ServerData()->TradeSystemStatus() )
		value += (UI32)(value*i->GetRndValueRate())/1000;
	if( value < 1 )
		value = 1;
	return value;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 calcGoodValue( CChar *npcnum2, CItem *i, UI32 value, bool isSelling )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calculate the value of a good
//o---------------------------------------------------------------------------o
UI32 calcGoodValue( CTownRegion *tReg, CItem *i, UI32 value, bool isSelling )
{
	if( tReg == NULL )
		return value;

	SI16 good			= i->GetGood();
	SI32 regvalue		= 0;

	if( good <= -1 )
		return value;

	if( isSelling )
		regvalue = tReg->GetGoodSell( static_cast<UI08>(i->GetGood()) );
	else
		regvalue = tReg->GetGoodBuy( static_cast<UI08>(i->GetGood()) );

	UI32 x = (UI32)( value * abs( regvalue ) ) / 1000;

	if( regvalue < 0 )
		value -= x;
	else
		value += x;

	if( value < 1 )
		value = 1;

	return value;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void buyItem( CSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player buys an item from a vendor
//o---------------------------------------------------------------------------o
bool CPIBuyItem::Handle( void )
{
	UI16 i;
	UI32 playergoldtotal, goldtotal = 0;
	bool soldout	= false, clear = false;
	CChar *mChar	= tSock->CurrcharObj();
	CItem *p		= mChar->GetPackItem();
	if( !ValidateObject( p ) ) 
		return true;

	ITEMLIST bitems;
	std::vector< UI08 > layer;
	std::vector< UI16 > amount;

	// vector for storing all objects that actually end up in user backpack
	std::vector< CItem * > boughtItems;

	CChar *npc		= calcCharObjFromSer( tSock->GetDWord( 3 ) );
	UI16 itemtotal	= static_cast<UI16>((tSock->GetWord( 1 ) - 8) / 7);
	if( itemtotal > 511 ) 
		return true;

	boughtItems.reserve( itemtotal );
	bitems.resize( itemtotal );
	amount.resize( itemtotal );
	layer.resize( itemtotal );
	int baseOffset = 0;
	for( i = 0; i < itemtotal; ++i )
	{
		baseOffset	= 7 * i;
		layer[i]	= tSock->GetByte( 8 + baseOffset );
		bitems[i]	= calcItemObjFromSer( tSock->GetDWord( 9 + baseOffset ) );
		amount[i]	= tSock->GetWord( 13 + baseOffset );
		goldtotal	+= ( amount[i] * ( bitems[i]->GetBuyValue() ) );
	}

	bool useBank = (goldtotal >= static_cast<UI32>(cwmWorldState->ServerData()->BuyThreshold() ));
	if( useBank )
		playergoldtotal = GetBankCount( mChar, 0x0EED );
	else
		playergoldtotal = GetItemAmount( mChar, 0x0EED );
	if( playergoldtotal >= goldtotal || mChar->IsGM() )
	{
		for( i = 0; i < itemtotal; ++i )
		{
			if( bitems[i]->GetAmount() < amount[i] )
				soldout = true;

			// Check if onBuyFromVendor JS event is present for each item being purchased
			// If true, and a return false has been returned from the script, halt the purchase
			UI16 targTrig		= bitems[i]->GetScriptTrigger();
			cScript *toExecute	= JSMapping->GetScript( targTrig );
			if( toExecute != NULL )
			{
				if( toExecute->OnBuyFromVendor( tSock, npc, bitems[i] ) )
				{
					bitems.clear(); //needed???
					return true;
				}
			}
		}
		if( soldout )
		{
			npc->TextMessage( tSock, 1336, TALK, false );
			clear = true;
		}
		else
		{
			if( mChar->IsGM() )
				npc->TextMessage( NULL, 1337, TALK, false, mChar->GetName().c_str() );
			else
			{
				if( goldtotal == 1 )
					npc->TextMessage( NULL, 1338, TALK, false, mChar->GetName().c_str(), goldtotal );
				else
					npc->TextMessage( NULL, 1339, TALK, false, mChar->GetName().c_str(), goldtotal );

				Effects->goldSound( tSock, goldtotal );
			}
			
			clear = true;
			if( !mChar->IsGM() ) 
			{
				if( useBank )
					DeleteBankItem( mChar, goldtotal, 0x0EED );
				else
					DeleteItemAmount( mChar, goldtotal, 0x0EED );
			}
			CItem *biTemp;
			CItem *iMade = NULL;
			UI16 j;
			for( i = 0; i < itemtotal; ++i )
			{
				biTemp	= bitems[i];
				iMade	= NULL;
				if( biTemp->GetAmount() > amount[i] )
				{
					if( biTemp->isPileable() )
					{
						iMade = Items->DupeItem( tSock, biTemp, amount[i] );
						if( iMade != NULL )
						{
							iMade->SetCont( p );
							iMade->PlaceInPack();
							boughtItems.push_back( iMade );
						}
					}
					else
					{
						for( j = 0; j < amount[i]; ++j )
						{
							iMade = Items->DupeItem( tSock, biTemp, 1 );
							if( iMade != NULL )
							{
								iMade->SetCont( p );
								iMade->PlaceInPack();
								boughtItems.push_back( iMade );
							}
						}
					}
					biTemp->IncAmount( -amount[i], true );
					biTemp->SetRestock( biTemp->GetRestock() + amount[i] );
				}
				else
				{
					switch( layer[i] )
					{
						case 0x1A: // Sell Container
							if( biTemp->isPileable() )
							{
								iMade = Items->DupeItem( tSock, biTemp, amount[i] );
								if( iMade != NULL )
								{
									iMade->SetCont( p );
									iMade->PlaceInPack();
									boughtItems.push_back( iMade );
								}
							}
							else
							{
								for( j = 0; j < amount[i]; ++j )
								{
									iMade = Items->DupeItem( tSock, biTemp, 1 );
									if( iMade != NULL )
									{
										iMade->SetCont( p );
										iMade->PlaceInPack();
										boughtItems.push_back( iMade );
									}
								}
							}
							biTemp->IncAmount( -amount[i], true );
							biTemp->SetRestock( biTemp->GetRestock() + amount[i] );
							break;
						case 0x1B: // Bought Container
							if( biTemp->isPileable() )
							{
								biTemp->SetCont( p );
								boughtItems.push_back( biTemp );
							}
							else
							{
								for( j = 0; j < amount[i]-1; ++j )
								{
									iMade = Items->DupeItem( tSock, biTemp, 1 );
									if( iMade != NULL )
									{
										iMade->SetCont( p );
										iMade->PlaceInPack();
										boughtItems.push_back( iMade );
									}
								}
								biTemp->SetCont( p );
								biTemp->SetAmount( 1 );
								boughtItems.push_back( biTemp );
							}
							break;
						default:
							Console.Error( " Fallout of switch statement without default. vendor.cpp, buyItem()" );
							break;
					}
				}
			}
			for( i = 0; i < boughtItems.size(); ++i )
			{
				if( boughtItems[i] )
				{
					cScript *toGrab = JSMapping->GetScript( boughtItems[i]->GetScriptTrigger() );
					if( toGrab != NULL )
						toGrab->OnBoughtFromVendor( tSock, npc, boughtItems[i] );
				}
			}
		}
	}
	else
		npc->TextMessage( NULL, 1340, TALK, false );
	
	if( clear )
	{
		CPBuyItem clrSend;
		clrSend.Serial( tSock->GetDWord( 3 ) );
		tSock->Send( &clrSend );
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void sellItem( CSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Player sells an item to the vendor
//o---------------------------------------------------------------------------o
bool CPISellItem::Handle( void )
{
	if( tSock->GetByte( 8 ) != 0 )
	{
		CChar *mChar	= tSock->CurrcharObj();
		CChar *n		= calcCharObjFromSer( tSock->GetDWord( 3 ) );
		if( !ValidateObject( n ) || !ValidateObject( mChar ) )
			return true;
		CItem *buyPack		= n->GetItemAtLayer( IL_BUYCONTAINER );
		CItem *boughtPack	= n->GetItemAtLayer( IL_BOUGHTCONTAINER );
		CItem *sellPack		= n->GetItemAtLayer( IL_SELLCONTAINER );
		if( !ValidateObject( buyPack ) || !ValidateObject( sellPack ) || !ValidateObject( boughtPack ) )
			return true;
		CItem *j = NULL, *k = NULL, *l = NULL;
		UI16 amt = 0, maxsell = 0;
		UI08 i = 0;
		UI32 totgold = 0, value = 0;
		for( i = 0; i < tSock->GetByte( 8 ); ++i )
		{
			j = calcItemObjFromSer( tSock->GetDWord( 9 + (6*i) ) );
			amt = tSock->GetWord( 13 + (6*i) );
			maxsell += amt;
		}

		if( maxsell > cwmWorldState->ServerData()->SellMaxItemsStatus() )
		{
			n->TextMessage( NULL, 1342, TALK, false, mChar->GetName().c_str(), cwmWorldState->ServerData()->SellMaxItemsStatus() );
			return true;
		}

		for( i = 0; i < tSock->GetByte( 8 ); ++i )
		{
			j = calcItemObjFromSer( tSock->GetDWord( 9 + (6*i) ) );
			amt = tSock->GetWord( 13 + (6*i) );
			if( ValidateObject( j ) )
			{
				if( j->GetAmount() < amt || FindItemOwner( j ) != mChar )
				{
					n->TextMessage( NULL, 1343, TALK, false );
					return true;
				}

				// Check if onSellToVendor JS event is present for each item being sold
				// If true, and a value of "false" has been returned from the script, halt the sale
				UI16 targTrig		= j->GetScriptTrigger();
				cScript *toExecute	= JSMapping->GetScript( targTrig );
				if( toExecute != NULL )
				{
					if( toExecute->OnSellToVendor( tSock, n, j ) )
					{
						return true;
					}
				}

				CItem *join = NULL;
				CDataList< CItem * > *pCont = boughtPack->GetContainsList();
				for( k = pCont->First(); !pCont->Finished(); k = pCont->Next() )
				{
					if( ValidateObject( k ) )
					{
						if( k->GetID() == j->GetID() && j->GetType() == k->GetType() )
							join = k;
					}
				}
				pCont = buyPack->GetContainsList();
				for( k = pCont->First(); !pCont->Finished(); k = pCont->Next() )
				{
					if( ValidateObject( k ) )
					{
						if( k->GetID() == j->GetID() && j->GetType() == k->GetType() )
							value = calcValue( j, k->GetSellValue() );
					}
				}

				// If an object already exist in the boughtPack that this one can be joined to...
				if( ValidateObject( join ) )
				{
					join->IncAmount( amt );
					join->SetRestock( join->GetRestock() - amt );
					l = join;

					totgold += ( amt * value );
					if( j->GetAmount() == amt )
						j->Delete();
					else
						j->IncAmount( -amt );
				}
				else
				{
					//Otherwise, move this item to the vendor's boughtPack
					totgold += ( amt * value );

					if( j->GetAmount() != amt ) 
					{
						l = Items->DupeItem( tSock, j, amt );
						j->SetAmount( j->GetAmount() - amt );
					}
					else
						l = j;

					if( ValidateObject( l ) )
						l->SetCont( boughtPack );
				}
				if( l )
				{
					cScript *toGrab = JSMapping->GetScript( l->GetScriptTrigger() );
					if( toGrab != NULL )
						toGrab->OnSoldToVendor( tSock, n, l );
				}
			} 
		}

		Effects->goldSound( tSock, totgold );
		while( totgold > MAX_STACK )
		{
			Items->CreateScriptItem( tSock, mChar, "0x0EED", MAX_STACK, OT_ITEM, true );
			totgold -= MAX_STACK;
		}
		if( totgold > 0 )
			Items->CreateScriptItem( tSock, mChar, "0x0EED", totgold, OT_ITEM, true );
	}
	
	CPBuyItem clrSend;
	clrSend.Serial( tSock->GetDWord( 3 ) );
	tSock->Send( &clrSend );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void restockNPC( CChar *i, bool stockAll )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Restock NPC Vendors
//o---------------------------------------------------------------------------o
void restockNPC( CChar& i, bool stockAll )
{
	if( !i.IsShop() )
		return;	// if we aren't a shopkeeper, why bother?

	CItem *ci = i.GetItemAtLayer( IL_SELLCONTAINER );
	if( ValidateObject( ci ) )
	{
		CDataList< CItem * > *ciCont = ci->GetContainsList();
		for( CItem *c = ciCont->First(); !ciCont->Finished(); c = ciCont->Next() )
		{
			if( ValidateObject( c ) )
			{
				if( stockAll )
				{
					c->IncAmount( c->GetRestock() );
					c->SetRestock( 0 );
				}
				else if( c->GetRestock() )
				{
					UI16 stockAmt = UOX_MIN( c->GetRestock(), static_cast<UI16>(( c->GetRestock() / 2 ) + 1) );
					c->IncAmount( stockAmt );
					c->SetRestock( c->GetRestock() - stockAmt );
				}
				if( cwmWorldState->ServerData()->TradeSystemStatus() ) 
				{
					CTownRegion *tReg = calcRegionFromXY( i.GetX(), i.GetY(), i.WorldNumber() );
					Items->StoreItemRandomValue( c, tReg );
				}
			}
		}
	}
}

bool restockFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	CChar *c = static_cast< CChar * >(a);
	if( ValidateObject( c ) )
			restockNPC( (*c), (b == 1) );

	return retVal;
}
//o---------------------------------------------------------------------------o
//|   Function    :  void restock( bool stockAll )
//|   Date        :  3/15/2003
//|   Programmer  :	 Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Restock all NPC Vendors
//o---------------------------------------------------------------------------o
void restock( bool stockAll )
{
	UI32 b = (stockAll ? 1 : 0);
	ObjectFactory::getSingleton().IterateOver( OT_CHAR, b, NULL, &restockFunctor );
}

}
