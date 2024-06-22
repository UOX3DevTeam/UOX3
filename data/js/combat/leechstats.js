function onEquip( pEquipper, iEquipped ) 
{
	pEquipper.AddScriptTrigger( 7003 );
}

// Remove script trigger on unequip
function onUnequip( pUnequipper, iUnequipped )
{
	pUnequipper.RemoveScriptTrigger( 7003 );
}

function onDamageDeal( attacker, damaged, damageValue, damageType ) 
{
	// Fetch weapon in main hand or secondary hand
	var iWeapon = attacker.FindItemLayer( 0x01 );
	if( !ValidateObject( iWeapon )) 
	{
		iWeapon = attacker.FindItemLayer( 0x02 );
	}

	if( ValidateObject( iWeapon ))
	{	// Apply leech effects based on weapon properties
		ApplyLeech( attacker, damaged, damageValue, iWeapon, 'healthLeech', 30 );
		ApplyLeech( attacker, damaged, damageValue, iWeapon, 'staminaLeech', 100 );
		ApplyLeech( attacker, damaged, damageValue, iWeapon, 'manaLeech', 40 );
	}

	return true;
}

function ApplyLeech( attacker, damaged, damageValue, weapon, leechType, multiplier )
{	
	// Get the leech amount for the specified leech type from the weapon
	var leechPercentVal = weapon[ leechType ];
	if( leechPercentVal > 0 ) 
	{   
		// Calculate the percent of health restored to the attacker
		var leechAmt = Math.round( damageValue * ( leechPercentVal / 100 ) * ( multiplier/100 ));

		// Apply the leech effect based on the leech type
		switch( leechType ) 
		{
			case 'healthLeech':
				attacker.Heal( leechAmt );
				break;
			case 'staminaLeech':
				attacker.stamina += leechAmt;
				break;
			case 'manaLeech':
				attacker.mana += leechAmt;
				break;
		}

		attacker.SoundEffect( 0x44D, true );
	}
}