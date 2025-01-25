// This is a generic script for determining weapon types based on item-ID,
// or combat skill based on equipped weapon type.
//
// To get weapon-type of the equipped weapon, use the following in an external script:
//		var weaponType = TriggerEvent( 2500, "GetWeaponType", mChar, null );
//
// To get weapon-type based on a specific ID, use the following in an external script:
// 		var weaponType = TriggerEvent( 2500, "GetWeaponType, null, [item ID]" );
//
// To get combat skill based on equipped weapon type, use the following in an external script:
//		var combatSkill = TriggerEvent( 2500, "GetCombatSkill", weaponType );
//
//	List of potential weaponTypes returned (as strings)
//		Unarmed:
//			"WRESTLING"
//		Swordsmanship-weapons:
//			"DEF_SWORDS"
//			"SLASH_SWORDS"
//			"ONEHND_LG_SWORDS"
//			"TWOHND_LG_SWORDS"
//			"BARDICHE"
//			"ONEHND_AXES"
//			"TWOHND_AXES"
//			"DUAL_SWORD"
//		Macefighting-weapons:
//			"DEF_MACES"
//			"LG_MACES"
//		Fencing-weapons:
//			"DEF_FENCING"
//			"TWOHND_FENCING"
//			"DUAL_FENCING_SLASH"
//			"DUAL_FENCING_STAB"
//		Archery-weapons:
//			"BOWS"
//			"XBOWS"
//		Thrown-weapons:
//			"THROWN"
//
//	List of combat skills returned by script:
//		"SWORDSMANSHIP"
//		"MACEFIGHTING"
//		"FENCING"
//		"ARCHERY"
//		"WRESTLING"
//		"THROWING"

function GetWeaponType( mChar, itemID )
{
	var weaponType;

	// If a valid character was passed in, try to find what weapon (if any) they have equipped
	if( ValidateObject( mChar ))
	{
		// Check first layer1 then layer2 for equipped weapons on character
		var tempItem = mChar.FindItemLayer( 1 );
		if( tempItem == null )
		{
			tempItem = mChar.FindItemLayer( 2 );
		}
		if( tempItem != null )
		{
			itemID = tempItem.id;
		}
		else
		{
			itemID = null;
		}
	}

	// If no equipped item, and no manually provided itemID, weaponType is WRESTLING
	if( itemID == null || itemID == 0 )
	{
		return "WRESTLING";
	}

	switch( itemID )
	{
		// Slashing Swords
		case 0x0EC4: //skinning knife
		case 0x0EC5: //skinning knife
		case 0x0F60: //longsword
		case 0x0F61: //longsword
		case 0x13B7: //long sword
		case 0x13B8: //long sword
		case 0x1440: //cutlass
		case 0x1441: //cutlass
		case 0x26BB: //bone harvester - AoS
		case 0x26C5: //bone harvester - AoS
		case 0x2573: //magic sword - LBR
		case 0x2574: //magic sword - LBR
		case 0x2575: //magic sword - LBR
		case 0x2576: //magic sword - LBR
		case 0x257E: //short sword - LBR
		case 0x27A4: //wakizashi - SE
		case 0x27EF: //wakizashi - SE
		case 0x0907: //gargish shortblade - SA
		case 0x4076: //gargish shortblade - SA
		case 0x48C6: //gargish bone harvester - SA
		case 0x48C7: //gargish bone harvester - SA
			weaponType = "DEF_SWORDS"; break;
	// Default Swords
		case 0x13F6: //butcher knife
		case 0x13F7: //butcher knife
		case 0x13FE: //katana
		case 0x13FF: //katana
		case 0x13B5: //scimitar
		case 0x13B6: //scimitar
		case 0x13B9: //viking sword
		case 0x13BA: //viking sword
		case 0x255E: //ratman sword - LBR
		case 0x2560: //skeleton scimitar - LBR
		case 0x0908: //gargish talwar - SA
		case 0x4075: //gargish talwar - SA
		case 0x090C: //gargish glass sword - SA
		case 0x4073: //gargish glass sword - SA
		case 0x0900: //gargish stone war sword - SA
		case 0x4071: //gargish stone war sword - SA
		case 0x48B6: //gargish butcherknife - SA
		case 0x48B7: //gargish butcherknife - SA
		case 0x48BA: //gargish katana - SA
		case 0x48BB: //gargish katana - SA
			weaponType = "SLASH_SWORDS"; break;
	// One-Handed Lg. Swords
		case 0x0F5E: //broadsword
		case 0x0F5F: //broadsword
		case 0x257D: //longsword - LBR
		case 0x26CE: //paladin sword - AoS
		case 0x26CF: //paladin sword - AoS
		case 0x2554: //daemon sword - LBR
        case 0x2D33: //radiant scimitar - ML
        case 0x2D27: //radiant scimitar - ML
		case 0x2D35: //elven machete - ML
		case 0x2D29: //elven machete - ML
		case 0x090B: //gargish dread sword - SA
		case 0x4074: //gargish dread sword - SA
		case 0xA33B: //antique sword 1 south
		case 0xA33C: //antique sword 1 east
		case 0xA33D: //antique sword 2 south
		case 0xA33E: //antique sword 2 east
		case 0xA33F: //antique sword 3 south
		case 0xA340: //antique sword 3 east
		case 0xA341: //skull sword south
		case 0xA342: //skull sword east
		case 0xA345: //gargoyle skull sword south
		case 0xA346: //gargoyle skull sword east
			weaponType = "ONEHND_LG_SWORDS"; break;
	// Two-Handed Lg. Swords
		case 0x143E: //halberd
		case 0x143F: //halberd
		case 0x26BD: //bladed staff - AoS
		case 0x26C7: //bladed staff - AoS
		case 0x26C1: //crescent blade - AoS
		case 0x26CB: //crescent blade - AoS
		case 0x2578: //no-dachi - LBR
		case 0x27A2: //no-dachi - SE
		case 0x27ED: //no-dachi - SE
		case 0x27A8: //bokuto - SE
		case 0x27F3: //bokuto - SE
		case 0x2D32: //rune blade - ML
		case 0x2D26: //rune blade - ML
			weaponType = "TWOHND_LG_SWORDS"; break;
	// Bardiche
		case 0x0F4D: //bardiche
		case 0x0F4E: //bardiche
		case 0x26BA: //scythe - AoS
		case 0x26C4: //scythe - AoS
		case 0x255B: //ophidian bardiche - LBR
		case 0x2577: //naginata - LBR
		case 0x48B4: //gargish bardiche - SA
		case 0x48B5: //gargish bardiche - SA
		case 0x48C4: //gargish scythe - SA
		case 0x48C5: //gargish scythe - SA
			weaponType = "BARDICHE"; break;
	// One-Handed Axes
		case 0x0EC2: //cleaver
		case 0x0EC3: //cleaver
		case 0x0E85: //pickaxe
		case 0x0E86: //pickaxe
		case 0x2567: //orc lord battleaxe - LBR
		case 0x2579: //pick - LBR
		case 0x48AE: //gargish cleaver
		case 0x48AF: //gargish cleaver
			weaponType = "ONEHND_AXES"; break;
	// Two-Handed Axes
		case 0x13FA: //large battle axe
		case 0x13FB: //large battle axe
		case 0x1442: //two handed axe
		case 0x1443: //two handed axe
		case 0x0F43: //hatchet
		case 0x0F44: //hatchet
		case 0x0F45: //executioner's axe
		case 0x0F46: //executioner's axe
		case 0x0F47: //battle axe
		case 0x0F48: //battle axe
		case 0x0F49: //axe
		case 0x0F4A: //axe
		case 0x0F4B: //double axe
		case 0x0F4C: //double axe
		case 0x255F: //skeleton axe - LBR
		case 0x255D: //ratman axe - LBR
		case 0x2564: //troll axe - LBR
		case 0x2570: //hatchet - LBR
		case 0x2D28: //ornate axe - ML
		case 0x2D34: //ornate axe - ML
		case 0x48B0: //gargish battleaxe - SA
		case 0x48B1: //gargish battleaxe - SA
		case 0x48B2: //gargish axe - SA
		case 0x48B3: //gargish axe - SA
			weaponType = "TWOHND_AXES"; break;
		// Default Maces
		case 0x0DF2: // Wand
		case 0x0DF3: // Wand
		case 0x0DF4: // Wand
		case 0x0DF5: // Wand
		case 0x0FB4: //sledge hammer
		case 0x0FB5: //sledge hammer
		case 0x0F5C: //mace
		case 0x0F5D: //mace
		case 0x13E3: //smith's hammer
		case 0x13E4: //smith's hammer
		case 0x13B3: //club
		case 0x13B4: //club
		case 0x13B0: //war axe
		case 0x13AF: //war axe
		case 0x1406: //war mace
		case 0x1407: //war mace
		case 0x143C: //hammer pick
		case 0x143D: //hammer pick
		case 0x143A: //maul
		case 0x143B: //maul
		case 0x26BC: //scepter - AoS
		case 0x26C6: //scepter - AoS
		case 0x2557: //lizardman's mace - LBR
		case 0x255C: //orc club - LBR
		case 0x256F: //smyth's hammer - LBR
		case 0x257F: //war mace - LBR
		case 0x2D24: //diamond mace - ML
		case 0x2D30: //diamond mace - ML
		case 0x0903: //gargish disc mace - SA
		case 0x406E: //gargish disc mace - SA
		case 0x48C2: //gargish maul - SA
		case 0x48C3: //gargish maul - SA
			weaponType = "DEF_MACES"; break;
		// Large Maces
		case 0x1438: //war hammer
		case 0x1439: //war hammer
		case 0x2555: //ettin hammer - LBR
		case 0x2559: //ogre's club - LBR
		case 0x2565: //troll maul - LBR
		case 0x2566: //frost troll club - LBR
		case 0x27A3: //tessen - SE
		case 0x27EE: //tessen - SE
		case 0x27A6: //tetsubo - SE
		case 0x27F1: //tetsubo - SE
		case 0x27AE: //nunchako - SE
		case 0x27F9: //nunchako - SE
		case 0x48C0: //gargish war hammer - SA
		case 0x48C1: //gargish war hammer - SA
		case 0x48CC: //gargish tessen - SA
		case 0x48CD: //gargish tessen - SA
			weaponType = "LG_MACES"; break;
		// Staffs - treated differently in some cases than other maces
		case 0x13F4: //crook
		case 0x13F5: //crook
		case 0x13F8: //gnarled staff
		case 0x13F9: //gnarled staff
		case 0x0DF0: //black staff
		case 0x0DF1: //black staff
		case 0x0E81: //shepherd's crook
		case 0x0E82: //shepherd's crook
		case 0x0E89: //quarter staff
		case 0x0E8A: //quarter staff
		case 0x2556: //liche's staff - LBR
		case 0x255A: //ophidian staff - LBR
		case 0x2561: //terathan staff - LBR
		case 0x2568: //orc mage staff - LBR
		case 0x2569: //bone mage staff - LBR
		case 0x256B: //magic staff - LBR
		case 0x256C: //magic staff - LBR
		case 0x256D: //magic staff - LBR
		case 0x256E: //magic staff - LBR
		case 0x2D25: //wild staff - ML
		case 0x2D31: //wild staff - ML
		case 0x0905: //gargish glass staff - SA
		case 0x4070: //gargish glass staff - SA
		case 0x0906: //gargish serpentstone staff - SA
		case 0x406F: //gargish serpentstone staff - SA
		case 0xA343: //skull staff south
		case 0xA344: //skull staff east
		case 0xA347: //gargoyle skull staff south
		case 0xA348: //gargoyle skull staff east
			weaponType = "TWOHND_STAFFS"; break;
	// Bows
		case 0x13B1: //bow
		case 0x13B2: //bow
		case 0x26C2: //composite bow - AoS
		case 0x26CC: //composite bow - AoS
		case 0x2571: //horseman's bow - LBR
		case 0x27A5: //yumi - SE
		case 0x27F0: //yumi - SE
		case 0x2D1F: //magical shortbow - ML
		case 0x2D2B: //magical shortbow - ML
			weaponType = "BOWS"; break;
	// Crossbows
		case 0x0F4F: //crossbow
		case 0x0F50: //crossbow
		case 0x13FC: //heavy crossbow
		case 0x13FD: //heavy crossbow
		case 0x26C3: //repeating crossbow - AoS
		case 0x26CD: //repeating crossbow - AoS
			weaponType = "XBOWS"; break;
		case 0x27AA: //fukiya - SE - Blowgun, uses Dart ammo (0x2806 or 0x2804)
		case 0x27F5: //fukiya - SE - Blowgun, uses Dart ammo (0x2806 or 0x2804)
			weaponType = "BLOWGUNS"; break;
	// Normal Fencing Weapons
		case 0x0F51: //dagger
		case 0x0F52: //dagger
		case 0x1400: //kryss
		case 0x1401: //kryss
		case 0x1402: //short spear
		case 0x1403: //short spear
		case 0x1404: //war fork
		case 0x1405: //war fork
		case 0x257C: //sword(rapier) - LBR
        case 0x2D21: //assassin spike - ML
		case 0x2D2D: //assassin spike - ML
		case 0x2D20: //elven spellblade - ML
		case 0x2D2C: //elven spellblade - ML
		case 0x2D22: //leafblade - ML
		case 0x2D2E: //leafblade - ML
		case 0x2D2F: //war cleaver - ML
		case 0x2D23: //war cleaver - ML
		case 0x2306: //gargish dagger - SA
		case 0x406A: //gargish dagger
		case 0x08FE: //gargish bloodblade - SA
		case 0x4072: //gargish bloodblade - SA
		case 0x48BC: //gargish kryss - SA
		case 0x48BD: //gargish kryss - SA
			weaponType = "DEF_FENCING"; break;
	// Stabbing Fencing Weapons
		case 0x0E87: //pitchfork
		case 0x0E88: //pitchfork
		case 0x0F62: //spear
		case 0x0F63: //spear
		case 0x2236: //lance
		case 0x2237: //lance
		case 0x2238: //lance
		case 0x2239: //lance
		case 0x26BE: //pike - AoS
		case 0x26C8: //pike - AoS
		case 0x26BF: //double bladed staff - AoS
		case 0x26C9: //double bladed staff - AoS
		case 0x26C0: //lance - AoS
		case 0x26CA: //lance - AoS
		case 0x2558: //lizardman's spear - LBR
		case 0x2562: //terathan spear - LBR
		case 0x2572: //horseman's javelin - LBR
		case 0x257A: //spear - LBR
		case 0x257B: //spear - LBR
		case 0x27A7: //lajatang - SE
		case 0x27F2: //lajatang - SE
		case 0x0904: //gargish dual pointed spear - SA
		case 0x406D: //gargish dual pointed spear - SA
		case 0x48C8: //gargish pike - SA
		case 0x48C9: //gargish pike - SA
		case 0x48CA: //gargish lance - SA
		case 0x48CB: //gargish lance - SA
			weaponType = "TWOHND_FENCING"; break;
		case 0x27AF: //sai - SE
		case 0x27FA: //sai - SE
			weaponType = "DUAL_FENCING_STAB"; break;
		case 0x27AB: //tekagi - SE
		case 0x27F6: //tekagi - SE
		case 0x27AD: //kama - SE
		case 0x27F8: //kama - SE
		case 0x48CE: //gargish tekagi - SA
		case 0x48CF: //gargish tekagi - SA
			weaponType = "DUAL_FENCING_SLASH"; break;
		case 0x27A9: //daisho - SE
		case 0x27F4: //daisho - SE
		case 0x08FD: //gargish dual short axes - SA
		case 0x4068: //gargish dual short axes - SA
		case 0x48D0: //gargish daisho - SA
		case 0x48D1: //gargish daisho - SA
			weaponType = "DUAL_SWORD"; break;
		case 0x08FF: //gargish boomerang - SA
		case 0x4067: //gargish boomerang - SA
		case 0x0901: //gargish cyclone - SA
		case 0x406C: //gargish cyclone - SA
		case 0x090A: //gargish soul glaive - SA
		case 0x406B: //gargish soul glaive - SA
			weapontype = "THROWN"; break;
		default: // Wrestling
			weaponType = "WRESTLING"; break;
	}

	return weaponType;
}

function GetCombatSkill( weaponType )
{
	var combatSkill;

	// Determine combatskill based on weaponType:
	switch( weaponType )
	{
		case "DEF_SWORDS":
		case "SLASH_SWORDS":
		case "ONEHND_LG_SWORDS":
		case "TWOHND_LG_SWORDS":
		case "ONEHND_AXES":
		case "TWOHND_AXES":
		case "BARDICHE":
		case "DUAL_SWORD":
			combatSkill = "SWORDSMANSHIP"; break;
		case "DEF_MACES":
		case "LG_MACES":
			combatSkill = "MACEFIGHTING"; break;
		case "DEF_FENCING":
		case "TWOHND_FENCING":
		case "DUAL_FENCING_SLASH":
		case "DUAL_FENCING_STAB":
			combatSkill = "FENCING"; break;
		case "BOWS":
		case "XBOWS":
			combatSkill = "ARCHERY"; break;
		case "THROWN":
			combatSkill = "THROWING"; break;
		case "WRESTLING":
		default:
			combatSkill = "WRESTLING"; break;
	}

	return weaponType;
}