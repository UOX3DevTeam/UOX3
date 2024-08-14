function onEquip(pEquipper, iEquipped) 
{
	if (!pEquipper.HasScriptTrigger(5062))
	{
		pEquipper.AddScriptTrigger(5062);
	}
	return false;
}

function onUnequip(pUnequipper, iUnequipped)
{
	pUnequipper.RemoveScriptTrigger(5062);
	return false;
}

function onDropItemOnItem(iDropped, cDropper, iDroppedOn)
{
	// Prevent the quiver from being dropped onto itself
	if (iDropped.id == 0x2FB7 && iDroppedOn.id == 0x2FB7) 
	{
		cDropper.SysMessage("You cannot drop the quiver onto itself.");
		return false;
	}

	// Allow the quiver to be dropped into any container that is not itself
	if (iDropped.id == 0x2FB7) 
	{
		return true;
	}

	// Initialize AmmoCount and WeightReduction tags if they don't exist
	if (iDroppedOn.GetTag("AmmoCount") == null)
	{
		iDroppedOn.SetTag("AmmoCount", 0);
	}

	if (iDroppedOn.GetTag("WeightReduction") == null)
	{
		iDroppedOn.SetTag("WeightReduction", 50); // Example: 50% weight reduction
	}

	// Retrieve the type of ammo currently in the quiver
	var ammo = iDroppedOn.GetTag("AmmoType");
	var ammoCount = iDroppedOn.GetTag("AmmoCount");
	var weightReduction = iDroppedOn.GetTag("WeightReduction");

	// Check if adding the dropped ammo would exceed the maximum limit
	var maxAmmoCount = 500; // Define maximum ammo count
	if (ammoCount + iDropped.amount > maxAmmoCount)
	{
		cDropper.SysMessage("The quiver cannot hold more than 500 units of ammo.");
		return false;
	}

	// Check if the item being dropped is an arrow
	if ((iDropped.id == 0x0f3f || iDropped.sectionID == "0x0f3f") && (ammo == "" || ammo == "Arrow"))
	{
		iDroppedOn.SetTag("AmmoType", "Arrow");
		ammoCount += iDropped.amount; // Add the amount of dropped arrows to the count
		iDroppedOn.SetTag("AmmoCount", ammoCount);

		// Apply weight reduction
		iDropped.weight -= iDropped.weight * (weightReduction / 100);
		iDropped.Refresh();
		iDroppedOn.Refresh();
		return true;
	}
	// Check if the item being dropped is a bolt
	else if ((iDropped.id == 0x1bfb || iDropped.sectionID == "0x1bfb") && (ammo == "" || ammo == "Bolt"))
	{
		iDroppedOn.SetTag("AmmoType", "Bolt");
		ammoCount += iDropped.amount; // Add the amount of dropped bolts to the count
		iDroppedOn.SetTag("AmmoCount", ammoCount);

		// Apply weight reduction
		iDropped.weight -= iDropped.weight * (weightReduction / 100);
		iDropped.Refresh();
		iDroppedOn.Refresh();
		return true;
	}
	// If the ammo type doesn't match or the quiver already holds a different type of ammo
	else 
	{
		cDropper.SysMessage("The container cannot hold that type of object.");
		return false;
	}
}

function onContRemoveItem(iCont, iItem, pChar)
{
	// Validate the container and item objects
	if (!ValidateObject(iCont) || !ValidateObject(iItem))
		return;

	// Check if the container is a quiver (id 0x2FB7)
	if (iCont.id == 0x2FB7)
	{
		var ammoType = iCont.GetTag("AmmoType");
		var ammoCount = iCont.GetTag("AmmoCount");
		var weightReduction = iCont.GetTag("WeightReduction");

		// Check if the item being removed matches the ammo type
		if ((ammoType == "Arrow" && iItem.id == 0x0f3f) || (ammoType == "Bolt" && iItem.id == 0x1bfb)) 
		{
			// Restore the original weight
			iItem.weight += iItem.weight * (weightReduction / (100 - weightReduction));
			iItem.Refresh();

			// Decrease the ammo count by the amount being removed
			ammoCount -= iItem.amount;

			// Update the AmmoCount tag
			iCont.SetTag("AmmoCount", ammoCount);

			// If ammo count is zero or less, reset the AmmoType and AmmoCount tags
			if (ammoCount <= 0)
			{
				iCont.SetTag("AmmoType", "");
				iCont.SetTag("AmmoCount", 0);
				pChar.SysMessage("The quiver is now empty and can hold a different type of ammo.");
			}
		}
	}
}

function onTooltip(equipment, pSocket)
{
	var weightReduction = parseInt(equipment.GetTag("WeightReduction")); // Default to 0 if not set
	var ammoCount = equipment.GetTag("AmmoCount"); // Default to 0 if not set
	var ammoType = equipment.GetTag("AmmoType");
	var tooltipText = "";

	// Add weight reduction information
	tooltipText = "Weight reduction: " + weightReduction + "%";

	// Add ammo information
	if (ammoType != null && ammoType != "") 
	{
		tooltipText += "\n" + "Ammo: " + ammoCount + "/500 " + ammoType + "s";
	}
	else
	{
		tooltipText += "\n" + "Ammo: " + ammoCount + "/500";
	}

	return tooltipText;
}