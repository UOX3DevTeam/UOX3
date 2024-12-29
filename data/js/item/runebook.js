// Runebook.js (v1.01)
// Rewritten mostly from scratch by Xuri (xuri@uox3.org)
// Inspired by the original Runebook script by Rais
// Last Updated 14th of September, 2022
//
// Runebooks can either be crafted via the Inscription skill, or added by GMs using this command:
// 'add item runebook

var maxCharges = 16;		// Default maximum amount of charges a runebook can hold
const showCoords = true; 	// Show regular coordinates in tooltip above latitude/longitude values
const scriptID = 5029;		// Script ID assigned to this script in jse_fileassociations.scp
const useDelay = 7000; 		// 7 seconds between each time a runebook can be used
const tooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day

function onUseChecked( pUser, runeBook )
{
	// Use maximum charges set on runebook, if any
	var runeBookMaxCharges = runeBook.maxhp;
	if( runeBookMaxCharges > 0 )
	{
		maxCharges = runeBookMaxCharges;
	}

	// get users socket
	var pSocket = pUser.socket;

	var iTime = GetCurrentClock();
	var NextUse = runeBook.GetTag( "useDelayed" );
	var Delay = 7000;

	if( ( iTime - NextUse ) < Delay )
	{
		pSocket.SysMessage( GetDictionaryEntry( 9250, pSocket.language )); // This book needs time to recharge.
		return false;
	}

	// Get root container for Runebook
	var iRootContainer = FindRootContainer( runeBook );
	if( !ValidateObject( iRootContainer ))
	{
		// No root container. Runebook is on the ground/floor!
		// Check if user is within range
		if( !runeBook.InRange( pUser, 3 ))
		{
	  		pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away.
			return false;
		}
	}
	else
	{
		// Root container found. Who does it belong to?
		if( iRootContainer != pUser.pack )
		{
			var ownerChar = GetPackOwner( iRootContainer, 0 );
			if( ValidateObject( ownerChar ))
			{
				// It's in someone else's backpack
				pSocket.SysMessage( GetDictionaryEntry( 9251, pSocket.language )); // You cannot use a Runebook from someone else's backpack!
				return false;
			}
			else if( !iRootContainer.InRange( pUser, 3 ))
			{
				// Check distance to container holding the Runebook
			  	pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away.
				return false;
			}
		}
	}

	// Initial checks passed, let's see if someone is already using the book
	var runeBookInUse = runeBook.GetTag( "inUse" );
	if( runeBookInUse )
	{
		var userSerial = runeBook.GetTag( "userSerial" );
		if( userSerial != 0 )
		{
			var runeBookUser = CalcCharFromSer( userSerial & 0x00FFFFFF );
			if( ValidateObject( runeBookUser ) && runeBookUser != pUser )
			{
				// Check that the other user is online and within range of the book
				if( runeBookUser.online && runeBookUser.InRange( runeBook, 3 ))
				{
					pSocket.SysMessage( GetDictionaryEntry( 2450, pSocket.language )); // Someone else is using this right now.
					return false;
				}
			}
		}
	}

	// Set player as new user of runeBook
	runeBook.SetTag( "inUse", true );
	runeBook.SetTag( "userSerial", ( pUser.serial ).toString() );

	// Make a noise as the gump opens
	pUser.SoundEffect( 0x58, false );

	// Display the gump
	DisplayGump( pSocket, pUser, runeBook );
	return false;
}

function DisplayGump( pSocket, pUser, runeBook )
{
	var runeBookCharges = runeBook.dir;
	var defaultRuneLoc = runeBook.GetTag( "defaultRuneLoc" );

	var runeBookGump = new Gump;
	runeBookGump.AddPage( 0 );
	runeBookGump.AddGump( 0, 0, 0x898 );

	// Display front page
	runeBookGump.AddHTMLGump( 40, 30, 120, 25, 1, 1, "<BASEFONT size=4>" + GetDictionaryEntry( 9252, pSocket.language ) + " " + runeBookCharges +  "</BASEFONT>" ); // Charges:
	runeBookGump.AddHTMLGump( 200, 30, 130, 25, 1, 1, "<BASEFONT size=4>" + GetDictionaryEntry( 9253, pSocket.language ) + " " + maxCharges + "</BASEFONT>" ); // Max Charges:

	// Add numbered buttons at bottom of gump, pointing at different pages
	runeBookGump.AddButton( 31, 178, 0x8b1, 0, 2, 0 );
	runeBookGump.AddButton( 66, 178, 0x8b2, 0, 3, 0 );
	runeBookGump.AddButton( 101, 178, 0x8b3, 0, 4, 0 );
	runeBookGump.AddButton( 136, 178, 0x8b4, 0, 5, 0 );
	runeBookGump.AddButton( 201, 178, 0x8b5, 0, 6, 0 );
	runeBookGump.AddButton( 236, 178, 0x8b6, 0, 7, 0 );
	runeBookGump.AddButton( 271, 178, 0x8b7, 0, 8, 0 );
	runeBookGump.AddButton( 306, 178, 0x8b8, 0, 9, 0 );
	// Mask the buttons with gumps to hide the button press
	runeBookGump.AddGump( 31, 178, 0x8b1 );
	runeBookGump.AddGump( 66, 178, 0x8b2 );
	runeBookGump.AddGump( 101, 178, 0x8b3 );
	runeBookGump.AddGump( 136, 178, 0x8b4 );
	runeBookGump.AddGump( 201, 178, 0x8b5 );
	runeBookGump.AddGump( 236, 178, 0x8b6 );
	runeBookGump.AddGump( 271, 178, 0x8b7 );
	runeBookGump.AddGump( 306, 178, 0x8b8 );

	// Add decorative lines
	runeBookGump.AddGump( 25, 40, 0x39 ); // Draw the DECORATIVE line under charges
	runeBookGump.AddGump( 55, 40, 0x3a );
	runeBookGump.AddGump( 71, 40, 0x3a );
	runeBookGump.AddGump( 87, 40, 0x3a );
	runeBookGump.AddGump( 103, 40, 0x3a );
	runeBookGump.AddGump( 119, 40, 0x3a );
	runeBookGump.AddGump( 129, 40, 0x3b );
	runeBookGump.AddGump( 189, 40, 0x39 ); //and the same on facing page
	runeBookGump.AddGump( 219, 40, 0x3a );
	runeBookGump.AddGump( 235, 40, 0x3a );
	runeBookGump.AddGump( 251, 40, 0x3a );
	runeBookGump.AddGump( 267, 40, 0x3a );
	runeBookGump.AddGump( 283, 40, 0x3a );
	runeBookGump.AddGump( 288, 40, 0x3a );
	runeBookGump.AddGump( 298, 40, 0x3b );

	runeBookGump.AddPage( 1 );

	runeBookGump.AddButton( 25, 8, 0x9aa, 1, 0, 20 ); // Rename Book button
	runeBookGump.AddHTMLGump( 55, 10, 100, 25, 1, 1, "<BASEFONT size=4>" + GetDictionaryEntry( 9254, pSocket.language ) + "</BASEFONT>" ); // Rename Book

	// Add turn page button and masking gump
	runeBookGump.AddButton( 293, 5, 0x89e, 0, 2, 0 );
	runeBookGump.AddGump( 293, 5, 0x89e );

	var leftBtnOffsetX = 28;
	var leftBtnOffsetY = 56;
	var leftTxtOffsetX = 41;
	var leftTxtOffsetY = 52;

	var rightBtnOffsetX = 192;
	var rightBtnOffsetY = 56;
	var rightTxtOffsetX = 203;
	var rightTxtOffsetY = 52;

	// Add shortcut entries to runes on first page
	for( var i = 1; i <= 16; i++ )
	{
		// Fetch rune data from tag on runebook
		var runeData = runeBook.GetTag( "rune" + i + "Data" );
		var splitData = isNaN( runeData ) ? runeData.split( "," ) : 0;

		if( i <= 8 )
		{
			// Left hand side of first two pages
			if( runeData == 0 || splitData == 0 )
			{
				runeBookGump.AddHTMLGump( leftTxtOffsetX, leftTxtOffsetY + ( 15 * ( i - 1 )), 120, 20, 1, 1, "<BASEFONT size=5>" + GetDictionaryEntry( 9255, pSocket.language ) + "</BASEFONT>" ); // Empty
			}
			else
			{
				runeBookGump.AddHTMLGump( leftTxtOffsetX, leftTxtOffsetY + ( 15 * ( i - 1)), 120, 20, 1, 1, "<BASEFONT color=#0xfd size=10>" + splitData[0] + "</BASEFONT>" );
				runeBookGump.AddToolTip( tooltipClilocID, pSocket, splitData[0] );
			}
			runeBookGump.AddButton( leftBtnOffsetX, leftBtnOffsetY + ( 15 * ( i - 1 )), 0x837, 1, 0, 30 + i );
		}
		else
		{
			// Right hand side of first two page
			if( runeData == 0 || splitData == 0 )
			{
				runeBookGump.AddHTMLGump( rightTxtOffsetX, rightTxtOffsetY + ( 15 * ( i - 9 )), 120, 20, 1, 1, "<BASEFONT size=5>" + GetDictionaryEntry( 9255, pSocket.language ) + "</BASEFONT>" ); // Empty
			}
			else
			{
				runeBookGump.AddHTMLGump( rightTxtOffsetX, rightTxtOffsetY + ( 15 * ( i - 9 )), 120, 20, 1, 1, "<BASEFONT color=0xfd size=5>" + splitData[0] + "</BASEFONT>" );
				runeBookGump.AddToolTip( tooltipClilocID, pSocket, splitData[0] );
			}
			runeBookGump.AddButton( rightBtnOffsetX, rightBtnOffsetY + ( 15 * ( i - 9 )), 0x837, 1, 0, 30 + i );
		}
	}

	// Add the other pages of the runebook
	for( i = 0; i <= 7; i++ )
	{
		var runeData = runeBook.GetTag( "rune" + (( i * 2 ) + 1 ) + "Data" );
		var splitData = isNaN( runeData ) ? runeData.split( "," ) : 0;

		runeBookGump.AddPage( i + 2 );

		switch( i )
		{
			case 0:
				runeBookGump.AddHTMLGump( 38, 178, 40, 20, 1, 1, "<BASEFONT size=5>1</BASEFONT>" );
				break;
			case 1:
				runeBookGump.AddHTMLGump( 70, 178, 40, 20, 1, 1, "<BASEFONT size=5>2</BASEFONT>" );
				break;
			case 2:
				runeBookGump.AddHTMLGump( 105, 178, 40, 20, 1, 1, "<BASEFONT size=5>3</BASEFONT>" );
				break;
			case 3:
				runeBookGump.AddHTMLGump( 139, 178, 40, 20, 1, 1, "<BASEFONT size=5>4</BASEFONT>" );
				break;
			case 4:
				runeBookGump.AddHTMLGump( 207, 178, 40, 20, 1, 1, "<BASEFONT size=5>5</BASEFONT>" );
				break;
			case 5:
				runeBookGump.AddHTMLGump( 241, 178, 40, 20, 1, 1, "<BASEFONT size=5>6</BASEFONT>" );
				break;
			case 6:
				runeBookGump.AddHTMLGump( 275, 178, 40, 20, 1, 1, "<BASEFONT size=5>7</BASEFONT>" );
				break;
			case 7:
				runeBookGump.AddHTMLGump( 310, 178, 40, 20, 1, 1, "<BASEFONT size=5>8</BASEFONT>" );
				break;
		}

		// Add next page button on all pages except last one
		if( i < 7 )
		{
			runeBookGump.AddButton( 293, 5, 0x89e, 0, i + 3, 0 );
			runeBookGump.AddGump( 293, 5, 0x89e );
		}

		// Add previous page button on all pages except first one
		runeBookGump.AddButton( 25, 5, 0x89d, 0, i + 1, 0 );//Turn Page button
		runeBookGump.AddGump( 25, 5, 0x89d );

		// Add Set Default button
		var setDefaultBtn = 0x939;
		if( defaultRuneLoc == (( i * 2 ) + 1 ))
		{
			setDefaultBtn = 0x938;
		}

		runeBookGump.AddButton( 60, 10, setDefaultBtn, 1, 0, ( i * 2 ) + 1 ); // ButtonIDs 1, 3, 5, 7, 9, 11, 13, 15
	  	runeBookGump.AddHTMLGump( 74, 6, 120, 40, 1, 1, "<BASEFONT size=2>" + GetDictionaryEntry( 9256, pSocket.language ) + "</BASEFONT>" ); // Set Default

	  	// Add internal recall button for rune
		runeBookGump.AddButton( 28, 56, 0x837, 1, 0, ( i * 2 ) + 31 ); // ButtonIDs 31, 33, 35, 37, 39, 41, 43, 45

		// Add name of rune, and location
		if( splitData == 0 )
		{
	  		runeBookGump.AddHTMLGump( 41, 52, 120, 25, 1, 1, "<BASEFONT size=5>" + GetDictionaryEntry( 9255, pSocket.language ) + "</BASEFONT>" ); // Empty
		}
	  	else
	  	{
		  	let mapCoords = TriggerEvent( 2503, "GetMapCoordinates", parseInt( splitData[2] ), parseInt( splitData[3] ), parseInt( splitData[5] ))
		  	let xLongDeg = mapCoords[0];
		  	let xLongMin = mapCoords[1];
		  	let xEast = mapCoords[2];

		  	// let yLatitude = GetLatitude( parseInt(splitData[2]), parseInt(splitData[3]), parseInt(splitData[5]) );
		  	let yLatDeg = mapCoords[3];
		  	let yLatMin = mapCoords[4];
		  	let ySouth = mapCoords[5];

		  	runeBookGump.AddHTMLGump( 41, 52, 130, 40, 1, 1, "<BASEFONT color=#0xfd size=5>" + splitData[0] + "</BASEFONT>" );
		  	runeBookGump.AddToolTip( tooltipClilocID, pSocket, splitData[0] );

		  	runeBookGump.AddHTMLGump( 35, 90, 125, 25, 1, 1, "<CENTER><BASEFONT size=3>" + yLatDeg + "o " + yLatMin + "'" + ( ySouth ? "N" : "S" ) + " " + xLongDeg + "o " + xLongMin + "'" + ( xEast ? "W" : "E" ) + "</BASEFONT></CENTER>" );
		  	if( showCoords )
		  	{
		  		runeBookGump.AddToolTip( tooltipClilocID, pSocket, splitData[2] + ", " + splitData[3].toString() );
		  	}
	  	}

	  	// Add drop rune button
		runeBookGump.AddButton( 35, 110, 0x985, 1, 0, ( 101 + ( i * 2 ))); // ButtonIDs 101, 103, 105, 107, 109, 111, 113, 115
		runeBookGump.AddHTMLGump( 45, 110, 100, 25, 1, 1, "<BASEFONT size=5>" + GetDictionaryEntry( 9257, pSocket.language ) + "</BASEFONT>" ); // Drop Rune

		// Add Recall buttons
		runeBookGump.AddButton( 35, 130, 0x8df, 1, 0, ( 121 + ( i * 2 ))); // ButtonIDs 121, 123, 125, 127, 129, 131, 133, 135
		runeBookGump.AddGump( 35, 130, 0x8df );

		// Add Gate buttons
		runeBookGump.AddButton( 110, 130, 0x8f3, 1, 0, ( 141 + ( i * 2 ))); // ButtonIDs 141, 143, 145, 147, 149, 151, 153, 155
		runeBookGump.AddGump( 110, 130, 0x8f3 );

		// -------------------- //

		// Handle opposite page, same approach
	  	runeData = runeBook.GetTag( "rune" + (( i * 2 ) + 2 ) + "Data" );
	  	splitData = isNaN( runeData ) ? runeData.split( "," ) : 0;

	  	// Add Set Default button
		setDefaultBtn = 0x939;
		if( defaultRuneLoc == (( i * 2 ) + 2 ))
		{
			setDefaultBtn = 0x938;
		}
		runeBookGump.AddButton( 190, 10, setDefaultBtn, 1, 0, ( i * 2 ) + 2 ); // ButtonIDs 2, 4, 6, 8, 10, 12, 14, 16
	  	runeBookGump.AddHTMLGump( 204, 6, 120, 40, 1, 1, "<BASEFONT size=2>" + GetDictionaryEntry( 9256, pSocket.language ) + "</BASEFONT>" ); // Set Default

		// Add internal recall button for rune
	  	runeBookGump.AddButton( 192, 56, 0x837, 1, 0, ( i * 2 ) + 32 ); // ButtonIDs 32, 34, 36, 38, 40, 42, 44, 46

		// Add name of rune, and location
		if( splitData == 0 )
		{
		  	runeBookGump.AddHTMLGump( 203, 52, 120, 25, 1, 1, "<BASEFONT size=5>" + GetDictionaryEntry( 9255, pSocket.language ) + "</BASEFONT>" ); // Empty
		}
		else
		{
		  	let mapCoords = TriggerEvent( 2503, "GetMapCoordinates", parseInt( splitData[2] ), parseInt( splitData[3] ), parseInt( splitData[5] ))
		  	let xLongDeg = mapCoords[0];
		  	let xLongMin = mapCoords[1];
		  	let xEast = mapCoords[2];

		  	// let yLatitude = GetLatitude( parseInt(splitData[2]), parseInt(splitData[3]), parseInt(splitData[5]) );
		  	let yLatDeg = mapCoords[3];
		  	let yLatMin = mapCoords[4];
		  	let ySouth = mapCoords[5];

		  	runeBookGump.AddHTMLGump( 203, 52, 120, 50, 1, 1, "<BASEFONT color=#0xfd size=5>" + splitData[0] + "</BASEFONT>" );
		  	runeBookGump.AddToolTip( tooltipClilocID, pSocket, splitData[0] );

		  	runeBookGump.AddHTMLGump( 195, 90, 125, 25, 1, 1, "<CENTER><BASEFONT size=3>" + yLatDeg + "o " + yLatMin + "'" + ( ySouth ? "S" : "N" ) + " " + xLongDeg + "o " + xLongMin + "'" + ( xEast ? "E" : "W" ) + "</BASEFONT></CENTER>" );
		  	if( showCoords )
		  	{
		  		runeBookGump.AddToolTip( tooltipClilocID, pSocket, splitData[2] + ", " + splitData[3].toString() );
		  	}
		}

	  	// Add drop rune button
		runeBookGump.AddButton( 193, 110, 0x985, 1, 0, ( 102 + ( i * 2 ))); // ButtonIDs 102, 104, 106, 108, 110, 112, 114, 116
	  	runeBookGump.AddHTMLGump( 203, 110, 100, 25, 1, 1, "<BASEFONT size=5>" + GetDictionaryEntry( 9257, pSocket.language ) + "</BASEFONT>" ); // Drop Rune

	  	// Add Recall button
		runeBookGump.AddButton( 193, 130, 0x8df, 1, 0, ( 122 + ( i * 2 ))); // ButtonIDs 122, 124, 126, 128, 130, 132, 134, 136
		runeBookGump.AddGump( 193, 130, 0x8df );

		// Add Gate button
		runeBookGump.AddButton( 268, 130, 0x8f3, 1, 0, ( 142 + ( i * 2 ))); // ButtonIDs 142, 144, 146, 148, 150, 152, 154, 156
		runeBookGump.AddGump( 268, 130, 0x8f3 );
	}

	// Let's pass the runeBook along for the ride
	pSocket.tempObj2 = runeBook;

	runeBookGump.Send( pSocket );
	runeBookGump.Free();
}

function onGumpPress( pSocket, myButton, gumpData )
{
	if( pSocket == null )
		return;

	var runeBook = pSocket.tempObj2;
	pSocket.tempObj2 = null;
	if( !ValidateObject( runeBook ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 9258, pSocket.language )); // Unable to detect Runebook. Does it still exist?
		return;
	}

	var pUser = pSocket.currentChar;
	if( !pUser.InRange( runeBook, 3 ))
	{
		var iRootContainer = FindRootContainer( runeBook );
		if( ValidateObject( iRootContainer ))
		{
			if( !pUser.InRange( iRootContainer, 3 ))
			{
		  		pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away.
				runeBook.SetTag( "inUse", null );
				runeBook.SetTag( "userSerial", null );
				return false;
			}
		}
		else
		{
	  		pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away.
			runeBook.SetTag( "inUse", null );
			runeBook.SetTag( "userSerial", null );
			return false;
		}
	}

	switch( myButton )
	{
		case 0: // Close gump
			runeBook.SetTag( "inUse", null );
			runeBook.SetTag( "userSerial", null );
			break;
		case 1: // Set Rune #1 as Default
		case 2: // Set Rune #2 as Default
		case 3: // Set Rune #3 as Default
		case 4: // Set Rune #4 as Default
		case 5: // Set Rune #5 as Default
		case 6: // Set Rune #6 as Default
		case 7: // Set Rune #7 as Default
		case 8: // Set Rune #8 as Default
		case 9: // Set Rune #9 as Default
		case 10: // Set Rune #10 as Default
		case 11: // Set Rune #11 as Default
		case 12: // Set Rune #12 as Default
		case 13: // Set Rune #13 as Default
		case 14: // Set Rune #14 as Default
		case 15: // Set Rune #15 as Default
		case 16: // Set Rune #16 as Default
		{
			// Verify that player can change default location for runebook
			if( !CheckAccessRights( pSocket, pUser, runeBook ))
				return;

			var runeData = runeBook.GetTag( "rune" + ( myButton ) + "Data" );
			if( runeData != 0 )
			{
				var splitData = runeData.split( "," );
				runeBook.morex = splitData[2]; // x
				runeBook.morey = splitData[3]; // y
				runeBook.morez = splitData[4]; // z
				runeBook.more = splitData[5]; // worldNumber
				if( splitData[6] )
				{
					runeBook.more0 = splitData[6]; // instanceID
				}
				runeBook.SetTag( "defaultRuneLoc", myButton );
				pSocket.SysMessage( GetDictionaryEntry( 9259, pSocket.language )); // New default location set.
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 9260, pSocket.language )); // This place in the book is empty.
			}
			break;
		}
		case 20: // Rename Runebook
		{
			// Verify that player can rename the runebook
			if( !CheckAccessRights( pSocket, pUser, runeBook ))
				return;

			pSocket.SysMessage( GetDictionaryEntry( 9261, pSocket.language )); // Please enter a title for the runebook:
		  	pUser.SpeechInput( 1, runeBook );
			break;
		}
		case 31: // Recall from rune #1 using runebook charge
		case 32: // Recall from rune #2 using runebook charge
		case 33: // Recall from rune #3 using runebook charge
		case 34: // Recall from rune #4 using runebook charge
		case 35: // Recall from rune #5 using runebook charge
		case 36: // Recall from rune #6 using runebook charge
		case 37: // Recall from rune #7 using runebook charge
		case 38: // Recall from rune #8 using runebook charge
		case 39: // Recall from rune #9 using runebook charge
		case 40: // Recall from rune #10 using runebook charge
		case 41: // Recall from rune #11 using runebook charge
		case 42: // Recall from rune #12 using runebook charge
		case 43: // Recall from rune #13 using runebook charge
		case 44: // Recall from rune #14 using runebook charge
		case 45: // Recall from rune #15 using runebook charge
		case 46: // Recall from rune #16 using runebook charge
			if( runeBook.dir <= 0 )
			{
				pSocket.SysMessage( GetDictionaryEntry( 9262, pSocket.language )); // This has no more charges.
				runeBook.dir = 0;
				return;
			}

			// Fetch data for the relevant rune
			var runeData = runeBook.GetTag( "rune" + ( myButton - 30 ) + "Data" );
			if( runeData != 0 )
			{
				var splitData = runeData.split( "," );
				var xLoc = splitData[2];
				var yLoc = splitData[3];
				var zLoc = splitData[4];
				var worldNum = splitData[5];

				// Reduce amount of charges left in runebook
				runeBook.dir -= 1;

				// Teleport user to coordinates stored in rune
				//pUser.Teleport( parseInt( xLoc ), parseInt( yLoc ), parseInt(zLoc ), parseInt( worldNum ));
				pSocket.tempObj2 = runeBook;
				pSocket.tempInt2 = ( myButton - 30 );
				runeBook.SetTag( "inUse", null );
				runeBook.SetTag( "userSerial", null );
				var iTime = GetCurrentClock();
				runeBook.SetTag( "useDelayed", iTime.toString() );
				CastSpell( pSocket, pUser, 32, false );
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 9260, pSocket.language )); // This place in the book is empty.
			}
			break;
		case 101: // Drop Rune #1
		case 102: // Drop Rune #2
		case 103: // Drop Rune #3
		case 104: // Drop Rune #4
		case 105: // Drop Rune #5
		case 106: // Drop Rune #6
		case 107: // Drop Rune #7
		case 108: // Drop Rune #8
		case 109: // Drop Rune #9
		case 110: // Drop Rune #10
		case 111: // Drop Rune #11
		case 112: // Drop Rune #12
		case 113: // Drop Rune #13
		case 114: // Drop Rune #14
		case 115: // Drop Rune #15
		case 116: // Drop Rune #16
		{
			// Check that the player is allowed to drop runes from this runebook
			if( !CheckAccessRights( pSocket, pUser, runeBook ))
				return;

			// Check that the player's backpack can contain a new rune
			var pPack = pUser.pack;
			if( pPack.totalItemCount >= pPack.maxItems || pPack.weight >= pPack.weightMax )
			{
				pSocket.SysMessage( GetDictionaryEntry( 9263, pSocket.language )); // You have no room for a rune in your backpack.
				return;
			}

			// Fetch data for the relevant rune
			var runeData = runeBook.GetTag( "rune" + ( myButton - 100 ) + "Data" );
			if( runeData != 0 )
			{
				var splitData = runeData.split( "," );
				var droppedRune = CreateDFNItem( pSocket, pUser, "0x1f14", 1, "ITEM", true )
				droppedRune.morex = parseInt( splitData[2] ); // x
				droppedRune.morey = parseInt( splitData[3] ); // y
				droppedRune.morez = parseInt( splitData[4] ); // z
				droppedRune.more = parseInt( splitData[5] ); // worldNumber
				if( splitData[6] )
				{
					droppedRune.more0 = parseInt( splitData[6] ); // instanceId
				}
				droppedRune.name = splitData[0];
				if( runeBook.GetTag( "defaultRuneLoc" ) == ( myButton - 100 ))
				{
					runeBook.SetTag( "defaultRuneLoc", null );
					runeBook.morex = 0;
					runeBook.morey = 0;
					runeBook.morez = 0;
					runeBook.more = 0;
					runeBook.more0 = 0;
				}
				runeBook.SetTag( "rune" + ( myButton - 100 ) + "Data", null );

				// Reduce runecount in book
				var runeCount = parseInt( runeBook.GetTag( "runeCount" ));
				runeBook.SetTag( "runeCount", runeCount - 1 );
				pSocket.SysMessage( GetDictionaryEntry( 9264, pSocket.language )); // You have removed the rune.
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 9265, pSocket.language )); // There is no rune to be dropped.
			}
			break;
		}
		case 121: // Cast recall spell on rune slot #1
		case 122: // Cast recall spell on rune slot #2
		case 123: // Cast recall spell on rune slot #3
		case 124: // Cast recall spell on rune slot #4
		case 125: // Cast recall spell on rune slot #5
		case 126: // Cast recall spell on rune slot #6
		case 127: // Cast recall spell on rune slot #7
		case 128: // Cast recall spell on rune slot #8
		case 129: // Cast recall spell on rune slot #9
		case 130: // Cast recall spell on rune slot #10
		case 131: // Cast recall spell on rune slot #11
		case 132: // Cast recall spell on rune slot #12
		case 133: // Cast recall spell on rune slot #13
		case 134: // Cast recall spell on rune slot #14
		case 135: // Cast recall spell on rune slot #15
		case 136: // Cast recall spell on rune slot #16
			var runeData = runeBook.GetTag( "rune" + ( myButton - 120 ) + "Data" );
			if( runeData != 0 )
			{
				pSocket.tempObj2 = runeBook;
				pSocket.tempInt2 = ( myButton - 120 );
				runeBook.SetTag( "inUse", null );
				runeBook.SetTag( "userSerial", null );
				var iTime = GetCurrentClock();
				runeBook.SetTag( "useDelayed", iTime.toString() );
				CastSpell( pSocket, pUser, 32, true );
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 9260, pSocket.language )); // This place in the book is empty.
			}
			break;
		case 141: // Cast gate spell on rune slot #1
		case 142: // Cast gate spell on rune slot #2
		case 143: // Cast gate spell on rune slot #3
		case 144: // Cast gate spell on rune slot #4
		case 145: // Cast gate spell on rune slot #5
		case 146: // Cast gate spell on rune slot #6
		case 147: // Cast gate spell on rune slot #7
		case 148: // Cast gate spell on rune slot #8
		case 149: // Cast gate spell on rune slot #9
		case 150: // Cast gate spell on rune slot #10
		case 151: // Cast gate spell on rune slot #11
		case 152: // Cast gate spell on rune slot #12
		case 153: // Cast gate spell on rune slot #13
		case 154: // Cast gate spell on rune slot #14
		case 155: // Cast gate spell on rune slot #15
		case 156: // Cast gate spell on rune slot #16
			var runeData = runeBook.GetTag( "rune" + ( myButton - 140 ) + "Data" );
			if( runeData != 0 )
			{
				pSocket.tempObj2 = runeBook;
				pSocket.tempInt2 = ( myButton - 140 );
				runeBook.SetTag( "inUse", null );
				runeBook.SetTag( "userSerial", null );
				var iTime = GetCurrentClock();
				runeBook.SetTag( "useDelayed", iTime.toString() );
				CastSpell( pSocket, pUser, 52, true );
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 9260, pSocket.language )); // This place in the book is empty.
			}
			break;
	}
}

function CastSpell( pSocket, pUser, spellNum, checkReagentReq )
{
	var runeBook = pSocket.tempObj2;
	var runeNum = pSocket.tempInt2;
	var runeData = runeBook.GetTag( "rune" + runeNum + "Data" );
	var splitData = runeData.split( "," );
	var targLocX = parseInt(splitData[2]);
	var targLocY = parseInt(splitData[3]);
	var targWorld = parseInt(splitData[5]);
	var targInstanceID = 0;

	if( splitData[6] )
	{
		targInstanceID = parseInt(splitData[6]);
	}

	// Are we already casting?
	if( pUser.GetTimer( Timer.SPELLTIME ) != 0 )
	{
		if( pUser.isCasting )
		{
			pSocket.SysMessage( GetDictionaryEntry( 762, pSocket.language )); // You are already casting a spell.
			return;
		}
		else if( pUser.GetTimer( Timer.SPELLTIME ) > GetCurrentClock() )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1638, pSocket.language )); // You must wait a little while before casting
			return;
		}
	}

	// Is player trying to recall in jail?
	if( pUser.isJailed )
	{
		pSocket.SysMessage( GetDictionaryEntry( 704, pSocket.language ));
		return;
	}

	//Checks if they can travell in the region.
	if( TriggerEvent( 6002, "CheckTravelRestrictions", pUser, spellNum, targLocX, targLocY, targWorld, targInstanceID ))
	{
		return;
	}

	// Is the spell we're trying to cast enabled?
	var mSpell	= Spells[spellNum];
	if( !mSpell.enabled )
	{
		pSocket.SysMessage( GetDictionaryEntry( 707, pSocket.language ));
		return;
	}

	// Does player actually have the spell they're trying to cast?
	if( checkReagentReq && !pUser.HasSpell( spellNum ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 9266, pSocket.language )); // You don't have that spell.
		return;
	}

	if( !GetServerSetting( "TravelSpellsWhileAggressor" ) && ( pUser.IsAggressor() || pUser.criminal ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 2066, pSocket.language )); // You are not allowed to use Recall or Gate spells while being the aggressor in a fight!
		return;
	}

	// Is the player casting recall holding any objects?
	var itemRHand = pUser.FindItemLayer( 0x01 );
	var itemLHand = pUser.FindItemLayer( 0x02 );
	if( ValidateObject( itemLHand ) && itemLHand.type != 119 || ( ValidateObject( itemRHand ) && ( itemRHand.type != 9 || itemRHand.type != 119 )))    // Spellbook //spell channeling
	{
		pSocket.SysMessage( GetDictionaryEntry( 708, pSocket.language )); // You cannot cast with a weapon equipped.
		return;
	}

	// Does player have enough reagents to cast the spell?
	if( checkReagentReq && !TriggerEvent( 6004, "CheckReagents", pUser, mSpell))
		return;

	// Make sure player has enough of the required stats to cast the spell
	if( mSpell.mana > pUser.mana )
	{
		if( pSocket )
		{
			pSocket.SysMessage( GetDictionaryEntry( 696, pSocket.language )); // You have insufficient mana to cast that spell.
		}
		return;
	}
	if( mSpell.stamina > pUser.stamina )
	{
		if( pSocket )
		{
			pSocket.SysMessage( GetDictionaryEntry( 697, pSocket.language )); // You have insufficient stamina to cast that spell.
		}
		return;
	}
	if( mSpell.health >= pUser.health )
	{
		if( pSocket )
		{
			pSocket.SysMessage( GetDictionaryEntry( 698, pSocket.language )); // You have insufficient health to cast that spell.
		}
		return;
	}

	// Turn character visible if they're not already visible
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	// Break character's concentration (affects Meditation skill)
	if( pSocket )
	{
		pUser.BreakConcentration( pSocket );
	}

	pUser.spellCast = spellNum;
	pUser.nextAct = 75;		// why 75?

	// Set cast time delay on player and freeze them to the spot
	var delay = mSpell.delay * 100;
	pUser.SetTimer( Timer.SPELLTIME, delay );
	pUser.frozen = true;

	// If they're not mounted, play casting animations
	if( !pUser.isonhorse )
	{
		var actionID = mSpell.action;
		if( pUser.isHuman || actionID != 0x22 )
		{
			pUser.DoAction( actionID );
		}
	}

	// Make sure player has enough skill to cast spell
	var lowSkill	= mSpell.lowSkill;
	var highSkill	= mSpell.highSkill;
	if( !pUser.CheckSkill( 25, lowSkill, highSkill ))
	{
		pUser.TextMessage( mSpell.mantra );
		if( checkReagentReq )
		{
			TriggerEvent( 6004, "DeleteReagents", pUser, mSpell );
		}
		pUser.SpellFail();
		pUser.isCasting = false;
		pUser.frozen 	= false;
		pUser.SetTimer( Timer.SPELLTIME, 0 );
		pUser.spellCast = -1;
		return;
	}

	// Subtract mana/other stats
	pUser.mana 		= pUser.mana - mSpell.mana;
	pUser.health 	= pUser.health - mSpell.health;
	pUser.stamina	= pUser.stamina - mSpell.stamina;

	// Delete reagents if needed
	if( checkReagentReq )
	{
		TriggerEvent( 6004, "DeleteReagents", pUser, mSpell );
	}

	pUser.TextMessage( mSpell.mantra );
	pUser.isCasting = true;

	pUser.StartTimer( delay, spellNum, true );
}

function onTimer( timerObj, timerID )
{
	var pSocket = timerObj.socket;
	if( !pSocket )
		return;

	var runeBook = pSocket.tempObj2;
	pSocket.tempObj2 = null;
	if( !ValidateObject( runeBook ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 9267, pSocket.language )); // That does not seem to be a valid Runebook.
		return;
	}

	var runeNum = pSocket.tempInt2;
	if( isNaN( runeNum ) || runeNum < 0 || runeNum > 16 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 9267, pSocket.language )); // That does not seem to be a valid location.
		return;
	}

	var runeData = runeBook.GetTag( "rune" + runeNum + "Data" );
	var splitData = runeData.split( "," );
	var targLocX = parseInt(splitData[2]);
	var targLocY = parseInt(splitData[3]);
	var targLocZ = parseInt(splitData[4]);
	var targWorld = parseInt(splitData[5]);
	var targInstanceID = 0;
	if( splitData[6] )
	{
		targInstanceID = parseInt(splitData[6]);
	}

	// Do mana/reagent cost
	var spellNum	= timerObj.spellCast;
	var mSpell		= Spells[spellNum];

	// Reset casting for user
	timerObj.isCasting = false;
	timerObj.frozen = false;
	timerObj.SetTimer( Timer.SPELLTIME, 0 );
	timerObj.spellCast = -1;

	// Play sound effect, and remove temporary rune
	timerObj.SoundEffect( mSpell.soundEffect, true );
	timerObj.SpellStaticEffect( mSpell );

	// Handle effect of spell
	if( spellNum == 32 ) // Recall spell
	{
		TriggerEvent( 6003, "TeleportHelper", timerObj, targLocX, targLocY, targLocZ, targWorld, targInstanceID, true )
	}
	else
	{
		// Gate spell
		timerObj.Gate( targLocX, targLocY, targLocZ, targWorld, targInstanceID );
	}
}

function CheckAccessRights( pSocket, pUser, runeBook )
{
	var iRootContainer = FindRootContainer( runeBook );
	if( ValidateObject( iRootContainer ))
	{
		if( ValidateObject( iRootContainer.owner ) && iRootContainer.owner != pUser )
		{
			pSocket.SysMessage( GetDictionaryEntry( 9268, pSocket.language )); // That is inaccessible.
			return false;
		}

	}

	if( runeBook.movable == 3 ) // Runebook is locked down
	{
		pSocket.SysMessage( GetDictionaryEntry( 9269, pSocket.language )); // That cannot be done while the book is locked down.
		return false;
	}
	return true;
}

// Handle renaming of Runebook
function onSpeechInput( pUser, runeBook, pSpeech, pSpeechID )
{
	var pSocket = pUser.socket;
	if( pSpeech == null || pSpeech == " " )
	{
		pSocket.SysMessage( GetDictionaryEntry( 9270, pSocket.language )); // That name is too short, or no name was entered.
		return;
	}

	switch( pSpeechID )
	{
		case 1: // Rename Runebook
			if( pSpeech.length > 50 )
			{
				pUser.SysMessage( GetDictionaryEntry( 9271, pSocket.language )); // That name is too long. Maximum 50 chars.
				return;
			}

			if( ValidateObject( runeBook ))
			{
				runeBook.name = pSpeech;
				var tempMsg = GetDictionaryEntry( 9272, pSocket.language ); // The new name of the Runebook is: %s
				pSocket.SysMessage( tempMsg.replace( /%s/gi, runeBook.name ));
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 9267, pSocket.language )); // That does not seem to be a valid Runebook.
			}
			break;
		default:
			break;
	}
}

const maxRunes = 16;
function onDropItemOnItem( iDropped, pUser, runeBook )
{
	var pSocket = pUser.socket;

	if( iDropped.type == 50 ) // Recall Rune dropped on runeBook
	{
		// Verify that player is allowed to add runes to the runebook
		if( !CheckAccessRights( pSocket, pUser, runeBook ))
			return false;

		// Prevent player from adding a blank rune to the runebook
		if( iDropped.morex == 0 && iDropped.morey == 0 && iDropped.morez == 0 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 431, pUser.socket.language )); // That rune is not yet marked!
			return false;
		}

		// Make sure there's space in the runebook for the new rune
		var runeCount = parseInt( runeBook.GetTag( "runeCount" ));
		if( runeCount >= maxRunes )
		{
			pSocket.SysMessage( GetDictionaryEntry( 9273, pSocket.language )); // This runebook is full.
			return false;
		}

		// Find a free spot in the runebook, and add the rune there
		for( var i = 1; i <= maxRunes; i++ )
		{
			var runeData = runeBook.GetTag( "rune" + i + "Data" );
			if( runeData == 0 )
			{
				var xLoc = iDropped.morex;
				var yLoc = iDropped.morey;
				var zLoc = iDropped.morez;
				var worldNum = iDropped.more;
				var instanceID = iDropped.more0;
				var iName = iDropped.name;
				var iColor = iDropped.color;
				runeBook.SetTag( "rune" + i + "Data", iName + "," + iColor + "," + xLoc + "," + yLoc + "," + zLoc + "," + worldNum + "," + instanceID );
				runeBook.SetTag( "runeCount", runeCount + 1 );
				iDropped.Delete();

				var tempMsg = GetDictionaryEntry( 9274, pSocket.language ); // You've added a rune named %s to your Runebook.
				pSocket.SysMessage( tempMsg.replace( /%s/gi, iName ));
				pSocket.CloseGump( 0xffff + scriptID, 0 );
				onUseChecked( pUser, runeBook );
				return 2;
			}
		}
	}
	else if( iDropped.id == 0x1F4C ) // Recall Scroll
	{
		var charges = runeBook.dir;
		if( charges >= maxCharges )
		{
			pSocket.SysMessage( GetDictionaryEntry( 9275, pSocket.language )); // This book already has the maximum amount of charges.
			return false;
		}

		// Is the player dropping a stack of scrolls?
		if( iDropped.amount > 1 )
		{
			// Can we use all the scrolls in the stack to charge the runebook?
			if( iDropped.amount <= maxCharges - charges )
			{
				// Let's charge the runebook with as many scrolls as we can, and delete the stack
				runeBook.dir = charges + iDropped.amount;
				iDropped.Delete();
			}
			else
			{
				// There's more scrolls in the stack than we can use to charge the runebook
				// Increase charges on runebook to max, and reduce amount of scrolls in stack accordingly
				runeBook.dir = maxCharges;
				iDropped.amount -= maxCharges - charges;
			}

			pSocket.SysMessage( GetDictionaryEntry( 9276, pSocket.language )); // You charge the Runebook with a stack of recall scroll.
		}
		else
		{
			// It's just a single scroll. Charge the runebook, delete the scroll!
			runeBook.dir = charges + 1;
			iDropped.Delete();
			pSocket.SysMessage( GetDictionaryEntry( 9277, pSocket.language )); // You charge the Runebook with a recall scroll.
		}

		pSocket.CloseGump( 0xffff + scriptID, 0 );
		onUseChecked( pUser, runeBook );
		return 2;
	}

	return true;
}

function GetMapCoordinates( xCoord, yCoord, worldNum )
{
	var resultArray = new Array();
	var xCenter = 0;
	var yCenter = 0;
	var xWidth = 5120;
	var yHeight = 4096;

	switch( worldNum )
	{
		case 0: // Trammel
		case 1: // Felucca
			if( xCoord >= 0 && yCoord >= 0 && xCoord < 5120 && yCoord < 4096 )
			{
				xCenter = 1323;
				yCenter = 1624;
			}
			else if( xCoord >= 5120 && yCoord >= 0 && xCoord < 7168 && yCoord < 4096)
			{
				xCenter = 5936;
				yCenter = 3112;
			}
			else
			{
				resultArray[0] = "-1";
				resultArray[1] = "-1";
				resultArray[2] = true;
				resultArray[3] = "-1";
				resultArray[4] = "-1";
				resultArray[5] = true;
				return xLongitude;
			}
			break;
		case 2: // Ilshenar
		case 3: // Tokuno
		case 4: // Malas
		case 5: // Termur
			if( xCoord >= 0 && yCoord >= 0 && xCoord < 7168 && yCoord < 4096 )
			{
				xCenter = 1323;
				yCenter = 1624;
			}
			else
			{
				resultArray[0] = "-1";
				resultArray[1] = "-1";
				resultArray[2] = true;
				resultArray[3] = "-1";
				resultArray[4] = "-1";
				resultArray[5] = true;
				return xLongitude;
			}
			break;
		default:
			break;
	}

	// Longitude
	var absLong = (( xCoord - xCenter ) * 360 ) / xWidth;
	if( absLong > 180 )
	{
		absLong = -180 + ( absLong % 180 );
	}

	var xEast = ( absLong >= 0 );

	if( absLong < 0 )
	{
		absLong = -absLong;
	}

	var xLongDeg = Math.round( absLong );
	var xLongMin = Math.round(( absLong % 1 ) * 60 );

	// Latitude
	var absLat = (( yCoord - yCenter ) * 360 ) / yHeight;
	if( absLat > 180 )
	{
		absLat = -180 + ( absLat % 180 );
	}

	var ySouth = ( absLat >= 0 );

	if( absLat < 0 )
	{
		absLat = -absLat;
	}

	var yLatDeg = Math.round( absLat );
	var yLatMin = Math.round(( absLat % 1 ) * 60 );

	// Build return array
	resultArray[0] = xLongDeg;
	resultArray[1] = xLongMin;
	resultArray[2] = xEast;
	resultArray[3] = yLatDeg;
	resultArray[4] = yLatMin;
	resultArray[5] = ySouth;

	return resultArray;
}

function _restorecontext_() {}