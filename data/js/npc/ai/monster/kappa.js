function onCombatStart( pKappa, pDefender )
{
	if(IsNearWater( pKappa ))
	{
		pKappa.SetTag( "NearWater", true );
	}
	else
	{
		pKappa.SetTag( "NearWater", false );
	}
}

function onAttack( pKappa, pDefender )
{
	// If Kappa is near water, drain life and heal Kappa
	if( pKappa.GetTag( "NearWater" ) == true )
	{
		var drainAmount = RandomNumber( 10, 19 );

		// Deal physical damage to the player, specifying the Kappa as the attacker
		pDefender.Damage( drainAmount, 1, pKappa );
		pKappa.Heal( drainAmount );
		pKappa.TextMessage( "The Kappa drains life from its foe, drawing strength from the water!" );

		if( pDefender.socket != null ) 
		{
			pDefender.SysMessage( "You feel your life being drained!" );
		}
	}

	// Simulate the Kappa bowing, causing it to release its corrosive liquid
	if( IsForcedToBow( pKappa ))
	{
		pKappa.TextMessage( "The Kappa bows, releasing its corrosive liquid!" );

		if( pDefender.socket != null )
		{
			pDefender.SysMessage( "The corrosive liquid from the Kappa damages you!" );
		}

		// Deal heat (fire) damage to both the player and the Kappa
		var damage = RandomNumber( 15, 30 ); // Deal 15-30 heat damage
		pDefender.Damage( damage, 5, pKappa ); // 5 = Heat/Fire damage

		// Kappa takes half the damage
		pKappa.Damage( damage / 2, 5 ); // No attacker specified, since it damages itself

		// Weaken the Kappa
		var strengthLoss = pKappa.strength * ( RandomNumber( 20, 30 ) / 100 ); // Reduce strength by a random percentage between 20% and 30%
		pKappa.strength -= strengthLoss;

		if( pDefender.socket != null )
			pDefender.SysMessage("The Kappa looks visibly weakened after losing its liquid.");
	}
}

function IsNearWater( pKappa )
{
	var waterTileFound = false;

	// Check if the static tile the Kappa is standing on is a liquid or wet area
	if( CheckStaticFlag( pKappa.x, pKappa.y, pKappa.z, pKappa.worldnumber, 7 )) // TF_WET = 7
		waterTileFound = true;

	// Check if the dynamic tile the Kappa is standing on is a liquid or wet area
	if( !waterTileFound && CheckDynamicFlag( pKappa.x, pKappa.y, pKappa.z, pKappa.worldnumber, pKappa.instanceID, 7 )) // TF_WET = 7
		waterTileFound = true;

	// Loop through all water tile IDs (0x1797 to 0x17B2)
	if( !waterTileFound )
	{
		for( var tileID = 0x1797; tileID <= 0x17B2; tileID++ )
		{
			if(StaticInRange( pKappa.x, pKappa.y, pKappa.worldnumber, 3, tileID )) 
			{
				waterTileFound = true;
				break;
			}
		}
	}
	return waterTileFound;
}

function IsForcedToBow( pKappa )
{
	return RandomNumber( 1, 100 ) <= 10; // 10% chance to trigger the bowing action
}