// Magical Items, Pre-AoS style
// v1.0
// by Xuri
// Last updated August 25thrd, 2021

// // Define the scriptID you've assigned to this script here:
const magicItemScriptID = 3300;
const accBonusScriptID = 3301;
const armorSpellsScriptID = 3302;
const magicWeaponSpellScriptID = 3304;

// Enable to dump generated magic item names to .txt files within the UOX3/shared folder
const outputMagicItemNamesToFile = false;

// How should we apply the damage-bonuses from Ruin/Might/Force/Power/Vanquishing to weapons?
// 0 = (Default) apply all bonus damage to weapon's max potential damage output
// 1 = split the bonus damage between min (~50%) and max (~50%) potential damage output
const dmgBonusType = 0;

// Let's define the various tiers of NPCs based on their amount of Fame
// Note: Tier 5 is not defined, but any NPCs with fame above the defined value for tier 4 belongs to tier 5
const tier1NPCFame = 3000;
const tier2NPCFame = 6000;
const tier3NPCFame = 8000;
const tier4NPCFame = 10000;

// Define the droprate for each tier of NPCs
// Works roughly like this: 1 in every X NPCs will get magic item loot
var tier1DropRate = 45;
var tier2DropRate = 37;
var tier3DropRate = 28;
var tier4DropRate = 16;
var tier5DropRate = 2;

// Adjust global drop rate across all tiers at once
// 0.5 will halve the droprate for each tier
// 1 is the default
// 2 will double the droprate for each tier
const globalDropRateMod = 1;

// Here we sort all items into tiers roughly based on their power/popularity
// Magical items from all tiers can drop from any monster, but the lower
// the fame of the monster, the more likely it will be to get a magical item
// from the lowest tiers, with the weakest magical properties. And vice versa.

// Tier 1: Smith Hammers, Crooks, Sledge Hammers, Pickaxes, Skinning Knives, Cleavers, Butcher Knives
const tier1weapons = new Array( "0x13E3", "0x13E4", "0x0E81", "0x0E82", "0x13F4", "0x13F5", "0x0FB4", "0x0FB5", "0x0E85", "0x0E86", "0x0EC4", "0x0EC5", "0x0EC2", "0x0EC3", "0x13F6", "0x13F7" );
// Tier 2: Hatchets, Hammer Picks, Pitchforks, Clubs, Daggers, Longswords
const tier2weapons = new Array( "0x0F43", "0x0F44", "0x143C", "0x143D", "0x0E87", "0x0E88", "0x13B3", "0x13B4", "0x0F51", "0x0F52", "0x0F60", "0x0F61", "0x13B7", "0x13B8" );
// Tier 3: Maces, Heavy Crossbows, Mauls, Crossbows, Cutlasses, Viking Swords, Bows, War Maces
const tier3weapons = new Array( "0x0F5C", "0x0F5D", "0x13FC", "0x13FD", "0x143A", "0x143B", "0x0F4F", "0x0F50", "0x1440", "0x1441", "0x13B9", "0x13BA", "0x13B1", "0x13B2", "0x1406", "0x1407" );
// Tier 4: War Axes, Axes, Gnarled Staffs, Executioner's Axes, Scimitars, Battle Axes, Large Battle Axes, Broad Swords, Bardiches, Two-Handed Axes, Black Staves, War Forks, War Hammers
const tier4weapons = new Array( "0x13AF", "0x13B0", "0x0F49", "0x0F4A", "0x13F8", "0x13F9", "0x0F45", "0x0F46", "0x13B5", "0x13B6", "0x0F47", "0x0F48", "0x13FA", "0x13FB", "0x0F5E", "0x0F5F", "0x0F4D", "0x0F4E", "0x1442", "0x1443", "0x0DF0", "0x0DF1", "0x1404", "0x1405", "0x1438", "0x1439" );
// Tier 5: Krysses, Quarterstaves, Spears, Double Axes, Short Spears, Halberds, Katanas
const tier5weapons = new Array( "0x1400", "0x1401", "0x0E89", "0x0E8A", "0x0F62", "0x0F63", "0x0F4B", "0x0F4C", "0x1402", "0x1403", "0x143E", "0x143F", "0x13FE", "0x13FF" );

//Leather/Studded Leather + buckler
const tier1armors = new Array( "0x13c5", "0x13c6", "0x13c7", "0x13cb", "0x13cc", "0x13cd", "0x13ce", "0x13d2", "0x13d3", "0x13d4",
							"0x13d5", "0x13d6", "0x13da", "0x13db", "0x13dc", "0x13dd", "0x13e1", "0x13e2", "0x1c00", "0x1c06",
							"0x1c08", "0x1c0a", "0x1c02", "0x1c0c", "0x1b73" );
//Ringmail + wooden shield, metal shield
const tier2armors = new Array( "0x13eb", "0x13ec", "0x13ed", "0x13ee", "0x13ef", "0x13f0", "0x13f1", "0x13f2", "0x1b7a", "0x1b7b" );
//Chainmail + bronze shield, wooden kite shield
const tier3armors = new Array( "0x13bb", "0x13be", "0x13bf", "0x13c0", "0x13c3", "0x13c4", "0x1b72", "0x1b78", "0x1b79" );
//Bone + metal kite shield
const tier4armors = new Array( "0x144e", "0x144f", "0x1450", "0x1451", "0x1452", "0x1453", "0x1454", "0x1455", "0x1456", "0x1457", "0x1b74", "0x1b75" );
//Plate + heater shield
const tier5armors = new Array( "0x1410", "0x1411", "0x1412", "0x1413", "0x1414", "0x1415", "0x1416", "0x1417", "0x1418", "0x1419", "0x141a", "0x1c04", "0x1b76", "0x1b77" );

// Magic Wands and gnarled staffs
const magicWands = new Array( "0x0df2", "0x0df3", "0x0df4", "0x0df5", "0x13f8", "0x13f9" );

// (Potentially) create magic item on corpse upon death
function onDeath( cNpc, iCorpse )
{
	// Double check that both NPC and corpse exist - they should,
	// as this script runs before the dead NPC is cleaned up
	if( !ValidateObject( cNpc ) || !ValidateObject( iCorpse ))
		return false;

	// Only trigger script if NPC has enough fame to actually own a magical item
	var npcFame = cNpc.fame;
	if( npcFame >= 1000 )
	{
		// Determine max amount of magic items that can potentially spawn for a given NPC
		var maxItemAmount = 0;
		if( npcFame < 3000 )
		{
			maxItemAmount = 1;
		}
		else if( npcFame < 6000 )
		{
			maxItemAmount = 2;
		}
		else if( npcFame < 8000 )
		{
			maxItemAmount = 3;
		}
		else if( npcFame < 10000 )
		{
			maxItemAmount = 5;
		}
		else if( npcFame < 15000 )
		{
			maxItemAmount = 6;
		}
		else
		{
			maxItemAmount = 7;
		}

		for( var i = 0; i < maxItemAmount; i++ )
		{
			// Run the chance calculator to determine chance of item being spawned
			var spawnMagicItem = MagicLootChanceCalculator( npcFame, false );

			if( spawnMagicItem )
			{
				// Determine what type of magic loot to create
				itemType = WeightedRandom( 1, 4 );
				switch( itemType )
				{
					case 1: // Weapon
					case 2: // Armor
						itemType = RandomNumber( 0, 1 );
						break;
					case 3: // Wand
						itemType = 2;
						break;
					case 4: // Ring
						itemType = 3;
						break;
				}

				// Actually create the magic loot!
				var magicItem = GenerateMagicItem( itemType, null );
				if( ValidateObject( magicItem ))
				{
					magicItem.container = iCorpse;
				}
			}
		}
	}

	return false;
}

// Function that runs upon creation of dummy items with this script attached,
// with the intention of adding random magic weapons directly, either via GM command
// or as guaranteed loot in treasure chests, on boss NPCs etc
function onCreateDFN( objMade, objType )
{
	if( objType == 0 && ValidateObject( objMade ))
	{
		var fameLevel = objMade.morex;
		if( fameLevel == 0 )
		{
			fameLevel = RandomNumber( 1, 10000 );
		}

		MagicLootChanceCalculator( fameLevel, true );

		// Determine what type of magic loot to create. If morex of dummy object is 5,
		// randomize the type of item to create. Otherwise, use the morex value
		var itemType = objMade.morey;
		if( itemType == 5 )
		{
			itemType = WeightedRandom( 1, 4 );
			switch( itemType )
			{
				case 1: // Weapon
				case 2: // Armor
					itemType = RandomNumber( 0, 1 );
					break;
				case 3: // Wand
					itemType = 2;
					break;
				case 4: // Ring
					itemType = 3;
					break;
			}
		}

		// Actually create the magic loot, and place it in the target Container
		var magicItem = GenerateMagicItem( itemType, objMade );

		// Delete the newly created magicItem, since we only used it as a temporary item and copied
		// all its properties to objMade already
		if( ValidateObject( magicItem ))
		{
			magicItem.Delete();
		}
	}
}

// Run chance calculator to determine if magical loot should be generated, and if so, what tier
var maxTierLevel = 0;
function MagicLootChanceCalculator( fameLevel, guaranteedLoot )
{
	var dropRateNum = 0;
	if( fameLevel <= tier1NPCFame )
	{
		maxTierLevel = 1;
		dropRateNum = Math.round( tier1DropRate / globalDropRateMod );
	}
	else if( fameLevel > tier1NPCFame && fameLevel <= tier2NPCFame )
	{
		maxTierLevel = 2;
		dropRateNum = Math.round( tier2DropRate / globalDropRateMod );
	}
	else if( fameLevel > tier2NPCFame && fameLevel <= tier3NPCFame )
	{
		maxTierLevel = 3;
		dropRateNum = Math.round( tier3DropRate / globalDropRateMod );
	}
	else if( fameLevel > tier3NPCFame && fameLevel <= tier4NPCFame )
	{
		maxTierLevel = 4;
		dropRateNum = Math.round( tier4DropRate / globalDropRateMod );
	}
	else if( fameLevel > tier4NPCFame )
	{
		maxTierLevel = 5;
		dropRateNum = Math.round( tier5DropRate / globalDropRateMod );
	}

	// Time to do a check to see if we should spawn a magic item or not
	var iNum = RandomNumber( 0, dropRateNum );
	if( iNum == dropRateNum )
	{
		return true; // success!
	}
	else
	{
		return false; // no such luck
	}
}

// Generate a magic item of specified itemType (weapon, armor) based on a given fame level. targCont is container to put item in
function GenerateMagicItem( itemType, placeHolderItem )
{
	// Let's figure out which tier of magic weapons to potentially spawn from
	var tierNum = RandomNumber( 1, maxTierLevel );

	// Let's put in a small chance of getting an item from a higher tier
	tierNum = CalcTierBonus( tierNum );

	// Time to create the magic item itself
	var magicItem;
	switch( itemType )
	{
		case 0: // magic weapon
			magicItem = CreateMagicWeapon( tierNum, placeHolderItem );
			break;
		case 1: // magic armor
			magicItem = CreateMagicArmor( tierNum, placeHolderItem );
			break;
		case 2: // magic wand/staff
			magicItem = CreateMagicWand( tierNum, placeHolderItem );
			break;
		case 3: // magic ring
			magicItem = CreateMagicRing( tierNum, placeHolderItem );
			break;
		/*case 4: // magic clothes ??
			break;
		default:
			break;*/
	}

	// Return the magic item we created!
	return magicItem;
}

function CalcTierBonus( tierVal )
{
	if( tierVal < 5 )
	{
		var bonusChance = RandomNumber( 0, 1000 );
		var tierValBonus = 0;

		switch( tierVal )
		{
			case 1:
				if( bonusChance >= 999 ) // 0.1% chance
				{
					tierValBonus = 4;
				}
				else if( bonusChance >= 998 ) // 0.2% chance
				{
					tierValBonus = 3;
				}
				else if( bonusChance >= 996 ) // 0.4% chance
				{
					tierValBonus = 2;
				}
				else if( bonusChance >= 992 ) // 0.8% chance
				{
					tierValBonus = 1;
				}
				break;
			case 2:
				if( bonusChance >= 998 ) // 0.2% chance
				{
					tierValBonus = 3;
				}
				else if( bonusChance >= 996 ) // 0.4% chance
				{
					tierValBonus = 2;
				}
				else if( bonusChance >= 992 ) // 0.8% chance
				{
					tierValBonus = 1;
				}
				break;
			case 3:
				if( bonusChance >= 996 ) // 0.4% chance
				{
					tierValBonus = 2;
				}
				else if( bonusChance >= 992 ) // 0.8% chance
				{
					tierValBonus = 1;
				}
				break;
			case 4:
				if( bonusChance >= 992 ) // 0.8% chance
				{
					tierValBonus = 1;
				}
				break;
		}

		tierVal += tierValBonus;
	}

	return tierVal;
}

// ----------------------
// CREATE MAGICAL WEAPON
// ----------------------

function CreateMagicWeapon( tierNum, placeHolderItem )
{
	// Create random Weapon from chosen tier
	var rndWeapon;
	var rndWeaponNum;
	switch( tierNum )
	{
		case 1:
			rndWeaponNum = RandomNumber( 0, tier1weapons.length - 1 );
			rndWeapon = CreateDFNItem( null, null, tier1weapons[rndWeaponNum], 1, "ITEM", false );
			break;
		case 2:
			rndWeaponNum = RandomNumber( 0, tier2weapons.length - 1 );
			rndWeapon = CreateDFNItem( null, null, tier2weapons[rndWeaponNum], 1, "ITEM", false );
			break;
		case 3:
			rndWeaponNum = RandomNumber( 0, tier3weapons.length - 1 );
			rndWeapon = CreateDFNItem( null, null, tier3weapons[rndWeaponNum], 1, "ITEM", false );
			break;
		case 4:
			rndWeaponNum = RandomNumber( 0, tier4weapons.length - 1 );
			rndWeapon = CreateDFNItem( null, null, tier4weapons[rndWeaponNum], 1, "ITEM", false );
			break;
		case 5:
			rndWeaponNum = RandomNumber( 0, tier5weapons.length - 1 );
			rndWeapon = CreateDFNItem( null, null, tier5weapons[rndWeaponNum], 1, "ITEM", false );
			break;
		default:
			Console.Error( "Unable to spawn magic item - switch case in SpawnMagicWeapon out of bounds!" );
			break;
	}

	if( !ValidateObject( rndWeapon ))
	{
		Console.Error( "Unable to spawn DFN item from tier-array " + tierNum + " with array-position (rndWeaponNum) " + rndWeaponNum );
		return null;
	}

	// Handle weapon bonuses
	// Time to add some bonuses: HP bonus, an ACCURACY bonus, a DAMAGE bonus - or combinations of two/all three
	var rndBonus = RandomNumber( 1, 100 );

	// Increase rndBonus by 2.5 to 12.5 depending on maxTierLevel of NPC, so the higher the
	// maxTierLevel, the better chance of getting a non-rubbish magical item!
	rndBonus = rndBonus + ( Math.round( maxTierLevel * 2.5 ));
	rndBonus = Math.round( rndBonus );

	var addHpBonus = false;
	var addAccBonus = false;
	var addDmgBonus = false;
	var addSilverBonus = false;

	if( rndBonus >= 0 && rndBonus < 70 )
	{
		// 1 Magical Effect
		rndBonus = RandomNumber( 0, 3 );
		if( rndBonus == 0 )
		{
			addHpBonus = true;
		}
		else if( rndBonus == 1 )
		{
			addAccBonus = true;
		}
		else if( rndBonus == 2 )
		{
			addDmgBonus = true;
		}
		else if( rndBonus == 3 )
		{
			addSilverBonus = true;
		}
	}
	else if( rndBonus >= 70 && rndBonus < 99 )
	{
		// 2 Magical Effects
		rndBonus = RandomNumber( 0, 5 );
		if( rndBonus == 0 )
		{
			addHpBonus = true;
			addAccBonus = true;
		}
		else if( rndBonus == 1 )
		{
			addHpBonus = true;
			addDmgBonus = true;
		}
		else if( rndBonus == 2 )
		{
			addAccBonus = true;
			addDmgBonus = true;
		}
		else if( rndBonus == 3 )
		{
			addHpBonus = true;
			addSilverBonus = true;
		}
		else if( rndBonus == 4 )
		{
			addAccBonus = true;
			addSilverBonus = true;
		}
		else if( rndBonus == 5 )
		{
			addDmgBonus = true;
			addSilverBonus = true;
		}
	}
	else if( rndBonus >= 99 && rndBonus < 110 )
	{
		// 3 Magical Effects
		rndBonus = RandomNumber( 0, 3 );
		if( rndBonus == 0 )
		{
			addHpBonus = true;
			addAccBonus = true;
			addDmgBonus = true;
		}
		else if( rndBonus == 1 )
		{
			addHpBonus = true;
			addAccBonus = true;
			addSilverBonus = true;
		}
		else if( rndBonus == 2 )
		{
			addHpBonus = true;
			addDmgBonus = true;
			addSilverBonus = true;
		}
		else if( rndBonus == 3 )
		{
			addAccBonus = true;
			addDmgBonus = true;
			addSilverBonus = true;
		}
	}
	else if( rndBonus >= 110 )
	{
		// 4 Magical Effects
		addHpBonus = true;
		addAccBonus = true;
		addDmgBonus = true;
		addSilverBonus = true;
	}

	// Grab a copy of the original name of the item, so we have it for later
	var tempName = rndWeapon.name;
	rndWeapon.name2 = "";

	// Are we adding HP bonuses?
	if( addHpBonus )
	{
		CalcHpBonus( rndWeapon, 0 );
	}

	// Are we adding ACC bonuses?
	if( addAccBonus )
	{
		CalcAccuracyBonus( rndWeapon, addHpBonus );
	}
	else if( addHpBonus )
	{
		rndWeapon.name2 += " ";
	}

	// Add Silver bonus?
	if( addSilverBonus )
	{
		rndWeapon.name2 += "silver ";
		rndWeapon.race = 11;
	}

	// Add item-name after HP and ACC bonuses, but before a potential DMG bonus
	rndWeapon.name2 += tempName;

	// Are we adding Damage bonuses?
	if( addDmgBonus )
	{
		rndWeapon.name2 += " of ";
		CalcDamageBonus( rndWeapon );
	}

	// Are we adding Spell bonuses?
	iNum = RandomNumber( 1, 100 );
	// if( iNum <= 5 ) // 5% chance of spell bonus for a given magical weapon
	if( iNum > 0 ) // 5% chance of spell bonus for a given magical weapon
	{
		if( addDmgBonus )
		{
			rndWeapon.name2 += " and ";
		}
		else
		{
			rndWeapon.name2 += " of ";
		}

		CalcWeaponSpellBonus( rndWeapon );
	}

	rndWeapon.name = "a magic " + tempName;

	// Is final name too long? Not much we can do about it, MAX_NAME in UOX3 code is set to 128... Log it anyway, though.
	if( rndWeapon.name2.length >= 128 )
	{
		Console.Log( "Magic item generated with name consisting of 128 or more letters! Serial: " + rndWeapon.serial + ", length: " + rndWeapon.name2.length );
	}

	if( outputMagicItemNamesToFile )
	{
		AddToDatabase( 0, rndWeapon.name2 );
	}

	if( ValidateObject( rndWeapon ) && ValidateObject( placeHolderItem ))
	{
		// Loop through all the properties for our new magic item and copy them over to the placeholder item
		for( var itemProp in rndWeapon )
		{
			if( itemProp != null && itemProp != "serial" && itemProp != "scripttrigger" && itemProp != "scriptTriggers" )
			{
				if( typeof( rndWeapon[itemProp] ) != "undefined" && rndWeapon[itemProp] != null )
				{
					if( itemProp == "race" && rndWeapon.race != null )
					{
						placeHolderItem.race = rndWeapon.race.id;
					}
					else
					{
						placeHolderItem[itemProp] = rndWeapon[itemProp];
					}
				}
			}
		}

		// Copy scripttriggers too!
		var scriptTriggerList = rndWeapon.scriptTriggers;
		for( var i = 0; i < scriptTriggerList.length; i++ )
		{
			if( scriptTriggerList[i] != 0 && scriptTriggerList[i] != magicItemScriptID )
			{
				placeHolderItem.AddScriptTrigger( scriptTriggerList[i] );
			}
		}

		// Set accBonus tag too!
		if( rndWeapon.GetTag( "accBonus" ))
		{
			placeHolderItem.SetTag( "accBonus", rndWeapon.GetTag( "accBonus" ));
		}
	}
	return rndWeapon;
}

function CalcHpBonus( rndItem, itemType )
{
	// Define some variables
	var hpBonus = 0;
	var hpBonusName = "";

	// Let's put in a small chance of getting a better bonus
	maxTierLevel = CalcTierBonus( maxTierLevel );

	// Which bonus-tier are we adding, exactly?
	var bonusTier = RandomNumber( 1, maxTierLevel );

	switch( bonusTier )
	{
		case 1:
			hpBonus = 10;
			hpBonusName = "durable";
			break;
		case 2:
			hpBonus = 20;
			hpBonusName = "substantial";
			break;
		case 3:
			hpBonus = 30;
			hpBonusName = "massive";
			break;
		case 4:
			hpBonus = 40;
			hpBonusName = "fortified";
			break;
		case 5:
			hpBonus = 50;
			hpBonusName = "indestructable";
			break;
	}

	// if itemType is armor, halve the hp bonus
	if( itemType == 1 )
	{
		hpBonus /= 2;
	}

	// Let's add that bonus and adjust the weapon's magical name accordingly
	rndItem.maxhp += hpBonus;
	rndItem.health = rndItem.maxhp;
	rndItem.name2 += hpBonusName;
}

function CalcAccuracyBonus( rndWeapon, addHpBonus )
{	// Let's add some Weapon Bonuses! (Bonii?)

	// Define some variables
	var accBonus = 0;
	var accBonusArchery = 0;
	var accBonusName = "";

	// Let's put in a small chance of getting a better bonus
	maxTierLevel = CalcTierBonus( maxTierLevel );

	// Which bonus-tier are we adding, exactly?
	var bonusTier = RandomNumber( 1, maxTierLevel );

	//Note that for the bonuses, 50 equals 5.0 skillpoints, 200 equals 20.0 skillpoints, etc.
	switch( bonusTier )
	{
		case 1:
			accBonus = 50;
			accBonusArchery = 46;
			accBonusName = "accurate";
			break;
		case 2:
			accBonus = 100;
			accBonusArchery = 93;
			accBonusName = "surpassingly accurate";
			break;
		case 3:
			accBonus = 150;
			accBonusArchery = 140;
			accBonusName = "eminently accurate";
			break;
		case 4:
			accBonus = 200;
			accBonusArchery = 187;
			accBonusName = "exceedingly accurate";
			break;
		case 5:
		case 6:
		case 7:
		default:
			accBonus = 250;
			accBonusArchery = 233;
			accBonusName = "supremely accurate";
			break;
	}

	// Let's add a bonus that will temporarily add to player skills when item is equipped
	// If ranged weapon, add to archery skill instead of tactics skill
	if( rndWeapon.id == 0x13B1 || rndWeapon.id == 0x13B2 || rndWeapon.id == 0x26C2 || rndWeapon.id == 0x26CC ||
		rndWeapon.id == 0x2571 || rndWeapon.id == 0x0F4F || rndWeapon.id == 0x0F50 || rndWeapon.id == 0x13FC ||
		rndWeapon.id == 0x13FD || rndWeapon.id == 0x26C3 || rndWeapon.id == 0x26CD || rndWeapon.id == 0x27A5 ||
		rndWeapon.id == 0x27F0 )
	{
		rndWeapon.SetTag( "accBonus", accBonusArchery );
	}
	else
	{
		rndWeapon.SetTag( "accBonus", accBonus );
	}

	// Add accuracy bonus JS-script to weapon
	rndWeapon.AddScriptTrigger( accBonusScriptID );

	// Let's add bonus-name to item
	if( addHpBonus == true )
	{
		rndWeapon.name2 += ", " + accBonusName + " ";
	}
	else
	{
		rndWeapon.name2 += accBonusName + " ";
	}
}

function CalcDamageBonus( rndWeapon )
{	// Let's add some Weapon Bonuses! (Bonii?)

	// Define some variables
	var dmgBonus = 0;
	var dmgBonusName = "";

	// Let's put in a small chance of getting a better bonus
	maxTierLevel = CalcTierBonus( maxTierLevel );

	// Which bonus-tier are we adding, exactly?
	var bonusTier = RandomNumber( 1, maxTierLevel );

	switch( bonusTier )
	{
		case 1:
			dmgBonus = 1;
			dmgBonusName = "ruin";
			break;
		case 2:
			dmgBonus = 3;
			dmgBonusName = "might";
			break;
		case 3:
			dmgBonus = 5;
			dmgBonusName = "force";
			break;
		case 4:
			dmgBonus = 7;
			dmgBonusName = "power";
			break;
		case 5:
			dmgBonus = 9;
			dmgBonusName = "vanquishing";
			break;
	}

	//Let's add that bonus-damage
	if( dmgBonusType = 0 )
	{
		rndWeapon.hidamage += dmgBonus;
	}
	else
	{
		rndWeapon.lodamage += ( dmgBonus / 2 );
		rndWeapon.hidamage += ( dmgBonus / 2 );
	}

	// Let's add the bonus-name
	rndWeapon.name2 += dmgBonusName;
}

function CalcWeaponSpellBonus( rndWeapon )
{
	// Define some variables
	var spellBonusName = "";
	var spellCircle = 0;
	var spellNumber = 0;
	var spellCharges = 0;

	// Let's put in a small chance of getting a better bonus
	maxTierLevel = CalcTierBonus( maxTierLevel );

	// Which bonus-tier are we adding, exactly?
	var bonusTier = RandomNumber( 1, maxTierLevel );

	switch( bonusTier )
	{
		case 1:
			spellCircle = 1;
			var iNum = RandomNumber( 0, 2 );
			if( iNum == 0 )
			{
				spellBonusName = "Clumsiness";
				spellNumber = 1;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( iNum == 1 )
			{
				spellBonusName = "Feeblemindedness";
				spellNumber = 3;
				spellCharges = RandomNumber( 5, 30 );
			}
			else
			{
				spellBonusName = "Weakness";
				spellNumber = 8;
				spellCharges = RandomNumber( 5, 30 );
			}
			break;
		case 2:
			var iNum = RandomNumber( 0, 100 );
			if( iNum < 45 ) // 45% chance
			{
				spellBonusName = "Burning"; // Magic Arrow
				spellCircle = 1;
				spellNumber = 5;
				spellCharges = RandomNumber( 5, 25 );
			}
			else if( iNum < 80 ) // 35% chance
			{
				spellBonusName = "Wounding"; // Harm
				spellCircle = 2;
				// spellNumber = 4;
				spellNumber = 12;
				spellCharges = RandomNumber( 5, 25 );
			}
			else // 20% chance
			{
				spellBonusName = "Daemon's Breath"; // Fireball
				spellCircle = 3;
				// spellNumber = 2;
				spellNumber = 18;
				spellCharges = RandomNumber( 5, 25 );
			}
			break;
		case 3:
			spellCircle = 4;
			var iNum = RandomNumber( 0, 1 );
			if( iNum == 0 )
			{
				spellBonusName = "Mages Bane"; // Mana Drain
				// spellNumber = 7;
				spellNumber = 31;
				spellCharges = RandomNumber( 5, 20 );
			}
			else
			{
				spellBonusName = "Evil"; // Curse
				// spellNumber = 3;
				spellNumber = 27;
				spellCharges = RandomNumber( 5, 20 );
			}
			break;
		case 4:
			spellBonusName = "Thunder"; // Lightning
			spellCircle = 4;
			// spellNumber = 6;
			spellNumber = 30;
			spellCharges = RandomNumber( 5, 15 );
			break;
		case 5:
			spellBonusName = "Ghoul's Touch"; // Paralyze
			spellCircle = 5;
			// spellNumber = 6;
			spellNumber = 38;
			spellCharges = RandomNumber( 5, 10 );
			break;
	}

	//Let's add that bonus-spell
	// rndWeapon.type = 15;
	rndWeapon.AddScriptTrigger( magicWeaponSpellScriptID );
	rndWeapon.morex = spellCircle;
	rndWeapon.morey = spellNumber;
	rndWeapon.morez = spellCharges;

	// Let's add the bonus-name
	rndWeapon.name2 += spellBonusName;
}

// ---------------------
// CREATE MAGICAL ARMOR
// ---------------------

function CreateMagicArmor( tierNum, placeHolderItem )
{
	// Create random item from chosen tier
	var rndArmor;
	var rndArmorNum;
	switch( tierNum )
	{
		case 1:
			rndArmorNum = RandomNumber( 0, tier1armors.length - 1);
			rndArmor = CreateDFNItem( null, null, tier1armors[rndArmorNum], 1, "ITEM", false );
			break;
		case 2:
			rndArmorNum = RandomNumber( 0, tier2armors.length - 1);
			rndArmor = CreateDFNItem( null, null, tier2armors[rndArmorNum], 1, "ITEM", false );
			break;
		case 3:
			rndArmorNum = RandomNumber( 0, tier3armors.length - 1);
			rndArmor = CreateDFNItem( null, null, tier3armors[rndArmorNum], 1, "ITEM", false );
			break;
		case 4:
			rndArmorNum = RandomNumber( 0, tier4armors.length - 1 );
			rndArmor = CreateDFNItem( null, null, tier4armors[rndArmorNum], 1, "ITEM", false );
			break;
		case 5:
			rndArmorNum = RandomNumber( 0, tier5armors.length - 1 );
			rndArmor = CreateDFNItem( null, null, tier5armors[rndArmorNum], 1, "ITEM", false );
			break;
		default:
			break;
	}

	if( !ValidateObject( rndArmor ))
	{
		Console.Error( "Unable to spawn DFN item from tier-array " + tierNum + " with array-position (rndArmorNum) " + rndArmorNum );
		return null;
	}

	// Handle armor bonuses
	// Should we add a HP bonus, an AR bonus - or both?
	var rndBonus = RandomNumber( 0, 100 );

	var addHpBonus = false;
	var addArmorBonus = false;

	// Increase rndBonus by 2.5 to 12.5 extra depending on maxTierLevel of NPC, so
	// the higher the maxTierLevel, the less chance of getting rubbish magical items!
	rndBonus = rndBonus + ( Math.round( maxTierLevel * 2.5 ));
	rndBonus = Math.round( rndBonus );

	if( rndBonus >= 0 && rndBonus < 95 )
	{
		// 1 Magical Effect
		iNum = RandomNumber( 0, 1 );
		if( iNum == 0 )
		{
			//Only HP Bonus
			addHpBonus = true;
		}
		else if( iNum == 1 )
		{
			//Only AR Bonus
			addArmorBonus = true;
		}
	}
	else
	{
		// 2 Magical Effects
		addHpBonus = true;
		addArmorBonus = true;
	}

	// First store the original item-name in a tempVariable
	var tempName = rndArmor.name;
	rndArmor.name2 = "";

	// Are we adding HP bonuses?
	if( addHpBonus )
	{
		CalcHpBonus( rndArmor, 1 );
		rndArmor.name2 += " ";
	}

	// Add item-name after HP bonus, but before a potential AR bonus
	rndArmor.name2 += tempName;

	// Are we adding AR bonuses?
	if( addArmorBonus )
	{
		CalcArmorBonus( rndArmor );
	}

	// Are we adding spell bonuses?
	iNum = RandomNumber( 1, 100 );
	if( iNum <= 5 ) // 5% chance of spell bonus on a given magic armor
	{
		if( addArmorBonus )
		{
			rndArmor.name2 += " and ";
		}
		else
		{
			rndArmor.name2 += " of ";
		}

		CalcArmorSpellBonus( rndArmor );

		// Add script that handles equip effects to the magic armor
		rndArmor.AddScriptTrigger( armorSpellsScriptID );
	}

	rndArmor.name = "magic " + tempName;

	// Is final name too long? Not much we can do about it, MAX_NAME in UOX3 code is set to 128... Log it anyway, though.
	if( rndArmor.name2.length >= 128 )
	{
		Console.Log( "Magic armor generated with name consisting of 128 or more letters! Serial: " + rndArmor.serial );
	}

	if( outputMagicItemNamesToFile )
	{
		AddToDatabase( 1, rndArmor.name2 );
	}

	if( ValidateObject( rndArmor ) && ValidateObject( placeHolderItem ))
	{
		// Loop through all the properties for our new magic item and copy them over to the placeholder item
		for( var itemProp in rndArmor )
		{
			if( itemProp != null && itemProp != "serial" && itemProp != "scripttrigger" && itemProp != "scriptTriggers" )
			{
				if( typeof( rndArmor[itemProp] ) != "undefined" && rndArmor[itemProp] != null )
				{
					if( itemProp == "race" && rndArmor.race != null )
					{
						placeHolderItem.race = rndArmor.race.id;
					}
					else
					{
						placeHolderItem[itemProp] = rndArmor[itemProp];
					}
				}
			}
		}

		// Copy scripttriggers too!
		var scriptTriggerList = rndArmor.scriptTriggers;
		for( var i = 0; i < scriptTriggerList.length; i++ )
		{
			if( scriptTriggerList[i] != 0 && scriptTriggerList[i] != magicItemScriptID )
			{
				placeHolderItem.AddScriptTrigger( scriptTriggerList[i] );
			}
		}
	}
	return rndArmor;
}

function CalcArmorBonus( rndArmor )
{
	// Define some variables
	var arBonus = 0;
	var arBonusName = "";

	// Let's put in a small chance of getting a better bonus
	maxTierLevel = CalcTierBonus( maxTierLevel );

	// Which bonus-tier are we adding, exactly?
	var bonusTier = RandomNumber( 1, maxTierLevel );

	switch( bonusTier )
	{
		case 1:
			arBonus = 5;
			arBonusName = "of defense";
			break;
		case 2:
			arBonus = 10;
			arBonusName = "of guarding";
			break;
		case 3:
			arBonus = 15;
			arBonusName = "of hardening";
			break;
		case 4:
			arBonus = 20;
			arBonusName = "of fortification";
			break;
		case 5:
		case 6:
		case 7:
		default:
			arBonus = 25;
			arBonusName = "of invulnerability";
			break;
	}

	//Let's add that bonus and change the name
	var defRating = rndArmor.Resist( 1 );
	defRating += arBonus;
	rndArmor.Resist( 1, defRating );
	rndArmor.name2 += " " + arBonusName;
}

function CalcArmorSpellBonus( rndArmor )
{
	// Define some variables
	var spellBonusName = "";
	var spellCircle = 0;
	var spellNumber = 0;
	var spellCharges = 0;

	// Let's put in a small chance of getting a better bonus
	maxTierLevel = CalcTierBonus( maxTierLevel );

	// Which bonus-tier are we adding, exactly?
	var bonusTier = RandomNumber( 1, maxTierLevel );

	var rndNum = 0;
	switch( bonusTier )
	{
		case 1:
			rndNum = RandomNumber( 0, 100 );
			if( rndNum <= 25 ) // 25% chance
			{
				spellBonusName = "Clumsiness";
				spellCircle = 1;
				spellNumber = 1;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 50 ) // 25% chance
			{
				spellBonusName = "Feeblemindedness";
				spellCircle = 1;
				spellNumber = 3;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 75 ) // 25% chance
			{
				spellBonusName = "Weakness";
				spellCircle = 1;
				spellNumber = 8;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 80 ) // 5% chance
			{
				spellBonusName = "Strength";
				spellCircle = 2;
				spellNumber = 16;
				spellCharges = RandomNumber( 5, 25 );
			}
			else if( rndNum <= 85 ) // 5% chance
			{
				spellBonusName = "Cunning";
				spellCircle = 2;
				spellNumber = 10;
				spellCharges = RandomNumber( 5, 25 );
			}
			else if( rndNum <= 90 ) // 5% chance
			{
				spellBonusName = "Agility";
				spellCircle = 2;
				spellNumber = 19
				spellCharges = RandomNumber( 5, 25 );
			}
			else  // 10% chance
			{
				spellBonusName = "Night Eyes";
				spellCircle = 1;
				spellNumber = 6;
				spellCharges = RandomNumber( 5, 30 );
			}
			break;
		case 2:
			rndNum = RandomNumber( 0, 100 );
			if( rndNum <= 10 ) // 10% chance
			{
				spellBonusName = "Clumsiness";
				spellCircle = 1;
				spellNumber = 1;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 20 ) // 10% chance
			{
				spellBonusName = "Feeblemindedness";
				spellCircle = 1;
				spellNumber = 3;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 30 ) // 10% chance
			{
				spellBonusName = "Weakness";
				spellCircle = 1;
				spellNumber = 8;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 50 ) // 20% chance
			{
				spellBonusName = "Strength";
				spellCircle = 2;
				spellNumber = 16;
				spellCharges = RandomNumber( 5, 25 );
			}
			else if( rndNum <= 70 ) // 20% chance
			{
				spellBonusName = "Cunning";
				spellCircle = 2;
				spellNumber = 10;
				spellCharges = RandomNumber( 5, 25 );
			}
			else if( rndNum <= 90 ) // 20% chance
			{
				spellBonusName = "Agility";
				spellCircle = 2;
				spellNumber = 19
				spellCharges = RandomNumber( 5, 25 );
			}
			else  // 10% chance
			{
				spellBonusName = "Night Eyes";
				spellCircle = 1;
				spellNumber = 6;
				spellCharges = RandomNumber( 5, 30 );
			}
			break;
		case 3:
			rndNum = RandomNumber( 0, 100 );
			if( rndNum <= 5 ) // 5% chance
			{
				spellBonusName = "Clumsiness";
				spellCircle = 1;
				spellNumber = 1;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 10 ) // 5% chance
			{
				spellBonusName = "Feeblemindedness";
				spellCircle = 1;
				spellNumber = 3;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 15 ) // 5% chance
			{
				spellBonusName = "Weakness";
				spellCircle = 1;
				spellNumber = 8;
				spellCharges = RandomNumber( 5, 30 );
			}
			else if( rndNum <= 40 ) // 25% chance
			{
				spellBonusName = "Strength";
				spellCircle = 2;
				spellNumber = 16;
				spellCharges = RandomNumber( 5, 25 );
			}
			else if( rndNum <= 65 ) // 25% chance
			{
				spellBonusName = "Cunning";
				spellCircle = 2;
				spellNumber = 10;
				spellCharges = RandomNumber( 5, 25 );
			}
			else if( rndNum <= 90 ) // 25% chance
			{
				spellBonusName = "Agility";
				spellCircle = 2;
				spellNumber = 19
				spellCharges = RandomNumber( 5, 25 );
			}
			if( rndNum <= 95 ) // 5% chance
			{
				spellBonusName = "Protection";
				spellCircle = 2;
				spellNumber = 5;
				spellCharges = RandomNumber( 5, 25 );
			}
			else // 5% chance
			{
				spellBonusName = "Curse";
				spellCircle = 4;
				spellNumber = 27;
				spellCharges = RandomNumber( 5, 20 );
			}
			break;
		case 4:
			rndNum = RandomNumber( 0, 100 );
			if( rndNum <= 45 ) // 45% chance
			{
				spellBonusName = "Protection";
				spellCircle = 2;
				spellNumber = 5;
				spellCharges = RandomNumber( 5, 30 );
			}
			if( rndNum <= 90 ) // 45% chance
			{
				spellBonusName = "Blessing";
				spellCircle = 3;
				spellNumber = 4;
				spellCharges = RandomNumber( 5, 25 );
			}
			else // 10% chance
			{
				spellCircle = 4;
				spellBonusName = "Curse";
				spellNumber = 27;
				spellCharges = RandomNumber( 5, 20 );
			}
			break;
		case 5:
			rndNum = RandomNumber( 0, 100 );
			if( rndNum <= 45 ) // 40% chance
			{
				spellBonusName = "Spell Reflection";
				spellCircle = 5;
				spellNumber = 36;
				spellCharges = RandomNumber( 5, 15 );
			}
			else if( rndNum <= 45 ) //40% chance
			{
				spellBonusName = "Invisibility";
				spellCircle = 6;
				spellNumber = 44;
				spellCharges = RandomNumber( 5, 10 );
			}
			else // 10% chance
			{
				spellBonusName = "Curse";
				spellCircle = 4;
				spellNumber = 27;
				spellCharges = RandomNumber( 5, 20 );
			}
			break;
		default:
			// Fallback, just in case...
			spellBonusName = "Night Eyes";
			spellCircle = 1;
			spellNumber = 6;
			spellCharges = RandomNumber( 5, 30 );
			break;
	}

	//Let's add that bonus-spell
	rndArmor.type = 15;
	rndArmor.morex = spellCircle;
	rndArmor.morey = spellNumber;
	rndArmor.morez = spellCharges;

	// Let's add the bonus-name
	rndArmor.name2 += spellBonusName;
}

// ----------------------
// CREATE MAGICAL WANDS/STAVES
// ----------------------

function CreateMagicWand( tierNum, placeHolderItem )
{
	// Create random wand or staff
	var rndWand;
	var rndWandNum;

	// Create a magic wand or staff
	rndWandNum = RandomNumber( 0, magicWands.length - 1 );
	rndWand = CreateDFNItem( null, null, magicWands[rndWandNum], 1, "ITEM", false );

	if( !ValidateObject( rndWand ))
	{
		Console.Error( "Unable to spawn DFN item from magic wand/staff array with array-position (rndWandNum) " + rndWandNum );
		return null;
	}

	// Grab a copy of the original name of the item, so we have it for later
	var tempName = rndWand.name;
	rndWand.name2 = rndWand.name;

	rndWand.name2 += " of ";
	CalcWandSpellBonus( rndWand );

	rndWand.name = "magic " + tempName;

	// Is final name too long? Not much we can do about it, MAX_NAME in UOX3 code is set to 128... Log it anyway, though.
	if( rndWand.name2.length >= 128 )
	{
		Console.Log( "Magic item generated with name consisting of 128 or more letters! Serial: " + rndWand.serial + ", length: " + rndWand.name2.length );
	}

	if( outputMagicItemNamesToFile )
	{
		AddToDatabase( 0, rndWand.name2 );
	}

	if( ValidateObject( rndWand ) && ValidateObject( placeHolderItem ))
	{
		// Loop through all the properties for our new magic item and copy them over to the placeholder item
		for( var itemProp in rndWand )
		{
			if( itemProp != null && itemProp != "serial" && itemProp != "scripttrigger" && itemProp != "scriptTriggers" )
			{
				if( typeof( rndWand[itemProp] ) != "undefined" && rndWand[itemProp] != null )
				{
					if( itemProp == "race" && rndWand.race != null )
					{
						placeHolderItem.race = rndWand.race.id;
					}
					else
					{
						placeHolderItem[itemProp] = rndWand[itemProp];
					}
				}
			}
		}

		// Copy scripttriggers too!
		var scriptTriggerList = rndWand.scriptTriggers;
		for( var i = 0; i < scriptTriggerList.length; i++ )
		{
			if( scriptTriggerList[i] != 0 && scriptTriggerList[i] != magicItemScriptID )
			{
				placeHolderItem.AddScriptTrigger( scriptTriggerList[i] );
			}
		}
	}
	return rndWand;
}

// Weighted randomizer for quick and easy randomization where lower scores have higher weights than higher ones
function WeightedRandom( min, max )
{
 	return Math.round( max / ( Math.random() * max + min ));
}

function CalcWandSpellBonus( rndWand )
{
	// Define some variables
	var spellBonusName = "";
	var spellCircle = 0;
	var spellNumber = 0;
	var spellCharges = 0;

	// Let's put in a small chance of getting a better bonus
	maxTierLevel = CalcTierBonus( maxTierLevel );

	// Which bonus-tier are we adding, exactly?
	var bonusTier = WeightedRandom( 1, maxTierLevel );

	switch( bonusTier )
	{
		case 1: // Tier 1 - Item ID, Heal
			spellCircle = 1;
			spellCharges = RandomNumber( 5, 125 );
			var iNum = RandomNumber( 0, 1 );
			if( iNum == 0 ) // 50%, or 13/26
			{
				spellBonusName = "Identification";
				spellNumber = 65;
			}
			else if( iNum == 1 ) // 50%, or 13/26
			{
				spellBonusName = "Heal";
				spellNumber = 4;
			}
			break;
		case 2: // Tier 2 - Clumsy, Feeblemind, Weaken
			spellCircle = 1;
			spellCharges = RandomNumber( 5, 100 );
			var iNum = RandomNumber( 0, 2 );
			if( iNum == 0 ) // 33%, or 11/33
			{
				spellBonusName = "Clumsiness";
				spellNumber = 1;
			}
			else if( iNum == 1 ) // 33%, or 11/33
			{
				spellBonusName = "Feeblemindedness";
				spellNumber = 3;
			}
			else // 33%, or 11/33
			{
				spellBonusName = "Weakness";
				spellNumber = 8;
			}
			break;
		case 3: // Tier 3 - Magic Arrow, Harm, Greater Heal, Fireball
			spellCharges = RandomNumber( 5, 75 );
			var iNum = RandomNumber( 1, 100 );
			if( iNum <= 31 ) // 31% chance, or 10/32
			{
				spellBonusName = "Burning"; // Magic Arrow
				spellCircle = 1;
				spellNumber = 5;
			}
			else if( iNum <= 56 ) // 25% chance, or 8/32
			{
				spellBonusName = "Wounding"; // Harm
				spellCircle = 2;
				spellNumber = 12;
			}
			else if( iNum <= 78 ) // 22% chance, or 7/32
			{
				spellBonusName = "Greater Heal"; // Greater Heal
				spellCircle = 4;
				spellNumber = 29;
			}
			else // 22% chance, or 7/32
			{
				spellBonusName = "Daemon's Breath"; // Fireball
				spellCircle = 3;
				spellNumber = 18;
			}
			break;
		case 4: // Tier 4 - Lightning, Mana Drain
			spellCircle = 4;
			spellCharges = RandomNumber( 5, 50 );
			var iNum = RandomNumber( 1, 100 );
			if( iNum <= 63 ) // 63% chance, or 5/8
			{
				spellBonusName = "Mages Bane"; // Mana Drain
				spellNumber = 31;
			}
			else // 27% chance, or 3/8
			{
				spellBonusName = "Thunder"; // Lightning
				spellNumber = 30;
			}
			break;
		case 5: // Tier 5 - Paralyze
			spellBonusName = "Mage's Bane"; // Paralyze
			spellCircle = 5;
			spellNumber = 38;
			spellCharges = RandomNumber( 5, 25 );
			break;
	}

	//Let's add that bonus-spell
	rndWand.type = 15;
	rndWand.morex = spellCircle;
	rndWand.morey = spellNumber;
	rndWand.morez = spellCharges;

	// Let's add the bonus-name
	rndWand.name2 += spellBonusName;
}

// Debug-function to output list of names generated by script
function AddToDatabase( itemType, rndItemName )
{
	var j = 0;
	var addName = true;
	var magicName = "";
	var mFileWrite = new UOXCFile();
	if( itemType == 0 ) // weapon
	{
		mFileWrite.Open( "magicweapons_list.txt", "a" );
	}
	else
	{
		mFileWrite.Open( "magicarmors_list.txt", "a" );
	}
	mFileWrite.Write( rndItemName + "\n");
	mFileWrite.Close()
	mFileWrite.Free();
}

// ---------------------
// CREATE MAGICAL RINGS
// ---------------------

function CreateMagicRing( tierNum, placeHolderItem )
{
	// Create random magic ring
	var rndRing;
	var rndRingNum;

	// Create a ring object
	rndRing = CreateDFNItem( null, null, "0x108a", 1, "ITEM", false );

	if( !ValidateObject( rndRing ))
	{
		Console.Error( "Unable to spawn magic ring item from DFN entry 0x108a" );
		return null;
	}

	// Grab a copy of the original name of the item, so we have it for later
	var tempName = rndRing.name;
	rndRing.name2 = rndRing.name;

	rndRing.name2 += " of ";
	CalcRingSpellBonus( rndRing );

	rndRing.name = "a magic " + tempName;

	// Is final name too long? Not much we can do about it, MAX_NAME in UOX3 code is set to 128... Log it anyway, though.
	if( rndRing.name2.length >= 128 )
	{
		Console.Log( "Magic item generated with name consisting of 128 or more letters! Serial: " + rndRing.serial + ", length: " + rndRing.name2.length );
	}

	if( outputMagicItemNamesToFile )
		AddToDatabase( 0, rndRing.name2 );

	if( ValidateObject( rndRing ) && ValidateObject( placeHolderItem ))
	{
		// Loop through all the properties for our new magic item and copy them over to the placeholder item
		for( var itemProp in rndRing )
		{
			if( itemProp != null && itemProp != "serial" && itemProp != "scripttrigger" && itemProp != "scriptTriggers" )
			{
				if( typeof( rndRing[itemProp] ) != "undefined" && rndRing[itemProp] != null )
				{
					if( itemProp == "race" && rndRing.race != null )
					{
						placeHolderItem.race = rndRing.race.id;
					}
					else
					{
						placeHolderItem[itemProp] = rndRing[itemProp];
					}
				}
			}
		}

		// Copy scripttriggers too!
		var scriptTriggerList = rndRing.scriptTriggers;
		for( var i = 0; i < scriptTriggerList.length; i++ )
		{
			if( scriptTriggerList[i] != 0 && scriptTriggerList[i] != magicItemScriptID )
			{
				placeHolderItem.AddScriptTrigger( scriptTriggerList[i] );
			}
		}
	}
	return rndRing;
}

function CalcRingSpellBonus( rndWand )
{
	// Define some variables
	var spellBonusName = "";
	var spellCircle = 0;
	var spellNumber = 0;
	var spellCharges = 0;

	var rndNum = RandomNumber( 0, 1 );

	switch( rndNum )
	{
		case 0: // Teleport
			spellCircle = 3
			spellCharges = RandomNumber( 5, 25 );
			spellBonusName = "Teleport";
			spellNumber = 22;
			break;
		case 1: // Invisibility
			spellCircle = 6
			spellCharges = RandomNumber( 5, 25 );
			spellBonusName = "Invisibility";
			spellNumber = 44;
			break;
		default:
			break;
	}

	//Let's add that bonus-spell
	rndWand.type = 15;
	rndWand.morex = spellCircle;
	rndWand.morey = spellNumber;
	rndWand.morez = spellCharges;

	// Let's add the bonus-name
	rndWand.name2 += spellBonusName;
}
