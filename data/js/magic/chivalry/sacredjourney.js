function onTimer(mChar, timerID) 
{
	mChar.isCasting = false;
	mChar.frozen = false;
	mChar.Refresh();

	var mSock = mChar.socket;
	if (mSock != null)
	{
		var spellNum = mChar.spellCast;
		if (spellNum == -1)
			return;

		mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[spellNum].recoveryDelay);
		mSock.CustomTarget(0, Spells[timerID].strToSay, 2);
	}
}

function onCallback0(mSock, ourTarg) 
{
	var mChar = mSock.currentChar;
	if (!ValidateObject(mChar))
		return;

	if (!mSock.GetWord(1) && ourTarg.isItem && mChar )
	{
		var txtMessage;
		if (ourTarg.type == 7)
		{
			// We targeted a key. For a house, or a boat?
			var shipSerial = ourTarg.more;
			var serialPart1 = (shipSerial >> 24);
			var serialPart2 = (shipSerial >> 16);
			var serialPart3 = (shipSerial >> 8);
			var serialPart4 = (shipSerial % 256);
			var shipMulti = CalcMultiFromSer(serialPart1, serialPart2, serialPart3, serialPart4);

			if (ValidateObject(shipMulti) && shipMulti.isBoat())
			{
				if (shipMulti.worldnumber == mSock.currentChar.worldnumber && shipMulti.instanceID == mSock.currentChar.instanceID)
				{
					mSock.currentChar.SetLocation(shipMulti.x + 1, shipMulti.y, shipMulti.z + 3);
				}
				else
				{
					mSock.SysMessage(GetDictionaryEntry(8093, mSock.language)); // You are unable to recall to your ship - it might be in another world!
				}
			}
			else
			{
				mSock.SysMessage(GetDictionaryEntry(8094, mSock.language)); // You can only recall off of valid ship keys.
			}
		}
		else if (ourTarg.morex <= 200 && ourTarg.morey <= 200)
		{
			txtMessage = GetDictionaryEntry(431, mSock.language); // That rune is not yet marked!
			mSock.SysMessage(txtMessage);
		}
		else
		{
			txtMessage = GetDictionaryEntry(682, mSock.language); // You have recalled from the rune.
			mSock.SysMessage(txtMessage);
			onSpellSuccess(mSock, mChar, ourTarg, 0)
		}
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

	mChar.SoundEffect(0x1FC, true);
	mChar.Recall(ourTarg);
}

function _restorecontext_() {}
