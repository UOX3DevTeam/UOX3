function onDamage(damaged, attacker, damageValue, damageType)
{
	if( !ValidateObject( damaged ) || !ValidateObject( attacker ))
		return true;

	// On first hit, summon Silvani
	if( damaged.GetTag( "SilvaniSummoned" ) != 1 )
	{
		damaged.TextMessage( "Come forth my Queen!" );
		let x = damaged.x + 1;
		let y = damaged.y + 1;
		let z = damaged.z;
		let map = damaged.worldNumber;
		let instance = damaged.instanceID;

		let queen = SpawnNPC( "silvani", x, y, z, map, instance, false );
		if( ValidateObject( queen ))
		{
			queen.SetTag( "LordOaksSerial", damaged.serial.toString() );
			damaged.SetTag( "SilvaniSerial", queen.serial.toString() );
		}

		damaged.SetTag( "SilvaniSummoned", 1 );
	}

	// If Silvani is alive, reduce damage drastically
	let silvaniSerial = parseInt( damaged.GetTag( "SilvaniSerial" ));
	if( silvaniSerial )
	{
		let silvani = CalcCharFromSer( silvaniSerial );

		// 20% chance to summon Pixies
		if( RandomNumber( 1, 100 ) <= 20 )
		{
			TriggerEvent( 3232, "SummonPixiesOn", silvani, attacker );
		}

		if( ValidateObject( silvani ))
		{
			let reducedDamage = Math.ceil( damageValue * 0.1 ); // 90% damage reduction
			damaged.Damage( reducedDamage, damageType, attacker, true );
			return false; // prevent default damage
		}
	}

	return true; // full damage if Queen is dead or not found
}

// Handle draining of health, stamina and mana on attack
function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt )
{
	if( !ValidateObject( pDefender ))
		return;

	if( RandomNumber( 1, 100 ) <= 25 ) // 25% chance to drain
	{
		// Amount to drain from target
		var drainAmount = RandomNumber( 20, 30 );

		switch( RandomNumber( 1, 3 )) // 33% chance to drain either health, stamina or mana
		{
			case 1: // Health
				if( pDefender.health >= drainAmount )
				{
					pAttacker.health += drainAmount;
				}
				else
				{
					pAttacker.health += pDefender.health;
				}
				pDefender.Damage( drainAmount, 1 );
				pAttacker.TextMessage( GetDictionaryEntry( 9069 ), false, 0, 5, pDefender.serial ); // I can grant life, and I can sap it as easily.
				break;
			case 2: // Stamina
				if( pDefender.stamina >= drainAmount )
				{
					pAttacker.stamina += drainAmount;
				}
				else
				{
					pAttacker.stamina += pDefender.stamina;
				}
				pDefender.stamina -= drainAmount;
				pAttacker.TextMessage( GetDictionaryEntry( 9070 ), false, 0, 5, pDefender.serial ); // You'll go nowhere, unless I deem it should be so.
				break;
			case 3: // Mana
				if( pDefender.mana >= drainAmount )
				{
					pAttacker.mana += drainAmount;
				}
				else
				{
					pAttacker.mana += pDefender.mana;
				}
				pDefender.mana -= drainAmount;
				pAttacker.TextMessage( GetDictionaryEntry( 9071 ), false, 0, 5, pDefender.serial ); // Your power is mine to use as I will.
				break;
			default:
				break;
		}
	}
}

function _restorecontext_() {}
