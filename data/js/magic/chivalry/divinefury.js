function onTimer(mChar, timerID)
{
	mChar.isCasting = false;
	mChar.frozen = false;
	mChar.Refresh();

	if (timerID == 1)
	{
		// Revert the temporary effects
		var skillLevel = mChar.baseskills.chivalry; // Chivalry skill
		var swingSpeedBonus = Math.floor((skillLevel - 500) / 50);
		var hitChanceBonus = Math.floor((skillLevel - 500) / 50);
		var defensePenalty = -Math.floor((skillLevel - 500) / 50);

		const coreShardEra = GetServerSetting("CoreShardEra");
		const isPostPublish71 = (EraStringToNum(coreShardEra) >= EraStringToNum("hs"));

		if (isPostPublish71 && skillLevel >= 1200 && mChar.karma > 10000)
		{
			// Revert Publish 71 bonuses
			swingSpeedBonus = 15;
			hitChanceBonus = 15;
			defensePenalty = -10;
		}

		mChar.swingSpeedIncrease -= swingSpeedBonus;
		mChar.hitChance -= hitChanceBonus;
		mChar.defenseChance -= defensePenalty;

		// Remove active state tag
		mChar.SetTag("DivineFuryActive", null);

		mChar.Refresh();
		TriggerEvent(50104, "RemoveBuff", mChar, 1010);
		mChar.SysMessage("The effects of Divine Fury have worn off.");
	}
	else if (mChar.npc)
	{
		onSpellSuccess(null, mChar, ourTarg, timerID);
	}
	else
	{
		var mSock = mChar.socket;
		if (mSock != null)
		{
			var spellNum = mChar.spellCast;
			var mSpell = Spells[spellNum];
			if (spellNum == -1)
				return

			mChar.SetTimer(Timer.SPELLRECOVERYTIME, mSpell.recoveryDelay);
			onSpellSuccess(mSock, mChar, 0);
		}
		else
		{
			mChar.SetTimer(Timer.SPELLTIME, 0);
			mChar.isCasting = false;
			mChar.spellCast = -1;
			mChar.frozen = false;
			mChar.Refresh();
		}
	}
}

function onSpellSuccess(mSock, mChar, spellID)
{
	if (mChar.isCasting)
	{
		return;
	}

	var spellNum = (mChar.spellCast != -1) ? mChar.spellCast : spellID;
	if (spellNum == -1)
	{
		return;
	}

	var mSpell = Spells[spellNum];
	var spellType = 0;

	if (mSock != null)
	{
		spellType = mSock.currentSpellType;
	}

	mChar.SetTimer(Timer.SPELLTIME, 0);
	mChar.spellCast = -1;

	// If player's command level is below GM-level, check for reagents
	if (mSock && mChar.commandlevel < 2)
	{
		// Check for enough reagents (type == 0 -> SpellBook)
		if (spellType == 0 && !TriggerEvent(6004, "CheckReagents", mChar, mSpell))
		{
			// Reset casting state
			mChar.SetTimer(Timer.SPELLTIME, 0);
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return;
		}
	}

	TriggerEvent(800, "SpellCasting", mChar, mSpell, spellType)

	// Deduct resources
	if (mChar.npc || spellType !== 2)
	{
		mChar.mana -= mSpell.mana;
		mChar.health -= mSpell.health;
		mChar.stamina -= mSpell.stamina;
		mChar.tithing -= mSpell.tithing;
	}

	if (!mChar.npc && spellType == 0) 
	{
		TriggerEvent(6004, "DeleteReagents", mChar, mSpell);
	}

	castDivineFury(mChar, mSpell, spellType);
}

// Function to cast Divine Fury
function castDivineFury(mChar, mSpell, spellType)
{
	// Check era for updated effects
	const coreShardEra = GetServerSetting("CoreShardEra");
	const isPostPublish71 = (EraStringToNum(coreShardEra) >= EraStringToNum("hs"));

	// Calculate spell duration using ComputePowerValue for Karma and Chivalry skill
	var skillLevel = mChar.baseskills.chivalry; // Chivalry skill
	var duration = TriggerEvent(800, "ComputePowerValue", mChar, 10);
	duration = Math.max(7, Math.min(duration, 24)); // Cap duration between 7 and 24 seconds

	// Calculate stamina refresh amount using ComputePowerValue
	var staminaRefresh = TriggerEvent(800, "ComputePowerValue", mChar, 20);
	mChar.stamina = Math.min(mChar.stamina + staminaRefresh, mChar.maxstamina); // Ensure it doesn't exceed max stamina

	// Define bonuses and penalties
	var swingSpeedBonus = Math.floor((skillLevel - 500) / 50); // Example calculation for swing speed bonus
	var hitChanceBonus = Math.floor((skillLevel - 500) / 50); // Example calculation for hit chance bonus
	var defensePenalty = -Math.floor((skillLevel - 500) / 50); // Example calculation for defense penalty
	var damageIncrease = 0;//TODO: Add Damage Increase Here"

	if (isPostPublish71 && skillLevel >= 1200 && mChar.karma > 10000)
	{
		// Apply Publish 71 bonuses
		swingSpeedBonus = 15;
		hitChanceBonus = 15;
		//damageIncrease = 20; // Subject to 100% DI cap
		defensePenalty = -10;
	}

	mChar.swingSpeedIncrease += swingSpeedBonus;
	mChar.hitChance += hitChanceBonus;
	mChar.defenseChance += defensePenalty;

	// Add Buff
	var buffDescription = swingSpeedBonus + "\t" + damageIncrease + "\t" + hitChanceBonus + "\t" + defensePenalty;
	TriggerEvent(50104, "AddBuff", mChar, 1010, 1060589, 1150218, duration, buffDescription);

	// Play visual effects
	mChar.StaticEffect(0x376A, 0x1, 0x29);

	// Start a timer to remove the effects after the duration ends
	mChar.StartTimer(duration * 1000, 1, 6105);

	// Mark Divine Fury as active
	mChar.SetTag("DivineFuryActive", "true");

	mChar.SysMessage("You feel a surge of divine fury coursing through you.");
	return true;
}

function _restorecontext_() {}
