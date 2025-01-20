function onTimer(mChar, timerID) 
{
	mChar.isCasting = false;
	mChar.frozen = false;
	mChar.Refresh();

	if (mChar.npc)
	{
		onSpellSuccess(null, mChar, timerID);
	}
	else if (mChar.socket)
	{
		var spellNum = mChar.spellCast;
		if (spellNum == -1)
			return;

		mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[spellNum].recoveryDelay);
		onSpellSuccess(mChar.socket, mChar, 0);
	}
}

function onSpellSuccess(mSock, mChar, spellID)
{
	if (mChar.isCasting)
		return;

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

	if (mChar.npc || spellType != 2)
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

	mChar.SoundEffect(0xF5, true);
	mChar.SoundEffect(0x299, true);
	mChar.StaticEffect(0x37C4, 1, 25);

	AreaCharacterFunction("AreaEffect", mChar, 4, mChar.socket);
}

function AreaEffect(srcChar, targetChar, socket)
{
	var dispelSkill = TriggerEvent(800, "ComputePowerValue", srcChar, 2);

	var chivalrySkill = caster.skills.chivalry;
	var dispelDifficulty = 10; // Target's Dispel Difficulty
	var dispelFocus = 20; // Target's Dispel Focus

	// Ensure Dispel Focus is valid to avoid division by zero
	if (dispelFocus <= 0)
	{
		dispelFocus = 1; // Default to 1 if not properly defined
	}

	// Calculate the base dispel chance
	var dispelChance = (50.0 + (100 * (chivalrySkill - dispelDifficulty)) / (dispelFocus * 2)) / 100;

	// Adjust chance based on the computed dispel skill
	dispelChance *= dispelSkill / 100.0;

	if (dispelChance > RandomNumber(0, 1) && (targetChar.isDispellable || targetChar.GetTag("animated")))
	{
		targetChar.SoundEffect(0x201, true);
		targetChar.StaticEffect(0x3728, 8, 20);
		targetChar.Delete();
		return true;
	}

	if (targetChar.npc)
	{
		targetChar.wandertype = 6;
	}

	// Additional effects for transformed necromancers
	if (targetChar.socket && targetChar.skills.necromancy > 0)
	{
		// Calculate Stamina and Mana damage
		var karmaFactor = (srcChar.karma + 10000) / 20000;
		var staminaDamage = Math.round(10 * karmaFactor);
		var manaDamage = Math.round(15 * karmaFactor);

		targetChar.stamina -= staminaDamage;
		targetChar.mana -= manaDamage;
		return true;
	}

	return false;
}

function _restorecontext_() {}
