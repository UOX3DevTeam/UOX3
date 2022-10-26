// Throws orc lord at attackers who cast magic at him
// Dispells summoned creatures

const dispelSummonsDelay = 10;

function onSpellTarget( myTarget, myTargetType, pCaster, spellID )
{
	var spell = Spells[spellID];
	if( spell.aggressiveSpell )
	{
		// Check if an orc lord has been spawned already, and if it's still alive
		var orcLordsThrown = myTarget.GetTempTag( "orcLordsThrown" );
		var spawningOrcLord = myTarget.GetTempTag( "spawningOrcLord" );
		if( orcLordsThrown < 10 && !spawningOrcLord )
		{
			// Someone dares use magic against the orcish brute? Throw an Orc Lord!
			myTarget.SetTempTag( "spawningOrcLord", true );
			myTarget.SetTempTag( "orcLordTargSerial", ( pCaster.serial ).toString() );
			myTarget.StartTimer( 1000, 0, true );
		}
	}
	return false;
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	var targChar = CalcCharFromSer( parseInt( timerObj.GetTempTag( "orcLordTargSerial" )));
	if( timerID == 0 )
	{
		if( ValidateObject( targChar ) && !targChar.dead && timerObj.InRange( targChar, 10 ))
		{
			DoMovingEffect( timerObj.x, timerObj.y, timerObj.z + 5, targChar.x, targChar.y, targChar.z + 1, 0x25b0, 0x06, 0x00, true );
			timerObj.StartTimer( 500, 1, true );
		}
	}
	else if( timerID == 1 )
	{
		if( ValidateObject( targChar ) && !targChar.dead && timerObj.InRange( targChar, 10 ))
		{
			var nSpawned = SpawnNPC( "orclord", targChar.x, targChar.y, targChar.z, targChar.worldnumber, targChar.instanceID );
			if( ValidateObject( nSpawned ))
			{
				nSpawned.SetTempTag( "masterBruteSerial", ( timerObj.serial ).toString() );
				nSpawned.AddScriptTrigger( 3209 );
			}
			var orcLordsThrown = timerObj.GetTempTag( "orcLordsThrown" );
			timerObj.SetTempTag( "orcLordsThrown", orcLordsThrown + 1 );
			timerObj.SetTempTag( "orcLordTargSerial", null );
			timerObj.SetTempTag( "spawningOrcLord", null );
		}
	}
}

function onAISliver( mNPC )
{
	if( !mNPC.atWar )
		return;

	var nextDispelTime = mNPC.GetTempTag( "nextDispel" );
	if( nextDispelTime < GetCurrentClock() || nextDispelTime > GetCurrentClock() + ( dispelSummonsDelay * 2000 ))
	{
		// Look for nearby summoned creatures
		AreaCharacterFunction( "LookForSummonedCreatures", mNPC, 5 );

		// Set a "timer" for when to next be allowed to use the dispel
		nextDispelTime = (( RandomNumber( dispelSummonsDelay, dispelSummonsDelay * 2 ) * 1000 ) + GetCurrentClock() );
		mNPC.SetTempTag( "nextDispel", nextDispelTime );
	}
}

function LookForSummonedCreatures( mNPC, targChar )
{
	if( mNPC == targChar )
		return;

	if( targChar.npc && targChar.isDispellable )
	{
		DoStaticEffect( targChar.x, targChar.y, targChar.z, 0x372a, 0x9, 0x0a, false );
		targChar.SoundEffect( 0x0201, true );
		targChar.Delete();
	}
}