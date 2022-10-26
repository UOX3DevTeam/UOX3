// Global Script
// Supported Events trigger for every character/item, use with care
function onLogin( socket, pChar )
{
	// Display Admin Welcome Gump for characters on admin account, until a choice has been made
	if( pChar.accountNum == 0 )
	{
		// Check if startupChoice has been saved in shared folder
		var seenWelcome = CheckCustomGlobalData( "welcomeGumpChoiceMade.jsdata", "seenWelcome" );
		if( !seenWelcome )
		{
			// Admin has either not seen or reacted to welcome gump yet
			TriggerEvent( 1, "DisplayAdminWelcomeGump", socket, pChar );
			return;
		}
	}
}

// Generic global-script function to look up data in /shared/jsWorldData/ folder
function CheckCustomGlobalData( fileName, dataToCheck )
{
	var rValue = false;
	var mFile = new UOXCFile();

	mFile.Open( fileName, "r", "jsWorldData" );
	if( mFile && mFile.Length() >= 0 )
	{
		// Read until we reach end of the file
		while( !mFile.EOF() )
		{
			// Read a line of text from the file
			var line = mFile.ReadUntil( "\n" );
			if( line.length <= 1 || line == "" )
				continue;

			var splitString = line.split( "=" );
			if( splitString[0] && splitString[0].toLowerCase() == dataToCheck.toLowerCase() )
			{
				if( splitString[1] )
				{
					// Return value
					rValue = splitString[1];
				}
			}
		}

		// Close file and free up memory allocated by file object
		mFile.Close();

		// Free memory allocated by file object
		mFile.Free();
	}

	return rValue;
}

// Fetch corpsedecaytimer from uox.ini
const corpseDecayTimer = GetServerSetting( "CORPSEDECAYTIMER" );
const playerCorpseIDs = [ 0x0190, 0x0191, 0x025D, 0x025E, 0x029A, 0x029B ];
// Handle turning player corpses into bones
function onDecay( iDecaying )
{
	// If not a corpse, or if corpse has no owner (i.e. NPC corpse), allow code to decay item like normal
	if( !iDecaying.corpse || !ValidateObject( iDecaying.owner ))
		return true;

	if( iDecaying.id == 0x2006 && playerCorpseIDs.indexOf( iDecaying.amount ) > -1 ) // character IDs are stored on corpses in amount property!
	{
		// We have a player corpse on our hands! Turn it into bones, and reset decay timer
		iDecaying.decaytime = corpseDecayTimer;

		// Add appropriate bones based on the direction the corpse lies in
		switch( iDecaying.dir )
		{
			case 4: 	// North, on back
			case 128: 	// North, on belly
				iDecaying.id = 0x0ecd;
				break;
			case 5: 	// North-East, on back
			case 129: 	// North-East, on belly
				iDecaying.id = 0x0ecf;
				break;
			case 6: 	// East, on back
			case 130: 	// East, on belly
				iDecaying.id = 0x0ecb;
				break;
			case 7: 	// South-East, on back
			case 131: 	// South-East, on belly
				iDecaying.id = 0x0ece;
				break;
			case 8: 	// South, on back
			case 132: 	// South, on belly
				iDecaying.id = 0x0eca;
				break;
			case 1: 	// South-West, on back
			case 133: 	// South-West, on belly
				iDecaying.id = 0x0ed0;
				break;
			case 2: 	// West, on back
			case 134: 	// West, on belly
				iDecaying.id = 0x0ecc;
				break;
			case 3: 	// North-West, on back
			case 135: 	// North-west on belly
				iDecaying.id = 0x0ece;
				break;
			default:
				break;
		}
		return false;
	}
	return true;
}

// Override the hard-coded help menu and display a JS based one
function onHelpButton( pChar )
{
	TriggerEvent( 2, "DisplayHelpMenu", pChar );
	return false;
}