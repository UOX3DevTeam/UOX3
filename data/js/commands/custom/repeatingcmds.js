// Repeating Commands || by Xuri (xuri at sensewave.com)
// v1.04
// Last updated: December 28th 2004
//
// This script contains commands which will make worldbuilding and constructing buildings ingame easier for the GMs.
// Any of the commands will, when used, be repeated over and over again after a target has been selected, so there will
// be no need for the user to repeatedly enter the same command.

var ReqNum = "You need to enter a numerical value with this command!";

function CommandRegistration(contextID)
{
	RegisterCommand( "rincx", 2, true,contextID ); // USE 'RINCX <value> - increases X value of multiple targets by <value>
	RegisterCommand( "rincy", 2, true,contextID ); // USE 'RINCY <value> - increases Y value of multiple targets by <value>
	RegisterCommand( "rincz", 2, true,contextID ); // Use 'RINCZ <value> - increases Z value of multiple targets by <value>
	RegisterCommand( "rtype", 2, true,contextID ); // Use 'RTYPE <type> - sets multiple targeted objects to TYPE <type>
	RegisterCommand( "rdye", 2, true,contextID ); // Use 'RDYE <hex-id> - dyes multiple targeted objects with colour <hex-id>
	RegisterCommand( "radd", 2, true,contextID ); // Use 'RADD <hex-id> - adds item <hex-id> at multiple targeted locations
	RegisterCommand( "rremove", 2, true,contextID ); // Use 'RREMOVE - Removes multiple targeted items.
	RegisterCommand( "radditem", 2, true,contextID ); // Use 'RADDITEM <item-id from dfns>
}

//Repeated Command: INCX <value>
function command_RINCX( pSock, execString )
{
	pUser = pSock.currentChar;
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
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 )
	{
			pUser.SysMessage( "Bleh?" );
			myTarget.x+= incXValue;
	}
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
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 )
	{
			myTarget.y += incYValue;
	}
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
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 )
	{
			myTarget.z += incZValue;
	}
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
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 )
	{
			myTarget.type = TempType;
	}
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
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 )
	{
			myTarget.colour = TempDye;
	}
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
	var targZ = pSock.GetByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );

	var tempItem = CreateBlankItem( pSock, pUser, 1, "#", TempItemID, 0x0, "ITEM", false );	
	if( tempItem )
		tempItem.SetLocation( targX, targY, targZ );
	pUser.CustomTarget( 6, "Select target location for item "+TempItemID+":" );
}

//Repeated Command: REMOVE
function command_RREMOVE( pSock, execString )
{
	pUser = pSock.currentChar;
	pUser.CustomTarget( 7, "Which item do you wish to remove?" );
}
function onCallback7( pSock, myTarget )
{
	var pUser = pSock.currentChar; 
	var StrangeByte = pSock.GetWord( 1 );
	if( StrangeByte == 0 && !myTarget.isChar )
	{
			myTarget.Delete();
	}
	else
	{
		pUser.SysMessage( "That's not an item you can remove!" );
	}
	pUser.CustomTarget( 7, "Which item do you wish to remove?" );
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
	if( !TempItemID == null )
	{
		var targX = pSock.GetWord( 11 );
		var targY = pSock.GetWord( 13 );
		var targZ = pSock.GetByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );
		pUser.SysMessage( TempItemID );
		var tempItem = CreateDFNItem( pSock, pUser, TempItemID, false, 10, false );
		tempItem.x = targX;
		tempItem.y = targY;
		tempItem.z = targZ;
		pUser.CustomTarget( 8, "Select target location for item "+TempItemID+":" );
	}
	else
		pUser.SysMessage( "That doesn't seem to be a valid item-id from the DFNs." );
}