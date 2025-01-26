// Handle Colossal Blow special ability - a stunning attack that applies a peacemaking effect
function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt )
{
	// Don't perform scolossal blow if attacker is invalid or dead
	if( !ValidateObject( pAttacker ) || pAttacker.dead )
		return;

	// Don't perform scolossal blow if defender is invalid or dead, or an offline player
	if( !ValidateObject( pDefender ) || pDefender.dead ||
		( !pDefender.npc && !pDefender.online ))
		return;

	// Don't perform the scolossal blow if neither character is vulnerable to damage
	if( pAttacker == pDefender || !pAttacker.vulnerable || !pDefender.vulnerable )
		return;

	// Don't perform the colossal blow if out of range
	if( !pAttacker.InRange( pDefender, 1 ))
		return;

	// Don't perform the colossal blow if it's on cooldown
	if( pAttacker.GetTempTag( "colossalBlowCD" ))
		return;

	// Fetch some initial data based on type of NPC
	var stunProps = GetStunProps( pAttacker );
	if( RandomNumber( 1, 100 ) <= stunProps.hitChance )
	{
		// Start cooldown timer on attacker
		pAttacker.SetTempTag( "colossalBlowCD", true );
		pAttacker.StartTimer( stunProps.cooldown, 2, true );

		// Deal damage to defender, if applicable
		if( stunProps.maxDmg > 0 )
		{
			var stunDmg = RandomNumber( stunProps.minDmg, stunProps.maxDmg );
			pDefender.Damage( stunDmg, 1, pAttacker );
		}

		// Freeze pDefender in place, and start a timer for when they can move again
		pDefender.frozen = true;
		pDefender.StartTimer( 5000, 1, true );

		// Play hitFX for pDefender, if applicable
		DoMovingEffect( pDefender, pDefender, stunProps.hitFX, 0, stunProps.fxLength, stunProps.explode, stunProps.fxHue, stunProps.fxRender );
		pDefender.SoundEffect( stunProps.sfx, true );

		// Play fall-over animation for pDefender
		pDefender.DoAction( 21 );

		// Remove target
		pDefender.target = null;

		// Apply peacemaking effect to pDefender for X seconds
		pDefender.setPeace = stunProps.stunTime;

		// Display stunned message over affected character's head, if they're a player
		if( pDefender.socket != null )
			pDefender.TextMessage( GetDictionaryEntry( stunProps.dictID, pDefender.socket.language ), false, 0x3b2, 0, pDefender.serial ); // You have been stunned by a colossal blow!
	}
}

function GetStunProps( pAttacker )
{
	// hitChance, stunTime, manaCost, cooldown, sysMsgHue, dictID, hitFX, fxLength, fxHue, sfx, minDmg, maxDmg, dmgType
	var stunProps;

	// If multiple NPCs share the same ID, check for extra stuff like skin color, name, etc.
	switch( pAttacker.sectionID )
	{
		case "chaosdragoon":
		case "chaosdragoonelite":
		case "m_chaosdragoon": // mounted on armored swamp dragon
		case "m_chaosdragoonelite": // mounted on armored swamp dragon
		case "f_chaosdragoon": // mounted on armored swamp dragon
		case "f_chaosdragoonelite": // mounted on armored swamp dragon
			stunProps = { hitChance:20, stunTime:5, manaCost:0, cooldown:5000, sysMsgHue:0x096a, dictID:9124, hitFX:0x36fe, fxLength:0x0f, fxHue:0, fxRender:1, explode:false, sfx:0x236, minDmg:15, maxDmg:30, dmgType:1 };
			break;
		// Iron Golem?
		case "golem": // Golem
		case "craftedgolem": // Crafted Golem
		case "betrayer": // Betrayer
		default:
			// If NPCs listed above need special versions of it, remove fallthrough and give them a custom stunProps line!
			stunProps = { hitChance:20, stunTime:5, manaCost:0, cooldown:5000, sysMsgHue:0x096a, dictID:9124, hitFX:0x36fe, fxLength:0x0f, fxHue:0, fxRender:1, explode:false, sfx:0x236, minDmg:20, maxDmg:60, dmgType:1 };
			break;
	}

	return stunProps;
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		if( !ValidateObject( timerObj ))
			return;

		// Return the character to their senses, if they're still alive
		timerObj.frozen = false;
		if( timerObj.socket != null )
			timerObj.TextMessage( GetDictionaryEntry( 9125, timerObj.socket.language ), false, 0x3b2, 0, timerObj.serial ); // You recover your senses.
	}
	else if( timerID == 2 )
	{
		// Take colossal blow off cooldown for NPC
		timerObj.SetTempTag( "colossalBlowCD", null );
	}
}

function _restorecontext_() {}
