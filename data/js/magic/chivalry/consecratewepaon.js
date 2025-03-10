function onTimer(mChar, timerID) 
{
	mChar.isCasting = false;
	mChar.frozen = false;
	mChar.Refresh();

	if (mChar.npc) 
	{
		onSpellSuccess(null, mChar, ourTarg, timerID);
	} 
	else
	{
		var mSock = mChar.socket;
		if (mSock != null)
		{
			var spellNum = mChar.spellCast;
			if (spellNum == -1)
				return

			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[spellNum].recoveryDelay);
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

	castConsecrateWeapon(mChar, mSpell, spellType);
}

function castConsecrateWeapon(mChar, mSpell, spellType)
{
	// Ensure the caster is holding a weapon
	var weapon = mChar.FindItemLayer(0x01); // Right-hand weapon
	if (!weapon)
	{
		mChar.SysMessage("You must be holding a weapon to cast this spell.");
		return false;
	}

	// Calculate spell duration based on Karma
	var duration = 5000 + Math.floor((mChar.karma / 10000) * 6000); // 5-11 seconds in milliseconds

	// Calculate chance to hit weakest resist
	var skillLevel = mChar.skills.chivalry; // Chivalry skill
	var hitWeakestResistChance = (skillLevel >= 800) ? 100 : Math.max(0, 50 + Math.floor((skillLevel - 500) * 0.2));

	// Calculate damage increase for skill levels above 90
	var damageIncrease = 0;
	if (skillLevel >= 900)
	{
		damageIncrease = Math.floor(((skillLevel - 900) / 10) / 2); // Correctly scale for 10x skill level
	}

	// Add tags to the character using SetTag
	mChar.SetTag("consecrateDamageBonus", String(damageIncrease));
	mChar.SetTag("consecrateWeakResistChance", String(hitWeakestResistChance));

	// Start a timer to remove the effect after the duration ends
	mChar.AddScriptTrigger(6111);
	mChar.StartTimer(duration, 0, 6111);

	var buffDescription = hitWeakestResistChance.toFixed(1) + "\t" + damageIncrease.toFixed(1);
	TriggerEvent(50104, "AddBuff", mChar, 1082, 1151385, 1151386, duration / 1000, buffDescription);

	// Play visual effects and sound
	mChar.StaticEffect(0x376A, 0x1, 0x29);
	mChar.StaticEffect(0x37C4, 0x1, 0x29);
	mChar.SoundEffect(0x212, true);

	mChar.SysMessage("Your weapon is now consecrated.");
	return true;
}

function _restorecontext_() {}
