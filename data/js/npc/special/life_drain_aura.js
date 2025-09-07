/// <reference path="../../definitions.d.ts" />
// @ts-check
// For creatures that can drain life of nearby players/pets when they are hit in combat
/** @type { ( damaged: Character, attacker: Character, damageValue: number, damageType: WeatherType ) => boolean } */
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

		switch( srcObj.sectionID )
		{
			case "succubus": // Succubus
				hpDrain = RandomNumber( 5, 10 );

				// Double drain from male characters
				//if( trgChar.gender == 0 || trgChar.gender == 2 || trgChar.gender == 4 )
					//hpDrain *= 2;
				drainFX = 0x374a;
				drianFXLength = 0x0f;
				drainFXHue = 0x496;
				drainSFX = 0x231;
				break;
			case "tentacles_of_the_harrower": // Tentacles of the Harrower
				hpDrain = RandomNumber( 14, 30 );

				drainFX = 0x374a;
				drianFXLength = 0x0f;
				drainFXHue = 0x455;
				drainSFX = 0x1F1;
				break;
			default:
				return false;
		}

		if( parseInt( trgChar.GetTag( "activeBalmLotion" )) == 5 )
		{
			// Target character has an active Life Shield Lotion effect
			// Reduce effect of hp drain by 50-100%
			hpDrain -= RandomNumber( Math.round( hpDrain / 2 ), hpDrain );
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
