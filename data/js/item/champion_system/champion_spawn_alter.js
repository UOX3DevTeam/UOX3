// Champion spawn difficulty scaling
var spawnEditorTooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day
const rankBreaks = [2, 3, 4]; // Level Rank mapping breakpoints
const maxKillsByRank = [256, 128, 64, 32]; // Max kills per rank
const radiusMods = [1.0, 0.75, 0.5, 0.25]; // Radius scaling per rank
const baseSpawnRadius = 30; // Max radius at rank 0
//Gold shower settings
const piles = 50; // Number of gold piles to spawn
const range = 25; // Range around the altar to spawn gold piles
const ChampionIDToName = {
	1: "Abyss",
	2: "Arachnid",
	3: "Cold",
	4: "Forest",
	5: "Unholy",
	6: "Vermin"
};

const ChampionNameToID = {
	"Abyss": 1,
	"Arachnid": 2,
	"Cold": 3,
	"Forest": 4,
	"Unholy": 5,
	"Vermin": 6
};

function onCreateDFN( objMade, objType )
{
	objMade.StartTimer( 5000, 13, 7500 );
}

function onUseChecked( pUser, altar )
{
	var socket = pUser.socket;
	socket.tempObj = altar;

	if( !ValidateObject( altar ) || !ValidateObject( pUser ))
		return false;

	ChampionMenu( pUser, altar )
}

function StartChampionWave( altar, stage )
{
	let spawnCount = AreaCharacterFunction( "ChampionSpawnNpc", altar, 60, null );
	let rankData = GetSpawnRankData( stage );
	let radius = rankData.spawnRadius;
	let maxSpawn = altar.morex;

	if( spawnCount >= maxSpawn )
		return;

	let spawnAmount = maxSpawn - spawnCount;
	let triesPerSpawn = 50; // How many times to retry finding valid ground

	for( let i = 0; i < spawnAmount; ++i )
	{
		let validSpotFound = false;
		let x = 0, y = 0, z = 0;

		for( let t = 0; t < triesPerSpawn && !validSpotFound; ++t )
		{
			x = altar.x + RandomNumber( -radius, radius );
			y = altar.y + RandomNumber( -radius, radius);
			z = GetMapElevation( x, y, altar.worldNumber ); // Use actual terrain Z

			let mapBlocked    = DoesMapBlock( x, y, z, altar.worldNumber, false, false, true, false );
			let staticBlocked = DoesStaticBlock( x, y, z, altar.worldNumber, false );
			let dynBlocked    = DoesDynamicBlock( x, y, z, altar.worldNumber, altar.instanceID, false, false, false, false );
			let isWet         = CheckStaticFlag(x, y, z, altar.worldNumber, 7 ); // TF_WET
			let staticStand   = CheckStaticFlag( x, y, z, altar.worldNumber, 20 );  // TF_MAP

			if (!mapBlocked && !staticBlocked && !dynBlocked && !isWet && !staticStand)
			{
				validSpotFound = true;
			}
		}

		if( !validSpotFound )
		{
			continue;
		}

		let champID = parseInt( altar.GetTag( "championType" )) || 0;
		if( !champID )
			return;

		let spawnData = TriggerEvent( 7503, "ChampionSpawnData", champID );
		if( !spawnData )
			return;

		let npcList = spawnData.levels[stage - 1];
		let chosenType = npcList[RandomNumber( 0, npcList.length - 1 )];

		let npc = SpawnNPC( chosenType, x, y, z, altar.worldNumber, altar.instanceID, false );

		if( ValidateObject( npc ))
		{
			npc.SetTag( "championSpawnID", altar.serial.toString() );
			npc.AddScriptTrigger( 7501 );
			npc.Wander( x, y, 10 );
		}
	}
}

function ChampionSpawnNpc( alter, spawn, socket )
{
	if( spawn.GetTag( "championSpawnID" ) == alter.serial.toString() )
	{
		return true;
	}
}

function SummonBoss( altar )
{
	if( !ValidateObject( altar ))
		return;

	let champID = altar.GetTag( "championType" ) || 0;
	if( isNaN( champID ))
	{
		return;
	}
	let spawnData = TriggerEvent( 7502, "ChampionSpawnData", champID );
	let bossID = spawnData.boss;

	// Spawn boss in the center of the platform
	let x = altar.x;
	let y = altar.y;
	let z = altar.z;
	let worldNum = altar.worldNumber;
	let instID = altar.instanceID;

	let boss = SpawnNPC( bossID, x, y, z, worldNum, instID, false );

	if( !ValidateObject( boss ))
	{
		altar.TextMessage( "Failed to summon the Champion Boss: " + bossID );
		return;
	}

	boss.SetTag( "championBoss", true ); // mark it
	boss.SetTag( "championSpawnID", altar.serial.toString() );
	boss.AddScriptTrigger( 7501 );
	boss.TextMessage( "You dare challenge me?!" );
	
	// Optional: Spawn Mini-Boss if defined
	if( spawnData.hasOwnProperty( "miniBoss" ))
	{
		let miniID = spawnData.miniBoss;
		let mx = x + 2;
		let my = y + 2;
		let mz = z;

		let mini = SpawnNPC( miniID, mx, my, mz, worldNum, instID, false );
		if( ValidateObject( mini ))
		{
			mini.SetTag( "championMiniBoss", true);
			mini.SetTag( "championSpawnID", altar.serial.toString());
			mini.AddScriptTrigger( 7501 );
			mini.TextMessage( "You face me as well!" );
		}
		else
		{
			altar.TextMessage( "Mini-boss failed to appear: " + miniID );
		}
	}
}

function CheckForNearbyPlayers( altar )
{
	return AreaCharacterFunction( "IsPlayerNearby", altar, 20, null );
}

function IsPlayerNearby( src, player, sock )
{
	if( !ValidateObject( player ))
		return false;

	// Skip if it's an item, NPC, or non-character object
	if( player.isItem || ( player.isChar && player.npc ))
		return false;

	// Must be a real player (not dead, not GM/Counselor, and online)
	if( player.online && !player.dead && !player.isGM && !player.isCounselor )
		return true;
}

function onTimer( altar, timerID )
{
	if( !ValidateObject( altar ))
		return;

	switch( timerID )
	{
		case 1:
			if( altar.GetTag( "spawnActive" ))
			{
				let stage = parseInt( altar.GetTag( "spawnStage" ));
				let kills = parseInt( altar.GetTag( "killCount" ));
				let data = GetSpawnRankData( stage );
				let threshold = data.maxKills;

				if( kills < threshold )
					StartChampionWave( altar, stage );

				altar.StartTimer( 30000, 1, 7500 );
			}
			break;
		case 2:
			ChampionGoldExplosion( altar );
			break;
		case 10:
			DecayChampionProgressIfEmpty( altar );
			altar.StartTimer( 600000, 10, 7500 );
			break;
		case 12: // Auto-Restart Champion Spawn
			{
				// Reset state
				altar.SetTag( "spawnActive", 1 );
				altar.SetTag( "killCount", 0 );
				altar.SetTag( "spawnStage", 1 );
				altar.SetTag( "redSkullCount", 1 );
				altar.SetTag( "whiteSkullCount", 0 );

				// Visuals
				PlaceRedSkulls( altar, 1 );
				PlaceWhiteSkulls( altar, 0, 1 );

				// Start wave and timers
				StartChampionWave( altar, 1 );
				altar.StartTimer( 30000, 1, 7500 );
				altar.StartTimer( 600000, 10, 7500 );
				break;
			}
		case 13: // Champion Spawn with OnCreateDFN
			{
				if(altar.x == 5178 && altar.y == 708 )
				{
					altar.SetTag("championType", 5); // Unholy
				}
				else if(altar.x == 5557 && altar.y == 824 )
				{
					altar.SetTag("championType", 6); //Vermin
				}
				else if(altar.x == 5259 && altar.y == 837 )
				{
					altar.SetTag("championType", 3);//Cold
				}
				else if(altar.x == 5814 && altar.y == 1350 )
				{
					altar.SetTag("championType", 1);//Abyss
				}
				else if(altar.x == 5190 && altar.y == 1605 )
				{
					altar.SetTag("championType", 2);//Arachnid
				}
				else if(altar.x == 5559 && altar.y == 3757 )
				{
					altar.SetTag("championType", 4);//Forest
				}
				else
				{
					return;
				}

				// Reset state
				altar.SetTag( "spawnActive", 1 );
				altar.SetTag( "killCount", 0 );
				altar.SetTag( "spawnStage", 1 );
				altar.SetTag( "redSkullCount", 1 );
				altar.SetTag( "whiteSkullCount", 0 );

				// Visuals
				PlaceRedSkulls( altar, 1 );
				PlaceWhiteSkulls( altar, 0, 1 );

				// Start wave and timers
				StartChampionWave( altar, 1 );
				altar.StartTimer( 30000, 1, 7500 );
				altar.StartTimer( 600000, 10, 7500 );
				break;
			}
	}
}

function DecayChampionProgressIfEmpty( altar )
{
	if( !ValidateObject( altar ))
		return;

	// If players are nearby, no decay
	if( CheckForNearbyPlayers( altar ) > 0)
		return;

	let stage = parseInt( altar.GetTag( "spawnStage" )) || 1;
	let redSkulls = parseInt( altar.GetTag( "redSkullCount" )) || 1;
	let whiteSkulls = parseInt( altar.GetTag( "whiteSkullCount" )) || 0;
	let kills = parseInt( altar.GetTag( "killCount" )) || 0;

	let changed = false;

	// Slowly reduce kills over time
	if( kills > 0 )
	{
		kills = Math.max(0, kills - 5); // Decay by 5 per tick
		altar.SetTag("killCount", kills);

		let data = GetSpawnRankData(stage);
		let threshold = data.maxKills;
		let skullCount = Math.min(4, Math.floor((kills / threshold) * 100 / 20));
		altar.SetTag( "whiteSkullCount", skullCount );
		TriggerEvent( 7500, "PlaceWhiteSkulls", altar, kills, stage );

		altar.TextMessage( "The shrine's energy wanes with no challengers present..." );
		changed = true;
	}

	// Decay white skulls first if any
	if( whiteSkulls > 0 )
	{
		whiteSkulls -= 1;
		altar.SetTag( "whiteSkullCount", whiteSkulls );
		TriggerEvent( 7500, "PlaceWhiteSkulls", altar, kills, stage );
		changed = true;
	}
	// Then decay red skulls and stage
	else if( redSkulls > 1 ) // Don't remove last red skull (stage 1)
	{
		redSkulls -= 1;
		stage = Math.max( 1, stage - 1 );
		altar.SetTag( "redSkullCount", redSkulls );
		altar.SetTag( "spawnStage", stage );
		TriggerEvent( 7500, "PlaceRedSkulls", altar, redSkulls );
		changed = true;
	}

	// Reset killCount again just to be safe
	if( changed && kills == 0 )
		altar.SetTag( "killCount", 0 );
}

function RankForLevel( level )
{
	for( var i = 0; i < rankBreaks.length; ++i )
	{
		if ( level <= rankBreaks[i] )
			return i;
	}
	return rankBreaks.length;
}

function GetSpawnRankData( level )
{
	var ranks = RankForLevel( level );
	return {
		rank: ranks,
		maxKills: maxKillsByRank[ranks],
		spawnRadius: Math.floor( baseSpawnRadius * radiusMods[ranks] )
	};
}

function RemoveSpawn( srcChar, trgChar, pSock )
{
	var altar = pSock.tempObj;
	if( trgChar.GetTag( "championSpawnID" ) == altar.serial )
	{
		trgChar.Delete();
	}
	return true;
}

function GetRedSkullPosition(altar, index)
{
	const positions = [[-2, -2], [-1, -2], [0, -2], [1, -2], [2, -2], [2, -1], [2, 0], [2, 1],
		[2, 2], [1, 2], [0, 2], [-1, 2], [-2, 2], [-2, 1], [-2, 0], [-2, -1]];
	let offset = positions[index] || [0, 0];
	let z = GetMapElevation(altar.x + offset[0], altar.y + offset[1], altar.worldNumber );
	return { x: altar.x + offset[0], y: altar.y + offset[1], z: z + 5 };
}

function GetWhiteSkullPosition(altar, index)
{
	const offsets = [[-1, -1], [1, -1], [1, 1], [-1, 1]];
	let offset = offsets[index % offsets.length];
	let z = GetMapElevation(altar.x + offset[0], altar.y + offset[1], altar.worldNumber );
	return { x: altar.x + offset[0], y: altar.y + offset[1], z: z + 5 };
}

function RemoveRedSkullsFunc( src, item )
{
	if( item.GetTag( "redSkull" ) == true )
	{
		item.Delete();
		return true;
	}
	return false;
}

function RemoveRedSkulls( srcAltar )
{
	return AreaItemFunction( "RemoveRedSkullsFunc", srcAltar, 10, null );
}

function RemoveWhiteSkullsFunc( src, item, pSock )
{
	if( item.GetTag( "whiteSkull" ) == true )
	{
		item.Delete();
		return true;
	}
	return false;
}

function RemoveWhiteSkulls( srcAltar )
{
	return AreaItemFunction( "RemoveWhiteSkullsFunc", srcAltar, 10, null );
}

function PlaceRedSkulls( altar, stage )
{
	RemoveRedSkulls( altar );
	for (let i = 0; i < stage; ++i )
	{
		let pos = GetRedSkullPosition( altar, i );
		let skull =  CreateDFNItem( null, null, "0x1854", 1, "ITEM", false, 0x26, altar.worldnumber, altar.instanceID );
		skull.x = pos.x;
		skull.y = pos.y;
		skull.z = pos.z;
		skull.movable = 2;
		skull.dir = 2;
		skull.decayable = false;
		skull.SetTag( "redSkull", true );
	}
	altar.Refresh();
}

function PlaceWhiteSkulls( altar, killCount, stage )
{
	RemoveWhiteSkulls( altar );
	let data = GetSpawnRankData( stage );
	let threshold = data.maxKills;
	let percent = Math.floor(( killCount / threshold ) * 100 );
	let skullCount = Math.min( 4, Math.floor( percent / 20 ));

	for (let i = 0; i < skullCount; ++i )
	{
		let pos = GetWhiteSkullPosition( altar, i );
		let skull = CreateDFNItem( null, null, "0x1854", 1, "ITEM", false, 0, altar.worldnumber, altar.instanceID );
		skull.x = pos.x;
		skull.y = pos.y;
		skull.z = pos.z + 3;
		skull.movable = 2;
		skull.dir = 1;
		skull.decayable = false;
		skull.SetTag( "whiteSkull", true );
		altar.SetTag( "whiteSkullCount", skullCount );
	}
	altar.Refresh();
}

function DelayedGoldExplosion(altar)
{
	if (!ValidateObject(altar))
		return;

	altar.StartTimer(1000, 2, 7500); //1 second delay, timerID 2
}

function ChampionGoldExplosion( altar )
{
	if( !ValidateObject( altar ))
		return;

	var minAmount = altar.morey;
	var maxAmount = altar.morez;

	const world = altar.worldNumber;
	const instance = altar.instanceID;

	let centerX = altar.x;
	let centerY = altar.y;
	let centerZ = altar.z;

	for( let i = 0; i < piles; ++i )
	{
		let offsetX = RandomNumber( -range, range );
		let offsetY = RandomNumber( -range, range );
		let x = centerX + offsetX;
		let y = centerY + offsetY;
		let z = GetMapElevation( x, y, world );

		let amount = RandomNumber( minAmount, maxAmount );
		let gold = CreateDFNItem( null, null, "0x0eed", amount, "ITEM", false, 0, world, instance );
		gold.x = x;
		gold.y = y;
		gold.z = z;
		if( !ValidateObject( gold ))
			continue;

		// Effects
		let fx = RandomNumber( 0, 2 );
		switch( fx )
		{
			case 0: // Fire column
				DoStaticEffect( x, y, z, 0x3709, 0x3, 0x10, false );
				altar.SoundEffect( 0x208, true );
				break;
			case 1: // Explosion
				DoStaticEffect( x, y, z, 0x3709, 0x3, 0x10, false );
				altar.SoundEffect( 0x307, true );
				break;
			case 2: // Ball of fire
				DoStaticEffect( x, y, z, 0x36FE, 0x3, 0x10, false );
				break;
		}
	}
}

function ChampionMenu( socket, altar )
{
	var champalter = new Gump;

	champalter.AddPage( 0 );

	// Main Background and Header
	champalter.AddBackground( 0, 0, 360, 295, 5054 );
	champalter.AddCheckerTrans( 0, 0, 360, 295 );
	champalter.AddHTMLGump(15, 7, 320, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>Champion Spawn Menu</BASEFONT></BIG></CENTER>" );// Champion Spawn Menu
	champalter.AddButton( 320, 1, 4017, 4018, 1, 0, 0 ); // Close Menu Button
	champalter.AddPicture( 50, 10, 7956 ); // Image of a "spawn rune"

	// Champion Alter Name Name
	champalter.AddHTMLGump( 20, 35, 140, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9801, socket.language ) + ":</basefont >" );// Spawner Name
	champalter.AddBackground( 115, 30, 235, 28, 5120 ); // Spawner Name Background

	// Champion Type
	champalter.AddHTMLGump( 20, 68, 140, 22, false, false, "<basefont color=#ffffff>Champion Type:</basefont>" );
	const championTypes = ["Abyss", "Arachnid", "Cold", "Forest", "Unholy", "Vermin"];
	let currentID = parseInt( altar.GetTag( "championType" )) || 0;

	let champY = 100;
	const columnsPerRow = 3;

	champalter.AddGroup(2); // Radio group 2 (avoid clashing)
	for (let i = 0; i < championTypes.length; ++i)
	{
		let name = championTypes[i];
		let id = ChampionNameToID[name];
		let label = "<basefont color=#00ff00><big>" + name + "</big></basefont>";
		let isSelected = ( currentID == id ) ? 1 : 0;

		let x = 20 + ( i % columnsPerRow ) * 100;
		let y = champY + Math.floor( i / columnsPerRow ) * 30;

		champalter.AddHTMLGump( x + 35, y, 100, 22, false, false, label );
		champalter.AddRadio( x, y, 2472, 2153, isSelected, id );
	}
	champalter.EndGroup();

	// Amount
	champalter.AddHTMLGump( 35, 205, 120, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9811, socket.language ) + ":</basefont>" );// Amount
	champalter.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9812, socket.language ));// Maximum amount to respawn
	champalter.AddBackground( 30, 230, 50, 28, 5120 );

	// Min / Max Time
	champalter.AddHTMLGump( 105, 205, 140, 22, false, false, "<basefont color=#ffffff>Min Gold:</basefont>" );
	champalter.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9814, socket.language ));// Minimum Gold Shower to Spawn
	champalter.AddHTMLGump( 180, 205, 140, 22, false, false, "<basefont color=#ffffff>Max Gold:</basefont>" );
	champalter.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9816, socket.language ));// Maximum Gold Shower to Spawn
	champalter.AddBackground( 105, 230, 50, 28, 5120 );
	champalter.AddBackground( 185, 230, 50, 28, 5120 );

	// Apply Changes!
	champalter.AddButton( 300, 230, 2122, 2124, 1, 0, 1 );
	// Enable  and Disable Champion Spawn
	if( altar.GetTag( "spawnActive" ) == 1 )
	{
		champalter.AddButton( 230, 265, 4005, 4007, 1, 0, 3 );
		champalter.AddHTMLGump( 265, 267, 140, 22, false, false, "<basefont color=#ffffff>Disable Champion</basefont>" );
	}
	else
	{
		champalter.AddButton( 230, 265, 4005, 4007, 1, 0, 2 );
		champalter.AddHTMLGump( 265, 267, 140, 22, false, false, "<basefont color=#ffffff>Enable Champion</basefont>" );
	}

	// Text Entry fields
	champalter.AddTextEntryLimited( 125, 35, 200, 25, 1153, 0, 13, altar.name, 30 );
	champalter.AddTextEntryLimited( 45, 235, 115, 20, 1153, 0, 14, altar.morex, 2 );
	champalter.AddTextEntryLimited( 115, 235, 40, 20, 1153, 0, 15, altar.morey, 4 );
	champalter.AddTextEntryLimited( 195, 235, 40, 20, 1153, 0, 16, altar.morez, 4 );

	champalter.Send( socket );
	champalter.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var altar = socket.tempObj;

	// Fetch data from text-fields
	var spawnerName = gumpData.getEdit( 0 );
	var spawnAmount = parseInt( gumpData.getEdit( 1 ));
	var minGold = parseInt( gumpData.getEdit( 2 ));
	var maxGold = parseInt( gumpData.getEdit( 3 ));

	// Fetch data from radio/checkbox-buttons
	var radiobtnGroup1 = gumpData.getButton( 0 );
	switch( pButton )
	{
		case 0: // Close gump, no changes
			break;
		case 1:
			// This assumes radiobtnGroup1 now returns the actual numeric ID (1–6)
			if (ChampionIDToName.hasOwnProperty(radiobtnGroup1))
			{
				altar.SetTag("championType", radiobtnGroup1);
			}

			// Update spawner properties
			if( minGold != altar.morey )
			{
				altar.morey = minGold;
				let tmpMsg = GetDictionaryEntry( 9825, socket.language ); // Min Gold updated to %i
				altar.TextMessage( tmpMsg.replace( /%i/gi, minGold.toString()), false, 0x3b2, 0, pUser.serial );
			}
			if( maxGold != altar.morez )
			{
				altar.morez = maxGold;
				let tmpMsg = GetDictionaryEntry( 9826, socket.language ); // Max Gold updated to %i
				altar.TextMessage( tmpMsg.replace( /%i/gi, maxGold.toString()), false, 0x3b2, 0, pUser.serial );
			}
			if( spawnAmount != altar.morex )
			{
				altar.morex = spawnAmount;
				let tmpMsg = GetDictionaryEntry( 9827, socket.language ); // Amount updated to %i
				altar.TextMessage( tmpMsg.replace( /%i/gi, spawnAmount.toString()), false, 0x3b2, 0, pUser.serial );
			}

			// Update spawner name
			if( spawnerName == null || spawnerName == "" )
			{
				socket.SysMessage( GetDictionaryEntry( 9270, socket.language )); // That name is too short, or no name was entered.
			}
			else if( spawnerName.length > 50 )
			{
				socket.SysMessage( GetDictionaryEntry( 9271, socket.language )); // That name is too long. Maximum 50 chars.
			}
			else if( spawnerName != altar.name )
			{
				altar.name = spawnerName;
				let tmpMsg = GetDictionaryEntry( 9829, socket.language ); // Spawner name updated to %s
				altar.TextMessage( tmpMsg.replace( /%s/gi, spawnerName ), false, 0x3b2, 0, pUser.serial );
			}

			ChampionMenu( socket, altar );
			altar.Refresh();
			break;
		case 2:
		{
			if( altar.GetTag( "spawnActive" )) 
			{
				pUser.SysMessage( "This spawn is already active!" );
				break;
			}
			let type = altar.GetTag( "championType" ) || 0;
			if( type == 0 )
			{
				pUser.SysMessage( "Please set the champion type first." );
				return;
			}
			altar.SetTag( "spawnActive", 1 );
			altar.SetTag( "killCount", 0 );
			altar.SetTag( "spawnStage", 1 );
			altar.SetTag( "redSkullCount", 1 );
			altar.SetTag( "whiteSkullCount", 0 );
			PlaceRedSkulls( altar, 1 );
			PlaceWhiteSkulls( altar, 0, 1 );
			StartChampionWave( altar, 1 );
			altar.StartTimer( 30000, 1, 7500 );
			altar.StartTimer( 600000, 10, 7500); // Decay timer
			ChampionMenu( socket, altar );
			altar.Refresh();
			break;
		}
		case 3:// Turn Off
		{
			if( altar.GetTag( "spawnActive") != 1 )
			{
				pUser.SysMessage( "The spawn is already inactive." );
				break;
			}
			altar.SetTag( "spawnActive", 0 );
			altar.SetTag( "killCount", 0 );
			altar.SetTag( "whiteSkullCount", 0 );
			altar.SetTag( "redSkullCount", 0 );
			altar.SetTag( "spawnStage", 1 );
			altar.KillJSTimer( 1, 7500 );
			altar.KillJSTimer( 10, 7500 );
			TriggerEvent( 7500, "RemoveRedSkulls", altar );
			TriggerEvent( 7500, "RemoveWhiteSkulls", altar );
			AreaCharacterFunction( "RemoveSpawn", pUser, 80, socket );
			ChampionMenu( socket, altar );
			altar.Refresh();
			break;
		}
		default:
			break;
	}
}

function onTooltip( altar, pSocket )
{
	if( !ValidateObject( altar ))
		return "";

	let type = altar.GetTag( "championType" ) || 0;
	let active = altar.GetTag( "spawnActive" ) == 1 ? "Yes" : "No";
	let stage = altar.GetTag( "spawnStage" ) || 1;
	let redSkulls = parseInt( altar.GetTag( "redSkullCount" )) || 0;
	let whiteSkulls = parseInt( altar.GetTag( "whiteSkullCount" )) || 0;
	let kills = parseInt( altar.GetTag( "killCount" )) || 0;
	let currentMobCount = AreaCharacterFunction( "ChampionSpawnNpc", altar, 60, null );
	let maxSpawn = altar.morex;

	let tooltipText =
		"Max Spawn Count: " + maxSpawn + "\n" +
		"Spawn Count: " + currentMobCount + "\n" +
		"Type: " + type + "\n" +
		"Active: " + active + "\n" +
		"Stage: " + stage + "\n" +
		"Red Skulls: " + redSkulls + "\n" +
		"White Skulls: " + whiteSkulls + "\n" +
		"Kills: " + kills;

	return tooltipText;
}