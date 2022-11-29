// Immunity to Physical damage, aka standard melee damage
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( damageType == 1 ) // Physical damage
	{
		if( ValidateObject( attacker ))
		{
			// Creature is immune to Physical damage, so return false
			var socket = attacker.socket;
			if( socket != null )
				socket.SysMessage( GetDictionaryEntry( 9129, socket.language )); // That target is immune to physical damage!
		}
		return false;
	}

	return true;
}