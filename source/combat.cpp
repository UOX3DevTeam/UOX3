#include "uox3.h"

#include "cGuild.h"
#include "combat.h"
#include "townregion.h"
#include "cRaces.h"
#include "skills.h"
#include "cMagic.h"
#include "cEffects.h"
#include "packets.h"

#undef DBGFILE
#define DBGFILE "combat.cpp"
#define SWINGAT (UI32)1.75 * CLOCKS_PER_SEC

const UI08 BODYPERCENT = 0;
const UI08 ARMSPERCENT = 1;
const UI08 HEADPERCENT = 2;
const UI08 LEGSPERCENT = 3;
const UI08 NECKPERCENT = 4;
const UI08 OTHERPERCENT = 5;
const UI08 TOTALTARGETSPOTS = 6;

const UI08 LOCPERCENTAGES[TOTALTARGETSPOTS] = { 44, 14, 14, 14, 7, 7 };

void npcSimpleAttackTarget( CChar *defender, CChar *attacker );

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 cCombat::getBowType( CChar *i )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find if character is holding a Bow or XBow
//o---------------------------------------------------------------------------o
UI08 cCombat::getBowType( CChar *i )
{
	if( i == NULL )
		return 0;

	CItem *getBow = i->GetItemAtLayer( 1 );
	if( getBow == NULL )
		getBow = i->GetItemAtLayer( 2 );

	if( getBow == NULL )
		return 0;

	switch( getWeaponType( getBow ) )
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
//|	Function	-	cCombat::ItemCastSpell( cSocket *mSock, CChar *c, CItem *i )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle items that cast spells during combat
//o---------------------------------------------------------------------------o
void cCombat::ItemCastSpell( cSocket *mSock, CChar *c, CItem *i )
{
	if( c == NULL || i == NULL || mSock == NULL ) 
		return;

	CChar *ourChar = mSock->CurrcharObj();
	UI16 spellnum = static_cast<UI16>(((i->GetMoreX()*8)-8) + i->GetMoreY());

	if( i->GetType() != 15 || i->GetMoreZ() <= 0 ) 
		return;
	
	switch( spellnum )
	{
	case 1:		
	case 3:		
	case 5:		
	case 8:		
	case 18:	
	case 22:	
	case 27:	
	case 30:	
	case 37:	
	case 38:	
	case 42:	
	case 43:	
	case 51:
		CastSpell( ourChar, c, spellnum );
		break;
	default:
		Effects->staticeffect( ourChar, 0x3735, 0, 30 );
		Effects->PlaySound( ourChar, 0x005C );
		break;
	}

	i->SetMoreZ( i->GetMoreZ() - 1 );
	if( i->GetMoreZ() == 0 )
		sysmessage( mSock, (SI32)0 );
}

//o--------------------------------------------------------------------------
//|	Function		-	InvalidateAttacker( CChar *attack )
//|	Date			-	3rd July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Resets the attacker attack so that it cancels attack 
//|						setup.
//o--------------------------------------------------------------------------
void InvalidateAttacker( CChar *attack )
{
	if( attack == NULL )
		return;

	if( attack->GetNPCAiType() == 4 )
	{
		attack->SetSummonTimer( BuildTimeValue( 20 ) );
		attack->SetNpcWander( 2 );
		attack->SetNpcMoveTime( BuildTimeValue(static_cast<R32>( cwmWorldState->ServerData()->GetNPCSpeed() )) );
		npcTalkAll( attack, 281, false );
	}
	attack->SetTarg( INVALIDSERIAL );
	CHARACTER attAttacker = attack->GetAttacker();
	if( attAttacker != INVALIDSERIAL && attAttacker < cwmWorldState->GetCMem() )
	{
		chars[attAttacker].SetAttackFirst( false );
		chars[attAttacker].SetAttacker( INVALIDSERIAL );
	}
	attack->SetAttacker( INVALIDSERIAL );
	attack->SetAttackFirst( false );
	if( attack->IsAtWar() && attack->IsNpc() && !attack->IsDead() && attack->GetNPCAiType() != 17 ) 
		npcToggleCombat( attack );
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
void AdjustRaceDamage( CChar *defend, CItem *weapon, SI16 &bDamage )
{
	if( defend == NULL || weapon == NULL )
		return;

	if( weapon->GetRace() == defend->GetRace() ) 
		bDamage *= 2;
	CRace *rPtr = Races->Race( defend->GetRace() );
	if( rPtr != NULL )
	{
		if( weapon->LightDamage() && rPtr->AffectedByLight() )
			bDamage *= 2;
		if( weapon->RainDamage() && rPtr->AffectedByRain() )
			bDamage *= 2;
		if( weapon->ColdDamage() && rPtr->AffectedByCold() )
			bDamage *= 2;
		if( weapon->HeatDamage() && rPtr->AffectedByHeat() )
			bDamage *= 2;
		if( weapon->LightningDamage() && rPtr->AffectedByLightning() )
			bDamage *= 2;
		if( weapon->SnowDamage() && rPtr->AffectedBySnow() )
			bDamage *= 2;
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	SI08 DoHitMessage( CChar *defend, CChar *attack, cSocket *mSock, SI16 damage )
//|	Date			-	3rd July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Prints out the hit message (if enabled) and calculates
//|						where on the body the person was hit and returns that
//o--------------------------------------------------------------------------
SI08 DoHitMessage( CChar *defend, CChar *attack, cSocket *mSock, SI16 damage )
{
	if( defend == NULL || attack == NULL || defend == attack )
		return 0;

	SI08 hitPos = RandomNum( 0, 99 ); // Determine area of Body Hit
	for( UI08 t = BODYPERCENT; t < TOTALTARGETSPOTS; t++ )
	{
		hitPos -= LOCPERCENTAGES[t];
		if( hitPos < 0 ) 
		{
			hitPos = t + 1;
			break;
		}
	}
	if( cwmWorldState->ServerData()->GetCombatDisplayHitMessage() )
	{
		if( !defend->IsNpc() && mSock != NULL && damage > 1 )
		{
			UI08 randHit = RandomNum( 0, 2 );
			switch( hitPos )
			{
			case 1:	// Body
				switch ( randHit )
				{
				case 1:
					if( damage < 10  ) 
						sysmessage( mSock, 284, attack->GetName() );
					else
						sysmessage( mSock, 285, attack->GetName() );
					break;
				case 2:
					if( damage < 10 ) 
						sysmessage( mSock, 286, attack->GetName() );
					else
						sysmessage( mSock, 287, attack->GetName() );
					break;
				default:
					if( damage < 10 ) 
						sysmessage( mSock, 288, attack->GetName() );
					else
						sysmessage( mSock, 289, attack->GetName() );
					break;
				}
				break;
			case 2:	// Arms
				switch( randHit )
				{
				case 1:		sysmessage( mSock, 290, attack->GetName() );		break;
				case 2:		sysmessage( mSock, 291, attack->GetName() );		break;
				default:	sysmessage( mSock, 292, attack->GetName() );		break;
				}
				break;
			case 3:	// Head
				switch( randHit )
				{
				case 1:
					if( damage <  10 ) 
						sysmessage( mSock, 293, attack->GetName() );
					else
						sysmessage( mSock, 294, attack->GetName() );
					break;
				case 2:
					if( damage <  10 ) 
						sysmessage( mSock, 295, attack->GetName() );
					else 
						sysmessage( mSock, 296, attack->GetName() );
					break;
				default:
					if( damage <  10 ) 
						sysmessage( mSock, 297, attack->GetName() );
					else
						sysmessage( mSock, 298, attack->GetName() );
					break;
				}
				break;
			case 4:	// Legs
				switch( randHit )
				{
				case 1:		sysmessage( mSock, 299, attack->GetName() );		break;
				case 2:		sysmessage( mSock, 300, attack->GetName() );		break;
				default:	sysmessage( mSock, 301, attack->GetName() );		break;
				}
				break;
			case 5:	// Neck
				sysmessage( mSock, 302, attack->GetName() );
				break;
			case 6:	// Hands
				switch( randHit )
				{
				case 1:		sysmessage( mSock, 303, attack->GetName() );		break;
				case 2:		sysmessage( mSock, 304, attack->GetName() );		break;
				default:	sysmessage( mSock, 305, attack->GetName() );		break;
				}
				break;
			}
		}
	}
	return hitPos;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cCombat::CombatHit( CChar *attack, CChar *defend, SI32 weaponType )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle combat hit (or miss), and calculate damage
//|					PARAM WARNING: weaponType is an unreferenced parameters
//|					Weapontype was meant for race and other weapon weaknesses
//o---------------------------------------------------------------------------o
void cCombat::CombatHit( CChar *attack, CChar *defend, SI32 weaponType )
{
	if( defend == NULL || attack == NULL || defend == attack ) 
		return;

	attack->SetSwingTarg( INVALIDSERIAL );
	if( defend->IsDead() || defend->IsInvulnerable() || defend->GetNPCAiType() == 17 )
	{
		InvalidateAttacker( attack );
		return;
	}

	cSocket *aSock = calcSocketObjFromChar( attack ), *dSock = calcSocketObjFromChar( defend );
 	UI08 getFightSkill = getCombatSkill( attack );

	bool los = false;
	if( getDist( attack, defend ) < 2 )
		los = true;
	else if( getFightSkill == ARCHERY )
		los = LineOfSight( aSock, attack, defend->GetX(), defend->GetY(), defend->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING );

	if( !los )
		return;

	UI08 bowType = getBowType( attack );
	UI16 attSkill = attack->GetSkill( getFightSkill );

	bool skillPassed = false;
	if( Skills->CheckSkill( attack, getFightSkill, 0, 1000 ) )
	{
		UI16 getAttackSkill = ( attSkill + attack->GetSkill( TACTICS ) ) / 2;
		UI16 getDefSkill = defend->GetSkill( TACTICS );
		skillPassed = ( !RandomNum( 0, 5 ) || RandomNum( (int)(getAttackSkill / 1.25 ), getAttackSkill ) >= RandomNum( (int)( getDefSkill / 1.25), getDefSkill ) );
	}
	if( !skillPassed )
	{
		if( getFightSkill == ARCHERY && !RandomNum( 0, 2 ) )
		{
			CItem *c = Items->SpawnItem( NULL, attack, 1, "#", true, 0x0E75, 0, false, false );
			if( c != NULL )
			{
				if( bowType == BOWS )
					c->SetID( 0x0F3F );
				else 
					c->SetID( 0x1BFB );

				c->SetLocation( defend );
				RefreshItem( c );
			}
		}
		doMissedSoundEffect( attack );
		if( defend->GetTarg() == INVALIDSERIAL )
		{//if the defender is swung at, and they don't have a target already, set this as their target
			npcSimpleAttackTarget( attack, defend );
			attack->SetTimeout( cwmWorldState->GetUICurrentTime() + ((( 100 - min( attack->GetDexterity(), 100 ) ) * CLOCKS_PER_SEC ) /25 ) + ( 1 * CLOCKS_PER_SEC ) );
		}
	} 
	else 
	{
		CItem *weapon = getWeapon( attack );

		Skills->CheckSkill( attack, TACTICS, 0, 1000 );

		if( defend->GetID() == 0x0191 )
			Effects->PlaySound( defend, 0x014B );
		else if( defend->GetID() == 0x0190 ) 
			Effects->PlaySound( defend, 0x0156 );
		else
			Effects->playMonsterSound( defend, defend->GetID(), SND_DEFEND );

		if( attack->GetPoison() && defend->GetPoisoned() < attack->GetPoison() )
		{
			if( ( getFightSkill == FENCING || getFightSkill == SWORDSMANSHIP ) && !RandomNum( 0, 2 ) )
			{
				defend->SetPoisoned( attack->GetPoison() );
				defend->SetPoisonTime( BuildTimeValue(static_cast<R32> (40 / defend->GetPoisoned() )) ); // a lev.1 poison takes effect after 40 secs, a deadly pois.(lev.4) takes 40/4 secs - AntiChrist
				defend->SetPoisonWearOffTime( defend->GetPoisonTime() + ( CLOCKS_PER_SEC * cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) ) ); //wear off starts after poison takes effect - AntiChrist
				if( dSock != NULL )
				{
					defend->SendToSocket( dSock, true, defend );
					sysmessage( dSock, 282 );
				}
			}
		}			

		if( getFightSkill != WRESTLING )
			ItemSpell( attack, defend );
		SI16 BaseDamage = calcAtt( attack );
		if( BaseDamage == -1 )  // No damage if weapon breaks
			return;
		// Race Dmg Modification: Bonus percentage.
		SI32 RaceDamage = Races->DamageFromSkill( getFightSkill, attack->GetRace() );
		if( RaceDamage != 0 )
			BaseDamage += (SI16)( (R32)BaseDamage * ( (R32)RaceDamage / 1000 ) );

		// Check to see if weapons affect defender's race.
		if( getFightSkill != WRESTLING )
			AdjustRaceDamage( defend, weapon, BaseDamage );

		R32 multiplier;
		SI16 damage;

		// Strength Damage Bonus, +20% Damage
		multiplier = static_cast<R32>( ( ( min( attack->GetStrength(), 200 ) * 20 ) / 100 ) / 100 ) + 1;
		damage = (SI16)(BaseDamage * multiplier);

		// Tactics Damage Bonus, % = ( Tactics + 50 )
		multiplier = static_cast<R32>( ( attack->GetSkill( TACTICS ) + 500 ) / 1000 );
		damage += (SI16)(BaseDamage * multiplier);

		if( defend->IsNpc() ) // Anatomy PvM damage Bonus, % = ( Anat / 5 )
			multiplier = static_cast<R32>( ( attack->GetSkill( ANATOMY ) / 5 ) / 1000 );
		else // Anatomy PvP damage Bonus, % = ( Anat / 2.5 )
			multiplier = static_cast<R32>( ( attack->GetSkill( ANATOMY ) / 2.5 ) / 1000 );
		damage += (SI16)(BaseDamage * multiplier);

		// Lumberjacking Bonus ( 30% Damage at GM Skill )
		if( attack->GetSkill( LUMBERJACKING ) >= 1000 )
			damage += (SI16)(BaseDamage * .3);

  		// Defender Tactics Damage Modifier, -20% Damage
		multiplier = static_cast<R32>(1.0 - ( ( ( defend->GetSkill( TACTICS ) * 20 ) / 1000 ) / 100 ));
		damage = (SI16)(damage * multiplier);

		if( defend->GetTarg() == INVALIDSERIAL )
		{//if the defender is swung at, and they don't have a target already, set this as their target
			npcSimpleAttackTarget( attack, defend );
			attack->SetTimeout( cwmWorldState->GetUICurrentTime() + ((( 100 - min( attack->GetDexterity(), 100 ) ) * CLOCKS_PER_SEC ) /25 ) + ( 1 * CLOCKS_PER_SEC ) );
		}


		if( damage < 1 )
		{
			doMissedSoundEffect( attack );
			return;
		}

		// Check Shield Defense
		CItem *shield = getShield( defend );
		if( shield != NULL )
		{
			Skills->CheckSkill( defend, PARRYING, 0, 1000 );
			// Chance to block with Shield ( % = Skill / 2 ) 
			UI16 defendParry = defend->GetSkill( PARRYING );
			if( RandomNum( (int)( defendParry / 2 ), defendParry ) >= RandomNum( (int)( attSkill / 2 ), attSkill ) )
			{
				damage -= RandomNum( (int)( shield->GetDef() / 2 ), shield->GetDef() );
				if( !RandomNum( 0, 5 ) ) 
					shield->IncHP( -1 );
				if( shield->GetHP() <= 0 )
				{
					if( dSock != NULL )
						sysmessage( dSock, 283 );
					Items->DeleItem( shield );
				}
			}
		}

		// Armor / Skin Absorbtion
		UI16 getDef;
		SI08 hitMsg = DoHitMessage( defend, attack, dSock, damage );
		if( defend->isHuman() )
		{
			getDef = calcDef( defend, hitMsg, true );
			getDef = RandomNum( ((int)(getDef / 2)), getDef );
		}
		else if( defend->GetDef() > 0 )
			getDef = defend->GetDef();
		else
			getDef = 20;

		// Damage based on Attack Skill (Armor defends less against low-skill characters)
		damage -= (SI16)( ( getDef * attSkill ) / 750 );

		// Give them one last chance to do Damage
		if( damage <= 0 ) 
			damage = RandomNum( 0, 4 );

		if( !defend->IsNpc() ) 
			damage /= cwmWorldState->ServerData()->GetCombatNPCDamageRate(); // Rate damage against other players

		if( !attack->IsNpc() && weapon != NULL )
			ItemCastSpell( aSock, defend, weapon );

		if( damage > 0 )
		{
			// Interrupt Spells
			if( damage > 1 && !defend->IsNpc() )
			{
				if( defend->GetCasting() && dSock->CurrentSpellType() == 0 )
				{
					defend->StopSpell();
					defend->SetFrozen( false );
					sysmessage( dSock, 306 );
				}
			}
			// Reactive Armor
			if( defend->GetReactiveArmour() )
			{
				int damage1 = (int)( damage * ( defend->GetSkill( MAGERY ) / 2000.0 ) );
				defend->IncHP( -( damage - damage1 ) );
				if( defend->IsNpc() ) 
					damage1 *= cwmWorldState->ServerData()->GetCombatNPCDamageRate();
				attack->IncHP( -damage1 );
				Effects->staticeffect( defend, 0x374A, 0, 15 );
			}
			else 
				defend->IncHP( -damage );
			// Splitting NPC's
			UI08 splitnum;
			if( defend->GetSplit() > 0 && defend->GetHP() >= 1 )
			{
				if( RandomNum( 0, 100 ) <= defend->GetSplitChance() )
				{
					if( defend->GetSplit() == 1 ) 
						splitnum = 1;
					else 
						splitnum = RandomNum( 1, defend->GetSplit() );
					
					for( UI08 splitcount = 0; splitcount < splitnum; splitcount++ )
						Npcs->Split( defend );
				}
			}
		}

		if( !attack->IsNpc() || attack->isHuman() )
			doSoundEffect( attack, weapon );

		if( defend->GetHP() < 0 ) 
			defend->SetHP( 0 );
		updateStats( defend, 0 );
		if( defend->GetID() >= 0x0190 )
		{
			if( !defend->IsOnHorse() ) 
				npcAction( defend, 0x14 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCombat::DoCombat( CChar *attack )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle combat
//o---------------------------------------------------------------------------o
void cCombat::DoCombat( CChar *attack )
{
	if( attack == NULL )
	{
		Console.Log( "DoCombat() - Bad Attacker\n", "combat.log" );
		return;
	}
	if( attack->isFree() || attack->IsDead() || attack->IsInvulnerable() )
		return;

	CHARACTER dfnd = attack->GetTarg();
	if( dfnd == INVALIDSERIAL || dfnd >= cwmWorldState->GetCMem() )
	{
		if( attack->IsAtWar() )
		{
			npcToggleCombat( attack );
			attack->SetAttacker( INVALIDSERIAL );
			attack->SetAttackFirst( false );
		}
		return;
	}
	CChar *defend = &chars[dfnd];
	if( defend == NULL )
		return;

	if( ( defend->isFree() || defend->IsDead() || defend->IsInvulnerable() || defend->GetNPCAiType() == 17 ) ||
		( !attack->IsNpc() && !isOnline( attack ) ) || ( !defend->IsNpc() && !isOnline( defend ) ) )
	{
		InvalidateAttacker( attack );
		return;
	}

	UI16 playerDistance = getDist( attack, defend );

	bool LoS = false;
	if( playerDistance <= 1 && abs( attack->GetZ() - defend->GetZ() ) <= MAX_Z_STEP )
		LoS = true;
	else
		LoS = LineOfSight( NULL, attack, defend->GetX(), defend->GetY(), defend->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING );
	
	if( !LoS )
		return;

	CItem *weapon = getWeapon( attack );
	cSocket *aSock = calcSocketObjFromChar( attack );
	int getDelay = 0;

	if( !charInRange( attack, defend ) )
	{
		if( attack->GetNPCAiType() == 4 && region[attack->GetRegion()]->IsGuarded() )
		{
			attack->SetLocation( defend );
			attack->Teleport();
			Effects->PlaySound( attack, 0x01FE );
			Effects->staticeffect( attack, 0x372A, 0x09, 0x06 );
			npcTalkAll( attack, 1616, true );
			npcAttackTarget( defend, attack );
		}
		else
			InvalidateAttacker( attack );
	}
	else
	{
		if( TimerOk( attack ) && ( attack->IsNpc() || attack->GetNpcMoveTime() < cwmWorldState->GetUICurrentTime() ) )
		{
			UI08 bowType = 0;
			UI08 getFightSkill = getCombatSkill( attack );
			bool doHit = false;
			if( getFightSkill == ARCHERY )
			{
				if( playerDistance < 2 )
					return;
				bowType = getBowType( attack );
				if( attack->IsOnHorse() && !cwmWorldState->ServerData()->GetShootOnAnimalBack() )
				{//Why can't we fire while mounted?!?! That's retarded
					if( aSock != NULL )
						sysmessage( aSock, 308 );
					attack->SetWar( false );
					attack->SetAttacker( INVALIDSERIAL );
					attack->SetAttackFirst( false );
					return;
				}
				UI32 getArrowAmt;
				if( bowType == BOWS ) 
					getArrowAmt = GetAmount( attack, 0x0F3F );
				else 
					getArrowAmt = GetAmount( attack, 0x1BFB );

				if( getArrowAmt > 0 || attack->IsNpc() ) 
					doHit = true;
				else if( aSock != NULL ) 
					sysmessage( aSock, 309 );
			}
			else if( playerDistance < 2 ) 
				doHit = true;
			if( doHit )
			{
				if( attack->GetNpcWander() != 5 )
				{
					UI08 charDir = getCharDir( attack, defend->GetX(), defend->GetY() );
					if( attack->GetDir() != charDir && charDir < 8 )
					{
						attack->SetDir( charDir );
						attack->Teleport();
					}
				}
				if( abs( cwmWorldState->ServerData()->GetCombatAttackStamina() ) > 0 && attack->GetCommandLevel() < CNS_CMDLEVEL )
				{
					attack->SetStamina( attack->GetStamina() + cwmWorldState->ServerData()->GetCombatAttackStamina() );
					if( attack->GetStamina() > attack->GetMaxStam() ) 
						attack->SetStamina( attack->GetMaxStam() );
					if( attack->GetStamina() == 0xFFFF ) 
						attack->SetStamina( 0 );
      				updateStats( attack, 2 );
				}
				UI16 charID = attack->GetID();
				if( charID < 0x0190 )
				{
					UI08 aa = RandomNum( 4, 6 ); // some creatures dont have animation #4
					if( creatures[charID].AntiBlink() )
					{
						aa++;
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
					npcAction( attack, aa ); 
					if( RandomNum( 0, 4 ) ) 
						Effects->playMonsterSound( attack, attack->GetID(), SND_ATTACK );
				}
				else if( attack->IsOnHorse() )
					CombatOnHorse( attack );
				else
					CombatOnFoot( attack );

				/*if( ( playerDistance < 2 || getFightSkill == ARCHERY ) &&  attack->GetNPCAiType() != 4 )
				{
					if( defend->GetTarg() == INVALIDSERIAL )
						npcSimpleAttackTarget( attack, defend );
				}*/

				if( getFightSkill == ARCHERY )
				{
					if( bowType == BOWS )
					{
						DeleteQuantity( attack, 0x0F3F, 1 );
						Effects->movingeffect( attack, defend, 0x0F42, 0x08, 0x00, 0x00 );
					}
					else
					{
						DeleteQuantity( attack, 0x1BFB, 1 );
						Effects->movingeffect( attack, defend, 0x1BFE, 0x08, 0x00, 0x00 );
					}
					CombatHit( attack, defend );
				}
				else 
					attack->SetSwingTarg( calcCharFromSer( defend->GetSerial() ) ); 
  			}
			// attack speed should be determined here.
			// attack speed = 15000 / ((DEX+100) * weapon speed)
			if( weapon != NULL )
			{
			    if( weapon->GetSpeed() <= 0 ) 
					weapon->SetSpeed( 35 );
				getDelay = (15000 / ((min( attack->GetDexterity(), 100 ) + 100 ) * weapon->GetSpeed() )*CLOCKS_PER_SEC); //Calculate combat delay
			}
			else
			{
				int getOffset;
				if( attack->GetSkill( WRESTLING ) < 800 )
					getOffset = (((int)(attack->GetSkill( WRESTLING ) / 200 )) * 5) + 30;
				else
					getOffset = 50;
				getDelay = (15000 / ((min( attack->GetDexterity(), 100 ) + 100) * getOffset)*CLOCKS_PER_SEC);
			}
			attack->SetTimeout( cwmWorldState->GetUICurrentTime() + getDelay );
			if( getFightSkill != ARCHERY ) 
				CombatHit( attack, defend );
		}
	}	

	if( defend->GetHP() < 1 )
	{
		Kill( attack, defend );
		return;
	}

	if( attack->IsNpc() && attack->GetMana() > 0 && attack->GetSpaTimer() <= cwmWorldState->GetUICurrentTime() )
	{
		if( !defend->IsDead() && playerDistance < cwmWorldState->ServerData()->GetCombatMaxSpellRange() )
		{
			SI16 spattacks = attack->GetSpAttack();
			if( spattacks <= 0 )
				return;
			if( spattacks > 8 )
				spattacks = 8;

			attack->StopSpell();
			switch( ( RandomNum( 0, spattacks ) + 1 ) )
			{
			case 1:
				switch( RandomNum( 1, 6 ) ) 
				{
				case 1:		CastSpell( attack, defend, 1 );		break;	// Clumsy
				case 2:		CastSpell( attack, defend, 3 );		break;	// Feeble Mind
				case 3:		CastSpell( attack, defend, 4 );		break;	// Heal (personal...switch target temporarily to self and then back after casting)
				case 4:		CastSpell( attack, defend, 5 );		break;	// Magic Arrow
				case 5:		CastSpell( attack, defend, 7 );		break;	// Reactive Armor (personal...switch target temporarily to self and then back after casting)
				case 6:			// Check if poisoned and cast cure (switch to self) - Hanse
					if( !CastSpell( attack, defend, 11 ) )	// cure
						CastSpell( attack, defend, 8 );		// weaken
					break;
				}
				break;
			case 2:			CastSpell( attack, defend, 12 );	break;	// Harm
			case 3:
				switch( RandomNum( 1, 2 ) ) 
				{
				case 1:		CastSpell( attack, defend, 18 );	break;	// Fireball
				case 2:		CastSpell( attack, defend, 20 );	break;	// Poison
				}
				break;
			case 4:
				switch( RandomNum( 1, 4 ) ) 
				{
				case 1:		CastSpell( attack, defend, 27 );	break;	// Curse
				case 2:		CastSpell( attack, defend, 29 );	break;	// Greater Healing (personal...switch target temporarily to self and then back after casting)
				case 3:		CastSpell( attack, defend, 30 );	break;	// Lightning
				case 4:		CastSpell( attack, defend, 31 );	break;	// Mana Drain
				}
				break;
			case 5:
				switch( RandomNum( 1, 4 ) ) 
				{
				case 1:		CastSpell( attack, defend, 33 );		break;	// Blade Spirits
				case 2:		CastSpell( attack, defend, 36 );		break;	// Magic Reflection (personal...no target switching required)
				case 3:		CastSpell( attack, defend, 37 );		break;	// Mind Blast
				case 4:		CastSpell( attack, defend, 38 );		break;	// Paralyze
				}
				break;
			case 6:		// If NPC is fighting summoned NPC, cast dispel...else cast other spells - Hanse
				if( defend->GetSummonTimer() > 0 && defend->GetNPCAiType() != 4 ) 
					CastSpell( attack, defend, 41 );
				else 
				{
					switch( RandomNum( 1, 3 ) ) 
					{
					case 1:	CastSpell( attack, defend, 42 );		break;	// Energy Bolt
					case 2:	CastSpell( attack, defend, 43 );		break;	// Explosion
					case 3:	CastSpell( attack, defend, 46 );		break;	// Mass Curse
					}
				}
				break;
			case 7:
				switch( RandomNum( 1, 4 ) )
				{
				case 1:		CastSpell( attack, defend, 49 );		break;	// Chain Lightning
				case 2:		CastSpell( attack, defend, 51 );		break;	// Flamestrike
				case 3:		CastSpell( attack, defend, 53 );		break;	// Mana Vampire
				case 4:		CastSpell( attack, defend, 55 );		break;	// Meteor Swarm
				}
				break;
			case 8:
				switch( RandomNum( 1, 2 ) ) 
				{
				case 1:		CastSpell( attack, defend, 57 );		break;	// Earthquake
				case 2:		CastSpell( attack, defend, 58 );		break;	// Energy Vortex
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
			if( attack->GetSpellCast() != -1 )
				Magic->CastSpell( NULL, attack );
			attack->SetSpaTimer( BuildTimeValue( attack->GetSpDelay() ) );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	inline void QuickSwitch( CChar *attack, CChar *defend, SI16 spellNum )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Allows beneficial spells to be cast on self during combat
//o---------------------------------------------------------------------------o
inline void QuickSwitch( CChar *attack, CChar *defend, SI16 spellNum )
{
	if( attack == NULL || defend == NULL || attack == defend )
		return;

	attack->SetSpellCast( static_cast<SI08>(spellNum ));
	attack->SetTarg( calcCharFromSer( attack->GetSerial() ) );
	Magic->CastSpell( NULL, attack );
	attack->SetTarg( calcCharFromSer( defend->GetSerial() ) );
	attack->StopSpell();
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCombat::CastSpell( CChar *attack, CChar *defend, SI16 spellNum )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handles spellcasting during combat
//o---------------------------------------------------------------------------o
bool cCombat::CastSpell( CChar *attack, CChar *defend, SI16 spellNum )
{
	if( attack == NULL || defend == NULL || attack == defend )
		return false;

	if( attack->GetMana() <= spells[spellNum].Mana() )
		return false;
	switch( spellNum )
	{
	case 7:
		if( !attack->GetReactiveArmour() )
			attack->SetSpellCast( static_cast<SI08>(spellNum) );
		else
			return false;
		break;
	case 11:
		if( attack->GetPoisoned() > 0 )
			QuickSwitch( attack, defend, spellNum );
		else
			return false;
		break;
	case 4:
	case 29:
		if( attack->GetStrength() > attack->GetHP() ) 
			QuickSwitch( attack, defend, spellNum );
		else
			return false;
		break;
	case 36:
		if( !attack->IsPermReflected() )
			attack->SetSpellCast( static_cast<SI08>(spellNum ));
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
		attack->SetSpellCast( static_cast<SI08>(spellNum) );
		break;
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI08 cCombat::getCombatSkill( CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Gets your combat skill based on the weapon in your hand (if any)
//o---------------------------------------------------------------------------o
UI08 cCombat::getCombatSkill( CChar *i )
{
	if( i == NULL )
		return WRESTLING;

	CItem *j = getWeapon( i );
	switch( getWeaponType( j ) )
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
//|   Function    :  CItem * cCombat::getShield( CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check players hands for a shield
//o---------------------------------------------------------------------------o
CItem * cCombat::getShield( CChar *i )
{
	if( i != NULL )
	{
		CItem *shield = i->GetItemAtLayer( 2 );
		if( shield != NULL && shield->IsShieldType() )
			return shield;
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI16 cCombat::calcAtt( CChar *p )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate total attack power and do damage to the weapon
//o---------------------------------------------------------------------------o
SI16 cCombat::calcAtt( CChar *p )
{
	if( p == NULL )
		return 0;

	SI16 getDamage = 0;

	CItem *weapon = getWeapon( p );
	if( weapon != NULL )
	{
		if( weapon->GetLoDamage() > 0 && weapon->GetHiDamage() > 0 )
		{
			if( weapon->GetLoDamage() >= weapon->GetHiDamage() ) 
				getDamage += weapon->GetLoDamage();
			else
				getDamage += RandomNum( weapon->GetLoDamage(), weapon->GetHiDamage() );

			if( !p->IsNpc() && !RandomNum( 0, 5 ) )
			{
				weapon->IncHP( -1 );
				if( weapon->GetHP() <= 0 )
				{
					cSocket *mSock = calcSocketObjFromChar( p );
					if( mSock != NULL )
					{
						char name[MAX_NAME];
						getTileName( weapon, name );
						sysmessage( mSock, 311, name );
					}
					Items->DeleItem( weapon );
					return -1;
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
			getDamage = RandomNum( (int)( getWrestSkill / 2 ), getWrestSkill );
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
//|	Function		-	UI16 cCombat::calcDef( CChar *mChar, UI08 hitLoc, bool doDamage )
//|	Date			-	3/03/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Finds the defense value of a specific location or the entire character based on hitLoc
//o--------------------------------------------------------------------------o
UI16 cCombat::calcDef( CChar *mChar, UI08 hitLoc, bool doDamage )
{
	if( mChar == NULL )
		return 0;

	SI32 total = mChar->GetDef();
	if( !mChar->IsNpc() || mChar->isHuman() )	// Polymorphed Characters and GM's can still wear armor
	{
		CItem *defendItem = NULL;
		if( hitLoc == 0 )
		{
			for( UI08 getLoc = 1; getLoc < 7; getLoc++ )
				total += getArmorDef( mChar, *(defendItem), getLoc, true );
			total = (total / 100);
		}
		else
			total = getArmorDef( mChar, *(defendItem), hitLoc );

		if( doDamage && defendItem != NULL && !mChar->IsNpc() )
		{
			SI08 armorDamage = 0;	// Based on OSI standards, each successful hit does 0 to 2 damage to armor hit
			armorDamage -= RandomNum( 0, 2 );
			defendItem->IncHP( armorDamage );

			if( defendItem->GetHP() <= 0 )
			{
				cSocket *mSock = calcSocketObjFromChar( mChar );
				if( mSock != NULL )
				{
					char name[MAX_NAME];
					getTileName( defendItem, name );
					sysmessage( mSock, 311, name );
					statwindow( mSock, mChar );
				}
				Items->DeleItem( defendItem );
			}
		}
	}

	if( total < 2 )
		total = 2;
	return (UI16)total;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI32 cCombat::getArmorDef( CChar *mChar, CItem& defItem, UI08 bodyLoc, bool findTotal )
//|	Date			-	3/03/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Finds the item covering the location bodyLoc with the greatest AR and
//|							returns it along with its def value
//o--------------------------------------------------------------------------o
SI32 cCombat::getArmorDef( CChar *mChar, CItem& defItem, UI08 bodyLoc, bool findTotal )
{
	SI32 armorDef = 0;
	CItem *currItem = NULL;
	switch( bodyLoc )
	{
	case 1:		// Torso
		currItem = checkDef( mChar->GetItemAtLayer( 5 ), currItem, armorDef );	// Shirt
		currItem = checkDef( mChar->GetItemAtLayer( 13 ), currItem, armorDef );	// Torso (Inner - Chest Armor)
		currItem = checkDef( mChar->GetItemAtLayer( 17 ), currItem, armorDef );	// Torso (Middle - Tunic, etc)
		currItem = checkDef( mChar->GetItemAtLayer( 20 ), currItem, armorDef );	// Back (Cloak)
		currItem = checkDef( mChar->GetItemAtLayer( 22 ), currItem, armorDef );	// Torso (Outer - Robe)
		if( findTotal )
			armorDef = (SI32)(100 * (R32)( armorDef / 2.8 ));
		break;
	case 2:		// Arms
		currItem = checkDef( mChar->GetItemAtLayer( 19 ), currItem, armorDef );	// Arms
		if( findTotal )
			armorDef = (SI32)(100 * (R32)( armorDef / 6.8 ));
		break;
	case 3:		// Head
		currItem = checkDef( mChar->GetItemAtLayer( 6 ), currItem, armorDef );	// Head
		if( findTotal )
			armorDef = (SI32)(100 * (R32)( armorDef / 7.3 ));
		break;
	case 4:		// Legs
		currItem = checkDef( mChar->GetItemAtLayer( 3 ), currItem, armorDef );	// Shoes
		currItem = checkDef( mChar->GetItemAtLayer( 4 ), currItem, armorDef );	// Pants
		currItem = checkDef( mChar->GetItemAtLayer( 12 ), currItem, armorDef );	// Waist (Half Apron)
		currItem = checkDef( mChar->GetItemAtLayer( 23 ), currItem, armorDef );	// Legs (Outer - Skirt, Kilt)
		currItem = checkDef( mChar->GetItemAtLayer( 24 ), currItem, armorDef );	// Legs (Inner - Leg Armor)
		if( findTotal )
			armorDef = (SI32)(100 * (R32)( armorDef / 4.5 ));
		break;
	case 5:		// Neck
		currItem = checkDef( mChar->GetItemAtLayer( 10 ), currItem, armorDef );	// Neck
		if( findTotal )
			armorDef = (SI32)(100 * (R32)( armorDef / 14.5 ));
		break;
	case 6:		// Hands
		currItem = checkDef( mChar->GetItemAtLayer( 7 ), currItem, armorDef );	// Gloves
		if( findTotal )
			armorDef = (SI32)(100 * (R32)( armorDef / 14.5 ));
		break;
	default:
		break;
	}
	if( !findTotal )
		defItem = *(currItem);
	return armorDef;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CItem *cCombat::checkDef( CItem *checkItem, CItem& currItem, SI32 &currDef )
//|	Date			-	3/03/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Checks the defense of checkItem vs the defense of currItem and returns
//|							the item with the greater Def and its def value
//o--------------------------------------------------------------------------o
CItem *cCombat::checkDef( CItem *checkItem, CItem *currItem, SI32 &currDef )
{
	if( checkItem != NULL && checkItem->GetDef() > currDef )
	{
		currDef = checkItem->GetDef();
		return checkItem;
	}
	return currItem;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCombat::TimerOk( CChar *c )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check combat timer (for next swing)
//o---------------------------------------------------------------------------o
bool cCombat::TimerOk( CChar *c )
{
	if( c == NULL )
		return false;

	bool retVal = false;
	if( static_cast<UI32>(c->GetTimeout()) < cwmWorldState->GetUICurrentTime() ) 
		retVal = true;
	if( cwmWorldState->GetOverflow() ) 
		retVal = true;
	return retVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * cCombat::getWeapon( CChar *i )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find what weapon (if any) character is holding
//o---------------------------------------------------------------------------o
CItem * cCombat::getWeapon( CChar *i )
{
	if( i == NULL )
		return NULL;

	CItem *j = i->GetItemAtLayer( 1 );
	if( j != NULL )
	{
		if( j->GetType() == 9 )	// spell books aren't weapons
			return NULL;
		return j;
	}
	j = i->GetItemAtLayer( 2 );
	if( j != NULL && !j->IsShieldType() )
		return j;
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cCombat::CombatOnHorse( CChar *i )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play animations for combat while mounted
//o---------------------------------------------------------------------------o
void cCombat::CombatOnHorse( CChar *i )
{
	if( i == NULL )
		return;

	CItem *j = getWeapon( i );
	switch( getWeaponType( j ) )
	{
	case BOWS:
		npcAction( i, 0x1B );
		return;
	case XBOWS:
		npcAction( i, 0x1C);
		return;
	case DEF_SWORDS:
	case SLASH_SWORDS:
	case ONEHND_LG_SWORDS:
	case ONEHND_AXES:
	case DEF_MACES:
	case DEF_FENCING:
		npcAction( i, 0x1A );
		return;
	case TWOHND_FENCING:
	case TWOHND_LG_SWORDS:
	case BARDICHE:
	case TWOHND_AXES:
	case LG_MACES:
		npcAction( i, 0x1D );
		return;
	case WRESTLING:
	default:
		npcAction( i, 0x1A );
		return;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cCombat::CombatOnFoot( CChar *i )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play animations for combat on foot
//o---------------------------------------------------------------------------o
void cCombat::CombatOnFoot( CChar *i )
{
	if( i == NULL )
		return;

	CItem *j = getWeapon( i );
	switch( getWeaponType( j ) )
	{
	case BOWS:
		npcAction( i, 0x12 );
		return;
	case XBOWS:
		npcAction( i, 0x13 );
		return;
	case DEF_SWORDS:
	case SLASH_SWORDS:
	case ONEHND_LG_SWORDS:
	case ONEHND_AXES:
		if( RandomNum( 0, 1 ) )
			npcAction( i, 0x09 );
		else
			npcAction( i, 0x0A );
		return;
	case DEF_MACES:
		if( RandomNum( 0, 1 ) )
			npcAction( i, 0x09 );
		else
			npcAction( i, 0x0B );
		return;
	case LG_MACES:
	case TWOHND_LG_SWORDS:
	case BARDICHE:
	case TWOHND_AXES:
		if( RandomNum( 0, 1 ) )
			npcAction( i, 0x0C );
		else
			npcAction( i, 0x0D );
		return;
	case DEF_FENCING:
		npcAction( i, 0x0A );
		return;
	case TWOHND_FENCING:
		npcAction( i, 0x0E );
		return;
	case WRESTLING:
	default:
		switch( RandomNum( 0, 2 ) )
		{
		case 0:		npcAction( i, 0x0A );	return;  //fist straight-punch
		case 1:		npcAction( i, 0x09 );	return;  //fist top-down
		default:	npcAction( i, 0x1F );	return;  //fist over-head
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 cCombat::getWeaponType( CItem *i )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find what weapon type a character is holding (based on its ID)
//o---------------------------------------------------------------------------o
UI08 cCombat::getWeaponType( CItem *i )
{
	if( i == NULL )
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
		return SLASH_SWORDS;
	// One-Handed Lg. Swords
	case 0x0F5E: //broadsword
	case 0x0F5F: //broadsword
		return ONEHND_LG_SWORDS;
	// Two-Handed Lg. Swords
	case 0x143E: //halberd
	case 0x143F: //halberd
		return TWOHND_LG_SWORDS;
	// Bardiche
	case 0x0F4D: //bardiche
	case 0x0F4E: //bardiche
		return BARDICHE;
	// One-Handed Axes
	case 0x0EC2: //cleaver
	case 0x0EC3: //cleaver
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
		return TWOHND_AXES;
	// Default Maces
	case 0x0E85: //pickaxe
	case 0x0E86: //pickaxe
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
		return LG_MACES;
	// Bows
	case 0x13B1: //bow
	case 0x13B2: //bow
		return BOWS;
	// Crossbows
	case 0x0F4F: //crossbow
	case 0x0F50: //crossbow
	case 0x13FC: //heavy crossbow
	case 0x13FD: //heavy crossbow
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
		return DEF_FENCING;
	// Stabbing Fencing Weapons
	case 0x0E87: //pitchfork
	case 0x0E88: //pitchfork
	case 0x0F62: //spear
	case 0x0F63: //spear
		return TWOHND_FENCING;
	default: // Wrestling
		return WRESTLING;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cCombat::ItemSpell( CChar *attacker, CChar *defender )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle weapons that cast spells
//o---------------------------------------------------------------------------o
void cCombat::ItemSpell( CChar *attacker, CChar *defender )
{
	if( attacker == NULL || defender == NULL || attacker == defender )
		return;

	cSocket *mSock = calcSocketObjFromChar( attacker );
	if( mSock != NULL )
		mSock->CurrentSpellType( 2 );

	CItem *wand = getWeapon( attacker );
	if( wand != NULL )
	{
		if( wand->GetOffSpell() && wand->GetHiDamage() && wand->GetType() == 15 )
		{
			switch( wand->GetOffSpell() )
			{
			case 1:		CastSpell( attacker, defender,  1 );	break;	// Clumsy
			case 2:		CastSpell( attacker, defender,  3 );	break;	// Feeblemind
			case 3:		CastSpell( attacker, defender,  5 );	break;	// Magic Arrow
			case 4:		CastSpell( attacker, defender,  8 );	break;	// Weaken
			case 5:		CastSpell( attacker, defender, 12 );	break;	// Harm
			case 6:		CastSpell( attacker, defender, 18 );	break;	// Fireball
			case 8:		CastSpell( attacker, defender, 27 );	break;	// Curse
			case 9:		CastSpell( attacker, defender, 30 );	break;	// Lightning
			case 11:	CastSpell( attacker, defender, 37 );	break;	// Mind Blast
			case 12:	CastSpell( attacker, defender, 38 );	break;	// Paralyze
			case 14:	CastSpell( attacker, defender, 43 );	break;	// Explosion
			case 15:	CastSpell( attacker, defender, 51 );	break;	// Flame strike
			}

			wand->SetMoreZ( wand->GetMoreZ() - 1 );
			if( wand->GetMoreZ() == 0 )
			{
				wand->SetType( wand->GetType2() );
				wand->SetMoreX( 0 );
				wand->SetMoreY( 0 );
				wand->SetOffSpell( 0 );
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cCombat::doSoundEffect( CChar *p, CItem *weapon )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do the "Hit" Sound Effect
//o---------------------------------------------------------------------------o
void cCombat::doSoundEffect( CChar *p, CItem *weapon )
{
	if( p == NULL || weapon == NULL )
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

//o---------------------------------------------------------------------------o
//|	Function	-	cCombat::doMissedSoundEffect( CChar *p )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do the "Missed" Sound Effect
//o---------------------------------------------------------------------------o
void cCombat::doMissedSoundEffect( CChar *p )
{
	if( p == NULL )
		return;

	switch( RandomNum( 0, 2 ) )
	{
	case 0:	Effects->PlaySound( p, 0x0238 );	break;
	case 1:	Effects->PlaySound( p, 0x0239 );	break;
	default: Effects->PlaySound( p, 0x023A );	break;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cCombat::Kill( CChar *attack, CChar *defend )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle death during combat
//o---------------------------------------------------------------------------o
void cCombat::Kill( CChar *attack, CChar *defend )
{
	if( attack == NULL || defend == NULL || attack == defend )
		return;

	Karma( attack, defend, ( 0 - ( defend->GetKarma() ) ) );
	Fame( attack, defend->GetFame() );
	if( attack->GetNPCAiType() == 4 && defend->IsNpc() )
	{
		npcAction( defend, 0x15 );
		Effects->playDeathSound( defend );

		Npcs->DeleteChar( defend ); // Guards, don't give body
		npcToggleCombat( attack );
		return;
	}

	// Add murder counts
	if( !attack->IsNpc() && !defend->IsNpc() )
	{
		if( attack->DidAttackFirst() && defend->IsInnocent() && GuildSys->ResultInCriminal( attack, defend ) && Races->Compare( attack, defend ) == 0 )
		{
			attack->SetKills( attack->GetKills() + 1 );
			cSocket *aSock = calcSocketObjFromChar( attack );
			if( aSock != NULL )
			{
				sysmessage( aSock, 314, attack->GetKills() );
				if( attack->GetKills() == cwmWorldState->ServerData()->GetRepMaxKills() + 1 )
					sysmessage( aSock, 315 );
			}
		}
		Console.Log( Dictionary->GetEntry( 1617 ), "PvP.log", defend->GetName(), attack->GetName() );
	}
	if( attack->IsNpc() )
		npcToggleCombat( attack );
	doDeathStuff( defend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	cCombat::SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle spawning a guard in guarded areas
//|					NEED TO REWORK FOR REGIONAL GUARD STUFF
//o---------------------------------------------------------------------------o
void cCombat::SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z )
{
	if( mChar == NULL || targChar == NULL )
		return;

	if( targChar->IsDead() || targChar->IsInvulnerable() )
		return;

	UI08 targRegion = mChar->GetRegion(); 
	if( !region[targRegion]->IsGuarded() ) 
		return;

	if( cwmWorldState->ServerData()->GetGuardsStatus() )
	{
		bool reUseGuard = false;

		CChar *getGuard = NULL;
		CHARACTER trgChar = calcCharFromSer( targChar->GetSerial() );
		if( trgChar == INVALIDSERIAL )
			return;

		int xOffset = MapRegion->GetGridX( mChar->GetX() );
		int yOffset = MapRegion->GetGridY( mChar->GetY() );

		SubRegion *toCheck = MapRegion->GetGrid( xOffset, yOffset, mChar->WorldNumber() );
		if( toCheck != NULL )
		{
			for( getGuard = toCheck->FirstChar(); !toCheck->FinishedChars(); getGuard = toCheck->GetNextChar() )
			{
				if( getGuard == NULL )
					break;

				if( !getGuard->IsNpc() || getGuard->GetNPCAiType() != 4 )
					continue;

				if( getGuard->GetTarg() == INVALIDSERIAL || getGuard->GetTarg() == trgChar  )
				{
					if( charInRange( getGuard, targChar ) )
						reUseGuard = true;
				}
				else if( getGuard->GetTarg() == trgChar )
					return;
			}
		}
		// 1/13/2003 - Maarc - Fix for JSE NocSpawner
		if( !reUseGuard )
		{
			int t = region[targRegion]->GetRandomGuard();
#pragma note( "DEPENDENT ON NUMERIC NPC SECTION" )
			char tempStuff[128];
			sprintf( tempStuff, "%i", t );
			getGuard = Npcs->AddNPCxyz( NULL, tempStuff, x, y, z, mChar->WorldNumber() ); 
			if( getGuard == NULL )
				return;

			getGuard->SetNPCAiType( 4 );
		}
		//
		if( getGuard != NULL )
		{
			getGuard->SetAttackFirst( true );
			getGuard->SetAttacker( trgChar );
			getGuard->SetTarg( trgChar );
			getGuard->SetNpcWander( 2 );
			if( !getGuard->IsAtWar() )
				npcToggleCombat( getGuard );

			if( reUseGuard )
			{
				getGuard->SetLocation( targChar );
				getGuard->Teleport();
			}
			else
			{
				getGuard->SetNpcMoveTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetNPCSpeed() )) );
				getGuard->SetSummonTimer( BuildTimeValue( 25 ) );

				Effects->PlaySound( getGuard, 0x01FE );
				Effects->staticeffect( getGuard, 0x372A, 0x09, 0x06 );
				
				getGuard->Update();
				npcTalkAll( getGuard, 313, true );
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool WillResultInCriminal( CChar *attack, CChar *targ )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check flagging, race, and guild info to find if character
//|					should be flagged criminal (returns true if so)
//o---------------------------------------------------------------------------o
bool WillResultInCriminal( CChar *attack, CChar *targ )
{
	if( attack == NULL || targ == NULL ) 
		return false;
	else if( !GuildSys->ResultInCriminal( attack, targ ) || Races->Compare( attack, targ ) != 0 ) 
		return false;
	else if( targ->GetFlag() == 0x04 && attack->GetFlag() != 0x01 )
		return true;
	else 
		return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	PlayerAttack( cSocket *s )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle player attacking (Double-clicking whilst in war mode)
//o---------------------------------------------------------------------------o
void PlayerAttack( cSocket *s )
{
	if( s == NULL )
		return;

	CChar *ourChar = s->CurrcharObj();
	if( ourChar == NULL )
		return;

	SERIAL serial = s->GetDWord( 1 );
	if( serial == INVALIDSERIAL ) 
	{
		ourChar->SetTarg( INVALIDSERIAL );
		return;
	}
	CChar *i = calcCharObjFromSer( serial );
	if( i == NULL )
	{
		ourChar->SetTarg( INVALIDSERIAL );
		return;
	}
	if( ourChar->IsDead() )
	{
		if( i->IsNpc() )
		{
			switch( i->GetNPCAiType() )
			{
			case 1:	// Good Healer
				if( !ourChar->IsCriminal() && !ourChar->IsMurderer() )
				{ // Character is innocent
					if( objInRange( i, ourChar, 3 ) )
					{	//let's resurrect him!
						npcAction( i, 0x10 );
						Targ->NpcResurrectTarget( ourChar );
						Effects->staticeffect( ourChar, 0x376A, 0x09, 0x06 );
						npcTalkAll( i, ( 316 + RandomNum( 0, 4 ) ), false );
					} 
					else
						npcTalkAll( i, 321, true );
				} 
				else // Character is criminal or murderer
					npcTalkAll( i, 322, true );
				break;
			case 666: // Evil Healer
				if( ourChar->IsMurderer() )
				{
					if( objInRange( i, ourChar, 3 ) )	// let's resurrect him
					{
						npcAction( i, 0x10 );
						Targ->NpcResurrectTarget( ourChar );
						Effects->staticeffect( ourChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
						npcTalkAll( i, ( 323 + RandomNum( 0, 4 ) ), false );
					} 
					else
						npcTalkAll( i, 328, true );
				} 
				else
					npcTalkAll( i, 329, true );
				break;
			default:
				sysmessage( s, 330 );
				break;
			}
			return;
		} //if isNpc
		else
		{
			if( cwmWorldState->ServerData()->GetPlayerPersecutionStatus() )
			{
				ourChar->SetTarg( calcCharFromSer( i->GetSerial() ) );
				Skills->Persecute( s );
				return;
			} 
			else
			{
				sysmessage( s, 330 );
				return;
			}
		}//if isNpc
	}//if isDead
	else if( ourChar->GetTarg() != calcCharFromSer( serial ) )
	{
		ourChar->SetTarg( calcCharFromSer( i->GetSerial() ) );
		if( ourChar->GetHidden() && !ourChar->IsPermHidden() )
			ourChar->ExposeToView();
		if( i->IsDead() || i->GetHP() <= 0 )
		{
			sysmessage( s, 331 );
			return;
		}
		if( i->GetNPCAiType() == 17 ) // PlayerVendors
		{
			sysmessage( s, 332, i->GetName() );
			return;
		}
		if( i->IsInvulnerable() )
		{
			sysmessage( s, 333 );
			return;
		}
		
		//flag them FIRST so that anything attacking them as a result of this is not flagged.
		if( WillResultInCriminal( ourChar, i ) ) //REPSYS
		{
			bool regionGuarded = ( region[i->GetRegion()]->IsGuarded() );
			if( cwmWorldState->ServerData()->GetGuardsStatus() && regionGuarded && i->IsNpc() && i->GetNPCAiType() != 4 && i->isHuman() )
				npcTalkAll( i, 335, true );
			criminal( ourChar );
		}

		if( i->IsGuarded() )
			petGuardAttack( ourChar, i, i->GetSerial() );

		npcEmoteAll( ourChar, 334, true, ourChar->GetName(), i->GetName() );	// Attacker emotes "You see attacker attacking target" to all nearby

		if( !i->IsNpc() )
		{
			cSocket *iSock = calcSocketObjFromChar( i );
			if( iSock != NULL )
				npcEmote( iSock, i, 1281, true, ourChar->GetName() ); // "Attacker is attacking you!" sent to target socket only
		}

		// keep the target highlighted
		CPAttackOK toSend = (*i);
		s->Send( &toSend );
		
		if( i->GetNPCAiType() != 4 && i->GetTarg() == INVALIDSERIAL )
		{
			i->SetAttacker( calcCharFromSer( ourChar->GetSerial() ) );
			i->SetAttackFirst( false );
		}
		ourChar->SetAttackFirst( true );
		ourChar->SetAttacker( calcCharFromSer( i->GetSerial() ) );
		npcAttackTarget( ourChar, i );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	npcAttackTarget( CChar *cTarg, CChar *cNpc )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	NPC attacks target
//o---------------------------------------------------------------------------o
void npcAttackTarget( CChar *cTarg, CChar *cNpc )
{
	if( cNpc == NULL || cTarg == NULL || cNpc == cTarg )
		return;
	if( cNpc->IsDead() || cTarg->IsDead() )
		return;
	if( !LineOfSight( NULL, cNpc, cTarg->GetX(), cTarg->GetY(), cTarg->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
		return;

	Effects->playMonsterSound( cNpc, cNpc->GetID(), SND_STARTATTACK );

	CHARACTER target = calcCharFromSer( cTarg->GetSerial() );
	CHARACTER npc = calcCharFromSer( cNpc->GetSerial() );
	bool returningAttack = false;

	cNpc->SetTarg( target );
	cNpc->SetAttacker( target );
	cNpc->SetAttackFirst( ( cTarg->GetTarg() != npc ) );
	if( cTarg->GetTarg() == INVALIDSERIAL )
	{
		if( cTarg->GetNPCAiType() != 17 && !cTarg->IsInvulnerable() )	// invulnerable and player vendors don't fight back!
		{
			cTarg->SetTarg( npc );
			cTarg->SetAttacker( npc );
			cTarg->SetAttackFirst( false );
			returningAttack = true;
		}
	}

	// If Attacker is hidden, show them!
	if( cNpc->GetHidden() && !cNpc->IsPermHidden() )
		cNpc->ExposeToView();
	cNpc->BreakConcentration( calcSocketObjFromChar( cNpc ) );

	// Only unhide the defender, if they're going to return the attack (otherwise they're doing nothing!)
	if( returningAttack )
	{
		if( cTarg->GetHidden() && !cTarg->IsPermHidden() )
			cTarg->ExposeToView();
		cTarg->BreakConcentration( calcSocketObjFromChar( cTarg ) );
	}
	// if the source is an npc, make sure they're in war mode and reset their movement time
	if( cNpc->IsNpc() )
	{
		if( !cNpc->IsAtWar() ) 
			npcToggleCombat( cNpc );
		cNpc->SetNpcMoveTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetNPCSpeed() )) );
	}
	// if the target is an npc, and not a guard, make sure they're in war mode and update their movement time
	// ONLY IF THEY'VE CHANGED ATTACKER
	if( returningAttack && cTarg->IsNpc() && cTarg->GetNPCAiType() != 4 )
	{
		if( !cTarg->IsAtWar() )
			npcToggleCombat( cTarg );
		cTarg->SetNpcMoveTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetNPCSpeed() )) );
	}

	bool gCompare = GuildSys->ResultInCriminal( cNpc, cTarg );
	SI08 rCompare = Races->Compare( cNpc, cTarg );
	
	// If the target is an innocent, not a racial or guild ally/enemy, then flag them as criminal
	// and, of course, call the guards ;>
	if( cTarg->IsInnocent() && gCompare && rCompare == 0 )
	{
		bool regionGuarded = ( region[cTarg->GetRegion()]->IsGuarded() );
		if( cwmWorldState->ServerData()->GetGuardsStatus() && regionGuarded )
		{
			if( cTarg->IsNpc() && cTarg->GetNPCAiType() != 4 && cTarg->isHuman() )
				npcTalkAll( cTarg, 1282, true );
			criminal( cNpc );
#ifdef DEBUG
			Console.Print( "DEBUG: [npcAttackTarget)] %s is being set to criminal", chars[cNpc].name );
#endif
		}
	}
	if( cNpc->DidAttackFirst() )
	{
		npcEmoteAll( cNpc, 334, true, cNpc->GetName(), cTarg->GetName() );  // NPC should emote "Source is attacking Target" to all nearby - Zane
		if( !cTarg->IsNpc() )
		{
			cSocket *iSock = calcSocketObjFromChar( cTarg );
			if( iSock != NULL )
				npcEmote( iSock, cTarg, 1281, true, cNpc->GetName() );	// Target should get an emote only to his socket "Target is attacking you!" - Zane
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	npcSimpleAttackTarget( CChar *defender, CChar *attacker )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	NPC attacks target
//o---------------------------------------------------------------------------o
void npcSimpleAttackTarget( CChar *defender, CChar *attacker )
{
	if( attacker == NULL || defender == NULL || attacker == defender )
		return;
	if( attacker->IsDead() || defender->IsDead() ) 
		return;
	if( ( attacker->GetTarg() != INVALIDSERIAL && &chars[attacker->GetTarg()] == defender ) &&
		( defender->GetTarg() != INVALIDSERIAL && &chars[defender->GetTarg()] == attacker ) )
		return;
	attacker->SetTarg( calcCharFromSer( defender->GetSerial() ) );
	attacker->SetAttacker( calcCharFromSer( defender->GetSerial() ) );
	attacker->SetAttackFirst( true );
	if( defender->GetTarg() == INVALIDSERIAL )
	{
		defender->SetTarg( calcCharFromSer( attacker->GetSerial() ) );
		defender->SetAttacker( calcCharFromSer( attacker->GetSerial() ) );
		defender->SetAttackFirst( false );
	}
	if( attacker->GetHidden() && !attacker->IsPermHidden() )
		attacker->ExposeToView();
	attacker->BreakConcentration( calcSocketObjFromChar( attacker ) );

	if( defender->GetHidden() && !defender->IsPermHidden() )
		defender->ExposeToView();
	defender->BreakConcentration( calcSocketObjFromChar( defender ) );

	if( attacker->IsNpc() )
	{
		if( !attacker->IsAtWar()  ) 
			npcToggleCombat( attacker );
		attacker->SetNpcMoveTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetNPCSpeed() )) );
	}
	if( defender->IsNpc() && defender->GetNPCAiType() != 4 )
	{
		if( !defender->IsAtWar() ) 
			npcToggleCombat( defender );
		defender->SetNpcMoveTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetNPCSpeed()) ) );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	petGuardAttack( CChar *mChar, CChar *owner, SERIAL guarded )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the pet guarding an item / character and have him attack
//|					the person using / attacking the item / character
//o---------------------------------------------------------------------------o
void petGuardAttack( CChar *mChar, CChar *owner, SERIAL guarded )
{
	if( mChar == NULL || owner == NULL || guarded == INVALIDSERIAL )
		return;

	if( mChar->GetSerial() == owner->GetSerial() || mChar->GetCommandLevel() >= CNS_CMDLEVEL )
		return;

	CChar *petGuard = Npcs->getGuardingPet( owner, guarded );
	if( petGuard != NULL && objInRange( mChar, petGuard, cwmWorldState->ServerData()->GetCombatMaxRange() ) )
	{
		if( !Npcs->checkPetFriend( mChar, petGuard ) )
			npcAttackTarget( mChar, petGuard );
		else
		{
			cSocket *oSock = calcSocketObjFromChar( owner );
			if( oSock != NULL )
				sysmessage( oSock, 1629 );
		}
	}
}
