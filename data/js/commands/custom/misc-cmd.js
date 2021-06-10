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
}

function command_RENAME( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString != "" )
	{
		pSock.xText = execString;
		pSock.CustomTarget( 0, "What do you wish to rename to '"+execString+"'?" );
	}
	else
		pUser.SysMessage( "You need to enter a new name!" );
}

function command_REFRESH( pSock, execString )
{
	var pUser = pSock.currentChar;
	pUser.Teleport( pUser.x, pUser.y, pUser.z );
}

function command_FREEZE( pSock, execString )
{
	pSock.CustomTarget( 1, "What do you wish to freeze to the ground'?" );
}

function command_UNFREEZE( pSock, execString )
{
	pSock.CustomTarget( 2, "What do you wish to unfreeze'?" );
}

//Open a specified webpage in default browser
function command_BROWSE( pSock, execString )
{
	if( execString != "" )
		pSock.OpenURL( execString );
	else
		pSock.SysMessage( "That's not a valid URL." );
}

function command_INVUL( pSock, execString )
{
	if( execString == "" || execString == "true" || execString == 1 )
		pSock.CustomTarget( 3, "Whom do you wish to make invulnerable?" );
	else if ( execString == "false" || execString == 0 )
		pSock.CustomTarget( 4, "Whom do you wish to make vulnerable?" );
	else
		pSock.SysMessage( "Accepted arguments for this command: true or false, 1 or 0!" );
}
// Alias for 'invul true'
function command_IMMORTAL( pSock, execString )
{
	command_INVUL( pSock, "true" );
}

// Alias for 'invul false
function command_NOINVUL( pSock, execString )
{
	command_INVUL( pSock, "false" );
}
// Alias for 'invul false'
function command_MORTAL( pSock, execString )
{
	command_INVUL( pSock, "false" );
}

function command_ADDPACK( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString == "" )
	{
		pSock.CustomTarget( 5, "On what character/at what location do you wish to add a backpack?" );
	}
	else
	{
		if( !isNaN(execString)) //Add from DFN-id
		{
			pSock.xText = execString;
			pSock.CustomTarget( 6, "On what character do you wish to add the item "+execString+"?" );
		}
		else //Add from Hex-Id
		{
			var Word = execString.split(" ");
			if( Word[0] == "hex" )
			{
				pSock.xText = Word[1];
				pUser.SetTag( "AddFromHex", "Yep" );
				pSock.CustomTarget( 6, "On what character do you wish to add the item "+Word[1]+"?" );
			}
			else
				pUser.SysMessage( "Erroneous parameter specified! Try 'ADDPACK hex <hexid> or 'ADDPACK <item-id from dfn>" );
		}
	}
}

function command_SETTAG( pSock, execString )
{
	var pUser = pSock.currentChar;
	var Word = execString.split(",");
	if(( execString == "" || execString == null ) || ( Word[0] == null || Word[0] == "" || Word[0] == " " || Word[1] == "" || Word[1] == null ))
		pUser.SysMessage( "You need to specify a tag and a value for the tag, seperated by a comma." );
	else
	{
		pUser.SetTag( "Word0", Word[0] );
		pUser.SetTag( "Word1", Word[1] );
		pUser.CustomTarget( 8, "Apply tag to which object?" );
	}
}

function command_GETTAG( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString == null || execString == "" )
		pUser.SysMessage( "You need to specify a tagname to retrieve the value for" );
	else
	{
		pUser.SetTag( "TempTag", execString );
		pUser.CustomTarget( 9, "Retrieve tag from which object?" );
	}
}

function command_NODECAY( pSock, execString )
{
	var pUser = pSock.currentChar;
	pUser.CustomTarget( 10, "Select an item to set as NOT decayable:" );
}

function command_DECAY( pSock, execString )
{
	var pUser = pSock.currentChar;
	pUser.CustomTarget( 11, "Select an item to set as decayable:" );
}

function command_XSAY( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString )
	{
		pSock.xText = execString;
		pUser.CustomTarget( 12, "Select object for remote speech:" );
	}
	else
		pUser.SysMessage( "You forgot to write some text to go with this command." );
}

//Rename
function onCallback0( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var NewName = pSock.xText;
	if( !pSock.GetWord( 1 ))
	{
		pUser.SysMessage( "'"+myTarget.name+"' has been renamed to '"+NewName+"'." );
		myTarget.name = NewName;
	}
	else
	{
		pUser.SysMessage( "You cannot rename that!" );
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
		pUser.SysMessage( "The selected character has been frozen." );
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem  )
	{
		myTarget.movable = 2;
		myTarget.decayable = false;
		pUser.SysMessage( "The selected item has been frozen." );
	}
	else
		pUser.SysMessage( "You cannot freeze that." );
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
			pUser.SysMessage( "The selected item has been unfrozen." );
		}
		else
			pUser.SysMessage( "That character isn't frozen! Can't unfreeze!" );
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		if( myTarget.movable <= 1 )
			pUser.SysMessage( "That item isn't frozen! Can't unfreeze!" );
		else
		{
			myTarget.movable = 1;
			pUser.SysMessage( "The selected item has been unfrozen." );
		}
	}
	else
		pUser.SysMessage( "You cannot unfreeze that." );
}

function onCallback3( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		if( myTarget.vulnerable == false )
		{
			pUser.SysMessage( "Error! That target is already invulnerable!" );
		}
		else
		{
			pUser.SysMessage( "The selected target has been made invulnerable." );
			myTarget.vulnerable = false;
		}
	}
	else
		pUser.SysMessage( "That is not a character. Try again." );
}
function onCallback4( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		if( myTarget.vulnerable == true )
			pUser.SysMessage( "Error! That target is already vulnerable!" );
		else
		{
			pUser.SysMessage( "The selected target has been made vulnerable." );
			myTarget.vulnerable = true;
		}
	}
	else
		pUser.SysMessage( "That is not a character. Try again." );
}

//Addpack without parameters
function onCallback5( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var targX = pSock.GetWord( 11 );
	var targY = pSock.GetWord( 13 );
	var targZ = pSock.GetSByte( 16 );
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{ //add backpack on character
		var tempObj = myTarget.FindItemLayer(21);
		if( tempObj == null )
		{
			var newPack = CreateDFNItem( pUser.socket, pUser, "0x09b2", 1, "ITEM", false );
			newPack.container = pUser;
			newPack.layer = 21;
			newPack.weight = 0;
		}
		else
			pUser.SysMessage( "That character already has a backpack. No new backpack added." );
	}
	else
	{ //add backpack on ground
		var newPack = CreateDFNItem( pUser.socket, pUser, "0x09b2", 1, "ITEM", false );
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
	var Word1 = Number(Word1);
	var AddFromHex = pUser.GetTag( "AddFromHex" );
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		var tempObj = myTarget.FindItemLayer(21);
		if( tempObj != null )
		{
			if( AddFromHex != "Yep" )
				var tempItem = CreateDFNItem( pUser.socket, pUser, TempItemID, 1, "ITEM", true );
			else
				var tempItem = CreateBlankItem( pSock, pUser, 1, "#", Word1, 0x0, "ITEM", true );
		}
		else
		{
			pUser.SysMessage( "That character has no backpack! Backpack being added before new item..." );
			var newPack = CreateDFNItem( pUser.socket, pUser, "0x09b2", 1, "ITEM", false );
			newPack.container = pUser;
			newPack.layer = 21;
			newPack.weight = 0;
			newPack.maxItems = parseInt( GetServerSetting( "MAXPLAYERPACKITEMS" ));
			if( AddFromHex != "Yep" )
				var tempItem = CreateDFNItem( pUser.socket, pUser, TempItemID, 1, "ITEM", false );
			else
				var tempItem = CreateBlankItem( pSock, pUser, 1, "#", Word1, 0x0, "ITEM", false );
		}
	}
	else
		pUser.SysMessage( "That is no character. Try again." );
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
			myTarget.SetTag( Word0, null );
		else
			myTarget.SetTag( Word0, Word1 );
		pUser.SysMessage( "You have set a tag named '"+Word0+"' with a value of '"+Word1+"' on the targeted object." );
	}
	else
		pUser.SysMessage( "You need to target a dynamic object (item or character)." );
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
		pUser.SysMessage( "The value of the targeted object's '"+TempTagName+"'-tag is: "+TagData );
	}
	else
		pUser.SysMessage( "You need to target a dynamic object (item or character)." );
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
			pUser.SysMessage( "Item successfully set as NOT decayable." );
		}
		else
			pUser.SysMessage( "This command can only be applied to items." );
	}
	else
		pUser.SysMessage( "You need to target a dynamic item." );
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
			pUser.SysMessage( "Item successfully set as decayable." );
		}
		else
			pUser.SysMessage( "This command can only be applied to items." );
	}
	else
		pUser.SysMessage( "You need to target a dynamic item." );
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
		pUser.SysMessage( "You must target either a character or a dynamic item." );
}


function command_LINKDOORS( pSock, execString )
{
	pSock.CustomTarget( 13, "Which two doors do you want to link? (1/2)" );
}

function onCallback13( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isItem && pSock.clickX != 1)
	{
		pSock.tempObj = myTarget;
		pSock.clickX = 1;
		pSock.CustomTarget( 13, "Which two doors do you want to link? (2/2)" );
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem && pSock.clickX == 1)
	{
		var Door1 = pSock.tempObj;
		var Door2 = myTarget;

		Door1.SetTag( "linked", true );
		Door1.SetTag( "linkSer1", Door2.GetSerial(1) );
		Door1.SetTag( "linkSer2", Door2.GetSerial(2) );
		Door1.SetTag( "linkSer3", Door2.GetSerial(3) );
		Door1.SetTag( "linkSer4", Door2.GetSerial(4) );

		Door2.SetTag( "linked", true );
		Door2.SetTag( "linkSer1", Door1.GetSerial(1) );
		Door2.SetTag( "linkSer2", Door1.GetSerial(2) );
		Door2.SetTag( "linkSer3", Door1.GetSerial(3) );
		Door2.SetTag( "linkSer4", Door1.GetSerial(4) );
		pUser.SysMessage( "The two doors have been linked." );
		pSock.clickX = null;
	}
	else
		pUser.SysMessage( "You need to target an item." );
}

function command_UNLINKDOORS( pSock, execString )
{
	pSock.CustomTarget( 14, "Unlink which two doors? (1/2)" );
}

function onCallback14( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isItem && pSock.clickX != 1)
	{
		pSock.tempObj = myTarget;
		pSock.clickX = 1;
		pSock.CustomTarget( 14, "Unlink which two doors? (2/2)" );
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
		pUser.SysMessage( "The two doors have been unlinked." );
		pSock.clickX = null;
	}
	else
		pUser.SysMessage( "You need to target an item." );
}

function command_SETAMMOEFFECT( pSock, execString )
{
	if( execString != "" )
	{
		pSock.xText = execString;
		pSock.CustomTarget( 15, "Set ammoeffect on which bow?" );
	}
}

function onCallback15( pSock, myTarget )
{
	var ammoEffect = pSock.xText;
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
		myTarget.ammoEffect = ammoEffect;
}

function command_SETAMMOTYPE( pSock, execString )
{
	if( execString != "" )
	{
		pSock.xText = execString;
		pSock.CustomTarget( 16, "Set ammotype on which bow?" );
	}
}

function onCallback16( pSock, myTarget )
{
	var ammoType = pSock.xText;
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
		myTarget.ammoType = ammoType;
}

function command_GETAMMOEFFECT( pSock, execString )
{
	pSock.CustomTarget( 17, "Get ammoeffect from which bow?" );
}

function onCallback17( pSock, myTarget )
{
	var ammoEffect = myTarget.ammoEffect;
	pUser.TextMessage( "AmmoEffect on selected bow: "+ammoEffect );
}

function command_GETAMMOTYPE( pSock, execString )
{
	var pUser = pSock.currentChar;
	pSock.CustomTarget( 18, "Get ammotype from which bow?" );
}

function onCallback18( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var ammoType = myTarget.ammoType;
	pUser.TextMessage( "AmmoType on selected bow: "+ammoType );
}

function command_UNDRESS( pSock, execString )
{
	var pUser = pSock.currentChar;
	var i = 0;
	for( i = 0; i <= 24; i++ )
	{
		var tempObj = pUser.FindItemLayer(i);
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
	pSock.CustomTarget( 19, "Get region info for which character?" );
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
	pSock.CustomTarget( 20, "Set which item as nonmovable container?" );
}

function onCallback20( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isChar )
	{
		pUser.SysMessage( "You must select a container!" );
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem  )
	{
		myTarget.movable = 2;
		myTarget.decayable = false;
		myTarget.type = 1;
		pUser.SysMessage( "The selected item has been nonmovable container'd." );
	}
	else
		pUser.SysMessage( "Impossible!" );
}

function command_ENDFIGHT( pSock, execString )
{
	pSock.CustomTarget( 21, "Subdue which fight?" );
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
			pUser.SysMessage( "Fight has been subdued." );
		}
		else
			pUser.SysMessage( "That character is not in a fight." );
	}
	else if( !pSock.GetWord( 1 ) && myTarget.isItem  )
	{
		pUser.SysMessage( "You must select a character!" );
	}
	else
		pUser.SysMessage( "Impossible!" );
}

function command_GETMULTI( pSock )
{
	pSock.CustomTarget( 22, "Get MultiObj from which item?" );
}

function onCallback22( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		var multiObj = myTarget.multi;
		if( multiObj )
			pUser.SysMessage( "Serial of multiObj: "+multiObj.serial );
		else
			pUser.SysMessage( "Target does not belong to a multiObj." );
	}
	else
		pUser.SysMessage( "Target is not an item." );
}

function command_FINDITEM( pSock, execString )
{
	pSock.CustomTarget( 23, "Find item at layer "+execString+" on which character?" );
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
			pUser.SysMessage( "Target has item with ID "+equippedItem.id+" equipped at layer "+myInt+"." );
		else
			pUser.SysMessage( "Target has no item equipped at layer "+myInt+"." );
	}
	else
		pUser.SysMessage( "Target is not a character." );
}

function command_MOVESPEED( pSock, execString )
{
	switch( execString )
	{
		case "0x0": // Normal mode
		case "0x1": // Mounted mode
		case "0x2": // Slow mode (walk only)
		case "0x3": // Hybrid mode ("jog"?)
		case "0x4": // Frozen
			pSock.CustomTarget( 24, "Choose target to set movement speed "+execString+" for:" );
			pSock.xText = execString;
			break;
		default:
			pSock.SysMessage( "Only values between 0x0 to 0x4 are supported!" );
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
		pUser.SysMessage( "Target is not a character." );
}

function command_WELCOME( pSock, execString )
{
	TriggerEvent( 1, "DisplayAdminWelcomeGump", pSock, pSock.currentChar );
}