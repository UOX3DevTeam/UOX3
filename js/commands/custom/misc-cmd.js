// Miscellaneous Custom Commands || by Xuri (xuri at sensewave.com)
// v1.05
// Last Updated: 26. July 2004
//
// This script contains some commands I scripted after the command-reorganization in the UOX3 source code,
// as well as some I've "invented" on my own.
// Updates:
// 10. July - added SETTAG and GETTAG commands
// 11. July - Allowed setting tags with value "null" to delete tags.

function CommandRegistration()
{
	RegisterCommand( "rename", 2, true ); //Lets GMs rename items/characters.
	RegisterCommand( "refresh", 0, true ); //Lets players refresh their screen to resend items/chars that have vanished from view.
	RegisterCommand( "freeze", 2, true ); //Will "freeze" any targeted char, and will make any targeted item immovable (i.e. locked down by GM)
	RegisterCommand( "unfreeze", 2, true );
	RegisterCommand( "browse", 0, true ); //WIll let users open a webpage in their default browser from within the UO client. BROWSE <url>
	RegisterCommand( "invul", 2, true ); //Will make the targeted character invulnerable or not, depending on the argument provided (true/false, 1/0)
	RegisterCommand( "addpack", 2, true ); //Will add a backpack to the targeted character, if it has none. Will add specified item-id(addpack <item-id> or hex id (addpack hex <hexid>) to backpack.
	RegisterCommand( "settag", 2, true ); //used to specify a value for a specified tag on a targeted object
	RegisterCommand( "gettag", 2, true ); //Used to retrieve the value of a specified tag from a targeted object
}

function command_RENAME( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( !execString == "" )
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
		pUser.SysMessage( "That's not a valid URL." );
}

function command_INVUL( pSock, execString )
{
	if( execString == "" )
	{
		pSock.SysMessage( "You need to provide an argument with this command! Either true or false, 1 or 0!" );
		return;
	}
	if( execString == "true" || execString == 1 )
		pSock.CustomTarget( 3, "Whom do you wish to make invulnerable?" );
	else if ( execString == "false" || execString == 0 )
		pSock.CustomTarget( 4, "Whom do you wish to make vulnerable?" );
}

function command_ADDPACK( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString == "" )
	{
		var pUser = pSock.currentChar;
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
	if( execString == "" || execString == null )
	{
		pUser.SysMessage( "You need to specify a tag and a value for the tag, seperated by a comma." );
		return;
	}
	var Word = execString.split(",");
	if( Word[0] == null || Word[0] == "" || Word[0] == " " || Word[1] == "" || Word[1] == null )
	{
		pUser.SysMessage( "You need to specify a tag and a value for the tag, seperated by a comma." );
		return;
	}
	pUser.SetTag( "Word0", Word[0] );
	pUser.SetTag( "Word1", Word[1] );
	pUser.CustomTarget( 8, "Apply tag to which object?" );
}

function command_GETTAG( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( execString == null || execString == "" )
	{
		pUser.SysMessage( "You need to specify a tagname to retrieve the value for" );
		return;
	}
	pUser.SetTag( "TempTag", execString );
	pUser.CustomTarget( 9, "Retrieve tag from which object?" );
}

//Rename
function onCallback0( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var NewName = pSock.xText;
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 )
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
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 && myTarget.isChar  )
	{
		myTarget.frozen = true;
		pUser.SysMessage( "The selected character has been frozen." );
		return;
	}
	if( StrangeByte == 0 && !myTarget.isChar  )
	{
		myTarget.movable = 2;
		pUser.SysMessage( "The selected item has been frozen." );
		return;
	}
	pUser.SysMessage( "You cannot freeze that." );
}

//Unfreeze
function onCallback2( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 && myTarget.isChar  )
	{
		if( myTarget.frozen == true )
		{
			myTarget.frozen = false;
			pUser.SysMessage( "The selected item has been unfrozen." );
		}
		else
			pUser.SysMessage( "That character isn't frozen! Can't unfreeze!" );
		return;
	}
	if( StrangeByte == 0 && !myTarget.isChar  )
	{
		if( myTarget.movable <= 1 )
			pUser.SysMessage( "That item isn't frozen! Can't unfreeze!" );
		else
		{
			myTarget.movable = 1;
			pUser.SysMessage( "The selected item has been unfrozen." );
		}
		return;
	}
	pUser.SysMessage( "You cannot unfreeze that." );
}

function onCallback3( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 && myTarget.isChar  )
	{
		if( myTarget.vulnerable == false )
		{
			pUser.SysMessage( "Error! That target is already invulnerable!" );
			return;
		}
		pUser.SysMessage( "The selected target has been made invulnerable." );
		myTarget.vulnerable = false;
		return;
	}
	pUser.SysMessage( "That is not a character. Try again." );
}
function onCallback4( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 && myTarget.isChar  )
	{
		if( myTarget.vulnerable == true )
		{
			pUser.SysMessage( "Error! That target is already vulnerable!" );
			return;
		}
		pUser.SysMessage( "The selected target has been made vulnerable." );
		myTarget.vulnerable = true;
		return;
	}
	pUser.SysMessage( "That is not a character. Try again." );
}

//Addpack without parameters
function onCallback5( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var targX = pSock.GetWord( 11 );
	var targY = pSock.GetWord( 13 );
	var targZ = pSock.GetByte( 16 );
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 && myTarget.isChar  )
	{
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
		return;
	}
	else
	{
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
	pUser.SysMessage( TempItemID );
	var Word1 = pSock.xText;
	var Word1 = Number(Word1);
	var AddFromHex = pUser.GetTag( "AddFromHex" );
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 && myTarget.isChar  )
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
			if( AddFromHex != "Yep" )
				var tempItem = CreateDFNItem( pUser.socket, pUser, TempItemID, 1, "ITEM", false );				
			else
				var tempItem = CreateBlankItem( pSock, pUser, 1, "#", Word1, 0x0, "ITEM", false );			
		}
		pUser.SetTag( "AddFromHex", null );
		return;
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
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 )
	{
		if( Word1 == "null" )
			myTarget.SetTag( Word0, null );
		else
			myTarget.SetTag( Word0, Word1 );
		pUser.SysMessage( "You have set a tag named '"+Word0+"' with a value of '"+Word1+"' on the targeted object." );
	}
	else
	{
		pUser.SysMessage( "You need to target a dynamic object (item or character)." );	
	}
	pUser.SetTag( "Word0", null );
	pUser.SetTag( "Word1", null );
}

// Get Tag
function onCallback9( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var TempTagName = pUser.GetTag( "TempTag" );
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 )
	{
		var TagData = myTarget.GetTag( TempTagName );
		pUser.SysMessage( "The value of the targeted object's '"+TempTagName+"'-tag is: "+TagData );
	}
	else
	{
		pUser.SysMessage( "You need to target a dynamic object (item or character)." );	
	}
	pUser.SetTag( "TempTag", null );
}
