// Miscellaneous Custom Commands || by Xuri (xuri at uox3.org)
// v1.09
// Last Updated: 2. March 2021
//
// This script contains some commands I scripted after the command-reorganization in the UOX3 source code,
// as well as some I've "invented" on my own.
// Updates:
// 10. July 2004 - added SETTAG and GETTAG commands
// 11. July 2004 - Allowed setting tags with value "null" to delete tags.
// 5. June 2005 - Added DECAY and NODECAY commands
// 21. June 2005 - Added XSAY command
// 15. January 2006 - Cleaned up the script, removed redundant return-statements, etc.
// 19. May 2007 - Added LINKDOORS and UNLINKDOORS commands
// 22. Sept 2018 - Added SETAMMOEFFECT, SETAMMOTYPE, GETAMMOEFFECT, GETAMMOTYPE, REGIONINFO and XREGIONINFO commands
// 5. Sept 2020 - Added CONT, ENDFIGHT, GETMULTI, FINDITEM and MOVESPEED commands
// 2. March 2021 - Added IMMORTAL, NOINVUL and MORTAL as aliases for INVUL TRUE/FALSE

function CommandRegistration()
{
	RegisterCommand( "rename", 2, true ); //Lets GMs rename items/characters.
	RegisterCommand( "refresh", 0, true ); //Lets players refresh their screen to resend items/chars that have vanished from view.
	RegisterCommand( "freeze", 2, true ); //Will "freeze" any targeted char, and will make any targeted item immovable (i.e. locked down by GM)
	RegisterCommand( "unfreeze", 2, true ); //Will "unfreeze" any targeted char, and will make any targeted item movable if previously immovable
	RegisterCommand( "browse", 0, true ); //WIll let users open a webpage in their default browser from within the UO client. BROWSE <url>
	RegisterCommand( "invul", 2, true ); //Will make the targeted character invulnerable or not, depending on the argument provided (true/false, 1/0)
	RegisterCommand( "immortal", 2, true ); //Will make the targeted character invulnerable
	RegisterCommand( "noinvul", 2, true ); //Will make the targeted character vulnerable
	RegisterCommand( "mortal", 2, true ); //Will make the targeted character vulnerable
	RegisterCommand( "addpack", 2, true ); //Will add a backpack to the targeted character, if it has none. Will add specified item-id(addpack <item-id> or hex id (addpack hex <hexid>) to backpack.
	RegisterCommand( "settag", 2, true ); //used to specify a value for a specified tag on a targeted object
	RegisterCommand( "gettag", 2, true ); //Used to retrieve the value of a specified tag from a targeted object
	RegisterCommand( "settemptag", 2, true ); //used to specify a value for a specified tag on a targeted object
	RegisterCommand( "gettemptag", 2, true ); //Used to retrieve the value of a specified tag from a targeted object
	RegisterCommand( "nodecay", 2, true ); //Will turn off decay for the targeted item.
	RegisterCommand( "decay", 2, true ); //Will turn on decay for the targeted item.
	RegisterCommand( "xsay", 2, true ); //Targeted charcter or item will say specified text out loud
	RegisterCommand( "linkdoors", 2, true ); //Link two doors together so that if one is opened, both will open.
	RegisterCommand( "unlinkdoors", 2, true ); //Unlinks two doors (use command on both!)
	RegisterCommand( "setammoeffect", 2, true ); //Set ammoeffect on a bow
	RegisterCommand( "setammotype", 2, true ); //Set ammotype on a bow
	RegisterCommand( "getammoeffect", 2, true );
	RegisterCommand( "getammotype", 2, true );
	RegisterCommand( "undress", 2, true ); //Character will completely undress all equipped items
	RegisterCommand( "regioninfo", 2, true ); // Get information on current region player is in
	RegisterCommand( "xregioninfo", 2, true ); // Get information on current region a target character is in
	RegisterCommand( "cont", 2, true ); //Targeted item will be made a container, set to nondecay and movable 2
	RegisterCommand( "endfight", 2, true ); //Targeted character (and character being fought) will stop fighting
	RegisterCommand( "getmulti", 2, true ); //Get multiObject for targeted item
	RegisterCommand( "finditem", 2, true ); //Find item at layer
	RegisterCommand( "movespeed", 2, true ); //Set movement speed of target player
	RegisterCommand( "welcome", 2, true ); // Display UOX3 welcome gump for admin
	RegisterCommand( "getjstimer", 2, true ); // Get time remaining for specified timer ID on targeted object
	RegisterCommand( "setjstimer", 2, true ); // Set time remaining for specified timer ID on targeted object
	RegisterCommand( "killjstimer", 2, true ); // Kill timer with specified timer ID on targeted object
	RegisterCommand( "addhouse", 2, true ); // Add a multi by its house ID in houses.dfn
	RegisterCommand( "addmulti", 2, true ); // Add a multi by its raw multi ID
	RegisterCommand( "useitem", 2, true ); // Use a target item as if it was double-clicked
	RegisterCommand( "gettagmap", 2, true ); // Spit out a list of all custom tags on object
	RegisterCommand( "gettemptagmap", 2, true ); // Spit out a list of all custom tags on object
	RegisterCommand( "listpets", 2, true ); // Spit out a list of all pets owned by target
	RegisterCommand( "listfollowers", 2, true ); // Spit out a list of all followers of target
}

function command_RENAME( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString != "" )
	{
		pSock.xText = execString;

		var tempMsg = GetDictionaryEntry( 8851, pSock.language ); // What do you wish to rename to '%s'?
		pSock.CustomTarget( 0, tempMsg.replace( /%s/gi, execString ));
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8852, pSock.language )); // You need to enter a new name!
	}
}

function command_REFRESH( pSock, execString )
{
	var pUser = pSock.currentChar;
	pUser.Teleport( pUser.x, pUser.y, pUser.z );
}

function command_FREEZE( pSock, execString )
{
	pSock.CustomTarget( 1, GetDictionaryEntry( 8853, pSock.language )); // What do you wish to freeze to the ground?
}

function command_UNFREEZE( pSock, execString )
{
	pSock.CustomTarget( 2, GetDictionaryEntry( 8854, pSock.language )); // What do you wish to unfreeze?
}

//Open a specified webpage in default browser
function command_BROWSE( pSock, execString )
{
	if( execString != "" )
	{
		pSock.OpenURL( execString );
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8855, pSock.language )); // That's not a valid URL.
	}
}

function command_INVUL( pSock, execString )
{
	if( execString == "" || execString == "true" || execString == 1 )
	{
		pSock.CustomTarget( 3, GetDictionaryEntry( 8856, pSock.language )); // Whom do you wish to make invulnerable?
	}
	else if ( execString == "false" || execString == 0 )
	{
		pSock.CustomTarget( 4, GetDictionaryEntry( 8857, pSock.language )); // Whom do you wish to make vulnerable?
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8858, pSock.language )); // Accepted arguments for this command: true or false, 1 or 0!
	}
}
function command_NOINVUL( pSock, execString )
{
	command_INVUL( pSock, "false" );
}

function command_IMMORTAL( pSock, execString )
{
	pSock.CustomTarget( 25, GetDictionaryEntry( 8856, pSock.language )); // Whom do you wish to make immortal?
}
function command_MORTAL( pSock, execString )
{
	pSock.CustomTarget( 26, GetDictionaryEntry( 8856, pSock.language )); // Whom do you wish to make mortal?
}

function command_ADDPACK( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString == "" )
	{
		pSock.CustomTarget( 5, GetDictionaryEntry( 8859, pSock.language ) ); // For which character/at which location do you wish to add a backpack?
	}
	else
	{
		if( !isNaN( execString )) //Add from DFN-id
		{
			pSock.xText = execString;
			var tempMsg = GetDictionaryEntry( 8860, pSock.language ); // On what character do you wish to add the item %s?
			pSock.CustomTarget( 6, tempMsg.replace( /%s/gi, execString ));
		}
		else //Add from Hex-Id
		{
			var Word = execString.split( " " );
			if( Word[0] == "hex" )
			{
				pSock.xText = Word[1];
				pUser.SetTag( "AddFromHex", "Yep" );
				var tempMsg = GetDictionaryEntry( 8860, pSock.language ); // On what character do you wish to add the item %s?
				pSock.CustomTarget( 6, tempMsg.replace( /%s/gi, Word[1] ));
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 8861, pSock.language )); // Erroneous parameter specified! Try 'ADDPACK hex <hexid> or 'ADDPACK <item-id from dfn>
			}
		}
	}
}

function command_SETTAG( pSock, execString )
{
	var pUser = pSock.currentChar;
	var Word = execString.split( "," );
	if(( execString == "" || execString == null ) || ( Word[0] == null || Word[0] == "" || Word[0] == " " || Word[1] == "" || Word[1] == null ))
	{
		pUser.SysMessage( GetDictionaryEntry( 8862, pSock.language )); // You need to specify a tag and a value for the tag, seperated by a comma.
	}
	else
	{
		pUser.SetTag( "Word0", Word[0] );
		pUser.SetTag( "Word1", Word[1] );
		pUser.CustomTarget( 8, GetDictionaryEntry( 8863, pSock.language )); // Apply tag to which object?
	}
}

function command_GETTAG( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString == null || execString == "" )
	{
		pUser.SysMessage( GetDictionaryEntry( 8864, pSock.language )); // You need to specify a tagname to retrieve the value for
	}
	else
	{
		pUser.SetTag( "TempTag", execString );
		pUser.CustomTarget( 9, GetDictionaryEntry( 8865, pSock.language )); // Retrieve tag from which object?
	}
}

function command_SETTEMPTAG( pSock, execString )
{
	var pUser = pSock.currentChar;
	var Word = execString.split( "," );
	if(( execString == "" || execString == null ) || ( Word[0] == null || Word[0] == "" || Word[0] == " " || Word[1] == "" || Word[1] == null ))
	{
		pUser.SysMessage( GetDictionaryEntry( 8862, pSock.language )); // You need to specify a tag and a value for the tag, seperated by a comma.
	}
	else
	{
		pUser.SetTempTag( "Word0", Word[0] );
		pUser.SetTempTag( "Word1", Word[1] );
		pUser.CustomTarget( 27, GetDictionaryEntry( 8863, pSock.language )); // Apply tag to which object?
	}
}

function command_GETTEMPTAG( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString == null || execString == "" )
	{
		pUser.SysMessage( GetDictionaryEntry( 8864, pSock.language )); // You need to specify a tagname to retrieve the value for
	}
	else
	{
		pUser.SetTempTag( "TempTag", execString );
		pUser.CustomTarget( 28, GetDictionaryEntry( 8865, pSock.language )); // Retrieve tag from which object?
	}
}

function command_NODECAY( pSock, execString )
{
	var pUser = pSock.currentChar;
	pUser.CustomTarget( 10, GetDictionaryEntry( 8866, pSock.language )); // Select an item to set as NOT decayable:
}

function command_DECAY( pSock, execString )
{
	var pUser = pSock.currentChar;
	pUser.CustomTarget( 11, GetDictionaryEntry( 8867, pSock.language )); // Select an item to set as decayable:
}

function command_XSAY( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString )
	{
		pSock.xText = execString;
		pUser.CustomTarget( 12, GetDictionaryEntry( 8868, pSock.language )); // Select object for remote speech:
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8869, pSock.language )); // You forgot to write some text to go with this command.
	}
}

//Rename
function onCallback0( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var newName = pSock.xText;
	if( !pSock.GetWord( 1 ))
	{
		var tempMsg = GetDictionaryEntry( 8870, pSock.language ); // '%s' has been renamed to '%t'.
		tempMsg = tempMsg.replace( /%s/gi, myTarget.name );
		pUser.SysMessage( tempMsg.replace( /%t/gi, newName ));
		myTarget.name = newName;
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8871, pSock.language )); // You cannot rename that!
	}
	pUser.SetTag( "RenameCmdText", null );
}

//Freeze
function onCallback1( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		myTarget.frozen = true;
		myTarget.Refresh();
		pUser.SysMessage( GetDictionaryEntry( 8872, pSock.language )); // The selected character has been frozen.
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem  )
	{
		myTarget.movable = 2;
		myTarget.decayable = false;
		myTarget.Refresh();
		pUser.SysMessage( GetDictionaryEntry( 8873, pSock.language )); // The selected item has been frozen.
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8874, pSock.language )); // You cannot freeze that.
	}
}

//Unfreeze
function onCallback2( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		if( myTarget.frozen == true )
		{
			myTarget.frozen = false;
			myTarget.Refresh();
			pUser.SysMessage( GetDictionaryEntry( 8875, pSock.language )); // The selected item has been unfrozen.
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8876, pSock.language )); // That character isn't frozen! Can't unfreeze!
		}
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		if( myTarget.movable <= 1 )
		{
			pUser.SysMessage( GetDictionaryEntry( 8877, pSock.language )); // That item isn't frozen! Can't unfreeze!
		}
		else
		{
			myTarget.movable = 1;
			myTarget.Refresh();
			pUser.SysMessage( GetDictionaryEntry( 8878, pSock.language )); // The selected item has been unfrozen.
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8879, pSock.language )); // You cannot unfreeze that.
	}
}

// Make target invulnerable
function onCallback3( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		if( myTarget.vulnerable == false )
		{
			pUser.SysMessage( GetDictionaryEntry( 8880, pSock.language )); // That target is already invulnerable!
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8881, pSock.language )); // The selected target has been made invulnerable.
			myTarget.vulnerable = false;
			myTarget.Refresh();
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8882, pSock.language )); // That is not a character. Try again.
	}
}

// Make target vulnerable
function onCallback4( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		if( myTarget.vulnerable == true )
		{
			pUser.SysMessage( GetDictionaryEntry( 8883, pSock.language )); // That target is already vulnerable!
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8884, pSock.language )); // The selected target has been made vulnerable.
			myTarget.vulnerable = true;
			myTarget.Refresh();
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8885, pSock.language )); // That is not a character. Try again.
	}
}

// Addpack without parameters
function onCallback5( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var targX = pSock.GetWord( 11 );
	var targY = pSock.GetWord( 13 );
	var targZ = pSock.GetSByte( 16 );
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		// Add backpack on character
		var tempObj = myTarget.FindItemLayer( 21 );
		if( tempObj == null )
		{
			var newPack = CreateDFNItem( pUser.socket, myTarget, "0x09b2", 1, "ITEM", false );
			newPack.container = myTarget;
			newPack.layer = 21;
			newPack.weight = 0;
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8886, pSock.language )); // That character already has a backpack. No new backpack added.
		}
	}
	else
	{
		// Add backpack on ground
		var newPack = CreateDFNItem( pUser.socket, myTarget, "0x09b2", 1, "ITEM", false );
		newPack.x = targX;
		newPack.y = targY;
		newPack.z = targZ;
	}
}

// ADDPACK callback function
function onCallback6( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var TempItemID = pSock.xText;
	var Word1 = pSock.xText;
	var Word1 = Number( Word1 );
	var AddFromHex = pUser.GetTag( "AddFromHex" );
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		var tempObj = myTarget.FindItemLayer( 21 );
		if( tempObj != null )
		{
			if( AddFromHex != "Yep" )
			{
				var tempItem = CreateDFNItem( pUser.socket, myTarget, TempItemID, 1, "ITEM", true );
			}
			else
			{
				var tempItem = CreateBlankItem( pSock, myTarget, 1, "#", Word1, 0x0, "ITEM", true );
			}
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8887, pSock.language )); // That character has no backpack! Backpack being added before new item...
			var newPack = CreateDFNItem( pUser.socket, myTarget, "0x09b2", 1, "ITEM", false );
			newPack.container = myTarget;
			newPack.layer = 21;
			newPack.weight = 0;
			newPack.maxItems = parseInt( GetServerSetting( "MAXPLAYERPACKITEMS" ));
			if( AddFromHex != "Yep" )
			{
				var tempItem = CreateDFNItem( pUser.socket, myTarget, TempItemID, 1, "ITEM", true );
			}
			else
			{
				var tempItem = CreateBlankItem( pSock, myTarget, 1, "#", Word1, 0x0, "ITEM", true );
			}
		}
	}
	else
	{
		pUser.SysMessage( "That is no character. Try again." );
	}
	pUser.SetTag( "AddFromHex", null );
}
// Set Tag
function onCallback8( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var Word0 = pUser.GetTag( "Word0" );
	var Word1 = pUser.GetTag( "Word1" );
	if( !pSock.GetWord( 1 ))
	{
		if( Word1 == "null" )
		{
			myTarget.SetTag( Word0, null );
		}
		else
		{
			myTarget.SetTag( Word0, Word1 );
		}

		var tempMsg = GetDictionaryEntry( 8888, pSock.language ); // You have set a tag named '%s' with a value of '%t' on the targeted object.
		tempMsg = tempMsg.replace( /%s/gi, Word0 );
		pUser.SysMessage( tempMsg.replace( /%t/gi, Word1 ));
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8889, pSock.language )); // You need to target a dynamic object (item or character).
	}
	pUser.SetTag( "Word0", null );
	pUser.SetTag( "Word1", null );
}

// Get Tag
function onCallback9( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var TempTagName = pUser.GetTag( "TempTag" );
	if( !pSock.GetWord( 1 ))
	{
		var TagData = myTarget.GetTag( TempTagName );

		var tempMsg = GetDictionaryEntry( 8890, pSock.language ); // The value of the targeted object's '%s'-tag is: " + TagData
		pUser.SysMessage( tempMsg.replace( /%s/gi, TempTagName ) + " " + TagData );
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8891, pSock.language )); // You need to target a dynamic object (item or character).
	}
	pUser.SetTag( "TempTag", null );
}

// Nodecay
function onCallback10( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ))
	{
		if( myTarget.isItem )
		{
			myTarget.decayable = false;
			pUser.SysMessage( GetDictionaryEntry( 8892, pSock.language )); // Item successfully set as NOT decayable.
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8893, pSock.language )); // This command can only be applied to items.
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8894, pSock.language )); // You need to target a dynamic item.
	}
}

// Decay
function onCallback11( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ))
	{
		if( myTarget.isItem )
		{
			myTarget.decayable = true;
			pUser.SysMessage( GetDictionaryEntry( 8895, pSock.language )); // Item successfully set as decayable.
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8896, pSock.language )); // This command can only be applied to items.
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8894, pSock.language )); // You need to target a dynamic item.
	}
}

// XSAY
function onCallback12( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && ( myTarget.isChar || myTarget.isItem ))
	{
		myTarget.TextMessage( pSock.xText );
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8897, pSock.language )); // You must target either a character or a dynamic item.
	}
}


function command_LINKDOORS( pSock, execString )
{
	pSock.CustomTarget( 13, GetDictionaryEntry( 8898, pSock.language )); // Which two doors do you want to link? (1/2)
}

function onCallback13( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isItem && pSock.clickX != 1)
	{
		pSock.tempObj = myTarget;
		pSock.clickX = 1;
		pSock.CustomTarget( 13, GetDictionaryEntry( 8899, pSock.language )); // Which two doors do you want to link? (2/2)
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem && pSock.clickX == 1)
	{
		var Door1 = pSock.tempObj;
		var Door2 = myTarget;

		Door1.SetTag( "linked", true );
		Door1.SetTag( "linkSer1", Door2.GetSerial( 1 ));
		Door1.SetTag( "linkSer2", Door2.GetSerial( 2 ));
		Door1.SetTag( "linkSer3", Door2.GetSerial( 3 ));
		Door1.SetTag( "linkSer4", Door2.GetSerial( 4 ));

		Door2.SetTag( "linked", true );
		Door2.SetTag( "linkSer1", Door1.GetSerial( 1 ));
		Door2.SetTag( "linkSer2", Door1.GetSerial( 2 ));
		Door2.SetTag( "linkSer3", Door1.GetSerial( 3 ));
		Door2.SetTag( "linkSer4", Door1.GetSerial( 4 ));
		pUser.SysMessage( GetDictionaryEntry( 8900, pSock.language )); // The two doors have been linked.
		pSock.clickX = null;
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8894, pSock.language )); // You need to target a dynamic item.
	}
}

function command_UNLINKDOORS( pSock, execString )
{
	pSock.CustomTarget( 14, GetDictionaryEntry( 8901, pSock.language )); // Unlink which two doors? (1/2)
}

function onCallback14( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isItem && pSock.clickX != 1)
	{
		pSock.tempObj = myTarget;
		pSock.clickX = 1;
		pSock.CustomTarget( 14, GetDictionaryEntry( 8902, pSock.language )); // Unlink which two doors? (2/2)
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem && pSock.clickX == 1)
	{
		var Door1 = pSock.tempObj;
		var Door2 = myTarget;

		Door1.SetTag( "linked", null );
		Door1.SetTag( "linkSer1", null );
		Door1.SetTag( "linkSer2", null );
		Door1.SetTag( "linkSer3", null );
		Door1.SetTag( "linkSer4", null );

		Door2.SetTag( "linked", null );
		Door2.SetTag( "linkSer1", null );
		Door2.SetTag( "linkSer2", null );
		Door2.SetTag( "linkSer3", null );
		Door2.SetTag( "linkSer4", null );
		pUser.SysMessage( GetDictionaryEntry( 8903, pSock.language )); // The two doors have been unlinked.
		pSock.clickX = null;
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8894, pSock.language )); // You need to target a dynamic item.
	}
}

function command_SETAMMOEFFECT( pSock, execString )
{
	if( execString != "" )
	{
		pSock.xText = execString;
		pSock.CustomTarget( 15, GetDictionaryEntry( 8904, pSock.language )); // Set ammoeffect on which bow?
	}
}

function onCallback15( pSock, myTarget )
{
	var ammoEffect = pSock.xText;
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		myTarget.ammoEffect = ammoEffect;
	}
}

function command_SETAMMOTYPE( pSock, execString )
{
	if( execString != "" )
	{
		pSock.xText = execString;
		pSock.CustomTarget( 16, GetDictionaryEntry( 8905, pSock.language )); // Set ammotype on which bow?
	}
}

function onCallback16( pSock, myTarget )
{
	var ammoType = pSock.xText;
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		myTarget.ammoType = ammoType;
	}
}

function command_GETAMMOEFFECT( pSock, execString )
{
	pSock.CustomTarget( 17, GetDictionaryEntry( 8906, pSock.language )); // Get ammoeffect from which bow?
}

function onCallback17( pSock, myTarget )
{
	var ammoEffect = myTarget.ammoEffect;
	pUser.TextMessage( GetDictionaryEntry( 8907, pSock.language ) + " " + ammoEffect ); // AmmoEffect on selected bow:
}

function command_GETAMMOTYPE( pSock, execString )
{
	var pUser = pSock.currentChar;
	pSock.CustomTarget( 18, GetDictionaryEntry( 8908, pSock.language )); // Get ammotype from which bow?
}

function onCallback18( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var ammoType = myTarget.ammoType;
	pUser.TextMessage( GetDictionaryEntry( 8909, pSock.language ) + " " + ammoType ); // AmmoType on selected bow:
}

function command_UNDRESS( pSock, execString )
{
	var pUser = pSock.currentChar;
	var i = 0;
	for( i = 0; i <= 24; i++ )
	{
		var tempObj = pUser.FindItemLayer( i );
		if( tempObj != null )
		{
			if( i != 11 && i != 16 && i != 21 )
			{
				tempObj.container = pUser.pack;
			}
		}
	}
}

function command_REGIONINFO( pSock, execString )
{
	var pUser = pSock.currentChar;
	var pRegion = pUser.region;
	pSock.SysMessage( "Region: " + pRegion.name );
	pSock.SysMessage( "CanMark: " + pRegion.canMark );
	pSock.SysMessage( "CanRecall: " + pRegion.canRecall );
	pSock.SysMessage( "IsGuarded: " + pRegion.isGuarded );
	pSock.SysMessage( "CanCastAggressive: " + pRegion.canCastAggressive );
	pSock.SysMessage( "IsSafeZone: " + pRegion.isSafeZone );
}

function command_XREGIONINFO( pSock, execString )
{
	pSock.CustomTarget( 19, GetDictionaryEntry( 8910, pSock.language )); // Get region info for which character?
}

function onCallback19( pSock, myTarget )
{
	var myTargetRegion = myTarget.region;
	pSock.SysMessage( "xRegion: " + myTargetRegion.name );
	pSock.SysMessage( "xCanMark: " + myTargetRegion.canMark );
	pSock.SysMessage( "xCanRecall: " + myTargetRegion.canRecall );
	pSock.SysMessage( "xIsGuarded: " + myTargetRegion.isGuarded );
	pSock.SysMessage( "xCanCastAggressive: " + myTargetRegion.canCastAggressive );
	pSock.SysMessage( "xIsSafeZone: " + myTargetRegion.isSafeZone );
}

function command_CONT( pSock, execString )
{
	pSock.CustomTarget( 20, GetDictionaryEntry( 8911, pSock.language )); // Set which item as nonmovable container?
}

function onCallback20( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		pUser.SysMessage( GetDictionaryEntry( 8913, pSock.language )); // You must select a container!
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem  )
	{
		myTarget.movable = 2;
		myTarget.decayable = false;
		myTarget.type = 1;
		pUser.SysMessage( GetDictionaryEntry( 8912, pSock.language )); // The selected item has been nonmovable container'd.
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8914, pSock.language )); // Impossible!
	}
}

function command_ENDFIGHT( pSock, execString )
{
	pSock.CustomTarget( 21, GetDictionaryEntry( 8915, pSock.language )); // Subdue which fight?
}

function onCallback21( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		if( myTarget.atWar == true )
		{
			pUser.SysMessage( myTarget.attacker );
			var opponent = myTarget.target;
			opponent.target = null;
			opponent.atWar = false;
			opponent.attacker = null;
			myTarget.target = null;
			myTarget.atWar = false;
			myTarget.attacker = null;
			pUser.SysMessage( GetDictionaryEntry( 8916, pSock.language )); // Fight has been subdued.
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8917, pSock.language )); // That character is not in a fight.
		}
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem  )
	{
		pUser.SysMessage( GetDictionaryEntry( 8918, pSock.language )); // You must select a character!
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8914, pSock.language )); // Impossible!
	}
}

function command_GETMULTI( pSock )
{
	pSock.CustomTarget( 22, GetDictionaryEntry( 8919, pSock.language )); // Get MultiObj from which item?
}

function onCallback22( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		var multiObj = myTarget.multi;
		if( multiObj )
		{
			pUser.SysMessage( GetDictionaryEntry( 8920, pSock.language ) + " " + multiObj.serial ); // Serial of multiObj: + multiObj.serial
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 8921, pSock.language )); // Target does not belong to a multiObj.
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8894, pSock.language )); // You need to target a dynamic item.
	}
}

function command_FINDITEM( pSock, execString )
{
	var tempMsg = GetDictionaryEntry( 8922, pSock.language ); // Find item at layer %s on which character?
	pSock.CustomTarget( 23, tempMsg.replace( /%s/gi, execString ));
	pSock.xText = execString;
}

function onCallback23( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		var myText = pSock.xText;
		var myInt = parseInt( myText );
		var equippedItem = pUser.FindItemLayer( myInt );
		if( equippedItem )
		{
			var tempMsg = GetDictionaryEntry( 8923, pSock.language ); // Target has item with ID %i equipped at layer %d.
			tempMsg = tempMsg.replace( /%i/gi, equippedItem.id );
			pSock.SysMessage( tempMsg.replace( /%d/gi, myInt ));
		}
		else
		{
			var tempMsg = GetDictionaryEntry( 8924, pSock.language ); // Target has no item equipped at layer %i.
			pSock.SysMessage( tempMsg.replace( /%i/gi, myInt ));
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8925, pSock.language )); // Target is not a character.
	}
}

function command_MOVESPEED( pSock, execString )
{
	switch( execString )
	{
		case "0x0": // Normal mode
		case "0x1": // Mounted mode
		case "0x2": // Slow mode (walk only)
		case "0x3": // Hybrid mode ( "jog"?)
		case "0x4": // Frozen
		{
			var tempMsg = GetDictionaryEntry( 8926, pSock.language ); // Choose target to set movement speed %i for:
			pSock.CustomTarget( 24, tempMsg.replace( /%i/gi, execString ));
			pSock.xText = execString;
			break;
		}
		default:
			pSock.SysMessage( GetDictionaryEntry( 8927, pSock.language )); // Only values between 0x0 to 0x4 are supported!
			break; // Unsupported
	}
}

function onCallback24( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		var toSend = new Packet;
		toSend.ReserveSize( 6 )
		toSend.WriteByte( 0, 0xbf ) // Packet
		toSend.WriteShort( 1, 0x06 ) // length
		toSend.WriteShort( 3, 0x26 ) // SubCmd
		toSend.WriteByte( 5, parseInt(pSock.xText) ) // Mode
		pSock.Send( toSend );
		toSend.Free();
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8925, pSock.language )); // Target is not a character.
	}
}

// Make target immortal (can take damage, but will never die)
function onCallback25( pSock, myTarget )
{
	var immortalScript = 3510; // Script ID of immortality.js, set in jse_fileassociations.scp

	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		var scriptTriggers = myTarget.scriptTriggers;
		var size = scriptTriggers.length;
		for( var i = 0; i < size; i++ )
		{
			if( scriptTriggers[i] == immortalScript )
			{
				pUser.SysMessage( GetDictionaryEntry( 8883, pSock.language )); // That target is already immortal!
				return;
			}
		}

		pUser.SysMessage( GetDictionaryEntry( 8884, pSock.language )); // The selected target has been made immortal; they can bleed, but they cannot die!.
		myTarget.AddScriptTrigger( immortalScript );
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8885, pSock.language )); // That is not a character. Try again.
	}
}

// Make target mortal (can die)
function onCallback26( pSock, myTarget )
{
	var immortalScript = 3510; // Script ID of immortality.js, set in jse_fileassociations.scp

	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		var scriptTriggers = myTarget.scriptTriggers;
		var size = scriptTriggers.length;
		for( var i = 0; i < size; i++ )
		{
			if( scriptTriggers[i] == immortalScript )
			{
				pUser.SysMessage( GetDictionaryEntry( 8884, pSock.language )); // The selected target has been made mortal; they can now die!
				myTarget.RemoveScriptTrigger( immortalScript );
				return;
			}
		}
		pUser.SysMessage( GetDictionaryEntry( 8883, pSock.language )); // That target is already a mortal!
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8885, pSock.language )); // That is not a character. Try again.
	}
}

// SetTempTag
function onCallback27( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var word0 = pUser.GetTempTag( "Word0" );
	var word1 = pUser.GetTempTag( "Word1" );
	if( !pSock.GetWord( 1 ))
	{
		if( word1 == "null" )
		{
			myTarget.SetTempTag( word0, null );
		}
		else
		{
			myTarget.SetTempTag( word0, word1 );
		}

		var tempMsg = GetDictionaryEntry( 8888, pSock.language ); // You have set a tag named '%s' with a value of '%t' on the targeted object.
		tempMsg = tempMsg.replace( /%s/gi, word0 );
		pUser.SysMessage( tempMsg.replace( /%t/gi, word1 ));
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8889, pSock.language )); // You need to target a dynamic object (item or character).
	}
	pUser.SetTempTag( "Word0", null );
	pUser.SetTempTag( "Word1", null );
}

// GetTempTag
function onCallback28( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var tempTagName = pUser.GetTempTag( "TempTag" );
	if( !pSock.GetWord( 1 ))
	{
		var tagData = myTarget.GetTempTag( tempTagName );
		var tempMsg = GetDictionaryEntry( 8890, pSock.language ); // The value of the targeted object's '%s'-tag is: " + tagData
		pUser.SysMessage( tempMsg.replace( /%s/gi, tempTagName ) + " " + tagData );
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8891, pSock.language )); // You need to target a dynamic object (item or character).
	}
	pUser.SetTempTag( "TempTag", null );
}

function command_WELCOME( pSock, execString )
{
	TriggerEvent( 1, "DisplayAdminWelcomeGump", pSock, pSock.currentChar );
}

function command_GETJSTIMER( pSock, execString )
{
	var params = execString.replace( /\s/g, '').split( "," );
	if( params.length != 2 )
	{
		pSock.SysMessage( GetDictionaryEntry( 2762, pSock.language )); // Invalid number of parameters - requires 2 (timerID,scriptID)!
		return;
	}

	pSock.tempTimerID = parseInt( params[0] );
	pSock.tempScriptID = parseInt( params[1] );

	pSock.CustomTarget( 29, GetDictionaryEntry( 2763, pSock.language )); // Fetch remaining time of JS timer for which object?
}

function onCallback29( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var timerID = pSock.tempTimerID;
	var scriptID = pSock.tempScriptID;

	if( !pSock.GetWord( 1 ))
	{
		var tempMsg;
		var currentTime = GetCurrentClock();
		var expiryTime = myTarget.GetJSTimer( timerID, scriptID );
		if( expiryTime != 0 )
		{
			var timeNow = new Date().getTime();
			var newExpiryTime = new Date( timeNow + parseInt( expiryTime - currentTime ));

			tempMsg = GetDictionaryEntry( 2764, pSock.language ); // Timer will expire at: %s
			pSock.SysMessage( tempMsg.replace( /%s/gi, newExpiryTime ));

			tempMsg = GetDictionaryEntry( 2765, pSock.language ); // // Remaining time: %d seconds
			pSock.SysMessage(tempMsg.replace(/%d/gi, Math.round(( expiryTime - currentTime ) / 1000 ).toString() ));
		}
		else
		{
			tempMsg = GetDictionaryEntry( 2766, pSock.language ); // No JS timer with timerID %d and scriptID %u active on object!
			tempMsg = tempMsg.replace( /%d/gi, timerID.toString() );
			pSock.SysMessage( tempMsg.replace( /%u/gi, scriptID.toString() ));
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8891, pSock.language )); // You need to target a dynamic object (item or character).
	}
}

function command_SETJSTIMER( pSock, execString )
{
	var params = execString.replace( /\s/g, '').split( "," );
	if( params.length != 3 )
	{
		pSock.SysMessage( GetDictionaryEntry( 2767, pSock.language )); // Invalid number of parameters - requires 3 (timerID,expiryTime,scriptID)!
		return;
	}

	pSock.tempTimerID = parseInt( params[0] );
	pSock.tempExpiryTime = parseInt( params[1] );
	pSock.tempScriptID = parseInt( params[2] );

	pSock.CustomTarget( 30, GetDictionaryEntry( 2768, pSock.language )); // Set remaining time of JS timer for which object?
}

function onCallback30( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var timerID = pSock.tempTimerID;
	var expiryTime = pSock.tempExpiryTime;
	var scriptID = pSock.tempScriptID;

	if( !pSock.GetWord( 1 ))
	{
		var tempMsg;
		var currentTime = GetCurrentClock();
		if( myTarget.SetJSTimer( timerID, expiryTime, scriptID ))
		{
			var timeNow = new Date().getTime();
			var newExpiryTime = new Date( timeNow + parseInt( expiryTime ));

			tempMsg = GetDictionaryEntry( 2764, pSock.language ); // Timer will expire at: %s
			pSock.SysMessage( tempMsg.replace( /%s/gi, newExpiryTime ));

			tempMsg = GetDictionaryEntry( 2765, pSock.language ); // // Remaining time: %d seconds
			pSock.SysMessage( tempMsg.replace( /%d/gi, ( expiryTime / 1000 ).toString() ));
		}
		else
		{
			tempMsg = GetDictionaryEntry( 2766, pSock.language ); // No JS timer with timerID %d and scriptID %u active on object!
			tempMsg = tempMsg.replace( /%d/gi, timerID.toString() );
			pSock.SysMessage( tempMsg.replace( /%u/gi, scriptID.toString() ));
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8891, pSock.language )); // You need to target a dynamic object (item or character).
	}
}

function command_KILLJSTIMER( pSock, execString )
{
	var params = execString.replace( /\s/g, '').split( "," );
	if( params.length != 2 )
	{
		pSock.SysMessage( GetDictionaryEntry( 2762, pSock.language )); // Invalid number of parameters - requires 2 (timerID,scriptID)!
		return;
	}

	pSock.tempTimerID = parseInt( params[0] );
	pSock.tempScriptID = parseInt( params[1] );

	pSock.CustomTarget( 31, GetDictionaryEntry( 2769, pSock.language )); // Kill specified JS timer for which object?
}

function onCallback31( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var scriptID = pSock.tempScriptID;
	var timerID = pSock.tempTimerID;

	if( !pSock.GetWord( 1 ))
	{
		if( myTarget.KillJSTimer( timerID, scriptID ))
		{
			pSock.SysMessage( GetDictionaryEntry( 2770, pSock.language )); // Specified JS timer has been killed for selected object.
		}
		else
		{
			var tempMsg = GetDictionaryEntry( 2766, pSock.language ); // No JS timer with timerID %d and scriptID %u active on object!
			tempMsg = tempMsg.replace( /%d/gi, timerID.toString() );
			pSock.SysMessage( tempMsg.replace( /%u/gi, scriptID.toString() ));
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8891, pSock.language )); // You need to target a dynamic object (item or character).
	}
}

function command_ADDHOUSE( socket, cmdString )
{
	if( cmdString )
	{
		var stringID = "";
		var splitString = cmdString.split( " " );
		if( splitString[0] )
		{
			// .addhouse houseID
			stringID = splitString[0];
			socket.tempint = parseInt( stringID );
		}

		if( stringID != "" )
		{
			socket.CustomTarget( 32, "Select location for house:" ); // Select location for house
		}
	}
}

function onCallback32( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	var mChar = socket.currentChar;
	if( mChar )
	{
		var x = socket.GetWord( 11 );
		var y = socket.GetWord( 13 );
		var z = socket.GetSByte( 16 );
		var StrangeByte = socket.GetWord(1);

		// If connected with a client lower than v7.0.9, manually add height of targeted tile
		if ((StrangeByte == 0 && ourObj.isItem) || (socket.clientMajorVer <= 7 && socket.clientSubVer < 9))
		{
			z += GetTileHeight( socket.GetWord( 17 ));
		}

		var houseID = socket.tempint;
		socket.tempint = 0;
		var newMulti = CreateHouse( houseID, x, y, z, mChar.worldnumber, mChar.instanceID, 0, false );

		if( !newMulti || !newMulti.IsMulti() )
		{
			mChar.SysMessage( "Unable to create house!" ); // Unable to create house!
		}
	}
}

function command_ADDMULTI( socket, cmdString )
{
	if( cmdString )
	{
		var stringID = "";
		var splitString = cmdString.split( " " );
		if( splitString[0] )
		{
			// .addmulti multiID
			stringID = splitString[0];
			socket.tempint = parseInt( stringID );
		}

		if( stringID != "" )
		{
			socket.CustomTarget( 33, "Select location for multi:" ); // Select location for multi
		}
	}
}

function onCallback33( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	var mChar = socket.currentChar;
	if( mChar )
	{
		var x = socket.GetWord( 11 );
		var y = socket.GetWord( 13 );
		var z = socket.GetSByte( 16 );
		var StrangeByte = socket.GetWord(1);

		// If connected with a client lower than v7.0.9, manually add height of targeted tile
		if ((StrangeByte == 0 && ourObj.isItem) || (socket.clientMajorVer <= 7 && socket.clientSubVer < 9))
		{
			z += GetTileHeight( socket.GetWord( 17 ));
		}

		var multiID = socket.tempint;
		socket.tempint = 0;
		var newBaseMulti = CreateBaseMulti( multiID, x, y, z, mChar.worldnumber, mChar.instanceID );

		if( !newBaseMulti || !newBaseMulti.IsMulti() )
		{
			mChar.SysMessage( "Unable to create base multi!" ); // Unable to create base multi!
		}
	}
}

function command_USEITEM( pSock, execString )
{
	pSock.CustomTarget( 34, GetDictionaryEntry( 97, pSock.language )); // Use which item?
}

function onCallback34( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	var mChar = socket.currentChar;
	if( mChar )
	{
		if( ValidateObject( ourObj ) && ourObj.isItem )
		{
			UseItem( mChar, ourObj );
		}
	}
}

function command_GETTAGMAP( pSock, execString )
{
	pSock.CustomTarget( 35, GetDictionaryEntry( 92, pSock.language )); // Get persistent tag map for which object?
}

function onCallback35( pSock, ourObj )
{
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	if( ValidateObject( ourObj ))
	{
		var tagMap = ourObj.GetTagMap();
		pSock.SysMessage( GetDictionaryEntry( 93, pSock.language ), tagMap.length ); // Number of persistent tags: %i
		for( var i = 0; i < tagMap.length; i++ )
		{
			var tagName = tagMap[i][0];
			var tagType;
			switch( tagMap[i][1] )
			{
				case 0:
					tagType = "Int";
					break;
				case 1:
					tagType = "String";
					break;
				case 2:
					tagType = "Bool";
					break;
			}
			var tagValue = tagMap[i][2].toString();
			pSock.SysMessage( GetDictionaryEntry( 96, pSock.language), tagName, tagType, tagValue ); // Tag: %s | Type: %i | Value: %u
		}
	}
}

function command_GETTEMPTAGMAP( pSock, execString )
{
	pSock.CustomTarget( 36, GetDictionaryEntry( 94, pSock.language )); // Get temporary tag map for which object?
}

function onCallback36( pSock, ourObj )
{
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	if( ValidateObject( ourObj ))
	{
		var tagMap = ourObj.GetTempTagMap();
		pSock.SysMessage( GetDictionaryEntry( 95, pSock.language ), tagMap.length ); // Number of temporary tags: %i
		for( var i = 0; i < tagMap.length; i++ )
		{
			var tagName = tagMap[i][0];
			var tagType;
			switch( tagMap[i][1] )
			{
				case 0:
					tagType = "Int";
					break;
				case 1:
					tagType = "String";
					break;
				case 2:
					tagType = "Bool";
					break;
			}
			var tagValue = tagMap[i][2].toString();
			pSock.SysMessage( GetDictionaryEntry( 96, pSock.language), tagName, tagType, tagValue ); // Tag: %s | Type: %i | Value: %u
		}
	}
}

function command_LISTPETS( pSock, execString )
{
	pSock.CustomTarget( 37, GetDictionaryEntry( 2775, pSock.language )); // Get list of pets for which character?
}

function onCallback37( pSock, ourObj )
{
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	if( ValidateObject( ourObj ))
	{
		var petList = ourObj.GetPetList();
		pSock.SysMessage( GetDictionaryEntry( 2773, pSock.language ), petList.length ); // Number of pets: %i
		for( var i = 0; i < petList.length; i++ )
		{
			if( ValidateObject( petList[i] ))
			{
				pSock.SysMessage( GetDictionaryEntry( 2778, pSock.language ), petList[i].name, petList[i].id, petList[i].serial, petList[i].x, petList[i].y, petList[i].z, petList[i].worldnumber, petList[i].instanceID ); // Name: %s | ID: %i | Serial: %i | x: %i | y: %i | z: %i | world: %i | instanceID: %i
			}
		}
	}
}

function command_LISTFOLLOWERS( pSock, execString )
{
	pSock.CustomTarget( 38, GetDictionaryEntry( 2776, pSock.language )); // Get list of followers for which character?
}

function onCallback38( pSock, ourObj )
{
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	if( ValidateObject( ourObj ))
	{
		var followerList = ourObj.GetFollowerList();
		pSock.SysMessage( GetDictionaryEntry( 2774, pSock.language ), followerList.length ); // Number of followers: %i
		for( var i = 0; i < followerList.length; i++ )
		{
			if( ValidateObject( followerList[i] ))
			{
				pSock.SysMessage( GetDictionaryEntry( 2778, pSock.language ), followerList[i].name, followerList[i].id, followerList[i].serial, followerList[i].x, followerList[i].y, followerList[i].z, followerList[i].worldnumber, followerList[i].instanceID ); // Name: %s | ID: %i | Serial: %i | x: %i | y: %i | z: %i | world: %i | instanceID: %i
			}
		}
	}
}

function _restorecontext_() {}