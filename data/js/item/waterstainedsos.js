// List of valid map water tiles, used to determine location of shipwreck
const mapWaterTiles = [ 0x00a8, 0x00a9, 0x00aa, 0x00ab ];

// List of SOS messages (dictionary IDs) that can potentially show up when player uses a waterstained SOS
const sosMessageEntries = [ 9321, 9322, 9323, 9324, 9325, 9326, 9327, 9328, 9329, 9330, 9331, 9332, 9333, 9334 ];

// Script ID assigned to this script in jse_fileassociations.scp
const scriptID = 5032;

// Enable ancient SOS feature? Normally available with AoS expansion and beyond
const enableAncientSOS = false;
const ancientSOSHue = 0x481;
const ancientSOSDropRate = 3 / 20; // 4/55 for ToL+? or 1/20

function onCreateDFN( objMade, objType )
{
	if( objType == 0 && ValidateObject( objMade ))
	{
		// Calculate shipwreck location
		FindShipwreckLocation( objMade );

		// Should this have a chance to be an ancient SOS message?
		var ancientSOS = false;
		if( enableAncientSOS )
		{
			var rndNum = RandomNumber( 1, 100 );
			if( rndNum <= 4 ) // 4% chance of ancient SOS, or 1/25
			{
				objMade.name = "ancient SOS";
				objMade.SetTag( "ancientSOS", true );
				objMade.color = 0x481;
				ancientSOS = true;
			}
		}

		// Randomize treasure level
		var treasureLevel = ( ancientSOS ? 4 : RandomNumber( 1, 3 )); // Ancient SOS will have a treasure chest of level 4
		objMade.SetTag( "treasureLevel", treasureLevel );

		// Randomize the message displayed to user when using the SOS
		var sosMsgID = sosMessageEntries[RandomNumber( 0, sosMessageEntries.length - 1 )];
		objMade.SetTag( "sosMsgID", sosMsgID );
	}
}

// Select random location for a ship wreck
function FindShipwreckLocation( sosMsg )
{
	var worldNum = (( sosMsg.worldnumber != 5 ) ? sosMsg.worldnumber : 0 ); // Default to 0 if user is in a map that doesn't support SOSes

	// Fetch SOS/shipwreckLocations for specified world
	var sosArea = GetRandomSOSArea( worldNum, sosMsg.instanceID );

	if( sosArea == null )
	{
		Console.Error( "[waterstainedsos.js] Unable to find valid SOS area - is the [SOSAREAS] section of regions.dfn setup correctly?" );
		return;
	}

	// Look for a valid SOS location within the randomly chosen area
	for( var i = 0; i < 50; i++ )
	{
		var rndX = RandomNumber( sosArea[0], sosArea[2] );
		var rndY = RandomNumber( sosArea[1], sosArea[3] );

		if( CheckForValidSOSLocation( rndX, rndY, worldNum ))
		{
			sosMsg.morex = rndX;
			sosMsg.morey = rndY;
			sosMsg.morez = worldNum;
			sosMsg.more = sosMsg.instanceID;
			return;
		}
	}

	// If script gets to this point, it failed to find a valid location in 50 attempts
	Console.Warning( "Failed to find valid SOS location for waterstained SOS (serial: " + sosMsg.serial + ")" );
}

function CheckForValidSOSLocation( xLoc, yLoc, worldNum )
{
	// A valid water tile was detected, but make sure the location is not right next to land
	var xOffset = 0;
	var yOffset = 0;
	var mapTileIDToCheck = 0;
	var valid = false;
	for( xOffset = xLoc - 5; !valid && xOffset < xLoc + 5; ++xOffset )
	{
		for( yOffset = yLoc - 5; !valid && yOffset < yLoc + 5; ++yOffset )
		{
			// Fetch ID of map tile at various places surrounding the target location
			mapTileIDToCheck = GetTileIDAtMapCoord( xLoc, yLoc, worldNum );

			// If a non-water map tile is found, player is attempting to fish in shallow water
			valid = ( mapWaterTiles.indexOf( mapTileIDToCheck ) > -1 );
		}
	}
	return valid;
}

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		// Make sure the SOS message actually contains valid coordinates
		if( iUsed.morex == 0 || iUsed.morey == 0 )
		{
			socket.SysMessage( "This message has sustained too much water damage, and any writing on it is no longer legible." );
			return false;
		}

		// Display SOS message with the location coordinates
		var sosMsgID = iUsed.GetTag( "sosMsgID" );
		DisplaySOSGump( socket, pUser, iUsed, sosMsgID );
	}
	return false;
}

function DisplaySOSGump( socket, pUser, iUsed, sosMsgID )
{
	// Prepare SOS Message
	var mapCoords = TriggerEvent( 2503, "GetMapCoordinates", iUsed.morex, iUsed.morey, iUsed.morez );

	// var sosCoords = (iUsed.morex).toString() + ", " + (iUsed.morey).toString();
	var sosCoords = mapCoords[3] + "o " + mapCoords[4] + "'" + ( mapCoords[5] ? "S" : "N" ) + " " + mapCoords[0] + "o " + mapCoords[1] + "'" + ( mapCoords[2] ? "E" : "W" );
	var sosMsg = GetDictionaryEntry( sosMsgID, socket.language );
	sosMsg = ( sosMsg.replace( /%s/gi, sosCoords ));

	// Close existing gump, if any
	socket.CloseGump( scriptID + 0xffff, 0 );

	// Open new gump
	var sosGump = new Gump;
	sosGump.AddPage( 0 );
	sosGump.AddBackground( 0, 0, 250, 350, 2520 ); // 9390 for ancient SOS background
	sosGump.AddHTMLGump( 30, 50, 190, 420, false, false, sosMsg );
	sosGump.Send( socket );
	sosGump.Free();
}