function onSwing(iSwung, pAttacker, pSwingAt)
{
	UpdateQuiverCount(pAttacker);
	return true;
}

function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt ) 
{
	UpdateQuiverCount(pAttacker);
	return true;
}

function UpdateQuiverCount(pAttacker)
{
	// Check if the attacker has a bow or crossbow equipped
	var weapon = pAttacker.FindItemLayer(0x2);// Assuming 1 is the layer for weapon (adjust if necessary)
	var isBow = weapon.id == 0x13B2 || weapon.id == 0x13B1; // IDs for bows
	var isCrossbow = weapon.id == 0x13FD || weapon.id == 0x13FC; // IDs for crossbows

	// If neither a bow nor a crossbow is equipped, return early
	if (!isBow && !isCrossbow) 
	{
		return true;
	}

	// Check if there is a quiver (or any item) equipped on the cloak layer (assumed to be layer 14)
	var cloakItem = pAttacker.FindItemLayer(0x14);

	// If there is no item equipped on the cloak layer, or it's not a container, return early
	if (!ValidateObject(cloakItem) || !cloakItem.container) 
	{
		return true;
	}

	// Check for ammo type in the quiver
	var ammoType = cloakItem.GetTag("AmmoType");

	var totalAmmoCount = 0;
	for (var mItem = cloakItem.FirstItem(); !cloakItem.FinishedItems(); mItem = cloakItem.NextItem()) {
		if ((ammoType == "Arrow" && mItem.id == 0x0f3f) || (ammoType == "Bolt" && mItem.id == 0x1bfb)) {
			totalAmmoCount += mItem.amount;
		}
	}
	if (totalAmmoCount != 0) 
	{
		cloakItem.SetTag("AmmoCount", totalAmmoCount - 1);
		cloakItem.Refresh();
	}
	return true;
}

function _restorecontext_() {}