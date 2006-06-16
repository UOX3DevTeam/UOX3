function SpellRegistration()
{
	RegisterSpell( 3, true );	// feeblemind 
	RegisterSpell( 4, true );	// heal
	RegisterSpell( 5, true );	// magic arrow
}

function spellTimerCheck( mChar, mSock )
{
	if( mChar.GetTimer( 6 ) != 0 )
	{
		if( mChar.isCasting )
		{
			mSock.SysMessage( GetDictionaryEntry( 762, mSock.Language ) );
			return false;
		}
		else if( mChar.GetTimer( 6 ) > GetCurrentClock() )
		{
			mSock.SysMessage( GetDictionaryEntry( 1638, mSock.Language ) );
			return false;
		}
	}
	return true;
}

function jailTimerCheck( mChar, mSock )
{
	if( mChar.isJailed && mChar.commandLevel < 2 )
	{
		mSock.SysMessage( GetDictionaryEntry( 704, mSock.Language ) );
		mChar.SetTimer( 6, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}
	return true;
}

function spellEnableCheck( mChar, mSock, mSpell )
{
	if( !mSpell.enabled )
	{
		mSock.SysMessage( GetDictionaryEntry( 707, mSock.Language ) );
		mChar.SetTimer( 6, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}
	return true;
}

function itemInHandCheck( mChar, mSock, spellType )
{
	// The following loop checks to see if any item is currently equipped (if not a GM)
	if( mChar.commandLevel < 2 )
	{
		if( spellType != 2 )
		{
			var itemRHand = mChar.FindItemLayer( 0x01 );
			var itemLHand = mChar.FindItemLayer( 0x02 );
			if( itemLHand || ( itemRHand && itemRHand.type != 9 ) )	// Spellbook
			{
				mSock.SysMessage( GetDictionaryEntry( 708, mSock.Language ) );
				mChar.SetTimer( 6, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return false;
			}
		}
	}
	return true;
}

function onSpellCast( mSock, mChar, directCast, spellNum )
{
	// Are we already casting?
	if( !spellTimerCheck( mChar, mSock ) )
		return false;

	var mSpell	= Spells[spellNum];
	var spellType 	= mSock.currentSpellType;

	mChar.spellCast = spellNum;

	if( !jailTimerCheck( mChar, mSock ) )
		return false;

	// Region checks
	var ourRegion = mChar.region;
	if( (spellNum == 45 && ourRegion.canMark) || (spellNum == 52 && !ourRegion.canGate()) || (spellNum == 32 && !ourRegion.canRecall()) )
	{
		mSock.SysMessage( GetDictionaryEntry( 705, mSock.Language ) );
		mChar.SetTimer( 6, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}

	if( !ourRegion.canCastAggressive && mSpell.agressiveSpell )
	{
		mSock.SysMessage( GetDictionaryEntry( 706, mSock.Language ) );
		mChar.SetTimer( 6, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}

	if( !spellEnableCheck( mChar, mSock, mSpell ) )
		return false;
	
	// Cut the casting requirement on scrolls
	var lowSkill, highSkill;
	if( spellType == 1 )
	{
		lowSkill	= mSpell.scrollLow;
		highSkill	= mSpell.scrollHigh;
	}
	else
	{
		lowSkill	= mSpell.lowSkill;
		highSkill	= mSpell.highSkill;
	}

	if( !itemInHandCheck( mChar, mSock, spellType ) )
		return false;
	
	if( mChar.visible == 1 || mChar.visible == 2 )
		mChar.visible = 0;

	mChar.BreakConcentration( mSock );

	if( mChar.commandLevel < 2  )
	{
		//Check for enough reagents
		// type == 0 -> SpellBook
		if( spellType == 0 && !checkReagents( mChar, mSpell ) )
		{
			mChar.SetTimer( 6, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return false;
		}

		// type == 2 - Wands
		if( type != 2 )
		{
			if( mSpell.mana > mChar.mana )
			{
				mSock.SysMessage( GetDictionaryEntry( 696, mSock.Language ) );
				mChar.SetTimer( 6, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return false;
			}
			if( mSpell.stamina > mChar.stamina )
			{
				mSock.SysMessage( GetDictionaryEntry( 697, mSock.Language ) );
				mChar.SetTimer( 6, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return false;
			}
			if( mSpell.health >= mChar.health )
			{
				mSock.SysMessage( GetDictionaryEntry( 698, mSock.Language ) );
				mChar.SetTimer( 6, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return false;
			}
		}
	}

	if( ( mChar.commandLevel < 2 ) && ( !mChar.CheckSkill( 25, lowSkill, highSkill ) ) )
	{
		mChar.TextMessage( mSpell.Mantra() );
		if( spellType == 0 )
		{
			deleteReagents( mChar, mSpell );
			mChar.SpellFail();
			mChar.SetTimer( 6, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return false;
		}
	}	
	   
	mChar.nextAct = 75;		// why 75?
	
	var delay = mSpell.delay * 100;
	if( spellType == 0 && mChar.commandLevel < 2 ) // if they are a gm they don't have a delay :-)
	{
		mChar.SetTimer( 6, delay );
		mChar.frozen = true;
	}
	else
		mChar.SetTimer( 6, 0 );
	
	if( !mChar.isonhorse )
	{
		var actionID = mSpell.action;
		if( mChar.isHuman || actionID != 0x22 )
			mChar.DoAction( actionID );
	}
	
	var tempString;
	if( mSpell.fieldSpell && mChar.skills.magery > 600 )
		tempString = "Vas " + mSpell.mantra;
	else
		tempString = mSpell.mantra;

	mChar.TextMessage( tempString );
	mChar.isCasting = true;

	mChar.StartTimer( delay, spellNum, true );

	return true;
}

function checkReagents( mChar, mSpell )
{
	if( mSpell.ash > 0 && mChar.ResourceCount( 0x0F8C ) < mSpell.ash )
		return false;
	if( mSpell.drake > 0 && mChar.ResourceCount( 0x0F86 ) < mSpell.drake )
		return false;
	if( mSpell.garlic > 0 && mChar.ResourceCount( 0x0F84 ) < mSpell.garlic )
		return false;
	if( mSpell.ginseng > 0 && mChar.ResourceCount( 0x0F85 ) < mSpell.ginseng )
		return false;
	if( mSpell.moss > 0 && mChar.ResourceCount( 0x0F7B ) < mSpell.moss )
		return false;
	if( mSpell.pearl > 0 && mChar.ResourceCount( 0x0F7A ) < mSpell.pearl )
		return false;
	if( mSpell.shade > 0 && mChar.ResourceCount( 0x0F88 ) < mSpell.shade )
		return false;
	if( mSpell.silk > 0 && mChar.ResourceCount( 0x0F8D ) < mSpell.silk )
		return false;
	return true;
}

function deleteReagents( mChar, mSpell )
{
	mChar.UseResource( 0x0F7A, mSpell.pearl );
	mChar.UseResource( 0x0F7B, mSpell.moss );
	mChar.UseResource( 0x0F84, mSpell.garlic );
	mChar.UseResource( 0x0F85, mSpell.ginseng );
	mChar.UseResource( 0x0F86, mSpell.drake );
	mChar.UseResource( 0x0F88, mSpell.shade );
	mChar.UseResource( 0x0F8C, mSpell.ash );
	mChar.UseResource( 0x0F8D, mSpell.silk );
}

function onTimer( mChar, timerID )
{
	mChar.isCasting = false;
	mChar.frozen 	= false;
	
	if( mChar.npc )
	{
		var ourTarg = mChar.target;
		if( ourTarg && ourTarg.isChar )
			onSpellSuccess( null, mChar, ourTarg );
	}
	else
	{
		var mSock = mChar.socket;
		if( mSock )
			mSock.CustomTarget( 0, Spells[timerID].strToSay );
	}
}

function onCallback0( mSock, ourTarg )
{
	if( ourTarg && ourTarg.isChar )
	{
		var mChar = mSock.currentChar;
		if( mChar )
			onSpellSuccess( mSock, mChar, ourTarg );
	}
}

function onSpellSuccess( mSock, mChar, ourTarg )
{
	if( mChar.isCasting )
		return;

	var spellNum	= mChar.spellCast;
	var mSpell	= Spells[spellNum];
	var spellType	= 0;
	var sourceChar	= mChar;

	if( mSock )
		spellType = mSock.currentSpellType;

	mChar.SetTimer( 6, 0 );
	mChar.spellCast = -1;

	if( mChar.npc || spellType != 2 )
	{
		mChar.mana 	-= mSpell.mana;
		mChar.health 	-= mSpell.health;
		mChar.stamina	-= mSpell.stamina;
	}
	else if( !mChar.npc && spellType == 0 )
		deleteReagents( mChar, mSpell );

	if( !mChar.InRange( ourTarg, 10 ) )
	{
		if( mSock )
			mSock.SysMessage( GetDictionaryEntry( 712, mSock.Language ) ); 
		return;
	}

	if( !mChar.CanSee( ourTarg ) )
	{
		return;
	}
	
	var targRegion = ourTarg.region;
	if( mSpell.agressiveSpell )
	{
		if( !targRegion.canCastAgressive )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 709, mSock.Language ) );
			return;
		}
		if( !ourTarg.vulnerable || ourTarg.aiType == 17 )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 713, mSock.Language ) );
			return;
		}
//		mChar.AttackTarget( ourTarg );
		if( mSpell.reflectable )
		{
			if( ourTarg.magicReflect )
			{
				ourTarg.magicReflect = false;
				ourTarg.StaticEffect( 0x373A, 0, 15 );
				sourceChar 	= ourTarg;
				ourTarg		= mChar;
			}
		}
	}

	if( sourceChar.npc )
		ourTarg.SoundEffect( spellNum, true ); 
	else
		sourceChar.SoundEffect( spellNum, true );
	sourceChar.SpellMoveEffect( ourTarg, mSpell );
	ourTarg.SpellStaticEffect( mSpell );

	// This is where the code actually executes ... all of this setup for a single line of code!

	DispatchSpell( spellNum, mSpell, sourceChar, ourTarg, mChar );
}

function SubtractMana( mChar, mana )
{
	if( mChar.noNeedMana )
		return;
	mChar.mana -= mana;
}

function SubtractStamina( mChar, stamina )
{
	if( mChar.noNeedMana )
		return;
	mChar.stamina -= stamina;
}

function SubtractHealth( mChar, health, mSpell )
{
	if( mChar.noNeedMana || mSpell.health == 0 )
		return;
	mChar.TextMessage( "Health to remove" );
	if( mSpell.health < 0 )
	{
		mChar.TextMessage( "Removing unusual health" );
		if( abs( mSpell.health * health ) > mChar.health )
			mChar.health = 0;
		else
			mChar.health += ( mSpell.health * health );
	}
	else
	{
		mChar.TextMessage( "Removing health of " + health );
		mChar.health -= health;
	}
}

function CalculateHitLoc( )
{
	var BODYPERCENT = 0;
	var ARMSPERCENT = 1;
	var HEADPERCENT = 2;
	var LEGSPERCENT = 3;
	var NECKPERCENT = 4;
	var OTHERPERCENT = 5;
	var TOTALTARGETSPOTS = 6;
	
	var LOCPERCENTAGES = new Array( 44, 14, 14, 14, 7, 7 );

	var hitLoc = RandomNumber( 0, 99 ); // Determine area of Body Hit
	for( var t = BODYPERCENT; t < TOTALTARGETSPOTS; ++t )
	{
		hitLoc -= LOCPERCENTAGES[t];
		if( hitLoc < 0 ) 
		{
			hitLoc = t + 1;
			break;
		}
	}
	return hitLoc;
}

function MagicDamage( p, amount, attacker, mSock, element )
{

	if( !ValidateObject( p ) || !ValidateObject( attacker ) )
		return;

	p.TextMessage( "Valid caster and attacker" );

	if( p.dead || p.health <= 0 )	// extra condition check, to see if deathstuff hasn't been hit yet
		return;

	p.TextMessage( "p not dead or below 0 health" );

	if( p.CheckSkill( 16, 0, 1000 ) )
	{
		p.TextMessage( "Successful eval int check" );
		var dmgReduction = RandomNumber( 0, p.skills.evaluatingintel ) / 10000;
		p.TextMessage( "Bleeding off " + dmgReduction + " damage" );
		amount -= ( amount * dmgReduction );
		if( amount < 1 )
			amount = 1;
	}
	p.TextMessage( "Damage to do: " + amount );
	if( p.frozen && p.dexterity > 0 )
	{
		p.TextMessage( "Unfreezing target" );
		p.frozen = false;
		if( mSock != null ) 
			mSock.SysMessage( GetDictionaryEntry( 700, mSock.Language ) );
	}
	p.TextMessage( "Checking for vulnerability and able to cast aggressive magic" );
	if( p.vulnerable && p.region.canCastAggressive )
	{
		p.TextMessage( "Vulnerable, here comes the damage" );
		var hitLoc = CalculateHitLoc();
		var damage = ApplyDamageBonuses( element, attacker, p, 25, hitLoc, amount);
		p.TextMessage( "Damage after the bonus: " + damage);
		damage = ApplyDefenseModifiers( element, attacker, p, 25, hitLoc, damage, true);
		p.TextMessage( "Damage after the defense: " + damage);

		if( damage <= 0 )
			damage = 1;

		p.Damage( damage, attacker, true );
		p.ReactOnDamage( element, attacker );
	}
}

function DispatchSpell( spellNum, mSpell, sourceChar, ourTarg, caster )
{
	var mMagery = caster.skills.magery;
	if( spellNum == 3 )	// Feeblemind
	{
		caster.TextMessage( "Casting feeblemind" );
		DoTempEffect( 0, sourceChar, ourTarg, 4, (mMagery / 100), 0, 0);	
	}
	else if( spellNum == 4 )	// Heal
	{
		caster.TextMessage( "Casting Heal" );
		var bonus = (mMagery/500) + (mMagery/100);
		caster.TextMessage( "Healing bonus " + bonus );
		caster.TextMessage( "Old health " + ourTarg.health );
		if( bonus != 0 )
		{
			var rAdd = RandomNumber( 0, 5 ) + bonus;
			caster.TextMessage( "Adding " + rAdd + " health!" );
			ourTarg.health += rAdd;
		}
		else
		{
			caster.TextMessage( "Adding 4 health!" );
			ourTarg.health += 4;
		}
		caster.TextMessage( "New health " + ourTarg.health );
		caster.TextMessage( "Subtracting health" );
		SubtractHealth( caster, bonus / 2, 4 );
		if( ourTarg.murderer )
			caster.criminal = true;
	}
	else if( spellNum == 5 )	// Magic arrow
	{
		caster.TextMessage( "Casting Magic Arrow!" );
		var baseDamage = 2 + RandomNumber( 0, 5 );
		var mageryAdjust = ( mMagery / 2000 + 1 );
		caster.TextMessage( "Base damage " + baseDamage );
		caster.TextMessage( "Magery multiplier: " + mageryAdjust );
		MagicDamage( ourTarg, baseDamage * mageryAdjust, caster, caster.socket, 5 );
	}
}