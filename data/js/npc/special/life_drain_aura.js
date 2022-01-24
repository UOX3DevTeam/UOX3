// For creatures that can drain life of nearby players/pets when they are hit in combat
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( damageType == 1 ) // Physical damage
	{
		AreaCharacterFunction( "DealAreaDamage", damaged, 2 );
	}
	return true;
}

function DealAreaDamage( srcObj, trgChar )
{
	if( ValidateObject( trgChar ) && trgChar != srcObj && trgChar.vulnerable && !trgChar.dead
		&& (( trgChar.npc && ( trgChar.tamed || trgChar.hireling )) || trgChar.online ))
	{
		// Get amount of HP to drain
		var hpDrain = 0;
		var drainFX = 0;
		var drainFXHue = 0;
		var drainSFX = 0;
		var drainFXLength = 0;

		switch( trgChar.id )
		{
			case 0x0095: // Succubus
				hpDrain = RandomNumber( 5, 10 );

				// Double drain from male characters
				//if( trgChar.gender == 0 || trgChar.gender == 2 || trgChar.gender == 4 )
					//hpDrain *= 2;
				drainFX = 0x374a;
				drianFXLength = 0x0f;
				drainFXHue = 0x496;
				drainSFX = 0x231;
				break;
			default:
				return false;
		}

		// Play effects
		DoMovingEffect( trgChar, trgChar, drainFX, 0, drainFXLength, false, drainFXHue, 0 );
		trgChar.SoundEffect( drainSFX, true );

		// Restore HP for creature
		if( trgChar.health >= hpDrain )
			srcObj.health += hpDrain;
		else
			srcObj.health += trgChar.health;

		// Apply damage to target
		trgChar.Damage( hpDrain, 1 );

		if( trgChar.socket != null )
			trgChar.socket.SysMessage( GetDictionaryEntry( 9068, trgChar.socket.language )); // You feel your life force being stolen away!
		return true;
	}

	return false;
}