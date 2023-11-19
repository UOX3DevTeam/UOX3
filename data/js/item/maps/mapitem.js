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

		var socket = pUser.socket;
		mapPresets( socket, mapItem );
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

function mapPresets( socket, mapItem )
{
	var pUser = socket.currentChar;
	switch ( mapItem.GetTag( "Map" ))
	{
		case 1:
			var map_name = 'smallworld';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			//var pinx = mapItem.GetTag('pinx');
			//var piny = mapItem.GetTag('piny');
			//if(pinx > 0 && piny > 0)
			//{
			//	sendAddMapPin(socket, mapItem, pinx, piny);
			//}
			break;
		case 2:
			var map_name = 'largeworld';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 3:
			var map_name = 'britain';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 4:
			var map_name = 'britaintoskarabrae';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 5:
			var map_name = 'britaintotrinsic';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 6:
			var map_name = 'bucsden';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 7:
			var map_name = 'bucsdentomagincia';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 8:
			var map_name = 'bucsdentoocllo';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 9:
			var map_name = 'jhelom';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 10:
			var map_name = 'magincia';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 11:
			var map_name = 'maginciatoocllo';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 12:
			var map_name = 'minoc';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 13:
			var map_name = 'minoctoyew';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 14:
			var map_name = 'minoctovesper';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 15:
			var map_name = 'moonglow';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 16:
			var map_name = 'moonglowtonujelm';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 17:
			var map_name = 'nujelm';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 18:
			var map_name = 'nujelmtomagincia';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 19:
			var map_name = 'occlo';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 20:
			var map_name = 'serpentshold';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 21:
			var map_name = 'serpentsholdtoocllo';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 22:
			var map_name = 'skarabrae';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 23:
			var map_name = 'trinsic';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 24:
			var map_name = 'trinsictobucsden';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 25:
			var map_name = 'trinsictojhelom';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 26:
			var map_name = 'vesper';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 27:
			var map_name = 'vespertonujelm';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 28:
			var map_name = 'yew';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 29:
			var map_name = 'yewtobritain';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 30:
			var map_name = 'Tokuno';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 31:
			var map_name = 'Malas';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 32:
			var map_name = 'Ilshenar';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
		case 33:
			var map_name = 'TerMur';
			var preset_values = MAP_PRESETS[map_name];
			sendMapDisplay( socket, mapItem );
			sendMapDetails( socket, mapItem, preset_values[0], preset_values[1], preset_values[2], preset_values[3], preset_values[4], preset_values[5] );
			sendMapEditable( socket, mapItem, false );
			break;
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

				sendMapDetails( socket, mapItem, height, width, xtop, ytop, xbottom, ybottom );
				sendMapEditable( socket, mapItem, false );
				break;
			}			
			else
			{
				socket.SysMessage( GetDictionaryEntry( 5700, socket.language ));// It appears to be blank.
				break;
			}
		case 100:// Treasure Map
			if( mapItem.GetTag( "Decoded" ) == 1 )
			{
				var mydimensions = mapItem.GetTag( "dimensions" ).split(",");
				var height = parseInt( mydimensions[0] );
				var width = parseInt( mydimensions[1] );

				var mybox = mapItem.GetTag( "boundingbox" ).split(",");
				var xtop = parseInt( mybox[0] );
				var ytop = parseInt( mybox[1] );
				var xbottom = parseInt( mybox[2] );
				var ybottom = parseInt( mybox[3] );

				sendMapDetails( socket, mapItem, height, width, xtop, ytop, xbottom, ybottom );
				sendMapEditable( socket, mapItem, false );
				socket.SoundEffect( 0x249, true );

				if( mapItem.GetTag( "found" ) == 1 ) 
				{
					socket.SysMessage( GetDictionaryEntry( 5701, socket.language ));// This treasure hunt has already been completed.
				}
				else 
				{
					sendAddMapPin( socket, mapItem, 100, 100 );
					socket.SysMessage( GetDictionaryEntry( 5702, socket.language ));// The treasure is marked by the red pin. Grab a shovel and go dig it up!
				}
				break;
			}
			else
			{
				var level = mapItem.GetTag( "Level" );
				var skillValue = ( pUser.baseskills.cartography / 10 ).toFixed( 1 );
				var decoded = false;
				var mapName = 0;

				switch (level)
				{
					case 0:
						if (skillValue >= 0.0)
						{
							decoded = true;
							mapName = 5704;// young treasure map
						}
						break;
					case 1:
						if( skillValue >= 27.0 )
						{
							decoded = true;
							mapName = 5714;// plainly drawn treasure map
						}
						break;
					case 2:
						if( skillValue >= 71.0 )
						{
							decoded = true;
							mapName = 5715;// expertly drawn treasure map
						}
						break;
					case 3:
						if( skillValue >= 81.0 )
						{
							decoded = true;
							mapName = 5716;// adeptly drawn treasure map
						}
						break;
					case 4:
						if( skillValue >= 91.0 )
						{
							decoded = true;
							mapName = 5717;// cleverly drawn treasure map
						}
						break;
					case 5:
					case 6:
						if( skillValue >= 100.0 )
						{
							decoded = true;
							mapName = 5718;// deviously drawn treasure map
						}
						break;
						default:
							break;
				}

				if( decoded )
				{
					socket.SysMessage( GetDictionaryEntry( 5703, socket.language ));// You decoded the map!
					mapItem.name = GetDictionaryEntry( mapName, socket.language );
					TreasureMapCoords( socket, mapItem );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 5705, socket.language ));// You cannot decode this map yet!
				}
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

	//sendMapDetails( socket, mapItem, size, size, pUser.x - dist, pUser.y - dist, pUser.x + dist, pUser.y + dist );
	sendMapDetails( socket, mapItem, size, size, ( pUser.x - dist ), ( pUser.y - dist ), ( pUser.x + dist ), ( pUser.y + dist ));
	mapItem.SetTag( "dimensions", size + "," + size );																						// saves information for the map to be reopened
	mapItem.SetTag( "boundingbox", ( pUser.x - dist ) + "," + ( pUser.y - dist ) + "," + ( pUser.x + dist ) + "," + ( pUser.y + dist ));	// saves information for the map to be reopened
	mapItem.SetTag( "Drawn", 1 );
}

function TreasureMapCoords( socket, mapItem )
{
	var pUser = socket.currentChar;
	var TREASURECOORDS = [
	[2643, 851],
	[2672, 392],
	[3404, 238],
	[3369, 637],
	[2740, 435],
	[2770, 345],
	[2781, 289],
	[2836, 233],
	[3014, 250],
	[3082, 202],
	[3246, 245],
	[3375, 458],
	[1314, 317],
	[1470, 229],
	[1504, 366],
	[2340, 645],
	[2350, 688],
	[2395, 723],
	[2433, 767],
	[1319, 889],
	[1414, 771],
	[1529, 753],
	[1555, 806],
	[1511, 967],
	[1510, 1071],
	[1562, 1058],
	[961, 506],
	[1162, 189],
	[2337, 1159],
	[2392, 1154],
	[2517, 1066],
	[2458, 1042],
	[1658, 2030],
	[2062, 1962],
	[2089, 1987],
	[2097, 1975],
	[2066, 1979],
	[2058, 1992],
	[2071, 2007],
	[2093, 2006],
	[2187, 1991],
	[1689, 1993],
	[1709, 1964],
	[1726, 1998],
	[1731, 2016],
	[1743, 2028],
	[1754, 2020],
	[2033, 1942],
	[2054, 1962],
	[581, 1455],
	[358, 1337],
	[208, 1444],
	[199, 1461],
	[348, 1565],
	[620, 1706],
	[1027, 1180],
	[1037, 1975],
	[1042, 1960],
	[1042, 1903],
	[1034, 1877],
	[1018, 1859],
	[980, 1849],
	[962, 1858],
	[977, 1879],
	[968, 1884],
	[969, 1893],
	[974, 1992],
	[989, 1992],
	[1024, 1990],
	[2648, 2167],
	[2956, 2200],
	[2968, 2170],
	[2957, 2150],
	[2951, 2177],
	[2629, 2221],
	[2642, 2289],
	[2682, 2290],
	[2727, 2309],
	[2782, 2293],
	[2804, 2255],
	[2850, 2252],
	[2932, 2240],
	[3568, 2402],
	[3702, 2825],
	[3511, 2421],
	[3533, 2471],
	[3417, 2675],
	[3425, 2723],
	[3476, 2761],
	[3541, 2784],
	[3558, 2819],
	[3594, 2825],
	[4419, 3117],
	[4471, 3188],
	[4507, 3227],
	[4496, 3241],
	[4642, 3369],
	[4694, 3485],
	[4448, 3130],
	[4453, 3148],
	[4500, 3108],
	[4513, 3103],
	[4424, 3152],
	[4466, 3209],
	[4477, 3230],
	[4477, 3282],
	[2797, 3452],
	[2803, 3488],
	[2793, 3519],
	[2831, 3511],
	[2989, 3606],
	[3035, 3603],
	[1427, 2405],
	[1466, 2181],
	[1519, 2214],
	[1523, 2150],
	[1541, 2115],
	[1534, 2189],
	[1546, 2221],
	[1595, 2193],
	[1619, 2236],
	[1654, 2268],
	[1655, 2304],
	[1433, 2381],
	[1724, 2288],
	[1703, 2318],
	[1772, 2321],
	[1758, 2333],
	[1765, 2430],
	[1647, 2641],
	[1562, 2705],
	[1670, 2808],
	[1471, 2340],
	[1562, 2312],
	[1450, 2301],
	[1437, 2294],
	[1478, 2273],
	[1464, 2245],
	[1439, 2217],
	[1383, 2840],
	[1388, 2984],
	[1415, 3059],
	[1602, 3012],
	[1664, 3062],
	[2062, 2144],
	[2178, 2151],
	[2104, 2123],
	[2098, 2101],
	[2123, 2120],
	[2130, 2108],
	[2129, 2132],
	[2153, 2120],
	[2162, 2148],
	[2186, 2144],
	[492, 2027],
	[477, 2044],
	[451, 2053],
	[468, 2087],
	[465, 2100],
	[958, 2504],
	[1025, 2702],
	[1290, 2735],
	[2013, 3269],
	[2149, 3362],
	[2097, 3384],
	[2039, 3427],
	[2516, 3999],
	[2453, 3942],
	[2528, 3982],
	[2513, 3962],
	[2512, 3918],
	[2513, 3901],
	[2480, 3908],
	[2421, 3902],
	[2415, 3920],
	[2422, 3928],
	[2370, 3428],
	[2341, 3482],
	[2360, 3507],
	[2387, 3505],
	[2152, 3950],
	[2157, 3924],
	[2140, 3940],
	[2143, 3986],
	[2154, 3983],
	[2162, 3988],
	[2467, 3581],
	[2527, 3585],
	[2482, 3624],
	[2535, 3608],
	[1090, 3110],
	[1094, 3131],
	[1073, 3133],
	[1076, 3156],
	[1068, 3182],
	[1096, 3178],
	[1129, 3403],
	[1135, 3445],
	[1162, 3469],
	[1128, 3500]
];
	// Generate a random index value within the range of valid indices for the array
	var randomIndex = Math.floor( Math.random() * TREASURECOORDS.length );

	// Retrieve the set of coordinates at the random index
	var [x, y] = TREASURECOORDS[randomIndex];
	var width = 200
	var height = 200
	var xtop = x - 300
	var ytop = y - 300
	var xbottom = x + 300
	var ybottom = y + 300

	sendMapDetails( socket, mapItem, height, width, xtop, ytop, xbottom, ybottom );		// Sets the coords for the map player opens.
	sendAddMapPin( socket, mapItem, 100, 100 );											// Pin Never changes Map changes.
	sendMapEditable( socket, mapItem, false);											// Sets the map so you cant edit the pin.

	mapItem.SetTag( "dimensions", height + "," + width );								// saves information for the map to be reopened
	mapItem.SetTag( "boundingbox", xtop + "," + ytop + "," + xbottom + "," + ybottom );	// saves information for the map to be reopened
	mapItem.SetTag( "coords", x + "," + y );											// Sets the treasure Location for Shovel or Pickaxe.
	mapItem.SetTag( "Decoded", 1 )        												// Decoded Map Will be set.
	socket.SoundEffect( 0x249, true );
}

function sendMapDetails( socket, map, Width, Height, xtop, ytop, xbottom, ybottom )
{
	var pUser = socket.currentChar;
	var toSend = new Packet;

	toSend.ReserveSize( 21 );
	toSend.WriteByte( 0, 0xF5 );
	toSend.WriteLong( 1, map.serial );	//map.serial
	toSend.WriteShort( 5, 0x139D);
	toSend.WriteShort( 7, xtop );	    // Left of Top Left
	toSend.WriteShort( 9, ytop );       // Top of Top Left
	toSend.WriteShort( 11, xbottom );   // Right of Bottom Right
	toSend.WriteShort( 13, ybottom );   // Bottom of Bottom Right
	toSend.WriteShort( 15, Width );     // Assuming End.X - Start.X 
	toSend.WriteShort( 17, Height );    // Assuming End.Y - Start.Y (or vice versa?)

	var mapType = 0x00
	switch( map.worldNum )
	{
		case 0:
			mapType = 0x00; // Felucca
			break;
		case 1:
			mapType = 0x01; // Trammel
			break;
		case 2:
			mapType = 0x02; // Ilshenar
			break;
		case 3:
			mapType = 0x03; // Malas
			break;
		case 4:
			mapType = 0x04; // Tokuno
			break;
		case 5:
			mapType = 0x05; // TerMur
			break;
		default:
			mapType = 0x00; // Felucca
			break;
	}
	toSend.WriteShort( 19, mapType ); // World facet
	socket.Send( toSend );
	toSend.Free();
}

function PacketRegistration()
{
	RegisterPacket( 0x56, 0x0 );
}

/*
Action Flag command
1: Add Pin
2: Insert New Pin
3: Change Pin
4: Remove Pin
5: Clear Pins
6: Toggle Edit Map (Client)
7: Reply From Server to Action 6*/

function onPacketReceive(pSocket, packetNum, subCmd )
{
	var pUser = pSocket.currentChar;
	var mapItem = CalcItemFromSer( parseInt( pUser.GetTempTag( "parentMapSerial" )));

	var cmd = pSocket.GetByte( 0 );
	if( cmd != packetNum )
		return;

	pSocket.ReadBytes( 11 );
	var subCmd = pSocket.GetByte( 5 ); // Fetch subCmd

	var editable = mapItem.GetTag( 'editable' );
	switch( subCmd )
	{
		case 1://Append to list of pins
			var x = pSocket.GetSWord( 7 );
			var y = pSocket.GetSWord( 9 );
			mapItem.SetTag( 'pinx', pinx );
			mapItem.SetTag( 'piny', piny );
			//pUser.SysMessage( pinx );
			//pUser.SysMessage( piny );
			//pUser.SysMessage( "Sending Sub Command, " + subCmd ); // Debug Msg
			break;
		case 2://Insert into list of pins
			var x = pSocket.GetSWord( 7 );
			var y = pSocket.GetSWord( 9 );
			mapItem.SetTag( 'pinx', pinx );
			mapItem.SetTag( 'piny', piny );
			//pUser.SysMessage( pinx );
			//pUser.SysMessage( piny );
			//pUser.SysMessage( "Sending Sub Command, " + subCmd ); // Debug msg
			break;
		case 3:// Change pin
			//pUser.SysMessage( "Sending Sub Command, " + subCmd ); // Debug msg
			break;
		case 4://Remove pin
			//pUser.SysMessage( "Sending Sub Command, " + subCmd ); // Debug msg
			break;
		case 5://Clear Pins
			mapItem.SetTag( 'pinx', null );
			mapItem.SetTag( 'piny', null );
			//pUser.SysMessage( "Sending Sub Command, " + subCmd ); // Debug msg
			break;
		case 6://Toggle Plotting Course
			if ( editable == 0 )
				sendMapEditable( pSocket, mapItem, false );
			//pUser.SysMessage( "Sending Sub Command, " + subCmd ); // Debug msg
			break;
		default:
			Console.Warning( "Unhandled subcommand (" + subCmd + ") in packet 0x56." );
			break;
	}
	return;
}

function sendMapCommand( socket, mapItem, command, editable, x, y )
{
	var pUser = socket.currentChar;
	var toSend = new Packet;
	toSend.ReserveSize( 11 );
	toSend.WriteByte( 0, 0x56 );
	toSend.WriteLong( 1, mapItem.serial );    //map.serial
	toSend.WriteByte( 5, command );           // command 1-7
	toSend.WriteByte( 6, editable );          // 1 or 0 
	toSend.WriteShort( 7, x );                // x location of pin on the map
	toSend.WriteShort( 9, y );                // y location of pin on the map
	socket.Send( toSend );
	toSend.Free();
}

function sendMapDisplay( socket, map )
{
	var pUser = socket.currentChar;
	sendMapCommand( socket, map, 0x05, 0, 0, 0 );
}

function sendAddMapPin( socket, map, x, y)
{
	var pUser = socket.currentChar;
	//Remove all Pins
	sendMapCommand( socket, map, 0x05, 0, 0, 0 );
	// Add Pins
	sendMapCommand( socket, map, 0x01, 0, x, y );
}

function sendInsertPin(socket, map, x, y)
{
	var pUser = socket.currentChar;
	sendMapCommand( socket, map, 0x02, 1, x, y );
}

function sendMapEditable( socket, mapItem, editable )
{
	var pUser = socket.currentChar;
	mapItem.SetTag( 'editable', editable );
	sendMapCommand( socket, mapItem, 0x07, editable ? 1 : 0, 0, 0 );
}

function onTooltip( map, pSocket )
{
	var tooltipText = "";

	switch( map.GetTag( "found" ))
	{
		case 1: tooltipText = ( "Completed" ); break;
	}

	return tooltipText;
}