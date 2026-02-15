/// <reference path="../../definitions.d.ts" />
// @ts-check
function CommandRegistration()
{
	RegisterCommand( "showspawnregions", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_SHOWSPAWNREGIONS( socket, cmdString )
{
	if( cmdString == "nearby" )
	{
		// Call onCallback0 directly to skip targeting
		socket.tempShowNearby = true;
		onCallback0( socket, null );
	}
	else
	{
		socket.CustomTarget( 0, "Select area to display Spawn Regions for" );
	}
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, ourObj )
{
	let pChar = socket.currentChar;

	// Use character's location if tempShowNearby exists
	let tempShowNearby = socket.tempShowNearby;
	let curX = ( typeof( tempShowNearby ) == "undefined" ? socket.GetWord( 11 ) : pChar.x );
	let curY = ( typeof( tempShowNearby ) == "undefined" ? socket.GetWord( 13 ) : pChar.y );
	delete socket.tempShowNearby; // Clean up
	let curWorldNum = pChar.worldnumber;
	let curInstanceID = pChar.instanceID;

	// Check for spawn regions every 5 tiles in a radius of 20 tiles
	let radius = 20;
	let step = 5;
	let foundRegs = {}; // Store unique regions
	let spawnRegs = [];

	// Loop through a grid around the target coordinates
	for( let x = curX - radius; x <= curX + radius; x += step )
	{
	    for( let y = curY - radius; y <= curY + radius; y += step )
	    {
	        let tempSpawnRegs = GetSpawnRegions( x, y, curWorldNum, curInstanceID );
	        if( tempSpawnRegs && tempSpawnRegs.length > 0 )
	        {
	            for( let r = 0; r < tempSpawnRegs.length; r++ )
	            {
	                let checkSpawnReg = tempSpawnRegs[r];

	                if( checkSpawnReg != null )
	                {
		                // Use regionNum as a unique key to avoid duplicates
		                if( !foundRegs[checkSpawnReg.regionNum] )
		                {
		                    foundRegs[checkSpawnReg.regionNum] = true;
		                    spawnRegs.push( checkSpawnReg );
		                }
	                }
	            }
	        }
	    }
	}

	if( spawnRegs.length === 0 )
	{
	    socket.SysMessage( "No spawn regions found within 20 tiles." );
	    return;
	}

	for( let i = 0; i < spawnRegs.length; i++ )
	{
		let hue = RandomNumber( 0x30, 0x3F );
		let spawnReg = spawnRegs[i];
		let spawnRegName = spawnReg.name;
		let spawnX1 = spawnReg.x1;
		let spawnY1 = spawnReg.y1;
		let spawnX2 = spawnReg.x2;
		let spawnY2 = spawnReg.y2;
		let spawnZ = 0;
		socket.SysMessage( "SpawnRegion #" + spawnReg.regionNum + ": " + spawnRegName + " (" + spawnX1 + "," + spawnY1 + " -> " + spawnX2 + "," + spawnY2 + ")" );
		for( let x = spawnX1; x <= spawnX2; x++ )
		{
			if( DistanceBetween( curX, curY, x, spawnY1 - 1 ) <= 30 )
			{
				spawnZ = GetMapElevation( x, spawnY1 - 1, curWorldNum );
				DoMovingEffect( x, spawnY1 - 1, spawnZ, x, spawnY1 - 1, spawnZ, 0x7, 0, 0xFF, false, hue, 0x2, true );
			}
			if( DistanceBetween( curX, curY, x, spawnY2 ) <= 30 )
			{
				spawnZ = GetMapElevation( x, spawnY2, curWorldNum );
				DoMovingEffect( x, spawnY2, spawnZ, x, spawnY2, spawnZ, 0x7, 0, 0xFF, false, hue, 0x2, true );
			}
		}
		for( let y = spawnY1; y <= spawnY2; y++ )
		{
			if( DistanceBetween( curX, curY, spawnX1 - 1, y ) <= 30 )
			{
				spawnZ = GetMapElevation( spawnX1 - 1, y, curWorldNum );
				DoMovingEffect( spawnX1 - 1, y, spawnZ, spawnX1 - 1, y, spawnZ, 0x8, 0, 0xFF, false, hue, 0x2, true );
			}
			if( DistanceBetween( curX, curY, spawnX2, y ) <= 30 )
			{
				spawnZ = GetMapElevation( spawnX2, y, curWorldNum );
				DoMovingEffect( spawnX2, y, spawnZ, spawnX2, y, spawnZ, 0x8, 0, 0xFF, false, hue, 0x2, true );
			}
		}

		// For each spawn region, create a short-lived dummy item in the center that can be used to show name and ID of spawn region
		let tempItem = CreateBlankItem( null, null, 1, spawnReg.name, 0x1771, 0x0, "ITEM", false );
		tempItem.shouldSave = false;
		tempItem.x = Math.round(( spawnX2 + spawnX1 ) / 2 );
		tempItem.y = Math.round(( spawnY2 + spawnY1 ) / 2 );
		tempItem.z = GetMapElevation( Math.round(( spawnX2 + spawnX1 ) / 2 ), Math.round(( spawnY2 + spawnY1 ) / 2 ), curWorldNum );
		tempItem.decayable = true;
		tempItem.decaytime = 12;
		tempItem.color = 50;
		tempItem.Refresh();
		tempItem.TextMessage( "#" + spawnReg.regionNum, false, 0x095, 0, socket.currentChar.serial );
	}
}
