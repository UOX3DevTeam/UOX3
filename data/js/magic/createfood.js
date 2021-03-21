function SpellRegistration()
{
	RegisterSpell( 2, true );	// Create Food, ID from spells.dfn
}

function onSpellCast( mSock, mChar, directCast, spellNum )
{
	// Are we already casting?
	if( mChar.GetTimer( 6 ) != 0 )
	{
		if( mChar.isCasting )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 762, mSock.language ) );
			return true;
		}
		else if( mChar.GetTimer( 6 ) > GetCurrentClock() )
		{
			if( mSock )
				mSock.SysMessage( GetDictionaryEntry( 1638, mSock.language ) );
			return true;
		}
	}

	var mSpell	= Spells[spellNum];
	var spellType 	= mChar.currentSpellType;

	mChar.spellCast = spellNum;

	if( mChar.isJailed && mChar.commandlevel < 2 )
	{
		if( mSock )
			mSock.SysMessage( GetDictionaryEntry( 704, mSock.language ) );
		mChar.SetTimer( 6, 0 );
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return true;
	}

	if( !mSpell.enabled )
	{
		if( mSock )
			mSock.SysMessage( GetDictionaryEntry( 707, mSock.language ) );
		mChar.SetTimer( 6, 0 );
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
			if( itemLHand || ( itemRHand && itemRHand.type != 9 ) )	// Spellbook
			{
				if( mSock )
					mSock.SysMessage( GetDictionaryEntry( 708, mSock.language ) );
				mChar.SetTimer( 6, 0 );
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
		}
	}

	// Turn character visible
	if( mChar.visible == 1 || mChar.visible == 2 )
		mChar.visible = 0;

	// Break character's concentration (affects Meditation skill)
	if( mSock )
		mChar.BreakConcentration( mSock );

	// If player commandlevel is below GM-level, check for reagents
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

	// If player commandlevel is below GM-level, perform skillcheck
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

	var tempString = mSpell.mantra;

	mChar.TextMessage( tempString );
	mChar.isCasting = true;

	mChar.StartTimer( delay, spellNum, true );

	return true;
}

function checkReagents( mChar, mSpell )
{
	var failedCheck = 0;
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
		mChar.SysMessage( "You do not have enough reagents to cast that spell." );
		return false;
	}
	else
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
	var ourTarg = mChar;

	if( mChar.npc )
	{
		onSpellSuccess( null, mChar, ourTarg );
	}
	else
	{
		var mSock = mChar.socket;
		if( mSock )
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

	sourceChar.SoundEffect( spellNum, true );
	sourceChar.SpellMoveEffect( ourTarg, mSpell );
	ourTarg.SpellStaticEffect( mSpell );

	// List of foods to randomize between when casting the spell
	foodItems = new Array (
		"0x09d0", "0x09b7", "0x09f2", "0x097b", "0x0d1a", "0x09c9", "0x09eb", "0x09d2",
		"0x09c0", "0x097c"
	);
	var rndNum = RandomNumber( 0, foodItems.length );

	if( mSock )
	{
		CreateDFNItem( mSock, mChar, foodItems[rndNum], 1, "ITEM", true );
	}
	else
	{
		CreateDFNItem( NULL, mChar, foodItems[rndNum], 1, "ITEM", true );
	}
}
