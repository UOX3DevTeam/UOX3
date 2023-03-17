// This script triggers for items right after they've been crafted
const scriptID = 4033;

// If enabled, runic hammers received from BOD rewards can be used to add the equivalent of
// magic properties to weapons (UOR/LBR) and/or armors (AoS and beyond)
const runicHammersEnabled = true;

// Damage bonus applied to exceptional weapons
const exceptionalDamageBonusUOR = 4; // 4 flat bonus damage
const exceptionalDamageBonusAoS = 35; // 35% bonus damage (originally 20% in publish 17, modified in publish 19)

// Armor bonuses for exceptional armors
const exceptionalArmorBonusUOR = 20; // 20% bonus to AR
const exceptionalArmorBonusAoS = 15; // 15% bonus to resists

function onMakeItem( pSock, pCrafter, itemCrafted, craftEntryID )
{
	if( pSock == null || !ValidateObject( pCrafter ))
		return;

	if( !ValidateObject( itemCrafted ))
	{
		if( craftEntryID != 0 )
		{
			// Crafting failed - let's handle any after effects of that failure to craft!
			HandleFailedCrafting( pSock, pCrafter, craftEntryID );
		}
		return;
	}

	// Apply special effect based on item crafted
	if( ValidateObject( itemCrafted ))
	{
		// Jewelry, furniture, potions
		switch( craftEntryID )
		{
			case 239: // necklace
			case 240: // necklace
			case 241: // earrings
			case 242: // necklace
			case 243: // bracelet
				// Name the jewelry after the gems used to craft them
				itemCrafted.name = pCrafter.GetTempTag( "targetedSubResourceName" ) + " " + itemCrafted.name;
				ClearTagsAndScript( pCrafter );
				return;
			case 63: // Small Chest
			case 64: // Small Crate
			case 65: // Medium Crate
			case 67: // Large Crate
			case 68: // Chest
			case 70: // Armoire
			case 71: // Armoire
				// Add locks (and keys) to these containers if player has high enough Tinkering skill
				var createEntry = CreateEntries[craftEntryID];
				var skills = createEntry.skills; // list of skill requirements
				for( var i = 0; i < skills.length; i++ )
			    {
			        var skillReq = skills[i];
			        var skillNumber = skillReq[0];
					var minSkill = skillReq[1];

					if( skillNumber == 11 ) // Carpentry
					{
						// Do a random check to see if player succeeds at making a lock for the container
						if( pCrafter.skills.tinkering >= RandomNumber( minSkill / 2, 1000 ))
						{
							// Great success! Add a key to the container, set MORE values to match for both!
							var newKey = CreateDFNItem( pSock, pCrafter, "0x100E", 1, "ITEM", true );
							newKey.container = itemCrafted;
							newKey.PlaceInPack();
							newKey.name = "key for " + itemCrafted.name;

							// Set more of key AND box to match serial of box
							newKey.more = itemCrafted.serial;
							itemCrafted.more = itemCrafted.serial;
							pSock.SysMessage( GetDictionaryEntry( 12009, pSock.language )); // You added a lock and key to the container!
						}
						break;
					}
				}
				ClearTagsAndScript( pCrafter );
				return;
			case 290: // Potion: Agility
			case 291: // Potion: Greater Agility
			case 292: // Potion: Lesser Cure
			case 293: // Potion: Cure
			case 294: // Potion: Greater Cure
			case 295: // Potion: Lesser Explosion
			case 296: // Potion: Explosion
			case 297: // Potion: Greater Explosion
			case 298: // Potion: Lesser Heal
			case 299: // Potion: Heal
			case 300: // Potion: Greater heal
			case 301: // Potion: Lesser Poison
			case 302: // Potion: Poison
			case 303: // Potion: Greater Poison
			case 304: // Potion: Deadly Poison
			case 305: // Potion: Refresh
			case 306: // Potion: Total Refresh
			case 307: // Potion: Strength
			case 308: // Potion: Greater Strength
			case 309: // Potion: Nightsight
				// Players auto-identify potions they craft
				itemCrafted.name = itemCrafted.name2;
				itemCrafted.name2 = "#";
				ClearTagsAndScript( pCrafter );
				return;
			case 0: // Failed to craft item
			default:
				break;
		}

		// Apply bonuses to exceptional items crafted, if rank system is enabled for items (T2A and beyond)
		var coreShardEraValue = EraStringToNum( GetServerSetting( "CoreShardEra" ));
		if( coreShardEraValue >= EraStringToNum( "t2a" ) && GetServerSetting( "RankSystem" ))
		{
			if( itemCrafted.layer > 1 && itemCrafted.def > 0 && itemCrafted.maxhp > 0 && itemCrafted.rank == 10 )
			{
				// Apply bonuses to exceptionally crafted Armor
				ApplyExceptionalArmorBonuses( pCrafter, itemCrafted, coreShardEraValue );
			}
			else if( itemCrafted.layer <= 2 && itemCrafted.hidamage > 0 && itemCrafted.rank == 10 )
			{
				// Apply bonuses to exceptionally crafted Weapons
				ApplyExceptionalWeaponBonuses( pCrafter, itemCrafted, coreShardEraValue );
			}
		}

		// Apply bonuses to items crafted with Runic Smithy Hammers (UOR and beyond)
		if( coreShardEraValue >= EraStringToNum( "uor" ) && runicHammersEnabled )
		{
			var usedRunicHammer = pCrafter.GetTempTag( "usedRunicHammer" );
			var runicHammerType = pCrafter.GetTempTag( "runicHammerType" );
			if( usedRunicHammer && coreShardEraValue >= EraStringToNum( "aos" ) && itemCrafted.layer > 1 && itemCrafted.def > 0 && itemCrafted.maxhp > 0 )
			{
				// Apply bonuses to armor crafted with Runic Smithy Hammers equipped (only from AoS expansion onwards)
				ApplyRunicArmorBonuses( pCrafter, itemCrafted, coreShardEraValue, runicHammerType );
			}
			else if( usedRunicHammer && itemCrafted.layer <= 2 && itemCrafted.hidamage > 0 )
			{
				// Apply bonuses to Weapons crafted with Runic Smithy Hammers equipped
				ApplyRunicWeaponBonuses( pCrafter, itemCrafted, coreShardEraValue, runicHammerType );
			}
		}
	}

	ClearTagsAndScript( pCrafter );
}

function ApplyExceptionalArmorBonuses( pCrafter, itemCrafted, coreShardEraValue )
{
	// Armors
	if( coreShardEraValue <= EraStringToNum( "lbr" ))
	{
		// T2A - http://web.archive.org/web/19991002054003/uo.stratics.com/strat/blacksmith.htm
		// LBR/Pre-AoS - https://web.archive.org/web/20020607153623/http://uo.stratics.com/content/guides/blacksmith/blacessay.shtml
		// Exceptional Plate = AR of 36 (+20%), Durability = +5 to +10 hps
		// Exceptional Heater = with 70% parrying 1 less AR than Chaos Shield, Durability = +5 to +10 hps
		// 20% bonus to armor's AR, 5-10 extra HP
		itemCrafted.def = Math.round( itemCrafted.def * ( 1 + ( exceptionalArmorBonusUOR / 100 )));
		itemCrafted.maxhp += RandomNumber( 5, 10 );
		itemCrafted.health = itemCrafted.maxhp;
	}
	else // AoS and beyond
	{
		// https://www.uoguide.com/Exceptional
		/* An exceptionally made piece of Armor will gain a 15% bonus that is distributed randomly
		among the base resistances of that item. For example, an exceptionally made Platemail Tunic
		could have 5% applied to Physical, 6% to Fire, and 2% to each Cold and Poison. Or, and this
		would be extremely rare, all 15% could be added to Physical.*/
		var totalExceptionalArmorBonus = exceptionalArmorBonusAoS;
		if( coreShardEraValue >= EraStringToNum( "ml" ))
		{
			// Arms Lore bonus
			// Publish 44, 2007
			// When a piece of armor (not including shields) is exceptionally crafted, it will receive a resist
			// bonus based on the pCrafter’s Arms Lore skill. For every 20 points of Arms Lore, one resist normally
			// available on that piece of armor will be raised by 1, for a total possible bonus of 5 distributed
			// randomly. (Ex. At GM Arms Lore, the total resist bonus is +5%. One resist could receive the
			// full +5% bonus. Alternatively, one resist could receive +2%, and three other resists could
			// receive +1%, for the total of +5%.)

			// https://www.uoguide.com/Exceptional
			/* Exceptionally crafted armor also receives an additional 1% bonus (again randomly distributed)
			for every 20 points of Arms Lore your character has. The maximum bonus is therefore 20%.
			However, the total additional points from Exceptional and Arms Lore is capped at 20%.*/
			totalExceptionalArmorBonus += Math.min( Math.round(( pCrafter.skills.armslore / 10 ) / 20 ), 5 );
		}

		// Distribute bonus randomly among the different resist types
		var bonusPoints = DistributeBonusPoints( totalExceptionalArmorBonus, 5 );
		itemCrafted.def += bonusPoints[0]; // def used as "resistPhysical"
		itemCrafted.resistFire += bonusPoints[1];
		itemCrafted.resistCold += bonusPoints[2];
		itemCrafted.resistPoison += bonusPoints[3];
		itemCrafted.resistLightning += bonusPoints[4];
	}
}

function ApplyExceptionalWeaponBonuses( pCrafter, itemCrafted, coreShardEraValue )
{
	// Weapons
	if( coreShardEraValue <= EraStringToNum( "lbr" ))
	{
		// T2A - http://web.archive.org/web/19991002054003/uo.stratics.com/strat/blacksmith.htm
		/* Exceptional Weapons and Armor have more Durability and do more damage or give more protection than Average Weapons and Armor do. Examples:
			Exceptional Viking = 26+ Damage + 1-3 Damage (Ruin/Might), Durability = +10 to +20 hps*/

		// Weapon Damage Bonus
		switch( GetServerSetting( "WeaponDamageBonusType" ))
		{
			case 0: // Apply flat weapon damage bonus to .hidamage
				itemCrafted.hidamage += exceptionalDamageBonusUOR;
				break;
			case 1: // Split weapon damage bonus between .lodamage and .hidamage
				itemCrafted.lodamage += Math.round( exceptionalDamageBonusUOR / 2 );
				itemCrafted.hidamage += Math.round( exceptionalDamageBonusUOR / 2 );
				break;
			case 2: // Apply flat weapon damage bonus to BOTH .lodamage and .hidamage
				itemCrafted.lodamage += exceptionalDamageBonusUOR;
				itemCrafted.hidamage += exceptionalDamageBonusUOR;
				break;
			default:
				break;
		}

		// Weapon Durability Bonus
		itemCrafted.maxhp += RandomNumber( 10, 20 ); // +10 to +20 hp
		itemCrafted.health = itemCrafted.maxhp;
	}
	else // AoS and beyond
	{
		/* Publish 17
			“Exceptional” weapons gain a 20% bonus to their damage rating (except for Runic weapons, which will only get a 10% bonus).*/
		/* Publish 19
			The “exceptional” bonus for player-crafted items (weapons and armor) has been improved to 35%.*/
		var totalExceptionalDamageBonus = exceptionalDamageBonusAoS;
		if( coreShardEraValue >= EraStringToNum( "ml" ))
		{
			// Add Arms Lore bonus
			// https://www.uoguide.com/Exceptional
			/*Exceptionally crafted Weapons get a 35% Damage Increase bonus as a free Item Property.
			This is boosted by 1% for every 20 points of Arms Lore your character has. The maximum bonus is therefore 40%.*/
			totalExceptionalDamageBonus += Math.min( Math.round(( pCrafter.skills.armslore / 10 ) / 20 ), 5 );
		}

		switch( GetServerSetting( "WeaponDamageBonusType" ))
		{
			case 0: // Apply flat weapon damage bonus to .hidamage
				itemCrafted.hidamage += Math.round(( itemCrafted.hidamage * totalExceptionalDamageBonus ) / 100 );
				break;
			case 1: // Split weapon damage bonus between .lodamage and .hidamage
				itemCrafted.lodamage += Math.round((( itemCrafted.hidamage * totalExceptionalDamageBonus ) / 100 ) / 2 );
				itemCrafted.hidamage += Math.round((( itemCrafted.hidamage * totalExceptionalDamageBonus ) / 100 ) / 2 );
				break;
			case 2: // Apply flat weapon damage bonus to BOTH .lodamage and .hidamage
				itemCrafted.lodamage += Math.round(( itemCrafted.hidamage * totalExceptionalDamageBonus ) / 100 );
				itemCrafted.hidamage += Math.round(( itemCrafted.hidamage * totalExceptionalDamageBonus ) / 100 );
				break;
			default:
				break;
		}
	}
}

function ApplyRunicArmorBonuses( pCrafter, itemCrafted, coreShardEraValue, runicHammerType )
{
	// TODO

	// Samurai Empire - Publish 21, November 2003
	// Runic crafting items will now make a linear roll between their minimum and maximum values when
	// determining the intensity of the magic properties they impart on the crafted item.
}

function ApplyRunicWeaponBonuses( pCrafter, itemCrafted, coreShardEraValue, runicHammerType )
{
	// QUESTION: Should these bonuses show on crafted weapons as "normal" magic bonuses? accurate blah of might, etc.
	var weaponType = TriggerEvent( 2500, "GetWeaponType", null, itemCrafted.id );
	if( weaponType != "WRESTLING" && weaponType != "BOWS" && weaponType != "XBOWS" && weaponType != "THROWN" )
	{
		switch( runicHammerType )
		{
			case 0x0973: // Dull Copper Runic Hamer
				// Durable / Accurate
				itemCrafted.maxhp += 10;
				itemCrafted.SetTag( "accBonus", 50 );
				itemCrafted.AddScriptTrigger( 3301 ); // magic_weapon_accbonus.js
				break;
			case 0x0966: // Shadow Iron Runic Hammer
				// Durable / Ruin
				itemCrafted.maxhp += 10;
				itemCrafted.hidamage += 1;
				break;
			case 0x07dd: // Copper Runic Hammer
				// Fortified / Surpassingly Accurate / Ruin
				itemCrafted.maxhp += 40;
				itemCrafted.hidamage += 1;
				itemCrafted.SetTag( "accBonus", 100 );
				itemCrafted.AddScriptTrigger( 3301 ); // magic_weapon_accbonus.js
				break;
			case 0x06d6: // Bronze Runic Hammer
				// Fortified / Surpassingly Accurate / Might
				itemCrafted.maxhp += 40;
				itemCrafted.hidamage += 3;
				itemCrafted.SetTag( "accBonus", 100 );
				itemCrafted.AddScriptTrigger( 3301 ); // magic_weapon_accbonus.js
				break;
			case 0x08a5: // Gold Runic Hammer
				// Indestructible / Eminently Accurate / Force
				itemCrafted.maxhp += 50;
				itemCrafted.hidamage += 5;
				itemCrafted.SetTag( "accBonus", 150 );
				itemCrafted.AddScriptTrigger( 3301 ); // magic_weapon_accbonus.js
				break;
			case 0x0979: // Agapite Runic Hammer
				// Indestructible / Eminently Accurate / Power
				itemCrafted.maxhp += 50;
				itemCrafted.hidamage += 7;
				itemCrafted.SetTag( "accBonus", 150 );
				itemCrafted.AddScriptTrigger( 3301 ); // magic_weapon_accbonus.js
				break;
			case 0x089f: // Verite Runic Hammer
				// Indestructible / Exceedingly Accurate / Power
				itemCrafted.maxhp += 50;
				itemCrafted.hidamage += 7;
				itemCrafted.SetTag( "accBonus", 200 );
				itemCrafted.AddScriptTrigger( 3301 ); // magic_weapon_accbonus.js
				break;
			case 0x08ab: // Valorite Runic Hammer
				// Indestructible / Supremely Accurate / Vanquishing
				itemCrafted.maxhp += 50;
				itemCrafted.hidamage += 9;
				itemCrafted.SetTag( "accBonus", 250 );
				itemCrafted.AddScriptTrigger( 3301 ); // magic_weapon_accbonus.js
				break;
			default:
				break;
		}

		itemCrafted.health = itemCrafted.maxhp;
	}
}

function DistributeBonusPoints( numPoints, numProperties )
{
	// Initialize weights for each property to 1
	var weights = [];
	for( var i = 0; i < numProperties; i++ )
	{
		weights[i] = 1;
	}

	// Initialize an array to store the number of points for each property
	var points = [];
	for( var i = 0; i < numProperties; i++ )
	{
		points[i] = 0;
	}

	// Loop through all the points and distribute them randomly
	for( var i = 0; i < numPoints; i++ )
	{
		// Calculate the sum of the weights
		var sumWeights = 0;
		for( var j = 0; j < numProperties; j++ )
		{
			sumWeights += weights[j];
		}

		// Choose a random number between 1 and the sum of the weights
		var randNum = Math.floor( Math.random() * sumWeights ) + 1;

		// Find the property that corresponds to the chosen number
		var propIndex = 0;
		var propSum = weights[0];
		while( randNum > propSum )
		{
			propIndex++;
			propSum += weights[propIndex];
		}

		// Add the point to the chosen property and update its weight
		points[propIndex]++;
		weights[propIndex] *= 1.1;
	}

	return points;
}

function HandleFailedCrafting( pSock, pCrafter, craftEntryID )
{
	// Handle any specifics related to failed attempts to craft specific items here
	// Example: Failed crafting of purple potion....explode in pCrafter's face? :D
	// ...

	// Clear up the temp tags, and remove this script from player
	ClearTagsAndScript( pCrafter );
}

function ClearTagsAndScript( pCrafter )
{
	// Clean up some tags while we're here
	pCrafter.SetTempTag( "targetedSubResourceId", null );
	pCrafter.SetTempTag( "targetedSubResourceName", null );
	pCrafter.SetTempTag( "targetedMainResourceId", null );
	pCrafter.SetTempTag( "targetedMainResourceSerial", null );
	pCrafter.SetTempTag( "usedRunicHammer", null );
	pCrafter.SetTempTag( "runicHammerType", null );

	// Remove script from player
	pCrafter.RemoveScriptTrigger( scriptID );
}