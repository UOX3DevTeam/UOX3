// Repeating Commands || by Xuri (xuri at sensewave.com)
// v1.09
// Last updated: April 16th 2006
//
// This script contains commands which will make worldbuilding and constructing buildings ingame easier for the GMs.
// Any of the commands will, when used, be repeated over and over again after a target has been selected, so there will
// be no need for the user to repeatedly enter the same command.

ReqNum = "You need to enter a numerical value with this command!";

function CommandRegistration()
{
	RegisterCommand( "rincx", 2, true ); // USE 'RINCX <value> - increases X value of multiple targets by <value>
	RegisterCommand( "rincy", 2, true ); // USE 'RINCY <value> - increases Y value of multiple targets by <value>
	RegisterCommand( "rincz", 2, true ); // Use 'RINCZ <value> - increases Z value of multiple targets by <value>
	RegisterCommand( "rtype", 2, true ); // Use 'RTYPE <type> - sets multiple targeted objects to TYPE <type>
	RegisterCommand( "rdye", 2, true ); // Use 'RDYE <hex-id> - dyes multiple targeted objects with colour <hex-id>
	RegisterCommand( "radd", 2, true ); // Use 'RADD <hex-id> - adds item <hex-id> at multiple targeted locations
	RegisterCommand( "rremove", 2, true ); // Use 'RREMOVE - Removes multiple targeted items.
	RegisterCommand( "radditem", 2, true ); // Use 'RADDITEM <item-id from dfns>
	RegisterCommand( "rtele", 2, true ); //Use 'RTELE <target teleport location>
	RegisterCommand( "raddnpc", 2, true ); //Use 'RADDNPC <id from DFNs> - Adds specified NPC at multiple targeted locations
}

//Repeated Command: INCX <value>
function command_RINCX( pSock, execString )
{
	var pUser = pSock.currentChar;
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pUser.CustomTarget( 0, "Select target to reposition by "+execString+" X:" );
	}
	else
		pUser.SysMessage( ReqNum );
}
function onCallback0( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var incXValue = pSock.xText;
	var incXValue = Number(incXValue);
	if( !pSock.GetWord( 1 ))
			myTarget.x+= incXValue;
	pUser.CustomTarget( 0, "Select target to reposition by "+incXValue+" X:" );
}

//Repeated Command: INCY <value>
function command_RINCY( pSock, execString )
{
	pUser = pSock.currentChar;
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pUser.CustomTarget( 1, "Select target to reposition by "+execString+" Y:" );
	}
	else
		pUser.SysMessage( ReqNum );
}
function onCallback1( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var incYValue = pSock.xText;
	var incYValue = Number(incYValue);
	if( !pSock.GetWord( 1 ))
			myTarget.y += incYValue;
	pUser.CustomTarget( 1, "Select target to reposition by "+incYValue+" Y:" );
}
//Repeated Command: INCZ <value>
function command_RINCZ( pSock, execString )
{
	pUser = pSock.currentChar;
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pUser.CustomTarget( 2, "Select target to reposition by "+execString+" Z:" );
	}
	else
		pUser.SysMessage( ReqNum );
}
function onCallback2( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var incZValue = pSock.xText;
	var incZValue = Number(incZValue);
	if( !pSock.GetWord( 1 ))
			myTarget.z += incZValue;
	pUser.CustomTarget( 2, "Select target to reposition by "+incZValue+" Z:" );
}

//Repeated Command: SET TYPE <type>
function command_RTYPE( pSock, execString )
{
	pUser = pSock.currentChar;
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pUser.CustomTarget( 4, "Select target to make TYPE "+execString+":" );
	}
	else
		pUser.SysMessage( ReqNum );
}
function onCallback4( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var TempType = pSock.xText;
	var TempType = Number(TempType);
	if( !pSock.GetWord( 1 ))
			myTarget.type = TempType;
	pUser.CustomTarget( 4, "Select target to make TYPE "+TempType+":" );
}

//Repeated Command: DYE <hex-id>
function command_RDYE( pSock, execString )
{
	pUser = pSock.currentChar;
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pUser.CustomTarget( 5, "Select target to DYE "+execString+":" );
	}
	else
		pUser.SysMessage( ReqNum );
}
function onCallback5( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var TempDye = pSock.xText;
	var TempDye = Number(TempDye);
	if( !pSock.GetWord( 1 ))
			myTarget.colour = TempDye;
	pUser.CustomTarget( 5, "Select target to DYE "+TempDye+":" );
}

//Repeated Command: ADD <hex-id>
function command_RADD( pSock, execString )
{
	pUser = pSock.currentChar;
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pUser.CustomTarget( 6, "Select target location for item "+execString+":" );
	}
	else
		pUser.SysMessage( ReqNum );	
}
function onCallback6( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var TempItemID = pSock.xText;
	var TempItemID = Number(TempItemID);
	var targX = pSock.GetWord( 11 );
	var targY = pSock.GetWord( 13 );
	var targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );

	var tempItem = CreateBlankItem( pSock, pUser, 1, "#", TempItemID, 0x0, "ITEM", false );	
	if( tempItem )
	{
		tempItem.SetLocation( targX, targY, targZ );
		tempItem.decayable = false;
	}
		
	pUser.CustomTarget( 6, "Select target location for item "+TempItemID+":" );
}

//Repeated Command: REMOVE
function command_RREMOVE( pSock, execString )
{
	pUser = pSock.currentChar;
	pUser.CustomTarget( 7, "Which object do you wish to remove?" );
}
function onCallback7( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	if( !pSock.GetWord( 1 ))
	{
		if( myTarget.npc || myTarget.isItem )
			myTarget.Delete();
		else
			pUser.SysMessage( "You can only remove items or NPCs." );
	}
	else
		pUser.SysMessage( "You can only remove items or NPCs." );
	pUser.CustomTarget( 7, "Which object do you wish to remove?" );
}

//Repeated Command: ADD ITEM <item-id from dfns>
function command_RADDITEM( pSock, execString )
{
	pUser = pSock.currentChar;
	pSock.xText = execString;
	pUser.CustomTarget( 8, "Select target location for item "+execString+":" );
}
function onCallback8( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var TempItemID = pSock.xText;
	if( !(TempItemID == null) )
	{
		var targX = pSock.GetWord( 11 );
		var targY = pSock.GetWord( 13 );
		var targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );
		var tempItem = CreateDFNItem( pSock, pUser, TempItemID, 1, "ITEM", false );
		tempItem.x = targX;
		tempItem.y = targY;
		tempItem.z = targZ;
		pUser.CustomTarget( 8, "Select target location for item "+TempItemID+":" );
	}
	else
		pUser.SysMessage( "That doesn't seem to be a valid item-id from the DFNs." );
}

//Repeated Command: TELE <select target location>
function command_RTELE( pSock, execString )
{
	pUser = pSock.currentChar;
	pUser.CustomTarget( 9, "Select location to teleport to:" );
}
function onCallback9( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var targX = pSock.GetWord( 11 );
	var targY = pSock.GetWord( 13 );
	var targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );
	pUser.Teleport( targX, targY, targZ );
	pUser.CustomTarget( 9, "Select location to teleport to:" );
}

//Repeated Command: ADD NPC <npc-id from DFNs>
function command_RADDNPC( pSock, execString )
{
	pUser = pSock.currentChar;
	pSock.xText = execString;
	pUser.CustomTarget( 10, "Select target location for the ["+execString+"]:" );
}
function onCallback10( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var TempNPCID = pSock.xText;
	if( !(TempNPCID == null ) )
	{
		var targX = pSock.GetWord( 11 );
		var targY = pSock.GetWord( 13 );
		var targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );
		var newNPC = SpawnNPC( TempNPCID, targX, targY, targZ, pUser.worldnumber );
		pUser.CustomTarget( 10, "Select target location for the ["+TempNPCID+"]:" );
	}
	else
		pUser.SysMessage( "That doesn't seem to be a valid NPC-id from the DFNs." );
}