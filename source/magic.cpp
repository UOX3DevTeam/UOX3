#include "uox3.h"
#include "magic.h"
#include "power.h"
#include "weight.h"
#include "cGuild.h"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "cMagic.h"
#include "skills.h"
#include "ssection.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "combat.h"
#include "Dictionary.h"
#include "movement.h"
#include "scriptc.h"
#include "StringUtility.hpp"
#include "ObjectFactory.h"
#include <algorithm>
#include "osunique.hpp"
CMagic *Magic = nullptr;

#define SPELL_MAX 214 // use define for now; can make autocount later

// Look up spell-names from dictionary-files
const MagicTable_st magic_table[] = {
	{ 593, (MAGIC_DEFN)&splClumsy },
	{ 594, (MAGIC_DEFN)&splCreateFood },
	{ 595, (MAGIC_DEFN)&splFeeblemind },
	{ 596, (MAGIC_DEFN)&splHeal },
	{ 597, (MAGIC_DEFN)&splMagicArrow },
	{ 598, (MAGIC_DEFN)&splNightSight },
	{ 599, (MAGIC_DEFN)&splReactiveArmor },
	{ 600, (MAGIC_DEFN)&splWeaken },
	{ 601, (MAGIC_DEFN)&splAgility },
	{ 602, (MAGIC_DEFN)&splCunning },
	{ 603, (MAGIC_DEFN)&splCure },
	{ 604, (MAGIC_DEFN)&splHarm },
	{ 605, (MAGIC_DEFN)&splMagicTrap },
	{ 606, (MAGIC_DEFN)&splMagicUntrap },
	{ 607, (MAGIC_DEFN)&splProtection },
	{ 608, (MAGIC_DEFN)&splStrength },
	{ 609, (MAGIC_DEFN)&splBless },
	{ 610, (MAGIC_DEFN)&splFireball },
	{ 611, (MAGIC_DEFN)&splMagicLock },
	{ 612, (MAGIC_DEFN)&splPoison },
	{ 613, (MAGIC_DEFN)&splTelekinesis },
	{ 614, (MAGIC_DEFN)&splTeleport },
	{ 615, (MAGIC_DEFN)&splUnlock },
	{ 616, (MAGIC_DEFN)&splWallOfStone },
	{ 617, (MAGIC_DEFN)&splArchCure },
	{ 618, (MAGIC_DEFN)&splArchProtection },
	{ 619, (MAGIC_DEFN)&splCurse },
	{ 620, (MAGIC_DEFN)&splFireField },
	{ 621, (MAGIC_DEFN)&splGreaterHeal },
	{ 622, (MAGIC_DEFN)&splLightning },
	{ 623, (MAGIC_DEFN)&splManaDrain },
	{ 624, (MAGIC_DEFN)&splRecall },
	{ 625, (MAGIC_DEFN)&splBladeSpirits },
	{ 626, (MAGIC_DEFN)&splDispelField },
	{ 627, (MAGIC_DEFN)&splIncognito },
	{ 628, (MAGIC_DEFN)&splMagicReflection },
	{ 629, (MAGIC_DEFN)&splMindBlast },
	{ 630, (MAGIC_DEFN)&splParalyze },
	{ 631, (MAGIC_DEFN)&splPoisonField },
	{ 632, (MAGIC_DEFN)&splSummonCreature },
	{ 633, (MAGIC_DEFN)&splDispel },
	{ 634, (MAGIC_DEFN)&splEnergyBolt },
	{ 635, (MAGIC_DEFN)&splExplosion },
	{ 636, (MAGIC_DEFN)&splInvisibility },
	{ 637, (MAGIC_DEFN)&splMark },
	{ 638, (MAGIC_DEFN)&splMassCurse },
	{ 639, (MAGIC_DEFN)&splParalyzeField },
	{ 640, (MAGIC_DEFN)&splReveal },
	{ 641, (MAGIC_DEFN)&splChainLightning },
	{ 642, (MAGIC_DEFN)&splEnergyField },
	{ 643, (MAGIC_DEFN)&splFlameStrike },
	{ 644, (MAGIC_DEFN)&splGateTravel },
	{ 645, (MAGIC_DEFN)&splManaVampire },
	{ 646, (MAGIC_DEFN)&splMassDispel },
	{ 647, (MAGIC_DEFN)&splMeteorSwarm },
	{ 648, (MAGIC_DEFN)&splPolymorph },
	{ 649, (MAGIC_DEFN)&splEarthquake },
	{ 650, (MAGIC_DEFN)&splEnergyVortex },
	{ 651, (MAGIC_DEFN)&splResurrection },
	{ 652, (MAGIC_DEFN)&splSummonAir },
	{ 653, (MAGIC_DEFN)&splSummonDaemon },
	{ 654, (MAGIC_DEFN)&splSummonEarth },
	{ 655, (MAGIC_DEFN)&splSummonFire },
	{ 656, (MAGIC_DEFN)&splSummonWater },
	{ 657, (MAGIC_DEFN)&splRandom },
	{ 658, (MAGIC_DEFN)&splNecro1 },
	{ 659, (MAGIC_DEFN)&splNecro2 },
	{ 660, (MAGIC_DEFN)&splNecro3 },
	{ 661, (MAGIC_DEFN)&splNecro4 },
	{ 662, (MAGIC_DEFN)&splNecro5 },
	{ 0, nullptr }
};

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SpawnGate()
//| Date		-	17th September, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	It will construct 2 linked gates, one at srcX / srcY / srcZ and another at trgX / trgY / trgZ
//o------------------------------------------------------------------------------------------------o
void SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, UI08 srcWorld, SI16 trgX, SI16 trgY, SI08 trgZ, UI08 trgWorld, UI16 trgInstanceId )
{
	CItem *g1 = Items->CreateItem( nullptr, caster, 0x0F6C, 1, 0, OT_ITEM );
	if( ValidateObject( g1 ))
	{
		g1->SetDecayable( true );
		g1->SetType( IT_GATE );
		g1->SetLocation( srcX, srcY, srcZ, srcWorld, g1->GetInstanceId() );
		g1->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_GATE ));
		g1->SetDir( 1 );

		CItem *g2 = Items->CreateItem( nullptr, caster, 0x0F6C, 1, 0, OT_ITEM );
		if( ValidateObject( g2 ))
		{
			g2->SetDecayable( true );
			g2->SetType( IT_GATE );
			g2->SetLocation( trgX, trgY, trgZ, trgWorld, trgInstanceId );
			g2->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_GATE ));
			g2->SetDir( 1 );

			g2->SetTempVar( CITV_MOREX, g1->GetSerial() );
			g1->SetTempVar( CITV_MOREX, g2->GetSerial() );
		}
		else
		{
			g1->Delete();
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FieldSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates rows of items for field spells like firewalls, wall of stone, etc
//o------------------------------------------------------------------------------------------------o
bool FieldSpell( CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 fieldDir, SI08 spellNum )
{
	SI16 fx[5], fy[5];
	if( fieldDir )
	{
		fx[0] = fx[1] = fx[2] = fx[3] = fx[4] = x;
		fy[0] = y - 1;
		fy[1] = y + 1;
		fy[2] = y - 2;
		fy[3] = y + 2;
		fy[4] = y;
	}
	else
	{
		fy[0] = fy[1] = fy[2] = fy[3] = fy[4] = y;
		fx[0] = x - 1;
		fx[1] = x + 1;
		fx[2] = x - 2;
		fx[3] = x + 2;
		fx[4] = x;
	}
	CItem *i = nullptr;
	UI08 worldNumber = caster->WorldNumber();
	UI16 instanceId = caster->GetInstanceId();
	for( UI08 j = 0; j < 5; ++j )		// how about we make this 5, huh?  missing part of the field
	{
		if( id != 0x0080 || ( id == 0x0080 && !Movement->CheckForCharacterAtXYZ( caster, fx[j], fy[j], z )))
		{
			i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
			if( i != nullptr )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );

				// Duration of field item based on caster's Magery skill
				i->SetDecayTime( BuildTimeValue( static_cast<R32>( caster->GetSkill( MAGERY ) / 15 )));

				// If Poison Field, use average of caster's Magery and Poisoning skills
				if( spellNum == 39 ) // Poison Field
				{
					i->SetTempVar( CITV_MOREX, static_cast<UI16>(( caster->GetSkill( MAGERY ) + caster->GetSkill( POISONING )) / 2 ));
				}
				else
				{
					i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY )); // remember casters magery skill for damage
				}

				// Store caster's serial in field item
				i->SetTempVar( CITV_MOREY, caster->GetSerial() );
				i->SetLocation( fx[j], fy[j], Map->Height( fx[j], fy[j], z, worldNumber, instanceId ));
				i->SetDir( 29 );
				i->SetMovable( 2 );

				// Don't save these temporary effect items!
				i->ShouldSave( false );

				CMagicStat temp = Magic->spells[spellNum].StaticEffect();
				if( temp.Effect() != INVALIDID )
				{
					Effects->PlayStaticAnimation( i, temp.Effect(), temp.Speed(), temp.Loop() );
				}
			}
		}
	}
	return true;
}

bool splClumsy( [[maybe_unused]] CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/level1targ.js
	return false;
}
bool splCreateFood( [[maybe_unused]] CSocket *sock, [[maybe_unused]] CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/createfood.js
	return false;
}
bool splFeeblemind( [[maybe_unused]] CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/level1targ.js
	return false;
}
bool splHeal( [[maybe_unused]] CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/level1targ.js
	return false;
}
bool splMagicArrow( [[maybe_unused]] CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/level1targ.js
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splNightSight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Night Sight spell
//o------------------------------------------------------------------------------------------------o
bool splNightSight( CChar *caster, CChar *target, CChar *src, [[maybe_unused]] SI08 curSpell )
{
	Effects->TempEffect( src, target, 2, 0, 0, 0 );
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splReactiveArmor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Reactive Armor spell
//o------------------------------------------------------------------------------------------------o
bool splReactiveArmor( CChar *caster, CChar *target, CChar *src, [[maybe_unused]] SI08 curSpell )
{
	Effects->TempEffect( src, target, 15, caster->GetSkill( MAGERY ) / 100, 0, 0 );
	target->SetReactiveArmour( true );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splWeaken()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Weaken spell
//o------------------------------------------------------------------------------------------------o
bool splWeaken( CChar *caster, CChar *target, CChar *src, [[maybe_unused]] SI08 curSpell )
{
	Effects->TempEffect( src, target, 5, caster->GetSkill( MAGERY ) / 100, 0, 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splAgility()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Agility spell
//o------------------------------------------------------------------------------------------------o
bool splAgility( CChar *caster, CChar *target, CChar *src, [[maybe_unused]] SI08 curSpell )
{
	Effects->TempEffect( src, target, 6, caster->GetSkill( MAGERY ) / 100, 0, 0 );
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splCunning()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Cunning spell
//o------------------------------------------------------------------------------------------------o
bool splCunning( CChar *caster, CChar *target, CChar *src, [[maybe_unused]] SI08 curSpell )
{
	Effects->TempEffect( src, target, 7, caster->GetSkill( MAGERY ) / 100, 0, 0 );
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splCure()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Cure spell
//o------------------------------------------------------------------------------------------------o
bool splCure( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	target->SetPoisoned( 0 );
	target->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->GetUICurrentTime() );
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splHarm()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Harm spell
//o------------------------------------------------------------------------------------------------o
bool splHarm( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Effects->TempEffect( src, target, 29, spellDamage, 0, 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMagicTrap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Magic Trap spell
//o------------------------------------------------------------------------------------------------o
bool splMagicTrap( CSocket *sock, CChar *caster, CItem *target, [[maybe_unused]] SI08 curSpell )
{
	if( target->IsContType() && target->GetId() != 0x0E75 )
	{
		target->SetTempVar( CITV_MOREZ, CalcSerial( 1, caster->GetSkill( MAGERY ) / 200, caster->GetSkill( MAGERY ) / 100, 0 ));
		if( Magic->spells[13].Effect() != INVALIDID )
		{
			Effects->PlaySound( caster, Magic->spells[13].Effect() );
		}

		CMagicStat temp = Magic->spells[13].StaticEffect();
		if( temp.Effect() != INVALIDID )
		{
			Effects->PlayStaticAnimation( target, temp.Effect(), temp.Speed(), temp.Loop() );
		}
	}
	else
	{
		sock->SysMessage( 663 ); // You cannot trap this!
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMagicUntrap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Magic Untrap spell
//o------------------------------------------------------------------------------------------------o
bool splMagicUntrap( CSocket *sock, CChar *caster, CItem *target, [[maybe_unused]] SI08 curSpell )
{
	if( target->IsContType() )
	{
		if( target->GetTempVar( CITV_MOREZ, 1 ) == 1 )
		{
			if( RandomNum( 1, 100 ) <= 50 + ( caster->GetSkill( MAGERY ) / 10 ) - target->GetTempVar( CITV_MOREZ, 3 ))
			{
				target->SetTempVar( CITV_MOREZ, 1, 0 );
				if( Magic->spells[14].Effect() != INVALIDID )
				{
					Effects->PlaySound( caster, Magic->spells[14].Effect() );
				}

				CMagicStat temp = Magic->spells[14].StaticEffect();
				if( temp.Effect() != INVALIDID )
				{
					Effects->PlayStaticAnimation( target, temp.Effect(), temp.Speed(), temp.Loop() );
				}
				sock->SysMessage( 664 ); // You successful untrap this item!
			}
			else
			{
				sock->SysMessage( 665 ); // You miss to untrap this item!
			}
		}
		else
		{
			sock->SysMessage( 667 ); // This items doesn't seem to be trapped!
		}
	}
	else
	{
		sock->SysMessage( 668 ); // This items cannot be trapped!
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splProtection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Protection spell
//o------------------------------------------------------------------------------------------------o
bool splProtection( CChar *caster, CChar *target, CChar *src, [[maybe_unused]] SI08 curSpell )
{
	Effects->TempEffect( src, target, 21, caster->GetSkill( MAGERY ) / 10, 0, 0 );
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splStrength()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Strength spell
//o------------------------------------------------------------------------------------------------o
bool splStrength( CChar *caster, CChar *target, CChar *src, [[maybe_unused]] SI08 curSpell )
{
	Effects->TempEffect( src, target, 8, caster->GetSkill( MAGERY ) / 100, 0, 0 );
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splBless()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Bless spell
//o------------------------------------------------------------------------------------------------o
bool splBless( CChar *caster, CChar *target, CChar *src, [[maybe_unused]] SI08 curSpell )
{
	SI32 j = caster->GetSkill( MAGERY ) / 100;
	Effects->TempEffect( src, target, 11, j, j, j);
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splFireball()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Fireball spell
//o------------------------------------------------------------------------------------------------o
bool splFireball( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Effects->TempEffect( src, target, 30, spellDamage, 0, 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMagicLock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Magic Lock spell
//o------------------------------------------------------------------------------------------------o
bool splMagicLock( CSocket *sock, CChar *caster, CItem *target, [[maybe_unused]] SI08 curSpell )
{
	ItemTypes type = target->GetType();
	if(( type == IT_CONTAINER || type == IT_DOOR || type == IT_SPAWNCONT ) && ( target->GetId( 1 ) != 0x0E || target->GetId( 2 ) != 0x75 ))
	{
		// Don't allow casting this on locks inside multis
		if( ValidateObject( target->GetMultiObj() ))
		{
			sock->SysMessage( 669 ); // You cannot lock this!
			return true;
		}

		switch( type )
		{
			case IT_CONTAINER:  target->SetType( IT_LOCKEDCONTAINER );	break;
			case IT_DOOR:		target->SetType( IT_LOCKEDDOOR );		break;
			case IT_SPAWNCONT:	target->SetType( IT_LOCKEDSPAWNCONT );	break;
			default:
				Console.Error( "Fallout of switch statement without default. magic.cpp, magiclocktarget()" );
				break;
		}

		// Mark lock as magically locked
		target->SetTempVar( CITV_MOREY, 2, static_cast<UI08>( 0x01 ));

		// Store min/max skill to unlock with magery based on player's magery skill
		target->SetTempVar( CITV_MOREY, 3, static_cast<UI08>(( caster->GetSkill( MAGERY ) / 10 ) / 2 ));
		target->SetTempVar( CITV_MOREY, 4, static_cast<UI08>( caster->GetSkill( MAGERY ) / 10 ));

		target->RemoveFromSight();
		target->Update();

		if( Magic->spells[19].Effect() != INVALIDID )
		{
			Effects->PlaySound( caster, Magic->spells[19].Effect() );
		}

		CMagicStat temp = Magic->spells[19].StaticEffect();
		if( temp.Effect() != INVALIDID )
		{
			auto iCont = target->GetCont();
			if( ValidateObject( iCont ) && iCont->CanBeObjType( OT_CHAR ))
			{
				// If container is a character, play lock FX on character
				Effects->PlayStaticAnimation( iCont, temp.Effect(), temp.Speed(), temp.Loop() );
			}
			else
			{
				// Otherwise play FX directly on object
				Effects->PlayStaticAnimation( target, temp.Effect(), temp.Speed(), temp.Loop() );
			}
		}

		// Make it an effect that expires in between 7 to 50 seconds, depending on caster's Magery skill
		auto magicLockDuration = static_cast<UI16>( floor( caster->GetSkill( MAGERY ) / 100 ) + floor( caster->GetSkill( MAGERY ) / 25 ));
		Effects->TempEffect( caster, target, 50, magicLockDuration, 0, 0 );
	}
	else
	{
		sock->SysMessage( 669 ); // You cannot lock this!
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CalculateMagicPoisonStrength()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate strength of magic poison based on caster and target
//o------------------------------------------------------------------------------------------------o
UI08 CalculateMagicPoisonStrength( CChar *caster, CChar *target, UI16 skillVal, bool checkDistance, UI08 spellCircle )
{
	// Default to Lesser Poison if caster is further than 2 tiles away from target
	UI08 poisonStrength = 1;

	// Check if caster is within 2 tiles of target
	if( !checkDistance || GetDist( caster, target ) <= 2 )
	{
		// Calculate poison strength based on average of caster's Magery and Poisoning skills
		if( skillVal == 0 )
		{
			skillVal = static_cast<UI16>(( caster->GetSkill( MAGERY ) + caster->GetSkill( POISONING )) / 2 );
		}
		if( skillVal >= 1000 ) // GM in both skills
		{
			// 5% chance of Deadly Poison if spellCircle is lower than 5, or always Deadly if spellCircle is 5
			if( spellCircle == 5 || RandomNum( 1, 100 ) <= 5 ) 
			{
				poisonStrength = 4; // Deadly Poison
			}
			else
			{
				poisonStrength = 3; // Greater Poison
			}
		}
		else if( skillVal > 701 )
		{
			poisonStrength = 3; // Greater Poison
		}
		else if( skillVal > 301 )
		{
			poisonStrength = 2; // Regular Poison
		}
	}

	// If target resists spell, reduce strength of poison
	if( Magic->CheckResist( caster, target, 3 ))
	{
		if( poisonStrength > 1 )
		{
			poisonStrength--;
		}
	}

	return poisonStrength;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splPoison()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Poison spell
//o------------------------------------------------------------------------------------------------o
bool splPoison( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	// Calculate poison strength of spell based on caster's skills, range to target and whether target resists
	UI08 poisonStrength = CalculateMagicPoisonStrength( caster, target, 0, true, 3 );

	// Apply poison on target
	target->SetPoisoned( poisonStrength );

	// Set time until poison wears off completely
	target->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( GetPoisonDuration( poisonStrength )));

	// Handle criminal flagging
	if( ValidateObject( caster ) && target->IsInnocent() )
	{
		if( WillResultInCriminal( caster, target ) && ValidateObject( caster ) && !caster->IsGM() && !caster->IsCounselor() )
		{
			MakeCriminal( caster );
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splTelekinesis()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Telekinesis spell
//o------------------------------------------------------------------------------------------------o
bool splTelekinesis( CSocket *sock, CChar *caster, CItem *target, [[maybe_unused]] SI08 curSpell )
{
	if( target->IsContType() )
	{
		if( target->GetTempVar( CITV_MOREZ, 1 ) == 1 )
		{
			target->SetTempVar( CITV_MOREZ, 1, 0 );

			if( Magic->spells[21].Effect() != INVALIDID )
			{
				Effects->PlaySound( caster, Magic->spells[21].Effect() );
			}

			CMagicStat temp = Magic->spells[21].StaticEffect();
			if( temp.Effect() != INVALIDID )
			{
				Effects->PlayStaticAnimation( target, temp.Effect(), temp.Speed(), temp.Loop() );
			}

			Magic->MagicTrap( caster, target );
		}
		else
		{
			sock->SysMessage( 667 ); // This item doesn't seem to be trapped!
		}
	}
	else
	{
		sock->SysMessage( 668 ); // This item cannot be trapped!
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splTeleport()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Teleport spell
//o------------------------------------------------------------------------------------------------o
bool splTeleport( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, [[maybe_unused]] SI08 curSpell )
{
	CMultiObj *m = FindMulti( x, y, z, caster->WorldNumber(), caster->GetInstanceId() );
	if( ValidateObject( m ) && m->GetOwnerObj() != caster && !caster->IsNpc() )
	{
		sock->SysMessage( 670 ); // You can't teleport here!
		return false;
	}
	if( !caster->IsNpc() )
	{
		CTile& tile = Map->SeekTile( sock->GetWord( 0x11 ));
		if(( tile.Name() == "water" ) || tile.CheckFlag( TF_WET ))
		{
			sock->SysMessage( 671 ); //You can't teleport there!
			return false;
		}
		if( tile.CheckFlag( TF_ROOF )) // slanted roof tile!!! naughty naughty
		{
			sock->SysMessage( 672 ); // You cannot teleport there!
			return false;
		}
	}
	if( Weight->IsOverloaded( caster ))
	{
		if( caster->GetMana() >= 20 )
		{
			caster->SetMana( static_cast<SI16>( caster->GetMana() - 20 ));
		}
		else
		{
			sock->SysMessage( 1386 ); // You are overloaded! You can't hold all this weight.
			return false;
		}
	}
	CMagicStat temp = Magic->spells[22].StaticEffect();
	if( temp.Effect() != INVALIDID )
	{
		Effects->PlayStaticAnimation( caster->GetX(), caster->GetY(), caster->GetZ(), temp.Effect(), temp.Speed(), temp.Loop(), false );
		caster->SetLocation( x, y, z );
		Magic->DoStaticEffect( caster, 22 );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splUnlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Unlock spell
//o------------------------------------------------------------------------------------------------o
bool splUnlock( CSocket *sock, CChar *caster, CItem *target, [[maybe_unused]] SI08 curSpell )
{
	if( target->IsDivineLocked() )
	{
		sock->SysMessage( 673 ); // This item has been protected with divine magic.  You cannot unlock this item.
		return false;
	}
	if( target->GetType() == IT_LOCKEDCONTAINER )
	{
		bool canMagicUnlock = true;
		if( target->GetTempVar( CITV_MOREY ) > 0 )
		{
			canMagicUnlock = static_cast<bool>( target->GetTempVar( CITV_MOREY, 2 ));
			if( !canMagicUnlock )
			{
				sock->SysMessage( 9102 ); // That lock cannot be unlocked with magic!
				return false;
			}
		}

		auto minSkill = target->GetTempVar( CITV_MOREY, 3 ) * 10;
		auto maxSkill = target->GetTempVar( CITV_MOREY, 4 ) * 10;
		if( !Skills->CheckSkill( caster, MAGERY, minSkill, ( maxSkill > 0 ? maxSkill : 1000 )))
		{
			sock->SysMessage( 6092 ); // You are not skilled enough to do that.
			return false;
		}

		target->SetType( IT_CONTAINER );
		sock->SysMessage( 674 ); // You unlock the item.
		target->RemoveFromSight();
		target->Update();

		if( Magic->spells[23].Effect() != INVALIDID )
		{
			Effects->PlaySound( caster, Magic->spells[23].Effect() );
		}

		CMagicStat temp = Magic->spells[23].StaticEffect();
		if( temp.Effect() != INVALIDID )
		{
			auto iCont = target->GetCont();
			if( ValidateObject( iCont ) && iCont->CanBeObjType( OT_CHAR ))
			{
				// If container is a character, play lock FX on character
				Effects->PlayStaticAnimation( iCont, temp.Effect(), temp.Speed(), temp.Loop() );
			}
			else
			{
				Effects->PlayStaticAnimation( target, temp.Effect(), temp.Speed(), temp.Loop() );
			}
		}
	}
	else if( target->GetType() == IT_LOCKEDSPAWNCONT )
	{
		bool canMagicUnlock = true;
		if( target->GetTempVar( CITV_MOREY ) > 0 )
		{
			canMagicUnlock = static_cast<bool>(target->GetTempVar( CITV_MOREY, 2 ));
			if( !canMagicUnlock )
			{
				sock->SysMessage( 9102 ); // That lock cannot be unlocked with magic!
				return false;
			}
		}

		auto minSkill = target->GetTempVar( CITV_MOREY, 3 ) * 10;
		auto maxSkill = target->GetTempVar( CITV_MOREY, 4 ) * 10;
		if( !Skills->CheckSkill( caster, MAGERY, minSkill, ( maxSkill > 0 ? maxSkill : 1000 )))
		{
			sock->SysMessage( 6092 ); // You are not skilled enough to do that.
			return false;
		}

		target->SetType( IT_SPAWNCONT );
		sock->SysMessage( 675 ); // You unlock the item.
		target->RemoveFromSight();
		target->Update();

		if( Magic->spells[23].Effect() != INVALIDID )
		{
			Effects->PlaySound( caster, Magic->spells[23].Effect() );
		}

		CMagicStat temp = Magic->spells[23].StaticEffect();
		if( temp.Effect() != INVALIDID )
		{
			Effects->PlayStaticAnimation( target, temp.Effect(), temp.Speed(), temp.Loop() );
		}
	}
	else if( target->GetType() == IT_CONTAINER || target->GetType() == IT_SPAWNCONT || target->GetType() == IT_LOCKEDSPAWNCONT || target->GetType() == IT_TRASHCONT )
	{
		sock->SysMessage( 676 ); // That is not locked.
	}
	else if( target->GetType() == IT_DOOR )
	{
		sock->SysMessage( 937 ); // That cannot be unlocked without a key!
	}
	else if( target->GetType() == IT_LOCKEDDOOR )
	{
		bool canMagicUnlock = true;
		if( target->GetTempVar( CITV_MOREY ) > 0 )
		{
			canMagicUnlock = static_cast<bool>(target->GetTempVar( CITV_MOREY, 2 ));
			if( !canMagicUnlock )
			{
				sock->SysMessage( 9102 ); // That lock cannot be unlocked with magic!
				return false;
			}
		}

		auto minSkill = target->GetTempVar( CITV_MOREY, 3 ) * 10;
		auto maxSkill = target->GetTempVar( CITV_MOREY, 4 ) * 10;
		if( !Skills->CheckSkill( caster, MAGERY, minSkill, ( maxSkill > 0 ? maxSkill : 1000 )))
		{
			sock->SysMessage( 6092 ); // You are not skilled enough to do that.
			return false;
		}

		target->SetType( IT_DOOR );
		sock->SysMessage( 675 ); // You unlock the item.
		target->RemoveFromSight();
		target->Update();

		if( Magic->spells[23].Effect() != INVALIDID )
		{
			Effects->PlaySound( caster, Magic->spells[23].Effect() );
		}

		CMagicStat temp = Magic->spells[23].StaticEffect();
		if( temp.Effect() != INVALIDID )
		{
			Effects->PlayStaticAnimation( target, temp.Effect(), temp.Speed(), temp.Loop() );
		}
	}
	else
	{
		sock->SysMessage( 678 ); // That does not have a lock.
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splWallOfStone()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Wall of Stone spell
//o------------------------------------------------------------------------------------------------o
bool splWallOfStone( [[maybe_unused]] CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	return FieldSpell( caster, 0x0080, x, y, z, fieldDir, curSpell );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ArchCureStub()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Arch Cure spell to an individual target
//o------------------------------------------------------------------------------------------------o
void ArchCureStub( [[maybe_unused]] CChar *caster, CChar *target, [[maybe_unused]] SI08 curSpell, [[maybe_unused]] SI08 targCount )
{
	target->SetPoisoned( 0 );
	target->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->GetUICurrentTime() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splArchCure()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Arch Cure as an area affect spell
//o------------------------------------------------------------------------------------------------o
bool splArchCure( CSocket *sock, CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &ArchCureStub, curSpell );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ArchProtectionStub()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Arch Protection spell to an individual target
//o------------------------------------------------------------------------------------------------o
void ArchProtectionStub( CChar *caster, CChar *target, [[maybe_unused]] SI08 curSpell, [[maybe_unused]] SI08 targCount )
{
	Magic->PlaySound( target, 26 );
	Magic->DoStaticEffect( target, 15 );	// protection
	Effects->TempEffect( caster, target, 21, caster->GetSkill( MAGERY ) / 10, 0, 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splArchProtection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Arch Protection as an area affect spell
//o------------------------------------------------------------------------------------------------o
bool splArchProtection( CSocket *sock, CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &ArchProtectionStub, curSpell );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splCurse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Curse spell
//o------------------------------------------------------------------------------------------------o
bool splCurse( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	SI32 j = caster->GetSkill( MAGERY ) / 100;
	Effects->TempEffect( caster, target, 12, j, j, j);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splFireField()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Fire Field as a field spell
//o------------------------------------------------------------------------------------------------o
bool splFireField( [[maybe_unused]] CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( fieldDir == 0 )
	{
		return FieldSpell( caster, 0x398C, x, y, z, fieldDir, curSpell );
	}
	else
	{
		return FieldSpell( caster, 0x3996, x, y, z, fieldDir, curSpell );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splGreaterHeal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Greater Heal spell
//o------------------------------------------------------------------------------------------------o
bool splGreaterHeal( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, SI08 curSpell )
{
	SI32 srcHealth		= target->GetHP();
	SI32 baseHealing	= Magic->spells[curSpell].BaseDmg();
	SI32 j				= caster->GetSkill( MAGERY ) / 30 + HalfRandomNum( baseHealing );

	target->Heal( j, caster );
	Magic->SubtractHealth( caster, std::min( target->GetStrength(), static_cast<SI16>( srcHealth + j )), 29 );
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splLightning()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Lightning spell
//o------------------------------------------------------------------------------------------------o
bool splLightning( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Effects->TempEffect( src, target, 31, spellDamage, 0, 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splManaDrain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mana Drain spell
//o------------------------------------------------------------------------------------------------o
bool splManaDrain( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	if( !Magic->CheckResist( caster, target, 4 ))
	{
		target->SetMana( target->GetMana() - caster->GetSkill( MAGERY )/35 );
		if( target->GetMana() < 0 )
		{
			target->SetMana( 0 );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splRecall()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Recall spell
//o------------------------------------------------------------------------------------------------o
auto splRecall( CSocket *sock, CChar *caster, CItem *i, [[maybe_unused]] SI08 curSpell ) -> bool
{
	// No recall if too heavy, GMs excempt
	if( Weight->IsOverloaded( caster ) && !cwmWorldState->ServerData()->TravelSpellsWhileOverweight() && ( !caster->IsCounselor() && !caster->IsGM() ))
	{
		sock->SysMessage( 680 ); // You are too heavy to do that!
		sock->SysMessage( 681 ); // You feel drained from the attempt.
		return false;
	}
	else if( i->GetType() == 7 )
	{
		// Player recalled off a key
		CMultiObj *shipMulti = CalcMultiFromSer( i->GetTempVar( CITV_MORE ));
		if( ValidateObject( shipMulti ) && shipMulti->CanBeObjType( OT_BOAT ))
		{
			if(( shipMulti->WorldNumber() == caster->WorldNumber() || cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() ) && shipMulti->GetInstanceId() == caster->GetInstanceId() )
			{
				if( shipMulti->WorldNumber() == 0 && caster->WorldNumber() != 0 && cwmWorldState->ServerData()->YoungPlayerSystem() && caster->GetAccount().wFlags.test( AB_FLAGS_YOUNG ) )
				{
					sock->SysMessage( 18733 ); //  You decide against traveling to Felucca while you are still young.
					return false;
				}

				// Teleport player's followers too
				auto myFollowers = caster->GetFollowerList();
				for( const auto &myFollower : myFollowers->collection() )
				{
					if( ValidateObject( myFollower ))
					{
						if( !myFollower->GetMounted() && myFollower->GetOwnerObj() == caster )
						{
							// Only teleport followers if they are set to actually following owner & are within range
							if( myFollower->GetNpcWander() == WT_FOLLOW && ObjInOldRange( caster, myFollower, DIST_CMDRANGE ))
							{
								myFollower->SetLocation( shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3, shipMulti->WorldNumber(), shipMulti->GetInstanceId() );
							}
						}
					}
				}

				// Teleport player
				caster->SetLocation( shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3, shipMulti->WorldNumber(), shipMulti->GetInstanceId() );
				return true;
			}
			else
			{
				sock->SysMessage( 2063 ); // You are unable to recall to your ship - it might be in another world!
				return false;
			}
		}
		else
		{
			sock->SysMessage( 2065 ); // You can only recall off of recall runes or valid ship keys!
			return false;
		}
	}
	else
	{
		// Check if rune was marked in a multi - if so, try to take user directly there
		TAGMAPOBJECT runeMore = i->GetTag( "multiSerial" );
		if( runeMore.m_StringValue != "" )
		{
			SERIAL mSerial = oldstrutil::value<SERIAL>( runeMore.m_StringValue );
			if( mSerial != 0 && mSerial != INVALIDSERIAL )
			{
				CMultiObj *shipMulti = CalcMultiFromSer( mSerial );
				if( ValidateObject( shipMulti ) && shipMulti->CanBeObjType( OT_BOAT ))
				{
					if(( shipMulti->WorldNumber() == caster->WorldNumber() || cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() ) && shipMulti->GetInstanceId() == caster->GetInstanceId() )
					{
						if( shipMulti->WorldNumber() == 0 && caster->WorldNumber() != 0 && cwmWorldState->ServerData()->YoungPlayerSystem() && caster->GetAccount().wFlags.test( AB_FLAGS_YOUNG ) )
						{
							sock->SysMessage( 18733 ); //  You decide against traveling to Felucca while you are still young.
							return false;
						}

						// Teleport player's followers too
						auto myFollowers = caster->GetFollowerList();
						for( const auto &myFollower : myFollowers->collection() )
						{
							if( ValidateObject( myFollower ))
							{
								if( !myFollower->GetMounted() && myFollower->GetOwnerObj() == caster )
								{
									if( myFollower->GetNpcWander() == WT_FOLLOW && ObjInOldRange( caster, myFollower, DIST_CMDRANGE ))
									{
										myFollower->SetLocation( shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3, shipMulti->WorldNumber(), shipMulti->GetInstanceId() );
									}
								}
							}
						}

						caster->SetLocation( shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3, shipMulti->WorldNumber(), shipMulti->GetInstanceId() );
						return true;
					}
					else
					{
						sock->SysMessage( 2063 ); // You are unable to recall to your ship - it might be in another world!
						return false;
					}
				}
			}

			sock->SysMessage( 2062 ); // Unable to locate ship - it might have been dry-docked... or sunk!
			return false;
		}
		else if( i->GetTempVar( CITV_MOREX ) == 0 && i->GetTempVar( CITV_MOREY ) == 0 )
		{
			sock->SysMessage( 679 ); // That rune has not been marked yet!
			return false;
		}
		else
		{
			UI08 worldNum = static_cast<UI08>( i->GetTempVar( CITV_MORE ));
			if( !Map->MapExists( worldNum ))
			{
				worldNum = caster->WorldNumber();
			}

			SI16 targLocX = static_cast<SI16>( i->GetTempVar( CITV_MOREX ));
			SI16 targLocY = static_cast<SI16>( i->GetTempVar( CITV_MOREY ));
			SI08 targLocZ = static_cast<SI08>( i->GetTempVar( CITV_MOREZ ));
			UI16 instanceId = static_cast<UI16>( i->GetTempVar( CITV_MORE0 ));
			if( worldNum != caster->WorldNumber() )
			{
				if( cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() )
				{
					if( worldNum == 0 && caster->WorldNumber() != 0 && cwmWorldState->ServerData()->YoungPlayerSystem() && caster->GetAccount().wFlags.test( AB_FLAGS_YOUNG ) )
					{
						sock->SysMessage( 18733 ); //  You decide against traveling to Felucca while you are still young.
						return false;
					}

					// Teleport player's followers too
					auto myFollowers = caster->GetFollowerList();
					for( const auto &myFollower : myFollowers->collection() )
					{
						if( ValidateObject( myFollower ))
						{
							if( !myFollower->GetMounted() && myFollower->GetOwnerObj() == caster )
							{
								if( myFollower->GetNpcWander() == WT_FOLLOW && ObjInOldRange( caster, myFollower, DIST_CMDRANGE ))
								{
									myFollower->SetLocation( targLocX, targLocY, targLocZ, worldNum, instanceId );
								}
							}
						}
					}

					// Teleport the player
					caster->SetLocation( targLocX, targLocY, targLocZ, worldNum, instanceId );
					SendMapChange( caster->WorldNumber(), sock, false );
				}
				else
				{
					sock->SysMessage( 2061 ); // Travelling between worlds using Recall or Gate spells is not possible.
					return false;
				}
			}
			else
			{
				// Teleport player's followers too
				auto myFollowers = caster->GetFollowerList();
				for( const auto &myFollower : myFollowers->collection() )
				{
					if( ValidateObject( myFollower ))
					{
						if( !myFollower->GetMounted() && myFollower->GetOwnerObj() == caster )
						{
							if( myFollower->GetNpcWander() == WT_FOLLOW && ObjInOldRange( caster, myFollower, DIST_CMDRANGE ))
							{
								myFollower->SetLocation( targLocX, targLocY, targLocZ, worldNum, instanceId );
							}
						}
					}
				}

				// Teleport the player
				caster->SetLocation( targLocX, targLocY, targLocZ, worldNum, instanceId );
			}
			sock->SysMessage( 682 ); // You have recalled from the rune.
			return true;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splBladeSpirits()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Blade Spirits spell (summons a Blade Spirit)
//o------------------------------------------------------------------------------------------------o
auto splBladeSpirits( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, [[maybe_unused]] SI08 curSpell ) -> bool
{
	Magic->SummonMonster( sock, caster, 6, x, y, z );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splDispelField()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Dispel Field spell
//o------------------------------------------------------------------------------------------------o
bool splDispelField( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	CItem *i = CalcItemObjFromSer( sock->GetDWord( 7 ));
	if( ValidateObject( i ))
	{
		if( LineOfSight( sock, caster, i->GetX(), i->GetY(), ( i->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) || caster->IsGM() )
		{
			if( i->IsDecayable() && i->IsDispellable() )
			{
				std::vector<UI16> scriptTriggers = i->GetScriptTriggers();
				for( auto scriptTrig : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( scriptTrig );
					if( toExecute != nullptr )
					{
						if( toExecute->OnDispel( i ) == 1 ) // if it exists and we don't want hard code, return
							return false;
					}
				}

				i->Delete();
			}
			Effects->PlaySound( caster, 0x0201 );
		}
		else
		{
			sock->SysMessage( 683 ); // There seems to be something in the way.
		}
	}
	return true;
}
void SetRandomName( CBaseObject *s, const std::string& namelist );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	splIncognito()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Incognito spell
//o------------------------------------------------------------------------------------------------o
bool splIncognito( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	if( caster->IsIncognito() )
	{
		sock->SysMessage( 1636 ); // You cannot go incognito while already incognito
		return false;
	}
	if( caster->IsPolymorphed() )
	{
		sock->SysMessage( 1674 ); // You can't do that while polymorphed.
		return false;
	}
	if( caster->IsDisguised() )
	{
		sock->SysMessage( 9232 ); // You can't do that while disguised.
		return false;
	}
	if( caster->GetId() == 0x00b8 || caster->GetId() == 0x00b7 ) // Savage Kin Paint transformation
	{
		sock->SysMessage( 1672 ); // You cannot use incognito while wearing body paint
		return false;
	}
	// ------ SEX ------
	caster->SetOrgId( caster->GetId() );
	UI08 randomGender = RandomNum( 0, 3 );
	switch( randomGender )
	{
		case 0:	caster->SetId( 0x0190 ); break;	// male, human
		case 1:	caster->SetId( 0x0191 ); break;	// female, human
		case 2:	caster->SetId( 0x025D ); break;	// male, elf
		case 3:	caster->SetId( 0x025E ); break;	// female, elf
	}

	// ------ NAME -----
	caster->SetOrgName( caster->GetName() );
	switch( randomGender )
	{
		case 0:	SetRandomName( caster, "1" );	break; // get a name from human male list
		case 1:	SetRandomName( caster, "2" );	break; // get a name from human female list
		case 2:	SetRandomName( caster, "3" );	break; // get a name from elf male list
		case 3:	SetRandomName( caster, "4" );	break; // get a name from elf female list
	}

	SI32 color	= RandomNum( 0x044E, 0x047D );
	CItem *j	= caster->GetItemAtLayer( IL_HAIR );
	if( ValidateObject( j ))
	{
		caster->SetHairColour( j->GetColour() );
		caster->SetHairStyle( j->GetId() );
		SI32 rNum = RandomNum( 0, 9 );
		switch( rNum )
		{
			case 0:
			case 1:
			case 2:
				j->SetId( 0x3B + rNum, 2 );
				break;
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
				j->SetId( 0x44 + ( rNum - 3 ), 2 );
				break;
		}
		j->SetColour( color );
	}

	// Handle facial hair
	j = caster->GetItemAtLayer( IL_FACIALHAIR );
	if( ValidateObject( j ))
	{
		if( randomGender == 0 || randomGender == 2 )
		{
			caster->SetBeardColour( j->GetColour() );
			caster->SetBeardStyle( j->GetId() );
			SI32 rNum2 = RandomNum( 0, 6 );
			switch( rNum2 )
			{
				case 0:
				case 1:
				case 2:
				case 3:
					j->SetId( 0x3E + rNum2, 2 );
					break;
				case 4:
				case 5:
				case 6:
					j->SetId( 0x4B + ( rNum2 - 4 ), 2 );
					break;
			}
			j->SetColour( color );
		}
		else
		{
			// Hide the facial hair for female characters
			j->SetVisible( VT_PERMHIDDEN );
		}
	}

	//only refresh once
	caster->SendWornItems( sock );
	Effects->PlaySound( caster, 0x0203 );
	Effects->TempEffect( caster, caster, 19, 0, 0, 0 );
	caster->IsIncognito( true );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMagicReflection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Magic Reflection spell
//o------------------------------------------------------------------------------------------------o
bool splMagicReflection( [[maybe_unused]] CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	caster->SetTempReflected( true );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMindBlast()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mind Blast spell
//o------------------------------------------------------------------------------------------------o
bool splMindBlast( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	SI16 baseDamage = 0;

	SI16 targetStats = target->GetIntelligence() + target->GetStrength() + target->GetDexterity();
	SI16 casterStats = caster->GetIntelligence() + caster->GetStrength() + caster->GetDexterity();

	//If target's int makes out a larger percentage of the total stats than the caster's, backfire spell onto caster
	if(( target->GetIntelligence() * 100 ) / targetStats > ( caster->GetIntelligence() * 100 ) / casterStats )
	{
		target = src;
	}

	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	baseDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = baseDamage;
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	//Damage should not exceed 60% of target maxHp
	if( spellDamage > ( target->GetMaxHP() * 0.6 ))
	{
		spellDamage = static_cast<SI16>( std::round( target->GetMaxHP() * 0.6 ));
	}

	//Damage should not exceed basedamage from DFN + 20%
	if( spellDamage > static_cast<SI16>( static_cast<R64>( baseDamage ) * 1.20 ))
	{
		spellDamage = static_cast<SI16>( static_cast<R64>( baseDamage ) * 1.20 );
	}

	Effects->TempEffect( src, target, 32, spellDamage, 0, 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splParalyze()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Paralyze spell
//o------------------------------------------------------------------------------------------------o
bool splParalyze( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	if( !Magic->CheckResist( caster, target, 7 ))
	{
		Effects->TempEffect( caster, target, 1, 0, 0, 0 );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splPoisonField()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Poison Field as a field spell
//o------------------------------------------------------------------------------------------------o
bool splPoisonField( [[maybe_unused]] CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( fieldDir == 0 )
	{
		return FieldSpell( caster, 0x3915, x, y, z, fieldDir, curSpell );
	}
	else
	{
		return FieldSpell( caster, 0x3921, x, y, z, fieldDir, curSpell );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splSummonCreature()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Creature spell (summons a creature)
//o------------------------------------------------------------------------------------------------o
bool splSummonCreature( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, [[maybe_unused]] SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 380 )
	{
		return false;
	}
	else
	{
		Magic->SummonMonster( sock, caster, 0, x, y, z );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splDispel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Dispel spell
//o------------------------------------------------------------------------------------------------o
bool splDispel( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	if( target->IsDispellable() && !target->IsGM() )
	{
		//Effects->PlayStaticAnimation( target, 0x372A, 0x09, 0x06 );		// why the specifics here?
		if( target->IsNpc() )
		{
			std::vector<UI16> scriptTriggers = target->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					if( toExecute->OnDispel( target ) == 1 ) // if it exists and we don't want hard code, return
						return false;
				}
			}

			// Base chance of dispelling on Dispel caster's Magery skill vs summoner's magicresistance
			UI16 casterMagery = caster->GetSkill( MAGERY );
			UI16 targetResist = 0;
			targetResist = target->GetSkill( MAGICRESISTANCE );

			// If creature was summoned, use highest of creature's resist and summoner's resist
			if( target->GetOwner() != INVALIDSERIAL )
			{
				targetResist = std::max( targetResist, target->GetOwnerObj()->GetSkill( MAGICRESISTANCE ));
			}

			UI16 dispelChance = std::max( static_cast<UI16>( 950 ), ( static_cast<UI16>( std::max( 0.0, static_cast<double>( std::ceil( static_cast<double>( 500 - ( targetResist - casterMagery )) / 1.5 ))))));
			if( dispelChance > RandomNum( 0, 1000 ))
			{
				// Dispel succeeded!
				if( Magic->spells[41].Effect() != INVALIDID )
				{
					Effects->PlaySound( target, Magic->spells[41].Effect() );
				}

				CMagicStat temp = Magic->spells[41].StaticEffect();
				if( temp.Effect() != INVALIDID )
				{
					// Play static effect at target's location, since target gets destroyed
					Effects->PlayStaticAnimation( target->GetX(), target->GetY(), target->GetZ(), temp.Effect(), temp.Speed(), temp.Loop(), false );
				}

				target->Delete();
			}
			else if( caster->GetSocket() != nullptr )
			{
				// Dispel resisted!
				caster->GetSocket()->SysMessage( 2761 ); // The creature resisted the attempt to dispel it!
			}
		}
		else
		{
			HandleDeath( target, nullptr );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splEnergyBolt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Energy Bolt spell
//o------------------------------------------------------------------------------------------------o
bool splEnergyBolt( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Effects->TempEffect( src, target, 33, spellDamage, 0, 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splExplosion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Explosion spell. Delay configurable in UOX.INI
//o------------------------------------------------------------------------------------------------o
bool splExplosion( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Effects->TempEffect( src, target, 27, spellDamage, 0, 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splInvisibility()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Invisibility spell
//o------------------------------------------------------------------------------------------------o
bool splInvisibility( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	target->SetVisible( VT_INVISIBLE );
	target->SetTimer( tCHAR_INVIS, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_INVISIBILITY ));
	if( target->IsMurderer() )
	{
		MakeCriminal( caster );
	}

	//Stop autodefending against targets in combat once you turn invisible
	if( !target->IsNpc() )
	{
		Combat->InvalidateAttacker( target );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMark()
//|	Date		-	09/22/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mark spell
//|
//|	Changes		-	09/22/2002	-	Fixed marking of locked down runes..
//|									(shouldn't be possible)
//o------------------------------------------------------------------------------------------------o
bool splMark( CSocket *sock, CChar *caster, CItem *i, [[maybe_unused]] SI08 curSpell )
{
	if( i->IsLockedDown() )
	{
		sock->SysMessage( 774 ); // That is locked down and you cannot use it.
		return false;
	}

	if( i->GetType() == IT_RUNEBOOK )
	{
		sock->SysMessage( 710 ); // That item is not a recall rune.
		return false;
	}

	bool markedInMulti = false;
	CMultiObj *multi = caster->GetMultiObj();
	if( ValidateObject( multi ))
	{
		if( !multi->IsOnOwnerList( caster ))
		{
			sock->SysMessage( 9034 ); // Marking a rune in property you don't own is not possible.
			return false;
		}
		else if( !cwmWorldState->ServerData()->MarkRunesInMultis() )
		{
			sock->SysMessage( 9035 ); // "Marking a rune inside one's property is not permitted.
			return false;
		}
		else
		{
			// Let's allow marking the rune in the multi, and store multi's serial in a tag
			auto mSerial = multi->GetSerial();
			TAGMAPOBJECT tagObject;
			tagObject.m_Destroy = false;
			tagObject.m_StringValue = std::to_string( mSerial );
			tagObject.m_IntValue = static_cast<SI32>( tagObject.m_StringValue.size() );
			tagObject.m_ObjectType = TAGMAP_TYPE_STRING;
			i->SetTag( "multiSerial", tagObject );
			markedInMulti = true;

			std::string tempRuneName = oldstrutil::format( Dictionary->GetEntry( 684 ), multi->GetNameRequest( caster, NRS_SYSTEM ).c_str() ); // A recall rune for %s.
			if( tempRuneName.length() > 0 )
			{
				i->SetName( tempRuneName );
			}
		}
	}

	if( !markedInMulti )
	{
		i->SetTempVar( CITV_MOREX, caster->GetX() );
		i->SetTempVar( CITV_MOREY, caster->GetY() );
		i->SetTempVar( CITV_MOREZ, caster->GetZ() );
		i->SetTempVar( CITV_MORE, caster->WorldNumber() );
		i->SetTempVar( CITV_MORE0, caster->GetInstanceId() );

		std::string tempitemname;

		if( caster->GetRegion()->GetName()[0] != 0 )
		{
			tempitemname = oldstrutil::format( Dictionary->GetEntry( 684 ), caster->GetRegion()->GetName().c_str() ); // A recall rune for %s.
		}
		else
		{
			tempitemname = Dictionary->GetEntry( 685 ); // A recall rune for An Unknown Location.
		}
		i->SetName( tempitemname );

		// Change ID of rune from a blank, unmarked rune (0x1f17) to a marked one (0x1f14)
		i->SetId( 0x1f14 );
	}

	sock->SysMessage( 686 ); // Recall rune marked.
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MassCurseStub()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mass Curse spell to an individual target
//o------------------------------------------------------------------------------------------------o
void MassCurseStub( CChar *caster, CChar *target, [[maybe_unused]] SI08 curSpell, [[maybe_unused]] SI08 targCount )
{
	if( target->IsNpc() && target->GetNpcAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed

	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed

	if( target->IsNpc() && target == caster )
		return; // Don't let NPC curse themselves

	if( target->IsNpc() && caster->IsNpc() )
	{
		if( target->GetNPCFlag() == caster->GetNPCFlag() )
			return; // Don't let NPCs curse other NPCs with same NPC Flag (innocent, evil, neutral)

		switch( caster->GetNpcAiType() )
		{
			case 2: // AI_EVIL
			case 11: // AI_EVIL_CASTER
			{
				if( target->GetNpcAiType() == AI_EVIL || target->GetNpcAiType() == AI_EVIL_CASTER )
					return; // Don't curse NPCs with similar AI types
				break;
			}
			case 5: // AI_FIGHTER
			case 10: // AI_CASTER
			{
				if( target->GetNpcAiType() == AI_FIGHTER || target->GetNpcAiType() == AI_CASTER )
					return; // Don't curse NPCs with similar AI types
				break;
			}
			default:
				break;
		}
	}

	SI32 j;
	if( target->IsNpc() )
	{
		Combat->AttackTarget( caster, target );
	}
	Effects->PlayStaticAnimation( target, 0x374A, 0, 15 );
	Effects->PlaySound( target, 0x01FC );
	if( Magic->CheckResist( caster, target, 6 ))
	{
		j = caster->GetSkill( MAGERY ) / 200;
	}
	else
	{
		j = caster->GetSkill( MAGERY ) / 75;
	}
	Effects->TempEffect( caster, target, 12, j, j, j );

}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMassCurse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Mass Curse as an area affect spell
//o------------------------------------------------------------------------------------------------o
bool splMassCurse( CSocket *sock, CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &MassCurseStub, curSpell );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splParalyzeField()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Paralyze Field as a field spell
//o------------------------------------------------------------------------------------------------o
bool splParalyzeField( [[maybe_unused]] CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( fieldDir == 0 )
	{
		return FieldSpell( caster, 0x3967, x, y, z, fieldDir, curSpell );
	}
	else
	{
		return FieldSpell( caster, 0x3979, x, y, z, fieldDir, curSpell );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splReveal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Reveal spell
//o------------------------------------------------------------------------------------------------o
auto splReveal( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, [[maybe_unused]] SI08 curSpell ) -> bool
{
	if( LineOfSight( sock, caster, x, y, ( z + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) || caster->IsGM() )
	{
		UI16 j = caster->GetSkill( MAGERY );
		SI32 revealRange = ((( j - 261 ) * ( 15 )) / 739 ) + 5;
		//If the caster has a Magery of 26.1 (min to cast reveal w/ scroll), revealRange  radius is
		//5 tiles, if magery is maxed out at 100.0 (except for gms I suppose), revealRange is 20

		for( auto &MapArea : MapRegion->PopulateList( caster ))
		{
			if( MapArea )
			{
				auto regChars = MapArea->GetCharList();
				for( const auto &tempChar : regChars->collection() )
				{
					if( ValidateObject( tempChar ) && tempChar->GetInstanceId() == caster->GetInstanceId() )
					{
						if( tempChar->GetVisible() == VT_TEMPHIDDEN || tempChar->GetVisible() == VT_INVISIBLE )
						{
							Point3_st difference = ( tempChar->GetLocation() - Point3_st( x, y, z ));
							if( difference.MagSquared() <= ( revealRange * revealRange )) // char to reveal is within radius or range
							{
								tempChar->ExposeToView();
								CMagicStat temp = Magic->spells[48].StaticEffect();
								if( temp.Effect() != INVALIDID )
								{
									Effects->PlayStaticAnimation( tempChar, temp.Effect(), temp.Speed(), temp.Loop() );
								}
							}
						}
					}
				}
			}
		}
		if( Magic->spells[48].Effect() != INVALIDID )
		{
			Effects->PlaySound( sock, Magic->spells[48].Effect(), true );
		}
	}
	else
	{
		sock->SysMessage( 687 ); // You would like to see if anything was there, but there is too much stuff in the way.
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ChainLightningStub()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Chain Lightning spell to an individual target
//o------------------------------------------------------------------------------------------------o
void ChainLightningStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	if( target->IsNpc() && target->GetNpcAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed

	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed

	if( target->IsNpc() && target == caster )
		return; // Don't let NPC hit themselves with damaging spell

	if( target->IsNpc() && caster->IsNpc() )
	{
		if( target->GetNPCFlag() == caster->GetNPCFlag() )
			return; // Don't let NPCs damage other NPCs with same NPC Flag (innocent, evil, neutral)

		switch( caster->GetNpcAiType() )
		{
			case 2: // AI_EVIL
			case 11: // AI_EVIL_CASTER
			{
				if( target->GetNpcAiType() == AI_EVIL || target->GetNpcAiType() == AI_EVIL_CASTER )
					return; // Don't damage NPCs with similar AI types
				break;
			}
			case 5: // AI_FIGHTER
			case 10: // AI_CASTER
			{
				if( target->GetNpcAiType() == AI_FIGHTER || target->GetNpcAiType() == AI_CASTER )
					return; // Don't damage NPCs with similar AI types
				break;
			}
			default:
				break;
		}
	}

	if( target->IsNpc() )
	{
		Combat->AttackTarget( target, caster );
	}
	/*Effects->PlaySound( caster, 0x0029 );*/
	[[maybe_unused]] CChar *def = nullptr;
	if( Magic->CheckMagicReflect( target ))
	{
		def = caster;
	}
	else
	{
		def = target;
	}

	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	//If targetCOunt is > 1, do equal damage to two targets, split damage on more targets
	if( targCount > 1 )
	{
		spellDamage = ( spellDamage * 2 ) / targCount;
	}

	Effects->TempEffect( caster, target, 34, spellDamage, 0, 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splChainLightning()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Chain Lightning as an area affect spell
//o------------------------------------------------------------------------------------------------o
bool splChainLightning( CSocket *sock, CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &ChainLightningStub, curSpell );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splEnergyField()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Energy Field spell as a field spell
//o------------------------------------------------------------------------------------------------o
bool splEnergyField( [[maybe_unused]] CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( fieldDir == 0 )
	{
		return FieldSpell( caster, 0x3946, x, y, z, fieldDir, curSpell );
	}
	else
	{
		return FieldSpell( caster, 0x3956, x, y, z, fieldDir, curSpell );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splFlameStrike()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Flame Strike spell on target
//o------------------------------------------------------------------------------------------------o
bool splFlameStrike( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Effects->TempEffect( caster, target, 35, spellDamage, 0, 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splGateTravel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Gate Travel spell (opens gate from recall rune)
//o------------------------------------------------------------------------------------------------o
bool splGateTravel( CSocket *sock, CChar *caster, CItem *i, [[maybe_unused]] SI08 curSpell )
{
	// No recall if too heavy, GM's excempt
	if( Weight->IsOverloaded( caster ) && !cwmWorldState->ServerData()->TravelSpellsWhileOverweight() && ( !caster->IsCounselor() && !caster->IsGM() ))
	{
		sock->SysMessage( 680 ); // You are too heavy to do that!
		sock->SysMessage( 681 ); // You feel drained from the attempt.
		return false;
	}
	else
	{
		// Check if rune was marked in a multi - if so, try to take user directly there
		TAGMAPOBJECT runeMore = i->GetTag( "multiSerial" );
		if( runeMore.m_StringValue != "" )
		{
			SERIAL mSerial = oldstrutil::value<SERIAL>( runeMore.m_StringValue );
			if( mSerial != 0 && mSerial != INVALIDSERIAL )
			{
				CMultiObj *shipMulti = CalcMultiFromSer( mSerial );
				if( ValidateObject( shipMulti ) && shipMulti->CanBeObjType( OT_BOAT ))
				{
					if(( shipMulti->WorldNumber() == caster->WorldNumber() || cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() ) && shipMulti->GetInstanceId() == caster->GetInstanceId() )
					{
						if( shipMulti->WorldNumber() == 0 && caster->WorldNumber() != 0 && cwmWorldState->ServerData()->YoungPlayerSystem() && caster->GetAccount().wFlags.test( AB_FLAGS_YOUNG ) )
						{
							sock->SysMessage( 18733 ); //  You decide against traveling to Felucca while you are still young.
							return false;
						}

						caster->SetLocation( shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3 );
						SpawnGate( caster, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ(), caster->WorldNumber(), shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3, shipMulti->WorldNumber(), shipMulti->GetInstanceId() );
						return true;
					}
					else
					{
						sock->SysMessage( 2064 ); // You are unable to open a gate to your ship - it might be in another world!
						return false;
					}
				}
			}

			sock->SysMessage( 2062 ); // Unable to locate ship - it might have been dry-docked... or sunk!
			return false;
		}
		else if( i->GetTempVar( CITV_MOREX ) == 0 && i->GetTempVar( CITV_MOREY ) == 0 )
		{
			sock->SysMessage( 679 ); // That rune has not been marked yet!
			return false;
		}
		else
		{
			UI08 worldNum = static_cast<UI08>( i->GetTempVar( CITV_MORE ));
			if( !Map->MapExists( worldNum ))
			{
				worldNum = caster->WorldNumber();
			}
			if( caster->WorldNumber() != worldNum && !cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() )
			{
				sock->SysMessage( 2061 ); // Travelling between worlds using Recall or Gate spells is not possible.
				return false;
			}

			if( worldNum == 0 && caster->WorldNumber() != 0 && cwmWorldState->ServerData()->YoungPlayerSystem() && caster->GetAccount().wFlags.test( AB_FLAGS_YOUNG ) )
			{
				sock->SysMessage( 18733 ); //  You decide against traveling to Felucca while you are still young.
				return false;
			}

			SI16 targLocX = static_cast<SI16>( i->GetTempVar( CITV_MOREX ));
			SI16 targLocY = static_cast<SI16>( i->GetTempVar( CITV_MOREY ));
			SI08 targLocZ = static_cast<SI08>( i->GetTempVar( CITV_MOREZ ));
			UI16 instanceId = static_cast<UI16>( i->GetTempVar( CITV_MORE0 ));
			SpawnGate( caster, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ(), caster->WorldNumber(), targLocX, targLocY, targLocZ, worldNum, instanceId );
			return true;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splManaVampire()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mana Vampire spell to target
//o------------------------------------------------------------------------------------------------o
bool splManaVampire( CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	if( !Magic->CheckResist( caster, target, 7 ))
	{
		if( target->GetMana() < 40 )
		{
			caster->SetMana( caster->GetMana() + target->GetMana() );
			target->SetMana( 0 );
		}
		else
		{
			target->SetMana( target->GetMana() - 40 );
			caster->SetMana( caster->GetMana() + 40 );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MassDispelStub()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mass Dispel spell to an individual target
//o------------------------------------------------------------------------------------------------o
void MassDispelStub( CChar *caster, CChar *target, [[maybe_unused]] SI08 curSpell, [[maybe_unused]] SI08 targCount )
{
	if( target->IsDispellable() )
	{
		//Effects->PlayStaticAnimation( target, 0x372A, 0x09, 0x06 );		// why the specifics here?
		if( target->IsNpc() )
		{
			std::vector<UI16> scriptTriggers = target->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					if( toExecute->OnDispel( target ) == 1 ) // if it exists and we don't want hard code, return
						return;
				}
			}

			// Base chance of dispelling on Dispel caster's Magery skill vs summoner's magicresistance
			UI16 casterMagery = caster->GetSkill( MAGERY );
			UI16 targetResist = 0;
			targetResist = target->GetSkill( MAGICRESISTANCE );

			// If creature was summoned, use highest of creature's resist and summoner's resist
			if( target->GetOwner() != INVALIDSERIAL )
			{
				targetResist = std::max( targetResist, target->GetOwnerObj()->GetSkill( MAGICRESISTANCE ));
			}

			UI16 dispelChance = static_cast<UI16>( std::max( static_cast<UI16>( 950 ),  static_cast<UI16>( std::ceil(( 500 - ( targetResist - casterMagery )) / 1.5 ))));
			if( dispelChance > RandomNum( 0, 1000 ))
			{
				// Dispel succeeded!
				if( Magic->spells[41].Effect() != INVALIDID )
				{
					Effects->PlaySound( target, Magic->spells[41].Effect() );
				}

				CMagicStat temp = Magic->spells[41].StaticEffect();
				if( temp.Effect() != INVALIDID )
				{
					// Play static effect at target's location, since target gets destroyed
					Effects->PlayStaticAnimation( target->GetX(), target->GetY(), target->GetZ(), temp.Effect(), temp.Speed(), temp.Loop(), false );
				}

				target->Delete();
			}
			else if( caster->GetSocket() != nullptr )
			{
				// Dispel resisted!
				caster->GetSocket()->SysMessage( 2761 ); // The creature resisted the attempt to dispel it!
			}
		}
		else
		{
			HandleDeath( target, nullptr );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMassDispel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Mass Dispel spell as an area affect spell
//o------------------------------------------------------------------------------------------------o
bool splMassDispel( CSocket *sock, CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &MassDispelStub, curSpell );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MeteorSwarmStub()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Meteor Swarm spell to an individual target
//o------------------------------------------------------------------------------------------------o
void MeteorSwarmStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	if( target->IsNpc() && target->GetNpcAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed

	if( target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed

	if( target->IsNpc() && target == caster )
		return; // Don't let NPC hit themselves with damaging spell

	if( target->IsNpc() && caster->IsNpc() )
	{
		if( target->GetNPCFlag() == caster->GetNPCFlag() )
			return; // Don't let NPCs damage other NPCs with same NPC Flag (innocent, evil, neutral)

		switch( caster->GetNpcAiType() )
		{
			case 2: // AI_EVIL
			case 11: // AI_EVIL_CASTER
			{
				if( target->GetNpcAiType() == AI_EVIL || target->GetNpcAiType() == AI_EVIL_CASTER )
					return; // Don't damage NPCs with similar AI types
				break;
			}
			case 5: // AI_FIGHTER
			case 10: // AI_CASTER
			{
				if( target->GetNpcAiType() == AI_FIGHTER || target->GetNpcAiType() == AI_CASTER )
					return; // Don't damage NPCs with similar AI types
				break;
			}
			default:
				break;
		}
	}

	if( target->IsNpc() )
	{
		Combat->AttackTarget( target, caster );
	}

	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	//If targetCOunt is > 1, do equal damage to two targets, split damage on more targets
	if( targCount > 1 )
	{
		spellDamage = ( spellDamage * 2 ) / targCount;
	}

	Effects->TempEffect( caster, target, 36, spellDamage, 0, 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AreaAffectSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Provides shared functionality for all area affect spells
//o------------------------------------------------------------------------------------------------o
auto AreaAffectSpell( CSocket *sock, CChar *caster, void (*trgFunc)( MAGIC_AREA_STUB_LIST ), SI08 curSpell ) -> bool
{
	SI16 x1, x2, y1, y2;
	SI08 z1, z2;
	x1 = x2 = y1 = y2 = z1 = z2 = 0;
	SI08 targCount = 0;
	UI16 i;
	std::vector<CChar *> targetList;

	Magic->BoxSpell( sock, caster, x1, x2, y1, y2, z1, z2 );

	for( auto &MapArea : MapRegion->PopulateList( caster ))
	{
		if( MapArea )
		{
			auto regChars = MapArea->GetCharList();
			for( const auto &tempChar : regChars->collection() )
			{
				if( ValidateObject( tempChar ) && tempChar->GetInstanceId() == caster->GetInstanceId() )
				{
					if( tempChar->GetX() >= x1 && tempChar->GetX() <= x2 && tempChar->GetY() >= y1 && tempChar->GetY() <= y2 &&
					   tempChar->GetZ() >= z1 && tempChar->GetZ() <= z2 && ( IsOnline(( *tempChar )) || tempChar->IsNpc() )) 
					  {
						if( tempChar == caster || LineOfSight( sock, caster, tempChar->GetX(), tempChar->GetY(), ( tempChar->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false  ) || caster->IsGM() )
						{
							//Store target candidates in targetList
							targetList.push_back( tempChar );
							//trgFunc( caster, tempChar, curSpell, targCount );
						}
						else if( sock )
						{
							sock->SysMessage( 688 ); // You cannot see the target well.
						}
					}
				}
			}
		}
	}

	// Iterate through list of valid targets, do damage
	targCount = static_cast<SI08>( targetList.size() );
	for( i = 0; i < targetList.size(); ++i )
	{
		trgFunc( caster, targetList[i], curSpell, targCount );
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splMeteorSwarm()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Meteor Swarm spell as an area affect spell
//o------------------------------------------------------------------------------------------------o
bool splMeteorSwarm( CSocket *sock, CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &MeteorSwarmStub, curSpell );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splPolymorph()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Polymorph spell
//o------------------------------------------------------------------------------------------------o
bool splPolymorph( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	if( caster->IsPolymorphed() )
	{
		sock->SysMessage( 1637 ); // You cannot polymorph while already in another form
	}
	else if( caster->IsIncognito() )
	{
		sock->SysMessage( 1673 ); // You can't do that while incognito.
	}
	else if( caster->IsDisguised() )
	{
		sock->SysMessage( 9233 ); // You cannot polymorph while disguised.
	}
	else if( caster->GetId() == 0x00b8 || caster->GetId() == 0x00b7 ) // Savage Kin Paint transformation
	{
		sock->SysMessage( 1671 ); // You cannot polymorph while wearing body paint
	}
	else
	{
		Magic->PolymorphMenu( sock, POLYMORPHMENUOFFSET ); // Polymorph
		return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	EarthquakeStub()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Earthquake spell to an individual target
//o------------------------------------------------------------------------------------------------o
void EarthquakeStub( CChar *caster, CChar *target, SI08 curSpell, [[maybe_unused]] SI08 targCount )
{
	// Check if target is in a safe zone, or invulnerable
	if( target->GetRegion()->IsSafeZone() || target->IsInvulnerable() )
	{
		// Target is in a safe zone, or invulnerable ignore spell effect
		return;
	}

	if( target == caster )
		return; // Don't let caster hit themselves with damaging spell

	if( target->IsNpc() && caster->IsNpc() )
	{
		if( target->GetNPCFlag() == caster->GetNPCFlag() )
			return; // Don't let NPCs damage other NPCs with same NPC Flag (innocent, evil, neutral)

		switch( caster->GetNpcAiType() )
		{
			case 2: // AI_EVIL
			case 11: // AI_EVIL_CASTER
			{
				if( target->GetNpcAiType() == AI_EVIL || target->GetNpcAiType() == AI_EVIL_CASTER )
					return; // Don't damage NPCs with similar AI types
				break;
			}
			case 5: // AI_FIGHTER
			case 10: // AI_CASTER
			{
				if( target->GetNpcAiType() == AI_FIGHTER || target->GetNpcAiType() == AI_CASTER )
					return; // Don't damage NPCs with similar AI types
				break;
			}
			default:
				break;
		}
	}

	// Turn caster criminal if applicable
	if( WillResultInCriminal( caster, target ) && !caster->IsGM() && !caster->IsCounselor() )
	{
		MakeCriminal( caster );
	}

	SI32 distx	= abs( target->GetX() - caster->GetX() );
	SI32 disty	= abs( target->GetY() - caster->GetY() );
	SI32 dmgmod	= std::min( distx, disty );
	dmgmod		= -( dmgmod - 7 );

	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	//If targetCOunt is > 1, do equal damage to two targets, split damage on more targets
	//if( targCount > 1 )
	//	spellDamage = ( spellDamage * 2 ) / targCount;

	//Apply bonus damage based on target distance from center of earthquake
	spellDamage += dmgmod;

	if( target->Damage( spellDamage, LIGHTNING, caster, true ))
	{
		// If this killed the target, don't continue
		if( target->IsDead() )
			return;
	}

	target->SetStamina( target->GetStamina() - ( RandomNum( 0, 9 ) + 5 ));

	if( target->GetStamina() == -1 )
	{
		target->SetStamina( 0 );
	}

	if(( !target->IsNpc() && IsOnline(( *target ))) || ( target->IsNpc() && cwmWorldState->creatures[target->GetId()].IsHuman() ))
	{
		if( !target->IsOnHorse() && !target->IsFlying() )
		{
			if( target->GetBodyType() == BT_GARGOYLE )
			{
				Effects->PlayNewCharacterAnimation( target, N_ACT_IMPACT ); // Impact anim (0x04) - can't seem to trigger death anim manually with new animation packet
			}
			else // BT_HUMAN and BT_ELF
			{
				if( RandomNum( 0, 1 ))
				{
					Effects->PlayCharacterAnimation( target, ACT_DIE_BACKWARD, 0, 6 ); // Death anim variation 1 - 0x15, forward
				}
				else
				{
					Effects->PlayCharacterAnimation( target, ACT_DIE_FORWARD, 0, 6 ); // Death anim variation 2 - 0x16, backward
				}
			}
		}
	}
	else // Monsters, animals
	{
		if( target->GetHP() > 0 )
		{
			Effects->PlayCharacterAnimation( target, 0x2, 0, 4 );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splEarthquake()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Earthquake spell as an area affect spell
//o------------------------------------------------------------------------------------------------o
bool splEarthquake( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( sock != nullptr )
	{
		sock->SetWord( 11, caster->GetX() );
		sock->SetWord( 13, caster->GetY() );
		sock->SetByte( 16, caster->GetZ() );
	}
	AreaAffectSpell( sock, caster, &EarthquakeStub, curSpell );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splEnergyVortex()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Energy Vortex spell (summons an Energy Vortex)
//o------------------------------------------------------------------------------------------------o
bool splEnergyVortex( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, [[maybe_unused]] SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;

	Magic->SummonMonster( sock, caster, 1, x, y, z );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splResurrection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Resurrection spell to a target
//o------------------------------------------------------------------------------------------------o
bool splResurrection( [[maybe_unused]] CChar *caster, CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	if( target->IsDead() )
	{
		NpcResurrectTarget( target );
		return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splSummonAir()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Air Elemental spell (Summons Air Elemental)
//o------------------------------------------------------------------------------------------------o
bool splSummonAir( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;

	Magic->SummonMonster( sock, caster, 2, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splSummonDaemon()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Daemon spell (Summons Daemon)
//o------------------------------------------------------------------------------------------------o
bool splSummonDaemon( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;

	Magic->SummonMonster( sock, caster, 7, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splSummonEarth()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Earth Elemental spell (Summons Earth Elemental)
//o------------------------------------------------------------------------------------------------o
bool splSummonEarth( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;

	Magic->SummonMonster( sock, caster, 3, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splSummonFire()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Fire Elemental spell (Summons Fire Elemental)
//o------------------------------------------------------------------------------------------------o
bool splSummonFire( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;

	Magic->SummonMonster( sock, caster, 4, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splSummonWater()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Water Elemental spell (Summons Water Elemental)
//o------------------------------------------------------------------------------------------------o
bool splSummonWater( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;

	Magic->SummonMonster( sock, caster, 5, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splRandom()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Monster spell (Summons random monster)
//o------------------------------------------------------------------------------------------------o
bool splRandom( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;

	Magic->SummonMonster( sock, caster, 8, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splNecro1()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
bool splNecro1( [[maybe_unused]] CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splNecro2()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effect of Necromantic summon spell (summons monster)
//o------------------------------------------------------------------------------------------------o
bool splNecro2( CSocket *sock, CChar *caster, [[maybe_unused]] SI08 curSpell )
{
	Magic->SummonMonster( sock, caster, 9, caster->GetX() +1, caster->GetY() +1, caster->GetZ() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splNecro3()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
bool splNecro3( [[maybe_unused]] CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splNecro4()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
bool splNecro4( [[maybe_unused]] CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	splNecro5()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
bool splNecro5( [[maybe_unused]] CChar *caster, [[maybe_unused]] CChar *target, [[maybe_unused]] CChar *src, [[maybe_unused]] SI08 curSpell )
{
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DiamondSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts field spell in the shape of a diamond
//o------------------------------------------------------------------------------------------------o
bool DiamondSpell( [[maybe_unused]] CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	SI32 j;
	SI16 fx[5], fy[5];
	SI16 yOffset = length;
	SI16 xOffset = length;

	fx[0] = -xOffset; fy[0] = 0;
	fx[1] = 0;		  fy[1] = yOffset;
	fx[2] = 0;		  fy[2] = -yOffset;
	fx[3] = xOffset;  fy[3] = 0;

	CItem *i			= nullptr;
	UI08 worldNumber = caster->WorldNumber();
	UI16 instanceId	= caster->GetInstanceId();
	for( j = 0; j < 4; ++j )	// Draw the corners of our diamond
	{
		i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
		if( i != nullptr )
		{
			i->SetDispellable( true );
			i->SetDecayable( true );
			i->SetDecayTime( BuildTimeValue( static_cast<R32>( caster->GetSkill( MAGERY ) / 15 )));
			i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY )); // remember casters magery skill for damage
			i->SetTempVar( CITV_MOREY, caster->GetSerial() );
			i->SetLocation( x + fx[j], y + fy[j], Map->Height( x + fx[j], y + fy[j], z, worldNumber, instanceId ));
			i->SetDir( 29 );
			i->SetMovable( 2 );
		}
	}
	for( j = -1; j < 2; j = j + 2 )
	{
		for( SI08 counter2 = -1; counter2 < 2; counter2 += 2 )
		{
			for( SI32 counter3 = 1; counter3 < yOffset; ++counter3 )
			{
				i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
				if( i != nullptr )
				{
					i->SetDispellable( true );
					i->SetDecayable( true );
					i->SetDecayTime( BuildTimeValue(static_cast<R32>( caster->GetSkill( MAGERY ) / 15 )));
					i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY )); // remember casters magery skill for damage
					i->SetTempVar( CITV_MOREY, caster->GetSerial() );
					const SI16 newX = x + counter2 * counter3;
					const SI16 newY = y + j * ( yOffset - counter3 );
					i->SetLocation( newX, newY, Map->Height( newX, newY, z, worldNumber, instanceId ));
					i->SetDir( 29 );
					i->SetMovable( 2 );
				}
			}
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SquareSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts field spell in the shape of a square
//o------------------------------------------------------------------------------------------------o
bool SquareSpell( [[maybe_unused]] CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	SI16 fx[5], fy[5];
	SI16 yOffset = length;
	SI16 xOffset = length;

	fx[0] = -xOffset;	fy[0] = xOffset;
	fx[1] = -yOffset;	fy[1] = yOffset;
	fx[2] = -xOffset;	fy[2] = xOffset;
	fx[3] = -yOffset;	fy[3] = yOffset;

	CItem *i = nullptr;
	UI08 worldNumber = caster->WorldNumber();
	UI16 instanceId = caster->GetInstanceId();
	for( UI08 j = 0; j < 4; ++j )	// Draw the corners of our diamond
	{
		for( SI32 counter = fx[j]; counter < fy[j]; ++counter )
		{
			i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
			if( i != nullptr )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( static_cast<R32>( caster->GetSkill( MAGERY ) / 15 )));
				i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY )); // remember casters magery skill for damage
				i->SetTempVar( CITV_MOREY, caster->GetSerial() );
				switch( j )
				{
					case 0:
					case 2:
						i->SetLocation( x + counter, y + (j - 1) * yOffset, Map->Height( x + counter, y + (j-1) * yOffset, z, worldNumber, instanceId ));
						break;
					case 1:
					case 3:
						i->SetLocation( x + (j-2 ) * xOffset, y + counter, Map->Height( x + (j-2 ) * xOffset, y + counter, z, worldNumber, instanceId ));
						break;
				}
				i->SetDir( 29 );
				i->SetMovable( 2 );
			}
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FloodSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts field spell in the shape of a filled square
//o------------------------------------------------------------------------------------------------o
bool FloodSpell( [[maybe_unused]] CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	SI16 yOffset = length;
	SI16 xOffset = length;

	SI16 maxX = x + xOffset;
	SI16 maxY = y + yOffset;
	SI16 minX = x - xOffset;
	SI16 minY = y - yOffset;

	UI08 worldNumber = caster->WorldNumber();
	UI16 instanceId = caster->GetInstanceId();
	for( SI32 counter1 = minX; counter1 <= maxX; ++counter1 )
	{
		for( SI32 counter2 = minY; counter2 <= maxY; ++counter2 )
		{
			CItem *i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
			if( i != nullptr )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( static_cast<R32>( caster->GetSkill( MAGERY ) / 15 )));
				i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY )); // remember casters magery skill for damage
				i->SetTempVar( CITV_MOREY, caster->GetSerial() );
				i->SetLocation( counter1, counter2, Map->Height( counter1, counter2, z, worldNumber, instanceId ));
				i->SetDir( 29 );
				i->SetMovable( 2 );
			}
		}
	}
	return true;
}
/////////////////////////////////////////////////////////////////
/// INDEX:
//              - misc magic functions
//              - NPCs casting spells related functions
//              - ITEMs magic powers related functions
//              - PCs casting spells related functions
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////// MISC MAGIC FUNCTIONS ////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

// added constructor/destructor
CMagic::CMagic() : spellCount( 1 )
{
	spells.resize( 0 );
}

CMagic::~CMagic()
{
	// delete any allocations
	spells.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::HasSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if given spellbook contains specific spell
//o------------------------------------------------------------------------------------------------o
bool CMagic::HasSpell( CItem *book, SI32 spellNum )
{
    if( !ValidateObject( book ))
        return false;

    // Validate spell range for the book type
    if ((book->GetType() == IT_SPELLBOOK && (spellNum < 1 || spellNum > 64)) ||
        (book->GetType() == IT_PALADINBOOK && (spellNum < 201 || spellNum > 210)) ||
        (book->GetType() == IT_NECROBOOK && (spellNum < 101 || spellNum > 117)))
    {
        Console.Error(oldstrutil::format("ERROR: HasSpell: SpellNum=%d is out of range for BookType=%d", spellNum, book->GetType()));
        return false;
    }

	// Adjust spell number for different book types
    if (book->GetType() == IT_PALADINBOOK)
        spellNum -= 200;
    else if (book->GetType() == IT_NECROBOOK)
        spellNum -= 100;

    // Determine which word and bit to check
    UI32 wordNum = ( spellNum - 1 ) / 32;  // Adjust for 0-based indexing
    UI32 bitNum = ( spellNum - 1 ) % 32;  // Calculate the bit index within the word

    if( wordNum >= 3 )  // Ensure wordNum is valid
    {
        return false;
    }

    UI32 sourceAmount = book->GetSpell(static_cast<UI08>(wordNum));
	UI32 flagToCheck = 1 << bitNum;

	return (( sourceAmount & flagToCheck ) == flagToCheck);
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AddSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds specific spell to given spellbook
//o------------------------------------------------------------------------------------------------o
void CMagic::AddSpell( CItem *book, SI32 spellNum )
{
    // Check for script triggers
    std::vector<UI16> scriptTriggers = book->GetScriptTriggers();
    for( auto scriptTrig : scriptTriggers )
    {
        cScript *toExecute = JSMapping->GetScript(scriptTrig);
        if( toExecute != nullptr )
        {
            // If script returns false/0, prevent addition of spell to spellbook
            if( toExecute->OnSpellGain( book, spellNum ) == 0 )
            {
                return;
            }
        }
    }

    if( !ValidateObject( book ))
        return;

    // Validate spell number range for the book type
    if(( book->GetType() == IT_SPELLBOOK && ( spellNum < 1 || spellNum > 64 )) ||
        ( book->GetType() == IT_PALADINBOOK && ( spellNum < 201 || spellNum > 210 )) ||
        ( book->GetType() == IT_NECROBOOK && ( spellNum < 101 || spellNum > 117 )))
    {
        Console.Error( oldstrutil::format( "ERROR: AddSpell: SpellNum=%d is out of range for BookType=%d", spellNum, book->GetType() ));
        return;
    }

    // Adjust spell number for different book types
    if( book->GetType() == IT_PALADINBOOK )
        spellNum -= 200;
    else if( book->GetType() == IT_NECROBOOK )
        spellNum -= 100;

    // Calculate word and bit position
    UI32 wordNum = ( spellNum - 1 ) / 32;  // Adjust for 0-based indexing
    UI32 bitNum = ( spellNum - 1 ) % 32;  // Calculate the bit index within the word

    if( wordNum < 3 ) // Ensure valid word index
    {
        UI32 flagToSet = 1 << bitNum;

        // Retrieve current spells and update
        UI32 targAmount = book->GetSpell( static_cast<UI08>( wordNum )) | flagToSet;
        book->SetSpell( static_cast<UI08>( wordNum ), targAmount );
    }
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::RemoveSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes specific spell from given spellbook
//o------------------------------------------------------------------------------------------------o
void CMagic::RemoveSpell( CItem *book, SI32 spellNum )
{
	std::vector<UI16> scriptTriggers = book->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// If script returns false/0, prevent removal of spell from spellbook
			if( toExecute->OnSpellLoss( book, spellNum ) == 0 )
			{
				return;
			}
		}
	}

	if( !ValidateObject( book ))
		return;

	UI32 wordNum = spellNum / 32;
	if( wordNum < 3 )
	{
		UI32 bitNum		= ( spellNum % 32 );
		UI32 flagToSet	= power( 2, bitNum );
		UI32 flagMask	= 0xFFFFFFFF;
		UI32 targAmount;
		flagMask ^= flagToSet;
		targAmount = ( book->GetSpell( static_cast<UI08>( wordNum )) & flagMask );
		book->SetSpell( static_cast<UI08>( wordNum ), targAmount );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::SpellBook()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the spellbook and displays all spells a character has in his book
//o------------------------------------------------------------------------------------------------o
void CMagic::SpellBook( CSocket *mSock )
{
    SERIAL serial = ( mSock->GetDWord( 1 ) & 0x7FFFFFFF );
    CChar *mChar = mSock->CurrcharObj();
    CItem *spellBook = CalcItemObjFromSer( serial );

    // Ensure we have the correct book
	if( !ValidateObject( spellBook ))
    {
        spellBook = FindItemOfType( mChar, IT_SPELLBOOK );
        if( !ValidateObject( spellBook ))
        {
            spellBook = FindItemOfType( mChar, IT_PALADINBOOK );
        }
        if( !ValidateObject(spellBook ))
        {
            spellBook = FindItemOfType( mChar, IT_NECROBOOK ); // Check for Necromancer book
        }
    }

    if( !ValidateObject( spellBook )) // Still no book found
    {
        mSock->SysMessage( 692 ); // You have no spellbook upon you!
        return;
    }

    CItem *pack = mChar->GetPackItem();
    if( spellBook->GetCont() != mChar && spellBook->GetCont() != pack )
    {
        mSock->SysMessage( 403 ); // If you wish to open a spellbook, it must be equipped or in your main backpack.
        return;
    }

    // Determine the type of spellbook
    bool isPaladinBook = (spellBook->GetType() == IT_PALADINBOOK);
    bool isNecroBook = (spellBook->GetType() == IT_NECROBOOK);
    int startSpell = isPaladinBook ? 201 : (isNecroBook ? 101 : 1);
    int endSpell = isPaladinBook ? 210 : (isNecroBook ? 117 : 64);

    // Initialize spellsList
    UI08 spellsList[70] = {0}; // Covers maximum spells for fallback logic

    // Populate spellsList with proper indexing
    for( int spellID = startSpell; spellID <= endSpell; ++spellID )
	{
		if( HasSpell( spellBook, spellID ))
	    {
			spellsList[spellID - startSpell] = 1;
		}
	}

    // Send the spellbook to the client
    CPDrawContainer sbStart( *spellBook );
    sbStart.Model( 0xFFFF );
    if( mSock->ClientType() >= CV_HS2D && mSock->ClientVerShort() >= CVS_7090 )
    {
        sbStart.ContType( 0x7D );
    }
    mSock->Send( &sbStart );

    // Support for new Client Spellbook
    CPNewSpellBook ourBook( *spellBook );
    if( ourBook.ClientCanReceive( mSock ))
    {
        mSock->Send(&ourBook);
        return;
    }

    // Fallback logic for older clients
    UI08 spellCount = 0;
    for (int i = 0; i < ( endSpell - startSpell + 1 ); ++i )
    {
        if( spellsList[i] )
        {
            ++spellCount;
        }
    }

    if( spellCount > 0 )
    {
        CPItemsInContainer mItems;
        if( mSock->ClientVerShort() >= CVS_6017 )
        {
            mItems.UOKRFlag( true );
        }
        mItems.NumberOfItems( spellCount );
        UI16 runningCounter = 0;
        const SERIAL containerSerial = spellBook->GetSerial();

        for( int i = 0; i < ( endSpell - startSpell + 1 ); ++i )
        {
            if (spellsList[i])
            {
                mItems.Add( runningCounter++, 0x41000000 + ( startSpell + i ), containerSerial, startSpell + i );
            }
        }
        mSock->Send( &mItems );
    }
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::GateCollision()
//|	Changes		-	collision dir
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used when a PLAYER passes through a gate.  Takes the player
//|						to the other side of the gate-link.
//o------------------------------------------------------------------------------------------------o
auto CMagic::GateCollision( CSocket *mSock, CChar *mChar, CItem *itemCheck, ItemTypes type ) -> void
{
	if( type == IT_GATE )
	{
		if( !mChar->IsNpc() )
		{
			auto otherGate = CalcItemObjFromSer( itemCheck->GetTempVar( CITV_MOREX ));
			if(  ValidateObject( otherGate ))
			{
				SI08 dirOffset = 0;
				if( mChar->GetDir() < SOUTH )
				{
					dirOffset = 1;
				}
				else
				{
					dirOffset = -1;
				}
				if( otherGate->WorldNumber() != mChar->WorldNumber() )
				{
					mChar->SetLocation( otherGate->GetX() + dirOffset, otherGate->GetY(), otherGate->GetZ(), otherGate->WorldNumber(), otherGate->GetInstanceId() );
					SendMapChange( mChar->WorldNumber(), mSock, false );
				}
				else
				{
					mChar->SetLocation( otherGate->GetX() + dirOffset, otherGate->GetY(), otherGate->GetZ(), otherGate->WorldNumber(), otherGate->GetInstanceId() );
				}
				
				// Teleport player's followers too
				auto myFollowers = mChar->GetFollowerList();
				for( const auto &myFollower : myFollowers->collection() )
				{
					if( ValidateObject( myFollower ))
					{
						if( !myFollower->GetMounted() && myFollower->GetOwnerObj() == mChar )
						{
							if( myFollower->GetNpcWander() == WT_FOLLOW && ObjInOldRange( mChar, myFollower, DIST_CMDRANGE ))
							{
								myFollower->SetLocation( mChar );
							}
						}
					}
				}
				Effects->PlaySound( mSock, 0x01FE, true );
				Effects->PlayStaticAnimation( mChar, 0x372A, 0x09, 0x06 );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::SummonMonster()
//|	Changes		-	May 4th, 2000 - made function general words PC's and NPC's
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Summon a monster (dispellable with DISPEL).
//o------------------------------------------------------------------------------------------------o
void CMagic::SummonMonster( CSocket *s, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z )
{
	bool validSocket = ( s != nullptr );
	if( validSocket )
	{
		caster = s->CurrcharObj();
	}
	else
	{	// Since caster will determine who is casting the spell... get a socket for players to see animations and hear effects
		if( caster->IsNpc() && !caster->GetTarg()->IsNpc() )
		{
			s = caster->GetTarg()->GetSocket();
		}
	}

	UI08 maxControlSlots = cwmWorldState->ServerData()->MaxControlSlots();
	UI08 maxFollowers = cwmWorldState->ServerData()->MaxFollowers();
	UI08 controlSlotsUsed = caster->GetControlSlotsUsed();
	UI08 followerCount = static_cast<UI08>( caster->GetFollowerList()->Num() );

	CChar *newChar=nullptr;

	switch( id )
	{
		case 0:	// summon monster
		{
			newChar = Npcs->CreateRandomNPC( "10000" );
			if( !ValidateObject( newChar ))
			{
				if( validSocket )
				{
					s->SysMessage( 694 ); // Contact your shard op to setup the summon list!
				}
				return;
			}

			if( maxControlSlots > 0 )
			{
				if( controlSlotsUsed + newChar->GetControlSlots() > maxControlSlots )
				{
					newChar->Delete();
					if( validSocket )
					{
						s->SysMessage( 2390 ); // That would exceed your maximum pet control slots.
					}
					return;
				}
			}
			else if( maxFollowers > 0 && followerCount >= maxFollowers )
			{
				newChar->Delete();
				if( validSocket )
				{
					s->SysMessage( 2346, maxFollowers ); // You can maximum have %i pets / followers active at the same time.
				}
				return;
			}

			if( Magic->spells[40].Effect() != INVALIDID )
			{
				Effects->PlaySound( newChar, Magic->spells[40].Effect() );
			}

			CMagicStat temp = Magic->spells[40].StaticEffect();
			if( temp.Effect() != INVALIDID )
			{
				Magic->DoStaticEffect( newChar, 40 );
			}

			newChar->SetOwner( caster );
			caster->SetControlSlotsUsed( std::clamp( controlSlotsUsed + newChar->GetControlSlots(), 0, 255 ));
			newChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( static_cast<R32>( caster->GetSkill( MAGERY ) / 5 )));
			newChar->SetLocation( caster );
			Effects->PlayCharacterAnimation( newChar, ACT_SPELL_AREA, 0, 7 ); // 0x11, used to be 0x0C
			newChar->SetFTarg( caster );
			newChar->SetNpcWander( WT_FOLLOW );
			newChar->SetDispellable( true );
			if( validSocket )
			{
				s->SysMessage( 695 ); // Your pet beings are following you.
			}
			return;
		}
		case 1: // Energy Vortex
			Effects->PlaySound( s, 0x0216, true ); // EV
			newChar = Npcs->CreateNPCxyz( "energyvortex-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		case 2: // Air Elemental
			Effects->PlaySound( s, 0x0217, true ); // AE
			newChar = Npcs->CreateNPCxyz( "airele-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		case 3: //Earth Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "earthele-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		case 4: //Fire Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "firele-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		case 5: //Water Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "waterele-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		case 6: //Blade Spirits
			Effects->PlaySound( s, 0x0212, true ); // I don't know if this is the right effect...
			newChar = Npcs->CreateNPCxyz( "bladespirit-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		case 7: // Daemon
			Effects->PlaySound( s, 0x0216, true );
			newChar = Npcs->CreateNPCxyz( "daemon-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		case 8: // Dupre The Hero
			Effects->PlaySound( s, 0x0246, true );
			newChar = Npcs->CreateNPCxyz( "dupre-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		case 9: // Black Night
			Effects->PlaySound( s, 0x0216, true );
			newChar = Npcs->CreateNPCxyz( "blacknight-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceId() );
			break;
		default:
			Effects->PlaySound( s, 0x0215, true );
	}

	if( !ValidateObject( newChar ))
		return;

	if( maxControlSlots > 0 )
	{
		if( controlSlotsUsed + newChar->GetControlSlots() > maxControlSlots )
		{
			newChar->Delete();
			if( validSocket )
			{
				s->SysMessage( 2390 ); // That would exceed your maximum pet control slots.
			}
			return;
		}
	}
	else if( maxFollowers > 0 && followerCount >= maxFollowers )
	{
		newChar->Delete();
		if( validSocket )
		{
			s->SysMessage( 2346, maxFollowers ); // You can maximum have %i pets / followers active at the same time.
		}
		return;
	}

	newChar->SetDispellable( true );

	// pc's don't own BS/EV, NPCs do
	if( caster->IsNpc() || ( id != 1 && id != 6 ))
	{
		newChar->SetOwner( caster );
		if( !caster->IsNpc() )
		{
			// Update control slots used
			caster->SetControlSlotsUsed( std::clamp(controlSlotsUsed + newChar->GetControlSlots(), 0, 255) );
		}
	}

	if( x == 0 )
	{
		newChar->SetLocation( caster->GetX()-1, caster->GetY(), caster->GetZ() );
	}
	else
	{
		if( Map->ValidSpawnLocation( x, y, z, caster->WorldNumber(), caster->GetInstanceId() ))
		{
			newChar->SetLocation( x, y, z );
		}
		else
		{
			newChar->SetLocation( caster->GetX()-1, caster->GetY(), caster->GetZ() );
		}
	}

	newChar->SetSpDelay( 10 );
	newChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( static_cast<R32>( caster->GetSkill( MAGERY ) / 5 )));
	Effects->PlayCharacterAnimation( newChar, ACT_SPELL_AREA, 0, 7 ); // 0x11, used to be 0x0C
	// (9/99) - added the chance to make the monster attack
	// the person you targeted ( if you targeted a char, naturally :) )
	CChar *i = nullptr;
	if( !caster->IsNpc() && s != nullptr )
	{
		if( s->GetDWord( 7 ) != INVALIDSERIAL )
		{
			i = CalcCharObjFromSer( s->GetDWord( 7 ));
			if( !ValidateObject( i ))
				return;
		}
	}
	else
	{
		i = caster->GetTarg();
	}
	Combat->AttackTarget( newChar, i );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckBook()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if the spell is memorized into the spellbook.
//o------------------------------------------------------------------------------------------------o
bool CMagic::CheckBook( int circle, int spellOffset, CItem* book )
{
    if( !ValidateObject( book ))
    {
       // Console.Error("CheckBook: Invalid book object provided.");
        return false;
    }

	// Determine the type of spellbook and calculate the appropriate spell number range
    int baseSpellNum = 1; // Default to regular spellbook
    if( book->GetType() == IT_PALADINBOOK )
        baseSpellNum = 201;
    else if( book->GetType() == IT_NECROBOOK )
        baseSpellNum = 101;

    int spellNum = (( circle - 1 ) * 8 ) + spellOffset + baseSpellNum;

    bool result = HasSpell( book, spellNum );
    return result;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckMana()
//|	Changes		-	to use spells[] array.
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character has enough mana to cast spell
//o------------------------------------------------------------------------------------------------o
bool CMagic::CheckMana( CChar *s, SI32 num )
{
	if( !ValidateObject( s ))
		return true;

	if( s->NoNeedMana() )
		return true;

	if( s->GetMana() >= spells[num].Mana() )
	{
		return true;
	}
	else
	{
		CSocket *p = s->GetSocket();
		if( p != nullptr )
		{
			p->SysMessage( 696 ); // You have insufficient mana to cast that spell.
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckStamina()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character has enough stamina to cast spell
//o------------------------------------------------------------------------------------------------o
bool CMagic::CheckStamina( CChar *s, SI32 num )
{
	if( !ValidateObject( s ))
		return true;

	if( s->NoNeedMana() )
		return true;

	if( s->GetStamina() >= spells[num].Stamina() )
	{
		return true;
	}
	else
	{
		CSocket *p = s->GetSocket();
		if( p != nullptr )
		{
			p->SysMessage( 697 ); // You have insufficient stamina to cast that spell.
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckHealth()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character has enough health to cast spell
//o------------------------------------------------------------------------------------------------o
bool CMagic::CheckHealth( CChar *s, SI32 num )
{
	if( !ValidateObject( s ))
		return true;

	if( s->NoNeedMana() )
		return true;

	if( spells[num].Health() == 0 )
		return true;

	if( spells[num].Health() > 0 )
	{
		if( s->GetHP() >= spells[num].Health() )
			return true;
	}
	else
	{
		return true;
	}
	CSocket *p = s->GetSocket();
	if( p != nullptr )
	{
		p->SysMessage( 698 ); // You have insufficient health to cast that spell.
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::SubtractMana()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtract the required mana from character's mana reserve
//o------------------------------------------------------------------------------------------------o
void CMagic::SubtractMana( CChar *s, SI32 mana)
{
	if( s->NoNeedMana() )
		return;

	s->SetMana( s->GetMana() - mana );
	if( s->GetMana() < 0 )
	{
		s->SetMana( 0 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::SubtractStamina()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtract the required stmina from character's stamina reserve
//o------------------------------------------------------------------------------------------------o
void CMagic::SubtractStamina( CChar *s, SI32 stamina )
{
	if( s->NoNeedMana() )
		return;

	s->SetStamina( s->GetStamina() - stamina );
	if( s->GetStamina() == -1 )
	{
		s->SetStamina( 0 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::SubtractHealth()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtract the required health from character's stamina reserve
//o------------------------------------------------------------------------------------------------o
void CMagic::SubtractHealth( CChar *s, SI32 health, SI32 spellNum )
{
	if( s->NoNeedMana() || spells[spellNum].Health() == 0 )
		return;

	if( spells[spellNum].Health() < 0 )
	{
		if( abs( spells[spellNum].Health() * health ) > s->GetHP() )
		{
			s->SetHP( 0 );
		}
		else
		{
			s->IncHP( spells[spellNum].Health() * health );
		}
	}
	else
	{
		s->IncHP( -health );
	}
	if( s->GetHP() < 0 )
	{
		s->SetHP( 0 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckMagicReflect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character is protected by MagicReflect.
//|						If yes, remove the protection and do visual effect.
//|						However, don't touch the protection if character is permanently protected
//o------------------------------------------------------------------------------------------------o
bool CMagic::CheckMagicReflect( CChar *i )
{
	if( i->IsTempReflected() )
	{
		// Is character temporarily protected by a magic reflect spell?
		i->SetTempReflected( false );
		Effects->PlayStaticAnimation( i, 0x373A, 0, 15 );
		return true;
	}
	else if( i->IsPermReflected() )
	{
		// Is character permanently protected by a magic reflect spell?
		Effects->PlayStaticAnimation( i, 0x373A, 0, 15 );
		return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckResist()
//|	Changes		-	to add EV.INT. check
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check character's magic resistance.
//o------------------------------------------------------------------------------------------------o
bool CMagic::CheckResist( CChar *attacker, CChar *defender, SI32 circle )
{
	bool i = Skills->CheckSkill( defender, MAGICRESISTANCE, 80 * circle, 800 + ( 80 * circle ));
	CSocket *s = nullptr;
	if( ValidateObject( attacker ))
	{
		// Check what is higher between user's normal resistchance and a fallback value
		// To ensure user always has a chance of resisting, however small (except at resist-skill 0 )
		SI16 defaultChance = defender->GetSkill( MAGICRESISTANCE ) / 5;
		SI16 resistChance = defender->GetSkill( MAGICRESISTANCE ) - ((( attacker->GetSkill( MAGERY ) - 20 ) / 5 ) + ( circle * 5 ));
		if( defaultChance > resistChance )
		{
			resistChance = defaultChance;
		}

		if( RandomNum( 1, 100 ) < resistChance / 10 )
		{
			s = defender->GetSocket();
			if( s != nullptr )
			{
				s->SysMessage( 699 ); // You feel yourself resisting magical energy!
			}
			i = true;
		}
		else
		{
			i = false;
		}
	}
	else
	{
		if( i )
		{
			s = defender->GetSocket();
			if( s != nullptr )
			{
				s->SysMessage( 699 ); // You feel yourself resisting magical energy!
			}
		}
	}
	return i;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckResist()
//|	Changes		-	to add EV.INT. check
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check character's magic resistance.
//o------------------------------------------------------------------------------------------------o
bool CMagic::CheckResist( SI16 resistDifficulty, CChar *defender, SI32 circle )
{
	bool i = Skills->CheckSkill( defender, MAGICRESISTANCE, 80 * circle, 800 + ( 80 * circle ));
	CSocket *s = nullptr;
	
	// Calculate chance of resisting the magic effect based on defender's magic resistance vs the resistDifficulty
	SI16 defaultChance = defender->GetSkill( MAGICRESISTANCE ) / 5;
	SI16 resistChance = defender->GetSkill( MAGICRESISTANCE ) - ((( resistDifficulty - 20 ) / 5 ) + ( circle * 5 ));

	if( defaultChance > resistChance )
	{
		resistChance = defaultChance;
	}

	if( RandomNum( 1, 100 ) < resistChance / 10 )
	{
		s = defender->GetSocket();
		if( s != nullptr )
		{
			s->SysMessage( 699 ); // You feel yourself resisting magical energy!
		}
		i = true;
	}
	else
	{
		i = false;
	}

	return i;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CalcSpellDamageMod()
//|	Date		-	November 7th 2011
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate Spell Damage after skill-modifiers
//o------------------------------------------------------------------------------------------------o
SI16 CalcSpellDamageMod( CChar *caster, CChar *target, SI16 spellDamage, bool spellResisted )
{
	// Randomize in upper-half of damage range for some variety
	spellDamage = HalfRandomNum( spellDamage );

	// If spell was resisted, halve damage
	if( spellResisted )
	{
		spellDamage = static_cast<SI16>( std::round( spellDamage / 2 ));
	}

	// Add damage bonus/penalty based on attacker's EVALINT vs defender's MAGICRESISTANCE
	UI16 casterEval = caster->GetSkill( EVALUATINGINTEL ) / 10;
	UI16 targetResist = target->GetSkill( MAGICRESISTANCE ) / 10;
	if( targetResist > casterEval )
	{
		spellDamage *= ( static_cast<SI16>((( casterEval - targetResist ) / 200.0f )) + 1 );
	}
	else
	{
		spellDamage *= ( static_cast<SI16>((( casterEval - targetResist ) / 500.0f )) + 1 );
	}

	// Randomize some more to get broader min/max damage values
	SI32 i = RandomNum( 0, 4 );
	if( i <= 2 )
	{
		spellDamage = std::round( RandomNum( static_cast<SI16>( HalfRandomNum( spellDamage ) / 2 ), spellDamage ));
	}
	else if( i == 3 )
	{
		spellDamage = std::round( HalfRandomNum( spellDamage ));
	}
	else //keep current spellDamage
	{
		spellDamage = std::round( spellDamage );
	}

	return spellDamage;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::MagicDamage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate and inflict magic damage.
//o------------------------------------------------------------------------------------------------o
void CMagic::MagicDamage( CChar *p, SI16 amount, CChar *attacker, WeatherType element )
{
	if( !ValidateObject( p ))
		return;

	if( p->IsDead() || p->GetHP() <= 0 )	// extra condition check, to see if deathstuff hasn't been hit yet
		return;

	CSocket *mSock = p->GetSocket();

	if( p->IsFrozen() && p->GetDexterity() > 0 && !p->IsCasting() )
	{
		p->SetFrozen( false );
		p->Dirty( UT_UPDATE );
		if( mSock != nullptr )
		{
			mSock->SysMessage( 700 ); // You are no longer frozen.
		}
	}

	// Allow damage if target is not invulnerable or in a zone safe from aggressive magic
	if( !p->IsInvulnerable() && !p->GetRegion()->IsSafeZone() && p->GetRegion()->CanCastAggressive() )
	{
		UI08 hitLoc = Combat->CalculateHitLoc();
		SI16 damage = Combat->ApplyDamageBonuses( element, attacker, p, MAGERY, hitLoc, amount);
		damage = Combat->ApplyDefenseModifiers( element, attacker, p, MAGERY, hitLoc, damage, true);
		if( damage <= 0 )
		{
			damage = 1;
		}

		[[maybe_unused]] bool retVal = p->Damage( damage, element, attacker, true );
		p->ReactOnDamage( element, attacker );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::PoisonDamage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Apply the poison to the character.
//o------------------------------------------------------------------------------------------------o
void CMagic::PoisonDamage( CChar *p, SI32 poison )
{
	if( p->IsFrozen() )
	{
		p->SetFrozen( false );
		p->Dirty( UT_UPDATE );
		CSocket *s = p->GetSocket();
		if( s != nullptr )
		{
			s->SysMessage( 700 ); // You are no longer frozen.
		}
	}

	// Allow poison damage if target is not invunlnerable or in a zone safe from aggressive magic
	if( !p->IsInvulnerable() && !p->GetRegion()->IsSafeZone() && p->GetRegion()->CanCastAggressive() )
	{
		if( poison > 5 )
		{
			poison = 5;
		}
		else if( poison < 0 )
		{
			poison = 1;
		}

		// Apply poison on target
		p->SetPoisoned( poison );

		// Set time until poison wears off completely
		p->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( GetPoisonDuration( poison )));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckFieldEffects()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character stands on a magic-field and apply effects.
//o------------------------------------------------------------------------------------------------o
auto CMagic::CheckFieldEffects( CChar& mChar ) -> void
{
	auto toCheck = MapRegion->GetMapRegion( &mChar );
	if( toCheck )
	{
		auto regItems = toCheck->GetItemList();
		for( const auto &inItemList :regItems->collection() )
		{
			if( ValidateObject( inItemList ) && inItemList->GetInstanceId() == mChar.GetInstanceId() )
			{
				if( inItemList->GetX() == mChar.GetX() && inItemList->GetY() == mChar.GetY() )
				{
					HandleFieldEffects(( &mChar ), inItemList, inItemList->GetId() );
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::HandleFieldEffects()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle the effects of characters entering field spells
//o------------------------------------------------------------------------------------------------o
bool CMagic::HandleFieldEffects( CChar *mChar, CItem *fieldItem, UI16 id )
{
	CChar *caster;
	if( id >= 0x398C && id <= 0x399F ) //fire field
	{
		caster = CalcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ));	// store caster in morey
		if( ValidateObject( caster) )
		{
			if( mChar->GetTimer( tCHAR_FIREFIELDTICK ) < cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
			{
				// Set a timer, so another field spell cannot "tick" for this character for a short while
				mChar->SetTimer( tCHAR_FIREFIELDTICK, BuildTimeValue( static_cast<R32>( Magic->spells[28].DamageDelay() )));

				// Fetch spell damage from Fire Field spell
				auto spellDamage = Magic->spells[28].BaseDmg();

				// Apply magic damage
				if( !CheckResist( nullptr, mChar, 4 ))
				{
					MagicDamage( mChar, spellDamage, caster, HEAT );
				}
				else
				{
					MagicDamage( mChar, spellDamage / 2, caster, HEAT );
				}
				Effects->PlaySound( mChar, 520 );
			}
		}
		return true;
	}
	else if( id >= 0x3914 && id <= 0x3929 ) //poison field
	{
		caster = CalcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ));	// caster is stored in MOREY
		if( ValidateObject( caster ) && mChar->IsInnocent() )
		{
			if( WillResultInCriminal( caster, mChar ) && ValidateObject( caster ) && !caster->IsGM() && !caster->IsCounselor() )
			{
				MakeCriminal( caster );
			}
		}
		if( mChar->GetTimer( tCHAR_POISONFIELDTICK ) < cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			// Set a timer, so another field spell cannot "tick" for this character for a short while
			mChar->SetTimer( tCHAR_POISONFIELDTICK, BuildTimeValue( static_cast<R32>( Magic->spells[39].DamageDelay() )));

			// Calculate strength of poison, but disregard range check
			UI08 poisonStrength = 1;

			// Fetch average skill value of poison field caster's Magery and Poisoning skills
			UI16 skillVal = fieldItem->GetTempVar( CITV_MOREX );

			// Calculate strength of poison
			poisonStrength = CalculateMagicPoisonStrength( caster, mChar, skillVal, false, 5 );

			// Apply poison on character
			PoisonDamage( mChar, poisonStrength );
			Effects->PlaySound( mChar, 520 );
		}
		return true;
	}
	else if( id >= 0x3967 && id <= 0x398A ) //paralysis field
	{
		caster = CalcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ));	// store caster in morey
		if( mChar->IsInnocent() )
		{
			if( WillResultInCriminal( caster, mChar ) && ValidateObject( caster ) && !caster->IsGM() && !caster->IsCounselor() )
			{
				MakeCriminal( caster );
			}
		}
		if( mChar->GetTimer( tCHAR_PARAFIELDTICK ) < cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			// Set a timer, so another field spell cannot "tick" for this character for a short while
			mChar->SetTimer( tCHAR_PARAFIELDTICK, BuildTimeValue( static_cast<R32>( Magic->spells[39].DamageDelay() )));

			if( !CheckResist( nullptr, mChar, 6 ))
			{
				Effects->TempEffect( caster, mChar, 1, 0, 0, 0 );
				Effects->PlaySound( mChar, 520 );
			}
		}
		return true;
	}
	else if( id >= 0x3946 && id <= 0x3964 ) //energy field
	{
		caster = CalcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ));	// store caster in morey
		if( ValidateObject( caster) )
		{
			if( RandomNum( 0, 2 ) == 1 )
			{
				if( !CheckResist( nullptr, mChar, 4 ))
				{
					MagicDamage( mChar, fieldItem->GetTempVar( CITV_MOREX ) / 50, caster, LIGHTNING );
				}
				else
				{
					MagicDamage( mChar, fieldItem->GetTempVar( CITV_MOREX ) / 100, caster, LIGHTNING );
				}
				Effects->PlaySound( mChar, 520 );
			}
		}
		return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::BoxSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the spell box effect, depending on character's magery skill
//o------------------------------------------------------------------------------------------------o
void CMagic::BoxSpell( CSocket *s, CChar *caster, SI16& x1, SI16& x2, SI16& y1, SI16& y2, SI08& z1, SI08& z2 )
{
	SI16 x, y;
	SI08 z;
	SI16 length;

	if( s != nullptr )
	{
		x = s->GetWord( 11 );
		y = s->GetWord( 13 );
		z = s->GetByte( 16 );
	}
	else
	{
		x = caster->GetTarg()->GetX();
		y = caster->GetTarg()->GetY();
		z = caster->GetTarg()->GetZ();
	}

	length = std::max( caster->GetSkill( MAGERY ) / 130, 1 );

	x1 = x - length;
	x2 = x + length;
	y1 = y - length;
	y2 = y + length;
	z1 = z;
	z2 = z + 3;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::MagicTrap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do the visual effect and apply damage when a player opens a trapped container
//o------------------------------------------------------------------------------------------------o
auto CMagic::MagicTrap( CChar *s, CItem *i ) -> void
{
	if( ValidateObject( s ) && ValidateObject( i ))
	{
		if( FindItemOwner( i ) == s || ObjInRange( s, i, DIST_NEARBY ))
		{
			Effects->PlayStaticAnimation( i, 0x36B0, 0x09, 0x09 );
			Effects->PlayStaticAnimation( s, 0x36B0, 0x09, 0x09 );
			Effects->PlaySound( s, 0x0207 );
			if( CheckResist( nullptr, s, 4 ))
			{
				MagicDamage( s, i->GetTempVar( CITV_MOREZ, 2 ) / 2, nullptr, HEAT );
			}
			else
			{
				MagicDamage( s, i->GetTempVar( CITV_MOREZ, 2 ), nullptr, HEAT );
			}
		}
		else
		{
			// Player is out of range, possibly opened the trapped container using Telekinesis from a safe distance
			Effects->PlayStaticAnimation( i, 0x36B0, 0x09, 0x09 );
			Effects->PlaySound( i, 0x0207 );
			
			// Find nearby players and apply damage to them?
			for( auto &MapArea : MapRegion->PopulateList( s ))
			{
				if( MapArea == nullptr )
					continue;

				auto regChars = MapArea->GetCharList();
				for( const auto &tempChar :regChars->collection() )
				{
					if( !ValidateObject( tempChar ) || tempChar->GetInstanceId() != s->GetInstanceId() || tempChar->GetSerial() == s->GetSerial() )
						continue;

					if( ObjInRange( tempChar, i, DIST_NEARBY ))
					{
						if( CheckResist( nullptr, tempChar, 4 ))
						{
							MagicDamage( tempChar, i->GetTempVar( CITV_MOREZ, 2 ) / 2, s, HEAT );
						}
						else
						{
							MagicDamage( tempChar, i->GetTempVar( CITV_MOREZ, 2 ), s, HEAT );
						}
								
						// Reveal hidden players impacted by the explosion
						if( tempChar->GetVisible() == VT_TEMPHIDDEN || tempChar->GetVisible() == VT_INVISIBLE )
						{
							tempChar->ExposeToView();
						}
					}
				}
			}
		}
		
		// Set first part of MOREZ to 0 to disable trap, but leave other parts intact
		i->SetTempVar( CITV_MOREZ, 1, 0 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CheckReagents()
//|	Changes		-	to use reag-st
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for required reagents in player's backpack.
//o------------------------------------------------------------------------------------------------o
bool CMagic::CheckReagents( CChar *s, const Reag_st *reagents )
{
	Reag_st failmsg;
	if( s->NoNeedReags() )
		return true;

	if( reagents->ash != 0 && GetItemAmount( s, 0x0F8C ) < reagents->ash )
	{
		failmsg.ash = 1;
	}
	if( reagents->drake!=0 && GetItemAmount( s, 0x0F86 ) < reagents->drake )
	{
		failmsg.drake = 1;
	}
	if( reagents->garlic != 0 && GetItemAmount( s, 0x0F84 ) < reagents->garlic )
	{
		failmsg.garlic = 1;
	}
	if( reagents->ginseng != 0 && GetItemAmount( s, 0x0F85 ) < reagents->ginseng )
	{
		failmsg.ginseng = 1;
	}
	if( reagents->moss != 0 && GetItemAmount( s, 0x0F7B ) < reagents->moss )
	{
		failmsg.moss = 1;
	}
	if( reagents->pearl != 0 && GetItemAmount( s, 0x0F7A ) < reagents->pearl )
	{
		failmsg.pearl = 1;
	}
	if( reagents->shade != 0 && GetItemAmount( s, 0x0F88 ) < reagents->shade )
	{
		failmsg.shade = 1;
	}
	if( reagents->silk != 0 && GetItemAmount( s, 0x0F8D ) < reagents->silk )
	{
		failmsg.silk = 1;
	}
	return RegMsg( s, failmsg );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::RegMsg()
//|	Changes		-	display missing reagents types
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display an error message if character has not enough regs
//o------------------------------------------------------------------------------------------------o
bool CMagic::RegMsg( CChar *s, Reag_st failmsg )
{
	if( !ValidateObject( s ))
		return true;

	bool display = false;
	char message[100] = { 0, };

	// Copy dictionary message into char array
	strcopy( message, 100, Dictionary->GetEntry( 702 ).c_str() ); // You do not have enough reagents to cast that spell.

	// Create temporary string to hold info on our missing reagents
	std::string tempString;
	tempString = " [";

	if( failmsg.ash )
	{
		display = true; tempString += "Sa, ";
	}
	if( failmsg.drake )
	{
		display = true; tempString += "Mr, ";
	}
	if( failmsg.garlic )
	{
		display = true; tempString += "Ga, ";
	}
	if( failmsg.ginseng )
	{
		display = true; tempString += "Gi, ";
	}
	if( failmsg.moss )
	{
		display = true; tempString += "Bm, ";
	}
	if( failmsg.pearl )
	{
		display = true; tempString += "Bp, ";
	}
	if( failmsg.shade )
	{
		display = true; tempString += "Ns, ";
	}
	if( failmsg.silk )
	{
		display = true; tempString += "Ss, ";
	}

	// Append our temporary string to the end of the char array and add an end-bracket
	mstrcat( message, 100, tempString.c_str() );
	message[strlen( message ) - 2] = ']';

	if( display )
	{
		CSocket *i = s->GetSocket();
		if( i != nullptr )
		{
			i->SysMessage( message );
		}
		return false;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::SpellFail()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do visual and sound effects when a player fails to cast a spell.
//o------------------------------------------------------------------------------------------------o
void CMagic::SpellFail( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	Effects->PlayStaticAnimation( mChar, 0x3735, 0, 30 );
	Effects->PlaySound( mChar, 0x005C );
	mChar->TextMessage( s, 771, EMOTE, false ); // The spell fizzles.
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////// PCs CASTING SPELLS RELATED FUNCTIONS ////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::SelectSpell()
//|	Date		-	28 August 1999 / 10 September 1999
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Execute the selected spell to cast.
//|
//|	Changes		-	Use spells (memory version of script info )
//|					Made memory optionable
//|					Perma cached
//|					Added JS handling
//o------------------------------------------------------------------------------------------------o
bool CMagic::SelectSpell( CSocket *mSock, SI32 num )
{
	if( num < 1 )
	{
		Console.Error( "Invalid spell ID passed to CMagic::SelectSpell() - aborting spellcast attempt!" );
		return false;
	}

	/*SI16 lowSkill = 0, highSkill = 0;*/
	CChar *mChar = mSock->CurrcharObj();

	cScript *jsScript = JSMapping->GetScript( spells[num].JSScript() );

	if( jsScript != nullptr )
	{
		// It is the responsibility of the JS script to call the proper events (ie onSpellSuccess and so forth)
		// They deal with their own mana subtraction and all sorts of things
		// They also deal with valid regions and what not.  s is *not* guaranteed to be valid (ie it could be an NPC)
		// Suggested script workflow:
		//	* Check if already casting, if so, stop and dump out
		//	* Check if frozen, if so, can't cast spell
		//	* Check if enabled
		//	* Check if scroll, wand or normal cast (scroll reduce requirements, wand do straight away)
		//	* Check requirements (min int, dex, str)
		//	* Check for reagants
		//	* Check if can cast in this area
		//	* Determine if the cast "succeeded"
		//	* If so, fire onSpellSuccess, otherwise fire onSpellFailure.  If failed, speak and return
		//	* Set the player as "casting" and freeze
		//	* Create timer for when the spell will be complete
		//	* Wait for timer to elapse
		//	* Upon timer elapsing,
		//	*	Unfreeze player
		//	*	Send target identifier if any (based on timer) and wait for callback
		//	*	If not target, do action (if any)
		//	*	If target, create onCallback function
		//	*	On callback, do stuff
		//	* NOTE * For area affect spells, use an AreaCharacterFunction
		if( jsScript->MagicSpellCast( mSock, mChar, false, num ))
		{
			return true;
		}
	}

	if( !mChar->GetCanAttack() )
	{
		mSock->SysMessage( 1778 ); // You are currently under the effect of peace and can not attack!
		return false;
	}

	if( mSock->GetCursorItem() != nullptr )
	{
		// Player is holding an item, disallow casting while holding items on the cursor
		mSock->SysMessage( Dictionary->GetEntry( 2862, mSock->Language() )); // Your hands must be free to cast spells or meditate.
		return false;
	}

	SI32 type = mSock->CurrentSpellType();
	CSpellInfo curSpellCasting = spells[num];

	// Spell recovery time active? This timer stars as soon as targeting cursor is ready, and defaults to 1.0 seconds unless set otherwise in spells.dfn
	if( mChar->GetTimer( tCHAR_SPELLRECOVERYTIME ) != 0 )
	{
		if( mChar->GetTimer( tCHAR_SPELLRECOVERYTIME ) > cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			mSock->SysMessage( 1638 ); // You must wait a little while before casting
			return false;
		}
	}

	// Player already casting a spell?
	if( mChar->GetTimer( tCHAR_SPELLTIME ) != 0 )
	{
		if( mChar->IsCasting() )
		{
			mSock->SysMessage( 762 ); // You are already casting a spell.
			return false;
		}
		else if( mChar->GetTimer( tCHAR_SPELLTIME ) > cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			mSock->SysMessage( 1638 ); // You must wait a little while before casting
			return false;
		}
	}

	mChar->SetSpellCast( static_cast<SI32>( num ));
	if( num > 63 && num <= static_cast<SI32>( spellCount ) && spellCount <= 70 )
	{
		LogSpell( Dictionary->GetEntry( magic_table[num].spell_name ), mChar, nullptr, "(Attempted)");
	}
	if( mChar->IsJailed() && !mChar->IsGM() )
	{
		mSock->SysMessage( 704 ); // You are in jail and cannot cast spells!
		mChar->StopSpell();
		return false;
	}

	// Region checks
	CTownRegion *ourReg = mChar->GetRegion();
	if(( num == 45 && !ourReg->CanMark() ) || ( num == 52 && !ourReg->CanGate() ) ||
	   ( num == 32 && !ourReg->CanRecall() ) || ( num == 22 && !ourReg->CanTeleport() ))
	{
		mSock->SysMessage( 705 ); // This is not allowed here.
		mChar->StopSpell();
		return false;
	}

	// Check if caster can cast aggressive spells in the region they're in
	if( spells[mChar->GetSpellCast()].AggressiveSpell() )
	{
		bool allowCasting = true;
		if( mChar->GetRegion()->IsSafeZone() )
		{
			mSock->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
			allowCasting = false;
		}
		else if( !mChar->GetRegion()->CanCastAggressive() )
		{
			mSock->SysMessage( 706 ); // This is not allowed in town.
			allowCasting = false;
		}

		if( !allowCasting )
		{
			// Not allowed to cast selected spell in this region, abort
			mChar->StopSpell();
			return false;
		}
	}

	if( !curSpellCasting.Enabled() )
	{
		mSock->SysMessage( 707 ); // That spell is currently not enabled.
		mChar->StopSpell();
		return false;
	}

	// The following loop checks to see if any item is currently equipped (if not a GM)
	if( !mChar->IsGM() && type != 2 )
    {
       bool autoUnequipEnabled = cwmWorldState->ServerData()->AutoUnequippedCasting();

        CItem *itemRHand = mChar->GetItemAtLayer( IL_RIGHTHAND );
        CItem *itemLHand = mChar->GetItemAtLayer( IL_LEFTHAND );
        auto mCharPack = mChar->GetPackItem();

        // Function to check and possibly unequip an item if it blocks spell casting
        auto handleItem = [&]( CItem* item, auto itemCheck, bool& blockFlag )
        {
            if( item && itemCheck( item ))
            {
                // If auto-unequip is enabled, make sure pack can hold another item before unequipping
                if( autoUnequipEnabled && ValidateObject( mCharPack ) && mCharPack->GetContainsList()->Num() < mCharPack->GetMaxItems() )
                {
                    item->SetCont( mCharPack );
                    blockFlag = false;
                }
                else
                {
                    blockFlag = true;
                }
            }
            else
            {
                blockFlag = false;
            }
        };

        bool lHandBlocks = true;
        bool rHandBlocks = true;

        // Evaluate blocking for left and right hand items
        handleItem( itemLHand, []( CItem* item ) { return item->GetType() != IT_SPELLCHANNELING; }, lHandBlocks );
        handleItem( itemRHand, []( CItem* item ) { return item->GetType() != IT_SPELLBOOK && item->GetType() != IT_SPELLCHANNELING; }, rHandBlocks );

        if( lHandBlocks || rHandBlocks )
        {
            mSock->SysMessage( 708 ); // You cannot cast with a weapon equipped.
            mChar->StopSpell();
            return false;
        }
    }

	if( mChar->GetVisible() == VT_TEMPHIDDEN || mChar->GetVisible() == VT_INVISIBLE )
	{
		mChar->ExposeToView();
	}
	mChar->BreakConcentration( mSock );

	//			What the JavaScript event has to handle:

	//			- Reag consumption
	//			- Health consumption
	//			- Mana consumption
	//			- Stamina consumption

	//			- Delay measurement

	SI16 castDelay = -2;

	// If more than one script contains the onSpellCast/onScrollCast events, whichever script runs last will
	// determine the final delay value
	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// Events:
			//  type 0: onSpellCast   (SpellBook)
			//  type 1: onScrollCast  (Scroll)
			//  type 2: onScrolllCast (Wand)

			if( type == 0 )
			{
				castDelay = toExecute->OnSpellCast( mChar, num );
			}
			else
			{
				castDelay = toExecute->OnScrollCast( mChar, num );
			}
		}
	}

	// If Delay is -1 the spell should be canceled
	if( castDelay == -1 )
	{
		mChar->StopSpell();
		return false;
	}

	if(( castDelay == -2 ) && !mChar->IsGM() )
	{
		// type == 2 - Wands
		if( type != 2 )
		{
			if( !CheckMana( mChar, num ))
			{
				mSock->SysMessage( 696 ); // You have insufficient mana to cast that spell.
				mChar->StopSpell();
				return false;
			}
			if( !CheckStamina( mChar, num ))
			{
				mSock->SysMessage( 697 ); // You have insufficient stamina to cast that spell.
				mChar->StopSpell();
				return false;
			}
			if( !CheckHealth( mChar, num ))
			{
				mSock->SysMessage( 698 ); // You have insufficient health to cast that spell.
				mChar->StopSpell();
				return false;
			}
		}
	}

	mChar->SetNextAct( 75 );		// why 75?

	// Delay measurement...
	if( castDelay >= 0 )
	{
		mChar->SetTimer( tCHAR_SPELLTIME, BuildTimeValue(static_cast<R32>( castDelay / 1000 )));
		if( !cwmWorldState->ServerData()->CastSpellsWhileMoving() )
		{
			mChar->SetFrozen( true );
			mChar->Dirty( UT_UPDATE );
		}
	}
	else if( type == 0 && mChar->GetCommandLevel() < 2 ) // if they are a gm they don't have a delay :-)
	{
		mChar->SetTimer( tCHAR_SPELLTIME, BuildTimeValue( static_cast<R32>( curSpellCasting.Delay() )));
		if( !cwmWorldState->ServerData()->CastSpellsWhileMoving() )
		{
			mChar->SetFrozen( true );
			mChar->Dirty( UT_UPDATE );
		}
	}
	else
	{
		// Inform GM why there's no spellcast delay
		mSock->SysMessage( 6280 ); // Spellcast delay ignored due to elevated command level!
		mChar->SetTimer( tCHAR_SPELLTIME, 0 );
	}
	// Delay measurement end

	//if( !mChar->IsOnHorse() )
		Effects->PlaySpellCastingAnimation( mChar, curSpellCasting.Action(), false, false ); // do the action

	std::string temp;
	if( spells[num].FieldSpell() )
	{
		if( mChar->GetSkill( MAGERY ) > 600 )
		{
			temp = "Vas " + curSpellCasting.Mantra();
		}
		else
		{
			temp = curSpellCasting.Mantra();
		}
	}
	else
	{
		temp = curSpellCasting.Mantra();
	}

	mChar->TextMessage( nullptr, temp, TALK, false );
	mChar->SetCasting( true );
	return true;

}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::GetFieldDir()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets field direction
//o------------------------------------------------------------------------------------------------o
UI08 CMagic::GetFieldDir( CChar *s, SI16 x, SI16 y )
{
	UI08 fieldDir = 0;
	switch( Movement->Direction( s, x, y ))
	{
		case NORTH:
		case SOUTH:
			break;
		case EAST:
		case WEST:
			fieldDir = 1;
			break;
		case NORTHEAST:
		case SOUTHEAST:
		case SOUTHWEST:
		case NORTHWEST:
		case UNKNOWNDIR:
			switch( s->GetDir() )
			{
				case NORTH:
				case SOUTH:
					break;
				case NORTHEAST:
				case EAST:
				case SOUTHEAST:
				case SOUTHWEST:
				case WEST:
				case NORTHWEST:
					fieldDir = 1;
					break;
				default:
					Console.Error( " Fallout of switch statement without default. uox3.cpp, GetFieldDir()" );
					break;
			}
			break;
		default:
			Console.Error( " Fallout of switch statement without default. uox3.cpp, GetFieldDir()" );
			break;
	}
	return fieldDir;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::CastSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles a character's attempt to cast spells
//o------------------------------------------------------------------------------------------------o
void CMagic::CastSpell( CSocket *s, CChar *caster )
{
	// for LocationTarget spell like ArchCure, ArchProtection etc...
	SI08 curSpell;

	curSpell = caster->GetSpellCast();
	
	// No spell! Abort
	if( curSpell == -1 )
		return;

	cScript *jsScript = JSMapping->GetScript( spells[curSpell].JSScript() );
	if( jsScript != nullptr )
	{
		if( jsScript->MagicSpellCast( s, caster, true, curSpell ))
		{
			return;
		}
	}

	if( caster->IsCasting() )
		return;	// interrupted

	caster->StopSpell();

	std::vector<UI16> scriptTriggers = caster->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// If script returns true/1, prevent other scripts with event from running
			if( toExecute->OnSpellSuccess( caster, curSpell ) == 1 )
			{
				break;
			}
		}
	}

	CChar *src = caster;
	bool validSocket = ( s != nullptr );

	// Check if caster can cast aggressive spells in the region they're in
	if( spells[curSpell].AggressiveSpell() )
	{
		bool allowCasting = true;
		if( caster->GetRegion()->IsSafeZone() )
		{
			// Caster is in a safe zone, where all aggressive actions are forbidden
			if( validSocket )
			{
				s->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
			}
			allowCasting = false;
		}
		else if( !caster->GetRegion()->CanCastAggressive() )
		{
			// Caster is in a region where casting aggressive spells is forbidden
			if( validSocket )
			{
				s->SysMessage( 709 ); // You can't cast in town!
			}
			allowCasting = false;
		}
		if( !allowCasting )
			return;
	}

	if( curSpell == 32 || curSpell == 52 )
	{
		if( !cwmWorldState->ServerData()->TravelSpellsWhileAggressor() )
		{
			// Aggressor flag only matters against other players, not against NPCs
			if(( caster->IsAggressor( true ) && ValidateObject( caster->GetTarg() ) && caster->GetTarg()->IsInnocent() ))
			{
				if( validSocket )
				{
					s->SysMessage( 2066 ); // You are not allowed to use Recall or Gate spells while being the aggressor in a fight!
				}
				return;
			}
		}

		if( caster->IsCriminal() || caster->HasStolen() )
		{
			if( validSocket )
			{
				s->SysMessage( 9278 ); // You cannot use Recall or Gate spells while you have an active criminal flag, or if you have stolen recently!
			}
			return;
		}
	}

	//Check for enough reagents
	// type == 0 -> SpellBook
	if( validSocket && s->CurrentSpellType() == 0 && !CheckReagents( caster, spells[curSpell].ReagantsPtr() ))
	{
		caster->StopSpell();
		return;
	}

	// Let human NPC casters say the magic words as they cast spells
	if( caster->IsNpc() && cwmWorldState->creatures[caster->GetId()].IsHuman() )
	{
		std::string temp;
		if( spells[curSpell].FieldSpell() )
		{
			if( caster->GetSkill( MAGERY ) > 600 )
			{
				temp = "Vas " + spells[curSpell].Mantra();
			}
			else
			{
				temp = spells[curSpell].Mantra();
			}
		}
		else
		{
			temp = spells[curSpell].Mantra();
		}

		caster->TextMessage( nullptr, temp, TALK, false );
	}

	// Consume mana/health/stamina for the spellcast attempt, if 
	// caster is an NPC or spelltype is spellbook or scrolls
	if( caster->IsNpc() || ( validSocket && ( s->CurrentSpellType() != 2 )))
	{
		SubtractMana( caster, spells[curSpell].Mana() );
		if( spells[curSpell].Health() > 0 )
		{
			SubtractHealth( caster, spells[curSpell].Health(), curSpell );
		}
		SubtractStamina( caster, spells[curSpell].Stamina() );
	}

	// Consume reagents for the spellcast attempt
	if( validSocket && s->CurrentSpellType() == 0 && !caster->IsNpc() )
	{
		DelReagents( caster, spells[curSpell].Reagants() );
	}

	// Cut the casting requirement on scrolls
	SI16 lowSkill = 0, highSkill = 0;
	if( validSocket && s->CurrentSpellType() == 1 && cwmWorldState->ServerData()->CutScrollRequirementStatus() ) // only if enabled
	{
		lowSkill	= spells[curSpell].ScrollLow();
		highSkill	= spells[curSpell].ScrollHigh();
	}
	else
	{
		lowSkill	= spells[curSpell].LowSkill();
		highSkill	= spells[curSpell].HighSkill();
	}

	// Do the skill check, and fizzle if player fails
	if( !caster->IsNpc() && !caster->IsGM() && ( !Skills->CheckSkill( caster, MAGERY, lowSkill, highSkill )))
	{
		if( validSocket )
		{
			SpellFail( s );
		}
		caster->StopSpell();
		return;
	}

	if( curSpell > 63 && static_cast<UI32>(curSpell) <= spellCount && spellCount <= 70 )
	{
		LogSpell( Dictionary->GetEntry( magic_table[curSpell].spell_name ), caster, nullptr, "(Succeeded)");
	}

	if( spells[curSpell].RequireTarget() )					// target spells if true
	{
		CItem *i = nullptr;
		if( validSocket && spells[curSpell].TravelSpell() )				// travel spells.... mark, recall and gate
		{
			// Region checks
			CTownRegion *ourReg = caster->GetRegion();
			if(( curSpell == 45 && !ourReg->CanMark() ) || ( curSpell == 52 && !ourReg->CanGate() ) ||
			   ( curSpell == 32 && !ourReg->CanRecall() ) || ( curSpell == 22 && !ourReg->CanTeleport() ))
			{
				if( validSocket )
				{
					s->SysMessage( 705 ); // This is not allowed here.
				}
				return;
			}
			// mark, recall and gate go here
			if( !caster->IsNpc() )
			{
				i = CalcItemObjFromSer( s->GetDWord( 7 ));
			}
			if( ValidateObject( i ))
			{
				if( i->GetCont() != nullptr || LineOfSight( s, caster, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) || caster->IsGM() )
				{
					if(( i->GetType() == IT_RECALLRUNE || (( curSpell == 32 || curSpell == 52 ) && i->GetType() == IT_RUNEBOOK )) || ( curSpell != 45 && i->GetType() == IT_KEY && cwmWorldState->ServerData()->TravelSpellsFromBoatKeys() ))
					{
						PlaySound( src, curSpell );
						DoMoveEffect( curSpell, i, src );
						DoStaticEffect( src, curSpell );
						switch( curSpell )
						{
							case 32: //////////// (32) RECALL ////////////////
							case 45: //////////// (45) MARK //////////////////
							case 52: //////////// (52) GATE //////////////////
							{
								// Check for onSpellTargetSelect on spell caster
								scriptTriggers.clear();
								scriptTriggers.shrink_to_fit();
								scriptTriggers = caster->GetScriptTriggers();
								for( auto scriptTrig : scriptTriggers )
								{
									cScript *toExecute = JSMapping->GetScript( scriptTrig );
									if( toExecute != nullptr )
									{
										auto retVal = toExecute->OnSpellTargetSelect( caster, i, curSpell );
										if( retVal == 1 )
										{
											break;
										}
										else if( retVal == 2 )
										{
											return;
										}
									}
								}

								// Check for onSpellTarget on spell target
								scriptTriggers.clear();
								scriptTriggers.shrink_to_fit();
								scriptTriggers = i->GetScriptTriggers();
								for( auto scriptTrig : scriptTriggers )
								{
									cScript *toExecute = JSMapping->GetScript( scriptTrig );
									if( toExecute != nullptr )
									{
										// If script returns true/1, prevent other scripts with event from running
										// If script returns 2, spell being cast on this target was rejected
										auto retVal = toExecute->OnSpellTarget( i, caster, curSpell );
										if( retVal == 1 )
										{
											break;
										}
										else if( retVal == 2 )
										{
											return;
										}
									}
								}

								(*(( MAGIC_ITEMFUNC )magic_table[curSpell-1].mag_extra ))( s, caster, i, curSpell );
								break;
							}
							default:
								Console.Error( oldstrutil::format( " Unknown Travel spell %i, magic.cpp", curSpell ));
								break;
						}
					}
					else if( validSocket )
					{
						s->SysMessage( 710 ); // That item is not a recall rune.
					}
				}
				else if( validSocket )
				{
					s->SysMessage( 683 ); // There seems to be something in the way.
				}
			}
			else if( validSocket )
			{
				s->SysMessage( 710 ); // That item is not a recall rune.
			}
			return;
		}

		if( spells[curSpell].RequireCharTarget() )
		{
			// TARGET CALC HERE
			CChar *c = nullptr;
			if( validSocket && !caster->IsNpc() )
			{
				c = CalcCharObjFromSer( s->GetDWord( 7 ));
			}
			else
			{
				c = caster->GetTarg();
			}

			if( ValidateObject( c )) // we have to have targetted a person to kill them :)
			{
				if( !ObjInRange( c, caster, cwmWorldState->ServerData()->CombatMaxSpellRange() ))
				{
					if( validSocket )
					{
						s->SysMessage( 712 ); // You can't cast on someone that far away!
					}
					return;
				}
				if( LineOfSight( s, caster, c->GetX(), c->GetY(), ( c->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) || caster->IsGM() )
				{
					if( spells[curSpell].AggressiveSpell() )
					{
						if( c->GetRegion()->IsSafeZone() )
						{
							// Target is in a safe zone, where all aggressive actions are forbidden
							if( validSocket )
							{
								s->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
							}
							return;
						}
						else if( !c->GetRegion()->CanCastAggressive() )
						{
							// Target is in a region where casting aggressive spells is forbidden
							if( validSocket )
							{
								s->SysMessage( 709 ); // You can't cast in town!
							}
							return;
						}
						if( c->GetNpcAiType() == AI_PLAYERVENDOR || c->IsInvulnerable() )
						{
							if( validSocket )
							{
								s->SysMessage( 713 ); // They are invulnerable merchants!
							}
							return;
						}
						Combat->AttackTarget( caster, c );
					}
					if( spells[curSpell].SpellReflectable() )
					{
						if( CheckMagicReflect( c ))
						{
							src = c;
							c = caster;
						}
					}

					// Don't play sound and effects for these direct damagespells - it's handled
					// in effect.cpp after their damage delay timer has expired. Dispel is also a special case
					if( curSpell != 5 && curSpell != 12 && curSpell != 18 && curSpell != 30 && curSpell != 41
						&& curSpell != 42 && curSpell != 43 && curSpell != 49 && curSpell != 51 && curSpell != 55 )
					{
						if( caster->IsNpc() )
						{
							PlaySound( c, curSpell );
						}
						else
						{
							PlaySound( src, curSpell );
						}

						DoMoveEffect( curSpell, c, src );
						DoStaticEffect( c, curSpell );
					}
					switch( curSpell )
					{
						case 1:  // Clumsy
						case 3:  // Feeblemind
						case 4:  // Heal 2-26-00 used OSI values from UO book
						case 5:  // Magic Arrow
						case 6:  // Night Sight
						case 7:	 // Reactive Armor
						case 8:  // Weaken
						case 9:  // Agility
						case 10: // Cunning
						case 11: // Cure
						case 12: // Harm
						case 15: // Protection
						case 16: // Strength
						case 17: // Bless
						case 18: // Fireball
						case 20: // Poison
						case 27: // Curse
						case 29: // Greater Heal
						case 30: // Lightning
						case 31: // Mana drain
						case 37: // Mind Blast
						case 38: // Paralyze
						case 41: // Dispel
						case 42: // Energy Bolt
						case 43: // Explosion
						case 44: // Invisibility
						case 51: // Flamestrike
						case 53: // Mana Vampire
						case 59: // Resurrection
						{
							// Check for onSpellTargetSelect on spell caster
							scriptTriggers.clear();
							scriptTriggers.shrink_to_fit();
							scriptTriggers = caster->GetScriptTriggers();
							for( auto scriptTrig : scriptTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( scriptTrig );
								if( toExecute != nullptr )
								{
									auto retVal = toExecute->OnSpellTargetSelect( caster, c, curSpell );
									if( retVal == 1 )
									{
										break;
									}
									else if( retVal == 2 )
									{
										return;
									}
								}
							}

							// Check for onSpellTarget event on spell target
							scriptTriggers.clear();
							scriptTriggers.shrink_to_fit();
							scriptTriggers = c->GetScriptTriggers();
							for( auto scriptTrig : scriptTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( scriptTrig );
								if( toExecute != nullptr )
								{
									auto retVal = toExecute->OnSpellTarget( c, caster, curSpell );
									if( retVal == 1 )
									{
										break;
									}
									else if( retVal == 2 )
									{
										return;
									}
								}
							}

							// All ok, continue casting spell
							(*(( MAGIC_CHARFUNC )magic_table[curSpell-1].mag_extra ))( caster, c, src, curSpell );
							break;
						}
						case 46:	// Mass cure
						case 25:	// Arch Cure
						case 26:	// Arch Protection
						case 49:	// Chain Lightning
						case 54:	// Mass Dispel
						case 55:	// Meteor Swarm
						{
							// Check for onSpellTargetSelect on spell caster
							scriptTriggers.clear();
							scriptTriggers.shrink_to_fit();
							scriptTriggers = caster->GetScriptTriggers();
							for( auto scriptTrig : scriptTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( scriptTrig );
								if( toExecute != nullptr )
								{
									auto retVal = toExecute->OnSpellTargetSelect( caster, c, curSpell );
									if( retVal == 1 )
									{
										break;
									}
									else if( retVal == 2 )
									{
										return;
									}
								}
							}

							// Check for onSpellTarget on spell target
							scriptTriggers.clear();
							scriptTriggers.shrink_to_fit();
							scriptTriggers = c->GetScriptTriggers();
							for( auto scriptTrig : scriptTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( scriptTrig );
								if( toExecute != nullptr )
								{
									auto retVal = toExecute->OnSpellTarget( c, caster, curSpell );
									if( retVal == 1 )
									{
										break;
									}
									else if( retVal == 2 )
									{
										return;
									}
								}
							}
							(*(( MAGIC_TESTFUNC )magic_table[curSpell-1].mag_extra ))( s, caster, c, src, curSpell );
							break;
						}
						default:
							Console.Error( oldstrutil::format( " Unknown CharacterTarget spell %i, magic.cpp", curSpell ));
							break;
					}
				}
				else if( validSocket )
				{
					s->SysMessage( 683 ); // There seems to be something in the way.
				}
			}
			else if( validSocket )
			{
				s->SysMessage( 715 ); // That is not a person.
			}
		}
		else if( spells[curSpell].RequireLocTarget() )
		{
			SI16 x = 0;
			SI16 y = 0;
			SI08 z=0;
			CBaseObject *getTarg = nullptr;
			if( !caster->IsNpc() )
			{
				if( s->GetDWord( 7 ) != INVALIDSERIAL )
				{
					SERIAL targSer = s->GetDWord( 7 );
					if( targSer >= BASEITEMSERIAL )
					{
						getTarg = CalcItemObjFromSer( targSer );
					}
					else
					{
						getTarg = CalcCharObjFromSer( targSer );
					}
				}
				else
				{
					x = s->GetWord( 11 );
					y = s->GetWord( 13 );
					z = s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ));
				}
			}
			else
			{
				getTarg = caster->GetTarg();
			}

			if( ValidateObject( getTarg ))
			{
				if( !getTarg->CanBeObjType( OT_ITEM ) || (static_cast<CItem *>(getTarg))->GetCont() == nullptr )
				{
					x = getTarg->GetX();
					y = getTarg->GetY();
					z = getTarg->GetZ();
				}
				else if( validSocket )
				{
					s->SysMessage( 717 ); // That is not a valid location.
					return;
				}
			}

			if( spells[curSpell].AggressiveSpell() )
			{
				CTownRegion *targetRegion = CalcRegionFromXY( x, y, caster->WorldNumber(), caster->GetInstanceId() );
				if( targetRegion->IsSafeZone() )
				{
					// Target location is in a region where hostile actions are forbidden, disallow
					if( validSocket )
					{
						s->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
					}
					return;
				}
				else if( !targetRegion->CanCastAggressive() )
				{
					// Target location is in a region where casting aggressive spells is forbidden, disallow
					if( validSocket )
					{
						s->SysMessage( 709 ); // You can't cast in town!
					}
					return;
				}
			}

			// Check distance to target location vs CombatMaxSpellRange
			UI16 targDist;
			Point3_st targLocation = Point3_st( x, y, z );
			Point3_st difference	= caster->GetLocation() - targLocation;
			targDist			= static_cast<UI16>( difference.Mag() );
			if( targDist > cwmWorldState->ServerData()->CombatMaxSpellRange() )
			{
				if( validSocket )
				{
					s->SysMessage( 393 ); // That is too far away.
				}
				return;
			}

			if( LineOfSight( s, caster, x, y, ( z + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
			{
				if( spells[curSpell].FieldSpell() )
				{
					UI08 j = GetFieldDir( caster, x, y );
					PlaySound( src, curSpell );
					DoStaticEffect( src, curSpell );
					(*(( MAGIC_FIELDFUNC )magic_table[curSpell-1].mag_extra ))( s, caster, j, x, y, z, curSpell );
				}
				else if( spells[curSpell].RequireNoTarget() )
				{
					if( curSpell != 48 )
					{
						PlaySound( src, curSpell );
						DoStaticEffect( src, curSpell );
					}
					switch( curSpell )
					{
						case 2: //create food
						case 22:// Teleport
						case 25:// Arch Cure
						case 26:// Arch Protection
						case 33:// Blade Spirits
						case 34:// Dispel Field
						case 40:// Summon Creature
						case 46:// Mass curse
						case 48:// Reveal
						case 49:// Chain Lightning
						case 54:// Mass Dispel
						case 55:// Meteor Swarm
						case 60:// Summon Air Elemental
						case 61:// Summon Daemon
						case 62:// Summon Earth Elemental
						case 63:// Summon Fire Elemental
						case 64:// Summon water Elemental
						case 65:// Summon Hero
						case 58:// Energy Vortex
							(*(( MAGIC_LOCFUNC )magic_table[curSpell-1].mag_extra ))( s, caster, x, y, z, curSpell );
							break;
						default:
							Console.Error(oldstrutil::format( " Unknown LocationTarget spell %i", curSpell ));
							break;
					}
				}
			}
			else if( validSocket )
			{
				s->SysMessage( 717 ); // That is not a valid location.
			}
		}
		else if( spells[curSpell].RequireItemTarget() && validSocket )
		{
			i = CalcItemObjFromSer( s->GetDWord( 7 ));
			if( ValidateObject( i ))
			{
				auto iCont = i->GetCont();
				if( iCont != nullptr && FindItemOwner( i ) != caster )
				{
					s->SysMessage( 718 ); // You can't cast on an item that far away!
					return;
				}

				if( iCont == nullptr && !ObjInRange( caster, i, cwmWorldState->ServerData()->CombatMaxSpellRange() ))
				{
					s->SysMessage( 718 ); // You can't cast on an item that far away!
					return;
				}

				if( iCont == nullptr && !LineOfSight( s, caster, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) && !caster->IsGM() )
				{
					s->SysMessage( 683 ); // There seems to be something in the way.
					return;
				}

				switch( curSpell )
				{
					case 13:	// Magic Trap
					case 14:	// Magic Untrap
					case 19:	// Magic Lock
					case 23:	// Magic Unlock
					case 21:	// Telekinesis
						(*(( MAGIC_ITEMFUNC )magic_table[curSpell-1].mag_extra))( s, caster, i, curSpell );
						break;
					default:
						Console.Error( oldstrutil::format(" Unknown ItemTarget spell %i, magic.cpp", curSpell ));
						break;
				}
			}
			else
			{
				s->SysMessage( 711 ); // That is not a valid item.
			}
		}
		else if( validSocket )
		{
			s->SysMessage( 720 ); // Can't cope with this spell, requires a target but it doesn't specify what type.
		}
		return;
	}
	else if( spells[curSpell].RequireNoTarget() )
	{
		// non targetted spells
		PlaySound( src, curSpell );
		DoStaticEffect( src, curSpell );
		switch( curSpell )
		{
			case 2:		// Create food
			case 40:	// Summon Monster	// summon monster is 40, not 33
			case 35:
			case 36:	// Magic Reflection
			case 56:	// Poly Morph
			case 57:	// Earthquake
			case 60:	// Summon Air Elemental
			case 61:	// Summon Daemon
			case 62:	// Summon Earth Elemental
			case 63:	// Summon Fire Elemental
			case 64:	// Summon Water Elemental
			case 65:	// Summon Hero
			case 67:
				(*(( MAGIC_NOFUNC )magic_table[curSpell-1].mag_extra ))( s, caster, curSpell );
				break;
			default:
				Console.Error( oldstrutil::format( " Unknown NonTarget spell %i, magic.cpp", curSpell ));
				break;
		}
		return;
	}

}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::LoadScript()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads spell data from spell DFNs
//|
//|	Notes		-	Avoid multiple reading of the spell script every time a spell is
//|					cast to avoid crippling the server when a mage enters combat
//o------------------------------------------------------------------------------------------------o
void CMagic::LoadScript( void )
{
	spells.resize( 0 );

	// for some strange reason, spells go from index 1 to SPELL_MAX and
	// apparently index 0 is left unused
	spells.resize( SPELL_MAX + 1 );

	std::string spEntry;
	std::string tag, data, UTag;
	UI08 i = 0;
	for( auto &spellScp : FileLookup->ScriptListings[spells_def] )
	{
		if( spellScp == nullptr )
			continue;

		for( const auto &[spEntry, SpellLoad]:spellScp->collection() )
		{
			if( SpellLoad == nullptr )
				continue;

			auto ssecs = oldstrutil::sections( spEntry, " " );
			if( ssecs[0] == "SPELL" )
			{
				i = static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 ));
				if( i <= SPELL_MAX )
				{
					++spellCount;
					spells[i].Enabled( false );
					Reag_st *mRegs = spells[i].ReagantsPtr();

					//Console.Log( "Spell number: %i", "spell.log", i ); // Disabled for performance reasons
					for( const auto &sec : SpellLoad->collection() )
					{
						tag = sec->tag;
						data = sec->data;
						UTag = oldstrutil::upper( tag );
						data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
						//Console.Log( "Tag: %s\tData: %s", "spell.log", UTag.c_str(), data.c_str() ); // Disabled for performance reasons
						switch(( UTag.data()[0] ))
						{
							case 'A':
								if( UTag == "ACTION" )
								{
									spells[i].Action( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
								}
								else if( UTag == "ASH" )
								{
									mRegs->ash =static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								break;
							case 'B':
								if( UTag == "BASEDMG" )
								{
									spells[i].BaseDmg( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
								}
								else if( UTag == "BATWING" )
								{
									mRegs->batwing =static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								break;
							case 'C':
								if( UTag == "CIRCLE" )
								{
									spells[i].Circle( static_cast<UI08>( std::stoul( data, nullptr, 0 )));
								}
								break;
							case 'D':
								if( UTag == "DAMAGEDELAY" )
								{
									spells[i].DamageDelay( static_cast<R32>( std::stof( data )));
								}
								else if( UTag == "DELAY" )
								{
									spells[i].Delay( static_cast<R32>( std::stof( data )));
								}
								else if( UTag == "DAEMONBLOOD" )
								{
									mRegs->daemonblood =static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								else if( UTag == "DRAKE" )
								{
									mRegs->drake = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								break;
							case 'E':
								if( UTag == "ENABLE" ) // presence of enable is enough to enable it
								{
									spells[i].Enabled( std::stoul( data, nullptr, 0 ) != 0 );
								}
								break;
							case 'F':
								if( UTag == "FLAGS" )
								{
									auto ssecs = oldstrutil::sections( data, " " );
									if( ssecs.size() > 1 )
									{
										// This is used to load flags from old-style spells.dfn, where flags are written as FLAGS=## ## instead of FLAGS=0x####
										spells[i].Flags((( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 16 ))) << 8 ) |
												    static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 16 )));
									}
									else
									{
										spells[i].Flags( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
									}
								}
								break;
							case 'G':
								if( UTag == "GARLIC" )
								{
									mRegs->garlic  = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								else if( UTag == "GINSENG" )
								{
									mRegs->ginseng = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								else if( UTag == "GRAVEDUST" )
								{
									mRegs->gravedust =static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								break;
							case 'H':
								if( UTag == "HISKILL" )
								{
									spells[i].HighSkill( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
								}
								else if( UTag == "HEALTH" )
								{
									spells[i].Health( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
								}
								break;
							case 'L':
								if( UTag == "LOSKILL" )
								{
									spells[i].LowSkill( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
								}
								break;
							case 'M':
								if( UTag == "MANA" )
								{
									spells[i].Mana( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
								}
								else if( UTag == "MANTRA" )
								{
									spells[i].Mantra( data );
								}
								else if( UTag == "MOSS" )
								{
									mRegs->moss = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								else if( UTag == "MOVEFX" )
								{
									auto ssecs = oldstrutil::sections( data, " " );
									if( ssecs.size() > 1 )
									{
										CMagicMove *mv = spells[i].MoveEffectPtr();
										mv->Effect( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 16 )), static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 16 )));
										mv->Speed( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 16 )));
										mv->Loop( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 16 )));
										mv->Explode( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[4], "//" )), nullptr, 16 )));
									}
								}
								break;
							case 'N':
								if( UTag == "NOXCRYSTAL" )
								{
									mRegs->noxcrystal =static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								break;
							case 'P':
								if( UTag == "PEARL" )
								{
									mRegs->pearl = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								else if( UTag == "PIGIRON" )
								{
									mRegs->pigiron = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								break;
								break;
							case 'R':
								if( UTag == "RECOVERYDELAY" )
								{
									spells[i].RecoveryDelay( static_cast<R32>( std::stof( data )));
								}
								break;
							case 'S':
								if( UTag == "SHADE" )
								{
									mRegs->shade = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								else if( UTag == "SILK" )
								{
									mRegs->silk = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
								}
								else if( UTag == "SOUNDFX" )
								{
									auto ssecs = oldstrutil::sections( data, " " );
									if( ssecs.size() > 1 )
									{
										// This is used to load sounds from old-style spells.dfn, where soundfx are written as SOUNDFX=## ## instead of SOUNDFX=0x####
										spells[i].Effect(((static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 16 )) << 8 ) |
													 static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 16 ))));
									}
									else
									{
										spells[i].Effect( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
									}
								}
								else if( UTag == "STATFX" )
								{
									auto ssecs = oldstrutil::sections( data, " " );
									if( ssecs.size() > 1 )
									{
										CMagicStat *stat = spells[i].StaticEffectPtr();

										stat->Effect( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 16 )), static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 16 )));
										stat->Speed( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 16 )));
										stat->Loop( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 16 )));
									}
								}
								else if( UTag == "SCLO" )
								{
									spells[i].ScrollLow( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
								}
								else if( UTag == "SCHI" )
								{
									spells[i].ScrollHigh(  static_cast<SI16>( std::stoi( data, nullptr, 0 )));
								}
								else if( UTag == "STAMINA" )
								{
									spells[i].Stamina(  static_cast<SI16>( std::stoi( data, nullptr, 0 )));
								}
								break;
							case 'T':
								if( UTag == "TARG" )
								{
									spells[i].StringToSay( data );
								}
								else if( UTag == "TITHING" )
								{
									spells[i].Tithing(  static_cast<SI32>( std::stoi( data, nullptr, 0 )));
								}
								break;
						}
					}
				}
			}
		}
	}

#if defined( UOX_DEBUG_MODE )
	Console.Print( "Registering spells\n" );
#endif

	CJSMappingSection *spellSection = JSMapping->GetSection( SCPT_MAGIC );

	for( cScript *ourScript = spellSection->First(); !spellSection->Finished(); ourScript = spellSection->Next() )
	{
		if( ourScript )
		{
			ourScript->ScriptRegistration( "Spell" );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::DelReagents()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes the specified amount of reagents when a spell is cast
//o------------------------------------------------------------------------------------------------o
void CMagic::DelReagents( CChar *s, Reag_st reags )
{
	if( s->NoNeedReags() )
		return;

	DeleteItemAmount( s, reags.pearl, 0x0F7A );
	DeleteItemAmount( s, reags.moss, 0x0F7B );
	DeleteItemAmount( s, reags.garlic, 0x0F84 );
	DeleteItemAmount( s, reags.ginseng, 0x0F85 );
	DeleteItemAmount( s, reags.drake, 0x0F86 );
	DeleteItemAmount( s, reags.shade, 0x0F88 );
	DeleteItemAmount( s, reags.ash, 0x0F8C  );
	DeleteItemAmount( s, reags.silk, 0x0F8D );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::PlaySound()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the SFX associated with a spell that is being cast
//o------------------------------------------------------------------------------------------------o
void CMagic::PlaySound( CChar *source, SI32 num )
{
	if( spells[num].Effect() != INVALIDID )
	{
		Effects->PlaySound( source, spells[num].Effect() );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::DoStaticEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the static FX associated with a spell that is being cast
//o------------------------------------------------------------------------------------------------o
void CMagic::DoStaticEffect( CChar *source, SI32 num )
{
	CMagicStat temp = spells[num].StaticEffect();
	if( temp.Effect() != INVALIDID )
	{
		Effects->PlayStaticAnimation( source, temp.Effect(), temp.Speed(), temp.Loop() );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::DoMoveEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the moving FX associated with a spell that is being cast
//o------------------------------------------------------------------------------------------------o
void CMagic::DoMoveEffect( SI32 num, CBaseObject *target, CChar *source )
{
	CMagicMove temp = spells[num].MoveEffect();

	if( temp.Effect() != INVALIDID )
	{
		Effects->PlayMovingAnimation( source, target, temp.Effect(), temp.Speed(), temp.Loop(), ( temp.Explode() == 1 ));
	}
}

void HandleCommonGump( CSocket *mSock, CScriptSection *gumpScript, UI16 gumpIndex );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::PolymorphMenu()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the polymorph menu to allow player to choose which creature to poly into
//o------------------------------------------------------------------------------------------------o
void CMagic::PolymorphMenu( CSocket *s, UI16 gmindex )
{
	std::string sect			= "POLYMORPHMENU " + oldstrutil::number( gmindex );
	CScriptSection *polyStuff	= FileLookup->FindEntry( sect, menus_def );
	if( polyStuff == nullptr )
		return;

	HandleCommonGump( s, polyStuff, gmindex );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::Polymorph()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Polymorph spell to caster
//o------------------------------------------------------------------------------------------------o
void CMagic::Polymorph( CSocket *s, UI16 polyId )
{
	UI08 id1		= static_cast<UI08>( polyId >> 8 );
	UI08 id2		= static_cast<UI08>( polyId % 256 );
	CChar *mChar	= s->CurrcharObj();

	// store our original ID
	mChar->SetOrgId( mChar->GetId() );
	Effects->PlaySound( mChar, 0x020F );
	// Temp effect will actually switch our body for us
	Effects->TempEffect( mChar, mChar, 18, id1, id2, 0 );
	mChar->IsPolymorphed( true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::LogSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out spell casting details to spell.log file
//o------------------------------------------------------------------------------------------------o
void CMagic::LogSpell( std::string spell, CChar *player1, CChar *player2, const std::string &extraInfo )
{
	if( spell.empty() || !ValidateObject( player1 ))
		return;

	std::string logName = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + "spell.log";
	std::ofstream logDestination;
	logDestination.open( logName.c_str(), std::ios::out | std::ios::app );
	if( !logDestination.is_open() )
	{
		Console.Error(oldstrutil::format( "Unable to open spell log file %s!", logName.c_str() ));
		return;
	}
	char dateTime[1024];
	RealTime( dateTime );

	logDestination << "[" << dateTime << "] ";

	std::string casterName = GetNpcDictName( player1, nullptr, NRS_SYSTEM );
	logDestination << casterName << " (serial: " << std::hex << player1->GetSerial() << " ) ";
	logDestination << "cast spell <" << spell << "> ";
	if( ValidateObject( player2 ))
	{
		std::string targetName = GetNpcDictName( player2, nullptr, NRS_SYSTEM );
		logDestination << "on player " << targetName << " (serial: " << player2->GetSerial() << " ) ";
	}
	logDestination << "Extra Info: " << extraInfo << std::endl;
	logDestination.close();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::Register()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers a spell in the JS Engine
//o------------------------------------------------------------------------------------------------o
void CMagic::Register( cScript *toRegister, SI32 spellNumber, bool isEnabled )
{
#if defined( UOX_DEBUG_MODE )
	Console.Print( oldstrutil::format( "Registering spell number %i\n", spellNumber ));
#endif
	if( spellNumber < 0 || static_cast<size_t>( spellNumber ) >= spells.size() )
		return;

	if( toRegister == nullptr )
		return;

	spells[spellNumber].JSScript( JSMapping->GetScriptId( toRegister->Object() ));
	spells[spellNumber].Enabled( isEnabled );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMagic::SetSpellStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the enabled state of a given spell
//o------------------------------------------------------------------------------------------------o
void CMagic::SetSpellStatus( SI32 spellNumber, bool isEnabled )
{
	if( spellNumber < 0 || static_cast<size_t>( spellNumber ) >= spells.size() )
		return;

	spells[spellNumber].Enabled( isEnabled );
}
