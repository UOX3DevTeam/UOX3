#include "uox3.h"
#include "cdice.h"
#include "targeting.h"
#include "skills.h"
#include "cGuild.h"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "commands.h"
#include "cMagic.h"
#include "ssection.h"
#include "gump.h"
#include "trigger.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "combat.h"
#include "magic.h"
#include "jail.h"
#include "Dictionary.h"

#include "ObjectFactory.h"

#undef DBGFILE
#define DBGFILE "targeting.cpp"

namespace UOX
{

cTargets *Targ = NULL;

void tweakItemMenu( cSocket *s, CItem *j );
void tweakCharMenu( cSocket *s, CChar *c );
void OpenPlank( CItem *p );
void MakeShop( CChar *c );

void cTargets::PlVBuy( cSocket *s )//PlayerVendors
{
	VALIDATESOCKET( s );

	CChar *vChar = static_cast<CChar *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( vChar ) || vChar->isFree() ) 
		return;

	CChar *mChar	= s->CurrcharObj();
	UI32 gleft		= GetItemAmount( mChar, 0x0EED );

	CItem *p		= mChar->GetPackItem();
	if( !ValidateObject( p ) ) 
	{
		s->sysmessage( 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) || i->GetCont() == NULL ) 
		return;
	
	if( FindItemOwner( i ) != vChar )
		return;
	if( vChar->GetNPCAiType() != aiPLAYERVENDOR )
		return;
	if( mChar == vChar->GetOwnerObj() )
	{
		vChar->talk( s, 999, false );
		return;
	}

	if( gleft < i->GetBuyValue() ) 
	{
		vChar->talk( s, 1000, false );
		return;
	} 
	else 
	{
		UI32 tAmount = DeleteItemAmount( mChar, i->GetBuyValue(), 0x0EED );
		// tAmount > 0 indicates there wasn't enough money...
		// could be expanded to take money from bank too...
	}
	
	vChar->talk( s, 1001, false );
	vChar->SetHoldG( vChar->GetHoldG() + i->GetBuyValue() );

	i->SetCont( p );	// move containers
	s->statwindow( mChar );
}

void cTargets::HandleGuildTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *trgChar	= NULL;
	CChar *mChar	= s->CurrcharObj();
	CGuild *mGuild	= NULL, *tGuild = NULL;
	switch( s->GetByte( 5 ) )
	{
		case 0:	// recruit character
			trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( trgChar ) )
			{
				if( trgChar->GetGuildNumber() == -1 )	// no existing guild
				{
					mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
					if( mGuild != NULL )
						mGuild->NewRecruit( (*trgChar) );
				}
				else
					s->sysmessage( 1002 );
			}
			break;
		case 1:		// declare fealty
			trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( trgChar ) )
			{
				if( trgChar->GetGuildNumber() == mChar->GetGuildNumber() )	// same guild
					mChar->SetGuildFealty( trgChar->GetSerial() );
				else
					s->sysmessage( 1003 );
			}
			break;
		case 2:	// declare war
			trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( trgChar ) )
			{
				if( trgChar->GetGuildNumber() != mChar->GetGuildNumber() )
				{
					if( trgChar->GetGuildNumber() == -1 )
						s->sysmessage( 1004 );
					else
					{
						mGuild = GuildSys->Guild(mChar->GetGuildNumber() );
						if( mGuild != NULL )
						{
							mGuild->SetGuildRelation( trgChar->GetGuildNumber(), GR_WAR );
							tGuild = GuildSys->Guild( trgChar->GetGuildNumber() );
							if( tGuild != NULL )
								tGuild->TellMembers( 1005, mGuild->Name().c_str() );
						}
					}
				}
				else
					s->sysmessage( 1006 );
			}
			break;
		case 3:	// declare ally
			trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( trgChar ) )
			{
				if( trgChar->GetGuildNumber() != mChar->GetGuildNumber() )
				{
					if( trgChar->GetGuildNumber() == -1 )
						s->sysmessage( 1004 );
					else
					{
						mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
						if( mGuild != NULL )
						{
							mGuild->SetGuildRelation( trgChar->GetGuildNumber(), GR_ALLY );
							tGuild = GuildSys->Guild( trgChar->GetGuildNumber() );
							if( tGuild != NULL )
								tGuild->TellMembers( 1007, mGuild->Name().c_str() );
						}
					}
				}
				else
					s->sysmessage( 1006 );
			}
			break;
	}

}

void cTargets::HandleSetScpTrig( cSocket *s )
{
	VALIDATESOCKET( s );
	UI16 targTrig		= (UI16)s->TempInt();
	cScript *toExecute	= Trigger->GetScript( targTrig );
	if( toExecute == NULL )
	{
		s->sysmessage( 1752 );
		return;
	}

	SERIAL ser = s->GetDWord( 7 );
	if( ser < BASEITEMSERIAL )
	{	// character
		CChar *targChar = calcCharObjFromSer( ser );
		if( ValidateObject( targChar ) )
		{
			targChar->SetScriptTrigger( targTrig );
			s->sysmessage( 1653 );
		}
	}
	else
	{	// item
		CItem *targetItem = calcItemObjFromSer( ser );
		if( ValidateObject( targetItem ) )
		{
			s->sysmessage( 1652 );
			targetItem->SetScriptTrigger( targTrig );
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cTargets::MultiTarget( cSocket *s )
//|	Date			-	Unknown
//|	Developers		-	Unknown
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Runs various commands based upon the target ID we sent to the socket
//o--------------------------------------------------------------------------o
//| Modifications	-	Overhauled to use an enum allowing simple modification (Zane)
//o--------------------------------------------------------------------------o
void cTargets::MultiTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
	{
		if( mChar->GetSpellCast() != -1 )	// need to stop casting if we don't target right
			mChar->StopSpell();
		return; // do nothing if user cancels, avoids CRASH! - Morrolan
	}
	
	UI08 a1 = s->GetByte( 2 );
	UI08 a2 = s->GetByte( 3 );
	UI08 a3 = s->GetByte( 4 );
	TargetIDs targetID = (TargetIDs)s->GetByte( 5 );
	s->TargetOK( false );
	if( mChar->IsDead() && !mChar->IsGM() && mChar->GetAccount().wAccountIndex != 0 )
	{
		s->sysmessage( 1008 );
		if( mChar->GetSpellCast() != -1 )	// need to stop casting if we don't target right
			mChar->StopSpell();
		return;
	}
	if( a1 == 0 && a2 == 1 )
	{
		if( a3 == 2 )	// Guilds
		{
			HandleGuildTarget( s );
			return;
		}
		else if( a3 == 1 )	// CustomTarget
		{
			cScript *tScript	= (cScript *)s->TempInt();
			if( tScript != NULL )
				tScript->DoCallback( s, s->GetDWord( 7 ), static_cast<UI08>(targetID) );
			return;
		}
 		else if( a3 == 0 )
		{
			switch( targetID )
			{
				case TARGET_ADDITEM:				AddItem( s );								break;
				case TARGET_ADDSCRIPTITEM:		AddScriptItem( s );						break;
				case TARGET_ADDNPC:				AddNpc( s );								break;
				case TARGET_ADDSCRIPTNPC:		AddScriptNpc( s );						break;
				case TARGET_ADDSPAWNER:			AddItem( s, OT_SPAWNER );				break;
				case TARGET_ADDSCRIPTSPAWNER:	AddScriptItem( s, OT_SPAWNER );		break;
				case TARGET_BUILDHOUSE:			BuildHouseTarget( s );						break;
				case TARGET_TELE:			TeleTarget( s );					break;	
				case TARGET_REMOVE:			RemoveTarget( s );					break;
				case TARGET_DYE:			DyeTarget( s );						break;
				case TARGET_KEY:			KeyTarget( s );						break;
				case TARGET_ISTATS:			IstatsTarget( s );					break;
				case TARGET_CSTATS:			CstatsTarget( s );					break;
				case TARGET_KILL:			KillTarget( s );					break;
				case TARGET_RESURRECT:		ResurrectionTarget( s );			break;
				case TARGET_BOLT:			BoltTarget( s );					break;
				case TARGET_KICK:			CloseTarget( s );					break;
				case TARGET_DYEALL:			ColorsTarget( s );					break;
				case TARGET_DVAT:			DvatTarget( s );					break;
				case TARGET_ALLSET:			AllSetTarget( s );					break;
				case TARGET_INFO:			InfoTarget( s );					break;
				case TARGET_SHOWDETAIL:		ShowDetail( s );					break;
				case TARGET_NPCACT:			DoActionTarget( s );				break;
				case TARGET_WSTATS:			WstatsTarget( s );					break;
				case TARGET_NPCFOLLOW:		NpcTarget( s );						break;
				case TARGET_NPCFOLLOW2:		NpcTarget2( s );					break;
				case TARGET_NPCRESURRECT:	NpcResurrectTarget( mChar );		break;
				case TARGET_TWEAK:			TweakTarget( s );					break;
				case TARGET_MAKESTATUS:		MakeStatusTarget( s );				break;
				case TARGET_SETSCPTRIG:		HandleSetScpTrig( s );				break;
				case TARGET_DELETECHAR:		DeleteCharTarget( s );				break;
				case TARGET_AXE:			AxeTarget( s );						break;
				case TARGET_SWORD:			SwordTarget( s );					break;
				case TARGET_AREACOMMAND:	AreaCommand( s );					break;
				case TARGET_DUPE:			DupeTarget( s );					break;
				case TARGET_MOVETOBAG:		MoveToBagTarget( s );				break;
				case TARGET_MANA:			ManaTarget( s );					break;
				case TARGET_STAMINA:		StaminaTarget( s );					break;
				case TARGET_GMOPEN:			GmOpenTarget( s );					break;
				case TARGET_JAIL:			JailTarget( s, s->GetDWord( 7 ) );		break;
				case TARGET_RELEASE:		ReleaseTarget( s, s->GetDWord( 7 ) );	break;
				case TARGET_PERMHIDE:		permHideTarget( s );				break;
				case TARGET_UNHIDE:			unHideTarget( s );					break;
				case TARGET_XTELEPORT:		XTeleport( s, 0 );					break;
				case TARGET_FULLSTATS:		FullStatsTarget( s );				break;
				case TARGET_LOADCANNON:		LoadCannon( s );					break;
				case TARGET_SETPOISONED:	SetPoisonedTarget( s );				break;
				case TARGET_VIAL:			VialTarget( s );					break;
				case TARGET_TILING:			Tiling( s );						break;
				case TARGET_WIPING:			Wiping( s );						break;
				case TARGET_POSSESS:		PossessTarget( s );					break;
				case TARGET_TELESTUFF:		TeleStuff( s );						break;        
				case TARGET_TELESTUFF2:		TeleStuff2( s );					break;
				case TARGET_SQUELCH:		SquelchTarg( s );					break;
				case TARGET_BAN:			BanTarg( s );						break;
				case TARGET_SHOWSKILLS:		ShowSkillTarget( s );				break;
				case TARGET_GLOW:			GlowTarget( s );					break;
				case TARGET_UNGLOW:			UnglowTarget( s );					break;
				// Vendors
				case TARGET_MAKESHOP:		MakeShopTarget( s );				break;
				case TARGET_REMOVESHOP:		RemoveShopTarget( s );				break;
				case TARGET_BUYSHOP:		BuyShopTarget( s );					break;
				case TARGET_SELL:			SellStuffTarget( s );				break;
				case TARGET_PLVBUY:			PlVBuy( s );						break;
				// Town Stuff
				case TARGET_TOWNALLY:		MakeTownAlly( s );					break;
				case TARGET_VOTEFORMAYOR:	regions[mChar->GetTown()]->VoteForMayor( s ); break;
				// House Functions
				case TARGET_HOUSEOWNER:		HouseOwnerTarget( s );				break;
				case TARGET_HOUSEEJECT:		HouseEjectTarget( s );				break;
				case TARGET_HOUSEBAN:		HouseBanTarget( s );				break;
				case TARGET_HOUSEFRIEND:	HouseFriendTarget( s );				break;
				case TARGET_HOUSEUNLIST:	HouseUnlistTarget( s );				break;
				case TARGET_HOUSELOCKDOWN:	HouseLockdown( s );					break;
				case TARGET_HOUSERELEASE:	HouseRelease( s );					break;
				// Pets
				case TARGET_FOLLOW:			FollowTarget( s );					break;
				case TARGET_ATTACK:			AttackTarget( s );					break;
				case TARGET_TRANSFER:		TransferTarget( s );				break;
				case TARGET_GUARD:			GuardTarget( s );					break;
				case TARGET_FRIEND:			FriendTarget( s );					break;
				// Magic
				case TARGET_CASTSPELL:		Magic->CastSpell( s, mChar );		break;
				case TARGET_RECALL:			Magic->Recall( s );					break;					
				case TARGET_MARK:			Magic->Mark( s );					break;
				case TARGET_GATE:			Magic->Gate( s );					break;					
				case TARGET_HEAL:			Magic->Heal( s );					break;
				// Skills Functions
				case TARGET_ARMSLORE:		Skills->ArmsLoreTarget( s );		break;
				case TARGET_ANATOMY:		Skills->AnatomyTarget( s );			break;
				case TARGET_ITEMID:			Skills->ItemIDTarget( s );			break;
				case TARGET_EVALINT:		Skills->EvaluateIntTarget( s );		break;
				case TARGET_TAME:			Skills->TameTarget( s );			break;
				case TARGET_FISH:			Skills->FishTarget( s );			break;
				case TARGET_SMITH:			Skills->Smith( s );					break;
				case TARGET_MINE:			Skills->Mine( s );					break;
				case TARGET_SMELTORE:		Skills->SmeltOre( s );				break;
				case TARGET_REPAIRLEATHER:	Skills->RepairLeather( s );			break;
				case TARGET_REPAIRBOW:		Skills->RepairBow( s );				break;
				case TARGET_REPAIRMETAL:	Skills->RepairMetal( s );			break;
				case TARGET_SMELT:			SmeltTarget( s );					break;
				case TARGET_DETECTHIDDEN:	Skills->DetectHidden( s );			break;
				case TARGET_PROVOCATION:	Skills->ProvocationTarget1( s );	break;
				case TARGET_PROVOCATION2:	Skills->ProvocationTarget2( s );	break;
				case TARGET_ENTICEMENT:		Skills->EnticementTarget1( s );		break;
				case TARGET_ENTICEMENT2:	Skills->EnticementTarget2( s );		break;
				case TARGET_ALCHEMY:		Skills->AlchemyTarget( s );			break;
				case TARGET_CREATEBANDAGE:	Skills->CreateBandageTarget( s );	break;
				case TARGET_HEALING:		Skills->HealingSkillTarget( s );	break;
				case TARGET_CARPENTRY:		Skills->Carpentry( s );				break;
				case TARGET_BEGGING:		Skills->BeggingTarget( s );			break;
				case TARGET_ANIMALLORE:		Skills->AnimalLoreTarget( s );		break;
				case TARGET_FORENSICS:		Skills->ForensicsTarget( s );		break;
				case TARGET_APPLYPOISON: 
											s->TempObj( calcItemObjFromSer( s->GetDWord( 7 ) ) );
											s->target( 0, TARGET_POISONITEM, 1613 );
											break;
				case TARGET_POISONITEM:		Skills->PoisoningTarget( s );		break;
				case TARGET_INSCRIBE:		Skills->Inscribe( s );				break;
				case TARGET_LOCKPICK:		Skills->LockPick( s );				break;
				case TARGET_WHEEL:			Skills->Wheel( s );					break;
				case TARGET_LOOM:			Skills->Loom( s );					break;
				case TARGET_TAILORING:		Skills->Tailoring( s );				break;
				case TARGET_COOKING:		Skills->handleCooking( s );			break;
				case TARGET_FLETCHING:		Skills->Fletching( s );				break;
				case TARGET_TINKERING:		Skills->Tinkering( s );				break; 
				case TARGET_TINKERAXEL:		Skills->TinkerAxel( s );			break;
				case TARGET_TINKERAWG:		Skills->TinkerAwg( s );				break;
				case TARGET_TINKERCLOCK:	Skills->TinkerClock( s );			break;
				case TARGET_STEALING:		Skills->StealingTarget( s );		break;
				default:					
					if( s->GetDWord( 7 ) >= BASEITEMSERIAL )
						MultiSetItemTarget( s, targetID );
					else
						MultiSetCharTarget( s, targetID );
					break;
			}
		}
	}
}

void cTargets::MultiSetCharTarget( cSocket *s, TargetIDs targetID )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		switch( targetID )
		{
		case TARGET_INCX:			i->SetLocation( static_cast<SI16>(i->GetX() + s->AddX( 0 )), i->GetY(), i->GetZ() );	break;
		case TARGET_INCY:			i->SetLocation( i->GetX(), static_cast<SI16>(i->GetY() + s->AddX( 0 )), i->GetZ() );	break;
		case TARGET_INCZ:			i->SetZ( static_cast<SI08>(i->GetZ() + s->AddX( 0 )) );	break;
		case TARGET_XBANK:			s->openBank( i );											break;
		case TARGET_XGO:			i->SetLocation( s->AddX( 0 ), s->AddY( 0 ), s->AddZ(), s->AddID4() );	break;
		case TARGET_NPCRECT:		if( i->IsNpc() )
									{
										i->SetFx( s->AddX( 0 ), 0 );
										i->SetFy( s->AddY( 0 ), 0 );
										i->SetFz( -1 );
										i->SetFx( s->AddX( 1 ), 1 );
										i->SetFy( s->AddY( 1 ), 1 );
										i->SetNpcWander( 3 );
									}															break;
		case TARGET_NPCCIRCLE:		if( i->IsNpc() )
									{
										i->SetFx( s->AddX( 0 ), 0 );
										i->SetFy( s->AddY( 0 ), 0 );
										i->SetFz( -1 );
										i->SetFx( s->AddX( 1 ), 1 );
										i->SetNpcWander( 4 );
									}															break;
		case TARGET_OWNER2:			i->SetOwner( static_cast<CChar *>(s->TempObj()) );			
									s->TempObj( NULL );											break;
		default:					Console.Error( 2, " MultiTarget() was passed a bad targetID (%u)", static_cast<UI08>(targetID) );	break;
		}
	}
}
void cTargets::MultiSetItemTarget( cSocket *s, TargetIDs targetID )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		switch( targetID )
		{
		case TARGET_INCX:			i->IncLocation( s->AddX( 0 ), 0 );	break;
		case TARGET_INCY:			i->IncLocation( 0, s->AddX( 0 ) );	break;
		case TARGET_INCZ:			i->IncZ( s->AddX( 0 ) );			break;
		case TARGET_OWNER2:			i->SetOwner( static_cast<CChar *>(s->TempObj()) );			
									s->TempObj( NULL );										break;
		default:					Console.Error( 2, " MultiTarget() was passed a bad targetID (%u)", static_cast<UI08>(targetID) );	break;
		}
	}
	else
		s->sysmessage( 1076 );
}

#if defined( _MSC_VER )
#pragma note( "Function Warning: cTargets::BanTarg() does nothing" )
#endif
void cTargets::BanTarg( cSocket *s )
{

}

void DoAddTarget( cSocket *mSock, CItem *mItem )
{
	const SI16 tX = mSock->GetWord( 11 );
	const SI16 tY = mSock->GetWord( 13 );
	const SI08 tZ = static_cast<SI08>(mSock->GetByte( 16 ) + Map->TileHeight( mSock->GetWord( 17 ) ));
	
	if( mItem->GetCont() == NULL )
		mItem->SetLocation( tX, tY, tZ );
	else
	{
		mItem->SetX( tX );
		mItem->SetY( tY );
		mItem->SetZ( tZ );
	}
}
void BuildHouse( cSocket *s, UI08 houseEntry );
void cTargets::BuildHouseTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	BuildHouse( s, s->AddID1() );//If its a valid house, send it to buildhouse!

	s->AddID1( 0 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddTarget( cSocket *s)
//|   Date        -  UnKnown
//|   Programmer  -  UnKnown  (Touched tabstops by Tauriel Dec 29, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds an item when using /add # # .
//o---------------------------------------------------------------------------o
void cTargets::AddItem( cSocket *s, ObjectType itemType )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	UI16 modelNum	= static_cast<UI16>(( ( s->AddID1() )<<8 ) + s->AddID2());
	CItem *c		= Items->CreateItem( NULL, s->CurrcharObj(), modelNum, 1, 0, itemType );
	if( c == NULL ) 
		return;
	
	DoAddTarget( s, c );

	s->AddID1( 0 );
	s->AddID2( 0 );
}

void cTargets::AddScriptItem( cSocket *s, ObjectType itemType ) //Tauriel 11-22-98 updated for new items
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL ) 
		return;

	CItem *c		= Items->CreateScriptItem( s, s->CurrcharObj(), s->XText(), 1, itemType );
	if( c == NULL )
		return;

	DoAddTarget( s, c );
}

void cTargets::AddNpc( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	CChar *npc = static_cast< CChar * >(ObjectFactory::getSingleton().CreateObject( OT_CHAR ));
	if( npc == NULL )
		return;

	npc->SetName( "Dummy" );
	npc->SetID( ( ( s->AddID1() <<8 ) + s->AddID2()) );
	npc->SetOrgID( npc->GetID() );
	npc->SetSkin( 0 );
	npc->SetSkillTitles( true );
	npc->SetLocation( s->GetWord( 11 ), s->GetWord( 13 ), s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ) );
	npc->SetNpc( true );
}

void cTargets::AddScriptNpc( cSocket *s )
// Abaddon 17th February, 2000
// Need to return the character we've made, else summon creature at least will fail
// We make the char, but never pass it back up the chain
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	CChar *mChar	= s->CurrcharObj();
	SI16 coreX		= s->GetWord( 11 );
	SI16 coreY		= s->GetWord( 13 );
	SI08 coreZ		= static_cast<SI08>(s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ));
	CChar *cCreated	= Npcs->CreateNPCxyz( s->XText(), coreX, coreY, coreZ, mChar->WorldNumber() );
}

void cTargets::TeleTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	SERIAL serial = s->GetDWord( 7 );

	CItem *i = calcItemObjFromSer( serial );
	if( ValidateObject( i ) )
	{
		s->sysmessage( 717 );
		return;
	}

	SI16 targX, targY;
	SI08 targZ;
	CChar *c = calcCharObjFromSer( serial );
	if( ValidateObject( c ) )
	{
		targX = c->GetX();
		targY = c->GetY();
		targZ = c->GetZ();
	}
	else
	{
		targX = s->GetWord( 11 );
		targY = s->GetWord( 13 );
		targZ = (SI08)(s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ));
	}
	CChar *mChar = s->CurrcharObj();

	if( mChar->IsGM() || LineOfSight( s, mChar, targX, targY, targZ, WALLS_CHIMNEYS + DOORS + ROOFING_SLANTED ) )	
	{
		if( s->CurrentSpellType() != 2 )  // not a wand cast
		{
			Magic->SubtractMana( mChar, 3 );  // subtract mana on scroll or spell
			if( s->CurrentSpellType() == 0 )             // del regs on normal spell
			{
				reag_st toDel;
				toDel.drake = 1;
				toDel.moss = 1;
				Magic->DelReagents( mChar, toDel );
			}
		}
		
		Effects->PlaySound( s, 0x01FE, true );
		
		mChar->SetLocation( targX, targY, targZ );
		Effects->PlayStaticAnimation( mChar, 0x372A, 0x09, 0x06 );
	} 
}

void cTargets::RemoveTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL serial	= s->GetDWord( 7 );
	CItem *i		= calcItemObjFromSer( serial );
	if( ValidateObject( i ) )
	{
		s->sysmessage( 1013 );
		i->Delete();
    } 
	else 
	{
		CChar *c = calcCharObjFromSer( serial );
		if( !ValidateObject( c ) )
			return;
		if( c->GetAccount().wAccountIndex != AB_INVALID_ID && !c->IsNpc() )
		{ 
			s->sysmessage( 1014 );
			return;
		}
		s->sysmessage( 1015 );
		c->Delete();
	}
}

void cTargets::DyeTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	int b;
	UI16 colour, k;
	CItem *i		= NULL;
	CChar *c		= NULL;
	SERIAL serial	= s->GetDWord( 7 );
	if( s->AddID1() == 0xFF && s->AddID2() == 0xFF )
	{
		CPDyeVat toSend;
		if( serial >= BASEITEMSERIAL )
		{
			i = calcItemObjFromSer( serial );
			if( ValidateObject( i ) )
			{
				toSend = (*i);
				s->Send( &toSend );
			}
		}
		else
		{
			c = calcCharObjFromSer( serial );
			if( ValidateObject( c ) )
			{
				toSend = (*c);
				toSend.Model( 0x2106 );
				s->Send( &toSend );
			}
		}
	}
	else
	{
		if( serial >= BASEITEMSERIAL )
		{
			i = calcItemObjFromSer( serial );
			if( !ValidateObject( i ) )
				return;
			colour = (UI16)(( (s->AddID1())<<8 ) + s->AddID2());
			if( !s->DyeAll() )
			{
				if( colour < 0x0002 || colour > 0x03E9 )
					colour = 0x03E9;
			}
			
			b = ((colour&0x4000)>>14) + ((colour&0x8000)>>15);   
			if( !b )
				i->SetColour( colour );
		}
		else
		{
			c = calcCharObjFromSer( serial );
			if( !ValidateObject( c ) )
				return;
			UI16 body = c->GetID();
			k = (UI16)(( ( s->AddID1() )<<8 ) + s->AddID2());
			
			b = k&0x4000; 
			if( b == 16384 && ( body >= 0x0190 && body <= 0x03E1 ) ) 
				k = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.
			
			if( k != 0x8000 ) // 0x8000 also crashes client ...
			{	
				c->SetSkin( k );
				c->SetOrgSkin( k );
			}
		}
	}
}

void cTargets::XTeleport( cSocket *s, UI08 x )
{
	VALIDATESOCKET( s );
	SERIAL serial	= INVALIDSERIAL;
	CChar *c		= NULL;
	// Abaddon 17th February 2000 Converted if to switch (easier to read)
	// Also made it so that it calls teleport, not updatechar, else you don't get the items in range
	CChar *mChar = s->CurrcharObj();
	switch( x )
	{
		case 0:
			serial	= s->GetDWord( 7 );
			c		= calcCharObjFromSer( serial );
			break;
		case 5:
			serial	= calcserial( (UI08)Commands->Argument( 1 ), (UI08)Commands->Argument( 2 ), (UI08)Commands->Argument( 3 ), (UI08)Commands->Argument( 4 ) );
			c		= calcCharObjFromSer( serial );
			break;
		case 2:
			cSocket *mSock;
			mSock = Network->GetSockPtr( Commands->Argument( 1 ) );
			if( mSock != NULL )
				c = mSock->CurrcharObj();
			else 
				return;
			break;
		default:
			s->sysmessage( 1654, x );
			return;
	}
	
	if( ValidateObject( c ) )
	{
		c->SetLocation( mChar );
		return;
	}

	CItem *i = calcItemObjFromSer( serial );
	if( ValidateObject( i ) )
		i->SetLocation( mChar );
}

void cTargets::KeyTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;

	CChar *mChar		= s->CurrcharObj();
	SERIAL moreSerial	= s->AddID();
	if( i->GetTempVar( CITV_MORE ) == 0 )
	{       
		if( i->GetType() == IT_KEY && objInRange( mChar, i, DIST_NEARBY ) )
		{
			if( !Skills->CheckSkill( mChar, TINKERING, 400, 1000 ) ) 
			{
				s->sysmessage( 1016 );
				i->Delete();
				return;
			}
			i->SetTempVar( CITV_MORE, moreSerial );
			s->sysmessage( 1017 );
		}
	}
	else if( i->GetTempVar( CITV_MORE ) == moreSerial || s->AddID1() == 0xFF )
	{
		if( objInRange( mChar, i, DIST_NEARBY ) )
		{
			switch( i->GetType() )
		{
			case IT_CONTAINER:
			case IT_SPAWNCONT:
			if( i->GetType() == IT_CONTAINER ) 
				i->SetType( IT_LOCKEDCONTAINER );
			else if( i->GetType() == IT_SPAWNCONT ) 
				i->SetType( IT_LOCKEDSPAWNCONT );
			s->sysmessage( 1018 );
				break;
			case IT_KEY:
			mChar->SetSpeechItem( i );
			mChar->SetSpeechMode( 5 );
			s->sysmessage( 1019 );
				break;
			case IT_LOCKEDCONTAINER:
			case IT_LOCKEDSPAWNCONT:
			if( i->GetType() == IT_LOCKEDCONTAINER ) 
				i->SetType( IT_CONTAINER );
			if( i->GetType() == IT_LOCKEDSPAWNCONT ) 
				i->SetType( IT_SPAWNCONT );
			s->sysmessage( 1020 );
				break;
			case IT_DOOR:
			i->SetType( IT_LOCKEDDOOR );
			s->sysmessage( 1021 );
			Effects->PlaySound( s, 0x0049, true );
				break;
			case IT_LOCKEDDOOR:
			i->SetType( IT_DOOR );
			s->sysmessage( 1022 );
			Effects->PlaySound( s, 0x0049, true );
				break;
			case IT_HOUSESIGN:
			s->sysmessage( 1023 );
			mChar->SetSpeechMode( 8 );
			mChar->SetSpeechItem( i );
				break;
			case IT_PLANK:
			OpenPlank( i );
				s->sysmessage( "You open the plank" );
				break;
			}
		}
		else
			s->sysmessage( 393 );
	}
	else
	{
		if( i->GetType() == IT_KEY ) 
			s->sysmessage( 1024 );
		else if( i->GetTempVar( CITV_MORE, 1 ) == 0 ) 
			s->sysmessage( 1025 );
		else 
			s->sysmessage( 1026 );
	}
}


void cTargets::IstatsTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CTile tile;
	if( s->GetDWord( 7 ) == 0 )
	{
		UI08 worldNumber	= 0;
		CChar *mChar		= s->CurrcharObj();
		if( ValidateObject( mChar ) )
			worldNumber = mChar->WorldNumber();
		UI16 targetID = s->GetWord( 0x11 );
		SI16 targetX = s->GetWord( 0x0B );		// store our target x y and z locations
		SI16 targetY = s->GetWord( 0x0D );
		SI08 targetZ = s->GetByte( 0x10 );
		if( targetID != 0 )	// we might have a static rock or mountain
		{
			MapStaticIterator msi( targetX, targetY, worldNumber );
			staticrecord *stat = NULL;
			while( ( ( stat = msi.Next() ) != NULL ) )
			{
				msi.GetTile(&tile);
				if( targetZ == stat->zoff )
				{
					GumpDisplay staticStat( s, 300, 300 );
					staticStat.SetTitle( "Item [Static]" );
					staticStat.AddData( "ID", targetID, 5 );
					staticStat.AddData( "Height", tile.Height() );
					staticStat.AddData( "Name", tile.Name() );
					staticStat.Send( 4, false, INVALIDSERIAL );
				}
			}
		}
		else		// or it could be a map only
		{
			// manually calculating the ID's if a maptype
			map_st map1;
			CLand land;
			map1 = Map->SeekMap0( targetX, targetY, worldNumber );
			Map->SeekLand( map1.id, &land );
			GumpDisplay mapStat( s, 300, 300 );
			mapStat.SetTitle( "Item [Map]" );
			mapStat.AddData( "ID", targetID, 5 );
			mapStat.AddData( "Name", land.Name() );
			mapStat.Send( 4, false, INVALIDSERIAL );
		}
	}
	else
	{
		CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
		if( !ValidateObject( i ) )
			return;
		GumpDisplay dynamicStat( s, 300, 300 );
		dynamicStat.SetTitle( "Item [Dynamic]" );

		SERIAL itemSerial	= i->GetSerial();
		SERIAL contSerial	= i->GetContSerial();
		SERIAL moreVal		= i->GetTempVar( CITV_MORE );
		SERIAL ownerSerial	= i->GetOwner();
		UI16 itemID			= i->GetID();
		UI16 itemColour		= i->GetColour();
		int hpStuff			= ((i->GetHP())<<8) + i->GetMaxHP();
		int itemDamage		= ((i->GetLoDamage())<<8) + i->GetHiDamage();
		long int decayTime	= int(R64(int(i->GetDecayTime()-cwmWorldState->GetUICurrentTime())/1000));

		dynamicStat.AddData( "Serial", itemSerial, 3 );
		dynamicStat.AddData( "ID", itemID, 5 );
		dynamicStat.AddData( "Name", i->GetName() );
		dynamicStat.AddData( "Name2", i->GetName2() );
		dynamicStat.AddData( "Colour", itemColour, 5 );
		dynamicStat.AddData( "Cont Serial", contSerial, 3 );
		dynamicStat.AddData( "Layer", i->GetLayer() );
		dynamicStat.AddData( "Type", static_cast< UI08 >(i->GetType()) );
		dynamicStat.AddData( "Moveable", i->GetMovable() );
		dynamicStat.AddData( "More", moreVal, 3 );
		dynamicStat.AddData( "X coord", i->GetX() );
		dynamicStat.AddData( "Y coord", i->GetY() );
		dynamicStat.AddData( "Z coord", i->GetZ() );
		dynamicStat.AddData( "Amount", i->GetAmount() );
		dynamicStat.AddData( "Owner", ownerSerial, 3 );
		dynamicStat.AddData( "Privs", i->GetPriv() );
		dynamicStat.AddData( "Strength", i->GetStrength() );
		dynamicStat.AddData( "HP/Max", hpStuff, 6 );
		dynamicStat.AddData( "Damage", itemDamage, 6 );
		dynamicStat.AddData( "Defense", i->GetDef() );
		dynamicStat.AddData( "Rank", i->GetRank() );
		dynamicStat.AddData( "More X", i->GetTempVar( CITV_MOREX ) );
		dynamicStat.AddData( "More Y", i->GetTempVar( CITV_MOREY ) );
		dynamicStat.AddData( "More Z", i->GetTempVar( CITV_MOREZ ) );
		dynamicStat.AddData( "Poisoned", i->GetPoisoned() );
		dynamicStat.AddData( "Weight", i->GetWeight() );
		dynamicStat.AddData( "Creator", i->GetCreator() );
		dynamicStat.AddData( "Madewith", i->GetMadeWith() );
		dynamicStat.AddData( "DecayTime", decayTime );
		dynamicStat.AddData( "Decay", i->isDecayable()?1:0 );
		dynamicStat.AddData( "Good", i->GetGood() );
		dynamicStat.AddData( "RandomValueRate", i->GetRndValueRate() );
		dynamicStat.AddData( "Buy Value", i->GetBuyValue() );
		dynamicStat.AddData( "Sell Value", i->GetSellValue() );
		dynamicStat.AddData( "Carve", i->GetCarve() );
		if( i->GetObjType() == OT_SPAWNER )
		{
			CSpawnItem *spawnItem = static_cast<CSpawnItem *>(i);
			if( spawnItem->IsSectionAList() )
				dynamicStat.AddData( "Spawnobjlist", spawnItem->GetSpawnSection() );
			else
				dynamicStat.AddData( "Spawnobj", spawnItem->GetSpawnSection() );
			dynamicStat.AddData( "Min Spawn Time", spawnItem->GetInterval( 0 ) );
			dynamicStat.AddData( "Max Spawn Time", spawnItem->GetInterval( 1 ) );
		}
		dynamicStat.AddData( "Multi", i->GetMulti(), 3 );
		dynamicStat.Send( 4, false, INVALIDSERIAL );
	}
}

void cTargets::CstatsTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	GumpDisplay charStat( s, 300, 300 );
	charStat.SetTitle( "Character" );
	UI16 charID			= i->GetID();
	UI16 charSkin		= i->GetSkin();
	SERIAL charSerial	= i->GetSerial();

	ACCOUNTSBLOCK actbTemp=i->GetAccount();

	charStat.AddData( "Serial", charSerial, 3 );
	charStat.AddData( "Body Type", charID, 5 );
	charStat.AddData( "Name", i->GetName() );
	charStat.AddData( "Skin", charSkin, 5 );
	charStat.AddData( "Account", actbTemp.wAccountIndex );
	charStat.AddData( "Privs", i->GetPriv(), 5 );
	charStat.AddData( "Strength", i->GetStrength() );
	charStat.AddData( "Dexterity", i->GetDexterity() );
	charStat.AddData( "Intelligence", i->GetIntelligence() );
	charStat.AddData( "Mana", i->GetMana() );
	charStat.AddData( "Hitpoints", i->GetHP() );
	charStat.AddData( "Stamina", i->GetStamina() );
	charStat.AddData( "X coord", i->GetX() );
	charStat.AddData( "Y coord", i->GetY() );
	charStat.AddData( "Z coord", i->GetZ() );
	charStat.AddData( "Fame", i->GetFame() );
	charStat.AddData( "Karma", i->GetKarma() );
	charStat.AddData( "Deaths", i->GetDeaths() );
	charStat.AddData( "Kills", i->GetKills() );
	charStat.AddData( "AI Type", i->GetNPCAiType()  );
	charStat.AddData( "NPC Wander", i->GetNpcWander() );
	charStat.AddData( "Weight", i->GetWeight() );
	charStat.AddData( "Poisoned", i->GetPoisoned() );
	charStat.AddData( "Poison", i->GetPoisonStrength() );
	charStat.AddData( "Hunger", i->GetHunger() );
	charStat.AddData( "Attacker", ( ValidateObject( i->GetAttacker() ) ) ? i->GetAttacker()->GetSerial() : INVALIDSERIAL );
	charStat.AddData( "Target", ( ValidateObject( i->GetTarg() ) ) ? i->GetTarg()->GetSerial() : INVALIDSERIAL );
	charStat.AddData( "Carve", i->GetCarve() );
	charStat.AddData( "Race", i->GetRace() );
	charStat.AddData( "RaceGate", i->GetRaceGate() );
	charStat.AddData( "CommandLevel", i->GetCommandLevel() );
	// 
	if( actbTemp.wAccountIndex != AB_INVALID_ID )
		charStat.AddData( "Last On", i->GetLastOn() );
	//
	charStat.AddData( "Multi", i->GetMulti(), 3 );
	charStat.AddData( "Region", i->GetRegion()->GetRegionNum() );
	charStat.Send( 4, false, INVALIDSERIAL );
	
#if defined( _MSC_VER )
#pragma note( "This Gumps->Open() locks up later clients, commenting out for now... what is it FOR?" )
#endif
//	Gumps->Open( s, i, 8 );
	s->statwindow( i );
}

void cTargets::WstatsTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	GumpDisplay wStat( s, 300, 300 );
	wStat.SetTitle( "Walking Stats" );
	SERIAL charSerial = i->GetSerial();
	UI16 charID = i->GetID();
	wStat.AddData( "Serial", charSerial, 3 );
	wStat.AddData( "Body ID", charID, 5 );
	wStat.AddData( "Name", i->GetName() );
	wStat.AddData( "X", i->GetX() );
	wStat.AddData( "Y", i->GetY() );
	char temp[15];
	sprintf( temp, "%d/%d", i->GetZ(), i->GetDispZ() );
	wStat.AddData( "Z/DispZ", temp );
	wStat.AddData( "Wander", i->GetNpcWander() );
	wStat.AddData( "FX1", i->GetFx( 0 ) );
	wStat.AddData( "FY1", i->GetFy( 0 ) );
	wStat.AddData( "FZ1", i->GetFz() );
	wStat.AddData( "FX2", i->GetFx( 1 ) );
	wStat.AddData( "FY2", i->GetFy( 1 ) );
	wStat.Send( 4, false, INVALIDSERIAL );
}

void cTargets::KillTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *k = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( k ) )
	{
		UString xText = s->XText();
		std::string key = xText.section( " ", 0, 0 ).upper();
		UI08 layer		= 0;
		if( key == "HAIR" )
			layer = IL_HAIR;
		else if( key == "BEARD" )
			layer = IL_FACIALHAIR;
		else if( key == "PACK" )
			layer = IL_PACKITEM;
		else if( key == "CHAR" )
		{
			if( !k->IsDead() )
			{
				Effects->bolteffect( k );
				Effects->PlaySound( k, 0x0029 );
				doDeathStuff( k );
			}
			else
				s->sysmessage( 1028 );
		}
		else if( key == "LAYER" )
			layer = xText.section( " ", 1, 1 ).toUByte();

		if( layer > 0 )
		{
			CItem *i = k->GetItemAtLayer( layer );
			if( ValidateObject( i ) )
				i->Delete();
		}
	}
}

void cTargets::BoltTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		Effects->bolteffect( i );
		Effects->PlaySound( i, 0x0029 );
	}
}

void cTargets::CloseTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )	
		return;
	cSocket *j = calcSocketObjFromChar( i );
	if( j != NULL )
	{
		s->sysmessage( 1029 );
		j->sysmessage( 1030 );
		Network->Disconnect( j );
	}
}

void cTargets::ColorsTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	if( i->GetID() == 0x0FAB || i->GetID() == 0x0EFF || i->GetID() == 0x0E27 )	// dye vat, hair dye
	{
		CPDyeVat toSend = (*i);
		s->Send( &toSend );
	}
	else
		s->sysmessage( 1031 );
}

void cTargets::DvatTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL serial	= s->GetDWord( 7 );
	CItem *i		= calcItemObjFromSer( serial );
	if( !ValidateObject( i ) )
		return;

	CChar *mChar = s->CurrcharObj();
	if( i->isDyeable() )
	{
		if( i->GetCont() != NULL )
		{
			CChar *c = FindItemOwner( i );
			if( ValidateObject( c ) && c != mChar )
			{
				s->sysmessage( 1032 );
				return;
			}
		}
		i->SetColour( ( ( s->AddID1() )<<8) + s->AddID2() );
		Effects->PlaySound( s, 0x023E, true );
	}
	else
		s->sysmessage( 1033 );
}

enum AllSetTypes
{
	// Base Objects
	AST_NAME			= 0,
	AST_STR,
	AST_DEX,
	AST_INT,
	AST_FAME,
	AST_KARMA,
	AST_KILLS,
	AST_COLOR,
	AST_OWNER,
	AST_X,
	AST_Y,
	AST_Z,
	AST_ID,
	AST_VISIBLE,
	// Characters
	AST_ALLSKILLS,
	AST_ORGSKIN,
	AST_FONT,
	AST_SPATTACK,
	AST_SPDELAY,
	AST_POISON,
	AST_TITLE,
	AST_NPCWANDER,
	AST_DIR,
	AST_NPCAI,
	AST_ADVOBJ,
	AST_INVULNERABLE,
	AST_SPLIT,
	AST_SPLITCHANCE,
	AST_COMMANDLEVEL,
	AST_CANTRAIN,
	AST_FROZEN,
	// Items
	AST_AMOUNT,
	AST_MOVABLE,
	AST_WIPABLE,
	AST_BUYVALUE,
	AST_SELLVALUE,
	AST_RESTOCK,
	AST_MORE,
	AST_MOREX,
	AST_MOREY,
	AST_MOREZ,
	AST_MOREXYZ,
	AST_DEVINELOCK,
	AST_NAME2,
	AST_TYPE,
	AST_DECAYABLE,
	// Spawners
	AST_SPAWNSECTION,
	AST_MININTERVAL,
	AST_MAXINTERVAL,
	AST_COUNT			= 0xFF
};

std::map< std::string, AllSetTypes > keyToAllSetType;

void InitKeyToAllSetType( void )
{
	// Base Objects
	keyToAllSetType["NAME"]			= AST_NAME;
	keyToAllSetType["STR"]			= AST_STR;
	keyToAllSetType["STRENGTH"]		= AST_STR;
	keyToAllSetType["DEX"]			= AST_DEX;
	keyToAllSetType["DEXTERITY"]	= AST_DEX;
	keyToAllSetType["INT"]			= AST_INT;
	keyToAllSetType["INTELLIGENCE"]	= AST_INT;
	keyToAllSetType["FAME"]			= AST_FAME;
	keyToAllSetType["KARMA"]		= AST_KARMA;
	keyToAllSetType["KILLS"]		= AST_KILLS;
	keyToAllSetType["COLOR"]		= AST_COLOR;
	keyToAllSetType["COLOUR"]		= AST_COLOR;
	keyToAllSetType["HUE"]			= AST_COLOR;
	keyToAllSetType["OWNER"]		= AST_OWNER;
	keyToAllSetType["X"]			= AST_X;
	keyToAllSetType["Y"]			= AST_Y;
	keyToAllSetType["Z"]			= AST_Z;
	keyToAllSetType["DIR"]			= AST_DIR;
	keyToAllSetType["ID"]			= AST_ID;
	keyToAllSetType["VISIBLE"]		= AST_VISIBLE;
	// Characters
	keyToAllSetType["ALLSKILLS"]	= AST_ALLSKILLS;
	keyToAllSetType["ORGSKIN"]		= AST_ORGSKIN;
	keyToAllSetType["FONT"]			= AST_FONT;
	keyToAllSetType["SPATTACK"]		= AST_SPATTACK;
	keyToAllSetType["SPDELAY"]		= AST_SPDELAY;
	keyToAllSetType["POISON"]		= AST_POISON;
	keyToAllSetType["TITLE"]		= AST_TITLE;
	keyToAllSetType["NPCWANDER"]	= AST_NPCWANDER;
	keyToAllSetType["NPCAI"]		= AST_NPCAI;
	keyToAllSetType["ADVOBJ"]		= AST_ADVOBJ;
	keyToAllSetType["INVULNERABLE"]	= AST_INVULNERABLE;
	keyToAllSetType["SPLIT"]		= AST_SPLIT;
	keyToAllSetType["SPLITCHANCE"]	= AST_SPLITCHANCE;
	keyToAllSetType["COMMANDLEVEL"]	= AST_COMMANDLEVEL;
	keyToAllSetType["CANTRAIN"]		= AST_CANTRAIN;
	keyToAllSetType["FROZEN"]		= AST_FROZEN;
	// Items
	keyToAllSetType["AMOUNT"]		= AST_AMOUNT;
	keyToAllSetType["MOVABLE"]		= AST_MOVABLE;
	keyToAllSetType["WIPABLE"]		= AST_WIPABLE;
	keyToAllSetType["BUYVALUE"]		= AST_BUYVALUE;
	keyToAllSetType["SELLVALUE"]	= AST_SELLVALUE;
	keyToAllSetType["RESTOCK"]		= AST_RESTOCK;
	keyToAllSetType["MORE"]			= AST_MORE;
	keyToAllSetType["MOREX"]		= AST_MOREX;
	keyToAllSetType["MOREY"]		= AST_MOREY;
	keyToAllSetType["MOREZ"]		= AST_MOREZ;
	keyToAllSetType["MOREXYZ"]		= AST_MOREXYZ;
	keyToAllSetType["DEVINELOCK"]	= AST_DEVINELOCK;
	keyToAllSetType["NAME2"]		= AST_NAME2;
	keyToAllSetType["TYPE"]			= AST_TYPE;
	keyToAllSetType["DECAYABLE"]	= AST_DECAYABLE;
	// Spawners
	keyToAllSetType["SPAWNSECTION"]	= AST_SPAWNSECTION;
	keyToAllSetType["MININTERVAL"]	= AST_MININTERVAL;
	keyToAllSetType["MAXINTERVAL"]	= AST_MAXINTERVAL;
}

AllSetTypes FindAllSetTypeFromKey( UString strToFind  )
{
	if( keyToAllSetType.size() == 0 )	// if we haven't built our array yet
		InitKeyToAllSetType();
	std::map< std::string, AllSetTypes >::iterator toFind = keyToAllSetType.find( strToFind.upper() );
	if( toFind != keyToAllSetType.end() )
		return toFind->second;
	return AST_COUNT;
}

void HandleSetChar( cSocket *s, CChar *myChar, AllSetTypes key, UString xtext, UString value, UString keyString )
{
	cSocket *charSock	= calcSocketObjFromChar( myChar );
	switch( key )
	{
	// Allskills
	case AST_ALLSKILLS:
		{
			for( UI08 i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SetBaseSkill( value.toUShort(), i );
				Skills->updateSkillLevel( myChar, i );

				if( charSock != NULL ) 
					charSock->updateskill( i );
			}
			s->sysmessage( "Succesfully set '%s' on '%s'", xtext.c_str(), myChar->GetName().c_str() );				
		}
		break;
	// OrgSkin
	case AST_ID:
							UI16 targID;
							targID = value.toUShort();
							if( targID <= 0x7CF )
							{
								myChar->SetID( targID );
								myChar->SetOrgID( targID );
							}
							break;
	case AST_ORGSKIN:		myChar->SetOrgSkin( value.toUShort() );
							s->sysmessage( "Succesfully set '%s' on '%s'", xtext.c_str(), myChar->GetName().c_str() );
							break;
	case AST_FONT:			myChar->SetFontType( value.toByte() );				break;
	case AST_SPATTACK:		myChar->SetSpAttack( value.toShort() );				break;
	case AST_SPDELAY:		myChar->SetSpDelay( value.toByte() );				break;
	case AST_POISON:		myChar->SetPoisonStrength( value.toUByte() );		break;
	case AST_TITLE:			myChar->SetTitle( value );							break;
	case AST_NPCWANDER:		myChar->SetNpcWander( value.toByte() );				break;
	case AST_DIR:			myChar->SetDir( value.toUByte() & 0x0F );			break;
	case AST_NPCAI:			myChar->SetNPCAiType( value.toShort() );			break;
	case AST_ADVOBJ:		myChar->SetAdvObj( value.toUShort() );				break;
	case AST_INVULNERABLE:	myChar->SetInvulnerable( (value.toUByte() != 0) );	break;
	case AST_SPLIT:			myChar->SetSplit( value.toUByte() );				break;
	case AST_SPLITCHANCE:	myChar->SetSplitChance( value.toUByte() );			break;
	case AST_COMMANDLEVEL:	myChar->SetCommandLevel( value.toUByte() );			break;
	case AST_Z:				myChar->SetZ( value.toByte() );			
							myChar->SetDispZ( value.toByte() );					break;
	case AST_CANTRAIN:		
							if( myChar->IsNpc() )
								myChar->SetCanTrain( (value.toUByte() != 0) );	
							break;
	case AST_FROZEN:		myChar->SetFrozen( (value.toUByte() != 0) );		break;
	case AST_VISIBLE:		myChar->SetHidden( value.toByte() );				break;
	case AST_COUNT:			
	default:
							{
								// Look for matching skills
								for( UI08 i = 0; i < ALLSKILLS; ++i )
								{
									if( keyString == skillname[ i ] )
									{
										myChar->SetBaseSkill( value.toUShort(), i );
										Skills->updateSkillLevel( myChar, i );

										if( charSock != NULL ) 
											charSock->updateskill( i );

										s->sysmessage( "Succesfully set '%s' on '%s'", xtext.c_str(), myChar->GetName().c_str() );				
										return;
									}
								}
							}
							return;
	}
	s->sysmessage( "Succesfully set '%s' on '%s'", xtext.c_str(), myChar->GetName().c_str() );				
}

void HandleSetItem( cSocket *s, CItem *myItem, AllSetTypes key, UString xtext, UString value )
{
	switch( key )
	{
	case AST_ID:			myItem->SetID( value.toUShort() );								break;
	case AST_AMOUNT:		myItem->SetAmount( value.toUShort() );							break;
	case AST_MOVABLE:		myItem->SetMovable( value.toByte() );							break;
	case AST_WIPABLE:		myItem->SetWipeable( (value.toUByte() != 0) );					break;
	case AST_BUYVALUE:		myItem->SetBuyValue( value.toLong() );							break;
	case AST_SELLVALUE:		myItem->SetSellValue( value.toLong() );							break;
	case AST_RESTOCK:		myItem->SetRestock( value.toUShort() );							break;
	case AST_MORE:			myItem->SetTempVar( CITV_MORE, value.toULong() );								break;
	case AST_MOREX:			myItem->SetTempVar( CITV_MOREX, value.toULong() );							break;
	case AST_MOREY:			myItem->SetTempVar( CITV_MOREY, value.toULong() );							break;
	case AST_MOREZ:			myItem->SetTempVar( CITV_MOREZ, value.toULong() );							break;
	case AST_MOREXYZ:		myItem->SetTempVar( CITV_MOREX, value.toULong() );
							myItem->SetTempVar( CITV_MOREY, xtext.section( " ", 2, 2 ).toULong() );
							myItem->SetTempVar( CITV_MOREZ, xtext.section( " ", 3, 3 ).toULong() );		break;
	case AST_DEVINELOCK:	s->sysmessage( (1010 + value.toUByte()) );
							myItem->SetDevineLock( (value.toUByte() != 0) );				break;
	case AST_DIR:			myItem->SetDir( value.toUByte() );								break;
	case AST_NAME2:			myItem->SetName2( value.c_str() );								break;
	case AST_TYPE:			myItem->SetType( static_cast< ItemTypes >(value.toUByte()) );	break;
	case AST_Z:				myItem->SetZ( value.toByte() );									break;
	case AST_DECAYABLE:		myItem->SetDecayable( (value.toUByte() != 0) );					break;
	case AST_VISIBLE:		myItem->SetVisible( value.toByte() );							break;
	default:																				return;
	}
	s->sysmessage( "Succesfully set '%s' on '%s'", xtext.c_str(), myItem->GetName().c_str() );
}

void HandleSetSpawner( cSocket *s, CSpawnItem *mySpawnItem, AllSetTypes key, UString xtext, UString value )
{
	switch( key )
	{
	case AST_SPAWNSECTION:
		{
							UString spawnList = xtext.section( " ", 2, 2 ).stripWhiteSpace().upper();
							if( !spawnList.empty() )
							{
								mySpawnItem->SetSpawnSection( value );
								mySpawnItem->IsSectionAList( (spawnList == "TRUE") );
							}
							else
							{
								mySpawnItem->SetSpawnSection( value );
								mySpawnItem->IsSectionAList( false );
							}
		}
																							break;
	case AST_MININTERVAL:	mySpawnItem->SetInterval( 0, value.toUByte() );					break;
	case AST_MAXINTERVAL:	mySpawnItem->SetInterval( 1, value.toUByte() );					break;
	default:																				return;
	}
	s->sysmessage( "Succesfully set '%s' on '%s'", xtext.c_str(), mySpawnItem->GetName().c_str() );
}

void HandleSetObjects( cSocket *s, cBaseObject *myObject, UString xtext )
{
	UString key		= xtext.section( " ", 0, 0 ).stripWhiteSpace();
	UString value	= xtext.section( " ", 1, 1 ).stripWhiteSpace();
	key				= key.upper();

	AllSetTypes myKey = FindAllSetTypeFromKey( key );
	switch( myKey )
	{
	// Settings for normal Base Objects
	case AST_NAME:			myObject->SetName( value );						break;
	case AST_STR:			myObject->SetStrength( value.toShort() );		break;
	case AST_DEX:			myObject->SetDexterity( value.toShort() );		break;
	case AST_INT:			myObject->SetIntelligence( value.toShort() );	break;
	case AST_FAME:			myObject->SetFame( value.toShort() );			break;
	case AST_KARMA:			myObject->SetKarma( value.toShort() );			break;
	case AST_KILLS:			myObject->SetKills( value.toShort() );			break;
	case AST_COLOR:			myObject->SetColour( value.toUShort() );		break;
	case AST_OWNER:			s->TempObj( myObject );
							s->target( 0, TARGET_OWNER2, 220 );				break;
	case AST_X:				myObject->SetLocation( value.toShort(), myObject->GetY(), myObject->GetZ() );	break;
	case AST_Y:				myObject->SetLocation( myObject->GetX(), value.toShort(), myObject->GetZ() );	break;
	default:
							if( myObject->GetObjType() == OT_CHAR )	
								HandleSetChar( s, static_cast<CChar *>(myObject), myKey, xtext, value, key );
							else if( myObject->GetObjType() == OT_ITEM )
								HandleSetItem( s, static_cast<CItem *>(myObject), myKey, xtext, value );
							else if( myObject->GetObjType() == OT_SPAWNER )
								HandleSetSpawner( s, static_cast<CSpawnItem *>(myObject), myKey, xtext, value );
							return;
	}
	s->sysmessage( "Successfully set '%s' on '%s'", xtext.c_str(), myObject->GetName().c_str() );
}
// DarkStorm: Updated AllSetTarget to process the string information sent by the client.
void cTargets::AllSetTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL Serial			= s->GetDWord( 7 );
	cBaseObject *myObject	= NULL;
	if( Serial >= BASEITEMSERIAL )
		myObject = calcItemObjFromSer( Serial );
	else
		myObject = calcCharObjFromSer( Serial );

	if( !ValidateObject( myObject ) )
		return;
	
	UString xtext( s->XText() );
	xtext = xtext.simplifyWhiteSpace();
	// Log EVERYTHING
	// Check whatever we targetted
	if( myObject->GetObjType() == OT_CHAR )
		Commands->Log( "/set", s->CurrcharObj(), (CChar*)myObject, xtext.c_str() );
	else 
	{
		// It gets a bit more complicated for items
		char Message[1024];
		sprintf( Message, "Setting '%s'on '%s'", xtext.c_str(), myObject->GetName().c_str() );
		Commands->Log( "/set", s->CurrcharObj(), NULL, Message );
	}

	HandleSetObjects( s, myObject, xtext );
}
void cTargets::InfoTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	map_st map1;
	CLand land;
	
	SI16 x = s->GetWord( 0x0B );
	SI16 y = s->GetWord( 0x0D );
//	SI08 z = s->GetByte( 0x10 );

	UI08 worldNumber = 0;
	CChar *mChar = s->CurrcharObj();
	if( ValidateObject( mChar ) )
		worldNumber = mChar->WorldNumber();
	if( s->GetWord( 0x11 ) == 0 )
	{
		// manually calculating the ID's if it's a maptype
		map1 = Map->SeekMap0( x, y, worldNumber );
		Map->SeekLand( map1.id, &land );
		GumpDisplay mapStat( s, 300, 300 );
		mapStat.SetTitle( "Map Tile" );

		mapStat.AddData( "Tilenum", map1.id );
		mapStat.AddData( "Flag1", land.Flag1(), 1 );
		mapStat.AddData( "Flag2", land.Flag2(), 1 );
		mapStat.AddData( "Flag3", land.Flag3(), 1 );
		mapStat.AddData( "Flag4", land.Flag4(), 1 );
		mapStat.AddData( "Unknown1", land.Unknown1(), 1 );
		mapStat.AddData( "Unknown2", land.Unknown2(), 1 );
		mapStat.AddData( "Name", land.Name() );
		mapStat.Send( 4, false, INVALIDSERIAL );
	} 
	else
	{
		CTile tile;
		UI16 tilenum = s->GetWord( 0x11 );
		Map->SeekTile( tilenum, &tile );

		GumpDisplay statTile( s, 300, 300 );
		statTile.SetTitle( "Map Tile" );

		statTile.AddData( "Tilenum", tilenum );
		statTile.AddData( "Flag1", tile.Flag1(), 1 );
		statTile.AddData( "Flag2", tile.Flag2(), 1 );
		statTile.AddData( "Flag3", tile.Flag3(), 1 );
		statTile.AddData( "Flag4", tile.Flag4(), 1 );
		statTile.AddData( "Weight", tile.Weight() );
		statTile.AddData( "Layer", tile.Layer(), 1 );
		statTile.AddData( "Anim", tile.Animation(), 1 );
		statTile.AddData( "Unknown1", tile.Unknown1(), 1 );
		statTile.AddData( "Unknown2", tile.Unknown2(), 1 );
		statTile.AddData( "Unknown3", tile.Unknown3(), 1 );
		statTile.AddData( "Height", tile.Height(), 1 );
		statTile.AddData( "Name", tile.Name() );
		statTile.Send( 4, false, INVALIDSERIAL );
	}
	s->sysmessage( 1034 );
}

void cTargets::TweakTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL serial	= s->GetDWord( 7 );
	CChar *c		= calcCharObjFromSer( serial );
	if( ValidateObject( c ) )
		tweakCharMenu( s, c );
	else 
	{
		CItem *i = calcItemObjFromSer( serial );
		if( ValidateObject( i ) )
			tweakItemMenu( s, i );
	}
}

void cTargets::LoadCannon( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	SERIAL moreSerial = s->AddID();
	if( i->GetTempVar( CITV_MORE ) == moreSerial || s->AddID1() == 0xFF )
	{
		if( i->GetTempVar( CITV_MOREZ ) == 0 && objInRange( s, i, DIST_NEARBY ) )
		{
			i->SetTempVar( CITV_MOREZ, 1 );
			s->sysmessage( 1035 );
		}
		else
		{
			if( i->GetTempVar( CITV_MORE, 1 ) == 0x00 ) 
				s->sysmessage( 1036 );
			else 
				s->sysmessage( 1037 );
		}
	}
}

void cTargets::Tiling( cSocket *s )  // Clicking the corners of tiling calls this function - Crwth 01/11/1999
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	if( s->ClickX() == -1 && s->ClickY() == -1 )
	{
		s->ClickX( s->GetWord( 11 ) );
		s->ClickY( s->GetWord( 13 ) );
		s->target( 0, TARGET_TILING, 1038 );
		return;
	}

	SI16 x1 = s->ClickX(), x2 = s->GetWord( 11 );
	SI16 y1 = s->ClickY(), y2 = s->GetWord( 13 );
	SI16 j;

	s->ClickX( -1 );
	s->ClickY( -1 );
	
	if( x1 > x2 ) 
	{ 
		j = x1;
		x1 = x2;
		x2 = j;
	}
	if( y1 > y2 ) 
	{ 
		j = y1; 
		y1 = y2; 
		y2 = j;
	}
	
	UI16 addid = (UI16)(( ( s->AddID1() ) << 8 ) + s->AddID2());

	CItem *c = NULL;
	for( SI16 x = x1; x <= x2; ++x )
	{
		for( SI16 y = y1; y <= y2; ++y ) 
		{
			c = Items->CreateItem( NULL, s->CurrcharObj(), addid, 1, 0, OT_ITEM );
			if( c == NULL ) 
				return;
			c->SetPriv( 0 );	//Make them not decay
			c->SetLocation( x, y, s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ) );
		}
	}
	s->AddID1( 0 );
	s->AddID2( 0 );
}

enum AreaCommandTypes
{
	ACT_DYE = 0,
	ACT_WIPE,
	ACT_INCX,
	ACT_INCY,
	ACT_INCZ,
	ACT_SETX,
	ACT_SETY,
	ACT_SETZ,
	ACT_SETTYPE,
	ACT_NEWBIE,
	ACT_SETSCPTRIG,
	ACT_MOVABLE,
	INVALID_CMD,
	ACT_CMDCOUNT
};

bool AreaCommandFunctor( cBaseObject *a, UI32 &b, void *extraData )
{
	UI32 *ourData = (UI32 *)extraData;
	UI32 x1			= ourData[0];
	UI32 x2			= ourData[1];
	UI32 y1			= ourData[2];
	UI32 y2			= ourData[3];
	UString *value	= (UString *)(ourData[4]);
	if( ValidateObject( a ) && a->CanBeObjType( OT_ITEM ) )
	{
		CItem *i = static_cast< CItem * >(a);
		if( i->GetCont() != NULL )
			return true;
		if( i->GetX() >= ourData[0] && i->GetX() <= ourData[1] && i->GetY() >= ourData[2] && i->GetY() <= ourData[3] )
		{
			switch( b )
			{
				case ACT_DYE:			i->SetColour( value->toUShort() );							break;	// dye
				case ACT_WIPE:			i->Delete();												break;	// wipe
				case ACT_INCX:			i->IncLocation( value->toShort(), 0 );						break;	// incx
				case ACT_INCY:			i->IncLocation( 0, value->toShort() );						break;	// incy
				case ACT_INCZ:			i->IncZ( value->toByte() );									break;	// incz
				case ACT_SETX:			i->SetLocation( value->toShort(), i->GetY(), i->GetZ() );	break;	// setx
				case ACT_SETY:			i->SetLocation( i->GetX(), value->toShort(), i->GetZ() );	break;	// sety
				case ACT_SETZ:			i->SetZ( value->toByte() );									break;	// setz
				case ACT_SETTYPE:		i->SetType( static_cast< ItemTypes >(value->toUByte()) );	break;	// settype
				case ACT_NEWBIE:		i->SetNewbie( value->toUShort() != 0 );						break;	// newbie
				case ACT_SETSCPTRIG:	i->SetScriptTrigger( value->toUShort() );					break;	// set script #
				case ACT_MOVABLE:		i->SetMovable( value->toByte() );							break;	// movable property
				default:																			break;
			}
			// process command here!
		}
	}
	return true;
}
void cTargets::AreaCommand( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	if( s->ClickX() == -1 && s->ClickY() == -1 )
	{
		s->ClickX( s->GetWord( 11 ) );
		s->ClickY( s->GetWord( 13 ) );
		s->target( 0, TARGET_AREACOMMAND, 1040 );
		return;
	}
	
	SI16 x1 = s->ClickX(), x2 = s->GetWord( 11 );
	SI16 y1 = s->ClickY(), y2 = s->GetWord( 13 );
	
	s->ClickX( -1 );
	s->ClickY( -1 );
	
	SI16 c;
	
	if( x1 > x2 ) 
	{ 
		c = x1;
		x1 = x2;
		x2 = c;
	}
	if( y1 > y2 ) 
	{ 
		c = y1; 
		y1 = y2; 
		y2 = c;
	}

	const std::string areaCommandStrings[ACT_CMDCOUNT] =
	{
		"dye",
		"wipe",
		"incx",
		"incy",
		"incz",
		"setx",
		"sety",
		"setz",
		"settype",
		"newbie",
		"setscptrig",
		"movable",
		""
	};

	AreaCommandTypes cmdType = INVALID_CMD;
	UString orgString( s->XText() );
	UString key, value;

	if( orgString.sectionCount( " " ) != 0 )
	{
		key		= orgString.section( " ", 0, 0 ).stripWhiteSpace();
		value	= orgString.section( " ", 1, 1 ).stripWhiteSpace();
	}
	else
		key = orgString;

	for( AreaCommandTypes k = ACT_DYE; k < INVALID_CMD; k = (AreaCommandTypes)((int)k + 1) )
	{
		if( key == areaCommandStrings[k] )
		{
			cmdType = k;
			break;
		}
	}

	if( cmdType == INVALID_CMD )
	{
		s->sysmessage( 1753 );
		return;
	}

	UI32 toPass[5];
	toPass[0]	= x1;
	toPass[1]	= x2;
	toPass[2]	= y1;
	toPass[3]	= y2;
	toPass[4]	= (UI32)&value;
	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, toPass, &AreaCommandFunctor );
}

bool WipeFunctor( cBaseObject *a, UI32 &b, void *extraData )
{
	UI32 *ourData = (UI32 *)extraData;
	UI32 x1			= ourData[0];
	UI32 x2			= ourData[1];
	UI32 y1			= ourData[2];
	UI32 y2			= ourData[3];
	bool wipeVal	= (ourData[4] == 1);
	bool shouldWipe	= wipeVal;
	if( ValidateObject( a ) && a->CanBeObjType( OT_ITEM ) )
	{
		CItem *i = static_cast< CItem * >(a);
		if( i->isWipeable() && i->GetCont() == NULL )
		{
			UI16 iX = i->GetX();
			UI16 iY = i->GetY();
			if( iX >= x1 && iX <= x2 && iY >= y1 && iY <= y2 )
				shouldWipe = !wipeVal;
			if( shouldWipe )
				i->Delete();
		}
	}
	return true;
}

void cTargets::Wiping( cSocket *s )  // Clicking the corners of wiping calls this function - Crwth 01/11/1999
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	if( s->ClickX() == -1 && s->ClickY() == -1 )
	{
		s->ClickX( s->GetWord( 11 ) );
		s->ClickY( s->GetWord( 13 ) );
		if( s->AddID1() ) 
			s->target( 0, TARGET_WIPING, 1039 );
		else 
			s->target( 0, TARGET_WIPING, 1040 );
		return;
	}
	
	SI16 x1 = s->ClickX(), x2 = s->GetWord( 11 );
	SI16 y1 = s->ClickY(), y2 = s->GetWord( 13 );
	
	SI16 c;
	
	if( x1 > x2 ) 
	{ 
		c = x1;
		x1 = x2;
		x2 = c;
	}
	if( y1 > y2 ) 
	{ 
		c = y1; 
		y1 = y2; 
		y2 = c;
	}

	bool iWipe = (s->AddID1() == 1), shouldWipe = iWipe;
	UI32 toPass[5];
	toPass[0]	= x1;
	toPass[1]	= x2;
	toPass[2]	= y1;
	toPass[3]	= y2;
	toPass[4]	= (iWipe ? 1 : 0);
	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, toPass, &WipeFunctor );
	s->ClickX( -1 );
	s->ClickY( -1 );
}

void cTargets::SquelchTarg( cSocket *s )
{
	CChar *p = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( p ) )
	{
		cSocket *pSock = calcSocketObjFromChar( p );
		if( pSock == NULL )
		{
			s->sysmessage( 1754 );
			return;
		}
		if( p->IsGM() )
		{
			s->sysmessage( 1042 );
			return;
		}
		if( p->GetSquelched() )
		{
			p->SetSquelched( 0 );
			s->sysmessage( 1655 );
			pSock->sysmessage( 1043 );
			pSock->SetTimer( tPC_MUTETIME, 0 );
		} 
		else 
		{
			pSock->SetTimer( tPC_MUTETIME, 0 );
			p->SetSquelched( 1 );
			s->sysmessage( 1044 );
			pSock->sysmessage( 761 );
			if( s->AddID1() != 0xFF && s->AddID1() != 0 )			// 255 used to be -1, not good for UI08s
			{
				pSock->SetTimer( tPC_MUTETIME, BuildTimeValue( static_cast<R32>(s->AddID1()) ) );
				s->AddID1( 255 );
				p->SetSquelched( 2 );
			}
		}
	}
}

void cTargets::TeleStuff( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL targSerial = s->GetDWord( 7 );
	if( targSerial == INVALIDSERIAL )
		return;
	s->AddID( targSerial );
	if( targSerial < BASEITEMSERIAL )
		s->target( 0, TARGET_TELESTUFF2, 1045 );
	else
		s->target( 0, TARGET_TELESTUFF2, 1046 );
}

void cTargets::TeleStuff2( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetByte( 11 ) == 0xFF )
		return;
	SERIAL targSerial = s->AddID();
	s->AddID( 0 );
	if( targSerial == INVALIDSERIAL )
		return;
	SI16 x = s->GetWord( 11 );
	SI16 y = s->GetWord( 13 );
	SI08 z = s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) );
	
	if( targSerial < BASEITEMSERIAL )//character
	{
		CChar *cTarg = calcCharObjFromSer( targSerial );
		if( ValidateObject( cTarg ) )
		{
			s->sysmessage( 1047 );
			cTarg->SetLocation( x, y, z );
		}
	} 
	else 
	{
		CItem *iTarg = calcItemObjFromSer( targSerial );
		if( ValidateObject( iTarg ) )
		{
			iTarg->SetLocation( x, y, z );
			s->sysmessage( 1048 );
		}
	}
}

void cTargets::SwordTarget( cSocket *s )
{
	VALIDATESOCKET( s );

	CChar *p		= calcCharObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar	= s->CurrcharObj();

	if( !ValidateObject( mChar ) )
		return;

	if( ValidateObject( p ) )
	{
		if( p->GetID() == 0xCF )
		{
			// Unshorn sheep
			// -> Add Wool and change id of the Sheep
			CItem *c = Items->CreateItem( s, mChar, 0x0DF8, 1, 0, OT_ITEM, true );

			p->SetID( 0xDF );			

			// Add an effect so the sheep can regain it's wool
			cDice myDice( 2, 3, 0 );
			UI32 Delay = myDice.roll();

			Effects->tempeffect( p, p, 43, static_cast<UI16>(Delay*300), 0, 0 );
		}
		else
		{
			// Already sheered
			//s->sysmessage( "" );
		}

		return;
	}
	
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	UI16 targetID = s->GetWord( 0x11 );

	switch( targetID )
	{
		case 0x0CD0:
		case 0x0CD3:
		case 0x0CD6:
		case 0x0CD8:
		case 0x0CDA:
		case 0x0CDD:
		case 0x0CE0:
		case 0x0CE3:
		case 0x0CE6:
		case 0x0CCA:
		case 0x0CCB:
		case 0x0CCC:
		case 0x0CCD:
		case 0x0C12:
		case 0x0CB8:
		case 0x0CB9:
		case 0x0CBA:
		case 0x0CBB:
		{
			SI16 targetX = s->GetWord( 0x0B );		// store our target x y and z locations
			SI16 targetY = s->GetWord( 0x0D );
			SI08 targetZ = s->GetByte( 0x10 );

			SI08 distZ = abs( targetZ - mChar->GetZ() );
			SI16 distY = abs( targetY - mChar->GetY() );
			SI16 distX = abs( targetX - mChar->GetX() );

			if( distY > 5 || distX > 5 || distZ > 9 )
			{
				s->sysmessage( 393 );
				return;
			}
			if( !mChar->IsOnHorse() )
				Effects->PlayCharacterAnimation( mChar, 0x0D );
			else 
				Effects->PlayCharacterAnimation( mChar, 0x1D );
			Effects->PlaySound( s, 0x013E, true );
			CItem *c = Items->CreateItem( s, mChar, 0x0DE1, 1, 0, OT_ITEM, true ); //Kindling
			if( c == NULL ) 
				return;
			s->sysmessage( 1049 );
			return;
		}
		case 0x09CC: 
		case 0x09CD: 
		case 0x09CE: 
		case 0x09CF: 
		{	
			CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) ); 
			if( !ValidateObject( i ) ) 
				return;		
			if( FindItemOwner( i ) != mChar ) 
			{ 
				s->sysmessage( 775 ); 
				return; 
			} 
			else 
			{ 
				UI32 getAmt = GetItemAmount( mChar, i->GetID() ); 
				if( getAmt < 1 ) 
				{ 
					s->sysmessage( 776 ); 
					return; 
				} 
				Effects->PlaySound( s, 0x013E, true); // I'm not sure 
				CItem *c = Items->CreateItem( s, mChar, 0x097A, 4, 0, OT_ITEM, true ); 
				if( ValidateObject( c ) ) 
				{
					c->SetName( "raw fish steak" );
					i->IncAmount( -1 );
				}
				return; 
			} 
		}
		case 0x1BDD:
		case 0x1BE0:
		{
			Skills->BowCraft( s );
			return;
		}
		case 0x2006:
		{
			CorpseTarget( s );
			return;
		}
	}
	s->sysmessage( 1050 );
}

void cTargets::CorpseTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( i ) || !ValidateObject( mChar ) )
		return;
	if( objInRange( mChar, i, DIST_NEARBY ) ) 
	{
		Effects->PlayCharacterAnimation( mChar, 0x20 );
		if( i->GetTempVar( CITV_MORE, 1 ) == 0 )
		{
			i->SetTempVar( CITV_MORE, 1, 1 );
			if( i->GetTempVar( CITV_MOREY ) || i->GetCarve() != -1 )
				newCarveTarget( s, i );
		} 
		else 
			s->sysmessage( 1051 );
	}
	else
		s->sysmessage( 393 );
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	void cTargets::newCarveTarget( cSocket *s, CItem *i )
//|	Date			-	09/22/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Target carving system.
//|									
//|	Modification	-	unknown   	-	Human-corpse carving code added
//|									
//|	Modification	-	unknown   	-	Scriptable carving product added
//|									
//|	Modification	-	09/22/2002	-	Xuri - Fixed erroneous names for body parts 
//|									& made all body parts that are carved from human corpse	
//|									lie in same direction.
//o--------------------------------------------------------------------------o
void cTargets::newCarveTarget( cSocket *s, CItem *i )
{
	VALIDATESOCKET( s );
	bool deletecorpse = false;
	CChar *mChar = s->CurrcharObj();
	CItem *c = Items->CreateItem( NULL, mChar, 0x122A, 1, 0, OT_ITEM ); // add the blood puddle
	if( c == NULL ) 
		return;
	c->SetLocation( i );
	c->SetMovable( 2 );
	c->SetDecayTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( DECAY ) )) );

	char temp[1024];
	// if it's a human corpse
	// Sept 22, 2002 - Xuri - Corrected the alignment of body parts that are carved.
	if( i->GetTempVar( CITV_MOREY ) )
	{
		// create the Head
		sprintf( temp, Dictionary->GetEntry( 1058 ).c_str(), i->GetName2() );
		c = Items->CreateItem( NULL, mChar, 0x1DAE, 1, 0, OT_ITEM );
		if( c == NULL ) 
			return;
		c->SetName( temp );
		c->SetLocation( i );
		c->SetOwner( i->GetOwnerObj() );

		// create the Body
		sprintf( temp, Dictionary->GetEntry( 1059 ).c_str(), i->GetName2() );
		c = Items->CreateItem( NULL, mChar, 0x1CED, 1, 0, OT_ITEM );
		if( c == NULL ) 
			return;
		c->SetName( temp );
		c->SetLocation( i );
		c->SetOwner( i->GetOwnerObj() );

		sprintf( temp, Dictionary->GetEntry( 1057 ).c_str(), i->GetName2() );
		c = Items->CreateItem( NULL, mChar, 0x1DAD, 1, 0, OT_ITEM );
		if( c == NULL ) 
			return;
		c->SetName( temp );
		c->SetLocation( i );
		c->SetOwner( i->GetOwnerObj() );

		// create the Left Arm
		sprintf( temp, Dictionary->GetEntry( 1060 ).c_str(), i->GetName2() );
		c = Items->CreateItem( NULL, mChar, 0x1D80, 1, 0, OT_ITEM );
		if( c == NULL ) 
			return;
		c->SetName( temp );
		c->SetLocation( i );
		c->SetOwner( i->GetOwnerObj() );

		// create the Right Arm
		sprintf( temp, Dictionary->GetEntry( 1061 ).c_str(), i->GetName2() );
		c = Items->CreateItem( NULL, mChar, 0x1DAF, 1, 0, OT_ITEM );
		if( c == NULL ) 
			return;
		c->SetName( temp );
		c->SetLocation( i );
		c->SetOwner( i->GetOwnerObj() );

		// create the Left Leg
		sprintf( temp, Dictionary->GetEntry( 1062 ).c_str(), i->GetName2() );
		c = Items->CreateItem( NULL, mChar, 0x1DB2, 1, 0, OT_ITEM );
		if( c == NULL ) 
			return;
		c->SetName( temp );
		c->SetLocation( i );
		c->SetOwner( i->GetOwnerObj() );

		// create the Right Leg
		sprintf( temp, Dictionary->GetEntry( 1063 ).c_str(), i->GetName2() );
		c = Items->CreateItem( NULL, mChar, 0x1D81, 1, 0, OT_ITEM );
		if( c == NULL ) 
			return;
		c->SetName( temp );
		c->SetLocation( i );
		c->SetOwner( i->GetOwnerObj() );

		//human: always delete corpse!
		deletecorpse = true;
		criminal( mChar );
	} 
	else
	{
		UString sect			= "CARVE " + UString::number( i->GetCarve() );
		ScriptSection *toFind	= FileLookup->FindEntry( sect, carve_def );
		if( toFind == NULL )
			return;
		UString tag;
		UString data;
		UI08 worldNumber = mChar->WorldNumber();
		for( tag = toFind->First(); !toFind->AtEnd(); tag = toFind->Next() )
		{
			data = toFind->GrabData();
			if( tag.upper() == "ADDITEM" )
			{
				if( data.sectionCount( "," ) != 0 )
					Items->CreateScriptItem( s, mChar, data.section( ",", 0, 0 ).stripWhiteSpace(), data.section( ",", 1, 1 ).stripWhiteSpace().toUShort(), OT_ITEM, true );
				else
					Items->CreateScriptItem( s, mChar, data, 0, OT_ITEM, true );
			}
		}
	}

	if( deletecorpse )
	{
		for( c = i->FirstItem(); !i->FinishedItems(); c = i->NextItem() )
		{
			if( ValidateObject( c ) )
			{
				c->SetCont( NULL );
				c->SetLocation( i );
				c->SetDecayTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( DECAY ) )) );
			}
		}
		i->Delete();
	}
}

void cTargets::NpcTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		s->TempObj( i );
		s->target( 0, TARGET_NPCFOLLOW2, 1742 );
	}
}

void cTargets::NpcTarget2( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		if( i->IsNpc() )
		{
			i->SetFTarg( static_cast<CChar *>(s->TempObj()) );
			i->SetNpcWander( 1 );
		}
	}
	s->TempObj( NULL );
}

void cTargets::DupeTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->AddID1() >= 1 )
	{
		UI08 dupetimes = s->AddID1();
		CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
		if( ValidateObject( i ) )
		{
			for( UI08 dupeit = 0; dupeit < dupetimes; ++dupeit )
				Commands->DupeItem( s, i, i->GetAmount() );
		}
	}
}

void cTargets::MoveToBagTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL serial	= s->GetDWord( 7 );
	CItem *i		= calcItemObjFromSer( serial );
	if( !ValidateObject( i ) )
		return;
	CChar *mChar	= s->CurrcharObj();
	CItem *p		= mChar->GetPackItem();
	if( ValidateObject( p ) && i->GetSerial() == serial )
	{
		i->SetCont( p );
		i->PlaceInPack();
		i->SetDecayTime( 0 );//reset decaytimer
	}
}

void cTargets::SellStuffTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
		sendSellStuff( s, i );
}

void cTargets::ReleaseTarget( cSocket *s, SERIAL c )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( c );
	if( ValidateObject( i ) )
	{
		if( !i->IsJailed() )
			s->sysmessage( 1064 );
		else
		{
			JailSys->ReleasePlayer( i );
			s->sysmessage( 1065, i->GetName().c_str() );
		}
	}
}

void cTargets::GmOpenTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *toCheck = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( toCheck ) )
	{
		s->sysmessage( 1066 );
		return;
	}

	CItem *i = toCheck->GetItemAtLayer( static_cast<UI08>(s->TempInt()) );
	if( ValidateObject( i ) )
	{
		s->openPack( i );
		return;
	}
	s->sysmessage( 1067 );
}

void cTargets::StaminaTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		Effects->PlaySound( i, 0x01F2 );
		Effects->PlayStaticAnimation( i, 0x376A, 0x09, 0x06 );
		i->SetStamina( i->GetMaxStam() );
		return;
	}
	s->sysmessage( 1066 );
}

void cTargets::ManaTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		Effects->PlaySound( i, 0x01F2 );
		Effects->PlayStaticAnimation( i, 0x376A, 0x09, 0x06 );
		i->SetMana( i->GetMaxMana() );
		return;
	}
	s->sysmessage( 1066 );
}

void cTargets::MakeShopTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		MakeShop( i );
		s->sysmessage( 1068 );
		return;
	}
	s->sysmessage( 1069 );
}

void cTargets::JailTarget( cSocket *s, SERIAL c )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( c );
    if( !ValidateObject( i ) ) 
		return;

	if( i->IsJailed() )
	{
		s->sysmessage( 1070 );
		return;
	}
    if( !JailSys->JailPlayer( i, 0xFFFFFFFF ) )
		s->sysmessage( 1072 );
}

void cTargets::AttackTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *target	= static_cast<CChar *>(s->TempObj());
	CChar *target2	= calcCharObjFromSer( s->GetDWord( 7 ) );
	s->TempObj( NULL );
	
	if( !ValidateObject( target2 ) || !ValidateObject( target ) ) 
		return;
	if( target2 == target )
	{
		s->sysmessage( 1073 );
		return;
	}
	Combat->AttackTarget( target, target2 );
	if( target2->IsInnocent() && target2 != target->GetOwnerObj() )
	{
		CChar *pOwner = target->GetOwnerObj();
		if( ValidateObject( pOwner ) && Combat->WillResultInCriminal( pOwner, target2 ) )
			criminal( pOwner );
	}
}

void cTargets::FollowTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *char1	= static_cast<CChar *>(s->TempObj());
	CChar *char2	= calcCharObjFromSer( s->GetDWord( 7 ) );
	s->TempObj( NULL );
	if( !ValidateObject( char1 ) || !ValidateObject( char2 ) )
		return;
	
	char1->SetFTarg( char2 );
	char1->SetNpcWander( 1 );
}

void cTargets::TransferTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *char1 = static_cast<CChar *>(s->TempObj());
	CChar *char2 = calcCharObjFromSer( s->GetDWord( 7 ) );
	s->TempObj( NULL );
	
	if( !ValidateObject( char1 ) )
		return;

	if( !ValidateObject( char2 ) )
	{
		s->sysmessage( 1066 );
		return;
	}
	if( char1 == char2 )
	{
		s->sysmessage( 1066 );
		return;
	}

	Npcs->stopPetGuarding( char1 );

	char1->talkAll( 1074, false, char1->GetName().c_str(), char2->GetName().c_str() );
	
	char1->SetOwner( char2 );
	char1->SetNpcWander( 1 );
	
	char1->SetFTarg( NULL );
	char1->SetNpcWander( 0 );
}

void cTargets::BuyShopTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL serial	= s->GetDWord( 7 );
	CChar *i		= calcCharObjFromSer( serial );
	if( ValidateObject( i ) )
	{
		BuyShop( s, i );
		return;
	}
	s->sysmessage( 1075 );
}

bool cTargets::BuyShop( cSocket *s, CChar *c )
{
	if( s == NULL )
		return false;
	if( !ValidateObject( c ) )
		return false;
	CItem *buyPack		= c->GetItemAtLayer( IL_BUYCONTAINER );
	CItem *boughtPack	= c->GetItemAtLayer( IL_BOUGHTCONTAINER );
	
	if( !ValidateObject( buyPack ) || !ValidateObject( boughtPack ) )
		return false;
	
	buyPack->Sort();
	boughtPack->Sort();
	c->Update( s );

	CPItemsInContainer iic( s, buyPack,c );	s->Send( &iic );
	CPOpenBuyWindow obw( buyPack, c );		s->Send( &obw );

	CPItemsInContainer iic2( s, boughtPack, c );	s->Send( &iic2 );
	CPOpenBuyWindow obw2( boughtPack, c );		s->Send( &obw2 );

	CPDrawContainer toSend;
	toSend.Model( 0x0030 );
	toSend.Serial( c->GetSerial() );
	s->Send( &toSend );
	s->statwindow( s->CurrcharObj() ); // Make sure the gold total has been sent.
	return true;
}

void cTargets::permHideTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		if( i->GetHidden() == 1 )
		{
			s->sysmessage( 833 );
			return;
		}
		i->SetPermHidden( true );
		i->SetHidden( 1 );
	}
}

void cTargets::unHideTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL ser = s->GetDWord( 7 );
	if( ser < BASEITEMSERIAL )
	{
		CChar *c = calcCharObjFromSer( ser );
		if( ValidateObject( c ) )
		{
			if( !c->IsGM() && !c->IsCounselor() )
			{
				c->SetPermHidden( false );
				c->SetHidden( false );
			}
		}
	}
	else
	{
		CItem *i = calcItemObjFromSer( ser );
		if( ValidateObject( i ) )
			i->SetVisible( false );
	}
}

void cTargets::SetPoisonedTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		i->SetPoisoned( static_cast<SI08>(s->TempInt()) );
		i->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( POISON ) )) );
		i->Dirty( UT_UPDATE );
	}
}

void cTargets::FullStatsTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		Effects->PlaySound( i, 0x01F2 );
		Effects->PlayStaticAnimation( i, 0x376A, 0x09, 0x06 );
		i->SetMana( i->GetMaxMana() );
		i->SetHP( i->GetMaxHP() );
		i->SetStamina( i->GetMaxStam() );
		return;
	}
	s->sysmessage( 1077 );
}


void cTargets::AxeTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	UI16 realID = s->GetWord( 0x11 );
    // [krazyglue] it may take more lines, but at least its readable and easier to debug =)
	if( realID == 0x0CD0 || realID == 0x0CD3 || realID == 0x0CD6 || realID == 0x0CD8 || realID == 0x0CDA || 
		realID == 0x0CDD || realID == 0x0CE0 || realID == 0x0CE3 || realID == 0x0CE6 || realID == 0x0D58 || 
		realID >= 0x0CCA && realID <= 0x0CCE || realID >= 0x12B8 && realID <= 0x12BB || realID == 0x0D42 ||
		realID == 0x0D43 || realID == 0x0D58 || realID == 0x0D59 || realID == 0x0D70 || realID == 0x0D85 || 
		realID == 0x0D94 || realID == 0x0D95 || realID == 0x0D98 || realID == 0x0DA4 || realID == 0x0DA8 )
	{
		Skills->TreeTarget( s );
	}
	else if( realID == 0x2006 )
		CorpseTarget( s );
    else 
		Skills->BowCraft( s );
}

//o---------------------------------------------------------------------------o
//|   Function		-  void npcresurrecttarget( CChar *i )
//|   Date			-  UnKnown
//|   Programmer	-  UnKnown  (Touched tabstops by Tauriel Dec 28, 1998)
//|									
//|	Modification	-	09/22/2002	-	Xuri - Made players not appear with full 
//|									health/stamina after being resurrected by NPC Healer
//o---------------------------------------------------------------------------o
//|   Purpose     -  Resurrects a character
//o---------------------------------------------------------------------------o
void cTargets::NpcResurrectTarget( CChar *i )
{
	if( !ValidateObject( i ) )
		return;

	if( i->IsNpc() )
	{
		Console.Error( 2, Dictionary->GetEntry( 1079 ).c_str(), i );
		return;
	}
	cSocket *mSock = calcSocketObjFromChar( i );
	// the char is a PC, but not logged in.....
	if( mSock != NULL )
	{
		if( i->IsDead() )
		{
			Fame( i, 0 );
			Effects->PlaySound( i, 0x0214 );
			i->SetID( i->GetOrgID() );
			i->SetSkin( i->GetOrgSkin() );
			i->SetDead( false );
			// Sept 22, 2002 - Xuri
			i->SetHP( i->GetMaxHP() / 10 );
			i->SetStamina( i->GetMaxStam() / 10 );
			//
			i->SetMana( i->GetMaxMana() / 10 );
			i->SetAttacker( NULL );
			i->SetAttackFirst( false );
			i->SetWar( false );
			i->SetHunger( 6 );
			CItem *c = NULL;
			for( CItem *j = i->FirstItem(); !i->FinishedItems(); j = i->NextItem() )
			{
				if( ValidateObject( j ) && !j->isFree() )
				{
					if( j->GetLayer() == IL_BUYCONTAINER )
					{
						j->SetLayer( IL_PACKITEM );	
						i->SetPackItem( j );
					}
					if( j->GetSerial() == i->GetRobe() )
					{
						j->Delete();
						
						c = Items->CreateItem( NULL, i, 0x1F03, 1, 0, OT_ITEM );
						if( c != NULL )
						{
							c->SetName( "a resurrect robe" );
							c->SetLayer( IL_ROBE );
							if( c->SetCont( i ) )
								c->SetDye( true );
						}
					}
				}
			}
			UI16 targTrig		= i->GetScriptTrigger();
			cScript *toExecute	= Trigger->GetScript( targTrig );
			if( toExecute != NULL )
				toExecute->OnResurrect( i );
		}
		else
			mSock->sysmessage( 1080 );
	}
	else
		Console.Warning( 2, "Attempt made to resurrect a PC (serial: 0x%X) that's not logged in", i->GetSerial() );
}


void killKeys( SERIAL targSerial );
void cTargets::HouseOwnerTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	SERIAL o_serial = s->GetDWord( 7 );
	if( o_serial == INVALIDSERIAL ) 
		return;

	CChar *own		= calcCharObjFromSer( o_serial );
	cSocket *oSock	= calcSocketObjFromChar( own );
	CItem *sign		= static_cast<CItem *>(s->TempObj());
	CItem *house	= calcItemObjFromSer( sign->GetTempVar( CITV_MORE ) );
	s->TempObj( NULL );
	if( !ValidateObject( sign ) || !ValidateObject( house ) )
		return;
	sign->SetOwner( own );
	house->SetOwner( own );

	killKeys( house->GetSerial() );

	CItem *key = Items->CreateItem( oSock, own, 0x100F, 1, 0, OT_ITEM, true );	// gold key for everything else
	if( key == NULL )
		return;
	key->SetName( "a house key" );
	key->SetTempVar( CITV_MORE, house->GetSerial() );
	key->SetType( IT_KEY );

	s->sysmessage( 1081, own->GetName().c_str() );
	oSock->sysmessage( 1082, mChar->GetName().c_str() );
}

void cTargets::HouseEjectTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	CItem *h = static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( c ) && ValidateObject( h ) ) 
	{
		SI16 sx, sy, ex, ey;
		Map->MultiArea( (CMultiObj *)h, sx, sy, ex, ey );
		if( c->GetX() >= sx && c->GetY() >= sy && c->GetX() <= ex && c->GetY() <= ey )
		{
			c->SetLocation( ex, ey, c->GetZ() );
			s->sysmessage( 1083 );
		} 
		else 
			s->sysmessage( 1084 );
	}
}

UI08 AddToHouse( CMultiObj *house, CChar *toAdd, UI08 mode = 0 );
void cTargets::HouseBanTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	// first, eject the player
	HouseEjectTarget( s );
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	CItem *h = static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( c ) && ValidateObject( h ) ) 
	{
		CMultiObj *house = static_cast<CMultiObj *>(h);
		UI08 r = AddToHouse( house, c, 1 );
		if( r == 1 ) 
			s->sysmessage( 1085, c->GetName().c_str() );
		else if( r == 2 ) 
			s->sysmessage( 1086 );
		else 
			s->sysmessage( 1087 );
	}
}

void cTargets::HouseFriendTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	CItem *h = static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( c ) && ValidateObject( h ) ) 
	{
		CMultiObj *house = static_cast<CMultiObj *>(h);
		UI08 r = AddToHouse( house, c );
		if( r == 1 ) 
		{
			cSocket *cSock = calcSocketObjFromChar( c );
			if( cSock != NULL )
				cSock->sysmessage( 1089 );
			s->sysmessage( 1088, c->GetName().c_str() );
		} 
		else if( r == 2 ) 
			s->sysmessage( 1090 );
		else 
			s->sysmessage( 1091 );
	}
}

bool DeleteFromHouseList( CMultiObj *house, CChar *toDelete, UI08 mode = 0 );
void cTargets::HouseUnlistTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	CItem *h = static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( c ) && ValidateObject( h ) ) 
	{
		CMultiObj *house = static_cast<CMultiObj *>(h);
		bool r = DeleteFromHouseList( house, c );
		if( r )
			s->sysmessage( 1092, c->GetName().c_str() );
		else 
			s->sysmessage( 1093 );
	}
}

void cTargets::GlowTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) ) 
	{ 
		s->sysmessage( 1095 ); 
		return; 
	}
	CChar *mChar = s->CurrcharObj(), *k = NULL;
	if( !ValidateObject( mChar ) )
		return;

	if( i->GetGlow() != INVALIDSERIAL ) 
	{
		s->sysmessage( 1097 );
		return;
	}

	cBaseObject *getCont = i->GetCont();
	if( getCont != NULL )
	{
		if( getCont->GetObjType() == OT_ITEM )
			k = FindItemOwner( (CItem *)getCont );
		else
			k = (CChar *)getCont;

		if( k != mChar )
		{
			s->sysmessage( 1096 );
			return;
		}
	}

	i->SetGlowColour( i->GetColour() );

	CItem *c = Items->CreateItem( s, mChar, 0x1647, 1, 0, OT_ITEM ); // spawn light emitting object
	if( c == NULL )
		return;
	c->SetName( "glower" );
    c->SetDir( 29 );
	c->SetVisible( 0 );
	c->SetMovable( 2 );
	if( getCont == NULL || getCont->GetObjType() == OT_ITEM ) // if not equipped -> coords of the light-object = coords of the 
		c->SetLocation( i );
	else // if equipped -> place lightsource at player ( height= approx hand level )
	{ 
		c->SetLocation( mChar );
		c->SetZ( mChar->GetZ() + 4 );
	}

    c->SetPriv( 0 ); // doesnt decay

	i->SetGlow( c->GetSerial() ); // set glow-identifier

	mChar->Update( s );
	s->sysmessage( 1098 );
}

void cTargets::UnglowTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
	{ 
		s->sysmessage( 1099 ); 
		return; 
	}
	CChar *mChar = s->CurrcharObj(), *k = NULL;
	if( !ValidateObject( mChar ) )
		return;

	cBaseObject *getCont = i->GetCont();
	if( getCont != NULL ) 
	{      
		if( getCont->GetObjType() == OT_ITEM )
			k = FindItemOwner( (CItem *)getCont );
		else
			k = (CChar *)getCont;

		if( k != mChar )
		{
			s->sysmessage( 1100 );
			return;
		}
	}

	CItem *j = calcItemObjFromSer( i->GetGlow() );
	if( i->GetGlow() == INVALIDSERIAL || !ValidateObject( j ) ) 
	{
		s->sysmessage( 1101 );
		return;
	}

	i->SetColour( i->GetGlowColour() );

	j->Delete();
	i->SetGlow( INVALIDSERIAL );
	mChar->Update( s );

	s->sysmessage( 1102 );
}

void cTargets::ShowSkillTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *p = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( p ) )
	{
		s->sysmessage( 1103 );
		return;
	}
	char spc[2]="\x20";
	char temp[1024];
	int j, k, zz, ges = 0;
	char skill_info[(ALLSKILLS+1)*40];
	UString sk;

	SI32 z = s->TempInt();
	if( z < 0 || z > 3 ) 
		z = 0;
	if( z == 2 || z == 3 ) 
		sprintf( skill_info, "%s's skills:", p->GetName().c_str() ); 
	else 
		sprintf( skill_info, "%s's baseskills:", p->GetName().c_str() );
	size_t b = p->GetName().size() + 11;
	if( b > 23 ) 
		b = 23;

	UI08 c;
	for( c = b; c <= 26; ++c )
	{
		strcpy( temp, spc );
		strcpy( &skill_info[strlen( skill_info )], temp );
	}

	sk = UString::number( ges );
	sprintf( temp, "sum: %s", sk.c_str() );
	strcpy( &skill_info[strlen( skill_info )], temp );
	for( UI08 a = 0; a < ALLSKILLS; ++a )
	{
		if( z == 0 || z == 1 ) 
			k = p->GetBaseSkill( a ); 
		else 
			k = p->GetSkill( a );
		if( z == 0 || z == 2 ) 
			zz = 9; 
		else 
			zz = -1;

		if( k > zz ) // show only if skills >= 1
		{
			if( z == 2 || z == 3 ) 
				j = p->GetSkill( a )/10; 
			else 
				j = p->GetBaseSkill( a )/10; // get skill value
			sk = UString::number( j );	// skill value string in sk
			ges += j;
			sprintf( temp, "%s%s%s", skillname[a], spc, sk.c_str() );
			strcpy( &skill_info[strlen( skill_info )], temp );

			b = strlen( skillname[a] ) + sk.length() + 1; // it doesn't like \n's, so insert spaces till end of line
			if( b > 23 )
				b = 23;
			for( c = b; c <= 26; ++c )
			{
				strcpy( temp, spc );
				strcpy( &skill_info[strlen( skill_info )], temp );
			}
		}
	}

	sk = UString::number( ges );
	sprintf( temp, "sum: %s  ", sk.c_str() );
	strcpy( &skill_info[ strlen( skill_info )], temp );

	CPUpdScroll toSend( 2 );
	toSend.AddString( skill_info );
	toSend.Finalize();
	s->Send( &toSend );
}

void cTargets::FriendTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CChar *targChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( targChar ) )
	{
		s->sysmessage( 1103 );
		return;
	}
	if( targChar->IsNpc() || !isOnline( targChar ) || targChar == mChar )
	{
		s->sysmessage( 1622 );
		return;
	}

	CChar *pet = static_cast<CChar *>(s->TempObj());
	s->TempObj( NULL );
	if( Npcs->checkPetFriend( targChar, pet ) )
	{
		s->sysmessage( 1621 );
		return;
	}

	CHARLIST *petFriends = pet->GetFriendList();
	// Make sure to cover the STL response
	if( petFriends != NULL )
	{
		if( petFriends->size() >= 10 )
		{
			s->sysmessage( 1623 );
			return;
		}
	}

	pet->AddFriend( targChar );
	s->sysmessage( 1624, pet->GetName().c_str(), targChar->GetName().c_str() );

	cSocket *targSock = calcSocketObjFromChar( targChar );
	if( targSock != NULL )
		targSock->sysmessage( 1625, mChar->GetName().c_str(), pet->GetName().c_str() );
}

void cTargets::GuardTarget( cSocket *s )
//PRE:	Pet has been commanded to guard
//POST: Pet guards person, if owner currently
//DEV:	Abaddon
//DATE: 3rd October
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CChar *petGuarding = static_cast<CChar *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( petGuarding ) )
		return;

	Npcs->stopPetGuarding( petGuarding );

	CChar *charToGuard = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( charToGuard ) )
	{
		if( charToGuard != petGuarding->GetOwnerObj() && !Npcs->checkPetFriend( charToGuard, petGuarding ) )
		{
			s->sysmessage( 1628 );
			return;
		}
		petGuarding->SetNPCAiType( aiPET_GUARD ); // 32 is guard mode
		petGuarding->SetGuarding( charToGuard );
		mChar->SetGuarded( true );
		return;
	}
	CItem *itemToGuard = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( itemToGuard ) && !itemToGuard->isPileable() )
	{
		CItem *p = mChar->GetPackItem();
		if( ValidateObject( p ) )
		{
			if( itemToGuard->GetCont() == p || itemToGuard->GetCont() == mChar )
			{
				itemToGuard->SetGuarded( true );
				petGuarding->SetGuarding( itemToGuard );
			}
		}
		else
		{
			CMultiObj *multi = findMulti( itemToGuard );
			if( ValidateObject( multi ) )
			{
				if( multi->GetOwnerObj() == mChar )
				{
					itemToGuard->SetGuarded( true );
					petGuarding->SetGuarding( itemToGuard );
				}
			}
			else
				s->sysmessage( 1628 );
		}
	}
}

void cTargets::ResurrectionTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
		NpcResurrectTarget( i );	// npcresurrect checks to see it's valid
}

void cTargets::HouseLockdown( cSocket *s ) // Abaddon
// PRE:		S is the socket of a valid owner/coowner and is in a valid house
// POST:	either locks down the item, or puts a message to the owner saying he's a moron
// CODER:	Abaddon
// DATE:	17th December, 1999
{
	VALIDATESOCKET( s );
	CItem *itemToLock = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( itemToLock ) )
	{
		if( !itemToLock->CanBeLockedDown() )
		{
			s->sysmessage( 1106 );
			return;
		}
		CItem *house = static_cast<CItem *>(s->TempObj());
		s->TempObj( NULL );
		// time to lock it down!
		CMultiObj *multi = findMulti( itemToLock );
		if( ValidateObject( multi ) )
		{
			if( multi != house )
			{
				s->sysmessage( 1105 );
				return;
			}
			if( multi->GetLockDownCount() < multi->GetMaxLockDowns() )
				multi->LockDownItem( itemToLock );
			else
				s->sysmessage( "You have too many locked down items" );
			return;
		}
		// not in a multi!
		s->sysmessage( 1107 );
	}
	else
		s->sysmessage( 1108 );
}

void cTargets::HouseRelease( cSocket *s ) // Abaddon
// PRE:		S is the socket of a valid owner/coowner and is in a valid house, the item is locked down
// POST:	either releases the item from lockdown, or puts a message to the owner saying he's a moron
// CODER:	Abaddon
// DATE:	17th December, 1999
{
	VALIDATESOCKET( s );
	CItem *itemToLock = calcItemObjFromSer( s->GetDWord( 7 ) );

	if( ValidateObject( itemToLock ) || !itemToLock->IsLockedDown() )
	{
		CItem *house = static_cast<CItem *>(s->TempObj());	// let's find our house
		s->TempObj( NULL );
		// time to lock it down!
		CMultiObj *multi = findMulti( itemToLock );
		if( ValidateObject( multi ) )
		{
			if( multi != house )
			{
				s->sysmessage( 1109 );
				return;
			}
			if( multi->GetLockDownCount() > 0 )
				multi->RemoveLockDown( itemToLock );	// Default as stored by the client, perhaps we should keep a backup?
			return;
		}
		// not in a multi!
		s->sysmessage( 1107 );
	}
	else
		s->sysmessage( 1108 );
}

void cTargets::ShowDetail( cSocket *s ) // Abaddon
// PRE:		S is the socket of the person getting the item information
// POST:	prints out detailed information about an item
// CODER:	Abaddon
// DATE:	11th January, 2000
{
	VALIDATESOCKET( s );
	char message[512];
	CItem *itemToCheck = calcItemObjFromSer( s->GetDWord( 7 ) );

	if( ValidateObject( itemToCheck ) )
	{
		switch( itemToCheck->GetType() )
		{
			case IT_NOTYPE:
				strcpy( message, "Default type" );
				break;
			case IT_CONTAINER:	// container/backpack
				strcpy( message, "Container/backpack:" );
				if( itemToCheck->GetTempVar( CITV_MOREB, 1 ) > 0 )
					strcat( message, "Magically trapped" );
				break;
			case IT_CASTLEGATEOPENER:	// opener for castle gate 1
			case IT_CHAOSGATEOPENER:	// opener for castle gate 2
				strcpy( message, "Opener for castle gate" );
				break;
			case IT_CASTLEGATE:	// castle gate 1
			case IT_CHAOSGATE:	// castle gate 2
				strcpy( message, "Castle gate" );
				break;
			case IT_TELEPORTITEM:	// teleporter rune
				strcpy( message, "Teleporter rune, acts like teleport was cast" );
				break;
			case IT_KEY:	// key
				strcpy( message, "Key" );
				break;
			case IT_LOCKEDCONTAINER:	// locked container
				strcpy( message, "Locked container:" );
				if( itemToCheck->GetTempVar( CITV_MOREB, 1 ) > 0 )
					strcat( message, "Magically trapped" );
				break;
			case IT_SPELLBOOK:	// Spellbook (item 14FA)
				strcpy( message, "Spellbook" );
				break;
			case IT_MAP:	// map( item 0E EB )
				strcpy( message, "This opens a map based on the serial num of the item" );
				break;
			case IT_READABLEBOOK:	// book
				sprintf( message, "A book:Entry in misc.scp: %li", itemToCheck->GetTempVar( CITV_MORE ) );
				break;
			case IT_DOOR:	// doors
				strcpy( message, "Unlocked door" );
				break;
			case IT_LOCKEDDOOR:	// locked door
				strcpy( message, "Locked door" );
				break;
			case IT_FOOD:	// food
				strcpy( message, "Food item, reduces hunger by one point" );
				break;
			case IT_MAGICWAND:	// magic wand
				sprintf( message, "Magic wand\nCircle: %u:Spell within circle: %u:Charges left: %u", itemToCheck->GetTempVar( CITV_MOREX ), itemToCheck->GetTempVar( CITV_MOREY ), itemToCheck->GetTempVar( CITV_MOREZ ) );
				break;
			case IT_RESURRECTOBJECT:	// resurrection object
				strcpy( message, "Resurrection object" );
				break;
			case IT_CRYSTALBALL:	// enchanted item (Crystal ball)
				strcpy( message, "Enchanted item that displays a random message" );
				break;
			case IT_POTION:	// potion
				strcpy( message, "Potion: " );
				switch( itemToCheck->GetTempVar( CITV_MOREY ) )
				{
					case 1: // Agility Potion
						switch( itemToCheck->GetTempVar( CITV_MOREZ ) )
						{
							case 1:		strcat( message, "Agility potion" );	break;
							case 2:		strcat( message, "Greater Agility potion" );	break;
							default:	strcat( message, "Unknown Agility potion" );	break;
						}
						break;
					case 2: // Cure Potion
						switch( itemToCheck->GetTempVar( CITV_MOREZ ) )
						{
							case 1:		strcat( message, "Lesser cure potion" ); break;
							case 2:		strcat( message, "Cure potion" ); break;
							case 3:		strcat( message, "Greater Cure potion" ); break;
							default:	strcat( message, "Unknown cure potion" ); break;
						}
						break;
					case 3: // Explosion Potion
						strcat( message, "Explosion potion" );
						break;
					case 4: // Heal Potion
						switch( itemToCheck->GetTempVar( CITV_MOREZ ) )
						{
							case 1:		strcat( message, "Lesser Heal potion" );	break;
							case 2:		strcat( message, "Heal potion" ); break;
							case 3:		strcat( message, "Greater Heal potion" ); break;
							default:	strcat( message, "Unknown Heal potion" );
						}
						break;
					case 5: // Night Sight Potion
						strcat( message, "Night sight potion" );
						break;
					case 6: // Poison Potion
						switch( itemToCheck->GetTempVar( CITV_MOREZ ) )
						{
							case 0:		strcat( message, "Poison potion with no poison" ); break;
							case 1:		strcat( message, "Lesser Poison potion" ); break;
							case 2:		strcat( message, "Poison potion" ); break;
							case 3:		strcat( message, "Greater Poison potion" ); break;
							case 4:		strcat( message, "Deadly Poison potion" ); break;
							default:	strcat( message, "Unknown Poison potion" );  break;
						}
						break;
					case 7: // Refresh Potion
						switch( itemToCheck->GetTempVar( CITV_MOREZ ) )
						{
							case 1:		strcat( message, "Lesser Refresh potion" ); break;
							case 2:		strcat( message, "Refresh potion" );	break;
							default:	strcat( message, "Unknown Refresh potion" ); break;
						}
						break;
					case 8: // Strength Potion
						switch( itemToCheck->GetTempVar( CITV_MOREZ ) )
						{
							case 1:		strcat( message, "Lesser Strength potion" );	break;
							case 2:		strcat( message, "Strength potion" ); break;
							default:	strcat( message, "Unknown Strength potion" ); break;
						}
						break;
					case 9: // Mana Potion
						switch( itemToCheck->GetTempVar( CITV_MOREZ ) )
						{
							case 1:		strcat( message, "Lesser Mana potion" );	break;
							case 2:		strcat( message, "Mana potion" ); break;
							default:	strcat( message, "Unknown Mana potion" ); break;
						}
						break;
					default:
						strcat( message, "Unknown potion" );
						break;
				}
				break;
			case IT_TOWNSTONE:	// townstone deed/stone
				if( itemToCheck->GetID() == 0x14F0 )
					strcpy( message, "Townstone deed, will make townstone" );
				else
					strcpy( message, "Townstone, use to find out information on the town" );
				break;
			case IT_RECALLRUNE:	// recall rune
				if( itemToCheck->GetTempVar( CITV_MOREX ) == 0 && itemToCheck->GetTempVar( CITV_MOREY ) == 0 && itemToCheck->GetTempVar( CITV_MOREZ ) == 0 )	// changed, to fix, Lord Vader
					strcpy( message,"Unmarked recall rune");
				else
					strcpy( message, "This will rename a recall rune" );
				break;
			case IT_GATE:	// Moongate;
			case IT_ENDGATE:
				CItem *moonGate;
				moonGate = calcItemObjFromSer( itemToCheck->GetTempVar( CITV_MOREX ) );
				sprintf( message, "Moongate going to X %i Y %i Z %i", moonGate->GetX(), moonGate->GetY(), moonGate->GetZ() );
				break;
			case IT_OBJTELEPORTER:	// object teleporter
				sprintf( message, "A teleporter going to X %i Y %i Z %i", itemToCheck->GetTempVar( CITV_MOREX ), itemToCheck->GetTempVar( CITV_MOREY ), itemToCheck->GetTempVar( CITV_MOREZ ) );
				break;
			case IT_ITEMSPAWNER:	// item spawner
				sprintf( message, "Item spawner:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", itemToCheck->GetTempVar( CITV_MOREX ), itemToCheck->GetTempVar( CITV_MOREZ ), itemToCheck->GetTempVar( CITV_MOREY ) );
				break;
			case IT_NPCSPAWNER:	// monster/npc spanwer
				sprintf( message, "Monster/NPC spawner:Amount: %i:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", itemToCheck->GetAmount(), itemToCheck->GetTempVar( CITV_MOREX ), itemToCheck->GetTempVar( CITV_MOREZ ), itemToCheck->GetTempVar( CITV_MOREY ) );
				break;
			case IT_SPAWNCONT:	// spawn container
				strcpy( message, "Item Spawn container:" );
				if( itemToCheck->GetTempVar( CITV_MOREB, 1 ) > 0 )
					strcat( message, "Magically trapped:" );
				sprintf( message, "%sRespawn max time: %i:Respawn min time: %i", message, itemToCheck->GetTempVar( CITV_MOREZ ), itemToCheck->GetTempVar( CITV_MOREY ) );
				break;
			case IT_LOCKEDSPAWNCONT:	// locked spawn container
				strcpy( message, "Locked item spawn container:" );
				if( itemToCheck->GetTempVar( CITV_MOREB, 1 ) > 0 )
					strcat( message, "Magically trapped:" );
				sprintf( message, "%sRespawn max time: %i:Respawn min time: %i", message, itemToCheck->GetTempVar( CITV_MOREZ ), itemToCheck->GetTempVar( CITV_MOREY ) );
				break;
			case IT_UNLOCKABLESPAWNCONT:	// unlockable item spawner container
				strcpy( message, "Unlockable item spawner container" );
				break;
			case IT_AREASPAWNER:	// area spawner
				sprintf( message, "Area spawner:X +/- value: %i:Y +/- value: %i:Amount: %i:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", itemToCheck->GetTempVar( CITV_MORE, 3 ), itemToCheck->GetTempVar( CITV_MORE, 4 ), itemToCheck->GetAmount(), itemToCheck->GetTempVar( CITV_MOREX ), itemToCheck->GetTempVar( CITV_MOREZ ), itemToCheck->GetTempVar( CITV_MOREY ) );
				break;
			case IT_ADVANCEGATE:	// single use advancement gate
				sprintf( message, "Single use advancement gate: advance.scp entry %i", itemToCheck->GetTempVar( CITV_MOREX ) );
				break;
			case IT_MULTIADVANCEGATE:	// multi-use advancement gate
				sprintf( message, "Multi use advancement gate: advance.scp entry %i", itemToCheck->GetTempVar( CITV_MOREX ) );
				break;
			case IT_MONSTERGATE:	// monster gate
				sprintf( message, "Monster gate: monster number %i", itemToCheck->GetTempVar( CITV_MOREX ) );
				break;
			case IT_RACEGATE:	// race gates
				sprintf( message, "Race Gate:Turns into %s:", Races->Name( static_cast<RACEID>(itemToCheck->GetTempVar( CITV_MOREX )) ) );
				if( itemToCheck->GetTempVar( CITV_MOREY ) == 1 )
					strcat( message, "Constantly reuseable:" );
				else
					strcat( message, "One time use only:" );
				if( Races->IsPlayerRace( static_cast<RACEID>(itemToCheck->GetTempVar( CITV_MOREX ) )) )
					strcat( message, "Player Race:" );
				else
					strcat( message, "NPC Race only:" );
				break;
			case IT_DAMAGEOBJECT:	// damage object
				sprintf( message, "Damage object:Minimum damage %i:Maximum Damage %i", itemToCheck->GetTempVar( CITV_MOREX ) + itemToCheck->GetTempVar( CITV_MOREY ), itemToCheck->GetTempVar( CITV_MOREX ) + itemToCheck->GetTempVar( CITV_MOREZ ) );
				break;
			case IT_TRASHCONT:	// trash container
				strcpy( message, "A trash container" );
				break;
			case IT_SOUNDOBJECT:	// sound object
				sprintf( message, "Sound object that plays whenever someone is near:Soundeffect: %i\nRadius: %i\nProbability: %i", itemToCheck->GetTempVar( CITV_MOREX ), itemToCheck->GetTempVar( CITV_MOREY ), itemToCheck->GetTempVar( CITV_MOREZ ) );
				break;
			case IT_MAPCHANGEOBJECT:	// map change
				sprintf( message, "Map Change object that changes world:World: %u", itemToCheck->GetTempVar( CITV_MORE ) );
				break;
			case 100:	// not documented
				strcpy( message, "Looks like hide/unhide object:More detail to come later\n" );
				break;
			case IT_MORPHOBJECT:	// morph object morex = npc# in npc.scp
				sprintf( message, "Morph object:Morphs char into body %x %x", itemToCheck->GetTempVar( CITV_MOREX )>>8, itemToCheck->GetTempVar( CITV_MOREX )%256 );
				break;
			case IT_UNMORPHOBJECT:	// unmorph
				strcpy( message, "Unmorph object, unmorphs back to body before using it as type 101, switches to 101 again" );
				break;
			case IT_DRINK:	// drink object
				strcpy( message, "You can drink this" );
				break;
			case IT_PLANK:	// plank
				strcpy( message, "This is a plank for a boat" );
				break;
			case IT_ESCORTNPCSPAWNER:	// escort npc spawner
				strcpy( message, "Escort NPC spawner, behaves like type 62/69" );
				break;
			case IT_FIREWORKSWAND:	// fireworks wand
				sprintf( message, "A fireworks wand with %u charges left on it", itemToCheck->GetTempVar( CITV_MOREX ) );
				break;
			case IT_GUILDSTONE:	// guildstone deed
				strcpy( message, "Guildstone deed" );
				break;
			case IT_HOUSESIGN:	// opens gump menu
				strcpy( message, "Opens housing gump: " );
				break;
			case IT_PLAYERVENDORDEED:	// player vendor deed
				strcpy( message, "Player vendor deed");
				break;
			case IT_WORLDCHANGEGATE:	// world change gate
				sprintf( message, "World change gate: %u", itemToCheck->GetTempVar( CITV_MOREX ) );
				break;
		}
		s->sysmessage( message );
	}
	else
		s->sysmessage( 1656 );
}

void cTargets::MakeTownAlly( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CChar *targetChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( targetChar ) )
	{
		s->sysmessage( 1110 );
		return;
	}
	UI08 srcTown = mChar->GetTown();
	UI08 trgTown = targetChar->GetTown();

//	if( srcTown == trgTown )
//		s->sysmessage( "That person already belongs to your town" );
//	else
//	{
		if( !regions[srcTown]->MakeAlliedTown( trgTown ) )	
			s->sysmessage( 1111 );
//	}
}

void cTargets::MakeStatusTarget( cSocket *sock )
{
	VALIDATESOCKET( sock );
	CChar *targetChar = calcCharObjFromSer( sock->GetDWord( 7 ) );
	if( !ValidateObject( targetChar ) )
	{
		sock->sysmessage( 1110 );
		return;
	}
	UI08 origCommand			= targetChar->GetCommandLevel();
	commandLevel_st *targLevel	= Commands->GetClearance( sock->XText() );
	commandLevel_st *origLevel	= Commands->GetClearance( origCommand );
	
	if( targLevel == NULL )
	{
		sock->sysmessage( 1112 );
		return;
	}
	CChar *mChar = sock->CurrcharObj();
	char temp[1024], temp2[1024];

	UI08 targetCommand = targLevel->commandLevel;
	sprintf( temp, "account%i.log", mChar->GetAccount().wAccountIndex );
	sprintf( temp2, "%s has made %s a %s.\n", mChar->GetName().c_str(), targetChar->GetName().c_str(), targLevel->name.c_str() );
	Console.Log( temp2, temp );

	DismountCreature( targetChar );

	if( targLevel->targBody != 0 )
	{
		targetChar->SetID( targLevel->targBody );
		targetChar->SetSkin( targLevel->bodyColour );
		targetChar->SetOrgID( targLevel->targBody );
		targetChar->SetOrgSkin( targLevel->bodyColour );
	}

	targetChar->SetPriv( targLevel->defaultPriv );
	targetChar->SetCommandLevel( targetCommand );
	
	if( targLevel->allSkillVals != 0 )
	{
		for( UI08 j = 0; j < ALLSKILLS; ++j )
		{
			targetChar->SetBaseSkill( targLevel->allSkillVals, j );
			targetChar->SetSkill( targLevel->allSkillVals, j );
		}
		targetChar->SetStrength( 100 );
		targetChar->SetDexterity( 100 );
		targetChar->SetIntelligence( 100 );
		targetChar->SetStamina(  100 );
		targetChar->SetHP( 100 );
		targetChar->SetMana( 100 );
	}
	char *playerName = new char[targetChar->GetName().size() + 1];
	strcpy( playerName, targetChar->GetName().c_str() );

	if( targetCommand != 0 && targetCommand != origCommand )
	{
		if( origLevel != NULL )
		{	// Strip name off it
			if( !strnicmp( origLevel->name.c_str(), playerName, strlen( origLevel->name.c_str() ) ) )
				playerName += ( strlen( origLevel->name.c_str() ) + 1 );
		}
		sprintf( temp, "%s %s", targLevel->name.c_str(), playerName );
		targetChar->SetName( temp );
	}
	else if( origCommand != 0 )
	{
		if( origLevel != NULL )
		{	// Strip name off it
			if( !strnicmp( origLevel->name.c_str(), playerName, strlen( origLevel->name.c_str() ) ) )
				playerName += ( strlen( origLevel->name.c_str() ) + 1 );
		}
		strcpy( temp, (const char*)playerName );
		targetChar->SetName( temp );
	}
	CItem *retitem	= NULL;
	CItem *mypack	= targetChar->GetPackItem();

	if( targLevel->stripOff )
	{
		for( CItem *z = targetChar->FirstItem(); !targetChar->FinishedItems(); z = targetChar->NextItem() )
		{
			if( ValidateObject( z ) )
			{
				switch( z->GetLayer() )
				{
					case IL_HAIR:
					case IL_FACIALHAIR:
						z->Delete();
						break;
					case IL_PACKITEM:
					case IL_BANKBOX:
						break;
					default:
						if( !ValidateObject( mypack ) )
							mypack = targetChar->GetPackItem();
						if( !ValidateObject( mypack ) )
						{
							CItem *iMade = Items->CreateItem( NULL, targetChar, 0x0E75, 1, 0, OT_ITEM );
							targetChar->SetPackItem( iMade );
							if( iMade == NULL ) 
								return;
							iMade->SetLayer( IL_PACKITEM );
							if( iMade->SetCont( targetChar ) )
							{
								iMade->SetType( IT_CONTAINER );
								iMade->SetDye( true );
								mypack = iMade;
								retitem = iMade;
							}
						}
						z->SetCont( mypack );
						z->PlaceInPack();
						break;
				}
			}
		}
	}
	delete [] playerName;
}

void cTargets::SmeltTarget( cSocket *s )
{
	VALIDATESOCKET( s );
#if defined( _MSC_VER )
#pragma todo( "Smelting needs to be heavily updated" )
#endif
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) || i->GetCont() == NULL )
		return;
	if( i->GetCreator() == INVALIDSERIAL )
	{
		s->sysmessage( 1113 );
		return;
	}
	UI16 iMadeFrom = i->EntryMadeFrom();

	createEntry *ourCreateEntry = Skills->FindItem( iMadeFrom );
	if( iMadeFrom == 0 || ourCreateEntry == NULL )
	{
		s->sysmessage( 1114 );
		return;
	}

	CChar *mChar = s->CurrcharObj();

	R32 avgMin = ourCreateEntry->AverageMinSkill();
	if( mChar->GetSkill( MINING ) < avgMin )
	{
		s->sysmessage( 1115 );
		return;
	}
	R32 avgMax = ourCreateEntry->AverageMaxSkill();

	Skills->CheckSkill( mChar, MINING, (SI16)avgMin, (SI16)avgMax );

	R32 sumAmountRestored = 0.0f;

	for( UI32 skCtr = 0; skCtr < ourCreateEntry->resourceNeeded.size(); ++skCtr )
	{
		UI16 amtToRestore = ourCreateEntry->resourceNeeded[skCtr].amountNeeded / 2;
		UI16 itemID = ourCreateEntry->resourceNeeded[skCtr].itemID;
		UI16 itemColour = ourCreateEntry->resourceNeeded[skCtr].colour;
		sumAmountRestored += amtToRestore;
		Items->CreateItem( s, mChar, itemID, amtToRestore, itemColour, OT_ITEM, true );
	}

	s->sysmessage( 1116, sumAmountRestored );
	i->Delete();
}

void cTargets::DeleteCharTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( s->CurrcharObj() == c )
	{
		s->sysmessage( 1066 );
		return;
	}
	if( ValidateObject( c ) )
	{
		if( c->IsNpc() )
		{
			s->sysmessage( 1658 );
			return;
		}
		if( isOnline( c ) )
		{
			cSocket *tSock = calcSocketObjFromChar( c );
			if( tSock != NULL )
				tSock->sysmessage( 1659 );
			Network->Disconnect( tSock );
		}
		c->Delete();
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cTargets::PossessTarget( cSocket *s ) 
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Allow a GM to take over the body of another Character
//o---------------------------------------------------------------------------o
void cTargets::PossessTarget( cSocket *s ) 
{
	VALIDATESOCKET( s );
	s->sysmessage( 1635 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cTargets::RemoveShop( cSocket *s )
//|	Programmer	-	Abaddon
//o---------------------------------------------------------------------------o
//|	Purpose		-	Removes shopkeeper layers from a character
//o---------------------------------------------------------------------------o
void cTargets::RemoveShopTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
	{
		s->sysmessage( 366 );
		return;
	}

	i->SetShop( false );
	CItem *buyLayer   = i->GetItemAtLayer( IL_BUYCONTAINER );
	CItem *sellLayer  = i->GetItemAtLayer( IL_BOUGHTCONTAINER );
	CItem *otherLayer = i->GetItemAtLayer( IL_SELLCONTAINER );
	if( ValidateObject( buyLayer ) )
		buyLayer->Delete();
	if( ValidateObject( sellLayer ) )
		sellLayer->Delete();
	if( ValidateObject( otherLayer ) )
		otherLayer->Delete();
	s->sysmessage( 367 );
}

void cTargets::VialTarget( cSocket *mSock )
{
	VALIDATESOCKET( mSock );
	SERIAL targSerial = mSock->GetDWord( 7 );
	if( targSerial == INVALIDSERIAL )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CItem *nVialID = static_cast<CItem *>(mSock->TempObj());
	mSock->TempObj( NULL );
	if( ValidateObject( nVialID ) )
	{
		CItem *nDagger = Combat->getWeapon( mChar );
		if( !ValidateObject( nDagger ) ) 
		{
			mSock->sysmessage( 742 );
			return;
		}
		if( nDagger->GetID() != 0x0F51 && nDagger->GetID() != 0x0F52 )
		{
			mSock->sysmessage( 743 );
			return;
		}

		nVialID->SetTempVar( CITV_MORE, 0, 1 );
		if( targSerial >= BASEITEMSERIAL )
		{	// it's an item
			CItem *targItem = calcItemObjFromSer( targSerial );
			if( !targItem->isCorpse() )
				mSock->sysmessage( 749 );
			else
			{
				nVialID->SetTempVar( CITV_MORE, targItem->GetTempVar( CITV_MORE, 1 ), 1 );
				Karma( mChar, NULL, -1000 );
				if( targItem->GetTempVar( CITV_MORE, 2 ) < 4 )
				{
					mSock->sysmessage( 750 );
					Skills->MakeNecroReg( mSock, nVialID, 0x0E24 );
					targItem->SetTempVar( CITV_MORE, targItem->GetTempVar( CITV_MORE, 2 ) + 1, 2 );
				}
				else 
					mSock->sysmessage( 751 );
			}
		}
		else
		{	// it's a char
			CChar *targChar = calcCharObjFromSer( targSerial );
			if( targChar == mChar )
			{
				if( targChar->GetHP() <= 10 )
					mSock->sysmessage( 744 );
				else
					mSock->sysmessage( 745 );
			}
			else if( objInRange( mChar, targChar, DIST_NEARBY ) )
			{
				if( targChar->IsNpc() )
				{
					if( targChar->GetID( 1 ) == 0x00 && ( targChar->GetID( 2 ) == 0x0C ||
						( targChar->GetID( 2 ) >= 0x3B && targChar->GetID( 2 ) <= 0x3D ) ) )
						nVialID->SetTempVar( CITV_MORE, 1, 1 );
				}
				else
				{
					cSocket *nCharSocket = calcSocketObjFromChar( targChar );
					nCharSocket->sysmessage( 746, mChar->GetName().c_str() );
				}
				if( Combat->WillResultInCriminal( mChar, targChar ) )
					criminal( mChar );
				Karma( mChar, targChar, -targChar->GetKarma() );
			}
			else
			{
				mSock->sysmessage( 747 );
				return;
			}
			targChar->SetHP( targChar->GetHP() - ( RandomNum( 0, 5 ) + 2 ) );
			Skills->MakeNecroReg( mSock, nVialID, 0x0E24 );
		}
	}	
}

//o---------------------------------------------------------------------------o
//|	Function	-	void DoActionTarget( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	NPC Action
//o---------------------------------------------------------------------------o
void cTargets::DoActionTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
		Effects->PlayCharacterAnimation( i, s->AddID1() );
}

}
