/// <reference path="../definitions.d.ts" />
// @ts-check
// Champion Spawn System
// v61 - Updated 7th Feb, 2026
const coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ));
const disableTopDamager = true; // Set to true to disable top damager logic and if so then all loot is free for players to fight over.
const powerScrollMaxReceivers = 6; // Who can get scrolls at most?

// Rarity weights (higher bonus = rarer; total should sum up to something sensible)
const powerScrollRarityTable = [
	{ bonus: 5,  weight: 60 },  // Wonderous (+5)
	{ bonus: 10, weight: 25 },  // Exalted (+10)
	{ bonus: 15, weight: 10 },  // Mythical (+15)
	{ bonus: 20, weight: 5 }    // Legendary (+20)
	//{ bonus: 25, weight: 1 }    // Ultima (+25)
];

const championRewardTable = [
	{ key: "uniqueList",     weight: 5 },   // rare
	{ key: "sharedList",     weight: 25 },  // common
	{ key: "decorativeList", weight: 10 }   // uncommon
];

/** @type {Record<string, "aos"|"se"|"sa">} */
var skillMinERA = {
	// AoS-era skills
	necromancy: "aos",
	chivalry: "aos",

	// Samurai Empire
	focus: "se",
	bushido: "se",
	ninjitsu: "se",

	// Stygian Abyss / later
	spellweaving: "sa",
	mysticism: "sa",
	throwing: "sa"
};

/** @type { ( skillProp: string ) => boolean } */
function IsSkillAllowedByEra( skillProp )
{
	var minEraName = skillMinERA[skillProp];
	if( !minEraName )
	{
		// No min era defined -> treat as classic skill, always allowed
		return true;
	}

	var minEra = EraStringToNum( minEraName );
	return ( coreShardEra >= minEra );
}

/** @type { ( pKilled: Character, pKiller: CharOrNull ) => boolean } */
function onDeathBlow( pKilled, pKiller )
{
	let altar = CalcItemFromSer( parseInt( pKilled.GetTag( "championSpawnID" )));

	if( !ValidateObject( altar ))
		return true;

	if( pKilled.GetTag( "championBoss" ) == true )
	{
		if( !disableTopDamager )
		{
			RewardTopDamagers( pKilled, altar );
		}
		// Delay restart in 30 mins (1800000ms)
		altar.StartTimer( 1800000, 12, 7500 ); // Timer ID 12 = Auto-Restart

		TriggerEvent( 7500, "DelayedGoldExplosion", altar );
		return true;
	}

	var stage = parseInt( altar.GetTag( "spawnStage" ));
	var kills = parseInt( altar.GetTag( "killCount" ));
	var spawnData = TriggerEvent( 7500, "GetSpawnRankData", stage );
	var threshold = spawnData.maxKills;
	var redSkulls = parseInt( altar.GetTag( "redSkullCount" ));
	var whiteSkulls = parseInt( altar.GetTag( "whiteSkullCount" ));

	kills += 1;
	altar.SetTag( "killCount", kills );

	let progress = Math.floor(( kills * 100 ) / threshold);
	let expectedWhite = Math.floor( progress / 20 ); // 20%, 40%, 60%, 80%, 100%

	if( expectedWhite > whiteSkulls && expectedWhite < 5 )
	{
		altar.SetTag( "whiteSkullCount", expectedWhite );
		TriggerEvent( 7500, "PlaceWhiteSkulls", altar, kills, stage );
	}

	if( expectedWhite >= 5 || kills >= threshold )
	{
		// At 100% progress, gain 1 red skull
		altar.SetTag( "killCount", 0 );
		altar.SetTag( "whiteSkullCount", 0 );
		TriggerEvent( 7500, "RemoveWhiteSkulls", altar );

		altar.SetTag( "redSkullCount", redSkulls + 1 );
		TriggerEvent( 7500, "PlaceRedSkulls", altar, redSkulls + 1 );

		if( redSkulls >= 16 )
		{
			altar.KillJSTimer( 1, 7500 );
			// Clear everything for boss fight
			altar.KillJSTimer( 1, 7500 );
			TriggerEvent( 7500, "RemoveRedSkulls", altar );

			TriggerEvent( 7500, "SummonBoss", altar );
		}
		else
		{
			// Move to next stage ONLY every 4 red skulls (4,8,12)
			if(( redSkulls % 4) == 0 )
			{
				stage += 1;
				altar.SetTag( "spawnStage", stage );
			}
		}
	}

	return true;
}

/** @type { ( damaged: Character, attacker: CharOrNull, damageValue: number, damageType: WeatherType ) => boolean } */
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( !disableTopDamager || !ValidateObject( attacker ) || !ValidateObject( damaged ))
		return true;

	let attackerSerial = attacker.serial.toString();
	let mapRaw = damaged.GetTag( "damageMap" ) || "";
	let damageMap = {};

	let entries = mapRaw.split( ";" );
	for( let i = 0; i < entries.length; ++i )
	{
		let parts = entries[i].split( ":" );
		if( parts.length == 2 )
		{
			let ser = parts[0];
			let dmg = parseInt( parts[1] );
			if( !isNaN( dmg ))
				damageMap[ser] = dmg;
		}
	}

	if( !damageMap.hasOwnProperty( attackerSerial ))
		damageMap[attackerSerial] = 0;

	damageMap[attackerSerial] += damageValue;

	let topList = [];

	for( let ser in damageMap )
	{
		let dmg = damageMap[ser];

		let inserted = false;
		for( let j = 0; j < topList.length; ++j )
		{
			if( dmg > topList[j][1] )
			{
				topList.splice(j, 0, [ser, dmg] );
				inserted = true;
				break;
			}
		}

		if( !inserted && topList.length < 5 )
			topList.push([ser, dmg]);

		if( topList.length > 5 )
			topList.length = 5;
	}

	// Rebuild the damageMap tag
	let newTag = [];
	for( let i = 0; i < topList.length; ++i )
		newTag.push( topList[i][0] + ":" + topList[i][1] );

	damaged.SetTag("damageMap", newTag.join(";"));
	return true;
}

/** @type { ( pKilled: Character, altar: Item ) => void } */
function RewardTopDamagers( pKilled, altar )
{
	if( !ValidateObject( pKilled ))
	{
		return;
	}

	let damageMapRaw = pKilled.GetTag( "damageMap" ) || "";
	let entries = damageMapRaw.split( ";" );
	let top5 = [];

	// Manual insert-sort to build top 5 damagers
	for( let i = 0; i < entries.length; ++i )
	{
		let entry = entries[i].split(":");
		if( entry.length != 2)
			continue;

		let serial = parseInt( entry[0] );
		let damage = parseInt( entry[1] );
		if( isNaN( serial ) || isNaN( damage ))
			continue;

		let inserted = false;

		for( let j = 0; j < top5.length; ++j )
		{
			if( damage > top5[j][1] )
			{
				top5.splice(j, 0, [serial, damage]);
				inserted = true;
				break;
			}
		}

		if( !inserted && top5.length < 5 )
			top5.push( [serial, damage] );

		if( top5.length > 5 )
			top5.length = 5;
	}

	let champType = altar.GetTag( "championType" ) || 0;
	let spawnData = TriggerEvent( 7502, "ChampionSpawnData", champType );
	let rewards = spawnData.rewards || [];

	// Announce and reward
	if( top5.length > 0 )
	{
		let topDamgerSerial = top5[0][0];
		let topDamagerChar = CalcCharFromSer( topDamgerSerial );
		if( ValidateObject( topDamagerChar ))
		{
			altar.TextMessage( GetDictionaryEntry( 30000 ), topDamagerChar.name ); // The Champion has fallen to %i !

			if( topDamagerChar.socket != null )
			{
				topDamagerChar.socket.SysMessage( GetDictionaryEntry( 30001, topDamagerChar.socket.language )); // You dealt the most damage to the Champion!
			}
		}

		for( let i = 0; i < top5.length; ++i )
		{
			let topSerial = top5[i][0];
			let topDamages = top5[i][1];
			let topPlayer = CalcCharFromSer( topSerial );

			if( ValidateObject( topPlayer ))
			{
				if( topPlayer.socket != null)
				{
					topPlayer.socket.SysMessage( GetDictionaryEntry( 30002, topPlayer.socket.language ), topDamages ); // You were among the top 5 damagers! (%i damage)
					// Give reward item
					if (rewards.length > 0)
					{
						let randIndex = RandomNumber(0, rewards.length - 1);
						let rewardSection = rewards[randIndex];
						CreateDFNItem( topPlayer.socket, topPlayer, rewardSection, 1, "ITEM", true );
					}
				}
			}
		}

		// Normal rewards (weighted by Unique/Shared/Decorative)
		var uniqueGiven = false;

		for (var k = 0; k < top5.length; ++k)
		{
			var topSerial = top5[k][0];
			var topDamages = top5[k][1];
			var topPlayer = CalcCharFromSer( topSerial );

			if( ValidateObject( topPlayer ))
			{
				if( topPlayer.socket != null )
				{
					topPlayer.socket.SysMessage( GetDictionaryEntry( 30002, topPlayer.socket.language ), topDamages ); // You were among the top 5 damagers! (%i damage)
				}

				// Weighted category rewards
				uniqueGiven = GiveChampionStandardReward( topPlayer, spawnData, uniqueGiven );
			}
		}

		// Power scrolls to top N (Fel only)
		if( spawnData && spawnData.powerScrollSkills && spawnData.powerScrollSkills.length > 0 )
		{
			var maxPS = Math.min( powerScrollMaxReceivers, top5.length );
			for( var idx = 0; idx < maxPS; ++idx )
			{
				var powerScrollSerial = top5[idx][0];
				var powerScrolPlayer = CalcCharFromSer( powerScrollSerial );
				if( ValidateObject( powerScrolPlayer ))
					GiveChampionPowerScroll( powerScrolPlayer, spawnData, altar );
			}
		}
	}
	else
	{
		altar.TextMessage( GetDictionaryEntry( 30003 )); // The Champion has fallen!
	}
}

/** @type { () => number } */
function RollPowerScrollBonus()
{
	var total = 0;
	for( var i = 0; i < powerScrollRarityTable.length; ++i )
	{
		total += powerScrollRarityTable[i].weight;
	}

	var roll = RandomNumber( 0, total - 1 );
	var accum = 0;

	for( var j = 0; j < powerScrollRarityTable.length; ++j )
	{
		accum += powerScrollRarityTable[j].weight;
		if( roll < accum )
			return powerScrollRarityTable[j].bonus;
	}
	return 5;
}

/** @type { ( spawnData: any ) => string | null } */
function RollPowerScrollSkill( spawnData )
{
	if( !spawnData || !spawnData.powerScrollSkills || spawnData.powerScrollSkills.length === 0 )
		return null;

	var list = spawnData.powerScrollSkills;
	var allowed = [];

	// Filter list by era
	for( var i = 0; i < list.length; ++i )
	{
		var skillProp = list[i];
		if( IsSkillAllowedByEra( skillProp ))
			allowed.push( skillProp );
	}

	if( allowed.length === 0 )
		return null;

	var idx = RandomNumber( 0, allowed.length - 1 );
	return allowed[idx];
}

// DFN section name: powerscroll_<skill>_<bonus>
/** @type { ( spawnData: any ) => string | null } */
function RollPowerScrollSection( spawnData )
{
	var skillProp = RollPowerScrollSkill( spawnData );
	if( !skillProp )
		return null;

	var bonus = RollPowerScrollBonus();
	return "powerscroll_" + skillProp + "_" + bonus;
}

// Give one power scroll to player (Fel-only)
/** @type { ( player: Character, spawnData: any, altar: Item ) => void } */
function GiveChampionPowerScroll( player, spawnData, altar )
{
	if( !ValidateObject( player ) || !spawnData || !ValidateObject( altar ))
		return;

	var facet = altar.worldnumber;
	if( facet !== 0 )
		return;

	var sectionID = RollPowerScrollSection( spawnData );
	if( !sectionID )
		return;

	if (player.socket != null)
	{
		var scroll = CreateDFNItem( player.socket, player, sectionID, 1, "ITEM", true );
		if( ValidateObject( scroll ))
		{
			scroll.Refresh();
			player.socket.SysMessage( GetDictionaryEntry( 30002, player.socket.language ));// "You have been rewarded with a Scroll of Power!" 
		}
	}
}

/** @type { ( spawnData: any, key: string ) => string[] } */
function RewardListForCategory( spawnData, key )
{
	if( !spawnData )
	{
		return [];
	}

	var list = spawnData[key];
	if( !list || !list.length )
	{
		return [];
	}
	return list;
}

/** @type { ( spawnData: any, uniqueAlreadyGiven: boolean ) => ("uniqueList"|"sharedList"|"decorativeList") | null } */
function RollChampionRewardCategory( spawnData, uniqueAlreadyGiven )
{
	var candidates = [];
	for( var i = 0; i < championRewardTable.length; ++i )
	{
		var entry = championRewardTable[i];
		if( entry.key === "uniqueList" && uniqueAlreadyGiven )
			continue;

		var list = RewardListForCategory( spawnData, entry.key );
		if( list.length === 0 )
			continue;

		candidates.push( entry );
	}

	if( candidates.length === 0 )
		return null;

	var total = 0;
	for( var j = 0; j < candidates.length; ++j )
	{
		total += candidates[j].weight;
	}

	var roll = RandomNumber( 0, total - 1 );
	var accum = 0;

	for( var k = 0; k < candidates.length; ++k )
	{
		accum += candidates[k].weight;
		if (roll < accum)
			return candidates[k].key;
	}
	return null;
}

/** @typedef {{ section: string, category: "uniqueList"|"sharedList"|"decorativeList" }} ChampionRewardPick */
/** @type { ( spawnData: any, uniqueAlreadyGiven: boolean ) => (string | ChampionRewardPick | null) } */
function RollChampionRewardSection( spawnData, uniqueAlreadyGiven )
{
	var category = RollChampionRewardCategory( spawnData, uniqueAlreadyGiven );
	if( !category )
	{
		if( spawnData && spawnData.rewards && spawnData.rewards.length > 0 )
		{
			var idxLegacy = RandomNumber( 0, spawnData.rewards.length - 1 );
			return spawnData.rewards[idxLegacy];
		}
		return null;
	}

	var pool = RewardListForCategory( spawnData, category );
	if( pool.length === 0 )
		return null;

	var idx = RandomNumber( 0, pool.length - 1 );
	return { section: pool[idx], category: category };
}

/** @type { ( player: Character, spawnData: any, uniqueAlreadyGiven: boolean ) => boolean } */
function GiveChampionStandardReward( player, spawnData, uniqueAlreadyGiven )
{
	if( !ValidateObject( player ) || !spawnData )
		return uniqueAlreadyGiven;

	var result = RollChampionRewardSection( spawnData, uniqueAlreadyGiven );
	if( !result)
		return uniqueAlreadyGiven;

	var sectionID, category;
	if( typeof result === "string" )
	{
		sectionID = result;
		category = "Legacy";
	}
	else
	{
		sectionID = result.section;
		category = result.category;
	}

	var item = CreateDFNItem( player.socket, player, sectionID, 1, "ITEM", true );
	if( ValidateObject( item ))
	{
		item.Refresh();
	}

	if( category === "uniqueList" )
		return true;

	return uniqueAlreadyGiven;
}
