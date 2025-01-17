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
#include "CGump.h"
#include "cGuild.h"
#include "ssection.h"
#include "cServerDefinitions.h"
#include "cSkillClass.h"
#include "Dictionary.h"

using namespace std::string_literals;

void		SendTradeStatus( CItem *cont1, CItem *cont2 );
CItem *		StartTrade( CSocket *mSock, CChar *i );
bool		CheckItemRange( CChar *mChar, CItem *i );
void		DoHouseTarget( CSocket *mSock, UI16 houseEntry );
void		SocketMapChange( CSocket *sock, CChar *charMoving, CItem *gate );
void		BuildGumpFromScripts( CSocket *s, UI16 m );
void		PlankStuff( CSocket *s, CItem *p );
CBoatObj *	GetBoat( CSocket *s );
void		ModelBoat( CSocket *s, CBoatObj *i );

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Bounce()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Bounce items back from where they came
//o------------------------------------------------------------------------------------------------o
void Bounce( CSocket *bouncer, CItem *bouncing, UI08 mode = 5 )
{
	if( bouncer == nullptr || !ValidateObject( bouncing ))
		return;

	CPBounce bounce( mode );
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
			if( !ObjInRange( bouncer, bouncing, DIST_INRANGE ))
			{
				x = bouncer->CurrcharObj()->GetX();
				y = bouncer->CurrcharObj()->GetY();
				z = bouncer->CurrcharObj()->GetZ();
				bouncing->SetLocation( x, y, z );
			}
			break;
		}
		case PL_OWNPACK:
		case PL_OTHERPACK:
		case PL_PAPERDOLL:
			bouncing->SetContSerial( spot );
			break;
	}
	bouncing->SetHeldOnCursor( false );
	bouncing->Dirty( UT_UPDATE );
	bouncer->Send( &bounce );
	bouncer->PickupSpot( PL_NOWHERE );
	bouncer->SetCursorItem( nullptr );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PickupBounce()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Bounce items back if pickup is illegal. Doesn't require updating item.
//o------------------------------------------------------------------------------------------------o
void PickupBounce( CSocket *bouncer, UI08 mode = 0 )
{
	if( bouncer == nullptr )
		return;

	CPBounce bounce( mode );
	bouncer->Send( &bounce );
	bouncer->PickupSpot( PL_NOWHERE );
	bouncer->SetCursorItem( nullptr );
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoStacking()
//|	Date		-	8/14/01
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches pack for pileable items that match the item being
//|					dropped into said pack (only if it's pileable), if found
//|					ensures the amount won't go over 65535 (the limit how large
//|					an item can stack) then stacks it. If the item is not stackable
//|					or it cannot stack the item with a pile and have an amount that
//|					is <= 65355 then it creates a new pile.
//|
//|	Changes		-	09/09/2001 - returns true if item deleted
//|					09/25/2002 - Weight fixes
//o------------------------------------------------------------------------------------------------o
CItem *DoStacking( CSocket *mSock, [[maybe_unused]] CChar *mChar, CItem *i, CItem *stack )
{
	UI32 newAmt = stack->GetAmount() + i->GetAmount();
	if( newAmt > MAX_STACK )
	{
		i->SetAmount(( newAmt - MAX_STACK ));
		stack->SetAmount( MAX_STACK );
	}
	else
	{
		stack->SetAmount( newAmt );
		i->Delete();
		if( mSock != nullptr )
		{
			Effects->ItemSound( mSock, stack, false );
		}
		return stack;
	}
	return i;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AutoStack()
//|	Status		-	Modified to v2
//|					v2 - accepts a possible nullptr socket to deal with the JSE
//|					v3 - returns a CItem * (stack if stacked, item otherwise)
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the pack to see if an item can be stacked
//|					stacking them automatically
//o------------------------------------------------------------------------------------------------o
auto AutoStack( CSocket *mSock, CItem *iToStack, CItem *iPack ) ->CItem *
{
	CChar *mChar = nullptr;
	if( mSock )
	{
		mChar = mSock->CurrcharObj();
	}
	if( !ValidateObject( iToStack ) || !ValidateObject( iPack ))
		return nullptr;

	iToStack->SetCont( iPack );
	if( iToStack->IsPileable() )
	{
		if( mSock != nullptr && ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ))
		{
			Weight->SubtractItemWeight( mChar, iToStack );
		}
		const UI16 itId		= iToStack->GetId();
		const SERIAL itSer	= iToStack->GetSerial();
		const UI16 itCol	= iToStack->GetColour();
		const UI32 itMore	= iToStack->GetTempVar( CITV_MORE );
		const UI32 itMoreX	= iToStack->GetTempVar( CITV_MOREX );
		const UI32 itMoreY	= iToStack->GetTempVar( CITV_MOREY );
		const UI32 itMoreZ	= iToStack->GetTempVar( CITV_MOREZ );
		const UI32 itBuyValue = iToStack->GetBuyValue();
		const UI32 itSellValue = iToStack->GetSellValue();

		auto ipCont = iPack->GetContainsList();
		for( const auto &stack :ipCont->collection() )
		{
			if( ValidateObject( stack ))
			{
				if( stack->IsPileable() && stack->GetAmount() < MAX_STACK &&
				   stack->GetSerial() != itSer && stack->GetId() == itId && stack->GetColour() == itCol && stack->GetTempVar( CITV_MORE ) == itMore && stack->GetTempVar( CITV_MOREX ) == itMoreX
				   && stack->GetTempVar( CITV_MOREY ) == itMoreY && stack->GetTempVar( CITV_MOREZ ) == itMoreZ && stack->GetBuyValue() == itBuyValue && stack->GetSellValue() == itSellValue )
				{
					// Autostack
					if( DoStacking( mSock, mChar, iToStack, stack ) == stack ) // compare to stack, if DoStacking returned the stack, then the raw object was deleted
					{
						return stack; // return the stack
					}
				}
			}
		}
		if( mSock != nullptr && ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ))
		{
			Weight->AddItemWeight( mChar, iToStack );
		}
	}
	iToStack->PlaceInPack();
	if( mSock )
	{
		Effects->ItemSound( mSock, iToStack, false );
	}
	return iToStack;
}

auto FindNearbyChars( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CChar *>;
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGetItem::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when a player picks up an item
//o------------------------------------------------------------------------------------------------o
bool CPIGetItem::Handle( void )
{
	CChar *ourChar	= tSock->CurrcharObj();
	SERIAL serial	= tSock->GetDWord( 1 );
	if( serial == INVALIDSERIAL )
		return true;

	// Break their meditation/concentration
	ourChar->BreakConcentration( tSock );

	// In case player is casting a spell, cancel their targeting cursor (if enabled in INI)
	if(( ourChar->IsCasting() || ourChar->GetSpellCast() != -1 ) && cwmWorldState->ServerData()->ItemsInterruptCasting() )
	{
		tSock->SendTargetCursor( 0, 0x00000000, Dictionary->GetEntry( 2862, tSock->Language() ), 3 ); // Your hands must be free to cast spells or meditate.
		ourChar->StopSpell();
		if( ourChar->IsFrozen() )
		{
			ourChar->SetFrozen( false );
			ourChar->Dirty( UT_UPDATE );
		}
	}

	CItem *i = CalcItemObjFromSer( serial );
	if( !ValidateObject( i ))
		return true;

	CPBounce bounce( 0 );

	ObjectType oType	= OT_CBO;
	CBaseObject *iOwner	= nullptr;
	CItem *x			= i;
	CBaseObject *iCont	= i->GetCont();
	if( iCont != nullptr )  //Find character owning item
	{
		iOwner = FindItemOwner( i, oType );
		tSock->PickupSerial( i->GetContSerial() );
		if( oType == OT_CHAR )
		{
			if( iCont->GetObjType() == OT_CHAR )
			{
				tSock->PickupSpot( PL_PAPERDOLL );
			}
			else
			{
				if( iOwner != ourChar )
				{
					tSock->PickupSpot( PL_OTHERPACK );
				}
				else
				{
					tSock->PickupSpot( PL_OWNPACK );
				}
			}
			bool otherPackCheck = ( iOwner != ourChar );
			if( otherPackCheck )	// okay, not our owner!
			{
				otherPackCheck = !ourChar->IsGM();
				if( otherPackCheck )	// not a GM either
				{
					if( iOwner->GetOwnerObj() == ourChar )
					{
						otherPackCheck = false;
					}
					else if( Npcs->CheckPetFriend( ourChar, static_cast<CChar *>( iOwner )))
					{
						otherPackCheck = false;
					}
				}
			}
			if( otherPackCheck || !ObjInRange( ourChar, iOwner, DIST_NEARBY ))
			{
				if( iOwner->CanBeObjType( OT_CHAR ) && static_cast<CChar *>( iOwner )->GetNpcAiType() == AI_PLAYERVENDOR )
				{
					tSock->SysMessage( 9175 ); // If you'd like to purchase an item, just say so.
				}
				else
				{
					tSock->SysMessage( 9065 ); // That item does not belong to you. You'll have to steal it.
				}
				Bounce( tSock, i );
				return true;
			}
			if( iCont->CanBeObjType( OT_ITEM ))
			{
				CItem *contItem = static_cast<CItem *>( iCont );
				if( contItem->GetType() == IT_TRADEWINDOW ) // Trade Window
				{
					tSock->SysMessage( 9064 ); // You cannot pick that up.
					Bounce( tSock, i );
					return true;
				}
				CItem *recurseItem = FindRootContainer( contItem );
				if( recurseItem->GetType() == IT_TRADEWINDOW )
				{
					CItem *z = CalcItemObjFromSer( recurseItem->GetTempVar( CITV_MOREX ));
					if( ValidateObject( z ))
					{
						if( z->GetTempVar( CITV_MOREZ ) || recurseItem->GetTempVar( CITV_MOREZ ))
						{
							z->SetTempVar( CITV_MOREZ, 0 );
							recurseItem->SetTempVar( CITV_MOREZ, 0 );
							SendTradeStatus( z, recurseItem );
						}
						CChar *zChar = FindItemOwner( z );
						if( ValidateObject( zChar ))
						{
							CSocket *zSock = zChar->GetSocket();
							if( zSock != nullptr )
							{
								Effects->PlaySound( zSock, 0x0057, true );
							}
						}
					}
				}
				else if( recurseItem->GetLayer() == IL_BANKBOX )
				{
					tSock->PickupSpot( PL_OTHERPACK );
				}
			}
		}
		else if( oType == OT_ITEM )
		{
			// Picking up item from non-character container
			tSock->PickupSpot( PL_OTHERPACK );
			CItem *x = static_cast<CItem *>( iOwner );
			if( !ObjInRange( ourChar, x, DIST_NEARBY ))
			{
				Bounce( tSock, i );
				tSock->SysMessage( 393 ); // That is too far away.

				// Resend items in container after bounce, or the bounced item will vanish from player's screen
				CPItemsInContainer itemsIn( tSock, static_cast<CItem *>( iCont ), 0x01 );
				tSock->Send( &itemsIn );
				return true;
			}

			if( x->IsCorpse() )
			{
				CChar *corpseTargChar = x->GetOwnerObj();
				if( ValidateObject( corpseTargChar ))
				{
					if( corpseTargChar->IsGuarded() ) // Is the corpse being guarded?
					{
						Combat->PetGuardAttack( ourChar, corpseTargChar, corpseTargChar );
					}
					else if( x->IsGuarded() )
					{
						CTownRegion *itemTownRegion = x->GetRegion();
						if( !itemTownRegion->IsGuarded() && !itemTownRegion->IsSafeZone() )
						{
							Combat->PetGuardAttack( ourChar, corpseTargChar, x );
						}
					}
				}
			}
		}
	}
	else
	{
		// Picking up item from ground - check that character is in range and can see the item!
		tSock->PickupSpot( PL_GROUND );
		tSock->PickupLocation( i->GetX(), i->GetY(), i->GetZ() );
		if( !ourChar->IsGM() && ( !ObjInRange( ourChar, i, DIST_NEARBY ) || !LineOfSight( tSock, ourChar, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS, true )))
		{
			tSock->SysMessage( 683 ); // There seems to be something in the way
			Bounce( tSock, i );
			return true;
		}
	}

	if( i->IsCorpse() || !CheckItemRange( ourChar, x ))
	{
		tSock->SysMessage( 9064 ); // You cannot pick that up.
		Bounce( tSock, i );
		return true;
	}

	if( x->GetMultiObj() != nullptr )
	{
		if(( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND ) && ( x->GetMultiObj() != ourChar->GetMultiObj() || x->IsLockedDown() ))
		{
			tSock->SysMessage( 9064 ); // You cannot pick that up.
			Bounce( tSock, i );
			return true;
		}
		i->SetMulti( INVALIDSERIAL );
	}

	if( i->IsDecayable() )
	{
		i->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ));
	}
	if( i->IsGuarded() )
	{
		// Only care about guarded state if outside of a guarded/safezone region
		CTownRegion *itemTownRegion = x->GetRegion();
		if( !itemTownRegion->IsGuarded() && !itemTownRegion->IsSafeZone() )
		{
			// Let's loop through a list of nearby characters to see if anyone is guarding the object
			for( auto &nearbyChar : FindNearbyChars(i->GetX(), i->GetY(), i->WorldNumber(), i->GetInstanceId(), DIST_INRANGE ))
			{
				// Only proceed if the character is guarding the object
				if( nearbyChar->GetGuarding() == i )
				{
					CChar *nearbyCharOwner = nearbyChar->GetOwnerObj();
					if( ValidateObject( nearbyCharOwner ))
					{
						// Turn the player who picked up the guarded item criminal
						MakeCriminal( ourChar );

						// Have pet/follower attack the player who picked up the item
						Combat->PetGuardAttack( ourChar, nearbyCharOwner, i, nearbyChar );
					
						// Stop guarding item, it is now in someone else's possession
						nearbyChar->SetGuarding( nullptr );
						i->SetGuarded( false );
					}
				}
			}
		}
	}

	CTile& tile = Map->SeekTile( i->GetId() );
	if( !ourChar->AllMove() && ( i->GetMovable() == 2 || i->IsLockedDown() ||
								( tile.Weight() == 255 && i->GetMovable() != 1 )))
	{
		if( ourChar->GetCommandLevel() < 2 || tSock->PickupSpot() != PL_OWNPACK )
		{
			tSock->SysMessage( 9064 ); // You cannot pick that up.
			if( ourChar->GetCommandLevel() >= 2 )
			{
				tSock->SysMessage( 9099 ); // Tip: Try 'ALLMOVE ON command or modify item's movable property with 'TWEAK command!
			}
			Bounce( tSock, i );
			return true;
		}
		else
		{
			tSock->SysMessage( 9098 ); // Item immovable to normal players, but can be dragged out of backpack by GM characters.
		}
	}
	
	// Trigger pickup event for item being picked up
	std::vector<UI16> scriptTriggers = i->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			SI08 retStatus = toExecute->OnPickup( i, ourChar, iCont );

			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( retStatus == 0 )
			{
				Bounce( tSock, i );
				return true;
			}
		}
	}

	// Trigger pickup event for character picking up item
	scriptTriggers.clear();
	scriptTriggers = ourChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			SI08 retStatus = toExecute->OnPickup( i, ourChar, iCont );

			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( retStatus == 0 )
			{
				Bounce( tSock, i );
				return true;
			}
		}
	}

	// Trigger pickup event for potential container item is being picked up from
	if( ValidateObject( iCont ) && iCont != nullptr )
	{
		// Trigger pickup event for character picking up item
		scriptTriggers.clear();
		scriptTriggers = iCont->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				SI08 retStatus = toExecute->OnPickup( i, ourChar, iCont );

				// -1 == script doesn't exist, or returned -1
				// 0 == script returned false, 0, or nothing - don't execute hard code
				// 1 == script returned true or 1
				if( retStatus == 0 )
				{
					Bounce( tSock, i );
					return true;
				}
			}
		}
	}

	if( i->GetAmount() > 1 && i->GetObjType() != OT_SPAWNER )
	{
		UI16 amount = tSock->GetWord( 5 );
		if( amount > i->GetAmount() )
		{
			amount = i->GetAmount();
		}

		if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
		{
			if( !Weight->CheckCharWeight( nullptr, ourChar, i, amount ))
			{
				tSock->SysMessage( 1743 ); // That person can not possibly hold more weight
				Bounce( tSock, i );
				return true;
			}
		}
		
		if( amount < i->GetAmount() )
		{
			CItem *c = i->Dupe();
			if( c != nullptr )
			{
				c->IncAmount( -amount );
				c->SetCont( i->GetCont() );
				
				c->SetLocation( c->GetX(), c->GetY(), c->GetZ(), 0, c->WorldNumber(), c->GetInstanceId() );
			}
			i->SetAmount( amount );
		}
		if( i->GetId() == 0x0EED )
		{
			if( tSock->PickupSpot() == PL_OWNPACK )
			{
				ourChar->Dirty( UT_STATWINDOW );
			}
		}
	}
	Effects->PlaySound( tSock, 0x0057, true );

	if( i->IsCorpse() )
	{
		// Store temp tag on corpse with serial of player who looted the corpse last
		TAGMAPOBJECT tagObject;
		tagObject.m_Destroy = false;
		tagObject.m_StringValue = oldstrutil::number( ourChar->GetSerial() );
		tagObject.m_IntValue = 0;
		tagObject.m_ObjectType = TAGMAP_TYPE_INT;
		i->SetTempTag( "lootedBy", tagObject );
	}

	if( iCont == nullptr )
	{
		// Remove item from mapregion it was in before being picked up
		MapRegion->RemoveItem( i );

		// Add item to mapregion of character who picked up item instead
		MapRegion->AddItem( i );
	}
	i->RemoveFromSight();

	tSock->SetCursorItem( i );
	i->SetHeldOnCursor( true );

	// Update region of item to match that of the character holding it
	i->SetRegion( ourChar->GetRegionNum() );

	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
	{
		Weight->AddItemWeight( ourChar, i );
	}

	if( iCont != nullptr )
	{
		if( ValidateObject( iCont ) && iCont->CanBeObjType( OT_ITEM ))
		{
			std::vector<UI16> contScriptTriggers = iCont->GetScriptTriggers();
			for( auto scriptTrig : contScriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					CItem *contItem = static_cast<CItem *>( iCont );
					toExecute->OnContRemoveItem( contItem, i, ourChar );
				}
			}
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIEquipItem::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when an item is dropped on a players paperdoll
//o------------------------------------------------------------------------------------------------o
bool CPIEquipItem::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	SERIAL cserial = tSock->GetDWord( 6 );
	SERIAL iserial = tSock->GetDWord( 1 );
	if( cserial == INVALIDSERIAL || iserial == INVALIDSERIAL )
		return true;

	ourChar->BreakConcentration( tSock );
	CItem *i = CalcItemObjFromSer( iserial );
	CChar *k = CalcCharObjFromSer( cserial );
	if( !ValidateObject( i ))
		return true;

	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_PAPERDOLL )
	{
		ObjectType pOType;
		CBaseObject *pOwner = FindItemOwner( i, pOType );
		if( pOType == OT_CHAR )
		{
			CChar *pOChar = static_cast<CChar *>( pOwner );
			if((( pOChar != ourChar ) && (( !ourChar->IsGM() && ( pOChar->GetOwnerObj() != ourChar )))) || !ObjInRange( ourChar, pOwner, DIST_NEARBY ))
			{
				tSock->SysMessage( 9066 ); // That belongs to someone else.
				Bounce( tSock, i );
				return true;
			}
		}
		else if( pOType == OT_ITEM )
		{
			if( !ObjInRange( ourChar, pOwner, DIST_NEARBY ))
			{
				tSock->SysMessage( 393 ); // That is too far away
				Bounce( tSock, i );
				return true;
			}
		}

	}
	else if( tSock->PickupSpot() == PL_GROUND )
	{
		if( !ObjInRange( ourChar, i, DIST_NEARBY ))
		{
			tSock->SysMessage( 393 ); // That is too far away
			Bounce( tSock, i );
			return true;
		}
	}

	if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
	{
		Weight->SubtractItemWeight( ourChar, i ); // SetCont() adds the weight for us (But we also had to add the weight in grabItem() since it sets cont as INVALIDSERIAL
	}

	if( !ourChar->IsGM() && k != ourChar )	// players cant equip items on other players or npc`s paperdolls.  // GM PRIVS
	{
		Bounce( tSock, i );
		tSock->SysMessage( 1186 ); // You can't put items on other players!
		return true;
	}

	if( ourChar->IsDead() )
	{
		Bounce( tSock, i );
		tSock->SysMessage( 1185 ); // You can't do much in your current state.
		return true;
	}
	if( !ValidateObject( k ))
		return true;

	RACEID raceId = k->GetRace();
	CRace *race = Races->Race( raceId );
	if( !race->CanEquipItem( i->GetId() ))
	{
		tSock->SysMessage( 1981, race->Name().c_str() ); // Members of the %s race cannot wear this
		Bounce( tSock, i );
		return true;
	}

	ARMORCLASS ac1 = Races->ArmorRestrict( raceId );
	ARMORCLASS ac2 = i->GetArmourClass();

	if( ac1 != 0 && (( ac1 & ac2 ) == 0 ))	// bit comparison, if they have ANYTHING in common, they can wear it
	{
		tSock->SysMessage( 1187 ); // You are unable to equip that due to your race.
		Bounce( tSock, i );
		return true;
	}
	if( k == ourChar )
	{
		bool canWear = false;
		const SI16 scaledStrength = ( i->GetStrength() * ( 100 - i->GetLowerStatReq() )) / 100;
		if( scaledStrength > k->GetStrength() )
		{
			tSock->SysMessage( 1188 ); // You are not strong enough to use that. (NOTE: Should these messages use color 0x096a to stand out and replicate hard coded client message?)
		}
		else if( i->GetDexterity() > k->GetDexterity() )
		{
			tSock->SysMessage( 1189 ); // You are not agile enough to use that.
		}
		else if( i->GetIntelligence() > k->GetIntelligence() )
		{
			tSock->SysMessage( 1190 ); // You are not smart enough to use that.
		}
		else
		{
			canWear = true;
		}
		if( !canWear )
		{
			Bounce( tSock, i );

			if( tSock->PickupSpot() == PL_OTHERPACK || tSock->PickupSpot() == PL_GROUND )
			{
				Effects->ItemSound( tSock, i, true );
			}
			else
			{
				Effects->ItemSound( tSock, i, false );
			}
			return true;
		}
	}

	CTile& tile = Map->SeekTile( i->GetId() );
	if( !ourChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != ourChar ) ||
								( tile.Weight() == 255 && i->GetMovable() != 1 )))
	{
		tSock->SysMessage( 9064 ); // You cannot pick that up.
		Bounce( tSock, i );
		return true;
	}

	if( i->GetLayer() == IL_NONE )
	{
		i->SetLayer( static_cast<ItemLayers>( tSock->GetByte( 5 )));
	}

	// 1/13/2003 - Fix for equipping an item to more than one hand, or multiple equipping.
	if( i->GetCont() != k )
	{
		bool conflictItem = true;
		CItem *j = k->GetItemAtLayer( i->GetLayer() );
		if( !ValidateObject( j ))
		{
			if( i->GetLayer() == IL_RIGHTHAND )
			{
				j = k->GetItemAtLayer( IL_LEFTHAND );
			}
			else if( i->GetLayer() == IL_LEFTHAND )
			{
				j = k->GetItemAtLayer( IL_RIGHTHAND );
			}

			// GetDir-check is to allow for torches and lanterns,
			// which use left-hand layer but are not 2-handers or shields
			if( ValidateObject( j ) && !i->IsShieldType() && i->GetDir() == 0 )
			{
				if( j->IsShieldType() || j->GetDir() != 0 )
				{
					conflictItem = false;
				}
			}
			else
			{
				conflictItem = false;
			}
		}
		if( conflictItem == true )
		{
			tSock->SysMessage( 1744 ); // You can't equip two items in the same slot
			Bounce( tSock, i );
			return true;
		}
	}

	if( !Weight->CheckCharWeight( ourChar, k, i ))
	{
		tSock->SysMessage( 1743 ); // That person can not possibly hold more weight
		Bounce( tSock, i );
		return true;
	}

	i->SetCont( k );
	i->SetHeldOnCursor( false );

	//Reset PickupSpot after dropping the item
	tSock->PickupSpot( PL_NOWHERE );
	tSock->SetCursorItem( nullptr );

	// In case player is casting a spell, cancel their targeting cursor (if enabled in INI)
	if(( ourChar->IsCasting() || ourChar->GetSpellCast() != -1 ) && cwmWorldState->ServerData()->ItemsInterruptCasting() )
	{
		tSock->SendTargetCursor( 0, 0x00000000, Dictionary->GetEntry( 2862, tSock->Language() ), 3 ); // Your hands must be free to cast spells or meditate.
		ourChar->StopSpell();
		if( ourChar->IsFrozen() )
		{
			ourChar->SetFrozen( false );
			ourChar->Dirty( UT_UPDATE );
		}
	}

	CPDropItemApproved lc;
	tSock->Send( &lc );

	auto weaponType = Combat->GetCombatSkill( i );
	if( weaponType == SWORDSMANSHIP || weaponType == MACEFIGHTING || weaponType == FENCING )
	{
		Effects->PlaySound( tSock, 0x0056, false ); // Play unsheath sound if 
	}
	else
	{
		Effects->PlaySound( tSock, 0x0057, false );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DropOnPC()
//|	Changes		-	September 14th, 2001, returns true if item deleted
//|				-	September 25, 2002, Weight fixes
//|				-	September 21st, 2003, moved into seperate file and few other minor tweaks
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when an item is dropped on a character
//o------------------------------------------------------------------------------------------------o
bool DropOnPC( CSocket *mSock, CChar *mChar, CChar *targPlayer, CItem *i )
{
	bool stackDeleted = false;
	if( targPlayer == mChar )
	{
		CItem *pack = mChar->GetPackItem();
		if( !ValidateObject( pack )) // if player has no pack, put it at its feet
		{
			i->SetCont( nullptr );
			i->SetLocation( mChar );
		}
		else
		{
			stackDeleted = ( AutoStack( mSock, i, pack ) != i );
		}
	}
	else // Trade stuff
	{
		if( IsOnline(( *targPlayer )))
		{
			CItem *j = StartTrade( mSock, targPlayer );
			if( ValidateObject( j ))
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
			{
				Weight->SubtractItemWeight( mChar, i );
			}
			mSock->SysMessage( 1754 ); // That character is not online.
			Bounce( mSock, i );
			return true;
		}
	}
	return stackDeleted;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	IsOnFoodList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if a particular item is on an NPC's list of accepted foods
//o------------------------------------------------------------------------------------------------o
auto IsOnFoodList( const std::string& sFoodList, const UI16 sItemId ) -> bool
{
	auto doesEat = false;

	const std::string sect = "FOODLIST "s + sFoodList;
	auto FoodList = FileLookup->FindEntry( sect, items_def );
	if( FoodList )
	{
		for( const auto &sec : FoodList->collection() )
		{
			auto tag = sec->tag;
			if( !tag.empty() )
			{
				if( oldstrutil::upper( tag ) == "FOODLIST" )
				{
					doesEat = IsOnFoodList( sec->data, sItemId );
				}
				else if( sItemId == static_cast<UI16>( std::stoul( tag, nullptr, 0 )))
				{
					doesEat = true;
				}
			}

			if( doesEat ) // We found a valid food, let's stop looking
				break;
		}
	}
	return doesEat;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DropOnNPC()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when an item is dropped on an NPC
//o------------------------------------------------------------------------------------------------o
bool DropOnNPC( CSocket *mSock, CChar *mChar, CChar *targNPC, CItem *i )
{
	UI08 dropResult		= 0;
	const bool isGM		= ( mChar->GetCommandLevel() >= CL_CNS );
	bool stackDeleted	= false;
	bool executeNpc		= true;

	SI08 rVal = 0;
	std::vector<UI16> scriptTriggers = i->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			rVal = toExecute->OnDropItemOnNpc( mChar, targNPC, i );	// returns 1 if we should bounce it
			switch( rVal )
			{
				case -1:	// no such event
				default:
					executeNpc = true;
					break;
				case 0:	// bounce, no code, error message handled in script
					if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					{
						Weight->SubtractItemWeight( mChar, i );
					}
					Bounce( mSock, i );
					return false;	// stack not deleted, as we're bouncing
				case 1:	// don't bounce, use code
					executeNpc = false;
					break;
				case 2:	// don't bounce, don't use code
					executeNpc = true;
					break;
			}
		}
	}

	if( executeNpc )
	{
		std::vector<UI16> targScriptTriggers = targNPC->GetScriptTriggers();
		for( auto scriptTrig : targScriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				rVal = toExecute->OnDropItemOnNpc( mChar, targNPC, i );	// returns 1 if we should bounce it
				switch( rVal )
				{
					case -1:	// no such event
					case 1:	// we don't want to bounce, use code
					default:
						break;
					case 0:	// we want to bounce and return. Handle error message in script
							// If we want to bounce, then it's safe to assume the item still exists!
						if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
						{
							Weight->SubtractItemWeight( mChar, i );
						}
						Bounce( mSock, i );
						return false;	// bouncing, so item exists
					case 2:	// we don't want to bounce, don't use code
						return true;	// don't let the code assume the item exists, so we never reference it
				}
			}
		}
	}

	if( targNPC->IsTamed() && ( isGM || targNPC->GetOwnerObj() == mChar || Npcs->CheckPetFriend( mChar, targNPC ))) // do food stuff
	{
		if( targNPC->WillHunger() && IsOnFoodList( targNPC->GetFood(), i->GetId() ))
		{
			if( targNPC->GetHunger() < 6 )
			{
				Effects->PlaySound( mSock, static_cast<UI16>( 0x003A + RandomNum( 0, 2 )), true );
				if( cwmWorldState->creatures[targNPC->GetId()].IsAnimal() )
				{
					Effects->PlayCharacterAnimation( targNPC, ACT_ANIMAL_EAT, 0, 5 );
				}

				// Restore loyalty upon feeding pet
				if( targNPC->GetLoyalty() < targNPC->GetMaxLoyalty() )
				{
					if( cwmWorldState->ServerData()->ExpansionCoreShardEra() >= ER_AOS )
					{
						// Post-AoS (Pub16), restore loyalty to max upon feeding, regardless of amount
						targNPC->SetLoyalty( targNPC->GetMaxLoyalty() );
					}
					else
					{
						// Pre-AoS (Pub16), restore loyalty by 10 each time pet is fed, regardless of amount
						targNPC->SetLoyalty( targNPC->GetLoyalty() + 10 );
					}
					mSock->SysMessage( 2416 ); // Your pet looks happier.
				}

				UI08 poisonStrength = i->GetPoisoned();
				if( poisonStrength && targNPC->GetPoisoned() < poisonStrength )
				{
					Effects->PlaySound( mSock, 0x0246, true ); // poison sound - SpaceDog

					// Apply poison on target
					targNPC->SetPoisoned( poisonStrength );

					// Set time until next time poison "ticks"
					targNPC->SetTimer( tCHAR_POISONTIME, BuildTimeValue( static_cast<R32>( GetPoisonTickTime( poisonStrength ))));

					// Set time until poison wears off completely
					targNPC->SetTimer( tCHAR_POISONWEAROFF, targNPC->GetTimer( tCHAR_POISONTIME ) + ( 1000 * GetPoisonDuration( poisonStrength ))); //wear off starts after poison takes effect

					// Cut loyalty in half if pet was fed poisoned food
					if( targNPC->GetLoyalty() > 0 )
					{
						targNPC->SetLoyalty( std::clamp( static_cast<UI16>( targNPC->GetLoyalty() / 2 ), static_cast<UI16>( 0 ), static_cast<UI16>( 100 )));
					}
				}

				//Remove a food item
				bool iDeleted = i->IncAmount( -1 );
				targNPC->SetHunger( static_cast<SI08>( targNPC->GetHunger() + 1 ));
				mSock->SysMessage( 1781 ); // That pet accepts your food and happily eats it.
				if( iDeleted )
				{
					return true; //stackdeleted
				}
			}
			else
			{
				mSock->SysMessage( 1780 ); // That pet is already stuffed, it can't eat more.
			}
		}
		else if( isGM || targNPC->GetId() == 0x0123 || targNPC->GetId() == 0x0124 || targNPC->GetId() == 0x0317 ) // It's a pack animal
		{
			dropResult = 2;
		}
	}
	else if( cwmWorldState->creatures[targNPC->GetId()].IsHuman() )
	{
		if( static_cast<CChar *>( mSock->TempObj() ) != targNPC )
		{
			if( isGM )
			{
				dropResult = 2;
			}
			else
			{
				dropResult = 1;
				targNPC->TextMessage( mSock, 1197, TALK, false ); // Thank thee kindly, but I have done nothing to warrant a gift.
			}
		}
		else if( i->GetId() == 0x0EED ) // They gave the NPC gold
		{
			targNPC->TextMessage( mSock, 1198, TALK, false ); // I thank thee for thy payment. That should give thee a good start on thy way. Farewell!
			UI08 trainedIn = targNPC->GetTrainingPlayerIn();
			UI16 oldskill = mChar->GetBaseSkill( trainedIn );
			mChar->SetBaseSkill( static_cast<UI16>( mChar->GetBaseSkill( trainedIn ) + i->GetAmount() ), trainedIn );
			if( mChar->GetBaseSkill( trainedIn ) > 250 )
			{
				mChar->SetBaseSkill( 250, trainedIn );
			}

			Skills->UpdateSkillLevel( mChar, trainedIn );
			mSock->UpdateSkill( trainedIn );
			UI16 getAmount = i->GetAmount();
			if( i->GetAmount() > 250 ) // Paid too much
			{
				i->IncAmount( - ( 250 - oldskill ));
				dropResult = 1;
			}
			else  // Gave exact change
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				{
					Weight->SubtractItemWeight( mChar, i );
				}
				i->Delete();
				stackDeleted = true;
			}

			mSock->TempObj( nullptr );
			targNPC->SetTrainingPlayerIn( 0xFF );
			Effects->GoldSound( mSock, getAmount, false );
		}
		else // Did not give gold
		{
			dropResult = 1;
			targNPC->TextMessage( mSock, 1199, TALK, false ); // I am sorry, but I can only accept gold.
		}
	}
	else
	{
		dropResult = ( isGM ? 2 : 1 );
	}

	switch( dropResult )
	{
		default:
		case 0:		// Do nothing;
			break;
		case 1:		// Bounce
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( mChar, i );
			}

			Bounce( mSock, i );
			break;
		case 2:		// Stack
			CItem *pack;
			pack = targNPC->GetPackItem();
			if( ValidateObject( pack ))
			{
				stackDeleted = ( AutoStack( mSock, i, pack ) != i );
			}
			break;
	}
	return stackDeleted;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DropOnChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when an item is dropped on an a character
//o------------------------------------------------------------------------------------------------o
bool DropOnChar( CSocket *mSock, CChar *targChar, CItem *i )
{
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ))
		return false;

	CItem *packItem = targChar->GetPackItem();
	if( ValidateObject( packItem ))
	{
		// Can character's backpack hold any more items?
		if( mSock->PickupSpot() != PL_OWNPACK && ( GetTotalItemCount( packItem ) >= packItem->GetMaxItems() ||
			GetTotalItemCount( packItem ) + std::max( static_cast<UI32>( 1 ), 1 + GetTotalItemCount( i )) > packItem->GetMaxItems() ))
		{
			mSock->SysMessage( 1818 ); // The container is already at capacity.
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( mChar, i );
			}
			Bounce( mSock, i );
			return false;
		}

		// Can character's backpack hold any more weight?
		if( !Weight->CheckCharWeight( mChar, targChar, i ))
		{
			mSock->SysMessage( 1743 ); // That person can not possibly hold more weight
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( mChar, i );
			}
			Bounce( mSock, i );
			return false;
		}

		if( !targChar->IsNpc() )
		{
			// Drop on player character
			return DropOnPC( mSock, mChar, targChar, i );
		}

		// Drop on NPC
		return DropOnNPC( mSock, mChar, targChar, i );
	}
	return false;

}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckForValidDropLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for a valid drop location at the location where client attempts to drop an item
//o------------------------------------------------------------------------------------------------o
bool CheckForValidDropLocation( CSocket *mSock, CChar *nChar, UI16 x, UI16 y, SI08 z )
{
	bool dropLocationBlocked = false;

	// Don't allow dropping item at a location far below or far above character
	if( z < nChar->GetZ() - 7 || z > nChar->GetZ() + 16 ) // 7 to allow to stand on bottom of stairs and drop items on floor/ground below
	{
		dropLocationBlocked = true;
	}

	if( !dropLocationBlocked )
	{
		// Check for a valid surface to drop item on first - then check for blocking tiles after
		// If done the other way, the valid surface will override the blocking tiles!

		// First, check for a static surface to drop item on
		UI16 foundTileId1 = 0;
		UI16 foundTileId2 = 0;
		if( !Map->CheckStaticFlag( x, y, z, nChar->WorldNumber(), TF_SURFACE, foundTileId1, false ))
		{
			// Nowhere static to put item? Check dynamic tiles for surface!
			if( !Map->CheckDynamicFlag( x, y, z, nChar->WorldNumber(), nChar->GetInstanceId(), TF_SURFACE, foundTileId1 ))
			{
				// No static OR dynamic surface was found to place item? Check if map itself blocks the placement
				dropLocationBlocked = Map->DoesMapBlock( x, y, z, nChar->WorldNumber(), true, false, false, false );
			}
		}

		if( !dropLocationBlocked )
		{
			// Some kind of valid surface was found. But is it blocked by...
			if( Map->CheckStaticFlag( x, y, z, nChar->WorldNumber(), TF_BLOCKING, foundTileId2, false ) || Map->CheckStaticFlag( x, y, z, nChar->WorldNumber(), TF_ROOF, foundTileId2, false ))
			{ // ...static items?
				if( foundTileId1 != foundTileId2 )
				{
					dropLocationBlocked = true;
				}
			}
			else if( Map->CheckDynamicFlag( x, y, z, nChar->WorldNumber(), nChar->GetInstanceId(), TF_BLOCKING, foundTileId2 ) || Map->CheckDynamicFlag( x, y, z, nChar->WorldNumber(), nChar->GetInstanceId(), TF_ROOF, foundTileId2 ))
			{ // No? What about dynamic items?
				if( foundTileId1 != foundTileId2 )
				{
					dropLocationBlocked = true;
				}
			}
		}
	}

	// Let's check line of sight as well, for good measure
	if( !nChar->IsGM() && !dropLocationBlocked && !LineOfSight( mSock, nChar, x, y, z, WALLS_CHIMNEYS + DOORS + ROOFING_SLANTED, true ))
	{
		dropLocationBlocked = true;
	}

	return !dropLocationBlocked;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Drop()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Item is dropped on the ground or on a character
//o------------------------------------------------------------------------------------------------o
void Drop( CSocket *mSock, SERIAL item, SERIAL dest, SI16 x, SI16 y, SI08 z, SI08 gridLoc )
{
	CChar *nChar	= mSock->CurrcharObj();
	CItem *i		= CalcItemObjFromSer( item );
	bool stackDeleted = false;

	if( !ValidateObject( i ))
	{
		nChar->Teleport();
		return;
	}

	//Bounce if no valid pickupspot exists
	if( mSock->PickupSpot() == PL_NOWHERE )
	{
		mSock->SysMessage( 9067 ); // Unable to drop item... where was it picked up from?
		Bounce( mSock, i );
		return;
	}

	std::vector<UI16> scriptTriggers = i->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			SI08 rVal = toExecute->OnDrop( i, nChar );	// returns 1 if we should bounce it
			switch( rVal )
			{
				case 1:	// don't bounce, use code
				case -1:	// no such event
				default:
					break;
				case 0:	// bounce, no code, handle error message in client
					Bounce( mSock, i );
					return;	// stack not deleted, as we're bouncing
				case 2:	// don't bounce, don't use code
					return;
			}
		}
	}

	CTile& tile = Map->SeekTile( i->GetId() );
	if( !nChar->AllMove() && ( i->GetMovable() == 2 || ( i->IsLockedDown() && i->GetOwnerObj() != nChar ) ||
								( tile.Weight() == 255 && i->GetMovable() != 1 )))
	{
		if( nChar->GetCommandLevel() < 2 || mSock->PickupSpot() != PL_OWNPACK )
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( nChar, i );
			}
			mSock->SysMessage( 9064 ); // You cannot pick that up.
			Bounce( mSock, i );
			return;
		}
	}

	if( mSock->GetByte( 5 ) != 0xFF )	// Dropped in a specific location or on an item
	{
		// Let's check for a valid drop location for the item, and bounce it if none was found
		if( !CheckForValidDropLocation( mSock, nChar, x, y, z ))
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( nChar, i );
			}
			Bounce( mSock, i );
			mSock->SysMessage( 683 ); // There seems to be something in the way
			return;
		}

		// New location either is not blocking, or has a surface we can put the item on, so let's find the exact Z of where to put it
		auto newZ = Map->StaticTop( x, y, z, nChar->WorldNumber(), 14 - ( z - nChar->GetZ() ));
		if( newZ == ILLEGAL_Z || newZ < z || newZ > z + 14 )
		{
			// No valid static elevation found, use dynamic elevation instead
			newZ = Map->DynamicElevation( x, y, z, nChar->WorldNumber(), nChar->GetInstanceId(), 14 );
			if( newZ < z || newZ > z + 14 )
			{
				// No valid dynamic elevation found. Use map elevation instead (don't implicitly trust Z from client)
				newZ = Map->MapElevation( x, y, nChar->WorldNumber() );
			}
		}
		else
		{
			auto dynZ = Map->DynamicElevation( x, y, z, nChar->WorldNumber(), nChar->GetInstanceId(), 14 );
			newZ = (( dynZ >= z && dynZ <= z + 14 ) ? dynZ : newZ );
		}

		auto iMulti = FindMulti( x, y, newZ, nChar->WorldNumber(), nChar->GetInstanceId() );
		if( ValidateObject( iMulti ) )
		{
			// Check if new Z plus height of dropped item will make it exceed 20 Z limit per floor in houses, and deny placement if so
			if( newZ > ( nChar->GetZ() + ( 20 - tile.Height() )))
			{
				// Item is too tall - can't drop it this high up
				if( nChar->GetCommandLevel() >= 2 || nChar->IsGM() )
				{
					// GM override
					mSock->SysMessage( 91 ); // Item placement rule overruled by GM privileges - Z too high for normal players!
				}
				else
				{
					if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					{
						Weight->SubtractItemWeight( nChar, i );
					}
					Bounce( mSock, i );
					mSock->SysMessage( 683 ); // There seems to be something in the way
					return;
				}
			}
		}
		else if( newZ + tile.Height() > z + tile.Height() )
		{
			// Item is too tall - can't drop it this high up
			if( nChar->GetCommandLevel() >= 2 || nChar->IsGM() )
			{
				// GM override
				mSock->SysMessage( 91 ); // Item placement rule overruled by GM privileges - Z too high for normal players!
			}
			else
			{
				if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				{
					Weight->SubtractItemWeight( nChar, i );
				}
				Bounce( mSock, i );
				mSock->SysMessage( 683 ); // There seems to be something in the way
				return;
			}
		}

		i->SetCont( nullptr );
		i->SetLocation( x, y, newZ, gridLoc, nChar->WorldNumber(), nChar->GetInstanceId() );

		// If item was picked up from a container and dropped on ground, update old location to match new!
		if( mSock->PickupSpot() != PL_GROUND )
		{
			i->SetOldLocation( x, y, newZ );
		}
	}
	else
	{
		// Dropped on a character
		CChar *t = CalcCharObjFromSer( dest );
		if( ValidateObject( t ))
		{
			stackDeleted = DropOnChar( mSock, t, i );
		}
		else
		{
			// Bounces items dropped in illegal locations in 3D UO client!!!
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( nChar, i );
			}
			mSock->SysMessage( 683 ); // There seems to be something in the way
			Bounce( mSock, i );
			return;
		}
	}

	if( !stackDeleted )
	{
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->SubtractItemWeight( nChar, i );
		}

		if( nChar->GetMultiObj() != nullptr )
		{
			CMultiObj *multi = FindMulti( i );
			if( ValidateObject( multi ))
			{
				if( i->IsDecayable() )
				{
					i->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAYINHOUSE ));
				}
				i->SetMulti( multi );
			}
			else if( i->IsDecayable() )
			{
				i->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ));
			}
		}
		else if( i->IsDecayable() )
		{
			i->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ));
		}

		i->SetHeldOnCursor( false );

		Effects->ItemSound( mSock, i, ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ));
	}
	// IF client-version is above 6.0.1.7, send approval packet for dropping item. If not, don't.
	CPDropItemApproved lc;

	mSock->Send( &lc );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DropOnTradeWindow()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when item is dropped in the secure trade window
//o------------------------------------------------------------------------------------------------o
void DropOnTradeWindow( CSocket& mSock, CChar& mChar, CItem& tradeWindowOne, CItem& iDropped, SI16 x, SI16 y, SI08 gridLoc )
{
	if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
	{
		Weight->SubtractItemWeight( &mChar, &iDropped );
	}
	iDropped.SetCont( &tradeWindowOne );
	iDropped.SetX( x );
	iDropped.SetY( y );
	iDropped.SetZ( 9 );
	iDropped.SetGridLocation( gridLoc );
	CItem *tradeWindowTwo = CalcItemObjFromSer( tradeWindowOne.GetTempVar( CITV_MOREX ));
	if( ValidateObject( tradeWindowTwo ))
	{
		if( tradeWindowTwo->GetTempVar( CITV_MOREZ ) || tradeWindowOne.GetTempVar( CITV_MOREZ ))
		{
			tradeWindowTwo->SetTempVar( CITV_MOREZ, 0 );
			tradeWindowOne.SetTempVar( CITV_MOREZ, 0 );
			SendTradeStatus( tradeWindowTwo, &tradeWindowOne );
		}
		CChar *tw2Owner = FindItemOwner( tradeWindowTwo );
		if( ValidateObject( tw2Owner ))
		{
			CSocket *tw2Sock = tw2Owner->GetSocket();
			if( tw2Sock != nullptr )
			{
				Effects->ItemSound( tw2Sock, &tradeWindowOne, ( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND ));
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ValidateLockdownAccess()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if player has access to a locked down item as an owner, co-owner or friend
//o------------------------------------------------------------------------------------------------o
bool ValidateLockdownAccess( CChar *mChar, CSocket *mSock, CItem *itemToCheck, bool checkFriend )
{
	CMultiObj *iMulti = itemToCheck->GetMultiObj();
	if( ValidateObject( iMulti ) && !iMulti->IsOwner( mChar ) && !iMulti->IsOnOwnerList( mChar ) 
		&& ( checkFriend && !iMulti->IsOnFriendList( mChar )))
	{
		mSock->SysMessage( 1032 ); // This is not yours!
		return false;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DropOnSpellBook()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when item is dropped on a spellbook
//o------------------------------------------------------------------------------------------------o
void DropOnSpellBook( CSocket& mSock, CChar& mChar, CItem& spellBook, CItem& iDropped )
{
	if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
	{
		Weight->SubtractItemWeight( &mChar, &iDropped );
	}
	if( iDropped.GetId( 1 ) != 0x1F || iDropped.GetId( 2 ) < 0x2D || iDropped.GetId( 2 ) > 0x72 )
	{
		Bounce( &mSock, &iDropped );
		mSock.SysMessage( 1202 ); // You can only place spell scrolls in a spellbook!
		return;
	}
	CChar *sbOwner = FindItemOwner( &spellBook );
	if( ValidateObject( sbOwner ) && sbOwner != &mChar && !mChar.CanSnoop() )
	{
		Bounce( &mSock, &iDropped );
		mSock.SysMessage( 1203 ); // You cannot place spells in other peoples spellbooks.
		return;
	}
	std::string name;
	name.reserve( MAX_NAME );
	if( iDropped.GetName()[0] == '#' )
	{
		GetTileName( iDropped, name );
	}
	else
	{
		name = iDropped.GetName();
	}

	if( spellBook.GetTempVar( CITV_MORE, 1 ) == 1 )	// using more1 to "lock" a spellbook for RP purposes
	{
		mSock.SysMessage( 1204 ); // There are no empty pages left in your book.
		Bounce( &mSock, &iDropped );
		return;
	}

	if( name == Dictionary->GetEntry( 1605 )) // All-Spell Scroll
	{
		if( spellBook.GetSpell( 0 ) == INVALIDSERIAL && spellBook.GetSpell( 1 ) == INVALIDSERIAL && spellBook.GetSpell( 2 ) == INVALIDSERIAL )
		{
			mSock.SysMessage( 1205 ); // You already have a full book!
			Bounce( &mSock, &iDropped );
			return;
		}
		spellBook.SetSpell( 0, INVALIDSERIAL );
		spellBook.SetSpell( 1, INVALIDSERIAL );
		spellBook.SetSpell( 2, INVALIDSERIAL );
	}
	else
	{
		SI32 targSpellNum = 0;
		UI16 scrollId = iDropped.GetId();
		if( scrollId == 0x1F2D )
		{
			targSpellNum = 6;
		}
		else if( scrollId > 0x1F2D && scrollId < 0x1F34 )
		{
			targSpellNum = ( scrollId - 0x1F2E );
		}
		else if( scrollId >= 0x1F34 && scrollId < 0x1F6D )
		{
			targSpellNum = ( scrollId - 0x1F2D );
		}

		if( Magic->HasSpell( &spellBook, targSpellNum ))
		{
			mSock.SysMessage( 1206 ); // You already have that spell.
			Bounce( &mSock, &iDropped );
			return;
		}
		else
		{
			Magic->AddSpell( &spellBook, targSpellNum );
		}
	}
	Effects->PlaySound( &mSock, 0x0042, false );
	if( iDropped.GetAmount() > 1 )
	{
		iDropped.IncAmount( -1 );
		Bounce( &mSock, &iDropped );
	}
	else
	{
		iDropped.Delete();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DropOnStack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when item is dropped on a stack of items
//o------------------------------------------------------------------------------------------------o
bool DropOnStack( CSocket& mSock, CChar& mChar, CItem& droppedOn, CItem& iDropped, bool &stackDeleted )
{
	// Check if player is allowed to drop item on a locked down stack of items
	if( droppedOn.IsLockedDown() && !ValidateLockdownAccess( &mChar, &mSock, &droppedOn, true ))
	{
		if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
		{
			Weight->SubtractItemWeight( &mChar, &iDropped );
		}
		mSock.SysMessage( 774 ); // That is locked down and you cannot use it.
		Bounce( &mSock, &iDropped );
		return false;
	}

	bool canHold = true;
	if( droppedOn.GetCont() != nullptr )
	{
		if( droppedOn.GetContSerial() >= BASEITEMSERIAL )
		{
			canHold = Weight->CheckPackWeight( &mChar, static_cast<CItem *>( droppedOn.GetCont() ), &iDropped );
		}
		else
		{
			canHold = Weight->CheckCharWeight( &mChar, static_cast<CChar *>( droppedOn.GetCont() ), &iDropped );
		}
	}
	if( !canHold )
	{
		if( droppedOn.GetContSerial() >= BASEITEMSERIAL )
		{
			mSock.SysMessage( 1385 ); // That pack cannot hold any more weight
		}
		else
		{
			mSock.SysMessage( 1743 ); // That person can not possibly hold more weight
		}
		if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
		{
			Weight->SubtractItemWeight( &mChar, &iDropped );
		}
		Bounce( &mSock, &iDropped );
		return false;
	}
	if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
	{
		Weight->SubtractItemWeight( &mChar, &iDropped );
	}
	stackDeleted = ( DoStacking( &mSock, &mChar, &iDropped, &droppedOn ) != &iDropped );
	if( !stackDeleted )	// if the item didn't stack or the stack was full
	{
		Bounce( &mSock, &iDropped );
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleAutoStack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Automatically stack an item with existing items in a container
//o------------------------------------------------------------------------------------------------o
UI16 HandleAutoStack( CItem *mItem, CItem *mCont, CSocket *mSock, CChar *mChar )
{
	// Check that container can hold more weight
	if( mItem != mCont->GetCont() && !Weight->CheckPackWeight( mChar, mCont, mItem ))
	{
		if( mSock != nullptr && ValidateObject( mChar ))
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( mChar, mItem );
			}
			mSock->SysMessage( 1385 ); // That pack cannot hold any more weight
			Bounce( mSock, mItem );
			return mItem->GetAmount();
		}
	}

	// Attempt to autostack item in container
	bool stackDeleted = ( AutoStack( mSock, mItem, mCont ) != mItem );
	if( !stackDeleted )
	{
		if( mSock != nullptr && ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ))
		{
			Weight->SubtractItemWeight( mChar, mItem );
		}

		return mItem->GetAmount();
	}

	return 0;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DropOnContainer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when item is dropped on a container
//o------------------------------------------------------------------------------------------------o
bool DropOnContainer( CSocket& mSock, CChar& mChar, CItem& droppedOn, CItem& iDropped, [[maybe_unused]] bool &stackDeleted, SI16 x, SI16 y, SI08 gridLoc )
{
	CChar *contOwner = FindItemOwner( &droppedOn );
	if( ValidateObject( contOwner ))
	{
		if( contOwner == &mChar )
		{
			CBaseObject *recurseCont = droppedOn.GetCont();
			while( ValidateObject( recurseCont ))
			{
				if( recurseCont->CanBeObjType( OT_ITEM ))
				{
					CItem *recurseItem = static_cast<CItem *>( recurseCont );
					if( recurseItem->GetType() == IT_TRADEWINDOW )
					{
						CItem *tradeWindowTwo = CalcItemObjFromSer( recurseItem->GetTempVar( CITV_MOREX ));
						if( ValidateObject( tradeWindowTwo ))
						{
							if( tradeWindowTwo->GetTempVar( CITV_MOREZ ) || recurseItem->GetTempVar( CITV_MOREZ ))
							{
								tradeWindowTwo->SetTempVar( CITV_MOREZ, 0 );
								recurseItem->SetTempVar( CITV_MOREZ, 0 );
								SendTradeStatus( tradeWindowTwo, recurseItem );
							}
							CChar *tw2Char = FindItemOwner( tradeWindowTwo );
							if( ValidateObject( tw2Char ))
							{
								CSocket *tw2Sock = tw2Char->GetSocket();
								if( tw2Sock != nullptr )
								{
									Effects->ItemSound( tw2Sock, recurseItem, ( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND ));
								}
							}
						}
						break;
					}
					else
					{
						recurseCont = recurseItem->GetCont();
					}
				}
				else
				{
					break;
				}
			}
		}
		else if( contOwner->IsNpc() && contOwner->GetNpcAiType() == AI_PLAYERVENDOR )
		{
			if( contOwner->GetOwnerObj() == &mChar || mChar.GetCommandLevel() > CL_PLAYER )
			{
				// Clear old values, if item have 'em
				iDropped.SetDesc( "" );
				iDropped.SetVendorPrice( 0 );

				// Ask player to enter price for item, or 0 to mark it as 'Not for Sale'
				mChar.SetSpeechMode( 3 );
				mChar.SetSpeechItem( &iDropped );
				mSock.SysMessage( 1207 ); // Set a price for this item.
			}
			else
			{
				if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
				{
					Weight->SubtractItemWeight( &mChar, &iDropped );
				}
				contOwner->TextMessage( &mSock, 9176, TALK, false ); // I can only accept items from the shop owner.

				Bounce( &mSock, &iDropped );
				return false;
			}
		}
		else if( mChar.GetCommandLevel() < CL_CNS && ( !contOwner->IsNpc() || !contOwner->IsTamed() ||
													  ( contOwner->GetId() != 0x0123 && contOwner->GetId() != 0x0124 && contOwner->GetId() != 0x0317 ) ||
													  ( contOwner->GetOwnerObj() != &mChar && !Npcs->CheckPetFriend( &mChar, contOwner ))))
		{
			if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( &mChar, &iDropped );
			}
			mSock.SysMessage( 1630 ); // You cannot put items in other players packs!
			Bounce( &mSock, &iDropped );
			return false;
		}
	}

	// Check if player is allowed to drop item on container
	if( droppedOn.IsLockedDown() )
	{
		auto iMultiObj = droppedOn.GetMultiObj();
		if( ValidateObject( iMultiObj ) && iMultiObj->IsSecureContainer( &droppedOn ))
		{
			if( !ValidateLockdownAccess( &mChar, &mSock, &droppedOn, true ))
			{
				if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
				{
					Weight->SubtractItemWeight( &mChar, &iDropped );
				}
				mSock.SysMessage( 774 ); // That is locked down and you cannot use it.
				Bounce( &mSock, &iDropped );
				return false;
			}
		}
	}

	// Check if container can hold more items
	if( iDropped.GetContSerial() != droppedOn.GetSerial() && ( mSock.PickupSpot() != PL_OWNPACK && ( GetTotalItemCount( &droppedOn ) >= droppedOn.GetMaxItems() ||
		GetTotalItemCount( &droppedOn ) + std::max( static_cast<UI32>( 1 ), 1 + GetTotalItemCount( &iDropped )) > droppedOn.GetMaxItems() )))
	{
		if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
		{
			Weight->SubtractItemWeight( &mChar, &iDropped );
		}
		mSock.SysMessage( 1818 ); // The container is already at capacity.
		Bounce( &mSock, &iDropped );
		return false;
	}

	if( mSock.GetByte( 5 ) != 0xFF )	// In a specific spot in a container
	{
		if( mSock.PickupSpot() == PL_OTHERPACK || mSock.PickupSpot() == PL_GROUND )
		{
			Weight->SubtractItemWeight( &mChar, &iDropped );
		}

		if( &droppedOn != iDropped.GetCont() && !Weight->CheckPackWeight( &mChar, &droppedOn, &iDropped ))
		{
			mSock.SysMessage( 1385 ); // That pack cannot hold any more weight
			Bounce( &mSock, &iDropped );
			return false;
		}
		iDropped.SetCont( &droppedOn );
		iDropped.SetX( x );
		iDropped.SetY( y );
		iDropped.SetZ( 9 );
		iDropped.SetGridLocation( gridLoc );

		// Only send tooltip if server feature for tooltips is enabled
		if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
		{
			// Refresh target container tooltip
			CPToolTip pSend( droppedOn.GetSerial(), &mSock );
			mSock.Send( &pSend );
		}
	}
	else // Drop directly on a container, placing it randomly inside
	{
		[[maybe_unused]] UI16 amountLeft = HandleAutoStack( &iDropped, &droppedOn, &mSock, &mChar );

		// Only send tooltip if server feature for tooltips is enabled
		if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
		{
			// Refresh target container tooltip
			CPToolTip pSend( droppedOn.GetSerial(), &mSock );
			mSock.Send( &pSend );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DropOnItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when an item is dropped in a container or on another item
//o------------------------------------------------------------------------------------------------o
void DropOnItem( CSocket *mSock, SERIAL item, SERIAL dest, SI16 x, SI16 y, SI08 z, SI08 gridLoc )
{
	CChar *mChar = mSock->CurrcharObj();
	CItem *nCont = CalcItemObjFromSer( dest );
	if( !ValidateObject( nCont ) || !ValidateObject( mChar ))
		return;

	CItem *nItem = CalcItemObjFromSer( item );
	if( !ValidateObject( nItem ))
		return;

	nItem->SetHeldOnCursor( false );

	bool executeCont = true;
	std::vector<UI16> scriptTriggers = nItem->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			SI08 rVal = toExecute->OnDropItemOnItem( nItem, mChar, nCont );	// returns 1 if we should bounce it
			switch( rVal )
			{
				case -1:	// no such event
				default:
					executeCont = true;
					break;
				case 0:	// bounce, no code, handle player feedback in script
					if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
					{
						Weight->SubtractItemWeight( mChar, nItem );
					}
					Bounce( mSock, nItem );
					return;	// stack not deleted, as we're bouncing
				case 1:	// don't bounce, use code
					executeCont = false;
					break;
				case 2:	// don't bounce, don't use code
					executeCont = true;
					break;
			}
		}
	}

	if( executeCont )
	{
		std::vector<UI16> contScriptTriggers = nCont->GetScriptTriggers();
		for( auto scriptTrig : contScriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				SI08 rVal = toExecute->OnDropItemOnItem( nItem, mChar, nCont );	// returns 1 if we should bounce it
				switch( rVal )
				{
					case 1:	// don't bounce, use code
					case -1:	// no such event
					default:
						break;
					case 0:	// bounce, no code, handle player feedback in script
						if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
						{
							Weight->SubtractItemWeight( mChar, nItem );
						}
						Bounce( mSock, nItem );
						return;	// stack not deleted, as we're bouncing
					case 2:	// don't bounce, don't use code
						return;
				}
			}
		}
	}

	bool stackDeleted = false;

	CTile& tile = Map->SeekTile( nItem->GetId() );
	if( !mChar->AllMove() && ( nItem->GetMovable() == 2 || ( nItem->IsLockedDown() && nItem->GetOwnerObj() != mChar ) ||
								( tile.Weight() == 255 && nItem->GetMovable() != 1 )))
	{
		if( mChar->GetCommandLevel() < 2 || mSock->PickupSpot() != PL_OWNPACK )
		{
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( mChar, nItem );
			}
			mSock->SysMessage( 9064 ); // You cannot pick that up.
			Bounce( mSock, nItem );
			return;
		}
	}

	if( nCont->GetType() == IT_TRADEWINDOW && FindItemOwner( nCont ) == mChar )	// Trade window
	{
		DropOnTradeWindow(( *mSock), ( *mChar), ( *nCont ), ( *nItem ), x, y, gridLoc );
	}
	else if( nCont->GetType() == IT_TRASHCONT )	// Trash container
	{
		Effects->PlaySound( mSock, 0x0042, false );
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->SubtractItemWeight( mChar, nItem );
		}
		nItem->Delete();
		mSock->SysMessage( 1201 ); // As you let go of the item it disappears.
		return;
	}
	else if( nCont->GetType() == IT_SPELLBOOK )	// Spell Book
	{
		DropOnSpellBook(( *mSock ), ( *mChar ), ( *nCont ), ( *nItem ));
		return;
	}
	else if( nCont->GetType() == IT_CONTAINER )
	{
		if( !DropOnContainer(( *mSock ), ( *mChar ), ( *nCont ), ( *nItem ), stackDeleted, x, y, gridLoc ))
		{
			return;
		}
	}
	else if( nCont->IsPileable() && nItem->IsPileable() && nCont->GetId() == nItem->GetId() && nCont->GetColour() == nItem->GetColour() && nCont->GetTempVar( CITV_MORE ) == nItem->GetTempVar( CITV_MORE )
		&& nCont->GetTempVar( CITV_MOREX ) == nItem->GetTempVar( CITV_MOREX ) && nCont->GetTempVar( CITV_MOREY ) == nItem->GetTempVar( CITV_MOREY ) && nCont->GetTempVar( CITV_MOREZ ) == nItem->GetTempVar( CITV_MOREZ )
		&& nCont->GetBuyValue() == nItem->GetBuyValue() && nCont->GetSellValue() == nItem->GetSellValue() )
	{	// Stacking
		if( !DropOnStack(( *mSock ), ( *mChar), ( *nCont ), ( *nItem ), stackDeleted ))
		{
			return;
		}
	}
	else
	{
		if( nCont->GetType() == IT_SPAWNCONT || nCont->GetType() == IT_UNLOCKABLESPAWNCONT ) // - Unlocked item spawner or unlockable item spawner
		{
			nItem->SetCont( nCont );
		}
		else
		{
			nItem->SetCont( nCont->GetCont() );
		}

		nItem->SetX( x );
		nItem->SetY( y );
		nItem->SetZ( z );
		nItem->SetGridLocation( gridLoc );

		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			Weight->SubtractItemWeight( mChar, nItem );
		}
	}
	if( !stackDeleted )
	{
		Effects->ItemSound( mSock, nItem, ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ));
	}

	//Would be nice to do this to instantly update the stats & weight on the container the item is dropped on
	// but unfortunately it seems like updating the container will also force-close it :/
	// nCont->Dirty( UT_UPDATE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIDropItem::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when an item is dropped
//o------------------------------------------------------------------------------------------------o
bool CPIDropItem::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	ourChar->BreakConcentration( tSock );

	if( dest >= BASEITEMSERIAL && dest != INVALIDSERIAL )
	{
		DropOnItem( tSock, item, dest, x, y, z, gridLoc );
	}
	else
	{
		Drop( tSock, item, dest, x, y, z, gridLoc );
	}

	// Display overloaded message if character is overloaded as a result of the above
	if( Weight->IsOverloaded( ourChar ))
	{
		SI32 maxWeight = ourChar->GetStrength() * cwmWorldState->ServerData()->WeightPerStr() + 40;
		SI32 currentWeight = ourChar->GetWeight() / 100;
		tSock->SysMessage( 1784, currentWeight, maxWeight ); //You are overloaded. Current / Max: %i / %i
	}

	// Only send tooltip if server feature for tooltips is enabled
	if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
	{
		// Refresh source container tooltip
		CPToolTip pSend( tSock->PickupSerial(), tSock );
		tSock->Send(&pSend);
	}

	//Reset PickupSpot after dropping the item
	tSock->PickupSpot( PL_NOWHERE );
	tSock->SetCursorItem( nullptr );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	BestSkill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return characters highest skill
//o------------------------------------------------------------------------------------------------o
UI08 BestSkill( CChar *mChar, SKILLVAL &skillLevel )
{
	UI08 retSkill = 0;
	std::vector<cSkillClass> vecSkills;
	for( UI08 sCtr = 0; sCtr < ALLSKILLS; ++sCtr )
	{
		vecSkills.push_back( cSkillClass( sCtr, mChar->GetBaseSkill( sCtr )));
	}

	std::sort( vecSkills.rbegin(), vecSkills.rend() );
	if( vecSkills[0].value > 0 )
	{
		retSkill	= vecSkills[0].skill;
		skillLevel	= vecSkills[0].value;
	}
	return retSkill;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetSkillProwessTitle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches the characters "prowess" and "skill" title based upon titles.dfn
//|                 entries and characters best skill
//o------------------------------------------------------------------------------------------------o
void GetSkillProwessTitle( CChar *mChar, std::string &SkillProwessTitle )
{
	if( cwmWorldState->prowessTitles.empty() )
		return;

	SKILLVAL skillLevel = 0;
	UI08 bestSkill = BestSkill( mChar, skillLevel );
	if( skillLevel <= cwmWorldState->prowessTitles[0].lowBound )
	{
		SkillProwessTitle = cwmWorldState->prowessTitles[0].toDisplay;
	}
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
	SkillProwessTitle += " " + cwmWorldState->title[static_cast<UI08>( bestSkill + 1 )].skill;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetFameTitle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns players reputation title based on their Fame and Karma
//o------------------------------------------------------------------------------------------------o
void GetFameTitle( CChar *p, std::string& fameTitle )
{
	if( ValidateObject( p ))
	{
		std::string theTitle;
		UI08 titleNum = 0xFF;

		SI16 k = p->GetKarma();
		SI16 f = p->GetFame();

		if( k >= 10000 )
		{
			if( f >= 5000 )
			{
				titleNum = 0;
			}
			else if( f >= 2500 )
			{
				titleNum = 1;
			}
			else if( f >= 1250 )
			{
				titleNum = 2;
			}
			else
			{
				titleNum = 3;
			}
		}
		else if( k >= 5000 )
		{
			if( f >= 5000 )
			{
				titleNum = 4;
			}
			else if( f >= 2500 )
			{
				titleNum = 5;
			}
			else if( f >= 1250 )
			{
				titleNum = 6;
			}
			else
			{
				titleNum = 7;
			}
		}
		else if( k >= 2500 )
		{
			if( f >= 5000 )
			{
				titleNum = 8;
			}
			else if( f >= 2500 )
			{
				titleNum = 9;
			}
			else if( f >= 1250 )
			{
				titleNum = 10;
			}
			else
			{
				titleNum = 11;
			}
		}
		else if( k >= 1250 )
		{
			if( f >= 5000 )
			{
				titleNum = 12;
			}
			else if( f >= 2500 )
			{
				titleNum = 13;
			}
			else if( f >= 1250 )
			{
				titleNum = 14;
			}
			else
			{
				titleNum = 15;
			}
		}
		else if( k >= 625 )
		{
			if( f >= 5000 )
			{
				titleNum = 16;
			}
			else if( f >= 1000 )
			{
				titleNum = 17;
			}
			else if( f >= 500 )
			{
				titleNum = 18;
			}
			else
			{
				titleNum = 19;
			}
		}
		else if( k > -625 )
		{
			if( f >= 5000 )
			{
				titleNum = 20;
			}
			else if( f >= 2500 )
			{
				titleNum = 21;
			}
			else if( f >= 1250 )
			{
				titleNum = 22;
			}
			else
			{
				titleNum = 23;
			}
		}
		else if( k > -1250 )
		{
			if( f >= 10000 )
			{
				titleNum = 28;
			}
			else if( f >= 5000 )
			{
				titleNum = 27;
			}
			else if( f >= 2500 )
			{
				titleNum = 26;
			}
			else if( f >= 1250 )
			{
				titleNum = 25;
			}
			else
			{
				titleNum = 24;
			}
		}
		else if( k > -2500 )
		{
			if( f >= 5000 )
			{
				titleNum = 32;
			}
			else if( f >= 2500 )
			{
				titleNum = 31;
			}
			else if( f >= 1250 )
			{
				titleNum = 30;
			}
			else
			{
				titleNum = 29;
			}
		}
		else if( k > -5000 )
		{
			if( f >= 10000 )
			{
				titleNum = 37;
			}
			else if( f >= 5000 )
			{
				titleNum = 36;
			}
			else if( f >= 2500 )
			{
				titleNum = 35;
			}
			else if( f >= 1250 )
			{
				titleNum = 34;
			}
			else
			{
				titleNum = 33;
			}
		}
		else if( k > -10000 )
		{
			if( f >= 5000 )
			{
				titleNum = 41;
			}
			else if( f >= 2500 )
			{
				titleNum = 40;
			}
			else if( f >= 1250 )
			{
				titleNum = 39;
			}
			else
			{
				titleNum = 38;
			}
		}
		else if( k <= -10000 )
		{
			if( f >= 5000 )
			{
				titleNum = 45;
			}
			else if( f >= 2500 )
			{
				titleNum = 44;
			}
			else if( f >= 1250 )
			{
				titleNum = 43;
			}
			else
			{
				titleNum = 42;
			}
		}
		if( !cwmWorldState->title[titleNum].fame.empty() )
		{
			theTitle = cwmWorldState->title[titleNum].fame + " ";
		}

		/*if( cwmWorldState->ServerData()->ShowRaceWithName() && p->GetRace() != 0 && p->GetRace() != 65535 )
		{
			theTitle = theTitle + Races->Name( p->GetRace() ) + " ";
		}*/

		if( f >= 10000 ) // bugfix for repsys
		{
			if( p->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
			{
				if( p->GetId( 2 ) == 0x91 )
				{
					fameTitle = oldstrutil::format( Dictionary->GetEntry( 1177 ), Races->Name( p->GetRace() ).c_str() ) + std::string( " " );
				}
				else
				{
					fameTitle = oldstrutil::format( Dictionary->GetEntry( 1178 ), Races->Name( p->GetRace() ).c_str() ) + std::string( " " );
				}
			}
			else if( p->GetId( 2 ) == 0x91 )
			{
				fameTitle = oldstrutil::format( Dictionary->GetEntry( 1179 ), theTitle.c_str() ) + std::string( " " );
			}
			else
			{
				fameTitle = oldstrutil::format( Dictionary->GetEntry( 1180 ), theTitle.c_str() ) + std::string( " " );
			}
		}
		else
		{
			if( p->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
			{
				fameTitle = Dictionary->GetEntry( 1181 ) + std::string( " " );
			}
			else if( !( theTitle = oldstrutil::removeTrailing( theTitle, "//" )).empty() )
			{
				fameTitle = oldstrutil::format( Dictionary->GetEntry( 1182 ), theTitle.c_str() );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PaperDoll()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Open a characters paperdoll and show titles based on skill,
//|					reputation, murder counts, race, ect.
//o------------------------------------------------------------------------------------------------o
void PaperDoll( CSocket *s, CChar *pdoll )
{
	CChar *myChar		= s->CurrcharObj();
	std::string tempstr;
	CPPaperdoll pd		= ( *pdoll );
	UnicodeTypes sLang	= s->Language();

	cScript *toExecute;
	std::vector<UI16> scriptTriggers = pdoll->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( toExecute->OnCharDoubleClick( myChar, pdoll ) == 0 )
			{
				// if it exists and we don't want hard code, return
				return;
			}
		}
	}

	// Also check global script!
	toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
	if( toExecute != nullptr )
	{
		if( toExecute->OnCharDoubleClick( myChar, pdoll ) == 0 )
		{
			// if it exists and we don't want hard code, return
			return;
		}
	}

	std::string skillProwessTitle;
	std::string fameTitle;
	GetSkillProwessTitle( pdoll, skillProwessTitle );
	GetFameTitle( pdoll, fameTitle );

	bool bContinue = false;
	if( pdoll->IsGM() )
	{
		tempstr = pdoll->GetName() + " " + pdoll->GetTitle();
	}
	else if( pdoll->IsNpc() )
	{
		// Prefix name with fame title, if it exists
		if( fameTitle != "" )
		{
			tempstr = fameTitle;
		}
		
		// Then add actual name
		tempstr += pdoll->GetNameRequest( myChar, NRS_PAPERDOLL );

		// Append race, if enabled
		if( cwmWorldState->ServerData()->ShowRaceInPaperdoll() && pdoll->GetRace() != 0 && pdoll->GetRace() != 65535 )
		{
			tempstr += " ("s + Races->Name( pdoll->GetRace() ) + ")";
		}

		// Append title, if one exists
		if( pdoll->GetTitle() != "" )
		{
			std::string tempTitle = GetNpcDictTitle( pdoll, s );
			tempstr += " " + tempTitle;
		}
	}
	else if( pdoll->IsDead() )
	{
		tempstr = pdoll->GetNameRequest( myChar, NRS_PAPERDOLL );
	}
	// Murder tags now scriptable in SECTION MURDERER - Titles.dfn
	else if( pdoll->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
	{
		if( cwmWorldState->murdererTags.empty() )
		{
			tempstr = oldstrutil::format( Dictionary->GetEntry( 374, sLang ), pdoll->GetNameRequest( myChar, NRS_PAPERDOLL ).c_str(), pdoll->GetTitle().c_str(), skillProwessTitle.c_str() );
		}
		else if( pdoll->GetKills() < cwmWorldState->murdererTags[0].lowBound )	// not a real murderer
		{
			bContinue = true;
		}
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
			{
				bContinue = true;
			}
			else
			{
				tempstr = cwmWorldState->murdererTags[kCtr].toDisplay + " " + pdoll->GetNameRequest( myChar, NRS_PAPERDOLL ) + ", " + pdoll->GetTitle() + skillProwessTitle;
			}
		}
	}
	else if( pdoll->IsCriminal() )
	{
		tempstr = oldstrutil::format( Dictionary->GetEntry( 373, sLang ), pdoll->GetNameRequest( myChar, NRS_PAPERDOLL ).c_str(), pdoll->GetTitle().c_str(), skillProwessTitle.c_str() );
	}
	else
	{
		bContinue = true;
	}
	if( bContinue )
	{
		tempstr = fameTitle + pdoll->GetNameRequest( myChar, NRS_PAPERDOLL );	//Repuation + Name

		// Append race, if enabled
		if( cwmWorldState->ServerData()->ShowRaceInPaperdoll() && pdoll->GetRace() != 0 && pdoll->GetRace() != 65535 )
		{
			tempstr += " ("s + Races->Name( pdoll->GetRace() ) + ")";
		}

		if( pdoll->GetTownTitle() || pdoll->GetTownPriv() == 2 )	// TownTitle
		{
			if( pdoll->GetTownPriv() == 2 )	// is Mayor
			{
				tempstr = oldstrutil::format( Dictionary->GetEntry( 379, sLang ), pdoll->GetNameRequest( myChar, NRS_PAPERDOLL ).c_str(), cwmWorldState->townRegions[pdoll->GetTown()]->GetName().c_str(), skillProwessTitle.c_str() );
			}
			else	// is Resident
			{
				tempstr = pdoll->GetNameRequest( myChar, NRS_PAPERDOLL ) + " of " + cwmWorldState->townRegions[pdoll->GetTown()]->GetName() + ", " + skillProwessTitle;
			}
		}
		else	// No Town Title
		{
			if( !pdoll->IsIncognito() && !pdoll->IsDisguised() && !( pdoll->GetTitle().empty() ))
			{
				if( cwmWorldState->ServerData()->ExpansionCoreShardEra() >= ER_T2A )
				{
					// Title & Skill
					tempstr += " " + pdoll->GetTitle() + ", " + skillProwessTitle;
				}
				else
				{
					// Title only
					tempstr += " " + pdoll->GetTitle();
				}
			}
			else if( cwmWorldState->ServerData()->ExpansionCoreShardEra() >= ER_T2A )
			{
				// Just Skill
				tempstr += ", " + skillProwessTitle;
			}
		}
	}
	pd.Text( tempstr );

	CChar *mChar = s->CurrcharObj();
	if( ValidateObject( mChar ) && ( mChar == pdoll ))
	{
		pd.FlagByte( 0x02 | ( mChar->IsAtWar() ? 0x01 : 0x00 ));
	}
	else if( ValidateObject( mChar ) && mChar->IsGM() )
	{
		pd.FlagByte( 0x02 );
	}

	s->Send( &pd );

	// Only send tooltip if server feature for tooltips is enabled
	if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
	{
		for( CItem *wearItem = pdoll->FirstItem(); !pdoll->FinishedItems(); wearItem = pdoll->NextItem() )
		{
			if( ValidateObject( wearItem ))
			{
				CPToolTip pSend( wearItem->GetSerial(), s );
				s->Send( &pSend );
			}
		}
	}
}

void MountCreature( CSocket *mSock, CChar *s, CChar *x );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	handleCharDoubleClick()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles double-clicks on a character
//o------------------------------------------------------------------------------------------------o
void handleCharDoubleClick( CSocket *mSock, SERIAL serial, bool keyboard )
{
	CChar *mChar	= mSock->CurrcharObj();
	CChar *c		= CalcCharObjFromSer( serial );
	if( !ValidateObject( c ))
		return;

	std::vector<UI16> scriptTriggers = c->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( toExecute->OnCharDoubleClick( mChar, c ) == 0 )	// if it exists and we don't want hard code, return
				return;
		}
	}

	if( c->IsNpc() )
	{
		CItem *pack		= nullptr;
		if( cwmWorldState->creatures[c->GetId()].MountId() != 0 )	// Is a mount
		{
			if(( c->IsTamed() && ( c->GetOwnerObj() == mChar || Npcs->CheckPetFriend( mChar, c ))) || mChar->GetCommandLevel() >= CL_GM )
			{
				if( ObjInRange( mChar, c, DIST_NEXTTILE ))
				{
					if( mChar->IsDead() )
					{
						mSock->SysMessage( 381 ); // You are dead and cannot do that.
					}
					else if( mChar->GetId() != mChar->GetOrgId() )
					{
						mSock->SysMessage( 380 ); // You cannot ride anything under polymorph effect.
					}
					else
					{
						MountCreature( mSock, mChar, c );
					}
				}
				else
				{
					mSock->SysMessage( 382 ); // You need to get closer.
				}
			}
			else
			{
				mSock->SysMessage( 1214 ); // You don't own that creature.
			}
			return;
		}
		else if( !cwmWorldState->creatures[c->GetId()].IsHuman() && !c->IsDead() )
		{
			if( c->GetId() == 0x0123 || c->GetId() == 0x0124 || c->GetId() == 0x0317 )	// Is a pack animal
			{
				if( mChar->IsDead() )
				{
					mSock->SysMessage( 392 ); // You are dead and cannot do that.
				}
				else if( !ObjInRange( mChar, c, DIST_NEARBY ))
				{
					mSock->SysMessage( 382 ); // You need to get closer.
				}
				else
				{
					pack = c->GetPackItem();
					if( mChar->GetCommandLevel() >= CL_CNS || c->GetOwnerObj() == mChar || Npcs->CheckPetFriend( mChar, c ))
					{
						if( ValidateObject( pack ))
						{
							mSock->OpenPack( pack );
						}
						else
						{
							Console << "Pack animal " << c->GetSerial() << " has no backpack!" << myendl;
						}
					}
					else
					{
						Skills->Snooping( mSock, c, pack );
						mSock->SysMessage( 383 ); // That is not your beast of burden!
					}
				}
			}
			return;
		}
		else if( c->GetNpcAiType() == AI_PLAYERVENDOR ) // PlayerVendors
		{
			c->TextMessage( mSock, 385, TALK, false ); // Take a look at my goods.
			pack = c->GetPackItem();
			if( ValidateObject( pack ))
			{
				mSock->OpenPack( pack, true );
			}
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleDoubleClickTypes()
//|	Date		-	2/11/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs a switch to match an item type to a function
//o------------------------------------------------------------------------------------------------o
bool HandleDoubleClickTypes( CSocket *mSock, CChar *mChar, CItem *iUsed, ItemTypes iType )
{
	CChar *iChar	= nullptr;
	CItem *i		= nullptr;
	UI16 itemId		= iUsed->GetId();
	bool canTrap	= false;

	// Begin Check items by type
	switch( iType )
	{
		case IT_NOTYPE:
		case IT_COUNT:
		case IT_HOUSEADDON:
			return false;
		case IT_CONTAINER:	// Container, Backpack
		case IT_SPAWNCONT: // Item spawn container
			canTrap = true;
			[[fallthrough]]; // Indicate to compiler that fallthrough is valid to suppress warning
		case IT_UNLOCKABLESPAWNCONT:	// Unlockable item spawn container
		case IT_TRASHCONT:	// Trash container
			bool packOpened;
			packOpened = false;
			CBaseObject *baseCont;
			ObjectType objType;
			baseCont = FindItemOwner( iUsed, objType );
			if( !ValidateObject( baseCont ))
			{
				baseCont = iUsed;
			}
			if( ValidateObject( baseCont ))
			{
				if( baseCont->CanBeObjType( OT_ITEM ))
				{
					CMultiObj * baseContMultiObj = baseCont->GetMultiObj();

					if( baseContMultiObj == nullptr || mChar->GetMultiObj() == baseContMultiObj )
					{
						if( baseContMultiObj && baseContMultiObj->IsSecureContainer( static_cast<CItem *>( baseCont )) && !mChar->GetMultiObj()->IsOnOwnerList( mChar ))
						{
							mSock->SysMessage( 9011 ); // That container is secure. You cannot use this unless you are the owner.
							return true;
						}
						else
						{
							mSock->OpenPack( iUsed );
							packOpened = true;
						}
					}
				}
				else
				{
					iChar = static_cast<CChar *>( baseCont );
					if( ValidateObject( iChar ))
					{
						if( mChar == iChar || mChar == iChar->GetOwnerObj() )
						{
							if( iChar->GetNpcAiType() == AI_PLAYERVENDOR ) // PlayerVendors
							{
								mSock->OpenPack( iUsed, true );
							}
							else
							{
								if( mChar->GetPackItem() == iUsed )
								{
									mChar->SetWeight( Weight->CalcCharWeight( mChar ));
								}
								mSock->OpenPack( iUsed );
							}
						}
						else if( iChar->GetNpcAiType() == AI_PLAYERVENDOR ) // PlayerVendors
						{
							mSock->OpenPack( iUsed, true );
						}
						else
						{
							CItem *rootItem = FindRootContainer( iUsed );
							if( ValidateObject( rootItem ) && rootItem->GetType() == IT_TRADEWINDOW )
							{
								mSock->OpenPack( iUsed, true );
							}
							else
							{
								Skills->Snooping( mSock, iChar, iUsed );
							}
						}
						packOpened = true;
					}
				}
			}
			if( packOpened )
			{
				if( canTrap && iUsed->GetTempVar( CITV_MOREZ, 1 ))// Is trapped
				{
					Magic->MagicTrap( mChar, iUsed );
				}
			}
			else
			{
				mSock->SysMessage( 400 ); // You are too far away!
			}
			return true;
		case IT_CASTLEGATEOPENER:	// Order gate opener
			i = CalcItemObjFromSer( iUsed->GetTempVar( CITV_MORE ));
			if( ValidateObject( i ))
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
		case IT_LOCKEDCONTAINER:	// Locked container
		case IT_LOCKEDSPAWNCONT: // locked spawn container
			mSock->SysMessage( 1428 ); // This item is locked.
			return true;
		case IT_SPELLBOOK:	// Spellbook
			i = mChar->GetPackItem();
			if( ValidateObject( i ))
			{
				if(( iUsed->GetCont() == i || iUsed->GetCont() == mChar ) || iUsed->GetLayer() == IL_RIGHTHAND )
				{
					Magic->SpellBook( mSock );
				}
				else
				{
					mSock->SysMessage( 403 ); // If you wish to open a spellbook, it must be equipped or in your main backpack.
				}
			}
			return true;
		case IT_SPELLCHANNELING: //  Spell Channeling
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
		case IT_BOOK:	// Readable book
			if( iUsed->GetTempVar( CITV_MOREX ) != 666 && iUsed->GetTempVar( CITV_MOREX ) != 999 )
			{
				Books->OpenPreDefBook( mSock, iUsed );
			}
			else
			{
				Books->OpenBook( mSock, iUsed, ( iUsed->GetTempVar( CITV_MOREX ) == 666 ));
			}
			return true;
		case IT_MAGICWAND: // Magic Wands
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, true ))
				return true;

			if( iUsed->GetTempVar( CITV_MOREZ ) != 0 )
			{
				mSock->CurrentSpellType( 2 );
				if( Magic->SelectSpell( mSock, iUsed->GetTempVar( CITV_MOREY )))
				{
					iUsed->SetTempVar( CITV_MOREZ, iUsed->GetTempVar( CITV_MOREZ ) - 1 );
					if( iUsed->GetTempVar( CITV_MOREZ ) == 0 ) // Number of charges have dropped to zero
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
			if( itemId == 0x14F0 )		// Check for Deed
			{
				// If item is locked down, check if player has access to use it
				if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, false ))
					return true;

				Items->CreateScriptItem( nullptr, mChar, "townstone", 1, OT_ITEM );
				iUsed->Delete();
			}
			else	// Display Townstone gump
			{
				CTownRegion *useRegion = iUsed->GetRegion();
				if( useRegion != nullptr )
				{
					useRegion->DisplayTownMenu( iUsed, mSock );
				}
			}
			return true;
		case IT_RECALLRUNE: // Recall Rune
			if( iUsed->GetTempVar( CITV_MOREX ) == 0 && iUsed->GetTempVar( CITV_MOREY ) == 0 && iUsed->GetTempVar( CITV_MOREZ ) == 0 )	// changed, to fix, Lord Vader
			{
				mSock->SysMessage( 431 ); // That rune is not yet marked!
			}
			else
			{
				// If item is locked down, check if player has access to use it
				if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, false ))
					return true;

				mSock->SysMessage( 432 ); // Enter new rune name.
				mChar->SetSpeechItem( iUsed );
				mChar->SetSpeechMode( 7 );
			}
			return true;
		case IT_OBJTELEPORTER:	// Object Teleporter
			if( ObjInRange( mChar, iUsed, DIST_NEARBY ))
			{
				mChar->SetLocation( static_cast<SI16>( iUsed->GetTempVar( CITV_MOREX )), static_cast<SI16>( iUsed->GetTempVar( CITV_MOREY )), static_cast<SI08>( iUsed->GetTempVar( CITV_MOREZ )));
			}
			return true;
		case IT_MAPCHANGEOBJECT:
			SocketMapChange( mSock, mChar, iUsed );
			return true;
		case IT_MORPHOBJECT: // Morph Object
			mChar->SetId( static_cast<UI16>(iUsed->GetTempVar( CITV_MOREX )));
			iUsed->SetType( IT_UNMORPHOBJECT );
			return true;
		case IT_UNMORPHOBJECT: // Unmorph Object
			mChar->SetId( mChar->GetOrgId() );
			iUsed->SetType( IT_MORPHOBJECT );
			return true;
		case IT_PLANK:	// Planks
			if( ObjInRange( mChar, iUsed, DIST_INRANGE ))
			{
				auto plankStatus = iUsed->GetTag( "plankLocked" );
				if( plankStatus.m_IntValue == 1 ) // Is plank locked?
				{
					auto iMulti = iUsed->GetMultiObj();
					auto mMulti = mChar->GetMultiObj();
					if( ValidateObject( iMulti ) && ValidateObject( mMulti ) && iMulti == mMulti )
					{
						// Allow opening plank for disembarking purposes if player is onboard boat
						switch( iUsed->GetId() )
						{
							// Open the plank visually
							case 0x3EE9: iUsed->SetId( 0x3E84 ); break;
							case 0x3EB1: iUsed->SetId( 0x3ED5 ); break;
							case 0x3EB2: iUsed->SetId( 0x3ED4 ); break;
							case 0x3E8A: iUsed->SetId( 0x3E89 ); break;
							case 0x3E85: iUsed->SetId( 0x3E84 ); break;
							// Exit from the open (but locked) plank
							case 0x3E84: [[fallthrough]];
							case 0x3ED5: [[fallthrough]];
							case 0x3ED4: [[fallthrough]];
							case 0x3E89: PlankStuff( mSock, iUsed ); break;
							default: return true;
						}

						// Start a timer to auto-close the plank
						mSock->SysMessage( 9144 ); // You open the plank, though it's still locked.
						iUsed->SetTempTimer( BuildTimeValue( static_cast<R32>( 5 )));
					}
					else
					{
						// Plank is locked! Tough luck
						mSock->SysMessage( 398 ); // Locked
					}
				}
				else
				{
					// Plank is unlocked! Open the plank, or use it to disembark
					switch( iUsed->GetId() )
					{
						case 0x3EE9: iUsed->SetId( 0x3E84 ); break;
						case 0x3EB1: iUsed->SetId( 0x3ED5 ); break;
						case 0x3EB2: iUsed->SetId( 0x3ED4 ); break;
						case 0x3E8A: iUsed->SetId( 0x3E89 ); break;
						case 0x3E85: iUsed->SetId( 0x3E84 ); break;
						default: PlankStuff( mSock, iUsed ); return true;;
					}
					mSock->SysMessage( 2031 ); // You open the plank
				}
			}
			else
			{
				mSock->SysMessage( 399 ); // You can't reach that!
			}
			return true;
		case IT_FIREWORKSWAND: //Fireworks wands
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, true ))
				return true;

			if( iUsed->GetTempVar( CITV_MOREX ) == 0 )
			{
				mSock->SysMessage( 396 ); // That is out of charges!
				return true;
			}
			iUsed->SetTempVar( CITV_MOREX, iUsed->GetTempVar( CITV_MOREX ) - 1 );
			mSock->SysMessage( 397, iUsed->GetTempVar( CITV_MOREX ));
			UI08 j;
			for( j = 0; j < static_cast<UI08>( RandomNum( 0, 3 ) + 2 ); ++j )
			{
				SI16 wx = ( mChar->GetX() + RandomNum( 0, 5 ) - 5 );
				SI16 wy = ( mChar->GetY() - RandomNum( 0, 7 ));
				Effects->PlayMovingAnimation( mChar, wx, wy, mChar->GetZ() + 10, 0x36E4, 17, 0, ( RandomNum( 0, 1 ) == 1 ));
				UI16 animId;
				switch( RandomNum( 0, 4 ))
				{
					default:
					case 0:	animId = 0x373A;		break;
					case 1:	animId = 0x374A;		break;
					case 2:	animId = 0x375A;		break;
					case 3:	animId = 0x376A;		break;
					case 4:	animId = 0x377A;		break;
				}
				Effects->PlayStaticAnimation( wx, wy, mChar->GetZ() + 10, animId, RandomNum( 0x04, 0x09 ), 30, 0 );
			}
			return true;
		case IT_RENAMEDEED: // Rename Deed
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, false ))
				return true;

			// Don't allow using GM-locked down tools
			if( iUsed->GetMovable() == 2 )
			{
				mSock->SysMessage( 1032 ); // This is not yours!
				return true;
			}

			mChar->SetSpeechItem( iUsed );
			mChar->SetSpeechMode( 6 );
			mSock->SysMessage( 434 ); // Enter your new name.
			return true;
		case IT_TILLER:	// Tillerman
		{
			CBoatObj *boat = static_cast<CBoatObj *>( iUsed->GetMultiObj() );
			if( ValidateObject( boat ))
			{
				if( boat->GetMoveType() != BOAT_ANCHORED )
				{
					mSock->SysMessage( 2029 ); // You must lower the anchor to dock the boat.
				}
				else
				{
					if( ObjInRange( mChar, boat, DIST_INRANGE ))
					{
						ModelBoat( mSock, boat );
					}
					else
					{
						mSock->SysMessage( 461 ); // You are too far away.
					}
				}
			}
			return true;
		}
		case IT_GUILDSTONE:	// Guildstone Deed
			if( itemId == 0x14F0 || itemId == 0x1869 )	// Check for Deed/Teleporter + Guild Type
			{
				// If item is locked down, check if player has access to use it
				if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, false ))
					return true;

				mChar->SetSpeechItem( iUsed );		// we're using this as a generic item
				GuildSys->PlaceStone( mSock, iUsed );
				return true;
			}
			else if( itemId == 0x0ED5 )			// Check for Guildstone + Guild Type
			{
				mSock->TempInt( iUsed->GetTempVar( CITV_MORE ));	// track things properly
				if( mChar->GetGuildNumber() == -1 || mChar->GetGuildNumber() != static_cast<SI16>( iUsed->GetTempVar( CITV_MORE )))
				{
					mSock->SysMessage( 1984 ); // You are not a member of this guild!
				}
				else if( mChar->GetGuildNumber() == static_cast<SI16>( iUsed->GetTempVar( CITV_MORE )))
				{
					GuildSys->Menu( mSock, BasePage + 1, static_cast<GUILDID>( iUsed->GetTempVar( CITV_MORE ))); // more of the stone is the guild number
				}
				else
				{
					mSock->SysMessage( 438 ); // You already belong to a guild.
				}
				return true;
			}
			else
			{
				Console << "Unhandled guild item type named: " << iUsed->GetName() << " with ID of: " << itemId << myendl;
			}
			return true;
		case IT_METALREPAIRTOOL:
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, true ))
				return true;

			mSock->TempObj( iUsed );
			mSock->SendTargetCursor( 0, TARGET_REPAIRMETAL, 0, 485 );	// What do we wish to repair?
			return true;
		case IT_FORGE:	// Forges
			mSock->TempObj( iUsed );
			mSock->SendTargetCursor( 0, TARGET_SMELT, 0, 440 );
			return true;
		case IT_DYE:	// Dye
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, false ))
				return true;

			// Don't allow using GM-locked down tools
			if( iUsed->GetMovable() == 2 )
			{
				mSock->SysMessage( 1032 ); // This is not yours!
				return true;
			}

			mSock->TempObj( iUsed );
			mSock->DyeAll( 0 );
			mSock->SendTargetCursor( 0, TARGET_DYEALL, 0, 441 );
			return true;
		case IT_DYEVAT:	// Dye vat
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, true ))
				return true;

			// Don't allow using GM-locked down tools
			if( iUsed->GetMovable() == 2 )
			{
				mSock->SysMessage( 1032 ); // This is not yours!
				return true;
			}

			mSock->TempObj( iUsed );
			mSock->AddId1( iUsed->GetColour( 1 ));
			mSock->AddId2( iUsed->GetColour( 2 ));
			mSock->SendTargetCursor( 0, TARGET_DVAT, 0, 442 );
			return true;
		case IT_MODELMULTI:	// Model boat/Houses
			if( iType != IT_TOWNSTONE && iType != IT_GUILDSTONE )
			{
				// If item is locked down, check if player has access to use it
				if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, false ))
					return true;

				if( iUsed->GetTempVar( CITV_MOREX ) == 0 )
					break;

				mChar->SetSpeechItem( iUsed );
				DoHouseTarget( mSock, static_cast<UI16>( iUsed->GetTempVar( CITV_MOREX )));
			}
			return true;
		case IT_SMITHYTOOL:
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, true ))
				return true;

			// Don't allow using GM-locked down tools
			if( iUsed->GetMovable() == 2 )
			{
				mSock->SysMessage( 1032 ); // This is not yours!
				return true;
			}

			mSock->TempObj( iUsed );
			mSock->SendTargetCursor( 0, TARGET_SMITH, 0, 444 );
			return true;
		case IT_MININGTOOL:	// Mining
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, true ))
				return true;

			// Don't allow using GM-locked down tools
			if( iUsed->GetMovable() == 2 )
			{
				mSock->SysMessage( 1032 ); // This is not yours!
				return true;
			}

			// Don't allow using tools with no usage left, if usage limits are enabled in ini
			if( cwmWorldState->ServerData()->ToolUseLimit() && iUsed->GetUsesLeft() == 0 )
			{
				mSock->SysMessage( 9262 ); // This has no more charges
				return true;
			}

			mSock->TempObj( iUsed );
			mSock->SendTargetCursor( 0, TARGET_MINE, 0, 446 );
			return true;
		case IT_EMPTYVIAL:	// empty vial
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, true ))
				return true;

			// Don't allow using GM-locked down tools
			if( iUsed->GetMovable() == 2 )
			{
				mSock->SysMessage( 1032 ); // This is not yours!
				return true;
			}

			i = mChar->GetPackItem();
			if( ValidateObject( i ))
			{
				if( iUsed->GetCont() == i )
				{
					mSock->TempObj( iUsed );
					mSock->SendTargetCursor( 0, TARGET_VIAL, 0, 447 );
				}
				else
				{
					mSock->SysMessage( 448 ); // The vial is not in your pack!
				}
			}
			return true;
		case IT_ORE:	// smelt ore
			// If item is locked down, check if player has access to use it
			if( iUsed->IsLockedDown() && !ValidateLockdownAccess( mChar, mSock, iUsed, false ))
				return true;

			// Don't allow using GM-locked down tools
			if( iUsed->GetMovable() == 2 )
			{
				mSock->SysMessage( 1032 ); // This is not yours!
				return true;
			}

			if( iUsed->GetId() == 0x19B7 && iUsed->GetAmount() < 2 )
			{
				mSock->SysMessage( 1814 ); // Too little ore to smelt!
			}
			else
			{
				mSock->TempObj( iUsed );
				if( iUsed->GetId() == 0x19B7 )
				{
					mSock->SendTargetCursor( 0, TARGET_SMELTORE, 0, 460 ); // Select the forge on which to smelt the ore.
				}
				else
				{
					mSock->SendTargetCursor( 0, TARGET_SMELTORE, 0, 1815 ); // Select the forge on which to smelt the ore, or another pile of ore with which to combine it.
				}
			}
			return true;
		case IT_MESSAGEBOARD:	// Message board opening
			if( ObjInRange( mChar, iUsed, DIST_NEARBY ))
			{
				CPIMsgBoardEvent *mbEvent = new CPIMsgBoardEvent( mSock, false );
				mbEvent->Handle();
			}
			else
			{
				mSock->SysMessage( 461 ); // You are too far away.
			}
			return true;
		case IT_MAGICSTATUE:
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ))
			{
				if( itemId == 0x1508 )
				{
					iUsed->SetId( 0x1509 );
				}
				else
				{
					iUsed->SetId( 0x1508 );
				}
			}
			else
			{
				mSock->SysMessage( 465 ); // You failed to use this statue.
			}
			return true;
		case IT_GUILLOTINE:
			if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ))
			{
				if( itemId == 0x1245 )
				{
					iUsed->SetId( 0x1230 );
				}
				else
				{
					iUsed->SetId( 0x1245 );
				}
			}
			else
			{
				mSock->SysMessage( 466 ); // You failed to use this.
			}
			return true;
		default:
			if( iType )
			{
				Console << "Unhandled item type for item: " << iUsed->GetName() << "[" << iUsed->GetSerial() << "] of type: " << static_cast<UI16>( iType ) << myendl;
			}
			break;
	}
	return false;
}

std::map<std::string, ItemTypes> tagToItemType;

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
	tagToItemType["BOOK"]					= IT_BOOK;
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
	tagToItemType["SPELLCHANNELING"]		= IT_SPELLCHANNELING;
	tagToItemType["SHIELD"]					= IT_SHIELD;
}

ItemTypes FindItemTypeFromTag( const std::string &strToFind )
{
	if( tagToItemType.empty() )	// if we haven't built our array yet
	{
		InitTagToItemType();
	}
	std::map<std::string, ItemTypes>::const_iterator toFind = tagToItemType.find( oldstrutil::upper( strToFind ));
	if( toFind != tagToItemType.end() )
		return toFind->second;

	return IT_COUNT;
}

std::map<UI16, ItemTypes> idToItemType;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	InitIdToItemType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load in and map IDs to item types based on entries in itemtypes.dfn
//o------------------------------------------------------------------------------------------------o
auto InitIdToItemType() -> void
{
	auto Itemtypes = FileLookup->FindEntry( "ITEMTYPES", items_def );
	if( Itemtypes )
	{
		SI32 sectionCount;
		ItemTypes iType = IT_COUNT;
		for( const auto &sec : Itemtypes->collection() )
		{
			auto tag = sec->tag;
			auto data = sec->data;
			auto comma_secs = oldstrutil::sections( data, "," );
			iType = FindItemTypeFromTag( tag );
			if( iType != IT_COUNT )
			{
				sectionCount = static_cast<SI32>( comma_secs.size() - 1 );
				if( sectionCount != 0 )
				{
					for( SI32 i = 0; i <= sectionCount; i++ )
					{
						idToItemType[ oldstrutil::value<UI16>( oldstrutil::extractSection( data, ",", i, i ), 16 )] = iType;
					}
				}
				else
				{
					idToItemType[static_cast<UI16>( std::stoul( data, nullptr, 16 ))] = iType;
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindItemTypeFromId()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find an item's type based on its ID
//o------------------------------------------------------------------------------------------------o
ItemTypes FindItemTypeFromId( UI16 idToFind )
{
	if( idToItemType.empty() )	// if we haven't built our array yet
	{
		InitIdToItemType();
	}
	std::map<UI16, ItemTypes>::const_iterator toFind = idToItemType.find( idToFind );
	if( toFind != idToItemType.end() )
	{
		return toFind->second;
	}

	return IT_COUNT;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindItemType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find an item's type, or try to find it's type from its ID if no type is specified
//o------------------------------------------------------------------------------------------------o
ItemTypes FindItemType( CItem *i )
{
	ItemTypes iType = i->GetType();
	switch( iType )
	{
		case IT_NOTYPE:
		case IT_COUNT:
			iType = FindItemTypeFromId( i->GetId() );	break;
		default:										break;
	}

	return iType;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ItemIsUsable()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if item is usable and if player can actually use it
//o------------------------------------------------------------------------------------------------o
bool ItemIsUsable( CSocket *tSock, CChar *ourChar, CItem *iUsed, ItemTypes iType )
{
	if( ourChar->IsDead() && iType != IT_PLANK && iType != IT_HOUSESIGN )
	{
		if( iType == IT_RESURRECTOBJECT )	// Check for a resurrect item type
		{
			NpcResurrectTarget( ourChar );
			tSock->SysMessage( 390 ); // You have been resurrected.
		}
		else // If it's not a ressurect item, and you're dead, forget it
		{
			tSock->SysMessage( 392 ); // You are dead and cannot do that.
		}
		return false;
	}
	// Range check for double clicking on items
	if( iType != IT_PLANK && iType != IT_TILLER && iType != IT_HOUSESIGN )
	{
		bool canUse = CheckItemRange( ourChar, iUsed );
		if( canUse )
		{
			canUse = ( iType == IT_DOOR || iType == IT_LOCKEDDOOR || CheckItemLineOfSight( ourChar, iUsed ));
		}
		if( !canUse )
		{
			tSock->SysMessage( 389 ); // That is too far away and you cannot reach it.
			return false;
		}
	}
	if( ourChar->GetCommandLevel() < CL_CNS )
	{
		if(( tSock->GetTimer( tPC_OBJDELAY ) >= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ))
		{
			if( !tSock->ObjDelayMsgShown() )
			{
				tSock->SysMessage( 386 ); // You must wait to perform another action.
				tSock->ObjDelayMsgShown( true );
			}
			return false;
		}
		tSock->SetTimer( tPC_OBJDELAY, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_OBJECTUSAGE ));
		tSock->ObjDelayMsgShown( false );

		CChar *iChar = nullptr;
		if( iUsed->GetCont() != nullptr )
		{
			iChar = FindItemOwner( iUsed );
			if( ValidateObject( iChar ) && iChar != ourChar &&
			   !( iUsed->IsContType() && cwmWorldState->ServerData()->RogueStatus() ))
			{
				tSock->SysMessage( 387 ); // You cannot use items equipped by other players.
				return false;
			}
		}
		// Check if item is in a house?
		else if( iType != IT_DOOR && iType != IT_LOCKEDDOOR && iType != IT_PLANK && iType != IT_HOUSESIGN && iType != IT_TILLER )
		{
			if( iUsed->GetMultiObj() != nullptr && iUsed->GetMultiObj() != ourChar->GetMultiObj() )
			{
				tSock->SysMessage( 389 ); // That is too far away and you cannot reach it.
				return false;
			}
		}

		iChar = iUsed->GetOwnerObj();
		if( iUsed->IsCorpse() )
		{
			if( cwmWorldState->ServerData()->LootingIsCrime() )
			{
				bool willCrim	= false;
				if( ValidateObject( iChar ))
				{
					// if the corpse is from an innocent player, and is not our own corpse				if( otherCheck
					// and if the corpse is not from an enemy/allied guild									&& guildCheck
					// and if the races are not allied/enemy												&& raceCheck )
					willCrim = WillResultInCriminal( ourChar, iChar );
				}
				else
				{
					willCrim = (( iUsed->GetTempVar( CITV_MOREZ ) & 0x04 ) == 0x04 );
				}
				if( willCrim )
				{
					MakeCriminal( ourChar );
				}
			}

			if( ValidateObject( iChar ))
			{
				CTownRegion *itemTownRegion = iUsed->GetRegion();
				if( !itemTownRegion->IsGuarded() && !itemTownRegion->IsSafeZone() )
				{
					if( iChar->IsGuarded() ) // Is the corpse being guarded?
					{
						Combat->PetGuardAttack( ourChar, iChar, iChar );
					}
					else if( iUsed->IsGuarded() )
					{
						Combat->PetGuardAttack( ourChar, iChar, iUsed );
					}
				}
			}
		}
		else if( iUsed->IsGuarded() )
		{
			CMultiObj *multi = iUsed->GetMultiObj();
			if( ValidateObject( multi ))
			{
				CChar *multiOwner = multi->GetOwnerObj();
				if( ValidateObject( multiOwner ) && multiOwner != ourChar )
				{
					Combat->PetGuardAttack( ourChar, multiOwner, iUsed );
				}
			}
			else
			{
				CTownRegion *itemTownRegion = iUsed->GetRegion();
				if( !itemTownRegion->IsGuarded() && !itemTownRegion->IsSafeZone() )
				{
					if( ValidateObject( iChar ))
					{
						Combat->PetGuardAttack( ourChar, iChar, iUsed );
					}
				}
			}
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIDblClick::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Player double clicks on a character or item
//|	Changes		-	09/22/2002 - Removed piece of code which is not needed. Cooking of raw
//|						fish shouldn't produce cooked ribs, people can cook the fish after filleting with a knife.
//o------------------------------------------------------------------------------------------------o
bool CPIDblClick::Handle( void )
{
	CChar *ourChar	= tSock->CurrcharObj();
	ourChar->BreakConcentration( tSock );
	UI08 a1			= tSock->GetByte( 1 );

	bool keyboard = (( a1 & 0x80 ) == 0x80 );
	if( keyboard )
	{
		objectId -= 0x80000000;
	}

	if( objectId < BASEITEMSERIAL )
	{
		handleCharDoubleClick( tSock, objectId, keyboard );
		return true;
	}

	// Begin Items / Guildstones Section
	CItem *iUsed = CalcItemObjFromSer( objectId );
	if( !ValidateObject( iUsed ))
		return true;

	ItemTypes iType		= FindItemType( iUsed );
	bool scriptExecuted = false;
	std::vector<UI16> scriptTriggers = iUsed->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		// First loop through all scripts, checking for OnUseUnChecked events, which should run before
		// item usage check is performed
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr ) // Was a script found?
		{
			// If retVal is -1, event doesn't exist in script
			// If retVal is 0, event exists, but returned false/0, and handles item usage. Don't proceed with hard code (or other scripts!)
			// If retVal is 1, event exists, proceed with hard code/other scripts
			SI08 retVal = toExecute->OnUseUnChecked( ourChar, iUsed );
			if( retVal == 1 )
			{
				// Event exists, and returned 1 - proceed with other scripts/hard code
				scriptExecuted = true;
			}
			else if( retVal == 0 )
			{
				// Event exists, and returned 0 - script handles usage, don't continue!
				return true;
			}
		}
	}

	bool itemUsageCheckComplete = false;
	// Then loop through all scripts again, checking for OnUseChecked event - but first run item usage check
	// once to make sure player can actually use item!
	// If it hasn't been done at least once already, run the usual item-usage checks
	if( !itemUsageCheckComplete )
	{
		if( !ItemIsUsable( tSock, ourChar, iUsed, iType ))
		{
			return true;
		}

		// If item is disabled, don't continue
		if( iUsed->IsDisabled() )
		{
			tSock->SysMessage( 394 ); // You cannot do that at the moment.
			return true;
		}

		// Item-usage check completed!
		itemUsageCheckComplete = true;
	}

	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// Check if OnUseChecked event is present in script
			SI08 retVal = toExecute->OnUseChecked( ourChar, iUsed );
			if( retVal == 1 )
			{
				// Event exists, and returned 1 - proceed with other scripts/hard code
				scriptExecuted = true;
			}
			else if( retVal == 0 )
			{
				// Script returned 0 - don't continue, usage is handled in script!
				return true;
			}
		}
	}

	// If no scripts were found, or if no script contained onUse events, proceed with envoke stuff
	if( scriptTriggers.size() == 0 || !scriptExecuted )
	{
		UI16 itemId	= iUsed->GetId();
		cScript *toExecute = nullptr;
		UI16 envTrig = 0;

		if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>( iType )))
		{
			// Get script to run by item type
			envTrig = JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>( iType ));
			toExecute = JSMapping->GetScript( envTrig );
		}
		else if( JSMapping->GetEnvokeById()->Check( itemId ))
		{
			// Get script to run by item ID
			envTrig = JSMapping->GetEnvokeById()->GetScript( itemId );
			toExecute = JSMapping->GetScript( envTrig );
		}

		// Check for the onUse events in envoke scripts!
		if( toExecute != nullptr )
		{
			if( toExecute->OnUseUnChecked( ourChar, iUsed ) == 0 )
				return true;

			if( toExecute->OnUseChecked( ourChar, iUsed ) == 0 )
				return true;
		}
	}

	if( HandleDoubleClickTypes( tSock, ourChar, iUsed, iType ))
		return true;

	tSock->SysMessage( 486 ); // You can't think of a way to use that item.
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AppendData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add data onto the end of the string in singleclick() based on an items type
//|					This is used by legacy singleclick names that don't show tooltips
//o------------------------------------------------------------------------------------------------o
const char *AppendData( CSocket *s, CItem *i, std::string &currentName )
{
	std::string dataToAdd;
	switch( i->GetType() )
	{
		case IT_CONTAINER:
		case IT_SPAWNCONT:
		case IT_UNLOCKABLESPAWNCONT:
			dataToAdd = std::string( " (" ) + oldstrutil::number( static_cast<SI32>( i->GetContainsList()->Num() )) + std::string( " items, " );
			dataToAdd += oldstrutil::number(( i->GetWeight() / 100 )) + std::string( " stones)" );
			break;
		case IT_LOCKEDCONTAINER:		// containers
		case IT_LOCKEDSPAWNCONT:	// spawn containers
			dataToAdd = std::string( " (" ) + oldstrutil::number( static_cast<SI32>( i->GetContainsList()->Num() )) + std::string( " items, " );
			dataToAdd += oldstrutil::number(( i->GetWeight() / 100 )) + std::string( " stones) " + Dictionary->GetEntry( 9050, s->Language() )); // [Locked]
			break;
		case IT_LOCKEDDOOR:
			dataToAdd = " " + Dictionary->GetEntry( 9050 ); // [Locked]
			break;
		case IT_RECALLRUNE:
		case IT_GATE:
		case IT_OBJTELEPORTER:
		{
			CTownRegion *newRegion = CalcRegionFromXY( static_cast<SI16>( i->GetTempVar( CITV_MOREX )), static_cast<SI16>( i->GetTempVar( CITV_MOREY )), i->WorldNumber(), i->GetInstanceId() );
			dataToAdd = std::string( " (" ) + newRegion->GetName() + std::string( ")" );
			break;
		}
		default:
			break;
	}

	if( i->IsLockedDown() )
	{
		auto iMultiObj = i->GetMultiObj();
		if( ValidateObject( iMultiObj ) && iMultiObj->IsSecureContainer( i ))
		{
			dataToAdd += Dictionary->GetEntry( 9052, s->Language() ); // [locked down & secure]
		}
		else
		{
			dataToAdd += Dictionary->GetEntry( 9053, s->Language() ); // [locked down]
		}
	}
	if( i->IsGuarded() )
	{
		CTownRegion *itemTownRegion = i->GetRegion();
		if( !itemTownRegion->IsGuarded() && !itemTownRegion->IsSafeZone() )
		{
			dataToAdd += " " + Dictionary->GetEntry( 9051, s->Language() ); // [Guarded]
		}
	}
	if( i->IsNewbie() )
	{
		dataToAdd += " " + Dictionary->GetEntry( 9055, s->Language() ); // [Blessed]
	}
	
	// Is the item a magical item, and unidentified?
	if( !i->IsCorpse() && i->GetName2() != "#" && i->GetName2() != "" )
	{
		s->ObjMessage( Dictionary->GetEntry( 9402, s->Language() ), i ); // [Unidentified]
	}

	currentName += dataToAdd;
	// Question: Do we put the creator thing here, saves some redundancy a bit later
	return currentName.c_str();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPISingleClick::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when an item or character is single-clicked (also used for AllNames macro)
//o------------------------------------------------------------------------------------------------o
bool CPISingleClick::Handle( void )
{
	if( objectId == INVALIDSERIAL ) // invalid
		return true;

	if( objectId < BASEITEMSERIAL )
	{
		// Begin chars/npcs section
		CChar *c = CalcCharObjFromSer( objectId );
		if( ValidateObject( c ))
		{
			std::vector<UI16> scriptTriggers = c->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					// If script returns true/1, don't show hard-coded name, and don't run additional onClick events
					if( toExecute->OnClick( tSock, c ) == 1 )
					{
						return true;
					}
				}
			}
			tSock->ShowCharName( c, false );
		}

		//End chars/npcs section
		return true;
	}

	std::string temp2;
	std::string realname;

	CChar *mChar = tSock->CurrcharObj();

	//Begin items section
	UI08 a1		= tSock->GetByte( 1 );
	UI08 a2		= tSock->GetByte( 2 );
	UI08 a3		= tSock->GetByte( 3 );
	UI08 a4		= tSock->GetByte( 4 );
	CItem *i	= CalcItemObjFromSer( objectId );
	if( !ValidateObject( i ))		// invalid item
		return true;

	// October 6, 2002 - Added support for the onClick event
	std::vector<UI16> scriptTriggers = i->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// If script returns true/0, don't show hard-coded name, and don't run additional onClick events
			if( toExecute->OnClick( tSock, i ) == 1 )
			{
				return true;
			}
		}
	}

	if( mChar->GetSingClickSer() )
	{
		tSock->ObjMessage( 1737, i, 0.0f, 0x03B2, a1, a2, a3, a4 );
	}

	UI16 getAmount = i->GetAmount();
	if( i->GetCont() != nullptr && i->GetContSerial() >= BASEITEMSERIAL )
	{
		CChar *w = FindItemOwner( static_cast<CItem *>( i->GetCont() ));
		if( ValidateObject( w ))
		{
			if( w->GetNpcAiType() == AI_PLAYERVENDOR )
			{
				std::string temp;
				temp = "Desc: " + i->GetDesc() + std::string( " at " ) + std::to_string( i->GetBuyValue() ) + std::string( "gp" );
				tSock->ObjMessage( temp, i );
			}
		}
	}

#if defined( _MSC_VER )
#pragma todo( "We need to update this to use GetTileName almost exclusively, for plurality" )
#endif
	if( i->GetNameRequest( tSock->CurrcharObj(), NRS_SINGLECLICK )[0] != '#' )
	{
		if( i->GetId() == 0x0ED5 ) //guildstone
		{
			realname = oldstrutil::format( Dictionary->GetEntry( 101, tSock->Language() ).c_str(), i->GetNameRequest( tSock->CurrcharObj(), NRS_SINGLECLICK ).c_str() );
		}
		if( !i->IsPileable() || getAmount == 1 )
		{
			if( mChar->IsGM() && !i->IsCorpse() && getAmount > 1 )
			{
				realname = oldstrutil::format( "%s (%u)", i->GetNameRequest( tSock->CurrcharObj(), NRS_SINGLECLICK ).c_str(), getAmount );
			}
			else
			{
				realname = i->GetNameRequest( tSock->CurrcharObj(), NRS_SINGLECLICK );
			}
		}
		else
		{
			realname = oldstrutil::format( "%u %ss", getAmount, i->GetNameRequest( tSock->CurrcharObj(), NRS_SINGLECLICK ).c_str() );
		}
	}
	else
	{
		GetTileName(( *i ), realname );
		if( i->GetAmount() > 1 )
			realname = std::to_string( getAmount ) + std::string(" ") + realname;
	}

	if( i->GetType() == IT_MAGICWAND )
	{
		if( i->GetName2() == "#" || i->GetName2() == "" )
		{
			realname += oldstrutil::format( Dictionary->GetEntry( 9404, tSock->Language() ), i->GetTempVar( CITV_MOREZ )); // with %d charges
		}
		else
		{
			realname += " (unidentified)";
		}
	}
	else if( i->IsContType() )
	{
		SI32 iCount = 0;
		if( i->IsCorpse() )
		{
			iCount = static_cast<SI32>( GetTotalItemCount( i ));
		}
		else
		{
			iCount = static_cast<SI32>( i->GetContainsList()->Num() );
		}
		realname += oldstrutil::format( ", (%u items, %u stones)", iCount, ( i->GetWeight() / 100 ));
	}

	// Use item's real name as starting point for final string
	temp2 = realname;

	if( i->GetCreator() != INVALIDSERIAL )
	{
		CChar *mCreator = CalcCharObjFromSer( i->GetCreator() );
		if( ValidateObject( mCreator ))
		{
			if( cwmWorldState->ServerData()->RankSystemStatus() && i->GetRank() == 10 )
			{
				std::string creatorName = GetNpcDictName( mCreator, tSock, NRS_SINGLECLICK );
				temp2 += oldstrutil::format( " %s", Dictionary->GetEntry( 9140, tSock->Language() ).c_str() ); // of exceptional quality

				if( cwmWorldState->ServerData()->DisplayMakersMark() && i->IsMarkedByMaker() )
				{
					temp2 += oldstrutil::format( " %s by %s", cwmWorldState->skill[i->GetMadeWith()-1].madeWord.c_str(), creatorName.c_str() ); //  [crafted] by %s
				}
			}
		}
	}

	tSock->ObjMessage( temp2, i );

	std::string itemTags = "";
	tSock->ObjMessage( AppendData( tSock, i, itemTags ), i );

	return true;
}
