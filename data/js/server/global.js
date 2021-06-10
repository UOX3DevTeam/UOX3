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