function SpellRegistration() 
{
	RegisterSpell(101, true); // animate dead
	RegisterSpell(102, true); // Blood Oath
	RegisterSpell(103, true); // corpse skin
	RegisterSpell(104, true); // Curse Weapon
	RegisterSpell(105, true); // Evil Omen
	RegisterSpell(106, true); // Horrific Beast
	RegisterSpell(107, true); // Lich Form
	RegisterSpell(108, true); // mind rot
	RegisterSpell(109, true); // Pain Spik
	RegisterSpell(110, true); // Poison Strike
	RegisterSpell(111, true); // Strangle
	RegisterSpell(112, true); // Summon Familiar
	RegisterSpell(113, true); // Vampiric Embrace
	RegisterSpell(114, true); // vengeful spirit
	RegisterSpell(115, true); // Wither
	RegisterSpell(116, true); // Wraith Form
	RegisterSpell(117, true); // Exorcism
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
// Option toggle if you later want to allow "Focused Necro" to cast
var HB_ALLOW_FOCUSED_NECRO = false;

function IsCasterInHorrificBeast(pChar)
{
    return (pChar.GetTag("necroForm") === "horrific");
}

function IsTransformSpell(spellNum) 
{
    // Horrific Beast, Lich Form, Wraith Form
    return (spellNum == 106 || spellNum == 107 || spellNum == 116);
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

	// --- block non-transform casting while in HB ---
	if (IsCasterInHorrificBeast(mChar) && !IsTransformSpell(spellNum))
	{
		if (mSock) 
			mSock.SysMessage("You cannot cast that while in Horrific Beast form.");
		return true;
	}

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

	mSock.SysMessage(spellNum);

	mChar.StartTimer(delay * 1000, spellNum, true);

	return true;
}

function onCallback0(mSock, ourTarg) 
{
	var mChar = mSock.currentChar;
	if (!ValidateObject(mChar))
		return;

	if (ValidateObject(ourTarg))
	{
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
	var mSock = mChar.socket;
	mChar.isCasting = false;
	mChar.frozen = false;
	mChar.Refresh();

	// --- Horrific Beast: no target required ---
	if (timerID == 106)
	{
		if (mSock)
			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[106].recoveryDelay);
		onSpellSuccess(mSock, mChar, mChar, 106); // self target
		return;
	}

	// --- Lich Form: no target required ---
	if (timerID == 107) 
	{
		if (mSock)
			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[107].recoveryDelay);
		onSpellSuccess(mSock, mChar, mChar, 107);
		return;
	}

	    // --- Summon Familiar: no target required ---
    if (timerID == 112)
	{
        if (mSock)
			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[109].recoveryDelay);
        // call success with self as target (not used, but keeps flow consistent)
        onSpellSuccess(mSock, mChar, mChar, 109);
        return;
    }
	// --- Wraith Form: no target required ---
	if (timerID == 116)
	{
		if (mSock)
			mChar.SetTimer(Timer.SPELLRECOVERYTIME, Spells[116].recoveryDelay);
		onSpellSuccess(mSock, mChar, mChar, 116);
		return;
	}

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
	if ((mChar.commandlevel < 2) && (!mChar.CheckSkill(49, lowSkill, highSkill))) 
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
	}

	// This is where the code actually executes ... all of this setup for a single line of code!
	DispatchSpell(spellNum, mSpell, sourceChar, ourTarg, mChar);
}

// ----- Familiar entries (OSI thresholds) -----
var FamiliarEntries = [
    { id: "hordeminion",  name: "Horde Minion",   cliloc: 1060146, necReq: 30,  ssReq: 30  },
    { id: "shadowwisp",   name: "Shadow Wisp",    cliloc: 1060142, necReq: 50,  ssReq: 50  },
    { id: "darkwolf",     name: "Dark Wolf",      cliloc: 1060143, necReq: 60,  ssReq: 60  },
    { id: "deathadder",   name: "Death Adder",    cliloc: 1060145, necReq: 80,  ssReq: 80  },
    { id: "vampirebat",   name: "Vampire Bat",    cliloc: 1060144, necReq: 100, ssReq: 100 }
];

var EnabledColor32  = 0x18CD00;
var DisabledColor32 = 0x4A8B52;

function ExitFormByKey(pChar, pSock, key)
{
	switch (key) 
	{
		case "horrific": ExitHorrificBeast(pChar, pSock); break;
		case "lich":     ExitLichForm(pChar, pSock);     break;
		case "wraith":   ExitWraithForm(pChar, pSock);   break;
	}
}

function EnterFormByKey(pChar, pSock, key) 
{
	switch (key)
	{
		case "horrific": EnterHorrificBeast(pChar, pSock); break;
		case "lich":     EnterLichForm(pChar, pSock);     break;
		case "wraith":   EnterWraithForm(pChar, pSock);   break;
	}
}

function ExitNecroFormOnLogout(pSock, pChar)
{
    if (!ValidateObject(pChar)) return true;

    var cur = pChar.GetTag("necroForm");
    if (!cur)
		return true;

    // remove any form-specific triggers (e.g., Wraith mana-leech)
    pChar.RemoveScriptTrigger(7004); // safe to call even if not present

    switch (cur)
    {
        case "horrific": ExitHorrificBeast(pChar, pSock); break;
        case "lich":     ExitLichForm(pChar, pSock);      break;
        case "wraith":   ExitWraithForm(pChar, pSock);    break;
        default:
            // Unknown/legacy form tag – just clear & refresh to avoid odd visuals
            pChar.SetTag("necroForm", null);
            pChar.Refresh();
            break;
    }
    return true;
}

// Toggle the requested form: if already in it, exit; else exit current (if any) and enter new.
function ToggleNecroForm(pChar, formKey) 
{
	var sock = pChar.socket;
	var cur  = pChar.GetTag("necroForm");
	if (cur === formKey) 
	{ 
		ExitFormByKey(pChar, sock, formKey);
		return;
	}
	if (cur) 
	{
		ExitFormByKey(pChar, sock, cur);
	}
	EnterFormByKey(pChar, sock, formKey); // then enter requested form
}

function DispatchSpell(spellNum, mSpell, sourceChar, ourTarg, caster) 
{
	if (spellNum == 101 )// animate dead
	{
		if (caster.socket.GetWord(1) || !ValidateObject(ourTarg))
		{
			caster.socket.SysMessage(GetDictionaryEntry(749, caster.socket.language)); // That is not a corpse!
			return;
		}

		if (ourTarg.isHuman || ourTarg.GetTag("animated"))
		{
			caster.socket.SysMessage("There's not enough life force there to animate."); // There's not enough life force there to animate.
			return;
		}

		var animateDead = 0;
		switch (ourTarg.sectionID) 
		{
			// AnimateDead = 1
			case "DreadSpider":
			case "FrostSpider":
			case "GiantSpider":
			case "GiantBlackWidow":
			case "BlackSolenInfiltratorQueen":
			case "BlackSolenInfiltratorWarrior":
			case "BlackSolenQueen":
			case "BlackSolenWarrior":
			case "BlackSolenWorker":
			case "RedSolenInfiltratorQueen":
			case "RedSolenInfiltratorWarrior":
			case "RedSolenQueen":
			case "RedSolenWarrior":
			case "RedSolenWorker":
			case "TerathanAvenger":
			case "TerathanDrone":
			case "TerathanMatriarch":
			case "TerathanWarrior":
				animateDead = 1;
				break;

			// AnimateDead = 2
			case "horse":
			case "Nightmare":
			case "FireSteed":
			case "Kirin":
			case "Unicorn":
				animateDead = 2;
				break;

			// AnimateDead = 3
			case "BloodElemental":
			case "EarthElemental":
			case "SummonedEarthElemental":
			case "AgapiteElemental":
			case "BronzeElemental":
			case "CopperElemental":
			case "DullCopperElemental":
			case "GoldenElemental":
			case "ShadowIronElemental":
			case "ValoriteElemental":
			case "VeriteElemental":
			case "PoisonElemental":
			case "FireElemental":
			case "SummonedFireElemental":
			case "SnowElemental":
			case "AirElemental":
			case "SummonedAirElemental":
			case "WaterElemental":
			case "ToxicElemental":
				animateDead = 3;
				break;

			// AnimateDead = 4
			case "AncientWyrm":
			case "Dragon":
			case "GreaterDragon":
			case "SerpentineDragon":
			case "ShadowWyrm":
			case "SkeletalDragon":
			case "WhiteWyrm":
			case "Drake":
			case "Wyvern":
			case "LesserHiryu":
			case "Hiryu":
				animateDead = 4;
				break;
		}

		if (animateDead == 0)
		{
			caster.socket.SysMessage("There's not enough life force there to animate."); // There's not enough life force there to animate.
			return;
		}

		// Calculate Value based on caster's skills
		var necroSkill = caster.skills.necromancy; // Assuming skills are accessed this way
		var spiritSpeakSkill = caster.skills.spiritspeak;
		var value = ((necroSkill * 0.3) + (spiritSpeakSkill * 0.3)) * 180;
		var fame = ourTarg.fame; // Assuming fame is an accessible property
		if (value > fame)
		{
			value = fame; // Adjust value if it exceeds target fame
		}

		// Determine the creature to spawn
		var npcName = "";
		switch (animateDead)
		{
			case 1: // Arachnid
				npcName = "orc";//MoundOfMaggots
				break;
			case 2: // Equinae
				if (value >= 10000)
				{
					npcName = "skeletalmount";
				}
				else
				{
					npcName = "skeletalmount";
				}
				break;
			default:
				caster.socket.SysMessage("error please report to gm");
				return;
		}

		if (npcName == "")
		{
			caster.socket.SysMessage("error please report to gm");
			return;
		}

		// Spawn the selected NPC
		var nSpawned = SpawnNPC(npcName, ourTarg.x, ourTarg.y, ourTarg.z, ourTarg.worldnumber);
		if (nSpawned != null)
		{
			nSpawned.owner = caster;
			nSpawned.Follow(caster);
			nSpawned.SetTag("ownerSerial", caster.serial);
			nSpawned.SetTag("animated", true);
			nSpawned.wandertype = 2;
			nSpawned.aitype = 88;
			nSpawned.AddScriptTrigger(3229);//animated script
			nSpawned.fame = 0;
			nSpawned.karma = -1500;
			nSpawned.StartTimer(86400000, 1, 3229);
			caster.SysMessage("You have successfully animated a " + npcName + "!");

			ourTarg.colour = 1109;
			ourTarg.SetTag("animated", true);
			//ourTarg.Delete();
		}
		else
		{
			caster.SysMessage("Failed to animate the dead. Something went wrong.");
		}
	}

	if (spellNum == 106) { ToggleNecroForm(caster, "horrific"); return; } // Horrific Beast

	if (spellNum == 107) { ToggleNecroForm(caster, "lich");     return; } // Lich Form

	if (spellNum == 112) // Summon Familiar
    {
        var sock = caster.socket;

        // Only one familiar at a time
        var famSer = parseInt(caster.GetTag("familiarSerial"), 10) || 0;
        if (caster.GetTag("familiarActive") == 1 && famSer > 0 && ValidateObject(CalcCharFromSer(famSer))) {
            if (sock) sock.SysMessage("You may only control one familiar at a time.");
            return;
        }

        OpenSummonFamiliarGump(sock, caster);

        return;
    }

	if (spellNum == 116) { ToggleNecroForm(caster, "wraith");   return; } // Wraith Form

}

function isFemale(pChar)
{
    // your shard: gender == 0 => male; anything else => female
    return !(pChar.gender == 0);
}

function EnterWraithForm(pChar, pSock)
{
    if (!pChar.GetTag("origBody"))      pChar.SetTag("origBody", pChar.id);
    if (!pChar.GetTag("origSkinColor")) pChar.SetTag("origSkinColor", pChar.color);

    var female = isFemale(pChar);
    pChar.id    = female ? 747 : 748;     // bodies
    pChar.color = female ? 0    : 0x4001; // hues
    pChar.SetTag("necroForm", "wraith");

    // --- OSI effects (store as tags & light live props) ---
    // Mana drain (leech) % = Spirit Speak / 5  (SS 100.0 => 20%, SS 120.0 => 24%)
	if (!pChar.GetTag("wraithLeechOn"))
	{
		pChar.AddScriptTrigger(6005);
		pChar.SetTag("wraithLeechOn", 1);
	}

	var fire = pChar.Resist( 5 );
	var phys = pChar.Resist( 1 );
	var energy = pChar.Resist( 6 );
	pChar.Resist( 1, phys + 15 ); // phys
	pChar.Resist( 5, fire - 5 ); // fire
	pChar.Resist( 6, energy - 5 ); // energy 
    pChar.Refresh();

	var ss = pChar.skills.spiritspeak | 0;
    var leechPct = ss / 50.0;

	TriggerEvent( 2204, "AddBuff", pChar, 1124, 1060524, 1153829, 0, "\t15\t5\t5\t" + leechPct );

	if (pSock)
		pSock.SysMessage("Your body fades into a wraith-like form.");
}

function ExitWraithForm(pChar, pSock)
{
    var orig = parseInt(pChar.GetTag("origBody"), 10);
    pChar.id = (isNaN(orig) ? pChar.id : orig);

    var origColor = parseInt(pChar.GetTag("origSkinColor"), 10);
    pChar.color = (isNaN(origColor) ? pChar.color : origColor);

    // clear tags
	pChar.SetTag("necroForm", null);
	pChar.SetTag("origBody", null);
	pChar.SetTag("origSkinColor", null);
	pChar.RemoveScriptTrigger(6005);
	pChar.SetTag("wraithLeechOn", null);
	var fire = pChar.Resist( 5 );
	var phys = pChar.Resist( 1 );
	var energy = pChar.Resist( 6 );
	pChar.Resist( 1, phys - 15 ); // phys
	pChar.Resist( 5, fire + 5 ); // fire
	pChar.Resist( 6, energy + 5 ); // energy 

    pChar.Refresh();
	TriggerEvent( 2204, "RemoveBuff", pChar, 1124 );
    if (pSock) 
		pSock.SysMessage("You return to your normal form.");
}


function EnterLichForm(pChar, pSock)
{
    if (!pChar.GetTag("origBody"))
		pChar.SetTag("origBody", pChar.id);
	if (!pChar.GetTag("origSkinColor"))
		pChar.SetTag("origSkinColor", pChar.color);

    pChar.id = 749;                // Lich body
	pChar.color = 0;
    pChar.SetTag("necroForm", "lich");

    // OSI effects (store as live props + tags so your combat/regen/resist code can honor them)
    // +13 Mana Regen, -5 Hitpoint Regen, resist shifts, undead-slayer vuln, bleed immunity.
    pChar.manaRegenBonus   = (pChar.manaRegenBonus|0) + 13;
    pChar.healthRegenBonus = (pChar.healthRegenBonus|0) - 5;
	var fire = pChar.Resist( 5 );
	var cold = pChar.Resist( 4 );
	var poison = pChar.Resist( 7 );
	pChar.Resist( 5, fire - 10 ); // fire
	pChar.Resist( 4, cold + 10 ); // cold
	pChar.Resist( 7, poison + 10 ); // poison 
    pChar.SetTag("Lich_BleedImmune", 1);

    pChar.Refresh();

	TriggerEvent( 2204, "AddBuff", pChar, 1086, 1060515, 1153767, 0, "\t5\t13\t10\t10\t10" );

    if (pSock) pSock.SysMessage("You assume the form of a lich.");
}

function ExitLichForm(pChar, pSock)
{
    var orig = parseInt(pChar.GetTag("origBody"), 10);
    pChar.id = (isNaN(orig) ? pChar.id : orig);

	var origColor = parseInt(pChar.GetTag("origSkinColor"), 10);
    pChar.color = (isNaN(origColor) ? pChar.color : origColor);


    // Revert bonuses and clear tags
    pChar.manaRegenBonus   = Math.max(0, (pChar.manaRegenBonus|0) - 13); // clamp non-negative if you want
    pChar.healthRegenBonus = (pChar.healthRegenBonus|0) + 5;

    pChar.SetTag("necroForm", null);
    pChar.SetTag("origBody", null);
	pChar.SetTag("origSkinColor", null);
	var fire = pChar.Resist( 5 );
	var cold = pChar.Resist( 4 );
	var poison = pChar.Resist( 7 );
	pChar.Resist( 5, fire + 10 ); // fire
	pChar.Resist( 4, cold - 10 ); // cold
	pChar.Resist( 7, poison - 10 ); // poison 
    pChar.SetTag("Lich_BleedImmune", null);
    pChar.Refresh();

	TriggerEvent( 2204, "RemoveBuff", pChar, 1086 );

    if (pSock)
		pSock.SysMessage("You return to your normal form.");
}

function EnterHorrificBeast(pChar, pSock)
{
    // Save original body the first time we morph
    if (!pChar.GetTag("origBody"))
        pChar.SetTag("origBody", pChar.id);
	if (!pChar.GetTag("origSkinColor"))
		pChar.SetTag("origSkinColor", pChar.color);

    // Set body to the Horrific Beast form
    pChar.id = 746; // <- as requested
	pChar.color = 0;
    pChar.SetTag("necroForm", "horrific");

    // Canon effects (set as tags so your combat/regen scripts can honor them):
    // +20 HPR 2 HP/sec
    // +25% Melee Damage Increase
    // Base hand damage becomes 5–15
	pChar.healthRegenBonus = 20;
	pChar.damageIncrease = 25;
    pChar.Refresh();

	TriggerEvent( 2204, "AddBuff", pChar, 1085, 1060514, 1153763, 0, "\t20\t25" );

    if (pSock)
		pSock.SysMessage("You transform into a horrific beast!");
}

function ExitHorrificBeast(pChar, pSock)
{
    // Restore original body (fallback to human if none)
    var orig = parseInt(pChar.GetTag("origBody"), 10);
    pChar.id = (isNaN(orig) ? pChar.id : orig);

	var origColor = parseInt(pChar.GetTag("origSkinColor"), 10);
    pChar.color = (isNaN(origColor) ? pChar.color : origColor);

    // Clear tags
    pChar.SetTag("necroForm", null);
    pChar.SetTag("origBody", null);
	pChar.SetTag("origSkinColor", null);
	pChar.healthRegenBonus = 0;
	pChar.damageIncrease = 0;
    pChar.Refresh();

	TriggerEvent( 2204, "RemoveBuff", pChar, 1085 );

    if (pSock) 
		pSock.SysMessage("You return to your normal form.");
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

function OpenSummonFamiliarGump(pSock, pChar)
{
    if (!pSock || !ValidateObject(pChar))
		return;

    var g = new Gump();
   // g.gumpID = FAMILIAR_GUMP_ID;
    g.AddPage(0);

    // frame & alpha stripe (mirrors ServUO layout roughly)
    g.AddBackground(10, 10, 250, 178, 9270);
    g.AddCheckerTrans(20, 20, 230, 158);

    // cosmetic images
    g.AddGump(220, 20, 10464);
    g.AddGump(220, 72, 10464);
    g.AddGump(220, 124, 10464);

    g.AddPicture(188,  16, 6883);
    g.AddPicture(198, 168, 6881);
    g.AddPicture(8, 15, 6882);
    g.AddPicture(2, 168, 6880);

    // Title: "Choose thy familiar..."
    // If your UOX3 has AddXmfHTMLGump, use cliloc 1060147. Otherwise use HTML text:
    g.AddHTMLGump(30, 26, 200, 20, false, false, "<BASEFONT COLOR=#" + EnabledColor32.toString(16) + ">Choose thy familiar...</BASEFONT>");

	var necro  = pChar.skills.necromancy | 0;   // 0..1000
	var spirit = pChar.skills.spiritspeak | 0;

	for (var i = 0; i < FamiliarEntries.length; i++)
	{
		var E = FamiliarEntries[i];
		var enabled = (necro >= E.necReq10 && spirit >= E.ssReq10);

		g.AddButton(27, 53 + (i * 21), 9702, 1, 1, i + 1);

		var hex = (enabled ? EnabledColor32 : DisabledColor32).toString(16);
		var label = "<BASEFONT COLOR=#" + hex + ">" + E.name + "</BASEFONT>";
		g.AddHTMLGump(50, 51 + (i * 21), 150, 20, false, false, label);
	}

    g.Send(pSock);
	g.Free();
}

const maxControlSlots = GetServerSetting("MaxControlSlots");

// maxFollowers only comes into play if maxControlSlots is 0 in UOX.INI
const maxFollowers = GetServerSetting("MaxFollowers");

/** @type {(pSock: Socket, buttonID: number, gumpID: number) => void} */
function onGumpPress(pSock, buttonID, gumpID)
{
	//if (!pSock || gumpID !== FAMILIAR_GUMP_ID)
	//	return;

	var pChar = pSock.currentChar;
	if (!ValidateObject(pChar)) return;

	if (buttonID <= 0)
	{
		pSock.SysMessage("You decide not to summon a familiar.");
		return;
	}

	var index = (buttonID - 1) | 0;
	if (index < 0 || index >= FamiliarEntries.length)
		return;

	var entry = FamiliarEntries[index];

	// Already have one?
	var famSer = parseInt(pChar.GetTag("familiarSerial"), 10) || 0;
	if (pChar.GetTag("familiarActive") == 1 && famSer > 0 && ValidateObject(CalcCharFromSer(famSer)))
	{
		pSock.SysMessage("You already have a familiar.");
		return;
	}

	// Skill gate check
	var necro = pChar.skills.necromancy | 0;   // 0..1000
	var spirit = pChar.skills.spiritspeak | 0;
	if (necro < entry.necReq10 || spirit < entry.ssReq10)
	{
		// Example localized message style (swap #s if you have a matching cliloc)
		// pSock.SysMessage(GetDictionaryEntry(XXXX, pSock.language));
		pSock.SysMessage(
			"That familiar requires " + (entry.necReq10 / 10).toFixed(1) +
			" Necromancy and " + (entry.ssReq10 / 10).toFixed(1) + " Spirit Speak."
		);
		OpenSummonFamiliarGump(pSock, pChar);
		return;
	}

	// Check to see if player actually has space for ANY more pets
	if (maxControlSlots > 0)
	{
		if (pUser.controlSlotsUsed >= maxControlSlots)
		{
			pSock.SysMessage(GetDictionaryEntry(2397, pSock.language)); // You have too many followers to tame that creature.
			return;
		}
	}
	else if (maxFollowers > 0 && pUser.followerCount >= maxFollowers)
	{
		pSock.SysMessage(GetDictionaryEntry(2397, pSock.language)); // You have too many followers to tame that creature.
		return;
	}

	// Spawn the familiar at feet
	var n = SpawnNPC(entry.id, pChar.x, pChar.y, pChar.z, pChar.worldnumber);
	if (!ValidateObject(n))
	{
		pSock.SysMessage("The familiar fails to answer your call.");
		OpenSummonFamiliarGump(pSock, pChar);
		return;
	}

	// Bind & mark
	n.owner = pChar;
	n.wandertype = 0;
	n.aitype = 0;
	n.tamed = true;
	n.Follow(pChar);
	n.SetTag("isFamiliar", 1);
	n.SetTag("familiarOf", pChar.serial);
	pChar.controlSlotsUsed = pUpCharser.controlSlotsUsed + n.controlSlots;

	// Also add pet to player's list of active followers
	pUser.AddFollower(n);

	// If you use follower slots, increment here:
	// var cur = parseInt(pChar.GetTag("followers"),10)||0; pChar.SetTag("followers", cur+1);

	pChar.SetTag("familiarActive", 1);
	pChar.SetTag("familiarSerial", n.serial);

	// Cosmetic
	// If your build supports these, add particles/sound; else omit.
	// pChar.FixedParticles(0x3728, 1, 10, 9910, 0); // Not all UOX3 builds expose this exactly
	// n.Sound(n.idleSound|0);

	pSock.SysMessage("You summon a " + entry.name + " to your side.");
}
