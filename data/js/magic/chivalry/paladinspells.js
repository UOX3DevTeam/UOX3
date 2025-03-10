function SpellRegistration() 
{
	RegisterSpell(201, true);
	RegisterSpell(202, true);
	RegisterSpell(203, true);
	RegisterSpell(204, true);
	RegisterSpell(205, true);
	//RegisterSpell(206, true);
	RegisterSpell(207, true);
	RegisterSpell(208, true);
	//RegisterSpell(209, true);
	RegisterSpell(210, true);
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

function StatsCheck(mChar, mSock, spellType, mSpell)
{
	if (mChar.commandlevel < 2)
	{
		// type == 2 - Wands
		if (spellType != 2)
		{
			if (mSpell.mana > mChar.mana)
			{
				if (mSock != null)
				{
					mSock.SysMessage(GetDictionaryEntry(696, mSock.language)); // You have insufficient mana to cast that spell.
				}
				mChar.SetTimer(Timer.SPELLTIME, 0);
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return false;
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
				return false;
			}

			if (mSpell.health > mChar.health)
			{
				if (mSock != null)
				{
					mSock.SysMessage(GetDictionaryEntry(698, mSock.language)); // You have insufficient health to cast that spell.
				}
				mChar.SetTimer(Timer.SPELLTIME, 0);
				mChar.isCasting = false;
				mChar.spellCast = -1;
				return false;
			}

			if (mSpell.tithing > mChar.tithing)
			{
				if (mSock != null)
				{
					mSock.SysMessage("You must have at least " + mSpell.tithing + " Tithing Points to use this ability,"); // You must have at least ~1_TITHE_REQUIREMENT~ Tithing Points to use this ability,
				}
				mChar.SetTimer(Timer.SPELLTIME, 0);
				mChar.isCasting = false;
				mChar.spellCast = -1;
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

	if (spellNum == 210 && !GetServerSetting("TravelSpellsWhileAggressor") && mChar.IsAggressor())
	{
		mSock.SysMessage("You cannot use the Sacred Journey ability to flee from combat."); // You cannot use the Sacred Journey ability to flee from combat.
		return true;
	}

	if (spellNum == 210 && (mChar.criminal || mChar.murderer))
	{
		mSock.SysMessage("Thou'rt a criminal and cannot escape so easily."); // Thou'rt a criminal and cannot escape so easily.
		return true;
	}


	// Region checks
	var ourRegion = mChar.region;
	//if ((spellNum == 45 && ourRegion.canMark) || (spellNum == 52 && !ourRegion.canGate()) || ((spellNum == 32 || spellNum == 210) && !ourRegion.canRecall()))
	//{
	//	if (mSock != null)
	//	{
	//		mSock.SysMessage(GetDictionaryEntry(705, mSock.language)); // This is not allowed here.
	//	}
	//	mChar.SetTimer(Timer.SPELLTIME, 0);
	//	mChar.isCasting = false;
	//	mChar.spellCast = -1;
	//	return true;
	//}

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

	if(!StatsCheck(mChar, mSock, spellType, mSpell))
		return true;

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

	var actionID = mSpell.action;
	if (!mChar.isonhorse && actionID > 0 )
	{
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

	// Check if Divine Fury is already active
	if (mChar.GetTag("DivineFuryActive") == "true")
	{
		mChar.SysMessage("Divine Fury is already active.");
		return false; // Prevent recasting
	}

	mChar.isCasting = true;
	mChar.SpellStaticEffect(mSpell);
	var triggerNum = 800;
	switch (spellNum)
	{
		case 201: triggerNum = 6101; break;// Cleanse by Fire
		case 202: triggerNum = 6102; break;// Close Wounds
		case 203: triggerNum = 6103; break;// Consecrate Weapon
		case 204: triggerNum = 6104; break;// Dispel Evil
		case 205: triggerNum = 6105; break;// Divine Fury
		//enemy of one
		case 207: triggerNum = 6107; break;// Holy Light
		case 208: triggerNum = 6108; break;// Noble Scrifice
		//remove curses
		case 210: triggerNum = 6110; break;// Sacred Journey
	}

	mChar.StartTimer(delay * 1000, spellNum, triggerNum);

	return true;
}

function RegionCheck(mSock, sourceChar, mSpell, ourTarg)
{
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
	}
	else if (toHeal > maxHeal)
	{
		toHeal = maxHeal;
	}

	// Ensure healing does not exceed character's max health
	if ((mChar.health + toHeal) > mChar.maxhp) 
	{
		toHeal = mChar.maxhp - mChar.health;
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

function CalculateHitLoc()
{
	var BODYPERCENT = 0;
	var ARMSPERCENT = 1;
	var HEADPERCENT = 2;
	var LEGSPERCENT = 3;
	var NECKPERCENT = 4;
	var OTHERPERCENT = 5;
	var TOTALTARGETSPOTS = 6;

	var LOCPERCENTAGES = new Array(44, 14, 14, 14, 7, 7);

	var hitLoc = RandomNumber(0, 99); // Determine area of Body Hit
	for (var t = BODYPERCENT; t < TOTALTARGETSPOTS; ++t)
	{
		hitLoc -= LOCPERCENTAGES[t];
		if (hitLoc < 0)
		{
			hitLoc = t + 1;
			break;
		}
	}
	return hitLoc;
}

function MagicDamage(p, amount, attacker, mSock, element)
{
	if (p != attacker)
	{
		if (!ValidateObject(p) || !ValidateObject(attacker))
			return;

		if (p.dead || p.health <= 0)	// extra condition check, to see if deathstuff hasn't been hit yet
			return;

		if (p.frozen && p.dexterity > 0)
		{
			p.frozen = false;
			p.Refresh();
			if (mSock != null)
			{
				mSock.SysMessage(GetDictionaryEntry(700, mSock.language)); // You are no longer frozen.
			}
		}

		if (p.vulnerable && p.region.canCastAggressive)
		{
			var hitLoc = CalculateHitLoc();
			var damage = ApplyDamageBonuses(element, attacker, p, 25, hitLoc, amount);
			damage = ApplyDefenseModifiers(element, attacker, p, 25, hitLoc, damage, true);

			if (damage <= 0)
			{
				damage = 1;
			}

			// Double spell-damage against non-human NPCs
			if (p.npc && !p.isHuman)
			{
				damage *= 2;
			}
			//DoTempEffect(0, attacker, p, 28, damage, 0, 0);
		}
	}
}

// Calculate damage to the caster based on Karma and Chivalry skill
function ComputePowerValue(mChar, div) 
{
	if (!mChar)
	{
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

// Function to handle spell casting logic
function SpellCasting(mChar, mSpell, spellType)
{
	var castingFocus = 40;
	var difficulty = 50; // Use 50% as the default difficulty for all spells

	// Set skill requirements based on spell type
	var lowSkill = (spellType == 1) ? mSpell.scrollLow : mSpell.lowSkill;
	var highSkill = (spellType == 1) ? mSpell.scrollHigh : mSpell.highSkill;

	// Perform skill check
	if (!mChar.CheckSkill(51, lowSkill, highSkill))
	{
		// Skill check failed
		mChar.SysMessage("You failed to cast the spell.");
		TriggerEvent(6004, "DeleteReagents", mChar, mSpell);
		//mChar.SpellFail(); currnetly doesnt show anything for spellfail in OSI
		mChar.SetTimer(Timer.SPELLTIME, 0);
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}

	// Calculate Chance to Cast
	var chanceToCast = ((mChar.baseskills[51] - (difficulty - (castingFocus / 2))) / castingFocus) * 100;
	if (chanceToCast < 0)
	{
		chanceToCast = 0;
	} 
	else if (chanceToCast > 100)
	{
		chanceToCast = 100;
	}

	// Perform casting check
	if (mChar.commandlevel < 2 && RandomNumber(0, 100) > chanceToCast)
	{
		// Handle spell failure
		TriggerEvent(6004, "DeleteReagents", mChar, mSpell);
		//mChar.SpellFail();
		mChar.SetTimer(Timer.SPELLTIME, 0);
		mChar.isCasting = false;
		mChar.spellCast = -1;
		return false;
	}
	return true;
}

function _restorecontext_() {}
