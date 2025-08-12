/// <reference path="../../definitions.d.ts" />
// @ts-check
// NPCs like Shadow Iron Elementals are immune to magic
/** @type { ( damaged: Character, attacker: Character, damageValue: number, damageType: WeatherType ) => void } */
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( damageType == 5 ) // Magical damage
	{
		if( ValidateObject( attacker ))
		{
			// Creature is immune to Magical damage, so return false
			var socket = attacker.socket;
			if( socket != null )
			{
				socket.SysMessage( GetDictionaryEntry( 9056, socket.language )); // That target is immune to magic!
			}
		}
		return false;
	}

	return true;
}
