//------------------------------------------------------------------------
//  targeting.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
#include <uox3.h>
#include <debug.h>
#include <cmdtable.h> // who took this out and why?? cj 8/11/99

#define DBGFILE "targeting.cpp"

void cTargets::PlVBuy(int s)//PlayerVendors
{
	int v = addx[s];
	if (v < 0 || v>cmem|| s==-1)
		return;
	if (chars[v].free)
		return;
	int serial, i, gleft = calcgold(currchar[s]), price;
	int p = packitem(currchar[s]);
	int np, npc;
	if (p==-1)
	{
		sysmessage(s, "Time to buy a backpack");
		return;
	}

	serial = calcserial(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	i = findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i==-1)
		return;
	if (items[i].contserial==-1)
		return;

	price = items[i].value;

	np = findbyserial(&itemsp[items[i].contserial%HASHMAX], items[i].contserial, 0);
	npc = GetPackOwner(np);
	if (npc != v || chars[v].npcaitype != 17)
		return;

	if (chars[currchar[s]].serial == chars[v].ownserial)
	{	
		npctalk(s, v, "I work for you, you need not buy things from me!", 0);
		return;
	}

	p = packitem(currchar[s]);
	if (p < 0)
		return;
	if (gleft < items[i].value)
	{
		npctalk(s, v, "You cannot afford that.", 0);
		return;
	}
	else
	{
	// This portion of code is grabbed from void buyaction(int s) in uox3.cpp
	int tAmount = 0;
	tAmount = delequan(currchar[s], 0x0E, 0xED, items[i].value);
	// tAmount > 0 indicates there wasn't enough money...
	// could be expanded to take money from bank too...
	}
	npctalk(s, v, "Thank you.", 0);
	chars[v].holdg += items[i].value;
	unsetserial(i, 1);
	setserial(i, p, 1);
	RefreshItem(i);
	Weight->NewCalc(currchar[s]);
	statwindow(s, currchar[s]);
}

void cTargets::MultiTarget(int s) // If player clicks on something with the targetting cursor
{
	int a1, a2, a3, a4, i;
	if(buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) 
	{
		if( chars[currchar[s]].spellCast != 0 )	// need to stop casting if we don't target right
		{
			chars[currchar[s]].spellCast = 0;
			chars[currchar[s]].casting = 0;
		}
		return; // do nothing if user cancels, avoids CRASH! - Morrolan
	}
	
	a1=buffer[s][2];
	a2=buffer[s][3];
	a3=buffer[s][4];
	a4=buffer[s][5];
	targetok[s]=0;
	if( chars[currchar[s]].dead && !( chars[currchar[s]].priv&1 ) && chars[currchar[s]].account != 0 )
	{
		if( chars[currchar[s]].priv&1 )
		{
		} 
		else
		{
			sysmessage( s, "You are dead and cannot do that!" );
			if( chars[currchar[s]].spellCast != 0 )	// need to stop casting if we don't target right
			{
				chars[currchar[s]].spellCast = 0;
				chars[currchar[s]].casting = 0;
			}
			return;
		}
	}
 	if ((a1==0)&&(a2==1)&&(a3==0))
	{
		switch(a4)
		{
		case 0: Targ->AddTarget( s ); break;
		case 1: Targ->RenameTarget( s ); break;
		case 2: Targ->TeleTarget( s ); break;
		case 3: Targ->RemoveTarget( s ); break;
		case 4: Targ->DyeTarget( s ); break;
		case 5: Targ->NewzTarget( s ); break;
		case 6: Targ->TypeTarget( s ); break;
		case 7: Targ->IDtarget( s ); break;
		case 8: Targ->XgoTarget( s ); break;
		case 9: Targ->PrivTarget( s ); break;
		case 10: Targ->MoreTarget( s ); break;
		case 11: Targ->KeyTarget( s ); break;
		case 12: Targ->IstatsTarget( s ); break;
		case 13: Targ->CstatsTarget( s ); break;
		case 14: Targ->GMTarget( s ); break;
		case 15: Targ->CnsTarget( s ); break;
		case 16: Targ->KillTarget( s, 0x0b ); break;
		case 17: Targ->KillTarget( s, 0x10 ); break;
		case 18: Targ->KillTarget( s, 0x15 ); break;
		case 19: Targ->FontTarget( s ); break;
		case 20: Targ->GhostTarget( s ); break;
		case 21: Targ->ResurrectionTarget( s ); break; // needed for /resurrect command
		case 22: Targ->BoltTarget( s ); break;
		case 23: Targ->AmountTarget( s ); break;
		case 24:
			{
				int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
				i=findbyserial(&itemsp[serial%HASHMAX],serial,0);
				if(i!=-1)
				{
					triggerwitem(s,i,0);
					chars[currchar[s]].envokeid1=0x00;
					chars[currchar[s]].envokeid2=0x00;
					return;
					//}  for
				}
				serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
				i = findbyserial( &charsp[serial%HASHMAX], serial, 1 );
				if( i != -1 )
				{
					triggernpc( s, i );
					chars[currchar[s]].envokeid1 = 0x00;
					chars[currchar[s]].envokeid2 = 0x00;
					return;
				}
				triggerwitem(s,-1,0);
				chars[currchar[s]].envokeid1=0x00;
				chars[currchar[s]].envokeid2=0x00;
				return;
			}
		case 25: CloseTarget(s); break;
		case 26: AddMenuTarget(s, 1, addmitem[s]); break;
		case 27: NpcMenuTarget(s); break;
		case 28: MovableTarget(s); break;
		case 29: Skills->ArmsLoreTarget(s); break;
		case 30: OwnerTarget(s); break;
		case 31: Targ->ColorsTarget(s); break;
		case 32: Targ->DvatTarget(s); break;
		case 33: AddNpcTarget(s); break;
		case 34: FreezeTarget(s); break;
		case 35: UnfreezeTarget(s); break;
		case 36: AllSetTarget(s); break;
		case 37: Skills->AnatomyTarget(s); break;
		case 38: Magic->Recall(s); break;					
		case 39: Magic->Mark(s); break;					
		case 40: Skills->ItemIdTarget(s); break;
		case 41: Skills->Evaluate_int_Target(s); break;
		case 42: Skills->TameTarget(s); break;
		case 43: Magic->Gate(s); break;					
		case 44: Magic->Heal(s); break; // we need this for /heal command
		case 45: Skills->FishTarget(s); break;
		case 46: InfoTarget(s); break;
		case 47: TitleTarget(s); break;
		case 48: ShowDetail( s ); break;
		case 49: Skills->Repair( s );	break;							// repairing
		case 50: Skills->Smith( s );	break;
		case 51: Skills->Mine( s );		break;
		case 52: Skills->SmeltOre( s );	break;
		case 53: npcact(s); break;
		case 54: GetAccount( s ); break;
		case 55: WstatsTarget(s); break;
		case 56: NpcTarget(s); break;
		case 57: NpcTarget2(s); break;
		case 58: NpcResurrectTarget( currchar[s] ); break;
		case 59: NpcCircleTarget(s); break;
		case 60: NpcWanderTarget(s); break;
		case 61: VisibleTarget(s); break;
		case 62: TweakTarget(s); break;
		case 63: MoreXTarget(s); break;
		case 64: MoreYTarget(s); break;
		case 65: MoreZTarget(s); break;
		case 66: MoreXYZTarget(s); break;
		case 67: NpcRectTarget( s ); break;
// FREE		case 67: break;
// FREE		case 68: break;
// FREE		case 69: break;
// FREE		case 70: break;
// FREE		case 71: break;
// FREE		case 72: break;
// FREE		case 73: break;
// FREE		case 74: break;
// FREE		case 75: break;
		case 76: AxeTarget(s); break;
		case 77: Skills->DetectHidden(s); break;
// FREE		case 78: break;
		case 79: Skills->ProvocationTarget1(s); break;
		case 80: Skills->ProvocationTarget2(s); break;
		case 81: Skills->EnticementTarget1(s); break;
		case 82: Skills->EnticementTarget2(s); break;
// FREE		case 83: break;
// FREE		case 84: break;
// FREE		case 85: break;
		case 86: Targ->SwordTarget(s); break;
		case 87: Magic->SbOpenContainer(s); break;
		case 88: Targ->SetDirTarget(s); break;
		case 89: Targ->ObjPrivTarget(s); break;
// FREE		case 90: break;
// FREE		case 91: break;
// FREE		case 92: break;
// FREE		case 93: break;
// FREE		case 94: break;
// FREE		case 95: break;
// FREE		case 96: break;
// FREE		case 97: break;
// FREE		case 98: break;
// FREE		case 99: break;
// FREE		case 100: break;		// we now have this as our new spell targeting location
		case 100: Magic->NewCastSpell( s ); break;
		case 101: CommandLevelTarget( s );	break;
// FREE		case 102: break;
// FREE		case 103: break;
// FREE		case 104: break;
// FREE		case 105: break;
		case 106: NpcAITarget( s );				break;
		case 107: xBankTarget( s );				break;
		case 108: Skills->AlchemyTarget( s );	break;
		case 109: Skills->BottleTarget( s );	break;
		case 110: DupeTarget( s );				break;
		case 111: MoveToBagTarget( s );			break;
		case 112: SellStuffTarget( s );			break;
		case 113: ManaTarget( s );				break;
		case 114: StaminaTarget( s );			break;
		case 115: GmOpenTarget( s );			break;
		case 116: MakeShopTarget( s );			break;
		case 117: FollowTarget( s );			break;
		case 118: AttackTarget( s );			break;
		case 119: TransferTarget( s );			break;
		case 120: GuardTarget( s );				break;
		case 121: BuyShopTarget( s );			break;
		case 122: SetValueTarget( s );			break;
		case 123: SetRestockTarget( s );		break;
// FREE		case 124: break;
// FREE		case 125: break;
		case 126: Targ->JailTarget(s,-1); break;
		case 127: Targ->ReleaseTarget(s,-1); break;
		case 128: Skills->CreateBandageTarget(s); break;
		case 129: Targ->SetAmount2Target(s); break;
		case 130: Skills->HealingSkillTarget(s); break;
		case 131: Targ->permHideTarget(s); break; /* not used */
		case 132: Targ->unHideTarget(s); break; /* not used */
		case 133: Targ->SetWipeTarget(s); break;
		case 134: Skills->Carpentry(s); break;
		case 135: Targ->SetSpeechTarget(s); break;
		case 136: Targ->XTeleport(s,0); break;
// FREE		case 137: break;	// AntiChrist (9/99)
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
		case 150: Targ->SetSpAttackTarget(s); break;
		case 151: Targ->FullStatsTarget(s); break;
		case 152: Skills->BeggingTarget(s); break;
		case 153: Skills->AnimalLoreTarget(s); break;
		case 154: Skills->ForensicsTarget(s); break;
		case 155: 
			{
				chars[currchar[s]].poisonserial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
				target(s, 0, 1, 0, 156, "What item do you want to poison?");
				return;
			}
		case 156: Skills->PoisoningTarget(s); break;
// FREE		case 157: break;
// FREE		case 158: break;
// FREE		case 159: break;
		case 160: Skills->Inscribe(s,0); break;
		case 161: Towns->VoteForMayorTarget(s); break;
		case 162: Skills->LockPick(s); break;
//FREE			   case 163: smeltgold(s); break;
		case 164: Skills->Wheel(s, YARN); break;
		case 165: Skills->Loom(s); break;
		case 166: Skills->Wheel(s, THREAD); break;
		case 167: Skills->Tailoring(s); break;
		case 168: Skills->CookMeat(s); break;
//FREE		case 169: Magic->CannonTarget(s); break;
		case 170: Targ->LoadCannon(s); break;
//FREE		case 171: Magic->BuildCannon(s); break;
		case 172: Skills->Fletching(s); break;
		case 173: Skills->MakeDough(s); break;
		case 174: Skills->MakePizza(s); break;
		case 175: Targ->SetPoisonTarget(s); break;
		case 176: Targ->SetPoisonedTarget(s); break;
		case 177: Targ->SetSpaDelayTarget(s); break;
		case 178: Targ->SetAdvObjTarget(s); break;
		case 179: Targ->SetInvulFlag(s); break; /* not used ...*/
		case 180: Skills->Tinkering(s); break;
		case 181: Skills->PoisoningTarget(s); break;  
//FREE		case 182: Magic->CureTarget(s); break;
		case 183: Skills->TinkerAxel(s); break;
		case 184: Skills->TinkerAwg(s); break;
		case 185: Skills->TinkerClock(s); break;
		case 186: vialtarget( s ); break;
//FREE		case 187: break;
//FREE		case 188: Magic->ManaDrainTarget(s); break;
//FREE		case 189: Magic->ManaVampireTarget(s); break;
//FREE		case 190: Magic->MeteorSwarmPTarget(s); break;
//FREE		case 191: Magic->ChainLightningTarget(s); break;
//FREE		case 192: Magic->ArchCureTarget(s); break;
//FREE		case 193: Magic->MassCurseTarget(s); break;
//FREE		case 194: Magic->MassDispelTarget(s); break;
//FREE		case 195: Magic->MagicLockTarget(s); break;
//FREE		case 196: Magic->MagicTrapTarget(s); break;
//FREE		case 197: Magic->MagicUntrapTarget(s); break;
		case 198: Targ->Tiling(s); break;
		case 199: Targ->Wiping(s); break;
		case 200: Commands->SetItemTrigger(s); break;
		case 201: Commands->SetNPCTrigger(s); break;
		case 202: Commands->SetTriggerType(s); break;
		case 203: Commands->SetTriggerWord(s); break;
		case 204: triggertarget(s); break; // Fixed by Magius(CHE)
		case 205: Skills->StealingTarget(s); break;
		case 206: Targ->CanTrainTarget(s); break;
		case 207: Targ->ExpPotionTarget(s); break;
		case 209: Targ->SetSplitTarget(s); break;
		case 210: Targ->SetSplitChanceTarget(s); break;
		case 212: Commands->Possess(s); break;
// FREE		case 213: break;
// FREE		case 214: break;
// FREE		case 215: break;
// FREE		case 216: break;
// FREE		case 217: break;
// FREE		case 218: break;
// FREE		case 219: break;
		case 220: Guilds->Recruit(s); break;
		case 221: Guilds->TargetWar(s); break;
		case 222: Targ->TeleStuff(s); break;        
		case 223: Targ->SquelchTarg(s); break;//Squelch
		case 224: Targ->PlVBuy(s); break;//PlayerVendors
		
//		case 225: Targ->Priv3XTarget(s); break; // SETPRIV3 +/- target
		case 227: Targ->HouseOwnerTarget(s); break; // cj aug11/99
		case 228: Targ->HouseEjectTarget(s); break; // cj aug11/99
		case 229: Targ->HouseBanTarget(s); break; // cj aug12/99
		case 230: Targ->HouseFriendTarget(s); break; // cj aug 12/99
		case 231: Targ->HouseUnlistTarget(s); break; // cj aug 12/99
		case 232: Targ->HouseLockdown( s ); break; // Abaddon 17th December 1999
		case 233: Targ->HouseRelease( s ); break; // Abaddon 17th December 1999
// FREE		case 234: break;
		
		case 235: BanTarg(s); break;
// FREE		case 236: break;
// FREE		case 237: break;
// FREE		case 238: break;
// FREE		case 239: break;
		case 240: SetMurderCount( s ); break; // Abaddon 13 Sept 1999
// FREE		case 241: break;
// FREE		case 242: break;
// FREE		case 243: break;
// FREE		case 244: break;
// FREE		case 245: break;
// FREE		case 246: break;
		case 247: Targ->ShowSkillTarget( s ); break; // showskill target
//		case 248: Targ->MenuPrivTarg( s ); break; //menupriv target
		case 249: Targ->UnglowTarget( s ); break; // unglow

		case 250: IncZTarget( s );	break; // INCZ
		case 251: NewXTarget( s );	break; // NEWX
		case 252: NewYTarget( s );	break; // NEWY
		case 253: IncXTarget( s );	break; // INCX
		case 254: IncYTarget( s );	break; // INCY
		case 255: GlowTarget( s );	break; // glow
			
		default:
			printf("ERROR: Fallout of switch statement (%i) without default. targeting.cpp, multitarget()\n", a4); //Morrolan 
		}
	}
}

//o---------------------------------------------------------------------------o
//|    Function       - void triggertarget( int s )
//|    Date           - 24 August 1999
//|    Programmer     - Magius(CHE)
//o---------------------------------------------------------------------------o
//|    Purpose        - Select an item or an npc to set with new trigger.
//o---------------------------------------------------------------------------o
//s: socket!! -- AntiChrist
void cTargets::triggertarget( int s )
{
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] ); 
	int i;
	i = findbyserial( &charsp[serial%HASHMAX], serial, 1 );
	if( i != -1 ) // Char
	{
		// triggerwitem( i, -1, 1 ); // is this used also for npcs?!?!
	}
	else
	{ // item
		i = findbyserial( &itemsp[serial%HASHMAX], serial, 0 );
		if( i != -1 )
		{
			triggerwitem( s, i, 1 );
		}
	}
}

void cTargets::BanTarg(int s)
// PARAM WARNING: s is an unreferenced parameter
{

}

//o---------------------------------------------------------------------------o
//|   Function    -  void addtarget(int s)
//|   Date        -  UnKnown
//|   Programmer  -  UnKnown  (Touched tabstops by Tauriel Dec 29, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds an item when using /add # # .
//o---------------------------------------------------------------------------o
void cTargets::AddTarget(int s)
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return;
	int /*j,*/c,pileable=0;
	tile_st tile;
	
	if (addid1[s]==0x40)
	{
		switch (addid2[s])
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
			buildhouse(s,addid3[s]);//If its a valid house, send it to buildhouse!
			return; // Morrolan, here we WANT fall-thru, don't mess with this switch
		}
	}
	Map->SeekTile( (addid1[s]<<8)+addid2[s], &tile);
	if (tile.flag2&0x08) pileable=1;
	
	c=Items->SpawnItem(s, 1, "#", pileable, addid1[s], addid2[s], 0, 0, 0,0);
	if( c == -1 ) return;
	items[c].priv=0;	//Make them not decay
	items[c].x=(buffer[s][11]<<8)+buffer[s][12];
	items[c].y=(buffer[s][13]<<8)+buffer[s][14];
	items[c].z=buffer[s][16]+Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
	
	// auto settype on doors - fur 11/5/1999
/*        for (int i = 0; i < DOORTYPES; ++i)
        {
	  if (itemid >= doorbase[i] && itemid < doorbase[i] + 16)
            {
	      items[c].type = 12;
	      break;
	    }
	}*/

	if (items[c].contserial==-1) mapRegions->AddItem(c);  // Add item to mapRegions
//	for (j=0;j<now;j++) if (perm[j]) senditem(j,c);
	RefreshItem( c ); // AntiChrist
	addid1[s]=0;
	addid2[s]=0;
}

void cTargets::RenameTarget(int s)
{
	int i,serial;
	//   
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if(addx[s]==1) //rename2 //New -- Zippy
			strcpy(items[i].name2,xtext[s]);
		else
			strcpy(items[i].name,xtext[s]);
	}
	
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		strcpy(chars[i].name,xtext[s]);
	}
}

void cTargets::TeleTarget( UOXSOCKET s )
{
	int success;
	CHARACTER mChar = currchar[s];
	// Abaddon October 10th, 1999 used variables instead
	int x, y, z;
	int targX, targY;
	signed char targZ;			// was an int, should be a signed char
	x = chars[mChar].x;
	y = chars[mChar].y;
	z = chars[mChar].dispz;
	targX = (buffer[s][11]<<8) + buffer[s][12];
	targY = (buffer[s][13]<<8) + buffer[s][14];
	targZ = buffer[s][16] + Map->TileHeight( (buffer[s][17]<<8) + buffer[s][18] );
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return; // moved above if test

	if( ( chars[mChar].priv&0x01 ) || line_of_sight( s, x, y, z, targX, targY, targZ, WALLS_CHIMNEYS + DOORS + ROOFING_SLANTED ) )	
	{
		
		if (currentSpellType[s] !=2)  // not a wand cast
		{
			success = Magic->SubtractMana( mChar, 3 );  // subtract mana on scroll or spell
			if (currentSpellType == 0)             // del regs on normal spell
			{
				reag_st toDel;
				memset( &toDel, 0, sizeof( reag_st ) );
				toDel.drake = 1;
				toDel.moss = 1;
				Magic->NewDelReagents( mChar, toDel );
			}
		}
		
//		soundeffect( s, 0x01, 0xFE );
		
		mapRegions->RemoveItem( mChar + 1000000 );
		
		chars[mChar].x=targX;
		chars[mChar].y=targY;
		chars[mChar].dispz = chars[mChar].z = targZ;
		
		mapRegions->AddItem( mChar + 1000000 );
		
		teleport( mChar );
//#define __FLAMESTRIKE__
#ifdef __FLAMESTRIKE__
		staticeffect( mChar, 0x37, 0x09, 0x09, 0x19 );
#else
		staticeffect( mChar, 0x37, 0x2A, 0x09, 0x06 );
#endif
	} 
}
void cTargets::GetAccount( int s )
{
	int i, serial;
//	serial = calcserial( &buffer[s][7], &buffer[s][8], &buffer[s][9], &buffer[s][10] );
	serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	i = findbyserial( &charsp[serial%HASHMAX], serial, 1 );

	if( i != -1 )
	{
		if( !chars[i].npc )
		{
			Admin->GumpAMenu( s, i );
		}
		else sysmessage( s, "That is not a player!" );
	}
	else sysmessage( s, "That is not a player!" );
}
void cTargets::RemoveTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		sysmessage(s, "Removing item.");
		Items->DeleItem(i);
    } else {
		i=findbyserial(&charsp[serial%HASHMAX],serial,1);
		if(i!=-1)
		{
			if (chars[i].account>-1 && !chars[i].npc) // player check added by LB
			{ 
				sysmessage(s,"You cant delete players");
				return;
			}
			sysmessage(s, "Removing character.");
			Npcs->DeleteChar(i);
		}
	}
}

void cTargets::DyeTarget(int s)
{
	int body,b,i,k,serial;
	unsigned char c1, c2;	// these were ints
		
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	if ((addid1[s]==255)&&(addid2[s]==255))
	{
		i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
		if (i!=-1)
		{
			dyevat[1]=items[i].ser1;
			dyevat[2]=items[i].ser2;
			dyevat[3]=items[i].ser3;
			dyevat[4]=items[i].ser4;
			dyevat[7]=items[i].id1;
			dyevat[8]=items[i].id2;
			Network->xSend(s, dyevat, 9, 0);
//			for (k=0;k<now;k++) if (perm[k]) senditem(k, i);
			RefreshItem( i ); // AntiChrist
		}
		i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
		if (i!=-1)
		{
			dyevat[1]=chars[i].ser1;
			dyevat[2]=chars[i].ser2;
			dyevat[3]=chars[i].ser3;
			dyevat[4]=chars[i].ser4;
			dyevat[7]=0x21;
			dyevat[8]=0x06;
			Network->xSend(s, dyevat, 9, 0);
		}
	}
	else
	{
		i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
		if (i!=-1)
		{
			
			
			c1=addid1[s];  // lord binary, dying crash bugfix
			c2=addid2[s];
			
			
			if (!dyeall[s])
			{
				if ((((c1<<8)+c2)<0x0002) ||
					(((c1<<8)+c2)>0x03E9) )
				{
					c1=0x03;
					c2=0xE9;
				}
			}
			
			b=((((c1<<8)+c2)&0x4000)>>14)+((((c1<<8)+c2)&0x8000)>>15);   
			if (!b)
			{
				items[i].color1=c1;
				items[i].color2=c2;
			}
			
			
//			for (j=0;j<now;j++) if (perm[j]) senditem(j,i);	
			RefreshItem( i ); // AntiChrist
			
		}
		
		
		i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
		
		if (i!=-1)
		{
			body=(chars[i].id1<<8)+chars[i].id2;
			k=(addid1[s]<<8)+addid2[s];
			if( ( (k>>8) < 0x80 ) && body >= 0x0190 && body <= 0x0193 ) k += 0x8000;
			
			
			b=k&0x4000; 
			if (b==16384 && (body >=0x0190 && body<=0x03e1)) k=0xf000; // but assigning the only "transparent" value that works, namly semi-trasnparency.
			
			if (k!=0x8000) // 0x8000 also crashes client ...
			{	
				
				
				chars[i].skin1=(unsigned char)(k>>8);
				chars[i].skin2=(unsigned char)(k%256);
				chars[i].xskin1=(unsigned char)(k>>8);
				chars[i].xskin2=(unsigned char)(k%256);
				
				updatechar(i);
				
			}
		}
	}
	
	
}

void cTargets::NewzTarget(int s)
{
	int i/*,j*/,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].z=addx[s];
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
	}
	
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].dispz=chars[i].z=addx[s];
		teleport(i);
	}
}

void cTargets::TypeTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].type=addid1[s];
	}
}


void cTargets::IDtarget(int s)
{
	int i/*,j*/,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].id1=addid1[s];
		items[i].id2=addid2[s];
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
		return;
	}
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].id1=chars[i].xid1=chars[i].orgid1=addid1[s];
		chars[i].id2=chars[i].xid2=chars[i].orgid2=addid2[s];
		updatechar(i);
	}
}

void cTargets::XTeleport(int s, int x)
{
	int i, serial = -1;
	// Abaddon 17th February 2000 Converted if to switch (easier to read)
	// Also made it so that it calls teleport, not updatechar, else you don't get the items in range
	switch( x )
	{
	case 0:
		serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
		i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
		break;
	case 5:
		serial = calcserial( (unsigned char)hexnumber(1), (unsigned char)hexnumber(2), (unsigned char)hexnumber(3), (unsigned char)hexnumber(4) );
		i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
		break;
	case 2:
		if (perm[makenumber(1)])
		{
			i=currchar[makenumber(1)];
		}
		else return;
		break;
	case 3:
		i=chars[currchar[s]].making;
		mapRegions->RemoveItem(currchar[i]+1000000); //lb
		chars[currchar[i]].x=chars[currchar[s]].x;
		chars[currchar[i]].y=chars[currchar[s]].y;
		chars[currchar[i]].dispz=chars[currchar[i]].z=chars[currchar[s]].z;
        mapRegions->AddItem(currchar[i]+1000000); //lb
		teleport(currchar[i]);
		return;
	default:
		sysmessage( s, "Unknown teleport option (%i)!", x );
		return;
	}
	
	if (i!=-1)
	{
		mapRegions->RemoveItem(i+1000000); //lb
		chars[i].x=chars[currchar[s]].x;
		chars[i].y=chars[currchar[s]].y;
		chars[i].dispz=chars[i].z=chars[currchar[s]].z;
        mapRegions->AddItem(i+1000000); //lb
		teleport( i );
		return;
	}
	i = calcItemFromSer( serial );
	if (i!=-1)
	{
		mapRegions->RemoveItem(i);
		items[i].x=chars[currchar[s]].x;
		items[i].y=chars[currchar[s]].y;
		items[i].z=chars[currchar[s]].z;
		mapRegions->AddItem(i);
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);   
		RefreshItem( i ); // AntiChrist
	}
}

void cTargets::XgoTarget(int s)
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		mapRegions->RemoveItem(i+1000000);
		chars[i].x=addx[s];
		chars[i].y=addy[s];
		chars[i].dispz=chars[i].z=addz[s];
		mapRegions->AddItem(i+1000000);
		teleport( i );	// need to see items, Abaddon 17th February, 2000
		impowncreate( s, i, 1 );

	}
}

void cTargets::MoreXYZTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].morex=addx[s];
		items[i].morey=addy[s];
		items[i].morez=addz[s];
	}
}

void cTargets::MoreXTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].morex=addx[s];
	}
}

void cTargets::MoreYTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].morey=addx[s];
	}
}

void cTargets::MoreZTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].morez=addx[s];
	}
}

void cTargets::PrivTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		//Logging
		sprintf(temp, "%s.log",chars[currchar[s]].name);
		sprintf(temp2, "%s as given %s Priv [%x][%x]\n",chars[currchar[s]].name,chars[i].name,addid1[s],addid2[s]);
		chars[i].priv=addid1[s];
		chars[i].priv2=addid2[s];
		savelog(temp2, temp);
	}
}

void cTargets::MoreTarget(int s)
{
	int i/*,j*/,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].more1=addid1[s];
		items[i].more2=addid2[s];
		items[i].more3=addid3[s];
		items[i].more4=addid4[s];
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
	}
}

void cTargets::KeyTarget(int s) // new keytarget by Morollan
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if ((items[i].more1==0)&&(items[i].more2==0)&&
			(items[i].more3==0)&&(items[i].more4==0))
		{       
			if ((items[i].type==7)&&(iteminrange(s,i,2)))
			{
				if (!Skills->CheckSkill(currchar[s],TINKERING, 400, 1000)) 
				{
					sysmessage(s,"You fail and destroy the key blank.");
					Items->DeleItem(i);
					return;
				}//if
				else
				{
					items[i].more1=addid1[s];
					items[i].more2=addid2[s];
					items[i].more3=addid3[s];
					items[i].more4=addid4[s];
					sysmessage(s, "You copy the key."); //Morrolan can copy keys
					return;
				}//else
			}//if
			return;
		}//if
		else if (((items[i].more1==addid1[s])&&(items[i].more2==addid2[s])&&
			(items[i].more3==addid3[s])&&(items[i].more4==addid4[s]))||
			(addid1[s] == 0xFF ) )
		{
			if (((items[i].type==1)||(items[i].type==63))&&(iteminrange(s,i,2)))
			{
				if(items[i].type==1) items[i].type=8;
				if(items[i].type==63) items[i].type=64;
				sysmessage(s, "You lock the container.");
				return;
			}//if
			else if ((items[i].type==7)&&(iteminrange(s,i,2)))
			{
				chars[currchar[s]].keynumb=i;
				sysmessage(s,"Enter new name for key.");//morrolan rename keys
				return;
			}//else if
			else if ((items[i].type==8)||(items[i].type==64)&&(iteminrange(s,i,2)))
			{
				if(items[i].type==8) items[i].type=1;
				if(items[i].type==64) items[i].type=63;
				sysmessage(s, "You unlock the container.");
				return;
			}//else if
			else if ((items[i].type==12)&&(iteminrange(s,i,2)))
			{
				items[i].type=13;
				sysmessage(s, "You lock the door.");
				return;
			}//else if
			else if ((items[i].type==13)&&(iteminrange(s,i,2)))
			{
				items[i].type=12;
				sysmessage(s, "You unlock the door.");
				return;
			}//else if
			else if ((items[i].id1==0x0b)&&(items[i].id2==0xd2))
			{
				sysmessage(s, "What do you wish the sign to say?");
				chars[currchar[s]].keynumb=i; //Morrolan sign kludge
				return;
			}//else if
			
			//Boats ->
			else if( items[i].type==117 )
			{
				Boats->OpenPlank( i );
				RefreshItem( i ); // AntiChrist
			}
			//End Boats --^
			
			
		}//else if
		
		else
		{
			if (items[i].type==7) sysmessage (s, "That key is not blank!");
			else if (items[i].more1=='\x00') sysmessage(s, "That does not have a lock.");
			else sysmessage(s, "The key does not fit into that lock.");
			return;
		}//else
		return;
	}//if
}//keytarget()


void cTargets::IstatsTarget(int s)
{
	int i,serial;
	tile_st tile;
	
	
	if ((buffer[s][7]==0)&&(buffer[s][8]==0)&&(buffer[s][9]==0)&&(buffer[s][10]==0))
	{
		Map->SeekTile(((buffer[s][0x11]<<8)+buffer[s][0x12]), &tile);
		sprintf(temp, "Item [Static] ID [%x %x]",buffer[s][0x11], buffer[s][0x12]);
		sysmessage(s, temp);
		sprintf(temp, "ID2 [%i],  Height [%i]",((buffer[s][0x11]<<8)+buffer[s][0x12]), tile.height);
		sysmessage(s, temp);
	}
	else
	{
		serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
		i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
		if (i!=-1)
		{
			// Modified by Magius(CHE)
			sprintf(temp, "Item [Dynamic] Ser [%x %x %x %x]      ID [%x %x] Name [%s] Name2 [%s] Color [%x %x] Cont [%x %x %x %x] Layer [%x] Type [%d] Magic [%x] More [%x %x %x %x] Position [%i %i %i] Amount [%i] Priv [%x]",
				items[i].ser1,items[i].ser2,items[i].ser3,items[i].ser4,items[i].id1,items[i].id2,
				items[i].name,items[i].name2, items[i].color1,items[i].color2,
				items[i].cont1,items[i].cont2,items[i].cont3,items[i].cont4,
				items[i].layer,items[i].type,items[i].magic,
				items[i].more1,items[i].more2,items[i].more3,items[i].more4,
				items[i].x,items[i].y,items[i].z,items[i].amount, items[i].priv );
			sysmessage(s, temp);
			sprintf(temp, "STR [%d] HP/MAX [%d/%d] Damage [%d-%d] Defence [%d] Rank [%d] MoreXYZ [%i %i %i] Poisoned [%i] Single Item weight [%d] Owner [%x %x %x %x ] Creator [%s] MadeValue [%i] Decaytime[%i] Decay [%i] GoodType[%i] RandomValueRate[%i] Value [%i]",
				items[i].st, items[i].hp, items[i].maxhp, items[i].lodamage, items[i].hidamage, items[i].def, items[i].rank,
				items[i].morex, items[i].morey, items[i].morez,items[i].poisoned,
				items[i].weight, items[i].owner1, items[i].owner2, items[i].owner3, items[i].owner4, // Ison 2-20-99
				items[i].creator,items[i].madewith,int(double(int(items[i].decaytime-uiCurrentTime)/CLOCKS_PER_SEC)),(items[i].priv)&0x01,items[i].good,items[i].rndvaluerate,items[i].value); // Magius(CHE) (2)
				sysmessage(s,temp); // Ison 2-20-99
		}
	}
}

void cTargets::CstatsTarget( UOXSOCKET s )
{
	int i,serial;
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		sprintf(temp, "Ser [%x %x %x %x] ID [%x %x] Name [%s] Skin [%x %x] Account [%x] Priv [%x %x] Position [%i %i %i] CTimeout [%i] Fame [%i] Karma [%i] Deaths [%i] Kills [%i] NPCAI [%x] NPCWANDER [%d] WEIGHT [%.2f]",
			chars[i].ser1,chars[i].ser2,chars[i].ser3,chars[i].ser4,chars[i].id1,chars[i].id2,
			chars[i].name,chars[i].skin1,chars[i].skin2,
			chars[i].account,chars[i].priv,chars[i].priv2,
			chars[i].x,chars[i].y,chars[i].z, chars[i].timeout,
			chars[i].fame,chars[i].karma,chars[i].deaths,chars[i].kills,
			chars[i].npcaitype, chars[i].npcWander, chars[i].weight);
		sysmessage(s, temp);
		sprintf(temp, "Other Info: Poisoned [%i] Poison [%i] Hunger [%i] Attacker [%i] Target [%i] Carve [%i] LastOn [%s]", // Changed by Magius(CHE)
			chars[i].poisoned,chars[i].poison, chars[i].hunger, chars[i].attacker,chars[i].targ, chars[i].carve, chars[i].laston ); // Changed by Magius(CHE)
		sysmessage(s, temp);
		sprintf( temp, "Race [%i], Race Gate [%i]", chars[i].race, chars[i].raceGate );
		sysmessage( s, temp );
		Gumps->Open(s, i, 0, 8);
		statwindow(s, i);
	}
}

void cTargets::WstatsTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		sprintf(temp, "Ser [%x %x %x %x] ID [%x %x] Name [%s]",
			chars[i].ser1,chars[i].ser2,chars[i].ser3,chars[i].ser4,chars[i].id1,chars[i].id2,
			chars[i].name);
		sysmessage(s, temp);
		sprintf(temp, "X: %d, Y: %d, Z: %d/%d", chars[i].x, chars[i].y, chars[i].z, chars[i].dispz);
		sysmessage(s, temp);
		sprintf(temp, "W: %d FX1: %d, FY1: %d, FZ1: %d FX2: %d, FY2: %d", chars[i].npcWander,
			chars[i].fx1, chars[i].fy1, chars[i].fz1, chars[i].fx2, chars[i].fy2);
		sysmessage(s, temp);
	}
}

void cTargets::GMTarget(int s)
{
	int j;
	int  n, z, mypack, retitem;
	
	mypack=-1; 
	retitem=-1;

	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		if( chars[i].npc )
		{
			sysmessage( s, "You can't promote an NPC to GM." );
			return;
		}
		sprintf( temp, "%s.log", chars[currchar[s]].name );
		sprintf( temp2, "%s as made %s a GM.\n", chars[currchar[s]].name, chars[i].name );
		savelog( temp2, temp );
		
		//AntiChrist bugfix
		unmounthorse( calcSocketFromChar( i ) );
		chars[i].id1 = 0x03;
		chars[i].id2 = 0xDB;
		chars[i].skin1 = 0x80;
		chars[i].skin2 = 0x21;
		chars[i].xid1 = 0x03;
		chars[i].xid2 = 0xDB;
		chars[i].orgid1 = 0x03;
		chars[i].orgid2 = 0xDB;
		chars[i].xskin1 = 0x80;
		chars[i].xskin2 = 0x21;
		chars[i].priv = 0xF7;			// may want to look at this, CGMT says it's really 7F, and the closest way to get F7
									// required me to have counselor clearance set as well (which it wouldn't allow)
		chars[i].priv2 = 0xD9;		// no longer viewing house icons by default (request from Xuri)
		chars[i].commandLevel = 2;
		
	
		for (j=0;j<TRUESKILLS;j++)
		{
			chars[i].baseskill[j]=1000;
			chars[i].skill[j]=1000;
		}

		// All Stats at TOP for GM... or not??? --- Magius(CHE) 
		// Abaddon, you're making an assumption that 100 is a maximum... not valid!
		chars[i].st = 100;
		chars[i].st2 = 100;
		chars[i].hp = 100;
		chars[i].stm = 100;
		chars[i].in = 100;
		chars[i].in2 = 100;
		chars[i].mn = 100;
		chars[i].mn2 = 100;
		chars[i].dx = 100;
		chars[i].dx2 = 100;
		// End magius addon
		if (strncmp(chars[i].name, "GM", 2))
		{
			sprintf(temp, "GM %s", chars[i].name);
			strcpy(chars[i].name, temp);
		}
		
		for(int a=0;a<contsp[chars[i].serial%HASHMAX].max;a++)
		{
			z=contsp[chars[i].serial%HASHMAX].pointer[a];
			
			if (z!=-1)
			{
			  if (items[z].contserial==chars[i].serial &&
			  	items[z].layer!=0x15 && items[z].layer!=0x1D &&
				items[z].layer!=0x10 && items[z].layer!=0x0B && (items[z].free==0))
			{
				if (mypack==-1)
				{
					mypack=packitem(i);
				}
				if (mypack==-1)
				{
					chars[i].packitem=n=Items->SpawnItem(calcSocketFromChar(i),i,1,"#",0,0x0E,0x75,0,0,0,0);
					if( n == -1 ) return;
					setserial(n,i,4);
					items[n].layer=0x15;
					items[n].type=1;
					items[n].dye=1;
					mypack=n;
					retitem=n;
				}
				items[z].x=(rand()%80)+50;
				items[z].y=(rand()%80)+50;
				items[z].z=9;
				if (items[z].contserial!=-1) removefromptr(&contsp[items[z].contserial%HASHMAX], z);
				setserial(z,mypack,1);
				items[z].layer=0x00;
				
				removeitem[1]=items[z].ser1;
				removeitem[2]=items[z].ser2;
				removeitem[3]=items[z].ser3;
				removeitem[4]=items[z].ser4;
				for(j=0;j<now;j++)
				if (perm[j])
				{
					Network->xSend(j, removeitem, 5, 0);
//					senditem(j, z);
				}
				RefreshItem( z ); // AntiChrist
			}
			else if (items[z].contserial==chars[i].serial &&
				(items[z].layer==0x0B || items[z].layer==0x10))
			{
				Items->DeleItem(z);
			}
			
		}
		} // end of z!=-1
		updatechar(i);
	} //LB !!!
}

void cTargets::CnsTarget(int s)
{
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	
	if(i!=-1)
	{
		
		
		// logging   
		sprintf(temp, "%s.log",chars[currchar[s]].name);
		sprintf(temp2, "%s as made %s a Counselor.\n",chars[currchar[s]].name,chars[i].name);
		savelog(temp2, temp); //AntiChrist
		chars[i].id1=0x03;
		chars[i].id2=0xDB;
		chars[i].skin1=0x80;
		chars[i].skin2=0x03;
		chars[i].xid1=0x03;
		chars[i].xid2=0xDB;
		chars[i].orgid1=0x03;
		chars[i].orgid2=0xDB;
		chars[i].xskin1=0x80;
		chars[i].xskin2=0x02;
		chars[i].priv=0xB6;
		chars[i].priv2=0x8D;
		chars[i].commandLevel = 1;
		if (strncmp(chars[i].name, "Counselor", 9))
		{
			sprintf(temp, "Counselor %s", chars[i].name);
			strcpy(chars[i].name, temp);
		}
		updatechar(i);
		//break;
	}
	//} for
}

void cTargets::KillTarget(int s, int ly)
{
	int i, k,serial,serhash,ci;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	
	k=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (k!=-1)
	{
		serhash=serial%HASHMAX;
		for (ci=0;ci<contsp[serhash].max;ci++)
		{
			i=contsp[serhash].pointer[ci];
			if (i!=-1)
				if ((items[i].contserial==serial) && 
					(items[i].layer==ly))
				{
					Items->DeleItem(i);
				}
		}
	}
}

void cTargets::FontTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].fonttype=addid1[s];
	}
}


void cTargets::GhostTarget(int s)
{
	//int x=0; //AntiChrist
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	
	if(i!=-1)
	{
		if(chars[i].dead==0)
		{
			chars[i].attacker=currchar[s]; //AntiChrist -- for forensics ev
			bolteffect(i);
			soundeffect2(i, 0x00, 0x29);
			//x++;
			deathstuff(i);
			//break;
		} else sysmessage(s,"That player is already dead.");
		//} for
	}
	//if(x==0)
	//	sysmessage(s,"That player is already dead.");
}


void cTargets::BoltTarget(int s)
{
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	
	if(i!=-1)
	{
		bolteffect(i);
		soundeffect2(i, 0x00, 0x29);
		//break;
		//} for
	}
}

void cTargets::AmountTarget(int s)
{
//	int j;
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	if(i!=-1)
	{
		items[i].amount=addx[s];
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
	}
	
	
}


void cTargets::SetAmount2Target(int s)
{
//	int j;
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	if(i!=-1)
	{
		items[i].amount2=addx[s];
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
	}
	//} for
}

void cTargets::CloseTarget(int s)
{
	int j;
	
	//char closestr[6]="\x26\x00\x00\x00\x00";
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if(i!=-1)	
	{
		j=calcSocketFromChar(i);
		if(j>-1)
		{
			sysmessage(s, "Kicking player");
			sysmessage(j, "You have been kicked!"); //New -- Zippy
			Network->Disconnect(j);//Network->xSend(j, closestr, 5, 0);
						  /*if (currchar[j]==i)
						  {
						  disconnect(j);//Network->xSend(j, closestr, 5, 0);
						  break;
		}*/
			//} for
			//break;
		}
	}
}

// sockets
int cTargets::AddMenuTarget(int s, int x, int addmitem) //Tauriel 11-22-98 updated for new items
{
	int c;//, j;
	if( s >= 0 ) if ( buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF ) return -1;
		c=Items->CreateScriptItem(s, addmitem, 0);
		if (c==-1) return -1;
		if (x)
			RefreshItem( c ); // AntiChrist
		return c;
}

int cTargets::NpcMenuTarget(int s)
// Abaddon 17th February, 2000
// Need to return the character we've made, else summon creature at least will fail
// We make the char, but never pass it back up the chain
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return -1;
	
	int x;
	x=addmitem[s];
	
	return Npcs->AddRespawnNPC(s,-1,x,0);
//	Npcs->AddRespawnNPC(s,x,0);
}

void cTargets::MovableTarget (int s)
{
//	int j;
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	
	if(i!=-1)
	{
		items[i].magic=addx[s];
		RefreshItem( i ); // AntiChrist
	}
}

void cTargets::VisibleTarget( UOXSOCKET s )
{
	int i;//,j;
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( buffer[s][7] >= 0x40 )//item
	{
		i = calcItemFromSer( serial );
		if( i != -1 )
		{
			items[i].visible = addx[s];
			RefreshItem( i ); // AntiChrist
		}
	} 
	else 
	{//char
		i = calcCharFromSer( serial );
		if( i != -1 )
		{
			chars[i].hidden = addx[s];
			updatechar(i);
#ifdef __FLAMESTRIKE__
			if( addx[s] == 0 && (chars[i].priv&0x01) )
			{
				staticeffect( i, 0x37, 0x09, 0x09, 0x19 );
				soundeffect2( i, 0x02, 0x08 );
			}
#endif
		}
	}
}

void cTargets::OwnerTarget(int s) // bugfixed by JM
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		if( calcserial( addid1[s], addid2[s], addid3[s], addid4[s] ) == -1 )
		{
			removefromptr( &cownsp[chars[i].ownserial%HASHMAX], i );
			chars[i].tamed = false;
		}
		else
		{
			setptr( &cownsp[calcserial( addid1[s], addid2[s], addid3[s], addid4[s])%HASHMAX], i );
			chars[i].tamed = true;
		}
		chars[i].own1=addid1[s];
		chars[i].own2=addid2[s];
		chars[i].own3=addid3[s];
		chars[i].own4=addid4[s];
		chars[i].ownserial=calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
	}
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if( calcserial( addid1[s], addid2[s], addid3[s], addid4[s] ) == -1 )
		{
			removefromptr( &cownsp[items[i].ownserial%HASHMAX], i );
		}
		items[i].owner1=addid1[s];
		items[i].owner2=addid2[s];
		items[i].owner3=addid3[s];
		items[i].owner4=addid4[s];
		items[i].ownserial=calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
		if( items[i].ownserial != -1 )
		{
			setptr(&cownsp[items[i].ownserial%HASHMAX], i);
		}
	}
}

void cTargets::ColorsTarget (int s)
{
	int i,serial;
	

	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if (((items[i].id1==0x0F)&&(items[i].id2==0xAB))||                        //dye vat
			((items[i].id1==0x0E)&&((items[i].id2==0xFF)||(items[i].id2==0x27)))) //hair dye
		{
			dyevat[1]=items[i].ser1;
			dyevat[2]=items[i].ser2;
			dyevat[3]=items[i].ser3;
			dyevat[4]=items[i].ser4;
			dyevat[7]=items[i].id1;
			dyevat[8]=items[i].id2;
			Network->xSend(s, dyevat, 9, 0);
		}
		else
		{
			sysmessage(s, "You can only use this item on a dye vat.");
		}
	}
}

void cTargets::DvatTarget (int s)
{
	int i,serial;
	int count, j, x, c, change;
	unsigned char x1, x2, x3, x4;		// these were ints
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if (items[i].dye==1)
		{
			c = -1;
			x = -1;
			x1 = items[i].cont1;
			x2 = items[i].cont2;
			x3 = items[i].cont3;
			x4 = items[i].cont4;
			serial = items[i].contserial = calcserial( x1, x2, x3, x4 );
			count = 0;
			do
			{
				change = 0;
				if( x1 >= 0x40 )
				{
					j = findbyserial( &itemsp[serial%HASHMAX],serial, 0 );
					if( j != -1 )
					{
						x = j;
						change = 1;
						x1 = items[j].cont1;
						x2 = items[j].cont2;
						x3 = items[j].cont3;
						x4 = items[j].cont4;
						serial = items[j].contserial = calcserial( x1, x2, x3, x4 );
					}
				}
				else
				{
					j = findbyserial( &charsp[serial%HASHMAX], serial, 1 );
					if( j != -1 )
					{
						change = 1;
						c = j;
						x1 = 255;
					}
				}
				count++;
			} while( ( x1 != 255 && change == 1 ) && count < 9999 );
			if( count >= 9999 ) printf("inf-loop avoided\n" );
			if( c == currchar[s] || items[i].contserial == -1 )
			{
				items[i].color1=addid1[s];
				items[i].color2=addid2[s];
	//			for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
				RefreshItem( i ); // AntiChrist
				soundeffect( s, 0x02, 0x3E );	// plays the dye sound, LB
			}
			else
			{
				sysmessage( s, "This is not yours!" );
			}
		}
		else
		{
			sysmessage(s, "You can only dye clothes with this.");
		}
	}
}

void cTargets::AddNpcTarget(int s)
{
	int c;
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF &&
		buffer[s][14]==0xFF) return;
	c=Npcs->MemCharFree ();
	
	Npcs->InitChar(c);
	strcpy(chars[c].name, "Dummy");
	chars[c].id1=addid1[s];
	chars[c].id2=addid2[s];
	chars[c].xid1=addid1[s];
	chars[c].xid2=addid2[s];
	chars[c].orgid1=addid1[s];
	chars[c].orgid2=addid2[s];
	chars[c].skin1=0;
	chars[c].skin2=0;
	chars[c].xskin1=0;
	chars[c].xskin2=0;
	chars[c].priv=0x10;
	chars[c].x=(buffer[s][11]<<8)+buffer[s][12];
	chars[c].y=(buffer[s][13]<<8)+buffer[s][14];
	chars[c].dispz=chars[c].z=buffer[s][16]+Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
	mapRegions->AddItem(c+1000000); // add it to da regions ...
	chars[c].npc=1;
	updatechar(c);
}

void cTargets::FreezeTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].priv2=chars[i].priv2|2;
	}
}

void cTargets::UnfreezeTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
//		chars[i].priv2=chars[i].priv2&(!2);
		chars[i].priv2 = chars[i].priv2 & 0xFD; // unfreeze, AntiChrist used LB bugfix
	}
}

void cTargets::AllSetTarget(int s)
{
	int j, k;
	
	//unsigned short int tempskill;
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if(i!=-1)
	{
		k=calcSocketFromChar(i);
		if (addx[s]<TRUESKILLS)
		{
			chars[i].baseskill[addx[s]]=addy[s];
			Skills->updateSkillLevel(i, addx[s]);
			if (k!=-1) updateskill(k, addx[s]);
		}
		else if (addx[s]==ALLSKILLS)
		{
			for (j=0;j<TRUESKILLS;j++)
			{
				chars[i].baseskill[j]=addy[s];
				Skills->updateSkillLevel(i, j);
				if (k!=-1) updateskill(k,j);
			}
		}
		else if (addx[s]==STRENGTH)
		{
			chars[i].st=addy[s];
			for (j=0;j<TRUESKILLS;j++)
			{
				Skills->updateSkillLevel(i,j);
				if (k!=-1) updateskill(k,j);
			}
			if (k!=-1) statwindow(k,i);
		}
		else if (addx[s]==DEXTERITY)
		{
			chars[i].dx=addy[s];
			for (j=0;j<TRUESKILLS;j++)
			{
				Skills->updateSkillLevel(i,j);
				if (k!=-1) updateskill(k,j);
			}
			if (k!=-1) statwindow(k,i);
		}
		else if (addx[s]==INTELLECT)
		{
			chars[i].in=addy[s];
			for (j=0;j<TRUESKILLS;j++)
			{
				Skills->updateSkillLevel(i,j);
				if (k!=-1) updateskill(k,j);
			}
			if (k!=-1) statwindow(k,i);
		}
		else if (addx[s]==FAME)
		{
			chars[i].fame=addy[s];
		}
		else if (addx[s]==KARMA)
		{
			chars[i].karma=addy[s];
		}
		//break;
		//} for
	}
}

void cTargets::InfoTarget(int s) // rewritten to work also with map-tiles, not only static ones by LB
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return;

	int tilenum;
	short int x, y;	// these were full ints
	signed char z;
//	unsigned long int pos;
	tile_st tile;
	map_st map1;
	land_st land;
	
	x=(buffer[s][0x0B]<<8)+buffer[s][0x0C];
	y=(buffer[s][0x0D]<<8)+buffer[s][0x0E];
	z=buffer[s][0x10];

	if ((buffer[s][0x11]==0)&&(buffer[s][0x12]==0))	// damn osi not me why the tilenum is only send for static tiles, LB
		{  // manually calculating the ID's if it's a maptype
			map1 = Map->SeekMap0( x, y );						
			Map->SeekLand(map1.id, &land);
			printf("type: map-tile\n");
	        printf("tilenum: %i\n",map1.id);
	        printf("Flag1:%x\n", land.flag1);
	        printf("Flag2:%x\n", land.flag2);
	        printf("Flag3:%x\n", land.flag3);
	        printf("Flag4:%x\n", land.flag4);
	        printf("Unknown1:%lx\n", land.unknown1);
	        printf("Unknown2:%x\n", land.unknown2);
	        printf("Name:%s\n", land.name);

		} else

		{

	      tilenum=((buffer[s][0x11])<<8)+buffer[s][0x12]; // lb, bugfix
	      Map->SeekTile(tilenum, &tile);

		  printf("type: static-tile\n");
	      printf("tilenum: %i\n",tilenum);
	      printf("Flag1:%x\n", tile.flag1);
	      printf("Flag2:%x\n", tile.flag2);
	      printf("Flag3:%x\n", tile.flag3);
	      printf("Flag4:%x\n", tile.flag4);
	      printf("Weight:%x\n", tile.weight);
	      printf("Layer:%x\n", tile.layer);
	      printf("Anim:%lx\n", tile.animation);
	      printf("Unknown1:%lx\n", tile.unknown1);
	      printf("Unknown2:%x\n", tile.unknown2);
	      printf("Unknown3:%x\n", tile.unknown3);
	      printf("Height:%x\n", tile.height);
	      printf("Name:%s\n", tile.name);
	     
		}
		sysmessage(s, "Item info has been dumped to the console.");
		printf("\n");
}

void cTargets::TweakTarget( UOXSOCKET s )//Lag fix -- Zippy
{
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i;
	i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if (i!=-1)//Char
	{
		tweakmenu(s, i, 2);
	} else {//item
		i=findbyserial(&itemsp[serial%HASHMAX],serial,0);
		if(i!=-1)
		{
			tweakmenu(s, i, 1);
		}
	}
}

void cTargets::LoadCannon(int s)
{
	int i,serial;
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if( i != -1 )
	{
		if (((items[i].more1==addid1[s])&&(items[i].more2==addid2[s])&&
			(items[i].more3==addid3[s])&&(items[i].more4==addid4[s]))||
			(addid1[s] == 0xFF ))
		{
			if ((items[i].morez==0)&&(iteminrange(s,i,2)))
			{
				if(items[i].morez==0) items[i].morez=1;
				sysmessage(s, "You load the cannon.");
			}
			else
			{
				if (items[i].more1=='\x00') sysmessage(s, "That doesn't work in cannon.");
				else sysmessage(s, "That object doesn't fit into cannon.");
			}
		}
	}
}

void cTargets::SetInvulFlag(int s)
{
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	
	if(i!=-1)
	{
		if (addx[s]==1)
		{
			chars[i].priv=chars[i].priv|4;
		}
		else
		{
			chars[i].priv=chars[i].priv&0xFB; // Morrolan
		}
		//} for
	}
}

void cTargets::Tiling(int s)  // Clicking the corners of tiling calls this function - Crwth 01/11/1999
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return;
	
	if (clickx[s]==-1 && clicky[s]==-1) {
		clickx[s]=(buffer[s][11]<<8)+buffer[s][12];
		clicky[s]=(buffer[s][13]<<8)+buffer[s][14];
		target( s, 0, 1, 0, 198, "Select second corner of bounding box." );
		return;
	}
	
	int /*j,*/c,pileable=0;
	tile_st tile;
	int x1=clickx[s],x2=(buffer[s][11]<<8)+buffer[s][12];
	int y1=clicky[s],y2=(buffer[s][13]<<8)+buffer[s][14];
	
	clickx[s]=-1;clicky[s]=-1;
	
	if (x1>x2) {c=x1;x1=x2;x2=c;}
	if (y1>y2) {c=y1;y1=y2;y2=c;}
	
	if (addid1[s]==0x40)
	{
		switch (addid2[s])
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
			Targ->AddTarget(s);
			return;
		}
	}
	
	int x,y;
	
	Map->SeekTile((addid1[s]<<8)+addid2[s], &tile);
	if (tile.flag2&0x08) pileable=1;
	for (x=x1;x<=x2;x++)
	  {
		for (y=y1;y<=y2;y++) {
			c=Items->SpawnItem(s, 1, "#", pileable, addid1[s], addid2[s], 0, 0, 0,0);
			if( c == -1 ) return;
			items[c].priv=0;	//Make them not decay
			items[c].x=x;
			items[c].y=y;
			items[c].z=buffer[s][16]+(Map->TileHeight( (buffer[s][17]<<8) + buffer[s][18]));
			RefreshItem( c ); // AntiChrist
			mapRegions->AddItem(c); // lord Binary
		}
	  }
		
		addid1[s]=0;
		addid2[s]=0;
}

void cTargets::Wiping(int s)  // Clicking the corners of wiping calls this function - Crwth 01/11/1999
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return;
	
	if (clickx[s]==-1 && clicky[s]==-1) {
		clickx[s]=(buffer[s][11]<<8)+buffer[s][12];
		clicky[s]=(buffer[s][13]<<8)+buffer[s][14];
		if (addid1[s]) target(s,0,1,0,199,"Select second corner of inverse wiping box.");
		else target(s,0,1,0,199,"Select second corner of wiping box.");
		return;
	}
	
	int x1=clickx[s],x2=(buffer[s][11]<<8)+buffer[s][12];
	int y1=clicky[s],y2=(buffer[s][13]<<8)+buffer[s][14];
	
	clickx[s]=-1;clicky[s]=-1;
	
	int c;
	
	if (x1>x2) {c=x1;x1=x2;x2=c;}
	if (y1>y2) {c=y1;y1=y2;y2=c;}
	
	if (addid1[s]==1) {  // addid1[s]==1 means to inverse wipe
		for (int i=0;i<itemcount;i++)
			if (!(items[i].x>=x1 && items[i].x<=x2 && items[i].y>=y1 && items[i].y<=y2) && items[i].contserial==-1 && items[i].wipe==0)
				Items->DeleItem(i);
	}
	else {
		for (int i=0;i<itemcount;i++)
			if (items[i].x>=x1 && items[i].x<=x2 && items[i].y>=y1 && items[i].y<=y2 && items[i].contserial==-1 && items[i].wipe==0)
				Items->DeleItem(i);
	}
}

void cTargets::ExpPotionTarget(int s) //Throws the potion and places it (unmovable) at that spot
{
	int i, x, y, z;
	//int dx,dy;
	x=(buffer[s][11]<<8)+buffer[s][12];
	y=(buffer[s][13]<<8)+buffer[s][14];
	z=buffer[s][16];
	if (x!=65535)
	{
		// ANTICHRIST -- CHECKS LINE OF SIGHT!
		if(line_of_sight(s,chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].dispz,x,y,z,WALLS_CHIMNEYS + DOORS + ROOFING_SLANTED))
		{
			i=calcItemFromSer(addid1[s],addid2[s],addid3[s],addid4[s]);
			if( i > -1 ) // crashfix LB
			{
				mapRegions->RemoveItem(i); //LB
				items[i].x=x;
				items[i].y=y;
				items[i].z=z;
				mapRegions->AddItem(i);
				items[i].cont1=255;		// was -1, not good for UNSIGNED chars
				items[i].cont2=255;		// was -1, not good for UNSIGNED chars
				items[i].cont3=255;		// was -1, not good for UNSIGNED chars
				items[i].cont4=255;		// was -1, not good for UNSIGNED chars
				items[i].contserial=-1;
				items[i].magic=2; //make item unmovable once thrown
				movingeffect2(currchar[s], i, 0x0F, 0x0D, 0x11, 0x00, 0x00);
//				for (unsigned int j=0;j<now;j++) if (perm[j]) senditem(j,i);
				RefreshItem( i ); // AntiChrist
			}
		} else sysmessage( s,"You cannot throw the potion there!");
	}
}

void cTargets::SquelchTarg(int s)//Squelch
{
	int p, serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	p=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if (p!=-1)
	{
		if(chars[p].priv&1)             
		{
			sysmessage(s, "You cannot squelch GMs.");
			return;
		} 
		if (chars[p].squelched)
		{
			chars[p].squelched=0;
			sysmessage(s, "Un-squelching...");
			sysmessage(calcSocketFromChar(p), "You have been unsquelched!");
			chars[p].mutetime=-1;
		} 
		else 
		{
			chars[p].mutetime=-1;
			chars[p].squelched=1;
			sysmessage(s, "Squelching...");
			sysmessage(calcSocketFromChar(p), "You have been squelched!");
			if (addid1[s]!=255 || addid1[s]!=0)			// 255 used to be -1, not good for unsigned chars
			{
				chars[p].mutetime = (unsigned int)( uiCurrentTime + ( addid1[s] * CLOCKS_PER_SEC ) );
				addid1[s]=255;
				chars[p].squelched=2;
			}
		}
	}
}


void cTargets::TeleStuff(int s)
{
    static int targ=-1;//What/who to tele
    int x, y, z;
    int serial, i;
	//	 
    if (targ==-1)
	{
		serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
		if (buffer[s][7]==0xFF) return;
		targ=findbyserial(&charsp[serial%HASHMAX],serial,1);
		
		if(targ!=-1)
		{
			targ+=1000000;
			target(s,0,1,0,222,"Select location to put this character.");
		} else {
			targ=findbyserial(&itemsp[serial%HASHMAX],serial,0);
			if(targ!=-1)
				target(s,0,1,0,222,"Select location to put this item.");
		}
		return;
	} else {
		if(buffer[s][11]==0xFF) return;
		x=(buffer[s][11]<<8)+buffer[s][12];
		y=(buffer[s][13]<<8)+buffer[s][14];
		z=buffer[s][16]+Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
		
		if (targ>999999)//character
		{
			sysmessage(s, "Moving character...");
			i=targ-1000000;
			mapRegions->RemoveItem(i+1000000);
			chars[i].x=x;
			chars[i].y=y;
			chars[i].z=chars[i].dispz=z;
			mapRegions->AddItem(i+1000000);
			teleport(i);
		} else {
			i=targ;
			mapRegions->RemoveItem(i);
			items[i].x=x;
			items[i].y=y;
			items[i].z=z;
			mapRegions->AddItem(i);
			sysmessage(s, "Moving item...");
//			for(int a=0;a<now;a++) if (iteminrange(a, i, 30)) senditem(a, i);
			RefreshItem( i ); // AntiChrist
		}
		targ=-1;
		return;
	}
}

void cTargets::SwordTarget(int s)
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return;
	int /*k,*/c;
	
	if (((buffer[s][0x11]==0x0C)&&
		((buffer[s][0x12]==0xD0)||(buffer[s][0x12]==0xD3)||(buffer[s][0x12]==0xD6)||
		(buffer[s][0x12]==0xD8)||(buffer[s][0x12]==0xDA)||(buffer[s][0x12]==0xDD)||
		(buffer[s][0x12]==0xE0)||(buffer[s][0x12]==0xE3)||(buffer[s][0x12]==0xE6)||
		((buffer[s][0x12]>=0xCA)&&(buffer[s][0x12]<=0xCD))))||
		((buffer[s][0x11]==0x12)&&(buffer[s][0x12]>=0xB8)&&(buffer[s][0x12]<=0xBB)))
	{
		if (!chars[s].onhorse) action(s,0x0D);
		else action(s,0x1d);
		soundeffect(s,0x01,0x3E);
		c=Items->SpawnItem(s,1,"#",1,0x0D,0xE1,0,0,0,0); //Kindling
		if( c == -1 ) return;

		items[c].x=chars[currchar[s]].x;
		items[c].y=chars[currchar[s]].y;
		items[c].z=chars[currchar[s]].z;

        mapRegions->AddItem(c); // lord Binary
		
//		for (k=0;k<now;k++) if (perm[k]) senditem(k,c);
		RefreshItem( c ); // AntiChrist
		sysmessage(s, "You hack at the tree and produce some kindling.");
	}
	else
		if((buffer[s][0x11]==0x1B) && ((buffer[s][0x12]==0xDD) || buffer[s][0x12]==0xE0))  // vagrant
		{
			Skills->BowCraft(s);
			return;
		}
		else if ((buffer[s][0x11]==0x20) && (buffer[s][0x12]==0x06))
		{
			Targ->CorpseTarget(s);
			return;
		}
		else sysmessage(s,"You can't think of a way to use your blade on that.");
}

void cTargets::CorpseTarget(int s)
{
	int n=0;
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	if(i!=-1)
	{
		if(iteminrange(s,i,1)) {
			npcshape[0]=i;
			action(s,0x20);
			n=1;
			if(items[i].more1==0)
			{
				items[i].more1=1;
				if( items[i].morey /*|| items[i].carve > -1 */ )
				{ // if specified, use enhanced carving system!
					Targ->newCarveTarget( s, i ); // AntiChrist
				}
				else
				{
					switch( items[i].amount ) {	// Fur amount -1 triggers Dragonskin instead of leather (Thunderstorm)
					case 0x01: CarveTarget(s, 0, 2, 0, 0, 0); break;  //Ogre
					case 0x02: CarveTarget(s, 0, 5, 0, 0, 0); break;  //Ettin
					case 0x03: break;                                 //Zombie
					case 0x04: break;                                 //Gargoyle
					case 0x05: CarveTarget(s, 36, 1, 0, 0, 0); break; //Eagle
					case 0x06: CarveTarget(s, 25, 1, 0, 0, 0); break; //Bird
					case 0x07: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Orc w/axe
					case 0x08: break;                                 //Corpser
					case 0x09: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Deamon
					case 0x0A: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Deamon w/sword
					case 0x0B: break;                                 //-NULL-
					case 0x0C: CarveTarget(s, 0, 19, 20, -1, 0); break;//Dragon (green)
					case 0x0D: break;                                 //Air Elemental
					case 0x0E: break;                                 //Earth Elemental
					case 0x0F: break;                                 //Fire Elemental
					case 0x10: break;                                 //Water Elemental
					case 0x11: CarveTarget(s, 0, 3, 0, 0, 0); break;  //Orc
					case 0x12: CarveTarget(s, 0, 5, 0, 0, 0); break;  //Ettin w/club
					case 0x13: break;                                 //-NULL-
					case 0x14: break;                                 //-NULL-
					case 0x15: CarveTarget(s, 0, 4, 20, 0, 0); break; //Giant Serpent
					case 0x16: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Gazer
					case 0x17: break;                                 //-NULL-
					case 0x18: break;                                 //Liche
					case 0x19: break;                                 //-NULL-
					case 0x1A: break;                                 //Ghoul
					case 0x1B: break;                                 //-NULL-
					case 0x1C: break;                                 //Spider
					case 0x1D: CarveTarget(s, 0, 1, 0, 1, 0); break;  //Gorilla
					case 0x1E: CarveTarget(s, 50, 1, 0, 0, 0); break; //Harpy
					case 0x1F: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Headless
					case 0x20: break;                                 //-NULL-
					case 0x21: CarveTarget(s, 0, 1, 12, 0, 0); break; //Lizardman
					case 0x22: break;                                 //-NULL-
					case 0x23: CarveTarget(s, 0, 1, 12, 0, 0); break; //Lizardman w/spear
					case 0x24: CarveTarget(s, 0, 1, 12, 0, 0); break; //Lizardman w/mace
					case 0x25: break;                                 //-NULL-
					case 0x26: break;                                 //-NULL-
					case 0x27: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Mongbat
					case 0x28: break;                                 //-NULL-
					case 0x29: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Orc w/club
					case 0x2A: break;                                 //Ratman
					case 0x2B: break;                                 //-NULL-
					case 0x2C: break;                                 //Ratman w/axe
					case 0x2D: break;                                 //Ratman w/dagger
					case 0x2E: break;                                 //-NULL-
					case 0x2F: break;                                 //Reaper
					case 0x30: break;                                 //Scorpion
					case 0x31: break;                                 //-NULL-
					case 0x32: break;                                 //Skeleton
					case 0x33: break;                                 //Slime
					case 0x34: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Snake
					case 0x35: CarveTarget(s, 0, 2, 0, 0, 0); break;  //Troll w/axe
					case 0x36: CarveTarget(s, 0, 2, 0, 0, 0); break;  //Troll
					case 0x37: CarveTarget(s, 0, 2, 0, 0, 0); break;  //Troll w/club
					case 0x38: break;                                 //Skeleton w/axe
					case 0x39: break;                                 //Skeleton w/sword
					case 0x3A: break;                                 //Wisp
					case 0x3B: CarveTarget(s, 0, 19, 20, -1, 0); break;//Dragon (red)
					case 0x3C: CarveTarget(s, 0, 10, 20, -1, 0); break;//Drake (green)
					case 0x3D: CarveTarget(s, 0, 10, 20, -1, 0); break;//Drake (red)
					case 0x46: CarveTarget(s, 0, 5, 7, 0, 0); break;//Terathen Matriarche - t2a
					case 0x47: CarveTarget(s, 0, 5, 7, 0, 0); break;//Terathen drone - t2a
					case 0x48: CarveTarget(s, 0, 5, 7, 0, 0); break;//Terathen warrior, Terathen Avenger - t2a
					case 0x4B: CarveTarget(s, 0, 15, 5, 0, 0); break;//Titan - t2a
					case 0x4C: CarveTarget(s, 0, 5, 2, 0, 0); break;//Cyclopedian Warrior - t2a
					case 0x50: CarveTarget(s, 0, 10, 2, 0, 0); break;//Giant Toad - t2a
					case 0x51: CarveTarget(s, 0, 4, 1, 0, 0); break;//Bullfrog - t2a
					case 0x55: CarveTarget(s, 0, 5, 7, 0, 0); break;//Ophidian apprentice, Ophidian Shaman - t2a
					case 0x56: CarveTarget(s, 0, 5, 7, 0, 0); break;//Ophidian warrior, Ophidian Enforcer, Ophidian Avenger - t2a
					case 0x57: CarveTarget(s, 0, 5, 7, 0, 0); break;//Ophidian Matriarche - t2a
					case 0x5F: CarveTarget(s, 0, 19, 20, 0, 0); break;//Kraken - t2a
						//case 0x3E-case 0x95: break;                     //-NULL-
					case 0x96: CarveTarget(s, 0, 10, 0, 0, 0); break; //Sea Monster
					case 0x97: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Dolphin
						//case 0x98-case 0xC7: break;                     //-NULL-
					case 0xC8: CarveTarget(s, 0, 3, 10, 0, 0); break; //Horse (tan)
					case 0xC9: CarveTarget(s, 0, 1, 0, 1, 0); break;  //Cat
					case 0xCA: CarveTarget(s, 0, 1, 12, 0, 0); break; //Alligator
					case 0xCB: CarveTarget(s, 0, 6, 0, 0, 0); break;  //Pig
					case 0xCC: CarveTarget(s, 0, 3, 10, 0, 0); break; //Horse (dark)
					case 0xCD: CarveTarget(s, 0, 1, 0, 1, 0); break;  //Rabbit
					case 0xCE: CarveTarget(s, 0, 1, 12,0 ,0); break;  //Lava Lizard - t2a
					case 0xCF: CarveTarget(s, 0, 3, 0, 0, 1); break;  //Sheep
					case 0xD0: CarveTarget(s, 25, 1, 0, 0, 0); break; //Chicken
					case 0xD1: CarveTarget(s, 0, 2, 8, 0, 0); break;  //Goat
					case 0xD2: CarveTarget(s, 0, 15, 0, 0, 0); break;  //Desert Ostarge - t2a
					case 0xD3: CarveTarget(s, 0, 1, 0, 1, 0); break;  //Bear
					case 0xD4: CarveTarget(s, 0, 1, 0, 1, 0); break;  //Grizzly Bear
					case 0xD5: CarveTarget(s, 0, 2, 0, 3, 0); break;  //Polar Bear
					case 0xD6: CarveTarget(s, 0, 1, 0, 1, 0); break;  //Cougar
					case 0xD7: CarveTarget(s, 0, 1, 0, 1, 0); break;  //Giant Rat
					case 0xD8: CarveTarget(s, 0, 8, 12, 0, 0); break; //Cow (black)
					case 0xD9: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Dog
					case 0xDA: CarveTarget(s, 0, 15, 0, 0, 0); break;  //Frenzied Ostard - t2a
					case 0xDB: CarveTarget(s, 0, 15, 0, 0, 0); break;  //Forest Ostard - t2a
					case 0xDC: CarveTarget(s, 0, 7, 0, 12, 0); break;  //Llama
					case 0xDD: CarveTarget(s, 0, 1, 12, 0, 0); break; //Walrus
					case 0xDE: break;                                 //-NULL-
					case 0xDF: CarveTarget(s, 0, 3, 0, 0, 0); break;  //Sheep (BALD)
					case 0xE1: CarveTarget(s, 0, 1, 0, 1, 0); break;  //Timber Wolf
					case 0xE2: CarveTarget(s, 0, 3, 10, 0, 0); break; //Horse (Silver)
					case 0xE3: break;                                 //-NULL-
					case 0xE4: CarveTarget(s, 0, 3, 10, 0, 0); break; //Horse (tan)
					case 0xE5: break;                                 //-NULL-
					case 0xE6: break;                                 //-NULL-
					case 0xE7: CarveTarget(s, 0, 3, 12, 0, 0); break; //Cow (brown)
					case 0xE8: CarveTarget(s, 0, 10, 15, 0, 0); break;//Bull (brown)
					case 0xE9: CarveTarget(s, 0, 10, 15, 0, 0); break;//Bull (d-brown)
					case 0xEA: CarveTarget(s, 0, 6, 15, 0, 0); break; //Great Heart
					case 0xEB: break;                                 //-NULL-
					case 0xEC: break;                                 //-NULL-
					case 0xED: CarveTarget(s, 0, 5, 8, 0, 0); break;  //Hind
					case 0xEE: CarveTarget(s, 0, 1, 0, 0, 0); break;  //Rat
						//case 0xEF-case 0xFF: break;                     //-NULL-   
					default:
						printf("ERROR: Fallout of switch statement without default. uox3.cpp, corpsetarget()\n"); //Morrolan
					}// switch
				 }
			} 
			else 
			{
				sysmessage(s, "You carve the corpse but find nothing useful.");
			}// if more1==0
			//break;
		}
	}// if i!=-1
  if (!n) sysmessage(s, "That is too far away.");
}

void cTargets::CarveTarget( UOXSOCKET s, int feat, int ribs, int hides, int fur, int wool )
{
	int c;
	int feather=0;
	int meat=0;
	int leath=0;
	int furs=0;
	int wools=0;
	
	c = Items->SpawnItem( s, 1, "#", 0, 0x12, 0x2A, 0, 0, 0, 0 );  //add the blood puddle
	if( c == -1 ) return;
//	if( fur == -1 )
//	{
//		items[c].color1 = 0x00;
//		items[c].color2 = 0xEF;
//	}

	items[c].x = items[npcshape[0]].x;
	items[c].y = items[npcshape[0]].y;
	items[c].z = items[npcshape[0]].z;

	items[c].magic = 2; // AntiChrist - makes the item unmovable
	mapRegions->RemoveItem(c);
    mapRegions->AddItem(c); // lord Binary
	
	items[c].decaytime = (unsigned int)(uiCurrentTime+(server_data.decaytimer*CLOCKS_PER_SEC));
	RefreshItem( c ); // AntiChrist
	
	if( feat )
	{
		c = Items->SpawnItem( s, currchar[s], feat,"feathers", 1, 0x1B, 0xD1, 0, 0, 1, 1 );
		if( c == -1 ) 
			return;
		items[c].layer=0x01;
		items[c].att=5;
		feather=1;
		
	}
	if (ribs)
	{
		c=Items->SpawnItem( s, currchar[s], ribs, "raw ribs", 1, 0x09, 0xF1, 0, 0, 1, 1 );
		if( c == -1 ) 
			return;
		items[c].layer=0x01;
		items[c].att=5;
		meat=1;
	}
	
	if (hides)
	{
		c = Items->SpawnItem( s, currchar[s], hides, "hides", 1, 0x10, 0x78, 0, 0, 1, 1 );
		if( c == -1 ) 
			return;
		if( fur == -1 )
		{
			items[c].color1 = 0x00;
			items[c].color2 = 0xEF;
			strcpy( items[c].name, "Dragonskin" );
			sendbpitem( s, c );	// update Display after tweaking item.
			fur = 0;
		}
		items[c].layer=0x01;
		items[c].att=5;
		leath=1;
	}
	
	if (fur)
	{
		c = Items->SpawnItem( s, currchar[s], fur, "fur", 1, 0x11, 0xFA, 0, 0, 1, 1 );
		if( c == -1 ) 
			return;
		items[c].layer=0x01;
		items[c].att=5;
		furs=1;
	}
	
	if (wool)
	{
		c = Items->SpawnItem( s, currchar[s], wool, "unspun wool", 1, 0x0D, 0xF8, 0, 0, 1, 1 );
		if( c == -1 ) 
			return;
		items[c].layer=0x01;
		items[c].att=5;
		wools=1;
	}
	
	if(feather) sysmessage(s,"You pluck the bird and get some feathers.");
	if(meat) sysmessage(s,"You carve away some meat.");
	if(leath) sysmessage(s,"You skin the corpse and get the hides.");
	if(furs) sysmessage(s, "You skin the corpse and get some fur.");
	if(wools) sysmessage(s, "You skin the corpse and get some unspun wool.");
	Weight->NewCalc(currchar[s]);
}

//AntiChrist - new carving system - 3/11/99
//Human-corpse carving code added
//Scriptable carving product added
void cTargets::newCarveTarget( UOXSOCKET s, ITEM i )
{
	bool deletecorpse = false;
	int c = Items->SpawnItem( s, 1, "#", 0, 0x12, 0x2A, 0, 0, 0, 0 ); // add the blood puddle
	if( c == -1 ) 
		return;
	items[c].x = items[npcshape[0]].x;
	items[c].y = items[npcshape[0]].y;
	items[c].z = items[npcshape[0]].z;

	items[c].magic = 2; // AntiChrist - makes the item unmovable

	mapRegions->RemoveItem( c );
	mapRegions->AddItem( c );

	items[c].decaytime = (unsigned int)( uiCurrentTime + ( server_data.decaytimer * CLOCKS_PER_SEC ) );
	RefreshItem( c );

	// if it's a human corpse
	if( items[i].morey )
	{
		// create the Head
		sprintf( temp, "the head of %s", items[i].name2 );
		c = Items->SpawnItem( s, 1, temp, 1, 0x1D, 0xA0, 0, 0, 0, 0 );
		if( c == -1 ) return;
		setserial( c, i, 1 );
		items[c].layer = 0x01;
		items[c].att = 5;
		items[c].owner1 = items[i].owner1;
		items[c].owner2 = items[i].owner2;
		items[c].owner3 = items[i].owner3;
		items[c].owner4 = items[i].owner4;
		items[c].ownserial = items[i].ownserial;

		// create the Body

		sprintf( temp, "the body of %s", items[i].name2 );
		c = Items->SpawnItem( s, 1, temp, 1, 0x1D, 0x9F, 0, 0, 0, 0 );
		if( c == -1 ) return;
		setserial( c, i, 1 );
		items[c].layer = 0x01;
		items[c].att = 5;
		items[c].owner1 = items[i].owner1;
		items[c].owner2 = items[i].owner2;
		items[c].owner3 = items[i].owner3;
		items[c].owner4 = items[i].owner4;
		items[c].ownserial = items[i].ownserial;

		sprintf( temp, "the heart of %s", items[i].name2 );
		c = Items->SpawnItem( s, 1, temp, 1, 0x1C, 0xED, 0, 0, 0, 0 );
		if( c == -1 ) return;
		setserial( c, i, 1 );
		items[c].layer = 0x01;
		items[c].att = 5;
		items[c].owner1 = items[i].owner1;
		items[c].owner2 = items[i].owner2;
		items[c].owner3 = items[i].owner3;
		items[c].owner4 = items[i].owner4;
		items[c].ownserial = items[i].ownserial;

		// create the Left Arm
		sprintf( temp, "the left arm of %s", items[i].name2 );
		c = Items->SpawnItem( s, 1, temp, 1, 0x1D, 0xA1, 0, 0, 0, 0 );
		if( c == -1 ) return;
		setserial( c, i, 1 );
		items[c].layer = 0x01;
		items[c].att = 5;
		items[c].owner1 = items[i].owner1;
		items[c].owner2 = items[i].owner2;
		items[c].owner3 = items[i].owner3;
		items[c].owner4 = items[i].owner4;
		items[c].ownserial = items[i].ownserial;

		// create the Right Arm
		sprintf( temp, "the right arm of %s", items[i].name2 );
		c = Items->SpawnItem( s, 1, temp, 1, 0x1D, 0xA2, 0, 0, 0, 0 );
		if( c == -1 ) return;
		setserial( c, i, 1 );
		items[c].layer = 0x01;
		items[c].att = 5;
		items[c].owner1 = items[i].owner1;
		items[c].owner2 = items[i].owner2;
		items[c].owner3 = items[i].owner3;
		items[c].owner4 = items[i].owner4;
		items[c].ownserial = items[i].ownserial;

		// create the Left Leg
		sprintf( temp, "the left leg of %s", items[i].name2 );
		c = Items->SpawnItem( s, 1, temp, 1, 0x1D, 0xA3, 0, 0, 0, 0 );
		if( c == -1 ) return;
		setserial( c, i, 1 );
		items[c].layer = 0x01;
		items[c].att = 5;
		items[c].owner1 = items[i].owner1;
		items[c].owner2 = items[i].owner2;
		items[c].owner3 = items[i].owner3;
		items[c].owner4 = items[i].owner4;
		items[c].ownserial = items[i].ownserial;

		// create the Right Leg
		sprintf( temp, "the right leg of %s", items[i].name2 );
		c = Items->SpawnItem( s, 1, temp, 1, 0x1D, 0xA4, 0, 0, 0, 0 );
		if( c == -1 ) return;
		setserial( c, i, 1 );
		items[c].layer = 0x01;
		items[c].att = 5;
		items[c].owner1 = items[i].owner1;
		items[c].owner2 = items[i].owner2;
		items[c].owner3 = items[i].owner3;
		items[c].owner4 = items[i].owner4;
		items[c].ownserial = items[i].ownserial;

		//human: always delete corpse!
		deletecorpse = true;
		criminal( currchar[s] );
	}
/*	else
	{
		openscript("carve.scp");
		sprintf( sect, "CARVE %i", items[i].carve );
		if( !i_scripts[carve_script]->find( sect ) )
		{
			closescript();
			return;
		}
		do
		{
			read2();
			if (script1[0]!='}')
			{ 
				if (!(strcmp("ADDITEM",script1)))
				{
					storeval=str2num(script2);
					pos=ftell(scpfile);
					closescript();
					n=Items->CreateScriptItem(s,storeval,0);
					items[n].layer=0;
					setserial(n, i, 1);
					items[n].x=20+(rand()%50);
					items[n].y=85+(rand()%75);
					items[n].z=9;
					RefreshItem(n);//let's finally refresh the item
					sprintf(script1, "DUMMY");
					openscript("carve.scp");
					fseek(scpfile, pos, SEEK_SET);
				}
			}  
		}
		while (script1[0]!='}');

		closescript();
	}*/

	if( deletecorpse ) // if corpse has to be deleted
	{
		// let's empty it
		int serial = items[i].serial;
		int serhash = serial%HASHMAX;
		for( int ci = 0; ci < contsp[serhash].max; ci++ )
		{
			c = contsp[serhash].pointer[ci];
			if( c != -1 ) // lb
			{
				if( items[c].contserial == items[i].serial )
				{
					if( items[c].contserial != -1 ) removefromptr( &contsp[items[c].contserial%HASHMAX], c );
					items[c].cont1 = 255;
					items[c].cont2 = 255;
					items[c].cont3 = 255;
					items[c].cont4 = 255;
					items[c].contserial = -1;
					mapRegions->RemoveItem( c );
					items[c].x = items[i].x;
					items[c].y = items[i].y;
					items[c].z = items[i].z;
					mapRegions->AddItem( c ); // add this item to a map cell
					items[c].decaytime = (unsigned int)( uiCurrentTime + ( server_data.decaytimer * CLOCKS_PER_SEC ) );
					RefreshItem( c ); // AntiChrist
				} // if contserial == serial
			} // if c != -1 
		} // for

		// and then delete the corpse
		Items->DeleItem( i );
	}
}

void cTargets::TitleTarget(int s)
{
	int i,serial;
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		strcpy(chars[i].title,xtext[s]);
	}
}

void cTargets::NpcTarget( UOXSOCKET s )
{
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	int i = calcCharFromSer( serial );
	if( i != -1 )
	{
		addid1[s] = chars[i].ser1;
		addid2[s] = chars[i].ser2;
		addid3[s] = chars[i].ser3;
		addid4[s] = chars[i].ser4;
		target(s, 0, 1, 0, 57, "Select NPC to follow this player.");
	}
}

void cTargets::NpcTarget2( UOXSOCKET s )
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
		if( chars[i].npc == 1 )
		{
			chars[i].ftarg = calcCharFromSer( addid1[s], addid2[s], addid3[s], addid4[s] );
			chars[i].npcWander=1;
		}
}

void cTargets::NpcRectTarget( UOXSOCKET s )
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
		if( chars[i].npc == 1 )
		{
			chars[i].fx1=addx[s];
			chars[i].fy1=addy[s];
			chars[i].fz1=-1;
			chars[i].fx2=addx2[s];
			chars[i].fy2=addy2[s];
			chars[i].npcWander=3;
		}
}

void cTargets::NpcCircleTarget( UOXSOCKET s )
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
		if ((chars[i].npc==1))
		{
			chars[i].fx1=addx[s];
			chars[i].fy1=addy[s];
			chars[i].fz1=-1;
			chars[i].fx2=addx2[s];
			chars[i].npcWander=4;
		}
}

void cTargets::NpcWanderTarget( UOXSOCKET s )
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if (i!=-1)
		if ((chars[i].npc==1)) 
			chars[i].npcWander=npcshape[0];
}

void cTargets::NpcAITarget(int s)
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if (i!=-1)
	{
		chars[i].npcaitype=addx[s];
	}
}

void cTargets::xBankTarget(int s)
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if (i!=-1)
	{
		openbank(s, i);
	}
}
void cTargets::xSpecialBankTarget( int s ) // AntiChrist
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		openspecialbank( s, i );
	}
}

void cTargets::DupeTarget(int s)
{
	int dupetimes,serial;
	
	if (addid1[s]>=1)
	{
		dupetimes=addid1[s];
		int dupeit;
		int i;
		
		
		serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
		i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
		if (i!=-1)
		{
			for (dupeit=0;dupeit<dupetimes;dupeit++)
			{
				Commands->DupeItem( s, i, items[i].amount );	// we want to dupe the pile properly, shouldn't
																// dupe a stack of gold and get ONE coin
			}
		}
	}
}

void cTargets::MoveToBagTarget(int s)
{
	int i, j, x, y, serial;
	x=rand()%80;
	y=rand()%80;
	
	int p=packitem(currchar[s]);
	//p=packitem(currchar[s]);
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
		if ((items[i].serial==serial))
		{
			items[i].x=x+50;
			items[i].y=y+50;
			items[i].z=9;
			if( p > -1 ) 
			{
				unsetserial( i, 1 );
				setserial( i, p, 1 );
			}
			items[i].layer=0x00;
			items[i].decaytime=0;//reset decaytimer
			removeitem[1]=items[i].ser1;
			removeitem[2]=items[i].ser2;
			removeitem[3]=items[i].ser3;
			removeitem[4]=items[i].ser4;
			for(j=0;j<now;j++) if (perm[j])
			{
				Network->xSend(j, removeitem, 5, 0);
			}
				RefreshItem( i ); // AntiChrist
		}
}

void cTargets::SellStuffTarget(int s)
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if (i!=-1)
	{
		sellstuff(s, i);
	}
}

void cTargets::ReleaseTarget(int s,int c)
{
	int i,serial;
	
	if(c==-1)
	{
		serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
		i=findbyserial(&charsp[serial%HASHMAX], serial, 1);	
	}
	else
	{
		i=findbyserial(&charsp[c%HASHMAX], c, 1);
		//	i=c;
	}
	if (i!=-1)
	{
		if(chars[i].cell==0)
		{
			sysmessage(s,"That player is not in jail!");
		}
		else
		{
			jails[chars[i].cell].occupied=0;
			mapRegions->RemoveItem(i+10000000);
			chars[i].x=chars[i].oldx;
			chars[i].y=chars[i].oldy;
			chars[i].dispz=chars[i].z=chars[i].oldz;
			mapRegions->AddItem(i+1000000);
			
			chars[i].cell=0;
			teleport(i);
			sprintf(temp,"Player %s released.",chars[i].name);
			sysmessage(s,temp);
		}
	}
}

void cTargets::GmOpenTarget(int s)
{
	int i,serial,serhash,ci;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
			if ((items[i].contserial==serial) && (items[i].layer==addmitem[s]))
			{
				backpack(s,items[i].ser1,items[i].ser2,items[i].ser3,items[i].ser4);
				return;
			}
	}
	sysmessage(s,"No object was found at that layer on that character");
}

void cTargets::StaminaTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		soundeffect2(i, 0x01, 0xF2);
		staticeffect(i, 0x37, 0x6A, 0x09, 0x06);
		chars[i].stm=chars[i].dx;
		updatestats(i, 2);
		return;
	}
	sysmessage(s,"That is not a person.");
}

void cTargets::ManaTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		soundeffect2(i, 0x01, 0xF2);
		staticeffect(i, 0x37, 0x6A, 0x09, 0x06);
		chars[i].mn=chars[i].in;
		updatestats(i, 1);
		return;
	}
	sysmessage(s,"That is not a person.");
}

void cTargets::MakeShopTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		Commands->MakeShop(i);
		teleport(i);
		sysmessage(s, "The buy containers have been added.");
		return;
	}
	sysmessage(s, "Target character not found...");
}

void cTargets::JailTarget(int s, int c)
{
	int i,tmpnum=0,serial;
	
	if (c==-1)
	{
		serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
		i=findbyserial(&charsp[serial%HASHMAX], serial, 1);	  
			tmpnum=i;
	}
	else
	{
		i=findbyserial(&charsp[c%HASHMAX], c, 1);
		tmpnum=i;
	}
	
    if (tmpnum==-1) return; //lb

	if(chars[tmpnum].cell>0)
	{
		sysmessage(s,"That player is already in jail!");
		return;
	}
	
    for (i=1;i<11;i++)
    {
		if(jails[i].occupied==0)
		{
			mapRegions->RemoveItem(tmpnum+1000000);
			chars[tmpnum].oldx=chars[tmpnum].x;
			chars[tmpnum].oldy=chars[tmpnum].y;
			chars[tmpnum].oldz=chars[tmpnum].z;
			chars[tmpnum].x=jails[i].x;
			chars[tmpnum].y=jails[i].y;
			chars[tmpnum].dispz=chars[tmpnum].z=jails[i].z;
			mapRegions->AddItem(tmpnum+1000000);
			
			chars[tmpnum].cell=i;
			teleport(tmpnum);
			jails[i].occupied=1;
			sprintf(temp,"Player %s has been jailed in jail %i.",chars[tmpnum].name,i);
			sysmessage(s,temp);
			return;
		}
	}
    sysmessage( s, "All jails are currently full!" );
	
}

void cTargets::AttackTarget(UOXSOCKET s)
{
	int target, target2;
	
	target = calcCharFromSer(addid1[s], addid2[s], addid3[s], addid4[s]);
	target2 = calcCharFromSer(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	
	if (target2==-1 || target==-1) return;
	if( target2 == target )
	{
		sysmessage( s, "Your pet cannot attack itself" );
		return;
	}
	npcattacktarget(target2, target);
}

void cTargets::FollowTarget(int s)
{
	int char1, char2;
	
	char1=calcCharFromSer(addid1[s], addid2[s], addid3[s], addid4[s]);
	char2=calcCharFromSer(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	
	chars[char1].ftarg=char2;
	chars[char1].npcWander=1;
}

void cTargets::TransferTarget(int s)
{
	int char1, char2;
	char t[120];
	
	char1=calcCharFromSer(addid1[s], addid2[s], addid3[s], addid4[s]);
	char2=calcCharFromSer(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	
	sprintf(t,"* %s will now take %s as his master *",chars[char1].name,chars[char2].name);
	npctalkall(char1,t, 0);
	
	if( chars[char1].ownserial != -1 ) removefromptr(&cownsp[chars[char1].ownserial%HASHMAX], char1);
	setserial(char1,char2, 5);
	chars[char1].npcWander=1;
	
	chars[char1].ftarg=-1;
	chars[char1].npcWander=0;
}

void cTargets::BuyShopTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
		if ((chars[i].serial==serial))
		{
			Targ->BuyShop(s, i);
			return;
		}
		sysmessage(s, "Target shopkeeper not found...");
}

int cTargets::BuyShop(int s, int c)
{
	int i, cont1, cont2,serial,serhash,ci;
	char shopgumpopen[8]="\x24\x00\x00\x00\x01\x00\x30";
	
	cont1=-1;
	cont2=-1;
	serial=chars[c].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
		{
			if ((items[i].contserial==serial))
			{
				if (items[i].layer==0x1A)
				{
					cont1=i;
				}
				else if (items[i].layer==0x1B)
				{
					cont2=i;
				}
			}
		}
	}
	
	if (cont1==-1 || cont2==-1)
	{
		return 0;
	}
	
	impowncreate( s, c, 0 ); // Send the NPC again to make sure info is current. (OSI does this we might not have to)
	sendshopinfo( s, c, cont1 ); // Send normal shop itemsa
	sendshopinfo( s, c, cont2 ); // Send items sold to shop by players
	shopgumpopen[1] = chars[c].ser1;
	shopgumpopen[2] = chars[c].ser2;
	shopgumpopen[3] = chars[c].ser3;
	shopgumpopen[4] = chars[c].ser4;
	Network->xSend( s, shopgumpopen, 7, 0 );
	statwindow( s, currchar[s] ); // Make sure the gold total has been sent.
	return 1;
}

void cTargets::SetValueTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if ((i!=-1))
	{
		items[i].value=addx[s];
		return;
	}
	sysmessage(s, "Target item not found.");
}

void cTargets::SetRestockTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if ((i!=-1))
	{
		items[i].restock=addx[s];
		return;
	}
	sysmessage(s, "Target item not found.");
}


void cTargets::permHideTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		if(chars[i].hidden==1)
		{
			sysmessage(s,"You are already hiding");
			return;
		}
		chars[i].priv2=chars[i].priv2|8;
		chars[i].hidden=1;
		updatechar(i); // crashfix LB
	}
}

void cTargets::unHideTarget(int s)
{
	addx[s] = 0;
	VisibleTarget(s);	// better code reuse!
}

void cTargets::SetWipeTarget(int s)
{
	int i/*, j*/,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		items[i].wipe=addid1[s];
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
	}
}
void cTargets::SetSpeechTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		if (chars[i].npc==0)
		{
			sysmessage(s,"You can only change speech for npcs.");
			return;
		}
		chars[i].speech=addx[s];
	}
}


void cTargets::SetSpAttackTarget(int s)
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].spattack=tempint[s];
	}
}

void cTargets::SetSpaDelayTarget(int s)
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].spadelay=tempint[s];
	}
}

void cTargets::SetPoisonTarget(int s)
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].poison=tempint[s];
	}
}

void cTargets::SetPoisonedTarget(int s)
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].poisoned=tempint[s];
		chars[i].poisonwearofftime = (unsigned int)(uiCurrentTime+(CLOCKS_PER_SEC*server_data.poisontimer));
		impowncreate(calcSocketFromChar(i),i,1); //Lb, sends the green bar ! 
		
	}
}

void cTargets::FullStatsTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		soundeffect2(i, 0x01, 0xF2);
		staticeffect(i, 0x37, 0x6A, 0x09, 0x06);
		chars[i].mn=chars[i].in;
		chars[i].hp=chars[i].st;
		chars[i].stm=chars[i].dx;
		updatestats(i, 0);
		updatestats(i, 1);
		updatestats(i, 2);
		return;
	}
	sysmessage(s,"That is not a person.");
}


void cTargets::SetAdvObjTarget(int s)
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		chars[i].advobj=tempint[s];
	}
}

//---------------------------------------------------------------------------o
//|	Class		:	void cantraintarget(int s)
//|	Date		:	1-3-99
//|	Programmer	:	Antrhacks
//o---------------------------------------------------------------------------o
//| Purpose		:Used for training by NPC's
//o---------------------------------------------------------------------------o
void cTargets::CanTrainTarget(int s)
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if (i!=-1)
	{
		if (chars[i].npc==0)
		{
			sysmessage(s, "Only NPC's may train.");
			return;
		}
		chars[i].cantrain=(chars[i].cantrain^1);  //turn on if off, off if on
	}
}

void cTargets::SetSplitTarget(int s)
{
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if (i!=-1)
	{
		chars[i].split=tempint[s];
		//break;
		//}
	}
}

void cTargets::SetSplitChanceTarget(int s)
{
	
	int serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if (i!=-1)
	{
		chars[i].splitchnc=tempint[s];
		//break;
		//}
	}
}

void cTargets::AxeTarget(int s)
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return;
	
    // [krazyglue] it may take more lines, but at least its readable and easier to debug =)
	if (
        (
         (buffer[s][0x11]==0x0C)&&
         (
          (buffer[s][0x12]==0xD0)||
          (buffer[s][0x12]==0xD3)||
          (buffer[s][0x12]==0xD6)||
          (buffer[s][0x12]==0xD8)||
          (buffer[s][0x12]==0xDA)||
          (buffer[s][0x12]==0xDD)||
          (buffer[s][0x12]==0xE0)||
          (buffer[s][0x12]==0xE3)||
          (buffer[s][0x12]==0xE6)||
          (
           (buffer[s][0x12] >= 0xCA)&&
           (buffer[s][0x12] <= 0xCD)
          )
         )
        )
        ||
	    (
         (buffer[s][0x11]==0x12)&&

         (buffer[s][0x12]>=0xB8)&&
         (buffer[s][0x12]<=0xBB)
        )
        ||
        (
         (buffer[s][0x11]==0x0D)&&
		 (
          (buffer[s][0x12]==0x42)||
          (buffer[s][0x12]==0x43)||
          (buffer[s][0x12]==0x58)||
		  (buffer[s][0x12]==0x59)||
          (buffer[s][0x12]==0x70)||
          (buffer[s][0x12]==0x85)||
		  (buffer[s][0x12]==0x94)||
          (buffer[s][0x12]==0x95)||
          (buffer[s][0x12]==0x98)||
		  (buffer[s][0x12]==0xa4)||
          (buffer[s][0x12]==0xa8)||
          (buffer[s][0x12]==0x58)
         )
        )
       )
	{
		Skills->TreeTarget(s);
	}
    else if ((buffer[s][0x11]==0x20) && (buffer[s][0x12]==0x06))
	{
		Targ->CorpseTarget(s);
	}
    else //     -- not sure if this is right... still researching this packet [krazyglue]
    {
		Skills->BowCraft( s );
//        sysmessage(s,"You are too far away to perform that action.");
    }
}

void cTargets::ObjPrivTarget(int s)
{
	int i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if (i!=-1)
	{
		switch( addid1[s] )
		{
		case 0:	items[i].priv &= 0xFE;	break;
		case 1: items[i].priv |= 0x01;	break;
		case 3: items[i].priv = addid2[s];	break;
		}
	}
}

void cTargets::SetDirTarget(int s)
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	
	if (buffer[s][7]>=0x40)
	{
		i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
		if (i!=-1)
		{
			items[i].dir=addx[s];
//			for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
			RefreshItem( i ); // AntiChrist
			return;
		}
	}
	else
	{
		i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
		if (i!=-1)
		{
			chars[i].dir=addx[s] & 0x0F;	// make sure high-bits are cleared
			updatechar(i);
			return;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void npcresurrecttarget(CHARACTER i)
//|   Date        -  UnKnown
//|   Programmer  -  UnKnown  (Touched tabstops by Tauriel Dec 28, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Resurrects a character
//o---------------------------------------------------------------------------o
void cTargets::NpcResurrectTarget( CHARACTER i )
{
	int j,c, ptr;
	if( chars[i].npc )
	{
		printf( "Resurrect attempted on character %i", i );
		return;
	}
	if (chars[i].dead==1)
	{//Shouldn' be a validNPCMove inside a door, might fix house break in. -- from zippy code
		Fame(i,0);
		soundeffect2(i, 0x02, 0x14);
		chars[i].id1=chars[i].xid1=chars[i].orgid1;
		chars[i].id2=chars[i].xid2=chars[i].orgid2;
		chars[i].skin1=chars[i].xskin1;
		chars[i].skin2=chars[i].xskin2;
		chars[i].dead=0;
		chars[i].hp=chars[i].st;// /10;
		chars[i].stm=chars[i].dx;// /10;
		chars[i].mn=chars[i].in/10;
		chars[i].attacker=-1;
		chars[i].attackfirst=0;
		chars[i].war=0;
		
		
		//for (j=0;j<itemcount;j++)
		for (ptr=0;ptr<contsp[chars[i].serial%HASHMAX].max;ptr++)
		{
			j=contsp[chars[i].serial%HASHMAX].pointer[ptr];
			if (j >-1 && !items[j].free)
			{
				if (items[j].contserial==chars[i].serial && items[j].layer==0x1A)
				{
					items[j].layer=0x15;
					chars[i].packitem=j;  //Tauriel packitem speedup
				}
				if ((items[j].ser1==chars[i].robe1)&&(items[j].ser2==chars[i].robe2)&&
					(items[j].ser3==chars[i].robe3)&&(items[j].ser4==chars[i].robe4))
				{
					Items->DeleItem(j);
					
					c=Items->SpawnItem(calcSocketFromChar(i),i,1,"a resurrect robe",0,0x1F,0x03,0,0,0,0);
					if( c != -1 )
					{
						setserial(c,i,4);
						items[c].layer=0x16;
						items[c].dye=1;
					}
				}
			}
		}
		teleport(i);
	}
	else
		sysmessage( calcSocketFromChar( i ), "That person isn't dead" );
	return;
}


void cTargets::NewXTarget(int s) // Notice a high similarity to th function above?  Wonder why.  - Gandalf
{
	//int i,j,serial;
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		mapRegions->RemoveItem(i); //lb
		items[i].x=addx[s];
		mapRegions->AddItem(i);
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
	}
	
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		mapRegions->RemoveItem(i+1000000);
		chars[i].x=addx[s];
		mapRegions->AddItem(i+1000000);
		teleport(i);
	}
	
}

void cTargets::NewYTarget(int s)

{
	int i/*,j*/,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		mapRegions->RemoveItem(i); //lb
		items[i].y=addx[s];
		mapRegions->AddItem(i);
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
	
	}
	
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
        mapRegions->RemoveItem(i+1000000);
		chars[i].y=addx[s];
        mapRegions->AddItem(i+1000000);
		teleport(i);
	}
	
}

void cTargets::IncXTarget(int s)

{
	int i/*,j*/,serial;
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	
	if(i!=-1)
	{
		mapRegions->RemoveItem(i); //lb
		items[i].x=items[i].x + addx[s];
		mapRegions->AddItem(i);
		RefreshItem( i ); // AntiChrist
	}
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		mapRegions->RemoveItem(i+1000000);
		chars[i].x = chars[i].x + addx[s];
		mapRegions->AddItem(i+1000000);
		teleport(i);
		impowncreate( s, i, 1 );
	}
	
}

void cTargets::IncYTarget(int s)
{
	int i/*,j*/,serial;
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if(i!=-1)
	{
		mapRegions->RemoveItem(i); //lb
		items[i].y=items[i].y + addx[s];
		mapRegions->AddItem(i);
//		for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
		RefreshItem( i ); // AntiChrist
	}
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		mapRegions->RemoveItem(i+1000000);
		chars[i].y = chars[i].y + addx[s];
		mapRegions->AddItem(i+1000000);
		teleport(i);
		impowncreate( s, i, 1 );
	}
	
}

void cTargets::HouseOwnerTarget(int s) // crackerjack 8/10/99 - change house owner
{
	int own, sign, house, o_serial, serial, os, i;
	o_serial=calcserial(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	if( o_serial == -1 ) return;
	own=findbyserial(&charsp[o_serial%HASHMAX],o_serial,1);
	if(o_serial>-1) 
	{
		int key;
		serial=calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
		sign=findbyserial(&itemsp[serial%HASHMAX],serial,0);
		serial=calcserial(items[sign].more1, items[sign].more2, items[sign].more3, items[sign].more4);
		house=findbyserial(&itemsp[serial%HASHMAX],serial,0);

		if (sign ==-1 || house ==-1) return; //lb

		items[sign].owner1=buffer[s][7];
		items[sign].owner2=buffer[s][8];
		items[sign].owner3=buffer[s][9];
		items[sign].owner4=buffer[s][10];
		items[sign].ownserial=o_serial;
		setptr(&ownsp[items[sign].ownserial%HASHMAX], sign);

		items[house].owner1=buffer[s][7];
		items[house].owner2=buffer[s][8];
		items[house].owner3=buffer[s][9];
		items[house].owner4=buffer[s][10];
		items[house].ownserial=o_serial;
		setptr(&ownsp[items[sign].ownserial%HASHMAX], house);

		killkeys(items[house].ser1,items[house].ser2,
			items[house].ser3, items[house].ser4);

		os=-1;
		for(i=0;i<now&&os==-1;i++)
		{
			if( chars[currchar[i]].serial == chars[own].serial )
			{
				os = i;
			}
		}

		if(os!=-1) {
			key=Items->SpawnItem(os, 1, "a house key", 0, 0x10, 0x0F, 0, 0,1,1);//gold key for everything else
			if( key == -1 ) return;
		} else {
			key=Items->SpawnItem(os, 1, "a house key", 0, 0x10, 0x0F, 0,0,0,0);//gold key for everything else
			if( key == -1 ) return;
			items[key].x=chars[own].x;
			items[key].y=chars[own].y;
			items[key].z=chars[own].z;
			mapRegions->AddItem(key); //LB
//		 	sendinrange(key);
			RefreshItem( key ); // AntiChrist
		}

		items[key].more1=items[house].ser1;
		items[key].more2=items[house].ser2;
		items[key].more3=items[house].ser3;
		items[key].more4=items[house].ser4;
		items[key].type=7;

		sprintf(temp, "You have transferred your house to %s.", chars[own].name);
		sysmessage(s, temp);
		sprintf(temp, "%s has transferred a house to %s.", chars[currchar[s]].name, chars[own].name);

		for(serial=0;serial<now;serial++)
			if(serial!=s&&(((perm[serial])&&inrange1p(10, currchar[s]))||
				(chars[currchar[serial]].serial == o_serial)))
				sysmessage(serial, temp);
	}
}

void cTargets::HouseEjectTarget(int s) // crackerjack 8/11/99 - kick someone out of house
{
	int serial, c, h;
	serial=calcserial(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	c=findbyserial(&charsp[serial%HASHMAX],serial,1);
	serial=calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
	h=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	if((c!=-1)&&(h!=-1)) {
		int sx, sy, ex, ey;
		Map->MultiArea(h, &sx,&sy,&ex,&ey);
		if(chars[c].x>=sx&&chars[c].y>=sy&&chars[c].x<=ex&&chars[c].y<=ey) {
			mapRegions->RemoveItem(c+1000000);
			chars[c].x=ex;
			chars[c].y=ey;
			mapRegions->AddItem(c+1000000);
			teleport(c);
			sysmessage(s, "Player ejected.");
		} else {
			sysmessage(s, "That is not inside the house.");
		}
	}
}

void cTargets::HouseBanTarget(int s) // crackerjack 8/12/99 - ban someobdy from the house
{
	int serial, c, h;
	// first, eject the player
	Targ->HouseEjectTarget(s);
	serial=calcserial(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	c=findbyserial(&charsp[serial%HASHMAX],serial,1);
	serial=calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
	h=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	if((c!=-1)&&(h!=-1)) {
		int r;
		r=add_hlist(c, h, H_BAN);
		if(r==1) {
			sprintf(temp, "%s has been banned from this house.", chars[c].name);
			sysmessage(s, temp);
		} else if(r==2) {
			sysmessage(s, "That player is already on a house register.");
		} else {
			sysmessage(s, "That player is not on the property.");
		}
	}
}

void cTargets::HouseFriendTarget(int s) // crackerjack 8/12/99 - add somebody to friends list
{
	int serial, c, h;
	serial=calcserial(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	c=findbyserial(&charsp[serial%HASHMAX],serial,1);
	serial=calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
	h=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	if((c!=-1)&&(h!=-1)) {
		unsigned int r;
		r=add_hlist(c, h, H_FRIEND);
		switch( r )
		{
		case 1:
			sprintf(temp, "%s has been made a friend of the house.", chars[c].name);
			for( r = 0; r < now; r++ )
			{
				if( chars[currchar[r]].serial == chars[c].serial )
				{
					sysmessage(r, "You have been made a friend of the house.");
				}
			}
			sysmessage(s, temp);
			break;
		case 2:
			sysmessage(s, "That player is already on a house register.");
			break;
		case 3:
			sysmessage(s, "That player is not on the property.");
			break;
		}
	}
}

void cTargets::HouseUnlistTarget(int s) // crackerjack 8/12/99 - remove somebody from a list
{
	int serial, c, h;
	serial=calcserial(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	c=findbyserial(&charsp[serial%HASHMAX],serial,1);
	serial=calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
	h=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	if((c!=-1)&&(h!=-1)) {
		int r;
		r=del_hlist(c, h);
		if(r>0) {
			sprintf(temp, "%s has been removed from the house registry.", chars[c].name);
			sysmessage(s, temp);
		} else {
			sysmessage(s, "That player is not on the house registry.");
		}
	}
}

void cTargets::SetMurderCount( int s )
{
	if (buffer[s][7]==0xFF && buffer[s][8]==0xFF && buffer[s][9]==0xFF && buffer[s][10]==0xFF) return;
	
	int x;
	x = addmitem[s];

	int serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i = findbyserial( &charsp[serial % HASHMAX], serial, 1);
	if( i != -1 )
	{
		chars[i].kills = x;
		setcharflag( i );	// update flag now
		return;
	}
	sysmessage( s, "Cannot set murder count... invalid char!" );	
}

void cTargets::GlowTarget( UOXSOCKET s ) // LB 4/9/99 makes items glow
{
	int c, i, serial, k, l, j;
	serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	i = findbyserial( &itemsp[serial%HASHMAX], serial, 0 );

	// moved above following if to avoid crashing.
	if (i==-1) 
	{ 
		sysmessage(s,"No item found there, only items can be made to glow."); 
		return; 
	}
	if( items[i].contserial != -1 )
	{
		j = findbyserial( &itemsp[items[i].contserial%HASHMAX], items[i].contserial, 0 ); // in bp ?
		l = findbyserial( &charsp[items[i].contserial%HASHMAX], items[i].contserial, 1 ); // equipped ?
		if( l == -1 )
			k = GetPackOwner( j );
		else
			k = l;
		if( k != currchar[s] ) // Creation only allowed in the creators pack/char otherwise things could go wrong
		{
		  sysmessage( s, "You can't create glowing items in other people's packs or hands" );
		   return;
		}
	}

	if (items[i].glow!=0) 
	{
		sysmessage(s, "That object already glows!\n");
		return;
	}

	c = 0x99;	
	items[i].glow_c1 = items[i].color1; // backup old colors
	items[i].glow_c2 = items[i].color2;

	items[i].color1 = (unsigned char)(c<<8); // set new color to yellow
	items[i].color2 = (unsigned char)(c%256);

	c=Items->SpawnItem(s,1,"glower",0,0x16,0x47,0,0,0,1); // spawn light emitting object
	if( c == -1 )
		return;
    items[c].dir=29; // set light radius maximal
	items[c].visible=0; // only visible to gm's
	                    // well, no way around that gms can see it, thats client side hardcoded :(
	items[c].magic = 3;
	mapRegions->RemoveItem(c); // remove if add in spawnitem
	items[c].layer=items[i].layer;
	if (items[c].layer==0) // if not equipped -> coords of the light-object = coords of the 
	{
	  items[c].x=items[i].x;
	  items[c].y=items[i].y;
	  items[c].z=items[i].z;
	} else // if equipped -> place lightsource at player ( height= approx hand level )
	{ 
		items[c].x=chars[currchar[s]].x;
		items[c].y=chars[currchar[s]].y;
		items[c].z = (signed char)chars[currchar[s]].z+4;
	}

	//mapRegions->AddItem(c);
    items[c].priv=0; // doesnt decay

	items[i].glow=items[c].serial; // set glow-identifier

	RefreshItem( i ); // AntiChrist
	RefreshItem( c ); // AntiChrist

	impowncreate(s,currchar[s],0); // if equipped send new color too
	sysmessage( s, "Item is now glowing.");

}

void cTargets::UnglowTarget( UOXSOCKET s )
{
	int c,i,serial,j,k,l;
	serial=calcserial(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX],serial,0);

	if (i==-1) 
	{ 
		sysmessage(s,"No item found, only items can be made to unglow."); 
		return; 
	}

	if (items[i].contserial!=-1) 
	{      
	     j=findbyserial(&itemsp[items[i].contserial%HASHMAX],items[i].contserial,0); // in bp ?
		 l=findbyserial(&charsp[items[i].contserial%HASHMAX],items[i].contserial,1); // equipped ?
		 if (l==-1
			 ) k=GetPackOwner(j); 
		 else 
			 k=l;
		 
		 if (k!=currchar[s])  // creation only allowed in the creators pack/char otherwise things could go wrong
		 {
		   sysmessage(s,"You can't unglow items in other people's packs or hands");
		   return;
		 }
		   
	}

	c=items[i].glow;
	j=findbyserial(&itemsp[c%HASHMAX],c,0);

	if (items[i].glow==0 || j==-1 ) 
	{
		sysmessage(s,"That object doesnt glow!\n");
		return;
	}

	items[i].color1=items[i].glow_c1;
    items[i].color2=items[i].glow_c2; // restore old color

	Items->DeleItem(j); // delete glowing object    

	items[i].glow=0; // remove glow-identifier

	RefreshItem( i ); // AntiChrist
	impowncreate(s,currchar[s],0); // if equipped send new old color too

	removefromptr(&glowsp[chars[currchar[s]].serial%HASHMAX],i);

	sysmessage( s, "Item is no longer glowing.");
}

void cTargets::ShowSkillTarget( int s ) // LB's showskills
{
	int p, serial, a, i, j, k, b = 0, c, z, zz, ges = 0;
	char skill_info[(ALLSKILLS+1)*40];
	char sk[25];

	serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	p = findbyserial( &charsp[serial%HASHMAX], serial, 1 );

	if( p != -1 )
	{
		z = addx[s];
		if( z < 0 || z > 3 ) z = 0;
		if( z == 2 || z == 3 ) sprintf( skill_info, "%s's skills:", chars[p].name); else sprintf( skill_info, "%s's baseskills:", chars[p].name );
		b = strlen( chars[p].name ) + 11;
		if( b > 23 ) b = 23;

		for( c = b; c <= 26; c++ )
		{
			strcpy( temp, spc );
			strcpy( &skill_info[strlen( skill_info )], temp );
		}

		numtostr( ges, sk );
		sprintf( temp, "sum: %s", sk );
		strcpy( &skill_info[strlen( skill_info )], temp );
		for( a = 0; a < ALLSKILLS; a++ )
		{
			if( z == 0 || z == 1 ) k = chars[p].baseskill[a]; else k = chars[p].skill[a];
			if( z == 0 || z == 2 ) zz = 9; else zz = -1;

			if( k > zz ) // show only if skills >= 1
			{
				if( z == 2 || z == 3 ) j = chars[p].skill[a]/10; else j = chars[p].baseskill[a]/10; // get skill value
				numtostr( j, sk );  // skill-value string in sk
				ges += j;
				sprintf( temp, "%s%s%s", skillname[a], spc, sk );
				strcpy( &skill_info[strlen( skill_info )], temp );

				b = strlen( skillname[a] ) + strlen( sk ) + 1; // it doesn't like \n's, so insert spaces till end of line
				if( b > 23 ) b = 23;
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

		i = strlen( skill_info ) + 10;
		updscroll[1] = (unsigned char)(i>>8);
		updscroll[2] = (unsigned char)(i%256);
		updscroll[3] = 2;
		updscroll[8] = (unsigned char)((i-10)>>8);
		updscroll[9] = (unsigned char)((i-10)%256);
		Network->xSend( s, updscroll, 10, 0 );
		Network->xSend( s, skill_info, strlen( skill_info ), 0 );
	} else sysmessage( s, "No valid target" );
}



void cTargets::GuardTarget( UOXSOCKET s )
//PRE:	Pet has been commanded to guard
//POST: Pet guards person, if owner currently
//DEV:	Abaddon
//DATE: 3rd October
{
	SERIAL targToGuard, petGuarding;
	targToGuard = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	petGuarding = calcCharFromSer( addid1[s], addid2[s], addid3[s], addid4[s] );
	if( chars[targToGuard].serial != chars[petGuarding].ownserial )
	{
		sysmessage( s, "Currently can't guard anyone but yourself!" );
		return;
	}
	chars[petGuarding].npcaitype = 32;											// 32 is guard mode
	chars[currchar[s]].guarded = true;
}

void cTargets::ResurrectionTarget( UOXSOCKET s )
{
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	int i = calcCharFromSer( serial );
	if( i != -1 )
	{
		if( chars[i].dead == 1 )
		{
			Targ->NpcResurrectTarget( i );
			return;
		}
	}
}

bool ValidLockDown( int item )
// PRE:		item is an index into item
// POST:	returns true if lockable, or false if not
// CODER:	Abaddon
// DATE:	22nd January, 2000
{
	if( items[item].type == 12 || items[item].type == 13 ) // can't lock down a door
		return false;
	if( Items->isFieldSpellItem( item ) ) // can't lock down a field spell
		return false;
	if( ( items[item].id1 == 0x0B && items[item].id2 == 0xD2 ) || items[item].type == 203 ) // housing sign
		return false;
	return true;
}

void cTargets::HouseLockdown( UOXSOCKET s ) // Abaddon
// PRE:		S is the socket of a valid owner/coowner and is in a valid house
// POST:	either locks down the item, or puts a message to the owner saying he's a moron
// CODER:	Abaddon
// DATE:	17th December, 1999
{
	int itemToLockSer, itemToLock, houseSer, house;
	itemToLockSer = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	itemToLock = findbyserial( &itemsp[itemToLockSer%HASHMAX], itemToLockSer, 0 );

	if( itemToLock != -1 )
	{
		houseSer = calcserial( addid1[s], addid2[s], addid3[s], addid4[s] );	// let's find our house
		house = findbyserial( &itemsp[houseSer%HASHMAX], houseSer, 0 );
		// time to lock it down!
		int multi;
		multi = findmulti( items[itemToLock].x, items[itemToLock].y, items[itemToLock].z );
		if( multi != -1 )
		{
			if( multi != house )
			{
				sysmessage( s, "You can't lock something down that's not in your house!" );
				return;
			}
			if( !ValidLockDown( itemToLock ) )
			{
				sysmessage( s, "You can't lock that down!" );
				return;
			}
			items[itemToLock].magic = 3;	// LOCKED DOWN!
			RefreshItem( itemToLock );
			return;
		}
		else
		{
			// not in a multi!
			sysmessage( s, "That item is not in your house!" );
			return;
		}
	}
	else
	{
		sysmessage( s, "Invalid item!" );
		return;
	}
}

void cTargets::HouseRelease( UOXSOCKET s ) // Abaddon
// PRE:		S is the socket of a valid owner/coowner and is in a valid house, the item is locked down
// POST:	either releases the item from lockdown, or puts a message to the owner saying he's a moron
// CODER:	Abaddon
// DATE:	17th December, 1999
{
	int itemToLockSer, itemToLock, houseSer, house;
	itemToLockSer = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	itemToLock = findbyserial( &itemsp[itemToLockSer%HASHMAX], itemToLockSer, 0 );

	if( itemToLock != -1 )
	{
		houseSer = calcserial( addid1[s], addid2[s], addid3[s], addid4[s] );	// let's find our house
		house = findbyserial( &itemsp[houseSer%HASHMAX], houseSer, 0 );
		// time to lock it down!
		int multi;
		multi = findmulti( items[itemToLock].x, items[itemToLock].y, items[itemToLock].z );
		if( multi != -1 )
		{
			if( multi != house )
			{
				sysmessage( s, "You can't release something that's not in your house!" );
				return;
			}
			items[itemToLock].magic = 0;	// Default as stored by the client, perhaps we should keep a backup?
			RefreshItem( itemToLock );
			return;
		}
		else
		{
			// not in a multi!
			sysmessage( s, "That item is not in your house!" );
			return;
		}
	}
	else
	{
		sysmessage( s, "Invalid item!" );
		return;
	}
}

void cTargets::ShowDetail( UOXSOCKET s ) // Abaddon
// PRE:		S is the socket of the person getting the item information
// POST:	prints out detailed information about an item
// CODER:	Abaddon
// DATE:	11th January, 2000
{
	int itemToCheckSer, itemToCheck;
	char message[512];
	itemToCheckSer = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	itemToCheck = findbyserial( &itemsp[itemToCheckSer%HASHMAX], itemToCheckSer, 0 );

	if( itemToCheck != -1 )
	{
		switch( items[itemToCheck].type )
		{
		case 0:
			strcpy( message, "Default type" );
			break;
		case 1:	// container/backpack
			strcpy( message, "Container/backpack:" );
			if( items[itemToCheck].moreb1 > 0 )
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
			if( items[itemToCheck].moreb1 > 0 )
				strcat( message, "Magically trapped" );
			break;
		case 9:	// Spellbook (item 14FA)
			strcpy( message, "Spellbook" );
			break;
		case 10:	// map( item 0E EB )
			strcpy( message, "This opens a map based on the serial num of the item" );
			break;
		case 11:	// book
			sprintf( message, "A book:Entry in misc.scp: %li", (items[itemToCheck].more1<<24)+(items[itemToCheck].more2<<16)+(items[itemToCheck].more3<<8)+items[itemToCheck].more4);
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
			sprintf( message, "Magic wand\nCircle: %i:Spell within circle: %i:Charges left: %i", items[itemToCheck].morex, items[itemToCheck].morey, items[itemToCheck].morez );
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
			switch(items[itemToCheck].morey)
			{
			case 1: // Agility Potion
				switch(items[itemToCheck].morez)
				{
				case 1:		strcat( message, "Agility potion" );	break;
				case 2:		strcat( message, "Greater Agility potion" );	break;
				default:	strcat( message, "Unknown Agility potion" );	break;
				}
				break;
			case 2: // Cure Potion
				switch(items[itemToCheck].morez)
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
				switch(items[itemToCheck].morez)
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
				switch( items[itemToCheck].morez )
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
				switch(items[itemToCheck].morez)
				{
				case 1:		strcat( message, "Lesser Refresh potion" ); break;
				case 2:		strcat( message, "Refresh potion" );	break;
				default:	strcat( message, "Unknown Refresh potion" ); break;
				}
				break;
			case 8: // Strength Potion
				switch(items[itemToCheck].morez)
				{
				case 1:		strcat( message, "Lesser Strength potion" );	break;
				case 2:		strcat( message, "Strength potion" ); break;
				default:	strcat( message, "Unknown Strength potion" ); break;
				}
				break;
			case 9: // Mana Potion
				switch(items[itemToCheck].morez)
				{
				case 1:		strcat( message, "Lesser Mana potion" );	break;
				case 2:		strcat( message, "Mana potion" ); break;
				default:	strcat( message, "Unknown Mana potion" ); break;
				}
				break;
			case 10: // LB's LSD potion, 5'th November 1999
				strcat( message, "LSD potion" );
				break;
			default:
				strcat( message, "Unknown potion" );
				break;
			}
			break;
		case 35:	// townstone deed/stone
			if( items[itemToCheck].id1 == 0x14 && items[itemToCheck].id2 == 0xF0 )
				strcpy( message, "Townstone deed, will make townstone" );
			else
				strcpy( message, "Townstone, use to find out information on the town" );
			break;
		case 50:	// recall rune
			if ( items[itemToCheck].morex == 0 && items[itemToCheck].morey == 0 && items[itemToCheck].morez == 0 )	// changed, to fix, Lord Vader
				strcpy( message,"Unmarked recall rune");
			else
				strcpy( message, "This will rename a recall rune" );
			break;
		case 51:	// start gate
			sprintf( message, "Start gate going to X %i Y %i Z %i", gatex[items[itemToCheck].gatenumber][1], gatey[items[itemToCheck].gatenumber][1], gatez[items[itemToCheck].gatenumber][1] );
			break;
		case 52:	// end gate
			sprintf( message, "End gate going to X %i Y %i Z %i", gatex[items[itemToCheck].gatenumber][0], gatey[items[itemToCheck].gatenumber][0], gatez[items[itemToCheck].gatenumber][0] );
			break;
		case 60:	// object teleporter
			sprintf( message, "A teleporter going to X %i Y %i Z %i", items[itemToCheck].morex, items[itemToCheck].morey, items[itemToCheck].morez );
			break;
		case 61:	// item spawner
			sprintf( message, "Item spawner:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", items[itemToCheck].morex, items[itemToCheck].morez, items[itemToCheck].morey  );
			break;
		case 62:	// monster/npc spanwer
			sprintf( message, "Monster/NPC spawner:Amount: %i:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", items[itemToCheck].amount, items[itemToCheck].morex, items[itemToCheck].morez, items[itemToCheck].morey  );
			break;
		case 63:	// spawn container
			strcpy( message, "Item Spawn container:" );
			if( items[itemToCheck].moreb1 > 0 )
				strcat( message, "Magically trapped:" );
			sprintf( message, "%sRespawn max time: %i:Respawn min time: %i", message, items[itemToCheck].morez, items[itemToCheck].morey );
			break;
		case 64:	// locked spawn container
			strcpy( message, "Locked item spawn container:" );
			if( items[itemToCheck].moreb1 > 0 )
				strcat( message, "Magically trapped:" );
			sprintf( message, "%sRespawn max time: %i:Respawn min time: %i", message, items[itemToCheck].morez, items[itemToCheck].morey );
			break;
		case 65:	// unlockable item spawner container
			strcpy( message, "Unlockable item spawner container" );
			break;
		case 69:	// area spawner
			sprintf( message, "Area spawner:X +/- value: %i:Y +/- value: %i:Amount: %i:NpcNum: %i:Respawn max time: %i:Respawn min time: %i", items[itemToCheck].more3, items[itemToCheck].more4, items[itemToCheck].amount, items[itemToCheck].morex, items[itemToCheck].morez, items[itemToCheck].morey );
			break;
		case 80:	// single use advancement gate
			sprintf( message, "Single use advancement gate: advance.scp entry %i", items[itemToCheck].morex );
			break;
		case 81:	// multi-use advancement gate
			sprintf( message, "Multi use advancement gate: advance.scp entry %i", items[itemToCheck].morex );
			break;
		case 82:	// monster gate
			sprintf( message, "Monster gate: monster number %i", items[itemToCheck].morex );
			break;
		case 83:	// race gates
			sprintf( message, "Race Gate:Turns into %s:", Races->getName( items[itemToCheck].morex ) );
			if( items[itemToCheck].morey == 1 )
				strcat( message, "Constantly reuseable:" );
			else
				strcat( message, "One time use only:" );
			if( Races->isPlayerRace( items[itemToCheck].morex ) )
				strcat( message, "Player Race:" );
			else
				strcat( message, "NPC Race only:" );
			break;
		case 85:	// damage object
			sprintf( message, "Damage object:Minimum damage %i:Maximum Damage %i", items[itemToCheck].morex + items[itemToCheck].morey, items[itemToCheck].morex + items[itemToCheck].morez );
			break;
		case 86:	// sound object
			sprintf( message, "Sound object:Percent chance of sound being played: %i:Sound effect to play: %i %i", items[itemToCheck].morez, items[itemToCheck].morex, items[itemToCheck].morey );
			break;
		case 87:	// trash container
			strcpy( message, "A trash container" );
			break;
		case 88:	// sound object
			sprintf( message, "Sound object that plays whenever someone is near:Soundeffect: %i\nRadius: %i\nProbability: %i", items[itemToCheck].morex, items[itemToCheck].morey, items[itemToCheck].morez );
			break;
		case 100:	// not documented
			strcpy( message, "Looks like hide/unhide object:More detail to come later\n" );
			break;
		case 101:	// morph object morex = npc# in npc.scp
			sprintf( message, "Morph object:Morphs char into body %i %i", items[itemToCheck].morex>>8, items[itemToCheck].morex%256 );
			break;
		case 102:	// unmorph
			strcpy( message, "Unmorph object, unmorphs back to body before using it as type 101, switches to 101 again" );
			break;
		case 103:	// army enlistment object
			sprintf( message, "Army enlistment object: Army #%i", items[itemToCheck].morex );
			break;
		case 104:	// teleport object (use triggers now)
			sprintf( message, "Teleport object: X %i Y %i Z %i", items[itemToCheck].morex, items[itemToCheck].morey, items[itemToCheck].morez );
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
			sprintf( message, "A fireworks wand with %i charges left on it", items[itemToCheck].morex );
			break;
		case 185:	// smoking type
			sprintf( message, "Smoking object: Duration %i secs", items[itemToCheck].morex );
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
		}

		sysmessage( s, message );

	}
	else
	{
		sysmessage( s, "Invalid item selected!" );
		return;
	}
}

void cTargets::CommandLevelTarget( UOXSOCKET s ) 
{
	if( acctno[s] != 0 && chars[currchar[s]].commandLevel < 5 )
	{
		sysmessage( s, "Only account 0 or command level 5 can do that" );
		return;
	}
	int i;
	i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		chars[i].commandLevel = addx[s];
	}
}

void cTargets::IncZTarget( UOXSOCKET s )
{
	int i;
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		items[i].z += addx[s];
		RefreshItem( i );
	}
	i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		chars[i].z += addx[s];
		teleport( i );
	}
	
}
