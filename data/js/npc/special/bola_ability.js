// Allows NPC to swing a bola and dismount a mounted player/character
function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt )
{
	// Don't swing bola if attacker is invalid or dead
	if( !ValidateObject( pAttacker ) || pAttacker.dead )
		return;

	// Don't swing bola if defender is invalid or dead, or an offline player
	if( !ValidateObject( pDefender ) || pDefender.dead ||
		( !pDefender.npc && !pDefender.online ))
		return;

	// Don't swing bola if neither character is vulnerable to damage
	if( pAttacker == pDefender || !pAttacker.vulnerable || !pDefender.vulnerable )
		return;

	// Don't swing bola if defender is not mounted
	if( !pDefender.isonhorse && !pDefender.mounted )
		return;

	// Don't swing bola if out of range
	if( !pAttacker.InRange( pDefender, 8 ))
		return;

	// Don't swing bola if it's on cooldown
	if( pAttacker.GetTempTag( "bolaCD" ))
		return;

	// Don't swing bola if NPC doesn't have one in their inventory
	if( pAttacker.ResourceCount( 0x26ac, 0 ) == 0 )
		return;

	// Only swing bola based on a percentage chance (20%) per attack
	if( RandomNumber( 1, 100 ) <= 20 )
	{
		// Start cooldown timer on attacker
		pAttacker.SetTempTag( "bolaCD", true );
		pAttacker.StartTimer( 3000, 1, true );

		// Set timer for when NPC can move again
		pAttacker.SetTimer( Timer.MOVETIME, 3000 );

		var tempMsg = GetDictionaryEntry( 1683 ); // %s starts swinging a bola around
		pAttacker.TextMessage( tempMsg.replace(/%s/gi, pAttacker.name ), true, 0x3b2 );
		pAttacker.StartTimer( 5000, 3, true );
	}
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		if( !ValidateObject( timerObj ))
			return;

		var myTarget = timerObj.target;
		if( !ValidateObject( myTarget ) || !myTarget.isonhorse && !myTarget.mounted )
			return;

		// Knock character off their mount, and apply a cooldown to when they can move/do any actions again
		myTarget.frozen = true;
		myTarget.Dismount();
		myTarget.DoAction( 0x03, 0x01 );
		timerObj.DoAction( 0x00, 0x04 );
		DoMovingEffect( timerObj, myTarget, 0x26AC, 0x10, 0x00, false );

		// Apply peacemaking effect to pDefender for X seconds
		myTarget.setPeace = 2;

		// Start timer to return target to their senses
		myTarget.StartTimer( 2000, 2, true );
	}
	else if( timerID == 2 )
	{
		// Return the character to their senses, if they're still alive
		timerObj.frozen = false;
		if( timerObj.socket != null )
		{
			timerObj.TextMessage( GetDictionaryEntry( 9125, timerObj.socket.language ), false, 0x3b2, 0, timerObj.serial ); // You recover your senses.
		}
	}
	else if( timerID == 3 )
	{
		// Take bola ability off cooldown for NPC
		timerObj.SetTempTag( "bolaCD", null );
	}
}
