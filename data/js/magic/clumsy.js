function SpellRegistration()
{
	RegisterSpell( 1, true );	// say, clumsy, same as in the spells.dfn file
}

function onSpellCast( mSock, mChar, directCast, spellNum )
{
	// Are we already casting?
	if( mChar.GetTimer( Timer.SPELLTIME ) != 0 )
	{
		if( mChar.isCasting )
		{
			if( mSock != null )
			{
				mSock.SysMessage( GetDictionaryEntry( 762, mSock.language )); // You are already casting a spell.
			}
			return true;
		}
		else if( mChar.GetTimer( Timer.SPELLTIME ) > GetCurrentClock() )
		{
			if( mSock != null )
			{
				mSock.SysMessage( GetDictionaryEntry( 1638, mSock.language )); // You must wait a little while before casting
			}
			return true;
		}
	}

	var mSpell	= Spells[spellNum];
	var spellType = 0

	// Fetch spelltype; 0 = normal spell, 1 = scroll, 2 = wand
	if( mSock != null )
	{
		spellType = mSock.currentSpellType;
	}

	mChar.spellCast = spellNum;

	if( mChar.isJailed && mChar.commandlevel < 2 )
	{
		if( mSock != null )
		{
			mSock.SysMessage( GetDictionaryEntry( 704, mSock.language )); // You are in jail and cannot cast spells!
		}
		mChar.SetTimer( Timer.SPELLTIME, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return true;
	}

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

	if( !mSpell.enabled )
	{
		if( mSock != null )
		{
			mSock.SysMessage( GetDictionaryEntry( 707, mSock.language )); // That spell is currently not enabled.
		}
		mChar.SetTimer( Timer.SPELLTIME, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return true;
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

	// The following loop checks to see if any item is currently equipped (if not a GM)
	if( mChar.commandlevel < 2 )
	{
		if( spellType != 2 )
		{
			var itemRHand = mChar.FindItemLayer( 0x01 );
			var itemLHand = mChar.FindItemLayer( 0x02 );
			if( itemLHand || ( itemRHand && itemRHand.type != 9 ))	// Spellbook
			{
				if( mSock != null )
				{
					mSock.SysMessage( GetDictionaryEntry( 708, mSock.language )); // You cannot cast with a weapon equipped.
				}
				mChar.SetTimer( Timer.SPELLTIME, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
		}
	}

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
		//Check for enough reagents
		// type == 0 -> SpellBook
		if( spellType == 0 && !CheckReagents( mChar, mSpell ))
		{
			mChar.SetTimer( Timer.SPELLTIME, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return true;
		}

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

	if(( mChar.commandlevel < 2 ) && ( !mChar.CheckSkill( 25, lowSkill, highSkill )))
	{
		if( mChar.isHuman )
		{
			mChar.TextMessage( mSpell.mantra );
		}

		if( spellType == 0 )
		{
			DeleteReagents( mChar, mSpell );
			mChar.SpellFail();
			mChar.SetTimer( Timer.SPELLTIME, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return true;
		}
	}

	mChar.nextAct = 75;		// why 75?

	var delay = mSpell.delay;
	if( spellType == 0 && mChar.commandlevel < 2 ) // if they are a gm they don't have a delay :-)
	{
		mChar.SetTimer( Timer.SPELLTIME, delay * 1000 );
		if( !GetServerSetting( "CastSpellsWhileMOving" ))
		{
			mChar.frozen = true;
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
	mChar.isCasting = true;

	mChar.StartTimer( delay, spellNum, true );

	return true;
}

function CheckReagents( mChar, mSpell )
{
	var failedCheck = 0;
	if( mSpell.ash > 0 && mChar.ResourceCount( 0x0F8C ) < mSpell.ash )
	{
		failedCheck = 1;
	}
	if( mSpell.drake > 0 && mChar.ResourceCount( 0x0F86 ) < mSpell.drake )
	{
		failedCheck = 1;
	}
	if( mSpell.garlic > 0 && mChar.ResourceCount( 0x0F84 ) < mSpell.garlic )
	{
		failedCheck = 1;
	}
	if( mSpell.ginseng > 0 && mChar.ResourceCount( 0x0F85 ) < mSpell.ginseng )
	{
		failedCheck = 1;
	}
	if( mSpell.moss > 0 && mChar.ResourceCount( 0x0F7B ) < mSpell.moss )
	{
		failedCheck = 1;
	}
	if( mSpell.pearl > 0 && mChar.ResourceCount( 0x0F7A ) < mSpell.pearl )
	{
		failedCheck = 1;
	}
	if( mSpell.shade > 0 && mChar.ResourceCount( 0x0F88 ) < mSpell.shade )
	{
		failedCheck = 1;
	}
	if( mSpell.silk > 0 && mChar.ResourceCount( 0x0F8D ) < mSpell.silk )
	{
		failedCheck = 1;
	}
	if( failedCheck == 1 )
	{
		mChar.SysMessage( "You do not have enough reagents to cast that spell." );
		return false;
	}
	else
	{
		return true;
	}
}

function DeleteReagents( mChar, mSpell )
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

function onCallback0( mSock, ourTarg )
{
	if( ourTarg && ourTarg.isChar )
	{
		var mChar = mSock.currentChar;
		if( mChar )
		{
			onSpellSuccess( mSock, mChar, ourTarg );
		}
	}
}

function onSpellSuccess( mSock, mChar, ourTarg )
{
	if( mChar.isCasting )
		return;

	var spellNum	= mChar.spellCast;
	var mSpell		= Spells[spellNum];
	var spellType	= 0;
	var sourceChar	= mChar;

	if( mSock != null )
	{
		spellType = mSock.currentSpellType;
	}

	mChar.SetTimer( Timer.SPELLTIME, 0 );
	mChar.spellCast = -1;

	if( mChar.npc || spellType != 2 )
	{
		mChar.mana 		= mChar.mana - mSpell.mana;
		mChar.health 	= mChar.health - mSpell.health;
		mChar.stamina	= mChar.stamina - mSpell.stamina;
	}
	if( !mChar.npc && spellType == 0 )
	{
		DeleteReagents( mChar, mSpell );
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
	{
		return;
	}

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
