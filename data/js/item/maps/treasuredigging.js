//http://web.archive.org/web/20020408163252/http://uo.stratics.com/content/basics/treasure.shtml

function TreasureDigging( pUser )
{
	var socket = pUser.socket;
	if( pUser.GetTempTag( "digging" ) == 1 )
	{
		socket.SysMessage( GetDictionaryEntry( 5706, socket.language ));// You are already digging treasure.
		return false;
	}
	else
		socket.CustomTarget(0, GetDictionaryEntry( 5707, socket.language ));//Where do you wish to dig?
}

function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	var ourObj = socket.tempObj2;

	// Getting Tags from Map Item
	if( socket.GetWord( 1 ))
	{
		var targX = socket.GetWord( 11 );
  		var targY = socket.GetWord( 13 );
  		var targZ = socket.GetSByte( 16 );

		var tagCoords = ourObj.GetTag( "coords" );
		if( !tagCoords )
		{
			// Not a valid treasure map, tell player
			socket.SysMessage( "Invalid map" );
			return;
		}
		var myCoords = tagCoords.split( "," );
		var treasureX = parseInt( myCoords[0] );
		var treasureY = parseInt( myCoords[1] );
		var mapLevel = ourObj.GetTag( "Level" );
		var radius = 0;
		var xDifference = 0;
		var yDifference = 0;
	
		if( ourObj.GetTag( "Decoded" ) == 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 5709, socket.language ));// You did not decode this map and have no clue where to look for the treasure.
		}
		else if( pUser.x == treasureX && pUser.y == treasureY )
		{
			socket.SysMessage( GetDictionaryEntry( 5708, socket.language ));// You stop digging because something is directly on top of the treasure chest
		}
		else if( ourObj.GetTag( "found" ) == 1 )
		{
			socket.SysMessage( GetDictionaryEntry( 5710, socket.language ));// The treasure for this map has already been found.
		}
		else
		{
			// Distance from Point of chest - we just want the difference, we don't care if it's negative or positive
			xDifference = Math.abs( treasureX - targX );
			yDifference = Math.abs( treasureY - targY );

			// Range where you can start digging up chest.
			var mining = ( pUser.skills.mining / 10 ).toFixed( 1 );

			if( mining < 51.0 )
				radius = 1;
			else if( mining < 81.0 )
				radius = 2;
			else if( mining < 99.0 )
				radius = 3;
			else
				radius = 4;

			//var mapElev = GetMapElevation(treasureX .x, treasureY .y, pUser.worldnumber);;

			// Checking if we had correct point
			if( xDifference <= radius && yDifference <= radius )
			{
				pUser.SetTempTag( "tmapFoundSer", ourObj.serial.toString() );// Set the Serial of the Map on user for later.
				pUser.SetTempTag( "digging", 1 );// keeps you from digging again.

				playDiggingAnimation( pUser );

				// Dirt Effect
				var dirtMade = CreateDFNItem( socket, pUser, "treasuredirt", 1, "ITEM", false );
				dirtMade.Teleport( treasureX, treasureY, pUser.z, pUser.worldnumber );
				dirtMade.tempObj = pUser;
				dirtMade.SetTempTag( "Level", mapLevel );
				dirtMade.SetTempTag( "diggerCharSer", pUser.serial );
				dirtMade.StartTimer( 1000, 0, 5400 );
				pUser.SetTempTag( "dirtMadeSer", dirtMade.serial.toString() );
			}
			else if( mapLevel > 0 && xDifference > radius && yDifference > radius )
			{
				socket.SysMessage( GetDictionaryEntry( 5711, socket.language ));// You dig and dig, but do not find any treasure (Meaning the treasure is nowhere near)
			}
			else if(( mapLevel > 0 && xDifference > radius && yDifference <= radius ) || ( yDifference > radius && xDifference <= radius ))
			{
				socket.SysMessage( GetDictionaryEntry( 5712, socket.language ));// You dig and dig, but no treasure seems to be here (Meaning you are close to the treasure, but not close enough to dig it up)
			}
			else
			{
				var direction = "";
				switch( pUser.DirectionTo( treasureX, treasureY ))
				{
					case 1:
						direction = "north";
						break;
					case 2:
						direction = "northeast";
						break;
					case 3:
						direction = "east";
						break;
					case 4:
						direction = "southeast";
						break;
					case 5:
						direction = "south";
						break;
					case 6:
						direction = "southwest";
						break;
					case 7:
						direction = "west";
						break;
					default:
						direction = "northwest";
						break;
				}
				socket.SysMessage( GetDictionaryEntry( 5721, socket.language ) + " " + direction + "." );// Try looking for the treasure chest more to the
			}
		}
	}
	else
	{
		// Tell player they can't dig there
		socket.SysMessage( GetDictionaryEntry( 5720, socket.language ) );// Try looking for the treasure chest more to the
	}
}

// Function creates the Treasure Chest and sets it up.
function TreasureChest( pUser, iUsed, mapLevel )
{
	var chest = "";
	switch( mapLevel )
	{
		case 0: chest = "treasurechestlevel0"; break;
		case 1: chest = "treasurechestlevel1"; break;
		case 2: chest = "treasurechestlevel2"; break;
		case 3: chest = "treasurechestlevel3"; break;
		case 4: chest = "treasurechestlevel4"; break;
		case 5: chest = "treasurechestlevel5"; break;
		case 6: chest = "treasurechestlevel6"; break;
	}

	if( chest != "" )
	{
		var chestmade = CreateDFNItem( pUser.socket, pUser, chest, 1, "SPAWNER", false );
		chestmade.Teleport( iUsed.x, iUsed.y, iUsed.z - 6, iUsed.worldnumber );
		chestmade.movable = 2;
		return chestmade;
	}
}

const initialSpawn = {
	2: ["orcmage", "gargoyle", "gazer", "hellhound", "earthele"],
	3: ["lich", "ogrelord", "dreadspider", "airele", "fireele"],
	4: ["dreadspider", "lichlord", "daemon", "eldergazer", "ogrelord"],
	5: ["lichlord", "daemon", " eldergazer", "poisonele", "bloodele"],
	6: ["LichLord", "daemon", " eldergazer", "poisonele", "bloodele"],
};

function playDiggingAnimation( pUser )
{
	pUser.SoundEffect( 0x33B, true );
	if( pUser.bodyType === 2 )// Gargoyle
	{
		pUser.DoAction( 0x0, 0x3 );
	}
	else if( pUser.isonhorse )// Mounted
	{
		pUser.DoAction( 0x1A );
	}
	else
		pUser.DoAction( 0x0B );
}

function onTimer( timerObj, timerID ) 
{
	var pUser = timerObj.tempObj;
	var level = timerObj.GetTempTag("Level");
	var scriptID = 5400;

	if (timerID < 13)
	{
		if( pUser.dead )
		{
			TriggerEvent( 5405, "KillTreasurEvent", timerObj, pUser );
		}

		if( timerID == 5 ) 
		{
			timerObj.id = 0x914;
			var spawnedChest = TreasureChest( pUser, timerObj, level );
			if( ValidateObject( spawnedChest ))
			{
				timerObj.SetTag( "tChestSer", spawnedChest.serial.toString() );
			}
		}

		if( timerID > 5 ) 
		{
			var chestRef = CalcItemFromSer( parseInt( timerObj.GetTag( "tChestSer" )));
			if( ValidateObject( chestRef ))
			{
				chestRef.z++;
			}
		}

		if( timerID == 10 ) 
		{
			timerObj.id = 0x913;
		}

		playDiggingAnimation( pUser );
		timerObj.StartTimer( 1000, timerID + 1, scriptID );
	}
	else 
	{
		timerObj.RemoveScriptTrigger (5405) ;
		var tmapItem = CalcItemFromSer( parseInt( pUser.GetTempTag( "tmapFoundSer" )));
		if (ValidateObject( tmapItem )) 
		{
			tmapItem.SetTag( "found", 1 );// Marks the chest found so you can't not dig more then one.
		}

		pUser.SetTempTag( "digging", null );
		var mapLevel = timerObj.GetTag( "Level" ); // set the level to a static value using the tag from the chest.
		if( mapLevel >= 2 )
		{
			var spawnList = initialSpawn[mapLevel]; // selects the array of possible NPCs for the given level
			for( var i = 0; i < 4; i++ ) 
			{
				var randomlySelectedNPC = spawnList[Math.floor( Math.random() * spawnList.length )]; // selects a random NPC from the array

				SpawnNPC( randomlySelectedNPC, timerObj.x, timerObj.y, timerObj.z, timerObj.worldnumber, timerObj.instanceID );
			}
		}
	}
}

function _restorecontext_() {}