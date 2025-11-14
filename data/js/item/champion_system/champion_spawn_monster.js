const disableTopDamager = true; // Set to true to disable top damager logic and if so then all loot is free for players to fight over.
// Who can get scrolls at most?
const POWER_SCROLL_MAX_RECEIVERS = 6;

// Rarity weights (higher bonus = rarer; total should sum up to something sensible)
const POWER_SCROLL_RARITY_TABLE = [
	{ bonus: 5,  weight: 60 },  // Wonderous (+5)
	{ bonus: 10, weight: 25 },  // Exalted (+10)
	{ bonus: 15, weight: 10 },  // Mythical (+15)
	{ bonus: 20, weight: 5 }    // Legendary (+20)
	//{ bonus: 25, weight: 1 }    // Ultima (+25)
];

const CHAMPION_REWARD_CATEGORY_TABLE = [
	{ key: "UniqueList",     weight: 5 },   // rare
	{ key: "SharedList",     weight: 25 },  // common
	{ key: "DecorativeList", weight: 10 }   // uncommon
];

var coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ));

// Minimum era per skill. If not listed here, skill is assumed pre-AoS and
// always allowed.
var SKILL_MIN_ERA = {
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

function isSkillAllowedByEra( skillProp )
{
	var minEraName = SKILL_MIN_ERA[skillProp];
	if( !minEraName )
	{
		// No min era defined -> treat as classic skill, always allowed
		return true;
	}

	var minEra = EraStringToNum( minEraName );
	return ( coreShardEra >= minEra );
}

/** @type { ( mKilled: Character, mKiller: Character ) => boolean } */
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

/** @type { ( damaged: Character, attacker: Character, damageValue: number, damageType: WeatherType ) => boolean } */
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( !disableTopDamager || !ValidateObject( attacker ) || !ValidateObject( damaged ))
		return true;

	let attackerSerial = attacker.serial.toString();
	let mapRaw = damaged.GetTag( "damageMap" ) || "";
	let damageMap = {};

	// Parse the existing tag into a map
	let entries = mapRaw.split(";");
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

	// Add or update attacker damage
	if( !damageMap.hasOwnProperty( attackerSerial ))
		damageMap[attackerSerial] = 0;

	damageMap[attackerSerial] += damageValue;

	// Manually build top 5 without using sort
	let topList = [];

	for( let ser in damageMap )
	{
		let dmg = damageMap[ser];

		// Insert into topList in descending order
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

		// Trim if too long
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


function RewardTopDamagers( pKilled, altar )
{
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
		let topSerial = top5[0][0];
		let topChar = CalcCharFromSer( topSerial );
		if( ValidateObject( topChar ))
		{
			altar.TextMessage( "The Champion has fallen to " + topChar.name + "!" );
			topChar.SysMessage( "You dealt the most damage to the Champion!" );
		}

		for( let i = 0; i < top5.length; ++i )
		{
			let serial = top5[i][0];
			let damage = top5[i][1];
			let player = CalcCharFromSer( serial );

			if( ValidateObject( player ))
			{
				player.SysMessage( "You were among the top 5 damagers! (" + damage + " damage)" );
				// Give reward item
				if( rewards.length > 0 )
				{
					let randIndex = RandomNumber( 0, rewards.length - 1 );
					let rewardSection = rewards[randIndex];
					CreateDFNItem( player.socket, player, rewardSection, 1, "ITEM", true );
				}
			}
		}

		// Normal rewards (weighted by Unique/Shared/Decorative)
		var uniqueGiven = false;

		for (var k = 0; k < top5.length; ++k)
		{
			var serial = top5[k][0];
			var damage = top5[k][1];
			var player = CalcCharFromSer(serial);

			if (ValidateObject(player))
			{
				player.SysMessage("You were among the top 5 damagers! (" + damage + " damage)");

				// Weighted category rewards
				uniqueGiven = giveChampionStandardReward(player, spawnData, uniqueGiven);
			}
		}

		// Power scrolls to top N (Fel only)
		if( spawnData && spawnData.powerScrollSkills && spawnData.powerScrollSkills.length > 0 )
		{
			var maxPS = Math.min( POWER_SCROLL_MAX_RECEIVERS, top5.length );
			for( var idx = 0; idx < maxPS; ++idx )
			{
				var psSerial = top5[idx][0];
				var psPlayer = CalcCharFromSer( psSerial );
				if( ValidateObject( psPlayer ))
					giveChampionPowerScroll( psPlayer, spawnData, altar );
			}
		}
	}
	else
	{
		altar.TextMessage( "The Champion has fallen!" );
	}
}

function rollPowerScrollBonus()
{
	var total = 0;
	for( var i = 0; i < POWER_SCROLL_RARITY_TABLE.length; ++i )
		total += POWER_SCROLL_RARITY_TABLE[i].weight;

	var roll = RandomNumber( 0, total - 1 );
	var accum = 0;

	for( var j = 0; j < POWER_SCROLL_RARITY_TABLE.length; ++j )
	{
		accum += POWER_SCROLL_RARITY_TABLE[j].weight;
		if( roll < accum )
			return POWER_SCROLL_RARITY_TABLE[j].bonus;
	}
	return 5;
}

function rollPowerScrollSkill( spawnData )
{
	if( !spawnData || !spawnData.powerScrollSkills || spawnData.powerScrollSkills.length === 0 )
		return null;

	var list = spawnData.powerScrollSkills;
	var allowed = [];

	// Filter list by era
	for( var i = 0; i < list.length; ++i )
	{
		var skillProp = list[i];
		if( isSkillAllowedByEra( skillProp ))
			allowed.push( skillProp );
	}

	// If nothing is allowed for this era, no scroll will be generated
	if( allowed.length === 0 )
		return null;

	var idx = RandomNumber( 0, allowed.length - 1 );
	return allowed[idx];
}

// DFN section name: powerscroll_<skill>_<bonus>
function rollPowerScrollSection( spawnData )
{
	var skillProp = rollPowerScrollSkill( spawnData );
	if( !skillProp )
		return null;

	var bonus = rollPowerScrollBonus();
	return "powerscroll_" + skillProp + "_" + bonus;
}

// Give one power scroll to player (Fel-only)
function giveChampionPowerScroll( player, spawnData, altar )
{
	if( !ValidateObject( player ) || !spawnData || !ValidateObject( altar ))
		return;

	var facet = altar.worldnumber;
	if( facet !== 0 )
		return;

	var sectionID = rollPowerScrollSection( spawnData );
	if( !sectionID )
		return;

	var scroll = CreateDFNItem( player.socket, player, sectionID, 1, "ITEM", true) ;
	if( ValidateObject( scroll ))
	{
		scroll.Refresh();
		player.SysMessage( "You have been rewarded with a Scroll of Power!" );
	}
}

function RewardListForCategory( spawnData, key )
{
	if( !spawnData )
		return [];
	var list = spawnData[key];
	if( !list || !list.length )
		return [];
	return list;
}

// Roll a category, respecting:
// - weights above
// - only categories that actually have items
// - UniqueList only once per champ kill (via uniqueAlreadyGiven flag)
function rollChampionRewardCategory( spawnData, uniqueAlreadyGiven )
{
	var candidates = [];
	for( var i = 0; i < CHAMPION_REWARD_CATEGORY_TABLE.length; ++i )
	{
		var entry = CHAMPION_REWARD_CATEGORY_TABLE[i];
		if( entry.key === "UniqueList" && uniqueAlreadyGiven )
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
		total += candidates[j].weight;

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

// Roll a concrete DFN section for a reward (using the category logic)
function rollChampionRewardSection( spawnData, uniqueAlreadyGiven )
{
	var category = rollChampionRewardCategory( spawnData, uniqueAlreadyGiven );
	if( !category )
	{
		// fallback to legacy rewards list if all new lists are empty
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

// Give a normal champion reward to player, returns updated unique flag
function giveChampionStandardReward( player, spawnData, uniqueAlreadyGiven )
{
	if( !ValidateObject(player) || !spawnData )
		return uniqueAlreadyGiven;

	var result = rollChampionRewardSection( spawnData, uniqueAlreadyGiven );
	if( !result)
		return uniqueAlreadyGiven;

	var sectionID, category;
	if( typeof result === "string" )
	{
		// fallback legacy result
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

		// Optional debug / flavor:
		player.SysMessage("You receive a " + category + " champion reward.");
	}

	if( category === "UniqueList" )
		return true; // mark unique as given

	return uniqueAlreadyGiven;
}