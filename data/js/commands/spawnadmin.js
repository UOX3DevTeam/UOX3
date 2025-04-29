// Menu displaying all item-based spawners, letting you edit them on-the-fly from menu

function CommandRegistration()
{
	RegisterCommand( "spawnadmin", 2, true );
}

var spawnerList = [];
var spawnerListButtonID = 50;
var spawnerListUpdated = 0;
var spawnerListTooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day

function command_SPAWNADMIN( socket, cmdString )
{
	ShowSpawnerList( socket, cmdString, null, null );
}

function ShowSpawnerList( socket, cmdString, filteredList, filterString )
{
	var gumpID = this.script_id + 0xffff;
	socket.CloseGump( gumpID, 0 );

	// Only refresh spawner list if it hasn't been updated in the last X seconds
	if( cmdString != "" || GetCurrentClock() - spawnerListUpdated > 20000 )
	{
		var timeStart = new Date().getTime();

		spawnerList = [];
		var count = IterateOver( "ITEM", socket );
		spawnerListUpdated = GetCurrentClock();

		var timeEnd = new Date().getTime();
		var totalTime = timeEnd - timeStart;

		let foundMsg = GetDictionaryEntry( 2631, socket.language ).replace( /%i/gi, spawnerList.length );
		socket.SysMessage( foundMsg.replace( /%u/gi, totalTime )); // Found %i item-based spawners in %u milliseconds.
	}

	DisplaySpawnerListGump( socket, filteredList, filterString, 0 );
}

function onIterate( toCheck, socket )
{
	if( socket == null )
		return false;

	if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.isSpawner )
	{
		if(( toCheck.type >= 61 && toCheck.type <= 65 ) || toCheck.type == 69 || toCheck.type == 125 )
		{
			spawnerList.push({
				buttonId: spawnerListButtonID,
				spawner: toCheck });
			spawnerListButtonID++;
			return true;
		}
	}
	return false;
}

function DisplaySpawnerListGump( socket, filteredList, incFilterString, spawnCountSort )
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
	var currentPage = parseInt( socket.currentChar.GetTempTag( "spawnerAdminCurrentPage" ));
	if( !currentPage || currentPage < 1 )
	{
		currentPage = 1;
	}

	var mySpawnerList = [];
	if( filteredList != null )
	{
		mySpawnerList = filteredList;
	}
	else
	{
		mySpawnerList = spawnerList;
	}

	if( spawnCountSort > 0 )
	{
		mySpawnerList.sort( function( a, b ) {
			// Sort based on amount of objects spawned
			return ( spawnCountSort == 1 ? a.spawner.amount - b.spawner.amount : b.spawner.amount - a.spawner.amount );
		});
	}
	else
	{
		mySpawnerList.sort( function( a, b ) {
			// Sort alphabetically
			return a.spawner.name.localeCompare( b.spawner.name );
		});
	}

	// Figure out the slice of the big list that we want on this page
	var startIndex = ( currentPage - 1 ) * pageSize;
	var endIndex   = startIndex + pageSize;
	var pageSlice  = mySpawnerList.slice( startIndex, endIndex );

	var spawnerListGump = new Gump();

	spawnerListGump.AddPage( 0 );
	spawnerListGump.AddBackground( 0, 0, 285, 615, 5054 );
	spawnerListGump.AddBackground( 29, 55, 250, 525, 5120 );
	spawnerListGump.AddBackground( 5, 30, 150, 25, 2620 ); // Tile White Background

	// Add transparency layer over the filter buttons as well to make them stand out less
	spawnerListGump.AddCheckerTrans( 0, 0, 285, 615 );
	var baseYPos = 60;

	// Generate quick-filter buttons for letters 'A' through 'Z'
	const letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for( let i = 0; i < letters.length; i++ )
	{
		let letter = letters[i];
		let yPos = baseYPos + ( i * 20 );
		let buttonID = 10 + i;

		// Add letter buttons + labels to gump
		spawnerListGump.AddButton( 5, yPos, 9028, 9026, 1, 0, buttonID );
		spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 2607, sockLang).replace( /%s/gi, letter )); // Filter for names starting with '%s'
		spawnerListGump.AddText(( letter == "I" ? 12 : 10 ), yPos, ( filterString == letter ? 100 : 50 ), letter );
	}

	spawnerListGump.AddButton( 250, 3, 4017, 4018, 1, 0, 3 ); // Close Menu Button
	spawnerListGump.AddHTMLGump( 15, 2, 240, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>" + GetDictionaryEntry( 2632, sockLang ) + "</BASEFONT></BIG></CENTER>" ); // Manage Spawn Objects

	// Loop over pageSlice, based on current page to show
	var quickButtons = 0;
	for( var i = 0; i < pageSlice.length; i++ )
	{
		// Spawner names/tweak button
		spawnerListGump.AddButton( 40, baseYPos + 5 + ( 26 * i ), 5031, 0x13ad, 1, 0, pageSlice[i].buttonId );
		var iRegion = pageSlice[i].spawner.region;

		// Tweak Spawn Object:<br>serial: %s<br>spawn-section: %w<br>Amount: %i<br>In region: (%u) %z
		let tweakSpawnObjMsg = GetDictionaryEntry( 2633, sockLang ).replace( /%s/gi, pageSlice[i].spawner.serial.toString() );
		tweakSpawnObjMsg = tweakSpawnObjMsg.replace( /%w/gi, pageSlice[i].spawner.spawnsection );
		tweakSpawnObjMsg = tweakSpawnObjMsg.replace( /%i/gi, pageSlice[i].spawner.amount );
		tweakSpawnObjMsg = tweakSpawnObjMsg.replace( /%u/gi, iRegion.id );
		spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, tweakSpawnObjMsg.replace( /%z/gi, iRegion.name ));
		var name = pageSlice[i].spawner.name;
		if( name.length > 15 )
		{
			name = name.substring( 0, 15 ) + "...";
		}
		spawnerListGump.AddText( 40, baseYPos + 5 + ( 26 * i ), 0, name );
		quickButtons++;

		// Teleport-to-spawner button
		spawnerListGump.AddButton( 152, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 1000000 + pageSlice[i].buttonId );
		spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 2634, sockLang )); // Teleport to Spawn Object
		spawnerListGump.AddText( 157, baseYPos + 5 + ( 26 * i ), 1000, "t" );
		quickButtons++;

		// Open Spawn Object Editor - only for Spawner Objects of type 61, 62, 69 or 125
		if( [61, 62, 69, 125].indexOf( pageSlice[i].spawner.type ) !== -1 )
		{
			spawnerListGump.AddButton( 172, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 2000000 + pageSlice[i].buttonId );
			spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 2635, sockLang )); // Open Spawn Object Editor
			spawnerListGump.AddText( 178, baseYPos + 5 + ( 26 * i ), 1000, "e" );
			quickButtons++;
		}

		// Delete spawner (with confirmation)
		spawnerListGump.AddButton( 252, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 3000000 + pageSlice[i].buttonId );
		spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 2636, sockLang )); // Remove Spawn Object
		spawnerListGump.AddText( 259, baseYPos + 5 + ( 26 * i ), 33, "r" );
		quickButtons++;
	}

	if( mySpawnerList.length == 0 )
	{
		spawnerListGump.AddText( 40, baseYPos + 5 + ( 26 * i ), 99, GetDictionaryEntry( 2621, sockLang )); // No entries found
		quickButtons++;
	}

	//var textEntryID = letters.length + ( pageSlice.length * 2 ) + 1;
	var textEntryID = letters.length + quickButtons + 1;

	// Pagination buttons if needed
	var totalPages = Math.ceil( mySpawnerList.length / pageSize );

	// Only show "Prev Page" if > page 1
	if( currentPage > 1 )
	{
		spawnerListGump.AddButton( 55, 585, 4014, 4015, 1, 0, 2 );
		spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 8124, sockLang )); // Previous Page
	}

	let pageString = GetDictionaryEntry( 2622, sockLang ).replace( /%i/gi, currentPage );
	pageString = pageString.replace( /%u/gi, totalPages );
	spawnerListGump.AddText( 110, 588, 50, pageString ); // Page %i / %u
	textEntryID++;

	// Only show "Next Page" if not on last page
	if( currentPage < totalPages )
	{
		spawnerListGump.AddButton( 215, 585, 4005, 4006, 1, 0, 1 );
		spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 8123, sockLang )); // Next Page
	}

	// Add text-entry filter at top of gump. TextEntry gump element needs to be last text-related gump element added
	spawnerListGump.AddTextEntryLimited( 15, 33, 150, 20, 55, 1, textEntryID, filterString, 15 );
	spawnerListGump.AddButton( 160, 30, ( filterString != "" ? 4012 : 4011 ), 4013, 1, 0, 4 ); // Filter Button
	spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 2632, sockLang )); // Apply Filter

	// Filter by spawner type
	var spawnObjListType = socket.currentChar.GetTempTag( "spawnObjListType" );
	spawnerListGump.AddButton( 195, 30, ( spawnObjListType == 1 ? 4009 : 4008 ), 4010, 1, 0, 5 ); // NPC Spawner Filter Button
	spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 2637, sockLang )); // Filter for NPC Spawners
	spawnerListGump.AddButton( 230, 30, ( spawnObjListType == 2 ? 4021 : 4020 ), 4022, 1, 0, 6 ); // Item Spawner Filter Button
	spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, GetDictionaryEntry( 2638, sockLang )); // Filter for Item Spawners

	// Sort by spawn count button
	var spawnCountSort = socket.currentChar.GetTempTag( "spawnAdminSpawnCountSort" );
	spawnerListGump.AddButton( 263, 32, ( spawnCountSort > 0 ? (( spawnCountSort == 1 || spawnCountSort == 2 ) ? 2647 : 2648 ) : 2648 ), 2646, 1, 0, 7 );

	let sortBySpawnAmt = GetDictionaryEntry( 2639, sockLang ).replace( /%s/gi, ( spawnCountSort > 0 ? ( spawnCountSort == 1 ? GetDictionaryEntry( 2627, sockLang ) : GetDictionaryEntry( 2628, sockLang )) : "N/A" )); // Sort by Spawn Count (%s) / Ascending/Descending
	spawnerListGump.AddToolTip( spawnerListTooltipClilocID, socket, sortBySpawnAmt );

	spawnerListGump.Send( socket );
	spawnerListGump.Free();
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
	confirmationGump.AddHTMLGump( 15, 4, 240, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>Spawn Object Removal</BASEFONT></BIG></CENTER>" );

	confirmationGump.AddHTMLGump( 15, 50, 240, 100, 0, 0, "<CENTER><BASEFONT color=#EECD8B>Please confirm removal of:</BASEFONT><br><BASEFONT color=#FFFFFF>" + chosenSpawnObj.name + " (" + chosenSpawnObj.serial.toString() + ")</BASEFONT><br><br><BASEFONT color=#FF240C>There is no undo button.</BASEFONT></CENTER>" );
	confirmationGump.AddButton( 50, 150, 247, 248, 1, 0, 8 ); // Confirm removal
	confirmationGump.AddButton( 165, 150, 241, 242, 1, 0, 9 ); // Cancel

	confirmationGump.Send( socket );
	confirmationGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;

	// Filter buttons
	var filterString = "";
	if( pButton >= 10 && pButton <= 36 )
	{
		let letterIndex = pButton - 10;
		let filterString = String.fromCharCode( 65 + letterIndex );

		// Don't forget to filter by spawner type
		var filteredList = FilterListBySpawnerType( spawnerList, pUser );

		var strictFilter = true;
		var filteredList = FilterListByName( filteredList, filterString, strictFilter );

		// Reset page to 1
		pUser.SetTempTag( "spawnerAdminCurrentPage", 1 );

		DisplaySpawnerListGump( socket, filteredList, filterString, 0 );
		return;
	}

	switch( pButton )
	{
		// Buttons 0-10, reserved for core gump functionality
		case 1: // Next page
			var pageNum = parseInt( pUser.GetTempTag( "spawnerAdminCurrentPage" ));
			if( !pageNum )
			{
				pageNum = 1;
			}
			pageNum = pageNum + 1;
			pUser.SetTempTag( "spawnerAdminCurrentPage", pageNum );

			var filteredList = FilterListBySpawnerType( spawnerList, pUser );

			// Re-apply filter to spawnerList
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			if( filterString && filterString.length > 0 )
			{
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}
			DisplaySpawnerListGump( socket, filteredList, filterString, 0 );
			break;
		case 2: // Prev page
			var pageNum = parseInt( pUser.GetTempTag( "spawnerAdminCurrentPage" ) );
			if( !pageNum )
			{
				pageNum = 1;
			}
			pageNum = pageNum - 1;
			if( pageNum < 1 )
			{
				pageNum = 1;
			}
			pUser.SetTempTag( "spawnerAdminCurrentPage", pageNum );

			var filteredList = FilterListBySpawnerType( spawnerList, pUser );

			// Re-apply filter to spawnerList
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			if( filterString && filterString.length > 0 )
			{
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}
			DisplaySpawnerListGump( socket, filteredList, filterString, 0 );
			break;
		case 0: // Do nothing, close gump
		case 3: // Do nothing, close gump
			break;
		case 4: // Filter by name
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');

			var filteredList = FilterListBySpawnerType( spawnerList, pUser );

			var strictFilter = false;
			if( filterString && filterString.length > 0 )
			{
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}

			DisplaySpawnerListGump( socket, filteredList, filterString, 0 );
			break;
		case 5: // Filter for NPC Spawners
			// Reset page to 1
			pUser.SetTempTag( "spawnerAdminCurrentPage", 1 );

			var filteredList = spawnerList;
			var spawnObjListType = pUser.GetTempTag( "spawnObjListType" );
			if( spawnObjListType == 1 )
			{
				pUser.SetTempTag( "spawnObjListType", null );
			}
			else
			{
				pUser.SetTempTag( "spawnObjListType", 1 );

				// Filter for NPC Spawners only
				filteredList = spawnerList.filter( function( spawnerObj ) {
					return ( spawnerObj.spawner.type === 62 || spawnerObj.spawner.type === 69 );
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

			DisplaySpawnerListGump( socket, filteredList, filterString, 0 );
			break;
		case 6: // Filter for Item Spawners
			// Reset page to 1
			pUser.SetTempTag( "spawnerAdminCurrentPage", 1 );

			var filteredList = spawnerList;
			var spawnObjListType = pUser.GetTempTag( "spawnObjListType" );
			if( spawnObjListType == 2 )
			{
				pUser.SetTempTag( "spawnObjListType", null );
			}
			else
			{
				pUser.SetTempTag( "spawnObjListType", 2 );

				// Filter for item Spawners only
				filteredList = spawnerList.filter( function( spawnerObj ) {
					return ( spawnerObj.spawner.type === 61 || spawnerObj.spawner.type === 63 || spawnerObj.spawner.type == 64 || spawnerObj.spawner.type == 65 );
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

			DisplaySpawnerListGump( socket, filteredList, filterString, 0 );
			break;
		case 7: // Sort by "spawn count"
			var filteredList = FilterListBySpawnerType( spawnerList, pUser );

			// Don't forget about the name filter
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			if( filterString && filterString.length > 0 )
			{
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}

			let spawnCountSort = pUser.GetTempTag( "spawnAdminSpawnCountSort" );
			spawnCountSort = ( spawnCountSort == 0 ? 1 : ( spawnCountSort == 1 ? 2 : ( spawnCountSort == 2 ? 0 : 0 )));
			pUser.SetTempTag( "spawnAdminSpawnCountSort", spawnCountSort );

			DisplaySpawnerListGump( socket, filteredList, filterString, spawnCountSort );
			break;
		case 8: // Confirm Removal of Spawn Object
			var objToRemove = CalcItemFromSer( parseInt( pUser.GetTempTag( "removeSpawnObject" )));
			if( ValidateObject( objToRemove ))
			{
				pUser.TextMessage( "Chosen spawn object (" + objToRemove.name + ", " + objToRemove.serial.toString() + ") has been removed.", false, 0x3b2, 0, pUser.serial );
				objToRemove.Delete();
			}

			pUser.StartTimer( 50, 0, this.script_id );
			break;
		case 9: // Cancel Removal of Spawn Object
			pUser.StartTimer( 50, 0, this.script_id );
			break;
		default:
			if( pButton >= 50 && pButton < 1000000 )
			{
				// Bring up Tweak menu for spawner
				var chosenSpawnerSerial = 0;
				for( var i = 0; i < spawnerList.length; i++ )
				{
					var spawnEntry = spawnerList[i];
					if( spawnEntry.buttonId == pButton )
					{
						chosenSpawnerSerial = spawnEntry.spawner.serial;
						break;
					}
				}

				if( chosenSpawnerSerial != 0 )
				{
					socket.xText = chosenSpawnerSerial.toString();
					pUser.ExecuteCommand( "tweak" );
				}
			}
			else if( pButton < 2000000 )
			{
				// Teleport to spawn object
				var chosenSpawnObj = null;
				for( var i = 0; i < spawnerList.length; i++ )
				{
					var spawnEntry = spawnerList[i];
					if( spawnEntry.buttonId == pButton - 1000000 )
					{
						chosenSpawnObj = spawnEntry.spawner;
						break;
					}
				}
				pUser.StartTimer( 50, 0, this.script_id );

				if( ValidateObject( chosenSpawnObj ))
				{
					pUser.Teleport( chosenSpawnObj );
				}
			}
			else if( pButton < 3000000 )
			{
				// Open Spawn Object Editor
				var chosenSpawnObj = null;
				for( var i = 0; i < spawnerList.length; i++ )
				{
					var spawnEntry = spawnerList[i];
					if( spawnEntry.buttonId == pButton - 2000000 )
					{
						chosenSpawnObj = spawnEntry.spawner;
						break;
					}
				}
				pUser.StartTimer( 50, 0, this.script_id );

				if( ValidateObject( chosenSpawnObj ))
				{
					TriggerEvent( 2205, "onUseChecked", pUser, chosenSpawnObj );
				}
			}
			else if( pButton < 4000000 )
			{
				// Remove Spawn Object - Ask for confirmation
				var chosenSpawnObj = null;
				for( var i = 0; i < spawnerList.length; i++ )
				{
					var spawnEntry = spawnerList[i];
					if( spawnEntry.buttonId == pButton - 3000000 )
					{
						chosenSpawnObj = spawnEntry.spawner;
						break;
					}
				}

				if( ValidateObject( chosenSpawnObj ))
				{
					socket.currentChar.SetTempTag( "removeSpawnObject", chosenSpawnObj.serial.toString() );
					DisplayConfirmationGump( socket, chosenSpawnObj );
				}
			}
			break;
		}
}

function FilterListBySpawnerType( listToFilter, pUser )
{
	var filteredList = listToFilter;
	var spawnObjListType = pUser.GetTempTag( "spawnObjListType" );
	if( spawnObjListType == 1 )
	{
		// Filter for NPC Spawners only
		filteredList = spawnerList.filter( function( spawnerObj ) {
			return ( spawnerObj.spawner.type === 62 || spawnerObj.spawner.type === 69 );
		});
	}
	else if( spawnObjListType == 2 )
	{
		// Filter for item Spawners only
		filteredList = spawnerList.filter( function( spawnerObj ) {
			return ( spawnerObj.spawner.type === 61 || spawnerObj.spawner.type === 63 || spawnerObj.spawner.type == 64 || spawnerObj.spawner.type == 65 );
		});
	}

	return filteredList;
}

function FilterListByName( listToFilter, filterString, strictFilter )
{
	var filterStringLow = filterString.toLowerCase();
	var filteredList = listToFilter.filter( function( spawnObj ) {
		// Convert both strings to lowercase, compare the first few chars
		return spawnObj.spawner.name
		  .substring( 0, filterString.length )
		  .toLowerCase() == filterStringLow;
	});

	if( !strictFilter && filteredList.length == 0 && filterString.length > 0 )
	{
		filteredList = listToFilter.filter( function( spawnObj ) {
			// Check if the lowercase name includes the lowercase filter string anywhere
			return spawnObj.spawner.name.toLowerCase().indexOf( filterStringLow ) !== -1;
		});
	}

	return filteredList;
}

function onTimer( timerObj, timerID )
{
	if( timerID == 0 )
	{
		DisplaySpawnerListGump( timerObj.socket, null, "", 0 );
	}
}

function _restorecontext_() {}