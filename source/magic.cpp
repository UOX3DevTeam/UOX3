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

cMagic *Magic = nullptr;

#define SPELL_MAX 68 // use define for now; can make autocount later

// Look up spell-names from dictionary-files
const MagicTable_s magic_table[] = {
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, UI08 srcWorld, SI16 trgX, SI16 trgY, SI08 trgZ, UI08 trgWorld )
//| Date		-	17th September, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	It will construct 2 linked gates, one at srcX / srcY / srcZ and another at trgX / trgY / trgZ
//o-----------------------------------------------------------------------------------------------o
void SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, UI08 srcWorld, SI16 trgX, SI16 trgY, SI08 trgZ, UI08 trgWorld )
{
	CItem *g1 = Items->CreateItem( nullptr, caster, 0x0F6C, 1, 0, OT_ITEM );
	if( ValidateObject( g1 ) )
	{
		g1->SetDecayable( true );
		g1->SetType( IT_GATE );
		g1->SetLocation( srcX, srcY, srcZ, srcWorld, g1->GetInstanceID() );
		g1->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_GATE ) );
		g1->SetDir( 1 );

		CItem *g2 = Items->CreateItem( nullptr, caster, 0x0F6C, 1, 0, OT_ITEM );
		if( ValidateObject( g2 ) )
		{
			g2->SetDecayable( true );
			g2->SetType( IT_GATE );
			g2->SetLocation( trgX, trgY, trgZ, trgWorld, g2->GetInstanceID() );
			g2->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_GATE ) );
			g2->SetDir( 1 );

			g2->SetTempVar( CITV_MOREX, g1->GetSerial() );
			g1->SetTempVar( CITV_MOREX, g2->GetSerial() );
		}
		else
			g1->Delete();
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool FieldSpell( CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 fieldDir )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates rows of items for field spells like firewalls, wall of stone, etc
//o-----------------------------------------------------------------------------------------------o
bool FieldSpell( CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 fieldDir )
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
	UI16 instanceID = caster->GetInstanceID();
	for( UI08 j = 0; j < 5; ++j )		// how about we make this 5, huh?  missing part of the field
	{
		if( id != 0x0080 || ( id == 0x0080 && !Movement->CheckForCharacterAtXYZ( caster, fx[j], fy[j], z )))
		{
			i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
			if( i != nullptr )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 15 )) );
				i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
				i->SetTempVar( CITV_MOREY, caster->GetSerial() );
				i->SetLocation( fx[j], fy[j], Map->Height( fx[j], fy[j], z, worldNumber, instanceID ) );
				i->SetDir( 29 );
				i->SetMovable( 2 );
			}
		}
	}
	return true;
}

bool splClumsy( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/level1targ.js
	return false;
}
bool splCreateFood( CSocket *sock, CChar *caster, SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/createfood.js
	return false;
}
bool splFeeblemind( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/level1targ.js
	return false;
}
bool splHeal( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/level1targ.js
	return false;
}
bool splMagicArrow( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	// This spell is now handled in JS/MAGIC/level1targ.js
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splNightSight( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Night Sight spell
//o-----------------------------------------------------------------------------------------------o
bool splNightSight( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	Effects->tempeffect( src, target, 2, 0, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splReactiveArmor( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Reactive Armor spell
//o-----------------------------------------------------------------------------------------------o
bool splReactiveArmor( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	Effects->tempeffect( src, target, 15, caster->GetSkill( MAGERY )/100, 0, 0 );
	target->SetReactiveArmour( true );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splWeaken( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Weaken spell
//o-----------------------------------------------------------------------------------------------o
bool splWeaken( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	Effects->tempeffect( src, target, 5, caster->GetSkill( MAGERY )/100, 0, 0);
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splAgility( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Agility spell
//o-----------------------------------------------------------------------------------------------o
bool splAgility( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	Effects->tempeffect( src, target, 6, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splCunning( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Cunning spell
//o-----------------------------------------------------------------------------------------------o
bool splCunning( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	Effects->tempeffect( src, target, 7, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splCure( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Cure spell
//o-----------------------------------------------------------------------------------------------o
bool splCure( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	target->SetPoisoned( 0 );
	target->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->GetUICurrentTime() );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splHarm( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Harm spell
//o-----------------------------------------------------------------------------------------------o
bool splHarm( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Magic->MagicDamage( target, spellDamage, caster, COLD );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMagicTrap( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Magic Trap spell
//o-----------------------------------------------------------------------------------------------o
bool splMagicTrap( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
{
	if( target->IsContType() && target->GetID() != 0x0E75 )
	{
		target->SetTempVar( CITV_MOREZ, calcserial( 1, caster->GetSkill( MAGERY ) / 20, caster->GetSkill( MAGERY ) / 10, 0 ) );
		Effects->PlaySound( target, 0x01F0 );
	}
	else
		sock->sysmessage( 663 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMagicUntrap( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Magic Untrap spell
//o-----------------------------------------------------------------------------------------------o
bool splMagicUntrap( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
{
	if( target->IsContType() )
	{
		if( target->GetTempVar( CITV_MOREZ, 1 ) == 1 )
		{
			if( RandomNum( 1, 100 ) <= 50 + ( caster->GetSkill( MAGERY )/10) - target->GetTempVar( CITV_MOREZ, 3 ) )
			{
				target->SetTempVar( CITV_MOREZ, 0 );
				Effects->PlaySound(target, 0x01F1 );
				sock->sysmessage( 664 );
			}
			else
				sock->sysmessage( 665 );
		}
		else
			sock->sysmessage( 667 );
	}
	else
		sock->sysmessage( 668 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splProtection( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Protection spell
//o-----------------------------------------------------------------------------------------------o
bool splProtection( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	Effects->tempeffect( src, target, 21, caster->GetSkill( MAGERY )/10, 0, 0 );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splStrength( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Strength spell
//o-----------------------------------------------------------------------------------------------o
bool splStrength( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	Effects->tempeffect( src, target, 8, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splBless( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Bless spell
//o-----------------------------------------------------------------------------------------------o
bool splBless( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI32 j = caster->GetSkill( MAGERY )/100;
	Effects->tempeffect( src, target, 11, j, j, j);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splFireball( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Fireball spell
//o-----------------------------------------------------------------------------------------------o
bool splFireball( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Magic->MagicDamage( target, spellDamage, caster, HEAT );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMagicLock( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Magic Lock spell
//o-----------------------------------------------------------------------------------------------o
bool splMagicLock( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
{
	ItemTypes type = target->GetType();
	if( ( type == IT_CONTAINER || type == IT_DOOR || type == IT_SPAWNCONT ) && ( target->GetID( 1 ) != 0x0E || target->GetID( 2 ) != 0x75 ) )
	{
		switch( type )
		{
			case IT_CONTAINER:  target->SetType( IT_LOCKEDCONTAINER );	break;
			case IT_DOOR:		target->SetType( IT_LOCKEDDOOR );		break;
			case IT_SPAWNCONT:	target->SetType( IT_LOCKEDSPAWNCONT );	break;
			default:
				Console.error( "Fallout of switch statement without default. magic.cpp, magiclocktarget()" );
				break;
		}
		Effects->PlaySound( target, 0x0200 );
	}
	else
		sock->sysmessage( 669 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splPoison( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Poison spell
//o-----------------------------------------------------------------------------------------------o
bool splPoison( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	if( Magic->CheckResist( caster, target, 1 ) )
		return false;

	target->SetPoisoned( 2 );
	target->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POISON ) );

	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splTelekinesis( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Telekinesis spell
//o-----------------------------------------------------------------------------------------------o
bool splTelekinesis( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
{
	if( target->IsContType() )
	{
		if( target->GetTempVar( CITV_MORE, 1 ) == 1 )
		{
			target->SetTempVar( CITV_MORE, 0 );
			Effects->PlaySound( target, 0x01FA, true);
		}
		else
			sock->sysmessage( 667 );
	}
	else
		sock->sysmessage( 668 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splTeleport( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Teleport spell
//o-----------------------------------------------------------------------------------------------o
bool splTeleport( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	CMultiObj *m = findMulti( x, y, z, caster->WorldNumber(), caster->GetInstanceID() );
	if( ValidateObject( m ) && m->GetOwnerObj() != caster && !caster->IsNpc() )
	{
		sock->sysmessage( 670 );
		return false;
	}
	if( !caster->IsNpc() )
	{
		CTile& tile = Map->SeekTile( sock->GetWord( 0x11 ) );
		if( (!strcmp( tile.Name(), "water")) || tile.CheckFlag( TF_WET ) )
		{
			sock->sysmessage( 671 );
			return false;
		}
		if( tile.CheckFlag( TF_ROOF ) )	// slanted roof tile!!! naughty naughty
		{
			sock->sysmessage( 672 );
			return false;
		}
	}
	if( Weight->isOverloaded( caster ) )
	{
		if( caster->GetMana() >= 20 )
			caster->SetMana( (SI16)(caster->GetMana() - 20) );
		else
		{
			sock->sysmessage( 1386 );
			return false;
		}
	}
	caster->SetLocation( x, y, z );
	Magic->doStaticEffect( caster, 22 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splUnlock( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Unlock spell
//o-----------------------------------------------------------------------------------------------o
bool splUnlock( CSocket *sock, CChar *caster, CItem *target, SI08 curSpell )
{
	if( target->isDivineLocked() )
	{
		sock->sysmessage( 673 );
		return false;
	}
	if( target->GetType() == IT_LOCKEDCONTAINER )
	{
		target->SetType( IT_CONTAINER );
		sock->sysmessage( 674 );
		target->RemoveFromSight();
		target->Update();
	}
	else if( target->GetType() == IT_LOCKEDSPAWNCONT )
	{
		target->SetType( IT_SPAWNCONT );
		sock->sysmessage( 675 );
		target->RemoveFromSight();
		target->Update();
	}
	else if( target->GetType() == IT_CONTAINER || target->GetType() == IT_SPAWNCONT || target->GetType() == IT_LOCKEDSPAWNCONT || target->GetType() == IT_TRASHCONT )
		sock->sysmessage( 676 );
	else if( target->GetType() == IT_DOOR )
		sock->sysmessage( 677 );
	else
		sock->sysmessage( 678 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splWallOfStone( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Wall of Stone spell
//o-----------------------------------------------------------------------------------------------o
bool splWallOfStone( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	return FieldSpell( caster, 0x0080, x, y, z, fieldDir );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ArchCureStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Arch Cure spell to an individual target
//o-----------------------------------------------------------------------------------------------o
void ArchCureStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	target->SetPoisoned( 0 );
	target->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->GetUICurrentTime() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splArchCure( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Arch Cure as an area affect spell
//o-----------------------------------------------------------------------------------------------o
bool splArchCure( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &ArchCureStub, curSpell );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ArchProtectionStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Arch Protection spell to an individual target
//o-----------------------------------------------------------------------------------------------o
void ArchProtectionStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	Magic->playSound( target, 26 );
	Magic->doStaticEffect( target, 15 );	// protection
	Effects->tempeffect( caster, target, 21, caster->GetSkill( MAGERY )/10, 0, 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splArchProtection( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Arch Protection as an area affect spell
//o-----------------------------------------------------------------------------------------------o
bool splArchProtection( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &ArchProtectionStub, curSpell );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splCurse( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Curse spell
//o-----------------------------------------------------------------------------------------------o
bool splCurse( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI32 j = caster->GetSkill( MAGERY ) / 100;
	Effects->tempeffect(caster, target, 12, j, j, j);
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splFireField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Fire Field as a field spell
//o-----------------------------------------------------------------------------------------------o
bool splFireField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( fieldDir == 0 )
		return FieldSpell( caster, 0x398C, x, y, z, fieldDir );
	else
		return FieldSpell( caster, 0x3996, x, y, z, fieldDir );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splGreaterHeal( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Greater Heal spell
//o-----------------------------------------------------------------------------------------------o
bool splGreaterHeal( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI32 srcHealth		= target->GetHP();
	SI32 baseHealing	= Magic->spells[curSpell].BaseDmg();
	SI32 j				= caster->GetSkill( MAGERY ) / 30 + HalfRandomNum( baseHealing );

	target->Heal( j, caster );
	Magic->SubtractHealth( caster, std::min( target->GetStrength(), (SI16)(srcHealth + j) ), 29 );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splLightning( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Lightning spell
//o-----------------------------------------------------------------------------------------------o
bool splLightning( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Effects->bolteffect( target );
	Magic->MagicDamage( target, spellDamage, caster, LIGHTNING );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splManaDrain( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mana Drain spell
//o-----------------------------------------------------------------------------------------------o
bool splManaDrain( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	if( !Magic->CheckResist( caster, target, 4 ) )
	{
		target->SetMana( target->GetMana() - caster->GetSkill( MAGERY )/35 );
		if( target->GetMana() < 0 )
			target->SetMana( 0 );
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splRecall( CSocket *sock, CChar *caster, CItem *i, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Recall spell
//o-----------------------------------------------------------------------------------------------o
bool splRecall( CSocket *sock, CChar *caster, CItem *i, SI08 curSpell )
{
	// No recall if too heavy, GMs excempt
	if( Weight->isOverloaded( caster ) && !cwmWorldState->ServerData()->TravelSpellsWhileOverweight() && ( !caster->IsCounselor() && !caster->IsGM() ) )
	{
		sock->sysmessage( 680 ); // You are too heavy to do that!
		sock->sysmessage( 681 ); // You feel drained from the attempt.
		return false;
	}
	else if( i->GetType() == 7 )
	{
		// Player recalled off a key
		CMultiObj *shipMulti = calcMultiFromSer( i->GetTempVar( CITV_MORE ) );
		if( ValidateObject( shipMulti ) && shipMulti->CanBeObjType( OT_BOAT ) )
		{
			if(( shipMulti->WorldNumber() == caster->WorldNumber() || cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() ) && shipMulti->GetInstanceID() == caster->GetInstanceID() )
			{
				caster->SetLocation( shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3, shipMulti->WorldNumber(), shipMulti->GetInstanceID() );
				return true;
			}
			else
			{
				sock->sysmessage( 2063 ); // You are unable to recall to your ship - it might be in another world!
				return false;
			}
		}
		else
		{
			sock->sysmessage( 2065 ); // You can only recall off of recall runes or valid ship keys!
			return false;
		}
	}
	else
	{
		// Check if rune was marked in a multi - if so, try to take user directly there
		TAGMAPOBJECT runeMore = i->GetTag( "multiSerial" );
		if( runeMore.m_StringValue != "" )
		{
			SERIAL mSerial = strutil::value<SERIAL>( runeMore.m_StringValue );
			if( mSerial != 0 && mSerial != INVALIDSERIAL )
			{
				CMultiObj *shipMulti = calcMultiFromSer( mSerial );
				if( ValidateObject( shipMulti ) && shipMulti->CanBeObjType( OT_BOAT ) )
				{
					if(( shipMulti->WorldNumber() == caster->WorldNumber() || cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() ) && shipMulti->GetInstanceID() == caster->GetInstanceID() )
					{
						caster->SetLocation( shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3, shipMulti->WorldNumber(), shipMulti->GetInstanceID() );
						return true;
					}
					else
					{
						sock->sysmessage( 2063 ); // You are unable to recall to your ship - it might be in another world!
						return false;
					}
				}
			}

			sock->sysmessage( 2062 ); // Unable to locate ship - it might have been dry-docked... or sunk!
			return false;
		}
		else if( i->GetTempVar( CITV_MOREX ) == 0 && i->GetTempVar( CITV_MOREY ) == 0 )
		{
			sock->sysmessage( 679 ); // That rune has not been marked yet!
			return false;
		}
		else
		{
			UI08 worldNum = static_cast<UI08>(i->GetTempVar( CITV_MORE ));
			if( !Map->MapExists( worldNum ) )
				worldNum = caster->WorldNumber();

			if( worldNum != caster->WorldNumber() )
			{
				if( cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() )
				{
					caster->SetLocation( static_cast<SI16>(i->GetTempVar( CITV_MOREX )), static_cast<SI16>(i->GetTempVar( CITV_MOREY )), static_cast<SI08>(i->GetTempVar( CITV_MOREZ )), worldNum, caster->GetInstanceID() );
					SendMapChange( caster->WorldNumber(), sock, false );
				}
				else
				{
					sock->sysmessage( 2061 ); // Travelling between worlds using Recall or Gate spells is not possible.
					return false;
				}
			}
			else
				caster->SetLocation( static_cast<SI16>(i->GetTempVar( CITV_MOREX )), static_cast<SI16>(i->GetTempVar( CITV_MOREY )), static_cast<SI08>(i->GetTempVar( CITV_MOREZ )), worldNum, caster->GetInstanceID() );
			sock->sysmessage( 682 ); // You have recalled from the rune.
			return true;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splBladeSpirits( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Blade Spirits spell (summons a Blade Spirit)
//o-----------------------------------------------------------------------------------------------o
bool splBladeSpirits( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	Magic->SummonMonster( sock, caster, 6, x, y, z );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splDispelField( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Dispel Field spell
//o-----------------------------------------------------------------------------------------------o
bool splDispelField( CSocket *sock, CChar *caster, SI08 curSpell )
{
	CItem *i = calcItemObjFromSer( sock->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		if( LineOfSight( sock, caster, i->GetX(), i->GetY(), ( i->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) || caster->IsGM() )
		{
			if( i->isDecayable() || i->isDispellable() )
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
			sock->sysmessage( 683 );
	}
	return true;
}
void setRandomName( CChar *s, const std::string& namelist );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splIncognito( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Incognito spell
//o-----------------------------------------------------------------------------------------------o
bool splIncognito( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( caster->IsIncognito() )
	{
		sock->sysmessage( 1636 );
		return false;
	}
	// ------ SEX ------
	caster->SetOrgID( caster->GetID() );
	UI08 randomGender = RandomNum( 0, 3 );
	switch( randomGender )
	{
		case 0:	caster->SetID( 0x0190 ); break;	// male, human
		case 1:	caster->SetID( 0x0191 ); break;	// female, human
		case 2:	caster->SetID( 0x025D ); break;	// male, elf
		case 3:	caster->SetID( 0x025E ); break;	// female, elf
	}

	// ------ NAME -----
	caster->SetOrgName( caster->GetName() );
	switch( randomGender )
	{
		case 0:	setRandomName( caster, "1" );	break; // get a name from human male list
		case 1:	setRandomName( caster, "2" );	break; // get a name from human female list
		case 2:	setRandomName( caster, "3" );	break; // get a name from elf male list
		case 3:	setRandomName( caster, "4" );	break; // get a name from elf female list
	}

	SI32 color	= RandomNum( 0x044E, 0x047D );
	CItem *j	= caster->GetItemAtLayer( IL_HAIR );
	if( ValidateObject( j ) )
	{
		caster->SetHairColour( j->GetColour() );
		caster->SetHairStyle( j->GetID() );
		SI32 rNum = RandomNum( 0, 9 );
		switch( rNum )
		{
			case 0:
			case 1:
			case 2:
				j->SetID( 0x3B + rNum, 2 );
				break;
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
				j->SetID( 0x44 + ( rNum - 3 ), 2 );
				break;
		}
		j->SetColour( color );
	}
	if( randomGender == 0 || randomGender == 2 )
	{
		j = caster->GetItemAtLayer( IL_FACIALHAIR );
		if( ValidateObject( j ) )
		{
			caster->SetBeardColour( j->GetColour() );
			caster->SetBeardStyle( j->GetID() );
			SI32 rNum2 = RandomNum( 0, 6 );
			switch( rNum2 )
			{
				case 0:
				case 1:
				case 2:
				case 3:
					j->SetID( 0x3E + rNum2, 2 );
					break;
				case 4:
				case 5:
				case 6:
					j->SetID( 0x4B + (rNum2 - 4), 2 );
					break;
			}
			j->SetColour( color );
		}
	}
	//only refresh once
	caster->SendWornItems( sock );
	Effects->PlaySound( caster, 0x0203 );
	Effects->tempeffect( caster, caster, 19, 0, 0, 0 );
	caster->IsIncognito( true );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMagicReflection( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Magic Reflection spell
//o-----------------------------------------------------------------------------------------------o
bool splMagicReflection( CSocket *sock, CChar *caster, SI08 curSpell )
{
	caster->SetPermReflected( true );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMindBlast( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mind Blast spell
//o-----------------------------------------------------------------------------------------------o
bool splMindBlast( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	SI16 baseDamage = 0;

	SI16 targetStats = target->GetIntelligence() + target->GetStrength() + target->GetDexterity();
	SI16 casterStats = caster->GetIntelligence() + caster->GetStrength() + caster->GetDexterity();

	//If target's int makes out a larger percentage of the total stats than the caster's, backfire spell onto caster
	if(( target->GetIntelligence() * 100 ) / targetStats > ( caster->GetIntelligence() * 100 ) / casterStats )
		target = src;

	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	baseDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = baseDamage;
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	//Damage should not exceed 60% of target maxhp
	if( spellDamage > ( target->GetMaxHP() * 0.6 ) )
		spellDamage = (SI16) roundNumber( target->GetMaxHP() * 0.6 );

	//Damage should not exceed basedamage from DFN + 20%
	if( spellDamage > baseDamage * 1.20 )
		spellDamage = baseDamage * 1.20;

	Magic->MagicDamage( target, spellDamage, src, COLD );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splParalyze( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Paralyze spell
//o-----------------------------------------------------------------------------------------------o
bool splParalyze( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	if( !Magic->CheckResist( caster, target, 7 ) )
		Effects->tempeffect( caster, target, 1, 0, 0, 0 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splPoisonField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Poison Field as a field spell
//o-----------------------------------------------------------------------------------------------o
bool splPoisonField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( fieldDir == 0 )
		return FieldSpell( caster, 0x3915, x, y, z, fieldDir );
	else
		return FieldSpell( caster, 0x3921, x, y, z, fieldDir );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splSummonCreature( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Creature spell (summons a creature)
//o-----------------------------------------------------------------------------------------------o
bool splSummonCreature( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 380 )
		return false;
	else
		Magic->SummonMonster( sock, caster, 0, x, y, z );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splDispel( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Dispel spell
//o-----------------------------------------------------------------------------------------------o
bool splDispel( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	if( target->IsDispellable() )
	{
		Effects->PlayStaticAnimation( target, 0x372A, 0x09, 0x06 );		// why the specifics here?
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

			target->Delete();
		}
		else
			HandleDeath( target, nullptr );
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splEnergyBolt( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Energy Bolt spell
//o-----------------------------------------------------------------------------------------------o
bool splEnergyBolt( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Magic->MagicDamage( target, spellDamage, caster, LIGHTNING );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splExplosion( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Explosion spell. Delay configurable in UOX.INI
//o-----------------------------------------------------------------------------------------------o
bool splExplosion( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Effects->tempeffect( src, target, 27, spellDamage, 0, 0 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splInvisibility( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Invisibility spell
//o-----------------------------------------------------------------------------------------------o
bool splInvisibility( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	target->SetVisible( VT_INVISIBLE );
	target->SetTimer( tCHAR_INVIS, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_INVISIBILITY ) );
	if( target->IsMurderer() )
		criminal( caster );

	//Stop autodefending against targets in combat once you turn invisible
	if( !target->IsNpc() )
		Combat->InvalidateAttacker( target );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMark( CSocket *sock, CChar *caster, CItem *i, SI08 curSpell )
//|	Date		-	09/22/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mark spell
//|
//|	Changes		-	09/22/2002	-	Fixed marking of locked down runes..
//|									(shouldn't be possible)
//o-----------------------------------------------------------------------------------------------o
bool splMark( CSocket *sock, CChar *caster, CItem *i, SI08 curSpell )
{
	if( i->IsLockedDown() )
	{
		sock->sysmessage( 774 ); // That is locked down and you cannot use it.
		return false;
	}

	bool markedInMulti = false;
	CMultiObj *multi = caster->GetMultiObj();
	if( ValidateObject( multi ) )
	{
		if( !multi->IsOnOwnerList( caster ) )
		{
			sock->sysmessage( "Marking a rune in property you don't own is not possible" ); // Marking a rune in property you don't own is not possible
			return false;
		}
		else if( !cwmWorldState->ServerData()->MarkRunesInMultis() )
		{
			sock->sysmessage( "Marking a rune inside one's property is not permitted." );
			return false;
		}
		else
		{
			// Let's allow marking the rune in the multi, and store multi's serial in a tag
			auto mSerial = multi->GetSerial();
			TAGMAPOBJECT tagObject;
			tagObject.m_Destroy = FALSE;
			tagObject.m_StringValue = std::to_string(mSerial);
			tagObject.m_IntValue = static_cast<SI32>( tagObject.m_StringValue.size() );
			tagObject.m_ObjectType = TAGMAP_TYPE_STRING;
			i->SetTag( "multiSerial", tagObject );
			markedInMulti = true;

			std::string tempRuneName = strutil::format( Dictionary->GetEntry( 684 ), multi->GetName().c_str() );
			if( tempRuneName.length() > 0 )
				i->SetName( tempRuneName );
		}
	}

	if( !markedInMulti )
	{
		i->SetTempVar( CITV_MOREX, caster->GetX() );
		i->SetTempVar( CITV_MOREY, caster->GetY() );
		i->SetTempVar( CITV_MOREZ, caster->GetZ() );
		i->SetTempVar( CITV_MORE, caster->WorldNumber() );

		std::string tempitemname;

		if( caster->GetRegion()->GetName()[0] != 0 ){
			tempitemname = strutil::format(Dictionary->GetEntry( 684 ), caster->GetRegion()->GetName().c_str() );
		}
		else {
			tempitemname=Dictionary->GetEntry( 685 );
		}
		i->SetName( tempitemname );
	}

	sock->sysmessage( 686 ); // Recall rune marked.
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MassCurseStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mass Curse spell to an individual target
//o-----------------------------------------------------------------------------------------------o
void MassCurseStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	if( target->IsNpc() && target->GetNPCAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	SI32 j;
	if( target->IsNpc() )
		Combat->AttackTarget( caster, target );
	Effects->PlayStaticAnimation( target, 0x374A, 0, 15 );
	Effects->PlaySound( target, 0x01FC );
	if( Magic->CheckResist( caster, target, 6 ) )
		j = caster->GetSkill( MAGERY )/200;
	else
		j = caster->GetSkill( MAGERY )/75;
	Effects->tempeffect( caster, target, 12, j, j, j );

}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMassCurse( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Mass Curse as an area affect spell
//o-----------------------------------------------------------------------------------------------o
bool splMassCurse( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &MassCurseStub, curSpell );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splParalyzeField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Paralyze Field as a field spell
//o-----------------------------------------------------------------------------------------------o
bool splParalyzeField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( fieldDir == 0 )
		return FieldSpell( caster, 0x3967, x, y, z, fieldDir );
	else
		return FieldSpell( caster, 0x3979, x, y, z, fieldDir );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splReveal( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Reveal spell
//o-----------------------------------------------------------------------------------------------o
bool splReveal( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( LineOfSight( sock, caster, x, y, ( z + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) || caster->IsGM() )
	{
		UI16 j = caster->GetSkill( MAGERY );
		SI32 range=(((j-261)*(15))/739)+5;
		//If the caster has a Magery of 26.1 (min to cast reveal w/ scroll), range  radius is
		//5 tiles, if magery is maxed out at 100.0 (except for gms I suppose), range is 20

		REGIONLIST nearbyRegions = MapRegion->PopulateList( caster );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == nullptr )	// no valid region
				continue;
			GenericList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( !ValidateObject( tempChar ) || tempChar->GetInstanceID() != caster->GetInstanceID() )
					continue;
				if( tempChar->GetVisible() == VT_INVISIBLE )
				{
					point3 difference = ( tempChar->GetLocation() - point3( x, y, z ) );
					if( difference.MagSquared() <= ( range * range ) ) // char to reveal is within radius or range
					{
						tempChar->ExposeToView();
					}
				}
			}
			regChars->Pop();
		}
		Effects->PlaySound( sock, 0x01FD, true );
	}
	else
		sock->sysmessage( 687 );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ChainLightningStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Chain Lightning spell to an individual target
//o-----------------------------------------------------------------------------------------------o
void ChainLightningStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	if( target->IsNpc() && target->GetNPCAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	if( target->IsNpc() )
		Combat->AttackTarget( target, caster );
	Effects->PlaySound( caster, 0x0029 );
	CChar *def = nullptr;
	if( Magic->CheckMagicReflect( target ) )
		def = caster;
	else
		def = target;

	Effects->bolteffect( def );

	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	//If targetCOunt is > 1, do equal damage to two targets, split damage on more targets
	if( targCount > 1 )
		spellDamage = ( spellDamage * 2 ) / targCount;

	Magic->MagicDamage( target, spellDamage, caster, LIGHTNING );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splChainLightning( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Chain Lightning as an area affect spell
//o-----------------------------------------------------------------------------------------------o
bool splChainLightning( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &ChainLightningStub, curSpell );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splEnergyField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Energy Field spell as a field spell
//o-----------------------------------------------------------------------------------------------o
bool splEnergyField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( fieldDir == 0 )
		return FieldSpell( caster, 0x3946, x, y, z, fieldDir );
	else
		return FieldSpell( caster, 0x3956, x, y, z, fieldDir );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splFlameStrike( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Flame Strike spell on target
//o-----------------------------------------------------------------------------------------------o
bool splFlameStrike( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	Magic->MagicDamage( target, spellDamage, caster, HEAT );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splGateTravel( CSocket *sock, CChar *caster, CItem *i, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Gate Travel spell (opens gate from recall rune)
//o-----------------------------------------------------------------------------------------------o
bool splGateTravel( CSocket *sock, CChar *caster, CItem *i, SI08 curSpell )
{
	// No recall if too heavy, GM's excempt
	if( Weight->isOverloaded( caster ) && !cwmWorldState->ServerData()->TravelSpellsWhileOverweight() && ( !caster->IsCounselor() && !caster->IsGM() ) )
	{
		sock->sysmessage( 680 ); // You are too heavy to do that!
		sock->sysmessage( 681 ); // You feel drained from the attempt.
		return false;
	}
	else
	{
		// Check if rune was marked in a multi - if so, try to take user directly there
		TAGMAPOBJECT runeMore = i->GetTag( "multiSerial" );
		if( runeMore.m_StringValue != "" )
		{
			SERIAL mSerial = strutil::value<SERIAL>( runeMore.m_StringValue );
			if( mSerial != 0 && mSerial != INVALIDSERIAL )
			{
				CMultiObj *shipMulti = calcMultiFromSer( mSerial );
				if( ValidateObject( shipMulti ) && shipMulti->CanBeObjType( OT_BOAT ) )
				{
					if( ( shipMulti->WorldNumber() == caster->WorldNumber() || cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() ) && shipMulti->GetInstanceID() == caster->GetInstanceID() )
					{
						caster->SetLocation( shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3 );
						SpawnGate( caster, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ(), caster->WorldNumber(), shipMulti->GetX() + 1, shipMulti->GetY(), shipMulti->GetZ() + 3, shipMulti->WorldNumber() );
						return true;
					}
					else
					{
						sock->sysmessage( 2064 ); // You are unable to open a gate to your ship - it might be in another world!
						return false;
					}
				}
			}

			sock->sysmessage( 2062 ); // Unable to locate ship - it might have been dry-docked... or sunk!
			return false;
		}
		else if( i->GetTempVar( CITV_MOREX ) == 0 && i->GetTempVar( CITV_MOREY ) == 0 )
		{
			sock->sysmessage( 679 ); // That rune has not been marked yet!
			return false;
		}
		else
		{
			UI08 worldNum = static_cast<UI08>( i->GetTempVar( CITV_MORE ) );
			if( !Map->MapExists( worldNum ) )
				worldNum = caster->WorldNumber();
			if( caster->WorldNumber() != worldNum && !cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() )
			{
				sock->sysmessage( 2061 ); // Travelling between worlds using Recall or Gate spells is not possible.
				return false;
			}

			SpawnGate( caster, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ(), caster->WorldNumber(), static_cast<SI16>(i->GetTempVar( CITV_MOREX )), static_cast<SI16>(i->GetTempVar( CITV_MOREY )), static_cast<SI08>(i->GetTempVar( CITV_MOREZ )), worldNum );
			return true;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splManaVampire( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mana Vampire spell to target
//o-----------------------------------------------------------------------------------------------o
bool splManaVampire( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	if( !Magic->CheckResist( caster, target, 7 ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MassDispelStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Mass Dispel spell to an individual target
//o-----------------------------------------------------------------------------------------------o
void MassDispelStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	if( target->IsDispellable() )
	{
		if( Magic->CheckResist( caster, target, 7 ) && RandomNum( 0, 1 ) == 0 )  // cant be 100% resisted , osi
		{
			if( target->IsNpc() )
				Combat->AttackTarget( caster, target );
		}
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
			Effects->PlaySound( target, 0x0204 );
			Effects->PlayStaticAnimation( target, 0x372A, 0x09, 0x06 );
			HandleDeath( target, caster );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMassDispel( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Mass Dispel spell as an area affect spell
//o-----------------------------------------------------------------------------------------------o
bool splMassDispel( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &MassDispelStub, curSpell );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MeteorSwarmStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Meteor Swarm spell to an individual target
//o-----------------------------------------------------------------------------------------------o
void MeteorSwarmStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	if( target->IsNpc() && target->GetNPCAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	if( target->IsNpc() )
		Combat->AttackTarget( target, caster );

	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	//If targetCOunt is > 1, do equal damage to two targets, split damage on more targets
	if( targCount > 1 )
		spellDamage = ( spellDamage * 2 ) / targCount;

	Effects->PlaySound( target, 0x160 );
	Effects->PlayMovingAnimation( caster, target, 0x36D5, 0x07, 0x00, 0x01 );

	Magic->MagicDamage( target, spellDamage, caster, HEAT );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AreaAffectSpell( CSocket *sock, CChar *caster, void (*trgFunc)( MAGIC_AREA_STUB_LIST ), SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Provides shared functionality for all area affect spells
//o-----------------------------------------------------------------------------------------------o
bool AreaAffectSpell( CSocket *sock, CChar *caster, void (*trgFunc)( MAGIC_AREA_STUB_LIST ), SI08 curSpell )
{
	SI16 x1, x2, y1, y2;
	SI08 z1, z2;
	x1 = x2 = y1 = y2 = z1 = z2 = 0;
	SI08 targCount = 0;
	UI16 i;
	std::vector< CChar * > targetList;

	Magic->BoxSpell( sock, caster, x1, x2, y1, y2, z1, z2 );

	REGIONLIST nearbyRegions = MapRegion->PopulateList( caster );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == nullptr )	// no valid region
			continue;
		GenericList< CChar * > *regChars = MapArea->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) || tempChar->GetInstanceID() != caster->GetInstanceID() )
				continue;

			if( tempChar->GetX() >= x1 && tempChar->GetX() <= x2 && tempChar->GetY() >= y1 && tempChar->GetY() <= y2 &&
			   tempChar->GetZ() >= z1 && tempChar->GetZ() <= z2 && ( isOnline( (*tempChar) ) || tempChar->IsNpc() ) )
			{
				if( tempChar == caster || LineOfSight( sock, caster, tempChar->GetX(), tempChar->GetY(), ( tempChar->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false  ) || caster->IsGM() )
				{
					//Store target candidates in targetList
					targetList.push_back( tempChar );
					//trgFunc( caster, tempChar, curSpell, targCount );
				}
				else if( sock != nullptr )
					sock->sysmessage( 688 );
			}
		}
		regChars->Pop();
	}

	// Iterate through list of valid targets, do damage
	targCount = static_cast<SI08>(targetList.size());
	for( i = 0; i < targetList.size(); ++i )
	{
		trgFunc( caster, targetList[i], curSpell, targCount );
	}

	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splMeteorSwarm( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Meteor Swarm spell as an area affect spell
//o-----------------------------------------------------------------------------------------------o
bool splMeteorSwarm( CSocket *sock, CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	AreaAffectSpell( sock, caster, &MeteorSwarmStub, curSpell );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splPolymorph( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Polymorph spell
//o-----------------------------------------------------------------------------------------------o
bool splPolymorph( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( caster->IsPolymorphed() )
	{
		sock->sysmessage( 1637 );
		return false;
	}
	Magic->PolymorphMenu( sock, POLYMORPHMENUOFFSET ); // Antichrists Polymorph
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void EarthquakeStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Earthquake spell to an individual target
//o-----------------------------------------------------------------------------------------------o
void EarthquakeStub( CChar *caster, CChar *target, SI08 curSpell, SI08 targCount )
{
	// Check if target is in a safe zone
	if( target->GetRegion()->IsSafeZone() )
	{
		// Target is in a safe zone, ignore spell effect
		return;
	}

	SI32 distx	= abs(target->GetX() - caster->GetX() );
	SI32 disty	= abs(target->GetY() - caster->GetY() );
	SI32 dmgmod	= std::min( distx, disty );
	dmgmod		= -(dmgmod - 7);

	SI16 spellDamage = 0;
	bool spellResisted = Magic->CheckResist( caster, target, Magic->spells[curSpell].Circle() );

	spellDamage = Magic->spells[curSpell].BaseDmg();
	spellDamage = CalcSpellDamageMod( caster, target, spellDamage, spellResisted );

	//If targetCOunt is > 1, do equal damage to two targets, split damage on more targets
	//if( targCount > 1 )
	//	spellDamage = ( spellDamage * 2 ) / targCount;

	//Apply bonus damage based on target distance from center of earthquake
	spellDamage += dmgmod;

	target->Damage( spellDamage, caster, true );
	target->SetStamina( target->GetStamina() - ( RandomNum( 0, 9 ) + 5 ) );

	if( target->GetStamina() == -1 )
		target->SetStamina( 0 );

	if(( !target->IsNpc() && isOnline((*target))) || ( target->IsNpc() && cwmWorldState->creatures[target->GetID()].IsHuman() ))
	{
		if( !target->IsOnHorse() && !target->IsFlying() )
		{
			if( target->GetBodyType() == BT_GARGOYLE )
			{
				Effects->PlayNewCharacterAnimation( target, N_ACT_IMPACT ); // Impact anim (0x04) - can't seem to trigger death anim manually with new animation packet
			}
			else // BT_HUMAN and BT_ELF
			{
				if( RandomNum( 0, 1 ) )
					Effects->PlayCharacterAnimation( target, ACT_DIE_BACKWARD ); // Death anim variation 1 - 0x15, forward
				else
					Effects->PlayCharacterAnimation( target, ACT_DIE_FORWARD ); // Death anim variation 2 - 0x16, backward
			}
		}
	}
	else // Monsters, animals
	{
		if( target->GetHP() > 0 )
		{
			Effects->PlayCharacterAnimation( target, 0x2 );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splEarthquake( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts Earthquake spell as an area affect spell
//o-----------------------------------------------------------------------------------------------o
bool splEarthquake( CSocket *sock, CChar *caster, SI08 curSpell )
{
	criminal( caster );
	if( sock != nullptr )
	{
		sock->SetWord( 11, caster->GetX() );
		sock->SetWord( 13, caster->GetY() );
		sock->SetByte( 16, caster->GetZ() );
	}
	AreaAffectSpell( sock, caster, &EarthquakeStub, curSpell );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splEnergyVortex( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Energy Vortex spell (summons an Energy Vortex)
//o-----------------------------------------------------------------------------------------------o
bool splEnergyVortex( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z, SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 1, x, y, z );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splResurrection( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Resurrection spell to a target
//o-----------------------------------------------------------------------------------------------o
bool splResurrection( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	if( target->IsDead() )
	{
		NpcResurrectTarget( target );
		return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splSummonAir( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Air Elemental spell (Summons Air Elemental)
//o-----------------------------------------------------------------------------------------------o
bool splSummonAir( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 2, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splSummonDaemon( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Daemon spell (Summons Daemon)
//o-----------------------------------------------------------------------------------------------o
bool splSummonDaemon( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 7, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splSummonEarth( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Earth Elemental spell (Summons Earth Elemental)
//o-----------------------------------------------------------------------------------------------o
bool splSummonEarth( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 3, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splSummonFire( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Fire Elemental spell (Summons Fire Elemental)
//o-----------------------------------------------------------------------------------------------o
bool splSummonFire( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 4, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splSummonWater( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Water Elemental spell (Summons Water Elemental)
//o-----------------------------------------------------------------------------------------------o
bool splSummonWater( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 5, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splRandom( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Summon Monster spell (Summons random monster)
//o-----------------------------------------------------------------------------------------------o
bool splRandom( CSocket *sock, CChar *caster, SI08 curSpell )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 8, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splNecro1( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
bool splNecro1( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splNecro2( CSocket *sock, CChar *caster, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effect of Necromantic summon spell (summons monster)
//o-----------------------------------------------------------------------------------------------o
bool splNecro2( CSocket *sock, CChar *caster, SI08 curSpell )
{
	Magic->SummonMonster( sock, caster, 9, caster->GetX() +1, caster->GetY() +1, caster->GetZ() );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splNecro3( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
bool splNecro3( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splNecro4( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
bool splNecro4( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool splNecro5( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
bool splNecro5( CChar *caster, CChar *target, CChar *src, SI08 curSpell )
{
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DiamondSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts field spell in the shape of a diamond
//o-----------------------------------------------------------------------------------------------o
bool DiamondSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
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
	UI16 instanceID	= caster->GetInstanceID();
	for( j = 0; j < 4; ++j )	// Draw the corners of our diamond
	{
		i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
		if( i != nullptr )
		{
			i->SetDispellable( true );
			i->SetDecayable( true );
			i->SetDecayTime( BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 15 )) );
			i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
			i->SetTempVar( CITV_MOREY, caster->GetSerial() );
			i->SetLocation( x + fx[j], y + fy[j], Map->Height( x + fx[j], y + fy[j], z, worldNumber, instanceID ) );
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
					i->SetDecayTime( BuildTimeValue(static_cast<R32>( caster->GetSkill( MAGERY ) / 15 )) );
					i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
					i->SetTempVar( CITV_MOREY, caster->GetSerial() );
					const SI16 newX = x + counter2 * counter3;
					const SI16 newY = y + j * ( yOffset - counter3 );
					i->SetLocation( newX, newY, Map->Height( newX, newY, z, worldNumber, instanceID ) );
					i->SetDir( 29 );
					i->SetMovable( 2 );
				}
			}
		}
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SquareSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts field spell in the shape of a square
//o-----------------------------------------------------------------------------------------------o
bool SquareSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
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
	UI16 instanceID = caster->GetInstanceID();
	for( UI08 j = 0; j < 4; ++j )	// Draw the corners of our diamond
	{
		for( SI32 counter = fx[j]; counter < fy[j]; ++counter )
		{
			i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
			if( i != nullptr )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 15 )) );
				i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
				i->SetTempVar( CITV_MOREY, caster->GetSerial() );
				switch( j )
				{
					case 0:
					case 2:
						i->SetLocation( x + counter, y + (j - 1) * yOffset, Map->Height( x + counter, y + (j-1) * yOffset, z, worldNumber, instanceID ) );
						break;
					case 1:
					case 3:
						i->SetLocation( x + (j-2) * xOffset, y + counter, Map->Height( x + (j-2) * xOffset, y + counter, z, worldNumber, instanceID ) );
						break;
				}
				i->SetDir( 29 );
				i->SetMovable( 2 );
			}
		}
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool FloodSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Casts field spell in the shape of a filled square
//o-----------------------------------------------------------------------------------------------o
bool FloodSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	SI16 yOffset = length;
	SI16 xOffset = length;

	SI16 maxX = x + xOffset;
	SI16 maxY = y + yOffset;
	SI16 minX = x - xOffset;
	SI16 minY = y - yOffset;

	UI08 worldNumber = caster->WorldNumber();
	UI16 instanceID = caster->GetInstanceID();
	for( SI32 counter1 = minX; counter1 <= maxX; ++counter1 )
	{
		for( SI32 counter2 = minY; counter2 <= maxY; ++counter2 )
		{
			CItem *i = Items->CreateItem( nullptr, caster, id, 1, 0, OT_ITEM );
			if( i != nullptr )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 15 )) );
				i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
				i->SetTempVar( CITV_MOREY, caster->GetSerial() );
				i->SetLocation( counter1, counter2, Map->Height( counter1, counter2, z, worldNumber, instanceID ) );
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
cMagic::cMagic() : spellCount( 1 )
{
	spells.resize( 0 );
}

cMagic::~cMagic()
{
	// delete any allocations
	spells.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HasSpell( CItem *book, SI32 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if given spellbook contains specific spell
//o-----------------------------------------------------------------------------------------------o
bool cMagic::HasSpell( CItem *book, SI32 spellNum )
{
	if( !ValidateObject( book ) )
		return false;
	UI32 wordNum	= spellNum / 32;
	UI32 bitNum		= (spellNum % 32);
	UI32 flagToCheck = power( 2, bitNum );
	UI32 sourceAmount = 0;
	if( wordNum < 3 )
		sourceAmount = book->GetSpell( (UI08)wordNum );
	return ((sourceAmount&flagToCheck) == flagToCheck);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddSpell( CItem *book, SI32 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds specific spell to given spellbook
//o-----------------------------------------------------------------------------------------------o
void cMagic::AddSpell( CItem *book, SI32 spellNum )
{
	std::vector<UI16> scriptTriggers = book->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// If script returns false/0, prevent addition of spell to spellbook
			if( toExecute->OnSpellGain( book, spellNum ) == 0 )
			{
				return;
			}
		}
	}

	if( !ValidateObject( book ) )
		return;
	UI32 wordNum = spellNum / 32;
	if( wordNum < 3 )
	{
		UI32 bitNum = (spellNum % 32);
		UI32 flagToSet = power( 2, bitNum );
		UI32 targAmount;
		targAmount = ( book->GetSpell( (UI08)wordNum ) | flagToSet );
		book->SetSpell( (UI08)wordNum, targAmount );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void cMagic::RemoveSpell( CItem *book, SI32 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes specific spell from given spellbook
//o-----------------------------------------------------------------------------------------------o
void cMagic::RemoveSpell( CItem *book, SI32 spellNum )
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

	if( !ValidateObject( book ) )
		return;
	UI32 wordNum	= spellNum / 32;
	if( wordNum < 3 )
	{
		UI32 bitNum		= (spellNum % 32);
		UI32 flagToSet	= power( 2, bitNum );
		UI32 flagMask	= 0xFFFFFFFF;
		UI32 targAmount;
		flagMask ^= flagToSet;
		targAmount = ( book->GetSpell( (UI08)wordNum ) & flagMask );
		book->SetSpell( (UI08)wordNum, targAmount );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SpellBook( CSocket *mSock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the spellbook and displays all spells a character has in his book
//o-----------------------------------------------------------------------------------------------o
void cMagic::SpellBook( CSocket *mSock )
{
	UI08 spellsList[70];

	SERIAL serial		= ( mSock->GetDWord( 1 ) & 0x7FFFFFFF );
	CChar *mChar		= mSock->CurrcharObj();
	CItem *spellBook	= calcItemObjFromSer( serial );

	// Here's the kicker. We NEVER needed to search through our pack for our spellbook!!! Why, you might ask???
	// Because we are able to calculate the serial of the spellbook, because it's targetted for us!!!!
	// Hence, we can remove all searches from here, and just use it directly
	// The only time we need to search is when they use the macro, and that's easily done (item == -1)

	if( !ValidateObject( spellBook ) )	// used a macro!!!
		spellBook = FindItemOfType( mChar, IT_SPELLBOOK );
	if( !ValidateObject( spellBook ) )	// we STILL have no spellbook!!!
	{
		mSock->sysmessage( 692 );
		return;
	}
	CItem *x = mChar->GetPackItem();
	if( spellBook->GetCont() != mChar && spellBook->GetCont() != x )
	{
		mSock->sysmessage( 403 );
		return;
	}
	CPDrawContainer sbStart( (*spellBook) );
	sbStart.Model( 0xFFFF );
	if( mSock->ClientType() >= CV_HS2D && mSock->ClientVerShort() >= CVS_7090 )
		sbStart.ContType( 0x7D );
	mSock->Send( &sbStart );

	// Support for new Client Spellbook
	CPNewSpellBook ourBook( (*spellBook) );
	if( ourBook.ClientCanReceive( mSock ) )
	{
		mSock->Send( &ourBook );
		return;
	}
	else
	{
		memset( spellsList, 0, sizeof( UI08 ) * 70 );
		for( UI08 j = 0; j < 65; ++j )
		{
			if( HasSpell( spellBook, j ) )
				spellsList[j] = 1;
		}
		UI08 i;
		i = spellsList[0];
		spellsList[0] = spellsList[1];
		spellsList[1] = spellsList[2];
		spellsList[2] = spellsList[3];
		spellsList[3] = spellsList[4];
		spellsList[4] = spellsList[5];
		spellsList[5] = spellsList[6];
		spellsList[6] = i;

		if( spellsList[64] )
		{
			for( i = 0; i < 65; ++i )
				spellsList[i] = 1;
			spellsList[64] = 0;
		}
		spellsList[64] = spellsList[65];
		spellsList[65] = 0;

		UI08 scount = 0;
		for( i = 0; i < 65; ++i )
		{
			if( spellsList[i] )
				++scount;
		}
		if( scount > 0 )
		{
			CPItemsInContainer mItems;
			if( mSock->ClientVerShort() >= CVS_6017 )
				mItems.UOKRFlag( true );
			mItems.NumberOfItems( scount );
			UI16 runningCounter		= 0;
			const SERIAL CONTSER	= spellBook->GetSerial();
			for( i = 0; i < 65; ++i )
			{
				if( spellsList[i] )
					mItems.Add( runningCounter++, 0x41000000 + i, CONTSER, i + 1 );
			}
			mSock->Send( &mItems );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GateCollision( CSocket *mSock, CChar *mChar, CItem *itemCheck, ItemTypes type )
//|	Changes		-	collision dir
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used when a PLAYER passes through a gate.  Takes the player
//|						to the other side of the gate-link.
//o-----------------------------------------------------------------------------------------------o

void cMagic::GateCollision( CSocket *mSock, CChar *mChar, CItem *itemCheck, ItemTypes type )
{
	if( type == IT_GATE )
	{
		CItem *otherGate = calcItemObjFromSer( itemCheck->GetTempVar( CITV_MOREX ) );
		if( !ValidateObject( otherGate ) )
			return;
		SI08 dirOffset = 0;
		if( mChar->GetDir() < SOUTH )
			dirOffset = 1;
		else
			dirOffset = -1;
		if( otherGate->WorldNumber() != mChar->WorldNumber() )
		{
			mChar->SetLocation( otherGate->GetX() + dirOffset, otherGate->GetY(), otherGate->GetZ(), otherGate->WorldNumber(), otherGate->GetInstanceID() );
			SendMapChange( mChar->WorldNumber(), mSock, false );
		}
		else
			mChar->SetLocation( otherGate->GetX() + dirOffset, otherGate->GetY(), otherGate->GetZ(), otherGate->WorldNumber(), otherGate->GetInstanceID() );
		if( !mChar->IsNpc() )
		{
			GenericList< CChar * > *myPets = mChar->GetPetList();
			for( CChar *myPet = myPets->First(); !myPets->Finished(); myPet = myPets->Next() )
			{
				if( !ValidateObject( myPet ) )
					continue;
				if( !myPet->GetMounted() && myPet->IsNpc() && myPet->GetOwnerObj() == mChar )
				{
					if( objInOldRange( mChar, myPet, DIST_INRANGE ) )
						myPet->SetLocation( mChar );
				}
			}
		}
		Effects->PlaySound( mSock, 0x01FE, true );
		Effects->PlayStaticAnimation( mChar, 0x372A, 0x09, 0x06 );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SummonMonster( CSocket *s, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z )
//|	Changes		-	May 4th, 2000 - made function general words PC's and NPC's
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Summon a monster (dispellable with DISPEL).
//o-----------------------------------------------------------------------------------------------o
void cMagic::SummonMonster( CSocket *s, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z )
{
	bool validSocket = ( s != nullptr );
	if( validSocket )
		caster = s->CurrcharObj();
	else
	{	// Since caster will determine who is casting the spell... get a socket for players to see animations and hear effects
		if( caster->IsNpc() && !caster->GetTarg()->IsNpc() )
			s = caster->GetTarg()->GetSocket();
	}

	CChar *newChar=nullptr;

	switch( id )
	{
		case 0:	// summon monster
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateRandomNPC( "10000" );
			if( !ValidateObject( newChar ) )
			{
				if( validSocket )
					s->sysmessage( 694 );
				return;
			}
			newChar->SetOwner( caster );
			newChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 5 )) );
			newChar->SetLocation( caster );
			Effects->PlayCharacterAnimation( newChar, ACT_SPELL_AREA ); // 0x11, used to be 0x0C
			newChar->SetFTarg( caster );
			newChar->SetNpcWander( WT_FOLLOW );
			if( validSocket )
				s->sysmessage( 695 );
			return;
		case 1: // Energy Vortex
			Effects->PlaySound( s, 0x0216, true ); // EV
			newChar = Npcs->CreateNPCxyz( "energyvortex-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		case 2: // Air Elemental
			Effects->PlaySound( s, 0x0217, true ); // AE
			newChar = Npcs->CreateNPCxyz( "airele-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		case 3: //Earth Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "earthele-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		case 4: //Fire Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "firele-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		case 5: //Water Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "waterele-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		case 6: //Blade Spirits
			Effects->PlaySound( s, 0x0212, true ); // I don't know if this is the right effect...
			newChar = Npcs->CreateNPCxyz( "bladespirit-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		case 7: // Daemon
			Effects->PlaySound( s, 0x0216, true );
			newChar = Npcs->CreateNPCxyz( "daemon-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		case 8: // Dupre The Hero
			Effects->PlaySound( s, 0x0246, true );
			newChar = Npcs->CreateNPCxyz( "dupre-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		case 9: // Black Night
			Effects->PlaySound( s, 0x0216, true );
			newChar = Npcs->CreateNPCxyz( "blacknight-summon", 0, 0, 0, caster->WorldNumber(), caster->GetInstanceID() );
			break;
		default:
			Effects->PlaySound( s, 0x0215, true );
	}

	if( !ValidateObject( newChar ) )
		return;

	newChar->SetDispellable( true );

	// pc's don't own BS/EV, NPCs do
	if( caster->IsNpc() || ( id != 1 && id != 6 ) )
		newChar->SetOwner( caster );

	if( x == 0 )
		newChar->SetLocation( caster->GetX()-1, caster->GetY(), caster->GetZ() );
	else
	{
		if( Map->ValidSpawnLocation( x, y, z, caster->WorldNumber(), caster->GetInstanceID() ) )
			newChar->SetLocation( x, y, z );
		else
			newChar->SetLocation( caster->GetX()-1, caster->GetY(), caster->GetZ() );
	}

	newChar->SetSpDelay( 10 );
	newChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 5 )) );
	Effects->PlayCharacterAnimation( newChar, ACT_SPELL_AREA ); // 0x11, used to be 0x0C
	// (9/99) - added the chance to make the monster attack
	// the person you targeted ( if you targeted a char, naturally :) )
	CChar *i = nullptr;
	if( !caster->IsNpc() && s != nullptr )
	{
		if( s->GetDWord( 7 ) != INVALIDSERIAL )
		{
			i = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( !ValidateObject( i ) )
				return;
		}
	}
	else
		i = caster->GetTarg();
	Combat->AttackTarget( newChar, i );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CheckBook( SI32 circle, SI32 spell, CItem *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if the spell is memorized into the spellbook.
//o-----------------------------------------------------------------------------------------------o

bool cMagic::CheckBook( SI32 circle, SI32 spell, CItem *i )
{
	SI32 spellnum = spell + ( circle - 1 ) * 8;
	return HasSpell( i, spellnum );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckMana( CChar *s, SI32 circle )
//|	Changes		-	to use spells[] array.
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character has enough mana to cast spell
//o-----------------------------------------------------------------------------------------------o
bool cMagic::CheckMana( CChar *s, SI32 num )
{
	if( !ValidateObject( s ) )
		return true;
	if( s->NoNeedMana() )
		return true;
	if( s->GetMana() >= spells[num].Mana() )
		return true;
	else
	{
		CSocket *p = s->GetSocket();
		if( p != nullptr )
			p->sysmessage( 696 );
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckStamina( CChar *s, SI32 num )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character has enough stamina to cast spell
//o-----------------------------------------------------------------------------------------------o
bool cMagic::CheckStamina( CChar *s, SI32 num )
{
	if( !ValidateObject( s ) )
		return true;
	if( s->NoNeedMana() )
		return true;
	if( s->GetStamina() >= spells[num].Stamina() )
		return true;
	else
	{
		CSocket *p = s->GetSocket();
		if( p != nullptr )
			p->sysmessage( 697 );
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckHealth( CChar *s, SI32 num )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character has enough health to cast spell
//o-----------------------------------------------------------------------------------------------o
bool cMagic::CheckHealth( CChar *s, SI32 num )
{
	if( !ValidateObject( s ) )
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
		return true;
	CSocket *p = s->GetSocket();
	if( p != nullptr )
		p->sysmessage( 698 );
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SubtractMana( CChar *s, SI32 mana)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtract the required mana from character's mana reserve
//o-----------------------------------------------------------------------------------------------o
void cMagic::SubtractMana( CChar *s, SI32 mana)
{
	if( s->NoNeedMana() )
		return;
	s->SetMana( s->GetMana() - mana );
	if( s->GetMana() < 0 )
		s->SetMana( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SubtractStamina( CChar *s, SI32 stamina )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtract the required stmina from character's stamina reserve
//o-----------------------------------------------------------------------------------------------o
void cMagic::SubtractStamina( CChar *s, SI32 stamina )
{
	if( s->NoNeedMana() )
		return;
	s->SetStamina( s->GetStamina() - stamina );
	if( s->GetStamina() == -1 )
		s->SetStamina( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SubtractHealth( CChar *s, SI32 health, SI32 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtract the required health from character's stamina reserve
//o-----------------------------------------------------------------------------------------------o
void cMagic::SubtractHealth( CChar *s, SI32 health, SI32 spellNum )
{
	if( s->NoNeedMana() || spells[spellNum].Health() == 0 )
		return;
	if( spells[spellNum].Health() < 0 )
	{
		if( abs( spells[spellNum].Health() * health ) > s->GetHP() )
			s->SetHP( 0 );
		else
			s->IncHP( spells[spellNum].Health() * health );
	}
	else
		s->IncHP( -health );
	if( s->GetHP() < 0 )
		s->SetHP( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckMagicReflect( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character is protected by MagicReflect.
//|						If yes, remove the protection and do visual effect.
//o-----------------------------------------------------------------------------------------------o
bool cMagic::CheckMagicReflect( CChar *i )
{
	if( i->IsPermReflected() )
	{
		i->SetPermReflected( false );
		Effects->PlayStaticAnimation( i, 0x373A, 0, 15 );
		return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckResist( CChar *attacker, CChar *defender, SI32 circle )
//|	Changes		-	to add EV.INT. check
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check character's magic resistance.
//o-----------------------------------------------------------------------------------------------o
bool cMagic::CheckResist( CChar *attacker, CChar *defender, SI32 circle )
{
	bool i = Skills->CheckSkill( defender, MAGICRESISTANCE, 80*circle, 800+(80*circle) );
	CSocket *s = nullptr;
	if( ValidateObject( attacker ) )
	{
		// Check what is higher between user's normal resistchance and a fallback value
		// To ensure user always has a chance of resisting, however small (except at resist-skill 0)
		SI16 defaultChance = defender->GetSkill( MAGICRESISTANCE ) / 5;
		SI16 resistChance = defender->GetSkill( MAGICRESISTANCE ) - ((( attacker->GetSkill( MAGERY ) - 20 ) / 5 ) + ( circle * 5 ));
		if( defaultChance > resistChance )
			resistChance = defaultChance;

		if( RandomNum( 1, 100 ) < resistChance / 10 )
		{
			s = defender->GetSocket();
			if( s != nullptr )
				s->sysmessage( 699 );
			i = true;
		}
		else
			i = false;
	}
	else
	{
		if( i )
		{
			s = defender->GetSocket();
			if( s != nullptr )
				s->sysmessage( 699 );
		}
	}
	return i;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CalcSpellDamageMod( CChar *caster, CChar *target, SI16 spellDamage, bool spellResisted )
//|	Date		-	November 7th 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate Spell Damage after skill-modifiers
//o-----------------------------------------------------------------------------------------------o
SI16 CalcSpellDamageMod( CChar *caster, CChar *target, SI16 spellDamage, bool spellResisted )
{
	// Randomize in upper-half of damage range for some variety
	spellDamage = HalfRandomNum( spellDamage );

	// If spell was resisted, halve damage
	if( spellResisted )
		spellDamage = (SI16) roundNumber( spellDamage / 2 );

	// Add damage bonus/penalty based on attacker's EVALINT vs defender's MAGICRESISTANCE
	UI16 casterEval = caster->GetSkill( EVALUATINGINTEL )/10;
	UI16 targetResist = target->GetSkill( MAGICRESISTANCE )/10;
	if( targetResist > casterEval )
		spellDamage *= ((( casterEval - targetResist ) / 200.0f ) + 1 );
	else
		spellDamage *= ((( casterEval - targetResist ) / 500.0f ) + 1 );

	// Randomize some more to get broader min/max damage values
	SI32 i = RandomNum( 0, 4 );
	if( i <= 2 )
		spellDamage = roundNumber( RandomNum( static_cast<SI16>( HalfRandomNum( spellDamage ) / 2 ), spellDamage ));
	else if( i == 3 )
		spellDamage = roundNumber( HalfRandomNum( spellDamage ));
	else //keep current spellDamage
		spellDamage = roundNumber( spellDamage );

	return spellDamage;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MagicDamage( CChar *p, SI16 amount, CChar *attacker, WeatherType element )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate and inflict magic damage.
//o-----------------------------------------------------------------------------------------------o
void cMagic::MagicDamage( CChar *p, SI16 amount, CChar *attacker, WeatherType element )
{
	if( !ValidateObject( p ) )
		return;

	if( p->IsDead() || p->GetHP() <= 0 )	// extra condition check, to see if deathstuff hasn't been hit yet
		return;

	CSocket *mSock = p->GetSocket();

	if( p->IsFrozen() && p->GetDexterity() > 0 )
	{
		p->SetFrozen( false );
		if( mSock != nullptr )
			mSock->sysmessage( 700 );
	}

	// Allow damage if target is not invulnerable or in a zone safe from aggressive magic
	if( !p->IsInvulnerable() && !p->GetRegion()->IsSafeZone() && p->GetRegion()->CanCastAggressive() )
	{
		UI08 hitLoc = Combat->CalculateHitLoc();
		SI16 damage = Combat->ApplyDamageBonuses( element, attacker, p, MAGERY, hitLoc, amount);
		damage = Combat->ApplyDefenseModifiers( element, attacker, p, MAGERY, hitLoc, damage, true);
		if( damage <= 0 )
			damage = 1;

		p->Damage( damage, attacker, true );
		p->ReactOnDamage( element, attacker );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PoisonDamage( CChar *p, SI32 poison )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Apply the poison to the character.
//o-----------------------------------------------------------------------------------------------o
void cMagic::PoisonDamage( CChar *p, SI32 poison )
{
	if( p->IsFrozen() )
	{
		p->SetFrozen( false );
		CSocket *s = p->GetSocket();
		if( s != nullptr )
			s->sysmessage( 700 );
	}

	// Allow poison damage if target is not invunlnerable or in a zone safe from aggressive magic
	if( !p->IsInvulnerable() && !p->GetRegion()->IsSafeZone() && p->GetRegion()->CanCastAggressive() )
	{
		if( poison > 5 )
			poison = 5;
		if( poison < 0 )
			poison = 1;

		p->SetPoisoned( poison );
		p->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POISON ) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckFieldEffects( CChar& mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character stands on a magic-field and apply effects.
//o-----------------------------------------------------------------------------------------------o
void cMagic::CheckFieldEffects( CChar& mChar )
{
	CMapRegion *toCheck = MapRegion->GetMapRegion( &mChar );
	if( toCheck == nullptr )	// no valid region
		return;
	GenericList< CItem * > *regItems = toCheck->GetItemList();
	regItems->Push();
	for( CItem *inItemList = regItems->First(); !regItems->Finished(); inItemList = regItems->Next() )
	{
		if( !ValidateObject( inItemList ) || inItemList->GetInstanceID() != mChar.GetInstanceID() )
			continue;
		if( inItemList->GetX() == mChar.GetX() && inItemList->GetY() == mChar.GetY() )
		{
			if( HandleFieldEffects( (&mChar), inItemList, inItemList->GetID() ) )
				break;
		}
	}
	regItems->Pop();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleFieldEffects( CChar *mChar, CItem *fieldItem, UI16 id )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle the effects of characters entering field spells
//o-----------------------------------------------------------------------------------------------o
bool cMagic::HandleFieldEffects( CChar *mChar, CItem *fieldItem, UI16 id )
{
	CChar *caster;
	if( id >= 0x398C && id <= 0x399F ) //fire field
	{
		caster = calcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ) );	// store caster in morey
		if( ValidateObject( caster) )
		{
			if( RandomNum( 0, 2 ) == 1 )
			{
				if( !CheckResist( nullptr, mChar, 4 ) )
					MagicDamage( mChar, fieldItem->GetTempVar( CITV_MOREX ) / 50, caster, HEAT );
				else
					MagicDamage( mChar, fieldItem->GetTempVar( CITV_MOREX ) / 100, caster, HEAT );
				Effects->PlaySound( mChar, 520 );
			}
		}
		return true;
	}
	else if( id >= 0x3914 && id <= 0x3929 ) //poison field
	{
		if( mChar->IsInnocent() )
		{
			caster = calcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ) );	// store caster in morey
			if( WillResultInCriminal( caster, mChar ) && ValidateObject( caster ) && !caster->IsGM() && !caster->IsCounselor() )
				criminal( caster );
		}
		if( RandomNum( 0, 2 ) == 1 )
		{
			if( !CheckResist( nullptr, mChar, 5 ) )
			{
				if( fieldItem->GetTempVar( CITV_MOREX ) < 997 )
					PoisonDamage( mChar, 2 );
				else
					PoisonDamage( mChar, 3 );
			}
			else
				PoisonDamage( mChar, 1 );
			Effects->PlaySound( mChar, 520 );
		}
		return true;
	}
	else if( id >= 0x3967 && id <= 0x398A ) //paralysis field
	{
		caster = calcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ) );	// store caster in morey
		if( mChar->IsInnocent() )
		{
			if( WillResultInCriminal( caster, mChar ) && ValidateObject( caster ) && !caster->IsGM() && !caster->IsCounselor() )
				criminal( caster );
		}
		if( RandomNum( 0, 2 ) == 1 && !CheckResist( nullptr, mChar, 6 ) )
		{
			Effects->tempeffect( caster, mChar, 1, 0, 0, 0 );
			Effects->PlaySound( mChar, 520 );
		}
		return true;
	}
	else if( id >= 0x3946 && id <= 0x3964 ) //energy field
	{
		caster = calcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ) );	// store caster in morey
		if( ValidateObject( caster) )
		{
			if( RandomNum( 0, 2 ) == 1 )
			{
				if( !CheckResist( nullptr, mChar, 4 ) )
					MagicDamage( mChar, fieldItem->GetTempVar( CITV_MOREX ) / 50, caster, LIGHTNING );
				else
					MagicDamage( mChar, fieldItem->GetTempVar( CITV_MOREX ) / 100, caster, LIGHTNING );
				Effects->PlaySound( mChar, 520 );
			}
		}
		return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void BoxSpell( CSocket *s, CChar *caster, SI16& x1, SI16& x2, SI16& y1, SI16& y2, SI08& z1, SI08& z2)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the spell box effect, depending on character's magery skill
//o-----------------------------------------------------------------------------------------------o
void cMagic::BoxSpell( CSocket *s, CChar *caster, SI16& x1, SI16& x2, SI16& y1, SI16& y2, SI08& z1, SI08& z2)
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
	z2 = z+3;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MagicTrap( CChar *s, CItem *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Do the visual effect and apply magic damage when a player opens a trapped container
//o-----------------------------------------------------------------------------------------------o
void cMagic::MagicTrap( CChar *s, CItem *i )
{
	if( !ValidateObject( s ) || !ValidateObject( i ) )
		return;
	Effects->PlayStaticAnimation( s, 0x36B0, 0x09, 0x09 );
	Effects->PlaySound( s, 0x0207 );
	if( CheckResist( nullptr, s, 4 ) )
		MagicDamage( s, i->GetTempVar( CITV_MOREZ, 2 ) / 4, nullptr, LIGHTNING );
	else
		MagicDamage( s, i->GetTempVar( CITV_MOREZ, 2 ) / 2, nullptr, LIGHTNING );

	// Set first part of MOREZ to 0 to disable trap, but leave other parts intact
	i->SetTempVar( CITV_MOREZ, 1, 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckReagents( CChar *s, const reag_st *reagents )
//|	Changes		-	to use reag-st
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for required reagents in player's backpack.
//o-----------------------------------------------------------------------------------------------o
bool cMagic::CheckReagents( CChar *s, const reag_st *reagents )
{
	reag_st failmsg;
	if( s->NoNeedReags() )
		return true;

	if( reagents->ash != 0 && GetItemAmount( s, 0x0F8C ) < reagents->ash )
		failmsg.ash = 1;
	if( reagents->drake!=0 && GetItemAmount( s, 0x0F86 ) < reagents->drake )
		failmsg.drake = 1;
	if( reagents->garlic != 0 && GetItemAmount( s, 0x0F84 ) < reagents->garlic )
		failmsg.garlic = 1;
	if( reagents->ginseng != 0 && GetItemAmount( s, 0x0F85 ) < reagents->ginseng )
		failmsg.ginseng = 1;
	if( reagents->moss != 0 && GetItemAmount( s, 0x0F7B ) < reagents->moss )
		failmsg.moss = 1;
	if( reagents->pearl != 0 && GetItemAmount( s, 0x0F7A ) < reagents->pearl )
		failmsg.pearl = 1;
	if( reagents->shade != 0 && GetItemAmount( s, 0x0F88 ) < reagents->shade )
		failmsg.shade = 1;
	if( reagents->silk != 0 && GetItemAmount( s, 0x0F8D ) < reagents->silk )
		failmsg.silk = 1;
	return RegMsg( s, failmsg );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RegMsg( CChar *s, reag_st failmsg )
//|	Changes		-	display missing reagents types
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Display an error message if character has not enough regs
//o-----------------------------------------------------------------------------------------------o
bool cMagic::RegMsg( CChar *s, reag_st failmsg )
{
	if( !ValidateObject( s ) )
		return true;

	bool display = false;
	char message[100] = { 0, };

	// Copy dictionary message into char array
	strcpy( message, Dictionary->GetEntry( 702 ).c_str() );

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
	strcat( message, tempString.c_str() );
	message[strlen( message ) - 2] = ']';

	if( display )
	{
		CSocket *i = s->GetSocket();
		if( i != nullptr )
			i->sysmessage( message );
		return false;
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SpellFail( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Do visual and sound effects when a player fails to cast a spell.
//o-----------------------------------------------------------------------------------------------o
void cMagic::SpellFail( CSocket *s )
{
	// Use Reagents on failure ( if casting from a spellbook )
	CChar *mChar = s->CurrcharObj();
	if( s->CurrentSpellType() == 0 )
		DelReagents( mChar, spells[mChar->GetSpellCast()].Reagants() );
	Effects->PlayStaticAnimation( mChar, 0x3735, 0, 30 );
	Effects->PlaySound( mChar, 0x005C );
	mChar->TextMessage( s, 771, EMOTE, false );
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////// PCs CASTING SPELLS RELATED FUNCTIONS ////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SelectSpell( CSocket *mSock, SI32 num )
//|	Date		-	28 August 1999 / 10 September 1999
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Execute the selected spell to cast.
//|
//|	Changes		-	Use spells (memory version of script info )
//|					Made memory optionable
//|					Perma cached
//|					Added JS handling
//o-----------------------------------------------------------------------------------------------o
bool cMagic::SelectSpell( CSocket *mSock, SI32 num )
{
	SI16 lowSkill = 0, highSkill = 0;
	CChar *mChar = mSock->CurrcharObj();

	cScript *jsScript	= JSMapping->GetScript( spells[num].JSScript() );

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
		if( jsScript->MagicSpellCast( mSock, mChar, false, num ) )
		{
			return true;
		}
	}

	if( !mChar->GetCanAttack() )
	{
		mSock->sysmessage( 1778 );
		return false;
	}
	SI32 type = mSock->CurrentSpellType();
	SpellInfo curSpellCasting = spells[num];

	if( mChar->GetTimer( tCHAR_SPELLTIME ) != 0 )
	{
		if( mChar->IsCasting() )
		{
			mSock->sysmessage( 762 );
			return false;
		}
		else if( mChar->GetTimer( tCHAR_SPELLTIME ) > cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			mSock->sysmessage( 1638 );
			return false;
		}
	}

	mChar->SetSpellCast( static_cast<SI08>(num) );
	if( num > 63 && num <= static_cast<SI32>(spellCount) && spellCount <= 70 )
		Log( Dictionary->GetEntry( magic_table[num].spell_name ), mChar, nullptr, "(Attempted)");
	if( mChar->IsJailed() && !mChar->IsGM() )
	{
		mSock->sysmessage( 704 );
		mChar->StopSpell();
		return false;
	}

	// Region checks
	CTownRegion *ourReg = mChar->GetRegion();
	if(( num == 45 && !ourReg->CanMark() ) || ( num == 52 && !ourReg->CanGate() ) ||
	   ( num == 32 && !ourReg->CanRecall() ) || ( num == 22 && !ourReg->CanTeleport() ))
	{
		mSock->sysmessage( 705 );
		mChar->StopSpell();
		return false;
	}

	// Check if caster can cast aggressive spells in the region they're in
	if( spells[mChar->GetSpellCast()].AggressiveSpell() )
	{
		bool allowCasting = true;
		if( mChar->GetRegion()->IsSafeZone() )
		{
			mSock->sysmessage( 1799 );
			allowCasting = false;
		}
		else if( !mChar->GetRegion()->CanCastAggressive() )
		{
			mSock->sysmessage( 706 );
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
		mSock->sysmessage( 707 );
		mChar->StopSpell();
		return false;
	}

	//Cut the casting requirement on scrolls
	if( type == 1 && cwmWorldState->ServerData()->CutScrollRequirementStatus() ) // only if enabled
	{
		lowSkill	= curSpellCasting.ScrollLow();
		highSkill	= curSpellCasting.ScrollHigh();
	}
	else
	{
		lowSkill	= curSpellCasting.LowSkill();
		highSkill	= curSpellCasting.HighSkill();
	}


	// The following loop checks to see if any item is currently equipped (if not a GM)
	if( !mChar->IsGM() )
	{
		if( type != 2 )
		{
			CItem *itemRHand = mChar->GetItemAtLayer( IL_RIGHTHAND );
			CItem *itemLHand = mChar->GetItemAtLayer( IL_LEFTHAND );
			if( itemLHand != nullptr || ( itemRHand != nullptr && itemRHand->GetType() != IT_SPELLBOOK ) )
			{
				mSock->sysmessage( 708 );
				mChar->StopSpell();
				return false;
			}
		}
	}

	if( mChar->GetVisible() == VT_TEMPHIDDEN || mChar->GetVisible() == VT_INVISIBLE )
		mChar->ExposeToView();
	mChar->BreakConcentration( mSock );

	//			What the JavaScript event has to handle:

	//			- Reag consumption
	//			- Health consumption
	//			- Mana consumption
	//			- Stamina consumption

	//			- Delay measurement

	SI16 Delay = -2;

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
				Delay = toExecute->OnSpellCast( mChar, num );
			}
			else
			{
				Delay = toExecute->OnScrollCast( mChar, num );
			}
		}
	}

	// If Delay is -1 the spell should be canceled
	if( Delay == -1 )
	{
		mChar->StopSpell();
		return false;
	}

	if( ( Delay == -2 ) && !mChar->IsGM() )
	{
		//Check for enough reagents
		// type == 0 -> SpellBook
		if( type == 0 && !CheckReagents( mChar, curSpellCasting.ReagantsPtr() ) )
		{
			mChar->StopSpell();
			return false;
		}

		// type == 2 - Wands
		if( type != 2 )
		{
			if( !CheckMana( mChar, num ) )
			{
				mSock->sysmessage( 696 );
				mChar->StopSpell();
				return false;
			}
			if( !CheckStamina( mChar, num ) )
			{
				mSock->sysmessage( 697 );
				mChar->StopSpell();
				return false;
			}
			if( !CheckHealth( mChar, num ) )
			{
				mSock->sysmessage( 698 );
				mChar->StopSpell();
				return false;
			}
		}
	}

	if( ( !mChar->IsGM() ) && ( !Skills->CheckSkill( mChar, MAGERY, lowSkill, highSkill ) ) )
	{
		mChar->TextMessage( nullptr, curSpellCasting.Mantra().c_str(), TALK, false );
		SpellFail( mSock );
		mChar->StopSpell();
		return false;
	}

	mChar->SetNextAct( 75 );		// why 75?

	// Delay measurement...
	if( Delay >= 0 )
	{
		mChar->SetTimer( tCHAR_SPELLTIME, BuildTimeValue(static_cast<R32>( Delay / 1000 )) );
		mChar->SetFrozen( true );
	}
	else if( type == 0 && !mChar->IsGM() ) // if they are a gm they don't have a delay :-)
	{
		mChar->SetTimer( tCHAR_SPELLTIME, BuildTimeValue( static_cast<R32>(curSpellCasting.Delay() / 10 )) );
		mChar->SetFrozen( true );
	}
	else
		mChar->SetTimer( tCHAR_SPELLTIME, 0 );
	// Delay measurement end

	//if( !mChar->IsOnHorse() )
		Effects->PlaySpellCastingAnimation( mChar, curSpellCasting.Action() ); // do the action

	std::string temp;
	if( spells[num].FieldSpell() )
	{
		if( mChar->GetSkill( MAGERY ) > 600 )
			temp = "Vas " + curSpellCasting.Mantra();
		else
			temp = curSpellCasting.Mantra();
	}
	else
		temp = curSpellCasting.Mantra();

	mChar->TextMessage( nullptr, temp, TALK, false );
	mChar->SetCasting( true );
	return true;

}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 getFieldDir( CChar *s, SI16 x, SI16 y )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets field direction
//o-----------------------------------------------------------------------------------------------o
UI08 cMagic::getFieldDir( CChar *s, SI16 x, SI16 y )
{
	UI08 fieldDir = 0;
	switch( Movement->Direction( s, x, y ) )
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
					Console.error( " Fallout of switch statement without default. uox3.cpp, getFieldDir()" );
					break;
			}
			break;
		default:
			Console.error( " Fallout of switch statement without default. uox3.cpp, getFieldDir()" );
			break;
	}
	return fieldDir;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CastSpell( CSocket *s, CChar *caster )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles a character's attempt to cast spells
//o-----------------------------------------------------------------------------------------------o
void cMagic::CastSpell( CSocket *s, CChar *caster )
{
	// for LocationTarget spell like ArchCure, ArchProtection etc...
	SI08 curSpell;

	curSpell = caster->GetSpellCast();
	cScript *jsScript = JSMapping->GetScript( spells[curSpell].JSScript() );
	if( jsScript != nullptr )
	{
		if( jsScript->MagicSpellCast( s, caster, true, curSpell ) )
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
				s->sysmessage( 1799 );
			allowCasting = false;
		}
		else if( !caster->GetRegion()->CanCastAggressive() )
		{
			// Caster is in a region where casting aggressive spells is forbidden
			if( validSocket )
				s->sysmessage( 709 );
			allowCasting = false;
		}
		if( !allowCasting )
			return;
	}

	if( curSpell == 32 || curSpell == 52 )
	{
		if( !cwmWorldState->ServerData()->TravelSpellsWhileAggressor() && ( ( caster->DidAttackFirst() && ValidateObject( caster->GetTarg() ) && caster->GetTarg()->IsInnocent() ) || caster->IsCriminal() ) )
		{
			if( validSocket )
				s->sysmessage( 2066 ); // You are not allowed to use Recall or Gate spells while being the aggressor in a fight!
			return;
		}
	}

	if( curSpell > 63 && static_cast<UI32>(curSpell) <= spellCount && spellCount <= 70 )
		Log( Dictionary->GetEntry( magic_table[curSpell].spell_name ), caster, nullptr, "(Succeeded)");
	if( caster->IsNpc() || (validSocket && (s->CurrentSpellType() != 2)) )	// delete mana if NPC, s is -1 otherwise!
	{
		SubtractMana( caster, spells[curSpell].Mana() );
		if( spells[curSpell].Health() > 0 )
			SubtractHealth( caster, spells[curSpell].Health(), curSpell );
		SubtractStamina( caster, spells[curSpell].Stamina() );
	}
	if( validSocket && s->CurrentSpellType() == 0 && !caster->IsNpc() )
		DelReagents( caster, spells[curSpell].Reagants() );

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
					s->sysmessage( 705 );
				return;
			}
			// mark, recall and gate go here
			if( !caster->IsNpc() )
				i = calcItemObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( i ) )
			{
				if( i->GetCont() != nullptr || LineOfSight( s, caster, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) || caster->IsGM() )
				{
					if( i->GetType() == IT_RECALLRUNE || (curSpell != 45 && i->GetType() == IT_KEY && cwmWorldState->ServerData()->TravelSpellsFromBoatKeys() ))
					{
						playSound( src, curSpell );
						doMoveEffect( curSpell, i, src );
						doStaticEffect( src, curSpell );
						switch( curSpell )
						{
							case 32: //////////// (32) RECALL ////////////////
							case 45: //////////// (45) MARK //////////////////
							case 52: //////////// (52) GATE //////////////////
							{
								scriptTriggers.clear();
								scriptTriggers.shrink_to_fit();
								scriptTriggers = i->GetScriptTriggers();
								for( auto scriptTrig : scriptTriggers )
								{
									cScript *toExecute = JSMapping->GetScript( scriptTrig );
									if( toExecute != nullptr )
									{
										// If script returns true/1, prevent other scripts with event from running
										if( toExecute->OnSpellTarget( i, caster, curSpell ) == 1 )
										{
											break;
										}
									}
								}

								(*((MAGIC_ITEMFUNC)magic_table[curSpell-1].mag_extra))( s, caster, i, curSpell );
								break;
							}
							default:
								Console.error( strutil::format(" Unknown Travel spell %i, magic.cpp", curSpell) );
								break;
						}
					}
					else if( validSocket )
						s->sysmessage( 710 );
				}
				else if( validSocket )
					s->sysmessage( 683 );
			}
			else if( validSocket )
				s->sysmessage( 710 );
			return;
		}

		if( spells[curSpell].RequireCharTarget() )
		{
			// TARGET CALC HERE
			CChar *c = nullptr;
			if( validSocket && !caster->IsNpc() )
				c = calcCharObjFromSer( s->GetDWord( 7 ) );
			else
				c = caster->GetTarg();

			if( ValidateObject( c ) ) // we have to have targetted a person to kill them :)
			{
				if( !objInRange( c, caster, cwmWorldState->ServerData()->CombatMaxSpellRange() ) )
				{
					if( validSocket )
						s->sysmessage( 712 );
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
								s->sysmessage( 1799 );
							return;
						}
						else if( !c->GetRegion()->CanCastAggressive() )
						{
							// Target is in a region where casting aggressive spells is forbidden
							if( validSocket )
								s->sysmessage( 709 );
							return;
						}
						if( c->GetNPCAiType() == AI_PLAYERVENDOR || c->IsInvulnerable() )
						{
							if( validSocket )
								s->sysmessage( 713 );
							return;
						}
						Combat->AttackTarget( caster, c );
					}
					if( spells[curSpell].SpellReflectable() )
					{
						if( CheckMagicReflect( c ) )
						{
							src = c;
							c = caster;
						}
					}
					if( curSpell != 43 )
					{
						if( caster->IsNpc() )
							playSound( c, curSpell );
						else
							playSound( src, curSpell );

						doMoveEffect( curSpell, c, src );
						doStaticEffect( c, curSpell );
					}
					switch( curSpell )
					{
						case 1:  // Clumsy
						case 3:  // Feeblemind
						case 4:  // Heal 2-26-00 used OSI values from UO book
						case 5:  // Magic Arrow
						case 6:  // Night Sight
						case 7:
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
							scriptTriggers.clear();
							scriptTriggers.shrink_to_fit();
							scriptTriggers = i->GetScriptTriggers();
							for( auto scriptTrig : scriptTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( scriptTrig );
								if( toExecute != nullptr )
								{
									toExecute->OnSpellTarget( c, caster, curSpell );
								}
							}
							(*((MAGIC_CHARFUNC)magic_table[curSpell-1].mag_extra))( caster, c, src, curSpell );
							break;
						}
						case 46:	// Mass cure
						case 25:	// Arch Cure
						case 26:	// Arch Protection
						case 49:	// Chain Lightning
						case 54:	// Mass Dispel
						case 55:	// Meteor Swarm
						{
							if( ValidateObject( i ))
							{
								scriptTriggers.clear();
								scriptTriggers.shrink_to_fit();
								scriptTriggers = i->GetScriptTriggers();
								for( auto scriptTrig : scriptTriggers )
								{
									cScript *toExecute = JSMapping->GetScript( scriptTrig );
									if( toExecute != nullptr )
									{
										toExecute->OnSpellTarget( c, caster, curSpell );
									}
								}
								(*((MAGIC_TESTFUNC)magic_table[curSpell-1].mag_extra))( s, caster, c, src, curSpell );
							}
							break;
						}
						default:
							Console.error( strutil::format(" Unknown CharacterTarget spell %i, magic.cpp", curSpell) );
							break;
					}
				}
				else if( validSocket )
					s->sysmessage( 683 );
			}
			else if( validSocket )
				s->sysmessage( 715 );
		}
		else if( spells[curSpell].RequireLocTarget() )
		{
			SI16 x = 0;
			SI16 y = 0 ;
			SI08 z=0;
			CBaseObject *getTarg = nullptr;
			if( !caster->IsNpc() )
			{
				if( s->GetDWord( 7 ) != INVALIDSERIAL )
				{
					SERIAL targSer = s->GetDWord( 7 );
					if( targSer >= BASEITEMSERIAL )
						getTarg = calcItemObjFromSer( targSer );
					else
						getTarg = calcCharObjFromSer( targSer );
				}
				else
				{
					x = s->GetWord( 11 );
					y = s->GetWord( 13 );
					z = s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) );
				}
			}
			else
				getTarg = caster->GetTarg();

			if( ValidateObject( getTarg ) )
			{
				if( !getTarg->CanBeObjType( OT_ITEM ) || (static_cast<CItem *>(getTarg))->GetCont() == nullptr )
				{
					x = getTarg->GetX();
					y = getTarg->GetY();
					z = getTarg->GetZ();
				}
				else if( validSocket )
				{
					s->sysmessage( 717 );
					return;
				}
			}

			if( spells[curSpell].AggressiveSpell() )
			{
				CTownRegion *targetRegion = calcRegionFromXY( x, y, caster->WorldNumber(), caster->GetInstanceID() );
				if( targetRegion->IsSafeZone() )
				{
					// Target location is in a region where hostile actions are forbidden, disallow
					if( validSocket )
						s->sysmessage( 1799 );
					return;
				}
				else if( !targetRegion->CanCastAggressive() )
				{
					// Target location is in a region where casting aggressive spells is forbidden, disallow
					if( validSocket )
						s->sysmessage( 709 );
					return;
				}
			}

			// Check distance to target location vs CombatMaxSpellRange
			UI16 targDist;
			point3 targLocation = point3( x, y, z );
			point3 difference	= caster->GetLocation() - targLocation;
			targDist			= static_cast<UI16>(difference.Mag());
			if( targDist > cwmWorldState->ServerData()->CombatMaxSpellRange() )
			{
				if( validSocket )
					s->sysmessage( 393 ); // That is too far away.
				return;
			}

			if( LineOfSight( s, caster, x, y, ( z + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) )
			{
				if( spells[curSpell].FieldSpell() )
				{
					UI08 j = getFieldDir( caster, x, y );
					playSound( src, curSpell );
					doStaticEffect( src, curSpell );
					(*((MAGIC_FIELDFUNC)magic_table[curSpell-1].mag_extra))( s, caster, j, x, y, z, curSpell );
				}
				else if( spells[curSpell].RequireNoTarget() )
				{
					playSound( src, curSpell );
					doStaticEffect( src, curSpell );
					switch( curSpell )
					{
						case 2: //create food
						case 22:// Teleport
						case 25:// Arch Cure
						case 26:// Arch Protection
						case 33:// Blade Spirits
						case 34:// Dispel Field
						case 40:	// Summon Creature
						case 46:// Mass curse
						case 48:
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
							(*((MAGIC_LOCFUNC)magic_table[curSpell-1].mag_extra))( s, caster, x, y, z, curSpell );
							break;
						default:
							Console.error(strutil::format( " Unknown LocationTarget spell %i", curSpell) );
							break;
					}
				}
			}
			else if( validSocket )
				s->sysmessage( 717 );
		}
		else if( spells[curSpell].RequireItemTarget() && validSocket )
		{
			i = calcItemObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( i ) )
			{
				if( ( i->GetCont() != nullptr && FindItemOwner( i ) != caster ) || ( !objInRange( caster, i, cwmWorldState->ServerData()->CombatMaxSpellRange() ) ) )
				{
					s->sysmessage( 718 );
					return;
				}
				if( LineOfSight( s, caster, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) || caster->IsGM() )
				{
					switch( curSpell )
					{
						case 13:	// Magic Trap
						case 14:	// Magic Untrap
						case 19:	// Magic Lock
						case 23:	// Magic Unlock
						case 21:	// Telekinesis
							(*((MAGIC_ITEMFUNC)magic_table[curSpell-1].mag_extra))( s, caster, i, curSpell );
							break;
						default:
							Console.error( strutil::format(" Unknown ItemTarget spell %i, magic.cpp", curSpell) );
							break;
					}
				}
				else
					s->sysmessage( 683 );
			}
			else
				s->sysmessage( 711 );
		}
		else if( validSocket )
			s->sysmessage( 720 );
		return;
	}
	else if( spells[curSpell].RequireNoTarget() )
	{
		// non targetted spells
		playSound( src, curSpell );
		doStaticEffect( src, curSpell );
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
				(*((MAGIC_NOFUNC)magic_table[curSpell-1].mag_extra))( s, caster, curSpell );
				break;
			default:
				Console.error( strutil::format(" Unknown NonTarget spell %i, magic.cpp", curSpell) );
				break;
		}
		return;
	}

}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadScript( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads spell data from spell DFNs
//|
//|	Notes		-	Avoid multiple reading of the spell script every time a spell is
//|					cast to avoid crippling the server when a mage enters combat
//o-----------------------------------------------------------------------------------------------o
void cMagic::LoadScript( void )
{
	spells.resize( 0 );

	// for some strange reason, spells go from index 1 to SPELL_MAX and
	// apparently index 0 is left unused
	spells.resize( SPELL_MAX + 1 );

	std::string spEntry;
	std::string tag, data, UTag;
	UI08 i = 0;
	for( Script *spellScp = FileLookup->FirstScript( spells_def ); !FileLookup->FinishedScripts( spells_def ); spellScp = FileLookup->NextScript( spells_def ) )
	{
		if( spellScp == nullptr )
			continue;

		for( ScriptSection *SpellLoad = spellScp->FirstEntry(); SpellLoad != nullptr; SpellLoad = spellScp->NextEntry() )
		{
			if( SpellLoad == nullptr )
				continue;

			spEntry = spellScp->EntryName();
			auto ssecs = strutil::sections( spEntry, " " );
			if( ssecs[0] == "SPELL" )
			{
				i = static_cast<UI08>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 0) );
				if( i <= SPELL_MAX )
				{
					++spellCount;
					spells[i].Enabled( false );
					reag_st *mRegs = spells[i].ReagantsPtr();

					//Console.Log( "Spell number: %i", "spell.log", i ); // Disabled for performance reasons

					for( tag = SpellLoad->First(); !SpellLoad->AtEnd(); tag = SpellLoad->Next() )
					{
						UTag = strutil::toupper( tag );
						data = SpellLoad->GrabData();
						data = strutil::stripTrim( data );
						//Console.Log( "Tag: %s\tData: %s", "spell.log", UTag.c_str(), data.c_str() ); // Disabled for performance reasons
						switch( (UTag.data()[0]) )
						{
							case 'A':
								if( UTag == "ACTION" )
								{
									spells[i].Action( static_cast<UI16>(std::stoul(data, nullptr, 0))  );
								}
								else if( UTag == "ASH" )
								{
									mRegs->ash =static_cast<UI08>(std::stoul(data, nullptr, 0));
								}
								break;
							case 'B':
								if( UTag == "BASEDMG" )
								{
									spells[i].BaseDmg( static_cast<SI16>(std::stoi(data, nullptr, 0)) );
								}
								break;
							case 'C':
								if( UTag == "CIRCLE" )
								{
									spells[i].Circle( static_cast<UI08>(std::stoul(data, nullptr, 0)) );
								}
								break;
							case 'D':
								if( UTag == "DELAY" )
								{
									spells[i].Delay( std::stoi(data, nullptr, 0) );
								}
								else if( UTag == "DRAKE" )
								{
									mRegs->drake = static_cast<UI08>(std::stoul(data, nullptr, 0));
								}
								break;
							case 'E':
								if( UTag == "ENABLE" ) // presence of enable is enough to enable it
								{
									spells[i].Enabled(std::stoul(data, nullptr, 0) != 0 );
								}
								break;
							case 'F':
								if( UTag == "FLAGS" )
								{
									auto ssecs = strutil::sections( data, " " );
									if( ssecs.size() > 1 )
									{
										spells[i].Flags(((static_cast<UI08>(std::stoul(strutil::stripTrim( ssecs[0] ), nullptr, 16)))<<8) ||
												    static_cast<UI08>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 16)));
												    
									}
									else
									{
										spells[i].Flags( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
									}
								}
								break;
							case 'G':
								if( UTag == "GARLIC" )
								{
									mRegs->garlic  = static_cast<UI08>(std::stoul(data, nullptr, 0));
								}
								else if( UTag == "GINSENG" )
								{
									mRegs->ginseng = static_cast<UI08>(std::stoul(data, nullptr, 0));
								}
								break;
							case 'H':
								if( UTag == "HISKILL" )
								{
									spells[i].HighSkill( static_cast<SI16>(std::stoi(data, nullptr, 0)));
								}
								else if( UTag == "HEALTH" )
								{
									spells[i].Health( static_cast<SI16>(std::stoi(data, nullptr, 0)) );
								}
								break;
							case 'L':
								if( UTag == "LOSKILL" )
								{
									spells[i].LowSkill( static_cast<SI16>(std::stoi(data, nullptr, 0)) );
								}
								break;
							case 'M':
								if( UTag == "MANA" )
								{
									spells[i].Mana( static_cast<SI16>(std::stoi(data, nullptr, 0)) );
								}
								else if( UTag == "MANTRA" )
								{
									spells[i].Mantra( data );
								}
								else if( UTag == "MOSS" )
								{
									mRegs->moss = static_cast<UI08>(std::stoul(data, nullptr, 0));
								}
								else if( UTag == "MOVEFX" )
								{
									auto ssecs = strutil::sections( data, " " );
									if( ssecs.size() > 1 )
									{										
										CMagicMove *mv = spells[i].MoveEffectPtr();
										mv->Effect( static_cast<UI08>(std::stoul(strutil::stripTrim(ssecs[0]), nullptr, 16)), static_cast<UI08>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 16)) );
										mv->Speed( static_cast<UI08>(std::stoul(strutil::stripTrim(ssecs[2]), nullptr, 16)) );
										mv->Loop( static_cast<UI08>(std::stoul(strutil::stripTrim(ssecs[3]), nullptr, 16)) );
										mv->Explode( static_cast<UI08>(std::stoul(strutil::stripTrim(ssecs[4]), nullptr, 16)));
									}
								}
								break;
							case 'P':
								if( UTag == "PEARL" )
								{
									mRegs->pearl = static_cast<UI08>(std::stoul(data,nullptr,0));
								}
								break;
							case 'S':
								if( UTag == "SHADE" )
								{
									mRegs->shade = static_cast<UI08>(std::stoul(data,nullptr,0));
								}
								else if( UTag == "SILK" )
								{
									mRegs->silk = static_cast<UI08>(std::stoul(data,nullptr,0));
								}
								else if( UTag == "SOUNDFX" )
								{
									auto ssecs = strutil::sections( data, " " );
									if( ssecs.size() > 1 )
									{
										spells[i].Effect( ( (static_cast<UI08>(std::stoul(strutil::stripTrim( ssecs[0] ), nullptr, 16))<<8) ||
												    static_cast<UI08>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 16))));
									}
									else
									{
										spells[i].Effect( static_cast<UI16>(std::stoul(data,nullptr,0)) );
									}
								}
								else if( UTag == "STATFX" )
								{
									auto ssecs = strutil::sections( data, " " );
									if( ssecs.size() > 1 )
									{
										CMagicStat *stat = spells[i].StaticEffectPtr();
										
										stat->Effect( static_cast<UI08>(std::stoul(strutil::stripTrim(ssecs[0]), nullptr, 16)), static_cast<UI08>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 16)) );
										stat->Speed( static_cast<UI08>(std::stoul(strutil::stripTrim(ssecs[2]), nullptr, 16)) );
										stat->Loop( static_cast<UI08>(std::stoul(strutil::stripTrim(ssecs[3]), nullptr, 16)) );
									}
								}
								else if( UTag == "SCLO" )
								{
									spells[i].ScrollLow( static_cast<SI16>(std::stoi(data, nullptr, 0)) );
								}
								else if( UTag == "SCHI" )
								{
									spells[i].ScrollHigh(  static_cast<SI16>(std::stoi(data, nullptr, 0)) );
								}
								else if( UTag == "STAMINA" )
								{
									spells[i].Stamina(  static_cast<SI16>(std::stoi(data, nullptr, 0)));
								}
								break;
							case 'T':
								if( UTag == "TARG" )
								{
									spells[i].StringToSay( data );
								}
								break;
						}
					}
				}
			}
		}
	}

#if defined( UOX_DEBUG_MODE )
	Console.print( "Registering spells\n" );
#endif

	CJSMappingSection *spellSection = JSMapping->GetSection( SCPT_MAGIC );
	for( cScript *ourScript = spellSection->First(); !spellSection->Finished(); ourScript = spellSection->Next() )
	{
		if( ourScript != nullptr )
			ourScript->ScriptRegistration( "Spell" );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DelReagents( CChar *s, reag_st reags )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes the specified amount of reagents when a spell is cast
//o-----------------------------------------------------------------------------------------------o
void cMagic::DelReagents( CChar *s, reag_st reags )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void playSound( CChar *source, SI32 num )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the SFX associated with a spell that is being cast
//o-----------------------------------------------------------------------------------------------o
void cMagic::playSound( CChar *source, SI32 num )
{
	if( spells[num].Effect() != INVALIDID )
		Effects->PlaySound( source, spells[num].Effect() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void doStaticEffect( CChar *source, SI32 num )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the static FX associated with a spell that is being cast
//o-----------------------------------------------------------------------------------------------o
void cMagic::doStaticEffect( CChar *source, SI32 num )
{
	CMagicStat temp = spells[num].StaticEffect();
	if( temp.Effect() != INVALIDID )
		Effects->PlayStaticAnimation( source, temp.Effect(), temp.Speed(), temp.Loop() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void doMoveEffect( SI32 num, CBaseObject *target, CChar *source )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the moving FX associated with a spell that is being cast
//o-----------------------------------------------------------------------------------------------o
void cMagic::doMoveEffect( SI32 num, CBaseObject *target, CChar *source )
{
	CMagicMove temp = spells[num].MoveEffect();

	if( temp.Effect() != INVALIDID )
		Effects->PlayMovingAnimation( source, target, temp.Effect(), temp.Speed(), temp.Loop(), ( temp.Explode() == 1 ) );
}

void HandleCommonGump( CSocket *mSock, ScriptSection *gumpScript, UI16 gumpIndex );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PolymorphMenu( CSocket *s, UI16 gmindex )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the polymorph menu to allow player to choose which creature to poly into
//o-----------------------------------------------------------------------------------------------o
void cMagic::PolymorphMenu( CSocket *s, UI16 gmindex )
{
	std::string sect			= "POLYMORPHMENU " + strutil::number( gmindex );
	ScriptSection *polyStuff	= FileLookup->FindEntry( sect, menus_def );
	if( polyStuff == nullptr )
		return;
	HandleCommonGump( s, polyStuff, gmindex );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Polymorph( CSocket *s, UI16 polyID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies effects of Polymorph spell to caster
//o-----------------------------------------------------------------------------------------------o
void cMagic::Polymorph( CSocket *s, UI16 polyID )
{
	UI08 id1		= static_cast<UI08>(polyID>>8);
	UI08 id2		= static_cast<UI08>(polyID%256);
	CChar *mChar	= s->CurrcharObj();

	// store our original ID
	mChar->SetOrgID( mChar->GetID() );
	Effects->PlaySound( mChar, 0x020F );
	// Temp effect will actually switch our body for us
	Effects->tempeffect( mChar, mChar, 18, id1, id2, 0 );
	mChar->IsPolymorphed( true );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Log( std::string spell, CChar *player1, CChar *player2, const std::string &extraInfo )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out spell casting details to spell.log file
//o-----------------------------------------------------------------------------------------------o
void cMagic::Log( std::string spell, CChar *player1, CChar *player2, const std::string &extraInfo )
{
	if( spell.empty() || !ValidateObject( player1 ) )
		return;
	std::string logName = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + "spell.log";
	std::ofstream logDestination;
	logDestination.open( logName.c_str(), std::ios::out | std::ios::app );
	if( !logDestination.is_open() )
	{
		Console.error(strutil::format( "Unable to open spell log file %s!", logName.c_str() ));
		return;
	}
	char dateTime[1024];
	RealTime( dateTime );

	logDestination << "[" << dateTime << "] ";
	logDestination << player1->GetName() << " (serial: " << std::hex << player1->GetSerial() << " ) ";
	logDestination << "cast spell <" << spell << "> ";
	if( ValidateObject( player2 ) )
		logDestination << "on player " << player2->GetName() << " (serial: " << player2->GetSerial() << " ) ";
	logDestination << "Extra Info: " << extraInfo << std::endl;
	logDestination.close();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Register( cScript *toRegister, SI32 spellNumber, bool isEnabled )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers a spell in the JS Engine
//o-----------------------------------------------------------------------------------------------o
void cMagic::Register( cScript *toRegister, SI32 spellNumber, bool isEnabled )
{
#if defined( UOX_DEBUG_MODE )
	Console.print( strutil::format("Registering spell number %i\n", spellNumber ));
#endif
	if( spellNumber < 0 || static_cast<size_t>(spellNumber) >= spells.size() )
		return;
	if( toRegister == nullptr )
		return;

	spells[spellNumber].JSScript( JSMapping->GetScriptID( toRegister->Object() ) );
	spells[spellNumber].Enabled( isEnabled );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetSpellStatus( SI32 spellNumber, bool isEnabled )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the enabled state of a given spell
//o-----------------------------------------------------------------------------------------------o
void cMagic::SetSpellStatus( SI32 spellNumber, bool isEnabled )
{
	if( spellNumber < 0 || static_cast<size_t>(spellNumber) >= spells.size() )
		return;
	spells[spellNumber].Enabled( isEnabled );
}
