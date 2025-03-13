// Function to handle timer expiration
function onTimer(timerObj, timerID)
{
	if (timerID == 0)
	{
		// Ensure the timer object is valid and is a character
		if (!timerObj || timerObj.GetTag("consecrateDamageBonus") === null)
		{
			return;
		}

		// Remove the consecrate tags
		timerObj.SetTag("consecrateDamageBonus", null);
		timerObj.SetTag("consecrateWeakResistChance", null);

		// Notify the character
		timerObj.SysMessage("The effects of Consecrate Weapon have worn off.");
		timerObj.RemoveScriptTrigger(6111);
		TriggerEvent(50104, "RemoveBuff", timerObj, 1082);
	}
}

// Function to modify damage
function onDamageDeal(attacker, damaged, damageValue, damageType)
{

	mChar.SysMessage("test2");

	if (!attacker.GetTag("consecrateDamageBonus"))
	{
		return true; // No effect if consecrate is not active
	}

	// Get tags as strings and parse as integers
	var damageBonus = parseInt(attacker.GetTag("consecrateDamageBonus"), 10);
	var weakResistChance = parseInt(attacker.GetTag("consecrateWeakResistChance"), 10);

	// Determine if weakest resist is used
	if (RandomNumber(0, 100) < weakResistChance)
	{
		var weakestResist = getWeakestResist(damaged); // Custom function to find weakest resist
		damageType = weakestResist;
	}

	// Apply bonus damage
	damageValue += Math.floor((damageValue * damageBonus) / 100);

	return true; // Allow damage to proceed with modifications
}

function getWeakestResist(targetChar)
{
	// Returns the weakest resist type of the target
	var resists = {
		1: targetChar.physicalResist,
		2: targetChar.lightResist,
		4: targetChar.coldResist,
		5: targetChar.fireResist,
		6: targetChar.energyResist,
	};

	var weakestType = 1;
	var weakestValue = resists[1];
	for (var type in resists)
	{
		if (resists[type] < weakestValue)
		{
			weakestValue = resists[type];
			weakestType = type;
		}
	}
	return weakestType;
}


function _restorecontext_() {}
