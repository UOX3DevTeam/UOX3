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

		TreasureMap( socket, mapItem );
		pUser.SetTempTag("parentMapSerial", ( mapItem.serial).toString() );
	}
}

function TreasureMap( socket, mapItem )
{
	var pUser = socket.currentChar;
	switch ( mapItem.GetTag( "Map" ))
	{
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

				TriggerEvent( 1503, "SendMapDetails", socket, mapItem, height, width, xtop, ytop, xbottom, ybottom );
				TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );
				socket.SoundEffect( 0x249, true );

				if( mapItem.GetTag( "found" ) == 1 ) 
				{
					socket.SysMessage( GetDictionaryEntry( 5701, socket.language ));// This treasure hunt has already been completed.
				}
				else 
				{
					TriggerEvent( 1503, "SendAddMapPin", socket, mapItem, 100, 100 );
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
	var width = 200;
	var height = 200;
	var xtop = x - 300;
	var ytop = y - 300;
	var xbottom = x + 300;
	var ybottom = y + 300;

	TriggerEvent( 1503, "SendMapDetails", socket, mapItem, height, width, xtop, ytop, xbottom, ybottom );
	TriggerEvent( 1503, "SendAddMapPin", socket, mapItem, 100, 100 );
	TriggerEvent( 1503, "SendMapEditable", socket, mapItem, false );

	mapItem.SetTag( "dimensions", height + "," + width );								// saves information for the map to be reopened
	mapItem.SetTag( "boundingbox", xtop + "," + ytop + "," + xbottom + "," + ybottom );	// saves information for the map to be reopened
	mapItem.SetTag( "coords", x + "," + y );											// Sets the treasure Location for Shovel or Pickaxe.
	mapItem.SetTag( "Decoded", 1 );														// Decoded Map Will be set.
	socket.SoundEffect( 0x249, true );
}

function onTooltip( map, pSocket )
{
	var tooltipText = "";

	if( map.GetTag( "Decoded" ) == 1 && map.GetTag( "found" ) == 0 )
	{
		var mybox = map.GetTag( "coords" ).split(",");
		var xtop = parseInt(mybox[0]);
		var ytop = parseInt(mybox[1]);
		var mapCoords = TriggerEvent( 2503, "GetMapCoordinates", xtop, ytop, map.worldnumber );
		var mapCoordsString = mapCoords[3] + "o " + mapCoords[4] + "'" + ( mapCoords[5] ? "N" : "S" ) + " " + mapCoords[0] + "o " + mapCoords[1] + "'" + ( mapCoords[2] ? "W" : "E" );
		var finalString = GetDictionaryEntry( 5722, pSocket.language ); // coordinates %s
		finalString = ( finalString.replace( /%s/gi, mapCoordsString ));
		tooltipText = ( finalString );
	}

	switch( map.GetTag( "found" ))
	{
		case 1: tooltipText = ( GetDictionaryEntry( 5723, pSocket.language ) ); break;// Completed
	}

	return tooltipText;
}