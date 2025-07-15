const disableTopDamager = true; // Set to true to disable top damager logic and if so then aall loot is free for players to fight over.

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

		TriggerEvent( 7500, "DelayedGoldExplosion", altar );
		return true;
	}

	var stage = parseInt( altar.GetTag( "spawnStage" )) || 1;
	var kills = parseInt( altar.GetTag( "killCount" )) || 0;
	var spawnData = TriggerEvent( 7500, "GetSpawnRankData", stage );
	var threshold = spawnData.maxKills;
	var redSkulls = parseInt( altar.GetTag( "redSkullCount" )) || 1;
	var whiteSkulls = parseInt( altar.GetTag( "whiteSkullCount" )) || 0;

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
		newTag.push(topList[i][0] + ":" + topList[i][1]);

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
		if (entry.length != 2)
			continue;

		let serial = parseInt(entry[0]);
		let damage = parseInt(entry[1]);
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

	let champType = altar.GetTag("championType") || "Unknown";
	let spawnData = TriggerEvent(7502, "ChampionSpawnData", champType);
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
				if (rewards.length > 0)
				{
					let randIndex = RandomNumber(0, rewards.length - 1);
					let rewardSection = rewards[randIndex];
					CreateDFNItem( player.socket, player, rewardSection, 1, "ITEM", true );
				}
			}
		}
	}
	else
	{
		altar.TextMessage("The Champion has fallen!");
	}
}