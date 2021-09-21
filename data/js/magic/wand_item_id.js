// Item Identification as cast by Wands/Gnarled Staffs
function SpellRegistration()
{
    RegisterSpell( 65, true );
}

function onSpellCast( mSock, mChar, directCast, spellNum )
{
    // Are we already casting?
    if( mChar.GetTimer( 6 ) != 0 )
    {
        if( mChar.isCasting )
        {
            // You are already casting a spell.
            mSock.SysMessage( GetDictionaryEntry( 762, mSock.language ) );
            return true;
        }
        else if( mChar.GetTimer( 6 ) > GetCurrentClock() )
        {
            // You must wait a little while before casting
            mSock.SysMessage( GetDictionaryEntry( 1638, mSock.language ) );
            return true;
        }
    }

    var mSpell = Spells[spellNum];
    var spellType = 2; // Wand

    // Set the spell as current spell being cast
    mChar.spellCast = spellNum;

    // Turn character visible if not already visible
    if( mChar.visible == 1 || mChar.visible == 2 )
        mChar.visible = 0;

    // Break caster's concentration (meditation skill)
    mChar.BreakConcentration( mSock );

    mChar.nextAct = 75;     // why 75?

    // Start the spellcast cooldown timer, which determines next time they can cast a spell
    var delay = mSpell.delay * 100;
    if( spellType == 0 && mChar.commandlevel < 2 ) // if they are a gm they don't have a delay :-)
    {
        mChar.SetTimer( 6, delay );
        mChar.frozen = true;
    }
    else
        mChar.SetTimer( 6, 0 );

    // Play casting anim, if caster is not on a horse!
    if( !mChar.isonhorse && mSpell.action != 0 )
    {
        var actionID = mSpell.action;
        if( mChar.isHuman || actionID != 0x22 )
            mChar.DoAction( actionID );
    }

    // Have caster speak the words of power for this spell
    var tempString = mSpell.mantra;
    if( tempString != "" )
    	mChar.TextMessage( tempString );
    mChar.isCasting = true;

    // Start the cast timer, after which a target cursor will be provided
    mChar.StartTimer( delay, spellNum, true );
    return true;
}

function onTimer( mChar, timerID )
{
    // Free caster from spellcasting action
    mChar.isCasting = false;
    mChar.frozen    = false;

    // Proceed to give caster a targeting cursor
    var mSock = mChar.socket;
    if( mSock )
        mSock.CustomTarget( 0, Spells[timerID].strToSay );
}

function onCallback0( mSock, ourTarg )
{
	var mChar = mSock.currentChar;
	if( ValidateObject( ourTarg ) && ourTarg.isItem )
	{
		if( ValidateObject( mChar ))
		{
			if( ourTarg.name2 != "" )
				onSpellSuccess( mSock, mChar, ourTarg, 0 );
			else
				mSock.SysMessage( )
		}
	}
	else
	{
		if( ValidateObject( mChar ))
		{
			mChar.SetTimer( Timer.SPELLTIME, 0 );
			mChar.isCasting = false;
			mChar.spellCast = -1;
			mChar.frozen = false;
		}
	}
}

function onSpellSuccess( mSock, mChar, ourTarg )
{
    if( mChar.isCasting )
        return;

    var spellNum    = mChar.spellCast;
    var mSpell      = Spells[spellNum];

    // Set spellcast cooldown for caster
    mChar.SetTimer( 6, 0 );
    mChar.spellCast = -1;

    // Play SFX associated with spell
    mChar.SoundEffect( mSpell.soundEffect, true );

    // Set a special variable we can check for in skill script to avoid skill checks
    mSock.tempInt2 = 42;

    // Identify the magic item
    TriggerEvent( 818, "onCallback0", mSock, ourTarg );
}

function onUseChecked( pUser, iUsed )
{
    pUser.CastSpell( 65 );
    return false;
}