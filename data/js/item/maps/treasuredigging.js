//http://web.archive.org/web/20020408163252/http://uo.stratics.com/content/basics/treasure.shtml

function TreasureDigging( pUser )
{
	var socket = pUser.socket;
	if( pUser.GetTempTag("digging") == 1 )
	{
		pUser.SysMessage( "You are already digging treasure." );
		return false;
	}
	else
		socket.CustomTarget( 0 , "Where do you wish to dig?" );
}

function onCallback0( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var ourObj = pSock.tempObj2;

	// Getting Tags from Map Item
	if( pSock.GetWord(1) )
	{
		var myCoords = ourObj.GetTag( "coords" ).split(",");
		var xtreas = parseInt( myCoords[0] );
		var ytreas = parseInt( myCoords[1] );

		var level = ourObj.GetTag( "Level" );
		var radius = 0;
		var xdif = 0;
		var ydif = 0;
	
		if( pUser.x == xtreas && pUser.y == ytreas )
		{
			pUser.SysMessage("You stop digging because something is directly on top of the treasure chest");
			return false;
		}

		if( ourObj.GetTag( "found" ) == 1 )
		{
			pUser.SysMessage( "The treasure for this map has already been found." );
			return false;
		}

		if( pUser.x > xtreas && pUser.y > ytreas )
		{
			pUser.SysMessage( "You dig and dig, but do not find any treasure" );//Meaning the treasure is not there.
			return false;
		}

		if( pUser.x > xtreas || pUser.y >= ytreas )
		{
			pUser.SysMessage( "You dig and dig, but no treasure seems to be here" );//Meaning you are close to the treasure, but not close enough to dig it up.
			return false;
		}

		// Distance from Point of chest
		if( xtreas >= pUser.x )
			xdif = xtreas - pUser.x;
		else
			xdif = pUser.x - xtreas;

		if( ytreas >= pUser.y )
			ydif = ytreas - pUser.y;
		else
			ydif = pUser.y - ytreas;

		// Range where you can start digging up chest.
		var mining = (pUser.baseskills.mining / 10).toFixed(1);

		if( mining < 51.0 )
			radius = 1;
		else if( mining < 81.0 )
			radius = 2;
		else if( mining < 100.0 )
			radius = 3;
		else
			radius = 4;

		var mapElev = GetMapElevation(xtreas.x, ytreas.y, pUser.worldnumber);;

		// Checking if we had correct point
		if (xdif <= radius && ydif <= radius)
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
			var dirtMade = CreateDFNItem( pSock, pUser, "dirt", 1, "ITEM", false );
			dirtMade.Teleport( xtreas, ytreas, pUser.z, pUser.worldnumber );
			dirtMade.tempObj = pUser;
			dirtMade.SetTempTag( "Level", level );
			dirtMade.StartTimer( 1000, 0, true );
			pSock.SoundEffect( 0x33B, true );
			pUser.frozen = 1;
		}
	}
}

// Function creates the Treasure Chest and sets it up.
function TreasureChest( pUser, iUsed, level ) 
{
	var chest = "";
	switch( level )
	{
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
		chestmade.StartTimer( 1000, 3, true );
	}
}

function onTimer( iUsed, timerID )
{
	var pUser = iUsed.tempObj;
	var level = iUsed.GetTempTag( "Level" );

	switch( timerID )
	{
		case 0:// Dirt Effect 2
			var dirtMade2 = CreateDFNItem( pUser.socket, pUser, "dirt2", 1, "ITEM", false );
			dirtMade2.Teleport( iUsed.x, iUsed.y, iUsed.z, iUsed.worldnumber );
			dirtMade2.StartTimer( 10000, 2, true);
			iUsed.StartTimer( 1000, 1, true);
			TreasureChest( pUser, iUsed, level );
			iUsed.SoundEffect( 0x33B, true );
			if( pUser.bodyType == 2) // Gargoyle
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
			break;
		case 1:
		case 2:
			iUsed.Delete();
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			iUsed.SoundEffect( 0x33B, true );
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
			iUsed.z += 2;
			if( timerID < 6 )
			{
				iUsed.StartTimer( 1000, timerID + 1, true );
			}
			else
			{
				iUsed.StartTimer( 1000, 7, true );
			}
			break;
		case 7:
			iUsed.SoundEffect( 0x33B, true );
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
			iUsed.z += 1;
			pUser.frozen = 0;
			pUser.SetTempTag( "digging", null );
			break;
	}
}