function onCallback0( socket, ourTarg )
{
	var caster = socket.currentChar;
	if( !ValidateObject( caster ))
		return;

	if( ValidateObject( ourTarg ) && ourTarg.isChar )
	{
		onSpellSuccess( socket, caster, ourTarg );
	}
	else
	{
		socket.SysMessage( "That is not a valid target." );
		caster.SetTimer( Timer.SPELLTIME, 0 );
		caster.isCasting = false;
		caster.spellCast = -1;
		caster.frozen = false;
		caster.Refresh();
	}
}

function onSpellSuccess( socket, caster, target )
{
	caster.SetTimer( Timer.SPELLTIME, 0 );
	caster.spellCast = -1;

	if( caster.npc || ( socket != null && caster.commandlevel < 2 ))
	{
		TriggerEvent( 6004, "DeleteReagents", caster, Spells[caster.spellCast] );
	}

	if( !caster.InRange( target, 12 ) || !caster.CanSee( target ))
	{
		if( socket != null )
			socket.SysMessage( "You are too far away or cannot see your target." );
		return;
	}

	var numCurses = CountCurses( target );

	if( numCurses == 0 )
	{
		if( socket != null )
		{
			socket.SysMessage( "That target has no curses to remove." );
		}
		return;
	}

	var skill = caster.skills.chivalry;
	var karma = caster.karma;

	var baseChance = ( skill / 10.0 ) + ( karma / 1000.0 );
	baseChance -= numCurses * 5;

	if( baseChance < 10 )
	{
		baseChance = 10;
	}

	if( baseChance > 95 )
	{
		baseChance = 95;
	}

	if( RandomNumber( 0, 100 ) <= baseChance )
	{
		RemoveCurses( target );
		target.SoundEffect( 0x01E1, true );
		target.StaticEffect( 0x373A, 0, 15 );
		if( socket != null )
		{
			socket.SysMessage( "You have cleansed the target of curses!" );
		}
	}
	else
	{
		if( socket != null )
		{
			socket.SysMessage( "You failed to remove the curses." );
		}
	}
}

function CountCurses( target )
{
	var count = 0;

	if( target.GetTempEffect( 1 ) > 0 )
		count++; // Paralyze
	if( target.GetTempEffect( 3 ) > 0 )
		count++; // Clumsy
	if( target.GetTempEffect( 4 ) > 0 )
		count++; // Feeblemind
	if( target.GetTempEffect( 5 ) > 0 )
		count++; // Weaken
	if( target.GetTempEffect( 12 ) > 0 )
		count++; // Curse

	if( target.GetTag( "CorpseSkin"))
		count++;
	if( target.GetTag( "EvilOmen"))
		count++;
	if( target.GetTag( "MindRot"))
		count++;
	if( target.GetTag( "Strangle"))
		count++;
	if( target.GetTag( "BloodOath"))
		count++;
	if( target.GettempTag( "blockHeal" )) // Mortal Strike
		count++;

	return count;
}

function RemoveCurses(target)
{
	target.ReverseTempEffect(1);  // Paralyze
	target.ReverseTempEffect(3);  // Clumsy
	target.ReverseTempEffect(4);  // Feeblemind
	target.ReverseTempEffect(5);  // Weaken
	target.ReverseTempEffect(12); // Curse

	target.SetTag( "CorpseSkin", null );
	target.SetTag( "EvilOmen", null );
	target.SetTag( "MindRot", null );
	target.SetTag( "Strangle", null );
	target.SetTag( "BloodOath", null );
	target.SetTempTag( "blockHeal", null ); // Mortal Strike
}

function onTimer( mChar, timerID )
{
	mChar.isCasting = false;
	mChar.frozen = false;
	mChar.Refresh();

	if( mChar.npc )
	{
		var target = mChar.target;
		if( target && target.isChar )
			onSpellSuccess(null, mChar, target);
	}
	else
	{
		var sock = mChar.socket;
		if( sock != null )
		{
			var spellNum = mChar.spellCast;
			if( spellNum == -1 )
				return;

			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[spellNum].recoveryDelay);
			sock.CustomTarget( 0, Spells[timerID].strToSay, 2 );
		}
	}
}

function _restorecontext_() { }