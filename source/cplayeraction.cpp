#include "uox3.h"
#include "weight.h"
#include "combat.h"
#include "cRaces.h"
#include "skills.h"
#include "cMagic.h"
#include "trigger.h"
#include "mapstuff.h"
#include "cScript.h"
#include "cEffects.h"
#include "packets.h"
#include "classes.h"
#include "regions.h"
#include "books.h"
#include "magic.h"
#include "townregion.h"
#include "gump.h"
#include "cGuild.h"
#include "boats.h"
#include "msgboard.h"

//o---------------------------------------------------------------------------o
//|	Function	-	Bounce( cSocket *bouncer, CItem *bouncing )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Bounce items back from where they came
//o---------------------------------------------------------------------------o
void Bounce( cSocket *bouncer, CItem *bouncing )
{
	if( bouncer == NULL || bouncing == NULL )
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
		}
			break;
		case PL_OWNPACK:
		case PL_OTHERPACK:
		case PL_PAPERDOLL:
			bouncing->SetContSerial( spot );	
			break;
	}
	bouncing->Dirty();
	bouncer->Send( &bounce );
	bouncer->PickupSpot( PL_NOWHERE );
}

//o---------------------------------------------------------------------------o
//|   Function    :  CItem *autoStack( cSocket *mSock, CItem *i, CItem *pack )
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
CItem *doStacking( cSocket *mSock, CChar *mChar, CItem *i, CItem *stack )
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
		Items->DeleItem( i );
		if( mSock != NULL )
		{
			mSock->statwindow( mChar );
			Effects->itemSound( mSock, stack, false );
		}
		return stack;
	}
	return i;
}

//o--------------------------------------------------------------------------o
//|	Function	-	bool autoStack( cSocket *mSock, CItem *i, CItem *pack )
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
CItem *autoStack( cSocket *mSock, CItem *iToStack, CItem *iPack )
{
	CChar *mChar = NULL;
	if( mSock != NULL )
		mChar = mSock->CurrcharObj();
	if( iToStack == NULL || iPack == NULL )
		return false;

	iToStack->SetCont( iPack );
	if( iToStack->isPileable() )
	{
		if( mSock != NULL && ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) )
			Weight->subtractItemWeight( mChar, iToStack );
		for( CItem *stack = iPack->FirstItemObj(); !iPack->FinishedItems(); stack = iPack->NextItemObj() )
		{
			if( stack == NULL )
				continue;

			if( stack->isPileable() && stack->GetSerial() != iToStack->GetSerial() &&
				stack->GetID() == iToStack->GetID() && stack->GetColour() == iToStack->GetColour() &&
				stack->GetAmount() < MAX_STACK )
			{ // Autostack
				if( doStacking( mSock, mChar, iToStack, stack ) == stack )	// compare to stack, if doStacking returned the stack, then the raw object was deleted
					return stack;	// return the stack
			}
		}
		if( mSock != NULL && ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) )
			Weight->addItemWeight( mChar, iToStack );
	}
	iToStack->SetX( (SI16)( 20 + RandomNum( 0, 79 ) ) );
	iToStack->SetY( (SI16)( 40 + RandomNum( 0, 99 ) ) );
	iToStack->SetZ( 9 );

//	iToStack->RemoveFromSight();
	if( mSock != NULL )
	{
		mSock->statwindow( mChar );
		Effects->itemSound( mSock, iToStack, false );
	}
	return iToStack;
}

//o---------------------------------------------------------------------------o
//|   Function    :  Grab( cSocket *mSock )
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
	if( i == NULL )
		return true;

	CPBounce bounce( 0 );

	CItem *x			= i;
	CChar *npc			= NULL;
	cBaseObject *iCont	= i->GetCont();
	if( iCont != NULL )  //Find character owning item
	{
		tSock->PickupSerial( i->GetContSerial() );
		if( iCont->GetObjType() == OT_CHAR )
			tSock->PickupSpot( PL_PAPERDOLL );
		else
		{
			if( getPackOwner( i ) != ourChar )
				tSock->PickupSpot( PL_OTHERPACK );
			else
				tSock->PickupSpot( PL_OWNPACK );
		}
		CChar *npc = getPackOwner( i );
		if( npc != NULL )
		{
			if( !ourChar->IsGM() && npc != ourChar && npc->GetOwnerObj() != ourChar )
			{
				tSock->Send( &bounce );
				return true;
			}
		}
		else
		{
			x = getRootPack( i );
			if( x != NULL )
			{
				if( x->isCorpse() )
				{
					CChar *corpseTargChar = (CChar *)x->GetOwnerObj();
					if( corpseTargChar != NULL )
					{
						if( corpseTargChar->IsGuarded() ) // Is the corpse being guarded?
							Combat->petGuardAttack( ourChar, corpseTargChar, corpseTargChar->GetSerial() );
						else if( x->isGuarded() )
							Combat->petGuardAttack( ourChar, corpseTargChar, x->GetSerial() );
					}
				}
				else if( x->GetLayer() == 0 && x->GetID() == 0x1E5E ) // Trade Window
				{
					serial = x->GetMoreB();
					if( serial == INVALIDSERIAL )
						return true;
					CItem *z = calcItemObjFromSer( serial );
					if( z != NULL )
					{
						if( z->GetMoreZ() || x->GetMoreZ() )
						{
							z->SetMoreZ( 0 );
							x->SetMoreZ( 0 );
							sendTradeStatus( z, x );
						}
						// Default item pick up sound sent to other player involved in trade
						cSocket *zSock = calcSocketObjFromChar( (CChar *)z->GetCont() );
						if( zSock != NULL )
							Effects->PlaySound( zSock, 0x0057, false );
					}
				}
			}
		}
	}
	else
	{
		tSock->PickupSpot( PL_GROUND );
		tSock->PickupLocation( i->GetX(), i->GetY(), i->GetZ() );
	}

	if( i->isCorpse() || !checkItemRange( ourChar, x, DIST_NEARBY ) )
	{
		tSock->Send( &bounce );
		return true;
	}

	if( x->GetMultiObj() != NULL )
	{
		if( ( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND ) && x->GetMultiObj() != ourChar->GetMultiObj() )
		{
			tSock->Send( &bounce );
			return true;
		}
		i->SetMulti( INVALIDSERIAL );
	}

	if( i->isDecayable() )
		i->SetDecayTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) ) );

	if( iCont != NULL )
	{
		if( iCont->GetObjType() == OT_CHAR )
		{
			CChar *pChar = (CChar *)iCont;
			if( pChar )
				pChar->TakeOffItem( i->GetLayer() );
		} 
		else 
		{
			CItem *pItem = (CItem *)iCont;
			if( pItem )
				pItem->ReleaseItem( i );
		}
	}

	if( i->isGuarded() )
	{
		if( npc != NULL && tSock->PickupSpot() == PL_OTHERPACK )
			Combat->petGuardAttack( ourChar, npc, i->GetSerial() );

		CChar *petGuard = Npcs->getGuardingPet( ourChar, i->GetSerial() );
		if( petGuard != NULL )
			petGuard->SetGuarding( INVALIDSERIAL );
		i->SetGuarded( false );
	}

	CTile tile;
	Map->SeekTile( i->GetID(), &tile );
	if( !ourChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != ourChar ) ||
		( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
	{
		tSock->Send( &bounce );
		return true;
	}

	Effects->PlaySound( tSock, 0x0057, true );
	if( i->GetAmount() > 1 )
	{
		UI16 amount = tSock->GetWord( 5 );
		if( amount > i->GetAmount() )
			amount = i->GetAmount();
		else if( amount < i->GetAmount() )
		{
			CItem *c = i->Dupe();
			if( c != NULL )
			{
				c->SetAmount( i->GetAmount() - amount );
				c->SetCont( i->GetCont() );
				if( c->GetSpawnObj() != NULL )
					nspawnsp.AddSerial( c->GetSpawn(), calcItemFromSer( c->GetSerial() ) );
			}
			i->SetAmount( amount );
		}
		if( i->GetID() == 0x0EED )
		{
			if( tSock->PickupSpot() == PL_OWNPACK )
				tSock->statwindow( ourChar );
		}
	}
	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
	{
		if( !Weight->checkCharWeight( NULL, ourChar, i ) )
		{
			tSock->sysmessage( "That person can not possibly hold more weight" );
			tSock->Send( &bounce );
			return true;
		}
	}

	MapRegion->RemoveItem( i );
	i->RemoveFromSight();
	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
	{
		Weight->addItemWeight( ourChar, i );
		tSock->statwindow( ourChar );
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  Equip( cSocket *mSock )
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
	CPBounce bounce( 5 );
	CItem *i = calcItemObjFromSer( iserial );
	CChar *k = calcCharObjFromSer( cserial );
	if( i == NULL )
		return true;

	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
		Weight->subtractItemWeight( ourChar, i ); // SetCont() adds the weight for us (But we also had to add the weight in grabItem() since it sets cont as INVALIDSERIAL

	if( !ourChar->IsGM() && k != ourChar )	// players cant equip items on other players or npc`s paperdolls.  // GM PRIVS
	{
		Bounce( tSock, i );
		tSock->sysmessage( 1186 );
		tSock->statwindow( ourChar );
		return true;
	}
/*
	if( i->GetCont() != NULL )
	{
		tSock->statwindow( ourChar );
		Bounce( tSock, i );
		return true;
	}
*/
	if( ourChar->IsDead() )
	{
		tSock->sysmessage( 1185 );
		return true;
	}
	if( k == NULL )
		return true;

	ARMORCLASS ac1 = Races->ArmorRestrict( k->GetRace() );
	ARMORCLASS ac2 = i->GetArmourClass();

	if( ac1 != 0 && ( (ac1&ac2) == 0 ) )	// bit comparison, if they have ANYTHING in common, they can wear it
	{
		tSock->sysmessage( 1187 );
		Bounce( tSock, i );
		tSock->statwindow( ourChar );
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
			{
				tSock->statwindow( ourChar );
				Effects->itemSound( tSock, i, true );
			}
			else
				Effects->itemSound( tSock, i, false );
			return true;
		}
	}
	CTile tile;
	Map->SeekTile( i->GetID(), &tile);
	if( !ourChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != ourChar ) ||
		( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
	{
		Bounce( tSock, i );
		tSock->statwindow( ourChar );
		return true;
	}

	if( i->GetLayer() == 0 )
		i->SetLayer( tSock->GetByte( 5 ) );

	// 1/13/2003 - Xuri - Fix for equiping an item to more than one hand, or multiple equiping.
	CItem *j = k->GetItemAtLayer( i->GetLayer() );
	if( j != NULL )
	{
		tSock->sysmessage( "You can't equip two items in the same slot." );
		tSock->statwindow( ourChar );
		Bounce( tSock, i );
		return true;
	}

	if( !Weight->checkCharWeight( ourChar, k, i ) )
	{
		tSock->sysmessage( "That person can not possibly hold more weight" );
		tSock->statwindow( ourChar );
		Bounce( tSock, i );
		return true;
	}

	i->SetCont( k );

	if( cwmWorldState->GetDisplayLayers() )
		Console << "Item equipped on layer " << i->GetLayer() << myendl;

//	i->RemoveFromSight();
	Effects->PlaySound( tSock, 0x0057, false );
	tSock->statwindow( ourChar );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function		:	DropOnChar( cSocket *mSock, CChar *targChar, CItem *i )
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
bool DropOnPC( cSocket *mSock, CChar *mChar, CChar *targPlayer, CItem *i )
{
	bool stackDeleted = false;
	if( targPlayer == mChar )
	{
		CItem *pack = getPack( mChar );
		if( pack == NULL ) // if player has no pack, put it at its feet
		{
			i->SetCont( NULL );
			i->SetLocation( mChar );
		} 
		else
			stackDeleted = ( autoStack( mSock, i, pack ) != i );
	}
	else // Trade stuff
	{
		if( isOnline( targPlayer ) )
		{
			CItem *j = startTrade( mSock, targPlayer );
			if( j )
			{
				i->SetCont( j );
				i->SetX( 30 );
				i->SetY( 30 );
				i->SetZ( 9 );

//				i->RemoveFromSight();
			}
		}
		else if( mChar->GetCommandLevel() >= CNS_CMDLEVEL )
		{
			CItem *p = getPack( targPlayer );
			if( p == NULL )
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					Weight->subtractItemWeight( mChar, i );
				Bounce( mSock, i );
				return stackDeleted;
			}
//			i->RemoveFromSight();
			stackDeleted = ( autoStack( calcSocketObjFromChar( targPlayer ), i, p ) != i );
		}
		else
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( mChar, i );
			Bounce( mSock, i );
		}
	}
	return stackDeleted;
}
bool DropOnNPC( cSocket *mSock, CChar *mChar, CChar *targNPC, CItem *i )
{
	bool stackDeleted	= false;
	bool executeNpc		= true;
	UI16 targTrig		= i->GetScriptTrigger();
	cScript *toExecute	= Trigger->GetScript( targTrig );
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
		toExecute	= Trigger->GetScript( targTrig );
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

	if( i->GetType() == 14 && targNPC->GetHunger() < 6 && targNPC->IsTamed() && 
		( targNPC->GetOwnerObj() == mChar || Npcs->checkPetFriend( mChar, targNPC ) ) ) // do food stuff
	{
		Effects->PlaySound( mSock, static_cast<UI16>(0x003A + RandomNum( 0, 2 )), true );
		npcAction( targNPC, 3 );

		if( i->GetPoisoned() && targNPC->GetPoisoned() < i->GetPoisoned() )
		{
			Effects->PlaySound( mSock, 0x0246, true ); //poison sound - SpaceDog
			targNPC->SetPoisoned( i->GetPoisoned() );
			targNPC->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( POISON )) ) );
			targNPC->SendToSocket( mSock, true, mChar );
		}
		//Remove a food item
		i = DecreaseItemAmount( i );
		targNPC->SetHunger( static_cast<SI08>(targNPC->GetHunger() + 1) );
		if( i == NULL )
			return true; //stackdeleted
	}
	if( !targNPC->isHuman() )
	{
		// Sept 25, 2002 - Xuri - Weight fixes
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( mChar, i );

		Bounce( mSock, i );
	}
	else if( mChar->GetTrainer() != targNPC->GetSerial() )
	{
		// Sept 25, 2002 - Xuri - weight fix
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( mChar, i );

		targNPC->talk( mSock, 1197, false );
		Bounce( mSock, i );
	}
	else // This NPC is training the player
	{
		if( i->GetID() == 0x0EED ) // They gave the NPC gold
		{
			UI08 trainedIn = targNPC->GetTrainingPlayerIn();
			targNPC->talk( mSock, 1198, false );
			UI16 oldskill = mChar->GetBaseSkill( trainedIn ); 
			mChar->SetBaseSkill( (UI16)( mChar->GetBaseSkill( trainedIn ) + i->GetAmount() ), trainedIn );
			if( mChar->GetBaseSkill( trainedIn ) > 250 )
				mChar->SetBaseSkill( 250, trainedIn );
			Skills->updateSkillLevel( mChar, trainedIn );
			mSock->updateskill( trainedIn );
			UI16 getAmount = i->GetAmount();
			if( i->GetAmount() > 250 ) // Paid too much
			{
				i->SetAmount( i->GetAmount() - 250 - oldskill );
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					Weight->subtractItemWeight( mChar, i );
				Bounce( mSock, i );
			}
			else  // Gave exact change
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					Weight->subtractItemWeight( mChar, i );
				Items->DeleItem( i );
				stackDeleted = true;
			}
			mChar->SetTrainer( INVALIDSERIAL );
			targNPC->SetTrainingPlayerIn( 255 );
			Effects->goldSound( mSock, getAmount, false );
		}
		else // Did not give gold
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( mChar, i );
			targNPC->talk( mSock, 1199, false );
			Bounce( mSock, i );
		}
	}
	return stackDeleted;
}
bool DropOnChar( cSocket *mSock, CChar *targChar, CItem *i )
{
	CChar *mChar = mSock->CurrcharObj();
	if( mChar == NULL )
		return false;

	if( !Weight->checkCharWeight( mChar, targChar, i ) )
	{
		mSock->sysmessage( "That person can not possibly hold more weight" );
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->subtractItemWeight( mChar, i );
			mSock->statwindow( mChar );
		}
		Bounce( mSock, i );
		return false;
	}

	if( !targChar->IsNpc() )
		return DropOnPC( mSock, mChar, targChar, i );

	return DropOnNPC( mSock, mChar, targChar, i );
}

//o---------------------------------------------------------------------------o
//|   Function    :  Drop( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Item is dropped on the ground or on a character
//o---------------------------------------------------------------------------o
void Drop( cSocket *mSock ) // Item is dropped on ground
{
	CChar *nChar	= mSock->CurrcharObj();
	SERIAL serial	= mSock->GetDWord( 1 );
	CItem *i		= calcItemObjFromSer( serial );
	bool stackDeleted = false;
	
	CPBounce bounce( 5 );
	if( i == NULL )
	{
		nChar->Teleport();
		return;
	}
/*	if( i->GetCont() != NULL )
	{
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( nChar, i );
		Bounce( mSock, i );
		return;
	}
*/
	CTile tile;
	Map->SeekTile( i->GetID(), &tile);
	if( !nChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != nChar ) ||
		( tile.Weight() == 255 && i->GetMovable() != 1 ) ) )
	{
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( nChar, i );
		Bounce( mSock, i );
		return;
	}
	
	if( mSock->GetByte( 5 ) != 0xFF )	// Dropped in a specific location or on an item
	{
//		i->RemoveFromSight();
		i->SetCont( NULL );
		i->SetLocation( mSock->GetWord( 5 ), mSock->GetWord( 7 ), mSock->GetByte( 9 ), nChar->WorldNumber() );
	}
	else
	{
		CChar *t = calcCharObjFromSer( mSock->GetDWord( 10 ) );
		if( t != NULL )
			stackDeleted = DropOnChar( mSock, t, i );
		else
		{
			//Bounces items dropped in illegal locations in 3D UO client!!!
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->subtractItemWeight( nChar, i );
			mSock->statwindow( nChar );
			Bounce( mSock, i );
			return;
		}
	}

	if( !stackDeleted )
	{
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( nChar, i );

		if( i->isDecayable() )
			i->SetDecayTime( BuildTimeValue( static_cast< R32 >(cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) ) ) );

		if( nChar->GetMultiObj() != NULL )
		{
			CMultiObj *multi = findMulti( i );
			if( multi != NULL )
				i->SetMulti( multi );
		}
		Effects->itemSound( mSock, i, ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) );
	}
	mSock->statwindow( nChar );
}

//o---------------------------------------------------------------------------o
//|   Function    :  DropOnItem( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item is dropped in a container or on another item
//o---------------------------------------------------------------------------o
void DropOnItem( cSocket *mSock )
{
	CChar *mChar = mSock->CurrcharObj();
	CItem *nCont = calcItemObjFromSer( mSock->GetDWord( 10 ) );
	if( nCont == NULL || mChar == NULL )
		return;

	CItem *nItem = calcItemObjFromSer( mSock->GetDWord( 1 ) );
	if( nItem == NULL )
		return;

	bool stackDeleted = false;
	if( nCont->GetLayer() == 0 && nCont->GetID() == 0x1E5E && nCont->GetCont() == mChar )
	{	// Trade window
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->subtractItemWeight( mChar, nItem );
			mSock->statwindow( mChar );
		}
		nItem->SetCont( mChar );
		CItem *z = calcItemObjFromSer( nCont->GetMoreB() );
		if( z != NULL )
		{
			if( z->GetMoreZ() || nCont->GetMoreZ() )
			{
				z->SetMoreZ( 0 );
				nCont->SetMoreZ( 0 );
				sendTradeStatus( z, nCont );
			}
			cSocket *zSock = calcSocketObjFromChar( (CChar *)z->GetCont() );
			if( zSock != NULL )
				Effects->itemSound( zSock, nCont, ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) );
		}
		return;
	}

/*
	if( nItem->GetCont() != NULL )
	{
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( mChar, nItem );
		Bounce( mSock, nItem );
		return;
	}
*/
	CTile tile;
	Map->SeekTile( nItem->GetID(), &tile );
	if( !mChar->AllMove() && ( nItem->GetMovable() == 2 || ( nItem->IsLockedDown() && nItem->GetOwnerObj() != mChar ) ||
		( tile.Weight() == 255 && nItem->GetMovable() != 1 ) ) )
	{
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->subtractItemWeight( mChar, nItem );
			mSock->statwindow( mChar );
		}
		Bounce( mSock, nItem );
		return;
	}

	if( nCont->GetType() == 87 )	// Trash container
	{
		Effects->PlaySound( mSock, 0x0042, false );
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->subtractItemWeight( mChar, nItem );
			mSock->statwindow( mChar );
		}
		Items->DeleItem( nItem );
		mSock->sysmessage( 1201 );
		return;
	}
	else if( nCont->GetType() == 9 )	// Spell Book
	{
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->subtractItemWeight( mChar, nItem );
			mSock->statwindow( mChar );
		}
		if( nItem->GetID( 1 ) != 0x1F || nItem->GetID( 2 ) < 0x2D || nItem->GetID( 2 ) > 0x72 )
		{
			Bounce( mSock, nItem );
			mSock->sysmessage( 1202 );
			return;
		}
		CChar *c = getPackOwner( nCont );
		if( c != NULL && c != mChar && !mChar->CanSnoop() )
		{
			Bounce( mSock, nItem );
			mSock->sysmessage( 1203 );
			return;
		}
		std::string name;
		name.reserve( MAX_NAME );
		if( nItem->GetName()[0] == '#' )
			getTileName( nItem, name );
		else
			name = nItem->GetName();

		if( nCont->GetMore( 1 ) == 1 )	// using more1 to "lock" a spellbook for RP purposes
		{
			mSock->sysmessage( 1204 );
			Bounce( mSock, nItem );
			return;
		}

		if( name == Dictionary->GetEntry( 1605 ) )
		{
			if( nCont->GetMoreX() == 0xFFFFFFFF && nCont->GetMoreY() == 0xFFFFFFFF && nCont->GetMoreZ() == 0xFFFFFFFF )
			{
				mSock->sysmessage( 1205 );
				Bounce( mSock, nItem );
				return;
			}
			nCont->SetMoreX( 0xFFFFFFFF );
			nCont->SetMoreY( 0xFFFFFFFF );
			nCont->SetMoreZ( 0xFFFFFFFF );
		}
		else
		{
			int targSpellNum = nItem->GetID() - 0x1F2D;
			if( Magic->HasSpell( nCont, targSpellNum ) )
			{
				mSock->sysmessage( 1206 );
				Bounce( mSock, nItem );
				return;
			}
			else
				Magic->AddSpell( nCont, targSpellNum );
		}
		Effects->PlaySound( mSock, 0x0042, false );
		Items->DeleItem( nItem );
		return;
	}
	else if( nCont->isPileable() && nItem->isPileable() && nCont->GetID() == nItem->GetID() && nCont->GetColour() == nItem->GetColour() )
	{	// Stacking
		bool canHold = true;
		if( nCont->GetContSerial() >= BASEITEMSERIAL )
			canHold = Weight->checkPackWeight( mChar, static_cast<CItem *>(nCont->GetCont()), nItem );
		else
			canHold = Weight->checkCharWeight( mChar, static_cast<CChar *>(nCont->GetCont()), nItem );
		if( !canHold )
		{
			if( nCont->GetContSerial() >= BASEITEMSERIAL )
				mSock->sysmessage( "That pack cannold hold any more weight" );
			else
				mSock->sysmessage( "That person can not possibly hold more weight" );
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->subtractItemWeight( mChar, nItem );
				mSock->statwindow( mChar );
			}
			Bounce( mSock, nItem );
			return;
		}
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->subtractItemWeight( mChar, nItem );
		nItem->SetCont( nCont );
		stackDeleted = ( doStacking( mSock, mChar, nItem, nCont ) != nItem );
		if( !stackDeleted )	// if the item didn't stack or the stack was full
		{
			Bounce( mSock, nItem );
		}
		mSock->statwindow( mChar );
	}
	else if( nCont->GetType() == 1 )
	{
		CChar *j = getPackOwner( nCont );
		if( j != NULL )
		{
			if( j->IsNpc() && j->GetNPCAiType() == aiPLAYERVENDOR && j->GetOwnerObj() == mChar )
			{
				mChar->SetSpeechMode( 3 );
				mChar->SetSpeechItem( nItem->GetSerial() );
				mSock->sysmessage( 1207 );
			}
			else if( j != mChar && mChar->GetCommandLevel() < CNS_CMDLEVEL )
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				{
					Weight->subtractItemWeight( mChar, nItem );
					mSock->statwindow( mChar );
				}
				mSock->sysmessage( 1630 );
				Bounce( mSock, nItem );
				return;
			}
		}
		if( mSock->GetByte( 5 ) != 0xFF )	// In a specific spot in a container
		{
			if( nCont != nItem->GetCont() && !Weight->checkPackWeight( mChar, nCont, nItem ) )
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				{
					Weight->subtractItemWeight( mChar, nItem );
					mSock->statwindow( mChar );
				}
				mSock->sysmessage( "That pack cannold hold any more weight" );
				Bounce( mSock, nItem );
				return;
			}
			nItem->SetX( mSock->GetWord( 5 ) );
			nItem->SetY( mSock->GetWord( 7 ) );

			nItem->SetCont( nCont );
			nItem->SetZ( 9 );

//			nItem->RemoveFromSight();
			mSock->statwindow( mChar );
		}
		else
		{
			if( nCont != nItem->GetCont() && !Weight->checkPackWeight( mChar, nCont, nItem ) )
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				{
					Weight->subtractItemWeight( mChar, nItem );
					mSock->statwindow( mChar );
				}
				mSock->sysmessage( "That pack cannold hold any more weight" );
				Bounce( mSock, nItem );
				return;
			}
			nItem->SetCont( nCont );
			stackDeleted = ( autoStack( mSock, nItem, nCont ) != nItem );
			if( !stackDeleted )
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					Weight->subtractItemWeight( mChar, nItem );
			}
			mSock->statwindow( mChar );
		}
	}
	else
	{
		MapRegion->RemoveItem( nItem );

		nItem->SetX( mSock->GetWord( 5 ) );
		nItem->SetY( mSock->GetWord( 7 )  );
		nItem->SetZ( mSock->GetByte( 9 ) );

		if( nCont->GetType() == 63 || nCont->GetType() == 65 ) // - Unlocked item spawner or unlockable item spawner
			nItem->SetCont( nCont );
		else
		{
			nItem->SetCont( NULL );
			MapRegion->AddItem( nItem );
		}

//		nItem->RemoveFromSight();
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->subtractItemWeight( mChar, nItem );
			mSock->statwindow( mChar );
		}
	}
	if( !stackDeleted )
		Effects->itemSound( mSock, nItem, ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) );
}

bool CPIDropItem::Handle( void )
{
	CChar *ourChar	= tSock->CurrcharObj();
	UI08 opt		= tSock->GetByte( 10 );
	if( opt >= 0x40 && opt != 0xFF )
		DropOnItem( tSock );
	else 
		Drop( tSock );
	ourChar->BreakConcentration( tSock );
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void prowessTitle( CChar *p, std::string& prowessTitle )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Fetches characters "prowess" title based on his skill and
//|                 entries in titles.scp
//|	Modified	-	Modified to match OSI's prowess titles by Zane
//o---------------------------------------------------------------------------o
void prowessTitle( CChar *p, std::string &prowesstitle )
{
	if( p == NULL )
		return;
	UI16 titlenum = 0;
	UI16 x = p->GetBaseSkill( p->GetBestSkill() );

	if( x < 1000 )
		titlenum = (UI16)((x - 10) / 100 - 2);
	else
		titlenum = 8;
	prowesstitle = cwmWorldState->title[titlenum].prowess;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void skillTitle( CChar *p, std::string& skilltitle )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns players bestskill for prowess title
//o---------------------------------------------------------------------------o
void skillTitle( CChar *p, std::string& skilltitle )
{
	UI08 titlenum	= (UI08)( p->GetBestSkill() + 1 );
	skilltitle		= cwmWorldState->title[titlenum].skill;
}

//o---------------------------------------------------------------------------o
//|	Function	-	fameTitle( CChar *p, std::string& fametitle )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns players reputation title based on their Fame and Karma
//o---------------------------------------------------------------------------o
void fameTitle( CChar *p, std::string& fametitle )
{
	if( p != NULL )
	{
		UString thetitle;
		UI08 titlenum = 0;

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
		if( p->GetRace() != 0 && p->GetRace() != 65535 )
			thetitle = cwmWorldState->title[titlenum].fame + " " + Races->Name( p->GetRace() ) + " ";
		else
			thetitle = cwmWorldState->title[titlenum].fame + " ";
		if( f >= 10000 ) // Morollans bugfix for repsys
		{
			if( p->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() )
			{
				if( p->GetID( 2 ) == 0x91 )
					fametitle = UString::sprintf( Dictionary->GetEntry( 1177 ).c_str(), Races->Name( p->GetRace() ) );
				else
					fametitle = UString::sprintf( Dictionary->GetEntry( 1178 ).c_str(), Races->Name( p->GetRace() ) );
			}
			else if( p->GetID( 2 ) == 0x91 )
				fametitle = UString::sprintf( Dictionary->GetEntry( 1179 ).c_str(), thetitle.c_str() );
			else
				fametitle = UString::sprintf( Dictionary->GetEntry( 1180 ).c_str(), thetitle.c_str() );
		}
		else
		{
			if( p->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() )
				fametitle = Dictionary->GetEntry( 1181 );
			else if( !thetitle.stripWhiteSpace().empty() )
				fametitle = UString::sprintf( Dictionary->GetEntry( 1182 ).c_str(), thetitle.c_str() );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  PaperDoll( cSocket *s, CChar *pdoll )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Open a characters paperdoll and show titles based on skill,
//|					 reputation, murder counts, race, ect.
//o---------------------------------------------------------------------------o
void PaperDoll( cSocket *s, CChar *pdoll )
{
	UString tempstr;
	CPPaperdoll pd		= (*pdoll);
	UnicodeTypes sLang	= s->Language();

	UString prowesstitle;
	UString fametitle;
	UString skilltitle;
	prowessTitle( pdoll, prowesstitle );
	fameTitle( pdoll, fametitle );
	skillTitle( pdoll, skilltitle );

	bool bContinue = false;
	if( pdoll->IsGM() )
		tempstr = pdoll->GetName() + " " + pdoll->GetTitle();
	else if( pdoll->IsNpc() )
		tempstr = fametitle + " " + pdoll->GetName() + " " + pdoll->GetTitle();
	else if( pdoll->IsDead() )
		tempstr = pdoll->GetName();
	// Murder tags now scriptable in SECTION MURDERER - Titles.scp - Thanks Ab - Zane
	else if( pdoll->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() )
	{
		if( cwmWorldState->murdererTags.size() < 1 )
			tempstr = UString::sprintf( Dictionary->GetEntry( 374, sLang ).c_str(), pdoll->GetName().c_str(), pdoll->GetTitle().c_str(), prowesstitle.c_str(), skilltitle.c_str() );
		else if( pdoll->GetKills() < cwmWorldState->murdererTags[0].loBound )	// not a real murderer
			bContinue = true;
		else
		{
			SI16 mKills = pdoll->GetKills();
			UI32 kCtr;
			for( kCtr = 0; kCtr < cwmWorldState->murdererTags.size() - 1; ++kCtr )
			{
				if( mKills >= cwmWorldState->murdererTags[kCtr].loBound && mKills < cwmWorldState->murdererTags[kCtr+1].loBound )
					break;
			}
			if( kCtr >= cwmWorldState->murdererTags.size() )
				bContinue = true;
			else
				tempstr = cwmWorldState->murdererTags[kCtr].toDisplay + " " + pdoll->GetName() + ", " + pdoll->GetTitle() + prowesstitle + " " + skilltitle;
		}
	}
	else if( pdoll->IsCriminal() )
		tempstr = UString::sprintf( Dictionary->GetEntry( 373, sLang ).c_str(), pdoll->GetName().c_str(), pdoll->GetTitle().c_str(), prowesstitle.c_str(), skilltitle.c_str() );
	else
		bContinue = true;
	if( bContinue )
	{
		tempstr = fametitle + pdoll->GetName();	//Repuation + Name
		if( pdoll->GetTownTitle() || pdoll->GetTownPriv() == 2 )	// TownTitle
		{
			if( pdoll->GetTownPriv() == 2 )	// is Mayor
				tempstr = UString::sprintf( Dictionary->GetEntry( 379, sLang ).c_str(), pdoll->GetName().c_str(), region[pdoll->GetTown()]->GetName().c_str(), prowesstitle.c_str(), skilltitle.c_str() );
			else	// is Resident
				tempstr = pdoll->GetName() + " of " + region[pdoll->GetTown()]->GetName() + ", " + prowesstitle + " " + skilltitle;
		}
		else	// No Town Title
		{
			if( !pdoll->IsIncognito() && !( pdoll->GetTitle().empty() ) )	// Titled & Skill
				tempstr += " " + pdoll->GetTitle() + ", " + prowesstitle + " " + skilltitle;
			else	// Just skilled
				tempstr += ", " + prowesstitle + " " + skilltitle;
		}
	}
	pd.Text( tempstr.c_str() );
	s->Send( &pd );
}

//o---------------------------------------------------------------------------o
//|	Function	-	usePotion( CChar *p, CItem *i )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Character uses a potion
//o---------------------------------------------------------------------------o
void usePotion( CChar *p, CItem *i )
{
	if( p == NULL || i == NULL )
		return;

	cSocket *mSock = calcSocketObjFromChar( p );
	if( cwmWorldState->ServerData()->GetPotionDelay() != 0 )
		Effects->tempeffect( p, p, 26, 0, 0, 0 );
	switch( i->GetMoreY() )
	{
		case 1: // Agility Potion
			Effects->staticeffect( p, 0x373A, 0, 15 );
			switch( i->GetMoreZ() )
			{
				case 1:
					Effects->tempeffect( p, p, 6, (UI16)RandomNum( 6, 15 ), 0, 0 );
					mSock->sysmessage( 1608 );
					break;
				case 2:
					Effects->tempeffect( p, p, 6, (UI16)RandomNum( 11, 30 ), 0, 0 );
					mSock->sysmessage( 1609 );
					break;
				default:
					Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
					return;
			}
			Effects->PlaySound( p, 0x01E7 );
			if( mSock != NULL )
				updateStats( p, 2 );
			break;
		case 2: // Cure Potion
			if( p->GetPoisoned() < 1 )
				mSock->sysmessage( 1344 );
			else
			{
				UI08 x;
				switch( i->GetMoreZ() )
				{
					case 1:
						x = (UI08)RandomNum( 1, 100 );
						if( p->GetPoisoned() == 1 && x < 81 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 2 && x < 41 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 3 && x < 21 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 4 && x < 6 )
							p->SetPoisoned( 0 );
						break;
					case 2:
						x = (UI08)RandomNum( 1, 100 );
						if( p->GetPoisoned() == 1 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 2 && x < 81 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 3 && x < 41 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 4 && x < 21 )
							p->SetPoisoned( 0 );
						break;
					case 3:
						x = (UI08)RandomNum( 1, 100 );
						if( p->GetPoisoned() == 1 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 2 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 3 && x < 81 )
							p->SetPoisoned( 0 );
						else if( p->GetPoisoned() == 4 && x < 61 )
							p->SetPoisoned( 0 );
						break;
					default:
						Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
						return;
				}
				
				if( p->GetPoisoned() )
					mSock->sysmessage( 1345 ); 
				else
				{
					Effects->staticeffect( p, 0x373A, 0, 15 );
					Effects->PlaySound( p, 0x01E0 ); 
					mSock->sysmessage( 1346 );
				} 
			}
			p->SendToSocket( mSock, true, p );
			break;
		case 3: // Explosion Potion
			if( region[p->GetRegion()]->IsGuarded() )
			{
				mSock->sysmessage( 1347 );
				return;
			}
			mSock->AddID( i->GetSerial() );
			mSock->sysmessage( 1348 );
			Effects->tempeffect( p, p, 16, 0, 1, 3 );
			Effects->tempeffect( p, p, 16, 0, 2, 2 );
			Effects->tempeffect( p, p, 16, 0, 3, 1 );
			Effects->tempeffect( p, i, 17, 0, 4, 0 );
			mSock->target( 0, 207, "" );
			return;
		case 4: // Heal Potion
			switch( i->GetMoreZ() )
			{
				case 1:
					p->SetHP( UOX_MIN( (SI16)(p->GetHP() + 5 + RandomNum( 1, 5 ) + p->GetSkill( 17 ) / 100 ), static_cast<SI16>(p->GetMaxHP()) ) );
					mSock->sysmessage( 1349 );
					break;
				case 2:
					p->SetHP( UOX_MIN( (SI16)(p->GetHP() + 15 + RandomNum( 1, 10 ) + p->GetSkill( 17 ) / 50 ), static_cast<SI16>(p->GetMaxHP()) ) );
					mSock->sysmessage( 1350 );
					break;
				case 3:
					p->SetHP( UOX_MIN( (SI16)(p->GetHP() + 20 + RandomNum( 1, 20 ) + p->GetSkill( 17 ) / 40 ), static_cast<SI16>(p->GetMaxHP()) ) );
					mSock->sysmessage( 1351 );
					break;
				default:
					Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
					return;
			}
			if( mSock != NULL )
				updateStats( p, 0 );
			Effects->staticeffect( p, 0x376A, 0x09, 0x06); // Sparkle effect
			Effects->PlaySound( p, 0x01F2 ); //Healing Sound - SpaceDog
			break;
		case 5: // Night Sight Potion
			//{
			Effects->staticeffect( p, 0x376A, 0x09, 0x06 );
			Effects->tempeffect( p, p, 2, 0, 0, 0 );
			Effects->PlaySound( p, 0x01E3 );
			break;
			//}
		case 6: // Poison Potion
			if( p->GetPoisoned() < (SI08)i->GetMoreZ() )
				p->SetPoisoned( (SI08)i->GetMoreZ() );
			if( i->GetMoreZ() > 4 )
				i->SetMoreZ( 4 );
			p->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( (R32)cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) ) );
			p->SendToSocket( mSock, true, p );
			Effects->PlaySound( p, 0x0246 );
			mSock->sysmessage( 1352 );
			break;
		case 7: // Refresh Potion
			switch( i->GetMoreZ() )
			{
				case 1:
					p->SetStamina( UOX_MIN( (SI16)(p->GetStamina() + 20 + RandomNum( 1, 10 )), p->GetMaxStam() ) );
					mSock->sysmessage( 1353 );
					break;
				case 2:
					p->SetStamina( UOX_MIN( (SI16)(p->GetStamina() + 40 + RandomNum( 1, 30 )), p->GetMaxStam() ) );
					mSock->sysmessage( 1354 );
					break;
				default:
					Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
					return;
			}
			if( mSock != NULL )
				updateStats( p, 2 );
			Effects->staticeffect( p, 0x376A, 0x09, 0x06); // Sparkle effect
			Effects->PlaySound( p, 0x01F2 ); //Healing Sound
			break;
		case 8: // Strength Potion
			Effects->staticeffect( p, 0x373A, 0, 15 );
			switch( i->GetMoreZ() )
			{
				case 1:
					Effects->tempeffect( p, p, 8, (UI16)( 5 + RandomNum( 1, 10 ) ), 0, 0);
					mSock->sysmessage( 1355 );
					break;
				case 2:
					Effects->tempeffect( p, p, 8, (UI16)( 10 + RandomNum( 1, 20 ) ), 0, 0);
					mSock->sysmessage( 1356 );
					break;
				default:
					Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
					return;
			}
			Effects->PlaySound( p, 0x01EE );     
			break;
		case 9: // Mana Potion
			switch( i->GetMoreZ() )
			{
				case 1:
					p->SetMana( UOX_MIN( (SI16)(p->GetMana() + 10 + i->GetMoreX()/100), p->GetMaxMana() ) );
					break;
				case 2:
					p->SetMana( UOX_MIN( (SI16)(p->GetMana() + 20 + i->GetMoreX()/50), p->GetMaxMana() ) );
					break;
				default:
					Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
					return;
			}
			if( mSock != NULL )
				updateStats( p, 1 );
			Effects->staticeffect( p, 0x376A, 0x09, 0x06); // Sparkle effect
			Effects->PlaySound( p, 0x01E7); //agility sound - SpaceDog
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
			return;
	}
	Effects->PlaySound( p, 0x0030 );
	if( p->GetID( 1 ) >= 1 && p->GetID( 2 )>90 && !p->IsOnHorse() )
		npcAction( p, 0x22);
	DecreaseItemAmount( i );
	CItem *bPotion = Items->SpawnItem( NULL, p, 1, "#", true, 0x0F0E, 0, true, false );
	if( bPotion != NULL )
	{
		if( bPotion->GetCont() == NULL )
			bPotion->SetLocation( p );
		bPotion->SetDecayable( true );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  handleCharDoubleClick( cSocket *mSock, SERIAL serial, bool keyboard )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles double-clicks on a character
//o---------------------------------------------------------------------------o
void handleCharDoubleClick( cSocket *mSock, SERIAL serial, bool keyboard )
{
	CChar *mChar	= mSock->CurrcharObj();
	CItem *pack		= NULL;
	CChar *c		= calcCharObjFromSer( serial );
	if( c == NULL )
		return;

	if( c->IsNpc() )
	{
		if( c->IsValidMount() )	// Is a mount
		{
			if( ( c->IsTamed() && c->GetOwnerObj() == mChar ) || mChar->IsGM() )
			{
				if( objInRange( mChar, c, DIST_NEXTTILE ) )
				{
					if( mChar->GetID() != mChar->GetOrgID() )
					{
						mSock->sysmessage( 380 );
						return;
					}
					if( mChar->IsDead() )
						mSock->sysmessage( 381 );
					else
						MountCreature( mChar, c );
				}
				else
					mSock->sysmessage( 382 );
			}
			else
				mSock->sysmessage( 1214 );
			return; 
		}
		else if( c->GetID( 1 ) != 0x01 || c->GetID( 2 ) < 0x90 || c->GetID( 2 ) > 0x93 ) // Is a monster
		{
			if( c->GetID() == 0x0123 || c->GetID() == 0x0124 )	// Is a pack animal
			{
				if( !objInRange( mChar, c, DIST_NEARBY ) )
					mSock->sysmessage( 382 );
				else
				{
					pack = getPack( c );
					if( c->GetOwnerObj() == mChar )
					{
						if( pack != NULL )
							openPack( mSock, pack );
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
//			else
//				mSock->sysmessage( 384 );
			return; 
		}
		else if( c->GetNPCAiType() == aiPLAYERVENDOR ) // PlayerVendors
		{
			c->talk( mSock, 385, false );
			pack = getPack( c );
			if( pack != NULL )
				openPack( mSock, pack );
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
//|   Function    :  bool handleDoubleClickTypes( cSocket *mSock, CChar *mChar, CItem *x, UI08 iType )
//|   Date        :  2/11/2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Runs a switch to match an item type to a function
//o---------------------------------------------------------------------------o
bool handleDoubleClickTypes( cSocket *mSock, CChar *mChar, CItem *x, UI08 iType )
{
	CChar *iChar	= NULL;
	CItem *i		= NULL;
	UI32 j;
	UI16 itemID		= x->GetID();

	// Begin Check items by type
	switch( iType )
	{
		case 0:
		case 255:
			//mSock->sysmessage( 486 );
			break;

		case 1:	// Container, Backpack
		case 63: // Item spawn container
			if( x->GetMoreB( 1 ) )// Is trapped
				Magic->MagicTrap( mChar, x );
		case 65:	// Unlocked item spawn container
		case 87:	// Trash container ?
			bool onGround, isGM, isCounselor, isPackOwner, isPetOwner, isPlayerVendor;
			onGround = false;

			if( x->GetCont() != NULL )	// It's inside another container, we need root container to calculate distance
			{
				iChar = getPackOwner( x );
				if( iChar == NULL )
				{
					CItem *rootContainer = getRootPack( x );	// In a pack on the ground
					if( rootContainer == NULL )
					{
						Console.Error( 2, "Pack with serial %li has an invalid container chain!", x->GetSerial() );
						return true;
					}
					else
						onGround = true;
				}
			}
			else
				onGround = true;	// It's on the ground

			isGM			= mChar->IsGM();
			isCounselor		= mChar->IsCounselor();
			isPackOwner		= ( iChar == mChar );
			isPetOwner		= ( iChar != NULL && iChar != mChar && iChar->GetOwnerObj() == mChar );
			isPlayerVendor	= ( iChar != NULL && iChar->GetNPCAiType() == aiPLAYERVENDOR );

			if( onGround || isGM || isCounselor || isPackOwner || isPetOwner || isPlayerVendor )
				openPack( mSock, x );
			else if( objInRange( mSock, iChar, DIST_NEARBY ) )	// Otherwise it's on an NPC, check if it's > 2 paces away
				Skills->Snooping( mSock, iChar, x );
			else
				mSock->sysmessage( 400 );
			return true;

			//Order and Chaos gate "openers" Item types 2 and 4 could cause alot of lag" )
		case 2:	// Order gates opener
			for( j = 0; j < cwmWorldState->GetItemCount(); ++j )
				if( items[j].GetType() == 3 )
				{
					if( items[j].GetMoreZ() == 1 )
					{
						items[j].SetMoreZ( 2 );
						items[j].SetZ( items[j].GetZ() + 17 );
						items[j].Dirty();
					}
					else if( items[j].GetMoreZ() == 2 )
					{
						items[j].SetMoreZ( 1 );
						items[j].SetZ( items[j].GetZ() - 17 );
						items[j].Dirty();
					}
				}
			return true;
		case 4:	// Chaos gate opener
			for( j = 0; j < cwmWorldState->GetItemCount(); ++j )
				if( items[j].GetType() == 5 )
				{
					if( items[j].GetMoreZ() == 3 )
					{
						items[j].SetMoreZ( 4 );
						items[j].SetZ( items[j].GetZ() + 17 );
						items[j].Dirty();
					}
					else if( items[j].GetMoreZ() == 4 )
					{
						items[j].SetMoreZ( 3 );
						items[j].SetZ( items[j].GetZ() - 17 );
						items[j].Dirty();
					}
				}
			return true;

			//Item type 6 in doubleclick() is yet another teleport item, should we remove?" )
		case 6:	// Teleport rune
			mSock->target( 0, TARGET_TELE, 401 );
			return true;
		case 7:	// Key
			mSock->AddID( x->GetMore() );
			mSock->target( 0, TARGET_KEY, 402 );
			return true;
		case 8:	// Locked container
		case 64: // locked spawn container
			mSock->sysmessage( 1428 );
			return true;
		case 9:	// Spellbook
			i = getPack( mChar );
			if( i != NULL )
			{
				if( ( x->GetCont() == i || x->GetCont() == mChar ) || x->GetLayer() == 1 )
					Magic->SpellBook( mSock );
				else
					mSock->sysmessage( 403 );
			}
			return true;
		case 10: // Map
			{
				CPMapMessage m1;
				CPMapRelated m2;
				m1.KeyUsed( x->GetSerial() );
				m1.GumpArt( 0x139D );
				m1.UpperLeft( 0, 0 );
				m1.LowerRight( 0x13FF, 0x0FA0 );
				m1.Dimensions( 0x0190, 0x0190 );
				mSock->Send( &m1 );

				m2.ID( x->GetSerial() );
				m2.Command( 5 );
				m2.Location( 0, 0 );
				m2.PlotState( 0 );
				mSock->Send( &m2 );
				return true;
			}
		case 11:	// Readable book
			if( x->GetMoreX() != 666 && x->GetMoreX() != 999 )
				Books->OpenPreDefBook( mSock, x );
			else
				Books->OpenBook( mSock, x, ( x->GetMoreX() == 666 ) ); 
			return true;
		case 12: // Unlocked door
			if( !isDoorBlocked( x ) )
				useDoor( mSock, x );
			else
				mSock->sysmessage( 1661 );
			return true;
		case 13: // Locked door
			if( mChar->IsGM() )
			{
				mSock->sysmessage( 404 );
				useDoor( mSock, x );
				return true;
			}
			if( !keyInPack( mSock, mChar, getPack( mChar ), x ) )
				mSock->sysmessage( 406 );
			return true;
		case 14: // Food
			if( mChar->GetHunger() >= 6 )
			{
				mSock->sysmessage( 407 );
				return true;
			}
			else
			{
				Effects->PlaySound( mChar, 0x003A + RandomNum( 0, 2 ) );
				if( mChar->GetHunger() >= 0 && mChar->GetHunger() <= 6 )
					mSock->sysmessage( 408 + mChar->GetHunger() );
				else
					mSock->sysmessage( 415 );

				if( x->GetPoisoned() && mChar->GetPoisoned() < x->GetPoisoned() )
				{
					mSock->sysmessage( 416 + RandomNum( 0, 2 ) );
					Effects->PlaySound( mChar, 0x0246 ); //poison sound - SpaceDog
					mChar->SetPoisoned( x->GetPoisoned() );
					mChar->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) )) );
					mChar->SendToSocket( mSock, true, mChar );
				}
				//Remove a food item
				x = DecreaseItemAmount( x );
				mChar->SetHunger( mChar->GetHunger() + 1 );
			}
			return true;
		case 15: // Magic Wands
			if( x->GetMoreZ() != 0 )
			{
				mSock->CurrentSpellType( 2 );
				if( Magic->SelectSpell( mSock, ( 8 * ( x->GetMoreX() - 1 ) ) + x->GetMoreY() ) )
				{
					x->SetMoreZ( x->GetMoreZ() - 1 );
					if( x->GetMoreZ() == 0 )
					{
						x->SetType( x->GetType2() );
						x->SetMoreX( 0 );
						x->SetMoreY( 0 );
						x->SetOffSpell( 0 );
					}
				}
			}
			return true;
		case 18: // Crystal Ball
			mSock->objMessage( 419 + RandomNum( 0, 9 ), x );
			Effects->PlaySound( mSock, 0x01EC, true );
			return true;
		case 19: // Potions
			if( mChar->IsUsingPotion() )
				mSock->sysmessage( 430 );
			else
				usePotion( mChar, x );
			return true;
		case 35: // Townstone and Townstone Deed
			if( x->GetID() == 0x14F0 )		// Check for Deed
			{
				CItem *c = Items->CreateScriptItem( mSock, "townstone", false, mChar->WorldNumber() );
				Items->DeleItem( x );
			}
			else	// Display Townstone gump
			{
				UI08 useRegion = calcRegionFromXY( x->GetX(), x->GetY(), mChar->WorldNumber() );
				region[useRegion]->DisplayTownMenu( x, mSock );
			}
			return true;

		case 50: // Recall Rune
			if( x->GetMoreX() == 0 && x->GetMoreY() == 0 && x->GetMoreZ() == 0 )	// changed, to fix, Lord Vader
				mSock->sysmessage( 431 );
			else
			{
				mSock->sysmessage( 432 );
				mChar->SetSpeechItem( x->GetSerial() );
				mChar->SetSpeechMode( 7 );
			}
			return true;
		case 51:	// Starting Moongate
		case 52:	// Ending Moongate
			if( objInRange( mChar, x, DIST_NEARBY ) )
			{
				CItem *otherGate = calcItemObjFromSer( x->GetMoreX() );
				if( otherGate != NULL )
				{
					mChar->SetLocation( otherGate );
					mChar->Teleport();
				}
			}
			return true;
		case 60:	// Object Teleporter
			if( objInRange( mChar, x, DIST_NEARBY ) )
			{
				mChar->SetLocation( static_cast<SI16>(x->GetMoreX()), static_cast<SI16>(x->GetMoreY()), static_cast<SI08>(x->GetMoreZ() ));
				mChar->Teleport();
			}
			return true;
		case 89:
			SocketMapChange( mSock, mChar, x );
			return true;
		case 101: // Morph Object
			mChar->SetID( (UI16)x->GetMoreX() );
			mChar->Teleport();
			x->SetType( 102 );
			return true;
		case 102: // Unmorph Object
			mChar->SetID( mChar->GetOrgID() );
			mChar->Teleport();
			x->SetType( 101 );
			return true;
		case 104: // Teleport object
			mChar->SetLocation( static_cast<SI16>(x->GetMoreX()), static_cast<SI16>(x->GetMoreY()), static_cast<SI08>(x->GetMoreZ() ));
			mChar->Teleport();
			return true;
		case 105:  // Drink
			Effects->PlaySound( mChar, 0x30 + RandomNum( 0, 1 ) );
			if( RandomNum( 0, 1 ) )
				mChar->talk( mSock, 435, false );
			else
				mChar->emote( mSock, 436, false );

			if( x->GetAmount() > 1 )
				DecreaseItemAmount( x );
			else if( !(RandomNum( 0, 4 )) || x->GetAmount() == 1 )	// they emptied it.
			{
				x->SetType( 0 );
				x->SetAmount( 0 );
				mSock->sysmessage( 437 );
				switch( itemID )
				{
					case 0x09F0:
					case 0x09AD:
					case 0x0FF8:
					case 0x0FF9: 
					case 0x1F95:
					case 0x1F96:
					case 0x1F97:
					case 0x1F98:
					case 0x1F99:
					case 0x1F9A:
					case 0x1F9B:
					case 0x1F9C:
					case 0x1F9D:
					case 0x1F9E://Pitchers
						x->SetID( 0x09A7 );
						break;

					case 0x09EE:
					case 0x09EF:
					case 0x1F7D:
					case 0x1F7E:
					case 0x1F7F:
					case 0x1F80:
					case 0x1F85:
					case 0x1F86:
					case 0x1F87:
					case 0x1F88:
					case 0x1F89:
					case 0x1F8A:
					case 0x1F8B:
					case 0x1F8C:
					case 0x1F8D:
					case 0x1F8E:
					case 0x1F8F:
					case 0x1F90:
					case 0x1F91:
					case 0x1F92:
					case 0x1F93:
					case 0x1F94://glasses
						x->SetID( 0x1F84 );
						break;
				}
			}
			return true;
		case 117:	// Planks
			if( objInRange( mChar, x, DIST_INRANGE ) )
			{
				if( x->GetID( 2 ) == 0x84 || x->GetID( 2 ) == 0xD5 || x->GetID( 2 ) == 0xD4 || x->GetID( 2 ) == 0x89 )
					Boats->PlankStuff( mSock, x );
				else
					mSock->sysmessage( 398 );//Locked
			}
			else
				mSock->sysmessage( 399 );
			return true;
		case 181: //Fireworks wands
			srand( cwmWorldState->GetUICurrentTime() );
			if( x->GetMoreX() == 0 )
			{
				mSock->sysmessage( 396 );
				return true;
			}
			x->SetMoreX( x->GetMoreX() - 1 );
			mSock->sysmessage( 397, x->GetMoreX() );
			UI08 j;
			for( j = 0; j < static_cast< UI08 >( RandomNum( 0, 3 ) + 2 ); ++j )
			{
				SI16 wx = ( mChar->GetX() + RandomNum( 0, 10 ) - 5 );
				SI16 wy = ( mChar->GetY() + RandomNum( 0, 10 ) - 5 );
				Effects->movingeffect( mChar, wx, wy, mChar->GetZ() + 10, 0x36E4, 17, 0, ( RandomNum( 0, 1 ) == 1 ) );
				switch( RandomNum( 0, 4 ) )
				{
					case 0:	Effects->staticeffect( wx, wy, mChar->GetZ() + 10, 0x373A, 0x09, 0, 0 );		break;
					case 1:	Effects->staticeffect( wx, wy, mChar->GetZ() + 10, 0x374A, 0x09, 0, 0 );		break;
					case 2:	Effects->staticeffect( wx, wy, mChar->GetZ() + 10, 0x375A, 0x09, 0, 0 );		break;
					case 3:	Effects->staticeffect( wx, wy, mChar->GetZ() + 10, 0x376A, 0x09, 0, 0 );		break;
					case 4:	Effects->staticeffect( wx, wy, mChar->GetZ() + 10, 0x377A, 0x09, 0, 0 );		break;
				}
			}
			return true;
		case 186: // Rename Deed
			mChar->SetSpeechItem( x->GetSerial() );
			mChar->SetSpeechMode( 6 );
			mSock->sysmessage( 434 );
			return true;
		case 190:	// Leather repair tool
			mSock->target( 0, TARGET_REPAIRLEATHER, 485 );	// What do we wish to repair?
			return true;
		case 191:	// Bow repair tool
			mSock->target( 0, TARGET_REPAIRBOW, 485 );	// What do we wish to repair?
			return true;
		case 200:	// Tillerman
			if( Boats->GetBoat( mSock ) == NULL )
			{
				CItem *boat = calcItemObjFromSer( x->GetMore() );
				Boats->ModelBoat( mSock, boat );
			}
			return true;
		case 202:	// Guildstone Deed
			if( itemID == 0x14F0 || itemID == 0x1869 )	// Check for Deed/Teleporter + Guild Type
			{
				mChar->SetSpeechItem( x->GetSerial() );		// we're using this as a generic item
				GuildSys->PlaceStone( mSock, x );
				return true;
			}
			else if( itemID == 0x0ED5 )			// Check for Guildstone + Guild Type
			{
				mSock->TempInt( x->GetMore() );	// track things properly
				if( mChar->GetGuildNumber() == -1 || mChar->GetGuildNumber() == x->GetMore() )
					GuildSys->Menu( mSock, BasePage + 1, static_cast<GUILDID>(x->GetMore()) );	// more of the stone is the guild number
				else
					mSock->sysmessage( 438 );
				return true;
			}
			else
				Console << "Unhandled guild item type named: " << x->GetName() << " with ID of: " << itemID << myendl;
			return true;
		case 203: // Open Housing Gump
			bool canOpen;
			canOpen = (  x->GetOwnerObj() == mChar || mChar->IsGM() );
			if( !canOpen && x->GetMoreZ() == 0 )
			{
				mSock->sysmessage( 439 );
				return true;
			}
			mSock->AddID( x->GetSerial() );
			if( !canOpen )
				Gumps->Menu( mSock, x->GetMoreZ() );
			else
				Gumps->Menu( mSock, x->GetMoreX() );
			return true;
		case 204:	// tinker's tools
			mSock->target( 0, TARGET_TINKERING, 484 );
			return true;
		case 205:
			mSock->target( 0, TARGET_REPAIRMETAL, 485 );	// What do we wish to repair?
			return true;
		case 207:	// Forges
			mSock->target( 0, TARGET_SMELT, 440 );
			return true;
		case 208:	// Dye
			mSock->DyeAll( 0 );
			mSock->target( 0, TARGET_DYEALL, 441 );
			return true;
		case 209:	// Dye vat
			mSock->AddID1( x->GetColour( 1 ) );
			mSock->AddID2( x->GetColour( 2 ) );
			mSock->target( 0, TARGET_DVAT, 442 );
			return true;
		case 210:	// Model boat/Houses
			if( iType != 103 && iType != 202 )
			{
				if( x->GetMoreX() == 0 )
					break;
				mChar->SetSpeechItem( x->GetSerial() );
				mSock->AddID3( static_cast<UI08>(x->GetMoreX()) );
				buildHouse( mSock, static_cast<UI08>(x->GetMoreX()) );
			}
			return true;
		case 211:	// Archery buttle
			Skills->AButte( mSock, x );
			return true;
		case 212:	// Drum
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x0038 );
			else
				Effects->PlaySound( mChar, 0x0039 );
			return true;
		case 213:	// Tambourine
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x0052 );
			else
				Effects->PlaySound( mChar, 0x0053 );
			return true;
		case 214:	// Harps
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x0045 );
			else
				Effects->PlaySound( mChar, 0x0046 );
			return true;
		case 215:	// Lute
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x004C );
			else
				Effects->PlaySound( mChar, 0x004D );
			return true;
		case 216:	// Axes
			mSock->target( 0, TARGET_AXE, 443 );
			return true;
		case 217:	//Player Vendor Deeds
			CChar *m;
			m = Npcs->AddNPCxyz(NULL, "playervendor", mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->WorldNumber() );
			m->SetNPCAiType( aiPLAYERVENDOR );
			m->SetInvulnerable( true );
			m->SetHidden( 0 );
			m->SetDir( mChar->GetDir() );
			m->SetNpcWander( 0 );
			m->SetFlag( m->GetFlag()^7 );
			m->SetOwner( (cBaseObject *)mChar );
			Items->DeleItem( x );
			m->Teleport();
			m->talk( mSock, 388, false, m->GetName().c_str() );
			return true;
		case 218:	// Smithy
			mSock->target( 0, TARGET_SMELTORE, 444 );
			return true; 
		case 219:	// Carpentry
			mSock->target( 0, TARGET_CARPENTRY, 445 );
			return true;
		case 220:	// Mining
			mSock->target( 0, TARGET_MINE, 446 );
			return true; 
		case 221:	// empty vial
			i = getPack( mChar );
			if( i != NULL )
			{
				if( x->GetCont() == i )
				{
					mSock->AddMItem( x );
					mSock->target( 0, TARGET_VIAL, 447 );
				}
				else
					mSock->sysmessage( 448 );
			}
			return true;
		case 222:	// wool to yarn/cotton to thread
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_WHEEL, 449 );
			return true;
		case 223:	// cooking fish
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_COOKING, 450 );
			return true;
		case 224:	//
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_COOKING, 451 );
			return true;
		case 225:	// yarn/thread to cloth
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_LOOM, 452 );
			return true;
		case 226:	// make shafts
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_FLETCHING, 454 );
			return true;
		case 227:	// cannon ball
			mSock->target( 0, TARGET_LOADCANNON, 455 );
			Items->DeleItem( x );
			return true;
		case 228:	// pitcher of water to flour
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_COOKING, 456 );
			return true;
		case 229:	// sausages to dough
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_COOKING, 457 );
			return true;
		case 230:	// sewing kit for tailoring
			mSock->target( 0, TARGET_TAILORING, 459 );
			return true;
		case 231:	// smelt ore
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_SMELTORE, 460 );
			return true;
		case 232:	// Message board opening
			if( objInRange( mChar, x, DIST_NEARBY ) )
				MsgBoardEvent( mSock );
			else
				mSock->sysmessage( 461 );
			return true;
		case 233:	// skinning
			mSock->target( 0, TARGET_SWORD, 462 );
			return true;
		case 234:	// camping
			if( objInRange( mChar, x, DIST_INRANGE ) )
			{
				if( Skills->CheckSkill( mChar, CAMPING, 0, 500 ) ) // Morrolan TODO: insert logout code for campfires here
				{
					i = Items->SpawnItem( NULL, mChar, 1, "#", false, 0x0DE3, 0, false, false );
					if( i == NULL )
						return true;
					i->SetType( 45 );
					i->SetDir( 2 );
					i->SetHP( 1 );		// only want 1 HP to decay straight away

					if( x->GetCont() == NULL )
						i->SetLocation( x );
					else
						i->SetLocation( mChar );
					i->SetDecayable( true );
					i->SetDecayTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) )) );
					DecreaseItemAmount( x );
				}
				else
					mSock->sysmessage( 463 );
			}
			else
				mSock->sysmessage( 393 );
			return true;
		case 235:	// magic statue?
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1509 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 465 );
			return true;
		case 236:	// guillotines?
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1245 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case 237:	// Guillotine stop animation
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1230 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case 238:	// closed flour sack
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x103A );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case 239:	// open flour sack
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1039 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case 240:	// fishing
			if( mChar->GetTimer( tPC_FISHING ) )
				mSock->sysmessage( 467 );
			else
				mSock->target( 0, TARGET_FISH, 468 );
			return true;
		case 241:	// clocks
			telltime( mSock );
			return true;
		case 242:	// Mortar for Alchemy
			if( !mChar->SkillUsed( ALCHEMY ) )
				mSock->target( 0, TARGET_ALCHEMY, 470 );
			else
				mSock->sysmessage( 1631 );
			return true;
		case 243:	// scissors
			mSock->target( 0, TARGET_CREATEBANDAGE, 471 );
			return true;
		case 244:	// healing
			if( mChar->GetTimer( tPC_SKILLDELAY ) <= cwmWorldState->GetUICurrentTime() )
			{
				mSock->AddMItem( x );
				mSock->target( 0, TARGET_HEALING, 472 );
				mChar->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetServerSkillDelayStatus() )) );
			}
			else
				mSock->sysmessage( 473 );
			return true;
		case 245:	// sextants
			mSock->sysmessage( 474, mChar->GetX(), mChar->GetY() );
			return true;
		case 246:	// Hair Dye
			mSock->AddID( x->GetSerial() );
			Gumps->Menu( mSock, 6 );
			return true;
		case 247:	// lockpicks
			mSock->AddMItem( x );
			mSock->target( 0, TARGET_LOCKPICK, 475 );
			return true;
		case 248:	// cotton plants
			if( !mChar->IsOnHorse() )
				Effects->action( mSock, 0x0D );
			else
				Effects->action( mSock, 0x1D );
			Effects->PlaySound( mSock, 0x013E, true );
			i = Items->SpawnItem( mSock, mChar, 1, "#", true, 0x0DF9, 0, true, true );
			if( i == NULL )
				return true;
			CItem *pack;
			pack = getPack( mChar );
			if( pack != NULL )
				i->SetCont( pack );
			CItem *skillItem;
			skillItem = mChar->GetSkillItem();
			if( skillItem != NULL )
			{
				skillItem->SetDecayable( true );
				mSock->sysmessage( 476 );
				mChar->SetSkillItem( NULL );
			}
			return true; // cotton
		case 249:	// tinker axle
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_TINKERAXEL, 477 );
			return true;
		case 250:
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_TINKERAWG, 478 );
			return true;
		case 251:	// tinker clock
			mSock->target( 0, TARGET_TINKERCLOCK, 479 );
			Items->DeleItem( x );
			return true;
		case 252:	//tinker sextant
			if( Skills->CheckSkill( mChar, TINKERING, 0, 1000 ) )
			{
				mSock->sysmessage( 480 );
				i = Items->SpawnItem( mSock, mChar, 1, Dictionary->GetEntry( 1429 ), false, 0x1057, 0, true, true );
				if( i == NULL )
					return true;
				pack = getPack( mChar );
				i->SetCont( pack );
				CItem *skillItem;
				skillItem = mChar->GetSkillItem();
				if( skillItem != NULL )
				{
					skillItem->SetDecayable( true );
					Items->DeleItem( x );
					mChar->SetSkillItem( NULL );
				}
			}
			else
				mSock->sysmessage( 481 );
			return true;
		case 253:	// training dummies
			if( objInRange( mChar, x, DIST_NEXTTILE ) )
				Skills->TDummy( mSock );
			else
				mSock->sysmessage( 482 );
			return true;
		case 254:	// swinging training dummy
			mSock->sysmessage( 483 );
			return true;
		default:
			if( iType )
				Console << "Unhandled item type for item: " << x->GetName() << "[" << x->GetSerial() << "] of type: " << iType << myendl;
			break;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool handleDoubleClickIDs( cSocket *mSock, CChar *mChar, CItem *x, UI16 itemID )
//|   Date        :  2/11/2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Runs a switch to match an item ID to a function.
//o---------------------------------------------------------------------------o
bool handleDoubleClickIDs( cSocket *mSock, CChar *mChar, CItem *x, UI16 itemID )
{
	CItem *i = NULL;
	UI08 iType = x->GetType();
	switch( itemID )
	{
		case 0x0FB1: // small forge
		case 0x1982: // partial lg forge
		case 0x197A: // partial lg forge
		case 0x197E: // partial lg forge
			mSock->target( 0, TARGET_SMELT, 440 );
			return true; 
		case 0x0FA9:// Dye
			mSock->DyeAll( 0 );
			mSock->target( 0, TARGET_DYEALL, 441 );
			return true;
		case 0x0FAB:	// Dye vat
			mSock->AddID1( x->GetColour( 1 ) );
			mSock->AddID2( x->GetColour( 2 ) );
			mSock->target( 0, TARGET_DVAT, 442 );
			return true;
		case 0x14F4:	// Model boat
		case 0x14F0:	// Houses
			if( iType != 103 && iType != 202 )
			{  //experimental house code
				if( x->GetMoreX() == 0 )
					break;
				mChar->SetSpeechItem( x->GetSerial() );
				mSock->AddID3( static_cast<UI08>(x->GetMoreX()) );
				buildHouse( mSock, static_cast<UI08>(x->GetMoreX()) );
			}
			return true;
		case 0x100A:
		case 0x100B:// Archery butte
			Skills->AButte( mSock, x );
			return true;
		case 0x0E9C: // Drum
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x0038 );
			else
				Effects->PlaySound( mChar, 0x0039 );
			return true;
		case 0x0E9D: // Tambourine
		case 0x0E9E:
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x0052 );
			else
				Effects->PlaySound( mChar, 0x0053 );
			return true;
		case 0x0EB1: // Standing harp
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x0043 );
			else
				Effects->PlaySound( mChar, 0x0044 );
			return true;
		case 0x0EB2: // Lap harp
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x0045 );
			else
				Effects->PlaySound( mChar, 0x0046 );
			return true;
		case 0x0EB3: // Lute
		case 0x0EB4: // Lute
			if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) )
				Effects->PlaySound( mChar, 0x004C );
			else
				Effects->PlaySound( mChar, 0x004D );
			return true;
			// SMITHY
		case 0x102A: // hammer
		case 0x102B: // hammer
		case 0x0FB4:// sledge hammer
		case 0x0FB5:// sledge hammer
		case 0x0FBB:// tongs
		case 0x0FBC:// tongs
		case 0x13E3:// smith's hammer
		case 0x13E4:// smith's hammer
			mSock->target( 0, TARGET_SMITH, 444 );
			return true; 
			// CARPENTRY
		case 0x1026: // chisels
		case 0x1027: // chisels
		case 0x1028:// Dove Tail Saw
		case 0x1029:// Dove Tail Saw
		case 0x102C:// Moulding Planes
		case 0x102D:// moulding planes
		case 0x102E: // nails - carp. tool but not sure if this works here?
		case 0x102F: // nails - carp. tool but not sure if this works here?
		case 0x1030:// jointing plane
		case 0x1031:// jointing plane
		case 0x1032:// smoothing plane
		case 0x1033:// smoothing plane
		case 0x1034:	// saw
		case 0x1035: // saw
		case 0x10E4: // draw knife
		case 0x10E5: // froe - carp. tool but not sure if this works here?
		case 0x10E6: // inshave - carp. tool but not sure if this works here?
//		case 0x10E7: // scorp - carp. tool but not sure if this works here?
			mSock->target( 0, TARGET_CARPENTRY, 445 );
			return true;
			// MINING
		case 0x0E85:// pickaxe
		case 0x0E86:// pickaxe
		case 0x0F39:// shovel
		case 0x0F3A:// shovel
			mSock->target( 0, TARGET_MINE, 446 );
			return true; 
		case 0x0E24: // empty vial
			i = getPack( mChar );
			if( i != NULL )
			{
				if( x->GetCont() == i )
				{
					mSock->AddMItem( x );
					mSock->target( 0, TARGET_VIAL, 447 );
				}
				else
					mSock->sysmessage( 448 );
			}
			return true;
		case 0x0DF8: // wool to yarn 
		case 0x0DF9: // cotton to thread
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_WHEEL, 449 );
			return true;
			// Sept 22, 2002 - Xuri 
			//case 0x09CC: // raw fish       -- krazyglue: added support for cooking raw fish
			//case 0x09CD: // raw fish          should they have to fillet them first then
			//case 0x09CE: // raw fish          cook the fillet pieces?  or is this good enough?
			//case 0x09CF: // raw fish
			//	mChar->SetSkillItem( x );
			//	mSock->target( 0, TARGET_COOKING, 450 );
			//	return true;
			//
		case 0x09F1: // 
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_COOKING, 451 );
			return true;
		case 0x0FA0:
		case 0x0FA1: // thread to cloth
		case 0x0E1D:
		case 0x0E1F:
		case 0xE11E:  // yarn to cloth
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_LOOM, 452 );
			return true;
		case 0x1BD1:
		case 0x1BD2:
		case 0x1BD3:
		case 0x1BD4:
		case 0x1BD5:
		case 0x1BD6:	// make shafts
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_FLETCHING, 454 );
			return true;
		case 0x0E73: // cannon ball
			mSock->target( 0, TARGET_LOADCANNON, 455 );
			Items->DeleItem( x );
			return true;
		case 0x0FF8:
		case 0x0FF9: // pitcher of water to flour
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_COOKING, 456 );
			return true;
		case 0x09C0:
		case 0x09C1: // sausages to dough
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_COOKING, 457 );
			return true;
		case 0x0F9D: // sewing kit for tailoring
			mSock->target( 0, TARGET_TAILORING, 459 );
			return true;
		case 0x19B7:
		case 0x19B9:
		case 0x19BA:
		case 0x19B8: // smelt ore
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_SMELTORE, 460 );
			return true;
		case 0x1E5E:
		case 0x1E5F: // Message board opening
			if( objInRange( mChar, x, DIST_NEARBY ) )
				MsgBoardEvent( mSock );
			else
				mSock->sysmessage( 461 );
			return true;
		case 0x0DE1:
		case 0x0DE2: // camping
			if( objInRange( mChar, x, DIST_INRANGE ) )
			{
				if( Skills->CheckSkill( mChar, CAMPING, 0, 500 ) ) // Morrolan TODO: insert logout code for campfires here
				{
					i = Items->SpawnItem( NULL, mChar, 1, "#", false, 0x0DE3, 0, false, false );
					if( i == NULL )
						return true;
					i->SetType( 45 );
					i->SetDir( 2 );

					if( x->GetCont() == NULL )
						i->SetLocation( x );
					else
						i->SetLocation( mChar );
					i->SetDecayable( true );
					i->SetDecayTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) )) );
					DecreaseItemAmount( x );
				}
				else
					mSock->sysmessage( 463 );
			}
			else
				mSock->sysmessage( 393 );
			return true;
		case 0x1508: // magic statue?
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1509 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 465 );
			return true;
		case 0x1509:
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1508 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 465 );
			return true;
		case 0x1230:
		case 0x1246: // guillotines?
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1245 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case 0x1245: // Guillotine stop animation
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1230 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case 0x1039:  // closed flour sack
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x103A );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case 0x103A: // open flour sack
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
			{
				x->SetID( 0x1039 );
				x->SetType( 45 );
			}
			else
				mSock->sysmessage( 466 );
			return true;
		case 0x0DBF:	// fishing pole
		case 0x0DC0:	// fishing pole
		case 0x0DC8:	// fishing nets
		case 0x0DC9:
		case 0x0DCA:
		case 0x0DCB:
		case 0x0DD0:
		case 0x0DD1:
		case 0x0DD2:
		case 0x0DD3:
		case 0x0DD4:
		case 0x0DD5:
		case 0x1EA3:	// nets
		case 0x1EA4:
		case 0x1EA5:
		case 0x1EA6:
			if( mChar->GetTimer( tPC_FISHING ) )
				mSock->sysmessage( 467 );
			else
				mSock->target( 0, TARGET_FISH, 468 );
			return true;
		case 0x104B: // clock
		case 0x104C: // clock
		case 0x1086: // bracelet
			telltime( mSock );
			return true;
		case 0x0E9B: // Mortar for Alchemy
			if( !mChar->SkillUsed( ALCHEMY ) )
				mSock->target( 0, TARGET_ALCHEMY, 470 );
			else
				mSock->sysmessage( 1631 );
			return true;
		case 0x0F9E:
		case 0x0F9F: // scissors
			mSock->target( 0, TARGET_CREATEBANDAGE, 471 );
			return true;
		case 0x0E21: // healing
			if( mChar->GetTimer( tPC_SKILLDELAY ) <= cwmWorldState->GetUICurrentTime() )
			{
				mSock->AddMItem( x );
				mSock->target( 0, TARGET_HEALING, 472 );
				mChar->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetServerSkillDelayStatus() )) );
			}
			else
				mSock->sysmessage( 473 );
			return true;
		case 0x1057:
		case 0x1058: // sextants
			mSock->sysmessage( 474, mChar->GetX(), mChar->GetY() );
			return true;
		case 0x0E27:
		case 0x0EFF:   // Hair Dye
			mSock->AddID( x->GetSerial() );
			Gumps->Menu( mSock, 6 );
			return true;
		case 0x14FB:
		case 0x14FC:
		case 0x14FD:
		case 0x14FE: // lockpicks
			mSock->AddMItem( x );
			mSock->target( 0, TARGET_LOCKPICK, 475 );
			return true;
		case 0x0C4F:
		case 0x0C50:
		case 0x0C51:
		case 0x0C52:
		case 0x0C53:
		case 0x0C54: // cotton plants
			if( !mChar->IsOnHorse() )
				Effects->action( mSock, 0x0D );
			else
				Effects->action( mSock, 0x1D );
			Effects->PlaySound( mSock, 0x013E, true );
			i = Items->SpawnItem( mSock, mChar, 1, "#", true, 0x0DF9, 0, true, true );
			if( i == NULL )
				return true;
			CItem *pack;
			pack = getPack( mChar );
			if( pack != NULL )
				i->SetCont( pack );
			CItem *skillItem;
			skillItem = mChar->GetSkillItem();
			if( skillItem != NULL )
			{
				skillItem->SetDecayable( true );
				mSock->sysmessage( 476 );
				mChar->SetSkillItem( NULL );
			}
			return true; // cotton
		case 0x105B:
		case 0x105C:
		case 0x1053:
		case 0x1054: // tinker axle
			mSock->AddID( x->GetSerial() );
			mSock->target( 0, TARGET_TINKERAXEL, 477 );
			return true;
		case 0x1051:
		case 0x1052:
		case 0x1055:
		case 0x1056:
		case 0x105D:
		case 0x105E:
			mChar->SetSkillItem( x );
			mSock->target( 0, TARGET_TINKERAWG, 478 );
			return true;
		case 0x104F:
		case 0x1050:
		case 0x104D:
		case 0x104E:// tinker clock
			mSock->target( 0, TARGET_TINKERCLOCK, 479 );
			Items->DeleItem( x );
			return true;
		case 0x1059:
		case 0x105A://tinker sextant
			if( Skills->CheckSkill( mChar, TINKERING, 0, 1000 ) )
			{
				mSock->sysmessage( 480 );
				i = Items->SpawnItem( mSock, mChar, 1, Dictionary->GetEntry( 1429 ), false, 0x1057, 0, true, true );
				if( i == NULL )
					return true;
				CItem *pack;
				pack = getPack( mChar );
				if( pack != NULL )
					i->SetCont( pack );
				CItem *skillItem;
				skillItem = mChar->GetSkillItem();
				if( skillItem != NULL )
				{
					skillItem->SetDecayable( true );
					Items->DeleItem( x );
					mChar->SetSkillItem( NULL );
				}
			}
			else
				mSock->sysmessage( 481 );
			return true;
		case 0x1070:
		case 0x1074: // training dummies
			if( objInRange( mChar, x, DIST_NEXTTILE ) )
				Skills->TDummy( mSock );
			else
				mSock->sysmessage( 482 );
			return true;
		case 0x1071:
		case 0x1073:
		case 0x1075:
		case 0x1077:// swinging training dummy
			mSock->sysmessage( 483 );
			return true;
		case 0x1EBC: // tinker's tools
			mSock->target( 0, TARGET_TINKERING, 484 );
			return true;
		case 0x0FAF:
		case 0x0FB0:
			mSock->target( 0, 24, 485 );	// What do we wish to repair?
			return true;
		case 0x0EC1:	// Leather repair tool
			mSock->target( 0, TARGET_REPAIRLEATHER, 485 );	// What do we wish to repair?
			return true;
		case 0x10E7:	// Bow repair tool
			mSock->target( 0, TARGET_REPAIRBOW, 485 );	// What do we wish to repair?
			return true;
		default:
			break;
	}
	return false;
}
//o---------------------------------------------------------------------------o
//|	Function	:  DoubleClick( cSocket *mSock )
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
	SERIAL serial	= tSock->GetDWord( 1 );
	UI08 a1			= tSock->GetByte( 1 );

	bool keyboard = ( (a1 & 0x80) == 0x80 );
	if( keyboard )
		serial -= 0x80000000;

	if( serial < BASEITEMSERIAL )
	{
		handleCharDoubleClick( tSock, serial, keyboard );
		return true;
	}

	// Begin Items / Guildstones Section
	CItem *x = calcItemObjFromSer( serial );
	if( x == NULL )
		return true;

	CChar *iChar = NULL;

	// Check for Object Delay
	if( ( ourChar->GetTimer( tPC_OBJDELAY ) >= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) && !ourChar->IsGM() )
	{
		tSock->sysmessage( 386 );
		return true;
	}
	else
		ourChar->SetTimer( tPC_OBJDELAY, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( OBJECT_USAGE ) )) );

	UI08 iType = x->GetType();
	UI16 itemID = x->GetID();
	if( x->GetCont() != NULL )
	{
		if( x->GetContSerial() >= BASEITEMSERIAL )
			iChar = getPackOwner( x );
		else	// Item is in a character
			iChar = (CChar *)x->GetCont();
		if( !ourChar->IsGM() && iChar != NULL && iChar != ourChar && !( x->IsContType() && cwmWorldState->ServerData()->GetRogueStatus() ) )
		{
			tSock->sysmessage( 387 );	// Can't use stuff that isn't in your pack.
			return true;
		}
	}
	// Check if item is in a house?
	else if( iType != 12 && iType != 13 && iType != 117 )
	{
		if( !ourChar->IsGM() && x->GetMultiObj() != ourChar->GetMultiObj() )
		{
			tSock->sysmessage( 389 );
			return true;
		}
	}

	if( ourChar->IsDead() )
	{
		if( iType == 16 )	// Check for a resurrect item type
		{
			Targ->NpcResurrectTarget( ourChar );
			tSock->sysmessage( 390 );
			return true;
		}
		else  // If it's not a ressurect item, and you're dead, forget it
		{
			tSock->sysmessage( 392 );
			return true;
		}
	}
	// Begin checking objects that we force an object delay for (std objects)
	else if( tSock != NULL )
	{
		UI16 itemTrig = x->GetScriptTrigger();
		cScript *toExecute = Trigger->GetScript( itemTrig );
		if( toExecute != NULL )
		{
			// on ground and not in range
			if( x->GetCont() == NULL && !objInRange( ourChar, x, DIST_INRANGE ) )
			{
				tSock->sysmessage( 393 );
				return true;
			}
			if( x->IsDisabled() )
			{
				tSock->sysmessage( 394 );
				return true;
			}
			if( toExecute->OnUse( ourChar, x ) == 1 )	// if it exists and we don't want hard code, return
				return true;
		}
		//check this on trigger in the event that the .trigger property is not set on the item
		//trigger code.  Check to see if item is envokable by id
		else if( Trigger->CheckEnvoke( itemID ) )
		{
			UI16 envTrig = Trigger->GetScriptFromEnvoke( itemID );
			cScript *envExecute = Trigger->GetScript( envTrig );
			if( envExecute->OnUse( ourChar, x ) == 1 )	// if it exists and we don't want hard code, return
				return true;
		}
	}
	// Range check for double clicking on items
	if( iType != 1 && iType != 117 && iType != 203 && !checkItemRange( ourChar, x, DIST_NEARBY ) )
	{
		tSock->sysmessage( 389 );
		return true;
	}
	if( x->isCorpse() && !ourChar->IsGM() && !ourChar->IsCounselor() )
	{
		bool willCrim	= false;
		iChar			= (CChar *)x->GetOwnerObj();
		if( iChar != NULL )
		{
			bool pInnocent	= iChar->IsInnocent();
			bool guildCheck	= GuildSys->ResultInCriminal( ourChar, iChar ); 
			bool raceCheck	= ( Races->Compare( ourChar, iChar ) == 0 );
			bool otherCheck	= ( pInnocent && iChar->GetSerial() != ourChar->GetSerial() ); 
			// if the corpse is from an innocent player, and is not our own corpse				if( otherCheck
			// and if the corpse is not from an enemy/allied guild									&& guildCheck
			// and if the races are not allied/enemy												&& raceCheck )
			willCrim = guildCheck && raceCheck && otherCheck;
		}
		else
			willCrim = ( (x->GetMoreZ()&0x04) == 0x04 );
		if( willCrim )
			criminal( ourChar );

		if( iChar != NULL )
		{
			if( iChar->IsGuarded() ) // Is the corpse being guarded?
				Combat->petGuardAttack( ourChar, iChar, iChar->GetSerial() );
			else if( x->isGuarded() )
				Combat->petGuardAttack( ourChar, iChar, x->GetSerial() );
		}
	}
	else if( x->isGuarded() )
	{
		CMultiObj *multi = findMulti( x );
		if( multi != NULL )
		{
			if( multi->GetOwnerObj() != ourChar )
				Combat->petGuardAttack( ourChar, (CChar *)multi->GetOwnerObj(), x->GetSerial() );
		}
		else
		{
			iChar = getPackOwner( x );
			if( iChar != NULL && iChar->GetSerial() != ourChar->GetSerial() )
				Combat->petGuardAttack( ourChar, iChar, x->GetSerial() );
		}
	}
	if( handleDoubleClickTypes( tSock, ourChar, x, iType ) )
		return true;
	if( handleDoubleClickIDs( tSock, ourChar, x, itemID ) )
		return true;

	switch( Combat->getWeaponType( x ) )	// Check weapon-types (For carving/chopping)
	{
		case DEF_SWORDS:
		case SLASH_SWORDS:
		case ONEHND_LG_SWORDS:
		case ONEHND_AXES:
			tSock->target( 0, TARGET_SWORD, 462 );
			return true;
		case DEF_FENCING:
			if( itemID == 0x0F51 || itemID == 0x0F52 ) //dagger
				tSock->target( 0, TARGET_SWORD, 462 );
			return true;
		case TWOHND_LG_SWORDS:
		case BARDICHE:
		case TWOHND_AXES:
			tSock->target( 0, TARGET_SWORD, 443 );
			return true;
	}

	//	Begin Scrolls check
	if( x->GetID( 1 ) == 0x1F && ( x->GetID( 2 ) > 0x2C && x->GetID( 2 ) < 0x6D ) )
	{
		bool success = false;
		tSock->CurrentSpellType( 1 );	// spell from scroll
		if( x->GetID( 2 ) == 0x2D )	// Reactive Armor spell scrolls
			success = Magic->SelectSpell( tSock, 7 );
		if( x->GetID( 2 ) >= 0x2E && x->GetID( 2 ) <= 0x34 )  // first circle spell scrolls
			success = Magic->SelectSpell( tSock, x->GetID( 2 ) - 0x2D );
		else if( x->GetID( 2 ) >= 0x35 && x->GetID( 2 ) <= 0x6C )  // 2 to 8 circle spell scrolls
			success = Magic->SelectSpell( tSock, x->GetID( 2 ) - 0x2D + 1 );

		if( success )
			DecreaseItemAmount( x );
		return true;
	}
	tSock->sysmessage( 486 );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  char *AppendData( CItem *i, char *currentName )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Add data onto the end of the string in singleclick() based
//|					 on an items type
//o---------------------------------------------------------------------------o
char *AppendData( CItem *i, char *currentName )
{
	char temp[256];
	switch( i->GetType() )
	{
		case 1:
		case 12:
		case 63:
		case 65:
			strcpy( temp, currentName );
			sprintf( currentName, "%s (%u items, %i stones)", temp, i->NumItems(), ( i->GetWeight() / 100 ) );
			break;
		case 8:		// containers
		case 13:
		case 64:	// spawn containers
			strcpy( temp, currentName );
			sprintf( currentName, "%s (%u items, %i stones) [Locked]", temp, i->NumItems(), ( i->GetWeight() / 100 ) );
			break;
		case 50:
		case 51:
		case 52:
		case 60:
			strcpy( temp, currentName );
			UI08 newRegion;
			newRegion = calcRegionFromXY( static_cast<SI16>(i->GetMoreX()), static_cast<SI16>(i->GetMoreY()), i->WorldNumber() );
			sprintf( currentName, "%s (%s)", temp, region[newRegion]->GetName().c_str() );
	}
	// Question: Do we put the creator thing here, saves some redundancy a bit later
	return currentName;
}

//o---------------------------------------------------------------------------o
//|   Function    :  SingleClick( cSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item or character is single-clicked (also
//|					 used for AllNames macro)
//o---------------------------------------------------------------------------o
bool CPISingleClick::Handle( void )
{
	char temp2[200];
	std::string realname;
	char temp[512];
	
	SERIAL serial = tSock->GetDWord( 1 );

	if( serial == INVALIDSERIAL ) // invalid
		return true;	// don't bother if it's cancelled
	CChar *mChar = tSock->CurrcharObj();

	if( serial < BASEITEMSERIAL )
	{
		// Begin chars/npcs section
		CChar *c = calcCharObjFromSer( serial );
		if( c != NULL )
			tSock->ShowCharName( c, 0 );
		//End chars/npcs section
		return true;
	}

	//Begin items section
	UI08 a1		= tSock->GetByte( 1 );
	UI08 a2		= tSock->GetByte( 2 );
	UI08 a3		= tSock->GetByte( 3 );
	UI08 a4		= tSock->GetByte( 4 );
	CItem *i	= calcItemObjFromSer( serial );
	if( i == NULL )		// invalid item
		return true;
	// October 6, 2002 - Brakhtus - Added support for the onClick event
	cScript *onClickScp = Trigger->GetScript( i->GetScriptTrigger() );
	if( onClickScp != NULL )
		onClickScp->OnClick( tSock, i );

	if( mChar->GetSingClickSer() )
	{
		sprintf( temp, "[%x %x %x %x]", a1, a2, a3, a4 );
		tSock->objMessage( AppendData( i, temp ), i );
	}

	UI16 getAmount = i->GetAmount();
	if( i->GetCont() != NULL && i->GetContSerial() >= BASEITEMSERIAL )
	{
		CChar *w = getPackOwner( (CItem *)i->GetCont() );
		if( w != NULL )
		{
			if( w->GetNPCAiType() == aiPLAYERVENDOR )
			{
				if( i->GetCreator() != INVALIDSERIAL && i->GetMadeWith() > 0 )
				{
					CChar *mCreater = calcCharObjFromSer( i->GetCreator() );
					if( mCreater != NULL )
						sprintf( temp2, "%s %s by %s", i->GetDesc(), cwmWorldState->skill[i->GetMadeWith()-1].madeword, mCreater->GetName().c_str() );
					else
						strcpy( temp2, i->GetDesc() );
				}
				else
					strcpy( temp2, i->GetDesc() );
				sprintf( temp, "%s at %igp", temp2, i->GetBuyValue() );
				tSock->objMessage( AppendData( i, temp ), i );
				return true;
			}
		}
	}

#if defined( _MSC_VER )
#pragma note( "We need to update this to use getTileName almost exclusively, for plurality" )
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
		getTileName( i, realname );

	if( i->GetType() == 15 )
	{
		int spellNum = ( 8 * ( i->GetMoreX() - 1 ) ) + i->GetMoreY() - 1;	// we pick it up from the array anyway
		realname += " of ";
		realname += Dictionary->GetEntry( magic_table[spellNum].spell_name, tSock->Language() );
		realname += " with ";
		realname += UString::number( i->GetMoreZ() );
		realname += " charges";
	}
	if( i->GetCreator() != INVALIDSERIAL && i->GetMadeWith() > 0 )
	{
		CChar *mCreater = calcCharObjFromSer( i->GetCreator() );
		if( mCreater != NULL )
			sprintf( temp2, "%s %s by %s", realname.c_str(), cwmWorldState->skill[i->GetMadeWith()-1].madeword, mCreater->GetName().c_str() );
		else
			strcpy( temp2, realname.c_str() );
	}
	else
		strcpy( temp2, realname.c_str() );
	tSock->objMessage( temp2, i );
	if( i->IsLockedDown() )
		tSock->objMessage( "[Locked down]", i );//, 0x0481 );
	if( i->isGuarded() )
		tSock->objMessage( "[Guarded]", i );//, 0x0481 );
	return true;
}
