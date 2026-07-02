/// <reference path="../definitions.d.ts" />
// @ts-check
// Menu listing all region-based spawners, letting you teleport to them from the menu

function CommandRegistration()
{
	RegisterCommand( "spawnregionadmin", 8, true );
}

var spawnRegionList = [];
var spawnRegionListButtonID = 50;
var spawnRegionListUpdated = 0;
var spawnRegionListTooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day
var spawnRegionListForceUpdate = false;

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_SPAWNREGIONADMIN( socket, cmdString )
{
	socket.currentChar.SetTempTag( "spawnRegionAdminWorldFilter", -1 );
	ShowSpawnRegionList( socket, cmdString );
}

function ShowSpawnRegionList( socket, cmdString )
{
	var gumpID = 1064 + 0xffff;
	socket.CloseGump( gumpID, 0 );

	// Only refresh spawn region list if it hasn't been updated in the last X seconds
	if( spawnRegionListForceUpdate || cmdString != "" || GetCurrentClock() - spawnRegionListUpdated > 20000 )
	{
		var timeStart = new Date().getTime();

		spawnRegionList = [];
		var count = IterateOverSpawnRegions( socket );
		spawnRegionListUpdated = GetCurrentClock();

		var timeEnd = new Date().getTime();
		var totalTime = timeEnd - timeStart;

		let foundMsg = GetDictionaryEntry( 2660, socket.language ).replace( /%i/gi, spawnRegionList.length );
		socket.SysMessage( foundMsg.replace( /%u/gi, totalTime )); // Found %i region-based spawners in %u milliseconds.
	}
	DisplaySpawnRegionListGump( socket, null, null, 0 );
}

/** @type { ( toCheck: SpawnRegion, socket: SocketOrNull ) => boolean } */
function onIterateSpawnRegions( toCheck, socket )
{
	if( toCheck != null )
	{
		spawnRegionList.push({
			//buttonId: spawnRegionListButtonID,
			buttonId: toCheck.regionNum + spawnRegionListButtonID,
			spawnRegion: toCheck });
		//spawnRegionListButtonID++;
		return true;
	}
	return false;
}

function DisplaySpawnRegionListGump( socket, filteredList, incFilterString, spawnCountSort )
{
	if( socket == null )
		return;

	var filterString = " ";
	var sockLang = socket.language;
	if( incFilterString != null && incFilterString != "" && incFilterString != " " )
	{
		filterString = incFilterString;
	}

	var pageSize = 20;
	var currentPage = socket.currentChar.GetTempTag( "spawnRegionAdminCurrentPage" );
	if( !currentPage || currentPage < 1 )
	{
		currentPage = 1;
	}

	var mySpawnRegionList = [];
	if( filteredList != null )
	{
		mySpawnRegionList = filteredList;
	}
	else
	{
		mySpawnRegionList = spawnRegionList;
	}

	if( spawnCountSort > 0 )
	{
		mySpawnRegionList.sort( function( a, b ) {
			// Sort based on amount of NPCs spawned
			return ( spawnCountSort == 1 ? a.spawnRegion.npcCount - b.spawnRegion.npcCount : b.spawnRegion.npcCount - a.spawnRegion.npcCount );
		});
	}
	else
	{
		mySpawnRegionList.sort( function( a, b ) {
			// Sort by region number (Ascending)
    		return a.spawnRegion.regionNum - b.spawnRegion.regionNum;
		});
	}

	// Figure out the slice of the big list that we want on this page
	var startIndex = ( currentPage - 1 ) * pageSize;
	var endIndex   = startIndex + pageSize;
	var pageSlice  = mySpawnRegionList.slice( startIndex, endIndex );

	var spawnRegionListGump = new Gump();

	spawnRegionListGump.AddPage( 0 );
	spawnRegionListGump.AddBackground( 0, 0, 285, 615, 5054 );
	spawnRegionListGump.AddBackground( 29, 55, 250, 525, 5120 );
	spawnRegionListGump.AddBackground( 5, 30, 150, 25, 2620 ); // Tile White Background

	// Add transparency layer over the filter buttons as well to make them stand out less
	spawnRegionListGump.AddCheckerTrans( 0, 0, 285, 615 );
	var baseYPos = 60;

	// Generate quick-filter buttons for facets
	// F - Felucca, T - Trammel, I - Ilshenar, M - Malas, T - Tokuno, T - Termur
	const letters = "FTIMTT";
	const facets = ["Felucca", "Trammel", "Ilshenar", "Malas", "Tokuno", "Termur" ];
	for( let i = 0; i < letters.length; i++ )
	{
		let letter = letters[i];
		let yPos = baseYPos + ( i * 20 );
		let buttonID = 10 + i;

		// Add letter buttons + labels to gump
		spawnRegionListGump.AddButton( 5, yPos, 9028, 9026, 1, 0, buttonID );
		spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, "Filter for Spawn Regions in " + facets[i] ); // Filter for Spawn Regions in %s
		spawnRegionListGump.AddText(( letter == "I" ? 12 : 10 ), yPos, ( filterString == letter ? 100 : 50 ), letter );
	}

	spawnRegionListGump.AddButton( 250, 3, 4017, 4018, 1, 0, 3 ); // Close Menu Button
	spawnRegionListGump.AddHTMLGump( 15, 2, 240, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>" + GetDictionaryEntry( 2661, socket.language ) + "</BASEFONT></BIG></CENTER>" ); // Manage Spawn Regions

	// Loop over pageSlice, based on current page to show
	var quickButtons = 0;
	for( var i = 0; i < pageSlice.length; i++ )
	{
		// Spawner names/tweak button
		spawnRegionListGump.AddButton( 40, baseYPos + 5 + ( 26 * i ), 5031, 0x13ad, 1, 0, pageSlice[i].buttonId );
		var spawnRegionID = pageSlice[i].spawnRegion.regionNum;

		// Spawn Region Details:<br>Name: %s<br>ID: %i<br>NPC: %w<br>Item: %z
		spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, "Spawn Region Details:<br>Name: " + pageSlice[i].spawnRegion.name + "<br>ID: " + spawnRegionID + "<br>NPC: " + pageSlice[i].spawnRegion.npc + "<BR>ITEM:" + pageSlice[i].spawnRegion.item );
		var name = pageSlice[i].spawnRegion.name;
		if( name.length > 15 )
		{
			name = name.substring( 0, 15 ) + "...";
		}
		spawnRegionListGump.AddText( 40, baseYPos + 5 + ( 26 * i ), 0, name );
		quickButtons++;

		// Teleport-to-spawn-region button
		spawnRegionListGump.AddButton( 152, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 1000000 + pageSlice[i].buttonId );
		spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, GetDictionaryEntry( 2662, sockLang )); // Teleport to Spawn Region
		spawnRegionListGump.AddText( 157, baseYPos + 5 + ( 26 * i ), 1000, "t" );
		quickButtons++;
	}

	if( mySpawnRegionList.length == 0 )
	{
		spawnRegionListGump.AddText( 40, baseYPos + 5 + ( 26 * i ), 99, GetDictionaryEntry( 2621, sockLang )); // No entries found
		quickButtons++;
	}

	var textEntryID = letters.length + quickButtons + 1;

	// Pagination buttons if needed
	var totalPages = Math.ceil( mySpawnRegionList.length / pageSize );

	// Only show "Prev Page" if > page 1
	if( currentPage > 1 )
	{
		spawnRegionListGump.AddButton( 55, 585, 4014, 4015, 1, 0, 2 );
		spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, GetDictionaryEntry( 8124, sockLang )); // Previous Page
	}

	let pageString = GetDictionaryEntry( 2622, sockLang ).replace( /%i/gi, currentPage );
	pageString = pageString.replace( /%u/gi, totalPages );
	spawnRegionListGump.AddText( 110, 588, 50, pageString ); // Page %i / %u
	textEntryID++;

	// Only show "Next Page" if not on last page
	if( currentPage < totalPages )
	{
		spawnRegionListGump.AddButton( 215, 585, 4005, 4006, 1, 0, 1 );
		spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, GetDictionaryEntry( 8123, sockLang )); // Next Page
	}

	// Add text-entry filter at top of gump. TextEntry gump element needs to be last text-related gump element added
	spawnRegionListGump.AddTextEntryLimited( 15, 33, 150, 20, 55, 1, textEntryID, filterString, 15 );
	spawnRegionListGump.AddButton( 160, 30, ( filterString != "" ? 4012 : 4011 ), 4013, 1, 0, 4 ); // Filter Button
	spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, GetDictionaryEntry( 2623, sockLang )); // Apply Filter

	// Filter by spawner type
	var spawnRegionAdminListType = socket.currentChar.GetTempTag( "spawnRegionAdminListType" );
	spawnRegionListGump.AddButton( 195, 30, ( spawnRegionAdminListType == 1 ? 4009 : 4008 ), 4010, 1, 0, 5 ); // NPC Spawner Filter Button
	spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, GetDictionaryEntry( 2637, sockLang )); // Filter for NPC Spawners
	spawnRegionListGump.AddButton( 230, 30, ( spawnRegionAdminListType == 2 ? 4021 : 4020 ), 4022, 1, 0, 6 ); // Item Spawner Filter Button
	spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, GetDictionaryEntry( 2638, sockLang )); // Filter for Item Spawners

	// Sort by spawn count button
	var spawnCountSort = socket.currentChar.GetTempTag( "spawnRegionAdminSpawnCountSort" );
	spawnRegionListGump.AddButton( 263, 32, ( spawnCountSort > 0 ? (( spawnCountSort == 1 || spawnCountSort == 2 ) ? 2647 : 2648 ) : 2648 ), 2646, 1, 0, 7 );

	let sortBySpawnAmt = GetDictionaryEntry( 2639, sockLang ).replace( /%s/gi, ( spawnCountSort > 0 ? ( spawnCountSort == 1 ? GetDictionaryEntry( 2627, sockLang ) : GetDictionaryEntry( 2628, sockLang )) : "N/A" )); // Sort by Spawn Count (%s) / Ascending/Descending
	sortBySpawnAmt += "<br>(NPC Spawn Count)";
	spawnRegionListGump.AddToolTip( spawnRegionListTooltipClilocID, socket, sortBySpawnAmt );

	spawnRegionListGump.Send( socket );
	spawnRegionListGump.Free();
}

function DisplayConfirmationGump( socket, chosenSpawnObj )
{
	var confirmationGump = new Gump();

	confirmationGump.AddPage( 0 );
	confirmationGump.AddBackground( 0, 0, 285, 185, 5054 );
	confirmationGump.AddBackground( 10, 35, 255, 110, 5120 );

	// Add transparency layer over the filter buttons as well to make them stand out less
	confirmationGump.AddCheckerTrans( 0, 0, 285, 185 );

	confirmationGump.AddButton( 250, 3, 4017, 4018, 1, 0, 3 ); // Close Menu Button
	confirmationGump.AddHTMLGump( 15, 4, 240, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>Disable Spawn Region</BASEFONT></BIG></CENTER>" );

	confirmationGump.AddHTMLGump( 15, 50, 240, 100, 0, 0, "<CENTER><BASEFONT color=#EECD8B>Please confirm disabling of:</BASEFONT><br><BASEFONT color=#FFFFFF>" + chosenSpawnObj.name + " (" + chosenSpawnObj.serial.toString() + ")</BASEFONT></CENTER>" );
	confirmationGump.AddButton( 50, 150, 247, 248, 1, 0, 8 ); // Confirm
	confirmationGump.AddButton( 165, 150, 241, 242, 1, 0, 9 ); // Cancel

	confirmationGump.Send( socket );
	confirmationGump.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;

	// Filter buttons for facets
	var filterString = "";
	if( pButton >= 10 && pButton <= 15 )
	{
		let worldIndex = pButton - 10;
		pUser.SetTempTag( "spawnRegionAdminWorldFilter", worldIndex.toString() );
		var filteredList = ApplyFilter( pUser, spawnRegionList, true, true, false, false, "" );

		// Reset page to 1
		pUser.SetTempTag( "spawnRegionAdminCurrentPage", 1 );

		DisplaySpawnRegionListGump( socket, filteredList, "", 0 );
		return;
	}

	switch( pButton )
	{
		// Buttons 0-10, reserved for core gump functionality
		case 1: // Next page
		{
			var pageNum = pUser.GetTempTag( "spawnRegionAdminCurrentPage" );
			if( !pageNum )
			{
				pageNum = 1;
			}
			pageNum = pageNum + 1;
			pUser.SetTempTag( "spawnRegionAdminCurrentPage", pageNum );

			// Re-apply filter to spawnRegionList
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var filteredList = ApplyFilter( pUser, spawnRegionList, true, true, true, false, filterString );

			DisplaySpawnRegionListGump( socket, filteredList, filterString, 0 );
			break;
		}
		case 2: // Prev page
		{
			var pageNum = pUser.GetTempTag( "spawnRegionAdminCurrentPage" );
			if( !pageNum )
			{
				pageNum = 1;
			}
			pageNum = pageNum - 1;
			if( pageNum < 1 )
			{
				pageNum = 1;
			}
			pUser.SetTempTag( "spawnRegionAdminCurrentPage", pageNum );

			// Re-apply filter to spawnRegionList
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var filteredList = ApplyFilter( pUser, spawnRegionList, true, true, true, false, filterString );

			DisplaySpawnRegionListGump( socket, filteredList, filterString, 0 );
			break;
		}
		case 0: // Do nothing, close gump
		case 3: // Do nothing, close gump
			break;
		case 4: // Filter by name
		{
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var filteredList = ApplyFilter( pUser, spawnRegionList, true, true, true, false, filterString );

			DisplaySpawnRegionListGump( socket, filteredList, filterString, 0 );
			break;
		}
		case 5: // Filter for NPC Spawners
		{
			// Reset page to 1
			pUser.SetTempTag( "spawnRegionAdminCurrentPage", 1 );

			//var filteredList = spawnRegionList;
			var worldFilter = parseInt( pUser.GetTempTag( "spawnRegionAdminWorldFilter" ));
			if( isNaN( worldFilter ))
			{
				worldFilter = -1;
			}
			var filteredList = FilterListByWorld( spawnRegionList, worldFilter );

			var spawnRegionAdminListType = pUser.GetTempTag( "spawnRegionAdminListType" );
			if( spawnRegionAdminListType == 1 )
			{
				pUser.SetTempTag( "spawnRegionAdminListType", null );
			}
			else
			{
				pUser.SetTempTag( "spawnRegionAdminListType", 1 );

				// Filter for NPC Spawners only
				filteredList = filteredList.filter( function( spawnRegObj ) {
					var spawnReg = spawnRegObj.spawnRegion;
					return ( spawnReg.npc != null && spawnReg.npc != "" ) || ( spawnReg.npcList != null && spawnReg.npcList != "" );
				});
			}

			// Don't forget about the name filter
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');

			var strictFilter = false;
			if( filterString && filterString.length > 0 )
			{
				if( filterString.length == 1 && filterString.toUpperCase() == filterString )
				{
					strictFilter = true;
				}
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}

			DisplaySpawnRegionListGump( socket, filteredList, filterString, 0 );
			break;
		}
		case 6: // Filter for Item Spawners
		{
			// Reset page to 1
			pUser.SetTempTag( "spawnRegionAdminCurrentPage", 1 );

			var worldFilter = parseInt( pUser.GetTempTag( "spawnRegionAdminWorldFilter" ));
			if( isNaN( worldFilter ))
			{
				worldFilter = -1;
			}
			var filteredList = FilterListByWorld( spawnRegionList, worldFilter );

			var spawnRegionAdminListType = pUser.GetTempTag( "spawnRegionAdminListType" );
			if( spawnRegionAdminListType == 2 )
			{
				// Toggle Off
				pUser.SetTempTag( "spawnRegionAdminListType", null );
			}
			else
			{
				// Toggle On
				pUser.SetTempTag( "spawnRegionAdminListType", 2 );

				// Filter for Item Spawners only
				filteredList = filteredList.filter( function( spawnRegObj ) {
					var spawnReg = spawnRegObj.spawnRegion;
					return ( spawnReg.item != null && spawnReg.item != "" ) || ( spawnReg.itemList != null && spawnReg.itemList != "" );
				});
			}

			// Filter by name
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			if( filterString && filterString.length > 0 )
			{
				if( filterString.length == 1 && filterString.toUpperCase() == filterString )
				{
					strictFilter = true;
				}
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}

			DisplaySpawnRegionListGump( socket, filteredList, filterString, 0 );
			break;
		}
		case 7: // Sort by "spawn count"
		{
			var worldFilter = parseInt( pUser.GetTempTag( "spawnRegionAdminWorldFilter" ));
			var filteredList = FilterListByWorld( spawnRegionList, worldFilter );

			filteredList = FilterListBySpawnerType( filteredList, pUser );

			// Don't forget about the name filter
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			if( filterString && filterString.length > 0 )
			{
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}

			let spawnCountSort = pUser.GetTempTag( "spawnRegionAdminSpawnCountSort" );
			spawnCountSort = ( spawnCountSort == 0 ? 1 : ( spawnCountSort == 1 ? 2 : ( spawnCountSort == 2 ? 0 : 0 )));
			pUser.SetTempTag( "spawnRegionAdminSpawnCountSort", spawnCountSort );

			DisplaySpawnRegionListGump( socket, filteredList, filterString, spawnCountSort );
			break;
		}
		default:
		{
			if( pButton >= 50 && pButton < 1000000 )
			{
				// Show details for selected spawn region
				var chosenSpawnRegion = null;
				for( var i = 0; i < spawnRegionList.length; i++ )
				{
					var spawnEntry = spawnRegionList[i];
					if( spawnEntry.buttonId == pButton )
					{
						chosenSpawnRegion = spawnEntry.spawnRegion;
						break;
					}
				}

				if( chosenSpawnRegion != null )
				{
					socket.SysMessage( chosenSpawnRegion.name );

					// Bring up Tweak menu for spawn regions
					pUser.SetTempTag( "tweakSpawnReg", true );
					pUser.SetTempTag( "tweakSpawnRegID", chosenSpawnRegion.regionNum );
					pUser.ExecuteCommand( "tweak" );
				}
			}
			else if( pButton < 2000000 )
			{
				// Teleport to center of Spawn Region
				var spawnRegX, spawnRegY, spawnRegWorld, spawnRegInstanceID;
				for( var i = 0; i < spawnRegionList.length; i++ )
				{
					var spawnRegionEntry = spawnRegionList[i].spawnRegion;

					if( spawnRegionEntry.regionNum == pButton - 1000000 - 50 )
					{
						spawnRegX = Math.round(( spawnRegionEntry.x1 + spawnRegionEntry.x2 ) / 2 );
						spawnRegY = Math.round(( spawnRegionEntry.y1 + spawnRegionEntry.y2 ) / 2 );
						spawnRegWorld = spawnRegionEntry.world;
						spawnRegInstanceID = spawnRegionEntry.instanceID;
						break;
					}
				}
				pUser.StartTimer( 50, 0, 1068 );

				if( spawnRegX != 0 && spawnRegY != 0 )
				{
					pUser.Teleport( spawnRegX, spawnRegY, GetMapElevation( spawnRegX, spawnRegY, spawnRegWorld ), spawnRegWorld, spawnRegInstanceID );
					pUser.StartTimer( 50, 1, 1068 );
				}
			}
			break;
		}
	}
}

function FilterListBySpawnerType( listToFilter, pUser )
{
	var filteredList = listToFilter;
	var spawnRegionAdminListType = parseInt( pUser.GetTempTag( "spawnRegionAdminListType" ));

	if( spawnRegionAdminListType == 1 )
	{
		// Filter for NPC Spawners only
		filteredList = spawnRegionList.filter( function( spawnerObj ) {
			let spawnRegion = spawnerObj.spawnRegion;
			return ( spawnRegion.npc != null && spawnRegion.npc != "" ) || ( spawnRegion.npcList != null && spawnRegion.npcList != "" );
		});
	}
	else if( spawnRegionAdminListType == 2 )
	{
		// Filter for item Spawners only
		filteredList = spawnRegionList.filter( function( spawnerObj ) {
			let spawnRegion = spawnerObj.spawnRegion;
			return ( spawnRegion.item != null && spawnRegion.item != "" ) || ( spawnRegion.itemList != null && spawnRegion.itemList != "" );
		});
	}

	return filteredList;
}

function FilterListByWorld( listToFilter, worldNum )
{
	var filteredList = listToFilter.filter( function( spawnObj ) {
		// Filter by facet
		return ( worldNum === -1 || spawnObj.spawnRegion.world == worldNum );
	});

	return filteredList;
}

function FilterListByName( listToFilter, filterString, strictFilter )
{
	var filterStringLow = filterString.toLowerCase();
	var filteredList = listToFilter.filter( function( spawnObj ) {
		// Convert both strings to lowercase, compare the first few chars
		return spawnObj.spawnRegion.name
		  .substring( 0, filterString.length )
		  .toLowerCase() == filterStringLow;
	});

	if( !strictFilter && filteredList.length == 0 && filterString.length > 0 )
	{
		filteredList = listToFilter.filter( function( spawnObj ) {
			// Check if the lowercase name includes the lowercase filter string anywhere
			return spawnObj.spawnRegion.name.toLowerCase().indexOf( filterStringLow ) !== -1;
		});
	}

	return filteredList;
}

function ApplyFilter( pUser, listToFilter, filterByWorld, filterByType, filterByName, strictFilter, filterString )
{
	// World Filter
	//let filteredList = [];
	let filteredList = listToFilter;
	if( filterByWorld )
	{
		var worldFilter = parseInt( pUser.GetTempTag( "spawnRegionAdminWorldFilter" ));
		filteredList = FilterListByWorld( listToFilter, worldFilter );
	}

	// Spawner Type Filter
	if( filterByType )
	{
		filteredList = FilterListBySpawnerType( filteredList, pUser );
	}

	// Name Filter
	if( filterByName )
	{
		if( filterString && filterString.length > 0 )
		{
			filteredList = FilterListByName( filteredList, filterString, strictFilter );
		}
	}
	return filteredList;
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( timerObj, timerID )
{
	if( timerID == 0 )
	{
		DisplaySpawnRegionListGump( timerObj.socket, null, "", 0 );
	}
	else if( timerID == 1 )
	{
		timerObj.ExecuteCommand( "showspawnregions nearby" );
	}
}
