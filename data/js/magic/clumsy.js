function SpellRegistration()
{
	RegisterSpell( 1, true );	// say, clumsy, same as in the spells.dfn file
}

function onSpellCast( mSock, mChar, directCast, spellNum )
{
	// Are we already casting?
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

	var mSpell	= Spells[spellNum];
	var spellType 	= mSock.currentSpellType;

	mChar.spellCast = spellNum;

	if( mChar.isJailed && mChar.commandLevel < 2 )
	{
		mSock.SysMessage( GetDictionaryEntry( 704, mSock.Language ) );
		mChar.SetTimer( 6, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}
	
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
	
	if( !mSpell.enabled )
	{
		mSock.SysMessage( GetDictionaryEntry( 707, mSock.Language ) );
		mChar.SetTimer( 6, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
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
	
	if( mChar.visible == 1 || mChar.visible == 2 )
		mChar.visile = 0;

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
		mChar.mana 	= mChar.mana - mSpell.mana;
		mChar.health 	= mChar.health - mSpell.health;
		mChar.stamina	= mChar.stamina - mSpell.stamina;
	}
	else if( !mChar.npc && spellType == 0 )
		deleteReagents( mChar, mSpell );

	if( !mChar.InRange( ourTarg, 10 ) )
	{
		if( mSock )
			mSock.SysMessage( GetDictionaryEntry( 712, mSock.Language ) ); 
		return;
	}

//	if( !mChar.LineOfSight( mSock, ourTarg.x, ourTarg.y, ourTarg.z ) )
//	{
//		return;
//	}
	
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

	DoTempEffect( 0, sourceChar, ourTarg, 3, (mChar.skills.magery / 100), 0, 0);	
}
