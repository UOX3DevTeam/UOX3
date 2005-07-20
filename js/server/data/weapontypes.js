// This is a generic script for determining weapon types based on item-ID
// You can call this script from an external script by using the following three lines code...
//		TriggerEvent( 2500, "getWeaponType", pUser );
//		var weaponType = pUser.GetTag( "weaponType" );
//		pUser.SetTag( "weaponType", null );

function getWeaponType( pUser )
{
	var tempItem = pUser.FindItemLayer( 1 );
	if( tempItem == null )
		tempItem = pUser.FindItemLayer( 2 );
	if( tempItem == null )
		pUser.SetTag( "weaponType", "WRESTLING" );
	else
	{
		switch( tempItem.id )
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
				pUser.SetTag( "weaponType", "DEF_SWORDS" ); break;
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
				pUser.SetTag( "weaponType", "SLASH_SWORDS" ); break;
		// One-Handed Lg. Swords
			case 0x0F5E: //broadsword
			case 0x0F5F: //broadsword
			case 0x257D: //longsword - LBR
			case 0x26CE: //paladin sword - AoS
			case 0x26CF: //paladin sword - AoS
			case 0x2554: //daemon sword - LBR
				pUser.SetTag( "weaponType", "ONEHND_LG_SWORDS" ); break;
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
				pUser.SetTag( "weaponType", "TWOHND_LG_SWORDS" ); break;
		// Bardiche
			case 0x0F4D: //bardiche
			case 0x0F4E: //bardiche
			case 0x26BA: //scythe - AoS
			case 0x26C4: //scythe - AoS
			case 0x255B: //ophidian bardiche - LBR
			case 0x2577: //naginata - LBR
				pUser.SetTag( "weaponType", "BARDICHE" ); break;
		// One-Handed Axes
			case 0x0EC2: //cleaver
			case 0x0EC3: //cleaver
			case 0x0E85: //pickaxe
			case 0x0E86: //pickaxe
			case 0x2567: //orc lord battleaxe - LBR
			case 0x2579: //pick - LBR
				pUser.SetTag( "weaponType", "ONEHND_AXES" ); break;
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
				pUser.SetTag( "weaponType", "TWOHND_AXES" ); break;
		// Default Maces
			case 0x13E3: //smith's hammer
			case 0x13E4: //smith's hammer
			case 0x13B3: //club
			case 0x13B4: //club
			case 0x0FB4: //sledge hammer
			case 0x0FB5: //sledge hammer
			case 0x0F5C: //mace
			case 0x0F5D: //mace
			case 0x1406: //war mace
			case 0x1407: //war mace
			case 0x143C: //hammer pick
			case 0x143D: //hammer pick
			case 0x13AF: //war axe
			case 0x13B0: //war axe
			case 0x143A: //maul
			case 0x143B: //maul
			case 0x26BC: //scepter - AoS
			case 0x26C6: //scepter - AoS
			case 0x2557: //lizardman's mace - LBR
			case 0x255C: //orc club - LBR
			case 0x256F: //smyth's hammer - LBR
			case 0x257F: //war mace - LBR
				pUser.SetTag( "weaponType", "DEF_MACES" ); break;
		// Large Maces
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
			case 0x1438: //war hammer
			case 0x1439: //war hammer
			case 0x2555: //ettin hammer - LBR
			case 0x2556: //liche's staff - LBR
			case 0x2559: //ogre's club - LBR
			case 0x255A: //ophidian staff - LBR
			case 0x2561: //terathan staff - LBR
			case 0x2565: //troll maul - LBR
			case 0x2566: //frost troll club - LBR
			case 0x2568: //orc mage staff - LBR
			case 0x2569: //bone mage staff - LBR
			case 0x256B: //magic staff - LBR
			case 0x256C: //magic staff - LBR
			case 0x256D: //magic staff - LBR
			case 0x256E: //magic staff - LBR
			case 0x27A3: //tessen - SE
			case 0x27EE: //tessen - SE
			case 0x27A6: //tetsubo - SE
			case 0x27F1: //tetsubo - SE
			case 0x27AE: //nunchako - SE
			case 0x27F9: //nunchako - SE
				pUser.SetTag( "weaponType", "LG_MACES" ); break;
		// Bows
			case 0x13B1: //bow
			case 0x13B2: //bow
			case 0x26C2: //composite bow - AoS
			case 0x26CC: //composite bow - AoS
			case 0x2571: //horseman's bow - LBR
				pUser.SetTag( "weaponType", "BOWS" ); break;
		// Crossbows
			case 0x0F4F: //crossbow
			case 0x0F50: //crossbow
			case 0x13FC: //heavy crossbow
			case 0x13FD: //heavy crossbow
			case 0x26C3: //repeating crossbow - AoS
			case 0x26CD: //repeating crossbow - AoS
			case 0x27A5: //yumi - SE
			case 0x27F0: //yumi - SE
		//case 0x27AA: //fukiya - SE - Blowgun, uses Dart ammo (0x2806 or 0x2804)
		//case 0x27F5: //fukiya - SE - Blowgun, uses Dart ammo (0x2806 or 0x2804)
				pUser.SetTag( "weaponType", "XBOWS" ); break;
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
				pUser.SetTag( "weaponType", "DEF_FENCING" ); break;
		// Stabbing Fencing Weapons
			case 0x0E87: //pitchfork
			case 0x0E88: //pitchfork
			case 0x0F62: //spear
			case 0x0F63: //spear
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
				pUser.SetTag( "weaponType", "TWOHND_FENCING" ); break;
			case 0x27AF: //sai - SE
			case 0x27FA: //sai - SE
				pUser.SetTag( "weaponType", "DUAL_FENCING_STAB" ); break;
			case 0x27AB: //tekagi - SE
			case 0x27F6: //tekagi - SE
			case 0x27AD: //kama - SE
			case 0x27F8: //kama - SE
				pUser.SetTag( "weaponType", "DUAL_FENCING_SLASH" ); break;
			case 0x27A9: //daisho - SE
			case 0x27F4: //daisho - SE
				pUser.SetTag( "weaponType", "DUAL_SWORD" ); break;
			default: // Wrestling
				pUser.SetTag( "weaponType", "WRESTLING" ); break;
		}
	}
}