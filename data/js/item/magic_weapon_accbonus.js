// Accuracy bonus for magical items
// See magicweapons.js for details on full setup

function onEquip( pEquipper, iEquipped )
{
	// Get bonus values
	var accBonus = parseInt( iEquipped.GetTag( "accBonus" ));

	// Get weapontype
	var weaponType = TriggerEvent( 2500, "GetWeaponType", pEquipper, null );

	// If weapon is a ranged weapon, add to archery skill
	// If not, add to tactics
	if( weaponType == "BOWS" || weaponType == "XBOWS" )
	{
		pEquipper.baseskills.archery += accBonus;
	}
	else
	{
		pEquipper.baseskills.tactics += accBonus;
	}
}

function onUnequip( pEquipper, iEquipped )
{
	// Get bonus values
	var accBonus = parseInt( iEquipped.GetTag( "accBonus" ));

	// Get weapontype
	var weaponType = TriggerEvent( 2500, "GetWeaponType", pEquipper, null );

	// If weapon is a ranged weapon, add to archery skill
	// If not, add to tactics
	if( weaponType == "BOWS" || weaponType == "XBOWS" )
	{
		pEquipper.baseskills.archery -= accBonus;
	}
	else
	{
		pEquipper.baseskills.tactics -= accBonus;
	}
}