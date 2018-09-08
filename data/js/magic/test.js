// Let's test our spell casting scripts... what fun!

function SpellRegistration()
{
	RegisterSpell( 1, true );	// say, clumsy, same as in the spells.dfn file
}

function onSpellCast( socket, character, directCast, spellNum )
{
	if( directCast )
		character.TextMessage( "I'm trying to cast a spell directly!" );
	else
		character.TextMessage( "This is an indirect spell cast.. somebody stop me!" );

	var mSpell	= Spells[spellNum];
	var action	= mSpell.action;
	var delay	= mSpell.delay * 1000;
	var mantra	= mSpell.mantra;

	character.DoAction( action );
	character.StartTimer( delay, spellNum, true );
	character.TextMessage( mantra );
	character.isCasting = true;
	character.spellCast = spellNum;

//	* Check if already casting, if so, stop and dump out
//	* Check if frozen, if so, can't cast spell
//	* Check if enabled
//	* Check if scroll, wand or normal cast (scroll reduce requirements, wand do straight away)
//	* Check requirements (min int, dex, str)
//	* Check for reagants
//	* Check if can cast in this area
//	* Determine if the cast "succeeded"
//	* If so, fire onSpellSuccess, otherwise fire onSpellFailure.  If failed, speak and return
//	* Set the player as "casting" and freeze
//	* Create timer for when the spell will be complete
//	* Wait for timer to elapse
//	* Upon timer elapsing,
//	*	Unfreeze player
//	*	Send target identifier if any (based on timer) and wait for callback
//	*	If not target, do action (if any)
//	*	If target, create onCallback function
//	*	On callback, do stuff
//	* NOTE * For area affect spells, use an AreaCharacterFunction

	return true;	// we succeeded in casting (consumption for wand/scroll)
}

function onTimer( character, timerID )
{
	character.TextMessage( "Spell casting timer has elapsed, time to throw up other stuff, wheeee!" );
	character.isCasting = false;
	character.spellCast = 0;
	character.TextMessage( "Beware, beware... of the BATMAN!" );
}