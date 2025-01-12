function onTimer(mChar, timerID) 
{
	mChar.isCasting = false;
	mChar.frozen = false;
	mChar.Refresh();

	if (mChar.npc) 
	{
		var ourTarg = mChar.target;
		if (ourTarg && ourTarg.isChar)
		{
			onSpellSuccess(null, mChar, ourTarg, timerID);
		}
	} 
	else
	{
		var mSock = mChar.socket;
		if (mSock != null)
		{
			var cursorType = 0;
			var spellNum = mChar.spellCast;
			if (spellNum == -1)
				return;

			if (Spells[spellNum].aggressiveSpell)
			{
				cursorType = 1; // Hostile cursor type
			}
			else if (spellNum == 202 || spellNum == 204) // Heal
			{
				cursorType = 2; // Friendly cursor type
			}

			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[spellNum].recoveryDelay);
			mSock.CustomTarget( 0, Spells[timerID].strToSay, cursorType );
		}
	}
}

function onCallback0(mSock, ourTarg) 
{
	var mChar = mSock.currentChar;
	if (!ValidateObject(mChar))
		return;

	if (ValidateObject(ourTarg) && ourTarg.isChar)
	{
		if (ourTarg != mChar && mChar.spellCast != -1)
		{
			if (DoesEventExist(2507, "onSpellTarget"))
			{
				if (TriggerEvent(2507, "onSpellTarget", ourTarg, mChar, mChar.spellCast) != false)
				{
					return;
				}
			}
		}

		onSpellSuccess(mSock, mChar, ourTarg, 0);
	}
	else
	{
		mSock.SysMessage("here 2 ");
		mChar.SetTimer(Timer.SPELLTIME, 0);
		mChar.isCasting = false;
		mChar.spellCast = -1;
		mChar.frozen = false;
		mChar.Refresh();
	}
}

function onSpellSuccess(mSock, mChar, ourTarg, spellID)
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
	var sourceChar = mChar;

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

	if (!mChar.InRange(ourTarg, 10))
	{
		if (mSock != null)
		{
			mSock.SysMessage(GetDictionaryEntry(712, mSock.language)); // You can't cast on someone that far away!
		}
		return;
	}

	if (!mChar.CanSee(ourTarg))
		return;

	var chivalrySkill = mChar.skills.chivalry;
	// Check if the target is poisoned or diseased
	if (!ourTarg.poison /*&& !ourTarg.diseased*/)
	{
		if (mSock != null)
		{
			mSock.SysMessage("That creature is not poisoned!");
		}
		return;
	}

	// Chance to cure calculation
	var poisonLevel = ourTarg.poison; // Poison level of the target
	var chanceToCure = 10000 + (chivalrySkill * 75) - ((poisonLevel + 1) * 2000);
	chanceToCure /= 100;

	if (chanceToCure > RandomNumber(0, 100)) 
	{
		// Successfully cured
		ourTarg.SetPoisoned(0, 0);
		//ourTarg.diseased = false;
		ourTarg.StaticEffect(0x373A, 0, 15);
		ourTarg.SoundEffect(0x1E0, true);

		if (mSock != null)
		{
			if (ourTarg == mChar)
			{
				mSock.SysMessage("You have been cured of all poisons.");
			}
			else
			{
				mSock.SysMessage("You have cured the target of all poisons!");
			}
		}
	}
	else
	{
		if (mSock != null) 
		{
			mSock.SysMessage("You have failed to cure your target!");
		}
	}

	var damage = TriggerEvent(6004, "CalculateDamage", mChar);

	if (mChar.health > damage)
	{
		mChar.Damage(damage, mChar);
	}
	else
	{
		mChar.health = 1;
	}
}

function _restorecontext_() {}
