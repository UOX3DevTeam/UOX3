function onSpecialMove( pUser, abilityID )
{
	// Define the array of restricted ability IDs
	var restrictedAbilities = [7, 10, 13, 14, 15, 16, 17,18,19,20,21,22,23,24,25,29];

	// Check if the abilityID is in the array of restricted abilities
	if( restrictedAbilities.indexOf( abilityID ) != -1 )
	{
		pUser.SysMessage( "This ability is not yet available." );
		TriggerEvent( 2205, "DeactivateSpecialMove", pUser, abilityID );
		return false;
	}

	// Check Skills
	if( !checkRequiredSkill( pUser, abilityID ))
		return true;

	// Check Mana
	if( !CheckMana(  pUser, abilityID ))
		return true;

	if( abilityID >= 1 )
		pUser.SetTempTag( "abilityID", abilityID );

    return true;
}

function checkSkillRequirement( pUser, requiredSkillLevel, requiredSkill, skillMessage, abilityID ) 
{
	var pSock = pUser.socket;
	if( pUser.skills[requiredSkill] < requiredSkillLevel )
	{
		pSock.SysMessage( GetDictionaryEntry( 19201, pSock.language), skillMessage); // You need %i weapon skill to perform that attack

		TriggerEvent( 2205, "DeactivateSpecialMove", pUser, abilityID );
		return false;
	}
	return true;
}

// archery 31 swordsmansip 40 macefighting 41 fencing 42 wrestling 43
// Define the RequiredSkill function
function checkRequiredSkill( pUser, abilityID )
{
	// Define weapon types and their skill requirements and if primary or secondary ability
	var weaponTypes = {
		"0x0df0": { primary: 13, secondary: 11, reqSkill: 41 },
		"0x0df1": { primary: 13, secondary: 11, reqSkill: 41 },	// Black Staves  // WhirlwindAttack, ParalyzingBlow

		"0x0df2": { primary: 6, secondary: 5, reqSkill: 41 },
		"0x0df3": { primary: 6, secondary: 5, reqSkill: 41 },
		"0x0df4": { primary: 6, secondary: 5, reqSkill: 41 },
		"0x0df5": { primary: 6, secondary: 5, reqSkill: 41 },	// wands  // Dismount, Disarm

		"0x0e81": { primary: 4, secondary: 5, reqSkill: 41 },  
		"0x0e82": { primary: 4, secondary: 5, reqSkill: 41 },	// Shepherd's Crooks  // CrushingBlow, Disarm

		"0x0e85": { primary: 7, secondary: 5, reqSkill: 40 },
		"0x0e86": { primary: 7, secondary: 5, reqSkill: 40 },	// pickaxe  // DoubleStrike, Disarm

		"0x0e87": { primary: 2, secondary: 6, reqSkill: 42 },
		"0x0e88": { primary: 2, secondary: 6, reqSkill: 42 },	// Pitchforks  // BleedAttack, Dismount

		"0x0e89": { primary: 7, secondary: 3, reqSkill: 41 },
		"0x0e8a": { primary: 7, secondary: 3, reqSkill: 41 },	// Quarter Staves  // DoubleStrike, ConcussionBlow

		"0x0ec2": { primary: 2, secondary: 8, reqSkill: 40 },
		"0x0ec3": { primary: 2, secondary: 8, reqSkill: 40 },	// Cleavers  // BleedAttack, InfectiousStrike

		"0x0ec4": { primary: 12, secondary: 2, reqSkill: 40 },
		"0x0ec5": { primary: 12, secondary: 2, reqSkill: 40 },	// Skinning Knives  // ShadowStrike, BleedAttack

		"0x0f43": { primary: 1, secondary: 5, reqSkill: 40 },
		"0x0f44": { primary: 1, secondary: 5, reqSkill: 40 },	// hatchets  // ArmorIgnore, Disarm

		"0x0f45": { primary: 2, secondary: 9, reqSkill: 40 },
		"0x0f46": { primary: 2, secondary: 9, reqSkill: 40 },	// Executioner Axes  // BleedAttack, MortalStrike

		"0x0f47": { primary: 2, secondary: 3, reqSkill: 40 },
		"0x0f48": { primary: 2, secondary: 3, reqSkill: 40 },	// Battle Axes  // BleedAttack, ConcussionBlow

		"0x0f49": { primary: 4, secondary: 6, reqSkill: 40 },
		"0x0f4a": { primary: 4, secondary: 6, reqSkill: 40 },	// Axes  // CrushingBlow, Dismount

		"0x0f4b": { primary: 7, secondary: 13, reqSkill: 40 },
		"0x0f4c": { primary: 7, secondary: 13, reqSkill: 40 },	// Double Axe  // DoubleStrike, WhirlwindAttack

		"0x0f4e": { primary: 11, secondary: 6, reqSkill: 40 },
		"0x0f4e": { primary: 11, secondary: 6, reqSkill: 40 },	// Bardiches  // ParalyzingBlow, Dismount

		"0x0f4f": { primary: 3, secondary: 9, reqSkill: 31 },
		"0x0f50": { primary: 3, secondary: 9, reqSkill: 31 },	// Crossbows  // ConcussionBlow, MortalStrike

		"0x0f51": { primary: 8, secondary: 12, reqSkill: 40 },
		"0x0f52": { primary: 8, secondary: 12, reqSkill: 40 },	// Daggers  // InfectiousStrike, ShadowStrike

		"0x0f5c": { primary: 3, secondary: 5, reqSkill: 41 },
		"0x0f5d": { primary: 3, secondary: 5, reqSkill: 41 },	// Maces  // ConcussionBlow, Disarm

		"0x0f5e": { primary: 4, secondary: 1, reqSkill: 40 },
		"0x0f5f": { primary: 4, secondary: 1, reqSkill: 40 },	// Broadswords  // CrushingBlow, ArmorIgnore

		"0x13b7": { primary: 1, secondary: 3, reqSkill: 40 },
		"0x13b8": { primary: 1, secondary: 3, reqSkill: 40 },
		"0x0f60": { primary: 1, secondary: 3, reqSkill: 40 },
		"0x0f61": { primary: 1, secondary: 3, reqSkill: 40 },	// Longswords  // ArmorIgnore, ConcussionBlow

		"0x0f62": { primary: 1, secondary: 11, reqSkill: 42 },
		"0x0f63": { primary: 1, secondary: 11, reqSkill: 42 },	// Spears  // ArmorIgnore, ParalyzingBlow

		"0x0fb4": { primary: 4, secondary: 12, reqSkill: 41 },
		"0x0fb5": { primary: 4, secondary: 12, reqSkill: 41 },	// Sledge hammers // CrushingBlow, ShadowStrike

		"0x13af": { primary: 1, secondary: 2, reqSkill: 40 },
		"0x13b0": { primary: 1, secondary: 2, reqSkill: 40 },	// War Axes // ArmorIgnore, BleedAttack

		"0x13b1": { primary: 11, secondary: 9, reqSkill: 31 },
		"0x13b2": { primary: 11, secondary: 9, reqSkill: 31 },	// Bows // ParalyzingBlow, MortalStrike

		"0x13b3": { primary: 12, secondary: 6, reqSkill: 41 },
		"0x13b4": { primary: 12, secondary: 6, reqSkill: 41 },	// Clubs // ShadowStrike, Dismount

		"0x13b5": { primary: 7, secondary: 11, reqSkill: 40 },
		"0x13b6": { primary: 7, secondary: 11, reqSkill: 40 },	// Scimitars // DoubleStrike, ParalyzingBlow

		"0x13b9": { primary: 11, secondary: 4, reqSkill: 40 },
		"0x13ba": { primary: 11, secondary: 4, reqSkill: 40 },	// Viking Swords // ParalyzingBlow, CrushingBlow

		"0x13fc": { primary: 10, secondary: 6, reqSkill: 31 },
		"0x13fd": { primary: 10, secondary: 6, reqSkill: 31 },	// Heavy Crossbows // MovingShot, Dismount

		"0x13e3": { primary: 4, secondary: 12, reqSkill: 41 },
		"0x13e4": { primary: 4, secondary: 12, reqSkill: 41 },	// Smith's Hammers // CrushingBlow, ShadowStrike

		"0x13f6": { primary: 8, secondary: 5, reqSkill: 40 },
		"0x13f7": { primary: 8, secondary: 5, reqSkill: 40 },	// Butcher Knives // InfectiousStrike,Disarm

		"0x13f8": { primary: 3, secondary: 0, reqSkill: 41 },
		"0x13f9": { primary: 3, secondary: 0, reqSkill: 41 },	// Gnarled Staves // ConcussionBlow,ForceOfNature

		"0x13fa": { primary: 13, secondary: 2, reqSkill: 40 },
		"0x13fb": { primary: 13, secondary: 2, reqSkill: 40 },	// Large Battle Axes // WhirlwindAttack,BleedAttack

		"0x13fe": { primary: 7, secondary: 1, reqSkill: 40 },
		"0x13ff": { primary: 7, secondary: 1, reqSkill: 40 },	// Katana // DoubleStrike,ArmorIgnore

		"0x1400": { primary: 1, secondary: 8, reqSkill: 42 },
		"0x1401": { primary: 1, secondary: 8, reqSkill: 42 },	// Kryss // ArmorIgnore,InfectiousStrike

		"0x1402": { primary: 12, secondary: 9, reqSkill: 42 },
		"0x1403": { primary: 12, secondary: 9, reqSkill: 42 },	// Short Spears // ShadowStrike,MortalStrike

		"0x1404": { primary: 2, secondary: 5, reqSkill: 42 },
		"0x1405": { primary: 2, secondary: 5, reqSkill: 42 },	// War Forks // BleedAttack,Disarm

		"0x1406": { primary: 4, secondary: 9, reqSkill: 41 },
		"0x1407": { primary: 4, secondary: 9, reqSkill: 41 },	// War Maces // CrushingBlow,MortalStrike

		"0x1438": { primary: 13, secondary: 4, reqSkill: 41 },
		"0x1439": { primary: 13, secondary: 4, reqSkill: 41 },	// War Hammers // WhirlwindAttack,CrushingBlow

		"0x143a": { primary: 7, secondary: 3, reqSkill: 41 },
		"0x143b": { primary: 7, secondary: 3, reqSkill: 41 },	// Mauls // DoubleStrike,ConcussionBlow

		"0x143c": { primary: 1, secondary: 9, reqSkill: 41 },
		"0x143d": { primary: 1, secondary: 9, reqSkill: 41 },	// Hammer Picks // ArmorIgnore,MortalStrike

		"0x143e": { primary: 13, secondary: 3, reqSkill: 40 },
		"0x143f": { primary: 13, secondary: 3, reqSkill: 40 },	// Halberds // WhirlwindAttack,ConcussionBlow

		"0x1440": { primary: 2, secondary: 12, reqSkill: 40 },
		"0x1441": { primary: 2, secondary: 12, reqSkill: 40 },	// Cutlasses // BleedAttack,ShadowStrike

		"0x1442": { primary: 7, secondary: 12, reqSkill: 40 },
		"0x1443": { primary: 7, secondary: 12, reqSkill: 40 }	// Two Handed Axes // DoubleStrike,ShadowStrike
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

			if(  abilityID == weapon.primary )
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

function getAbilityManaTable()
{// Mana is Based off 2003 AOS and 2005 ML
	return {
		0: { manaAmount: 0 },		// Cancel Ability Attempt
		1: { manaAmount: 30 },		// Armor Ignore // Start AOS abiltiies
		2: { manaAmount: 30 },		// Bleed Attack
		3: { manaAmount: 30 },		// Concusion Blow
		4: { manaAmount: 30 },		// Crushing Blow
		5: { manaAmount: 20 },		// Disarm
		6: { manaAmount: 20 },		// Dismount
		7: { manaAmount: 30 },		// Double Strike
		8: { manaAmount: 20 },		// Infecting
		9: { manaAmount: 30 },		// Mortal Strike
		10: { manaAmount: 20 },		// Moving Shot
		11: { manaAmount: 35 },		// Paralyzing Blow
		12: { manaAmount: 25 },		// Shadow Strike
		13: { manaAmount: 25 },		// Whirlwind Attack // End AOS abilities
		14: { manaAmount: 30 },		// Riding Swipe // Start SE abiltiies
		15: { manaAmount: 30 },		// Frenzied Whirlwind
		16: { manaAmount: 30 },		// Block
		17: { manaAmount: 30 },		// Defense Mastery
		18: { manaAmount: 30 },		// Nerve Strike
		19: { manaAmount: 30 },		// Talon Strike
		20: { manaAmount: 30 },		// Feint
		21: { manaAmount: 30 },		// Dual Wield
		22: { manaAmount: 30 },		// Double shot
		23: { manaAmount: 30 },		// Armor Peirce // End SE abilities
		24: { manaAmount: 25 },		// Bladeweave // Start ML abilities
		25: { manaAmount: 15 },		// Force Arrow
		26: { manaAmount: 15 },		// Lightning Arrow
		27: { manaAmount: 25 },		// Psychic Attack
		28: { manaAmount: 35 },		// Serpent Arrow
		29: { manaAmount: 35 }		// Force of Nature // End ML abilities
	};
}

function CheckMana( pUser, abilityID )
{
	var pSock = pUser.socket;
	var abilitieMana = getAbilityManaTable();
	var NextUse = pUser.GetTempTag( "doubleMana" );
	var iTime = GetCurrentClock();
	var Delay = 3000; // If ability is performed within 3 seconds of the last ability, it will then cost double mana
	var requiredMana = abilitieMana[abilityID].manaAmount;

	if( NextUse == null )
	{
		NextUse = 0;
	}

	var isDoubleMana = ( iTime - NextUse ) < Delay;
	if( isDoubleMana )
	{
		requiredMana *= 2;
	}

	if( pUser.mana < requiredMana )
	{
		pSock.SysMessage( GetDictionaryEntry( 19202, pSock.language), requiredMana); // You need %i mana to perform that attack
		TriggerEvent( 2205, "DeactivateSpecialMove", pUser, abilityID );
		return false;
	}
	else
	{
		return true;
	}
}

function DeductMana(  pUser, abilityID )
{
	var abilitieMana = getAbilityManaTable();
	var NextUse = pUser.GetTempTag( "doubleMana" );
	var iTime = GetCurrentClock();
	var Delay = 3000; // If ability is performed within 3 seconds of the last ability, it will then cost double mana
	var requiredMana = abilitieMana[abilityID].manaAmount;

	if( NextUse == null )
	{
		NextUse = 0;
	}

	var isDoubleMana = ( iTime - NextUse ) < Delay;
	if( isDoubleMana )
	{
		requiredMana *= 2;
	}

	pUser.mana -= requiredMana;
	pUser.SetTempTag( "doubleMana", iTime.toString() );
}

function onCombatDamageCalc( pAttacker, pDefender, fightSkill, hitLoc )
{
	var abilityID = pAttacker.GetTempTag( "abilityID" );
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
		if( pDefender.maxhp > 0) 
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
	if( pAttacker.npc && !pDefender.npc )
		damage /= GetServerSetting( "NPCDAMAGERATE" );

    return damage;
}

function onCombatHit( pAttacker, pDefender )
{
	var abilityID = pAttacker.GetTempTag( "abilityID" );

	onAbility( pAttacker, pDefender, abilityID );
}

function onAttack( pAttacker, pDefender )
{
	var abilityID = pAttacker.GetTempTag( "abilityID" );

	onAbility( pAttacker, pDefender, abilityID );
}

function onAbility( pAttacker, pDefender, abilityID )
{
	var pSockAttacker = pAttacker.socket;
	var pSockDefender = pDefender.socket;

	if( abilityID == 1 ) // armnor ignore
	{
		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 1 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		pSockAttacker.SysMessage( GetDictionaryEntry( 19203, pSockAttacker.language ));// Your attack penetrates their armor!
		if( pSockDefender != null )
			pSockDefender.SysMessage( GetDictionaryEntry( 19204, pSockDefender.language ));// The blow penetrated your armor!

		pDefender.SoundEffect( 0x0056, true );
		pDefender.StaticEffect( 0x3728, 0x09, 0x06 );

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 2 ) // bleedattack
	{
		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 2 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		pSockAttacker.SysMessage( GetDictionaryEntry( 19205, pSockAttacker.language ));// Your target is bleeding!
		if( pSockDefender != null )
		{
			pSockDefender.SysMessage(GetDictionaryEntry(19206, pSockDefender.language));// You are bleeding!

			pSockDefender.TextMessage(GetDictionaryEntry(19207, pSockDefender.language), false, 0x21);// You are bleeding profusely
			pSockDefender.TextMessage(pDefender.name, GetDictionaryEntry(19208, pSockDefender.language), true, 0x21, 1);// %i is bleeding profusely
		}

		pDefender.StartTimer( 2000, 9300, 7001 ); // Start 2 second timer for blood and damage last total 10 seconds
		pDefender.SetTempTag( "doBleed", true );

		if( pDefender.socket)
			TriggerEvent(50104, "AddBuff", pDefender, 1039, 1075829, 1075830, 10, " 1, 10 ,2" );

		pAttacker.SoundEffect( 0x133, true );
		pDefender.StaticEffect( 0x377A, 0x09, 0x32 );

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 3 ) // ConcussionBlow
	{
		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 3 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		pSockAttacker.SysMessage( GetDictionaryEntry( 19209, pSockAttacker.language ));// You have delivered a concussion!

		if( pSockDefender != null )
			pSockDefender.SysMessage( GetDictionaryEntry( 19210, pSockDefender.language ));// You feel disoriented!

		pAttacker.SoundEffect( 0x213, true );
		pDefender.StaticEffect( 0x377A, 0x09, 0x32 );

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 4 ) // crushingblow
	{

		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 4 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		pSockAttacker.SysMessage( GetDictionaryEntry( 19211, pSockAttacker.language ));// You have delivered a crushing blow!
		if( pSockDefender != null )
			pSockDefender.SysMessage( GetDictionaryEntry( 19212, pSockDefender.language ));// You take extra damage from the crushing attack!

		pAttacker.SoundEffect( 0x1E1, true );
		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 5 ) // Disarm
	{
		var itemRHand = pDefender.FindItemLayer( 0x01 );
		var itemLHand = pDefender.FindItemLayer( 0x02 );

		if( pDefender.pack == null || itemLHand != null && itemLHand.movable >= 2 || itemRHand != null && itemRHand.movable >= 2 )
		{
			pSockAttacker.SysMessage( GetDictionaryEntry( 19213, pSockAttacker.language ));// You cannot disarm your opponent.
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );
			return false;
		}

		if( itemLHand != null && itemLHand.type == 9 || itemRHand != null && itemRHand.type == 9 ) 
		{
			pSockAttacker.SysMessage( GetDictionaryEntry( 19214, pSockAttacker.language ));// Your target is already unarmed!
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );
			return false;
		}

		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 5 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		pAttacker.SoundEffect( 0x3B9, true );
		pDefender.StaticEffect( 0x37BE, 0x09, 0x32 );

		pSockAttacker.SysMessage( GetDictionaryEntry( 19215, pSockAttacker.language ));// You disarm their weapon!
		if( pSockDefender != null )
			pSockDefender.SysMessage( GetDictionaryEntry( 19216, pSockDefender.language ));// Your weapon has been disarmed!

		if( itemLHand != null ) 
		{
			itemLHand.container = pDefender.pack;
		}

		if( itemRHand != null )
		{
			itemRHand.container = pDefender.pack;
		}

		pDefender.AddScriptTrigger(7002);//block equip for 5 seconds script added
		pDefender.SetTempTag( "blockEquip", true );
		pDefender.StartTimer( 5000, 9100, 7001 );

		TriggerEvent(50104, "AddBuff", pDefender, 0x3ea, 1075637, 0, 5, " " );

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 6 ) // Dismount
	{
		// Check to see if player is mount or flying.
		if( pAttacker.isonhorse || pAttacker.isflying )
		{
			pSockAttacker.SysMessage( GetDictionaryEntry( 19217, pSockAttacker.language ));// You cannot perform that attack while mounted or flying!
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );
			return true;
		}

		// Only Can work on players or npcs that is mounted
		if( !pDefender.isonhorse || !pDefender.isflying )
		{
			pSockAttacker.SysMessage( GetDictionaryEntry( 19218, pSockAttacker.language ));// This attack only works on mounted or flying targets
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );
			return true;
		}

		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 6 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID )) 
		{
			DeductMana( pAttacker, abilityID );
		}

		//Lesser Hiryu have an 80% chance of missing this attack needs added

		//This is the Dismount Information sent to Attacker and Defender
		pDefender.SoundEffect( 0x140, true );
		pDefender.StaticEffect( 0x3728, 0x09, 0x06 );
		pSockAttacker.SysMessage( GetDictionaryEntry( 19219, pSockAttacker.language ));// The force of your attack has dislodged them from their mount!
		pDefender.Dismount();

		if( pSockDefender != null )
			pSockDefender.SysMessage( GetDictionaryEntry( 19220, pSockDefender.language ), pAttacker.name );// You have been knocked off of your mount by %i !

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 7 ) // Double Strike
	{
		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 7 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 8 ) // Infectious Strike
	{
		var itemRHand = pAttacker.FindItemLayer(0x01);
		var itemLHand = pAttacker.FindItemLayer(0x02);

		if( itemLHand != null && itemLHand.poison <= 0 || itemRHand != null && itemRHand.poison <= 0 )
		{
			pSockAttacker.SysMessage( GetDictionaryEntry( 19221, pSockAttacker.language ));// Your weapon must have a dose of poison to perform an infectious strike!
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );
			return;
		}

		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 8 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		var level = 0;
		var chance = Math.random();
		if( pAttacker.skills[30] >= 0 && pAttacker.skills[30] <= 199 )
		{
			level = 1;
		}
		else if( pAttacker.skills[30] >= 200 && pAttacker.skills[30] <= 399 )
		{
			level = 2;
		}
		else if( pAttacker.skills[30] >= 400 && pAttacker.skills[30] <= 599 )
		{
			level = 3;
		}
		else if( pAttacker.skills[30] >= 600 && pAttacker.skills[30] <= 1000 )
		{
			level = 4;
		}

		// Adjust the poison level based on chance
		if( chance < 0.2 )
		{
			level--; // Decrease the level by 1
			if( level < 0 )
			{
				level = 1; // Ensure the level doesn't go below 0 and is always set to least 1
			}
		}
		else if( chance > 0.8 )
		{
			level++; // Increase the level by 1
			pSockAttacker.SysMessage( GetDictionaryEntry( 19222, pSockAttacker.language ));// Your precise strike has increased the level of the poison by 1
			if( pSockDefender != null )
				pSockDefender.SysMessage( GetDictionaryEntry( 19223, pSockDefender.language ));// The poison seems extra effective!
		}

		pSockAttacker.SysMessage( GetDictionaryEntry( 19224, pSockAttacker.language), pDefender.name);// You have poisoned your target : 
		if( pSockDefender != null )
			pSockDefender.SysMessage( pAttacker.name, GetDictionaryEntry( 19225, pSockDefender.language ));// %i : poisoned you!

		pDefender.poison = level;

		pAttacker.SoundEffect( 0xDD, true );
		pDefender.StaticEffect( 0x3728, 0x09, 0x32 );
		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 9 ) // Mortal Strike
	{

		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 9 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		pSockAttacker.SysMessage( GetDictionaryEntry( 19226, pSockAttacker.language ));// You deliver a mortal wound!

		if( pSockDefender != null )
			pSockDefender.SysMessage( GetDictionaryEntry( 19227, pSockDefender.language ));// You have been mortally wounded!

		pAttacker.SoundEffect( 0x1E1, true );
		pDefender.StaticEffect( 0x37B9, 0x09, 0x32 );

		pDefender.SetTempTag( "blockHeal", true );

		var seconds = 6000; // We want this applied to players even if they are "offline" (aka disconnected but not vanished from view yet)
		if( pDefender.npc ) 
		{
			seconds = 12000;
		}
		pDefender.StartTimer(seconds, 9200, true );

		if( pDefender.socket )
			TriggerEvent( 50104, "AddBuff", pDefender, 1027, 1075810, 1075811, 6, " " );

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 10 ) //  Moving Shot
	{
		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 10 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 11 ) // ParalyzingBlow
	{
		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 11 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		pAttacker.SoundEffect( 0x204, true );
		pDefender.StaticEffect( 0x376A, 0x09, 0x32 );
		var isImmune = pDefender.GetTempTag( "isImmune" )

		if( isImmune != null && isImmune == true )
		{
			pSockAttacker.SysMessage( GetDictionaryEntry( 19228, pSockAttacker.language ));// Your target resists paralysis.
			if( pSockDefender != null )
				pSockDefender.SysMessage( GetDictionaryEntry( 19229, pSockDefender.language ));// You resist paralysis.
			return true;
		}

		var seconds = 3000; // We want this applied to players even if they are "offline" (aka disconnected but not vanished from view yet)
		if( pDefender.npc )
		{
			seconds = 6000;
		}
		else if( pDefender.socket ) 
		{
			pSockDefender.TextMessage( GetDictionaryEntry( 19230, pSockDefender.language ), false, 0x3b2, 0, pDefender.serial );// The attack has temporarily paralyzed you!
		}

		pDefender.SetTempTag( "isImmune", true );
		pDefender.StartTimer( seconds + 8000, 9000, 7001 );
		pDefender.StartTimer( seconds, 8000, 7001 );
		pDefender.frozen = true;

		if( pAttacker.socket )
		{
			pSockAttacker.SysMessage( GetDictionaryEntry( 17702, pAttacker.socket.language), false, 0x3b2, 0, pAttacker.serial );// You deliver a paralyzing blow!
		}
		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
	else if( abilityID == 12 ) // shadowstrike
	{
		if( pAttacker.skills[47] < 800 ) // Stealth
		{
			pSockAttacker.SysMessage( GetDictionaryEntry( 19231, pSockAttacker.language ));// "You lack the required stealth to perform that attack
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );
			return true;
		}

		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 12 )
			TriggerEvent( 2205, "DeactivateSpecialMove", pAttacker, abilityID );

		//checking mana
		if( CheckMana( pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		pSockAttacker.SysMessage( GetDictionaryEntry( 19232, pSockAttacker.language ));// You strike and hide in the shadows!
		if( pSockDefender != null )
			pSockDefender.SysMessage( GetDictionaryEntry( 19233, pSockDefender.language ));// You are dazed by the attack and your attacker vanishes!

		pAttacker.StaticEffect( 0x376A, 0x09, 0x06 );

		pDefender.SoundEffect( 0x482, true );
		pDefender.StaticEffect( 0x37BE, 0x09, 0x06 );

		pAttacker.atWar = false;
		pDefender.atWar = false;
		pAttacker.visible = 1;

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );
	}
	else if( abilityID == 13 ) //  Whirlwind Attack
	{
		// Clear out any current ability the player is doing when he switches abilities
		if( abilityID != 13 )
			TriggerEvent(2205, "DeactivateSpecialMove", pAttacker, abilityID);

		//checking mana
		if( CheckMana(pAttacker, abilityID ))
		{
			DeductMana( pAttacker, abilityID );
		}

		TriggerEvent( 2205, "ClearSpecialMove", pAttacker, abilityID );// Clear the Ability after success
	}
}

function onTimer( timerObj, timerID )
{
	var socket = timerObj.socket;
	var abilityID = timerObj.GetTempTag( "abilityID" );
	var damage = 31;

	if( timerObj == null || timerObj.dead)
	{
		timerObj.frozen = false;
		timerObj.SetTempTag( "isImmune", null );
		timerObj.RemoveScriptTrigger( 7002 );
		timerObj.SetTempTag( "blockEquip", null );
		timerObj.SetTempTag( "blockHeal", null );
		timerObj.KillJSTimer( 9400, 7001 );
		timerObj.SetTempTag( "doBleed", null );
		TriggerEvent( 2205, "ClearSpecialMove", timerObj, abilityID );
		return;
	}
	else if( timerID == 8000 )
	{
		timerObj.frozen = false;
	}
	else if( timerID == 9000 ) 
	{
		timerObj.SetTempTag( "isImmune", null );
	}
	else if( timerID == 9100 ) 
	{
		timerObj.RemoveScriptTrigger( 7002 );
		timerObj.SetTempTag( "blockEquip", null );
		if( socket != null )
			socket.SysMessage( GetDictionaryEntry( 19234, socket.language ));// Your confusion has passed, you may now arm a weapon!
	}
	else if( timerID == 9200 )
	{
		timerObj.SetTempTag("blockHeal", null );
		socket.SysMessage( GetDictionaryEntry( 19235, socket.language ));// You are no longer mortally wounded.
	}

	if( timerID >= 9300 && timerID < 9310 )
	{
		var damage = RandomNumber( 1, 10 );
		//timerObj.StaticEffect( 0x122A, 0, 15); // blood effect got to figure how to add it to ground.
		timerObj.health -= damage;
		timerObj.StartTimer( 2000, 9300 + 1, 7001 );//restart timer every 2 seconds until it shuts off
	}
	else if( timerID == 9310 ) 
	{
		timerObj.SetTempTag( "doBleed", null );
		timerObj.KillJSTimer( 9400, 7001 );
		if( socket != null )
			socket.SysMessage( GetDictionaryEntry( 19236, socket.language ));// The bleeding wounds have healed, you are no longer bleeding!
	}
}