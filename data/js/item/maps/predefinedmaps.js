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

		MapPresets( socket, mapItem );
		pUser.SetTempTag("parentMapSerial", ( mapItem.serial).toString() );
	}
}

// Define the MAP_PRESETS object
const MAP_PRESETS = {
	//# Width, Height, xtop, ytop, xbottom, ybottom
	'britain': [200, 200, 1092, 1396, 1736, 1924], //# map of Britain
	'britaintoskarabrae': [200, 200, 256, 1792, 1736, 2560], //# map of Britain to Skara Brae
	'britaintotrinsic': [200, 200, 1024, 1280, 2304, 3072], //# map of Britain to Trinsic
	'bucsden': [200, 200, 2500, 1900, 3000, 2400], //# map of Buccaneer's Den
	'bucsdentomagincia': [200, 200, 2560, 1792, 3840, 2560], //# map of Buccaneer's Den to Magincia
	'bucsdentoocllo': [200, 200, 2560, 1792, 3840, 3072], //# map of Buccaneer's Den to Ocllo
	'jhelom': [200, 200, 1088, 3572, 1528, 4056], //# map of Jhelom
	'magincia': [200, 200, 3530, 2022, 3818, 2298], //# map of Magincia
	'maginciatoocllo': [200, 200, 3328, 1792, 3840, 2304], //# map of Magincia to Ocllo
	'minoc': [200, 200, 2360, 356, 2706, 702],// # map of Minoc
	'minoctoyew': [200, 200, 0, 256, 2304, 3072], //# map of Minoc to Yew
	'minoctovesper': [200, 200, 2467, 572, 2878, 746], //# map of Minoc to Vesper
	'moonglow': [200, 200, 4156, 808, 4732, 1528], //# map of Moonglow
	'moonglowtonujelm': [200, 200, 3328, 768, 4864, 1536], //# map of Moonglow to Nujelm
	'nujelm': [200, 200, 3446, 1030, 3832, 1424], //# map of Nujelm
	'nujelmtomagincia': [200, 200, 3328, 1024, 3840, 2304], //# map of Nujelm to Magincia
	'occlo': [200, 200, 3582, 2456, 3770, 2742], //# map of Ocllo
	'serpentshold': [200, 200, 2714, 3329, 3100, 3639], //# map of Serpent's Hold
	'serpentsholdtoocllo': [200, 200, 2560, 2560, 3840, 3840], //# map of Serpent's Hold to Ocllo
	'skarabrae': [200, 200, 524, 2064, 960, 2452], //# map of Skara Brae
	'trinsic': [200, 200, 1792, 2630, 2118, 2952], //# map of Trinsic
	'trinsictobucsden': [200, 200, 1792, 1792, 3072, 3072], //# map of Trinsic to Buccaneer's Den
	'trinsictojhelom': [200, 200, 256, 1792, 2304, 4095], //# map of Trinsic to Jhelom
	'vesper': [200, 200, 2636, 592, 3064, 1012], //# map of Vesper
	'vespertonujelm': [200, 200, 2636, 592, 3840, 1536], //# map of Vesper to Nujelm
	'yew': [200, 200, 236, 741, 766, 1269], //# map of Yew
	'yewtobritain': [200, 200, 0, 512, 1792, 2048], //# map of Yew to Britain
	'smallworld': [200, 200, 0, 0, 5119, 4095], //# map small world
	'largeworld': [400, 400, 0, 0, 5119, 4095], //# map large world
	'Tokuno': [400, 400, 0, 0, 1448, 1430], //# map Tokuno world
	'Malas': [400, 400, 520, 0, 2580, 2050], //# map Malas world
	'Ilshenar': [400, 400, 130, 136, 1927, 1468], //# map Ilshenar world
	'TerMur': [400, 400, 260, 2780, 1280, 4090], //# map TerMur world
};

function MapPresets( socket, mapItem )
{
	var pUser = socket.currentChar;
	switch( mapItem.GetTag( "Map" ))
	{
		case 1:
			var map_name = 'smallworld';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 2:
			var map_name = 'largeworld';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 3:
			var map_name = 'britain';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5], preset_values[6] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 4:
			var map_name = 'britaintoskarabrae';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 5:
			var map_name = 'britaintotrinsic';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 6:
			var map_name = 'bucsden';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 7:
			var map_name = 'bucsdentomagincia';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 8:
			var map_name = 'bucsdentoocllo';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 9:
			var map_name = 'jhelom';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 10:
			var map_name = 'magincia';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 11:
			var map_name = 'maginciatoocllo';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 12:
			var map_name = 'minoc';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 13:
			var map_name = 'minoctoyew';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 14:
			var map_name = 'minoctovesper';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 15:
			var map_name = 'moonglow';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 16:
			var map_name = 'moonglowtonujelm';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 17:
			var map_name = 'nujelm';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 18:
			var map_name = 'nujelmtomagincia';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 19:
			var map_name = 'occlo';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 20:
			var map_name = 'serpentshold';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 21:
			var map_name = 'serpentsholdtoocllo';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 22:
			var map_name = 'skarabrae';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 23:
			var map_name = 'trinsic';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 24:
			var map_name = 'trinsictobucsden';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 25:
			var map_name = 'trinsictojhelom';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 26:
			var map_name = 'vesper';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 27:
			var map_name = 'vespertonujelm';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 28:
			var map_name = 'yew';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 29:
			var map_name = 'yewtobritain';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 30:
			var map_name = 'Tokuno';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 31:
			var map_name = 'Malas';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 32:
			var map_name = 'Ilshenar';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
		case 33:
			var map_name = 'TerMur';
			var preset_values = MAP_PRESETS[map_name];
			TriggerEvent( 1503, "SendMapDisplay", socket, mapItem );
			TriggerEvent( 1503, "SendMapDetails", socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
			break;
	}

	DisplayPinsOnMap( socket, mapItem );
}

function DisplayPinsOnMap( socket, mapItem )
{
	var pins = [];
	if (mapItem.GetTag("pins"))
	{
		pins = mapItem.GetTag("pins").split(";" );
	}

	for (var i = 0; i < pins.length; i++)
	{
		// Split each pin string into an array of two values
		var pin = pins[i].split("," );

		// Extract the pinx and piny values, converting them to integers
		var pinx = parseInt(pin[0] );
		var piny = parseInt(pin[1] );

		// Assuming sendAddMapPin is correctly implemented
		TriggerEvent( 1503, "SendAddMapPin", socket, mapItem, pinx, piny );
	}
}