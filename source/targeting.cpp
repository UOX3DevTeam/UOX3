#include "uox3.h"
#include "debug.h"
#include "cmdtable.h"
#include "cdice.h"
#include "ssection.h"
#include "mstring.h"

#undef DBGFILE
#define DBGFILE "targeting.cpp"

void cTargets::PlVBuy( cSocket *s )//PlayerVendors
{
	if( s == NULL )
		return;

	CHARACTER v = s->AddX();
	if( v == INVALIDSERIAL || v > cmem ) 
		return;
	if( chars[v].isFree() ) 
		return;

	CChar *mChar = s->CurrcharObj();
	SI32 gleft = calcGold( mChar );

	CItem *p = getPack( mChar );
	if( p == NULL ) 
	{
		sysmessage( s, 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL ) 
		return;	
	if( i->GetCont() == INVALIDSERIAL ) 
		return;
	
	CItem *np = calcItemObjFromSer( i->GetCont() );
	CChar *npc = getPackOwner( np );
	if( npc != &chars[v] || chars[v].GetNPCAiType() != 17 ) 
		return;
	
	if( mChar->GetSerial() == chars[v].GetOwner() )
	{
		npcTalk( s, &chars[v], 999, false );
		return;
	}

	if( gleft < i->GetValue() ) 
	{
		npcTalk( s, &chars[v], 1000, false );
		return;
	} 
	else 
	{
		SI32 tAmount = deleQuan( mChar, 0x0EED, i->GetValue() );
		// tAmount > 0 indicates there wasn't enough money...
		// could be expanded to take money from bank too...
	}
	
	npcTalk( s, &chars[v], 1001, false );
	chars[v].SetHoldG( chars[v].GetHoldG() + i->GetValue() );

	i->SetCont( p->GetCont() );	// move containers
	RefreshItem( i );
	Weight->AddItemWeight( i, mChar );
	statwindow( s, mChar );
}

void cTargets::HandleGuildTarget( cSocket *s )
{
	CChar *trgChar;
	CChar *mChar = s->CurrcharObj();
	CGuild *mGuild = NULL, *tGuild = NULL;
	switch( s->GetByte( 5 ) )
	{
	case 0:	// recruit character
		trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
		if( trgChar != NULL )
		{
			if( trgChar->GetGuildNumber() == -1 )	// no existing guild
			{
				mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
				if( mGuild != NULL )
					mGuild->NewRecruit( (*trgChar) );
			}
			else
				sysmessage( s, 1002 );
		}
		break;
	case 1:		// declare fealty
		trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
		if( trgChar != NULL )
		{
			if( trgChar->GetGuildNumber() == mChar->GetGuildNumber() )	// same guild
				mChar->SetGuildFealty( trgChar->GetSerial() );
			else
				sysmessage( s, 1003 );
		}
		break;
	case 2:	// declare war
		trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
		if( trgChar != NULL )
		{
			if( trgChar->GetGuildNumber() != mChar->GetGuildNumber() )
			{
				if( trgChar->GetGuildNumber() == -1 )
					sysmessage( s, 1004 );
				else
				{
					mGuild = GuildSys->Guild(mChar->GetGuildNumber() );
					if( mGuild != NULL )
					{
						mGuild->SetGuildRelation( trgChar->GetGuildNumber(), GR_WAR );
						tGuild = GuildSys->Guild( trgChar->GetGuildNumber() );
						if( tGuild != NULL )
							tGuild->TellMembers( 1005, mGuild->Name() );
					}
				}
			}
			else
				sysmessage( s, 1006 );
		}
		break;
	case 3:	// declare ally
		trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
		if( trgChar != NULL )
		{
			if( trgChar->GetGuildNumber() != mChar->GetGuildNumber() )
			{
				if( trgChar->GetGuildNumber() == -1 )
					sysmessage( s, 1004 );
				else
				{
					mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
					if( mGuild != NULL )
					{
						mGuild->SetGuildRelation( trgChar->GetGuildNumber(), GR_ALLY );
						tGuild = GuildSys->Guild( trgChar->GetGuildNumber() );
						if( tGuild != NULL )
							tGuild->TellMembers( 1007, mGuild->Name() );
					}
				}
			}
			else
				sysmessage( s, 1006 );
		}
		break;
	}

}

void cTargets::MultiTarget( cSocket *s ) // If player clicks on something with the targetting cursor
{
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
	UI08 a4 = s->GetByte( 5 );
	s->TargetOK( false );
	if( mChar->IsDead() && !mChar->IsGM() && mChar->GetAccount().wAccountIndex != 0 )
	{
		sysmessage( s, 1008 );
		if( mChar->GetSpellCast() != -1 )	// need to stop casting if we don't target right
			mChar->StopSpell();
		return;
	}
	if( a1 == 0 && a2 == 1 )
	{
		if( a3 == 2 )	// GUILDS!!!!!
		{
			HandleGuildTarget( s );
			return;
		}
		if( a3 == 1 ) // CustomTarget() !!!!
		{
			UI16 scriptNum = (UI16)s->AddX();
			cScript *tScript = Trigger->GetScript( scriptNum );
			if( tScript != NULL )
				tScript->DoCallback( s, s->GetDWord( 7 ), a4 );
			return;
		}
 		if( a3 == 0 )
		{
			CItem *targetItem;
			switch( a4 )
			{
			case 0:		AddTarget( s );						break;
			case 1:		RenameTarget( s );					break;
			case 2:		TeleTarget( s );					break;	
			case 3:		RemoveTarget( s );					break;
			case 4:		DyeTarget( s );						break;
			case 5:		NewzTarget( s );					break;
			case 6:		TypeTarget( s );					break;
			case 7:		IDtarget( s );						break;
			case 8:		XgoTarget( s );						break;
			case 9:		PrivTarget( s );					break;
			case 10:	MoreTarget( s );					break;
			case 11:	KeyTarget( s );						break;
			case 12:	IstatsTarget( s );					break;
			case 13:	CstatsTarget( s );					break;
			case 14:	Skills->RepairLeather( s );			break;
			case 15:	Skills->RepairBow( s );				break;
			case 16:	KillTarget( s, 0x0B );				break;
			case 17:	KillTarget( s, 0x10 );				break;
			case 18:	KillTarget( s, 0x15 );				break;
			case 19:	FontTarget( s );					break;
			case 20:	GhostTarget( s );					break;
			case 21:	ResurrectionTarget( s ); break; // needed for /resurrect command
			case 22:	BoltTarget( s );					break;
			case 23:	AmountTarget( s );					break;
			case 24:	Skills->RepairMetal( s );			break;
			case 25:	CloseTarget( s );					break;
			case 26:	AddMenuTarget( s, true, s->XText() ); break;
			case 27:	NpcMenuTarget( s );					break;
			case 28:	MovableTarget( s );					break;
			case 29:	Skills->ArmsLoreTarget( s );		break;
			case 30:	OwnerTarget( s );					break;
			case 31:	ColorsTarget( s );					break;
			case 32:	DvatTarget( s );					break;
			case 33:	AddNpcTarget( s );					break;
			case 34:	FreezeTarget( s );					break;
			case 35:	UnfreezeTarget( s );				break;
			case 36:	AllSetTarget( s );					break;
			case 37:	Skills->AnatomyTarget( s );			break;
			case 38:	Magic->Recall( s );					break;					
			case 39:	Magic->Mark( s );					break;					
			case 40:	Skills->ItemIDTarget( s );			break;
			case 41:	Skills->EvaluateIntTarget( s );		break;
			case 42:	Skills->TameTarget( s );			break;
			case 43:	Magic->Gate( s );					break;					
			case 44:	Magic->Heal( s );					break; // we need this for /heal command
			case 45:	Skills->FishTarget( s );			break;
			case 46:	InfoTarget( s );					break;
			case 47:	TitleTarget( s );					break;
			case 48:	ShowDetail( s );					break;
			case 49:	MakeTownAlly( s );					break;
			case 50:	Skills->Smith( s );					break;
			case 51:	Skills->Mine( s );					break;
			case 52:	Skills->SmeltOre( s );				break;
			case 53:	npcAct( s );						break;
//			case 54:	GetAccount( s );					break;
			case 55:	WstatsTarget( s );					break;
			case 56:	NpcTarget( s );						break;
			case 57:	NpcTarget2( s );					break;
			case 58:	NpcResurrectTarget( mChar );		break;
			case 59:	NpcCircleTarget( s );				break;
			case 60:	NpcWanderTarget( s );				break;
			case 61:	VisibleTarget( s );					break;
			case 62:	TweakTarget( s );					break;
			case 63:	MoreXTarget( s );					break;
			case 64:	MoreYTarget( s );					break;
			case 65:	MoreZTarget( s );					break;
			case 66:	MoreXYZTarget( s );					break;
			case 67:	NpcRectTarget( s );					break;
			case 68: 
				targetItem = calcItemObjFromSer( s->GetDWord( 7 ) );
				if( targetItem != NULL )
				{
					sysmessage( s, 1010 );
					targetItem->enhanced ^= 0x0001;	// Set DevineLock bit in items
				}
				break;
			case 69: 
				targetItem = calcItemObjFromSer( s->GetDWord( 7 ) );
				if( targetItem != NULL )
				{
					sysmessage( s, 1011 );
					targetItem->enhanced &= 0xFFFE;	// ReSet DevineLock bit in items
				}
				break;
			case 70: MakeStatusTarget( s );				break;
			case 71: 
				targetItem = calcItemObjFromSer( s->GetDWord( 7 ) );
				if( targetItem != NULL )
				{
					sysmessage( s, 1652 );
					targetItem->SetScriptTrigger( (UI16)s->AddX() );
				}
				else
				{
					CChar *targChar = calcCharObjFromSer( s->GetDWord( 7 ) );
					if( targChar != NULL )
					{
						targChar->SetScriptTrigger( (UI16)s->AddX() );
						sysmessage( s, 1653 );
					}
				}
				break;
			case 72: DeleteCharTarget( s );				break;
	// FREE		case 73: break;
	// FREE		case 74: break;
	// FREE		case 75: break;
			case 76: AxeTarget( s );					break;
			case 77: Skills->DetectHidden( s );			break;
	// FREE		case 78: break;
			case 79: Skills->ProvocationTarget1( s );	break;
			case 80: Skills->ProvocationTarget2( s );	break;
			case 81: Skills->EnticementTarget1( s );	break;
			case 82: Skills->EnticementTarget2( s );	break;
	// FREE		case 83: break;
	// FREE		case 84: break;
	// FREE		case 85: break;
			case 86: SwordTarget( s );					break;
			case 87:									break;
			case 88: SetDirTarget( s );					break;
			case 89: ObjPrivTarget( s );				break;
			case 90: AreaCommand( s );					break;
	// FREE		case 91: break;
	// FREE		case 92: break;
	// FREE		case 93: break;
	// FREE		case 94: break;
	// FREE		case 95: break;
	// FREE		case 96: break;
	// FREE		case 97: break;
	// FREE		case 98: break;
	// FREE		case 99: break;
	// FREE		case 100: break;
			case 100: Magic->CastSpell( s, mChar ); break;
	// FREE		case 101: break;
	// FREE		case 102: break;
	// FREE		case 103: break;
			case 104: 			
				if( mChar->GetAccount().wAccountIndex == 0 )
					 CommandLevel( s ); 
				else
					sysmessage( s, 1009 );
					break;

			case 105: Commands->RemoveShop( s );		break;
			case 106: NpcAITarget( s );					break;
			case 107: xBankTarget( s );					break;
			case 108: Skills->AlchemyTarget( s );		break;
			case 109: Skills->BottleTarget( s );		break;
			case 110: DupeTarget( s );					break;
			case 111: MoveToBagTarget( s );				break;
			case 112: SellStuffTarget( s );				break;
			case 113: ManaTarget( s );					break;
			case 114: StaminaTarget( s );				break;
			case 115: GmOpenTarget( s );				break;
			case 116: MakeShopTarget( s );				break;
			case 117: FollowTarget( s );				break;
			case 118: AttackTarget( s );				break;
			case 119: TransferTarget( s );				break;
			case 120: GuardTarget( s );					break;
			case 121: BuyShopTarget( s );				break;
			case 122: SetValueTarget( s );				break;
			case 123: SetRestockTarget( s );			break;
			case 124: FriendTarget( s );				break;
	// FREE		case 125: break;
			case 126: JailTarget( s, INVALIDSERIAL );	break;
			case 127: ReleaseTarget( s, INVALIDSERIAL );	break;
			case 128: Skills->CreateBandageTarget( s );	break;
	// FREE	case 129:									break;
			case 130: Skills->HealingSkillTarget( s );	break;
			case 131: permHideTarget( s );				break;
			case 132: unHideTarget( s );				break;
			case 133: SetWipeTarget( s );				break;
			case 134: Skills->Carpentry( s );			break;
			case 135:									break;
			case 136: XTeleport( s, 0 );				break;
	// FREE		case 137: break;
	// FREE		case 138: break;
	// FREE		case 139: break;
	// FREE		case 140: break;
	// FREE		case 141: break;
	// FREE		case 142: break;
	// FREE		case 143: break;
	// FREE		case 144: break;
	// FREE		case 145: break;
	// FREE		case 146: break;
	// FREE		case 147: break;
	// FREE		case 148: break;
	// FREE		case 149: break;
			case 150: SetSpAttackTarget( s );			break;
			case 151: FullStatsTarget( s );				break;
			case 152: Skills->BeggingTarget( s );		break;
			case 153: Skills->AnimalLoreTarget( s );	break;
			case 154: Skills->ForensicsTarget( s );		break;
			case 155: 
				{
					mChar->SetPoisonSerial( s->GetDWord( 7 ) );
					target( s, 0, 1, 0, 156, 1613 );
					return;
				}
			case 156: Skills->PoisoningTarget( s );		break;
	// FREE		case 157: break;
	// FREE		case 158: break;
	// FREE		case 159: break;
			case 160: Skills->Inscribe( s, 0 );			break;
			case 161: region[mChar->GetTown()]->VoteForMayor( s ); break; 
			case 162: Skills->LockPick( s );			break;
	// FREE		case 163: break;
			case 164: Skills->Wheel( s );				break;
			case 165: Skills->Loom( s );				break;
	// FREE		case 166: break;
			case 167: Skills->Tailoring( s );			break;
			case 168: Skills->CookMeat( s );			break;
	//FREE		case 169: break;
			case 170: LoadCannon( s );					break;
	//FREE		case 171: break;
			case 172: Skills->Fletching( s );			break;
			case 173: Skills->MakeDough( s );			break;
			case 174: Skills->MakePizza( s );			break;
			case 175: SetPoisonTarget( s );				break;
			case 176: SetPoisonedTarget( s );			break;
			case 177: SetSpaDelayTarget( s );			break;
			case 178: SetAdvObjTarget( s );				break;
			case 179: SetInvulFlag( s );				break;
			case 180: Skills->Tinkering( s );			break;
			case 181: Skills->PoisoningTarget( s );		break;  
	//FREE		case 182: break;
			case 183: Skills->TinkerAxel( s );			break;
			case 184: Skills->TinkerAwg( s );			break;
			case 185: Skills->TinkerClock( s );			break;
			case 186: vialtarget( s );					break;
	//FREE		case 187: break;
	//FREE		case 188: break;
	//FREE		case 189: break;
	//FREE		case 190: break;
	//FREE		case 191: break;
	//FREE		case 192: break;
	//FREE		case 193: break;
	//FREE		case 194: break;
	//FREE		case 195: break;
	//FREE		case 196: break;
	//FREE		case 197: break;
			case 198: Tiling( s );						break;
			case 199: Wiping( s );						break;
			case 200:									break;
			case 201:									break;
			case 202:									break;
			case 203:									break;
	//		case 204: triggertarget(s);					break;
			case 205: Skills->StealingTarget( s );		break;
			case 206: CanTrainTarget( s );				break;
			case 207: ExpPotionTarget( s );				break;
			case 209: SetSplitTarget( s );				break;
			case 210: SetSplitChanceTarget( s );		break;
			case 212: Commands->Possess( s );			break;
	// FREE		case 213: break;
	// FREE		case 214: break;
	// FREE		case 215: break;
	// FREE		case 216: break;
	// FREE		case 217: break;
	// FREE		case 218: break;
	// FREE		case 219: break;
//			case 220: Guilds->Recruit( s );				break;
//			case 221: Guilds->TargetWar( s );			break;
			case 222: TeleStuff( s );					break;        
			case 223: SquelchTarg( s );					break;//Squelch
			case 224: PlVBuy( s );						break;//PlayerVendors
			
	//		case 225: Priv3XTarget(s); break; // SETPRIV3 +/- target
			case 227: HouseOwnerTarget( s );			break;
			case 228: HouseEjectTarget( s );			break;
			case 229: HouseBanTarget( s );				break;
			case 230: HouseFriendTarget( s );			break;
			case 231: HouseUnlistTarget( s );			break;
			case 232: HouseLockdown( s );				break; // Abaddon 17th December 1999
			case 233: HouseRelease( s );				break; // Abaddon 17th December 1999
	// FREE		case 234: break;
			
			case 235: BanTarg( s );						break;
	// FREE		case 236: break;
			case 237: SmeltTarget( s );					break;
	// FREE		case 238: break;
	// FREE		case 239: break;
	// FREE		case 240: break;
	// FREE		case 241: break;
	// FREE		case 242: break;
	// FREE		case 243: break;
	// FREE		case 244: break;
	// FREE		case 245: break;
	// FREE		case 246: break;
			case 247: ShowSkillTarget( s );				break;
	//		case 248: break;
			case 249: UnglowTarget( s );				break;

			case 250: IncZTarget( s );					break;
			case 251: NewXTarget( s );					break;
			case 252: NewYTarget( s );					break;
			case 253: IncXTarget( s );					break;
			case 254: IncYTarget( s );					break;
			case 255: GlowTarget( s );					break;
				
			default:
				Console.Error( 2, " Fallout of switch statement (%i) without default. targeting.cpp, multitarget()", a4 );
			}
		}
	}
}

void cTargets::BanTarg( cSocket *s )
// PARAM WARNING: s is an unreferenced parameter
{

}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddTarget( cSocket *s)
//|   Date        -  UnKnown
//|   Programmer  -  UnKnown  (Touched tabstops by Tauriel Dec 29, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds an item when using /add # # .
//o---------------------------------------------------------------------------o
void cTargets::AddTarget( cSocket *s )
{
	if( s == NULL || s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	bool pileable = false;
	CTile tile;
	
	if( s->AddID1() == 0x40 )
	{
		switch( s->AddID2() )
		{
		case 100:
		case 102:
		case 104:
		case 106:
		case 108:
		case 110:
		case 112:
		case 114:
		case 116:
		case 118:
		case 120:
		case 122:
		case 124:
		case 126:
		case 140:
			buildHouse( s, s->AddID3() );//If its a valid house, send it to buildhouse!
			return; // Morrolan, here we WANT fall-thru, don't mess with this switch
		}
	}
	UI16 modelNum = (UI16)(( ( s->AddID1() )<<8 ) + s->AddID2());
	Map->SeekTile( modelNum, &tile );
	if( tile.Stackable() ) 
		pileable = true;
	
	CItem *c = Items->SpawnItem( s, 1, "#", pileable, modelNum, 0, false, false );
	if( c == NULL ) 
		return;
	c->SetPriv( 0 );	//Make them not decay
	const SI16 tX = s->GetWord( 11 );
	const SI16 tY = s->GetWord( 13 );
	const SI08 tZ = (SI08)(s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ));
	
	if( c->GetCont() == INVALIDSERIAL )
		c->SetLocation( tX, tY, tZ );
	else
	{
		c->SetX( tX );
		c->SetY( tY );
		c->SetZ( tZ );
	}
	RefreshItem( c );
	s->AddID1( 0 );
	s->AddID2( 0 );
}

void cTargets::RenameTarget( cSocket *s )
{
	if( s == NULL )
		return;
	SERIAL serial = s->GetDWord( 7 );
	if( serial >= 0x40000000 )
	{
		CItem *i = calcItemObjFromSer( serial );
		if( i != NULL )
		{
			if( s->AddX() == 1 ) //rename2
				i->SetName2( s->XText() );
			else
				i->SetName( s->XText() );
		}
	}
	else
	{
		CChar *c = calcCharObjFromSer( serial );
		if( c != NULL )
		{
			c->SetName( s->XText() );
			statwindow( s, c );
		}
	}
}

void cTargets::TeleTarget( cSocket *s )
{
	if( s == NULL || s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	SERIAL serial = s->GetDWord( 7 );

	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		sysmessage( s, 717 );
		return;
	}

	SI16 targX, targY;
	SI08 targZ;
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )
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
		
		soundeffect( s, 0x01FE, true );
		
		mChar->SetLocation( targX, targY, targZ );
		mChar->Teleport();
		staticeffect( mChar, 0x372A, 0x09, 0x06 );
	} 
}

void cTargets::RemoveTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		sysmessage( s, 1013 );
		Items->DeleItem( i );
    } 
	else 
	{
		CChar *c = calcCharObjFromSer( serial );
		if( c == NULL )
			return;
		if( c->GetAccount().wAccountIndex != 0xffff && !c->IsNpc() )
		{ 
			sysmessage( s, 1014 );
			return;
		}
		sysmessage( s, 1015 );
		Npcs->DeleteChar( c );
	}
}

void cTargets::DyeTarget( cSocket *s )
{
	int b;
	UI16 colour, k;
	CItem *i = NULL;
	CChar *c = NULL;
	SERIAL serial = s->GetDWord( 7 );
	if( s->AddID1() == 0xFF && s->AddID2() == 0xFF )
	{
		CPDyeVat toSend;
		if( serial >= 0x40000000 )
		{
			i = calcItemObjFromSer( serial );
			if( i != NULL )
			{
				toSend = (*i);
				s->Send( &toSend );
			}
		}
		else
		{
			c = calcCharObjFromSer( serial );
			if( c != NULL )
			{
				toSend = (*c);
				toSend.Model( 0x2106 );
				s->Send( &toSend );
			}
		}
	}
	else
	{
		if( serial >= 0x40000000 )
		{
			i = calcItemObjFromSer( serial );
			if( i == NULL )
				return;
			colour = (UI16)(( (s->AddID1())<<8 ) + s->AddID2());
			if( !s->DyeAll() )
			{
				if( colour < 0x0002 || colour > 0x03E9 )
					colour = 0x03E9;
			}
			
			b = ((colour&0x4000)>>14) + ((colour&0x8000)>>15);   
			if( !b )
			{
				i->SetColour( colour );
				RefreshItem( i );
			}
		}
		else
		{
			c = calcCharObjFromSer( serial );
			if( c == NULL )
				return;
			UI16 body = c->GetID();
			k = (UI16)(( ( s->AddID1() )<<8 ) + s->AddID2());
			
			b = k&0x4000; 
			if( b == 16384 && ( body >= 0x0190 && body <= 0x03E1 ) ) 
				k = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.
			
			if( k != 0x8000 ) // 0x8000 also crashes client ...
			{	
				c->SetSkin( k );
				c->SetxSkin( k );
				c->Teleport();
			}
		}
	}
}

void cTargets::NewzTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		i->SetZ( s->AddX() );
		RefreshItem( i );
		return;
	}

	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )
	{
		c->SetDispZ( s->AddX() );
		c->SetZ( s->AddX() );
		c->Teleport();
	}
}

void cTargets::TypeTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetType( s->AddID1() );
}


void cTargets::IDtarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		i->SetID( (UI16)(( (s->AddID1())<<8) + s->AddID2()) );
		RefreshItem( i );
		return;
	}

	CChar *c = calcCharObjFromSer( serial );
	if( c == NULL )
		return;
	c->SetID( ( (s->AddID1())<<8) + s->AddID2() );
	c->SetxID( c->GetID() );
	c->SetOrgID( c->GetID() );
	c->Teleport();
}

void cTargets::XTeleport( cSocket *s, UI08 x )
{
	SERIAL serial = INVALIDSERIAL;
	CChar *c = NULL;
	// Abaddon 17th February 2000 Converted if to switch (easier to read)
	// Also made it so that it calls teleport, not updatechar, else you don't get the items in range
	CChar *mChar = s->CurrcharObj();
	switch( x )
	{
	case 0:
		serial = s->GetDWord( 7 );
		c = calcCharObjFromSer( serial );
		break;
	case 5:
		serial = calcserial( (UI08)makenumber( 1 ), (UI08)makenumber( 2 ), (UI08)makenumber( 3 ), (UI08)makenumber( 4 ) );
		c = calcCharObjFromSer( serial );
		break;
	case 2:
		cSocket *mSock;
		mSock = calcSocketObjFromSock( makenumber(1) );
		if( mSock != NULL )
			c = mSock->CurrcharObj();
		else 
			return;
		break;
	default:
		sysmessage( s, 1654, x );
		return;
	}
	
	if( c != NULL )
	{
		c->SetLocation( mChar );
		c->Teleport();
		return;
	}

	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		i->SetLocation( mChar );
		RefreshItem( i );
	}
}

void cTargets::XgoTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;

	i->SetLocation( s->AddX(), s->AddY(), s->AddZ() );
	i->Teleport();
}

void cTargets::MoreXYZTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;

	i->SetMoreX( s->AddX() );
	i->SetMoreY( s->AddY() );
	i->SetMoreZ( s->AddZ() );
}

void cTargets::MoreXTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetMoreX( s->AddX() );
}

void cTargets::MoreYTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetMoreY( s->AddX() );
}

void cTargets::MoreZTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetMoreZ( s->AddX() );
}

void cTargets::PrivTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;

	CChar *mChar = s->CurrcharObj();
	char temp[1024], temp2[1024];
	sprintf( temp, "%s/logs/%s.log", cwmWorldState->ServerData()->GetRootDirectory(), mChar->GetName() );
	sprintf( temp2, "%s as given %s Priv [%x][%x]\n", mChar->GetName(), i->GetName(), s->AddID1(), s->AddID2() );
	i->SetPriv( s->AddID1() );
	i->SetPriv2( s->AddID2() );
	savelog( temp2, temp );
}

void cTargets::MoreTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		i->SetMore( s->AddID() );
		RefreshItem( i );
	}
}

void cTargets::KeyTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;

	CChar *mChar = s->CurrcharObj();
	SERIAL moreSerial = s->AddID();
	if( i->GetMore() == 0 )
	{       
		if( i->GetType() == 7 && itemInRange( mChar, i, 2 ) )
		{
			if( !Skills->CheckSkill( mChar, TINKERING, 400, 1000 ) ) 
			{
				sysmessage( s, 1016 );
				Items->DeleItem( i );
				return;
			}
			i->SetMore( moreSerial );
			sysmessage( s, 1017 );
			return;
		}
	}
	else if( i->GetMore() == moreSerial || s->AddID1() == 0xFF )
	{
		if( ( i->GetType() == 1 || i->GetType() == 63 ) && itemInRange( mChar, i, 2 ) )
		{
			if( i->GetType() == 1 ) 
				i->SetType( 8 );
			else if( i->GetType() == 63 ) 
				i->SetType( 64 );
			sysmessage( s, 1018 );
			return;
		}
		if( i->GetType() == 7 && itemInRange( mChar, i, 2 ) )
		{
			mChar->SetSpeechItem( i->GetSerial() );
			mChar->SetSpeechMode( 5 );
			sysmessage( s, 1019 );
			return;
		}
		if( ( i->GetType() == 8 || i->GetType() == 64 ) && itemInRange( mChar, i, 2 ) )
		{
			if( i->GetType() == 8 ) 
				i->SetType( 1 );
			if( i->GetType() == 64 ) 
				i->SetType( 63 );
			sysmessage( s, 1020 );
			return;
		}
		if( i->GetType() == 12 && itemInRange( mChar, i, 2 ) )
		{
			i->SetType( 13 );
			sysmessage( s, 1021 );
			soundeffect( s, 0x0049, true );
			return;
		}
		if( i->GetType() == 13 && itemInRange( mChar, i, 2 ) )
		{
			i->SetType( 12 );
			sysmessage( s, 1022 );
			soundeffect( s, 0x0049, true );
			return;
		}
		if( i->GetID() == 0x0BD2 )
		{
			sysmessage( s, 1023 );
			mChar->SetSpeechMode( 8 );
			mChar->SetSpeechItem( i->GetSerial() );
			return;
		}
		if( i->GetType() == 117 )
		{
			Boats->OpenPlank( i );
			RefreshItem( i );
		}
	}
	else
	{
		if( i->GetType() == 7 ) 
			sysmessage( s, 1024 );
		else if( i->GetMore( 1 ) == 0 ) 
			sysmessage( s, 1025 );
		else 
			sysmessage( s, 1026 );
	}
}


void cTargets::IstatsTarget( cSocket *s )
{
	CTile tile;
	if( s->GetDWord( 7 ) == 0 )
	{
		UI08 worldNumber = 0;
		if( s->CurrcharObj() != NULL )
			worldNumber = s->CurrcharObj()->WorldNumber();
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
		{  // manually calculating the ID's if a maptype
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
		if( i == NULL )
			return;
		GumpDisplay dynamicStat( s, 300, 300 );
		dynamicStat.SetTitle( "Item [Dynamic]" );

		SERIAL itemSerial = i->GetSerial();
		SERIAL contSerial = i->GetCont();
		SERIAL moreVal = i->GetMore();
		SERIAL ownerSerial = i->GetOwner();
		UI16 itemID = i->GetID();
		UI16 itemColour = i->GetColour();
		int hpStuff = ((i->GetHP())<<8) + i->GetMaxHP();
		int itemDamage = ((i->GetLoDamage())<<8) + i->GetHiDamage();
		long int decayTime = int(R64(int(i->GetDecayTime()-uiCurrentTime)/CLOCKS_PER_SEC));

		dynamicStat.AddData( "Serial", itemSerial, 3 );
		dynamicStat.AddData( "ID", itemID, 5 );
		dynamicStat.AddData( "Name", i->GetName() );
		dynamicStat.AddData( "Name2", i->GetName2() );
		dynamicStat.AddData( "Colour", itemColour, 5 );
		dynamicStat.AddData( "Cont Serial", contSerial, 3 );
		dynamicStat.AddData( "Layer", i->GetLayer() );
		dynamicStat.AddData( "Type", i->GetType() );
		dynamicStat.AddData( "Moveable", i->GetMagic() );
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
		dynamicStat.AddData( "More X", i->GetMoreX() );
		dynamicStat.AddData( "More Y", i->GetMoreY() );
		dynamicStat.AddData( "More Z", i->GetMoreZ() );
		dynamicStat.AddData( "Poisoned", i->GetPoisoned() );
		dynamicStat.AddData( "Weight", i->GetWeight() );
		dynamicStat.AddData( "Creator", i->GetCreator() );
		dynamicStat.AddData( "Madewith", i->GetMadeWith() );
		dynamicStat.AddData( "DecayTime", decayTime );
		dynamicStat.AddData( "Decay", i->isDecayable()?1:0 );
		dynamicStat.AddData( "Good", i->GetGood() );
		dynamicStat.AddData( "RandomValueRate", i->GetRndValueRate() );
		dynamicStat.AddData( "Value", i->GetValue() );
		dynamicStat.AddData( "Carve", i->GetCarve() );
		dynamicStat.Send( 4, false, INVALIDSERIAL );
	}
}

void cTargets::CstatsTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	GumpDisplay charStat( s, 300, 300 );
	charStat.SetTitle( "Character" );
	UI16 charID = i->GetID();
	UI16 charSkin = i->GetSkin();
	int charPrivs = (i->GetPriv()<<8) + i->GetPriv2();
	SERIAL charSerial = i->GetSerial();

	ACCOUNTSBLOCK actbTemp;
	actbTemp=i->GetAccount();

	charStat.AddData( "Serial", charSerial, 3 );
	charStat.AddData( "Body Type", charID, 5 );
	charStat.AddData( "Name", i->GetName() );
	charStat.AddData( "Skin", charSkin, 5 );
	charStat.AddData( "Account", actbTemp.wAccountIndex );
	charStat.AddData( "Privs", charPrivs, 5 );
	charStat.AddData( "Strength", i->GetStrength() );
	charStat.AddData( "Dexterity", i->GetDexterity() );
	charStat.AddData( "Intelligence", i->GetIntelligence() );
	charStat.AddData( "Mana", i->GetMana() );
	charStat.AddData( "Hitpoints", i->GetHP() );
	charStat.AddData( "Stamina", i->GetStamina() );
	charStat.AddData( "X coord", i->GetX() );
	charStat.AddData( "Y coord", i->GetY() );
	charStat.AddData( "Z coord", i->GetZ() );
	charStat.AddData( "Timeout", i->GetTimeout() );
	charStat.AddData( "Fame", i->GetFame() );
	charStat.AddData( "Karma", i->GetKarma() );
	charStat.AddData( "Deaths", i->GetDeaths() );
	charStat.AddData( "Kills", i->GetKills() );
	charStat.AddData( "AI Type", i->GetNPCAiType()  );
	charStat.AddData( "NPC Wander", i->GetNpcWander() );
	charStat.AddData( "Weight", i->GetWeight() );
	charStat.AddData( "Poisoned", i->GetPoisoned() );
	charStat.AddData( "Poison", i->GetPoison() );
	charStat.AddData( "Hunger", i->GetHunger() );
	charStat.AddData( "Attacker", i->GetAttacker() );
	charStat.AddData( "Target", i->GetTarg() );
	charStat.AddData( "Carve", i->GetCarve() );
	charStat.AddData( "Race", i->GetRace() );
	charStat.AddData( "RaceGate", i->GetRaceGate() );
	charStat.AddData( "CommandLevel", i->GetCommandLevel() );
	// 
	if( actbTemp.wAccountIndex != 0xffff )
		charStat.AddData( "Last On", i->GetLastOn() );
	//
	charStat.Send( 4, false, INVALIDSERIAL );
	
	Gumps->Open( s, i, 8 );
	statwindow( s, i );
}

void cTargets::WstatsTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
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
	wStat.AddData( "FX1", i->GetFx( 1 ) );
	wStat.AddData( "FY1", i->GetFy( 1 ) );
	wStat.AddData( "FZ1", i->GetFz() );
	wStat.AddData( "FX2", i->GetFx( 2 ) );
	wStat.AddData( "FY2", i->GetFy( 2 ) );
	wStat.Send( 4, false, INVALIDSERIAL );
}

void cTargets::KillTarget( cSocket *s, UI08 layer )
{
	CChar *k = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( k != NULL )
	{
		CItem *i = FindItemOnLayer( k, layer );
		if( i != NULL )
			Items->DeleItem( i );
	}
}

void cTargets::FontTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetFontType( s->AddID1() );
}


void cTargets::GhostTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	if( !i->IsDead() )
	{
		i->SetAttacker( s->Currchar() );
		bolteffect( i );
		soundeffect( i, 0x0029 );
		doDeathStuff( i );
	} 
	else 
		sysmessage( s, 1028 );
}


void cTargets::BoltTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		bolteffect( i );
		soundeffect( i, 0x0029 );
	}
}

void cTargets::AmountTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		i->SetAmount( s->AddX() );
		RefreshItem( i );
	}
}

void cTargets::CloseTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )	
		return;
	cSocket *j = calcSocketObjFromChar( i );
	if( j != NULL )
	{
		sysmessage( s, 1029 );
		sysmessage( j, 1030 );
		Network->Disconnect( calcSocketFromSockObj( j ) );
	}
}

// sockets
CItem * cTargets::AddMenuTarget( cSocket *s, bool x, char *addmitem ) //Tauriel 11-22-98 updated for new items
{
	if( s == NULL )
		return NULL;

	if( s->GetDWord( 11 ) == INVALIDSERIAL ) 
		return NULL;

	CChar *mChar = s->CurrcharObj();
	CItem *c = Items->CreateScriptItem( s, addmitem, false, mChar->WorldNumber() );
	if( c == NULL ) 
		return NULL;

	if( x )
		RefreshItem( c );

	return c;
}

CChar *cTargets::NpcMenuTarget( cSocket *s )
// Abaddon 17th February, 2000
// Need to return the character we've made, else summon creature at least will fail
// We make the char, but never pass it back up the chain
{
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return NULL;
	
	char *x = s->XText();
	CChar *mChar = s->CurrcharObj();
	return Npcs->AddNPC( s, NULL, x, mChar->WorldNumber() );
}

void cTargets::MovableTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		i->SetMagic( s->AddX() );
		RefreshItem( i );
	}
}

void cTargets::VisibleTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	if( s->GetByte( 7 ) >= 0x40 )
	{
		CItem *i = calcItemObjFromSer( serial );
		if( i != NULL )
		{
			i->SetVisible( s->AddX() );
			RefreshItem( i );
		}
	} 
	else 
	{
		CChar *c = calcCharObjFromSer( serial );
		if( c != NULL )
		{
			c->SetHidden( s->AddX() );
			c->Update();
		}
	}
}

void cTargets::OwnerTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CChar *i = calcCharObjFromSer( serial );
	if( i != NULL )
		i->SetOwner( s->AddID() );
	
	CItem *c = calcItemObjFromSer( serial );
	if( c != NULL )
		c->SetOwner( s->AddID() );
}

void cTargets::ColorsTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	if( i->GetID() == 0x0FAB || i->GetID() == 0x0EFF || i->GetID() == 0x0E27 )	// dye vat, hair dye
	{
		CPDyeVat toSend = (*i);
		s->Send( &toSend );
	}
	else
		sysmessage( s, 1031 );
}

void cTargets::DvatTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i == NULL )
		return;

	CChar *mChar = s->CurrcharObj();
	if( i->isDyeable() )
	{
		if( i->GetCont() != INVALIDSERIAL )
		{
			CChar *c = getPackOwner( i );
			if( c != NULL && c != mChar )
			{
				sysmessage( s, 1032 );
				return;
			}
		}
		i->SetColour( ( ( s->AddID1() )<<8) + s->AddID2() );
		RefreshItem( i );
		soundeffect( s, 0x023E, true );
	}
	else
		sysmessage( s, 1033 );
}

void cTargets::AddNpcTarget( cSocket *s )
{
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	CHARACTER npcOff;
	CChar *npc = Npcs->MemCharFree( npcOff );
	if( npc == NULL )
		return;

	npc->SetName( "Dummy" );
	npc->SetID( ( ( s->AddID1() )<<8 ) + s->AddID2() );
	npc->SetxID( npc->GetID() );
	npc->SetOrgID( npc->GetID() );
	npc->SetSkin( 0 );
	npc->SetxSkin( 0 );
	npc->SetSkillTitles( true );
	npc->SetLocation( s->GetWord( 11 ), s->GetWord( 13 ), s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ) );
	npc->SetNpc( true );
	npc->Update();
}

void cTargets::FreezeTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetFrozen( true );
}

void cTargets::UnfreezeTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetFrozen( false );
}

// DarkStorm: Updated AllSetTarget to process the string information sent by
// the client.
void cTargets::AllSetTarget( cSocket *s )
{
	mstring Command( s->XText() );
	SERIAL Serial = s->GetDWord( 7 );

	cBaseObject *myObject = NULL;

	// We're dealing with both, items and characters
	if( Serial >= 0x40000000 )
		myObject = calcItemObjFromSer( Serial );
	else
		myObject = calcCharObjFromSer( Serial );

	if( myObject == NULL )
		return;
	
	// Split Command into 2 parts, key and value
	vector< mstring > Parts = Command.split( " ", 2 );

	if( Parts.size() < 2 ) 
	{
		sysmessage( s, "Invalid command: '%s'", s->XText() );
		return;
	}

	mstring Key = Parts[ 0 ];
	mstring Value = Parts[ 1 ];
	Key = Key.upper();

	// Log EVERYTHING
	// Check whatever we targetted
	if( myObject->GetObjType() == OT_CHAR )
		Commands->Log( "/set", s->CurrcharObj(), (CChar*)myObject, s->XText() );
	else 
	{
		// It gets a bit more complicated for items
		char Message[1024];
		sprintf( Message, "Setting '%s'on '%s'", s->XText(), myObject->GetName() );
		Commands->Log( "/set", s->CurrcharObj(), NULL, Message );
	}

	// Do Character specific stuff here...
	if( myObject->GetObjType() == OT_CHAR )
	{	
		CChar *myChar = static_cast< CChar* >( myObject );
		cSocket *charSock = calcSocketObjFromChar( myChar );

		// Allskills
		if( Key.compare( "ALLSKILLS" ) )
		{
			for( UI08 i = 0; i < ALLSKILLS; i++ )
			{
				myChar->SetBaseSkill( Value.toSI16(), i );
				Skills->updateSkillLevel( myChar, i );

				if( charSock != NULL ) 
					updateskill( charSock, i );
			}

			sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myChar->GetName() );				
			return;
		}

		// Look for matching skills
		for( UI08 i = 0; i < ALLSKILLS; i++ )
		{
			if( Key.compare( skillname[ i ] ) )
			{
				myChar->SetBaseSkill( Value.toSI16(), i );
				Skills->updateSkillLevel( myChar, i );

				if( charSock != NULL ) 
					updateskill( charSock, i );

				sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myChar->GetName() );				
				return;
			}
		}

		// XSkin, OrgSkin
		if( Key.compare( "XSKIN" ) )
		{
			myChar->SetxSkin( Value.toUI16() );
			sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myChar->GetName() );
			return;
		}
		else if( Key.compare( "ORGSKIN" ) )
		{
			myChar->SetOrgSkin( Value.toUI16() );
			sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myChar->GetName() );
			return;
		}
	}

	// Settings for normal Base Objects
	// Set the name
	if( Key.compare( "NAME" ) )
	{
		myObject->SetName( Value.c_str() );

		sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myObject->GetName() );
		return;
	}

	// updateStats
	// 0: Strength
	// 1: Intelligence
	// 2: Dexterity

	// Set the Strength
	else if( ( Key.compare( "STR" ) ) || ( Key.compare( "STENGTH" ) ) )
	{
		myObject->SetStrength( Value.toSI16() );
	
		if( myObject->GetObjType() == OT_CHAR )
		{
			updateStats( (CChar*)myObject, 0 );
		}

		sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myObject->GetName() );
		return;
	}

	// Set the Dexterity
	else if( ( Key.compare( "DEX" ) ) || ( Key.compare( "DEXTERITY" ) ) )
	{
		myObject->SetDexterity( Value.toSI16() );

		if( myObject->GetObjType() == OT_CHAR )
		{
			updateStats( (CChar*)myObject, 0 );
		}

		sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myObject->GetName() );
		return;
	}

	// Set the intelligence
	else if( ( Key.compare( "INT" ) ) || ( Key.compare( "INTELLIGENCE" ) ) )
	{
		myObject->SetIntelligence( Value.toSI16() );

		if( myObject->GetObjType() == OT_CHAR )
		{
			updateStats( (CChar*)myObject, 0 );
		}

		sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myObject->GetName() );
		return;
	}

	// Set the fame
	else if( Key.compare( "FAME" ) )
	{
		myObject->SetFame( Value.toSI16() );

		sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myObject->GetName() );
		return;
	}

	// Set the karma
	else if( Key.compare( "KARMA" ) )
	{
		myObject->SetKarma( Value.toSI16() );

		sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myObject->GetName() );
		return;
	}

	// Set the kills
	else if( Key.compare( "KILLS" ) )
	{
		myObject->SetKills( Value.toSI16() );

		sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myObject->GetName() );
		return;
	}

	// Set the Color/Hue/Colour
	else if( ( Key.compare( "COLOR" ) ) || ( Key.compare( "COLOUR" ) ) || ( Key.compare( "HUE" ) ) )
	{
		myObject->SetColour( Value.toUI16() );

		sysmessage( s, "Succesfully set '%s' on '%s'", s->XText(), myObject->GetName() );
		return;
	}

	/*CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	
	if( i == NULL )
		return;
	
	CChar *mChar = s->CurrcharObj();
	Commands->Log( "/set", mChar, i, "PostUsage" );

	cSocket *kSock = calcSocketObjFromChar( i );

	if( s->AddX() < TRUESKILLS )
	{
		i->SetBaseSkill( s->AddY(), s->AddX() );
		Skills->updateSkillLevel( i, s->AddX() );
		if( kSock != NULL ) 
			updateskill( kSock, s->AddX() );
	}
	else 
	{
		UI08 j;
		switch( s->AddX() )
		{
		case ALLSKILLS:
			for( j = 0; j < TRUESKILLS; j++ )
			{
				i->SetBaseSkill( s->AddY(), j );
				Skills->updateSkillLevel( i, j );
				if( kSock != NULL ) 
					updateskill( kSock, j );
			}
			break;
		case STRENGTH:
			i->SetStrength( s->AddY() );
			for( j = 0; j < TRUESKILLS; j++ )
			{
				Skills->updateSkillLevel( i, j );
				if( kSock != NULL ) 
					updateskill( kSock, j );
			}
			if( kSock != NULL ) 
				statwindow( kSock, i );
			break;
		case DEXTERITY:
			i->SetDexterity( s->AddY() );
			for( j = 0; j < TRUESKILLS; j++ )
			{
				Skills->updateSkillLevel( i, j );
				if( kSock != NULL ) 
					updateskill( kSock, j );
			}
			if( kSock != NULL ) 
				statwindow( kSock, i );
			break;
		case INTELLECT:
			i->SetIntelligence( s->AddY() );
			for( j = 0; j < TRUESKILLS; j++ )
			{
				Skills->updateSkillLevel( i, j );
				if( kSock != NULL ) 
					updateskill( kSock, j );
			}
			if( kSock != NULL ) 
				statwindow( kSock, i );
			break;
		case FAME:
			i->SetFame( s->AddY() );
			break;
		case KARMA:
			i->SetKarma( s->AddY() );
			break;
		case SKILLS + 1:
			i->SetKills( s->AddY() );
			break;
		}
	}*/
}

void cTargets::InfoTarget( cSocket *s )
{
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	map_st map1;
	CLand land;
	
	SI16 x = s->GetWord( 0x0B );
	SI16 y = s->GetWord( 0x0D );
//	SI08 z = s->GetByte( 0x10 );

	UI08 worldNumber = 0;
	if( s->CurrcharObj() != NULL )
		worldNumber = s->CurrcharObj()->WorldNumber();
	if( s->GetWord( 0x11 ) == 0 )
	{  // manually calculating the ID's if it's a maptype
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
		SI32 tilenum = s->GetWord( 0x11 );
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
	sysmessage( s, 1034 );
}

void cTargets::TweakTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )
		tweakCharMenu( s, c );
	else 
	{
		CItem *i = calcItemObjFromSer( serial );
		if( i != NULL )
			tweakItemMenu( s, i );
	}
}

void cTargets::LoadCannon( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	SERIAL moreSerial = s->AddID();
	if( i->GetMore() == moreSerial || s->AddID1() == 0xFF )
	{
		if( i->GetMoreZ() == 0 && itemInRange( s->CurrcharObj(), i, 2 ) )
		{
			i->SetMoreZ( 1 );
			sysmessage( s, 1035 );
		}
		else
		{
			if( i->GetMore( 1 ) == 0x00 ) 
				sysmessage( s, 1036 );
			else 
				sysmessage( s, 1037 );
		}
	}
}

void cTargets::SetInvulFlag( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetInvulnerable( s->AddX() == 1 );
}

void cTargets::Tiling( cSocket *s )  // Clicking the corners of tiling calls this function - Crwth 01/11/1999
{
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	if( s->ClickX() == -1 && s->ClickY() == -1 )
	{
		s->ClickX( s->GetWord( 11 ) );
		s->ClickY( s->GetWord( 13 ) );
		target( s, 0, 1, 0, 198, 1038 );
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
	
	if( s->AddID1() == 0x40 )
	{
		switch( s->AddID2() )
		{
		case 100:
		case 102:
		case 104:
		case 106:
		case 108:
		case 110:
		case 112:
		case 114:
		case 116:
		case 118:
		case 120:
		case 122:
		case 124:
		case 126:
		case 140:
			AddTarget( s );
			return;
		}
	}

	bool pileable = false;
	UI16 addid = (UI16)(( ( s->AddID1() ) << 8 ) + s->AddID2());
	CTile tile;
	Map->SeekTile( addid, &tile );
	if( tile.Stackable() ) 
		pileable = true;

	CItem *c = NULL;
	for( SI16 x = x1; x <= x2; x++ )
	{
		for( SI16 y = y1; y <= y2; y++ ) 
		{
			c = Items->SpawnItem( s, 1, "#", pileable, addid, 0, false, false );
			if( c == NULL ) 
				return;
			c->SetPriv( 0 );	//Make them not decay
			c->SetLocation( x, y, s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ) );
			RefreshItem( c );
		}
	}
	s->AddID1( 0 );
	s->AddID2( 0 );
}

void cTargets::AreaCommand( cSocket *s )
{
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	if( s->ClickX() == -1 && s->ClickY() == -1 )
	{
		s->ClickX( s->GetWord( 11 ) );
		s->ClickY( s->GetWord( 13 ) );
		target( s, 0, 1, 0, 90, 1040 );
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
		INVALID_CMD,
		ACT_CMDCOUNT
	};

	const string areaCommandStrings[ACT_CMDCOUNT] =
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
		""
	};

	AreaCommandTypes cmdType = INVALID_CMD;

	char *orgString = NULL, *foundSpace = NULL;
	char temp[512];

	orgString = s->XText();
	if( orgString == NULL )
		return;

	UI16 j = 0;
	UI16 sLen = strlen( orgString );
	for( j = 0; j < sLen; j++ )
	{
		if( orgString[j] == ' ' )
		{
			foundSpace = &orgString[j];
			break;
		}
	}

	if( foundSpace == NULL )
	{
		strcpy( temp, orgString );
	}
	else
	{

		UI16 sLen2 = (UI16)(foundSpace - orgString);
		strncpy( temp, orgString, sLen2 );
		temp[sLen2] = 0;
	}

	for( AreaCommandTypes k = ACT_DYE; k < INVALID_CMD; k = (AreaCommandTypes)((int)k + 1) )
	{
		if( !strcmp( temp, areaCommandStrings[k].c_str() ) )
		{
			cmdType = k;
			break;
		}
	}

	if( cmdType == INVALID_CMD )
	{
		sysmessage( s, "Unknown command type" );
		return;
	}

	for( ITEM i = 0; i < itemcount; i++ )
	{
		if( items[i].isFree() )
			continue;
		if( items[i].GetCont() != INVALIDSERIAL )
			continue;
		if( items[i].GetX() >= x1 && items[i].GetX() <= x2 && items[i].GetY() >= y1 && items[i].GetY() <= y2 )
		{
			switch( cmdType )
			{
			case ACT_DYE:	// dye
				items[i].SetColour( (UI16)makeNum( foundSpace ) );
				RefreshItem( &items[i] );
				break;
			case ACT_WIPE:	// wipe
				Items->DeleItem( &items[i] );
				break;
			case ACT_INCX: // incx
				MapRegion->RemoveItem( &items[i] );
				items[i].IncX( (SI16)makeNum( foundSpace ) );
				MapRegion->AddItem( &items[i] );
				RefreshItem( &items[i] );
				break;
			case ACT_INCY:	// incy
				MapRegion->RemoveItem( &items[i] );
				items[i].IncY( (SI16)makeNum( foundSpace ) );
				MapRegion->AddItem( &items[i] );
				RefreshItem( &items[i] );
				break;
			case ACT_INCZ:	// incz
				items[i].IncZ( (SI08)makeNum( foundSpace ) );
				RefreshItem( &items[i] );
				break;
			case ACT_SETX: // setx
				MapRegion->RemoveItem( &items[i] );
				items[i].SetX( (SI16)makeNum( foundSpace ) );
				MapRegion->AddItem( &items[i] );
				RefreshItem( &items[i] );
				break;
			case ACT_SETY:	// sety
				MapRegion->RemoveItem( &items[i] );
				items[i].SetY( (SI16)makeNum( foundSpace ) );
				MapRegion->AddItem( &items[i] );
				RefreshItem( &items[i] );
				break;
			case ACT_SETZ:	// setz
				items[i].SetZ( (SI08)makeNum( foundSpace ) );
				RefreshItem( &items[i] );
				break;
			case ACT_SETTYPE: // settype
				items[i].SetType( (UI08)makeNum( foundSpace ) );
				break;
			case ACT_NEWBIE:	// newbie
				items[i].SetNewbie( makeNum( foundSpace ) != 0 );
				break;
			case ACT_SETSCPTRIG:	// set script #
				items[i].SetScriptTrigger( makeNum( foundSpace ) );
				break;
			default:
				break;
			}
			// process command here!
		}
	}
}


void cTargets::Wiping( cSocket *s )  // Clicking the corners of wiping calls this function - Crwth 01/11/1999
{
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	if( s->ClickX() == -1 && s->ClickY() == -1 )
	{
		s->ClickX( s->GetWord( 11 ) );
		s->ClickY( s->GetWord( 13 ) );
		if( s->AddID1() ) 
			target( s, 0, 1, 0, 199, 1039 );
		else 
			target( s, 0, 1, 0, 199, 1040 );
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

	ITEM i;
	if( s->AddID1() == 1  )
	{  // addid1[s]==1 means to inverse wipe
		for( i = 0; i < itemcount; i++ )
		{
			if(!(items[i].GetX() >= x1 && items[i].GetX() <= x2 && items[i].GetY() >= y1 && items[i].GetY() <= y2 ) 
				&& items[i].GetCont() == INVALIDSERIAL && !items[i].isWipeable() )
				Items->DeleItem( &items[i] );
		}
	}
	else 
	{
		for( i = 0; i < itemcount; i++ )
		{
			if( items[i].GetX() >= x1 && items[i].GetX() <= x2 && items[i].GetY() >= y1 && items[i].GetY() <= y2 
				&& items[i].GetCont() == INVALIDSERIAL && !items[i].isWipeable() )
				Items->DeleItem( &items[i] );
		}
	}
}

void cTargets::ExpPotionTarget( cSocket *s ) //Throws the potion and places it (unmovable) at that spot
{
	SI16 x = s->GetWord( 11 );
	SI16 y = s->GetWord( 13 );
	SI08 z = s->GetByte( 16 );
	CChar *mChar = s->CurrcharObj();
	if( x != -1 )
	{
		if( LineOfSight( s, mChar, x, y, z, WALLS_CHIMNEYS + DOORS + ROOFING_SLANTED ) )
		{
			CItem *i = calcItemObjFromSer( s->AddID() );
			if( i != NULL )
			{
				i->SetCont( INVALIDSERIAL );
				i->SetLocation( x, y, z );
				i->SetMagic( 2 ); //make item unmovable once thrown
				movingeffect( mChar, i, 0x0F0D, 0x11, 0x00, 0x00 );
				RefreshItem( i );
			}
		} 
		else 
			sysmessage( s, 1041 );
	}
}

void cTargets::SquelchTarg( cSocket *s )
{
	CChar *p = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( p != NULL )
	{
		if( p->IsGM() )
		{
			sysmessage( s, 1042 );
			return;
		} 
		if( p->GetSquelched() )
		{
			p->SetSquelched( 0 );
			sysmessage( s, 1655 );
			sysmessage( calcSocketObjFromChar( p ), 1043 );
			p->SetMuteTime( -1 );
		} 
		else 
		{
			p->SetMuteTime( -1 );
			p->SetSquelched( 1 );
			sysmessage( s, 1044 );
			sysmessage( calcSocketObjFromChar( p ), 761 );
			if( s->AddID1() != 0xFF && s->AddID1() != 0 )			// 255 used to be -1, not good for UI08s
			{
				p->SetMuteTime( BuildTimeValue( s->AddID1() ) );
				s->AddID1( 255 );
				p->SetSquelched( 2 );
			}
		}
	}
}


void cTargets::TeleStuff( cSocket *s )
{
    static UI32 targ = INVALIDSERIAL;//What/who to tele
    if( targ == INVALIDSERIAL )
	{
		SERIAL serial = s->GetDWord( 7 );
		if( s->GetByte( 7 ) == 0xFF ) 
			return;
		targ = calcCharFromSer( serial );
		
		if( targ != INVALIDSERIAL )
		{
			targ += 1000000;
			target( s, 0, 1, 0, 222, 1045 );
		} 
		else 
		{
			targ = calcItemFromSer( serial );
			if( targ != INVALIDSERIAL )
				target( s, 0, 1, 0, 222, 1046 );
		}
		return;
	} 
	else 
	{
		if( s->GetByte( 11 ) == 0xFF )
			return;
		CHARACTER i;
		SI16 x = s->GetWord( 11 );
		SI16 y = s->GetWord( 13 );
		SI08 z = s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) );
		
		if( targ > 999999 )//character
		{
			sysmessage( s, 1047 );
			i = targ - 1000000;
			chars[i].SetLocation( x, y, z );
			chars[i].Teleport();
		} 
		else 
		{
			i = targ;
			items[i].SetLocation( x, y, z );
			sysmessage( s, 1048 );
			RefreshItem( &items[i] );
		}
		targ = INVALIDSERIAL;
	}
}

void cTargets::SwordTarget( cSocket *s )
{
	if( s == NULL )
		return;

	CChar *p = calcCharObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();

	if( mChar == NULL )
		return;

	if( p != NULL )
	{
		if( p->GetID() == 0xCF )
		{
			// Unshorn sheep
			// -> Add Wool and change id of the Sheep
			CItem *c = Items->SpawnItemToPack( s, mChar, "0x0DF8", false );

			p->SetID( 0xDF );			
			p->Teleport();

			// Add an effect so the sheep can regain it's wool
			cDice *myDice = new cDice( "2d3" );
			UI32 Delay = myDice->roll();
			delete myDice;			

			tempeffect( p, p, 43, Delay*300, 0, 0, INVALIDSERIAL );
		}
		else
		{
			// Already sheered
			//sysmessage( s, "" );
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
				sysmessage( s, 393 );
				return;
			}
			if( !mChar->IsOnHorse() )
				action( s, 0x0D );
			else 
				action( s, 0x1D );
			soundeffect( s, 0x013E, true );
			CItem *c = Items->SpawnItem( s, 1, "#", true, 0x0DE1, 0, false, false ); //Kindling
			if( c == NULL ) 
				return;

			c->SetLocation( mChar );
			RefreshItem( c );
			sysmessage( s, 1049 );
			return;
		}
		case 0x09CC: 
		case 0x09CD: 
		case 0x09CE: 
		case 0x09CF: 
		{	
			CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) ); 
			if( i == NULL ) 
				return;		
			if( getPackOwner( i ) != mChar ) 
			{ 
				sysmessage( s, 775 ); 
				return; 
			} 
			else 
			{ 
				int getAmt = getAmount( mChar, i->GetID() ); 
				if( getAmt < 1 ) 
				{ 
					sysmessage( s, 776 ); 
					return; 
				} 
				soundeffect( s, 0x013E, true); // I'm not sure 
				CItem *c = Items->SpawnItem( s, 4, "raw fish steak", true, 0x097A, 0, true, true); 
				if( c == NULL ) 
				{ 
					return; 
				} 
				decItemAmount( i ); 
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
	sysmessage( s, 1050 );
}

void cTargets::CorpseTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	bool n = false;
	if( itemInRange( s->CurrcharObj(), i, 1 ) ) 
	{
		s->AddMItem( calcItemFromSer( i->GetSerial() ) );
		action( s, 0x20 );
		n = true;
		if( i->GetMore( 1 ) == 0 )
		{
			i->SetMore( 1, 1 );
			if( i->GetMoreY() || i->GetCarve() != -1 )
				newCarveTarget( s, i );
		} 
		else 
			sysmessage( s, 1051 );
	}
	if( !n ) 
		sysmessage( s, 393 );
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/22/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
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
//|	Returns				-	N/A 
//o--------------------------------------------------------------------------o	
void cTargets::newCarveTarget( cSocket *s, CItem *i )
{
	bool deletecorpse = false;

	CItem *c = Items->SpawnItem( s, 1, "#", false, 0x122A, 0, false, false ); // add the blood puddle
	if( c == NULL ) 
		return;
	MapRegion->RemoveItem( c );

	ITEM mItem = s->AddMItem();
	CChar *mChar = s->CurrcharObj();

	c->SetLocation( &items[mItem] );
	c->SetMagic( 2 );
	c->SetDecayTime( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) ) );
	RefreshItem( c );

	char temp[1024];
	// if it's a human corpse
	// Sept 22, 2002 - Xuri - Corrected the alignment of body parts that are carved.
	if( i->GetMoreY() )
	{
		// create the Head
		sprintf( temp, Dictionary->GetEntry( 1058 ), i->GetName2() );
		c = Items->SpawnItem( s, 1, temp, true, 0x1DAE, 0, false, false );
		if( c == NULL ) 
			return;
		c->SetLayer( 0x01 );
		c->SetCont( i->GetSerial() );
		c->SetOwner( i->GetOwner() );

		// create the Body
		sprintf( temp, Dictionary->GetEntry( 1059 ), i->GetName2() );
		c = Items->SpawnItem( s, 1, temp, true, 0x1CED, 0, false, false );
		if( c == NULL ) 
			return;
		c->SetLayer( 0x01 );
		c->SetCont( i->GetSerial() );
		c->SetOwner( i->GetOwner() );

		sprintf( temp, Dictionary->GetEntry( 1057 ), i->GetName2() );
		c = Items->SpawnItem( s, 1, temp, true, 0x1DAD, 0, false, false );
		if( c == NULL ) 
			return;
		c->SetLayer( 0x01 );
		c->SetCont( i->GetSerial() );
		c->SetOwner( i->GetOwner() );

		// create the Left Arm
		sprintf( temp, Dictionary->GetEntry( 1060 ), i->GetName2() );
		c = Items->SpawnItem( s, 1, temp, true, 0x1D80, 0, false, false );
		if( c == NULL ) 
			return;
		c->SetLayer( 0x01 );
		c->SetCont( i->GetSerial() );
		c->SetOwner( i->GetOwner() );

		// create the Right Arm
		sprintf( temp, Dictionary->GetEntry( 1061 ), i->GetName2() );
		c = Items->SpawnItem( s, 1, temp, true, 0x1DAF, 0, false, false );
		if( c == NULL ) 
			return;
		c->SetLayer( 0x01 );
		c->SetCont( i->GetSerial() );
		c->SetOwner( i->GetOwner() );

		// create the Left Leg
		sprintf( temp, Dictionary->GetEntry( 1062 ), i->GetName2() );
		c = Items->SpawnItem( s, 1, temp, true, 0x1DB2, 0, false, false );
		if( c == NULL ) 
			return;
		c->SetLayer( 0x01 );
		c->SetCont( i->GetSerial() );
		c->SetOwner( i->GetOwner() );

		// create the Right Leg
		sprintf( temp, Dictionary->GetEntry( 1063 ), i->GetName2() );
		c = Items->SpawnItem( s, 1, temp, true, 0x1D81, 0, false, false );
		if( c == NULL ) 
			return;
		c->SetLayer( 0x01 );
		c->SetCont( i->GetSerial() );
		c->SetOwner( i->GetOwner() );

		//human: always delete corpse!
		deletecorpse = true;
		criminal( mChar );
	} 
	else
	{
		char sect[512];
		sprintf( sect, "CARVE %i", i->GetCarve() );
		ScriptSection *toFind = FileLookup->FindEntry( sect, carve_def );
		if( toFind == NULL )
			return;
		const char *tag = NULL;
		const char *data = NULL;
		UI08 worldNumber = mChar->WorldNumber();
		for( tag = toFind->First(); !toFind->AtEnd(); tag = toFind->Next() )
		{
			data = toFind->GrabData();
			if( !strcmp( "ADDITEM", tag ) )
			{
				c = Items->CreateScriptItem( s, data, false, worldNumber );
				if( c != NULL )
				{
					c->SetCont( i->GetSerial() );
					c->SetX( 20 + RandomNum( 0, 49 ) );
					c->SetY( 85 + RandomNum( 0, 74 ) );
					c->SetZ( 9 );
					RefreshItem( c );
				}
			}
		}
	}

	if( deletecorpse )
	{
		for( c = i->FirstItemObj(); !i->FinishedItems(); c = i->NextItemObj() )
		{
			if( c != NULL )
			{
				c->SetCont( INVALIDSERIAL );
				c->SetLocation( i );
				c->SetDecayTime( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) ) );
				RefreshItem( c );
			}
		}
		Items->DeleItem( i );
	}
}

void cTargets::TitleTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetTitle( s->XText() );
}

void cTargets::NpcTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		s->AddID( i->GetSerial() );
		target( s, 0, 1, 0, 57, "Select NPC to follow this player." );
	}
}

void cTargets::NpcTarget2( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( i->IsNpc() )
		{
			i->SetFTarg( calcCharFromSer( s->AddID() ) );
			i->SetNpcWander( 1 );
		}
	}
}

void cTargets::NpcRectTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( i->IsNpc() )
		{
			i->SetFx( s->AddX(), 1 );
			i->SetFy( s->AddY(), 1 );
			i->SetFz( -1 );
			i->SetFx( s->AddX2(), 2 );
			i->SetFy( s->AddY2(), 2 );
			i->SetNpcWander( 3 );
		}
	}
}

void cTargets::NpcCircleTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( i->IsNpc() )
		{
			i->SetFx( s->AddX(), 1 );
			i->SetFy( s->AddY(), 1 );
			i->SetFz( -1 );
			i->SetFx( s->AddX2(), 2 );
			i->SetNpcWander( 4 );
		}
	}
}

void cTargets::NpcWanderTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( i->IsNpc() )
			i->SetNpcWander( npcshape[0] );
	}
}

void cTargets::NpcAITarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetNPCAiType( s->AddX() );
}

void cTargets::xBankTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		openBank( s, i );
}
void cTargets::xSpecialBankTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		openSpecialBank( s, i );
}

void cTargets::DupeTarget( cSocket *s )
{
	if( s->AddID1() >= 1 )
	{
		UI08 dupetimes = s->AddID1();
		CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
		if( i != NULL )
		{
			for( UI08 dupeit = 0; dupeit < dupetimes; dupeit++ )
				Commands->DupeItem( s, i, i->GetAmount() );
		}
	}
}

void cTargets::MoveToBagTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i == NULL )
		return;
	CChar *mChar = s->CurrcharObj();
	CItem *p = getPack( mChar );
	if( i->GetSerial() == serial )
	{
		if( i->GetCont() == INVALIDSERIAL )
			MapRegion->RemoveItem( i );
		i->SetX( RandomNum( 0, 79 ) + 50 );
		i->SetY( RandomNum( 0, 79 ) + 50 );
		i->SetZ( 9 );
		if( p != NULL ) 
			i->SetCont( p->GetSerial() );
		i->SetDecayTime( 0 );//reset decaytimer
		CPRemoveItem toRemove = (*i);

		Network->PushConn();
		for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
			tSock->Send( &toRemove );
		Network->PopConn();
		RefreshItem( i );
	}
}

void cTargets::SellStuffTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		sendSellStuff( s, i );
}

void cTargets::ReleaseTarget( cSocket *s, SERIAL c )
{
	CChar *i = NULL;
	if( c == INVALIDSERIAL )
		i = calcCharObjFromSer( s->GetDWord( 7 ) );	
	else
		i = calcCharObjFromSer( c );
	if( i != NULL )
	{
		if( !i->IsJailed() )
			sysmessage( s, 1064 );
		else
		{
			JailSys->ReleasePlayer( i );
			sysmessage( s, 1065, i->GetName() );
		}
	}
}

void cTargets::GmOpenTarget( cSocket *s )
{
	CChar *toCheck = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( toCheck == NULL )
	{
		sysmessage( s, 1066 );
		return;
	}

	CItem *i = FindItemOnLayer( toCheck, s->AddMItem() );
	if( i != NULL )
	{
		openPack( s, i );
		return;
	}
	sysmessage( s, 1067 );
}

void cTargets::StaminaTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		soundeffect( i, 0x01F2 );
		staticeffect( i, 0x376A, 0x09, 0x06 );
		i->SetStamina( i->GetMaxStam() );
		updateStats( i, 2 );
		return;
	}
	sysmessage( s, 1066 );
}

void cTargets::ManaTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		soundeffect( i, 0x01F2 );
		staticeffect( i, 0x376A, 0x09, 0x06 );
		i->SetMana( i->GetMaxMana() );
		updateStats( i, 1 );
		return;
	}
	sysmessage( s, 1066 );
}

void cTargets::MakeShopTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		Commands->MakeShop( i );
		sysmessage( s, 1068 );
		return;
	}
	sysmessage( s, 1069 );
}

void cTargets::JailTarget( cSocket *s, SERIAL c )
{
	CChar *i = NULL;
	if( c == INVALIDSERIAL )
		i = calcCharObjFromSer( s->GetDWord( 7 ) );	  
	else
		i = calcCharObjFromSer( c );
	
    if( i == NULL ) 
		return;

	if( i->IsJailed() )
	{
		sysmessage( s, 1070 );
		return;
	}
    if( !JailSys->JailPlayer( i, 0xFFFFFFFF ) )
		sysmessage( s, 1072 );
}

void cTargets::AttackTarget( cSocket *s )
{
	CChar *target = calcCharObjFromSer( s->AddID() );
	CChar *target2 = calcCharObjFromSer( s->GetDWord( 7 ) );
	
	if( target2 == NULL || target == NULL ) 
		return;
	if( target2 == target )
	{
		sysmessage( s, 1073 );
		return;
	}
	npcAttackTarget( target2, target );
	if( target2->IsInnocent() && target2->GetSerial() != target->GetOwner() )
	{
		CChar *pOwner = (CChar *)target->GetOwnerObj();
		if( pOwner != NULL )
			criminal( pOwner );
	}
}

void cTargets::FollowTarget( cSocket *s )
{
	CChar *char1 = calcCharObjFromSer( s->AddID() );
	CHARACTER char2 = calcCharFromSer( s->GetDWord( 7 ) );
	
	char1->SetFTarg( char2 );
	char1->SetNpcWander( 1 );
}

void cTargets::TransferTarget( cSocket *s )
{
	char t[120];
	
	CChar *char1 = calcCharObjFromSer( s->AddID() );
	CChar *char2 = calcCharObjFromSer( s->GetDWord( 7 ) );
	
	sprintf( t, Dictionary->GetEntry( 1074 ), char1->GetName(), char2->GetName() );
	npcTalkAll( char1, t, false );
	
	char1->SetOwner( char2->GetSerial() );
	char1->SetNpcWander( 1 );
	
	char1->SetFTarg( INVALIDSERIAL );
	char1->SetNpcWander( 0 );
}

void cTargets::BuyShopTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CChar *i = calcCharObjFromSer( serial );
	if( i != NULL )
	{
		BuyShop( s, i );
		return;
	}
	sysmessage( s, 1075 );
}

bool cTargets::BuyShop( cSocket *s, CChar *c )
{
	if( c == NULL )
		return false;
	CItem *buyPack		= FindItemOnLayer( c, 0x1A );
	CItem *boughtPack	= FindItemOnLayer( c, 0x1B );
	
	if( buyPack == NULL || boughtPack == NULL )
		return false;
	
	buyPack->Sort();
	boughtPack->Sort();
	c->SendToSocket( s, false, c );

	CPItemsInContainer iic( buyPack );		s->Send( &iic );
	CPOpenBuyWindow obw( buyPack, c );		s->Send( &obw );

	CPItemsInContainer iic2( boughtPack );	s->Send( &iic2 );
	CPOpenBuyWindow obw2( boughtPack, c );	s->Send( &obw2 );

	CPDrawContainer toSend;
	toSend.Model( 0x0030 );
	toSend.Serial( c->GetSerial() );
	s->Send( &toSend );
	statwindow( s, s->CurrcharObj() ); // Make sure the gold total has been sent.
	return true;
}

void cTargets::SetValueTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		i->SetValue( s->AddX() );
		return;
	}
	sysmessage( s, 1076 );
}

void cTargets::SetRestockTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		i->SetRestock( s->AddX() );
		return;
	}
	sysmessage( s, 1076 );
}


void cTargets::permHideTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( i->GetHidden() == 1 )
		{
			sysmessage( s, 833 );
			return;
		}
		i->SetPermHidden( true );
		i->SetHidden( 1 );
		i->Update();
	}
}

void cTargets::unHideTarget( cSocket *s )
{
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( c != NULL )
	{
		if( !c->IsGM() && !c->IsCounselor() )
			c->SetPermHidden( false );
	}
	s->AddX( 0 );
	VisibleTarget( s );	// better code reuse!
}

void cTargets::SetWipeTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		i->SetWipeable( s->AddID1() != 0 );
		RefreshItem( i );
	}
}
void cTargets::SetSpAttackTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetSpAttack( s->TempInt() );
}

void cTargets::SetSpaDelayTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetSpDelay( s->TempInt() );
}

void cTargets::SetPoisonTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetPoison( s->TempInt() );
}

void cTargets::SetPoisonedTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		i->SetPoisoned( s->TempInt() );
		i->SetPoisonWearOffTime( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) ) );
		i->SendToSocket( calcSocketObjFromChar( i ), true, i );
	}
}

void cTargets::FullStatsTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		soundeffect( i, 0x01F2 );
		staticeffect( i, 0x376A, 0x09, 0x06 );
		i->SetMana( i->GetMaxMana() );
		i->SetHP( i->GetMaxHP() );
		i->SetStamina( i->GetMaxStam() );
		updateStats( i, 0 );
		updateStats( i, 1 );
		updateStats( i, 2 );
		return;
	}
	sysmessage( s, 1077 );
}


void cTargets::SetAdvObjTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetAdvObj( s->TempInt() );
}

//---------------------------------------------------------------------------o
//|	Class		:	void cantraintarget(int s)
//|	Date		:	1-3-99
//|	Programmer	:	Antrhacks
//o---------------------------------------------------------------------------o
//| Purpose		:Used for training by NPC's
//o---------------------------------------------------------------------------o
void cTargets::CanTrainTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( !i->IsNpc() )
		{
			sysmessage( s, 1078 );
			return;
		}
		i->SetCanTrain( !i->CanTrain() );
	}
}

void cTargets::SetSplitTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetSplit( s->TempInt() );
}

void cTargets::SetSplitChanceTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		i->SetSplitChance( s->TempInt() );
}

void cTargets::AxeTarget( cSocket *s )
{
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

void cTargets::ObjPrivTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		switch( s->AddID1() )
		{
		case 0:	i->SetDecayable( false );		break;
		case 1:	i->SetDecayable( true );		break;
		case 3:	i->SetPriv( s->AddID2() );	break;
		}
	}
}

void cTargets::SetDirTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	if( s->GetByte( 7 ) >= 0x40 )
	{
		CItem *i = calcItemObjFromSer( serial );
		if( i != NULL )
		{
			i->SetDir( s->AddX() );
			RefreshItem( i );
			return;
		}
	}
	else
	{
		CChar *c = calcCharObjFromSer( serial );
		if( c != NULL )
		{
			c->SetDir( (s->AddX()) & 0x0F );	// make sure high-bits are cleared
			c->Update();
			return;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void npcresurrecttarget( CChar *i )
//|   Date        -  UnKnown
//|   Programmer  -  UnKnown  (Touched tabstops by Tauriel Dec 28, 1998)
//|									
//|	Modification	-	09/22/2002	-	Xuri - Made players not appear with full 
//|									health/stamina after being resurrected by NPC Healer
//o---------------------------------------------------------------------------o
//|   Purpose     -  Resurrects a character
//o---------------------------------------------------------------------------o
void cTargets::NpcResurrectTarget( CChar *i )
{
	if( i == NULL )
		return;

	if( i->IsNpc() )
	{
		Console.Error( 2, Dictionary->GetEntry( 1079 ), i );
		return;
	}
	cSocket *mSock = calcSocketObjFromChar( i );
	if( i->IsDead() && mSock != NULL )
	{
		Fame( i, 0 );
		soundeffect( i, 0x0214 );
		i->SetID( i->GetOrgID() );
		i->SetxID( i->GetOrgID() );
		i->SetSkin( i->GetxSkin() );
		i->SetDead( false );
		// Sept 22, 2002 - Xuri
		i->SetHP( i->GetMaxHP() / 10 );
		i->SetStamina( i->GetMaxStam() / 10 );
		//
		i->SetMana( i->GetMaxMana() / 10 );
		i->SetAttacker( INVALIDSERIAL );
		i->SetAttackFirst( false );
		i->SetWar( false );
		CItem *c = NULL;
		for( CItem *j = i->FirstItem(); !i->FinishedItems(); j = i->NextItem() )
		{
			if( j != NULL && !j->isFree() )
			{
				if( j->GetLayer() == 0x1A )
				{
					j->SetLayer( 0x15 );	
					i->SetPackItem( j );
				}
				if( j->GetSerial() == i->GetRobe() )
				{
					Items->DeleItem( j );
					
					c = Items->SpawnItem( mSock, i, 1, "a resurrect robe", false, 0x1F03, 0, false, false );
					if( c != NULL )
					{
						c->SetLayer( 0x16 );
						if( c->SetCont( i->GetSerial() ) )
							c->SetDye( true );
					}
				}
			}
		}
		i->Teleport();
		UI16 targTrig = i->GetScriptTrigger();
		cScript *toExecute = Trigger->GetScript( targTrig );
		if( toExecute != NULL )
			toExecute->OnResurrect( i );
	}
	else
		sysmessage( mSock, 1080 );
}


void cTargets::NewXTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		MapRegion->RemoveItem( i );
		i->SetX( s->AddX() );
		MapRegion->AddItem( i );
		RefreshItem( i );
	}
	
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )
	{
		c->SetLocation( s->AddX(), c->GetY(), c->GetZ() );
		c->Teleport();
	}
	
}

void cTargets::NewYTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		MapRegion->RemoveItem( i );
		i->SetY( s->AddX() );
		MapRegion->AddItem( i );
		RefreshItem( i );
	}
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )
	{
		c->SetLocation( c->GetX(), s->AddX(), c->GetZ() );
		c->Teleport();
	}
	
}

void cTargets::IncXTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		MapRegion->RemoveItem( i );
		i->IncX( s->AddX() );
		MapRegion->AddItem( i );
		RefreshItem( i );
	}
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )
	{
		c->SetLocation( c->GetX() + s->AddX(), c->GetY(), c->GetZ() );
		c->Teleport();
	}
}

void cTargets::IncYTarget( cSocket *s )
{
	SERIAL serial = s->GetDWord( 7 );
	CItem *i = calcItemObjFromSer( serial );
	if( i != NULL )
	{
		MapRegion->RemoveItem( i );
		i->IncY( s->AddX() );
		MapRegion->AddItem( i );
		RefreshItem( i );
	}
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )
	{
		c->SetLocation( c->GetX(), c->GetY() + s->AddX(), c->GetZ() );
		c->Teleport();
	}
}

void cTargets::HouseOwnerTarget( cSocket *s )
{
	cSocket *os = NULL;
	SERIAL o_serial = s->GetDWord( 7 );
	if( o_serial == INVALIDSERIAL ) 
		return;

	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	CChar *own = calcCharObjFromSer( o_serial );

	CItem *sign = calcItemObjFromSer( s->AddID() );
	if( sign == NULL )
		return;

	CItem *house = calcItemObjFromSer( sign->GetMore() );
	if( house == NULL )
		return;

	sign->SetOwner( o_serial );
	house->SetOwner( o_serial );

	killKeys( house->GetSerial() );

	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets() && os == NULL; tSock = Network->NextSocket() )
	{
		CChar *tChar = tSock->CurrcharObj();
		if( tChar == NULL )
			continue;
		if( tChar->GetSerial() == own->GetSerial() )
			os = tSock;
	}
	Network->PopConn();

	CItem *key = NULL;
	if( os != NULL ) 
	{
		key = Items->SpawnItem( os, 1, "a house key", false, 0x100F, 0, true, true );//gold key for everything else
		if( key == NULL ) 
			return;
	} 
	else 
	{
		key = Items->SpawnItem( os, 1, "a house key", false, 0x100F, 0, false, false );//gold key for everything else
		if( key == NULL ) 
			return;
		key->SetLocation( own );
		RefreshItem( key );
	}

	key->SetMore( house->GetSerial() );
	key->SetType( 7 );

	sysmessage( s, 1081, own->GetName() );
	cSocket *iSock = calcSocketObjFromChar( own );
	sysmessage( iSock, 1082, mChar->GetName() );
}

void cTargets::HouseEjectTarget( cSocket *s )
{
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	CItem *h = calcItemObjFromSer( s->AddID() );
	if( c != NULL && h != NULL ) 
	{
		SI16 sx, sy, ex, ey;
		Map->MultiArea( (CMultiObj *)h, sx, sy, ex, ey );
		if( c->GetX() >= sx && c->GetY() >= sy && c->GetX() <= ex && c->GetY() <= ey )
		{
			c->SetLocation( ex, ey, c->GetZ() );
			c->Teleport();
			sysmessage( s, 1083 );
		} 
		else 
			sysmessage( s, 1084 );
	}
}

void cTargets::HouseBanTarget( cSocket *s )
{
	// first, eject the player
	HouseEjectTarget( s );
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	CItem *h = calcItemObjFromSer( s->AddID() );
	if( c != NULL && h != NULL ) 
	{
		CMultiObj *house = static_cast<CMultiObj *>(h);
		int r = AddToHouse( house, c, 1 );
		if( r == 1 ) 
			sysmessage( s, 1085, c->GetName() );
		else if( r == 2 ) 
			sysmessage( s, 1086 );
		else 
			sysmessage( s, 1087 );
	}
}

void cTargets::HouseFriendTarget( cSocket *s )
{
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	CItem *h = calcItemObjFromSer( s->AddID() );
	if( c != NULL && h != NULL ) 
	{
		CMultiObj *house = static_cast<CMultiObj *>(h);
		int r = AddToHouse( house, c );
		if( r == 1 ) 
		{
			sysmessage( calcSocketObjFromChar( c ), 1089 );
			sysmessage( s, 1088, c->GetName() );
		} 
		else if( r == 2 ) 
			sysmessage( s, 1090 );
		else 
			sysmessage( s, 1091 );
	}
}

void cTargets::HouseUnlistTarget( cSocket *s )
{
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	CItem *h = calcItemObjFromSer( s->AddID() );
	if( c != NULL && h != NULL ) 
	{
		CMultiObj *house = static_cast<CMultiObj *>(h);
		int r = DeleteFromHouseList( house, c );
		if( r > 0 ) 
			sysmessage( s, 1092, c->GetName() );
		else 
			sysmessage( s, 1093 );
	}
}

void cTargets::GlowTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL ) 
	{ 
		sysmessage( s, 1095 ); 
		return; 
	}
	CChar *mChar = s->CurrcharObj(), *k = NULL;
	if( mChar == NULL )
		return;

	if( i->GetGlow() > 0 ) 
	{
		sysmessage( s, 1097 );
		return;
	}

	SERIAL getCont = i->GetCont();
	if( getCont != INVALIDSERIAL )
	{
		if( getCont >= 0x4000000 )
			k = getPackOwner( calcItemObjFromSer( getCont ) );
		else
			k = calcCharObjFromSer( getCont );

		if( k != mChar )
		{
			sysmessage( s, 1096 );
			return;
		}
	}

	i->SetGlowColour( i->GetColour() );

	CItem *c = Items->SpawnItem( s, 1, "glower", false, 0x1647, 0, false, true ); // spawn light emitting object
	if( c == NULL )
		return;

    c->SetDir( 29 );
	c->SetVisible( 0 );
	c->SetMagic( 3 );
	MapRegion->RemoveItem( c );
	//c->SetLayer( items[i].GetLayer() );
	if( getCont == INVALIDSERIAL || getCont >= 0x4000000 ) // if not equipped -> coords of the light-object = coords of the 
	{
		c->SetX( i->GetX() );
		c->SetY( i->GetY() );
		c->SetZ( i->GetZ() );
	} 
	else // if equipped -> place lightsource at player ( height= approx hand level )
	{ 
		c->SetX( mChar->GetX() );
		c->SetY( mChar->GetY() );
		c->SetZ( mChar->GetZ() + 4 );
	}

    c->SetPriv( 0 ); // doesnt decay

	i->SetGlow( c->GetSerial() ); // set glow-identifier

	RefreshItem( i );
	RefreshItem( c );

	mChar->SendToSocket( s, false, mChar );
	sysmessage( s, 1098 );
}

void cTargets::UnglowTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
	{ 
		sysmessage( s, 1099 ); 
		return; 
	}
	CChar *mChar = s->CurrcharObj(), *k = NULL;
	if( mChar == NULL )
		return;

	SERIAL getCont = i->GetCont();
	if( getCont != INVALIDSERIAL ) 
	{      
		if( getCont >= 0x4000000 )
			k = getPackOwner( calcItemObjFromSer( getCont ) );
		else
			k = calcCharObjFromSer( getCont );

		if( k != mChar )
		{
			sysmessage( s, 1100 );
			return;
		}
	}

	CItem *j = calcItemObjFromSer( i->GetGlow() );
	if( i->GetGlow() == 0 || j == NULL ) 
	{
		sysmessage( s, 1101 );
		return;
	}

	i->SetColour( i->GetGlowColour() );

	Items->DeleItem( j );   
	i->SetGlow( 0 );
	RefreshItem( i );
	mChar->SendToSocket( s, false, mChar );

	sysmessage( s, 1102 );
}

void cTargets::ShowSkillTarget( cSocket *s )
{
	CChar *p = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( p == NULL )
	{
		sysmessage( s, 1103 );
		return;
	}
	char temp[1024];
	int j, k, zz, ges = 0;
	char skill_info[(ALLSKILLS+1)*40];
	char sk[25];

	int z = s->AddX();
	if( z < 0 || z > 3 ) 
		z = 0;
	if( z == 2 || z == 3 ) 
		sprintf( skill_info, "%s's skills:", p->GetName() ); 
	else 
		sprintf( skill_info, "%s's baseskills:", p->GetName() );
	int b = strlen( p->GetName() ) + 11;
	if( b > 23 ) 
		b = 23;

	int c;
	for( c = b; c <= 26; c++ )
	{
		strcpy( temp, spc );
		strcpy( &skill_info[strlen( skill_info )], temp );
	}

	numtostr( ges, sk );
	sprintf( temp, "sum: %s", sk );
	strcpy( &skill_info[strlen( skill_info )], temp );
	for( int a = 0; a < ALLSKILLS; a++ )
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
			numtostr( j, sk );  // skill-value string in sk
			ges += j;
			sprintf( temp, "%s%s%s", skillname[a], spc, sk );
			strcpy( &skill_info[strlen( skill_info )], temp );

			b = strlen( skillname[a] ) + strlen( sk ) + 1; // it doesn't like \n's, so insert spaces till end of line
			if( b > 23 )
				b = 23;
			for( c = b; c <= 26; c++ )
			{
				strcpy( temp, spc );
				strcpy( &skill_info[strlen( skill_info )], temp );
			}
		}
	}

	numtostr( ges, sk );
	sprintf( temp, "sum: %s  ", sk );
	strcpy( &skill_info[ strlen( skill_info )], temp );

	int i = strlen( skill_info ) + 10;
	updscroll[1] = (UI08)(i>>8);
	updscroll[2] = (UI08)(i%256);
	updscroll[3] = 2;
	updscroll[8] = (UI08)((i-10)>>8);
	updscroll[9] = (UI08)((i-10)%256);
	s->Send( updscroll, 10 );
	s->Send( skill_info, strlen( skill_info ) );
}

void cTargets::FriendTarget( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	CChar *targChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( targChar == NULL )
	{
		sysmessage( s, 1103 );
		return;
	}
	if( targChar->IsNpc() || !isOnline( targChar ) || targChar == mChar )
	{
		sysmessage( s, 1622 );
		return;
	}

	CChar *pet = calcCharObjFromSer( s->AddID() );
	if( Npcs->checkPetFriend( targChar, pet ) )
	{
		sysmessage( s, 1621 );
		return;
	}

	CHARLIST *petFriends = pet->GetFriendList();
	if( petFriends != NULL )
	{
		if( petFriends->size() >= 20 )
		{
			sysmessage( s, 1623 );
			return;
		}
	}

	pet->AddFriend( targChar );
	sysmessage( s, 1624, pet->GetName(), targChar->GetName() );

	cSocket *targSock = calcSocketObjFromChar( targChar );
	if( targSock != NULL )
		sysmessage( targSock, 1625, mChar->GetName(), pet->GetName() );
}

void cTargets::GuardTarget( cSocket *s )
//PRE:	Pet has been commanded to guard
//POST: Pet guards person, if owner currently
//DEV:	Abaddon
//DATE: 3rd October
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	CChar *petGuarding = calcCharObjFromSer( s->AddID() );
	if( petGuarding == NULL )
		return;

	CChar *charToGuard = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( charToGuard != NULL )
	{
		if( charToGuard->GetSerial() != petGuarding->GetOwner() && !Npcs->checkPetFriend( charToGuard, petGuarding ) )
		{
			sysmessage( s, 1628 );
			return;
		}
		petGuarding->SetNPCAiType( 32 ); // 32 is guard mode
		petGuarding->SetGuarding( charToGuard->GetSerial() );
		mChar->SetGuarded( true );
		return;
	}
	CItem *itemToGuard = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( itemToGuard != NULL && !itemToGuard->isPileable() )
	{
		CItem *p = getPack( mChar );
		if( p != NULL )
		{
			if( itemToGuard->GetCont() == p->GetSerial() || itemToGuard->GetCont() == mChar->GetSerial() )
			{
				itemToGuard->SetGuarded( true );
				petGuarding->SetGuarding( calcItemFromSer( itemToGuard->GetSerial() ) );
			}
		}
		else
		{
			CMultiObj *multi = findMulti( itemToGuard->GetX(), itemToGuard->GetY(), itemToGuard->GetZ(), itemToGuard->WorldNumber() );
			if( multi != NULL )
			{
				if( multi->GetOwner() == mChar->GetSerial() )
				{
					itemToGuard->SetGuarded( true );
					petGuarding->SetGuarding( itemToGuard->GetSerial() );
				}
			}
			else
				sysmessage( s, 1628 );
		}
	}
}

void cTargets::ResurrectionTarget( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		NpcResurrectTarget( i );	// npcresurrect checks to see it's valid
}

void cTargets::HouseLockdown( cSocket *s ) // Abaddon
// PRE:		S is the socket of a valid owner/coowner and is in a valid house
// POST:	either locks down the item, or puts a message to the owner saying he's a moron
// CODER:	Abaddon
// DATE:	17th December, 1999
{
	CItem *itemToLock = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( itemToLock != NULL )
	{
		CItem *house = calcItemObjFromSer( s->AddID() );
		// time to lock it down!
		CMultiObj *multi = findMulti( itemToLock->GetX(), itemToLock->GetY(), itemToLock->GetZ(), itemToLock->WorldNumber() );
		if( multi != NULL )
		{
			if( multi != house )
			{
				sysmessage( s, 1105 );
				return;
			}
			if( !itemToLock->CanBeLockedDown() )
			{
				sysmessage( s, 1106 );
				return;
			}
			itemToLock->LockDown();
			RefreshItem( itemToLock );
			return;
		}
		// not in a multi!
		sysmessage( s, 1107 );
	}
	else
		sysmessage( s, 1108 );
}

void cTargets::HouseRelease( cSocket *s ) // Abaddon
// PRE:		S is the socket of a valid owner/coowner and is in a valid house, the item is locked down
// POST:	either releases the item from lockdown, or puts a message to the owner saying he's a moron
// CODER:	Abaddon
// DATE:	17th December, 1999
{
	CItem *itemToLock = calcItemObjFromSer( s->GetDWord( 7 ) );

	if( itemToLock != NULL )
	{
		CItem *house = calcItemObjFromSer( s->AddID() );	// let's find our house
		// time to lock it down!
		CMultiObj *multi = findMulti( itemToLock->GetX(), itemToLock->GetY(), itemToLock->GetZ(), itemToLock->WorldNumber() );
		if( multi != NULL )
		{
			if( multi != house )
			{
				sysmessage( s, 1109 );
				return;
			}
			itemToLock->SetMagic( 0 );	// Default as stored by the client, perhaps we should keep a backup?
			RefreshItem( itemToLock );
			return;
		}
		// not in a multi!
		sysmessage( s, 1107 );
	}
	else
		sysmessage( s, 1108 );
}

void cTargets::ShowDetail( cSocket *s ) // Abaddon
// PRE:		S is the socket of the person getting the item information
// POST:	prints out detailed information about an item
// CODER:	Abaddon
// DATE:	11th January, 2000
{
	char message[512];
	CItem *itemToCheck = calcItemObjFromSer( s->GetDWord( 7 ) );

	if( itemToCheck != NULL )
	{
		switch( itemToCheck->GetType() )
		{
		case 0:
			strcpy( message, "Default type" );
			break;
		case 1:	// container/backpack
			strcpy( message, "Container/backpack:" );
			if( itemToCheck->GetMoreB( 1 ) > 0 )
				strcat( message, "Magically trapped" );
			break;
		case 2:	// opener for castle gate 1
			strcpy( message, "Opener for order gate" );
			break;
		case 3:	// castle gate 1
			strcpy( message, "Order gate" );
			break;
		case 4:	// opener for castle gate 2
			strcpy( message, "Opener for Chaos gate" );
			break;
		case 5:	// castle gate 2
			strcpy( message, "Chaos gate" );
			break;
		case 6:	// teleporter rune
			strcpy( message, "Teleporter rune, acts like teleport was cast" );
			break;
		case 7:	// key
			strcpy( message, "Key" );
			break;
		case 8:	// locked container
			strcpy( message, "Locked container:" );
			if( itemToCheck->GetMoreB( 1 ) > 0 )
				strcat( message, "Magically trapped" );
			break;
		case 9:	// Spellbook (item 14FA)
			strcpy( message, "Spellbook" );
			break;
		case 10:	// map( item 0E EB )
			strcpy( message, "This opens a map based on the serial num of the item" );
			break;
		case 11:	// book
			sprintf( message, "A book:Entry in misc.scp: %li", itemToCheck->GetMore() );
			break;
		case 12:	// doors
			strcpy( message, "Unlocked door" );
			break;
		case 13:	// locked door
			strcpy( message, "Locked door" );
			break;
		case 14:	// food
			strcpy( message, "Food item, reduces hunger by one point" );
			break;
		case 15:	// magic wand
			sprintf( message, "Magic wand\nCircle: %i:Spell within circle: %i:Charges left: %i", itemToCheck->GetMoreX(), itemToCheck->GetMoreY(), itemToCheck->GetMoreZ() );
			break;
		case 16:	// resurrection object
			strcpy( message, "Resurrection object" );
			break;
		case 17:	// full mortar (alchemy)
			strcpy( message, "Full mortar/alchemy, more info at a later date" );
			break;
		case 18:	// enchanted item (Crystal ball)
			strcpy( message, "Enchanted item that displays a random message" );
			break;
		case 19:	// potion
			strcpy( message, "Potion: " );
			switch( itemToCheck->GetMoreY() )
			{
			case 1: // Agility Potion
				switch( itemToCheck->GetMoreZ() )
				{
				case 1:		strcat( message, "Agility potion" );	break;
				case 2:		strcat( message, "Greater Agility potion" );	break;
				default:	strcat( message, "Unknown Agility potion" );	break;
				}
				break;
			case 2: // Cure Potion
				switch( itemToCheck->GetMoreZ() )
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
				switch( itemToCheck->GetMoreZ() )
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
				switch( itemToCheck->GetMoreZ() )
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
				switch( itemToCheck->GetMoreZ() )
				{
				case 1:		strcat( message, "Lesser Refresh potion" ); break;
				case 2:		strcat( message, "Refresh potion" );	break;
				default:	strcat( message, "Unknown Refresh potion" ); break;
				}
				break;
			case 8: // Strength Potion
				switch( itemToCheck->GetMoreZ() )
				{
				case 1:		strcat( message, "Lesser Strength potion" );	break;
				case 2:		strcat( message, "Strength potion" ); break;
				default:	strcat( message, "Unknown Strength potion" ); break;
				}
				break;
			case 9: // Mana Potion
				switch( itemToCheck->GetMoreZ() )
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
		case 35:	// townstone deed/stone
			if( itemToCheck->GetID() == 0x14F0 )
				strcpy( message, "Townstone deed, will make townstone" );
			else
				strcpy( message, "Townstone, use to find out information on the town" );
			break;
		case 50:	// recall rune
			if( itemToCheck->GetMoreX() == 0 && itemToCheck->GetMoreY() == 0 && itemToCheck->GetMoreZ() == 0 )	// changed, to fix, Lord Vader
				strcpy( message,"Unmarked recall rune");
			else
				strcpy( message, "This will rename a recall rune" );
			break;
		case 51:	// start gate
			CItem *otherGate1;
			otherGate1 = calcItemObjFromSer( itemToCheck->GetMoreX() );
			sprintf( message, "Start gate going to X %i Y %i Z %i", otherGate1->GetX(), otherGate1->GetY(), otherGate1->GetZ() );
			break;
		case 52:	// end gate
			CItem *otherGate2;
			otherGate2 = calcItemObjFromSer( itemToCheck->GetMoreX() );
			sprintf( message, "End gate going to X %i Y %i Z %i", otherGate2->GetX(), otherGate2->GetY(), otherGate2->GetZ() );
			break;
		case 60:	// object teleporter
			sprintf( message, "A teleporter going to X %i Y %i Z %i", itemToCheck->GetMoreX(), itemToCheck->GetMoreY(), itemToCheck->GetMoreZ() );
			break;
		case 61:	// item spawner
			sprintf( message, "Item spawner:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", itemToCheck->GetMoreX(), itemToCheck->GetMoreZ(), itemToCheck->GetMoreY() );
			break;
		case 62:	// monster/npc spanwer
			sprintf( message, "Monster/NPC spawner:Amount: %i:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", itemToCheck->GetAmount(), itemToCheck->GetMoreX(), itemToCheck->GetMoreZ(), itemToCheck->GetMoreY() );
			break;
		case 63:	// spawn container
			strcpy( message, "Item Spawn container:" );
			if( itemToCheck->GetMoreB( 1 ) > 0 )
				strcat( message, "Magically trapped:" );
			sprintf( message, "%sRespawn max time: %i:Respawn min time: %i", message, itemToCheck->GetMoreZ(), itemToCheck->GetMoreY() );
			break;
		case 64:	// locked spawn container
			strcpy( message, "Locked item spawn container:" );
			if( itemToCheck->GetMoreB( 1 ) > 0 )
				strcat( message, "Magically trapped:" );
			sprintf( message, "%sRespawn max time: %i:Respawn min time: %i", message, itemToCheck->GetMoreZ(), itemToCheck->GetMoreY() );
			break;
		case 65:	// unlockable item spawner container
			strcpy( message, "Unlockable item spawner container" );
			break;
		case 69:	// area spawner
			sprintf( message, "Area spawner:X +/- value: %i:Y +/- value: %i:Amount: %i:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", itemToCheck->GetMore( 3 ), itemToCheck->GetMore( 4 ), itemToCheck->GetAmount(), itemToCheck->GetMoreX(), itemToCheck->GetMoreZ(), itemToCheck->GetMoreY() );
			break;
		case 80:	// single use advancement gate
			sprintf( message, "Single use advancement gate: advance.scp entry %i", itemToCheck->GetMoreX() );
			break;
		case 81:	// multi-use advancement gate
			sprintf( message, "Multi use advancement gate: advance.scp entry %i", itemToCheck->GetMoreX() );
			break;
		case 82:	// monster gate
			sprintf( message, "Monster gate: monster number %i", itemToCheck->GetMoreX() );
			break;
		case 83:	// race gates
			sprintf( message, "Race Gate:Turns into %s:", Races->Name( itemToCheck->GetMoreX() ) );
			if( itemToCheck->GetMoreY() == 1 )
				strcat( message, "Constantly reuseable:" );
			else
				strcat( message, "One time use only:" );
			if( Races->IsPlayerRace( itemToCheck->GetMoreX() ) )
				strcat( message, "Player Race:" );
			else
				strcat( message, "NPC Race only:" );
			break;
		case 85:	// damage object
			sprintf( message, "Damage object:Minimum damage %i:Maximum Damage %i", itemToCheck->GetMoreX() + itemToCheck->GetMoreY(), itemToCheck->GetMoreX() + itemToCheck->GetMoreZ() );
			break;
		case 86:	// sound object
			sprintf( message, "Sound object:Percent chance of sound being played: %i:Sound effect to play: %i %i", itemToCheck->GetMoreZ(), itemToCheck->GetMoreX(), itemToCheck->GetMoreY() );
			break;
		case 87:	// trash container
			strcpy( message, "A trash container" );
			break;
		case 88:	// sound object
			sprintf( message, "Sound object that plays whenever someone is near:Soundeffect: %i\nRadius: %i\nProbability: %i", itemToCheck->GetMoreX(), itemToCheck->GetMoreY(), itemToCheck->GetMoreZ() );
			break;
		case 89:	// map change
			sprintf( message, "Map Change object that changes world:World: %i", itemToCheck->GetMore() );
			break;
		case 100:	// not documented
			strcpy( message, "Looks like hide/unhide object:More detail to come later\n" );
			break;
		case 101:	// morph object morex = npc# in npc.scp
			sprintf( message, "Morph object:Morphs char into body %x %x", itemToCheck->GetMoreX()>>8, itemToCheck->GetMoreX()%256 );
			break;
		case 102:	// unmorph
			strcpy( message, "Unmorph object, unmorphs back to body before using it as type 101, switches to 101 again" );
			break;
		case 103:	// army enlistment object
			sprintf( message, "Army enlistment object: Army #%i", itemToCheck->GetMoreX() );
			break;
		case 104:	// teleport object (use triggers now)
			sprintf( message, "Teleport object: X %i Y %i Z %i", itemToCheck->GetMoreX(), itemToCheck->GetMoreY(), itemToCheck->GetMoreZ() );
			break;
		case 105:	// drink object
			strcpy( message, "You can drink this" );
			break;
		case 117:	// backpack?
			strcpy( message, "I believe that this is a backpack, though it could be ship related" );
			break;
		case 125:	// escort npc spawner
			strcpy( message, "Escort NPC spawner, behaves like type 62/69" );
			break;
		case 181:	// fireworks wand
			sprintf( message, "A fireworks wand with %i charges left on it", itemToCheck->GetMoreX() );
			break;
		case 185:	// smoking type
			sprintf( message, "Smoking object: Duration %i secs", itemToCheck->GetMoreX() );
			break;
		case 202:	// guildstone deed
			strcpy( message, "Guildstone deed" );
			break;
		case 203:	// opens gump menu
			strcpy( message, "Opens housing gump: " );
			break;
		case 217:	// player vendor deed
			strcpy( message, "Player vendor deed");
			break;
		case 234:	// world change gate
			sprintf( message, "World change gate: %i", itemToCheck->GetMoreX() );
			break;
		}
		sysmessage( s, message );
	}
	else
		sysmessage( s, 1656 );
}

void cTargets::CommandLevel( cSocket *s )
{
	CChar *targToEdit = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( targToEdit != NULL )
		targToEdit->SetCommandLevel( s->AddX() );
}

void cTargets::MakeTownAlly( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	CChar *targetChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( targetChar == NULL )
	{
		sysmessage( s, 1110 );
		return;
	}
	UI08 srcTown = mChar->GetTown();
	UI08 trgTown = targetChar->GetTown();

//	if( srcTown == trgTown )
//		sysmessage( s, "That person already belongs to your town" );
//	else
//	{
		if( !region[srcTown]->MakeAlliedTown( trgTown ) )	
			sysmessage( s, 1111 );
//	}
}

void cTargets::MakeStatusTarget( cSocket *sock )
{
	CChar *targetChar = calcCharObjFromSer( sock->GetDWord( 7 ) );
	if( targetChar == NULL )
	{
		sysmessage( sock, 1110 );
		return;
	}
	UI08 origCommand = targetChar->GetCommandLevel();

	commandLevel_st *targLevel = Commands->GetClearance( sock->XText() );
	commandLevel_st *origLevel = Commands->GetClearance( origCommand );
	
	if( targLevel == NULL )
	{
		sysmessage( sock, 1112 );
		return;
	}
	CChar *mChar = sock->CurrcharObj();
	char temp[1024], temp2[1024];

	UI08 targetCommand = targLevel->commandLevel;
	sprintf( temp, "account%i.log", mChar->GetAccount() );
	sprintf( temp2, "%s has made %s a %s.\n", mChar->GetName(), targetChar->GetName(), targLevel->name );
	savelog( temp2, temp );

	DismountCreature( targetChar );

	if( targLevel->targBody != 0 )
	{
		targetChar->SetID( targLevel->targBody );
		targetChar->SetSkin( targLevel->bodyColour );
		targetChar->SetxID( targLevel->targBody );
		targetChar->SetOrgID( targLevel->targBody );
		targetChar->SetxSkin( targLevel->bodyColour );
	}

	targetChar->SetPriv( (targLevel->defaultPriv)%256 );
	targetChar->SetPriv2( (targLevel->defaultPriv)>>8 );
	targetChar->SetCommandLevel( targetCommand );
	
	if( targLevel->allSkillVals != 0 )
	{
		for( int j = 0; j < TRUESKILLS; j++ )
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
		targetChar->SetMana2( 100 );
		targetChar->SetStamina2( 100 );
	}
	char *playerName = (char *)targetChar->GetName();

	if( targetCommand != 0 && targetCommand != origCommand )
	{
		if( origLevel != NULL )
		{	// Strip name off it
			if( !strnicmp( origLevel->name, playerName, strlen( origLevel->name ) ) )
				playerName += ( strlen( origLevel->name ) + 1 );
		}
		sprintf( temp, "%s %s", targLevel->name, playerName );
		targetChar->SetName( temp );
	}
	else if( origCommand != 0 )
	{
		if( origLevel != NULL )
		{	// Strip name off it
			if( !strnicmp( origLevel->name, playerName, strlen( origLevel->name ) ) )
				playerName += ( strlen( origLevel->name ) + 1 );
		}
		strcpy( temp, playerName );
		targetChar->SetName( temp );
	}
	CItem *retitem = NULL;
	CItem *mypack = getPack( targetChar );

	if( targLevel->stripOff )
	{
		for( UI08 lyrCounter = 0; lyrCounter < MAXLAYERS; lyrCounter++ )
		{
			CItem *z = targetChar->GetItemAtLayer( lyrCounter );
			if( z != NULL )
			{
				switch( lyrCounter )
				{
				case 0x0B:
				case 0x10:
					Items->DeleItem( z );
					break;
				case 0x15:
				case 0x1D:
					break;
				default:
					if( mypack == NULL )
						mypack = getPack( targetChar );
					if( mypack == NULL )
					{
						CItem *iMade = Items->SpawnItem( calcSocketObjFromChar( targetChar ), targetChar, 1, "#", false, 0x0E75, 0, false, false );
						targetChar->SetPackItem( iMade );
						if( iMade == NULL ) 
							return;
						iMade->SetLayer( 0x15 );
						if( iMade->SetCont( targetChar->GetSerial() ) )
						{
							iMade->SetType( 1 );
							iMade->SetDye( true );
							mypack = iMade;
							retitem = iMade;
						}
					}
					z->SetX( ( RandomNum( 0, 79 ) + 50 ) );
					z->SetY( ( RandomNum( 0, 79 ) + 50 ) );
					z->SetZ( 9 );
					z->SetCont( mypack->GetSerial() );

					CPRemoveItem toRemove = (*z);
					
					Network->PushConn();
					for( cSocket *cSock = Network->FirstSocket(); !Network->FinishedSockets(); cSock = Network->NextSocket() )
						cSock->Send( &toRemove );
					Network->PopConn();
					RefreshItem( z );
					break;
				}
			}
		}
	}
	if( targLevel->targBody != 0 )
		targetChar->Teleport();
	else
		targetChar->Update();
}

void cTargets::SmeltTarget( cSocket *s )
{
#pragma note( "Smelting needs to be heavily updated" )
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL || i->GetCont() == INVALIDSERIAL )
		return;
	if( i->GetCreator() == INVALIDSERIAL )
	{
		sysmessage( s, 1113 );
		return;
	}
	SI32 iMadeFrom = i->EntryMadeFrom();

	createEntry *ourCreateEntry = Skills->FindItem( iMadeFrom );
	if( iMadeFrom == -1 || ourCreateEntry == NULL )
	{
		sysmessage( s, 1114 );
		return;
	}

	CChar *mChar = s->CurrcharObj();

	R32 avgMin = ourCreateEntry->AverageMinSkill();
	if( mChar->GetSkill( MINING ) < avgMin )
	{
		sysmessage( s, 1115 );
		return;
	}
	R32 avgMax = ourCreateEntry->AverageMaxSkill();

	Skills->CheckSkill( mChar, MINING, (int)avgMin, (int)avgMax );

	R32 sumAmountRestored = 0.0f;

	for( UI32 skCtr = 0; skCtr < ourCreateEntry->resourceNeeded.size(); skCtr++ )
	{
		UI16 amtToRestore = ourCreateEntry->resourceNeeded[skCtr].amountNeeded / 2;
		UI16 itemID = ourCreateEntry->resourceNeeded[skCtr].itemID;
		UI16 itemColour = ourCreateEntry->resourceNeeded[skCtr].colour;
		sumAmountRestored += amtToRestore;
		Items->SpawnItem( s, mChar, amtToRestore, "#", true, itemID, itemColour, true, true );
	}

	sysmessage( s, 1116, sumAmountRestored );
	Items->DeleItem( i );
}

void cTargets::IncZTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		i->IncZ( s->AddX() );
		RefreshItem( i );
		return;
	}
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( c != NULL )
	{
		c->SetLocation( c->GetX(), c->GetY(), c->GetZ() + s->AddX() );
		c->Teleport();
	}
}

void cTargets::DeleteCharTarget( cSocket *s )
{
	CChar *c = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( s->CurrcharObj() == c )
	{
		sysmessage( s, "You cannot delete yourself" );
		return;
	}
	if( c != NULL )
	{
		if( c->IsNpc() )
		{
			sysmessage( s, 1658 );
			return;
		}
		if( isOnline( c ) )
		{
			cSocket *tSock = calcSocketObjFromChar( c );
			sysmessage( tSock, 1659 );
			Network->Disconnect( calcSocketFromSockObj( tSock ) );
		}
		Npcs->DeleteChar( c );
	}
}
