/// <reference path="../definitions.d.ts" />
// @ts-check
function SpellRegistration()
{
	RegisterSpell( 1, true );	// say, clumsy, same as in the spells.dfn file
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
	if( mChar.isJailed && mChar.commandlevel < GetCommandLevelVal( "CNS" ))
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
	if( mChar.commandlevel < GetCommandLevelVal( "CNS" ))
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

/** @type { ( tChar: Character, SpellId: number ) => number } */
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
			return true;
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

	if( mChar.commandlevel < GetCommandLevelVal( "CNS" ))
	{
		// type == 2 - Wands
		if( spellType != 2 )
		{
			if( mSpell.mana > mChar.mana )
			{
				if( mSock != null )
				{
					mSock.SysMessage( GetDictionaryEntry( 696, mSock.Language )); // You have insufficient mana to cast that spell.
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
	if( spellType == 0 && mChar.commandlevel < GetCommandLevelVal( "CNS" )) // if they are a gm they don't have a delay :-)
	{
		mChar.SetTimer( Timer.SPELLTIME, delay * 1000 );
		if( !GetServerSetting( "CastSpellsWhileMOving" ))
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

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
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
			onSpellSuccess( null, mChar, ourTarg );
		}
	}
	else
	{
		var mSock = mChar.socket;
		if( mSock != null )
		{
			mSock.CustomTarget( 0, Spells[timerID].strToSay, 1 ); // hostile cursor type
		}
	}
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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

		onSpellSuccess( mSock, mChar, ourTarg );
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

/** @type { ( tChar: Character, SpellId: number ) => boolean } */
function onSpellSuccess( mSock, mChar, ourTarg )
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

	var mSpell		= Spells[spellNum];
	var spellType	= 0;
	var sourceChar	= mChar;

	if( mSock != null )
	{
		spellType = mSock.currentSpellType;
	}

	mChar.SetTimer( Timer.SPELLTIME, 0 );
	mChar.spellCast = -1;

	// If player commandlevel is below GM-level, check for reagents
	if( mSock != null && mChar.commandlevel < GetCommandLevelVal( "CNS" ))
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

	if( !mChar.InRange( ourTarg, 10 ))
	{
		if( mSock != null )
		{
			mSock.SysMessage( GetDictionaryEntry( 712, mSock.language )); // You can't cast on someone that far away!
		}
		return;
	}

	if( !mChar.CanSee( ourTarg ))
		return;

	var attackTarget = false;
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
			if( mSock )
			{
				mSock.SysMessage( GetDictionaryEntry( 709, mSock.language )); // You can't cast in town!
			}
			return;
		}
		if( !ourTarg.vulnerable || ourTarg.aiType == 17 )
		{
			if( mSock )
			{
				mSock.SysMessage( GetDictionaryEntry( 713, mSock.language )); // They are invulnerable merchants!
			}
			return;
		}

		attackTarget = true;

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
	if(( mChar.commandlevel < GetCommandLevelVal( "CNS" )) && ( !mChar.CheckSkill( 25, lowSkill, highSkill )))
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
		mChar.mana 		= mChar.mana - mSpell.mana;
		mChar.health 	= mChar.health - mSpell.health;
		mChar.stamina	= mChar.stamina - mSpell.stamina;
	}
	if( !mChar.npc && spellType == 0 )
	{
		TriggerEvent( 6004, "DeleteReagents", mChar, mSpell );
	}

	if( attackTarget )
	{
		sourceChar.InitiateCombat( ourTarg );
	}

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

	DoTempEffect( 0, sourceChar, ourTarg, 3, Math.round( mChar.skills.magery / 100 ), 0, 0 );
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
