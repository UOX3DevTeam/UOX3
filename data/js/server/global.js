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

	// Store login timestamp (in minutes) in temp tag
	var loginTime = Math.round( GetCurrentClock() / 1000 / 60 );
	pChar.SetTempTag( "loginTime", loginTime );

	// Attach OnFacetChange to characters logging into the shard
	if( !pChar.HasScriptTrigger( 2508 ))
    {
        pChar.AddScriptTrigger( 2508 );
    }

    if( pChar.account.isYoung )
    {
  		// Attach "Young" player script, if the account is young and does not have script
		if( !pChar.HasScriptTrigger( 8001 ))
		{
			pChar.AddScriptTrigger( 8001 );
		}

    	// Check if "Young" player still meets requirement for being considered young
    	TriggerEvent( 8001, "CheckYoungStatus", socket, pChar, true );
    }


	//Attach the special moves Book
	if( !pChar.npc && !pChar.HasScriptTrigger( 7001 ))
	{
		pChar.AddScriptTrigger( 7001 );
  }

	// Re-adds Buff for disguise kit if player still has time left.
	var currentTime = GetCurrentClock();
	var disguiseKitTime = pChar.GetJSTimer( 1, 5023 );
	var timeLeft = Math.round(( disguiseKitTime - currentTime ) / 1000 );
	if( disguiseKitTime > 0 )
	{
		TriggerEvent( 2204, "RemoveBuff", pChar, 1033 );
		TriggerEvent( 2204, "AddBuff", pChar, 1033, 1075821, 1075820, timeLeft, "" );
	}
}

function onLogout( pSock, pChar )
{
	var minSinceLogin = Math.round( GetCurrentClock() / 1000 / 60 ) - pChar.GetTempTag( "loginTime" );
	pChar.playTime += minSinceLogin;
	pChar.account.totalPlayTime += minSinceLogin;

	//Used to remove the candy timer in items/candy.js
	pChar.KillJSTimer( 0, 5601 );
	pChar.SetTempTag( "toothach", null );
	pChar.SetTempTag( "Acidity", null );

	//Treasure Hunting Kill Event.
	var dirtItem = CalcItemFromSer( parseInt( pChar.GetTempTag( "dirtMadeSer" )));
	if( ValidateObject( dirtItem ))
	{
		TriggerEvent( 5405, "KillTreasureEvent", dirtItem, pChar );
		return;
	}
}

function onCreatePlayer( pChar )
{
	// If player character is created on a Young account, give them Young-specific items
	if( pChar.account.isYoung )
	{
		// Attach "Young" player script, if the account is young and does not have script
		if( !pChar.HasScriptTrigger( 8001 ))
		{
			pChar.AddScriptTrigger( 8001 );
		}

		TriggerEvent( 8001, "GiveYoungPlayerItems", pChar.socket, pChar );
	}

	//Attach the special moves Book
	if( !pChar.npc && !pChar.HasScriptTrigger( 7001 ))
	{
		pChar.AddScriptTrigger( 7001 );
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

// Override hard-coded snooping request and go with scripted one
function onSnoopAttempt( snoopTarget, targCont, pUser )
{
	return TriggerEvent( 4055, "SnoopAttempt", snoopTarget, targCont, pUser );
}

// Look for Gem of Salvation in player's backpack to give them chance to resurrect
function onDeath( pDead, iCorpse )
{
	if( !ValidateObject( pDead ) || pDead.npc || !pDead.online )
		return false;

	return TriggerEvent( 5045, "onDeath", pDead, iCorpse );
}