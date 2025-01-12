function onTimer(mChar, timerID) 
{
	mChar.isCasting = false;
	mChar.frozen = false;
	mChar.Refresh();

	if (mChar.npc) 
	{
		onSpellSuccess(null, mChar, timerID);
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

	//var sacrifice = false;
	AreaCharacterFunction("ResurrectCureHealEffect", mChar, 6, mSock);
}

function ResurrectCureHealEffect(caster, targetChar, socket)
{
	var sendEffect = false;
	var sacrifice = false;
	if (targetChar != caster || ValidateObject(targetChar))
	{
		if (targetChar.dead)
		{
			if (targetChar.region && targetChar.region.name == "Khaldun")
			{
				caster.TextMessage("The veil of death in this area is too strong and resists thy efforts to restore life.");
			}
			else
			{
				targetChar.Resurrect();
				targetChar.StaticEffect(0x376A, 10, 16);
				targetChar.SoundEffect(0x214, true);
				sacrifice = true;
			}
		}
		else
		{
			if (targetChar.poison && targetChar.CurePoison(caster))
			{
				if (caster != targetChar)
				{
					caster.TextMessage("You have cured the target of all poisons!");
				}
				targetChar.TextMessage("You have been cured of all poisons.");
				sendEffect = true;
				sacrifice = true;
			}

			if (targetChar.health < targetChar.maxhp)
			{
				var healAmount = TriggerEvent(800, "CalculateHealing", caster)
				targetChar.health = Math.min(targetChar.health + healAmount, targetChar.maxhp);
				targetChar.TextMessage("You feel rejuvenated as your wounds heal!");
				sendEffect = true;
			}

			if (targetChar.frozen)
			{
				targetChar.frozen = false;
				sendEffect = true;
			}

			if (sendEffect)
			{
				targetChar.StaticEffect(0x375A, 1, 15);
			}
		}
	}

	if (sacrifice)
	{
		var sound = 0x423;
		if (caster.gender == 1 && caster.commandlevel < 2)
		{
			sound = 0x150;
		}
		caster.SoundEffect(sound, true);
		caster.health = 1;
		caster.stamina = 1;
		caster.mana = 1;
	}
	return true;
}

function _restorecontext_() {}
