function SpellRegistration()
{
	RegisterSpell( 3, true );	// feeblemind
	RegisterSpell( 4, true );	// heal
	RegisterSpell( 5, true );	// magic arrow
}

function SpellTimerCheck( mChar, mSock )
{
	if( mChar.GetTimer( Timer.SPELLTIME ) != 0 )
	{
		if( mChar.isCasting )
		{
			if( mSock )
			{
				mSock.SysMessage( GetDictionaryEntry( 762, mSock.language )); // You are already casting a spell.
			}
			return false;
		}
		else if( mChar.GetTimer( Timer.SPELLTIME ) > GetCurrentClock() )
		{
			if( mSock )
			{
				mSock.SysMessage( GetDictionaryEntry( 1638, mSock.language )); // You must wait a little while before casting
			}
			return false;
		}
	}
	return true;
}

function JailTimerCheck( mChar, mSock )
{
	if( mChar.isJailed && mChar.commandlevel < 2 )
	{
		mSock.SysMessage( GetDictionaryEntry( 704, mSock.language )); // You are in jail and cannot cast spells!
		mChar.SetTimer( Timer.SPELLTIME, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}
	return true;
}

function SpellEnableCheck( mChar, mSock, mSpell )
{
	if( !mSpell.enabled )
	{
		if( mSock )
		{
			mSock.SysMessage( GetDictionaryEntry( 707, mSock.language )); // That spell is currently not enabled.
		}
		mChar.SetTimer( Timer.SPELLTIME, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}
	return true;
}

function ItemInHandCheck( mChar, mSock, spellType )
{
	// The following loop checks to see if any item is currently equipped (if not a GM)
	if( mChar.commandlevel < 2 )
	{
		if( spellType != 2 )
		{
			var itemRHand = mChar.FindItemLayer( 0x01 );
			var itemLHand = mChar.FindItemLayer( 0x02 );
			var lHandBlocks = false;
			var rHandBlocks = false;

			// Evaluate blocking for left and right hand items
			if( !isSpellCastingAllowed( itemRHand ) || !isSpellCastingAllowed( itemLHand ))
			{
				var result = AutoUnequipAttempt( itemLHand, itemRHand, mChar );
				lHandBlocks = result.lHandBlocks;
				rHandBlocks = result.rHandBlocks;
			}

			if( lHandBlocks || rHandBlocks )
			{
				if( mSock != null ) 
				{
					mSock.SysMessage( GetDictionaryEntry( 708, mSock.language )); // You cannot cast with a weapon equipped.
				}

				if( !mChar.isCasting ) 
				{
					mChar.SetTimer( Timer.SPELLTIME, 0 );
					mChar.isCasting = false;
					mChar.spellCast = -1;
				}
				return false;
			}
		}
	}
	return true;
}

function onSpellCast( mSock, mChar, directCast, spellNum )
{
	// Are we recovering from another spell that was just cast
	if( mChar.GetTimer( Timer.SPELLRECOVERYTIME ) != 0 )
	{
		if( mChar.GetTimer( Timer.SPELLRECOVERYTIME ) > GetCurrentClock() )
		{
			if( mSock != null )
			{
				mSock.SysMessage( GetDictionaryEntry( 1638, mSock.language )); // You must wait a little while before casting
			}
			return true;
		}
	}

	// Are we already casting?
	if( !SpellTimerCheck( mChar, mSock ))
		return true;

	var mSpell	= Spells[spellNum];
	var spellType = 0

	// Fetch spelltype; 0 = normal spell, 1 = scroll, 2 = wand
	if( mSock != null )
	{
		spellType = mSock.currentSpellType;
	}

	mChar.spellCast = spellNum;

	if( !JailTimerCheck( mChar, mSock ))
		return true;

	// Region checks
	var ourRegion = mChar.region;
	if(( spellNum == 45 && ourRegion.canMark ) || ( spellNum == 52 && !ourRegion.canGate() ) || ( spellNum == 32 && !ourRegion.canRecall() ))
	{
		if( mSock != null )
		{
			mSock.SysMessage( GetDictionaryEntry( 705, mSock.language )); // This is not allowed here.
		}
		mChar.SetTimer( Timer.SPELLTIME, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return true;
	}

	if( mSpell.aggressiveSpell )
	{
		if( ourRegion.isSafeZone )
		{
			if( mSock != null )
			{
				mSock.SysMessage( GetDictionaryEntry( 1799, mSock.language )); // Hostile actions are not permitted in this safe area.
			}
			mChar.SetTimer( Timer.SPELLTIME, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return;
		}

		if( !ourRegion.canCastAggressive )
		{
			if( mSock != null )
			{
				mSock.SysMessage( GetDictionaryEntry( 706, mSock.language )); // This is not allowed in town.
			}
			mChar.SetTimer( Timer.SPELLTIME, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return true;
		}
	}

	if( !SpellEnableCheck( mChar, mSock, mSpell ))
		return true;

	if( !ItemInHandCheck( mChar, mSock, spellType ))
		return true;

	if( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}

	if( mSock != null )
	{
		mChar.BreakConcentration( mSock );
	}

	if( mChar.commandlevel < 2  )
	{
		// type == 2 - Wands
		if( spellType != 2 )
		{
			if( mSpell.mana > mChar.mana )
			{
				if( mSock != null )
				{
					mSock.SysMessage( GetDictionaryEntry( 696, mSock.language )); // You have insufficient mana to cast that spell.
				}
				mChar.SetTimer( Timer.SPELLTIME, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
			if( mSpell.stamina > mChar.stamina )
			{
				if( mSock != null )
				{
					mSock.SysMessage( GetDictionaryEntry( 697, mSock.language )); // You have insufficient stamina to cast that spell.
				}
				mChar.SetTimer( Timer.SPELLTIME, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
			if( mSpell.health >= mChar.health )
			{
				if( mSock != null )
				{
					mSock.SysMessage( GetDictionaryEntry( 698, mSock.language )); // You have insufficient health to cast that spell.
				}
				mChar.SetTimer( Timer.SPELLTIME, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
		}
	}

	mChar.nextAct = 75;		// why 75?

	var delay = mSpell.delay;
	if( spellType == 0 && mChar.commandlevel < 2 ) // if they are a gm they don't have a delay :-)
	{
		mChar.SetTimer( Timer.SPELLTIME, delay * 1000 );
		if( !GetServerSetting( "CastSpellsWhileMoving" ))
		{
			mChar.frozen = true;
			mChar.Refresh();
		}
	}
	else
	{
		mChar.SetTimer( Timer.SPELLTIME, 0 );
	}

	if( !mChar.isonhorse )
	{
		var actionID = mSpell.action;
		if( mChar.isHuman || actionID != 0x22 )
		{
			mChar.DoAction( actionID );
		}
	}

	// Only human casters will say the spellcasting mantras
	if( mChar.isHuman )
	{
		var tempString;
		if( mSpell.fieldSpell && mChar.skills.magery > 600 )
		{
			tempString = "Vas " + mSpell.mantra;
		}
		else
		{
			tempString = mSpell.mantra;
		}

		mChar.TextMessage( tempString );
	}

	mChar.isCasting = true;

	mChar.StartTimer( delay * 1000, spellNum, true );

	return true;
}

function onTimer( mChar, timerID )
{
	mChar.isCasting = false;
	mChar.frozen 	= false;
	mChar.Refresh();

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
		if( mSock != null )
		{
			var cursorType = 0;
			var spellNum = mChar.spellCast;
			if( spellNum == -1 )
				return;

			if( Spells[spellNum].aggressiveSpell )
			{
				cursorType = 1; // Hostile cursor type
			}
			else if( spellNum == 4 ) // Heal
			{
				cursorType = 2; // Friendly cursor type
			}

			mChar.SetTimer( Timer.SPELLRECOVERYTIME, Spells[spellNum].recoveryDelay );
			mSock.CustomTarget( 0, Spells[timerID].strToSay, cursorType );
		}
	}
}

function onCallback0( mSock, ourTarg )
{
	var mChar = mSock.currentChar;
	if( !ValidateObject( mChar ))
		return;

	if( ValidateObject( ourTarg ) && ourTarg.isChar )
	{
		if( ourTarg != mChar && mChar.spellCast != -1 )
		{
			if( DoesEventExist( 2507, "onSpellTarget" ))
			{
				if( TriggerEvent( 2507, "onSpellTarget", ourTarg, mChar, mChar.spellCast ) != false )
				{
					return;
				}
			}
		}

		onSpellSuccess( mSock, mChar, ourTarg, 0 );
	}
	else
	{
		mChar.SetTimer( Timer.SPELLTIME, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		mChar.frozen = false;
		mChar.Refresh();
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
		{
			spellNum = spellID;
		}
		else
		{
			return;
		}
	}

	var mSpell	= Spells[spellNum];
	var spellType	= 0;
	var sourceChar	= mChar;

	if( mSock != null )
	{
		spellType = mSock.currentSpellType;
	}

	mChar.SetTimer( Timer.SPELLTIME, 0 );
	mChar.spellCast = -1;

	// If player commandlevel is below GM-level, check for reagents
	if( mSock != null && mChar.commandlevel < 2  )
	{
		//Check for enough reagents
		// type == 0 -> SpellBook
		if( spellType == 0 && !TriggerEvent( 6004, "CheckReagents", mChar, mSpell))
		{
			mChar.SetTimer( Timer.SPELLTIME, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return;
		}
	}

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

	// Do skillcheck
	if(( mChar.commandlevel < 2 ) && ( !mChar.CheckSkill( 25, lowSkill, highSkill )))
	{
		if( spellType == 0 )
		{
			TriggerEvent( 6004, "DeleteReagents", mChar, mSpell );
			mChar.SpellFail();
			mChar.SetTimer( Timer.SPELLTIME, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return;
		}
	}

	if( mChar.npc || spellType != 2 )
	{
		mChar.mana 		-= mSpell.mana;
		mChar.health 	-= mSpell.health;
		mChar.stamina	-= mSpell.stamina;
	}
	if( !mChar.npc && spellType == 0 )
	{
		TriggerEvent( 6004, "DeleteReagents", mChar, mSpell );
	}

	if( !mChar.InRange( ourTarg, 10 ) )
	{
		if( mSock != null )
		{
			mSock.SysMessage( GetDictionaryEntry( 712, mSock.language )); // You can't cast on someone that far away!
		}
		return;
	}

	if( !mChar.CanSee( ourTarg ))
		return;

	var targRegion = ourTarg.region;
	if( mSpell.aggressiveSpell )
	{
		if( targRegion.isSafeZone )
		{
			if( mSock != null )
			{
				mSock.SysMessage( GetDictionaryEntry( 1799, mSock.language )); // Hostile actions are not permitted in this safe area.
			}
			return;
		}
		if( !targRegion.canCastAggressive )
		{
			if( mSock != null )
			{
				mSock.SysMessage( GetDictionaryEntry( 709, mSock.language )); // You can't cast in town!
			}
			return;
		}
		if( !ourTarg.vulnerable || ourTarg.aiType == 17 )
		{
			if( mSock != null )
			{
				mSock.SysMessage( GetDictionaryEntry( 713, mSock.language )); // They are invulnerable merchants!
			}
			return;
		}

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

	if( spellNum != 5 )
	{
		if( sourceChar.npc )
		{
			ourTarg.SoundEffect( mSpell.soundEffect, true );
		}
		else
		{
			sourceChar.SoundEffect( mSpell.soundEffect, true );
		}
		sourceChar.SpellMoveEffect( ourTarg, mSpell );
		ourTarg.SpellStaticEffect( mSpell );
	}

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
		{
			mChar.health = 0;
		}
		else
		{
			mChar.health += ( mSpell.health * health );
		}
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
	if( !ValidateObject( p ) || !ValidateObject( attacker ))
		return;

	if( p.dead || p.health <= 0 )	// extra condition check, to see if deathstuff hasn't been hit yet
		return;

	if( p.frozen && p.dexterity > 0 )
	{
		p.frozen = false;
		p.Refresh();
		if( mSock != null )
		{
			mSock.SysMessage( GetDictionaryEntry( 700, mSock.language )); // You are no longer frozen.
		}
	}

	if( p.vulnerable && p.region.canCastAggressive )
	{
		var hitLoc = CalculateHitLoc();
		var damage = ApplyDamageBonuses( element, attacker, p, 25, hitLoc, amount );
		damage = ApplyDefenseModifiers( element, attacker, p, 25, hitLoc, damage, true );

		if( damage <= 0 )
		{
			damage = 1;
		}

		// Double spell-damage against non-human NPCs
		if( p.npc && !p.isHuman )
		{
			damage *= 2;
		}

		DoTempEffect( 0, attacker, p, 28, damage, 0, 0 );
	}
}

function DispatchSpell( spellNum, mSpell, sourceChar, ourTarg, caster )
{
	var mMagery = caster.skills.magery;
	if( spellNum == 3 )	// Feeblemind
	{
		// Target Resist Check
		var spellResisted = TriggerEvent( 6000, "CheckResist", caster, ourTarg, mSpell.circle );

		//caster.TextMessage( "Casting feeblemind" );
		DoTempEffect( 0, sourceChar, ourTarg, 4, Math.round( mMagery / 100 ), 0, 0 );
	}
	else if( spellNum == 4 )	// Heal
	{
		if( caster.npc )
		{
			ourTarg = caster;
		}

		var baseHealing = Math.round( RandomNumber( mSpell.baseDmg / 2, mSpell.baseDmg ));

		var bonus = ( mMagery / 500 ) + ( mMagery / 100 );
		if( bonus != 0 )
		{
			var rAdd = baseHealing + bonus;
			ourTarg.health += rAdd;
		}
		else
		{
			ourTarg.health += baseHealing;
		}

		if( ourTarg.murderer )
		{
			caster.criminal = true;
		}
	}
	else if( spellNum == 5 )	// Magic arrow
	{
		var baseDamage = mSpell.baseDmg;

		// Take target's magic resistance into account
		var spellResisted = TriggerEvent( 6000, "CheckResist", caster, ourTarg, mSpell.circle );

		// Calculate final damage based on caster's eval int vs target's resistance, and whether spell was resisted
		baseDamage = TriggerEvent( 6001, "CalcFinalSpellDamage", caster, ourTarg, baseDamage, spellResisted )

		MagicDamage( ourTarg, baseDamage, caster, caster.socket, 5 );
	}
}

// Function to check if an equipped item allows casting
function isSpellCastingAllowed( item ) 
{
	return item != null && ( item.type == 9 || item.type == 119 ); // Assuming type 9 is spellbook, and type 119 is spell channeling item
}

// Function to handle items
function AutoUnequipAttempt( itemLHand, itemRHand, mChar )
{
	const autoUnequip = GetServerSetting( "AutoUnequippedCasting" );
	var lHandBlocks = false; // Default to false
	var rHandBlocks = false; // Default to false
	if( itemLHand != null && !isSpellCastingAllowed( itemLHand ))
	{
		if( autoUnequip && mChar.pack.totalItemCount < mChar.pack.maxItems )
		{
			itemLHand.container = mChar.pack;
		}
		else 
		{
			lHandBlocks = true; // Set to true if item is blocking
		}
	}

	if( itemRHand != null && !isSpellCastingAllowed( itemRHand ))
	{
		if( autoUnequip && mChar.pack.totalItemCount < mChar.pack.maxItems )
		{
			itemRHand.container = mChar.pack;
		}
		else
		{
			rHandBlocks = true; // Set to true if item is blocking
		}
	}
	return { lHandBlocks: lHandBlocks, rHandBlocks: rHandBlocks };
}

function _restorecontext_() {}
