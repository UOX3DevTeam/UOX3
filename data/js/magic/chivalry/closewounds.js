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
			var spellNum = mChar.spellCast;
			if (spellNum == -1)
				return;

			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[spellNum].recoveryDelay);
			mSock.CustomTarget( 0, Spells[timerID].strToSay, 2 );
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
		if (DoesEventExist(2507, "onSpellTarget"))
		{
			if (TriggerEvent(2507, "onSpellTarget", ourTarg, mChar, mChar.spellCast) != false)
			{
				return;
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

	if (mSock != null)
	{
		spellType = mSock.currentSpellType;
	}

	mChar.SetTimer(Timer.SPELLTIME, 0);
	mChar.spellCast = -1;

	// If player commandlevel is below GM-level, check for reagents
	if (mSock != null && mChar.commandlevel < 2)
	{
		//Check for enough reagents
		// type == 0 -> SpellBook
		if (spellType == 0 && !TriggerEvent(6004, "CheckReagents", mChar, mSpell))
		{
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

	if (mChar.npc)
	{
		ourTarg = mChar;
	}
	if (mChar.InRange(ourTarg, 2) && mChar.CanSee(ourTarg) && Math.abs(mChar.z - ourTarg.z) < 4) 
	{
		if (mSock != null)
		{
			mSock.SysMessage("You are too far away to perform that action!");
		}
		return;
	}
	else if (ourTarg.dead)
	{
		if (mSock != null) 
		{
			mSock.SysMessage("You cannot heal that which is not alive.");
		}
		return;
	}
	else if (ourTarg.health >= ourTarg.maxhp)
	{
		if (mSock != null)
		{
			mSock.SysMessage("That being is not damaged!");
			return;
		}
	}
	else if (ourTarg.poison)
	{
		if (mSock != null)
		{
			if (ourTarg == mChar)
			{
				mSock.SysMessage("You cannot heal yourself in your current state.");
				return;
			}
			else 
			{
				mSock.SysMessage("You cannot heal that target in their current state.");
				return;
			}
		}
	}

	var healingAmount = TriggerEvent(6004, "CalculateHealing", mChar);
	ourTarg.health += healingAmount;

	if (ourTarg.socket != null)
	{
		ourTarg.socket.SysMessage("You have had " + healingAmount + " hit points of damage healed.");
	}

	ourTarg.SoundEffect(0x202, true);
	ourTarg.StaticEffect(0x376A, 1, 62);
	ourTarg.StaticEffect(0x3779, 1, 46);
}

function _restorecontext_() {}
