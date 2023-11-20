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
	if( socket.GetWord(1) )
	{
		var myCoords = ourObj.GetTag( "coords" ).split(",");
		var treasureX = parseInt( myCoords[0] );
		var treasureY = parseInt( myCoords[1] );
		var mapLevel = ourObj.GetTag( "Level" );
		var radius = 0;
		var xDifference = 0;
		var yDifference = 0;
	
		if( pUser.x == treasureX && pUser.y == treasureY )
		{
			socket.SysMessage( GetDictionaryEntry( 5708, socket.language ));// You stop digging because something is directly on top of the treasure chest
			return false;
		}

		if( ourObj.GetTag("Decoded") == 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 5709, socket.language ));// You did not decode this map and have no clue where to look for the treasure.
			return false;
		}

		if( ourObj.GetTag( "found" ) == 1 )
		{
			socket.SysMessage( GetDictionaryEntry( 5710, socket.language ));// The treasure for this map has already been found.
			return false;
		}

		if( pUser.x > treasureX && pUser.y > treasureY )
		{
			socket.SysMessage( GetDictionaryEntry( 5711, socket.language ));// You dig and dig, but do not find any treasure Meaning the treasure is not there.
			return false;
		}

		if( pUser.x > treasureX || pUser.y >= treasureY )
		{
			socket.SysMessage( GetDictionaryEntry( 5712, socket.language ));// You dig and dig, but no treasure seems to be here Meaning you are close to the treasure, but not close enough to dig it up.
			return false;
		}

		// Distance from Point of chest
		if( treasureX >= pUser.x )
			xDifference = treasureX - pUser.x;
		else
			xDifference = pUser.x - treasureX;

		if( treasureY >= pUser.y )
			yDifference = treasureY - pUser.y;
		else
			yDifference = pUser.y - treasureY;

		// Range where you can start digging up chest.
		var mining = ( pUser.skills.mining / 10 ).toFixed(1);

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
			ourObj.SetTag( "found", 1 );// Marks the chest found so you can not dig more then one.
			pUser.SetTempTag( "digging", 1 );// keeps you from digging again.

			// Digging Animation
			if( pUser.bodyType == 2 ) // Gargoyle
			{
				pUser.DoAction( 0x0, 0x3 );
			}
			else if( pUser.isonhorse ) // Mounted
			{
				pUser.DoAction( 0x1A );
			}
			else // On foot
			{
				pUser.DoAction( 0x0B );
			}

			// Dirt Effect
			var dirtMade = CreateDFNItem( socket, pUser, "dirt", 1, "ITEM", false );
			dirtMade.Teleport( treasureX, treasureY, pUser.z, pUser.worldnumber );
			dirtMade.tempObj = pUser;
			dirtMade.SetTempTag( "Level", mapLevel );
			dirtMade.StartTimer( 1000, 0, 5400 );
			socket.SoundEffect( 0x33B, true );
			pUser.frozen = 1;
		}
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
		chestmade.Teleport( iUsed.x, iUsed.y, iUsed.z - 7, iUsed.worldnumber );
		chestmade.movable = 2;
		chestmade.tempObj = pUser;
		chestmade.StartTimer( 1000, 3, 5400 );
	}
}

const initialSpawn = {
	2: ["orcmage", "gargoyle", "gazer", "hellhound", "earthele"],
	3: ["lich", "ogrelord", "dreadspider", "airele", "fireele"],
	4: ["dreadspider", "lichlord", "daemon", "eldergazer", "ogrelord"],
	5: ["lichlord", "daemon", " eldergazer", "poisonele", "bloodele"],
	6: ["LichLord", "daemon", " eldergazer", "poisonele", "bloodele"],
};

function playDiggingAnimation( pUser, timerObj )
{
	timerObj.SoundEffect( 0x33B, true );
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
	var level = timerObj.GetTempTag( "Level" );
	var scriptID = 5400;

	switch( timerID )
	{
		case 0:// Dirt Effect 2
			var dirtMade2 = CreateDFNItem( pUser.socket, pUser, "dirt2", 1, "ITEM", false );
			dirtMade2.Teleport( timerObj.x, timerObj.y, timerObj.z, timerObj.worldnumber );
			dirtMade2.StartTimer( 10000, 2, scriptID);
			timerObj.StartTimer( 1000, 1, scriptID);
			TreasureChest( pUser, timerObj, level );
			playDiggingAnimation( pUser, timerObj );
			break;
		case 1:
		case 2:
			timerObj.Delete();
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			playDiggingAnimation( pUser, timerObj );
			timerObj.z += 2;
			if( timerID < 6 )
			{
				timerObj.StartTimer( 1000, timerID + 1, scriptID );
			}
			else
			{
				timerObj.StartTimer( 1000, 7, scriptID );
			}
			break;
		case 7:
			playDiggingAnimation( pUser, timerObj );
			timerObj.z += 1;
			pUser.frozen = 0;
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
			break;
	}
}

function _restorecontext_() {}