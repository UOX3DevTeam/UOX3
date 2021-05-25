// Repeating Commands || by Xuri (xuri at uox3.org)
// v1.12
// Last updated: 21/03/2021
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
	RegisterCommand( "m", 2, true ); // Use 'm delete - Removes multiple targeted items.
	RegisterCommand( "radditem", 2, true ); // Use 'RADDITEM <item-id from dfns>
	RegisterCommand( "rtele", 2, true ); //Use 'RTELE <target teleport location>
	RegisterCommand( "raddnpc", 2, true ); //Use 'RADDNPC <id from DFNs> - Adds specified NPC at multiple targeted locations
}

//Repeated Command: INCX <value>
function command_RINCX( pSock, execString )
{
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pSock.CustomTarget( 0, "Select target to reposition by "+execString+" X:" );
	}
	else
		pSock.SysMessage( ReqNum );
}
function onCallback0( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var incXValue = pSock.xText;
		var incXValue = Number(incXValue);
		if( !pSock.GetWord( 1 ))
				myTarget.x+= incXValue;
		pSock.CustomTarget( 0, "Select target to reposition by "+incXValue+" X:" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: INCY <value>
function command_RINCY( pSock, execString )
{
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pSock.CustomTarget( 1, "Select target to reposition by "+execString+" Y:" );
	}
	else
		pSock.SysMessage( ReqNum );
}
function onCallback1( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var incYValue = pSock.xText;
		var incYValue = Number(incYValue);
		if( !pSock.GetWord( 1 ))
				myTarget.y += incYValue;
		pSock.CustomTarget( 1, "Select target to reposition by "+incYValue+" Y:" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: INCZ <value>
function command_RINCZ( pSock, execString )
{
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pSock.CustomTarget( 2, "Select target to reposition by "+execString+" Z:" );
	}
	else
		pSock.SysMessage( ReqNum );
}
function onCallback2( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var incZValue = pSock.xText;
		var incZValue = Number(incZValue);
		if( !pSock.GetWord( 1 ))
				myTarget.z += incZValue;
		pSock.CustomTarget( 2, "Select target to reposition by "+incZValue+" Z:" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: SET TYPE <type>
function command_RTYPE( pSock, execString )
{
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pSock.CustomTarget( 4, "Select target to make TYPE "+execString+":" );
	}
	else
		pSock.SysMessage( ReqNum );
}
function onCallback4( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var TempType = pSock.xText;
		var TempType = Number(TempType);
		if( !pSock.GetWord( 1 ))
				myTarget.type = TempType;
		pSock.CustomTarget( 4, "Select target to make TYPE "+TempType+":" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: DYE <hex-id>
function command_RDYE( pSock, execString )
{
	if( !isNaN(execString))
	{
		pSock.xText = execString;
		pSock.CustomTarget( 5, "Select target to DYE "+execString+":" );
	}
	else
		pSock.SysMessage( ReqNum );
}
function onCallback5( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var TempDye = pSock.xText;
		var TempDye = Number(TempDye);
		if( !pSock.GetWord( 1 ))
				myTarget.colour = TempDye;
		pSock.CustomTarget( 5, "Select target to DYE "+TempDye+":" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: ADD <hex-id>
function command_RADD( pSock, execString )
{
	// if( !isNaN(execString))
	// {
		var splitString = execString.split( " " );
		pSock.xText = execString;
		pSock.CustomTarget( 6, "Select target location for item 0x"+parseInt(splitString[0]).toString(16)+":" );
	// }
	// else
		// pSock.SysMessage( ReqNum );
}
function onCallback6( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var tempItemID = ""; //pSock.xText;

		var splitString = pSock.xText.split( " " );
		if( splitString[1] )
		{
			// radd itemID rndValue
			tempItemID = (parseInt(splitString[0]) + RandomNumber(0, parseInt(splitString[1]))).toString();
		}
		else
		{
			tempItemID = pSock.xText;
		}

		// Find targeted location
		var targX = pSock.GetWord( 11 );
		var targY = pSock.GetWord( 13 );
		var targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );

		// Create item and set it's location
		var pUser = pSock.currentChar;
		var tempItem = CreateBlankItem( pSock, pUser, 1, "#", parseInt(tempItemID), 0x0, "ITEM", false );
		if( tempItem )
		{
			tempItem.SetLocation( targX, targY, targZ );
			tempItem.decayable = false;
			pSock.CustomTarget( 6, "Select target location for item 0x0"+parseInt(tempItemID).toString(16)+":" );
		}
		else
			pSock.SysMessage( "The specified itemID does not seem to be valid. No item added!" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: REMOVE
function command_RREMOVE( pSock, execString )
{
	pSock.CustomTarget( 7, "Which object do you wish to remove?" );
}
//Repeated Command: M DELETE
function command_M( pSock, execString )
{
	var splitString = execString.split( " " );
	if( splitString[0].toUpperCase() == "DELETE" )
		pSock.CustomTarget( 7, "Which object do you wish to remove?" );
}
function onCallback7( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		if( !pSock.GetWord( 1 ))
		{
			if( ValidateObject(myTarget) && myTarget.npc || myTarget.isItem )
				myTarget.Delete();
			else
				pSock.SysMessage( "You can only remove items or NPCs." );
		}
		else
			pSock.SysMessage( "You can only remove items or NPCs." );

		pSock.CustomTarget( 7, "Which object do you wish to remove?" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: ADD ITEM <item-id from dfns>
function command_RADDITEM( pSock, execString )
{
	pSock.xText = execString;
	pSock.CustomTarget( 8, "Select target location for item "+execString+":" );
}
function onCallback8( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var pUser = pSock.currentChar;
		var TempItemID = pSock.xText;
		if( !(TempItemID == null) )
		{
			var targX = pSock.GetWord( 11 );
			var targY = pSock.GetWord( 13 );
			var targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );
			var tempItem = CreateDFNItem( pSock, pUser, TempItemID, 1, "ITEM", false );
			if( tempItem )
			{
				tempItem.x = targX;
				tempItem.y = targY;
				tempItem.z = targZ;
				pSock.CustomTarget( 8, "Select target location for item "+TempItemID+":" );
			}
			else
				pSock.SysMessage( "That doesn't seem to be a valid item-id from the DFNs. No item added!" );
		}
		else
			pSock.SysMessage( "That doesn't seem to be a valid item-id from the DFNs. No item added!" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: TELE <select target location>
function command_RTELE( pSock, execString )
{
	pSock.CustomTarget( 9, "Select location to teleport to:" );
}
function onCallback9( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var pUser = pSock.currentChar;
		var targX = pSock.GetWord( 11 );
		var targY = pSock.GetWord( 13 );
		var targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );

		pUser.Teleport( targX, targY, targZ );
		pSock.CustomTarget( 9, "Select location to teleport to:" );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}

//Repeated Command: ADD NPC <npc-id from DFNs>
function command_RADDNPC( pSock, execString )
{
	pSock.xText = execString;
	pSock.CustomTarget( 10, "Select target location for the ["+execString+"]:" );
}
function onCallback10( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var pUser = pSock.currentChar;
		var TempNPCID = pSock.xText;
		if( !(TempNPCID == null ) )
		{
			var targX = pSock.GetWord( 11 );
			var targY = pSock.GetWord( 13 );
			var targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );
			var newNPC = SpawnNPC( TempNPCID, targX, targY, targZ, pUser.worldnumber, pUser.instanceID );
			pSock.CustomTarget( 10, "Select target location for the ["+TempNPCID+"]:" );
		}
		else
			pSock.SysMessage( "That doesn't seem to be a valid NPC-id from the DFNs." );
	}
	else
		pSock.SysMessage( "Repeating command ended." );
}