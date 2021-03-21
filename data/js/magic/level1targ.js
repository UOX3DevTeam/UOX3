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
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 762, mSock.language ) );
			return false;
		}
		else if( mChar.GetTimer( 6 ) > GetCurrentClock() )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 1638, mSock.language ) );
			return false;
		}
	}
	return true;
}

function jailTimerCheck( mChar, mSock )
{
	if( mChar.isJailed && mChar.commandlevel < 2 )
	{
		mSock.SysMessage( GetDictionaryEntry( 704, mSock.language ) );
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
		if( mSock )
			mSock.SysMessage( GetDictionaryEntry( 707, mSock.language ) );
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
	if( mChar.commandlevel < 2 )
	{
		if( spellType != 2 )
		{
			var itemRHand = mChar.FindItemLayer( 0x01 );
			var itemLHand = mChar.FindItemLayer( 0x02 );
			if( itemLHand || ( itemRHand && itemRHand.type != 9 ) )	// Spellbook
			{
				if( mSock )
					mSock.SysMessage( GetDictionaryEntry( 708, mSock.language ) );
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
		return true;

	var mSpell	= Spells[spellNum];
	var spellType = mChar.currentSpellType;

	mChar.spellCast = spellNum;

	if( !jailTimerCheck( mChar, mSock ) )
		return true;

	// Region checks
	var ourRegion = mChar.region;
	if( (spellNum == 45 && ourRegion.canMark) || (spellNum == 52 && !ourRegion.canGate()) || (spellNum == 32 && !ourRegion.canRecall()) )
	{
		if( mSock )
			mSock.SysMessage( GetDictionaryEntry( 705, mSock.language ) );
		mChar.SetTimer( 6, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return true;
	}

	if( mSpell.aggressiveSpell )
	{
		if( ourRegion.isSafeZone )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 1799, mSock.language ) );
			mChar.SetTimer( 6, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return;
		}

		if( !ourRegion.canCastAggressive )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 706, mSock.language ) );
			mChar.SetTimer( 6, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return true;
		}
	}

	if( !spellEnableCheck( mChar, mSock, mSpell ) )
		return true;

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
		return true;

	if( mChar.visible == 1 || mChar.visible == 2 )
		mChar.visible = 0;

	if( mSock )
		mChar.BreakConcentration( mSock );

	if( mChar.commandlevel < 2  )
	{
		//Check for enough reagents
		// type == 0 -> SpellBook
		if( spellType == 0 && !checkReagents( mChar, mSpell ) )
		{
			mChar.SetTimer( 6, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return true;
		}

		// type == 2 - Wands
		if( spellType != 2 )
		{
			if( mSpell.mana > mChar.mana )
			{
				if( mSock )
					mSock.SysMessage( GetDictionaryEntry( 696, mSock.language ) );
				mChar.SetTimer( 6, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
			if( mSpell.stamina > mChar.stamina )
			{
				if( mSock )
					mSock.SysMessage( GetDictionaryEntry( 697, mSock.language ) );
				mChar.SetTimer( 6, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
			if( mSpell.health >= mChar.health )
			{
				if( mSock )
					mSock.SysMessage( GetDictionaryEntry( 698, mSock.language ) );
				mChar.SetTimer( 6, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
		}
	}

	if( ( mChar.commandlevel < 2 ) && ( !mChar.CheckSkill( 25, lowSkill, highSkill ) ) )
	{
		mChar.TextMessage( mSpell.mantra );
		if( spellType == 0 )
		{
			deleteReagents( mChar, mSpell );
			mChar.SpellFail();
			mChar.SetTimer( 6, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return true;
		}
	}

	mChar.nextAct = 75;		// why 75?

	var delay = mSpell.delay * 100;
	if( spellType == 0 && mChar.commandlevel < 2 ) // if they are a gm they don't have a delay :-)
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
	var failedCheck = 0;
	if( mChar.noNeedReags == false )
	{
		if( mSpell.ash > 0 && mChar.ResourceCount( 0x0F8C ) < mSpell.ash )
			failedCheck = 1;
		if( mSpell.drake > 0 && mChar.ResourceCount( 0x0F86 ) < mSpell.drake )
			failedCheck = 1;
		if( mSpell.garlic > 0 && mChar.ResourceCount( 0x0F84 ) < mSpell.garlic )
			failedCheck = 1;
		if( mSpell.ginseng > 0 && mChar.ResourceCount( 0x0F85 ) < mSpell.ginseng )
			failedCheck = 1;
		if( mSpell.moss > 0 && mChar.ResourceCount( 0x0F7B ) < mSpell.moss )
			failedCheck = 1;
		if( mSpell.pearl > 0 && mChar.ResourceCount( 0x0F7A ) < mSpell.pearl )
			failedCheck = 1;
		if( mSpell.shade > 0 && mChar.ResourceCount( 0x0F88 ) < mSpell.shade )
			failedCheck = 1;
		if( mSpell.silk > 0 && mChar.ResourceCount( 0x0F8D ) < mSpell.silk )
			failedCheck = 1;
		if( failedCheck == 1 )
		{
			mChar.SysMessage( GetDictionaryEntry( 702, mChar.socket.language ) );  // not enough reagents to cast spell
			return false;
		}
	}
	return true;
}

function deleteReagents( mChar, mSpell )
{
	mChar.UseResource( mSpell.pearl, 0x0F7A );
	mChar.UseResource( mSpell.moss, 0x0F7B );
	mChar.UseResource( mSpell.garlic, 0x0F84 );
	mChar.UseResource( mSpell.ginseng, 0x0F85 );
	mChar.UseResource( mSpell.drake, 0x0F86 );
	mChar.UseResource( mSpell.shade, 0x0F88 );
	mChar.UseResource( mSpell.ash, 0x0F8C );
	mChar.UseResource( mSpell.silk, 0x0F8D );
}

function onTimer( mChar, timerID )
{
	mChar.isCasting = false;
	mChar.frozen 	= false;

	if( mChar.npc )
	{
		var ourTarg = mChar.target;
		if( ourTarg && ourTarg.isChar )
		{
			onSpellSuccess( null, mChar, ourTarg, timerID );
		}
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
			onSpellSuccess( mSock, mChar, ourTarg, 0 );
	}
}

function onSpellSuccess( mSock, mChar, ourTarg, spellID )
{

	if( mChar.isCasting )
		return;

	var spellNum	= mChar.spellCast;
	if( spellNum == -1 )
	{
		if( spellID != -1 )
			spellNum = spellID;
		else
			return;
	}

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
			mSock.SysMessage( GetDictionaryEntry( 712, mSock.language ) );
		return;
	}

	if( !mChar.CanSee( ourTarg ) )
		return;

	var targRegion = ourTarg.region;
	if( mSpell.aggressiveSpell )
	{
		if( targRegion.isSafeZone )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 1799, mSock.language ) );
			return;
		}
		if( !targRegion.canCastAggressive )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 709, mSock.language ) );
			return;
		}
		if( !ourTarg.vulnerable || ourTarg.aiType == 17 )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 713, mSock.language ) );
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
		ourTarg.SoundEffect( mSpell.soundEffect, true );
	else
		sourceChar.SoundEffect( mSpell.soundEffect, true );
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
	if( mSpell.health < 0 )
	{
		if( abs( mSpell.health * health ) > mChar.health )
			mChar.health = 0;
		else
			mChar.health += ( mSpell.health * health );
	}
	else
	{
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

	//p.TextMessage( "Valid caster and attacker" );

	if( p.dead || p.health <= 0 )	// extra condition check, to see if deathstuff hasn't been hit yet
		return;

	//p.TextMessage( "p not dead or below 0 health" );

	//p.TextMessage( "Damage to do: " + amount );
	if( p.frozen && p.dexterity > 0 )
	{
		//p.TextMessage( "Unfreezing target" );
		p.frozen = false;
		if( mSock != null )
			mSock.SysMessage( GetDictionaryEntry( 700, mSock.language ) );
	}
	//p.TextMessage( "Checking for vulnerability and able to cast aggressive magic" );
	if( p.vulnerable && p.region.canCastAggressive )
	{
		//p.TextMessage( "Vulnerable, here comes the damage" );
		var hitLoc = CalculateHitLoc();
		var damage = ApplyDamageBonuses( element, attacker, p, 25, hitLoc, amount);
		//p.TextMessage( "Damage after the bonus: " + damage);
		damage = ApplyDefenseModifiers( element, attacker, p, 25, hitLoc, damage, true);
		//p.TextMessage( "Damage after the defense: " + damage);

		if( damage <= 0 )
			damage = 1;

		// Double spell-damage against non-human NPCs
		if( p.npc && !p.isHuman )
			damage *= 2;

		p.Damage( damage, attacker, true );
		p.ReactOnDamage( element, attacker );
	}
}

function DispatchSpell( spellNum, mSpell, sourceChar, ourTarg, caster )
{
	var mMagery = caster.skills.magery;
	if( spellNum == 3 )	// Feeblemind
	{
		// Target Resist Check
		var spellResisted = CheckTargetResist( caster, ourTarg, mSpell.circle );

		//caster.TextMessage( "Casting feeblemind" );
		DoTempEffect( 0, sourceChar, ourTarg, 4, (mMagery / 100), 0, 0);
	}
	else if( spellNum == 4 )	// Heal
	{
		//Store original target
		//var oldTarg = ourTarg;

		if( caster.npc )
			ourTarg = caster;

		var baseHealing = Math.round( RandomNumber( mSpell.baseDmg / 2, mSpell.baseDmg ));

		//caster.TextMessage( "Casting Heal" );
		var bonus = (mMagery/500) + (mMagery/100);
		//caster.TextMessage( "Healing bonus " + bonus );
		//caster.TextMessage( "Old health " + ourTarg.health );
		if( bonus != 0 )
		{
			var rAdd = baseHealing + bonus;
			//caster.TextMessage( "Adding " + rAdd + " health!" );
			ourTarg.health += rAdd;
		}
		else
		{
			//caster.TextMessage( "Adding 4 health!" );
			ourTarg.health += baseHealing;
		}
		//caster.TextMessage( "New health " + ourTarg.health );
		//caster.TextMessage( "Subtracting health" );
		//SubtractHealth( caster, bonus / 2, 4 );
		if( ourTarg.murderer )
			caster.criminal = true;

		//Restore original target
		//if( caster.npc )
		//	ourTarg = oldTarg;
	}
	else if( spellNum == 5 )	// Magic arrow
	{
		var baseDamage = mSpell.baseDmg;

		// Target Resist Check
		var spellResisted = CheckTargetResist( caster, ourTarg, mSpell.circle );

		// Apply spell-damage modifiers to baseDamage
		baseDamage = CalcSpellDamage( caster, ourTarg, baseDamage, spellResisted );

		MagicDamage( ourTarg, baseDamage, caster, caster.socket, 5 );
	}
}

function CheckTargetResist( caster, ourTarg, circle )
{
	var i = ourTarg.CheckSkill( 26, 80*circle, 800+(80*circle) );
	if( ValidateObject( ourTarg ) )
	{
		var defaultChance = ourTarg.skills.magigresistance / 5;
		var resistChance = ourTarg.skills.magicresistance - ((( caster.skills.magery - 20 ) / 5 ) + ( circle * 5 ));
		if( defaultChance > resistChance )
			resistChance = defaultChance;
		if( RandomNumber( 1, 100 ) < resistChance / 10 )
		{
			var tSock = ourTarg.socket;
			if( tSock != null )
				tSock.SysMessage( GetDictionaryEntry( 699, tSock.language ) );
			i = true;
		}
		else
			i = false;
	}
	else
	{
		if( i )
		{
			var tSock = ourTarg.socket;
			if( tSock != null )
				tSock.SysMessage( GetDictionaryEntry( 699, tSock.language ) );
		}
	}
	return i;
}

function CalcSpellDamage( caster, ourTarg, baseDamage, spellResisted )
{
	baseDamage = RandomNumber( baseDamage / 2, baseDamage );
	if( spellResisted )
		baseDamage = baseDamage / 2;

	var casterEval = caster.skills.evaluatingintel / 10;
	var targetResist = ourTarg.skills.magicresistance / 10;
	if( targetResist > casterEval )
		baseDamage *= ((( casterEval - targetResist ) / 200 ) + 1 );
	else
		baseDamage *= ((( casterEval - targetResist ) / 500 ) + 1 );

	var i = RandomNumber( 0, 4 );
	if( i <= 2 )
		baseDamage = Math.round( RandomNumber( RandomNumber( baseDamage / 2, baseDamage ) / 2, baseDamage ));
	else if( i == 3 )
		baseDamage = Math.round( RandomNumber( baseDamage / 2, baseDamage ));
	else //keep current damage
		baseDamage = Math.round( baseDamage );

	return baseDamage;
}