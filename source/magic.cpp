#include "uox3.h"
#include "debug.h"
#include "magic.h" 
#include "power.h"
#include "ssection.h"

#undef DBGFILE
#define DBGFILE "magic.cpp"

MagicTable_s magic_table[] = {
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
//|	Function	-	SpawnGate( cSocket *sock, CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, SI16 trgX, SI16 trgY, SI08 trgZ )
//|	Programmer	-	Abaddon
//| Date		-	17th September, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	It will construct 2 linked gates, one at srcX / srcY / srcZ and another at trgX / trgY / trgZ
//o---------------------------------------------------------------------------o
void SpawnGate( cSocket *sock, CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, SI16 trgX, SI16 trgY, SI08 trgZ )
{
	CItem *g1 = Items->SpawnItem( sock, caster, 1, "#", false, 0x0F6C, 0, false, false );
	if( g1 != NULL )
	{
		g1->SetType( 51 );
		g1->SetLocation( srcX, srcY, srcZ );
		g1->SetGateTime( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( GATE ) ) );
		g1->SetDir( 1 );
		RefreshItem( g1 );
	}
	else
		return;

	CItem *g2 = Items->SpawnItem( sock, caster, 1, "#", false, 0x0F6C, 0, false, false );
	if( g2 != NULL )
	{
		g2->SetType( 52 );
		g2->SetLocation( trgX, trgY, trgZ );
		g2->SetGateTime( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( GATE ) ) );
		g2->SetDir( 1 );
		RefreshItem( g2 );

		g2->SetMoreX( g1->GetSerial() );
		g1->SetMoreX( g2->GetSerial() );
	}
	else
		return;
}

bool FieldSpell( cSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 fieldDir )
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
	for( int j = 0; j < 5; j++ )		// how about we make this 5, huh?  missing part of the field
	{
		i = Items->SpawnItem( sock, 1, "#", false, id, 0, false, false );
		if( i != NULL )
		{
			i->SetDispellable( true );
			i->SetDecayable( true );
			i->SetDecayTime( BuildTimeValue( caster->GetSkill( MAGERY ) / 15 ) );
			i->SetMoreX( caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
			i->SetMoreY( caster->GetSerial() );
			i->SetLocation( fx[j], fy[j], Map->Height( fx[j], fy[j], z, worldNumber ) );
			i->SetDir( 29 );
			i->SetMagic( 2 );
			RefreshItem( i );
		}
	}
	return true;
}

bool splClumsy( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 3, caster->GetSkill( MAGERY )/100, 0, 0);
	return true;
}
bool splCreateFood( cSocket *sock, CChar *caster )
{
	CItem *j = Items->SpawnItem( sock, 1, "#", true, 0x09D3, 0x0000, true, true );
	if( j != NULL )
	{
		j->SetType( 14 );
		RefreshItem( j );
	}
	return true;
}
bool splFeeblemind( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 4, caster->GetSkill( MAGERY )/100, 0, 0);
	return true;
}
bool splHeal( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	int bonus = (caster->GetSkill( MAGERY )/500) + ( caster->GetSkill( MAGERY )/100 );
	if( bonus != 0 )
		target->SetHP( ( target->GetHP() + RandomNum( 0, 5 ) + bonus ) );
	else
		target->SetHP( target->GetHP() + 4 );
	updateStats( target, 0 );
	Magic->SubtractHealth( caster, bonus, 4 );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splMagicArrow( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	Magic->MagicDamage( target, (1+(rand()%1)+1)*(caster->GetSkill( MAGERY )/2000+1), caster );
	return true;
}
bool splNightSight( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 2, 0, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splReactiveArmor( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 15, caster->GetSkill( MAGERY )/100, 0, 0 );
	target->SetReactiveArmour( true );
	return true;
}
bool splWeaken( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 5, caster->GetSkill( MAGERY )/100, 0, 0);
	return true;
}
bool splAgility( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 6, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splCunning( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 7, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splCure( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	target->SetPoisoned( 0 );
	target->SetPoisonWearOffTime( uiCurrentTime );
	target->SendToSocket( sock, true, target );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splHarm( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 2 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/500+1, caster );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/250+RandomNum( 1, 2 ), caster );
	return true;
}
bool splMagicTrap( cSocket *sock, CChar *caster, CItem *target )
{
	if( target->IsContType() && target->GetID() != 0x0E75 )  
	{
		target->SetMoreB( 1, caster->GetSkill( MAGERY )/20, caster->GetSkill( MAGERY )/10, target->GetMoreB( 4 ) );
		soundeffect( target, 0x01F0 );
	} 
	else 
		sysmessage( sock, 663 );
	return true;
}
bool splMagicUntrap( cSocket *sock, CChar *caster, CItem *target )
{
	if( target->IsContType() )
	{
		if( target->GetMoreB( 1 ) == 1 )
		{
			if( rand()%100 <= 50 + ( caster->GetSkill( MAGERY )/10) - target->GetMoreB( 3 ) )
			{
				target->SetMoreB( 0, 0, 0, target->GetMoreB( 4 ) );
				soundeffect(target, 0x01F1 );
				sysmessage( sock, 664 );
			}
			else 
				sysmessage( sock, 665 );
		}
		else 
			sysmessage( sock, 667 );
	}
	else 
		sysmessage( sock, 668 );
	return true;
}
bool splProtection( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 21, caster->GetSkill( MAGERY )/10, 0, 0 );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splStrength( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	tempeffect( src, target, 8, caster->GetSkill( MAGERY )/100, 0, 0);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splBless( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	int j = caster->GetSkill( MAGERY )/100;
	tempeffect( src, target, 11, j, j, j);
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splFireball( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 3 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 280 + 1, caster );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 140 + RandomNum( 1, 4 ), caster );
	return true;
}
bool splMagicLock( cSocket *sock, CChar *caster, CItem *target )
{
	UI08 type = target->GetType();
	if( ( type == 1 || type == 12 || type == 63 ) && ( target->GetID( 1 ) != 0x0E || target->GetID( 2 ) != 0x75 ) )
	{
		switch( target->GetType() )
		{
		case 1:  target->SetType( 8 ); break;
		case 12: target->SetType( 13 ); break;
		case 63: target->SetType( 64 ); break;
		default:
			Console.Error( 2, "Fallout of switch statement without default. magic.cpp, magiclocktarget()" );
			break;
		}
		soundeffect( target, 0x0200 );
	}
	else 
		sysmessage( sock, 669 ); 
	return true;
}
bool splPoison( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 1 ) )
		return false;
		
	target->SetPoisoned( 2 );   
	target->SetPoisonWearOffTime( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) ) );
	target->SendToSocket( sock, true, target );

	return true;
}
bool splTelekinesis( cSocket *sock, CChar *caster, CItem *target )
{
	return true;
}
bool splTeleport( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	CMultiObj *m = findMulti( x, y, z, caster->WorldNumber() );
	if( m != NULL && m->GetOwner() != caster->GetSerial() && !caster->IsNpc() )
	{
		sysmessage( sock, 670 );
		return false;
	}
	CTile tile;
	if( !caster->IsNpc() )
	{
		Map->SeekTile( sock->GetWord( 0x11 ), &tile );
		if( (!strcmp( tile.Name(), "water")) || tile.LiquidWet() )
		{
			sysmessage( sock, 671 );
			return false;
		}						
		if( tile.WallRoofWeap() )	// slanted roof tile!!! naughty naughty
		{
			sysmessage( sock, 672 );
			return false;
		}
	}
	caster->SetLocation( x, y, z );
	caster->Teleport();
	Magic->doStaticEffect( caster, 22 );
	return true;
}
bool splUnlock( cSocket *sock, CChar *caster, CItem *target )
{
	if( target->enhanced&0x01 )
	{
		sysmessage( sock, 673 );
		return false;
	}
	if( target->GetType() == 8 )
	{
		target->SetType( 1 );
		sysmessage( sock, 674 );
	} 
	else if( target->GetType() == 64 )
	{
		target->SetType( 63 );
		sysmessage( sock, 675 );
	} 
	else if( target->GetType() == 1 || target->GetType() == 63 || target->GetType() == 65 || target->GetType() == 87 )
		sysmessage( sock, 676 );
	else if( target->GetType() == 12 )
		sysmessage( sock, 677 );
	else 
		sysmessage( sock, 678 );
	return true;
}
bool splWallOfStone( cSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	if( caster->GetSkill( MAGERY ) >= 800 )
		return FloodSpell( sock, caster, 0x0080, x, y, z, 4 );
	else if( caster->GetSkill( MAGERY ) >= 600 )
		return DiamondSpell( sock, caster, 0x0080, x, y, z, 4 );
	else
		return FieldSpell( sock, caster, 0x0080, x, y, z, fieldDir );
}

void ArchCureStub( CChar *caster, CChar *target )
{
	if( target->GetPoisoned() )
	{
		staticeffect( target, 0x376A, 0x09, 0x06 );
		soundeffect( target, 0x01E9 );
		target->SetPoisoned( 0 );
	}
}
bool splArchCure( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	AreaAffectSpell( sock, caster, &ArchCureStub );
	return true;
}

void ArchProtectionStub( CChar *caster, CChar *target )
{
	Magic->playSound( target, 26 );
	Magic->doStaticEffect( target, 15 );	// protection
	tempeffect( caster, target, 21, caster->GetSkill( MAGERY )/10, 0, 0 );
}

bool splArchProtection( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	AreaAffectSpell( sock, caster, &ArchProtectionStub );
	return true;
}
bool splCurse( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( !Magic->CheckResist( caster, target, 1 ) )
	{
		int j = caster->GetSkill( MAGERY ) / 100;
		tempeffect(caster, target, 12, j, j, j);
	}
	return true;
}
bool splFireField( cSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	if( caster->GetSkill( MAGERY ) >= 600 )
		return DiamondSpell( sock, caster, 0x3996, x, y, z, 4 );
	else
	{
		if( fieldDir )
			return FieldSpell( sock, caster, 0x3996, x, y, z, fieldDir );
		else 
			return FieldSpell( sock, caster, 0x398C, x, y, z, fieldDir );
	}	
}
bool splGreaterHeal( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	int j = target->GetHP() + (caster->GetSkill( MAGERY )/30+RandomNum(1,12));
	target->SetHP( min(target->GetMaxHP(), (SI16)j) );
	updateStats( target, 0 );
	Magic->SubtractHealth( caster, min( target->GetStrength(), (SI16)j ), 29 );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}
bool splLightning( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	bolteffect( target );
	if( Magic->CheckResist( caster, target, 4 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 180 + RandomNum( 1, 2 ), caster );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY ) / 90 + RandomNum( 1, 5 ), caster );
	return true;
}
bool splManaDrain( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( !Magic->CheckResist( caster, target, 4 ) )
	{
		target->SetMana( target->GetMana() - caster->GetSkill( MAGERY )/35 );
		if( target->GetMana() < 0 ) 
			target->SetMana( 0 );
		updateStats( target, 1 );
	}
	return true;
}
bool splRecall( cSocket *sock, CChar *caster, CItem *i )
{
	if( i->GetMoreX() <= 200 && i->GetMoreY() <= 200 )
	{
		sysmessage( sock, 679 );
		return false;
	} 
	else if( caster->GetCommandLevel() < CNSCMDLEVEL && !Weight->checkWeight( caster, true ) ) // no recall if too heavy, GM's excempt
	{
		sysmessage( sock, 680 );
		sysmessage( sock, 681 );
		statwindow( sock, caster );
		return false;
	}
	else
	{
		caster->SetLocation( i->GetMoreX(), i->GetMoreY(), i->GetMoreZ() );
		caster->Teleport();
		sysmessage( sock, 682 );
		return true;
	}
	return false;
}
bool splBladeSpirits( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	Magic->SummonMonster( sock, caster, 0x023E, "a blade spirit", 0x00, x, y, z );
	return true;
}
bool splDispelField( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	CItem *i = calcItemObjFromSer( sock->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( LineOfSight( sock, caster, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
		{
			if( i->isDecayable() || i->isDispellable() ) 
			{
				UI16 scpNum = i->GetScriptTrigger();
				cScript *tScript = Trigger->GetScript( scpNum );
				if( tScript != NULL )
					tScript->OnDispel( i );
				Items->DeleItem( i );
			}
			soundeffect( caster, 0x0201 );
		}
		else
			sysmessage( sock, 683 );
	}
	return true;
}
bool splIncognito( cSocket *sock, CChar *caster )
{
	if( caster->IsIncognito() )
	{
		sysmessage( sock, 1636 );
		return false;
	}
	// ------ SEX ------
	caster->SetOrgID( caster->GetID() );
	if( ( rand()%2 ) == 0 ) 
		caster->SetID( 0x0190 ); 
	else 
		caster->SetID( 0x0191 );
	
	// ------ NAME -----
	caster->SetOrgName( caster->GetName() );
	
	if( caster->GetID() == 0x0190 )
		setRandomName( caster, "1" );//get a name from male list
	else 
		setRandomName( caster, "2" );//get a name from female list

	int color = RandomNum( 0x044E, 0x047D );
	CItem *j = FindItemOnLayer( caster, 0x0B );
	if( j != NULL ) 
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
	if( caster->GetID() == 0x0190 )
	{
		j = FindItemOnLayer( caster, 0x10 );
		if( j != NULL ) 
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
	wornItems( sock, caster );
	caster->Teleport();
	soundeffect( caster, 0x0203 );
	tempeffect( caster, caster, 19, 0, 0, 0 );
	caster->IsIncognito( true );
	return true;
}
bool splMagicReflection( cSocket *sock, CChar *caster )
{
	caster->SetPermReflected( true );
	return true;
}
bool splMindBlast( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( caster->GetIntelligence() > target->GetIntelligence() )
	{
		if( Magic->CheckResist( caster, target, 5 ) )
			Magic->MagicDamage( target, (src->GetIntelligence() - target->GetIntelligence())/4, src );
		else
			Magic->MagicDamage( target, (src->GetIntelligence() - target->GetIntelligence())/2, src );
	}
	else
	{
		if( Magic->CheckResist( caster, src, 5 ) )
			Magic->MagicDamage( src, (target->GetIntelligence() - src->GetIntelligence())/4, src );
		else
			Magic->MagicDamage( src, (target->GetIntelligence() - src->GetIntelligence())/2, src );
	}
	return true;
}
bool splParalyze( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( !Magic->CheckResist( caster, target, 7 ) )
		tempeffect( caster, target, 1, 0, 0, 0 );
	return true;
}
bool splPoisonField( cSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	if( caster->GetSkill( MAGERY ) >= 800 )
		return FloodSpell( sock, caster, 0x3920, x, y, z, 4 );
	else if( caster->GetSkill( MAGERY ) >= 600 )
		return DiamondSpell( sock, caster, 0x3920, x, y, z, 4 );
	else
	{
		if( fieldDir )
			return FieldSpell( sock, caster, 0x3920, x, y, z, fieldDir );
		else 
			return FieldSpell( sock, caster, 0x3915, x, y, z, fieldDir );
	}
}
bool splSummonCreature( cSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 0, "#", 0, caster->GetX()+1, caster->GetY()+1, caster->GetZ() );
	return true;
}
bool splDispel( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( target->IsDispellable() )
	{
		staticeffect( target, 0x372A, 0x09, 0x06 );		// why the specifics here?
		if( target->IsNpc() ) 
		{
			cScript *tScript = NULL;
			UI16 scpNum = target->GetScriptTrigger();
			tScript = Trigger->GetScript( scpNum );
			if( tScript != NULL )
				tScript->OnDispel( target );
			Npcs->DeleteChar( target );
		}
		else 
			doDeathStuff(target);
	}
	return true;
}
bool splEnergyBolt( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 6 ) ) 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/120, caster );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/35+RandomNum(1,10), caster );
	return true;
}
bool splExplosion( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 6 ) )
		tempeffect( src, target, 27, (caster->GetSkill( MAGERY ) / 120 + RandomNum( 1, 5 ) ), 0, 0 );
	else 
		tempeffect( src, target, 27, (caster->GetSkill( MAGERY ) / 40 + RandomNum( 1, 10 ) ), 0, 0 );
	return true;
}
bool splInvisibility( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	target->SetHidden( 2 );
	target->Update();
	target->SetInvisTimeout( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( INVISIBILITY ) ) );
	if( target->IsMurderer() )
		criminal( caster );
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	bool splMark( cSocket *sock, CChar *caster, CItem *i )
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
bool splMark( cSocket *sock, CChar *caster, CItem *i )
{
	// Sept 22, 2002 - Xuri
	if ( i->GetMagic() == 3)
	{
		sysmessage( sock, 774 );
		return false;
	}
	//	
	i->SetMoreX( caster->GetX() ); 
	i->SetMoreY( caster->GetY() );
	i->SetMoreZ( caster->GetZ() );

	char tempitemname[512];

	if( region[caster->GetRegion()]->GetName()[0] != 0 )
		sprintf( tempitemname, Dictionary->GetEntry( 684 ), region[caster->GetRegion()]->GetName() );
	else 
		strcpy( tempitemname, Dictionary->GetEntry( 685 ) );
	i->SetName( tempitemname );
	sysmessage( sock, 686 ); 
	return true;
}

void MassCurseStub( CChar *caster, CChar *target )
{
	if( target->IsNpc() && target->GetNPCAiType() == 17 )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	int j;
	if( target->IsNpc() ) 
		npcAttackTarget( target, caster );
	staticeffect( target, 0x374A, 0, 15 );
	soundeffect( target, 0x01FC );
	if( Magic->CheckResist( caster, target, 6 ) )
		j = caster->GetSkill( MAGERY )/200;
	else 
		j = caster->GetSkill( MAGERY )/75;
	tempeffect( caster, target, 12, j, j, j );

}

bool splMassCurse( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	AreaAffectSpell( sock, caster, &MassCurseStub );
	return true;
}
bool splParalyzeField( cSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	if( caster->GetSkill( MAGERY ) >= 600 )
		return DiamondSpell( sock, caster, 0x3979, x, y, z, 4 );
	else
	{
		if( fieldDir )
			return FieldSpell( sock, caster, 0x3979, x, y, z, fieldDir );
		else 
			return FieldSpell( sock, caster, 0x3967, x, y, z, fieldDir );
	}
}
bool splReveal( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	if( LineOfSight( sock, caster, x, y, z, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
	{ 
		int j = caster->GetSkill( MAGERY ); 
		int range=(((j-261)*(15))/739)+5;
		//If the caster has a Magery of 26.1 (min to cast reveal w/ scroll), range  radius is
		//5 tiles, if magery is maxed out at 100.0 (except for gms I suppose), range is 20
		
		int xOffset = MapRegion->GetGridX( caster->GetX() );
		int yOffset = MapRegion->GetGridY( caster->GetY() );
		UI08 worldNumber = caster->WorldNumber();
		for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
		{
			for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
			{
				SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
				if( MapArea == NULL )	// no valid region
					continue;
				MapArea->PushChar();
				for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
				{
					if( tempChar == NULL )
						continue;
					if( tempChar->GetHidden() != 0 )
					{
						point3 difference = ( tempChar->GetLocation() - point3( x, y, z ) );
						if( difference.MagSquared() <= ( range * range ) ) // char to reveal is within radius or range 
						{
							if( tempChar->GetHidden() && !tempChar->IsPermHidden() )
								tempChar->ExposeToView();
						}
					}
				}
				MapArea->PopChar();
			}
		}
		soundeffect( sock, 0x01FD, true );
	}
	else
		sysmessage( sock, 687 );
	return true;
}

void ChainLightningStub( CChar *caster, CChar *target )
{
	if( target->IsNpc() && target->GetNPCAiType() == 17 )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	if( target->IsNpc() )
		npcAttackTarget( caster, target );
	soundeffect( caster, 0x0029 );
	CChar *def = NULL;
	CChar *att = NULL;
	if( Magic->CheckMagicReflect( target ) )
		def = caster;
	else
		def = target;

	bolteffect( def );
	if( Magic->CheckResist( caster, def, 7 ) )
		Magic->MagicDamage( def, caster->GetSkill( MAGERY )/70, att );
	else 
		Magic->MagicDamage( def, caster->GetSkill( MAGERY )/50, att );
}

bool splChainLightning( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	AreaAffectSpell( sock, caster, &ChainLightningStub );
	return true;
}
bool splEnergyField( cSocket *sock, CChar *caster, UI08 fieldDir, SI16 x, SI16 y, SI08 z )
{
	if( caster->GetSkill( MAGERY ) >= 600 )
		return DiamondSpell( sock, caster, 0x3956, x, y, z, 4 );
	else
	{
		if( fieldDir )
			return FieldSpell( sock, caster, 0x3956, x, y, z, fieldDir );
		else 
			return FieldSpell( sock, caster, 0x3946, x, y, z, fieldDir );
	}
}
bool splFlameStrike( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( Magic->CheckResist( caster, target, 7 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/80, caster );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/40+RandomNum(1,25), caster );
	return true;
}
bool splGateTravel( cSocket *sock, CChar *caster, CItem *i )
{
	if( i->GetMoreX() <= 200 && i->GetMoreY() <= 200 )
	{
		sysmessage( sock, 679 );
		return false;
	}
	else
	{
		SpawnGate( sock, caster, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ(), i->GetMoreX(), i->GetMoreY(), i->GetMoreZ() );
		return true;
	}
	return true;
}
bool splManaVampire( cSocket *sock, CChar *caster, CChar *target, CChar *src )
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
		updateStats( target, 1 );
		updateStats( caster, 1 );
	}
	return true;
}

void MassDispelStub( CChar *caster, CChar *target )
{
	if( target->IsDispellable() )
	{
		if( Magic->CheckResist( caster, target, 7 ) && rand()%2 == 0 )  // cant be 100% resisted , osi
		{
			if( target->IsNpc() ) 
				npcAttackTarget( target, caster );
		}
		if( target->IsNpc() ) 
		{
			cScript *tScript = NULL;
			UI16 scpNum = target->GetScriptTrigger();
			tScript = Trigger->GetScript( scpNum );
			if( tScript != NULL )
				tScript->OnDispel( target );
			doDeathStuff(target);
		}
		soundeffect(target, 0x0204);
		staticeffect(target, 0x372A, 0x09, 0x06);
	}
}

bool splMassDispel( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	AreaAffectSpell( sock, caster, &MassDispelStub );
	return true;
}

void MeteorSwarmStub( CChar *caster, CChar *target )
{
	if( target->IsNpc() && target->GetNPCAiType() == 17 )
		return;	// Player Vendors can't be killed
	if( target->IsGM() || target->IsInvulnerable() )
		return;	// GMs/Invuls can't be killed
	if( target->IsNpc() ) 
		npcAttackTarget( caster, target );
	soundeffect( target, 0x160 ); 
	movingeffect( caster, target, 0x36D5, 0x07, 0x00, 0x01 );
	if( Magic->CheckResist( caster, target, 7 ) )
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/80, caster );
	else 
		Magic->MagicDamage( target, caster->GetSkill( MAGERY )/40, caster );
}

bool AreaAffectSpell( cSocket *sock, CChar *caster, void (*trgFunc)( MAGIC_AREA_STUB_LIST ) )
{
	SI16 x1, x2, y1, y2;
	SI08 z1, z2;
	x1 = x2 = y1 = y2 = z1 = z2 = 0;
	bool HurtSelf = false;
	
	Magic->BoxSpell( sock, caster, x1, x2, y1, y2, z1, z2 );

	int xOffset = MapRegion->GetGridX( caster->GetX() );
	int yOffset = MapRegion->GetGridY( caster->GetY() );
	UI08 worldNumber = caster->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;

				if( tempChar->GetX() >= x1 && tempChar->GetX() <= x2 && tempChar->GetY() >= y1 && tempChar->GetY() <= y2 &&
					tempChar->GetZ() >= z1 && tempChar->GetZ() <= z2 && ( isOnline( tempChar ) || tempChar->IsNpc() ) )
				{
					if( tempChar == caster )
					{//we can't cast on ourselves till this loop is over, because me might kill ourself, then try to LOS with someone after we are deleted.
						HurtSelf = true;
						continue;
					}

					if( LineOfSight( sock, caster, tempChar->GetX(), tempChar->GetY(), tempChar->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING  ) || caster->IsGM() )
						trgFunc( caster, tempChar );
					else
						sysmessage( sock, 688 );
				}
			}
			MapArea->PopChar();
		}
	}

	if( HurtSelf )	
		trgFunc( caster, caster );

	return true;
}
bool splMeteorSwarm( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	AreaAffectSpell( sock, caster, &MeteorSwarmStub );
	return true;
}
bool splPolymorph( cSocket *sock, CChar *caster )
{
	if( caster->IsPolymorphed() )
	{
		sysmessage( sock, 1637 );
		return false;
	}
	Magic->PolymorphMenu( sock, POLYMORPHMENUOFFSET ); // Antichrists Polymorph
	return true;
}
void EarthquakeStub( CChar *caster, CChar *target )
{
	int distx = abs(target->GetX() - caster->GetX() );
	int disty = abs(target->GetY() - caster->GetY() );
	int dmg = (caster->GetSkill( MAGERY )/40)+(rand()%20-10);
	int dmgmod = min( distx, disty );
	dmgmod = -(dmgmod - 7);
	target->IncHP( - (dmg+dmgmod) );
	target->SetStamina( target->GetStamina() - (rand()%10+5) );
	
	if( target->GetStamina() < 0 )
		target->SetStamina( 0 );
	if( target->GetHP() < 0 )
		target->SetHP( 0 );
	
	char temp[1024];
	if( !target->IsNpc() && isOnline( target ) )
	{
		if( RandomNum( 0, 1 ) ) 
			npcAction( target, 0x15 );
		else 
			npcAction( target, 0x16 );
		if( target->GetHP() <= 0 ) 
		{
			if( target->IsInnocent() && GuildSys->ResultInCriminal( caster, target ) && Races->Compare( caster, target ) == 0 )
			{
				caster->SetKills( caster->GetKills() + 1 );
				sysmessage( calcSocketObjFromChar( caster ), 689, caster->GetName(), caster->GetKills() );
				if( caster->GetKills() == cwmWorldState->ServerData()->GetRepMaxKills() + 1 )
					sysmessage( calcSocketObjFromChar( caster ), 690 );
			}
			
			sprintf( temp, Dictionary->GetEntry( 691 ) ,target->GetName(), caster->GetName() );
			savelog( temp, "PvP.log" );
			Karma( caster, target, ( 0 - ( target->GetKarma() ) ) );
			Fame( caster, target->GetFame() );
			doDeathStuff( target );                              
		}
	} 
	else 
	{ 
		if( target->GetHP() <= 0 ) 
		{
			Karma( caster, target, ( 0 - ( target->GetKarma() ) ) );
			Fame( caster, target->GetFame() );
			doDeathStuff(target); 
		}
		else if( target->IsNpc() ) 
		{ 
				npcAction( target, 0x2 ); 
				npcAttackTarget(caster,target); 
		}
	} 
}
bool splEarthquake( cSocket *sock, CChar *caster )
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
bool splEnergyVortex( cSocket *sock, CChar *caster, SI16 x, SI16 y, SI08 z )
{
	Magic->SummonMonster( sock, caster, 0x000D, "an energy vortex", 0x0075, x, y, z );
	return true;
}
bool splResurrection( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	if( target->IsDead() )
	{
		Targ->NpcResurrectTarget( target );
		return true;
	}
	return false;
}
bool splSummonAir( cSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 0x000D, "Air", 0, caster->GetX()+1, caster->GetY()+1, caster->GetZ() );
	return true;
}
bool splSummonDaemon( cSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 0x000A, "Bob", 0, caster->GetX()+1, caster->GetY()+1, caster->GetZ() );
	return true;
}
bool splSummonEarth( cSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 0x000E, "Earth", 0, caster->GetX()+1, caster->GetY()+1, caster->GetZ() );
	return true;
}
bool splSummonFire( cSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 0x000F, "Fire", 0, caster->GetX()+1, caster->GetY()+1, caster->GetZ() );
	return true;
}
bool splSummonWater( cSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 0x0010, "Water", 0, caster->GetX() + 1, caster->GetY() + 1, caster->GetZ() );
	return true;
}
bool splRandom( cSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 0x03E2, "Dupre the Hero", 0, caster->GetX()+1, caster->GetY()+1, caster->GetZ() );
	return true;
}
bool splNecro1( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	return true;
}
bool splNecro2( cSocket *sock, CChar *caster )
{
	Magic->SummonMonster( sock, caster, 0x000A, "Black Night", 5000, caster->GetX() +1, caster->GetY() +1, caster->GetZ() );
	return true;
}
bool splNecro3( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	return true;
}
bool splNecro4( cSocket *sock, CChar *caster, CChar *target, CChar *src )
{
	return true;
}
bool splNecro5( cSocket *sock, CChar *caster, CChar *target, CChar *src )  
{
	return true;
}

bool DiamondSpell( cSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	int j;
	SI16 fx[5], fy[5];
	SI16 yOffset = length;
	SI16 xOffset = length;

	fx[0] = -xOffset; fy[0] = 0;
	fx[1] = 0;		  fy[1] = yOffset;
	fx[2] = 0;		  fy[2] = -yOffset;
	fx[3] = xOffset;  fy[3] = 0;

	CItem *i = NULL;
	UI08 worldNumber = caster->WorldNumber();
	for( j = 0; j < 4; j++ )	// Draw the corners of our diamond
	{
		i = Items->SpawnItem( sock, caster, 1, "#", false, id, 0, false, false );
		if( i != NULL )
		{
			i->SetDispellable( true );
			i->SetDecayable( true );
			i->SetDecayTime( BuildTimeValue( caster->GetSkill( MAGERY ) / 15 ) );
			i->SetMoreX( caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
			i->SetMoreY( caster->GetSerial() );
			i->SetLocation( x + fx[j], y + fy[j], Map->Height( x + fx[j], y + fy[j], z, worldNumber ) );
			i->SetDir( 29 );
			i->SetMagic( 2 );
			RefreshItem( i );
		}
	}
	for( j = -1; j < 2; j = j + 2 )
	{
		for( SI08 counter2 = -1; counter2 < 2; counter2 += 2 )
		{
			for( int counter3 = 1; counter3 < yOffset; counter3++ )
			{
				i = Items->SpawnItem( sock, caster, 1, "#", false, id, 0, false, false );
				if( i != NULL )
				{
					i->SetDispellable( true );
					i->SetDecayable( true );
					i->SetDecayTime( BuildTimeValue( caster->GetSkill( MAGERY ) / 15 ) );
					i->SetMoreX( caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
					i->SetMoreY( caster->GetSerial() );
					const SI16 newX = x + counter2 * counter3;
					const SI16 newY = y + j * ( yOffset - counter3 );
					i->SetLocation( newX, newY, Map->Height( newX, newY, z, worldNumber ) );
					i->SetDir( 29 );
					i->SetMagic( 2 );
					RefreshItem( i );
				}
			}
		}
	}
	return true;
}

bool SquareSpell( cSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
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
	for( UI08 j = 0; j < 4; j++ )	// Draw the corners of our diamond
	{
		for( int counter = fx[j]; counter < fy[j]; counter++ )
		{
			i = Items->SpawnItem( sock, caster, 1, "#", false, id, 0, false, false );
			if( i != NULL )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( caster->GetSkill( MAGERY ) / 15 ) );
				i->SetMoreX( caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
				i->SetMoreY( caster->GetSerial() );
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
				i->SetMagic( 2 );
				RefreshItem( i );
			}
		}
	}
	return true;
}

bool FloodSpell( cSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length )
{
	SI16 yOffset = length;
	SI16 xOffset = length;

	SI16 maxX = x + xOffset;
	SI16 maxY = y + yOffset;
	SI16 minX = x - xOffset;
	SI16 minY = y - yOffset;

	UI08 worldNumber = caster->WorldNumber();
	for( int counter1 = minX; counter1 <= maxX; counter1++ )
	{
		for( int counter2 = minY; counter2 <= maxY; counter2++ )
		{
			CItem *i = Items->SpawnItem( sock, caster, 1, "#", 0, id, 0, false, false );
			if( i != NULL )
			{
				i->SetDispellable( true );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( caster->GetSkill( MAGERY ) / 15 ) );
				i->SetMoreX( caster->GetSkill( MAGERY ) ); // remember casters magery skill for damage
				i->SetMoreY( caster->GetSerial() );
				i->SetLocation( counter1, counter2, Map->Height( counter1, counter2, z, worldNumber ) );
				i->SetDir( 29 );
				i->SetMagic( 2 );
				RefreshItem( i );
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
	spells.resize( 0 );
}

bool cMagic::HasSpell( CItem *book, int spellNum )
{
	if( book == NULL )
		return 0;
	UI32 wordNum = spellNum / 32;
	UI32 bitNum = (spellNum % 32);
	UI32 sourceAmount;
	switch( wordNum )
	{
	case 0:	sourceAmount = book->GetMoreX();	break;
	case 1:	sourceAmount = book->GetMoreY();	break;
	case 2:	sourceAmount = book->GetMoreZ();	break;
	default: sourceAmount = 0;					break;
	};
	return ( ((sourceAmount>>bitNum)%2) == 1 );
}

void cMagic::AddSpell( CItem *book, int spellNum )
{
	cScript *tScript = NULL;
	UI16 scpNum = book->GetScriptTrigger();
	tScript = Trigger->GetScript( scpNum );
	if( tScript != NULL )
		tScript->OnSpellGain( book, spellNum );	

	if( book == NULL )
		return;
	UI32 wordNum = spellNum / 32;
	UI32 bitNum = (spellNum % 32);
	UI32 flagToSet = power( 2, bitNum );
	UI32 targAmount;
	switch( wordNum )
	{
	case 0:
		targAmount = ( book->GetMoreX() | flagToSet );
		book->SetMoreX( targAmount );
		return;
	case 1:
		targAmount = ( book->GetMoreY() | flagToSet );
		book->SetMoreY( targAmount );
		return;
	case 2:
		targAmount = ( book->GetMoreZ() | flagToSet );
		book->SetMoreZ( targAmount );
		return;
	default:
		return;
	};
}

void cMagic::RemoveSpell( CItem *book, int spellNum )
{
	cScript *tScript = NULL;
	UI16 scpNum = book->GetScriptTrigger();
	tScript = Trigger->GetScript( scpNum );
	if( tScript != NULL )
	tScript->OnSpellLoss( book, spellNum );	

	if( book == NULL )
		return;
	UI32 wordNum = spellNum / 32;
	UI32 bitNum = (spellNum % 32);
	UI32 flagToSet = power( 2, bitNum );
	UI32 flagMask = 0xFFFFFFFF;
	UI32 targAmount;
	flagMask ^= flagToSet;
	switch( wordNum )
	{
	case 0:
		targAmount = ( book->GetMoreX() & flagMask );
		book->SetMoreX( targAmount );
		return;
	case 1:
		targAmount = ( book->GetMoreY() & flagMask );
		book->SetMoreY( targAmount );
		return;
	case 2:
		targAmount = ( book->GetMoreZ() & flagMask );
		book->SetMoreZ( targAmount );
		return;
	default:
		return;
	};
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SpellBook( cSocket *mSock )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Opens the spellbook and displays all spells a
//|								 character has in his book.
//o---------------------------------------------------------------------------o
void cMagic::SpellBook( cSocket *mSock )
{
	int scount;
	UI08 i;
	UI08 spellsList[70];
	char sbookstart[8] = "\x24\x40\x01\x02\x03\xFF\xFF";
	char sbookinit[6] = "\x3C\x00\x3E\x00\x03";
	char sbookspell[20] = "\x40\x01\x02\x03\x1F\x2E\x00\x00\x01\x00\x48\x00\x7D\x40\x01\x02\x03\x00\x00";

	SERIAL serial = mSock->GetDWord( 1 );
	serial &= 0x7FFFFFFF;
	CChar *mChar = mSock->CurrcharObj();
	CItem *item = calcItemObjFromSer( serial );

	// Here's the kicker.  We NEVER needed to search through our pack for our spellbook!!! Why, you might ask???
	// Because we are able to calculate the serial of the spellbook, because it's targetted for us!!!!
	// Hence, we can remove all searches from here, and just use it directly
	// The only time we need to search is when they use the macro, and that's easily done (item == -1)
	// Abaddon

	CItem *spellBook = item;
	CItem *x = getPack( mChar );
	if( spellBook == NULL )	// used a macro!!!
		spellBook = FindItemOfType( mChar, 9 );
	if( spellBook == NULL )	// we STILL have no spellbook!!!
	{
		sysmessage( mSock, 692 );
		return;
	}
	if( spellBook->GetCont() != mChar->GetSerial() && spellBook->GetCont() != x->GetSerial() )
	{
		sysmessage( mSock, 403 );
		return;
	}
	item = spellBook;		
	sendItem( mSock, item );
	
	sbookstart[1] = item->GetSerial( 1 );
	sbookstart[2] = item->GetSerial( 2 );
	sbookstart[3] = item->GetSerial( 3 );
	sbookstart[4] = item->GetSerial( 4 );
	mSock->Send( sbookstart, 7 );
	scount = 0;
	memset( spellsList, 0, sizeof( UI08 ) * 70 );

	CItem *ourBook = spellBook;
	for( UI08 j = 0; j < 65; j++ )
	{
		if( HasSpell( ourBook, j ) )
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
		for( i = 0; i < 65; i++ )
			spellsList[i] = 1;
		spellsList[64] = 0;
	}
	spellsList[64] = spellsList[65];
	spellsList[65] = 0;
	
	for( i = 0; i < 65; i++ )
	{
		if( spellsList[i] )
			scount++;
	}
	sbookinit[1] = (UI08)(((scount*19)+5)>>8);
	sbookinit[2] = (UI08)(((scount*19)+5)%256);
	sbookinit[3] = (UI08)(scount>>8);
	sbookinit[4] = (UI08)(scount%256);
	if( scount > 0 ) 
		mSock->Send( sbookinit, 5 );
	for( i = 0; i < 65; i++ )
	{
		if( spellsList[i] )
		{
			sbookspell[0] = 0x41;
			sbookspell[1] = 0x00;
			sbookspell[2] = 0x00;
			sbookspell[3] = i+1;
			sbookspell[8] = i+1;
			sbookspell[13] = item->GetSerial( 1 );
			sbookspell[14] = item->GetSerial( 2 );
			sbookspell[15] = item->GetSerial( 3 );
			sbookspell[16] = item->GetSerial( 4 );
			mSock->Send( sbookspell, 19 );
		} 
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          GateCollision( PLAYER s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Modified by AntiChrist - collision dir
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used when a PLAYER passes through a gate.  Takes the player
//|                              to the other side of the gate-link.
//o---------------------------------------------------------------------------o

bool cMagic::GateCollision( CChar *s )
{
	if( s == NULL )
		return false;
	UI08 worldNumber = s->WorldNumber();
	SubRegion *toCheck = MapRegion->GetCell( s->GetX(), s->GetY(), worldNumber );
	if( toCheck == NULL )
		return false;
	CItem *itemCheck = NULL;
	toCheck->PushChar();
	toCheck->PushItem();
	for( itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
	{
		if( itemCheck == NULL )
			continue;
		if( itemCheck->GetType() == 51 || itemCheck->GetType() == 52 )
		{
			if( s->GetX() == itemCheck->GetX() && s->GetY() == itemCheck->GetY() && s->GetZ() == itemCheck->GetZ() )
			{
				CItem *otherGate = calcItemObjFromSer( itemCheck->GetMoreX() );
				if( otherGate == NULL )
				{
					toCheck->PopChar();
					toCheck->PopItem();
					return false;
				}
				SI08 dirOffset = 0;
				if( s->GetDir() < SOUTH )
					dirOffset = 1;
				else
					dirOffset = -1;
				if( !s->IsNpc() )
				{
					for( CChar *charCheck = toCheck->FirstChar(); !toCheck->FinishedChars(); charCheck = toCheck->GetNextChar() )
					{
						if( charCheck == NULL )
							continue;
						if( charCheck->IsNpc() && charCheck->GetOwner() == s->GetSerial() )	// follow our owner
						{
							if( getDist( charCheck, s ) <= 4 )
							{
								charCheck->SetLocation( otherGate->GetX() + dirOffset, otherGate->GetY(), otherGate->GetZ() );
								charCheck->Teleport();
							}
						}
					}
				}
				s->SetLocation( otherGate->GetX() + dirOffset, otherGate->GetY(), otherGate->GetZ() );
				s->Teleport();
				soundeffect( calcSocketObjFromChar( s ), 0x01FE, true );
				staticeffect( s, 0x372A, 0x09, 0x06 );
			}
		}
	}
	toCheck->PopChar();
	toCheck->PopItem();
	return true;
}

//o---------------------------------------------------------------------------o
//|     Class         :          SummonMonster( cSocket *s, CChar *caster, UI16 id, char * monstername, UI16 color, SI16 x, SI16 y, SI08 z )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|		Revision	  :			 May 4th, 2000 - made function general words PC's and NPC's - Hanse
//o---------------------------------------------------------------------------o
//|     Purpose       :          Summon a monster (dispellable with DISPEL).
//o---------------------------------------------------------------------------o
void cMagic::SummonMonster( cSocket *s, CChar *caster, UI16 id, char * monstername, UI16 colour, SI16 x, SI16 y, SI08 z )
{
	if( s != NULL )
		caster = s->CurrcharObj();
	else
	{	// Since caster will determine who is casting the spell... get a socket for players to see animations and hear effects - Hanse
		if( caster->IsNpc() && !chars[caster->GetTarg()].IsNpc() )
			s = calcSocketObjFromChar( caster->GetTarg() );
	}

	CHARACTER newCharOff;
	CChar *newChar = Npcs->MemCharFree( newCharOff );
	if( newChar == NULL )
		return;

	switch( id )
	{
	case 0x0000:	// summon monster
		Npcs->DeleteChar( newChar );
		soundeffect( s, 0x0217, true );
		newChar = Npcs->CreateRandomNpc( s, "10000", caster->WorldNumber() );
		if( newChar == NULL )
		{
			sysmessage( s, 694 );
			return;
		}
		newChar->SetOwner( caster->GetSerial() );
		newChar->SetSummonTimer( BuildTimeValue( caster->GetSkill( MAGERY ) / 5 ) );
		newChar->SetLocation( caster );
		newChar->Teleport();
		npcAction( newChar, 0x0C );
		newChar->SetFTarg( calcCharFromSer( caster->GetSerial() ) );
		newChar->SetNpcWander( 1 );
		sysmessage( s, 695 );
		return;
	case 0x000D: // Energy Vortex & Air elemental
		if( colour == 0x0075 )
		{
			soundeffect( s, 0x0216, true ); // EV
			newChar->SetDef( 22 );
			newChar->SetLoDamage( 10 );	// Damage may be high, but 10-30 did on average 4 to a troll...
			newChar->SetHiDamage( 70 );
			newChar->SetSpAttack( 7 );	// 1-7 level spells (do EV's cast for sure?)
			newChar->SetBaseSkill( 950, MAGERY );
			newChar->SetBaseSkill( 2000, WRESTLING );
			newChar->SetBaseSkill( 2000, TACTICS );
			newChar->SetSkill( 850, MAGICRESISTANCE );
			newChar->SetStrength( 800 );
			newChar->SetHP( 800 );
			newChar->SetDexterity( 160 );
			newChar->SetStamina( 160 );
			newChar->SetIntelligence( 180 );
			newChar->SetMana( 180 );
			newChar->SetNpcWander( 2 );
			newChar->SetNPCAiType( 0x50 );
			newChar->SetPoison( 3 );	
		}
		else
		{
			soundeffect( s, 0x0217, true ); // AE
			newChar->SetDef( 19 );
			newChar->SetLoDamage( 5 );
			newChar->SetHiDamage( 13 );
			newChar->SetSpAttack( 6 ); // 1-6 level spells
			newChar->SetBaseSkill( 750, MAGERY );
			newChar->SetBaseSkill( 950, WRESTLING );
			newChar->SetBaseSkill( 700, TACTICS );
			newChar->SetSkill( 450, MAGICRESISTANCE );
			newChar->SetStrength( 125 );
			newChar->SetHP( 125 );
			newChar->SetDexterity( 100 );
			newChar->SetStamina( 100 );
			newChar->SetIntelligence( 80 );
			newChar->SetMana( 80 );
		}
		break;
	case 0x000A: // Daemon
		soundeffect( s, 0x0216, true );
		newChar->SetDef( 20 );
		newChar->SetLoDamage( 10 );
		newChar->SetHiDamage( 45 );
		newChar->SetSpAttack( 7 ); // 1-7 level spells
		newChar->SetBaseSkill( 900, MAGERY );
		newChar->SetBaseSkill( 1500, TACTICS );
		newChar->SetBaseSkill( 1500, WRESTLING );
		newChar->SetSkill( 650, MAGICRESISTANCE );
		newChar->SetStrength( 400 );
		newChar->SetHP( 400 );
		newChar->SetDexterity( 70 );
		newChar->SetStamina( 70 );
		newChar->SetIntelligence( 400 );
		newChar->SetMana( 400 );
		break;
	case 0x000E: //Earth
		soundeffect( s, 0x0217, true );
		newChar->SetDef( 15 );
		newChar->SetLoDamage( 3 );
		newChar->SetHiDamage( 18 );
		newChar->SetBaseSkill( 850, TACTICS );
		newChar->SetBaseSkill( 850, WRESTLING );
		newChar->SetSkill( 350, MAGICRESISTANCE );
		newChar->SetStrength( 125 );
		newChar->SetHP( 125 );
		newChar->SetDexterity( 90 );
		newChar->SetStamina( 90 );
		newChar->SetIntelligence( 70 );
		newChar->SetMana( 70 );
		break;
	case 0x000F: //Fire
	case 0x0010: //Water
		soundeffect( s, 0x0217, true );
		newChar->SetDef( 19 );
		newChar->SetLoDamage( 4 );
		newChar->SetHiDamage( 12 );
		newChar->SetSpAttack( 6 ); // 1-6 level spells
		newChar->SetBaseSkill( 800, MAGERY );
		newChar->SetBaseSkill( 800, TACTICS );
		newChar->SetSkill( 450, MAGICRESISTANCE );
		newChar->SetBaseSkill( 800, WRESTLING );
		newChar->SetStrength( 120 );
		newChar->SetHP( 120 );
		newChar->SetDexterity( 95 );
		newChar->SetStamina( 95 );
		newChar->SetIntelligence( 70 );
		newChar->SetMana( 70 );
		break;
	case 0x023E: //Blade Spirits
		soundeffect( s, 0x0212, true ); // I don't know if this is the right effect...
		newChar->SetDef( 24 );
		newChar->SetLoDamage( 5 );
		newChar->SetHiDamage( 10 );
		newChar->SetBaseSkill( 950, TACTICS );
		newChar->SetBaseSkill( 950, WRESTLING );
		newChar->SetSkill( 650, MAGICRESISTANCE );
		newChar->SetStrength( 400 );
		newChar->SetHP( 400 );
		newChar->SetDexterity( 95 );
		newChar->SetStamina( 95 );
		newChar->SetIntelligence( 70 );
		newChar->SetMana( 70 );
		newChar->SetNpcWander( 2 );
		newChar->SetNPCAiType( 0x50 );
		newChar->SetPoison( 2 );
		break;
	case 0x03e2: // Dupre The Hero
		soundeffect( s, 0x0246, true );
		newChar->SetDef( 50 );
		newChar->SetLoDamage( 50 );
		newChar->SetHiDamage( 100 );
		newChar->SetSpAttack( 7 ); // 1-7 level spells
		newChar->SetBaseSkill( 900, MAGERY );
		newChar->SetBaseSkill( 1000, TACTICS );
		newChar->SetBaseSkill( 1000, SWORDSMANSHIP );
		newChar->SetBaseSkill( 1000, PARRYING );
		newChar->SetSkill( 650, MAGICRESISTANCE );
		newChar->SetStrength( 600 );
		newChar->SetHP( 600 );
		newChar->SetDexterity( 70 );
		newChar->SetStamina( 70 );
		newChar->SetIntelligence( 100 );
		newChar->SetMana( 100 );
		newChar->SetFame( 10000 );
		newChar->SetKarma( 10000 );
		break;
	case 0x000B: // Black Night
		soundeffect( s, 0x0216, true );
		newChar->SetDef( 50 );
		newChar->SetLoDamage( 50 );
		newChar->SetHiDamage( 100 );
		newChar->SetSpAttack( 7 ); // 1-7 level spells
		newChar->SetBaseSkill( 1000, MAGERY );
		newChar->SetBaseSkill( 1000, TACTICS );
		newChar->SetBaseSkill( 1000, SWORDSMANSHIP );
		newChar->SetBaseSkill( 1000, PARRYING );
		newChar->SetSkill( 1000, MAGICRESISTANCE );
		newChar->SetStrength( 600 );
		newChar->SetHP( 600 );
		newChar->SetDexterity( 70 );
		newChar->SetStamina( 70 );
		newChar->SetIntelligence( 100 );
		newChar->SetMana( 100 );
		break;
	case 0x0190: // Death Knight
		soundeffect( s, 0x0246, true );
		newChar->SetDef( 20 );
		newChar->SetLoDamage( 10 );
		newChar->SetHiDamage( 45 );
		newChar->SetSpAttack( 7 ); // 1-7 level spells
		newChar->SetBaseSkill( 500, MAGERY );
		newChar->SetBaseSkill( 1000, TACTICS );
		newChar->SetBaseSkill( 1000, SWORDSMANSHIP );
		newChar->SetBaseSkill( 1000, PARRYING );
		newChar->SetSkill( 650, MAGICRESISTANCE );
		newChar->SetStrength( 600 );
		newChar->SetHP( 600 );
		newChar->SetDexterity( 70 );
		newChar->SetStamina( 70 );
		newChar->SetIntelligence( 100 );
		newChar->SetMana( 100 );
		newChar->SetFame( -10000 );
		newChar->SetKarma( -10000 );
		break;
	default:
		soundeffect( s, 0x0215, true );
	}
	newChar->SetName( monstername );
	newChar->SetID( id );
	newChar->SetxID( id );
	newChar->SetOrgID( id );
	newChar->SetSkin( colour );
	newChar->SetxSkin( newChar->GetSkin() );
	newChar->SetDispellable( true );
	newChar->SetNpc( true );

	// pc's don't own BS/EV, NPCs do
	if( caster->IsNpc() || ( id != 0x023E && !( id == 0x000D && colour == 0x0075 ) ) )
		newChar->SetOwner( caster->GetSerial() );
	
	if( x == 0 )
		newChar->SetLocation( caster->GetX()-1, caster->GetY(), caster->GetZ() );
	else
		newChar->SetLocation( x, y, z );
	
	
	newChar->SetSpDelay( 10 );
	newChar->SetSummonTimer( BuildTimeValue( caster->GetSkill( MAGERY ) / 5 ) );
	newChar->Update();
	npcAction( newChar, 0x0C );
	// AntiChrist (9/99) - added the chance to make the monster attack
	// the person you targeted ( if you targeted a char, naturally :) )
	CChar *i = NULL;
	if( !caster->IsNpc() && s != NULL )
	{
		if( s->GetDWord( 7 ) != INVALIDSERIAL )
		{
			i = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( i == NULL ) 
				return;
		}
	}
	else
		i = &chars[caster->GetTarg()];
	npcAttackTarget( i, newChar );
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckBook( int circle, int spell, int i )
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
		spellnum++;
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
	if( s == NULL )
		return true;
	if( s->NoNeedMana() )
		return true;
	if( s->GetMana() >= spells[num].Mana() ) 
		return true;
	else
	{
		cSocket *p = calcSocketObjFromChar( s );
		if( p != NULL ) 
			sysmessage( p, 696 );
	}
	return false;
}

bool cMagic::CheckStamina( CChar *s, int num )
{
	if( s == NULL )
		return true;
	if( s->NoNeedMana() )
		return true;
	if( s->GetStamina() >= spells[num].Stamina() ) 
		return true;
	else
	{
		cSocket *p = calcSocketObjFromChar( s );
		if( p != NULL ) 
			sysmessage( p, 697 );
	}
	return false;
}

bool cMagic::CheckHealth( CChar *s, int num )
{
	if( s == NULL )
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
	cSocket *p = calcSocketObjFromChar( s );
	if( p != NULL ) 
		sysmessage( p, 698 );
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

	updateStats( s, 1 );
}

void cMagic::SubtractStamina( CChar *s, int stamina )
{
	if( s->NoNeedMana() )
		return;
	s->SetStamina( s->GetStamina() - stamina );
	if( s->GetStamina() < 0 ) 
		s->SetStamina( 0 );

	updateStats( s, 2 );
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

	updateStats( s, 0 );
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
		staticeffect( i, 0x373A, 0, 15 );
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
	cSocket *s = NULL;
	if( i )
	{
		if( attacker != NULL )
		{
			if( ( defender->GetSkill( MAGICRESISTANCE ) - attacker->GetSkill( EVALUATINGINTEL ) ) >= 0 )
			{
				s = calcSocketObjFromChar( defender );
				if( s != NULL )
					sysmessage( s, 699 );
			}
			else
				i = false;
		}
		else
		{
			s = calcSocketObjFromChar( defender );
			if( s != NULL )
				sysmessage( s, 699 );
		}
	}
	return i;
}

//o---------------------------------------------------------------------------o
//|     Class         :          MagicDamage( CChar *p )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Calculate and inflict magic damage.
//o---------------------------------------------------------------------------o

void cMagic::MagicDamage( CChar *p, int amount, CChar *attacker )
{
	cSocket *s = NULL;
	if ( !p || !attacker )
		return;

	if( p->IsDead() || p->GetHP() <= 0 )	// extra condition check, to see if deathstuff hasn't been hit yet
		return;
	if( Skills->CheckSkill( p, EVALUATINGINTEL, 0, 1000 ) ) 
		amount = max( 1, amount - ( amount * ( p->GetSkill( EVALUATINGINTEL )/10000 ) ) ); // Take off 0 to 10% damage but still hurt at least 1hp (1000/10000=0.10)
	if( p->IsFrozen() && p->GetDexterity() > 0 )
	{
		p->SetFrozen( false );
		s = calcSocketObjFromChar( p );
		if( s != NULL ) 
			sysmessage( s, 700 );
	}           
	if( !p->IsInvulnerable() && !region[p->GetRegion()]->CanCastAggressive() )
	{
		if( p->IsNpc() ) 
			amount *= 2;      // double damage against non-players
		p->SetHP( max( 0, p->GetHP() - amount) );
		updateStats( p, 0 );
		if( p->GetHP() <= 0 )
		{
			if( attacker != NULL && p != attacker )	// can't gain fame and karma for suicide :>
			{
				Karma( attacker, p, ( 0 - ( p->GetKarma() ) ) );
				Fame( attacker, p->GetFame() );
			}
			doDeathStuff( p );
		}
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
		cSocket *s = calcSocketObjFromChar( p );
		if( s != NULL ) 
			sysmessage( s, 700 );
	}           
	if( !p->IsInvulnerable() && !region[p->GetRegion()]->CanCastAggressive() )
	{
		if( poison > 5 ) 
			poison = 5;
		if( poison < 0 ) 
			poison = 1;
		p->SetPoisoned( poison );
		p->SetPoisonWearOffTime( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) ) );
		if( !p->IsNpc() )
			p->SendToSocket( calcSocketObjFromChar( p ), true, p );
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          CheckFieldEffects2( CChar *c, char timecheck )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if character stands on a magic-field,
//|                              and apply effects.
//o------------------------------------------------------------------------

//o------------------------------------------------------------------------
// timecheck: 0: always executed no matter of the nextfieldspelltime value
// timecheck: 1: only executed if the time is right for next fieldeffect check
// we need this cause its called from npccheck and pc-check
// npc-check already has its own timer, pc check not.
// thus in npccheck its called with 0, in pc check with 1
// we could add the fieldeffect check time the server.scp but I think this solution is better.
// LB October 99
//o---------------------------------------------------------------------------o

void cMagic::CheckFieldEffects2( CChar *c, char timecheck )
{
	if( timecheck && nextfieldeffecttime > uiCurrentTime )
		return;

	CChar *caster = NULL;

	int xOffset = MapRegion->GetGridX( c->GetX() );
	int yOffset = MapRegion->GetGridY( c->GetY() );
	UI08 worldNumber = c->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( toCheck == NULL )
				continue;
			toCheck->PushItem();
			for( CItem *itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
			{
				if( itemCheck == NULL )
					continue;
				if( itemCheck->GetX() == c->GetX() && itemCheck->GetY() == c->GetY() )
				{
					switch( itemCheck->GetID() )
					{
					case 0x3996:
					case 0x398C:	// firefield
						if( c->IsInnocent() )
						{
							caster = calcCharObjFromSer( itemCheck->GetMoreY() );	// store caster in morey
							if( c->IsInnocent() && caster != NULL && !caster->IsGM() && !caster->IsCounselor() )
								criminal( caster );
						}
						if( RandomNum( 0, 2 ) == 1 )
						{
							if( !CheckResist( NULL, c, 4 ) )
								MagicDamage( c, itemCheck->GetMoreX() / 100 );
							else
								MagicDamage( c, itemCheck->GetMoreX() / 200 );
						}
						soundeffect( c, 520 );
						toCheck->PopItem();
						return;
					case 0x3915:
					case 0x3920:	// poison
						if( c->IsInnocent() )
						{
							caster = calcCharObjFromSer( itemCheck->GetMoreY() );	// store caster in morey
							if( c->IsInnocent() && caster != NULL && !caster->IsGM() && !caster->IsCounselor() )
								criminal( caster );
						}
						if( RandomNum( 0, 2 ) == 1 )
						{
							if( !CheckResist( NULL, c, 5 ) )
							{
								if( itemCheck->GetMoreX() < 997 )
									PoisonDamage( c, 2 );
								else 
									PoisonDamage( c, 3 );
							} 
							else 
								PoisonDamage( c, 1 );
						}
						soundeffect( c, 520 );
						toCheck->PopItem();
						return;
					case 0x3979:
					case 0x3967:	// para
						if( c->IsInnocent() )
						{
							caster = calcCharObjFromSer( itemCheck->GetMoreY() );	// store caster in morey
							if( c->IsInnocent() && caster != NULL && !caster->IsGM() && !caster->IsCounselor() )
								criminal( caster );
						}
						if( RandomNum( 0, 2 ) == 1 && !CheckResist( NULL, c, 6 ) )
							tempeffect( c, c, 1, 0, 0, 0 );
						soundeffect( c, 520 );     
						toCheck->PopItem();
						return;
					}
					break;
				}
			}
			toCheck->PopItem();
		}
	}	
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::BoxSpell( cSocket * s, int& x1, int& x2, int& y1, int& y2, int& z1, int& z2 )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Calculate the spell box effect, depending 
//|                              on character's magery skill.
//o---------------------------------------------------------------------------o

void cMagic::BoxSpell( cSocket *s, CChar *caster, SI16& x1, SI16& x2, SI16& y1, SI16& y2, SI08& z1, SI08& z2)
{
	SI16 x, y;
	SI08 z;
	SI16 length;
	
	if( s != NULL )
	{
		x = s->GetWord( 11 );
		y = s->GetWord( 13 );
		z = s->GetByte( 16 );
		caster = s->CurrcharObj();
	}
	else
	{
		x = chars[caster->GetTarg()].GetX();
		y = chars[caster->GetTarg()].GetY();
		z = chars[caster->GetTarg()].GetZ();
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
	if( s == NULL || i == NULL )
		return;
    staticeffect( s, 0x36B0, 0x09, 0x09 );
    soundeffect( s, 0x0207 );
	if( CheckResist( NULL, s, 4 ) ) 
		MagicDamage( s, i->GetMoreB( 2 ) / 2 );
	else 
		MagicDamage( s, i->GetMoreB( 2 ) / 2 );
	i->SetMoreB( 0, 0, 0, i->GetMoreB( 4 ) );
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

	if( reagents->ash != 0 && getAmount( s, 0x0F8C ) < reagents->ash )
		failmsg.ash = 1;
	if( reagents->drake!=0 && getAmount( s, 0x0F86 ) < reagents->drake )
		failmsg.drake = 1;
	if( reagents->garlic != 0 && getAmount( s, 0x0F84 ) < reagents->garlic )
		failmsg.garlic = 1;
	if( reagents->ginseng != 0 && getAmount( s, 0x0F85 ) < reagents->ginseng )
		failmsg.ginseng = 1;
	if( reagents->moss != 0 && getAmount( s, 0x0F7B ) < reagents->moss )
		failmsg.moss = 1;
	if( reagents->pearl != 0 && getAmount( s, 0x0F7A ) < reagents->pearl )
		failmsg.pearl = 1;
	if( reagents->shade != 0 && getAmount( s, 0x0F88 ) < reagents->shade )
		failmsg.shade = 1;
	if( reagents->silk != 0 && getAmount( s, 0x0F8D ) < reagents->silk )
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
	if( s == NULL )
		return true;
	bool display = false;
	char message[100] = { 0, };
	
	strcpy( message, Dictionary->GetEntry( 702 ) );
	if( failmsg.ash ) { display = true; sprintf( message, "%sSa, ", message ); }
	if( failmsg.drake ) { display = true; sprintf( message, "%sMr, ", message ); }
	if( failmsg.garlic ) { display = true; sprintf( message, "%sGa, ", message ); }
	if( failmsg.ginseng ) { display = true; sprintf( message, "%sGi, ", message ); }
	if( failmsg.moss ) { display = true; sprintf( message, "%sBm, ", message ); }
	if( failmsg.pearl ) { display = true; sprintf( message, "%sBp, ", message ); }
	if( failmsg.shade ) { display = true; sprintf( message, "%sNs, ", message ); }
	if( failmsg.silk ) { display = true; sprintf( message, "%sSs, ", message ); }
	
	message[strlen( message ) - 1] = ']';
	
	if( display )
	{
		cSocket *i = calcSocketObjFromChar( s );
		if( i != NULL )
			sysmessage( i, message );
		return false;
	}
	return true;
	
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SpellFail( cSocket *s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Do visual and sound effects when a player
//|                              fails to cast a spell.
//o---------------------------------------------------------------------------o

void cMagic::SpellFail( cSocket *s )
{
	// Use Reagents on failure ( if casting from a spellbook )
	CChar *mChar = s->CurrcharObj();
	if( s->CurrentSpellType() == 0 )
		DelReagents( mChar, spells[mChar->GetSpellCast()].Reagants() );
	staticeffect( mChar, 0x3735, 0, 30 );
	soundeffect( mChar, 0x005C );
	npcEmote( s, mChar, 771, false );
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
		cSocket *s = calcSocketObjFromChar( player );
		if( s != NULL )
			sysmessage( s, 703 );
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
//|     Class         :          ::SelectSpell( cSocket *mSock, int num )
//|     Date          :          28 August 1999 / 10 September 1999
//|     Programmer    :          Abaddon / AntiChrist
//o---------------------------------------------------------------------------o
//|     Purpose       :          Execute the selected spell to cast.
//o---------------------------------------------------------------------------o

bool cMagic::SelectSpell( cSocket *mSock, int num )
// PRE:		s is a valid socket, num is a valid spell number
// POST:	Spell selected to cast
// Comments: Written by Abaddon (28 August 1999)
//:Terrin: Use spells (memory version of script info )
//Abaddon: Made memory optionable
//Abaddon: Perma cached
//DarkStorm: Added JS handling
{
	int lowSkill = 0, highSkill = 0;
	CChar *mChar = mSock->CurrcharObj();
	int type = mSock->CurrentSpellType();
	SpellInfo curSpellCasting = spells[num];

	if( mChar->GetSpellTime() != 0 )
	{
		if( mChar->GetCasting() == 1 || mChar->GetCasting() == -1 )
		{
			sysmessage( mSock, 762 );
			return false;
		}
		else if( mChar->GetSpellTime() > uiCurrentTime )
		{
			sysmessage( mSock, 1638 );
			return false;
		}
	}
	
	mChar->SetSpellCast( num );
	if( num > 63 && num <= spellCount && spellCount <= 70 )
		Log( Dictionary->GetEntry( magic_table[num].spell_name ), mChar, NULL, "(Attempted)");
	if( mChar->IsJailed() && !mChar->IsGM() )
	{
		sysmessage( mSock, 704 );
		mChar->StopSpell();
		return false;
	}
	
	// (Abaddon) Region checks
	cTownRegion *ourReg = region[mChar->GetRegion()];
	if( (num == 45 && !ourReg->CanMark()) || (num == 52 && !ourReg->CanGate()) || (num == 32 && !ourReg->CanRecall()) )
	{
		sysmessage( mSock, 705 );
		mChar->StopSpell();
		return false;
	}

	if( !region[mChar->GetRegion()]->CanCastAggressive() && spells[mChar->GetSpellCast()].AggressiveSpell() ) // ripper 9-11-99 not in town :)
	{
		sysmessage( mSock, 706 );
		mChar->StopSpell();
		return false;
	}
	
	if( !curSpellCasting.Enabled() )
	{
		sysmessage( mSock, 707 );
		mChar->StopSpell();
		return false;
	}
	
	//Cut the casting requirement on scrolls
	if( type == 1 && cwmWorldState->ServerData()->GetCutScrollRequirementStatus() ) // only if enabled
	{
		lowSkill = curSpellCasting.ScrollLow();
		highSkill = curSpellCasting.ScrollHigh();
	}
	else
	{
		lowSkill = curSpellCasting.LowSkill();
		highSkill = curSpellCasting.HighSkill();
	}
	
	
	// The following loop checks to see if any item is currently equipped (if not a GM)
	if( !mChar->IsGM() )
	{
		for( CItem *i = mChar->FirstItem(); !mChar->FinishedItems(); i = mChar->NextItem() )
		{
			if( i != NULL )
			{
				if( type != 2 && ( i->GetLayer() == 2 || ( i->GetLayer() == 1 && i->GetType() != 9 ) ) )
				{
					sysmessage( mSock, 708 );
					mChar->StopSpell();
					return false;
				}
			}
		}
	}
	
	if( mChar->GetHidden() && !mChar->IsPermHidden() )
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
	cScript *toExecute = Trigger->GetScript( spellTrig );
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
	
	if( ( !mChar->IsGM() ) && ( !Skills->CheckSkill( mChar, MAGERY, lowSkill, highSkill ) ) )
	{
		npcTalkAll( mChar, curSpellCasting.Mantra(), false );
		SpellFail( mSock );
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
				sysmessage( mSock, 696 );
				mChar->StopSpell();
				return false;
			}
			if( !CheckStamina( mChar, num ) )
			{
				sysmessage( mSock, 697 );
				mChar->StopSpell();
				return false;
			}
			if( !CheckHealth( mChar, num ) )
			{
				sysmessage( mSock, 698 );
				mChar->StopSpell();
				return false;
			}
		}
	}	
	   
	mChar->SetSpellAction( curSpellCasting.Action() );
	mChar->SetNextAct( 75 );		// why 75?

	// Delay measurement...
	if( Delay >= 0 ) 
	{
		mChar->SetSpellTime( BuildTimeValue( Delay / 1000 ) );
		mChar->SetFrozen( true );
	}
	else if( type == 0 && !mChar->IsGM() ) // if they are a gm they don't have a delay :-)
	{
		mChar->SetSpellTime( BuildTimeValue( curSpellCasting.Delay() / 10 ) );
		mChar->SetFrozen( true );
	}
	else
		mChar->SetSpellTime( 0 );
	// Delay measurement end
	
	if( !mChar->IsOnHorse() )
		impaction( mSock, curSpellCasting.Action() ); // do the action
	
	char temp[128];
	if( spells[num].FieldSpell() )
	{
		if( mChar->GetSkill( MAGERY ) > 600 )
			sprintf( temp, "Vas %s", curSpellCasting.Mantra() );
		else
			strcpy( temp, curSpellCasting.Mantra() );
	}
	else
		strcpy( temp, curSpellCasting.Mantra() );

	npcTalkAll( mChar, temp, false );
	mChar->SetCasting( 1 );
	return true;
	
}

void cMagic::CastSpell( cSocket *s, CChar *caster )
{
	// for LocationTarget spell like ArchCure, ArchProtection etc...
	SI08 curSpell; // = chars[currchar[s]].GetSpellCast();
	SI16 x, y;
	SI08 z;

	if( s == NULL ) 
		curSpell = caster->GetSpellCast();
	else
	{
		caster = s->CurrcharObj();
		curSpell = caster->GetSpellCast();
	}

	if( caster->GetCasting() == 0 && caster->GetSpellCast() == -1 )
		return;	// interrupted

	caster->StopSpell();

	UI16 spellTrig = caster->GetScriptTrigger();
	cScript *toExecute = Trigger->GetScript( spellTrig );
	if( toExecute != NULL )
	{
		toExecute->OnSpellSuccess( caster, curSpell );
	}

	CChar *src = caster;

	if( curSpell > 63 && curSpell <= spellCount && spellCount <= 70 )
		Log( Dictionary->GetEntry( magic_table[curSpell].spell_name ), caster, NULL, "(Succeeded)");
	if( caster->IsNpc() || s != NULL && s->CurrentSpellType() != 2 )	// delete mana if NPC, s is -1 otherwise!
	{
		SubtractMana( caster, spells[curSpell].Mana() );
		if( spells[curSpell].Health() > 0 )
			SubtractHealth( caster, spells[curSpell].Health(), curSpell );
		SubtractStamina( caster, spells[curSpell].Stamina() );
	}
	if( s != NULL && s->CurrentSpellType() == 0 && !caster->IsNpc() ) 
		DelReagents( caster, spells[curSpell].Reagants() );
	
	if( spells[curSpell].AggressiveSpell() && !region[caster->GetRegion()]->CanCastAggressive() )
	{
		sysmessage( s, 709 );
		return;
	}

	UI16 tScript = 0xFFFF;
	cScript *tScriptExec = NULL;
	if( spells[curSpell].RequireTarget() )					// target spells if true
	{
		CItem *i;
		if( spells[curSpell].TravelSpell() )				// travel spells.... mark, recall and gate
		{
			// (Abaddon) Region checks
			cTownRegion *ourReg = region[caster->GetRegion()];
			if( (curSpell == 45 && !ourReg->CanMark()) || (curSpell == 52 && !ourReg->CanGate()) || (curSpell == 32 && !ourReg->CanRecall()) )
			{
				sysmessage( s, 705 );
				return;
			}
			// mark, recall and gate go here
			i = calcItemObjFromSer( s->GetDWord( 7 ) );
			if( i != NULL )
			{
				if( i->GetCont() != INVALIDSERIAL || LineOfSight( s, caster, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
				{
					if( i->GetType() == 50 )
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
							tScriptExec = Trigger->GetScript( tScript );
							if( tScriptExec != NULL )
							{
								tScriptExec->OnSpellTarget( i, caster, curSpell );
							}
							(*((MAGIC_ITEMFUNC)magic_table[curSpell-1].mag_extra))( s, caster, i );
							break;
						default:
							Console.Error( 2, " Unknown Travel spell %i, magic.cpp", curSpell );
							break;
						}
					}
					else
						sysmessage( s, 710 );
				}
			}
			else
				sysmessage( s, 710 );
			return;
		}
		
		if( spells[curSpell].RequireCharTarget() )
		{
			// TARGET CALC HERE
			CChar *c;
			if( !caster->IsNpc() )
				c = calcCharObjFromSer( s->GetDWord( 7 ) );
			else
				c = &chars[caster->GetTarg()];

			if( c != NULL ) // we have to have targetted a person to kill them :)
			{
				if( getDist( c, caster ) > cwmWorldState->ServerData()->GetCombatMaxSpellRange() )
				{
					sysmessage( s, 712 );
					return;
				}
				if( LineOfSight( s, caster, c->GetX(), c->GetY(), c->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
				{
					if( spells[curSpell].AggressiveSpell() )
					{
						if( !region[caster->GetRegion()]->CanCastAggressive() )
						{
							sysmessage( s, 709 );
							return;
						}
						if( c->GetNPCAiType() == 17 )
						{
							sysmessage( s, 713 );
							return;
						}
						npcAttackTarget( c, caster );
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
						{
							playSound( c, curSpell );
							doMoveEffect( curSpell, c, src );
							doStaticEffect( c, curSpell );
						}
						else
						{
							playSound( src, curSpell );
							doMoveEffect( curSpell, c, src );
							doStaticEffect( c, curSpell );
						}
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
						tScriptExec = Trigger->GetScript( tScript );
						if( tScriptExec != NULL )
						{
							tScriptExec->OnSpellTarget( c, caster, curSpell );
						}
						(*((MAGIC_CHARFUNC)magic_table[curSpell-1].mag_extra))( s, caster, c, src );
						break;
					case 66: // Cannon Firing
						if( CheckParry( c, 6 ) )
							MagicDamage( c, caster->GetSkill( TACTICS )/50, caster );
						else
							MagicDamage( c, caster->GetSkill( TACTICS )/25, caster );
						break; 
					default:
						Console.Error( 2, " Unknown CharacterTarget spell %i, magic.cpp", curSpell );
						break;
					}
				}
				else
					sysmessage( s, 714 );
			}
			else
				sysmessage( s, 715 );
		}
		else if( spells[curSpell].RequireLocTarget() )
		{
			if( !caster->IsNpc() )
			{
				x = s->GetWord( 11 );
				y = s->GetWord( 13 );
				z = s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) );
				if( s->GetDWord( 7 ) != 0 )
				{	// we targetted something not a location!
					sysmessage( s, 716 );
					return;
				}
			}
			else
			{
				CHARACTER defender = caster->GetTarg();
				x = chars[defender].GetX();
				y = chars[defender].GetY();
				z = chars[defender].GetZ();
			}

			if( !caster->IsNpc() && s != NULL && s->GetWord( 11 ) != INVALIDSERIAL )
			{
				
				if( LineOfSight( s, caster, x, y, z, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) || caster->IsGM() )
				{
					if( spells[curSpell].FieldSpell() )
					{
						UI08 j = getFieldDir( caster, x, y );
						playSound( src, curSpell );
						doStaticEffect( src, curSpell );
						(*((MAGIC_FIELDFUNC)magic_table[curSpell-1].mag_extra))( s, caster, j, x, y, z );
					}
					else
					{
						playSound( src, curSpell );
						doStaticEffect( src, curSpell );
						switch( curSpell )
						{
						case 22:// Teleport
						case 25://Arch Cure
						case 26://Arch Protection
						case 33:// Blade Spirits
						case 34:// Dispel Field
						case 46:// Mass curse
						case 48: 
						case 49: ///////// Chain Lightning ///////////
						case 54://Mass Dispel
						case 55://Meteor Swarm
						case 58:// Energy Vortex
							(*((MAGIC_LOCFUNC)magic_table[curSpell-1].mag_extra))( s, caster, x, y, z );
							break;
						default:
							Console.Error( 2, " Unknown LocationTarget spell %i", curSpell );
							break;
						}
					}					
				}
			} 
			else 
				sysmessage( s, 717 );
		}
		else if( spells[curSpell].RequireItemTarget() )
		{
			// CItem *target
			i = calcItemObjFromSer( s->GetDWord( 7 ) );
			if( i != NULL )
			{
				if( ( i->GetCont() != INVALIDSERIAL && getPackOwner( i ) != caster ) || ( getDist( caster, i ) > cwmWorldState->ServerData()->GetCombatMaxSpellRange() ) )
				{
					sysmessage( s, 718 );
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
						(*((MAGIC_ITEMFUNC)magic_table[curSpell-1].mag_extra))( s, caster, i );
						break;
					default:
						Console.Error( 2, " Unknown ItemTarget spell %i, magic.cpp", curSpell );
						break;
					}
				}
				else
					sysmessage( s, 714 );
			}
			else
				sysmessage( s, 711 );
		}
		else
			sysmessage( s, 720 );
		return;
	}
	else
	{
		// non targetted spells
		playSound( src, curSpell );
		doStaticEffect( src, curSpell );
		switch( curSpell )
		{
		case 2:		// Create Food
		case 40:	// Summon Monster	// summon monster is 40, not 33
		case 35:
		case 36:	// Magic Reflection
		case 56:
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
			Console.Error( 2, " Unknown NonTarget spell %i, magic.cpp", curSpell );
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
	
	int i = 0;
	bool done = false;
	int moveFX[5];
	int sFX[2];
	int stFX[4];
	char sect[512];
	const char *tag = NULL;
	const char *data = NULL;
	spellCount = 1;
	while( !done )
	{
		sprintf( sect, "SPELL %d", spellCount );
		ScriptSection *tSect = FileLookup->FindEntry( sect, spells_def );
		if( tSect == NULL )
			done = true;
		else
			spellCount++;
	}
	
	--spellCount; // spell count will be one too high after that loop - fur 
    
	if( spellCount > SPELL_MAX )
	{ 
		Console.Error( 3, " Too many spells (%d) in spells.dfn, ones after %d will be ignored.", spellCount, SPELL_MAX ); 
		spellCount = SPELL_MAX; 
	}
	ScriptSection *SpellLoad = NULL;
	
	for( i = 1; i <= spellCount; i++ )
	{
		sprintf(sect, "SPELL %d", i);
		SpellLoad = FileLookup->FindEntry( sect, spells_def );
		spells[i].Enabled( false );
		moveFX[0] = moveFX[1] = moveFX[2] = moveFX[3] = moveFX[4] = -1;
		sFX[0] = sFX[1] = -1;
		stFX[0] = stFX[1] = stFX[2] = stFX[3] = -1;
		reag_st *mRegs = spells[i].ReagantsPtr();
		for( tag = SpellLoad->First(); !SpellLoad->AtEnd(); tag = SpellLoad->Next() )
		{
			data = SpellLoad->GrabData();
			switch( tag[0] )
			{
			case 'a':
			case 'A':
				if( !strcmp("ACTION", tag ) ) 
					spells[i].Action( (SI32)makeNum( data ) );
				else if( !strcmp("ASH", tag ) ) 
					mRegs->ash = makeNum( data );
				break;

			case 'c':
			case 'C':
				if( !strcmp("CIRCLE",  tag ) ) 
					spells[i].Circle( (UI08)makeNum( data ) );
				break;
			
			case 'd':
			case 'D':
				if( !strcmp("DELAY", tag ) ) 
					spells[i].Delay( (SI32)makeNum( data ) );
				else if( !strcmp("DRAKE", tag ) ) 
					mRegs->drake   = makeNum( data );
				break;

			case 'e':
			case 'E':
				if( !(strcmp("ENABLE",  tag ) ) )   // presence of enable is enough to enable it
					spells[i].Enabled( makeNum( data ) != 0 );
				break;

			case 'f':
			case 'F':
				if( !strcmp( "FLAGS", tag ) )
				{
					UI32 fl1, fl2;
					sscanf( data, "%x%x", &fl1, &fl2 );
					spells[i].Flags( (UI08)fl1, (UI08)fl2 );
				}
				break;

			case 'g':
			case 'G':
				if( !strcmp("GARLIC", tag ) ) 
					mRegs->garlic  = makeNum( data );
				else if( !strcmp("GINSENG", tag ) ) 
					mRegs->ginseng = makeNum( data );
				break;

			case 'h':
			case 'H':
				if( !strcmp("HISKILL", tag ) ) 
					spells[i].HighSkill( makeNum( data ) );
				else if( !strcmp( "HEALTH", tag ) )
					spells[i].Health( makeNum( data ) );
				break;

			case 'l':
			case 'L':
				if( !strcmp("LOSKILL", tag ) ) 
					spells[i].LowSkill( makeNum( data ) );
				break;

			case 'm':
			case 'M':
				if( !strcmp("MANA", tag ) ) 
					spells[i].Mana( makeNum( data ) );
				else if( !strcmp("MANTRA", tag ) ) 
					spells[i].Mantra( data );
				else if( !strcmp("MOSS", tag ) ) 
					mRegs->moss = makeNum( data );
				else if( !strcmp("MOVEFX", tag ) ) 
				{
					sscanf( data, "%x%x%x%x%x", &moveFX[0], &moveFX[1], &moveFX[2], &moveFX[3], &moveFX[4] );
					CMagicMove *mv = spells[i].MoveEffectPtr();
					mv->Effect( moveFX[0], moveFX[1] );
					mv->Speed( moveFX[2] );
					mv->Loop( moveFX[3] );
					mv->Explode( moveFX[4] );
				}
				break;

			case 'p':
			case 'P':
				if( !strcmp("PEARL", tag ) ) 
					mRegs->pearl = makeNum( data );
				break;

			case 's':
			case 'S':
				if( !strcmp("SHADE", tag ) ) 
					mRegs->shade = makeNum( data );
				else if( !strcmp("SILK", tag ) ) 
					mRegs->silk = makeNum( data );
				else if( !strcmp("SOUNDFX", tag ) ) 
				{
					sscanf( data, "%x%x", &sFX[0], &sFX[1] );
					CMagicSound *snd = spells[i].SoundEffectPtr();
					snd->Effect( sFX[0], sFX[1] );
				}
				else if( !strcmp("STATFX", tag ) ) 
				{
					sscanf( data, "%x%x%x%x", &stFX[0], &stFX[1], &stFX[2], &stFX[3] );
					CMagicStat *stat = spells[i].StaticEffectPtr();
					stat->Effect( stFX[0], stFX[1] );
					stat->Speed( stFX[2] );
					stat->Loop( stFX[3] );
				}
				else if( !strcmp("SCLO", tag ) ) 
					spells[i].ScrollLow( makeNum( data ) );
				else if( !strcmp("SCHI", tag ) ) 
					spells[i].ScrollHigh( makeNum( data ) );
				else if( !strcmp( "STAMINA", tag ) )
					spells[i].Stamina( makeNum( data ) );
				break;

			case 't':
			case 'T':
				if( !strcmp("TARG", tag ) ) 
					spells[i].StringToSay( data );
				break;
			}
		}
	}
}


void cMagic::DelReagents( CChar *s, reag_st reags )
{
	if( s->NoNeedReags() ) 
		return;
	deleQuan( s, 0x0F7A, reags.pearl );
	deleQuan( s, 0x0F7B, reags.moss );
	deleQuan( s, 0x0F84, reags.garlic );
	deleQuan( s, 0x0F85, reags.ginseng );
	deleQuan( s, 0x0F86, reags.drake );
	deleQuan( s, 0x0F88, reags.shade );
	deleQuan( s, 0x0F8C, reags.ash );
	deleQuan( s, 0x0F8D, reags.silk );
}

void cMagic::playSound( CChar *source, int num )
{
	CMagicSound temp = spells[num].SoundEffect();
	
	if( temp.Effect() != INVALIDID )
		soundeffect( source, temp.Effect() );
}

void cMagic::doStaticEffect( CChar *source, int num )
{
	CMagicStat temp = spells[num].StaticEffect();
	if( temp.Effect() != INVALIDID )
		staticeffect( source, temp.Effect(), temp.Speed(), temp.Loop() );
}

void cMagic::doMoveEffect( int num, cBaseObject *target, CChar *source )
{
	CMagicMove temp = spells[num].MoveEffect();
	
	if( temp.Effect() != INVALIDID )
		movingeffect( source, target, temp.Effect(), temp.Speed(), temp.Loop(), ( temp.Explode() == 1 ) );
}

void cMagic::PolymorphMenu( cSocket *s, int gmindex )
{
	int total, i;
	char lentext;
	char sect[512];
	char gmtext[30][257];
	int gmid[30];
	int gmnumber=0,dummy=0;
	
	sprintf( sect, "POLYMORPHMENU %i", gmindex );
	ScriptSection *polyStuff = FileLookup->FindEntry( sect, polymorph_def );
	if( polyStuff == NULL )
		return;
	const char *tag = NULL;
	const char *data = NULL;
	char fullLine[128];
	tag = polyStuff->First(); data = polyStuff->GrabData();
	sprintf( fullLine, "%s %s", tag, data );
	lentext = sprintf(gmtext[0], "%s", fullLine );
	for( tag = polyStuff->Next(); !polyStuff->AtEnd(); tag = polyStuff->Next() )
	{
		data = polyStuff->GrabData();
		if( strcmp( "POLYMORPHID", tag ) )
		{
			gmnumber++;
			gmid[gmnumber] = makeNum( tag );
			strcpy( gmtext[gmnumber], data );
			tag = polyStuff->Next();
			data = polyStuff->GrabData();
		}
		
	}
	// reading polymorph duration time ...
	sprintf( sect, "POLYMORPHDURATION %i", dummy );
	polyStuff = FileLookup->FindEntry( sect, polymorph_def );
	if( polyStuff == NULL )
		return;
	CChar *mChar = s->CurrcharObj();
	for( tag = polyStuff->First(); !polyStuff->AtEnd(); data = polyStuff->Next() )
	{
		data = polyStuff->GrabData();
		polyduration = makeNum( tag );
		tag = polyStuff->Next(); data = polyStuff->GrabData();
	}
	
	total=9+1+lentext+1;
	for( i = 1; i <= gmnumber; i++ ) 
		total += 4 + 1 + strlen( gmtext[i] );
	gmprefix[1] = (UI08)(total>>8);
	gmprefix[2] = (UI08)(total%256);
	gmprefix[3] = mChar->GetSerial( 1 );
	gmprefix[4] = mChar->GetSerial( 2 );
	gmprefix[5] = mChar->GetSerial( 3 );
	gmprefix[6] = mChar->GetSerial( 4 );
	gmprefix[7] = (UI08)(gmindex>>8);
	gmprefix[8] = (UI08)(gmindex%256);
	s->Send( gmprefix, 9 );
	s->Send( &lentext, 1 );
	s->Send( gmtext[0], lentext );
	lentext = gmnumber;
	s->Send( &lentext, 1 );
	for( i = 1; i <= gmnumber; i++ )
	{
		gmmiddle[0]=gmid[i]>>8;
		gmmiddle[1]=gmid[i]%256;
		s->Send( gmmiddle, 4 );
		lentext = strlen( gmtext[i] );
		s->Send( &lentext, 1 );
		s->Send( gmtext[i], lentext );
	}
}

void cMagic::Polymorph( cSocket *s, int gmindex, int creaturenumber)
{
	int i = 0, k = 0;
	UI08 id1, id2;
	
	char sect[512];
	const char *tag = NULL;
	const char *data = NULL;
	ScriptSection *polyStuff = FileLookup->FindEntry( sect, polymorph_def );
	if( polyStuff == NULL )
		return;
	tag = polyStuff->First();
	for( tag = polyStuff->Next(); !polyStuff->AtEnd() && i < creaturenumber; tag = polyStuff->Next() )
	{
		if( strcmp( "POLYMORPHID", tag ) )
			i++;
	}
	tag = polyStuff->Next();
	data = polyStuff->GrabData();
	k = makeNum( data );
	
	id1 = (UI08)(k>>8);
	id2 = (UI08)(k%256);
	CChar *mChar = s->CurrcharObj();
	soundeffect( mChar, 0x020F );
	tempeffect( mChar, mChar, 18, id1, id2, 0 );
	
	mChar->Teleport();
	mChar->IsPolymorphed( true );
}

void cMagic::Heal( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	
	if( i != NULL )
	{
		playSound( mChar, 4 );
		doStaticEffect( i, 4 );
		i->SetHP( i->GetMaxHP() );
		updateStats( i, 0 );
	}
	else
		sysmessage( s, 721 );
}

// only used for the /recall command
void cMagic::Recall( cSocket *s )
{
	// Targeted item
	CChar *mChar = s->CurrcharObj();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL ) 
		return;
	if( i->GetMoreX() <= 200 && i->GetMoreY() <= 200 )
		sysmessage( s, 431 );
	else
	{
		mChar->SetLocation( i->GetMoreX(), i->GetMoreY(), i->GetMoreZ() );
		mChar->Teleport();
		sysmessage( s, 682 );
	}
}

// only used for the /mark command
void cMagic::Mark( cSocket *s )
{
	// Targeted item
	CChar *mChar = s->CurrcharObj();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	
	i->SetMoreX( mChar->GetX() );
	i->SetMoreY( mChar->GetY() );
	i->SetMoreZ( mChar->GetZ() );

	char tempname[512];

	if( region[mChar->GetRegion()]->GetName()[0] != 0 )
		sprintf( tempname, Dictionary->GetEntry( 684 ),region[mChar->GetRegion()]->GetName() );
	else 
		strcpy( tempname, Dictionary->GetEntry( 685 ) );
	i->SetName( tempname );
	sysmessage( s, 686 );
}

// only used for the /gate command
void cMagic::Gate( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	
	if( i->GetMoreX() <= 200 && i->GetMoreY() <= 200 )
		sysmessage( s, 679 );
	else
		SpawnGate( s, mChar, mChar->GetX(), mChar->GetY(), mChar->GetZ(), i->GetMoreX(), i->GetMoreY(), i->GetMoreZ() );
}

void cMagic::Log( const char *spell, CChar *player1, CChar *player2, const char *extraInfo )
// PRE:		Commands is initialized, toLog is not NULL
// POST:	Writes out toLog to a file
{
	if( spell == NULL || player1 == NULL )
		return;
	char logName[MAX_PATH];
	sprintf( logName, "%s/logs/spell.log", cwmWorldState->ServerData()->GetRootDirectory() );
	FILE *spellLog = fopen( logName, "a+" );
	if( spellLog == NULL )
	{
		Console.Error( 1, "Unable to open spell log file %s for appending!", logName );
		return;
	}
	char dateTime[1024];
	time_t ltime;
	time( &ltime );
	char *t = ctime( &ltime );
	// just to be paranoid and avoid crashing
	if( NULL == t )
		t = "";
	else
	{
		// some ctime()s like to stick \r\n on the end, we don't want that
		for( int end = strlen(t) - 1; end >= 0 && isspace(t[end]); --end )
			t[end] = '\0';
	}
	safeCopy( dateTime, t, 1024 );

	if( player2 != NULL )
		fprintf( spellLog, "[%s] %s (serial: %i ) cast spell <%s> on player %s (serial: %i ) Extra Info: %s\n", dateTime, player1->GetName(), player1->GetSerial(), spell, player2->GetName(), player2->GetSerial(), extraInfo );
	else
		fprintf( spellLog, "[%s] %s (serial: %i ) cast spell <%s> Extra Info: %s\n", dateTime, player1->GetName(), player1->GetSerial(), spell, extraInfo );
	fclose( spellLog );
}
