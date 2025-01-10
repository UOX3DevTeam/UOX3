function SpellRegistration() 
{
	RegisterSpell(201, true);
	RegisterSpell(202, true);
}

function SpellTimerCheck(mChar, mSock)
{
	if (mChar.GetTimer(Timer.SPELLTIME) != 0)
	{
		if (mChar.isCasting)
		{
			if (mSock)
			{
				mSock.SysMessage(GetDictionaryEntry(762, mSock.language)); // You are already casting a spell.
			}
			return false;
		}
		else if (mChar.GetTimer(Timer.SPELLTIME) > GetCurrentClock())
		{
			if (mSock) 
			{
				mSock.SysMessage(GetDictionaryEntry(1638, mSock.language)); // You must wait a little while before casting
			}
			return false;
		}
	}
	return true;
}

function JailTimerCheck(mChar, mSock)
{
	if (mChar.isJailed && mChar.commandlevel < 2) 
	{
		mSock.SysMessage(GetDictionaryEntry(704, mSock.language)); // You are in jail and cannot cast spells!
		mChar.SetTimer(Timer.SPELLTIME, 0);
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}
	return true;
}

function SpellEnableCheck(mChar, mSock, mSpell) 
{
	if (!mSpell.enabled)
	{
		if (mSock)
		{
			mSock.SysMessage(GetDictionaryEntry(707, mSock.language)); // That spell is currently not enabled.
		}
		mChar.SetTimer(Timer.SPELLTIME, 0);
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}
	return true;
}

function ItemInHandCheck(mChar, mSock, spellType)
{
	// The following loop checks to see if any item is currently equipped (if not a GM)
	if (mChar.commandlevel < 2)
	{
		if (spellType != 2)
		{
			var itemRHand = mChar.FindItemLayer(0x01);
			var itemLHand = mChar.FindItemLayer(0x02);
			var lHandBlocks = false;
			var rHandBlocks = false;

			// Evaluate blocking for left and right hand items
			if (!isSpellCastingAllowed(itemRHand) || !isSpellCastingAllowed(itemLHand))
			{
				var result = AutoUnequipAttempt(itemLHand, itemRHand, mChar);
				lHandBlocks = result.lHandBlocks;
				rHandBlocks = result.rHandBlocks;
			}

			if (lHandBlocks || rHandBlocks)
			{
				if (mSock != null)
				{
					mSock.SysMessage(GetDictionaryEntry(708, mSock.language)); // You cannot cast with a weapon equipped.
				}

				if (!mChar.isCasting)
				{
					mChar.SetTimer(Timer.SPELLTIME, 0);
					mChar.isCasting = false;
					mChar.spellCast = -1;
				}
				return false;
			}
		}
	}
	return true;
}

function onSpellCast(mSock, mChar, directCast, spellNum)
{
	// Are we recovering from another spell that was just cast
	if (mChar.GetTimer(Timer.SPELLRECOVERYTIME) != 0)
	{
		if (mChar.GetTimer(Timer.SPELLRECOVERYTIME) > GetCurrentClock())
		{
			if (mSock != null)
			{
				mSock.SysMessage(GetDictionaryEntry(1638, mSock.language)); // You must wait a little while before casting
			}
			return true;
		}
	}

	// Are we already casting?
	if (!SpellTimerCheck(mChar, mSock))
		return true;

	var mSpell = Spells[spellNum];
	var spellType = 0

	// Fetch spelltype; 0 = normal spell, 1 = scroll, 2 = wand
	if (mSock != null)
	{
		spellType = mSock.currentSpellType;
	}

	mChar.spellCast = spellNum;

	if (!JailTimerCheck(mChar, mSock))
		return true;

	// Region checks
	var ourRegion = mChar.region;
	if ((spellNum == 45 && ourRegion.canMark) || (spellNum == 52 && !ourRegion.canGate()) || (spellNum == 32 && !ourRegion.canRecall()))
	{
		if (mSock != null)
		{
			mSock.SysMessage(GetDictionaryEntry(705, mSock.language)); // This is not allowed here.
		}
		mChar.SetTimer(Timer.SPELLTIME, 0);
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return true;
	}

	if (mSpell.aggressiveSpell)
	{
		if (ourRegion.isSafeZone)
		{
			if (mSock != null)
			{
				mSock.SysMessage(GetDictionaryEntry(1799, mSock.language)); // Hostile actions are not permitted in this safe area.
			}
			mChar.SetTimer(Timer.SPELLTIME, 0);
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return;
		}

		if (!ourRegion.canCastAggressive)
		{
			if (mSock != null)
			{
				mSock.SysMessage(GetDictionaryEntry(706, mSock.language)); // This is not allowed in town.
			}
			mChar.SetTimer(Timer.SPELLTIME, 0);
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return true;
		}
	}

	if (!SpellEnableCheck(mChar, mSock, mSpell))
		return true;

	if (!ItemInHandCheck(mChar, mSock, spellType))
		return true;

	if (mChar.visible == 1 || mChar.visible == 2)
	{
		mChar.visible = 0;
	}
	if (mSock != null)
	{
		mChar.BreakConcentration(mSock);
	}
	if (mChar.commandlevel < 2)
	{
		// type == 2 - Wands
		if (spellType != 2) {
			if (mSpell.mana > mChar.mana)
			{
				if (mSock != null)
				{
					mSock.SysMessage(GetDictionaryEntry(696, mSock.language)); // You have insufficient mana to cast that spell.
				}
				mChar.SetTimer(Timer.SPELLTIME, 0);
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
			if (mSpell.stamina > mChar.stamina)
			{
				if (mSock != null)
				{
					mSock.SysMessage(GetDictionaryEntry(697, mSock.language)); // You have insufficient stamina to cast that spell.
				}
				mChar.SetTimer(Timer.SPELLTIME, 0);
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
			if (mSpell.health >= mChar.health)
			{
				if (mSock != null)
				{
					mSock.SysMessage(GetDictionaryEntry(698, mSock.language)); // You have insufficient health to cast that spell.
				}
				mChar.SetTimer(Timer.SPELLTIME, 0);
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
			if (mSpell.tithing >= mChar.tithing) 
			{
				if (mSock != null)
				{
					mSock.SysMessage("You must have at least " + mSpell.tithing + " Tithing Points to use this ability,"); // You must have at least ~1_TITHE_REQUIREMENT~ Tithing Points to use this ability,
				}
				mChar.SetTimer(Timer.SPELLTIME, 0);
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return true;
			}
		}
	}

	mChar.nextAct = 75;		// why 75?

	var delay = mSpell.delay;
	if (spellType == 0 && mChar.commandlevel < 2) // if they are a gm they don't have a delay :-)
	{
		mChar.SetTimer(Timer.SPELLTIME, delay * 1000);
		if (!GetServerSetting("CastSpellsWhileMoving"))
		{
			mChar.frozen = true;
			mChar.Refresh();
		}
	}
	else
	{
		mChar.SetTimer(Timer.SPELLTIME, 0);
	}

	if (!mChar.isonhorse)
	{
		var actionID = mSpell.action;
		if (mChar.isHuman || actionID != 0x22)
		{
			mChar.DoAction(actionID);
		}
	}

	// Only human casters will say the spellcasting mantras
	if (mChar.isHuman)
	{
		var tempString;
		tempString = mSpell.mantra;
		mChar.TextMessage(tempString);
	}

	mChar.isCasting = true;

	mChar.StartTimer(delay * 1000, spellNum, true);

	return true;
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
		mChar.SetTimer(Timer.SPELLTIME, 0);
		mChar.isCasting = false;
		mChar.spellCast = -1;
		mChar.frozen = false;
		mChar.Refresh();
	}
}

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
			else if (spellNum == 202) // Heal
			{
				cursorType = 2; // Friendly cursor type
			}

			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[spellNum].recoveryDelay);
			mSock.CustomTarget( 0, Spells[timerID].strToSay, cursorType );
		}
	}
}

function onSpellSuccess(mSock, mChar, ourTarg, spellID)
{

	if (mChar.isCasting)
		return;

	var spellNum = mChar.spellCast;
	if (spellNum == -1) 
	{
		if (spellID != -1) 
		{
			spellNum = spellID;
		}
		else
		{
			return;
		}
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

	// Cut the casting requirement on scrolls
	var lowSkill, highSkill;
	if (spellType == 1)
	{
		lowSkill = mSpell.scrollLow;
		highSkill = mSpell.scrollHigh;
	}
	else
	{
		lowSkill = mSpell.lowSkill;
		highSkill = mSpell.highSkill;
	}

	// Do skillcheck
	if ((mChar.commandlevel < 2) && (!mChar.CheckSkill(51, lowSkill, highSkill))) 
	{
		if (spellType == 0) 
		{
			TriggerEvent(6004, "DeleteReagents", mChar, mSpell);
			mChar.SpellFail();
			mChar.SetTimer(Timer.SPELLTIME, 0);
			mChar.isCasting = false;
			mChar.spellCast = -1;
			return;
		}
	}

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

	var targRegion = ourTarg.region;
	if (mSpell.aggressiveSpell) 
	{
		if (targRegion.isSafeZone) 
		{
			if (mSock != null)
			{
				mSock.SysMessage(GetDictionaryEntry(1799, mSock.language)); // Hostile actions are not permitted in this safe area.
			}
			return;
		}
		if (!targRegion.canCastAggressive)
		{
			if (mSock != null)
			{
				mSock.SysMessage(GetDictionaryEntry(709, mSock.language)); // You can't cast in town!
			}
			return;
		}
		if (!ourTarg.vulnerable || ourTarg.aiType == 17)
		{
			if (mSock != null)
			{
				mSock.SysMessage(GetDictionaryEntry(713, mSock.language)); // They are invulnerable merchants!
			}
			return;
		}

		if (mSpell.reflectable)
		{
			if (ourTarg.magicReflect)
			{
				ourTarg.magicReflect = false;
				ourTarg.StaticEffect(0x373A, 0, 15);
				sourceChar = ourTarg;
				ourTarg = mChar;
			}
		}
	}

	if (spellNum != 5)
	{
		if (sourceChar.npc)
		{
			ourTarg.SoundEffect(mSpell.soundEffect, true);
		}
		else
		{
			sourceChar.SoundEffect(mSpell.soundEffect, true);
		}
		sourceChar.SpellMoveEffect(ourTarg, mSpell);
		ourTarg.SpellStaticEffect(mSpell);
	}

	// This is where the code actually executes ... all of this setup for a single line of code!
	DispatchSpell(spellNum, mSpell, sourceChar, ourTarg, mChar);
}

function DispatchSpell(spellNum, mSpell, sourceChar, ourTarg, caster) 
{
	var chivalrySkill = caster.skills.chivalry;
	if (spellNum == 201)// cleanse by fire
	{
		// Check if the target is poisoned or diseased
		if (!ourTarg.poison /*&& !ourTarg.diseased*/)
		{
			if (caster.socket != null)
			{
				caster.socket.SysMessage("That creature is not poisoned!");
			}
			return;
		}

		// Chance to cure calculation
		//var chivalrySkill = sourceChar.baseskills[51]; // Chivalry skill index
		var poisonLevel = ourTarg.poison; // Poison level of the target
		var chanceToCure = 10000 + (chivalrySkill * 75) - ((poisonLevel + 1) * 2000);
		chanceToCure /= 100;

		if (chanceToCure > RandomNumber(0, 100))
		{
			// Successfully cured
			ourTarg.SetPoisoned(0, 0);
			//ourTarg.diseased = false;
			ourTarg.StaticEffect(0x373A, 0, 15); // Cleansing visual effect
			ourTarg.SoundEffect(0x1E0, true);

			if (caster.socket != null)
			{
				if (ourTarg == caster)
				{
					caster.socket.SysMessage("You have been cured of all poisons.");
				}
				else
				{
					caster.socket.SysMessage("You have cured the target of all poisons!");
				}
			}
		}
		else
		{
			if (caster.socket != null)
			{
				caster.socket.SysMessage("You have failed to cure your target!");
			}
		}

		var damage = CalculateDamage(caster);

		if (caster.health > damage)
		{
			caster.Damage(damage, caster); // Apply damage to the caster
		}
		else 
		{
			caster.health = 1; // Ensure the caster stays alive
		}

		//DoTempEffect(0, sourceChar, ourTarg, 4, Math.round(chivalrySkill / 100), 0, 0);
	}
	else if (spellNum == 202)// Close Wounds
	{
		if (caster.npc) 
		{
			ourTarg = caster;
		}
		
		if (ourTarg.dead) 
		{
			if (caster.socket != null) 
			{
				caster.socket.SysMessage("You cannot heal that which is not alive.");
			}
			return;
		}
		else if (ourTarg.health >= ourTarg.health)
		{
			if (caster.socket != null)
			{
				caster.socket.SysMessage("That being is not damaged!");
				return;
			}
		}
		else if (ourTarg.poison)
		{
			if (caster.socket != null) 
			{
				if (ourTarg == caster)
				{
					caster.socket.SysMessage("You cannot heal yourself in your current state.");
					return;
				}
				else 
				{
					caster.socket.SysMessage("You cannot heal that target in their current state.");
					return;
				}
			}
		}

		var healingAmount = CalculateHealing(caster);
		ourTarg.health += healingAmount;

		if (ourTarg.murderer)
		{
			caster.criminal = true;
		}

		if (ourTarg.socket != null)
		{
			ourTarg.socket.SysMessage("You have had " + healingAmount + " hit points of damage healed.");
		}

		ourTarg.SoundEffect(0x202, true);
		ourTarg.StaticEffect(0x376A, 1, 62);
		ourTarg.StaticEffect(0x3779, 1, 46);
	}
}

function CalculateHealing(mChar) 
{
	var div = 6;
	var minHeal = 7;
	var maxHeal = 39;

	// Compute the base healing value
	var toHeal = ComputePowerValue(mChar, div) + RandomNumber(0, 2);

	// Apply caps to the healing amount
	if (toHeal < minHeal)
	{
		toHeal = minHeal;
	} else if (toHeal > maxHeal)
	{
		toHeal = maxHeal;
	}

	// Ensure healing does not exceed character's max health
	if ((mChar.hp + toHeal) > mChar.hpMax) 
	{
		toHeal = mChar.hpMax - mChar.hp;
	}

	return toHeal;
}

function CalculateDamage(mChar)
{
	var maxDamage = 55;
	var minDamage = 13;
	var div = 4;

	// Compute the power value
	var powerValue = ComputePowerValue(mChar, div);

	// Calculate the damage based on the power value
	var damage = maxDamage - powerValue;

	// Apply caps to damage
	if (damage < minDamage)
	{
		damage = minDamage;
	}
	else if (damage > maxDamage)
	{
		damage = maxDamage;
	}

	return damage;
}

// Calculate damage to the caster based on Karma and Chivalry skill
function ComputePowerValue(mChar, div) 
{
	if (!mChar) {
		return 0;
	}

	// Calculate the power value based on Karma and Chivalry skill
	var karmaAdjusted = mChar.karma + 20000;
	var chivalrySkill = mChar.baseskills[51];
	var v = Math.sqrt(karmaAdjusted + (chivalrySkill * 10));

	return Math.floor(v / div);
}

// Function to check if an equipped item allows casting
function isSpellCastingAllowed(item)
{
	return item != null && (item.type == 9 || item.type == 100 || item.type == 119); // Assuming type 9 is spellbook, type 100 paladin book and type 119 is spell channeling item
}

// Function to handle items
function AutoUnequipAttempt(itemLHand, itemRHand, mChar)
{
	const autoUnequip = GetServerSetting("AutoUnequippedCasting");
	var lHandBlocks = false; // Default to false
	var rHandBlocks = false; // Default to false
	if (itemLHand != null && !isSpellCastingAllowed(itemLHand)) 
	{
		if (autoUnequip && mChar.pack.totalItemCount < mChar.pack.maxItems) 
		{
			itemLHand.container = mChar.pack;
		}
		else
		{
			lHandBlocks = true; // Set to true if item is blocking
		}
	}

	if (itemRHand != null && !isSpellCastingAllowed(itemRHand))
	{
		if (autoUnequip && mChar.pack.totalItemCount < mChar.pack.maxItems)
		{
			itemRHand.container = mChar.pack;
		} 
		else
		{
			rHandBlocks = true; // Set to true if item is blocking
		}
	}
	return { lHandBlocks: lHandBlocks, rHandBlocks: rHandBlocks };
}

function _restorecontext_() {}
