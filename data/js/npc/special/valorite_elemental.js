// Valorite Elementals reflect back part of any physical damage dealt
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( damageType == 1 ) // Physical damage
	{
		if( ValidateObject( attacker ))
		{
			// Reflect about half of the physical damage done back to the attacker
			var dmgToReflect = RandomNumber(( damageValue / 4 ), ( damageValue / 4 ) * 3 );
			attacker.Damage( dmgToReflect, 1, damaged );
		}
	}
	return true;
}