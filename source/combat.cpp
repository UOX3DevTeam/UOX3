#include "uox3.h"
#include <algorithm>
#include "cGuild.h"
#include "combat.h"
#include "townregion.h"
#include "cRaces.h"
#include "skills.h"
#include "cMagic.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "magic.h"
#include "targeting.h"
#include "Dictionary.h"
#include "movement.h"
#include "cScript.h"
#include "trigger.h"

#undef DBGFILE
#define DBGFILE "combat.cpp"

namespace UOX
{

CHandleCombat *Combat = NULL;

#define SWINGAT (UI32)1.75 * 1000

const UI08 BODYPERCENT = 0;
const UI08 ARMSPERCENT = 1;
const UI08 HEADPERCENT = 2;
const UI08 LEGSPERCENT = 3;
const UI08 NECKPERCENT = 4;
const UI08 OTHERPERCENT = 5;
const UI08 TOTALTARGETSPOTS = 6;

const UI08 LOCPERCENTAGES[TOTALTARGETSPOTS] = { 44, 14, 14, 14, 7, 7 };

bool CHandleCombat::StartAttack( CChar *cAttack, CChar *cTarget )
{
	if( !ValidateObject( cAttack ) || !ValidateObject( cTarget ) || cAttack == cTarget )
		return false;
	if( cAttack->IsDead() || cTarget->IsDead() )
		return false;
	if( cAttack->GetTarg() == cTarget )
		return false;
	if( !objInRange( cAttack, cTarget, DIST_NEXTTILE ) && !LineOfSight( NULL, cAttack, cTarget->GetX(), cTarget->GetY(), cTarget->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
		return false;

	bool returningAttack = false;

	cAttack->SetTarg( cTarget );
	cAttack->SetAttacker( cTarget );
	cAttack->SetAttackFirst( ( cTarget->GetTarg() != cAttack ) );
	if( !cTarget->IsInvulnerable() && (!ValidateObject( cTarget->GetTarg() ) || !objInRange( cTarget, cTarget->GetTarg(), DIST_INRANGE )) )	// Only invuln don't fight back
	{
		cTarget->SetTarg( cAttack );
		cTarget->SetAttacker( cAttack );
		cTarget->SetAttackFirst( false );
		returningAttack = true;
	}

	if( cAttack->GetHidden() && !cAttack->IsPermHidden() )
		cAttack->ExposeToView();

	if( !cAttack->IsNpc() )
		cAttack->BreakConcentration( calcSocketObjFromChar( cAttack ) );
	else
	{
		Effects->playMonsterSound( cAttack, cAttack->GetID(), SND_STARTATTACK );

		// if the source is an npc, make sure they're in war mode and reset their movement time
		if( !cAttack->IsAtWar() ) 
			cAttack->ToggleCombat();
		cAttack->SetTimer( tNPC_MOVETIME, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->NPCSpeed() )) );
	}

	// Only unhide the defender, if they're going to return the attack (otherwise they're doing nothing!)
	if( returningAttack )
	{
		if( cTarget->GetHidden() && !cTarget->IsPermHidden() )
			cTarget->ExposeToView();

		if( !cTarget->IsNpc() )
		cTarget->BreakConcentration( calcSocketObjFromChar( cTarget ) );

		// if the target is an npc, and not a guard, make sure they're in war mode and update their movement time
		// ONLY IF THEY'VE CHANGED ATTACKER
		if( cTarget->IsNpc() && cTarget->GetNPCAiType() != aiGUARD )
		{
			if( !cTarget->IsAtWar() )
				cTarget->ToggleCombat();
			cTarget->SetTimer( tNPC_MOVETIME, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->NPCSpeed() )) );
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	PlayerAttack( cSocket *s )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle player attacking (Double-clicking whilst in war mode)
//o---------------------------------------------------------------------------o
void CHandleCombat::PlayerAttack( cSocket *s )
{
	if( s == NULL )
		return;

	CChar *ourChar = s->CurrcharObj();
	if( !ValidateObject( ourChar ) )
		return;

	SERIAL serial = s->GetDWord( 1 );
	if( serial == INVALIDSERIAL ) 
	{
		ourChar->SetTarg( NULL );
		return;
	}
	CChar *i = calcCharObjFromSer( serial );
	if( !ValidateObject( i ) )
	{
		ourChar->SetTarg( NULL );
		return;
	}
	if( ourChar->IsDead() )
	{
		if( i->IsNpc() )
		{
			switch( i->GetNPCAiType() )
			{
				case aiHEALER_G:	// Good Healer
					if( !ourChar->IsCriminal() && !ourChar->IsMurderer() )
					{ // Character is innocent
						if( objInRange( i, ourChar, DIST_NEARBY ) )
						{	//let's resurrect him!
							Effects->PlayCharacterAnimation( i, 0x10 );
							Targ->NpcResurrectTarget( ourChar );
							Effects->PlayStaticAnimation( ourChar, 0x376A, 0x09, 0x06 );
							i->talkAll( ( 316 + RandomNum( 0, 4 ) ), false );
						} 
						else
							i->talkAll( 321, true );
					} 
					else // Character is criminal or murderer
						i->talkAll( 322, true );
					break;
				case aiHEALER_E: // Evil Healer
					if( ourChar->IsMurderer() )
					{
						if( objInRange( i, ourChar, DIST_NEARBY ) )	// let's resurrect him
						{
							Effects->PlayCharacterAnimation( i, 0x10 );
							Targ->NpcResurrectTarget( ourChar );
							Effects->PlayStaticAnimation( ourChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
							i->talkAll( ( 323 + RandomNum( 0, 4 ) ), false );
						} 
						else
							i->talkAll( 328, true );
					} 
					else
						i->talkAll( 329, true );
					break;
				default:
					s->sysmessage( 330 );
					break;
			}
			return;
		} //if isNpc
		else
		{
			if( cwmWorldState->ServerData()->PlayerPersecutionStatus() )
			{
				ourChar->SetTarg( i );
				Skills->Persecute( s );
				return;
			} 
			else
			{
				s->sysmessage( 330 );
				return;
			}
		}//if isNpc
	}//if isDead
	else if( ourChar->GetTarg() != i )
	{
		ourChar->SetTarg( i );
		if( ourChar->GetHidden() && !ourChar->IsPermHidden() )
			ourChar->ExposeToView();
		if( i->IsDead() || i->GetHP() <= 0 )
		{
			s->sysmessage( 331 );
			return;
		}
		if( i->GetNPCAiType() == aiPLAYERVENDOR ) // PlayerVendors
		{
			s->sysmessage( 332, i->GetName().c_str() );
			return;
		}
		if( i->IsInvulnerable() )
		{
			s->sysmessage( 333 );
			return;
		}
		
		//flag them FIRST so that anything attacking them as a result of this is not flagged.
		if( WillResultInCriminal( ourChar, i ) ) //REPSYS
		{
			bool regionGuarded = ( i->GetRegion()->IsGuarded() );
			if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded && i->IsNpc() && i->GetNPCAiType() != aiGUARD && i->isHuman() )
				i->talkAll( 335, true );
			criminal( ourChar );
		}

		if( i->IsGuarded() )
			petGuardAttack( ourChar, i, i );

		ourChar->emoteAll( 334, true, ourChar->GetName().c_str(), i->GetName().c_str() );	// Attacker emotes "You see attacker attacking target" to all nearby

		if( !i->IsNpc() )
		{
			cSocket *iSock = calcSocketObjFromChar( i );
			if( iSock != NULL )
				i->emote( iSock, 1281, true, ourChar->GetName().c_str() ); // "Attacker is attacking you!" sent to target socket only
		}

		// keep the target highlighted
		CPAttackOK toSend = (*i);
		s->Send( &toSend );
		
		if( i->GetNPCAiType() != aiGUARD && !ValidateObject( i->GetTarg() ) )
		{
			i->SetAttacker( ourChar );
			i->SetAttackFirst( false );
		}
		ourChar->SetAttackFirst( true );
		ourChar->SetAttacker( i );
		AttackTarget( i, ourChar );
	}
}

void CHandleCombat::AttackTarget( CChar *cAttack, CChar *cTarget )
{
	if( !StartAttack( cAttack, cTarget ) )
		return;

	// If the target is an innocent, not a racial or guild ally/enemy, then flag them as criminal
	// and, of course, call the guards ;>
	if( WillResultInCriminal( cAttack, cTarget ) )
	{
		bool regionGuarded = ( cTarget->GetRegion()->IsGuarded() );
		if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded )
		{
			if( cTarget->IsNpc() && cTarget->GetNPCAiType() != aiGUARD && cTarget->isHuman() )
				cTarget->talkAll( 1282, true );
			criminal( cAttack );
#ifdef DEBUG
			Console.Print( "DEBUG: [AttackTarget] %s is being set to criminal", cAttack->GetName() );
#endif
		}
	}
	if( cAttack->DidAttackFirst() )
	{
		cAttack->emoteAll( 334, true, cAttack->GetName().c_str(), cTarget->GetName().c_str() );  // NPC should emote "Source is attacking Target" to all nearby - Zane
		if( !cTarget->IsNpc() )
		{
			cSocket *iSock = calcSocketObjFromChar( cTarget );
			if( iSock != NULL )
				cTarget->emote( iSock, 1281, true, cAttack->GetName().c_str() );	// Target should get an emote only to his socket "Target is attacking you!" - Zane
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * getWeapon( CChar *i )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find what weapon (if any) character is holding
//o---------------------------------------------------------------------------o
CItem * CHandleCombat::getWeapon( CChar *i )
{
	if( !ValidateObject( i ) )
		return NULL;

	CItem *j = i->GetItemAtLayer( IL_RIGHTHAND );
	if( ValidateObject( j ) )
	{
		if( j->GetType() == IT_SPELLBOOK )	// spell books aren't weapons
			return NULL;
		return j;
	}
	j = i->GetItemAtLayer( IL_LEFTHAND );
	if( ValidateObject( j ) && !j->IsShieldType() )
		return j;
	return NULL;
}

//o---------------------------------------------------------------------------o
//|   Function    :  CItem * getShield( CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check players hands for a shield
//o---------------------------------------------------------------------------o
CItem * CHandleCombat::getShield( CChar *i )
{
	if( ValidateObject( i ) )
	{
		CItem *shield = i->GetItemAtLayer( IL_LEFTHAND );
		if( ValidateObject( shield ) && shield->IsShieldType() )
			return shield;
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 getWeaponType( CItem *i )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find what weapon type a character is holding (based on its ID)
//o---------------------------------------------------------------------------o
UI08 CHandleCombat::getWeaponType( CItem *i )
{
	if( !ValidateObject( i ) )
		return WRESTLING;

	switch( i->GetID() )
	{
		// Slashing Swords
		case 0x0EC4: //skinning knife
		case 0x0EC5: //skinning knife
		case 0x0F60: //longsword
		case 0x0F61: //longsword
		case 0x13B7: //long sword
		case 0x13B8: //long sword
		case 0x1440: //cutlass
		case 0x1441: //cutlass
		case 0x26BB: //bone harvester - AoS
		case 0x26C5: //bone harvester - AoS
		case 0x2573: //magic sword - LBR
		case 0x2574: //magic sword - LBR
		case 0x2575: //magic sword - LBR
		case 0x2576: //magic sword - LBR
		case 0x257E: //short sword - LBR
			return DEF_SWORDS;
		// Default Swords
		case 0x13F6: //butcher knife
		case 0x13F7: //butcher knife
		case 0x13FE: //katana
		case 0x13FF: //katana
		case 0x13B5: //scimitar
		case 0x13B6: //scimitar
		case 0x13B9: //viking sword
		case 0x13BA: //viking sword
		case 0x255E: //ratman sword - LBR
		case 0x2560: //skeleton scimitar - LBR
			return SLASH_SWORDS;
		// One-Handed Lg. Swords
		case 0x0F5E: //broadsword
		case 0x0F5F: //broadsword
		case 0x257D: //longsword - LBR
		case 0x26CE: //paladin sword - AoS
		case 0x26CF: //paladin sword - AoS
		case 0x2554: //daemon sword - LBR
			return ONEHND_LG_SWORDS;
		// Two-Handed Lg. Swords
		case 0x143E: //halberd
		case 0x143F: //halberd
		case 0x26BD: //bladed staff - AoS
		case 0x26C7: //bladed staff - AoS
		case 0x26C1: //crescent blade - AoS
		case 0x26CB: //crescent blade - AoS
		case 0x2578: //no-dachi - LBR
			return TWOHND_LG_SWORDS;
		// Bardiche
		case 0x0F4D: //bardiche
		case 0x0F4E: //bardiche
		case 0x26BA: //scythe - AoS
		case 0x26C4: //scythe - AoS
		case 0x255B: //ophidian bardiche - LBR
		case 0x2577: //naginata - LBR
			return BARDICHE;
		// One-Handed Axes
		case 0x0EC2: //cleaver
		case 0x0EC3: //cleaver
		case 0x0E85: //pickaxe
		case 0x0E86: //pickaxe
		case 0x2567: //orc lord battleaxe - LBR
		case 0x2579: //pick - LBR
			return ONEHND_AXES;
		// Two-Handed Axes
		case 0x13FA: //large battle axe
		case 0x13FB: //large battle axe
		case 0x1442: //two handed axe
		case 0x1443: //two handed axe
		case 0x0F43: //hatchet
		case 0x0F44: //hatchet
		case 0x0F45: //executioner's axe
		case 0x0F46: //executioner's axe
		case 0x0F47: //battle axe
		case 0x0F48: //battle axe
		case 0x0F49: //axe
		case 0x0F4A: //axe
		case 0x0F4B: //double axe
		case 0x0F4C: //double axe
		case 0x255F: //skeleton axe - LBR
		case 0x255D: //ratman axe - LBR
		case 0x2564: //troll axe - LBR
		case 0x2570: //hatchet - LBR
			return TWOHND_AXES;
		// Default Maces
		case 0x13E3: //smith's hammer
		case 0x13E4: //smith's hammer
		case 0x13B3: //club
		case 0x13B4: //club
		case 0x0FB4: //sledge hammer
		case 0x0FB5: //sledge hammer
		case 0x0F5C: //mace
		case 0x0F5D: //mace
		case 0x1406: //war mace
		case 0x1407: //war mace
		case 0x143C: //hammer pick
		case 0x143D: //hammer pick
		case 0x13AF: //war axe
		case 0x13B0: //war axe
		case 0x143A: //maul
		case 0x143B: //maul
		case 0x26BC: //scepter - AoS
		case 0x26C6: //scepter - AoS
		case 0x2557: //lizardman's mace - LBR
		case 0x255C: //orc club - LBR
		case 0x256F: //smyth's hammer - LBR
		case 0x257F: //war mace - LBR
			return DEF_MACES;
		// Large Maces
		case 0x13F4: //crook
		case 0x13F5: //crook
		case 0x13F8: //gnarled staff
		case 0x13F9: //gnarled staff
		case 0x0DF0: //black staff
		case 0x0DF1: //black staff
		case 0x0E81: //shepherd's crook
		case 0x0E82: //shepherd's crook
		case 0x0E89: //quarter staff
		case 0x0E8A: //quarter staff
		case 0x1438: //war hammer
		case 0x1439: //war hammer
		case 0x2555: //ettin hammer - LBR
		case 0x2556: //liche's staff - LBR
		case 0x2559: //ogre's club - LBR
		case 0x255A: //ophidian staff - LBR
		case 0x2561: //terathan staff - LBR
		case 0x2565: //troll maul - LBR
		case 0x2566: //frost troll club - LBR
		case 0x2568: //orc mage staff - LBR
		case 0x2569: //bone mage staff - LBR
		case 0x256B: //magic staff - LBR
		case 0x256C: //magic staff - LBR
		case 0x256D: //magic staff - LBR
		case 0x256E: //magic staff - LBR
			return LG_MACES;
		// Bows
		case 0x13B1: //bow
		case 0x13B2: //bow
		case 0x26C2: //composite bow - AoS
		case 0x26CC: //composite bow - AoS
		case 0x2571: //horseman's bow - LBR
			return BOWS;
		// Crossbows
		case 0x0F4F: //crossbow
		case 0x0F50: //crossbow
		case 0x13FC: //heavy crossbow
		case 0x13FD: //heavy crossbow
		case 0x26C3: //repeating crossbow - AoS
		case 0x26CD: //repeating crossbow - AoS
			return XBOWS;
		// Normal Fencing Weapons
		case 0x0F51: //dagger
		case 0x0F52: //dagger
		case 0x1400: //kryss
		case 0x1401: //kryss
		case 0x1402: //short spear
		case 0x1403: //short spear
		case 0x1404: //war fork
		case 0x1405: //war fork
		case 0x257C: //sword(rapier) - LBR
			return DEF_FENCING;
		// Stabbing Fencing Weapons
		case 0x0E87: //pitchfork
		case 0x0E88: //pitchfork
		case 0x0F62: //spear
		case 0x0F63: //spear
		case 0x26BE: //pike - AoS
		case 0x26C8: //pike - AoS
		case 0x26BF: //double bladed staff - AoS
		case 0x26C9: //double bladed staff - AoS
		case 0x26C0: //lance - AoS
		case 0x26CA: //lance - AoS
		case 0x2558: //Lizardman's spear - LBR
		case 0x2562: //terathan spear - LBR
		case 0x2572: //horseman's javelin - LBR
		case 0x257A: //spear - LBR
		case 0x257B: //spear - LBR
			return TWOHND_FENCING;
		default: // Wrestling
			return WRESTLING;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 getBowType( CItem *bItem )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find if character is holding a Bow or XBow
//o---------------------------------------------------------------------------o
UI08 CHandleCombat::getBowType( CItem *bItem )
{
	if( !ValidateObject( bItem ) )
		return 0;

	switch( getWeaponType( bItem ) )
	{
		case BOWS:
			return BOWS;
		case XBOWS:
			return XBOWS;
		default:
			return 0;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI08 getCombatSkill( CItem *wItem )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Gets your combat skill based on the weapon in your hand (if any)
//o---------------------------------------------------------------------------o
UI08 CHandleCombat::getCombatSkill( CItem *wItem )
{
	if( !ValidateObject( wItem ) )
		return WRESTLING;

	switch( getWeaponType( wItem ) )
	{
		case DEF_SWORDS:
		case SLASH_SWORDS:
		case ONEHND_LG_SWORDS:
		case TWOHND_LG_SWORDS:
		case ONEHND_AXES:
		case TWOHND_AXES:
		case BARDICHE:
			return SWORDSMANSHIP;
		case DEF_MACES:
		case LG_MACES:
			return MACEFIGHTING;
		case DEF_FENCING:
		case TWOHND_FENCING:
			return FENCING;
		case BOWS:
		case XBOWS:
			return ARCHERY;
		case WRESTLING:
		default:
			return WRESTLING;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI16 calcAtt( CChar *p )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate total attack power and do damage to the weapon
//o---------------------------------------------------------------------------o
SI16 CHandleCombat::calcAtt( CChar *p, bool doDamage )
{
	if( !ValidateObject( p ) )
		return 0;

	SI16 getDamage = 0;

	CItem *weapon = getWeapon( p );
	if( ValidateObject( weapon ) )
	{
		if( weapon->GetLoDamage() > 0 && weapon->GetHiDamage() > 0 )
		{
			if( weapon->GetLoDamage() >= weapon->GetHiDamage() ) 
				getDamage += weapon->GetLoDamage();
			else
				getDamage += RandomNum( weapon->GetLoDamage(), weapon->GetHiDamage() );

			if( doDamage && !p->IsNpc() && !RandomNum( 0, 5 ) )
			{
				weapon->IncHP( -1 );
				if( weapon->GetHP() <= 0 )
				{
					cSocket *mSock = calcSocketObjFromChar( p );
					if( mSock != NULL )
					{
						std::string name;
						getTileName( weapon, name );
						mSock->sysmessage( 311, name.c_str() );
					}
					weapon->Delete();
				}
			}
		}
	}
	else if( p->IsNpc() ) 
	{
		if( p->GetLoDamage() >= p->GetHiDamage() )
			getDamage = p->GetLoDamage();
		else if( p->GetHiDamage() > 2 )
			getDamage = RandomNum( p->GetLoDamage(), p->GetHiDamage() );
	}
	else
	{
		UI16 getWrestSkill = ( p->GetSkill( WRESTLING ) / 65 );
		if( getWrestSkill > 0 )
			getDamage = HalfRandomNum( getWrestSkill );
		else
			getDamage = RandomNum( 1, 2 );
	}
	if( getDamage < 1 )
		getDamage = 1;
	return getDamage;
}

/*
	New Defense Calculations
	Programmer: Zane (giwo)
	Date: 3/03/2003

	Purpose:
		To emulate OSI's armor calculation methods for specific hit locations and total AR level

	How we do this:
		We need to first determine what we are calculating, total AR (hitLoc 0) or the armor level of a specific locations (hitLoc 1-6).
		Next we need to loop through the items a player has equipped searching for items that match the following criteria:
			Defence > 0
			On a specific layer (depending on the hit location)
			The item with the most AR covering that location
		Lastly we have to return this information. If we are returning the AR of a specific hit location, then we return the actual Def value of the item
			if we are returning the total AR of a character, we need to find the AR / the divisor used for that specific location.

		Divisors based upon location / Armor (as best I can match it to OSI)
			Torso (hitLoc 1):	AR / 2.8
			Arms (hitLoc 2):	AR / 6.8
			Head (hitLoc 3):	AR / 7.3
			Legs (hitLoc 4):	AR / 4.5
			Neck (hitLoc 5):	AR / 14.5
			Hands (hitLoc 6):	AR / 14.5

		Functions:
			checkDef( CItem *checkItem, CItem& currItem, SI32 &currDef )
			getArmorDef( CChar *mChar, CItem& defItem, UI08 bodyLoc, bool findTotal )
			calcDef( CChar *mChar, UI08 hitLoc, bool doDamage )

	All information on armor defence values, divisors, and combat calculations gleaned from uo.stratics.com
	http://uo.stratics.com/content/arms-armor/armor.shtml
	http://uo.stratics.com/content/arms-armor/combat.shtml
	http://uo.stratics.com/content/arms-armor/clothes.shtml
	http://uo.stratics.com/content/aos/combatchanges.shtml
*/

//o--------------------------------------------------------------------------o
//|	Function		-	CItem *checkDef( CItem *checkItem, CItem& currItem, SI32 &currDef )
//|	Date			-	3/03/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Checks the defense of checkItem vs the defense of currItem and returns
//|							the item with the greater Def and its def value
//o--------------------------------------------------------------------------o
CItem *CHandleCombat::checkDef( CItem *checkItem, CItem *currItem, SI32 &currDef )
{
	if( ValidateObject( checkItem ) && checkItem->GetDef() > currDef )
	{
		currDef = checkItem->GetDef();
		return checkItem;
	}
	return currItem;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CItem * getArmorDef( CChar *mChar, SI32 &totalDef, UI08 bodyLoc, bool findTotal )
//|	Date			-	3/03/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Finds the item covering the location bodyLoc with the greatest AR and
//|							returns it along with its def value
//o--------------------------------------------------------------------------o
CItem * CHandleCombat::getArmorDef( CChar *mChar, SI32 &totalDef, UI08 bodyLoc, bool findTotal )
{
	SI32 armorDef = 0;
	CItem *currItem = NULL;
	switch( bodyLoc )
	{
		case 1:		// Torso
			currItem = checkDef( mChar->GetItemAtLayer( IL_INNERSHIRT ), currItem, armorDef );	// Shirt
			currItem = checkDef( mChar->GetItemAtLayer( IL_TUNIC ), currItem, armorDef );	// Torso (Inner - Chest Armor)
			currItem = checkDef( mChar->GetItemAtLayer( IL_OUTERSHIRT ), currItem, armorDef );	// Torso (Middle - Tunic, etc)
			currItem = checkDef( mChar->GetItemAtLayer( IL_CLOAK ), currItem, armorDef );	// Back (Cloak)
			currItem = checkDef( mChar->GetItemAtLayer( IL_ROBE ), currItem, armorDef );	// Torso (Outer - Robe)
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 2.8 ));
			break;
		case 2:		// Arms
			currItem = checkDef( mChar->GetItemAtLayer( IL_ARMS ), currItem, armorDef );	// Arms
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 6.8 ));
			break;
		case 3:		// Head
			currItem = checkDef( mChar->GetItemAtLayer( IL_HELM ), currItem, armorDef );	// Head
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 7.3 ));
			break;
		case 4:		// Legs
			currItem = checkDef( mChar->GetItemAtLayer( IL_FOOTWEAR ), currItem, armorDef );	// Shoes
			currItem = checkDef( mChar->GetItemAtLayer( IL_PANTS ), currItem, armorDef );	// Pants
			currItem = checkDef( mChar->GetItemAtLayer( IL_WAIST ), currItem, armorDef );	// Waist (Half Apron)
			currItem = checkDef( mChar->GetItemAtLayer( IL_OUTERLEGGINGS ), currItem, armorDef );	// Legs (Outer - Skirt, Kilt)
			currItem = checkDef( mChar->GetItemAtLayer( IL_INNERLEGGINGS ), currItem, armorDef );	// Legs (Inner - Leg Armor)
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 4.5 ));
			break;
		case 5:		// Neck
			currItem = checkDef( mChar->GetItemAtLayer( IL_NECK ), currItem, armorDef );	// Neck
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 14.5 ));
			break;
		case 6:		// Hands
			currItem = checkDef( mChar->GetItemAtLayer( IL_GLOVES ), currItem, armorDef );	// Gloves
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 14.5 ));
			break;
		default:
			break;
	}
	if( findTotal )
		totalDef += armorDef;
	else
		totalDef = armorDef;

	return currItem;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 calcDef( CChar *mChar, UI08 hitLoc, bool doDamage )
//|	Date			-	3/03/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Finds the defense value of a specific location or the entire character based on hitLoc
//o--------------------------------------------------------------------------o
UI16 CHandleCombat::calcDef( CChar *mChar, UI08 hitLoc, bool doDamage )
{
	if( !ValidateObject( mChar ) )
		return 0;

	SI32 total = mChar->GetDef();
	if( !mChar->IsNpc() || mChar->isHuman() )	// Polymorphed Characters and GM's can still wear armor
	{
		CItem *defendItem = NULL;
		if( hitLoc == 0 )
		{
			for( UI08 getLoc = 1; getLoc < 7; ++getLoc )
				getArmorDef( mChar, total, getLoc, true );
			total = (total / 100);
		}
		else
			defendItem = getArmorDef( mChar, total, hitLoc );

		if( total > 0 && doDamage && ValidateObject( defendItem ) && !mChar->IsNpc() )
		{
			SI08 armorDamage = 0;	// Based on OSI standards, each successful hit does 0 to 2 damage to armor hit
			armorDamage -= RandomNum( 0, 2 );
			defendItem->IncHP( armorDamage );

			if( defendItem->GetHP() <= 0 )
			{
				cSocket *mSock = calcSocketObjFromChar( mChar );
				if( mSock != NULL )
				{
					std::string name;
					getTileName( defendItem, name );
					mSock->sysmessage( 311, name.c_str() );
					mSock->statwindow( mChar );
				}
				defendItem->Delete();
			}
		}
	}

	if( total < 2 )
		total = 2;
	return (UI16)total;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CombatOnHorse( CChar *i )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play animations for combat while mounted
//o---------------------------------------------------------------------------o
void CHandleCombat::CombatOnHorse( CChar *i )
{
	if( !ValidateObject( i ) )
		return;

	UI16 animToPlay = 0x00;
	CItem *j		= getWeapon( i );
	switch( getWeaponType( j ) )
	{
		case BOWS:				animToPlay = 0x1B;	break;
		case XBOWS:				animToPlay = 0x1C;	break;
		case DEF_SWORDS:
		case SLASH_SWORDS:
		case ONEHND_LG_SWORDS:
		case ONEHND_AXES:
		case DEF_MACES:
		case DEF_FENCING:		animToPlay = 0x1A;	break;
		case TWOHND_FENCING:
		case TWOHND_LG_SWORDS:
		case BARDICHE:
		case TWOHND_AXES:
		case LG_MACES:			animToPlay = 0x1D;	break;
		case WRESTLING:
		default:				animToPlay = 0x1A;	break;
	}
	Effects->PlayCharacterAnimation( i, animToPlay );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CombatOnFoot( CChar *i )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play animations for combat on foot
//o---------------------------------------------------------------------------o
void CHandleCombat::CombatOnFoot( CChar *i )
{
	if( !ValidateObject( i ) )
		return;

	UI16 animToPlay	= 0x00;
	CItem *j		= getWeapon( i );
	switch( getWeaponType( j ) )
	{
		case BOWS:				animToPlay = 0x12;									break;
		case XBOWS:				animToPlay = 0x13;									break;
		case DEF_SWORDS:
		case SLASH_SWORDS:
		case ONEHND_LG_SWORDS:
		case ONEHND_AXES:		animToPlay = (RandomNum( 0, 1 ) ? 0x09 : 0x0A );	break;
		case DEF_MACES:			animToPlay = (RandomNum( 0, 1 ) ? 0x09 : 0x0B );	break;
		case LG_MACES:
		case TWOHND_LG_SWORDS:
		case BARDICHE:
		case TWOHND_AXES:		animToPlay = (RandomNum( 0, 1 ) ? 0x0C : 0x0D );	break;
		case DEF_FENCING:		animToPlay = 0x0A;									break;
		case TWOHND_FENCING:	animToPlay = 0x0E;									break;
		case WRESTLING:
		default:
			switch( RandomNum( 0, 2 ) )
			{
				case 0:		animToPlay = 0x0A;										break;  //fist straight-punch
				case 1:		animToPlay = 0x09;										break;  //fist top-down
				default:	animToPlay = 0x1F;										break;  //fist over-head
			}
			break;
	}
	Effects->PlayCharacterAnimation( i, animToPlay );
}

void CHandleCombat::PlaySwingAnimations( CChar *mChar )
{
	UI16 charID = mChar->GetID();
	if( charID < 0x0190 )
	{
		UI08 aa = RandomNum( 4, 6 ); // some creatures dont have animation #4
		if( cwmWorldState->creatures[charID].AntiBlink() )
		{
			++aa;
			if( charID == 5 ) // eagles need special treatment
			{
				switch( RandomNum( 0, 2 ) )
				{
					case 0: aa = 0x1;  break;
					case 1: aa = 0x14; break;
					case 2: aa = 0x4;  break;
				}
			}
		}
		Effects->PlayCharacterAnimation( mChar, aa ); 
		if( RandomNum( 0, 4 ) ) 
			Effects->playMonsterSound( mChar, charID, SND_ATTACK );
	}
	else if( mChar->IsOnHorse() )
		CombatOnHorse( mChar );
	else
		CombatOnFoot( mChar );
}

//o---------------------------------------------------------------------------o
//|	Function	-	PlayMissedSoundEffect( CChar *p )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do the "Missed" Sound Effect
//o---------------------------------------------------------------------------o
void CHandleCombat::PlayMissedSoundEffect( CChar *p )
{
	if( !ValidateObject( p ) )
		return;

	switch( RandomNum( 0, 2 ) )
	{
		case 0:	Effects->PlaySound( p, 0x0238 );	break;
		case 1:	Effects->PlaySound( p, 0x0239 );	break;
		default: Effects->PlaySound( p, 0x023A );	break;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	doSoundEffect( CChar *p, CItem *weapon )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do the "Hit" Sound Effect
//o---------------------------------------------------------------------------o
void CHandleCombat::PlayHitSoundEffect( CChar *p, CItem *weapon )
{
	if( !ValidateObject( p ) || !ValidateObject( weapon ) )
		return;

	switch( getWeaponType( weapon ) )
	{
		case ONEHND_AXES:
		case TWOHND_AXES:
		case DEF_MACES:
		case LG_MACES:
			Effects->PlaySound( p, 0x0232 ); // Whoosh Weapons
			break;
		case DEF_SWORDS:
		case DEF_FENCING:
		case TWOHND_FENCING:
			Effects->PlaySound( p, 0x023C ); // Stabbing Weapons
			break;
		case BARDICHE:
			Effects->PlaySound( p, 0x0236 ); // Bardiche
			break;
		case SLASH_SWORDS:
			Effects->PlaySound( p, 0x023B ); // Slashing Weapons
			break;
		case ONEHND_LG_SWORDS:
		case TWOHND_LG_SWORDS:
			Effects->PlaySound( p, 0x0237 ); // Large Swords
			break;
		case BOWS:
		case XBOWS:
			Effects->PlaySound( p, 0x0223 ); // Bows
			break;
		case WRESTLING:
		default:
			switch( RandomNum( 0, 3 ) ) // Wrestling
			{
				case 0:		Effects->PlaySound( p, 0x0135 );	break;
				case 1:		Effects->PlaySound( p, 0x0137 );	break;
				case 2:		Effects->PlaySound( p, 0x013D );	break;
				default:	Effects->PlaySound( p, 0x013B );	break;
			}
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	void AdjustRaceDamage( CChar *defend, CItem *weapon, SI16 &bDamage )
//|	Date			-	3rd July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adjusts the damage dealt to defend by weapon based on
//|						race and weather weaknesses
//o--------------------------------------------------------------------------
void CHandleCombat::AdjustRaceDamage( CChar *defend, CItem *weapon, SI16 &bDamage )
{
	if( !ValidateObject( defend ) || !ValidateObject( weapon ) )
		return;

	if( weapon->GetRace() == defend->GetRace() ) 
		bDamage *= 2;
	CRace *rPtr = Races->Race( defend->GetRace() );
	if( rPtr != NULL )
	{
		if( weapon->GetWeatherDamage( LIGHT ) && rPtr->AffectedByLight() )
			bDamage *= 2;
		if( weapon->GetWeatherDamage( RAIN ) && rPtr->AffectedByRain() )
			bDamage *= 2;
		if( weapon->GetWeatherDamage( COLD ) && rPtr->AffectedByCold() )
			bDamage *= 2;
		if( weapon->GetWeatherDamage( HEAT ) && rPtr->AffectedByHeat() )
			bDamage *= 2;
		if( weapon->GetWeatherDamage( LIGHTNING ) && rPtr->AffectedByLightning() )
			bDamage *= 2;
		if( weapon->GetWeatherDamage( SNOW ) && rPtr->AffectedBySnow() )
			bDamage *= 2;
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	SI08 DoHitMessage( CChar *defend, CChar *mChar, cSocket *mSock, SI16 damage )
//|	Date			-	3rd July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Prints out the hit message (if enabled) and calculates
//|						where on the body the person was hit and returns that
//o--------------------------------------------------------------------------
SI08 CHandleCombat::DoHitMessage( CChar *mChar, CChar *ourTarg, cSocket *targSock, SI16 damage )
{
	SI08 hitPos = RandomNum( 0, 99 ); // Determine area of Body Hit
	for( UI08 t = BODYPERCENT; t < TOTALTARGETSPOTS; ++t )
	{
		hitPos -= LOCPERCENTAGES[t];
		if( hitPos < 0 ) 
		{
			hitPos = t + 1;
			break;
		}
	}
	if( cwmWorldState->ServerData()->CombatDisplayHitMessage() )
	{
		if( !ourTarg->IsNpc() && targSock != NULL && damage > 1 )
		{
			UI08 randHit = RandomNum( 0, 2 );
			switch( hitPos )
			{
				case 1:	// Body
					switch ( randHit )
					{
						case 1:
							if( damage < 10  ) 
								targSock->sysmessage( 284, mChar->GetName().c_str() );
							else
								targSock->sysmessage( 285, mChar->GetName().c_str() );
							break;
						case 2:
							if( damage < 10 ) 
								targSock->sysmessage( 286, mChar->GetName().c_str() );
							else
								targSock->sysmessage( 287, mChar->GetName().c_str() );
							break;
						default:
							if( damage < 10 ) 
								targSock->sysmessage( 288, mChar->GetName().c_str() );
							else
								targSock->sysmessage( 289, mChar->GetName().c_str() );
							break;
						}
					break;
				case 2:	// Arms
					switch( randHit )
					{
						case 1:		targSock->sysmessage( 290, mChar->GetName().c_str() );		break;
						case 2:		targSock->sysmessage( 291, mChar->GetName().c_str() );		break;
						default:	targSock->sysmessage( 292, mChar->GetName().c_str() );		break;
					}
					break;
				case 3:	// Head
					switch( randHit )
					{
						case 1:
							if( damage <  10 ) 
								targSock->sysmessage( 293, mChar->GetName().c_str() );
							else
								targSock->sysmessage( 294, mChar->GetName().c_str() );
							break;
						case 2:
							if( damage <  10 ) 
								targSock->sysmessage( 295, mChar->GetName().c_str() );
							else 
								targSock->sysmessage( 296, mChar->GetName().c_str() );
							break;
						default:
							if( damage <  10 ) 
								targSock->sysmessage( 297, mChar->GetName().c_str() );
							else
								targSock->sysmessage( 298, mChar->GetName().c_str() );
							break;
					}
					break;
				case 4:	// Legs
					switch( randHit )
					{
						case 1:		targSock->sysmessage( 299, mChar->GetName().c_str() );		break;
						case 2:		targSock->sysmessage( 300, mChar->GetName().c_str() );		break;
						default:	targSock->sysmessage( 301, mChar->GetName().c_str() );		break;
					}
					break;
				case 5:	// Neck
					targSock->sysmessage( 302, mChar->GetName().c_str() );
					break;
				case 6:	// Hands
					switch( randHit )
					{
						case 1:		targSock->sysmessage( 303, mChar->GetName().c_str() );		break;
						case 2:		targSock->sysmessage( 304, mChar->GetName().c_str() );		break;
						default:	targSock->sysmessage( 305, mChar->GetName().c_str() );		break;
					}
					break;
			}
		}
	}
	return hitPos;
}

SI16 CHandleCombat::calcDamage( CChar *mChar, CChar *ourTarg, cSocket *targSock, CItem *mWeapon, UI08 getFightSkill )
{
	SI16 BaseDamage = calcAtt( mChar, true );
	if( BaseDamage == -1 )  // No damage if weapon breaks
		return 0;
	// Race Dmg Modification: Bonus percentage.
	const SI32 RaceDamage = Races->DamageFromSkill( getFightSkill, mChar->GetRace() );
	if( RaceDamage != 0 )
		BaseDamage += (SI16)( (R32)BaseDamage * ( (R32)RaceDamage / 1000 ) );

	// Check to see if weapons affect defender's race.
	if( getFightSkill != WRESTLING )
		AdjustRaceDamage( ourTarg, mWeapon, BaseDamage );

	R32 multiplier;
	SI16 damage = 0;

	// Strength Damage Bonus, +20% Damage
	multiplier = static_cast<R32>( ( ( UOX_MIN( mChar->GetStrength(), static_cast<SI16>(200) ) * 20 ) / 100 ) / 100 ) + 1;
	damage = (SI16)(BaseDamage * multiplier);

	// Tactics Damage Bonus, % = ( Tactics + 50 )
	multiplier = static_cast<R32>( ( mChar->GetSkill( TACTICS ) + 500 ) / 1000 );
	damage += (SI16)(BaseDamage * multiplier);

	if( ourTarg->IsNpc() ) // Anatomy PvM damage Bonus, % = ( Anat / 5 )
		multiplier = static_cast<R32>( ( mChar->GetSkill( ANATOMY ) / 5 ) / 1000 );
	else // Anatomy PvP damage Bonus, % = ( Anat / 2.5 )
		multiplier = static_cast<R32>( ( mChar->GetSkill( ANATOMY ) / 2.5 ) / 1000 );
	damage += (SI16)(BaseDamage * multiplier);

	// Lumberjacking Bonus ( 30% Damage at GM Skill )
	if( mChar->GetSkill( LUMBERJACKING ) >= 1000 )
		damage += (SI16)(BaseDamage * .3);

  	// Defender Tactics Damage Modifier, -20% Damage
	multiplier = static_cast<R32>(1.0 - ( ( ( ourTarg->GetSkill( TACTICS ) * 20 ) / 1000 ) / 100 ));
	damage = (SI16)(damage * multiplier);

	if( damage < 1 )
		return 0;

	const UI16 attSkill = mChar->GetSkill( getFightSkill );

	// Check Shield Defense
	CItem *shield = getShield( ourTarg );
	if( ValidateObject( shield ) )
	{
		Skills->CheckSkill( ourTarg, PARRYING, 0, 1000 );
		// Chance to block with Shield ( % = Skill / 2 ) 
		UI16 defendParry = ourTarg->GetSkill( PARRYING );
		if( HalfRandomNum( defendParry ) >= HalfRandomNum( attSkill ) )
		{
			damage -= HalfRandomNum( shield->GetDef() );
			if( !RandomNum( 0, 5 ) ) 
				shield->IncHP( -1 );
			if( shield->GetHP() <= 0 )
			{
				if( targSock != NULL )
					targSock->sysmessage( 283 );
				shield->Delete();
			}
		}
	}

	// Armor / Skin Absorbtion
	UI16 getDef;
	const SI08 hitMsg = DoHitMessage( mChar, ourTarg, targSock, damage );
	if( ourTarg->isHuman() )
	{
		getDef = calcDef( ourTarg, hitMsg, true );
		getDef = HalfRandomNum( getDef );
	}
	else if( ourTarg->GetDef() > 0 )
		getDef = ourTarg->GetDef();
	else
		getDef = 20;

	// Damage based on Attack Skill (Armor defends less against low-skill characters)
	damage -= (SI16)( ( getDef * attSkill ) / 750 );

	// Give them one last chance to do Damage
	if( damage <= 0 ) 
		damage = RandomNum( 0, 4 );

	if( !ourTarg->IsNpc() ) 
		damage /= cwmWorldState->ServerData()->CombatNPCDamageRate(); // Rate damage against other players

	return damage;
}

void CHandleCombat::HandleSplittingNPCs( CChar *toSplit )
{
	UI08 splitnum;
	if( toSplit->GetSplit() > 0 && toSplit->GetHP() >= 1 )
	{
		if( RandomNum( 0, 100 ) <= toSplit->GetSplitChance() )
		{
			if( toSplit->GetSplit() == 1 ) 
				splitnum = 1;
			else 
				splitnum = RandomNum( static_cast< UI08 >(1), toSplit->GetSplit() );

			for( UI08 splitcount = 0; splitcount < splitnum; ++splitcount )
				Npcs->Split( toSplit );
		}
	}
}

void CHandleCombat::HandleCombat( cSocket *mSock, CChar *mChar, CChar *ourTarg )
{
	const UI16 ourDist			= getDist( mChar, ourTarg );
	CItem *mWeapon		= getWeapon( mChar );
	const UI08 getFightSkill	= getCombatSkill( mWeapon );
	UI08 bowType		= 0;

	bool checkDist		= (ourDist <= 1);

	UI16 targTrig		= mChar->GetScriptTrigger();
	cScript *toExecute	= Trigger->GetScript( targTrig );
	if( toExecute != NULL )
		toExecute->OnSwing( mWeapon, mChar, ourTarg );

	if( !checkDist && getFightSkill == ARCHERY )
		checkDist = LineOfSight( mSock, mChar, ourTarg->GetX(), ourTarg->GetY(), ourTarg->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING );

	if( checkDist )
	{
		if( mChar->IsNpc() )
		{
			if( mChar->GetNpcWander() != 5 )
			{
				UI08 charDir = Movement->Direction( mChar, ourTarg->GetX(), ourTarg->GetY() );
				if( mChar->GetDir() != charDir && charDir < 8 )
					mChar->SetDir( charDir );
			}
		}

		PlaySwingAnimations( mChar );
		if( getFightSkill == ARCHERY )
		{
			bowType = getBowType( mWeapon );
			if( mChar->IsNpc() || DeleteItemAmount( mChar, 1, ((bowType == BOWS) ? 0x0F3F : 0x1BFB) ) == 1 )
				Effects->PlayMovingAnimation( mChar, ourTarg, ((bowType == BOWS) ? 0x0F42 : 0x1BFE), 0x08, 0x00, 0x00 );
			else
				return;
		}

		SI16 staminaToLose = cwmWorldState->ServerData()->CombatAttackStamina();
		if( staminaToLose && ( !mChar->IsGM() && !mChar->IsCounselor() ) )
			mChar->IncStamina( staminaToLose );

		bool skillPassed = false;
		if( Skills->CheckSkill( mChar, getFightSkill, 0, 1000 ) )
		{
			UI16 getAttackSkill = ( mChar->GetSkill( getFightSkill ) + mChar->GetSkill( TACTICS ) ) / 2;
			UI16 getDefSkill	= ourTarg->GetSkill( TACTICS );
			skillPassed			= ( !RandomNum( 0, 5 ) || RandomNum( static_cast< UI16 >(getAttackSkill / 1.25 ), getAttackSkill ) >= RandomNum( static_cast< UI16 >( getDefSkill / 1.25), getDefSkill ) );
		}
		if( !skillPassed )
		{
			if( getFightSkill == ARCHERY && !RandomNum( 0, 2 ) )
				Items->CreateItem( NULL, ourTarg, ((bowType == BOWS) ? 0x0F3F : 0x1BFB), 1, 0, OT_ITEM );

			PlayMissedSoundEffect( mChar );
		}
		else
		{
			cSocket *targSock = calcSocketObjFromChar( ourTarg );

			Skills->CheckSkill( mChar, TACTICS, 0, 1000 );

			switch( ourTarg->GetID() )
			{
			case 0x0191:	Effects->PlaySound( ourTarg, 0x014B );									break;
			case 0x0190:	Effects->PlaySound( ourTarg, 0x0156 );									break;
			default:		Effects->playMonsterSound( ourTarg, ourTarg->GetID(), SND_DEFEND );		break;
			}

			if( mChar->GetPoisonStrength() && ourTarg->GetPoisoned() < mChar->GetPoisonStrength() )
			{
				if( ( getFightSkill == FENCING || getFightSkill == SWORDSMANSHIP ) && !RandomNum( 0, 2 ) )
				{
					ourTarg->SetPoisoned( mChar->GetPoisonStrength() );
					ourTarg->SetTimer( tCHAR_POISONTIME, BuildTimeValue(static_cast<R32> (40 / ourTarg->GetPoisoned() )) ); // a lev.1 poison takes effect after 40 secs, a deadly pois.(lev.4) takes 40/4 secs - AntiChrist
					ourTarg->SetTimer( tCHAR_POISONWEAROFF, ourTarg->GetTimer( tCHAR_POISONTIME ) + ( 1000 * cwmWorldState->ServerData()->SystemTimer( POISON ) ) ); //wear off starts after poison takes effect - AntiChrist
					if( targSock != NULL )
						targSock->sysmessage( 282 );
				}
			}

			UI16 ourDamage = calcDamage( mChar, ourTarg, targSock, mWeapon, getFightSkill );
			if( ourDamage > 0 )
			{
				// Interrupt Spellcasting
				if( !ourTarg->IsNpc() )
				{
					if( ourTarg->IsCasting() && targSock->CurrentSpellType() == 0 )
					{
						ourTarg->StopSpell();
						ourTarg->SetFrozen( false );
						if( targSock != NULL )
							targSock->sysmessage( 306 );
					}
				}
				// Reactive Armor
				if( ourTarg->GetReactiveArmour() )
				{
					int retDamage = (int)( ourDamage * ( ourTarg->GetSkill( MAGERY ) / 2000.0 ) );
					ourTarg->IncHP( -( ourDamage - retDamage ) );
					if( ourTarg->IsNpc() ) 
						retDamage *= cwmWorldState->ServerData()->CombatNPCDamageRate();
					mChar->IncHP( -retDamage );
					Effects->PlayStaticAnimation( ourTarg, 0x374A, 0, 15 );
				}
				else 
				{
					CPDisplayDamage toDisplay( (*ourTarg), ourDamage );
					if( mSock != NULL )
						mSock->Send( &toDisplay );
					if( targSock != NULL )
						targSock->Send( &toDisplay );
					ourTarg->IncHP( -ourDamage );
				}

				// Splitting NPC's
				if( ourTarg->IsNpc() )
					HandleSplittingNPCs( ourTarg );
			}
			if( mChar->isHuman() )
				PlayHitSoundEffect( mChar, mWeapon );

			if( toExecute != NULL )
				toExecute->OnAttack( mChar, ourTarg );
			UI16 defScript	= ourTarg->GetScriptTrigger();
			toExecute		= Trigger->GetScript( defScript );
			if( toExecute != NULL )
				toExecute->OnDefense( mChar, ourTarg );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	inline void QuickSwitch( CChar *mChar, CChar *defend, SI16 spellNum )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Allows beneficial spells to be cast on self during combat
//o---------------------------------------------------------------------------o
inline void QuickSwitch( CChar *mChar, CChar *ourTarg, SI08 spellNum )
{
	if( !ValidateObject( mChar ) || !ValidateObject( ourTarg ) || mChar == ourTarg )
		return;

	mChar->SetSpellCast( spellNum );
	mChar->SetTarg( mChar );
	Magic->CastSpell( NULL, mChar );
	mChar->SetTarg( ourTarg );
	mChar->StopSpell();
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool CastSpell( CChar *mChar, CChar *defend, SI16 spellNum )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handles spellcasting during combat
//o---------------------------------------------------------------------------o
bool CHandleCombat::CastSpell( CChar *mChar, CChar *ourTarg, SI08 spellNum )
{
	if( !ValidateObject( mChar ) || !ValidateObject( ourTarg ) || mChar == ourTarg )
		return false;

	if( mChar->GetMana() <= Magic->spells[spellNum].Mana() )
		return false;
	switch( spellNum )
	{
		case 7:
			if( !mChar->GetReactiveArmour() )
				mChar->SetSpellCast( spellNum );
			else
				return false;
			break;
		case 11:
			if( mChar->GetPoisoned() > 0 )
				QuickSwitch( mChar, ourTarg, spellNum );
			else
				return false;
			break;
		case 4:
		case 29:
			if( mChar->GetStrength() > mChar->GetHP() ) 
				QuickSwitch( mChar, ourTarg, spellNum );
			else
				return false;
			break;
		case 36:
			if( !mChar->IsPermReflected() )
				mChar->SetSpellCast( spellNum );
			else
				return false;
			break;
		case 1:
		case 3:
		case 5:
		case 18:
		case 20:
		case 27:
		case 30:
		case 31:
		case 33:
		case 37:
		case 38:
		case 41:
		case 42:
		case 43:
		case 46:
		case 49:
		case 51:
		case 53:
		case 55:
		case 57:
		case 58:
			mChar->SetSpellCast( spellNum );
			break;
	}
	return true;
}

void CHandleCombat::HandleNPCSpellAttack( CChar *npcAttack, CChar *cDefend, UI16 playerDistance )
{
	if( npcAttack->GetTimer( tNPC_SPATIMER ) <= cwmWorldState->GetUICurrentTime() )
	{
		if( playerDistance < cwmWorldState->ServerData()->CombatMaxSpellRange() )
		{
			SI16 spattacks = npcAttack->GetSpAttack();
			if( spattacks <= 0 )
				return;
			if( spattacks > 8 )
				spattacks = 8;

			npcAttack->StopSpell();
			switch( ( RandomNum( static_cast< SI16 >(0), spattacks ) + 1 ) )
			{
				case 1:
					switch( RandomNum( 1, 6 ) ) 
					{
						case 1:		CastSpell( npcAttack, cDefend, 1 );			break;	// Clumsy
						case 2:		CastSpell( npcAttack, cDefend, 3 );			break;	// Feeble Mind
						case 3:		CastSpell( npcAttack, cDefend, 4 );			break;	// Heal (personal...switch target temporarily to self and then back after casting)
						case 4:		CastSpell( npcAttack, cDefend, 5 );			break;	// Magic Arrow
						case 5:		CastSpell( npcAttack, cDefend, 7 );			break;	// Reactive Armor (personal...switch target temporarily to self and then back after casting)
						case 6:			// Check if poisoned and cast cure (switch to self) - Hanse
							if( !CastSpell( npcAttack, cDefend, 11 ) )					// cure
								CastSpell( npcAttack, cDefend, 8 );						// weaken
							break;
					}
					break;
				case 2:			CastSpell( npcAttack, cDefend, 12 );		break;	// Harm
				case 3:
					switch( RandomNum( 1, 2 ) ) 
					{
						case 1:		CastSpell( npcAttack, cDefend, 18 );		break;	// Fireball
						case 2:		CastSpell( npcAttack, cDefend, 20 );		break;	// Poison
					}
					break;
				case 4:
					switch( RandomNum( 1, 4 ) ) 
					{
						case 1:		CastSpell( npcAttack, cDefend, 27 );		break;	// Curse
						case 2:		CastSpell( npcAttack, cDefend, 29 );		break;	// Greater Healing (personal...switch target temporarily to self and then back after casting)
						case 3:		CastSpell( npcAttack, cDefend, 30 );		break;	// Lightning
						case 4:		CastSpell( npcAttack, cDefend, 31 );		break;	// Mana Drain
					}
					break;
				case 5:
					switch( RandomNum( 1, 4 ) ) 
					{
						case 1:		CastSpell( npcAttack, cDefend, 33 );		break;	// Blade Spirits
						case 2:		CastSpell( npcAttack, cDefend, 36 );		break;	// Magic Reflection (personal...no target switching required)
						case 3:		CastSpell( npcAttack, cDefend, 37 );		break;	// Mind Blast
						case 4:		CastSpell( npcAttack, cDefend, 38 );		break;	// Paralyze
					}
					break;
				case 6:	// If NPC is fighting summoned NPC, cast dispel...else cast other spells - Hanse
					if( cDefend->IsNpc() )
					{
						if( cDefend->GetTimer( tNPC_SUMMONTIME ) > 0 && cDefend->GetNPCAiType() != aiGUARD ) 
						{
							CastSpell( npcAttack, cDefend, 41 );
							break;
						}
					}
					switch( RandomNum( 1, 3 ) ) 
					{
						case 1:		CastSpell( npcAttack, cDefend, 42 );		break;	// Energy Bolt
						case 2:		CastSpell( npcAttack, cDefend, 43 );		break;	// Explosion
						case 3:		CastSpell( npcAttack, cDefend, 46 );		break;	// Mass Curse
					}
					break;
				case 7:
					switch( RandomNum( 1, 4 ) )
					{
						case 1:		CastSpell( npcAttack, cDefend, 49 );		break;	// Chain Lightning
						case 2:		CastSpell( npcAttack, cDefend, 51 );		break;	// Flamestrike
						case 3:		CastSpell( npcAttack, cDefend, 53 );		break;	// Mana Vampire
						case 4:		CastSpell( npcAttack, cDefend, 55 );		break;	// Meteor Swarm
					}
					break;
				case 8:
					switch( RandomNum( 1, 2 ) ) 
					{
						case 1:		CastSpell( npcAttack, cDefend, 57 );		break;	// Earthquake
						case 2:		CastSpell( npcAttack, cDefend, 58 );		break;	// Energy Vortex
					}
					break;
				// This is where dragon attacks go eventually when the npc.scp is fixed... - Hanse
				/*
				case 9:
				case 10:
				case 11:
				*/
			default:
				break;
			}
			if( npcAttack->GetSpellCast() != -1 )
				Magic->CastSpell( NULL, npcAttack );
			npcAttack->SetTimer( tNPC_SPATIMER, BuildTimeValue( npcAttack->GetSpDelay() ) );
		}
	}
}

R32 CHandleCombat::GetCombatTimeout( CChar *mChar )
{
	R32 getDelay	= (R32)( (R32)UOX_MIN( mChar->GetDexterity(), static_cast<SI16>(100) ) + 100 );
	int getOffset	= 0;

	CItem *mWeapon = getWeapon( mChar );
	if( ValidateObject( mWeapon ) )
	{
		if( mWeapon->GetSpeed() == 0 ) 
			mWeapon->SetSpeed( 35 );
		getOffset = mWeapon->GetSpeed();
	}
	else
	{
		if( mChar->GetSkill( WRESTLING ) < 800 )
			getOffset = (((int)(mChar->GetSkill( WRESTLING ) / 200 )) * 5) + 30;
		else
			getOffset = 50;
	}
	getDelay = (15000 / (getDelay * getOffset));
	return getDelay;
}

//o--------------------------------------------------------------------------
//|	Function		-	InvalidateAttacker( CChar *mChar )
//|	Date			-	3rd July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Resets the attacker attack so that it cancels attack 
//|						setup.
//o--------------------------------------------------------------------------
void CHandleCombat::InvalidateAttacker( CChar *mChar )
{
	if( mChar->IsNpc() && mChar->GetNPCAiType() == aiGUARD )
	{
		mChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 20 ) );
		mChar->SetNpcWander( 2 );
		mChar->SetTimer( tNPC_MOVETIME, BuildTimeValue(static_cast<R32>( cwmWorldState->ServerData()->NPCSpeed() )) );
		mChar->talkAll( 281, false );
	}

	CChar *ourTarg = mChar->GetTarg();
	if( ValidateObject( ourTarg ) && ourTarg->GetTarg() == mChar )
	{
		ourTarg->SetTarg( NULL );
		ourTarg->SetAttacker( NULL );
		ourTarg->SetAttackFirst( false );
		if( ourTarg->IsAtWar() )
			ourTarg->ToggleCombat();
	}
	mChar->SetTarg( NULL );
	CChar *attAttacker = mChar->GetAttacker();
	if( ValidateObject( attAttacker ) )
	{
		attAttacker->SetAttackFirst( false );
		attAttacker->SetAttacker( NULL );
	}
	mChar->SetAttacker( NULL );
	mChar->SetAttackFirst( false );
	if( mChar->IsAtWar() && mChar->IsNpc() && !mChar->IsDead() && mChar->GetNPCAiType() != aiPLAYERVENDOR )
		mChar->ToggleCombat();
}

//o---------------------------------------------------------------------------o
//|	Function	-	Kill( CChar *mChar, CChar *ourTarg )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle death during combat
//o---------------------------------------------------------------------------o
void CHandleCombat::Kill( CChar *mChar, CChar *ourTarg )
{
	Karma( mChar, ourTarg, ( 0 - ( ourTarg->GetKarma() ) ) );
	Fame( mChar, ourTarg->GetFame() );
	if( mChar->GetNPCAiType() == aiGUARD && ourTarg->IsNpc() )
	{
		Effects->PlayCharacterAnimation( ourTarg, 0x15 );
		Effects->playDeathSound( ourTarg );

		ourTarg->Delete(); // Guards, don't give body
		mChar->ToggleCombat();
		return;
	}

	// Add murder counts
	if( !mChar->IsNpc() && !ourTarg->IsNpc() )
	{
		if( mChar->DidAttackFirst() && WillResultInCriminal( mChar, ourTarg ) )
		{
			mChar->SetKills( mChar->GetKills() + 1 );
			cSocket *aSock = calcSocketObjFromChar( mChar );
			if( aSock != NULL )
			{
				aSock->sysmessage( 314, mChar->GetKills() );
				if( mChar->GetKills() == cwmWorldState->ServerData()->RepMaxKills() + 1 )
					aSock->sysmessage( 315 );
			}
		}
		Console.Log( Dictionary->GetEntry( 1617 ).c_str(), "PvP.log", ourTarg->GetName().c_str(), mChar->GetName().c_str() );
	}
	InvalidateAttacker( mChar );
	doDeathStuff( ourTarg );
}

void CHandleCombat::CombatLoop( cSocket *mSock, CChar *mChar )
{
	CChar *ourTarg = mChar->GetTarg();
	if( ourTarg == NULL )
		return;

	if( mChar->GetTimer( tCHAR_TIMEOUT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		bool validTarg = false;
		if( !mChar->IsDead() && ValidateObject( ourTarg ) && !ourTarg->isFree() && ( ourTarg->IsNpc() || isOnline( ourTarg ) ) )
		{
			if( !ourTarg->IsInvulnerable() && !ourTarg->IsDead() && ourTarg->GetNPCAiType() != aiPLAYERVENDOR )
			{
				if( charInRange( mChar, ourTarg ) )
				{
					validTarg = true;
					if( mChar->IsNpc() && mChar->GetSpAttack() > 0 && mChar->GetMana() > 0 && !RandomNum( 0, 4 ) )
						HandleNPCSpellAttack( mChar, ourTarg, getDist( mChar, ourTarg ) );
					else
						HandleCombat( mSock, mChar, ourTarg );

					if( !ValidateObject( ourTarg->GetTarg() ) || !objInRange( ourTarg, ourTarg->GetTarg(), DIST_INRANGE ) )		//if the defender is swung at, and they don't have a target already, set this as their target
						StartAttack( ourTarg, mChar );
				}
				else if( mChar->IsNpc() && mChar->GetNPCAiType() == aiGUARD && mChar->GetRegion()->IsGuarded() )
				{
					validTarg = true;
					mChar->SetLocation( ourTarg );
					Effects->PlaySound( mChar, 0x01FE );
					Effects->PlayStaticAnimation( mChar, 0x372A, 0x09, 0x06 );
					mChar->talkAll( 1616, true );
				}
				else
					InvalidateAttacker( mChar );
			}
		}
		if( !validTarg )
		{
			mChar->ToggleCombat();
			mChar->SetTarg( NULL );
			mChar->SetAttacker( NULL );
			mChar->SetAttackFirst( false );
		}
		else
		{
			if( ourTarg->GetHP() <= 0 )
				Kill( mChar, ourTarg );
			mChar->SetTimer( tCHAR_TIMEOUT, BuildTimeValue( GetCombatTimeout( mChar ) ) );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle spawning a guard in guarded areas
//|					NEED TO REWORK FOR REGIONAL GUARD STUFF
//o---------------------------------------------------------------------------o
void CHandleCombat::SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z )
{
	if( !ValidateObject( mChar ) || !ValidateObject( targChar ) )
		return;

	if( targChar->IsDead() || targChar->IsInvulnerable() )
		return;

	cTownRegion *targRegion = mChar->GetRegion(); 

	if( !targRegion->IsGuarded() || !cwmWorldState->ServerData()->GuardsStatus() )
		return;

	bool reUseGuard		= false;
	CChar *getGuard		= NULL;
	SubRegion *toCheck	= MapRegion->GetCell( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );

	if( toCheck != NULL )
	{
		for( getGuard = toCheck->FirstChar(); !toCheck->FinishedChars(); getGuard = toCheck->GetNextChar() )
		{
			if( !ValidateObject( getGuard ) )
				continue;

			if( !getGuard->IsNpc() || getGuard->GetNPCAiType() != aiGUARD )
				continue;

			if( !ValidateObject( getGuard->GetTarg() ) || getGuard->GetTarg() == targChar  )
			{
				if( charInRange( getGuard, targChar ) )
					reUseGuard = true;
			}
			else if( getGuard->GetTarg() == targChar )
				return;
		}
	}
	// 1/13/2003 - Maarc - Fix for JSE NocSpawner
	if( !reUseGuard )
	{
		getGuard = targRegion->GetRandomGuard();
		if( ValidateObject( getGuard ) )
		{
			getGuard->SetLocation( x, y, z, mChar->WorldNumber() );
			Npcs->PostSpawnUpdate( getGuard );
		}
		else
			return;
	}
	//
	if( ValidateObject( getGuard ) )
	{
		getGuard->SetAttackFirst( true );
		getGuard->SetAttacker( targChar );
		getGuard->SetTarg( targChar );
		getGuard->SetNpcWander( 2 );
		if( !getGuard->IsAtWar() )
			getGuard->ToggleCombat();

		if( reUseGuard )
			getGuard->SetLocation( targChar );
		else
		{
			getGuard->SetTimer( tNPC_MOVETIME, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->NPCSpeed() )) );
			getGuard->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 25 ) );

			Effects->PlaySound( getGuard, 0x01FE );
			Effects->PlayStaticAnimation( getGuard, 0x372A, 0x09, 0x06 );
			
			getGuard->talkAll( 313, true );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool WillResultInCriminal( CChar *mChar, CChar *targ )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check flagging, race, and guild info to find if character
//|					should be flagged criminal (returns true if so)
//o---------------------------------------------------------------------------o
bool CHandleCombat::WillResultInCriminal( CChar *mChar, CChar *targ )
{
	if( !ValidateObject( mChar ) || !ValidateObject( targ ) || mChar == targ ) 
		return false;
	else if( !GuildSys->ResultInCriminal( mChar, targ ) || Races->Compare( mChar, targ ) != 0 ) 
		return false;
	else if( targ->IsInnocent() )
		return true;
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	petGuardAttack( CChar *mChar, CChar *owner, SERIAL guarded )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the pet guarding an item / character and have him attack
//|					the person using / attacking the item / character
//o---------------------------------------------------------------------------o
void CHandleCombat::petGuardAttack( CChar *mChar, CChar *owner, cBaseObject *guarded )
{
	if( !ValidateObject( mChar ) || !ValidateObject( owner ) || !ValidateObject( guarded ) )
		return;

	if( mChar->GetSerial() == owner->GetSerial() || mChar->IsCounselor() || mChar->IsGM() )
		return;

	CChar *petGuard = Npcs->getGuardingPet( owner, guarded );
	if( ValidateObject( petGuard ) && objInRange( mChar, petGuard, cwmWorldState->ServerData()->CombatMaxRange() ) )
	{
		if( !Npcs->checkPetFriend( mChar, petGuard ) )
			AttackTarget( petGuard, mChar );
		else
		{
			cSocket *oSock = calcSocketObjFromChar( owner );
			if( oSock != NULL )
				oSock->sysmessage( 1629 );
		}
	}
}

}
