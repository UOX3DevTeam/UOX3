#include "uox3.h"
#include "weight.h"
#include "combat.h"
#include "cRaces.h"
#include "skills.h"
#include "cMagic.h"
#include "CJSMapping.h"
#include "mapstuff.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "books.h"
#include "magic.h"
#include "townregion.h"
#include "gump.h"
#include "cGuild.h"
#include "ssection.h"
#include "cServerDefinitions.h"
#include "cSkillClass.h"
#include "Dictionary.h"

namespace UOX
{

void		sendTradeStatus( CItem *cont1, CItem *cont2 );
CItem *		startTrade( CSocket *mSock, CChar *i );
bool		checkItemRange( CChar *mChar, CItem *i );
bool		isDoorBlocked( CItem *door );
void		DoHouseTarget( CSocket *mSock, UI08 houseEntry );
void		SocketMapChange( CSocket *sock, CChar *charMoving, CItem *gate );
void		BuildGumpFromScripts( CSocket *s, UI16 m );
void		PlankStuff( CSocket *s, CItem *p );
CBoatObj *	GetBoat( CSocket *s );
void		ModelBoat( CSocket *s, CBoatObj *i );

//o---------------------------------------------------------------------------o
//|	Function	-	Bounce( CSocket *bouncer, CItem *bouncing )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Bounce items back from where they came
//o---------------------------------------------------------------------------o
void Bounce( CSocket *bouncer, CItem *bouncing )
{
	if( bouncer == NULL || !ValidateObject( bouncing ) )
		return;
	CPBounce bounce( 5 );
	PickupLocations from	= bouncer->PickupSpot();
	SERIAL spot				= bouncer->PickupSerial();
	switch( from )
	{
		default:
		case PL_NOWHERE:	break;
		case PL_GROUND:		
		{
			SI16 x = bouncer->PickupX();
			SI16 y = bouncer->PickupY();
			SI08 z = bouncer->PickupZ();
			bouncing->SetLocation( x, y, z );

			//If item bounces out of range, bounce it to character's feet instead
			if( !objInRange( bouncer, bouncing, DIST_INRANGE ))
			{
				x = bouncer->CurrcharObj()->GetX();
				y = bouncer->CurrcharObj()->GetY();
				z = bouncer->CurrcharObj()->GetZ();
				bouncing->SetLocation( x, y, z );
			}
		}
			break;
		case PL_OWNPACK:
		case PL_OTHERPACK:
		case PL_PAPERDOLL:
			bouncing->SetContSerial( spot );	
			break;
	}
	bouncing->Dirty( UT_UPDATE );
	bouncer->Send( &bounce );
	bouncer->PickupSpot( PL_NOWHERE );
	bouncer->SetCursorItem( NULL );
}

//o---------------------------------------------------------------------------o
//|	Function	-	PickupBounce( CSocket *bouncer, CItem *bouncing )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Bounce items back if pickup is illegal. Doesn't require updating item.
//o---------------------------------------------------------------------------o
void PickupBounce( CSocket *bouncer )
{
	if( bouncer == NULL )
		return;
	CPBounce bounce( 0 );
	bouncer->Send( &bounce );
	bouncer->PickupSpot( PL_NOWHERE );
	bouncer->SetCursorItem( NULL );
}
//o---------------------------------------------------------------------------o
//|   Function    :  CItem *autoStack( CSocket *mSock, CItem *i, CItem *pack )
//|   Date        :  8/14/01
//|   Programmer  :  Zane
//|	  Modified	  :	 Abaddon, 9th September, 2001, returns true if item deleted
//o---------------------------------------------------------------------------o
//|   Purpose     :  Searches pack for pileable items that match the item being
//|					 dropped into said pack (only if it's pileable), if found
//|					 ensures the amount won't go over 65535 (the limit how large
//|					 an item can stack) then stacks it. If the item is not stackable
//|					 or it cannot stack the item with a pile and have an amount that
//|					 is <= 65355 then it creates a new pile.
//|									
//|	Modification	-	09/25/2002	-	Brakthus - Weight fixes
//o---------------------------------------------------------------------------o
CItem *doStacking( CSocket *mSock, CChar *mChar, CItem *i, CItem *stack )
{
	UI32 newAmt = stack->GetAmount() + i->GetAmount();
	if( newAmt > MAX_STACK )
	{
		i->SetAmount( ( newAmt - MAX_STACK ) );
		stack->SetAmount( MAX_STACK );
	}
	else
	{
		stack->SetAmount( newAmt );
		i->Delete();
		if( mSock != NULL )
			Effects->itemSound( mSock, stack, false );
		return stack;
	}
	return i;
}

//o--------------------------------------------------------------------------o
//|	Function	-	bool autoStack( CSocket *mSock, CItem *i, CItem *pack )
//|	Date		-	
//|	Developers	-	UOX3 DevTeam
//|	Organization-	UOX3 DevTeam
//|	Status		-	Modified to v2
//|					v2 - accepts a possible NULL socket to deal with the JSE
//|					v3 - returns a CItem * (stack if stacked, item otherwise)
//o--------------------------------------------------------------------------o
//|	Description	-	Searches through the pack to see if an item can be stacked
//|					stacking them automatically
//|
//o--------------------------------------------------------------------------o
CItem *autoStack( CSocket *mSock, CItem *iToStack, CItem *iPack )
{
	CChar *mChar = NULL;
	if( mSock != NULL )
		mChar = mSock->CurrcharObj();
	if( !ValidateObject( iToStack ) || !ValidateObject( iPack ) )
		return NULL;

	iToStack->SetCont( iPack );
	if( iToStack->isPileable() )
	{
		if( mSock != NULL && ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) )
			Weight->subtractItemWeight( mChar, iToStack );
		const UI16 itID		= iToStack->GetID();
		const SERIAL itSer	= iToStack->GetSerial();
		const UI16 itCol	= iToStack->GetColour();
		CDataList< CItem * > *ipCont = iPack->GetContainsList();
		for( CItem *stack = ipCont->First(); !ipCont->Finished(); stack = ipCont->Next() )
		{
			if( !ValidateObject( stack ) )
				continue;

			if( stack->isPileable() && stack->GetAmount() < MAX_STACK && 
				stack->GetSerial() != itSer && stack->GetID() == itID && stack->GetColour() == itCol )
			{ // Autostack
				if( doStacking( mSock, mChar, iToStack, stack ) == stack )	// compare to stack, if doStacking returned the stack, then the raw object was deleted
					return stack;	// return the stack
			}
		}
		if( mSock != NULL && ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) )
			Weight->addItemWeight( mChar, iToStack );
	}
	iToStack->PlaceInPack();
	if( mSock != NULL )
		Effects->itemSound( mSock, iToStack, false );
	return iToStack;
}

//o---------------------------------------------------------------------------o
//|   Function    :  Grab( CSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when a player picks up an item
//o---------------------------------------------------------------------------o
bool CPIGetItem::Handle( void )
{
	CChar *ourChar	= tSock->CurrcharObj();
	SERIAL serial	= tSock->GetDWord( 1 );
	if( serial == INVALIDSERIAL )
		return true;
	ourChar->BreakConcentration( tSock );

	CItem *i = calcItemObjFromSer( serial );
	if( !ValidateObject( i ) )
		return true;

	CPBounce bounce( 0 );

	ObjectType oType	= OT_CBO;
	CBaseObject *iOwner	= NULL;
	CItem *x			= i;
	CBaseObject *iCont	= i->GetCont();
	if( iCont != NULL )  //Find character owning item
	{
		iOwner = FindItemOwner( i, oType );
		tSock->PickupSerial( i->GetContSerial() );
		if( oType == OT_CHAR )
		{
			if( iCont->GetObjType() == OT_CHAR )
				tSock->PickupSpot( PL_PAPERDOLL );
			else
			{
				if( iOwner != ourChar )
					tSock->PickupSpot( PL_OTHERPACK );
				else
					tSock->PickupSpot( PL_OWNPACK );
			}
			bool otherPackCheck = (iOwner != ourChar);
			if( otherPackCheck )	// okay, not our owner!
			{
				otherPackCheck = !ourChar->IsGM();
				if( otherPackCheck )	// not a GM either
				{
					if( iOwner->GetOwnerObj() == ourChar )
						otherPackCheck = false;
					else if( Npcs->checkPetFriend( ourChar, static_cast< CChar * >(iOwner) ) )
						otherPackCheck = false;
				}
			}
			if( otherPackCheck || !objInRange( ourChar, iOwner, DIST_NEARBY ) )
			{
				PickupBounce( tSock );
				return true;
			}
			if( iCont->CanBeObjType( OT_ITEM ) )
			{
				CItem *contItem = static_cast<CItem *>(iCont);
				if( contItem->GetType() == IT_TRADEWINDOW ) // Trade Window
				{
					PickupBounce( tSock );
					return true;
				}
				CItem *recurseItem = FindRootContainer( contItem );
				if( recurseItem->GetType() == IT_TRADEWINDOW )
				{
					CItem *z = calcItemObjFromSer( recurseItem->GetTempVar( CITV_MOREX ) );
					if( ValidateObject( z ) )
					{
						if( z->GetTempVar( CITV_MOREZ ) || recurseItem->GetTempVar( CITV_MOREZ ) )
						{
							z->SetTempVar( CITV_MOREZ, 0 );
							recurseItem->SetTempVar( CITV_MOREZ, 0 );
							sendTradeStatus( z, recurseItem );
						}
						CChar *zChar = FindItemOwner( z );
						if( ValidateObject( zChar ) )
						{
							CSocket *zSock = zChar->GetSocket();
							if( zSock != NULL )
								Effects->PlaySound( zSock, 0x0057, true );
						}
					}
				}
				else if( recurseItem->GetLayer() == IL_BANKBOX )
					tSock->PickupSpot( PL_OTHERPACK );
			}
		}
		else if( oType == OT_ITEM )
		{
			tSock->PickupSpot( PL_OTHERPACK );
			CItem *x = static_cast<CItem *>(iOwner);
			if( !objInRange( ourChar, x, DIST_NEARBY ) )
			{
				PickupBounce( tSock );
				return true;
			}

			if( x->IsLockedDown() )
			{
				CMultiObj *ourMulti = x->GetMultiObj();
				if( ValidateObject( ourMulti ) && !ourMulti->IsOwner( ourChar ) )
				{
					PickupBounce( tSock );
					return true;
				}
			}

			if( x->isCorpse() )
			{
				CChar *corpseTargChar = x->GetOwnerObj();
				if( ValidateObject( corpseTargChar ) )
				{
					if( corpseTargChar->IsGuarded() ) // Is the corpse being guarded?
						Combat->petGuardAttack( ourChar, corpseTargChar, corpseTargChar );
					else if( x->isGuarded() )
						Combat->petGuardAttack( ourChar, corpseTargChar, x );
				}
			}
		}
	}
	else
	{
		tSock->PickupSpot( PL_GROUND );
		tSock->PickupLocation( i->GetX(), i->GetY(), i->GetZ() );
		if( !ourChar->IsGM() && ( !objInRange( ourChar, i, DIST_NEARBY ) || !LineOfSight( tSock, ourChar, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS, true )))
		{
			PickupBounce( tSock );
			return true;
		}
	}

	if( i->isCorpse() || !checkItemRange( ourChar, x ) )
	{
		PickupBounce( tSock );
		return true;
	}

	if( x->GetMultiObj() != NULL )
	{
		if( ( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND ) && ( x->GetMultiObj() != ourChar->GetMultiObj() || x->IsLockedDown() ))
		{
			PickupBounce( tSock );
			return true;
		}
		i->SetMulti( INVALIDSERIAL );
	}

	if( i->isDecayable() )
		i->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ) );
	if( i->isGuarded() )
	{
		if( oType == OT_CHAR && tSock->PickupSpot() == PL_OTHERPACK )
			Combat->petGuardAttack( ourChar, static_cast<CChar *>(iOwner), i );

		CChar *petGuard = Npcs->getGuardingPet( ourChar, i );
		if( ValidateObject( petGuard ) )
			petGuard->SetGuarding( NULL );
		i->SetGuarded( false );
	}

	if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
	{
		CTileHS& tile = Map->SeekTileHS( i->GetID() );
		if( !ourChar->AllMove() && ( i->GetMovable() == 2 || i->IsLockedDown() ||
			( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
		{
			PickupBounce( tSock );
			return true;
		}
	}
	else
	{
		CTile& tile = Map->SeekTile( i->GetID() );
		if( !ourChar->AllMove() && ( i->GetMovable() == 2 || i->IsLockedDown() ||
			( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
		{
			PickupBounce( tSock );
			return true;
		}
	}

	UI16 targTrig		= i->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	if( toExecute != NULL )
	{
		if( !toExecute->OnPickup( i, ourChar ) )	// returns false if we should bounce it
		{
			PickupBounce( tSock );
			return true;
		}
	}

	Effects->PlaySound( tSock, 0x0057, true );
	if( i->GetAmount() > 1 && i->GetObjType() != OT_SPAWNER )
	{
		UI16 amount = tSock->GetWord( 5 );
		if( amount > i->GetAmount() )
			amount = i->GetAmount();
		else if( amount < i->GetAmount() )
		{
			CItem *c = i->Dupe();
			if( c != NULL )
			{
				c->IncAmount( -amount );
				c->SetCont( i->GetCont() );
			}
			i->SetAmount( amount );
		}
		if( i->GetID() == 0x0EED )
		{
			if( tSock->PickupSpot() == PL_OWNPACK )
				ourChar->Dirty( UT_STATWINDOW );
		}
	}
	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
	{
		if( !Weight->checkCharWeight( NULL, ourChar, i ) )
		{
			tSock->sysmessage( 1743 );
			PickupBounce( tSock );
			return true;
		}
	}

	if( iCont == NULL )
		MapRegion->RemoveItem( i );
	i->RemoveFromSight();

	tSock->SetCursorItem( i );

	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
		Weight->addItemWeight( ourChar, i );
	return true;
}



//o---------------------------------------------------------------------------o
//|   Function    :  Equip( CSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item is dropped on a players paperdoll
//o---------------------------------------------------------------------------o
bool CPIEquipItem::Handle( void )
{
	CChar *ourChar	= tSock->CurrcharObj();
	SERIAL cserial	= tSock->GetDWord( 6 );
	SERIAL iserial	= tSock->GetDWord( 1 );
	if( cserial == INVALIDSERIAL || iserial == INVALIDSERIAL )
		return true;

	ourChar->BreakConcentration( tSock );
	CItem *i = calcItemObjFromSer( iserial );
	CChar *k = calcCharObjFromSer( cserial );
	if( !ValidateObject( i ) )
		return true;

	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_PAPERDOLL )
	{
		ObjectType pOType;
		CBaseObject *pOwner	= FindItemOwner( i, pOType );
		if( pOType == OT_CHAR )
		{
			CChar *pOChar = static_cast<CChar *>(pOwner);
			if( pOChar != ourChar && ( ( !ourChar->IsGM() && pOChar->GetOwnerObj() != ourChar ) ) || !objInRange( ourChar, pOwner, DIST_NEARBY ) )
			{
				Bounce( tSock, i );
				return true;
			}
		}
		else if( pOType == OT_ITEM )
		{
			if( !objInRange( ourChar, pOwner, DIST_NEARBY ) )
			{
				Bounce( tSock, i );
				return true;
			}
			CItem *pOItem = static_cast<CItem *>(pOwner);
			if( pOItem->IsLockedDown() )
			{
				CMultiObj *ourMulti = pOwner->GetMultiObj();
				if( ValidateObject( ourMulti ) && !ourMulti->IsOwner( ourChar ) )
				{
					Bounce( tSock, i );
					return true;
				}
			}
		}

	}
	else if( tSock->PickupSpot() == PL_GROUND )
	{
		if( !objInRange( ourChar, i, DIST_NEARBY ) )
		{
			Bounce( tSock, i );
			return true;
		}
	}

	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
		Weight->subtractItemWeight( ourChar, i ); // SetCont() adds the weight for us (But we also had to add the weight in grabItem() since it sets cont as INVALIDSERIAL

	if( !ourChar->IsGM() && k != ourChar )	// players cant equip items on other players or npc`s paperdolls.  // GM PRIVS
	{
		Bounce( tSock, i );
		tSock->sysmessage( 1186 );
		return true;
	}

	if( ourChar->IsDead() )
	{
		tSock->sysmessage( 1185 );
		return true;
	}
	if( !ValidateObject( k ) )
		return true;

	ARMORCLASS ac1 = Races->ArmorRestrict( k->GetRace() );
	ARMORCLASS ac2 = i->GetArmourClass();

	if( ac1 != 0 && ( (ac1&ac2) == 0 ) )	// bit comparison, if they have ANYTHING in common, they can wear it
	{
		tSock->sysmessage( 1187 );
		Bounce( tSock, i );
		return true;
	}
	if( k == ourChar )
	{
		bool canWear = false;
		if( i->GetStrength() > k->GetStrength() )
			tSock->sysmessage( 1188 );
		else if( i->GetDexterity() > k->GetDexterity() )
			tSock->sysmessage( 1189 );
		else if( i->GetIntelligence() > k->GetIntelligence() )
			tSock->sysmessage( 1190 );
		else
			canWear = true;
		if( !canWear )
		{
			Bounce( tSock, i );

			if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
				Effects->itemSound( tSock, i, true );
			else
				Effects->itemSound( tSock, i, false );
			return true;
		}
	}
	if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
	{
		CTileHS& tile = Map->SeekTileHS( i->GetID() );
		if( !ourChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != ourChar ) ||
			( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
		{
			Bounce( tSock, i );
			return true;
		}
	}
	else
	{
		CTile& tile = Map->SeekTile( i->GetID() );
		if( !ourChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != ourChar ) ||
			( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
		{
			Bounce( tSock, i );
			return true;
		}
	}

	if( i->GetLayer() == IL_NONE )
		i->SetLayer( static_cast<ItemLayers>(tSock->GetByte( 5 )) );

	// 1/13/2003 - Xuri - Fix for equipping an item to more than one hand, or multiple equipping.
	if( i->GetCont() != k )
	{
		bool conflictItem = true;
		CItem *j = k->GetItemAtLayer( i->GetLayer() );
		if( !ValidateObject( j ))
		{
			if( i->GetLayer() == IL_RIGHTHAND )
				j = k->GetItemAtLayer( IL_LEFTHAND );
			else if( i->GetLayer() == IL_LEFTHAND )
				j = k->GetItemAtLayer( IL_RIGHTHAND );

			// GetDir-check is to allow for torches and lanterns, 
			// which use left-hand layer but are not 2-handers or shields
			if( ValidateObject( j ) && !i->IsShieldType() && i->GetDir() == 0 )
			{
				if( j->IsShieldType() || j->GetDir() != 0 )
					conflictItem = false;
			}
			else
				conflictItem = false;
		}
		if( conflictItem == true )
		{
			tSock->sysmessage( 1744 );
			Bounce( tSock, i );
			return true;
		}
	}

	if( !Weight->checkCharWeight( ourChar, k, i ) )
	{
		tSock->sysmessage( 1743 );
		Bounce( tSock, i );
		return true;
	}

	i->SetCont( k );

	//Reset PickupSpot after dropping the item
	tSock->PickupSpot( PL_NOWHERE );
	tSock->SetCursorItem( NULL );

	CPDropItemApproved lc;
	tSock->Send( &lc );

	Effects->PlaySound( tSock, 0x0057, false );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function		:	DropOnChar( CSocket *mSock, CChar *targChar, CItem *i )
//|   Date			:	Unknown
//|   Programmer	:	UOX3 DevTeam
//|	  Modified		:	Abaddon, September 14th, 2001, returns true if item deleted
//|					:	Zane, September 21st, 2003, moved into seperate file and
//|					:		few other minor tweaks
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item is dropped on a character
//|									
//|	Modification	-	09/25/2002	-	Xuri/Brakthus - Weight fixes
//o---------------------------------------------------------------------------o
bool DropOnPC( CSocket *mSock, CChar *mChar, CChar *targPlayer, CItem *i )
{
	bool stackDeleted = false;
	if( targPlayer == mChar )
	{
		CItem *pack = mChar->GetPackItem();
		if( !ValidateObject( pack ) ) // if player has no pack, put it at its feet
		{
			i->SetCont( NULL );
			i->SetLocation( mChar );
		} 
		else
			stackDeleted = ( autoStack( mSock, i, pack ) != i );
	}
	else // Trade stuff
	{
		if( isOnline( (*targPlayer) ) )
		{
			CItem *j = startTrade( mSock, targPlayer );
			if( ValidateObject( j ) )
			{
				i->SetCont( j );
				i->SetX( 30 );
				i->SetY( 30 );
				i->SetZ( 9 );
			}
		}
		else
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( mChar, i );
			Bounce( mSock, i );
			return true;
		}
	}
	return stackDeleted;
}

bool IsOnFoodList( const std::string& sFoodList, const UI16 sItemID )
{
	bool doesEat = false;

	const UString sect		= "FOODLIST " + sFoodList;
	ScriptSection *FoodList = FileLookup->FindEntry( sect, items_def );
	if( FoodList != NULL )
	{
		for( UString tag = FoodList->First(); !FoodList->AtEnd() && !doesEat; tag = FoodList->Next() )
		{
			if( !tag.empty() )
			{
				if( tag.upper() == "FOODLIST" )
					doesEat = IsOnFoodList( FoodList->GrabData(), sItemID );
				else if( sItemID == tag.toUShort() )
					doesEat = true;
			}
		}
	}
	return doesEat;
}

bool DropOnNPC( CSocket *mSock, CChar *mChar, CChar *targNPC, CItem *i )
{
	UI08 dropResult		= 0;
	const bool isGM		= (mChar->GetCommandLevel() >= CL_CNS);
	bool stackDeleted	= false;
	bool executeNpc		= true;
	UI16 targTrig		= i->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	UI08 rVal			= 0;
	if( toExecute != NULL )
	{
		rVal = toExecute->OnDropItemOnNpc( mChar, targNPC, i );	// returns 1 if we should bounce it
		switch( rVal )
		{
			case 0:	// no such event
			default:
				executeNpc = true;
				break;
			case 1:	// bounce, no code
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					Weight->subtractItemWeight( mChar, i );
				Bounce( mSock, i );
				return false;	// stack not deleted, as we're bouncing
			case 2:	// don't bounce, use code
				executeNpc = false;
				break;
			case 3:	// don't bounce, don't use code
				executeNpc = true;
				break;
		}
	}
	if( executeNpc )
	{
		targTrig	= targNPC->GetScriptTrigger();
		toExecute	= JSMapping->GetScript( targTrig );
		if( toExecute != NULL )
		{
			rVal = toExecute->OnDropItemOnNpc( mChar, targNPC, i );
			switch( rVal )
			{
				case 0:	// no such event
				case 2:	// we don't want to bounce, use code
				default:
					break;
				case 1:	// we want to bounce and return
					// If we want to bounce, then it's safe to assume the item still exists!
					if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
						Weight->subtractItemWeight( mChar, i );
					Bounce( mSock, i );
					return false;	// bouncing, so item exists
				case 3:	// we don't want to bounce, don't use code
					return true;	// don't let the code assume the item exists, so we never reference it
			}
		}
	}

	if( targNPC->IsTamed() && ( isGM || targNPC->GetOwnerObj() == mChar || Npcs->checkPetFriend( mChar, targNPC ) ) ) // do food stuff
	{	
		if( targNPC->WillHunger() && IsOnFoodList( targNPC->GetFood(), i->GetID() ) )
		{
			if( targNPC->GetHunger() < 6 )
			{
				Effects->PlaySound( mSock, static_cast<UI16>(0x003A + RandomNum( 0, 2 )), true );
				Effects->PlayCharacterAnimation( targNPC, 3 );

				if( i->GetPoisoned() && targNPC->GetPoisoned() < i->GetPoisoned() )
				{
					Effects->PlaySound( mSock, 0x0246, true ); //poison sound - SpaceDog
					targNPC->SetPoisoned( i->GetPoisoned() );
					targNPC->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POISON ) );
				}
				//Remove a food item
				bool iDeleted = i->IncAmount( -1 );
				targNPC->SetHunger( static_cast<SI08>(targNPC->GetHunger() + 1) );
				mSock->sysmessage( 1781 );
				if( iDeleted )
					return true; //stackdeleted
			}
			else
				mSock->sysmessage( 1780 );
		}
		else if( isGM || targNPC->GetID() == 0x0123 || targNPC->GetID() == 0x0124 )	// It's a pack animal
			dropResult = 2;
	}
	else if( cwmWorldState->creatures[targNPC->GetID()].IsHuman() )
	{
		if( static_cast<CChar *>(mSock->TempObj()) != targNPC )
		{
			if( isGM )
				dropResult = 2;
			else
			{
				dropResult = 1;
				targNPC->TextMessage( mSock, 1197, TALK, false );
			}
		}
		else if( i->GetID() == 0x0EED ) // They gave the NPC gold
		{
			UI08 trainedIn = targNPC->GetTrainingPlayerIn();
			targNPC->TextMessage( mSock, 1198, TALK, false );
			UI16 oldskill = mChar->GetBaseSkill( trainedIn ); 
			mChar->SetBaseSkill( (UI16)( mChar->GetBaseSkill( trainedIn ) + i->GetAmount() ), trainedIn );
			if( mChar->GetBaseSkill( trainedIn ) > 250 )
				mChar->SetBaseSkill( 250, trainedIn );
			Skills->updateSkillLevel( mChar, trainedIn );
			mSock->updateskill( trainedIn );
			UI16 getAmount = i->GetAmount();
			if( i->GetAmount() > 250 ) // Paid too much
			{
				i->IncAmount( -(250 - oldskill) );
				dropResult = 1;
			}
			else  // Gave exact change
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					Weight->subtractItemWeight( mChar, i );
				i->Delete();
				stackDeleted = true;
			}
			mSock->TempObj( NULL );
			targNPC->SetTrainingPlayerIn( 0xFF );
			Effects->goldSound( mSock, getAmount, false );
		}
		else // Did not give gold
		{
			dropResult = 1;
			targNPC->TextMessage( mSock, 1199, TALK, false );
		}
	}
	else
		dropResult = ( isGM ? 2 : 1 );

	switch( dropResult )
	{
	default:
	case 0:		// Do nothing;
		break;
	case 1:		// Bounce
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( mChar, i );

		Bounce( mSock, i );
		break;
	case 2:		// Stack
		CItem *pack;
		pack = targNPC->GetPackItem();
		if( ValidateObject( pack ) )
			stackDeleted = ( autoStack( mSock, i, pack ) != i );
		break;
	}
	return stackDeleted;
}
bool DropOnChar( CSocket *mSock, CChar *targChar, CItem *i )
{
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return false;

	if( !Weight->checkCharWeight( mChar, targChar, i ) )
	{
		mSock->sysmessage( 1743 );
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( mChar, i );
		Bounce( mSock, i );
		return false;
	}

	if( !targChar->IsNpc() )
		return DropOnPC( mSock, mChar, targChar, i );

	return DropOnNPC( mSock, mChar, targChar, i );
}

//o---------------------------------------------------------------------------o
//|   Function    :  Drop( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Item is dropped on the ground or on a character
//o---------------------------------------------------------------------------o
void Drop( CSocket *mSock, SERIAL item, SERIAL dest, SI16 x, SI16 y, SI08 z, SI08 gridLoc )
{
	CChar *nChar	= mSock->CurrcharObj();
	CItem *i		= calcItemObjFromSer( item );
	bool stackDeleted = false;

	if( !ValidateObject( i ) )
	{
		nChar->Teleport();
		return;
	}

	//Bounce if no valid pickupspot exists
	if( mSock->PickupSpot() == PL_NOWHERE )
	{
		Bounce( mSock, i );
		return;
	}

	UI16 targTrig		= i->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	if( toExecute != NULL )
		toExecute->OnDrop( i, nChar );

	if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
	{
		CTileHS& tile = Map->SeekTileHS( i->GetID() );
		if( !nChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != nChar ) ||
			( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( nChar, i );
			Bounce( mSock, i );
			return;
		}
	}
	else
	{
		CTile& tile = Map->SeekTile( i->GetID() );
		if( !nChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != nChar ) ||
			( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( nChar, i );
			Bounce( mSock, i );
			return;
		}
	}
	if( mSock->GetByte( 5 ) != 0xFF )	// Dropped in a specific location or on an item
	{
		if( !nChar->IsGM() && !LineOfSight( mSock, nChar, x, y, z, WALLS_CHIMNEYS + DOORS, true ))
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( nChar, i );
			Bounce( mSock, i );
			return;
		}
		i->SetCont( NULL );
		i->SetLocation( x, y, z, gridLoc, nChar->WorldNumber() );
	}
	else
	{
		CChar *t = calcCharObjFromSer( dest );
		if( ValidateObject( t ) )
			stackDeleted = DropOnChar( mSock, t, i );
		else
		{
			//Bounces items dropped in illegal locations in 3D UO client!!!
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( nChar, i );
			Bounce( mSock, i );
			return;
		}
	}

	if( !stackDeleted )
	{
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( nChar, i );

		if( i->isDecayable() )
			i->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ) );

		if( nChar->GetMultiObj() != NULL )
		{
			CMultiObj *multi = findMulti( i );
			if( ValidateObject( multi ) )
				i->SetMulti( multi );
		}
		Effects->itemSound( mSock, i, ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) );
	}
	// IF client-version is above 6.0.1.7, send approval packet for dropping item. If not, don't.
	CPDropItemApproved lc;

	mSock->Send( &lc );
}

void DropOnTradeWindow( CSocket& mSock, CChar& mChar, CItem& tradeWindowOne, CItem& iDropped, SI16 x, SI16 y, SI08 gridLoc )
{
	if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
		Weight->subtractItemWeight( &mChar, &iDropped );
	iDropped.SetCont( &tradeWindowOne );
	iDropped.SetX( x );
	iDropped.SetY( y );
	iDropped.SetZ( 9 );
	iDropped.SetGridLocation( gridLoc );
	CItem *tradeWindowTwo = calcItemObjFromSer( tradeWindowOne.GetTempVar( CITV_MOREX ) );
	if( ValidateObject( tradeWindowTwo ) )
	{
		if( tradeWindowTwo->GetTempVar( CITV_MOREZ ) || tradeWindowOne.GetTempVar( CITV_MOREZ ) )
		{
			tradeWindowTwo->SetTempVar( CITV_MOREZ, 0 );
			tradeWindowOne.SetTempVar( CITV_MOREZ, 0 );
			sendTradeStatus( tradeWindowTwo, &tradeWindowOne );
		}
		CChar *tw2Owner = FindItemOwner( tradeWindowTwo );
		if( ValidateObject( tw2Owner ) )
		{
			CSocket *tw2Sock = tw2Owner->GetSocket();
			if( tw2Sock != NULL )
				Effects->itemSound( tw2Sock, &tradeWindowOne, ( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND ) );
		}
	}
}

void DropOnSpellBook( CSocket& mSock, CChar& mChar, CItem& spellBook, CItem& iDropped )
{
	if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
		Weight->subtractItemWeight( &mChar, &iDropped );
	if( iDropped.GetID( 1 ) != 0x1F || iDropped.GetID( 2 ) < 0x2D || iDropped.GetID( 2 ) > 0x72 )
	{
		Bounce( &mSock, &iDropped );
		mSock.sysmessage( 1202 );
		return;
	}
	CChar *sbOwner = FindItemOwner( &spellBook );
	if( ValidateObject( sbOwner ) && sbOwner != &mChar && !mChar.CanSnoop() )
	{
		Bounce( &mSock, &iDropped );
		mSock.sysmessage( 1203 );
		return;
	}
	std::string name;
	name.reserve( MAX_NAME );
	if( iDropped.GetName()[0] == '#' )
		getTileName( iDropped, name );
	else
		name = iDropped.GetName();

	if( spellBook.GetTempVar( CITV_MORE, 1 ) == 1 )	// using more1 to "lock" a spellbook for RP purposes
	{
		mSock.sysmessage( 1204 );
		Bounce( &mSock, &iDropped );
		return;
	}

	if( name == Dictionary->GetEntry( 1605 ) )
	{
		if( spellBook.GetSpell( 0 ) == INVALIDSERIAL && spellBook.GetSpell( 1 ) == INVALIDSERIAL && spellBook.GetSpell( 2 ) == INVALIDSERIAL )
		{
			mSock.sysmessage( 1205 );
			Bounce( &mSock, &iDropped );
			return;
		}
		spellBook.SetSpell( 0, INVALIDSERIAL );
		spellBook.SetSpell( 1, INVALIDSERIAL );
		spellBook.SetSpell( 2, INVALIDSERIAL );
	}
	else
	{
		int targSpellNum = 0;
		UI16 scrollID = iDropped.GetID();
		if( scrollID == 0x1F2D )
			targSpellNum = 6;
		else if( scrollID > 0x1F2D && scrollID < 0x1F34 )
			targSpellNum = (scrollID - 0x1F2E);
		else if( scrollID >= 0x1F34 && scrollID < 0x1F6D )
			targSpellNum = (scrollID - 0x1F2D);

		if( Magic->HasSpell( &spellBook, targSpellNum ) )
		{
			mSock.sysmessage( 1206 );
			Bounce( &mSock, &iDropped );
			return;
		}
		else
			Magic->AddSpell( &spellBook, targSpellNum );
	}
	Effects->PlaySound( &mSock, 0x0042, false );
	if( iDropped.GetAmount() > 1 )
	{
		iDropped.IncAmount( -1 );
		Bounce( &mSock, &iDropped );
	}
	else
		iDropped.Delete();
}

bool DropOnStack( CSocket& mSock, CChar& mChar, CItem& droppedOn, CItem& iDropped, bool &stackDeleted )
{
	bool canHold = true;
	if( droppedOn.GetCont() != NULL )
	{
		if( droppedOn.GetContSerial() >= BASEITEMSERIAL )
			canHold = Weight->checkPackWeight( &mChar, static_cast<CItem *>(droppedOn.GetCont()), &iDropped );
		else
			canHold = Weight->checkCharWeight( &mChar, static_cast<CChar *>(droppedOn.GetCont()), &iDropped );
	}
	if( !canHold )
	{
		if( droppedOn.GetContSerial() >= BASEITEMSERIAL )
			mSock.sysmessage( 1385 );
		else
			mSock.sysmessage( 1743 );
		if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( &mChar, &iDropped );
		Bounce( &mSock, &iDropped );
		return false;
	}
	if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
		Weight->subtractItemWeight( &mChar, &iDropped );
	stackDeleted = ( doStacking( &mSock, &mChar, &iDropped, &droppedOn ) != &iDropped );
	if( !stackDeleted )	// if the item didn't stack or the stack was full
		Bounce( &mSock, &iDropped );

	return true;
}

bool DropOnContainer( CSocket& mSock, CChar& mChar, CItem& droppedOn, CItem& iDropped, bool &stackDeleted, SI16 x, SI16 y, SI08 gridLoc )
{
	CChar *contOwner = FindItemOwner( &droppedOn );
	if( ValidateObject( contOwner ) )
	{
		if( contOwner == &mChar )
		{
			CBaseObject *recurseCont = droppedOn.GetCont();
			while( ValidateObject( recurseCont ) )
			{
				if( recurseCont->CanBeObjType( OT_ITEM ) )
				{
					CItem *recurseItem = static_cast<CItem *>(recurseCont);
					if( recurseItem->GetType() == IT_TRADEWINDOW )
					{
						CItem *tradeWindowTwo = calcItemObjFromSer( recurseItem->GetTempVar( CITV_MOREX ) );
						if( ValidateObject( tradeWindowTwo ) )
						{
							if( tradeWindowTwo->GetTempVar( CITV_MOREZ ) || recurseItem->GetTempVar( CITV_MOREZ ) )
							{
								tradeWindowTwo->SetTempVar( CITV_MOREZ, 0 );
								recurseItem->SetTempVar( CITV_MOREZ, 0 );
								sendTradeStatus( tradeWindowTwo, recurseItem );
							}
							CChar *tw2Char = FindItemOwner( tradeWindowTwo );
							if( ValidateObject( tw2Char ) )
							{
								CSocket *tw2Sock = tw2Char->GetSocket();
								if( tw2Sock != NULL )
									Effects->itemSound( tw2Sock, recurseItem, ( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND ) );
							}
						}
						break;
					}
					else
						recurseCont = recurseItem->GetCont();
				}
				else
					break;
			}
		}
		else if( contOwner->IsNpc() && contOwner->GetNPCAiType() == AI_PLAYERVENDOR )
		{
			if( contOwner->GetOwnerObj() == &mChar || mChar.GetCommandLevel() > CL_PLAYER )
			{
				mChar.SetSpeechMode( 3 );
				mChar.SetSpeechItem( &iDropped );
				mSock.sysmessage( 1207 );
			}
			else
			{
				if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
					Weight->subtractItemWeight( &mChar, &iDropped );
				mSock.sysmessage( 1630 );
				Bounce( &mSock, &iDropped );
				return false;
			}
		}
		else if( mChar.GetCommandLevel() < CL_CNS && ( !contOwner->IsNpc() || !contOwner->IsTamed() || 
			( contOwner->GetID() != 0x0123 && contOwner->GetID() != 0x0124 ) ||
			( contOwner->GetOwnerObj() != &mChar && !Npcs->checkPetFriend( &mChar, contOwner ) ) ) )
		{
			if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( &mChar, &iDropped );
			mSock.sysmessage( 1630 );
			Bounce( &mSock, &iDropped );
			return false;
		}
	}
	if( mSock.GetByte( 5 ) != 0xFF )	// In a specific spot in a container
	{
		if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( &mChar, &iDropped );

		if( &droppedOn != iDropped.GetCont() && !Weight->checkPackWeight( &mChar, &droppedOn, &iDropped ) )
		{
			mSock.sysmessage( 1385 );
			Bounce( &mSock, &iDropped );
			return false;
		}
		iDropped.SetCont( &droppedOn );
		iDropped.SetX( x );
		iDropped.SetY( y );
		iDropped.SetZ( 9 );
		iDropped.SetGridLocation( gridLoc );
	}
	else
	{
		if( &droppedOn != iDropped.GetCont() && !Weight->checkPackWeight( &mChar, &droppedOn, &iDropped ) )
		{
			if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( &mChar, &iDropped );
			mSock.sysmessage( 1385 );
			Bounce( &mSock, &iDropped );
			return false;
		}
		iDropped.SetCont( &droppedOn );
		stackDeleted = ( autoStack( &mSock, &iDropped, &droppedOn ) != &iDropped );
		if( !stackDeleted )
		{
			if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( &mChar, &iDropped );
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  DropOnItem( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item is dropped in a container or on another item
//o---------------------------------------------------------------------------o
void DropOnItem( CSocket *mSock, SERIAL item, SERIAL dest, SI16 x, SI16 y, SI08 z, SI08 gridLoc )
{
	CChar *mChar = mSock->CurrcharObj();
	CItem *nCont = calcItemObjFromSer( dest );
	if( !ValidateObject( nCont ) || !ValidateObject( mChar ) )
		return;

	CItem *nItem = calcItemObjFromSer( item );
	if( !ValidateObject( nItem ) )
		return;

	bool executeCont	= true;
	UI16 targTrig		= nItem->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	if( toExecute != NULL )
	{
		UI08 rVal = toExecute->OnDropItemOnItem( nItem, mChar, nCont );	// returns 1 if we should bounce it
		switch( rVal )
		{
			case 0:	// no such event
			default:
				executeCont = true;
				break;
			case 1:	// bounce, no code
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					Weight->subtractItemWeight( mChar, nItem );
				Bounce( mSock, nItem );
				return;	// stack not deleted, as we're bouncing
			case 2:	// don't bounce, use code
				executeCont = false;
				break;
			case 3:	// don't bounce, don't use code
				executeCont = true;
				break;
		}
	}
	if( executeCont )
	{
		targTrig			= nCont->GetScriptTrigger();
		toExecute	= JSMapping->GetScript( targTrig );
		if( toExecute != NULL )
		{
			UI08 rVal = toExecute->OnDropItemOnItem( nItem, mChar, nCont );	// returns 1 if we should bounce it
			switch( rVal )
			{
				case 2:	// don't bounce, use code
				case 0:	// no such event
				default:
					break;
				case 1:	// bounce, no code
					if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
						Weight->subtractItemWeight( mChar, nItem );
					Bounce( mSock, nItem );
					return;	// stack not deleted, as we're bouncing
				case 3:	// don't bounce, don't use code
					return;
			}
		}
	}

	bool stackDeleted = false;

	if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
	{
		//7.0.9.0 data and later
		CTileHS& tile = Map->SeekTileHS( nItem->GetID() );
		if( !mChar->AllMove() && ( nItem->GetMovable() == 2 || ( nItem->IsLockedDown() && nItem->GetOwnerObj() != mChar ) ||
			( tile.Weight() == 255 && nItem->GetMovable() != 1 ) ) )
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( mChar, nItem );
			Bounce( mSock, nItem );
			return;
		}
	}
	else
	{
		//7.0.8.2 data and earlier
		CTile& tile = Map->SeekTile( nItem->GetID() );
		if( !mChar->AllMove() && ( nItem->GetMovable() == 2 || ( nItem->IsLockedDown() && nItem->GetOwnerObj() != mChar ) ||
			( tile.Weight() == 255 && nItem->GetMovable() != 1 ) ) )
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( mChar, nItem );
			Bounce( mSock, nItem );
			return;
		}
	}

	if( nCont->GetType() == IT_TRADEWINDOW && FindItemOwner( nCont ) == mChar )	// Trade window
		DropOnTradeWindow( (*mSock), (*mChar), (*nCont), (*nItem), x, y, gridLoc );
	else if( nCont->GetType() == IT_TRASHCONT )	// Trash container
	{
		Effects->PlaySound( mSock, 0x0042, false );
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( mChar, nItem );
		nItem->Delete();
		mSock->sysmessage( 1201 );
		return;
	}
	else if( nCont->GetType() == IT_SPELLBOOK )	// Spell Book
	{
		DropOnSpellBook( (*mSock), (*mChar), (*nCont), (*nItem) );
		return;
	}
	else if( nCont->GetType() == IT_CONTAINER )
	{
		if( !DropOnContainer( (*mSock), (*mChar), (*nCont), (*nItem), stackDeleted, x, y, gridLoc ) )
			return;
	}
	else if( nCont->isPileable() && nItem->isPileable() && nCont->GetID() == nItem->GetID() && nCont->GetColour() == nItem->GetColour() )
	{	// Stacking
		if( !DropOnStack( (*mSock), (*mChar), (*nCont), (*nItem), stackDeleted ) )
			return;
	}
	else
	{
		if( nCont->GetType() == IT_SPAWNCONT || nCont->GetType() == IT_UNLOCKABLESPAWNCONT ) // - Unlocked item spawner or unlockable item spawner
			nItem->SetCont( nCont );
		else
			nItem->SetCont( nCont->GetCont() );

		nItem->SetX( x );
		nItem->SetY( y );
		nItem->SetZ( z );
		nItem->SetGridLocation( gridLoc );

		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( mChar, nItem );
	}
	if( !stackDeleted )
		Effects->itemSound( mSock, nItem, ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) );

	//Would be nice to do this to instantly update the stats & weight on the container the item is dropped on
	// but unfortunately it seems like updating the container will also force-close it :/
	// nCont->Dirty( UT_UPDATE );
}

bool CPIDropItem::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	ourChar->BreakConcentration( tSock );

	if( dest >= BASEITEMSERIAL && dest != INVALIDSERIAL )
		DropOnItem( tSock, item, dest, x, y, z, gridLoc );
	else 
		Drop( tSock, item, dest, x, y, z, gridLoc );

	// Display overloaded message if character is overloaded as a result of the above
	if( Weight->isOverloaded( ourChar ) )
	{
		SI32 maxWeight = ourChar->GetStrength() * cwmWorldState->ServerData()->WeightPerStr() + 40;
		SI32 currentWeight = ourChar->GetWeight() / 100;
		tSock->sysmessage( 1784, currentWeight, maxWeight ); //You are overloaded. Current / Max: %i / %i
	}

	//Reset PickupSpot after dropping the item
	tSock->PickupSpot( PL_NOWHERE );
	tSock->SetCursorItem( NULL );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI08 BestSkill( CChar *mChar, SKILLVAL &skillLevel )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Return characters highest skill
//o---------------------------------------------------------------------------o
UI08 BestSkill( CChar *mChar, SKILLVAL &skillLevel )
{
	UI08 retSkill = 0;
	std::vector< cSkillClass > vecSkills;
	for( UI08 sCtr = 0 ; sCtr < ALLSKILLS; ++sCtr )
		vecSkills.push_back( cSkillClass( sCtr, mChar->GetBaseSkill( sCtr ) ) );

	std::sort( vecSkills.rbegin(), vecSkills.rend() );
	if( vecSkills[0].value > 0 )
	{
		retSkill	= vecSkills[0].skill;
		skillLevel	= vecSkills[0].value;
	}
	return retSkill;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void getSkillProwessTitle( CChar *p, std::string& prowessTitle )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Fetches the characters "prowess" and "skill" title based upon titles.dfn
//|                 entries and characters best skill
//o---------------------------------------------------------------------------o
void getSkillProwessTitle( CChar *mChar, std::string &SkillProwessTitle )
{
	if( cwmWorldState->prowessTitles.empty() )
		return;
	SKILLVAL skillLevel = 0;
	UI08 bestSkill = BestSkill( mChar, skillLevel );
	if( skillLevel <= cwmWorldState->prowessTitles[0].lowBound )
		SkillProwessTitle = cwmWorldState->prowessTitles[0].toDisplay;
	else
	{
		size_t pEntry = 0;
		for( pEntry = 0; pEntry < cwmWorldState->prowessTitles.size() - 1; ++pEntry )
		{
			if( skillLevel >= cwmWorldState->prowessTitles[pEntry].lowBound && skillLevel < cwmWorldState->prowessTitles[pEntry+1].lowBound )
				break;
		}
		SkillProwessTitle = cwmWorldState->prowessTitles[pEntry].toDisplay;
	}
	SkillProwessTitle += " " + cwmWorldState->title[static_cast<UI08>(bestSkill + 1)].skill;
}

//o---------------------------------------------------------------------------o
//|	Function	-	getFameTitle( CChar *p, std::string& Fametitle )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns players reputation title based on their Fame and Karma
//o---------------------------------------------------------------------------o
void getFameTitle( CChar *p, std::string& FameTitle )
{
	if( ValidateObject( p ) )
	{
		UString thetitle;
		UI08 titlenum = 0xFF;

		SI16 k = p->GetKarma();
		SI16 f = p->GetFame();

		if( k >= 10000 )
		{
			if( f >= 5000 )
				titlenum = 0;
			else if( f >= 2500 )
				titlenum = 1;
			else if( f >= 1250 )
				titlenum = 2;
			else
				titlenum = 3;
		}
		else if( k >= 5000 )
		{
			if( f >= 5000 )
				titlenum = 4;
			else if( f >= 2500 )
				titlenum = 5;
			else if( f >= 1250 )
				titlenum = 6;
			else
				titlenum = 7;
		}
		else if( k >= 2500 )
		{
			if( f >= 5000 )
				titlenum = 8;
			else if( f >= 2500 )
				titlenum = 9;
			else if( f >= 1250 )
				titlenum = 10;
			else
				titlenum = 11;
		}
		else if( k >= 1250 )
		{
			if( f >= 5000 )
				titlenum = 12;
			else if( f >= 2500 )
				titlenum = 13;
			else if( f >= 1250 )
				titlenum = 14;
			else
				titlenum = 15;
		}
		else if( k >= 625 )
		{
			if( f >= 5000 )
				titlenum = 16;
			else if( f >= 1000 )
				titlenum = 17;
			else if( f >= 500 )
				titlenum = 18;
			else
				titlenum = 19;
		}
		else if( k > -625 )
		{
			if( f >= 5000 )
				titlenum = 20;
			else if( f >= 2500 )
				titlenum = 21;
			else if( f >= 1250 )
				titlenum = 22;
			else
				titlenum = 23;
		}
		else if( k > -1250 )
		{
			if( f >= 10000 )
				titlenum = 28;
			else if( f >= 5000 )
				titlenum = 27;
			else if( f >= 2500 )
				titlenum = 26;
			else if( f >= 1250 )
				titlenum = 25;
			else
				titlenum = 24;
		}
		else if( k > -2500 )
		{
			if( f >= 5000 )
				titlenum = 32;
			else if( f >= 2500 )
				titlenum = 31;
			else if( f >= 1250 )
				titlenum = 30;
			else
				titlenum = 29;
		}
		else if( k > -5000 )
		{
			if( f >= 10000 )
				titlenum = 37;
			else if( f >= 5000 )
				titlenum = 36;
			else if( f >= 2500 )
				titlenum = 35;
			else if( f >= 1250 )
				titlenum = 34;
			else
				titlenum = 33;
		}
		else if( k > -10000 )
		{
			if( f >= 5000 )
				titlenum = 41;
			else if( f >= 2500 )
				titlenum = 40;
			else if( f >= 1250 )
				titlenum = 39;
			else
				titlenum = 38;
		}
		else if( k <= -10000 )
		{
			if( f >= 5000 )
				titlenum = 45;
			else if( f >= 2500 )
				titlenum = 44;
			else if( f >= 1250 )
				titlenum = 43;
			else
				titlenum = 42;
		}
		if( !cwmWorldState->title[titlenum].fame.empty() )
			thetitle = cwmWorldState->title[titlenum].fame + " ";

		if( p->GetRace() != 0 && p->GetRace() != 65535 )
			thetitle = thetitle + Races->Name( p->GetRace() ) + " ";

		if( f >= 10000 ) // Morollans bugfix for repsys
		{
			if( p->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
			{
				if( p->GetID( 2 ) == 0x91 )
					FameTitle = UString::sprintf( Dictionary->GetEntry( 1177 ).c_str(), Races->Name( p->GetRace() ).c_str() ) + " ";
				else
					FameTitle = UString::sprintf( Dictionary->GetEntry( 1178 ).c_str(), Races->Name( p->GetRace() ).c_str() ) + " ";
			}
			else if( p->GetID( 2 ) == 0x91 )
				FameTitle = UString::sprintf( Dictionary->GetEntry( 1179 ).c_str(), thetitle.c_str() ) + " ";
			else
				FameTitle = UString::sprintf( Dictionary->GetEntry( 1180 ).c_str(), thetitle.c_str() ) + " ";
		}
		else
		{
			if( p->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
				FameTitle = Dictionary->GetEntry( 1181 ) + " ";
			else if( !thetitle.stripWhiteSpace().empty() )
				FameTitle = UString::sprintf( Dictionary->GetEntry( 1182 ).c_str(), thetitle.c_str() );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  PaperDoll( CSocket *s, CChar *pdoll )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Open a characters paperdoll and show titles based on skill,
//|					 reputation, murder counts, race, ect.
//o---------------------------------------------------------------------------o
void PaperDoll( CSocket *s, CChar *pdoll )
{
	CChar *myChar	= s->CurrcharObj();
	UString tempstr;
	CPPaperdoll pd		= (*pdoll);
	UnicodeTypes sLang	= s->Language();

	// Check if onCharDoubleClick event exists
	UI16 charTrig		= pdoll->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( charTrig );
	if( toExecute != NULL )
	{
		if( toExecute->OnCharDoubleClick( myChar, pdoll ) == 1 )	// if it exists and we don't want hard code, return
			return;
	}

	UString SkillProwessTitle;
	UString FameTitle;
	getSkillProwessTitle( pdoll, SkillProwessTitle );
	getFameTitle( pdoll, FameTitle );

	bool bContinue = false;
	if( pdoll->IsGM() )
		tempstr = pdoll->GetName() + " " + pdoll->GetTitle();
	else if( pdoll->IsNpc() )
		tempstr = FameTitle + pdoll->GetName() + " " + pdoll->GetTitle();
	else if( pdoll->IsDead() )
		tempstr = pdoll->GetName();
	// Murder tags now scriptable in SECTION MURDERER - Titles.scp - Thanks Ab - Zane
	else if( pdoll->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
	{
		if( cwmWorldState->murdererTags.empty() )
			tempstr = UString::sprintf( Dictionary->GetEntry( 374, sLang ).c_str(), pdoll->GetName().c_str(), pdoll->GetTitle().c_str(), SkillProwessTitle.c_str() );
		else if( pdoll->GetKills() < cwmWorldState->murdererTags[0].lowBound )	// not a real murderer
			bContinue = true;
		else
		{
			SI16 mKills = pdoll->GetKills();
			size_t kCtr;
			for( kCtr = 0; kCtr < cwmWorldState->murdererTags.size() - 1; ++kCtr )
			{
				if( mKills >= cwmWorldState->murdererTags[kCtr].lowBound && mKills < cwmWorldState->murdererTags[kCtr+1].lowBound )
					break;
			}
			if( kCtr >= cwmWorldState->murdererTags.size() )
				bContinue = true;
			else
				tempstr = cwmWorldState->murdererTags[kCtr].toDisplay + " " + pdoll->GetName() + ", " + pdoll->GetTitle() + SkillProwessTitle;
		}
	}
	else if( pdoll->IsCriminal() )
		tempstr = UString::sprintf( Dictionary->GetEntry( 373, sLang ).c_str(), pdoll->GetName().c_str(), pdoll->GetTitle().c_str(), SkillProwessTitle.c_str() );
	else
		bContinue = true;
	if( bContinue )
	{
		tempstr = FameTitle + pdoll->GetName();	//Repuation + Name
		if( pdoll->GetTownTitle() || pdoll->GetTownPriv() == 2 )	// TownTitle
		{
			if( pdoll->GetTownPriv() == 2 )	// is Mayor
				tempstr = UString::sprintf( Dictionary->GetEntry( 379, sLang ).c_str(), pdoll->GetName().c_str(), cwmWorldState->townRegions[pdoll->GetTown()]->GetName().c_str(), SkillProwessTitle.c_str() );
			else	// is Resident
				tempstr = pdoll->GetName() + " of " + cwmWorldState->townRegions[pdoll->GetTown()]->GetName() + ", " + SkillProwessTitle;
		}
		else	// No Town Title
		{
			if( !pdoll->IsIncognito() && !( pdoll->GetTitle().empty() ) )	// Titled & Skill
				tempstr += " " + pdoll->GetTitle() + ", " + SkillProwessTitle;
			else	// Just skilled
				tempstr += ", " + SkillProwessTitle;
		}
	}
	pd.Text( tempstr );

	CChar *mChar = s->CurrcharObj();
	if( ValidateObject( mChar ) && ( mChar == pdoll ))
		pd.FlagByte( 0x02 | ( mChar->IsAtWar() ? 0x01 : 0x00 ));
	else if( ValidateObject( mChar ) && mChar->IsGM() )
		pd.FlagByte( 0x02 );
	
	s->Send( &pd );

	for( CItem *wearItem = pdoll->FirstItem(); !pdoll->FinishedItems(); wearItem = pdoll->NextItem() )
	{
		if( ValidateObject( wearItem ) )
		{
			CPToolTip pSend( wearItem->GetSerial() );
			s->Send( &pSend );
		}
	}
}

void MountCreature( CSocket *mSock, CChar *s, CChar *x );
//o---------------------------------------------------------------------------o
//|   Function    :  handleCharDoubleClick( CSocket *mSock, SERIAL serial, bool keyboard )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles double-clicks on a character
//o---------------------------------------------------------------------------o
void handleCharDoubleClick( CSocket *mSock, SERIAL serial, bool keyboard )
{
	CChar *mChar	= mSock->CurrcharObj();
	CChar *c		= calcCharObjFromSer( serial );
	if( !ValidateObject( c ) )
		return;

	// Check if onCharDoubleClick event exists
	UI16 charTrig		= c->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( charTrig );
	if( toExecute != NULL )
	{
		if( toExecute->OnCharDoubleClick( mChar, c ) == 1 )	// if it exists and we don't want hard code, return
			return;
	}

	if( c->IsNpc() )
	{
		CItem *pack		= NULL;
		if( cwmWorldState->creatures[c->GetID()].MountID() != 0 )	// Is a mount
		{
			if( ( c->IsTamed() && ( c->GetOwnerObj() == mChar || Npcs->checkPetFriend( mChar, c ) ) ) || mChar->GetCommandLevel() >= CL_GM )
			{
				if( objInRange( mChar, c, DIST_NEXTTILE ) )
				{
					if( mChar->IsDead() )
						mSock->sysmessage( 381 );
					else if( mChar->GetID() != mChar->GetOrgID() )
						mSock->sysmessage( 380 );
					else
						MountCreature( mSock, mChar, c );
				}
				else
					mSock->sysmessage( 382 );
			}
			else
				mSock->sysmessage( 1214 );
			return; 
		}
		else if( !cwmWorldState->creatures[c->GetID()].IsHuman() && !c->IsDead() )
		{
			if( c->GetID() == 0x0123 || c->GetID() == 0x0124 )	// Is a pack animal
			{
				if( mChar->IsDead() )
					mSock->sysmessage( 392 );
				else if( !objInRange( mChar, c, DIST_NEARBY ) )
					mSock->sysmessage( 382 );
				else
				{
					pack = c->GetPackItem();
					if( mChar->GetCommandLevel() >= CL_CNS || c->GetOwnerObj() == mChar || Npcs->checkPetFriend( mChar, c ) )
					{
						if( ValidateObject( pack ) )
							mSock->openPack( pack );
						else
							Console << "Pack animal " << c->GetSerial() << " has no backpack!" << myendl;
					}
					else
					{
						Skills->Snooping( mSock, c, pack );
						mSock->sysmessage( 383 );
					}
				}
				return;
			}
			return; 
		}
		else if( c->GetNPCAiType() == AI_PLAYERVENDOR ) // PlayerVendors
		{
			c->TextMessage( mSock, 385, TALK, false );
			pack = c->GetPackItem();
			if( ValidateObject( pack ) )
				mSock->openPack( pack, true );
			return;
		}
	}
	else
	{
		if( mChar->GetSerial() == serial )	// Double-clicked yourself
		{
			if( mChar->IsOnHorse() && !keyboard )
			{
				DismountCreature( mChar );	// If on horse, dismount
				return;
			}
		}
	}
	PaperDoll( mSock, c );
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool handleDoubleClickTypes( CSocket *mSock, CChar *mChar, CItem *x, ItemTypes iType )
//|   Date        :  2/11/2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Runs a switch to match an item type to a function
//o---------------------------------------------------------------------------o
bool handleDoubleClickTypes( CSocket *mSock, CChar *mChar, CItem *iUsed, ItemTypes iType )
{
	CChar *iChar	= NULL;
	CItem *i		= NULL;
	UI16 itemID		= iUsed->GetID();
	bool canTrap	= false;

	// Begin Check items by type
	switch( iType )
	{
		case IT_NOTYPE:
		case IT_COUNT:
			return false;
		case IT_CONTAINER:	// Container, Backpack
		case IT_SPAWNCONT: // Item spawn container
			canTrap = true;
		case IT_UNLOCKABLESPAWNCONT:	// Unlockable item spawn container
		case IT_TRASHCONT:	// Trash container
			bool packOpened;
			packOpened = false;
			CBaseObject *baseCont;
			ObjectType objType;
			baseCont = FindItemOwner( iUsed, objType );
			if( !ValidateObject( baseCont ) )
				baseCont = iUsed;
			if( ValidateObject( baseCont ) )
			{
				if( baseCont->CanBeObjType( OT_ITEM ) )
				{
					if( baseCont->GetMultiObj() == NULL || mChar->GetMultiObj() == baseCont->GetMultiObj() )
					{
						mSock->openPack( iUsed );
						packOpened = true;
					}
				}
				else
				{
					iChar = static_cast<CChar *>(baseCont);
					if( ValidateObject( iChar ) )
					{
						if( mChar == iChar || mChar == iChar->GetOwnerObj() )
						{
							if( iChar->GetNPCAiType() == AI_PLAYERVENDOR ) // PlayerVendors
								mSock->openPack( iUsed, true );
							else
							{
								if( mChar->GetPackItem() == iUsed )
									mChar->SetWeight( Weight->calcCharWeight( mChar ));
								mSock->openPack( iUsed );
							}
						}
						else if( iChar->GetNPCAiType() == AI_PLAYERVENDOR ) // PlayerVendors
							mSock->openPack( iUsed, true );
						else
						{
							CItem *rootItem = FindRootContainer( iUsed );
							if( ValidateObject( rootItem ) && rootItem->GetType() == IT_TRADEWINDOW )
								mSock->openPack( iUsed, true );
							else
								Skills->Snooping( mSock, iChar, iUsed );
						}
						packOpened = true;	
					}
						
				}
			}
			if( packOpened )
			{
				if( canTrap && iUsed->GetTempVar( CITV_MOREZ, 1 ))// Is trapped
					Magic->MagicTrap( mChar, iUsed );
			}
			else
				mSock->sysmessage( 400 );
			return true;
		case IT_CASTLEGATEOPENER:	// Order gate opener
		case IT_CHAOSGATEOPENER:	// Chaos gate opener
			i = calcItemObjFromSer( iUsed->GetTempVar( CITV_MORE ) );
			if( ValidateObject( i ) )
			{
				if( i->GetType() == IT_CASTLEGATE )
				{
					if( i->GetTempVar( CITV_MOREZ ) == 1 )
					{
						i->SetTempVar( CITV_MOREZ, 2 );
						i->SetZ( i->GetZ() + 17 );
					}
					else if( i->GetTempVar( CITV_MOREZ ) == 2 )
					{
						i->SetTempVar( CITV_MOREZ, 1 );
						i->SetZ( i->GetZ() - 17 );
					}
				}
			}
			return true;
		case IT_KEY:	// Key
			mSock->AddID( iUsed->GetTempVar( CITV_MORE ) );
			mSock->target( 0, TARGET_KEY, 402 );
			return true;
		case IT_LOCKEDCONTAINER:	// Locked container
		case IT_LOCKEDSPAWNCONT: // locked spawn container
			mSock->sysmessage( 1428 );
			return true;
		case IT_SPELLBOOK:	// Spellbook
			i = mChar->GetPackItem();
			if( ValidateObject( i ) )
			{
				if( ( iUsed->GetCont() == i || iUsed->GetCont() == mChar ) || iUsed->GetLayer() == IL_RIGHTHAND )
					Magic->SpellBook( mSock );
				else
					mSock->sysmessage( 403 );
			}
			return true;
		case IT_MAP: // Map
			{
				CPMapMessage m1;
				CPMapRelated m2;
				m1.KeyUsed( iUsed->GetSerial() );
				m1.GumpArt( 0x139D );
				m1.UpperLeft( 0, 0 );
				m1.LowerRight( 0x13FF, 0x0FA0 );
				m1.Dimensions( 0x0190, 0x0190 );
				mSock->Send( &m1 );

				m2.ID( iUsed->GetSerial() );
				m2.Command( 5 );
				m2.Location( 0, 0 );
				m2.PlotState( 0 );
				mSock->Send( &m2 );
				return true;
			}
		case IT_READABLEBOOK:	// Readable book
			if( iUsed->GetTempVar( CITV_MOREX ) != 666 && iUsed->GetTempVar( CITV_MOREX ) != 999 )
				Books->OpenPreDefBook( mSock, iUsed );
			else
				Books->OpenBook( mSock, iUsed, ( iUsed->GetTempVar( CITV_MOREX ) == 666 ) ); 
			return true;
		case IT_MAGICWAND: // Magic Wands
			if( iUsed->GetTempVar( CITV_MOREZ ) != 0 )
			{
				mSock->CurrentSpellType( 2 );
				if( Magic->SelectSpell( mSock, ( 8 * ( iUsed->GetTempVar( CITV_MOREX ) - 1 ) ) + iUsed->GetTempVar( CITV_MOREY ) ) )
				{
					iUsed->SetTempVar( CITV_MOREZ, iUsed->GetTempVar( CITV_MOREZ ) - 1 );
					if( iUsed->GetTempVar( CITV_MOREZ ) == 0 )
					{
						iUsed->SetType( IT_NOTYPE );
						iUsed->SetTempVar( CITV_MOREX, 0 );
						iUsed->SetTempVar( CITV_MOREY, 0 );
						iUsed->SetOffSpell( 0 );
					}
					//Update item so player will see how many charges remain (if any)
					iUsed->Dirty( UT_UPDATE );
				}
			}
			return true;
		case IT_TOWNSTONE: // Townstone and Townstone Deed
			if( itemID == 0x14F0 )		// Check for Deed
			{
				Items->CreateScriptItem( NULL, mChar, "townstone", 1, OT_ITEM );
				iUsed->Delete();
			}
			else	// Display Townstone gump
			{
				CTownRegion *useRegion = calcRegionFromXY( iUsed->GetX(), iUsed->GetY(), mChar->WorldNumber() );
				if( useRegion != NULL )
					useRegion->DisplayTownMenu( iUsed, mSock );
			}
			return true;
		case IT_RECALLRUNE: // Recall Rune
			if( iUsed->GetTempVar( CITV_MOREX ) == 0 && iUsed->GetTempVar( CITV_MOREY ) == 0 && iUsed->GetTempVar( CITV_MOREZ ) == 0 )	// changed, to fix, Lord Vader
				mSock->sysmessage( 431 );
			else
			{
				mSock->sysmessage( 432 );
				mChar->SetSpeechItem( iUsed );
				mChar->SetSpeechMode( 7 );
			}
			return true;
		case IT_OBJTELEPORTER:	// Object Teleporter
			if( objInRange( mChar, iUsed, DIST_NEARBY ) )
				mChar->SetLocation( static_cast<SI16>(iUsed->GetTempVar( CITV_MOREX )), static_cast<SI16>(iUsed->GetTempVar( CITV_MOREY )), static_cast<SI08>(iUsed->GetTempVar( CITV_MOREZ ) ));
			return true;
		case IT_MAPCHANGEOBJECT:
			SocketMapChange( mSock, mChar, iUsed );
			return true;
		case IT_MORPHOBJECT: // Morph Object
			mChar->SetID( static_cast<UI16>(iUsed->GetTempVar( CITV_MOREX )) );
			iUsed->SetType( IT_UNMORPHOBJECT );
			return true;
		case IT_UNMORPHOBJECT: // Unmorph Object
			mChar->SetID( mChar->GetOrgID() );
			iUsed->SetType( IT_MORPHOBJECT );
			return true;
		case IT_PLANK:	// Planks
			if( objInRange( mChar, iUsed, DIST_INRANGE ) )
			{
				if( iUsed->GetID( 2 ) == 0x84 || iUsed->GetID( 2 ) == 0xD5 || iUsed->GetID( 2 ) == 0xD4 || iUsed->GetID( 2 ) == 0x89 )
					PlankStuff( mSock, iUsed );
				else
					mSock->sysmessage( 398 );//Locked
			}
			else
				mSock->sysmessage( 399 );
			return true;
		case IT_FIREWORKSWAND: //Fireworks wands
			srand( cwmWorldState->GetUICurrentTime() );
			if( iUsed->GetTempVar( CITV_MOREX ) == 0 )
			{
				mSock->sysmessage( 396 );
				return true;
			}
			iUsed->SetTempVar( CITV_MOREX, iUsed->GetTempVar( CITV_MOREX ) - 1 );
			mSock->sysmessage( 397, iUsed->GetTempVar( CITV_MOREX ) );
			UI08 j;
			for( j = 0; j < static_cast< UI08 >( RandomNum( 0, 3 ) + 2 ); ++j )
			{
				SI16 wx = ( mChar->GetX() + RandomNum( 0, 10 ) - 5 );
				SI16 wy = ( mChar->GetY() + RandomNum( 0, 10 ) - 5 );
				Effects->PlayMovingAnimation( mChar, wx, wy, mChar->GetZ() + 10, 0x36E4, 17, 0, ( RandomNum( 0, 1 ) == 1 ) );
				UI16 animID;
				switch( RandomNum( 0, 4 ) )
				{
					default:
					case 0:	animID = 0x373A;		break;
					case 1:	animID = 0x374A;		break;
					case 2:	animID = 0x375A;		break;
					case 3:	animID = 0x376A;		break;
					case 4:	animID = 0x377A;		break;
				}
				Effects->PlayStaticAnimation( wx, wy, mChar->GetZ() + 10, animID, 0x09, 0, 0 );
			}
			return true;
		case IT_RENAMEDEED: // Rename Deed
			mChar->SetSpeechItem( iUsed );
			mChar->SetSpeechMode( 6 );
			mSock->sysmessage( 434 );
			return true;
		case IT_TILLER:	// Tillerman
			if( ValidateObject( GetBoat( mSock ) ) )
			{
				CBoatObj *boat = static_cast<CBoatObj *>(iUsed->GetMultiObj());
				if( ValidateObject( boat ) )
					ModelBoat( mSock, boat );
			}
			return true;
		case IT_GUILDSTONE:	// Guildstone Deed
			if( itemID == 0x14F0 || itemID == 0x1869 )	// Check for Deed/Teleporter + Guild Type
			{
				mChar->SetSpeechItem( iUsed );		// we're using this as a generic item
				GuildSys->PlaceStone( mSock, iUsed );
				return true;
			}
			else if( itemID == 0x0ED5 )			// Check for Guildstone + Guild Type
			{
				mSock->TempInt( iUsed->GetTempVar( CITV_MORE ) );	// track things properly
				if( mChar->GetGuildNumber() == -1 || mChar->GetGuildNumber() == static_cast<SI16>(iUsed->GetTempVar( CITV_MORE )) )
					GuildSys->Menu( mSock, BasePage + 1, static_cast<GUILDID>(iUsed->GetTempVar( CITV_MORE )) );	// more of the stone is the guild number
				else
					mSock->sysmessage( 438 );
				return true;
			}
			else
				Console << "Unhandled guild item type named: " << iUsed->GetName() << " with ID of: " << itemID << myendl;
			return true;
		case IT_HOUSESIGN: // Open Housing Gump
			bool canOpen;
			canOpen = (  iUsed->GetOwnerObj() == mChar || mChar->IsGM() );
			if( !canOpen && iUsed->GetTempVar( CITV_MOREZ ) == 0 )
			{
				mSock->sysmessage( 439 );
				return true;
			}
			mSock->TempObj( iUsed );
			if( !canOpen )
				BuildGumpFromScripts( mSock, (UI16)iUsed->GetTempVar( CITV_MOREZ ) );
			else
				BuildGumpFromScripts( mSock, (UI16)iUsed->GetTempVar( CITV_MOREX ) );
			return true;
		case IT_METALREPAIRTOOL:
			mSock->target( 0, TARGET_REPAIRMETAL, 485 );	// What do we wish to repair?
			return true;
		case IT_FORGE:	// Forges
			mSock->target( 0, TARGET_SMELT, 440 );
			return true;
		case IT_DYE:	// Dye
			mSock->DyeAll( 0 );
			mSock->target( 0, TARGET_DYEALL, 441 );
			return true;
		case IT_DYEVAT:	// Dye vat
			mSock->AddID1( iUsed->GetColour( 1 ) );
			mSock->AddID2( iUsed->GetColour( 2 ) );
			mSock->target( 0, TARGET_DVAT, 442 );
			return true;
		case IT_MODELMULTI:	// Model boat/Houses
			if( iType != IT_TOWNSTONE && iType != IT_GUILDSTONE )
			{
				if( iUsed->GetTempVar( CITV_MOREX ) == 0 )
					break;
				mChar->SetSpeechItem( iUsed );
				DoHouseTarget( mSock, static_cast<UI08>(iUsed->GetTempVar( CITV_MOREX )) );
			}
			return true;
		case IT_PLAYERVENDORDEED:	//Player Vendor Deeds
			CChar *m;
			m = Npcs->CreateNPCxyz( "playervendor", mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->WorldNumber() );
			m->SetNPCAiType( AI_PLAYERVENDOR );
			m->SetInvulnerable( true );
			m->SetDir( mChar->GetDir() );
			m->SetOwner( mChar );
			iUsed->Delete();
			m->TextMessage( mSock, 388, TALK, false, m->GetName().c_str() );
			return true;
		case IT_SMITHYTOOL:
			mSock->target( 0, TARGET_SMITH, 444 );
			return true;
		case IT_MININGTOOL:	// Mining
			mSock->target( 0, TARGET_MINE, 446 );
			return true; 
		case IT_EMPTYVIAL:	// empty vial
			i = mChar->GetPackItem();
			if( ValidateObject( i ) )
			{
				if( iUsed->GetCont() == i )
				{
					mSock->TempObj( iUsed );
					mSock->target( 0, TARGET_VIAL, 447 );
				}
				else
					mSock->sysmessage( 448 );
			}
			return true;
		case IT_CANNONBALL:	// cannon ball
			mSock->target( 0, TARGET_LOADCANNON, 455 );
			iUsed->Delete();
			return true;
		case IT_ORE:	// smelt ore
			mSock->TempObj( iUsed );
			mSock->target( 0, TARGET_SMELTORE, 460 );
			return true;
		case IT_MESSAGEBOARD:	// Message board opening
			if( objInRange( mChar, iUsed, DIST_NEARBY ) )
			{
				CPIMsgBoardEvent *mbEvent = new CPIMsgBoardEvent( mSock, false );
				mbEvent->Handle();
			}
			else
				mSock->sysmessage( 461 );
			return true;
		case IT_MAGICSTATUE:
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				if( itemID == 0x1508 )
					iUsed->SetID( 0x1509 );
				else
					iUsed->SetID( 0x1508 );
			}
			else
				mSock->sysmessage( 465 );
			return true;
		case IT_GUILLOTINE:
		case IT_GUILLOTINEANIM:
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				if( itemID == 0x1245 )
					iUsed->SetID( 0x1230 );
				else
					iUsed->SetID( 0x1245 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case IT_FISHINGPOLE:	// fishing
			if( mSock->GetTimer( tPC_FISHING ) )
				mSock->sysmessage( 467 );
			else
				mSock->target( 0, TARGET_FISH, 468 );
			return true;
		case IT_SEXTANT:	// sextants
			mSock->sysmessage( 474, mChar->GetX(), mChar->GetY() );
			return true;
		case IT_HAIRDYE:	// Hair Dye
			mSock->TempObj( iUsed );
			BuildGumpFromScripts( mSock, 6 );
			return true;
		default:
			if( iType )
				Console << "Unhandled item type for item: " << iUsed->GetName() << "[" << iUsed->GetSerial() << "] of type: " << static_cast<UI16>(iType) << myendl;
			break;
	}
	return false;
}

std::map< std::string, ItemTypes > tagToItemType;

void InitTagToItemType( void )
{
	tagToItemType["CONTAINER"]				= IT_CONTAINER;
	tagToItemType["CASTLEGATEOPENER"]		= IT_CASTLEGATEOPENER;
	tagToItemType["CASTLEGATE"]				= IT_CASTLEGATE;
	tagToItemType["TELEPORTITEM"]			= IT_TELEPORTITEM;
	tagToItemType["KEY"]					= IT_KEY;
	tagToItemType["LOCKEDCONTAINER"]		= IT_LOCKEDCONTAINER;
	tagToItemType["SPELLBOOK"]				= IT_SPELLBOOK;
	tagToItemType["MAP"]					= IT_MAP;
	tagToItemType["READABLEBOOK"]			= IT_READABLEBOOK;
	tagToItemType["DOOR"]					= IT_DOOR;
	tagToItemType["LOCKEDDOOR"]				= IT_LOCKEDDOOR;
	tagToItemType["FOOD"]					= IT_FOOD;
	tagToItemType["MAGICWAND"]				= IT_MAGICWAND;
	tagToItemType["RESURRECTOBJECT"]		= IT_RESURRECTOBJECT;
	tagToItemType["CRYSTALBALL"]			= IT_CRYSTALBALL;
	tagToItemType["POTION"]					= IT_POTION;
	tagToItemType["TOWNSTONE"]				= IT_TOWNSTONE;
	tagToItemType["RECALLRUNE"]				= IT_RECALLRUNE;
	tagToItemType["GATE"]					= IT_GATE;
	tagToItemType["OBJTELEPORTER"]			= IT_OBJTELEPORTER;
	tagToItemType["ITEMSPAWNER"]			= IT_ITEMSPAWNER;
	tagToItemType["NPCSPAWNER"]				= IT_NPCSPAWNER;
	tagToItemType["SPAWNCONT"]				= IT_SPAWNCONT;
	tagToItemType["LOCKEDSPAWNCONT"]		= IT_LOCKEDSPAWNCONT;
	tagToItemType["UNLOCKABLESPAWNCONT"]	= IT_UNLOCKABLESPAWNCONT;
	tagToItemType["AREASPAWNER"]			= IT_AREASPAWNER;
	tagToItemType["ADVANCEGATE"]			= IT_ADVANCEGATE;
	tagToItemType["MULTIADVANCEGATE"]		= IT_MULTIADVANCEGATE;
	tagToItemType["MONSTERGATE"]			= IT_MONSTERGATE;
	tagToItemType["RACEGATE"]				= IT_RACEGATE;
	tagToItemType["DAMAGEOBJECT"]			= IT_DAMAGEOBJECT;
	tagToItemType["TRASHCONT"]				= IT_TRASHCONT;
	tagToItemType["SOUNDOBJECT"]			= IT_SOUNDOBJECT;
	tagToItemType["MAPCHANGEOBJECT"]		= IT_MAPCHANGEOBJECT;
	tagToItemType["WORLDCHANGEGATE"]		= IT_WORLDCHANGEGATE;
	tagToItemType["MORPHOBJECT"]			= IT_MORPHOBJECT;
	tagToItemType["UNMORPHOBJECT"]			= IT_UNMORPHOBJECT;
	tagToItemType["ZEROKILLSGATE"]			= IT_ZEROKILLSGATE;
	tagToItemType["PLANK"]					= IT_PLANK;
	tagToItemType["FIREWORKSWAND"]			= IT_FIREWORKSWAND;
	tagToItemType["ESCORTNPCSPAWNER"]		= IT_ESCORTNPCSPAWNER;
	tagToItemType["RENAMEDEED"]				= IT_RENAMEDEED;
	tagToItemType["TILLER"]					= IT_TILLER;
	tagToItemType["GUILDSTONE"]				= IT_GUILDSTONE;
	tagToItemType["HOUSESIGN"]				= IT_HOUSESIGN;
	tagToItemType["METALREPAIRTOOL"]		= IT_METALREPAIRTOOL;
	tagToItemType["FORGE"]					= IT_FORGE;
	tagToItemType["DYE"]					= IT_DYE;
	tagToItemType["DYEVAT"]					= IT_DYEVAT;
	tagToItemType["MODELMULTI"]				= IT_MODELMULTI;
	tagToItemType["PLAYERVENDORDEED"]		= IT_PLAYERVENDORDEED;
	tagToItemType["SMITHYTOOL"]				= IT_SMITHYTOOL;
	tagToItemType["MININGTOOL"]				= IT_MININGTOOL;
	tagToItemType["EMPTYVIAL"]				= IT_EMPTYVIAL;
	tagToItemType["CANNONBALL"]				= IT_CANNONBALL;
	tagToItemType["ORE"]					= IT_ORE;
	tagToItemType["MESSAGEBOARD"]			= IT_MESSAGEBOARD;
	tagToItemType["MAGICSTATUE"]			= IT_MAGICSTATUE;
	tagToItemType["GUILLOTINE"]				= IT_GUILLOTINE;
	tagToItemType["FISHINGPOLE"]			= IT_FISHINGPOLE;
	tagToItemType["CLOCK"]					= IT_CLOCK;
	tagToItemType["SEXTANT"]				= IT_SEXTANT;
	tagToItemType["HAIRDYE"]				= IT_HAIRDYE;
}

ItemTypes FindItemTypeFromTag( const UString& strToFind )
{
	if( tagToItemType.empty() )	// if we haven't built our array yet
		InitTagToItemType();
	std::map< std::string, ItemTypes >::const_iterator toFind = tagToItemType.find( strToFind.upper() );
	if( toFind != tagToItemType.end() )
		return toFind->second;
	return IT_COUNT;
}

std::map< UI16, ItemTypes > idToItemType;

void InitIDToItemType( void )
{
	ScriptSection *Itemtypes = FileLookup->FindEntry( "ITEMTYPES", items_def );
	if( Itemtypes == NULL )
		return;

	int sectionCount;
	UString data;
	ItemTypes iType = IT_COUNT;
	for( UString tag = Itemtypes->First(); !Itemtypes->AtEnd(); tag = Itemtypes->Next() )
	{
		data	= Itemtypes->GrabData();
		iType	= FindItemTypeFromTag( tag );
		if( iType != IT_COUNT )
		{
			sectionCount = data.sectionCount( "," );
			if( sectionCount != 0 )
			{
				for( int i = 0; i <= sectionCount; i++ )
				{
					idToItemType[data.section( ",", i, i ).toUShort( 0, 16 )] = iType;
				}
			}
			else
				idToItemType[data.toUShort( 0, 16 )] = iType;
		}
	}
}

ItemTypes FindItemTypeFromID( UI16 idToFind )
{
	if( idToItemType.empty() )	// if we haven't built our array yet
		InitIDToItemType();
	std::map< UI16, ItemTypes >::const_iterator toFind = idToItemType.find( idToFind );
	if( toFind != idToItemType.end() )
		return toFind->second;
	return IT_COUNT;
}

ItemTypes findItemType( CItem *i )
{
	ItemTypes iType = i->GetType();
	switch( iType )
	{
		case IT_NOTYPE:
		case IT_COUNT:
			iType = FindItemTypeFromID( i->GetID() );	break;
		default:										break;
	}
	return iType;
}

bool ItemIsUsable( CSocket *tSock, CChar *ourChar, CItem *iUsed, ItemTypes iType )
{
	if( ourChar->IsDead() && iType != IT_PLANK && iType != IT_HOUSESIGN )
	{
		if( iType == IT_RESURRECTOBJECT )	// Check for a resurrect item type
		{
			NpcResurrectTarget( ourChar );
			tSock->sysmessage( 390 );
		}
		else // If it's not a ressurect item, and you're dead, forget it
			tSock->sysmessage( 392 );
		return false;
	}
	// Range check for double clicking on items
	if( iType != IT_PLANK && iType != IT_HOUSESIGN )
	{
		bool canUse = checkItemRange( ourChar, iUsed );
		if( canUse )
			canUse = (iType == IT_DOOR || iType == IT_LOCKEDDOOR || checkItemLineOfSight( ourChar, iUsed ) );
		if( !canUse )
		{
			tSock->sysmessage( 389 );
			return false;
		}
	}
	if( ourChar->GetCommandLevel() < CL_CNS )
	{
		if( ( tSock->GetTimer( tPC_OBJDELAY ) >= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
		{
			tSock->sysmessage( 386 );
			return false;
		}
		tSock->SetTimer( tPC_OBJDELAY, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_OBJECTUSAGE ) );

		CChar *iChar = NULL;
		if( iUsed->GetCont() != NULL )
		{
			iChar = FindItemOwner( iUsed );
			if( ValidateObject( iChar ) && iChar != ourChar && 
				!( iUsed->IsContType() && cwmWorldState->ServerData()->RogueStatus() ) )
			{
				tSock->sysmessage( 387 );	// Can't use stuff that isn't in your pack.
				return false;
			}
		}
		// Check if item is in a house?
		else if( iType != IT_DOOR && iType != IT_LOCKEDDOOR && iType != IT_PLANK && iType != IT_HOUSESIGN )
		{
			if( iUsed->GetMultiObj() != NULL && iUsed->GetMultiObj() != ourChar->GetMultiObj() )
			{
				tSock->sysmessage( 389 );
				return false;
			}
		}
		if( iUsed->isCorpse() )
		{
			if( cwmWorldState->ServerData()->LootingIsCrime() )
			{
				bool willCrim	= false;
				iChar			= iUsed->GetOwnerObj();
				if( ValidateObject( iChar ) )
				{
					// if the corpse is from an innocent player, and is not our own corpse				if( otherCheck
					// and if the corpse is not from an enemy/allied guild									&& guildCheck
					// and if the races are not allied/enemy												&& raceCheck )
					willCrim = WillResultInCriminal( ourChar, iChar );
				}
				else
					willCrim = ( (iUsed->GetTempVar( CITV_MOREZ )&0x04) == 0x04 );
				if( willCrim )
					criminal( ourChar );
			}

			if( ValidateObject( iChar ) )
			{
				if( iChar->IsGuarded() ) // Is the corpse being guarded?
					Combat->petGuardAttack( ourChar, iChar, iChar );
				else if( iUsed->isGuarded() )
					Combat->petGuardAttack( ourChar, iChar, iUsed );
			}
		}
		else if( iUsed->isGuarded() )
		{
			CMultiObj *multi = iUsed->GetMultiObj();
			if( ValidateObject( multi ) )
			{
				CChar *multiOwner = multi->GetOwnerObj();
				if( ValidateObject( multiOwner ) && multiOwner != ourChar )
					Combat->petGuardAttack( ourChar, multiOwner, iUsed );
			}
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	:  DoubleClick( CSocket *mSock )
//|	Date		:  Unknown
//|	Programmer	:  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		:	Player double clicks on a character or item
//|	Modification:	09/22/2002	-	Xuri - Removed piece of code which is not 
//|									needed. Cooking of raw fish shouldn't produce cooked ribs,
//|									people can cook the fish after filleting with a knife.
//o---------------------------------------------------------------------------o
bool CPIDblClick::Handle( void )
{
	CChar *ourChar	= tSock->CurrcharObj();
	ourChar->BreakConcentration( tSock );
	UI08 a1			= tSock->GetByte( 1 );

	bool keyboard = ( (a1 & 0x80) == 0x80 );
	if( keyboard )
		objectID -= 0x80000000;

	if( objectID < BASEITEMSERIAL )
	{
		handleCharDoubleClick( tSock, objectID, keyboard );
		return true;
	}

	// Begin Items / Guildstones Section
	CItem *iUsed = calcItemObjFromSer( objectID );
	if( !ValidateObject( iUsed ) )
		return true;

	ItemTypes iType		= findItemType( iUsed );

	UI16 itemID			= iUsed->GetID();
	UI16 itemTrig		= iUsed->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( itemTrig );
	if( itemTrig == 0 || toExecute == NULL )
	{
		UI16 envTrig		= 0;
		if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>(iType) ) )
		{
			envTrig = JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>(iType) );
			toExecute = JSMapping->GetScript( envTrig );
		}
		else if( JSMapping->GetEnvokeByID()->Check( itemID ) )
		{
			envTrig = JSMapping->GetEnvokeByID()->GetScript( itemID );
			toExecute = JSMapping->GetScript( envTrig );
		}
	}

	if( toExecute != NULL )
	{
		if( toExecute->OnUseUnChecked( ourChar, iUsed ) == 1 )	// if it exists and we don't want hard code, return
			return true;
	}

	if( !ItemIsUsable( tSock, ourChar, iUsed, iType ) )
		return true;

	if( toExecute != NULL )
	{
		if( iUsed->isDisabled() )
		{
			tSock->sysmessage( 394 );
			return true;
		}
		if( toExecute->OnUseChecked( ourChar, iUsed ) == 1 )	// if it exists and we don't want hard code, return
			return true;
	}


	if( handleDoubleClickTypes( tSock, ourChar, iUsed, iType ) )
		return true;

	//	Begin Scrolls check
	//if( iUsed->GetID( 1 ) == 0x1F && ( iUsed->GetID( 2 ) > 0x2C && iUsed->GetID( 2 ) < 0x6D ) )
	//{
	//	bool success = false;
	//	tSock->CurrentSpellType( 1 );	// spell from scroll
	//	if( iUsed->GetID( 2 ) == 0x2D )	// Reactive Armor spell scrolls
	//		success = Magic->SelectSpell( tSock, 7 );
	//	if( iUsed->GetID( 2 ) >= 0x2E && iUsed->GetID( 2 ) <= 0x34 )  // first circle spell scrolls
	//		success = Magic->SelectSpell( tSock, iUsed->GetID( 2 ) - 0x2D );
	//	else if( iUsed->GetID( 2 ) >= 0x35 && iUsed->GetID( 2 ) <= 0x6C )  // 2 to 8 circle spell scrolls
	//		success = Magic->SelectSpell( tSock, iUsed->GetID( 2 ) - 0x2D + 1 );

	//	if( success )
	//		iUsed->IncAmount( -1 );
	//	return true;
	//}
	tSock->sysmessage( 486 );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  char *AppendData( CItem *i, std::string currentName )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Add data onto the end of the string in singleclick() based
//|					 on an items type
//o---------------------------------------------------------------------------o
const char *AppendData( CItem *i, std::string currentName )
{
	UString dataToAdd;
	switch( i->GetType() )
	{
		case IT_CONTAINER:
		case IT_SPAWNCONT:
		case IT_UNLOCKABLESPAWNCONT:
			dataToAdd = " (" + UString::number( i->GetContainsList()->Num() ) + " items, ";
			dataToAdd += UString::number( ( i->GetWeight() / 100 ) ) + " stones)";
			break;
		case IT_LOCKEDCONTAINER:		// containers
		case IT_LOCKEDSPAWNCONT:	// spawn containers
			dataToAdd = " (" + UString::number( i->GetContainsList()->Num() ) + " items, ";
			dataToAdd += UString::number( ( i->GetWeight() / 100 ) ) + " stones) [Locked]";
			break;
		case IT_LOCKEDDOOR:
			dataToAdd = " [Locked]";
			break;
		case IT_RECALLRUNE:
		case IT_GATE:
		case IT_OBJTELEPORTER:
		{
			CTownRegion *newRegion = calcRegionFromXY( static_cast<SI16>(i->GetTempVar( CITV_MOREX )), static_cast<SI16>(i->GetTempVar( CITV_MOREY )), i->WorldNumber() );
			dataToAdd = " (" + newRegion->GetName() + ")";
			break;
		}
	}
	currentName += dataToAdd;
	// Question: Do we put the creator thing here, saves some redundancy a bit later
	return currentName.c_str();
}

//o---------------------------------------------------------------------------o
//|   Function    :  SingleClick( CSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item or character is single-clicked (also
//|					 used for AllNames macro)
//o---------------------------------------------------------------------------o
bool CPISingleClick::Handle( void )
{
	if( objectID == INVALIDSERIAL ) // invalid
		return true;
	else if( objectID < BASEITEMSERIAL )
	{
		// Begin chars/npcs section
		CChar *c = calcCharObjFromSer( objectID );
		if( ValidateObject( c ) )
			tSock->ShowCharName( c, false );
		//End chars/npcs section
		return true;
	}

	char temp2[200];
	std::string realname;

	CChar *mChar = tSock->CurrcharObj();

	//Begin items section
	UI08 a1		= tSock->GetByte( 1 );
	UI08 a2		= tSock->GetByte( 2 );
	UI08 a3		= tSock->GetByte( 3 );
	UI08 a4		= tSock->GetByte( 4 );
	CItem *i	= calcItemObjFromSer( objectID );
	if( !ValidateObject( i ) )		// invalid item
		return true;
	// October 6, 2002 - Brakhtus - Added support for the onClick event
	cScript *onClickScp = JSMapping->GetScript( i->GetScriptTrigger() );
	if( onClickScp != NULL )
		onClickScp->OnClick( tSock, i );

	if( mChar->GetSingClickSer() )
		tSock->objMessage( 1737, i, 0.0f, 0x03B2, a1, a2, a3, a4 );

	UI16 getAmount = i->GetAmount();
	if( i->GetCont() != NULL && i->GetContSerial() >= BASEITEMSERIAL )
	{
		CChar *w = FindItemOwner( static_cast<CItem *>( i->GetCont() ));
		if( ValidateObject( w ) )
		{
			if( w->GetNPCAiType() == AI_PLAYERVENDOR )
			{
				char temp[512];
				if( i->GetCreator() != INVALIDSERIAL && i->GetMadeWith() > 0 )
				{
					CChar *mCreater = calcCharObjFromSer( i->GetCreator() );
					if( ValidateObject( mCreater ) )
						sprintf( temp2, "%s %s by %s", i->GetDesc().c_str(), cwmWorldState->skill[i->GetMadeWith()-1].madeword.c_str(), mCreater->GetName().c_str() );
					else
						strcpy( temp2, i->GetDesc().c_str() );
				}
				else
					strcpy( temp2, i->GetDesc().c_str() );
				sprintf( temp, "%s at %lugp", temp2, i->GetBuyValue() );
				tSock->objMessage( AppendData( i, temp ), i );
				return true;
			}
		}
	}

#if defined( _MSC_VER )
#pragma todo( "We need to update this to use getTileName almost exclusively, for plurality" )
#endif
	if( i->GetName()[0] != '#' )
	{
		if( i->GetID() == 0x0ED5 )//guildstone
			realname = UString::sprintf( Dictionary->GetEntry( 101, tSock->Language() ).c_str(), i->GetName().c_str() );
		if( !i->isPileable() || getAmount == 1 )
		{
			if( mChar->IsGM() && !i->isCorpse() && getAmount > 1 )
				realname = UString::sprintf( "%s (%u)", i->GetName().c_str(), getAmount );
			else
				realname = i->GetName();
		}
		else
			realname = UString::sprintf( "%u %ss", getAmount, i->GetName().c_str() );
	}
	else
	{
		getTileName( (*i), realname );
		if( i->GetAmount() > 1 )
			realname = UString::number( getAmount ) + " " + realname;
	}

	if( i->GetType() == IT_MAGICWAND )
	{
		int spellNum = ( 8 * ( i->GetTempVar( CITV_MOREX ) - 1 ) ) + i->GetTempVar( CITV_MOREY ) - 1;	// we pick it up from the array anyway
		realname += " of ";
		realname += Dictionary->GetEntry( magic_table[spellNum].spell_name, tSock->Language() );
		realname += " with ";
		realname += UString::number( i->GetTempVar( CITV_MOREZ ) );
		realname += " charges";
	}
	else if( i->IsContType() )
	{
		realname += UString::sprintf( ", (%u items, %u stones)", i->GetContainsList()->Num(), (i->GetWeight()/100) );
	}
	if( i->GetCreator() != INVALIDSERIAL && i->GetMadeWith() > 0 )
	{
		CChar *mCreater = calcCharObjFromSer( i->GetCreator() );
		if( ValidateObject( mCreater ) )
			sprintf( temp2, "%s %s by %s", realname.c_str(), cwmWorldState->skill[i->GetMadeWith()-1].madeword.c_str(), mCreater->GetName().c_str() );
		else
			strcpy( temp2, realname.c_str() );
	}
	else
		strcpy( temp2, realname.c_str() );
	tSock->objMessage( temp2, i );
	if( i->IsLockedDown() )
		tSock->objMessage( "[Locked down]", i );
	if( i->isGuarded() )
		tSock->objMessage( "[Guarded]", i );
	return true;
}

}
