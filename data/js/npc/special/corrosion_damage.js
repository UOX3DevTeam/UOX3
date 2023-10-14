// Acidic creatures have a chance to corrode the weapons of its attackers
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( damageType == 1 ) // Physical damage
	{
		if( !ValidateObject( attacker ))
			return true;

		// Find weapon of attacking player
		var weaponInHand = attacker.FindItemLayer( 0x01 ); // Item in right hand - 1h weapon

		if( !ValidateObject( weaponInHand ) || weaponInHand.hidamage == 0  )
		{
			weaponInHand = attacker.FindItemLayer( 0x02 ); // Item in left hand - 2h weapon/shield
		}

		// If it's a weapon...
		if( ValidateObject( weaponInHand ) && weaponInHand.hidamage > 0 )
		{
			// ... and a melee weapon...
			var weaponType = TriggerEvent( 2500, "GetWeaponType", attacker, null );
			if( weaponType == "WRESTLING" || weaponType == "BOWS" || weaponType == "XBOWS" || weaponType == "BLOWGUNS" || weaponType == "THROWN" )
				return true;

			// ... apply some corrosive damage to the weapon!
			var rnd = RandomNumber( 1, 4 );
			if( rnd == 3 )
			{
				if( weaponInHand.health > 0 )
				{
					weaponInHand.health--;
					weaponInHand.Refresh();
					if( attacker.socket != null )
						attacker.TextMessage( GetDictionaryEntry( 9057, attacker.socket.language ), false, 0x03b2 ); // *Splashing acid blood scars your weapon!*
				}
			}
		}
	}
	return true;
}