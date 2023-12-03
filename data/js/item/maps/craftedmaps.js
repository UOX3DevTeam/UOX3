/*When a player uses a certain item (referred to as mapItem), it triggers the onUseChecked function.
This function then sets up various preset maps based on the item used, such as maps for different cities or worlds within the game.

The script defines a set of map presets in an object called MAP_PRESETS. Each preset includes information like width, height,
and coordinates defining the boundaries of the map.

The mapPresets function is called to handle the specifics of displaying the chosen map. 
It looks at the type of map specified by the mapItem and selects the appropriate preset from the MAP_PRESETS object.
It then sends commands to display and detail the map to the player's game interface.

For certain special maps like crafted city maps and treasure maps, additional actions are taken. For example,
if a player uses a crafted city map, the script calculates the size and boundaries of the map based on the player's skill level.
If it's a treasure map, the script checks if the map has been decoded and displays the treasure location on the map, indicating where the player should dig for a reward.

// TODO: Figure out how to prevent map from being opened multiple times
*/

function onUseChecked( pUser, mapItem )
{
	var socket = pUser.socket;
	if( socket && mapItem && mapItem.isItem )
	{
		var itemOwner = GetPackOwner(mapItem, 0);
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		CraftedMaps( socket, mapItem );
		pUser.SetTempTag("parentMapSerial", ( mapItem.serial).toString() );
	}
}

function CraftedMaps( socket, mapItem )
{
	var pUser = socket.currentChar;
	switch ( mapItem.GetTag( "Map" ))
	{
		case 50:// Crafted Map
			if ( mapItem.GetTag( "Drawn" ) == 1 )
			{
				var mydimensions = mapItem.GetTag( "dimensions" ).split(",");
				var height = parseInt( mydimensions[0] );
				var width = parseInt( mydimensions[1] );

				var mybox = mapItem.GetTag( "boundingbox" ).split(",");
				var xtop = parseInt( mybox[0] );
				var ytop = parseInt( mybox[1] );
				var xbottom = parseInt( mybox[2] );
				var ybottom = parseInt( mybox[3] );

				TriggerEvent( 1503, "SendMapDetails", socket, mapItem, height, width, xtop, ytop, xbottom, ybottom );
				TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
				TriggerEvent( 5403, "DisplayPinsOnMap", socket, mapItem );
				break;
			}			
			else
			{
				socket.SysMessage( GetDictionaryEntry( 5700, socket.language ));// It appears to be blank.
				break;
			}
	}
}

function CraftedMapCoords( socket, mapItem )
{
	var pUser = socket.currentChar;
	var skillValue = ( pUser.baseskills.cartography/10 ).toFixed( 1 );

	// Define the minimum and maximum distances
	if( mapItem.sectionID == "craftedlocalmap" )
	{
		var minDist = 32;
	}
	else if( mapItem.sectionID == "craftedcitymap" ) 
	{
		var minDist = 64;
	}
	else 
	{
		var minDist = 92;
	}
		var maxDist = 200;

	// Linear interpolation for distance based on skill
	var dist = Math.round( minDist + ( maxDist - minDist ) * ( skillValue / 100 ));

	if( mapItem.sectionID == "craftedlocalmap" )
	{
		var size = 200;
	}
	else if( mapItem.sectionID == "craftedcitymap" )
	{
		var size = 300;
	}
	else
	{
		// Define the minimum and maximum sizes
		var minSize = 300;
		var maxSize = 400;

		// Linear interpolation for size based on skill
		var size = Math.round( minSize + ( maxSize - minSize ) * ( skillValue / 100 ));
	}

	TriggerEvent( 1503, "SendMapDetails", socket, mapItem, size, size, (pUser.x - dist), (pUser.y - dist), (pUser.x + dist), (pUser.y + dist) );
	mapItem.SetTag( "dimensions", size + "," + size );																						// saves information for the map to be reopened
	mapItem.SetTag( "boundingbox", ( pUser.x - dist ) + "," + ( pUser.y - dist ) + "," + ( pUser.x + dist ) + "," + ( pUser.y + dist ));	// saves information for the map to be reopened
	mapItem.SetTag( "Drawn", 1 );
}