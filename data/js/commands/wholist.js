// Menu displaying all player characterss/NPCs in the world, with options for search/filter/edit

function CommandRegistration()
{
	RegisterCommand( "wholist", 2, true );
	RegisterCommand( "forcewho", 2, true ); // shortcut to 'wholist force
	RegisterCommand( "playeradmin", 2, true ); // alias, to match up with npcadmin/spawnadmin commands
	RegisterCommand( "npcadmin", 2, true ); // display all NPCs instead of players
}

var adminPlayerList = [];
var adminPlayerListUpdated = 0;
var adminNpcList = [];
var adminNpcListUpdated = 0;
const charAdminTooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day

function command_WHOLIST( socket, cmdString )
{
	ShowPlayerList( socket, cmdString, null, null );
}

function command_PLAYERADMIN( socket, cmdString )
{
	ShowPlayerList( socket, cmdString, null, null );
}

function command_FORCEWHO( socket, cmdString )
{
	ShowPlayerList( socket, "force", null, null );
}

function command_NPCADMIN( socket, cmdString )
{
	ShowNpcList( socket, "npcadmin", null, null );
}

function ShowPlayerList( socket, cmdString, filteredList, filterString )
{
	var gumpID = this.script_id + 0xffff;
	socket.CloseGump( gumpID, 0 );

	// Only refresh playerlist if it hasn't been updated in the last X seconds
	if( cmdString != "" || GetCurrentClock() - adminPlayerListUpdated > 20000 )
	{
		var timeStart = new Date().getTime();

		var splitString = cmdString.split( " " )[0].toUpperCase();
		if( splitString == "ONLINE" || splitString == "ON" || splitString == "" )
		{
			socket.tempInt2 = 1;
		}
		else if( splitString == "OFFLINE" || splitString == "OFF" )
		{
			socket.tempInt2 = -1;
		}
		else if( splitString == "FORCE" ) // Force refresh
		{
			socket.tempInt2 = 0;
		}

		adminPlayerList = [];
		var count = IterateOver( "CHARACTER", socket );
		adminPlayerListUpdated = GetCurrentClock();

		var timeEnd = new Date().getTime();
		var totalTime = timeEnd - timeStart;

		let foundMsg = GetDictionaryEntry( 2629, socket.language ).replace( /%i/gi, adminPlayerList.length );
		socket.SysMessage( foundMsg.replace( /%u/gi, totalTime )); // Found %i player characters in %u milliseconds.
	}

	socket.currentChar.SetTempTag( "listType", 1 ); // Player List
	let dateSort = socket.currentChar.GetTempTag( "wholistDateSort" );
	DisplayCharacterListGump( socket, filteredList, filterString, dateSort, 1 );
}

function ShowNpcList( socket, cmdString, filteredList, filterString )
{
	var gumpID = this.script_id + 0xffff;
	socket.CloseGump( gumpID, 0 );

	// Only refresh npclist if it hasn't been updated in the last X seconds
	if( cmdString != "" || GetCurrentClock() - adminNpcListUpdated > 20000 )
	{
		var timeStart = new Date().getTime();
		socket.tempInt2 = 2;

		adminNpcList = [];
		var count = IterateOver( "CHARACTER", socket );
		adminNpcListUpdated = GetCurrentClock();

		var timeEnd = new Date().getTime();
		var totalTime = timeEnd - timeStart;

		let foundMsg = GetDictionaryEntry( 2630, socket.language ).replace( /%i/gi, adminNpcList.length );
		socket.SysMessage( foundMsg.replace( /%u/gi, totalTime )); // Found %i NPCs in %u milliseconds.
	}

	socket.currentChar.SetTempTag( "listType", 2 ); // NPC List
	DisplayCharacterListGump( socket, filteredList, filterString, 0, 2 );
}

function onIterate( toCheck, socket )
{
	if( socket == null )
		return false;

	// Don't allow seeing characters with higher command level than yourself
	var findNPCs = ( socket.tempInt2 == 2 ? true : false );
	if( ValidateObject( toCheck ) && toCheck.isChar )
	{
		if( toCheck.npc && findNPCs )
		{
			adminNpcList.push( toCheck );
			return true;
		}
		else if( !toCheck.npc && ( toCheck == socket.currentChar || toCheck.commandlevel < socket.currentChar.commandlevel ))
		{
			if( socket.tempInt2 == 0 || ( socket.tempInt2 == 1 && toCheck.socket ) || ( socket.tempInt2 == -1 && !toCheck.socket ))
			{
				adminPlayerList.push( toCheck );
				return true;
			}
		}
	}
	return false;
}

function DisplayCharacterListGump( socket, filteredList, incFilterString, sortByDate, listType )
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
	var currentPage = ( listType == 1 ? parseInt( socket.currentChar.GetTempTag( "playerAdminCurrentPage" )) : parseInt( socket.currentChar.GetTempTag( "npcAdminCurrentPage" )));
	if( !currentPage || currentPage < 1 )
	{
		currentPage = 1;
	}

	var myCharList = [];
	if( filteredList != null )
	{
		myCharList = filteredList;
	}
	else
	{
		myCharList = ( listType == 1 ? adminPlayerList : adminNpcList );
	}

	if( listType == 1 && sortByDate > 0 )
	{
		myCharList.sort( function( a, b ) {
			// Sort by last-on date
			return ( sortByDate == 1 ? parseInt( a.lastOnSecs ) - parseInt( b.lastOnSecs ) : parseInt( b.lastOnSecs ) - parseInt( a.lastOnSecs ));
		});
	}
	else
	{
		myCharList.sort( function( a, b ) {
			// Sort alphabetically
			return a.name.localeCompare( b.name );
		});

	}

	// Pagination buttons if needed
	var totalPages = Math.ceil( myCharList.length / pageSize );
	if( currentPage > totalPages )
	{
		currentPage = 1;
		socket.currentChar.SetTempTag(( listType == 1 ? "playerAdminCurrentPage" : "npcAdminCurrentPage" ));
	}

	// Figure out the slice of the big list that we want on this page
	var startIndex = ( currentPage - 1 ) * pageSize;
	var endIndex   = startIndex + pageSize;
	var pageSlice  = myCharList.slice( startIndex, endIndex );

	var charListGump = new Gump();

	charListGump.AddPage( 0 );
	charListGump.AddBackground( 0, 0, 285, 615, 5054 );
	charListGump.AddBackground( 29, 55, 250, 525, 5120 );
	charListGump.AddBackground( 5, 30, 150, 25, 2620 ); // Tile White Background

	// Add transparency layer over the filter buttons as well to make them stand out less
	charListGump.AddCheckerTrans( 0, 0, 285, 615 );
	var baseYPos = 60;

	// Generate quick-filter buttons for letters 'A' through 'Z'
	const letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for( let i = 0; i < letters.length; i++ )
	{
		let letter = letters[i];
		let yPos = baseYPos + ( i * 20 );
		let buttonID = 10 + i;

		// Add letter buttons + labels to gump
		charListGump.AddButton( 5, yPos, 9028, 9026, 1, 0, buttonID );

		let letterFilterString = GetDictionaryEntry( 2607, sockLang ); // Filter for names starting with '%s'
		charListGump.AddToolTip( charAdminTooltipClilocID, socket, letterFilterString.replace( /%s/gi, letter ));
		charListGump.AddText(( letter == "I" ? 12 : 10 ), yPos, ( filterString == letter ? 100 : 50 ), letter );
	}

	charListGump.AddButton( 250, 3, 4017, 4018, 1, 0, 3 ); // Close Menu Button
	if( listType == 1 )
	{
		let allOnlineOffline = ( socket.tempInt2 == 0 ? GetDictionaryEntry( 2610, sockLang ) : ( socket.tempInt2 == 1 ? GetDictionaryEntry( 2611, sockLang ) : GetDictionaryEntry( 2612, sockLang )));
		charListGump.AddHTMLGump( 15, 2, 240, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>" + GetDictionaryEntry( 2608, sockLang ).replace( /%s/gi, allOnlineOffline ) + "</BASEFONT></BIG></CENTER>" ); // Manage Players - %s
	}
	else
	{
		charListGump.AddHTMLGump( 15, 2, 240, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>" + GetDictionaryEntry( 2609, sockLang ) + "</BASEFONT></BIG></CENTER>" ); // Manage NPCs
	}

	// Loop over pageSlice, based on current page to show
	var horizontalBtnCount = 0;
	for( var i = 0; i < pageSlice.length; i++ )
	{
		// Character names/tweak button
		charListGump.AddButton( 40, baseYPos + 5 + ( 26 * i ), 5031, 0x13ad, 1, 0, 50 + pageSlice[i].serial );

		let tweakBtnString = GetDictionaryEntry( 2613, sockLang );
		tweakBtnString = tweakBtnString.replace( /%s/gi, pageSlice[i].serial.toString() );
		tweakBtnString = tweakBtnString.replace( /%w/gi, ( pageSlice[i].npc ? "N/A" : pageSlice[i].lastOn ));
		charListGump.AddToolTip( charAdminTooltipClilocID, socket, tweakBtnString );
		var name = pageSlice[i].name;
		if( name.length > 15 )
		{
			name = name.substring( 0, 15 ) + "...";
		}
		charListGump.AddText( 40, baseYPos + 5 + ( 26 * i ), ( pageSlice[i].isGM ? 53 : ( pageSlice[i].murderer ? 32 : 0 )), name );

		let btnColor = 1000;
		if( pageSlice[i].serial == socket.currentChar.serial )
		{
			btnColor = 981;
		}

		// Teleport-to-character button
		if( pageSlice[i].serial != socket.currentChar.serial )
		{
			charListGump.AddButton( 152, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 1000000 + pageSlice[i].serial );
			charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2614, sockLang )); // Teleport to character
		}
		charListGump.AddText( 157, baseYPos + 5 + ( 26 * i ), btnColor, "t" );

		// Get/Fetch-character button
		if( pageSlice[i].serial != socket.currentChar.serial )
		{
			charListGump.AddButton( 172, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 2000000 + pageSlice[i].serial );
			charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2615, sockLang )); // Get character
		}
		charListGump.AddText( 178, baseYPos + 5 + ( 26 * i ), btnColor, "g" );

		// Inspect character
		charListGump.AddButton( 192, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 3000000 + pageSlice[i].serial );
		charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2616, sockLang )); // Inspect character
		charListGump.AddText( 200, baseYPos + 5 + ( 26 * i ), 1000, "i" );

		horizontalBtnCount = 4;

		if( listType == 1 ) // Players only
		{
			// Jail/Release character
			if( pageSlice[i].serial != socket.currentChar.serial )
			{
				charListGump.AddButton( 212, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 4000000 + pageSlice[i].serial );
				charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2617, sockLang )); // Jail/Release character
			}
			charListGump.AddText( 218, baseYPos + 5 + ( 26 * i ), btnColor, "j" );

			// Kick player
			charListGump.AddButton( 232, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 5000000 + pageSlice[i].serial );
			charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2618, sockLang )); // Kick player
			charListGump.AddText( 239, baseYPos + 5 + ( 26 * i ), 1000, "k" );

			// Ban player
			if( pageSlice[i].serial != socket.currentChar.serial )
			{
				charListGump.AddButton( 252, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 6000000 + pageSlice[i].serial );
				charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2619, sockLang )); // Ban Player
			}
			charListGump.AddText( 259, baseYPos + 5 + ( 26 * i ), btnColor, "b" );
			horizontalBtnCount += 3;
		}
		else
		{
			// NPC only
			// Delete NPC (with confirmation)
			charListGump.AddButton( 252, baseYPos + 4 + ( 26 * i ), 9028, 9026, 1, 0, 7000000 + pageSlice[i].serial );
			charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2620, sockLang )); // Remove NPC
			charListGump.AddText( 259, baseYPos + 5 + ( 26 * i ), 33, "r" );
			horizontalBtnCount++;
		}
	}

	var textEntryID = letters.length + ( pageSlice.length * horizontalBtnCount ) + 1;

	if( myCharList.length == 0 )
	{
		charListGump.AddText( 40, baseYPos + 5 + ( 26 * i ), 99, GetDictionaryEntry( 2621, sockLang )); // No entries found!
		textEntryID++;
	}

	// Only show "Prev Page" if > page 1
	if( currentPage > 1 )
	{
		charListGump.AddButton( 55, 585, 4014, 4015, 1, 0, 2 );
		charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 8124, socket.language )); // Previous Page
	}

	let pageString = GetDictionaryEntry( 2622, sockLang ).replace( /%i/gi, currentPage );
	pageString = pageString.replace( /%u/gi, totalPages );
	charListGump.AddText( 110, 588, 50, pageString ); // Page %i / %u
	textEntryID++;

	// Only show "Next Page" if not on last page
	if( currentPage < totalPages )
	{
		charListGump.AddButton( 215, 585, 4005, 4006, 1, 0, 1 );
		charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 8123, socket.language )); // Next Page
	}

	// Add text-entry filter at top of gump. TextEntry gump element needs to be last text-related gump element added
	charListGump.AddTextEntryLimited( 15, 33, 150, 20, 55, 1, textEntryID, filterString, 15 );
	charListGump.AddButton( 160, 30, ( filterString != "" ? 4012 : 4011 ), 4013, 1, 0, 4 ); // Filter Button
	charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2623, sockLang )); // Apply Filter

	// Online/Offline filter buttons
	if( listType == 1 ) // Players only
	{
		charListGump.AddButton( 195, 30, ( socket.tempInt2 == 1 ? 4009 : 4008 ), 4010, 1, 0, 5 ); // Online Filter Button
		charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2624, sockLang )); // Filter for Online Players
		charListGump.AddButton( 230, 30, ( socket.tempInt2 == -1 ? 4021 : 4020 ), 4022, 1, 0, 6 ); // Offline Filter Button
		charListGump.AddToolTip( charAdminTooltipClilocID, socket, GetDictionaryEntry( 2625, sockLang )); // Filter for Offline Players

		// Sort by date button
		var dateSort = socket.currentChar.GetTempTag( "wholistDateSort" );
		charListGump.AddButton( 263, 32, ( dateSort > 0 ? (( dateSort == 1 || dateSort == 2 ) ? 2647 : 2648 ) : 2648 ), 2646, 1, 0, 7 ); // Sort by last on time

		let sortByDateStr = GetDictionaryEntry( 2626, sockLang ).replace( /%s/gi, ( dateSort > 0 ? ( dateSort == 1 ? GetDictionaryEntry( 2627, sockLang ) : GetDictionaryEntry( 2628, sockLang )) : "N/A" )); // Sort by Last On (%s) / Ascending/Descending
		charListGump.AddToolTip( charAdminTooltipClilocID, socket, sortByDateStr );
	}

	charListGump.Send( socket );
	charListGump.Free();
}

function DisplayConfirmationGump( socket, npcToRemove )
{
	var confirmationGump = new Gump();

	confirmationGump.AddPage( 0 );
	confirmationGump.AddBackground( 0, 0, 285, 185, 5054 );
	confirmationGump.AddBackground( 10, 35, 255, 110, 5120 );

	// Add transparency layer over the filter buttons as well to make them stand out less
	confirmationGump.AddCheckerTrans( 0, 0, 285, 185 );

	confirmationGump.AddButton( 250, 3, 4017, 4018, 1, 0, 3 ); // Close Menu Button
	confirmationGump.AddHTMLGump( 15, 4, 240, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>NPC Removal</BASEFONT></BIG></CENTER>" );

	let rmvConfirmString = GetDictionaryEntry( 2605, socket.language );
	rmvConfirmString = rmvConfirmString.replace( /%s/gi, npcToRemove.name );
	rmvConfirmString = rmvConfirmString.replace( /%w/gi, npcToRemove.serial.toString() );
	let rmvNoUndoBtnString = GetDictionaryEntry( 2606, socket.language );

	confirmationGump.AddHTMLGump( 15, 50, 240, 100, 0, 0, "<CENTER>" + rmvConfirmString + "<br><br>" + rmvNoUndoBtnString + "</CENTER>" );
	confirmationGump.AddButton( 50, 150, 247, 248, 1, 0, 8 ); // Confirm removal
	confirmationGump.AddButton( 165, 150, 241, 242, 1, 0, 9 ); // Cancel

	confirmationGump.Send( socket );
	confirmationGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var listType = pUser.GetTempTag( "listType" );

	// Filter buttons
	var filterString = "";
	if( pButton >= 10 && pButton <= 36 )
	{
		let letterIndex = pButton - 10;
		let filterString = String.fromCharCode( 65 + letterIndex );

		var strictFilter = true;
		var filteredList = ( listType == 1 ? adminPlayerList : adminNpcList );
		filteredList = FilterListByName( filteredList, filterString, strictFilter )

		// Reset page to 1
		pUser.SetTempTag(( listType == 1 ? "playerAdminCurrentPage" : "npcAdminCurrentPage" ), 1 );

		DisplayCharacterListGump( socket, filteredList, filterString, 0, listType );
		return;
	}

	let dateSort = pUser.GetTempTag( "wholistDateSort" );
	switch( pButton )
	{
		// Buttons 0-10, reserved for core gump functionality
		case 1: // Next page
			var pageNum = parseInt( pUser.GetTempTag(( listType == 1 ? "playerAdminCurrentPage" : "npcAdminCurrentPage" )));
			if( !pageNum )
			{
				pageNum = 1;
			}
			pageNum = pageNum + 1;
			pUser.SetTempTag(( listType == 1 ? "playerAdminCurrentPage" : "npcAdminCurrentPage" ), pageNum );

			// Re-apply filter to playerlist
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			var finalList = ( listType == 1 ? adminPlayerList : adminNpcList );
			if( filterString && filterString.length > 0 )
			{
				finalList = FilterListByName( finalList, filterString, strictFilter )
			}

			DisplayCharacterListGump( socket, finalList, filterString, dateSort, listType );
			break;
		case 2: // Prev page
			var pageNum = parseInt( pUser.GetTempTag(( listType == 1 ? "playerAdminCurrentPage" : "npcAdminCurrentPage" )));
			if( !pageNum )
			{
				pageNum = 1;
			}
			pageNum = pageNum - 1;
			if( pageNum < 1 )
			{
				pageNum = 1;
			}
			pUser.SetTempTag(( listType == 1 ? "playerAdminCurrentPage" : "npcAdminCurrentPage" ), pageNum );

			// Re-apply filter to playerlist
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			var finalList = ( listType == 1 ? adminPlayerList : adminNpcList );
			if( filterString && filterString.length > 0 )
			{
				finalList = FilterListByName( finalList, filterString, strictFilter )
			}

			DisplayCharacterListGump( socket, finalList, filterString, dateSort, listType );
			break;
		case 0: // Do nothing, close gump
		case 3: // Do nothing, close gump
			break;
		case 4: // Filter by name
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');

			var strictFilter = false;
			var filteredList = ( listType == 1 ? adminPlayerList : adminNpcList );
			if( filterString && filterString.length > 0 )
			{
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}

			DisplayCharacterListGump( socket, filteredList, filterString, 0, listType );
			break;
		case 5: // Filter for online players
			// Reset page to 1
			pUser.SetTempTag( "playerAdminCurrentPage", 1 );

			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			if( socket.tempInt2 != 1 )
			{
				ShowPlayerList( socket, "online", null, filterString )
			}
			else
			{
				ShowPlayerList( socket, "force", null, filterString )
			}
			break;
		case 6: // Filter for offline players
			// Reset page to 1
			pUser.SetTempTag( "playerAdminCurrentPage", 1 );

			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			if( socket.tempInt2 != -1 )
			{
				ShowPlayerList( socket, "offline", null, filterString )
			}
			else
			{
				ShowPlayerList( socket, "force", null, filterString )
			}
			break;
		case 7: // Sort by "last on" timestamp
			// First sort by name
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');

			var strictFilter = false;
			var filteredList = ( listType == 1 ? adminPlayerList : adminNpcList );
			if( filterString && filterString.length > 0 )
			{
				filteredList = FilterListByName( filteredList, filterString, strictFilter );
			}

			// Determine new date sort mode
			// 0 = no date sort, 1 = ascending, 2 = descending
			let newDateSort = ( dateSort == 0 ? 1 : ( dateSort == 1 ? 2 : ( dateSort == 2 ? 0 : 0 )));
			pUser.SetTempTag( "wholistDateSort", newDateSort );

			DisplayCharacterListGump( socket, filteredList, filterString, newDateSort, listType );
			break;
		case 8: // Confirm Removal of NPC
			var npcToRemove = CalcCharFromSer( parseInt( pUser.GetTempTag( "npcToRemove" )));
			if( ValidateObject( npcToRemove ))
			{
				let rmvMsg = GetDictionaryEntry( 2600, socket.language ); // Chosen NPC (%s, %w) has been removed.
				rmvMsg = rmvMsg.replace( /%s/gi, npcToRemove.name );
				pUser.TextMessage( rmvMsg.replace( /%w/gi, npcToRemove.serial.toString() ), false, 0x3b2, 0, pUser.serial );
				npcToRemove.Delete();
			}

			// Re-apply filter to playerlist
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			var finalList = ( listType == 1 ? adminPlayerList : adminNpcList );
			if( filterString && filterString.length > 0 )
			{
				finalList = FilterListByName( finalList, filterString, strictFilter )
			}

			DisplayCharacterListGump( socket, finalList, filterString, 0, listType );
			pUser.StartTimer( 50, 0, this.script_id );
			break;
		case 9: // Cancel Removal of NPC
			// Re-apply filter to playerlist
			filterString = gumpData.getEdit( 0 ).replace(/^\s+/, '');
			var strictFilter = false;
			var finalList = ( listType == 1 ? adminPlayerList : adminNpcList );
			if( filterString && filterString.length > 0 )
			{
				finalList = FilterListByName( finalList, filterString, strictFilter )
			}

			DisplayCharacterListGump( socket, finalList, filterString, 0, listType );
			break;
		default:
			// Buttons 11 to 999999, reserved for character serials
			if( pButton >= 50 && pButton < 1000000 )
			{
				// Bring up Tweak menu for character
				socket.SetDWord( 7, pButton - 50 );
				pUser.ExecuteCommand( "tweak" );
			}
			else if( pButton < 2000000 )
			{
				// Teleport to character
				var targSerial = pButton - 1000000;
				var targChar = CalcCharFromSer( targSerial );
				pUser.StartTimer( 50, 0, this.script_id );
				if( ValidateObject( targChar ))
				{
					pUser.Teleport( targChar );
				}
			}
			else if( pButton < 3000000 )
			{
				// Get/Teleport character to GM
				var targSerial = pButton - 2000000;
				var targChar = CalcCharFromSer( targSerial );
				pUser.StartTimer( 50, 0, this.script_id );
				if( ValidateObject( targChar ))
				{
					pUser.ExecuteCommand( "xteleport 0x" + targSerial.toString(16) );
				}
			}
			else if( pButton < 4000000 )
			{
				// Inspect character
				var targSerial = pButton - 3000000;
				var targChar = CalcCharFromSer( targSerial );
				pUser.StartTimer( 50, 0, this.script_id );
				if( ValidateObject( targChar ))
				{
					// Inspect backpack
					var iPack = targChar.pack;
					if( ValidateObject( iPack ))
					{
						SendObjectInfoPacket( socket, iPack );
						SendWornItemPacket( socket, iPack, targChar );
						targChar.OpenLayer( socket, 0x15 );
					}
					else
					{
						pUser.TextMessage( GetDictionaryEntry( 2601, socket.language ), false, 0x3b2, 0, pUser.serial ); // Warning: No backpack found on character!
					}

					// Inspect bankbox
					if( !targChar.npc )
					{
						var iBankBox = targChar.FindItemLayer( 0x1D );
						if( ValidateObject( iBankBox ))
						{
							SendObjectInfoPacket( socket, iBankBox );
							SendWornItemPacket( socket, iBankBox, targChar );
							targChar.OpenLayer( socket, 0x1d );
						}
						else
						{
							pUser.TextMessage( GetDictionaryEntry( 2602, socket.language ), false, 0x3b2, 0, pUser.serial ); // Warning: No bank box found for character!
						}
					}

					// Inspect paperdoll
					if( pUser.DistanceTo( targChar ) <= 24 )
					{
						targChar.Refresh( socket );
						OpenPaperdoll( socket, targChar );
					}
					else
					{
						let paperdollOutofRange = GetDictionaryEntry( 2603, socket.language ); // Too far away to inspect paperdoll (current range: %i)
						pUser.TextMessage( paperdollOutofRange.replace( /%i/gi, pUser.DistanceTo( targChar )), false, 0x3b2, 0, pUser.serial );
					}
				}
				pUser.StartTimer( 50, 0, this.script_id );
			}
			else if( pButton < 5000000 )
			{
				// Jail/Release character
				var targSerial = pButton - 4000000;
				var targChar = CalcCharFromSer( targSerial );
				pUser.StartTimer( 50, 0, this.script_id );
				if( ValidateObject( targChar ))
				{
					if( !targChar.isJailed )
					{
						targChar.Jail( 86400 ); // Jail target for 24 hours
						var tempMsg = GetDictionaryEntry( 8087, socket.language ); // %s has been jailed for %i hours.
						tempMsg = tempMsg.replace( /%s/gi, targChar.name );
						socket.SysMessage( tempMsg.replace( /%i/gi, ( 86400 / 60 / 60 )));
					}
					else
					{
						targChar.Release();
						var tempMsg = GetDictionaryEntry( 8088, socket.language ); // Released %s from jail.
						socket.SysMessage( tempMsg.replace( /%s/gi, targChar.name ));
					}
				}
			}
			else if( pButton < 6000000 )
			{
				// Kick player
				var targSerial = pButton - 5000000;
				var targChar = CalcCharFromSer( targSerial );
				pUser.StartTimer( 50, 0, this.script_id );
				if( ValidateObject( targChar ))
				{
					if( targChar.socket )
					{
						socket.SysMessage( GetDictionaryEntry( 1029, socket.language ) ); // Kicking player!
						targSock.SysMessage( GetDictionaryEntry( 1030, targChar.socket.language ) ); // You have been kicked!
						targChar.Disconnect();
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 9042, socket.language )); // That player is not online.
					}
				}
			}
			else if( pButton < 7000000 )
			{
				// Ban/Unban player
				var targSerial = pButton - 6000000;
				var targChar = CalcCharFromSer( targSerial );
				pUser.StartTimer( 50, 0, this.script_id );
				if( ValidateObject( targChar ))
				{
					if( targChar.account && !targChar.account.isBanned )
					{
						var timebanDuration = 60 * 24;
						targChar.account.isBanned = true;
						targChar.account.timeban = timebanDuration;

						// Disconnect player, if online
						if( ValidateObject( targChar ) && targChar.account.isOnline && targChar.socket != null )
						{
							var banMsg = GetDictionaryEntry( 2017, targChar.socket.language ); // You have been banned for %d minutes!
							targChar.SysMessage( banMsg, timebanDuration );
							targChar.socket.Disconnect();
						}

						socket.SysMessage( GetDictionaryEntry( 2016, socket.language ), targChar.account.id, timebanDuration ); // Account with ID #%d has been banned for %d minutes.
					}
					else if( targChar.account )
					{
						targChar.account.isBanned = false;
						targChar.account.timeban = 0;
						socket.SysMessage( GetDictionaryEntry( 2020, socket.language ), targChar.account.id ); // Account with ID #%d has been unbanned.
					}
				}
			}
			else if( pButton < 8000000 )
			{
				// Remove NPC - with confirmation
				var targSerial = pButton - 7000000;
				var targChar = CalcCharFromSer( targSerial );
				if( ValidateObject( targChar ))
				{
					socket.currentChar.SetTempTag( "npcToRemove", targChar.serial.toString() );
					DisplayConfirmationGump( socket, targChar );
				}
			}
			break;
	}
}

function FilterListByName( listToFilter, filterString, strictFilter )
{
	var filterStringLow = filterString.toLowerCase();
	var filteredList = listToFilter.filter( function( charObj ) {
		// Convert both strings to lowercase, compare the first few chars
		return charObj.name
		  .substring( 0, filterString.length )
		  .toLowerCase() == filterStringLow;
	});

	if( !strictFilter && filteredList.length == 0 && filterString.length > 0 )
	{
		filteredList = listToFilter.filter( function( charObj ) {
			// Check if the lowercase name includes the lowercase filter string anywhere
			return charObj.name.toLowerCase().indexOf( filterStringLow ) !== -1;
		});
	}

	return filteredList;
}

function onTimer( timerObj, timerID )
{
	if( timerID != 0 )
	{
		timerObj.socket.tempObj.OpenLayer( timerObj.socket, 0x15 );
	}

	var listType = timerObj.GetTempTag( "listType" );
	DisplayCharacterListGump( timerObj.socket, null, "", 0, listType );
}

function OpenPaperdoll( socket, targChar )
{
	// Create packet to open a paperdoll
	var pStream = new Packet;
	pStream.ReserveSize( 66 );
	pStream.WriteByte( 0, 0x88 );
	pStream.WriteLong( 1, targChar.serial );
	pStream.WriteString( 5, targChar.name + ( targChar.title ? " " + targChar.title : "" ), targChar.name.length + ( targChar.title ? targChar.title.length + 1 : 0 ));
	pStream.WriteByte( 65, 0x2 );
	socket.Send( pStream );
	pStream.Free();
}

function SendObjectInfoPacket( socket, objectToSend )
{
	if( !objectToSend )
		return;

	// Create a packet to send object info even though target char is out of range
	var pStream = new Packet;
	if( socket.clientType <= 9 )
	{
		pStream.ReserveSize( 24 ); // 24 bytes SA
	}
	else
	{
		pStream.ReserveSize( 26 ); // 26 bytes HS
	}
	pStream.WriteByte( 0, 0xf3 ); // cmd
	pStream.WriteShort( 1, 0x1 ); // Always 0x1
	pStream.WriteByte( 3, objectToSend.isItem ? 0x00 : 0x01 ); // 0x00 = item, 0x01 = character, 0x02 = Multi
	pStream.WriteLong( 4, objectToSend.serial );
	pStream.WriteShort( 8, objectToSend.id );
	pStream.WriteByte( 10, 0x00 ); // offset/facing, 0x00 if multi
	pStream.WriteShort( 11, 1 ); // amount
	pStream.WriteShort( 13, 1 ); // amount, again
	pStream.WriteShort( 15, objectToSend.x ); // x
	pStream.WriteShort( 17, objectToSend.y ); // y
	pStream.WriteByte( 19, objectToSend.z ); // Z
	pStream.WriteByte( 20, objectToSend.dir ); // direction/light level
	pStream.WriteShort( 21, objectToSend.colour ); // colour
	pStream.WriteByte( 23, 0x20 ); // flag
	if( socket.clientType >= 10 )
	{
		pStream.WriteShort( 24, 0x00 ); // Extra 2 bytes for HS and beyond
	}
	socket.Send( pStream );
	pStream.Free();
}

function SendWornItemPacket( socket, objectToSend, targChar )
{
	if( !objectToSend )
		return;

	// Create a packet to send worn item details even though target char is out of range
	var pStream = new Packet;
	pStream.ReserveSize( 15 );
	pStream.WriteByte( 0, 0x2e );
	pStream.WriteLong( 1, objectToSend.serial );
	pStream.WriteShort( 5, objectToSend.id );
	pStream.WriteByte( 7, 0x00 );
	pStream.WriteByte( 8, objectToSend.layer );
	pStream.WriteLong( 9, targChar.serial );
	pStream.WriteShort( 13, objectToSend.colour );
	socket.Send( pStream );
	pStream.Free();
}

function _restorecontext_() {}