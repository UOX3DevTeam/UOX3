function onSpecialMove( pUser, abilityID )
{
	// Check Skills
	if( !RequiredSkill( pUser, abilityID ))
		return true;

	// Check Mana
	if( !CheckMana( pUser, abilityID ))
		return true;

	pUser.SetTempTag( "abilityID", abilityID );
    return true;

	//The rest of the AOS Abilites before any other expansions
	//Double Strike
	//Whirlwind Attack
}

function checkSkillRequirement( pUser, requiredSkillLevel, requiredSkill, skillMessage, abilityID ) 
{
	if( pUser.skills[requiredSkill] < requiredSkillLevel )
	{
		pUser.SysMessage( "You need " + skillMessage + " weapon skill to perform that attack" );
		DeactivateSpecialMove( pUser, abilityID );
		return false;
	}
	return true;
}

// archery 31 swordsmansip 40 macefighting 41 fencing 42 wrestling 43
// Define the RequiredSkill function
function RequiredSkill( pUser, abilityID )
{
	// Define weapon types and their skill requirements and if primary or secondary ability
	var weaponTypes = {
		"0x0DF0": { primary: 0, secondary: 11, reqSkill: 41 },
		"0x0DF1": { primary: 0, secondary: 11, reqSkill: 41 },	// Black Staves  // WhirlwindAttack, ParalyzingBlow

		"0x0DF2": { primary: 6, secondary: 5, reqSkill: 41 },
		"0x0DF3": { primary: 6, secondary: 5, reqSkill: 41 },
		"0x0DF4": { primary: 6, secondary: 5, reqSkill: 41 },
		"0x0DF5": { primary: 6, secondary: 5, reqSkill: 41 },	// wands  // Dismount, Disarm

		"0x0E81": { primary: 4, secondary: 5, reqSkill: 41 },  
		"0x0E82": { primary: 4, secondary: 5, reqSkill: 41 },	// Shepherd's Crooks  // CrushingBlow, Disarm

		"0x0e85": { primary: 0, secondary: 5, reqSkill: 40 },
		"0x0e86": { primary: 0, secondary: 5, reqSkill: 40 },	// pickaxe  // DoubleStrike, Disarm

		"0x0E87": { primary: 2, secondary: 6, reqSkill: 42 },
		"0x0E88": { primary: 2, secondary: 6, reqSkill: 42 },	// Pitchforks  // BleedAttack, Dismount

		"0x0E89": { primary: 0, secondary: 3, reqSkill: 41 },
		"0x0E8A": { primary: 0, secondary: 3, reqSkill: 41 },	// Quarter Staves  // DoubleStrike, ConcussionBlow

		"0x0EC2": { primary: 2, secondary: 8, reqSkill: 40 },
		"0x0EC3": { primary: 2, secondary: 8, reqSkill: 40 },	// Cleavers  // BleedAttack, InfectiousStrike

		"0x0EC4": { primary: 12, secondary: 2, reqSkill: 40 },
		"0x0EC5": { primary: 12, secondary: 2, reqSkill: 40 },	// Skinning Knives  // ShadowStrike, BleedAttack

		"0x0F43": { primary: 1, secondary: 5, reqSkill: 40 },
		"0x0F44": { primary: 1, secondary: 5, reqSkill: 40 },	// hatchets  // ArmorIgnore, Disarm

		"0x0F45": { primary: 2, secondary: 9, reqSkill: 40 },
		"0x0F46": { primary: 2, secondary: 9, reqSkill: 40 },	// Executioner Axes  // BleedAttack, MortalStrike

		"0x0F47": { primary: 2, secondary: 3, reqSkill: 40 },
		"0x0F48": { primary: 2, secondary: 3, reqSkill: 40 },	// Battle Axes  // BleedAttack, ConcussionBlow

		"0x0F49": { primary: 4, secondary: 6, reqSkill: 40 },
		"0x0F4A": { primary: 4, secondary: 6, reqSkill: 40 },	// Axes  // CrushingBlow, Dismount

		"0x0F4B": { primary: 0, secondary: 0, reqSkill: 40 },
		"0x0F4C": { primary: 0, secondary: 0, reqSkill: 40 },	// Double Axe  // DoubleStrike, WhirlwindAttack

		"0x0F4D": { primary: 11, secondary: 6, reqSkill: 40 },
		"0x0F4E": { primary: 11, secondary: 6, reqSkill: 40 },	// Bardiches  // ParalyzingBlow, Dismount

		"0x0F4F": { primary: 3, secondary: 9, reqSkill: 31 },
		"0x0F50": { primary: 3, secondary: 9, reqSkill: 31 },	// Crossbows  // ConcussionBlow, MortalStrike

		"0x0F51": { primary: 8, secondary: 12, reqSkill: 40 },
		"0x0F52": { primary: 8, secondary: 12, reqSkill: 40 },	// Daggers  // InfectiousStrike, ShadowStrike

		"0x0F5C": { primary: 3, secondary: 5, reqSkill: 41 },
		"0x0F5D": { primary: 3, secondary: 5, reqSkill: 41 },	// Maces  // ConcussionBlow, Disarm

		"0x0F5E": { primary: 4, secondary: 1, reqSkill: 40 },
		"0x0F5F": { primary: 4, secondary: 1, reqSkill: 40 },	// Broadswords  // CrushingBlow, ArmorIgnore

		"0x13B7": { primary: 1, secondary: 3, reqSkill: 40 },
		"0x13B8": { primary: 1, secondary: 3, reqSkill: 40 },
		"0x0F60": { primary: 1, secondary: 3, reqSkill: 40 },
		"0x0F61": { primary: 1, secondary: 3, reqSkill: 40 },	// Longswords  // ArmorIgnore, ConcussionBlow

		"0x0F62": { primary: 1, secondary: 11, reqSkill: 42 },
		"0x0F63": { primary: 1, secondary: 11, reqSkill: 42 },	// Spears  // ArmorIgnore, ParalyzingBlow

		"0x0FB4": { primary: 4, secondary: 12, reqSkill: 41 },
		"0x0FB5": { primary: 4, secondary: 12, reqSkill: 41 },	// Sledge hammers // CrushingBlow, ShadowStrike

		"0x13AF": { primary: 1, secondary: 2, reqSkill: 40 },
		"0x13B0": { primary: 1, secondary: 2, reqSkill: 40 },	// War Axes // ArmorIgnore, BleedAttack

		"0x13B1": { primary: 11, secondary: 9, reqSkill: 31 },
		"0x13B2": { primary: 11, secondary: 9, reqSkill: 31 },	// Bows // ParalyzingBlow, MortalStrike

		"0x13B3": { primary: 12, secondary: 6, reqSkill: 41 },
		"0x13B4": { primary: 12, secondary: 6, reqSkill: 41 },	// Clubs // ShadowStrike, Dismount

		"0x13B5": { primary: 0, secondary: 11, reqSkill: 40 },
		"0x13B6": { primary: 0, secondary: 1, reqSkill: 40 },	// Scimitars // DoubleStrike, ParalyzingBlow

		"0x13B9": { primary: 11, secondary: 4, reqSkill: 40 },
		"0x13BA": { primary: 11, secondary: 4, reqSkill: 40 },	// Viking Swords // ParalyzingBlow, CrushingBlow

		"0x13FC": { primary: 0, secondary: 6, reqSkill: 31 },
		"0x13FD": { primary: 0, secondary: 6, reqSkill: 31 },	// Heavy Crossbows // MovingShot, Dismount

		"0x13E3": { primary: 4, secondary: 12, reqSkill: 41 },
		"0x13E4": { primary: 4, secondary: 12, reqSkill: 41 },	// Smith's Hammers // CrushingBlow, ShadowStrike

		"0x13F6": { primary: 8, secondary: 5, reqSkill: 40 },
		"0x13F7": { primary: 8, secondary: 5, reqSkill: 40 },	// Butcher Knives // InfectiousStrike,Disarm

		"0x13F8": { primary: 3, secondary: 0, reqSkill: 41 },
		"0x13F9": { primary: 3, secondary: 0, reqSkill: 41 },	// Gnarled Staves // ConcussionBlow,ForceOfNature

		"0x13FA": { primary: 0, secondary: 2, reqSkill: 40 },
		"0x13FB": { primary: 0, secondary: 2, reqSkill: 40 },	// Large Battle Axes // WhirlwindAttack,BleedAttack

		"0x13FE": { primary: 0, secondary: 1, reqSkill: 40 },
		"0x13FF": { primary: 0, secondary: 1, reqSkill: 40 },	// Katana // DoubleStrike,ArmorIgnore

		"0x1400": { primary: 1, secondary: 8, reqSkill: 42 },
		"0x1401": { primary: 1, secondary: 8, reqSkill: 42 },	// Kryss // ArmorIgnore,InfectiousStrike

		"0x1402": { primary: 12, secondary: 9, reqSkill: 42 },
		"0x1403": { primary: 12, secondary: 9, reqSkill: 42 },	// Short Spears // ShadowStrike,MortalStrike

		"0x1404": { primary: 2, secondary: 5, reqSkill: 42 },
		"0x1405": { primary: 2, secondary: 5, reqSkill: 42 },	// War Forks // BleedAttack,Disarm

		"0x1406": { primary: 4, secondary: 9, reqSkill: 41 },
		"0x1407": { primary: 4, secondary: 9, reqSkill: 41 },	// War Maces // CrushingBlow,MortalStrike

		"0x1438": { primary: 0, secondary: 4, reqSkill: 41 },
		"0x1439": { primary: 0, secondary: 4, reqSkill: 41 },	// War Hammers // WhirlwindAttack,CrushingBlow

		"0x143A": { primary: 0, secondary: 3, reqSkill: 41 },
		"0x143B": { primary: 0, secondary: 3, reqSkill: 41 },	// Mauls // DoubleStrike,ConcussionBlow

		"0x143C": { primary: 1, secondary: 9, reqSkill: 41 },
		"0x143D": { primary: 1, secondary: 9, reqSkill: 41 },	// Hammer Picks // ArmorIgnore,MortalStrike

		"0x143E": { primary: 0, secondary: 3, reqSkill: 40 },
		"0x143F": { primary: 0, secondary: 3, reqSkill: 40 },	// Halberds // WhirlwindAttack,ConcussionBlow

		"0x1440": { primary: 2, secondary: 12, reqSkill: 40 },
		"0x1441": { primary: 2, secondary: 12, reqSkill: 40 },	// Cutlasses // BleedAttack,ShadowStrike

		"0x1442": { primary: 0, secondary: 12, reqSkill: 40 },
		"0x1443": { primary: 0, secondary: 12, reqSkill: 40 }	// Two Handed Axes // DoubleStrike,ShadowStrike
	};

	// Get items in user's hands
	var itemRHand = pUser.FindItemLayer( 0x01 );
	var itemLHand = pUser.FindItemLayer( 0x02 );

	// Check if either hand has an item
	if( itemRHand != null )
	{
		// Check item in the right hand
		if( weaponTypes[itemRHand.sectionID] )
		{
			var weapon = weaponTypes[itemRHand.sectionID];

			if( abilityID == weapon.primary )
			{
				return checkSkillRequirement( pUser, 700, weapon.reqSkill, "70", abilityID );
			}
			else if( abilityID == weapon.secondary )
			{
				return checkSkillRequirement( pUser, 900, weapon.reqSkill, "90", abilityID );
			}
		}
	}
	else if( itemLHand != null )
	{
		// Check item in the left hand
		if( weaponTypes[itemLHand.sectionID] )
		{
			var weapon = weaponTypes[itemLHand.sectionID];

			if ( abilityID == weapon.primary )
			{
				return checkSkillRequirement( pUser, 700, weapon.reqSkill, "70", abilityID );
			}
			else if( abilityID == weapon.secondary )
			{
				return checkSkillRequirement( pUser, 900, weapon.reqSkill, "90", abilityID );
			}
		}
	}
	else// checking if both hands null if so, it is a wrestling skill
	{
		if( abilityID == 5 )
		{
			return checkSkillRequirement( pUser, 700, 43, "70" );
		}
		else if( abilityID == 11 )
		{
			return checkSkillRequirement( pUser, 900, 43, "90" );
		}
	}

	return true;
}

function CheckMana( pUser, abilityID )
{
	var abilitieMana = {
		1: { manaAmount: 14 },
		2: { manaAmount: 30 },
		3: { manaAmount: 20 },
		4: { manaAmount: 20 },
		5: { manaAmount: 20 },
		6: { manaAmount: 16 },
		7: { manaAmount: 20 },
		8: { manaAmount: 20 },
		9: { manaAmount: 30 },
		10: { manaAmount: 30 },
		11: { manaAmount: 30 },
		12: { manaAmount: 20 }
	};

	var NextUse = pUser.GetTempTag( "doubleMana" );
	var iTime = GetCurrentClock();
	var requiredMana = abilitieMana[abilityID].manaAmount;

	if( pUser.mana <= requiredMana )
    {
		pUser.SysMessage( "You need " + requiredMana + " mana to perform that attack" );
        DeactivateSpecialMove( pUser, abilityID );
        return false;
    }
    else
	{
		pUser.SetTempTag( "doubleMana", iTime.toString() );
		var Delay = 3000;// If abilitie is performed with 3 seconds of last abilitie it will then cost double mana
		if(( iTime - NextUse ) < Delay )
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
	if( abilityID == 1 ) // armorignore
    {
        // Armor Ignore ignores defense modifiers, but deals only 90% of potential damage
        damage *= 0.9;

        if( fightSkill == 31 ) // Archery
        {
            // Cap damage from Armor Strike attack at 30 for archery weapons
            if( damage > 30 )
                damage = 30;
        }
        else
		{
            // For all othe rfighting skills, cap damage from Armor Strike at 35
            if( damage > 35 )
                damage = 35;
        }
    }
	else if( abilityID == 12 )// shadowstrike
	{
		damage *= 1.25;
	}
	else if( abilityID == 4 )// crushingblow
	{
		damage *= 1.5;
	}
	else if( abilityID == 3 )// ConcussionBlow
	{	
		if (pDefender.maxhp > 0) 
		{
			var hitsPercent = ( pDefender.hp / pDefender.maxhp ) * 100.0;

            var manaPercent = 0;

			if( pDefender.maxmana > 0 )
				manaPercent = ( pDefender.mana / pDefender.maxmana ) * 100.0;

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

	onAbility(pAttacker, pDefender, abilityID);
}

function onAbility(pAttacker, pDefender, abilityID)
{
	if (abilityID == 1) // armnor ignore
	{
		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 1)
			DeactivateSpecialMove(pAttacker.socket, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		pAttacker.TextMessage("Your attack penetrates their armor!");
		pDefender.TextMessage("The blow penetrated your armor!");

		pDefender.SoundEffect(0x0056, true);
		pDefender.StaticEffect(0x3728, 0x09, 0x06);

		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 2) // bleedattack
	{
		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 2)
			DeactivateSpecialMove(pAttacker, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		pAttacker.SysMessage("Your target is bleeding!");
		pDefender.SysMessage("You are bleeding!");

		pDefender.TextMessage("You are bleeding profusely", false, 0x21);
		pDefender.TextMessage(pDefender.name + " is bleeding profusely", true, 0x21, 1);

		pDefender.StartTimer(10000, 9300, 7001);
		pDefender.StartTimer(2000, 9400, 7001);
		pDefender.SetTempTag("doBleed", true);

		if (pDefender.socket)
			TriggerEvent(50104, "AddBuff", pDefender, 1039, 1075829, 1075830, 10, " 1, 10 ,2");

		pAttacker.SoundEffect(0x133, true);
		pDefender.StaticEffect(0x377A, 0x09, 0x32);

		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 3) // ConcussionBlow
	{

		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 3)
			DeactivateSpecialMove(pAttacker, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		pAttacker.SysMessage("You have delivered a concussion!");
		pDefender.SysMessage("You feel disoriented!");

		pAttacker.SoundEffect(0x213, true);
		pDefender.StaticEffect(0x377A, 0x09, 0x32);

		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 4) // crushingblow
	{

		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 4)
			DeactivateSpecialMove(pAttacker, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		pAttacker.SysMessage("You have delivered a crushing blow!");
		pDefender.SysMessage("You take extra damage from the crushing attack!");

		pAttacker.SoundEffect(0x1E1, true);
	}
	else if (abilityID == 5) // Disarm
	{
		var itemRHand = pDefender.FindItemLayer(0x01);
		var itemLHand = pDefender.FindItemLayer(0x02);

		if (pDefender.pack == null || itemLHand != null && itemLHand.movable >= 2 || itemRHand != null && itemRHand.movable >= 2) {
			pAttacker.SysMessage("You cannot disarm your opponent.");
			DeactivateSpecialMove(pAttacker.socket, abilityID);
			return false;
		}

		if (itemLHand != null && itemLHand.type == 9 || itemRHand != null && itemRHand.type == 9) {
			pAttacker.SysMessage("Your target is already unarmed!");
			DeactivateSpecialMove(pAttacker.socket, abilityID);
			return false;
		}

		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 5)
			DeactivateSpecialMove(pAttacker, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		pAttacker.SoundEffect(0x3B9, true);
		pDefender.StaticEffect(0x37BE, 0x09, 0x32);

		pAttacker.SysMessage("You disarm their weapon!");
		pDefender.SysMessage("Your weapon has been disarmed!");

		if (itemLHand != null) {
			itemLHand.container = pDefender.pack;
		}

		if (itemRHand != null) {
			itemRHand.container = pDefender.pack;
		}

		pDefender.AddScriptTrigger(7002);//block equip for 5 seconds script added
		pDefender.SetTempTag("BlockEquip", true);
		pDefender.StartTimer(5000, 9100, 7001);

		TriggerEvent(50104, "AddBuff", pDefender, 0x3ea, 1075637, 0, 5, " ");

		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 6) // Dismount
	{
		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 6)
			DeactivateSpecialMove(pAttacker.socket, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		// Check to see if player is mount or flying.
		if (pAttacker.isonhorse) {
			pAttacker.TextMessage("You cannot perform that attack while mounted or flying!");
			DeactivateSpecialMove(pAttacker.socket, abilityID);
			return true;
		}

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
	else if (abilityID == 8) // Infectious Strike
	{
		var itemRHand = pAttacker.FindItemLayer(0x01);
		var itemLHand = pAttacker.FindItemLayer(0x02);

		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 8)
			DeactivateSpecialMove(pAttacker, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		if (itemLHand != null && itemLHand.poison <= 0 || itemRHand != null && itemRHand.poison <= 0) {
			pDefender.SysMessage("Your weapon must have a dose of poison to perform an infectious strike!");
			return;
		}

		var level = 0;
		var chance = Math.random();
		if (pAttacker.skills[30] >= 0 && pAttacker.skills[30] <= 199) {
			level = 1;
		}
		else if (pAttacker.skills[30] >= 200 && pAttacker.skills[30] <= 399) {
			level = 2;
		}
		else if (pAttacker.skills[30] >= 400 && pAttacker.skills[30] <= 599) {
			level = 3;
		}
		else if (pAttacker.skills[30] >= 600 && pAttacker.skills[30] <= 1000) {
			level = 4;
		}

		// Adjust the poison level based on chance
		if (chance < 0.2) {
			level--; // Decrease the level by 1
			if (level < 0) {
				level = 1; // Ensure the level doesn't go below 0 and is always set to least 1
			}
		}
		else if (chance > 0.8) {
			level++; // Increase the level by 1
			pAttacker.SysMessage("Your precise strike has increased the level of the poison by 1");
			pDefender.SysMessage("The poison seems extra effective!");
		}

		pAttacker.TextMessage("You have poisoned your target : " + pDefender.name);
		pDefender.TextMessage(pAttacker.name + " : poisoned you!");
		pDefender.poison = level;

		pAttacker.SoundEffect(0xDD, true);
		pDefender.StaticEffect(0x3728, 0x09, 0x32);
		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 9) // Mortal Strike
	{// turn healthbar yellow on defender need spacket sent for this.

		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 9)
			DeactivateSpecialMove(pAttacker, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		pAttacker.SysMessage("You deliver a mortal wound!");
		pDefender.SysMessage("You have been mortally wounded!");

		pAttacker.SoundEffect(0x1E1, true);
		pDefender.StaticEffect(0x37B9, 0x09, 0x32);

		pDefender.SetTempTag("blockHeal", true);

		var seconds = 6000; // We want this applied to players even if they are "offline" (aka disconnected but not vanished from view yet)
		if (pDefender.npc) {
			seconds = 12000;
		}
		pDefender.StartTimer(seconds, 9200, true);

		if (pDefender.socket)
			TriggerEvent(50104, "AddBuff", pDefender, 1027, 1075810, 1075811, 6, " ");

		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 11) // ParalyzingBlow
	{
		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 11)
			DeactivateSpecialMove(pAttacker.socket, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

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
		pDefender.StartTimer(seconds + 8000, 9000, 7001);
		pDefender.StartTimer(seconds, 8000, 7001);
		pDefender.frozen = true;

		if (pAttacker.socket) {
			pAttacker.TextMessage(GetDictionaryEntry(17702, pAttacker.socket.language), false, 0x3b2, 0, pAttacker.serial);// You deliver a paralyzing blow!
		}
		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
	}
	else if (abilityID == 12) // shadowstrike
	{
		if (pAttacker.skills[47] < 800) // Stealth
		{
			pAttacker.TextMessage("You lack the required stealth to perform that attack");
			DeactivateSpecialMove(pAttacker.socket, abilityID);
			return true;
		}

		// Clear out any current ability the player is doing when he switches abilities
		if (abilityID != 12)
			DeactivateSpecialMove(pAttacker.socket, abilityID);

		//checking mana
		if (!CheckMana(pAttacker, abilityID))
			return true;

		pAttacker.TextMessage("You strike and hide in the shadows!");
		pDefender.TextMessage("You are dazed by the attack and your attacker vanishes!");

		pAttacker.StaticEffect(0x376A, 0x09, 0x06);

		pDefender.SoundEffect(0x482, true);
		pDefender.StaticEffect(0x37BE, 0x09, 0x06);

		pAttacker.atWar = false;
		pDefender.atWar = false;
		pAttacker.visible = 1;

		ClearSpecialMove(pAttacker, abilityID);// Clear the Ability after success
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
	pUser.SetTempTag("abilityID", null);

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
	var damage = 31;

	if (timerObj == null || timerObj.dead)
	{
		timerObj.frozen = false;
		timerObj.SetTempTag("IsImmune", null);
		timerObj.RemoveScriptTrigger(7002);
		timerObj.SetTempTag("BlockEquip", null);
		timerObj.SetTempTag("blockHeal", null);
		timerObj.KillJSTimer(9400, 7001);
		timerObj.SetTempTag("doBleed", null);
		ClearSpecialMove(timerObj, abilityID);
		return;
	}
	else if (timerID == 8000)
	{
		timerObj.frozen = false;
	}
	else if (timerID == 9000) 
	{
		timerObj.SetTempTag("IsImmune", null);
	}
	else if (timerID == 9100) 
	{
		timerObj.RemoveScriptTrigger(7002);
		timerObj.SetTempTag("BlockEquip", null);
		socket.SysMessage("Your confusion has passed, you may now arm a weapon!");
	}
	else if (timerID == 9200)
	{
		timerObj.SetTempTag("blockHeal", null);
		socket.SysMessage("You are no longer mortally wounded.");
	}
	else if (timerID == 9300) 
	{
		timerObj.SetTempTag("doBleed", null);
		timerObj.KillJSTimer(9400, 7001);
		socket.SysMessage("The bleeding wounds have healed, you are no longer bleeding!");
	}
	else if (timerID == 9400)
	{
		var damage = RandomNumber(1, 10);
		timerObj.health -= damage;
		//timerObj.StaticEffect(0x122A, 0, 15); // blood effect got to figure how to add it to ground.
		timerObj.StartTimer(2000, 9400, 7001);//restart timer every 2 seconds until it shuts off
	}
}