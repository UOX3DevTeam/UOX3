function onSpecialMove(pUser, abilityID)
{
    /*switch (abilityID)
    {
        case 0x01: // Armor Ignore
            HandleArmorIgnore(pUser, abilityID);
            break;
		case 0x05:// Disarm
			HandleDisarm(pUser, abilityID);
			break;
		case 0x06:// Dismount
			HandleDismount(pUser, abilityID);
			break;
		case 0x0C:// ShadowStrike
			HandleShadowStrike(pUser, abilityID);
			break;
		case 0x0B:// Paralyzing Blow
			HandleParalyzingBlow(pUser, abilityID);
			break;
        default:
            break;
    }*/

	pUser.TextMessage("I'm activating ability #" + abilityID);
	pUser.SetTempTag("abilityID", abilityID);
	//pUser.StartTimer(5000, abilityID, true);
    return true;

	//The rest of the AOS Abilites before any other expansions
	//Mortal Strike
	//Infectious Strike
	//Double Strike
	//Bleed Attack
	//Whirlwind Attack 
}

// Define the checkSkillRequirement function outside RequiredSkill
function checkSkillRequirement(pUser, requiredSkillLevel, requiredSkill, skillMessage) 
{
	if (pUser.skills[requiredSkill] < requiredSkillLevel)
	{
		pUser.SysMessage("You need " + skillMessage + " weapon skill to perform that attack");
		return false;
	}
	return true;
}

// Define the RequiredSkill function
function RequiredSkill(pUser, abilityID)
{
	// Define weapon types and their skill requirements and if primary or secondary ability
	var weaponTypes = {
		"0x0DF0": { primary: 0, secondary: 11, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0DF1": { primary: 0, secondary: 11, reqSkill: 41, skillMsg: "mace fighting" },	// Black Staves  // WhirlwindAttack, ParalyzingBlow

		"0x0DF2": { primary: 6, secondary: 5, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0DF3": { primary: 6, secondary: 5, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0DF4": { primary: 6, secondary: 5, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0DF5": { primary: 6, secondary: 5, reqSkill: 41, skillMsg: "mace fighting" },	// wands  // Dismount, Disarm

		"0x0E81": { primary: 0, secondary: 5, reqSkill: 41, skillMsg: "mace fighting" },  
		"0x0E82": { primary: 0, secondary: 5, reqSkill: 41, skillMsg: "mace fighting" },	// Shepherd's Crooks  // CrushingBlow, Disarm

		"0x0e85": { primary: 0, secondary: 5, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0e86": { primary: 0, secondary: 5, reqSkill: 40, skillMsg: "Swordsmanship" },	// pickaxe  // DoubleStrike, Disarm

		"0x0E87": { primary: 0, secondary: 6, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0E88": { primary: 0, secondary: 6, reqSkill: 41, skillMsg: "mace fighting" },	// Pitchforks  // BleedAttack, Dismount

		"0x0E89": { primary: 0, secondary: 0, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0E8A": { primary: 0, secondary: 0, reqSkill: 41, skillMsg: "mace fighting" },	// Quarter Staves  // DoubleStrike, ConcussionBlow

		"0x0EC2": { primary: 0, secondary: 0, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0EC3": { primary: 0, secondary: 0, reqSkill: 41, skillMsg: "mace fighting" },	// Cleavers  // BleedAttack, InfectiousStrike

		"0x0EC4": { primary: 0, secondary: 0, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0EC5": { primary: 0, secondary: 0, reqSkill: 41, skillMsg: "mace fighting" },	// Skinning Knives  // ShadowStrike, BleedAttack

		"0x0F43": { primary: 1, secondary: 5, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F44": { primary: 1, secondary: 5, reqSkill: 40, skillMsg: "Swordsmanship" },	// hatchets  // ArmorIgnore, Disarm

		"0x0F45": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F46": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Executioner Axes  // BleedAttack, MortalStrike

		"0x0F47": { primary: 1, secondary: 5, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F48": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Battle Axes  // BleedAttack, ConcussionBlow

		"0x0F49": { primary: 0, secondary: 6, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F4A": { primary: 0, secondary: 6, reqSkill: 40, skillMsg: "Swordsmanship" },	// Axes  // CrushingBlow, Dismount

		"0x0F4B": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F4C": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Double Axe  // DoubleStrike, WhirlwindAttack

		"0x0F4D": { primary: 11, secondary: 6, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F4E": { primary: 11, secondary: 6, reqSkill: 40, skillMsg: "Swordsmanship" },	// Bardiches  // ParalyzingBlow, Dismount

		"0x0F4F": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F50": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Crossbows  // ConcussionBlow, MortalStrike

		"0x0F51": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F52": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Daggers  // InfectiousStrike, ShadowStrike

		"0x0F5C": { primary: 0, secondary: 5, reqSkill: 41, skillMsg: "mace fighting" },
		"0x0F5D": { primary: 0, secondary: 5, reqSkill: 41, skillMsg: "mace fighting" },	// Maces  // ConcussionBlow, Disarm

		"0x0F5E": { primary: 0, secondary: 1, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F5F": { primary: 0, secondary: 1, reqSkill: 40, skillMsg: "Swordsmanship" },	// Broadswords  // CrushingBlow, ArmorIgnore

		"0x13B7": { primary: 1, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13B8": { primary: 1, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F60": { primary: 1, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F61": { primary: 1, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Longswords  // ArmorIgnore, ConcussionBlow

		"0x0F62": { primary: 1, secondary: 11, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0F63": { primary: 1, secondary: 11, reqSkill: 40, skillMsg: "Swordsmanship" },	// Spears  // ArmorIgnore, ParalyzingBlow

		"0x0FB4": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x0FB5": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Sledge hammers // CrushingBlow, ShadowStrike

		"0x13AF": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13B0": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// War Axes // ArmorIgnore, BleedAttack

		"0x13B1": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13B2": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Bows // ParalyzingBlow, MortalStrike

		"0x13B3": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13B4": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Clubs // ShadowStrike, Dismount

		"0x13B5": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13B6": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Scimitars // DoubleStrike, ParalyzingBlow

		"0x13B9": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13BA": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Viking Swords // ParalyzingBlow, CrushingBlow

		"0x13FC": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13FD": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Heavy Crossbows // MovingShot, Dismount

		"0x13E3": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13E4": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Smith's Hammers // CrushingBlow, ShadowStrike

		"0x13F6": { primary: 0, secondary: 5, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13F7": { primary: 0, secondary: 5, reqSkill: 40, skillMsg: "Swordsmanship" },	// Butcher Knives // InfectiousStrike,Disarm

		"0x13F8": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13F9": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Gnarled Staves // ConcussionBlow,ForceOfNature

		"0x13FA": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13FB": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Large Battle Axes // WhirlwindAttack,BleedAttack

		"0x13FE": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x13FF": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Katana // DoubleStrike,ArmorIgnore

		"0x1400": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x1401": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Kryss // ArmorIgnore,InfectiousStrike

		"0x1402": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x1403": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Short Spears // ShadowStrike,MortalStrike

		"0x1404": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x1405": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// War Forks // BleedAttack,Disarm

		"0x1406": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x1407": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// War Maces // CrushingBlow,MortalStrike

		"0x1438": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x1439": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// War Hammers // WhirlwindAttack,CrushingBlow

		"0x143A": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x143B": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Mauls // DoubleStrike,ConcussionBlow

		"0x143C": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x143D": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Hammer Picks // ArmorIgnore,MortalStrike

		"0x143E": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x143F": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Halberds // WhirlwindAttack,ConcussionBlow

		"0x1440": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x1441": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },	// Cutlasses // BleedAttack,ShadowStrike

		"0x1442": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" },
		"0x1443": { primary: 0, secondary: 0, reqSkill: 40, skillMsg: "Swordsmanship" }		// Two Handed Axes // DoubleStrike,ShadowStrike
	};

	// Get items in user's hands
	var itemRHand = pUser.FindItemLayer(0x01);
	var itemLHand = pUser.FindItemLayer(0x02);

	// Check if either hand has an item
	if (itemRHand != null) {
		// Check item in the right hand
		if (weaponTypes[itemRHand.sectionID])
		{
			var weapon = weaponTypes[itemRHand.sectionID];

			if (abilityID == weapon.primary)
			{
				return checkSkillRequirement(pUser, 700, weapon.reqSkill, weapon.skillMsg);
			}
			else if (abilityID == weapon.secondary)
			{
				return checkSkillRequirement(pUser, 900, weapon.reqSkill, weapon.skillMsg);
			}
		}
	}
	else if (itemLHand != null)
	{
		// Check item in the left hand
		if (weaponTypes[itemLHand.sectionID])
		{
			var weapon = weaponTypes[itemLHand.sectionID];
			if (abilityID == weapon.primary)
			{
				return checkSkillRequirement(pUser, 700, weapon.reqSkill, weapon.skillMsg);
			}
			else if (abilityID == weapon.secondary)
			{
				return checkSkillRequirement(pUser, 900, weapon.reqSkill, weapon.skillMsg);
			}
		}
	}
	else //checking if both hands null if so, it is a wrestling skill
	{
		if (abilityID == 5)
		{
			return checkSkillRequirement(pUser, 700, 43, "Wrestling");
		}
		else if (abilityID == 11)
		{
			return checkSkillRequirement(pUser, 900, 43, "Wrestling");
		}
	}

	return true;
}

function CheckMana(pUser, abilityID, requiredMana)
{
	var NextUse = pUser.GetTempTag("doubleMana");
	var iTime = GetCurrentClock();
	if (pUser.mana <= requiredMana)
    {
		pUser.TextMessage("You need " + requiredMana + " mana to perform that attack");
        DeactivateSpecialMove(pUser.socket, abilityID);
        return false;
    }
    else
	{
		pUser.SetTempTag("doubleMana", iTime.toString());
		var Delay = 3000;
		if ((iTime - NextUse) < Delay)
		{
			pUser.mana -= requiredMana * 2;
			return true;
		}
		else
		{
			pUser.mana -= requiredMana;
			return true;
		}
	}
}

function HandleArmorIgnore(pUser, abilityID)
{
    pUser.SetTempTag("ArmorIgnore", true);
	pUser.SetTempTag("abilityID", abilityID);
}

function HandleDismount(pUser, abilityID)
{
    pUser.SetTempTag("Dismount", true);
	pUser.SetTempTag("abilityID", abilityID);
}

function HandleShadowStrike(pUser, abilityID)
{
    pUser.SetTempTag("ShadowStrike", true);
	pUser.SetTempTag("abilityID", abilityID);
}

function HandleParalyzingBlow(pUser, abilityID)
{
	pUser.SetTempTag("ParalyzingBlow", true);
	pUser.SetTempTag("abilityID", abilityID);
}

function HandleDisarm(pUser, abilityID)
{
	pUser.SetTempTag("Disarm", true);
}

function onCombatDamageCalc( pAttacker, pDefender, fightSkill, hitLoc )
{
	var abilityID = pAttacker.GetTempTag("abilityID");
    var baseDamage = pAttacker.attack;

    if( baseDamage == -1 )  // No damage if weapon breaks
        return 0;

    var damage = ApplyDamageBonuses( 1, pAttacker, pDefender, fightSkill, hitLoc, baseDamage );

    if( damage < 1 )
        return 0;

    // Check if attacker has armor ignore enabled
	if (abilityID == 1) // armorignore
    {
        // Armor Ignore ignores defense modifiers, but deals only 90% of potential damage
        damage *= 0.9;

        if(fightSkill == 31) // Archery
        {
            // Cap damage from Armor Strike attack at 30 for archery weapons
            if(damage > 30)
                damage = 30;
        }
        else
		{
            // For all othe rfighting skills, cap damage from Armor Strike at 35
            if(damage > 35)
                damage = 35;
        }
    }
	else if (abilityID == 12)// shadowstrike
	{
		damage *= 1.25;
	}
	else if (abilityID == 4)// crushingblow
	{
		damage *= 1.5;
	}
	else if (abilityID == 3)// ConcussionBlow
	{	
		if (pDefender.maxhp > 0) 
		{
			var hitsPercent = (pDefender.hp / pDefender.maxhp) * 100.0;

            var manaPercent = 0;

			if (pDefender.maxmana > 0)
				manaPercent = (pDefender.mana / pDefender.maxmana) * 100.0;

			damage += Math.min(Math.floor(Math.abs(hitsPercent - manaPercent) / 4), 20);
		}
	}
    else
	{
        // Otherwise, apply normal defense modifiers
        damage = ApplyDefenseModifiers( 1, pAttacker, pDefender, fightSkill, hitLoc, damage, true );
    }

    // If damage after defense modifiers is below 0, do a small random amount of damage still
    if( damage <= 0 )
        damage = RandomNumber( 0, 4 );

    // If defender is a player, damage is divided by this modifier from uox.ini
    if( !pDefender.npc )
        damage /= GetServerSetting( "NPCDAMAGERATE" );

    return damage;
}

function onAttack( pAttacker, pDefender )
{
	var abilityID = pAttacker.GetTempTag("abilityID");

	if (abilityID == 1) // armnor ignore
	{
		var requiredMana = 14;

		// Clear out any current ability the player is doing
		if (abilityID != 1)
			DeactivateSpecialMove(pAttacker.socket, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID, requiredMana))
			return true;

		pAttacker.TextMessage("Your attack penetrates their armor!");
		pDefender.TextMessage("The blow penetrated your armor!");

		pDefender.SoundEffect(0x0056, true);
		pDefender.StaticEffect(0x3728, 0x09, 0x06);
	}
	else if (abilityID == 12) // shadowstrike
	{
		var requiredMana = 20;

		if (pAttacker.skills[47] < 800) // Stealth
		{
			pAttacker.TextMessage("You lack the required stealth to perform that attack");
			return true;
		}

		// Clear out any current ability the player is doing
		if (abilityID != 12)
			DeactivateSpecialMove(pAttacker.socket, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID, requiredMana))
			return true;

		pAttacker.TextMessage("You strike and hide in the shadows!");
		pDefender.TextMessage("You are dazed by the attack and your attacker vanishes!");

		pAttacker.StaticEffect(0x376A, 0x09, 0x06);

		pDefender.SoundEffect(0x482, true);
		pDefender.StaticEffect(0x37BE, 0x09, 0x06);

		pAttacker.atWar = false;
		pDefender.atWar = false;
		pAttacker.visible = 1;
	}
	else if (abilityID == 6) // Dismount
	{
		var requiredMana = 16;

		//checking mana
		if (!CheckMana(pAttacker, abilityID, requiredMana))
			return true;

		// Check to see if player is mount or flying.
		if (pAttacker.isonhorse) {
			pAttacker.TextMessage("You cannot perform that attack while mounted or flying!");
			DeactivateSpecialMove(pAttacker.socket, abilityID);
			return true;
		}

		// Clear out any current ability the player is doing
		if (abilityID != 6)
			DeactivateSpecialMove(pAttacker.socket, abilityID);

		// Only Can work on players or npcs that is mounted
		if (!pDefender.isonhorse) {
			pAttacker.TextMessage("This attack only works on mounted or flying targets");
			return true;
		}

		//Lesser Hiryu have an 80% chance of missing this attack needs added

		//This is the Dismount Information sent to Attacker and Defender
		pDefender.SoundEffect(0x140, true);
		pDefender.StaticEffect(0x3728, 0x09, 0x06);
		pAttacker.TextMessage("The force of your attack has dislodged them from their mount!");
		pDefender.Dismount();
		pDefender.TextMessage("You have been knocked off of your mount by " + pAttacker.name + "!");
		pAttacker.SetTempTag("Dismount", false);
		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 11) // ParalyzingBlow
	{
		var requiredMana = 30;

		//checking mana
		if (!CheckMana(pAttacker, abilityID, requiredMana))
			return true;

		// Clear out any current ability the player is doing
		if (abilityID != 11)
			DeactivateSpecialMove(pAttacker.socket, abilityID);

		pAttacker.SoundEffect(0x204, true);
		pDefender.StaticEffect(0x376A, 0x09, 0x32);
		var IsImmune = pDefender.GetTempTag("IsImmune")

		if (IsImmune != null && IsImmune == true) {
			pAttacker.TextMessage("Your target resists paralysis.");
			pDefender.TextMessage("You resist paralysis.");
			return true;
		}

		var seconds = 3000; // We want this applied to players even if they are "offline" (aka disconnected but not vanished from view yet)
		if (pDefender.npc) {
			seconds = 6000;
		}
		else if (pDefender.socket) {
			pDefender.TextMessage("The attack has temporarily paralyzed you!", false, 0x3b2, 0, pDefender.serial);// The attack has temporarily paralyzed you!
		}

		pDefender.SetTempTag("IsImmune", true);
		pDefender.StartTimer(seconds + 8000, 9000, true);
		pDefender.StartTimer(seconds, 8000, true);
		pDefender.frozen = true;

		if (pAttacker.socket) {
			pAttacker.TextMessage(GetDictionaryEntry(17702, pAttacker.socket.language), false, 0x3b2, 0, pAttacker.serial);// You deliver a paralyzing blow!
		}
		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 5) // Disarm
	{
		//check skills
		if (!RequiredSkill(pAttacker, abilityID))
			return true;

		var itemRHand = pDefender.FindItemLayer(0x01);
		var itemLHand = pDefender.FindItemLayer(0x02);
		var requiredMana = 20;

		if (pDefender.pack == null || itemLHand != null && itemLHand.movable >= 2 || itemRHand != null && itemRHand.movable >= 2) {
			pAttacker.SysMessage("You cannot disarm your opponent.");
			return false;
		}

		if (itemLHand != null && itemLHand.type == 9 || itemRHand != null && itemRHand.type == 9) {
			pAttacker.SysMessage("Your target is already unarmed!");
			return false;
		}

		//checking mana
		if (!CheckMana(pAttacker, abilityID, requiredMana))
			return true;

		// Clear out any current ability the player is doing
		if (abilityID != 5)
			DeactivateSpecialMove(pAttacker, abilityID);

		pAttacker.SoundEffect(0x3B9, true);
		pDefender.StaticEffect(0x37BE, 0x09, 0x32);

		pAttacker.SysMessage("You disarm their weapon!");
		pDefender.SysMessage("Your weapon has been disarmed!");

		if (itemLHand != null) 
		{
			itemLHand.container = pDefender.pack;
		}

		if (itemRHand != null) 
		{
			itemRHand.container = pDefender.pack;
		}

		pDefender.AddScriptTrigger(7002);//block equip for 5 seconds script added
		pDefender.SetTempTag("BlockEquip", true);
		pDefender.StartTimer(5000, 9100, true);

		TriggerEvent(50104, "AddBuff", pDefender, 0x3ea, 1075637, 0, 5, " ");

		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 4) // crushingblow
	{
		var requiredMana = 20;

		//checking mana
		if (!CheckMana(pAttacker, abilityID, requiredMana))
			return true;

		// Clear out any current ability the player is doing
		if (abilityID != 4)
			DeactivateSpecialMove(pAttacker, abilityID);

		pAttacker.SysMessage("You have delivered a crushing blow!");
		pDefender.SysMessage("You take extra damage from the crushing attack!");

		pAttacker.SoundEffect(0x1E1, true);
	}
	else if (abilityID == 3) // ConcussionBlow
	{
		var requiredMana = 20;

		//checking mana
		if (!CheckMana(pAttacker, abilityID, requiredMana))
			return true;

		// Clear out any current ability the player is doing
		if (abilityID != 3)
			DeactivateSpecialMove(pAttacker, abilityID);

		pAttacker.SysMessage("You have delivered a concussion!");
		pDefender.SysMessage("You feel disoriented!");

		pAttacker.SoundEffect(0x213, true);
		pDefender.StaticEffect(0x377A, 0x09, 0x32);
	}
}

function ClearSpecialMove(pUser, abilityID)
{
	var socket = pUser.socket;
	pUser.SetTempTag("abilityID", null);
	pUser.SetTempTag("doubleMana", null);

	var toSend = new Packet;
	toSend.ReserveSize(7)
	toSend.WriteByte(0, 0xbf); // Packet
	toSend.WriteShort(1, 0x07); // Length
	toSend.WriteShort(3, 0x21); // SubCmd
	toSend.WriteByte(5, abilityID); // Ability ID
	toSend.WriteByte(6, 0); // On/off
	socket.Send(toSend);
	toSend.Free();
}

function DeactivateSpecialMove(pUser, abilityID )
{
	var socket = pUser.socket;
    var toSend = new Packet;

    toSend.ReserveSize( 7 )
    toSend.WriteByte( 0, 0xbf ); // Packet
    toSend.WriteShort( 1, 0x07 ); // Length
    toSend.WriteShort( 3, 0x21 ); // SubCmd
    toSend.WriteByte( 5, abilityID ); // Ability ID
    toSend.WriteByte( 6, 0 ); // On/off
	socket.Send( toSend );
    toSend.Free();
}

function onTimer( timerObj, timerID )
{
	var socket = timerObj.socket;
	var abilityID = timerObj.GetTempTag("abilityID");
	if (timerObj == null || timerObj.dead)
	{
		timerObj.frozen = false;
		timerObj.SetTempTag("IsImmune", false);
		timerObj.RemoveScriptTrigger(7002);
		timerObj.SetTempTag("BlockEquip", false);
		ClearSpecialMove(timerObj, abilityID);
		return;
	}
	else if (timerID == 8000)
	{
		timerObj.frozen = false;
	}
	else if (timerID == 9000) 
	{
		timerObj.SetTempTag("IsImmune", false);
	}
	else if (timerID == 9100) 
	{
		timerObj.RemoveScriptTrigger(7002);
		timerObj.SetTempTag("BlockEquip", false);
		socket.SysMessage("Your confusion has passed, you may now arm a weapon!");
	}
	else if (timerObj.socket)
	{
		//Toggle ability off after 5 second timer has elapsed
		DeactivateSpecialMove(timerObj, timerID);
	}
}