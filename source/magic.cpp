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

#include "ObjectFactory.h"

#undef DBGFILE
#define DBGFILE "magic.cpp"

namespace UOX
{

cMagic *Magic = NULL;

#define SPELL_MAX 68 //:Terrin: use define for now; can make autocount later

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
	{ 0, NULL } 
};

//o---------------------------------------------------------------------------o
//|	Function	-	SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, SI16 trgX, SI16 trgY, SI08 trgZ )
//|	Programmer	-	Abaddon
//| Date		-	17th September, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	It will construct 2 linked gates, one at srcX / srcY / srcZ and another at trgX / trgY / trgZ
//o---------------------------------------------------------------------------o
void SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, UI08 srcWorld, SI16 trgX, SI16 trgY, SI08 trgZ, UI08 trgWorld )
{
	CItem *g1 = Items->CreateItem( NULL, caster, 0x0F6C, 1, 0, OT_ITEM );
	if( ValidateObject( g1 ) )
	{
		g1->SetDecayable( true );
		g1->SetType( IT_GATE );
		g1->SetLocation( srcX, srcY, srcZ, srcWorld );
		g1->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_GATE ) );
		g1->SetDir( 1 );

		CItem *g2 = Items->CreateItem( NULL, caster, 0x0F6C, 1, 0, OT_ITEM );
		if( ValidateObject( g2 ) )
		{
			g2->SetDecayable( true );
			g2->SetType( IT_GATE );
			g2->SetLocation( trgX, trgY, trgZ, trgWorld );
			g2->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_GATE ) );
			g2->SetDir( 1 );

			g2->SetTempVar( CITV_MOREX, g1->GetSerial() );
			g1->SetTempVar( CITV_MOREX, g2->GetSerial() );
		}
		else
			g1->Delete();
	}
}

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
	CItem *i = NULL;
	UI08 worldNumber = caster->WorldNumber();
	for( UI08 j = 0; j < 5; ++j )		// how about we make this 5, huh?  missing part of the field
	{
		i = Items->CreateItem( NULL, caster, id, 1, 0, OT_ITEM );
		if( i != NULL )
		{
			i->SetDispellable( true );
			i->SetDecayable( true );
			i->SetDecayTime( BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 15 )) );
			i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
			i->SetTempVar( CITV_MOREY, caster->GetSerial() );
			i->SetLocation( fx[j], fy[j], Map->Height( fx[j], fy[j], z, worldNumber ) );
			i->SetDir( 29 );
			i->SetMovable( 2 );
		}
	}
	return true;
}

bool splClumsy( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 3, caster->GetSkill( MAGERY )/100, 0, 0);
	return true;
}
bool splCreateFood( CSocket *sock, CChar *caster )
{
	CItem *j = Items->CreateItem( sock, caster, 0x09D3, 1, 0x0000, OT_ITEM, true );
	if( ValidateObject( j ) )
		j->SetType( IT_FOOD );
	return true;
}
bool splFeeblemind( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 4, caster->GetSkill( MAGERY )/100, 0, 0);
	return true;
}
bool splHeal( CChar *caster, CChar *target, CChar *src )
{
	int bonus = (caster->GetSkill( MAGERY )/500) + ( caster->GetSkill( MAGERY )/100 );
	if( bonus != 0 )
		target->Heal( RandomNum( 0, 5 ) + bonus, caster );
	else
		target->Heal( 4 );
	Magic->SubtractHealth( caster, bonus, 4 );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splMagicArrow( CChar *caster, CChar *target, CChar *src )
{
	Magic->MagicDamage( target, (1+(RandomNum( 0, 1 ))+1)*(caster->GetSkill( MAGERY )/2000+1), caster, COLD );
	return true;
}
bool splNightSight( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 2, 0, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splReactiveArmor( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 15, caster->GetSkill( MAGERY )/100, 0, 0 );
	target->SetReactiveArmour( true );
	return true;
}
bool splWeaken( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 5, caster->GetSkill( MAGERY )/100, 0, 0);
	return true;
}
bool splAgility( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 6, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splCunning( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 7, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splCure( CChar *caster, CChar *target, CChar *src )
{
	target->SetPoisoned( 0 );
	target->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->GetUICurrentTime() );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splHarm( CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 2 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/500+1, caster, COLD );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/250+RandomNum( 1, 2 ), caster, COLD );
	return true;
}
bool splMagicTrap( CSocket *sock, CChar *caster, CItem *target )
{
	if( target->IsContType() && target->GetID() != 0x0E75 )  
	{
		target->SetTempVar( CITV_MORE, calcserial( 1, caster->GetSkill( MAGERY ) / 20, caster->GetSkill( MAGERY ) / 10, 0 ) );
		Effects->PlaySound( target, 0x01F0 );
	} 
	else 
		sock->sysmessage( 663 );
	return true;
}
bool splMagicUntrap( CSocket *sock, CChar *caster, CItem *target )
{
	if( target->IsContType() )
	{
		if( target->GetTempVar( CITV_MORE, 1 ) == 1 )
		{
			if( RandomNum( 1, 100 ) <= 50 + ( caster->GetSkill( MAGERY )/10) - target->GetTempVar( CITV_MORE, 3 ) )
			{
				target->SetTempVar( CITV_MORE, 0 );
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
bool splProtection( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 21, caster->GetSkill( MAGERY )/10, 0, 0 );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splStrength( CChar *caster, CChar *target, CChar *src )
{
	Effects->tempeffect( src, target, 8, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splBless( CChar *caster, CChar *target, CChar *src )
{
	int j = caster->GetSkill( MAGERY )/100;
	Effects->tempeffect( src, target, 11, j, j, j);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splFireball( CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 3 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 280 + 1, caster, HEAT );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 140 + RandomNum( 1, 4 ), caster, HEAT );
	return true;
}
bool splMagicLock( CSocket *sock, CChar *caster, CItem *target )
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
				Console.Error( "Fallout of switch statement without default. magic.cpp, magiclocktarget()" );
				break;
		}
		Effects->PlaySound( target, 0x0200 );
	}
	else 
		sock->sysmessage( 669 ); 
	return true;
}
bool splPoison( CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 1 ) )
		return false;
		
	target->SetPoisoned( 2 );   
	target->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POISON ) );

	return true;
}
bool splTelekinesis( CSocket *sock, CChar *caster, CItem *target )
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
bool splTeleport( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	CMultiObj *m = findMulti( x, y, z, caster->WorldNumber() );
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
bool splUnlock( CSocket *sock, CChar *caster, CItem *target )
{
	if( target->isDevineLocked() )
	{
		sock->sysmessage( 673 );
		return false;
	}
	if( target->GetType() == IT_LOCKEDCONTAINER )
	{
		target->SetType( IT_CONTAINER );
		sock->sysmessage( 674 );
	} 
	else if( target->GetType() == IT_LOCKEDSPAWNCONT )
	{
		target->SetType( IT_SPAWNCONT );
		sock->sysmessage( 675 );
	} 
	else if( target->GetType() == IT_CONTAINER || target->GetType() == IT_SPAWNCONT || target->GetType() == IT_LOCKEDSPAWNCONT || target->GetType() == IT_TRASHCONT )
		sock->sysmessage( 676 );
	else if( target->GetType() == IT_DOOR )
		sock->sysmessage( 677 );
	else 
		sock->sysmessage( 678 );
	return true;
}
bool splWallOfStone( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	return FieldSpell( caster, 0x0080, x, y, z, fieldDir );
}

void ArchCureStub( CChar *caster, CChar *target )
{
	target->SetPoisoned( 0 );
	target->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->GetUICurrentTime() );
}
bool splArchCure( CSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	AreaAffectSpell( sock, caster, &ArchCureStub );
	return true;
}

void ArchProtectionStub( CChar *caster, CChar *target )
{
	Magic->playSound( target, 26 );
	Magic->doStaticEffect( target, 15 );	// protection
	Effects->tempeffect( caster, target, 21, caster->GetSkill( MAGERY )/10, 0, 0 );
}
bool splArchProtection( CSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	AreaAffectSpell( sock, caster, &ArchProtectionStub );
	return true;
}
bool splCurse( CChar *caster, CChar *target, CChar *src )
{
	if( !Magic->CheckResist( caster, target, 1 ) )
	{
		int j = caster->GetSkill( MAGERY ) / 100;
		Effects->tempeffect(caster, target, 12, j, j, j);
	}
	return true;
}
bool splFireField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	return FieldSpell( caster, 0x398C, x, y, z, fieldDir );
}
bool splGreaterHeal( CChar *caster, CChar *target, CChar *src )
{
	int srcHealth	= target->GetHP();
	int j			= caster->GetSkill( MAGERY ) / 30 + RandomNum( 1, 12 );
	target->Heal( j, caster );
	Magic->SubtractHealth( caster, UOX_MIN( target->GetStrength(), (SI16)(srcHealth + j) ), 29 );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splLightning( CChar *caster, CChar *target, CChar *src )
{
	Effects->bolteffect( target );
	if( Magic->CheckResist( caster, target, 4 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 180 + RandomNum( 1, 2 ), caster, LIGHTNING );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 90 + RandomNum( 1, 5 ), caster, LIGHTNING );
	return true;
}
bool splManaDrain( CChar *caster, CChar *target, CChar *src )
{
	if( !Magic->CheckResist( caster, target, 4 ) )
	{
		target->SetMana( target->GetMana() - caster->GetSkill( MAGERY )/35 );
		if( target->GetMana() < 0 ) 
			target->SetMana( 0 );
	}
	return true;
}
bool splRecall( CSocket *sock, CChar *caster, CItem *i )
{
	if( i->GetTempVar( CITV_MOREX ) <= 200 && i->GetTempVar( CITV_MOREY ) <= 200 )
	{
		sock->sysmessage( 679 );
		return false;
	}
	else if( Weight->isOverloaded( caster ) && ( !caster->IsCounselor() && !caster->IsGM() ) ) // no recall if too heavy, GM's excempt
	{
		sock->sysmessage( 680 );
		sock->sysmessage( 681 );
		return false;
	}
	else
	{
		UI08 worldNum = static_cast<UI08>(i->GetTempVar( CITV_MORE ));
		if( !Map->MapExists( worldNum ) )
			worldNum = caster->WorldNumber();
		caster->SetLocation( static_cast<SI16>(i->GetTempVar( CITV_MOREX )), static_cast<SI16>(i->GetTempVar( CITV_MOREY )), static_cast<SI08>(i->GetTempVar( CITV_MOREZ )), worldNum );
		sock->sysmessage( 682 );
		return true;
	}
	return false;
}
bool splBladeSpirits( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	Magic->SummonMonster( sock, caster, 6, x, y, z );
	return true;
}
bool splDispelField( CSocket *sock, CChar *caster )
{
	CItem *i = calcItemObjFromSer( sock->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		if( LineOfSight( sock, caster, i->GetX(), i->GetY(), ( i->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
		{
			if( i->isDecayable() || i->isDispellable() ) 
			{
				UI16 scpNum = i->GetScriptTrigger();
				cScript *tScript = JSMapping->GetScript( scpNum );
				if( tScript != NULL )
					tScript->OnDispel( i );
				i->Delete();
			}
			Effects->PlaySound( caster, 0x0201 );
		}
		else
			sock->sysmessage( 683 );
	}
	return true;
}
void setRandomName( CChar *s, const std::string namelist );

bool splIncognito( CSocket *sock, CChar *caster )
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

	int color	= RandomNum( 0x044E, 0x047D );
	CItem *j	= caster->GetItemAtLayer( IL_HAIR );
	if( ValidateObject( j ) ) 
	{
		caster->SetHairColour( j->GetColour() );
		caster->SetHairStyle( j->GetID() );
		int rNum = RandomNum( 0, 9 );
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
			int rNum2 = RandomNum( 0, 6 );
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
bool splMagicReflection( CSocket *sock, CChar *caster )
{
	caster->SetPermReflected( true );
	return true;
}
bool splMindBlast( CChar *caster, CChar *target, CChar *src )
{
	if( caster->GetIntelligence() > target->GetIntelligence() )
	{
		if( Magic->CheckResist( caster, target, 5 ) )
			Magic->MagicDamage( target, (src->GetIntelligence() - target->GetIntelligence())/4, src, COLD );
		else
			Magic->MagicDamage( target, (src->GetIntelligence() - target->GetIntelligence())/2, src, COLD );
	}
	else
	{
		if( Magic->CheckResist( caster, src, 5 ) )
			Magic->MagicDamage( src, (target->GetIntelligence() - src->GetIntelligence())/4, src, COLD );
		else
			Magic->MagicDamage( src, (target->GetIntelligence() - src->GetIntelligence())/2, src, COLD );
	}
	return true;
}
bool splParalyze( CChar *caster, CChar *target, CChar *src )
{
	if( !Magic->CheckResist( caster, target, 7 ) )
		Effects->tempeffect( caster, target, 1, 0, 0, 0 );
	return true;
}
bool splPoisonField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	return FieldSpell( caster, 0x3915, x, y, z, fieldDir );
}
bool splSummonCreature( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	if( caster->GetSkill( MAGERY ) <= 380 )
		return false;
	else
		Magic->SummonMonster( sock, caster, 0, x, y, z );
	return true;
}
bool splDispel( CChar *caster, CChar *target, CChar *src )
{
	if( target->IsDispellable() )
	{
		Effects->PlayStaticAnimation( target, 0x372A, 0x09, 0x06 );		// why the specifics here?
		if( target->IsNpc() ) 
		{
			cScript *tScript = NULL;
			UI16 scpNum = target->GetScriptTrigger();
			tScript = JSMapping->GetScript( scpNum );
			if( tScript != NULL )
				tScript->OnDispel( target );
			target->Delete();
		}
		else 
			HandleDeath( target );
	}
	return true;
}
bool splEnergyBolt( CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 6 ) ) 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/120, caster, LIGHTNING );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/35+RandomNum(1,10), caster, LIGHTNING );
	return true;
}
bool splExplosion( CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 6 ) )
		Effects->tempeffect( src, target, 27, (caster->GetSkill( MAGERY ) / 120 + RandomNum( 1, 5 ) ), 0, 0 );
	else 
		Effects->tempeffect( src, target, 27, (caster->GetSkill( MAGERY ) / 40 + RandomNum( 1, 10 ) ), 0, 0 );
	return true;
}
bool splInvisibility( CChar *caster, CChar *target, CChar *src )
{
	target->SetVisible( VT_INVISIBLE );
	target->SetTimer( tCHAR_INVIS, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_INVISIBILITY ) );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	bool splMark( CSocket *sock, CChar *caster, CItem *i )
//|	Date					-	09/22/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Mark rune spell?
//|									
//|	Modification	-	09/22/2002	-	Xuri - Fixed marking of locked down runes.. 
//|									(shouldn't be possible)
//o--------------------------------------------------------------------------o
//|	Returns				-	[TRUE] If Successfull, [FALSE] otherwise
//o--------------------------------------------------------------------------o	
bool splMark( CSocket *sock, CChar *caster, CItem *i )
{
	// Sept 22, 2002 - Xuri
	if( i->IsLockedDown() )
	{
		sock->sysmessage( 774 );
		return false;
	}
	//	
	i->SetTempVar( CITV_MOREX, caster->GetX() ); 
	i->SetTempVar( CITV_MOREY, caster->GetY() );
	i->SetTempVar( CITV_MOREZ, caster->GetZ() );
	i->SetTempVar( CITV_MORE, caster->WorldNumber() );

	char tempitemname[512];

	if( caster->GetRegion()->GetName()[0] != 0 )
		sprintf( tempitemname, Dictionary->GetEntry( 684 ).c_str(), caster->GetRegion()->GetName().c_str() );
	else 
		strcpy( tempitemname, Dictionary->GetEntry( 685 ).c_str() );
	i->SetName( tempitemname );
	sock->sysmessage( 686 ); 
	return true;
}

void MassCurseStub( CChar *caster, CChar *target )
{
	if( target->IsNpc() && target->GetNPCAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	int j;
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
bool splMassCurse( CSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	AreaAffectSpell( sock, caster, &MassCurseStub );
	return true;
}
bool splParalyzeField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	return FieldSpell( caster, 0x3967, x, y, z, fieldDir );
}
bool splReveal( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	if( LineOfSight( sock, caster, x, y, ( z + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
	{ 
		UI16 j = caster->GetSkill( MAGERY ); 
		int range=(((j-261)*(15))/739)+5;
		//If the caster has a Magery of 26.1 (min to cast reveal w/ scroll), range  radius is
		//5 tiles, if magery is maxed out at 100.0 (except for gms I suppose), range is 20
		
		REGIONLIST nearbyRegions = MapRegion->PopulateList( caster );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( !ValidateObject( tempChar ) )
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

void ChainLightningStub( CChar *caster, CChar *target )
{
	if( target->IsNpc() && target->GetNPCAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	if( target->IsNpc() )
		Combat->AttackTarget( target, caster );
	Effects->PlaySound( caster, 0x0029 );
	CChar *def = NULL;
	if( Magic->CheckMagicReflect( target ) )
		def = caster;
	else
		def = target;

	Effects->bolteffect( def );
	if( Magic->CheckResist( caster, target, 4 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 180 + RandomNum( 1 ,2 ), caster, LIGHTNING );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 90 + RandomNum( 1, 5 ), caster, LIGHTNING );
}
bool splChainLightning( CSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	AreaAffectSpell( sock, caster, &ChainLightningStub );
	return true;
}
bool splEnergyField( CSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	return FieldSpell( caster, 0x3946, x, y, z, fieldDir );
}
bool splFlameStrike( CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 7 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/80, caster, HEAT );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/40+RandomNum(1,25), caster, HEAT );
	return true;
}
bool splGateTravel( CSocket *sock, CChar *caster, CItem *i )
{
	if( i->GetTempVar( CITV_MOREX ) <= 200 && i->GetTempVar( CITV_MOREY ) <= 200 )
	{
		sock->sysmessage( 679 );
		return false;
	}
	else
	{
		UI08 worldNum = static_cast<UI08>(i->GetTempVar( CITV_MORE ));
		if( !Map->MapExists( worldNum ) )
			worldNum = caster->WorldNumber();
		SpawnGate( caster, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ(), caster->WorldNumber(), static_cast<SI16>(i->GetTempVar( CITV_MOREX )), static_cast<SI16>(i->GetTempVar( CITV_MOREY )), static_cast<SI08>(i->GetTempVar( CITV_MOREZ )), worldNum );
		return true;
	}
	return true;
}
bool splManaVampire( CChar *caster, CChar *target, CChar *src )
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

void MassDispelStub( CChar *caster, CChar *target )
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
			cScript *tScript = NULL;
			UI16 scpNum = target->GetScriptTrigger();
			tScript = JSMapping->GetScript( scpNum );
			if( tScript != NULL )
				tScript->OnDispel( target );
			Effects->PlaySound( target, 0x0204 );
			Effects->PlayStaticAnimation( target, 0x372A, 0x09, 0x06 );
			HandleDeath( target );
		}
	}
}

bool splMassDispel( CSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	AreaAffectSpell( sock, caster, &MassDispelStub );
	return true;
}

void MeteorSwarmStub( CChar *caster, CChar *target )
{
	if( target->IsNpc() && target->GetNPCAiType() == AI_PLAYERVENDOR )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	if( target->IsNpc() ) 
		Combat->AttackTarget( target, caster );
	Effects->PlaySound( target, 0x160 ); 
	Effects->PlayMovingAnimation( caster, target, 0x36D5, 0x07, 0x00, 0x01 );
	if( Magic->CheckResist( caster, target, 7 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/80, caster, HEAT );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/40, caster, HEAT );
}

bool AreaAffectSpell( CSocket *sock, CChar *caster, void (*trgFunc)( MAGIC_AREA_STUB_LIST ) )
{
	SI16 x1, x2, y1, y2;
	SI08 z1, z2;
	x1 = x2 = y1 = y2 = z1 = z2 = 0;
	bool HurtSelf = false;
	
	Magic->BoxSpell( sock, caster, x1, x2, y1, y2, z1, z2 );

	REGIONLIST nearbyRegions = MapRegion->PopulateList( caster );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CChar * > *regChars = MapArea->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) )
				continue;

			if( tempChar->GetX() >= x1 && tempChar->GetX() <= x2 && tempChar->GetY() >= y1 && tempChar->GetY() <= y2 &&
				tempChar->GetZ() >= z1 && tempChar->GetZ() <= z2 && ( isOnline( (*tempChar) ) || tempChar->IsNpc() ) )
			{
				if( tempChar == caster )
				{//we can't cast on ourselves till this loop is over, because me might kill ourself, then try to LOS with someone after we are deleted.
					HurtSelf = true;
					continue;
				}

				if( LineOfSight( sock, caster, tempChar->GetX(), tempChar->GetY(), ( tempChar->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING  ) || caster->IsGM() )
					trgFunc( caster, tempChar );
				else if( sock != NULL )
					sock->sysmessage( 688 );
			}
		}
		regChars->Pop();
	}

	if( HurtSelf )	
		trgFunc( caster, caster );

	return true;
}
bool splMeteorSwarm( CSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	AreaAffectSpell( sock, caster, &MeteorSwarmStub );
	return true;
}
bool splPolymorph( CSocket *sock, CChar *caster )
{
	if( caster->IsPolymorphed() )
	{
		sock->sysmessage( 1637 );
		return false;
	}
	Magic->PolymorphMenu( sock, POLYMORPHMENUOFFSET ); // Antichrists Polymorph
	return true;
}
void EarthquakeStub( CChar *caster, CChar *target )
{
	int distx	= abs(target->GetX() - caster->GetX() );
	int disty	= abs(target->GetY() - caster->GetY() );
	int dmg		= (caster->GetSkill( MAGERY )/40) + ( RandomNum( 0, 19 ) - 10 );
	int dmgmod	= UOX_MIN( distx, disty );
	dmgmod		= -(dmgmod - 7);
	target->Damage( (dmg + dmgmod), caster, true );
	target->SetStamina( target->GetStamina() - ( RandomNum( 0, 9 ) + 5 ) );
	
	if( target->GetStamina() == -1 )
		target->SetStamina( 0 );
	
	if( !target->IsNpc() && isOnline( (*target) ) )
	{
		if( RandomNum( 0, 1 ) ) 
			Effects->PlayCharacterAnimation( target, 0x15 );
		else 
			Effects->PlayCharacterAnimation( target, 0x16 );
	} 
	else 
	{ 
		if( target->GetHP() > 0 ) 
		{
			Effects->PlayCharacterAnimation( target, 0x2 ); 
		}
	} 
}
bool splEarthquake( CSocket *sock, CChar *caster )
{
	criminal( caster );
	if( sock != NULL )
	{
		sock->SetWord( 11, caster->GetX() );
		sock->SetWord( 13, caster->GetY() );
		sock->SetByte( 16, caster->GetZ() );
	}
	AreaAffectSpell( sock, caster, &EarthquakeStub );
	return true;
}
bool splEnergyVortex( CSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 1, x, y, z );
	return true;
}
bool splResurrection( CChar *caster, CChar *target, CChar *src )
{
	if( target->IsDead() )
	{
		NpcResurrectTarget( target );
		return true;
	}
	return false;
}
bool splSummonAir( CSocket *sock, CChar *caster )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 2, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}
bool splSummonDaemon( CSocket *sock, CChar *caster )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 7, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}
bool splSummonEarth( CSocket *sock, CChar *caster )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 3, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}
bool splSummonFire( CSocket *sock, CChar *caster )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 4, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}
bool splSummonWater( CSocket *sock, CChar *caster )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 5, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}
bool splRandom( CSocket *sock, CChar *caster )
{
	if( caster->GetSkill( MAGERY ) <= 800 )
		return false;
	Magic->SummonMonster( sock, caster, 8, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}
bool splNecro1( CChar *caster, CChar *target, CChar *src )
{
	return true;
}
bool splNecro2( CSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 9, caster->GetX() +1, caster->GetY() +1, caster->GetZ() );
	return true;
}
bool splNecro3( CChar *caster, CChar *target, CChar *src )
{
	return true;
}
bool splNecro4( CChar *caster, CChar *target, CChar *src )
{
	return true;
}
bool splNecro5( CChar *caster, CChar *target, CChar *src )  
{
	return true;
}

bool DiamondSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	int j;
	SI16 fx[5], fy[5];
	SI16 yOffset = length;
	SI16 xOffset = length;

	fx[0] = -xOffset; fy[0] = 0;
	fx[1] = 0;		  fy[1] = yOffset;
	fx[2] = 0;		  fy[2] = -yOffset;
	fx[3] = xOffset;  fy[3] = 0;

	CItem *i			= NULL;
	UI08 worldNumber	= caster->WorldNumber();
	for( j = 0; j < 4; ++j )	// Draw the corners of our diamond
	{
		i = Items->CreateItem( NULL, caster, id, 1, 0, OT_ITEM );
		if( i != NULL )
		{
			i->SetDispellable( true );
			i->SetDecayable( true );
			i->SetDecayTime( BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 15 )) );
			i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
			i->SetTempVar( CITV_MOREY, caster->GetSerial() );
			i->SetLocation( x + fx[j], y + fy[j], Map->Height( x + fx[j], y + fy[j], z, worldNumber ) );
			i->SetDir( 29 );
			i->SetMovable( 2 );
		}
	}
	for( j = -1; j < 2; j = j + 2 )
	{
		for( SI08 counter2 = -1; counter2 < 2; counter2 += 2 )
		{
			for( int counter3 = 1; counter3 < yOffset; ++counter3 )
			{
				i = Items->CreateItem( NULL, caster, id, 1, 0, OT_ITEM );
				if( i != NULL )
				{
					i->SetDispellable( true );
					i->SetDecayable( true );
					i->SetDecayTime( BuildTimeValue(static_cast<R32>( caster->GetSkill( MAGERY ) / 15 )) );
					i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
					i->SetTempVar( CITV_MOREY, caster->GetSerial() );
					const SI16 newX = x + counter2 * counter3;
					const SI16 newY = y + j * ( yOffset - counter3 );
					i->SetLocation( newX, newY, Map->Height( newX, newY, z, worldNumber ) );
					i->SetDir( 29 );
					i->SetMovable( 2 );
				}
			}
		}
	}
	return true;
}

bool SquareSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	SI16 fx[5], fy[5];
	SI16 yOffset = length;
	SI16 xOffset = length;

	fx[0] = -xOffset;	fy[0] = xOffset;
	fx[1] = -yOffset;	fy[1] = yOffset;
	fx[2] = -xOffset;	fy[2] = xOffset;
	fx[3] = -yOffset;	fy[3] = yOffset;

	CItem *i = NULL;
	UI08 worldNumber = caster->WorldNumber();
	for( UI08 j = 0; j < 4; ++j )	// Draw the corners of our diamond
	{
		for( int counter = fx[j]; counter < fy[j]; ++counter )
		{
			i = Items->CreateItem( NULL, caster, id, 1, 0, OT_ITEM );
			if( i != NULL )
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
						i->SetLocation( x + counter, y + (j - 1) * yOffset, Map->Height( x + counter, y + (j-1) * yOffset, z, worldNumber ) );
						break;
					case 1:
					case 3:
						i->SetLocation( x + (j-2) * xOffset, y + counter, Map->Height( x + (j-2) * xOffset, y + counter, z, worldNumber ) );
						break;
				}
				i->SetDir( 29 );
				i->SetMovable( 2 );
			}
		}
	}
	return true;
}

bool FloodSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	SI16 yOffset = length;
	SI16 xOffset = length;

	SI16 maxX = x + xOffset;
	SI16 maxY = y + yOffset;
	SI16 minX = x - xOffset;
	SI16 minY = y - yOffset;

	UI08 worldNumber = caster->WorldNumber();
	for( int counter1 = minX; counter1 <= maxX; ++counter1 )
	{
		for( int counter2 = minY; counter2 <= maxY; ++counter2 )
		{
			CItem *i = Items->CreateItem( NULL, caster, id, 1, 0, OT_ITEM );
			if( i != NULL )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 15 )) );
				i->SetTempVar( CITV_MOREX, caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
				i->SetTempVar( CITV_MOREY, caster->GetSerial() );
				i->SetLocation( counter1, counter2, Map->Height( counter1, counter2, z, worldNumber ) );
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

//:Terrin: added constructor/destructor
cMagic::cMagic() : spellCount( 1 )
{
	spells.resize( 0 );
}

cMagic::~cMagic()
{
	// delete any allocations
	spells.clear();
}

bool cMagic::HasSpell( CItem *book, int spellNum )
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

void cMagic::AddSpell( CItem *book, int spellNum )
{
	cScript *tScript	= NULL;
	UI16 scpNum			= book->GetScriptTrigger();
	tScript				= JSMapping->GetScript( scpNum );
	if( tScript != NULL )
		tScript->OnSpellGain( book, spellNum );	

	if( !ValidateObject( book ) )
		return;
	UI32 wordNum = spellNum / 32;
	UI32 bitNum = (spellNum % 32);
	UI32 flagToSet = power( 2, bitNum );
	UI32 targAmount;
	if( wordNum < 3 )
	{
		targAmount = ( book->GetSpell( (UI08)wordNum ) | flagToSet );
		book->SetSpell( (UI08)wordNum, targAmount );
	}
}

void cMagic::RemoveSpell( CItem *book, int spellNum )
{
	cScript *tScript = NULL;
	UI16 scpNum = book->GetScriptTrigger();
	tScript = JSMapping->GetScript( scpNum );
	if( tScript != NULL )
	tScript->OnSpellLoss( book, spellNum );	

	if( !ValidateObject( book ) )
		return;
	UI32 wordNum	= spellNum / 32;
	UI32 bitNum		= (spellNum % 32);
	UI32 flagToSet	= power( 2, bitNum );
	UI32 flagMask	= 0xFFFFFFFF;
	UI32 targAmount;
	flagMask ^= flagToSet;
	if( wordNum < 3 )
	{
		targAmount = ( book->GetSpell( (UI08)wordNum ) & flagMask );
		book->SetSpell( (UI08)wordNum, targAmount );
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SpellBook( CSocket *mSock )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Opens the spellbook and displays all spells a
//|								 character has in his book.
//o---------------------------------------------------------------------------o
void cMagic::SpellBook( CSocket *mSock )
{
	UI08 i;
	UI08 spellsList[70];

	SERIAL serial		= ( mSock->GetDWord( 1 ) & 0x7FFFFFFF );
	CChar *mChar		= mSock->CurrcharObj();
	CItem *spellBook	= calcItemObjFromSer( serial );

	// Here's the kicker.  We NEVER needed to search through our pack for our spellbook!!! Why, you might ask???
	// Because we are able to calculate the serial of the spellbook, because it's targetted for us!!!!
	// Hence, we can remove all searches from here, and just use it directly
	// The only time we need to search is when they use the macro, and that's easily done (item == -1)
	// Abaddon

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
	mSock->Send( &sbStart );

	// Support for new Client Spellbook (giwo)
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

//o---------------------------------------------------------------------------o
//|     Class         :          GateCollision( CChar *s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Modified by AntiChrist - collision dir
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used when a PLAYER passes through a gate.  Takes the player
//|                              to the other side of the gate-link.
//o---------------------------------------------------------------------------o

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
		mChar->SetLocation( otherGate->GetX() + dirOffset, otherGate->GetY(), otherGate->GetZ() );
		if( !mChar->IsNpc() )
		{
			CDataList< CChar * > *myPets = mChar->GetPetList();
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

//o---------------------------------------------------------------------------o
//|     Class         :          SummonMonster( CSocket *s, CChar *caster, UI16 id, std::string monstername, UI16 color, SI16 x, SI16 y, SI08 z )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|		Revision	  :			 May 4th, 2000 - made function general words PC's and NPC's - Hanse
//o---------------------------------------------------------------------------o
//|     Purpose       :          Summon a monster (dispellable with DISPEL).
//o---------------------------------------------------------------------------o
void cMagic::SummonMonster( CSocket *s, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z )
{
	if( s != NULL )
		caster = s->CurrcharObj();
	else
	{	// Since caster will determine who is casting the spell... get a socket for players to see animations and hear effects - Hanse
		if( caster->IsNpc() && !caster->GetTarg()->IsNpc() )
			s = caster->GetTarg()->GetSocket();
	}

	CChar *newChar;

	switch( id )
	{
		case 0:	// summon monster
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateRandomNPC( "10000" );
			if( !ValidateObject( newChar ) )
			{
				s->sysmessage( 694 );
				return;
			}
			newChar->SetOwner( caster );
			newChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 5 )) );
			newChar->SetLocation( caster );
			Effects->PlayCharacterAnimation( newChar, 0x0C );
			newChar->SetFTarg( caster );
			newChar->SetNpcWander( WT_FOLLOW );
			s->sysmessage( 695 );
			return;
		case 1: // Energy Vortex
			Effects->PlaySound( s, 0x0216, true ); // EV
			newChar = Npcs->CreateNPCxyz( "energyvortex-summon", 0, 0, 0, caster->WorldNumber() );
			break;
		case 2: // Air Elemental
			Effects->PlaySound( s, 0x0217, true ); // AE
			newChar = Npcs->CreateNPCxyz( "airele-summon", 0, 0, 0, caster->WorldNumber() );
			break;
		case 3: //Earth Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "earthele-summon", 0, 0, 0, caster->WorldNumber() );
			break;
		case 4: //Fire Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "firele-summon", 0, 0, 0, caster->WorldNumber() );
			break;
		case 5: //Water Elemental
			Effects->PlaySound( s, 0x0217, true );
			newChar = Npcs->CreateNPCxyz( "waterele-summon", 0, 0, 0, caster->WorldNumber() );
			break;
		case 6: //Blade Spirits
			Effects->PlaySound( s, 0x0212, true ); // I don't know if this is the right effect...
			newChar = Npcs->CreateNPCxyz( "bladespirit-summon", 0, 0, 0, caster->WorldNumber() );
			break;
		case 7: // Daemon
			Effects->PlaySound( s, 0x0216, true );
			newChar = Npcs->CreateNPCxyz( "daemon-summon", 0, 0, 0, caster->WorldNumber() );
			break;
		case 8: // Dupre The Hero
			Effects->PlaySound( s, 0x0246, true );
			newChar = Npcs->CreateNPCxyz( "dupre-summon", 0, 0, 0, caster->WorldNumber() );
			break;
		case 9: // Black Night
			Effects->PlaySound( s, 0x0216, true );
			newChar = Npcs->CreateNPCxyz( "blacknight-summon", 0, 0, 0, caster->WorldNumber() );
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
		if( Map->ValidSpawnLocation( x, y, z, caster->WorldNumber() ) )
			newChar->SetLocation( x, y, z );
		else
			newChar->SetLocation( caster->GetX()-1, caster->GetY(), caster->GetZ() );
	}
	
	
	newChar->SetSpDelay( 10 );
	newChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( static_cast<R32>(caster->GetSkill( MAGERY ) / 5 )) );
	Effects->PlayCharacterAnimation( newChar, 0x0C );
	// AntiChrist (9/99) - added the chance to make the monster attack
	// the person you targeted ( if you targeted a char, naturally :) )
	CChar *i = NULL;
	if( !caster->IsNpc() && s != NULL )
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

//o---------------------------------------------------------------------------o
//|     Class         :          CheckBook( int circle, int spell, CItem *i )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if the spell is memorized into the spellbook.
//o---------------------------------------------------------------------------o

bool cMagic::CheckBook( int circle, int spell, CItem *i )
{
	bool raflag = false;
	int spellnum = spell + ( circle - 1 ) * 8;
	// Fix for OSI stupidity. :) 
	if( spellnum == 6 ) 
		raflag = true;
	if( spellnum >= 0 && spellnum < 6 ) 
		++spellnum;
	if( raflag ) 
		spellnum = 0;
	return HasSpell( i, spellnum );
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckMana( CChar *s, int circle )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//                               Modified by AntiChrist to use spells[] array.
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if character has enough mana to cast
//|                              a spell of that circle.
//o---------------------------------------------------------------------------o

bool cMagic::CheckMana( CChar *s, int num )
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
		if( p != NULL ) 
			p->sysmessage( 696 );
	}
	return false;
}

bool cMagic::CheckStamina( CChar *s, int num )
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
		if( p != NULL ) 
			p->sysmessage( 697 );
	}
	return false;
}

bool cMagic::CheckHealth( CChar *s, int num )
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
	if( p != NULL ) 
		p->sysmessage( 698 );
	return false;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SubtractMana( CChar *s, int mana )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Subtract the required mana from character's
//|                              mana reserve.
//o---------------------------------------------------------------------------o

void cMagic::SubtractMana( CChar *s, int mana)
{
	if( s->NoNeedMana() )
		return;
	s->SetMana( s->GetMana() - mana );
	if( s->GetMana() < 0 ) 
		s->SetMana( 0 );
}

void cMagic::SubtractStamina( CChar *s, int stamina )
{
	if( s->NoNeedMana() )
		return;
	s->SetStamina( s->GetStamina() - stamina );
	if( s->GetStamina() == -1 ) 
		s->SetStamina( 0 );
}

void cMagic::SubtractHealth( CChar *s, int health, int spellNum )
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

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckMagicReflect( CChar *i )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if character is protected by MagicReflect;
//|                              if yes, remove the protection and do visual effect.
//o---------------------------------------------------------------------------o

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

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckResist( CChar *attacker, CChar *defender, int circle )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Modified by AntiChrist to add EV.INT. check
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check character's magic resistance.
//o---------------------------------------------------------------------------o

bool cMagic::CheckResist( CChar *attacker, CChar *defender, int circle )
{
	bool i = Skills->CheckSkill( defender, MAGICRESISTANCE, 80*circle, 800+(80*circle) );
	CSocket *s = NULL;
	if( i )
	{
		if( ValidateObject( attacker ) )
		{
			if( ( defender->GetSkill( MAGICRESISTANCE ) - attacker->GetSkill( EVALUATINGINTEL ) ) >= 0 )
			{
				s = defender->GetSocket();
				if( s != NULL )
					s->sysmessage( 699 );
			}
			else
				i = false;
		}
		else
		{
			s = defender->GetSocket();
			if( s != NULL )
				s->sysmessage( 699 );
		}
	}
	return i;
}

//o---------------------------------------------------------------------------o
//|     Class         :          MagicDamage( CChar *p, int amount, CChar *attacker )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Calculate and inflict magic damage.
//o---------------------------------------------------------------------------o

void cMagic::MagicDamage( CChar *p, SI16 amount, CChar *attacker, WeatherType element )
{
	if( !ValidateObject( p ) || !ValidateObject( attacker ) )
		return;

	if( p->IsDead() || p->GetHP() <= 0 )	// extra condition check, to see if deathstuff hasn't been hit yet
		return;

	CSocket *mSock = p->GetSocket();
	if( Skills->CheckSkill( p, EVALUATINGINTEL, 0, 1000 ) ) 
		amount = (SI16)UOX_MAX( 1, amount - ( amount * ( p->GetSkill( EVALUATINGINTEL )/10000 ) ) ); // Take off 0 to 10% damage but still hurt at least 1hp (1000/10000=0.10)
	if( p->IsFrozen() && p->GetDexterity() > 0 )
	{
		p->SetFrozen( false );
		if( mSock != NULL ) 
			mSock->sysmessage( 700 );
	}           
	if( !p->IsInvulnerable() && p->GetRegion()->CanCastAggressive() )
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

//o---------------------------------------------------------------------------o
//|     Class         :          ::PoisonDamage( CChar *p )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Apply the poison to the character.
//o---------------------------------------------------------------------------o

void cMagic::PoisonDamage( CChar *p, int poison) // new functionality, lb !!!
{
	if( p->IsFrozen() )
	{
		p->SetFrozen( false );
		CSocket *s = p->GetSocket();
		if( s != NULL ) 
			s->sysmessage( 700 );
	}           
	if( !p->IsInvulnerable() && p->GetRegion()->CanCastAggressive() )
	{
		if( poison > 5 ) 
			poison = 5;
		if( poison < 0 ) 
			poison = 1;

		p->SetPoisoned( poison );
		p->SetTimer( tCHAR_POISONWEAROFF, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POISON ) );
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          CheckFieldEffects( CChar &mChar )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if character stands on a magic-field,
//|                              and apply effects.
//o------------------------------------------------------------------------
void cMagic::CheckFieldEffects( CChar& mChar )
{
	CMapRegion *toCheck = MapRegion->GetMapRegion( &mChar );
	if( toCheck == NULL )	// no valid region
		return;
	CDataList< CItem * > *regItems = toCheck->GetItemList();
	regItems->Push();
	for( CItem *inItemList = regItems->First(); !regItems->Finished(); inItemList = regItems->Next() )
	{
		if( !ValidateObject( inItemList ) )
			continue;
		if( inItemList->GetX() == mChar.GetX() && inItemList->GetY() == mChar.GetY() )
		{
			if( HandleFieldEffects( (&mChar), inItemList, inItemList->GetID() ) )
				break;
		}
	}
	regItems->Pop();
}

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
				if( !CheckResist( NULL, mChar, 4 ) )
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
			if( !CheckResist( NULL, mChar, 5 ) )
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
		if( mChar->IsInnocent() )
		{
			caster = calcCharObjFromSer( fieldItem->GetTempVar( CITV_MOREY ) );	// store caster in morey
			if( WillResultInCriminal( caster, mChar ) && ValidateObject( caster ) && !caster->IsGM() && !caster->IsCounselor() )
				criminal( caster );
		}
		if( RandomNum( 0, 2 ) == 1 && !CheckResist( NULL, mChar, 6 ) )
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
				if( !CheckResist( NULL, mChar, 4 ) )
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

//o---------------------------------------------------------------------------o
//|     Class         :          ::BoxSpell( CSocket * s, int& x1, int& x2, int& y1, int& y2, int& z1, int& z2 )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Calculate the spell box effect, depending 
//|                              on character's magery skill.
//o---------------------------------------------------------------------------o

void cMagic::BoxSpell( CSocket *s, CChar *caster, SI16& x1, SI16& x2, SI16& y1, SI16& y2, SI08& z1, SI08& z2)
{
	SI16 x, y;
	SI08 z;
	SI16 length;
	
	if( s != NULL )
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

	length = caster->GetSkill( MAGERY )/130; // increasde max-range, LB
	
	x1 = x - length;
	x2 = x + length;
	y1 = y - length;
	y2 = y + length;
	z1 = z;
	z2 = z+3;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::MagicTrap( CChar *s, int i )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Do the visual effect and apply magic damage
//|                              when a player opens a trapped container.
//o---------------------------------------------------------------------------o

void cMagic::MagicTrap( CChar *s, CItem *i )
{
	if( !ValidateObject( s ) || !ValidateObject( i ) )
		return;
    Effects->PlayStaticAnimation( s, 0x36B0, 0x09, 0x09 );
    Effects->PlaySound( s, 0x0207 );
	if( CheckResist( NULL, s, 4 ) ) 
		MagicDamage( s, i->GetTempVar( CITV_MORE, 2 ) / 2, NULL, LIGHTNING );
	else 
		MagicDamage( s, i->GetTempVar( CITV_MORE, 2 ) / 2, NULL, LIGHTNING );
	i->SetTempVar( CITV_MORE, 0 );
}

//o---------------------------------------------------------------------------o
//|     Class         :          CheckReagents( CChar *s, reag_st *reagents )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Modified by AntiChrist to use reag-st
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check for required reagents in player's backpack.
//o---------------------------------------------------------------------------o

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

//o---------------------------------------------------------------------------o
//|     Class         :          RegMsg( CChar *s, reag_st failmsg )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Reprogrammed by AntiChrist to display
//|                              missing reagents types.
//o---------------------------------------------------------------------------o
//|     Purpose       :          Display an error message if character has not enough regs.
//o---------------------------------------------------------------------------o

bool cMagic::RegMsg( CChar *s, reag_st failmsg )
{
	if( !ValidateObject( s ) )
		return true;
	bool display = false;
	char message[100] = { 0, };
	
	strcpy( message, Dictionary->GetEntry( 702 ).c_str() );
	if( failmsg.ash )
	{
		display = true; sprintf( message, "%sSa, ", message );
	}
	if( failmsg.drake )
	{
		display = true; sprintf( message, "%sMr, ", message );
	}
	if( failmsg.garlic )
	{
		display = true; sprintf( message, "%sGa, ", message );
	}
	if( failmsg.ginseng )
	{
		display = true; sprintf( message, "%sGi, ", message );
	}
	if( failmsg.moss )
	{
		display = true; sprintf( message, "%sBm, ", message );
	}
	if( failmsg.pearl )
	{
		display = true; sprintf( message, "%sBp, ", message );
	}
	if( failmsg.shade )
	{
		display = true; sprintf( message, "%sNs, ", message );
	}
	if( failmsg.silk )
	{
		display = true; sprintf( message, "%sSs, ", message );
	}
	
	message[strlen( message ) - 1] = ']';
	
	if( display )
	{
		CSocket *i = s->GetSocket();
		if( i != NULL )
			i->sysmessage( message );
		return false;
	}
	return true;
	
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SpellFail( CSocket *s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Do visual and sound effects when a player
//|                              fails to cast a spell.
//o---------------------------------------------------------------------------o

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

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckParry( CChar *player, int circle )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check player's parrying skill (for cannonblast).
//o---------------------------------------------------------------------------o

bool cMagic::CheckParry( CChar *player, int circle )
{
    if( Skills->CheckSkill( player, PARRYING, 80*circle, 800+( 80 * circle ) ) )
	{
		CSocket *s = player->GetSocket();
		if( s != NULL )
			s->sysmessage( 703 );
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////// PCs CASTING SPELLS RELATED FUNCTIONS ////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


//o---------------------------------------------------------------------------o
//|     Class         :          ::SelectSpell( CSocket *mSock, int num )
//|     Date          :          28 August 1999 / 10 September 1999
//|     Programmer    :          Abaddon / AntiChrist
//o---------------------------------------------------------------------------o
//|     Purpose       :          Execute the selected spell to cast.
//o---------------------------------------------------------------------------o

bool cMagic::SelectSpell( CSocket *mSock, int num )
// PRE:		s is a valid socket, num is a valid spell number
// POST:	Spell selected to cast
// Comments: Written by Abaddon (28 August 1999)
//:Terrin: Use spells (memory version of script info )
//Abaddon: Made memory optionable
//Abaddon: Perma cached
//DarkStorm: Added JS handling
{
	SI16 lowSkill = 0, highSkill = 0;
	CChar *mChar = mSock->CurrcharObj();

	cScript *jsScript	= JSMapping->GetScript( spells[num].JSScript() );

	if( jsScript != NULL )
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
			return true;
	}

	if( !mChar->GetCanAttack() )
	{
		mSock->sysmessage( 1778 );
		return false;
	}
	int type = mSock->CurrentSpellType();
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
	if( num > 63 && num <= static_cast<int>(spellCount) && spellCount <= 70 )
		Log( Dictionary->GetEntry( magic_table[num].spell_name ), mChar, NULL, "(Attempted)");
	if( mChar->IsJailed() && !mChar->IsGM() )
	{
		mSock->sysmessage( 704 );
		mChar->StopSpell();
		return false;
	}
	
	// (Abaddon) Region checks
	CTownRegion *ourReg = mChar->GetRegion();
	if( (num == 45 && !ourReg->CanMark()) || (num == 52 && !ourReg->CanGate()) || (num == 32 && !ourReg->CanRecall()) )
	{
		mSock->sysmessage( 705 );
		mChar->StopSpell();
		return false;
	}

	if( !mChar->GetRegion()->CanCastAggressive() && spells[mChar->GetSpellCast()].AggressiveSpell() ) // ripper 9-11-99 not in town :)
	{
		mSock->sysmessage( 706 );
		mChar->StopSpell();
		return false;
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
			if( itemLHand != NULL || ( itemRHand != NULL && itemRHand->GetType() != IT_SPELLBOOK ) )
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

	UI16 spellTrig = mChar->GetScriptTrigger();
	cScript *toExecute = JSMapping->GetScript( spellTrig );
	if( toExecute != NULL )
	{
		// Events:
		//  type 0: onSpellCast   (SpellBook)
		//  type 1: onScrollCast  (Scroll)
		//  type 2: onScrolllCast (Wand)

		if( type == 0 )
			Delay = toExecute->OnSpellCast( mChar, num );
		else
			Delay = toExecute->OnScrollCast( mChar, num );
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
		mChar->TextMessage( NULL, curSpellCasting.Mantra().c_str(), TALK, false );
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
	
	if( !mChar->IsOnHorse() )
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

	mChar->TextMessage( NULL, temp, TALK, false );
	mChar->SetCasting( true );
	return true;
	
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 getFieldDir( CChar *s, SI16 x, SI16 y )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get field direction
//o---------------------------------------------------------------------------o
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
					Console.Error( " Fallout of switch statement without default. uox3.cpp, getFieldDir()" );
					break;
			}
			break;
		default:
			Console.Error( " Fallout of switch statement without default. uox3.cpp, getFieldDir()" );
			break;
	}
	return fieldDir;
}


void cMagic::CastSpell( CSocket *s, CChar *caster )
{
	// for LocationTarget spell like ArchCure, ArchProtection etc...
	SI08 curSpell;
	SI16 x, y;
	SI08 z;

	curSpell			= caster->GetSpellCast();
	cScript *jsScript	= JSMapping->GetScript( spells[curSpell].JSScript() );
	if( jsScript != NULL )
	{
		if( jsScript->MagicSpellCast( s, caster, true, curSpell ) )
			return;
	}

	if( caster->IsCasting() )
		return;	// interrupted

	caster->StopSpell();

	UI16 spellTrig		= caster->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( spellTrig );
	if( toExecute != NULL )
	{
		toExecute->OnSpellSuccess( caster, curSpell );
	}

	CChar *src			= caster;
	bool validSocket	= ( s != NULL );

	if( curSpell > 63 && static_cast<UI32>(curSpell) <= spellCount && spellCount <= 70 )
		Log( Dictionary->GetEntry( magic_table[curSpell].spell_name ), caster, NULL, "(Succeeded)");
	if( caster->IsNpc() || validSocket && s->CurrentSpellType() != 2 )	// delete mana if NPC, s is -1 otherwise!
	{
		SubtractMana( caster, spells[curSpell].Mana() );
		if( spells[curSpell].Health() > 0 )
			SubtractHealth( caster, spells[curSpell].Health(), curSpell );
		SubtractStamina( caster, spells[curSpell].Stamina() );
	}
	if( validSocket && s->CurrentSpellType() == 0 && !caster->IsNpc() ) 
		DelReagents( caster, spells[curSpell].Reagants() );
	
	if( spells[curSpell].AggressiveSpell() && !caster->GetRegion()->CanCastAggressive() )
	{
		if( validSocket )
			s->sysmessage( 709 );
		return;
	}

	UI16 tScript = 0xFFFF;
	cScript *tScriptExec = NULL;
	if( spells[curSpell].RequireTarget() )					// target spells if true
	{
		CItem *i = NULL;
		if( validSocket && spells[curSpell].TravelSpell() )				// travel spells.... mark, recall and gate
		{
			// (Abaddon) Region checks
			CTownRegion *ourReg = caster->GetRegion();
			if( (curSpell == 45 && !ourReg->CanMark()) || (curSpell == 52 && !ourReg->CanGate()) || (curSpell == 32 && !ourReg->CanRecall()) )
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
				if( i->GetCont() != NULL || LineOfSight( s, caster, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
				{
					if( i->GetType() == IT_RECALLRUNE )
					{
						playSound( src, curSpell );
						doMoveEffect( curSpell, i, src );
						doStaticEffect( src, curSpell );
						switch( curSpell )
						{
							case 32: //////////// (32) RECALL ////////////////
							case 45: //////////// (45) MARK //////////////////
							case 52: //////////// (52) GATE //////////////////
								tScript = i->GetScriptTrigger();
								tScriptExec = JSMapping->GetScript( tScript );
								if( tScriptExec != NULL )
								{
									tScriptExec->OnSpellTarget( i, caster, curSpell );
								}
								(*((MAGIC_ITEMFUNC)magic_table[curSpell-1].mag_extra))( s, caster, i );
								break;
							default:
								Console.Error( " Unknown Travel spell %i, magic.cpp", curSpell );
								break;
						}
					}
					else if( validSocket )
						s->sysmessage( 710 );
				}
			}
			else if( validSocket )
				s->sysmessage( 710 );
			return;
		}
		
		if( spells[curSpell].RequireCharTarget() )
		{
			// TARGET CALC HERE
			CChar *c = NULL;
			if( !caster->IsNpc() )
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
				if( LineOfSight( s, caster, c->GetX(), c->GetY(), ( c->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
				{
					if( spells[curSpell].AggressiveSpell() )
					{
						if( !caster->GetRegion()->CanCastAggressive() )
						{
							if( validSocket )
								s->sysmessage( 709 );
							return;
						}
						if( c->GetNPCAiType() == AI_PLAYERVENDOR )
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
						case 4:    // Heal 2-26-00 changed by Homey, used OSI values from UO book
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
							tScript = c->GetScriptTrigger();
							tScriptExec = JSMapping->GetScript( tScript );
							if( tScriptExec != NULL )
							{
								tScriptExec->OnSpellTarget( c, caster, curSpell );
							}
							(*((MAGIC_CHARFUNC)magic_table[curSpell-1].mag_extra))( caster, c, src );
							break;
						case 66: // Cannon Firing
							if( CheckParry( c, 6 ) )
								MagicDamage( c, caster->GetSkill( TACTICS )/50, caster );
							else
								MagicDamage( c, caster->GetSkill( TACTICS )/25, caster );
							break; 
						case 46:	// Mass cure
						case 25:	// Arch Cure
						case 26:	// Arch Protection
						case 49:	// Chain Lightning
						case 54:	// Mass Dispel
						case 55:	// Meteor Swarm
							tScript = c->GetScriptTrigger();
							tScriptExec = JSMapping->GetScript( tScript );
							if( tScriptExec != NULL )
							{
								tScriptExec->OnSpellTarget( c, caster, curSpell );
							}
							(*((MAGIC_TESTFUNC)magic_table[curSpell-1].mag_extra))( s, caster, c, src );
							break;
						default:
							Console.Error( " Unknown CharacterTarget spell %i, magic.cpp", curSpell );
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
			CBaseObject *getTarg = NULL;
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
				if( !getTarg->CanBeObjType( OT_ITEM ) || ((CItem *)getTarg)->GetCont() == NULL )
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

			if( LineOfSight( s, caster, x, y, ( z + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
			{
				if( spells[curSpell].FieldSpell() )
				{
					UI08 j = getFieldDir( caster, x, y );
					playSound( src, curSpell );
					doStaticEffect( src, curSpell );
					(*((MAGIC_FIELDFUNC)magic_table[curSpell-1].mag_extra))( s, caster, j, x, y, z );
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
							(*((MAGIC_LOCFUNC)magic_table[curSpell-1].mag_extra))( s, caster, x, y, z );
							break;
						default:
							Console.Error( " Unknown LocationTarget spell %i", curSpell );
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
				if( ( i->GetCont() != NULL && FindItemOwner( i ) != caster ) || ( objInRange( caster, i, cwmWorldState->ServerData()->CombatMaxSpellRange() ) ) )
				{
					s->sysmessage( 718 );
					return;
				}
				if( LineOfSight( s, caster, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
				{	
					switch( curSpell )
					{
						case 13:	// Magic Trap
						case 14:	// Magic Untrap
						case 19:	// Magic Lock
						case 23:	// Magic Unlock
						case 21:	// Telekinesis
							(*((MAGIC_ITEMFUNC)magic_table[curSpell-1].mag_extra))( s, caster, i );
							break;
						default:
							Console.Error( " Unknown ItemTarget spell %i, magic.cpp", curSpell );
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
				(*((MAGIC_NOFUNC)magic_table[curSpell-1].mag_extra))( s, caster );
				break;
			default:	
				Console.Error( " Unknown NonTarget spell %i, magic.cpp", curSpell );
				break;
		}
		return;
	}
	
}


//:Terrin: Avoid multiple reading of the spell script every time a spell is
//         cast to avoid crippling the server when a mage enters combat
void cMagic::LoadScript( void )
{
	spells.resize( 0 );
	  
    // for some strange reason, spells go from index 1 to SPELL_MAX and 
    // apparently index 0 is left unused - fur 
	spells.resize( SPELL_MAX + 1 );

	UString spEntry;
	UString tag, data, UTag;
	UI08 i = 0;
	for( Script *spellScp = FileLookup->FirstScript( spells_def ); !FileLookup->FinishedScripts( spells_def ); spellScp = FileLookup->NextScript( spells_def ) )
	{
		if( spellScp == NULL )
			continue;

		for( ScriptSection *SpellLoad = spellScp->FirstEntry(); SpellLoad != NULL; SpellLoad = spellScp->NextEntry() )
		{
			if( SpellLoad == NULL )
				continue;

			spEntry = spellScp->EntryName();
			if( spEntry.section( " ", 0, 0 ) == "SPELL" )
			{
				i = spEntry.section( " ", 1, 1 ).toUByte();
				if( i <= SPELL_MAX )
				{
					++spellCount;
					spells[i].Enabled( false );
					reag_st *mRegs = spells[i].ReagantsPtr();

					Console.Log( "Spell number: %i", "spell.log", i );

					for( tag = SpellLoad->First(); !SpellLoad->AtEnd(); tag = SpellLoad->Next() )
					{
						UTag = tag.upper();
						data = SpellLoad->GrabData();
						Console.Log( "Tag: %s\tData: %s", "spell.log", UTag.c_str(), data.c_str() );
						switch( (UTag.data()[0]) )
						{
							case 'A':
								if( UTag == "ACTION" ) 
									spells[i].Action( data.toUShort() );
								else if( UTag == "ASH" ) 
									mRegs->ash = data.toUByte();
								break;
							case 'C':
								if( UTag == "CIRCLE" )
									spells[i].Circle( data.toUByte() );
								break;
							case 'D':
								if( UTag == "DELAY" )
									spells[i].Delay( data.toLong() );
								else if( UTag == "DRAKE" )
									mRegs->drake = data.toUByte();
								break;
							case 'E':
								if( UTag == "ENABLE" )   // presence of enable is enough to enable it
									spells[i].Enabled( data.toUShort() != 0 );
								break;
							case 'F':
								if( UTag == "FLAGS" )
								{
									if( data.sectionCount( " " ) != 0 )
										spells[i].Flags( (UI16)((data.section( " ", 0, 0 ).stripWhiteSpace().toUByte( 0, 16 )<<8) + data.section( " ", 1, 1 ).stripWhiteSpace().toUByte( 0, 16 )) );
									else
										spells[i].Flags( data.stripWhiteSpace().toUShort() );
								}
								break;
							case 'G':
								if( UTag == "GARLIC" )
									mRegs->garlic  = data.toUByte();
								else if( UTag == "GINSENG" )
									mRegs->ginseng = data.toUByte();
								break;
							case 'H':
								if( UTag == "HISKILL" )
									spells[i].HighSkill( data.toShort() );
								else if( UTag == "HEALTH" )
									spells[i].Health( data.toShort() );
								break;
							case 'L':
								if( UTag == "LOSKILL" )
									spells[i].LowSkill( data.toShort() );
								break;
							case 'M':
								if( UTag == "MANA" )
									spells[i].Mana( data.toShort() );
								else if( UTag == "MANTRA" )
									spells[i].Mantra( data );
								else if( UTag == "MOSS" )
									mRegs->moss = data.toUByte();
								else if( UTag == "MOVEFX" )
								{
									if( data.sectionCount( " " ) != 0 )
									{
										CMagicMove *mv = spells[i].MoveEffectPtr();
										mv->Effect( data.section( " ", 0, 0 ).stripWhiteSpace().toUByte( 0, 16 ), data.section( " ", 1, 1 ).stripWhiteSpace().toUByte( 0, 16 ) );
										mv->Speed( data.section( " ", 2, 2 ).stripWhiteSpace().toUByte( 0, 16 ) );
										mv->Loop( data.section( " ", 3, 3 ).stripWhiteSpace().toUByte( 0, 16 ) );
										mv->Explode( data.section( " ", 4, 4 ).stripWhiteSpace().toUByte( 0, 16 ) );
									}
								}
								break;
							case 'P':
								if( UTag == "PEARL" ) 
									mRegs->pearl = data.toUByte();
								break;
							case 'S':
								if( UTag == "SHADE" )
									mRegs->shade = data.toUByte();
								else if( UTag == "SILK" )
									mRegs->silk = data.toUByte();
								else if( UTag == "SOUNDFX" )
								{
									if( data.sectionCount( " " ) != 0 )
										spells[i].Effect( (UI16)((data.section( " ", 0, 0 ).stripWhiteSpace().toUByte( 0, 16 )<<8) + data.section( " ", 1, 1 ).stripWhiteSpace().toUByte( 0, 16 )) );
									else
										spells[i].Effect( data.stripWhiteSpace().toUShort() );
								}
								else if( UTag == "STATFX" )
								{
									if( data.sectionCount( " " ) != 0 )
									{
										CMagicStat *stat = spells[i].StaticEffectPtr();
										stat->Effect( data.section( " ", 0, 0 ).stripWhiteSpace().toUByte( 0, 16 ), data.section( " ", 1, 1 ).stripWhiteSpace().toUByte( 0, 16 ) );
										stat->Speed( data.section( " ", 2, 2 ).stripWhiteSpace().toUByte( 0, 16 ) );
										stat->Loop( data.section( " ", 3, 3 ).stripWhiteSpace().toUByte( 0, 16 ) );
									}
								}
								else if( UTag == "SCLO" )
									spells[i].ScrollLow( data.toShort() );
								else if( UTag == "SCHI" )
									spells[i].ScrollHigh( data.toShort() );
								else if( UTag == "STAMINA" )
									spells[i].Stamina( data.toShort() );
								break;
							case 'T':
								if( UTag == "TARG" )
									spells[i].StringToSay( data );
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
		if( ourScript != NULL )
			ourScript->ScriptRegistration( "Spell" );
	}
}


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

void cMagic::playSound( CChar *source, int num )
{
	if( spells[num].Effect() != INVALIDID )
		Effects->PlaySound( source, spells[num].Effect() );
}

void cMagic::doStaticEffect( CChar *source, int num )
{
	CMagicStat temp = spells[num].StaticEffect();
	if( temp.Effect() != INVALIDID )
		Effects->PlayStaticAnimation( source, temp.Effect(), temp.Speed(), temp.Loop() );
}

void cMagic::doMoveEffect( int num, CBaseObject *target, CChar *source )
{
	CMagicMove temp = spells[num].MoveEffect();
	
	if( temp.Effect() != INVALIDID )
		Effects->PlayMovingAnimation( source, target, temp.Effect(), temp.Speed(), temp.Loop(), ( temp.Explode() == 1 ) );
}

void HandleCommonGump( CSocket *mSock, ScriptSection *gumpScript, UI16 gumpIndex );
void cMagic::PolymorphMenu( CSocket *s, UI16 gmindex )
{
	UString sect				= "POLYMORPHMENU " + UString::number( gmindex );
	ScriptSection *polyStuff	= FileLookup->FindEntry( sect, menus_def );
	if( polyStuff == NULL )
		return;
	HandleCommonGump( s, polyStuff, gmindex );
}

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

void cMagic::Log( std::string spell, CChar *player1, CChar *player2, std::string extraInfo )
// PRE:		Commands is initialized, toLog is not NULL
// POST:	Writes out toLog to a file
{
	if( spell.empty() || !ValidateObject( player1 ) )
		return;
	std::string logName = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + "spell.log";
	std::ofstream logDestination;
	logDestination.open( logName.c_str(), std::ios::out | std::ios::app );
	if( !logDestination.is_open() )
	{
		Console.Error( "Unable to open spell log file %s!", logName.c_str() );
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

void cMagic::Register( cScript *toRegister, int spellNumber, bool isEnabled )
{
#if defined( UOX_DEBUG_MODE )
	Console.Print( "Registering spell number %i\n", spellNumber );
#endif
	if( spellNumber < 0 || static_cast<size_t>(spellNumber) >= spells.size() )
		return;
	if( toRegister == NULL )
		return;

	spells[spellNumber].JSScript( JSMapping->GetScriptID( toRegister->Object() ) );
	spells[spellNumber].Enabled( isEnabled );
}

void cMagic::SetSpellStatus( int spellNumber, bool isEnabled )
{
	if( spellNumber < 0 || static_cast<size_t>(spellNumber) >= spells.size() )
		return;
	spells[spellNumber].Enabled( isEnabled );
}

}
