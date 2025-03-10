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

	// Holy Light area effect
	AreaCharacterFunction("AreaEffect", mChar, 4, mSock);
}

function AreaEffect(caster, targetChar, socket)
{
	if (targetChar != caster || ValidateObject(targetChar) || !targetChar.dead || targetChar.health >= 0)
	{
		// Calculate base damage based on caster's karma
		var karma = caster.karma;
		var baseDamage = 8 + Math.round((karma / 10000) * (24 - 8));

		// Ensure damage is within bounds
		baseDamage = Math.max(8, Math.min(24, baseDamage));

		// Check resistances and calculate final damage
		var spellResisted = TriggerEvent(6000, "CheckResist", caster, targetChar, 0); // Circle 0 for simplicity
		var finalDamage = TriggerEvent(6001, "CalcFinalSpellDamage", caster, targetChar, baseDamage, spellResisted);

		// Apply energy damage
		TriggerEvent(800, "MagicDamage", targetChar, finalDamage, caster, socket, 6);// 6 represents energy damage type

		caster.StaticEffect(0x376A, 0x1, 0x29);
		caster.StaticEffect(0x37C4, 0x1, 0x29);
		caster.SoundEffect(0x212, true);

		return true;
	}
}

function _restorecontext_() {}
