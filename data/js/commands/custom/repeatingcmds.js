// Repeating Commands || by Xuri ( xuri at uox3.org )
// v1.15
// Last updated: 06/11/2021
//
// This script contains commands which will make worldbuilding and constructing buildings ingame easier for the GMs.
// Any of the commands will, when used, be repeated over and over again after a target has been selected, so there will
// be no need for the user to repeatedly enter the same command.

const ReqNum = "You need to enter a numerical value with this command!";

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
	RegisterCommand( "raddspawner", 2, true ); //Use 'RADDSPAWNER <id from DFNs> - Adds specified Spawner at multiple targeted locations
	RegisterCommand( "rincid", 2, true ); // Use 'RINCID <value> - repeatedly increase/decrease ID of target by <value>
	RegisterCommand( "rmovable", 2, true ); // Use 'RMOVABLE <value> - repeatedly set movable status of target item to <value>
	RegisterCommand( "rnodecay", 2, true ); // Use 'RNODECAY - repeatedly set decayable status of target item to false
}

//Repeated Command: INCX <value>
function command_RINCX( pSock, execString )
{
	if( !isNaN( execString ))
	{
		pSock.xText = execString;

		var tempMsg = GetDictionaryEntry( 8929, pSock.language ); // Select target to reposition by %i X:
		pSock.CustomTarget( 0, tempMsg.replace( /%i/gi, execString ));
	}
	else
	{
		pSock.SysMessage( ReqNum );
	}
}
function onCallback0( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var incXValue = pSock.xText;
		var incXValue = Number( incXValue );
		if( !pSock.GetWord( 1 ))
		{
			myTarget.x += incXValue;
		}

		var tempMsg = GetDictionaryEntry( 8929, pSock.language ); // Select target to reposition by %i X:
		pSock.CustomTarget( 0, tempMsg.replace( /%i/gi, incXValue ));
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: INCY <value>
function command_RINCY( pSock, execString )
{
	if( !isNaN( execString ))
	{
		pSock.xText = execString;

		var tempMsg = GetDictionaryEntry( 8930, pSock.language ); // Select target to reposition by %i Y:
		pSock.CustomTarget( 1, tempMsg.replace( /%i/gi, execString ));
	}
	else
	{
		pSock.SysMessage( ReqNum );
	}
}
function onCallback1( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var incYValue = pSock.xText;
		var incYValue = Number( incYValue );
		if( !pSock.GetWord( 1 ))
		{
			myTarget.y += incYValue;
		}

		var tempMsg = GetDictionaryEntry( 8930, pSock.language ); // Select target to reposition by %i Y:
		pSock.CustomTarget( 1, tempMsg.replace( /%i/gi, incYValue ));
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: INCZ <value>
function command_RINCZ( pSock, execString )
{
	if( !isNaN( execString ))
	{
		pSock.xText = execString;
		var tempMsg = GetDictionaryEntry( 8931, pSock.language ); // Select target to reposition by %i Z:
		pSock.CustomTarget( 2, tempMsg.replace( /%i/gi, execString ));
	}
	else
	{
		pSock.SysMessage( ReqNum );
	}
}
function onCallback2( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var incZValue = pSock.xText;
		var incZValue = Number( incZValue );
		if( !pSock.GetWord( 1 ))
		{
			myTarget.z += incZValue;
		}

		var tempMsg = GetDictionaryEntry( 8931, pSock.language ); // Select target to reposition by %i Z:
		pSock.CustomTarget( 2, tempMsg.replace( /%i/gi, incZValue ));
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: SET TYPE <type>
function command_RTYPE( pSock, execString )
{
	if( !isNaN( execString ))
	{
		pSock.xText = execString;

		var tempMsg = GetDictionaryEntry( 8933, pSock.language ); // Select target to make TYPE %i:
		pSock.CustomTarget( 4, tempMsg.replace( /%i/gi, execString ));
	}
	else
	{
		pSock.SysMessage( ReqNum );
	}
}
function onCallback4( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var tempType = pSock.xText;
		var tempType = Number( tempType );
		if( !pSock.GetWord( 1 ))
		{
			myTarget.type = tempType;
		}

		var tempMsg = GetDictionaryEntry( 8933, pSock.language ); // Select target to make TYPE %i:
		pSock.CustomTarget( 4, tempMsg.replace( /%i/gi, tempType ));
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: DYE <hex-id>
function command_RDYE( pSock, execString )
{
	if( !isNaN( execString ))
	{
		pSock.xText = execString;
		var tempMsg = GetDictionaryEntry( 8935, pSock.language ); // Select target to DYE %s:
		pSock.CustomTarget( 5, tempMsg.replace( /%i/gi, execString ));
	}
	else
	{
		pSock.SysMessage( ReqNum );
	}
}
function onCallback5( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var tempDye = pSock.xText;
		var tempDye = Number( tempDye );
		if( !pSock.GetWord( 1 ))
		{
			myTarget.colour = tempDye;
		}
		var tempMsg = GetDictionaryEntry( 8935, pSock.language ); // Select target to DYE %s:
		pSock.CustomTarget( 5, tempMsg.replace( /%i/gi, tempDye ));
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: ADD <hex-id>
function command_RADD( pSock, execString )
{
	// if( !isNaN( execString ))
	// {
		var splitString = execString.split( " " );
		if( splitString[0] == "item" )
		{
			pSock.SysMessage( GetDictionaryEntry( 8210, pSock.language )); // Redirecting to 'radditem command...
			command_RADDITEM( pSock, splitString[1] );
			return;
		}
		if( splitString[0] == "npc" )
		{
			pSock.SysMessage( GetDictionaryEntry( 8211, pSock.language )); // Redirecting to 'raddnpc command...
			command_RADDNPC( pSock, splitString[1] );
			return;
		}

		if( isNaN( splitString[0] ) && isNaN( splitString[1] ))
		{
			pSock.SysMessage( GetDictionaryEntry( 8212, pSock.language )); // Valid command syntax for repeating adding objects: 'radd [hexID/decimalID], 'radditem [itemSection], 'raddnpc [npcSection]
			return;
		}

		var itemID;
		if( splitString[0] == "tree" )
		{
			if( splitString[1] )
			{
				itemID = parseInt( splitString[1] ).toString( 16 );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 9108, pSock.language )); // IDs for both tree trunk AND leaves are required by this command. Syntax: cmd tree [trunkID] [leafID]
				return;
			}
		}
		else
		{
			itemID = parseInt( splitString[0] ).toString( 16 );
		}

		pSock.xText = execString;
		var tempMsg = GetDictionaryEntry( 8936, pSock.language ); // Select target location for item 0x%i:
		pSock.CustomTarget( 6, tempMsg.replace( /%i/gi, itemID ));
	// }
	// else
	//{
		// pSock.SysMessage( ReqNum );
	//}
}
function onCallback6( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var tempItemID = ""; //pSock.xText;
		var tempItemID2 = "";

		var splitString = pSock.xText.split( " " );
		if( splitString[0] == "tree")
		{
			if( !splitString[1] || !splitString[2] )
			{
				pSock.SysMessage( GetDictionaryEntry( 9108, pSock.language )); // IDs for both tree trunk AND leaves are required by this command. Syntax: cmd tree [trunkID] [leafID]
				return;
			}
			tempItemID = splitString[1];
			tempItemID2 = splitString[2];
		}
		else if( splitString[1] )
		{
			// radd itemID rndValue
			tempItemID = ( parseInt( splitString[0] ) + RandomNumber( 0, parseInt( splitString[1] ))).toString();
		}
		else
		{
			tempItemID = pSock.xText;
		}

		// Find targeted location
		var targX = pSock.GetWord( 11 );
		var targY = pSock.GetWord( 13 );
		var targZ = pSock.GetSByte( 16 );
		var StrangeByte = pSock.GetWord(1);

		// If connected with a client lower than v7.0.9, manually add height of targeted tile
		if ((StrangeByte == 0 && myTarget.isItem) || (pSock.clientMajorVer <= 7 && pSock.clientSubVer < 9))
		{
			targZ += GetTileHeight( pSock.GetWord( 17 ));
		}

		// Create item and set it's location
		var pUser = pSock.currentChar;
		var tempItem = CreateBlankItem( pSock, pUser, 1, "#", parseInt( tempItemID ), 0x0, "ITEM", false );
		if( tempItem )
		{
			tempItem.SetLocation( targX, targY, targZ );
			tempItem.decayable = false;

			if( tempItemID2 != "" )
			{
				var tempItem2 = CreateBlankItem( pSock, pUser, 1, "#", parseInt( tempItemID2 ) + RandomNumber( 0, 1 ), 0x0, "ITEM", false );
				if( tempItem2 )
				{
					tempItem2.SetLocation( targX, targY, targZ );
					tempItem2.decayable = false;
				}
			}
			var tempMsg = GetDictionaryEntry( 8936, pSock.language ); // Select target location for item 0x%i:
			pSock.CustomTarget( 6, tempMsg.replace( /%i/gi, parseInt( tempItemID ).toString( 16 )));
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 8937, pSock.language )); // The specified itemID does not seem to be valid. No item added!
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: REMOVE
function command_RREMOVE( pSock, execString )
{
	pSock.CustomTarget( 7, GetDictionaryEntry( 8938, pSock.language ), 1 ); // Which object do you wish to remove?
}
//Repeated Command: M DELETE
function command_M( pSock, execString )
{
	var splitString = execString.split( " " );
	if( splitString[0].toUpperCase() == "DELETE" )
	{
		pSock.CustomTarget( 7, GetDictionaryEntry( 8938, pSock.language ), 1 ); // Which object do you wish to remove?
	}
}
function onCallback7( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		if( !pSock.GetWord( 1 ))
		{
			if( ValidateObject( myTarget ) && myTarget.npc || myTarget.isItem )
			{
				if( myTarget.isChar && ValidateObject( myTarget.owner ))
				{
					// Remove target as a follower of owner
					myTarget.owner.RemoveFollower( myTarget );

					// Reduce controlSlotsUsed for owner
					myTarget.owner.controlSlotsUsed -= myTarget.controlSlots;
				}

				if( myTarget.type == 1 && myTarget.id == 0x0e43 && ValidateObject( myTarget.multi ) && myTarget.more == myTarget.multi.serial )
				{
					// Chest of a tent multi. Destroy the multi too!
					var objMulti = myTarget.multi;
					objMulti.Delete();
				}

				// Reset hair/beard style/colour if item being removed is hair or beard on a character
				var packOwner = GetPackOwner( myTarget, 0 );
				if( packOwner != null && packOwner.isChar )
				{
					if( myTarget.layer == 0x0b ) // Hair
					{
						packOwner.hairStyle = 0;
						packOwner.hairColour = 0;
					}
					else if( myTarget.layer == 0x10 ) // Beard
					{
						packOwner.beardStyle = 0;
						packOwner.beardColour = 0;
					}
				}				

				// If item being removed was locked down in a multi, release item from multi to update lockdown count properly
				var iMulti = myTarget.multi;
				if( ValidateObject( iMulti ))
				{
					if( iMulti.IsSecureContainer( myTarget ))
					{
						// Targeted item is a secure container
						iMulti.UnsecureContainer( myTarget );
					}
					else
					{
						// Release the targeted item before deleting it
						iMulti.ReleaseItem( myTarget );
					}

					if( myTarget.type == 1 )
					{
						// Loop through any items in container and release them
						var tempItem;
						for( tempItem = myTarget.FirstItem(); !myTarget.FinishedItems(); tempItem = myTarget.NextItem() )
						{
							if( !ValidateObject( tempItem ))
								continue;

							iMulti.ReleaseItem( tempItem );
						}
					}
				}

				myTarget.Delete();
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 8939, pSock.language )); // You can only remove items or NPCs.
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 8939, pSock.language )); // You can only remove items or NPCs.
		}

		pSock.CustomTarget( 7, GetDictionaryEntry( 8938, pSock.language ), 1 ); // Which object do you wish to remove?
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: ADD ITEM <item-id from dfns>
function command_RADDITEM( pSock, execString )
{
	pSock.xText = execString;
	var tempMsg = GetDictionaryEntry( 8940, pSock.language ); // Select target location for item %s:
	pSock.CustomTarget( 8, tempMsg.replace( /%s/gi, execString ));
}
function onCallback8( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var pUser = pSock.currentChar;
		var TempItemID = pSock.xText;
		if( !( TempItemID == null ))
		{
			var targX = pSock.GetWord( 11 );
			var targY = pSock.GetWord( 13 );
			var targZ = pSock.GetSByte( 16 );
			var StrangeByte = pSock.GetWord(1);

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if ((StrangeByte == 0 && myTarget.isItem) || (pSock.clientMajorVer <= 7 && pSock.clientSubVer < 9))
			{
				targZ += GetTileHeight( pSock.GetWord( 17 ));
			}

			var tempItem = CreateDFNItem( pSock, pUser, TempItemID, 1, "ITEM", false );
			if( tempItem )
			{
				if( ValidateObject( myTarget ) && ValidateObject( myTarget.container ))
				{
					tempItem.container = myTarget.container;
					tempItem.PlaceInPack();
				}
				else if( ValidateObject( myTarget ) && myTarget.isChar && ValidateObject( myTarget.pack ))
				{
					tempItem.container = myTarget.pack;
					tempItem.PlaceInPack();
				}
				else
				{
					tempItem.x = targX;
					tempItem.y = targY;
					tempItem.z = targZ;
				}

				// Check if force movable off/on option has been set in 'add menu
				if( pUser.GetTag( "forceMovableOff" ))
				{
					tempItem.movable = 2;
				}
				else if( pUser.GetTag( "forceMovableOn" ))
				{
					tempItem.movable = 1;
				}

				// Check if force decay off/on option has been set in 'add menu
				if( pUser.GetTag( "forceDecayOff" ))
				{
					tempItem.decayable = 0;
				}
				else if( pUser.GetTag( "forceDecayOn" ))
				{
					tempItem.decayable = 1;
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 8941, pSock.language )); // That doesn't seem to be a valid item-id from the DFNs. No item added!
			}

			var tempMsg = GetDictionaryEntry( 8940, pSock.language ); // Select target location for item %s:
			pSock.CustomTarget( 8, tempMsg.replace( /%s/gi, TempItemID ));
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 8941, pSock.language )); // That doesn't seem to be a valid item-id from the DFNs. No item added!
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: TELE <select target location>
function command_RTELE( pSock, execString )
{
	pSock.CustomTarget( 9, GetDictionaryEntry( 8942, pSock.language )); // Select location to teleport to:
}
function onCallback9( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var pUser = pSock.currentChar;
		var targX = pSock.GetWord( 11 );
		var targY = pSock.GetWord( 13 );
		var targZ = pSock.GetSByte( 16 );
		var StrangeByte = pSock.GetWord(1);

		// If connected with a client lower than v7.0.9, manually add height of targeted tile
		if ((StrangeByte == 0 && myTarget.isItem) || (pSock.clientMajorVer <= 7 && pSock.clientSubVer < 9))
		{
			targZ += GetTileHeight( pSock.GetWord( 17 ));
		}

		pUser.Teleport( targX, targY, targZ, pUser.worldnumber, pUser.instanceID );
		pSock.CustomTarget( 9, GetDictionaryEntry( 8942, pSock.language )); // Select location to teleport to:
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: ADD NPC <npc-id from DFNs>
function command_RADDNPC( pSock, execString )
{
	pSock.xText = execString;
	var tempMsg = GetDictionaryEntry( 8943, pSock.language ); // Select target location for the [%s]:
	pSock.CustomTarget( 10, tempMsg.replace( /%s/gi, execString ));
}
function onCallback10( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var pUser = pSock.currentChar;
		var tempNPCID = pSock.xText;
		if( !( tempNPCID == null ))
		{
			var targX = pSock.GetWord( 11 );
			var targY = pSock.GetWord( 13 );
			var targZ = pSock.GetSByte( 16 );
			var StrangeByte = pSock.GetWord(1);

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if ((StrangeByte == 0 && myTarget.isItem) || (pSock.clientMajorVer <= 7 && pSock.clientSubVer < 9))
			{
				targZ += GetTileHeight( pSock.GetWord( 17 ));
			}

			var newNPC = SpawnNPC( tempNPCID, targX, targY, targZ, pUser.worldnumber, pUser.instanceID );
			var tempMsg = GetDictionaryEntry( 8943, pSock.language ); // Select target location for the [%s]:
			pSock.CustomTarget( 10, tempMsg.replace( /%s/gi, tempNPCID ));
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 8944, pSock.language )); // That doesn't seem to be a valid NPC-id from the DFNs.
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: ADD SPAWNER <item-id from dfns>
function command_RADDSPAWNER( pSock, execString )
{
	pSock.xText = execString;
	var tempMsg = GetDictionaryEntry( 9060, pSock.language ); // Select target location for spawner %s:
	pSock.CustomTarget( 11, tempMsg.replace( /%s/gi, execString ));
}
function onCallback11( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var pUser = pSock.currentChar;
		var TempItemID = pSock.xText;
		if( !( TempItemID == null ))
		{
			var targX = pSock.GetWord( 11 );
			var targY = pSock.GetWord( 13 );
			var targZ = pSock.GetSByte( 16 );
			var StrangeByte = pSock.GetWord(1);

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if ((StrangeByte == 0 && myTarget.isItem) || (pSock.clientMajorVer <= 7 && pSock.clientSubVer < 9))
			{
				targZ += GetTileHeight( pSock.GetWord( 17 ));
			}

			var tempItem = CreateDFNItem( pSock, pUser, TempItemID, 1, "SPAWNER", false );
			if( tempItem )
			{
				if( ValidateObject( myTarget ) && ValidateObject( myTarget.container ))
				{
					tempItem.container = myTarget.container;
					tempItem.PlaceInPack();
				}
				else
				{
					tempItem.x = targX;
					tempItem.y = targY;
					tempItem.z = targZ;
				}

				// Check if force movable off/on option has been set in 'add menu
				if( pUser.GetTag( "forceMovableOff" ))
				{
					tempItem.movable = 2;
				}
				else if( pUser.GetTag( "forceMovableOn" ))
				{
					tempItem.movable = 1;
				}

				// Check if force decay off/on option has been set in 'add menu
				if( pUser.GetTag( "forceDecayOff" ))
				{
					tempItem.decayable = 0;
				}
				else if( pUser.GetTag( "forceDecayOn" ))
				{
					tempItem.decayable = 1;
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 8941, pSock.language )); // That doesn't seem to be a valid item-id from the DFNs. No item added!
			}

			var tempMsg = GetDictionaryEntry( 9060, pSock.language ); // Select target location for spawner %s:
			pSock.CustomTarget( 11, tempMsg.replace( /%s/gi, TempItemID ));
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 8941, pSock.language )); // That doesn't seem to be a valid item-id from the DFNs. No item added!
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

//Repeated Command: RINCID <value>
function command_RINCID( pSock, execString )
{
	if( !isNaN( execString ))
	{
		pSock.xText = execString;
		var tempMsg = GetDictionaryEntry( 9101, pSock.language ); // Select target to modify ID by %i:
		pSock.CustomTarget( 12, tempMsg.replace( /%i/gi, execString ));
	}
	else
	{
		pSock.SysMessage( ReqNum );
	}
}
function onCallback12( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response ( unlike
	// regular UO client ), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var rincIDVal = pSock.xText;
		var rincIDVal = Number( rincIDVal );
		if( !pSock.GetWord( 1 ))
		{
			myTarget.id += rincIDVal;
		}

		var tempMsg = GetDictionaryEntry( 9101, pSock.language ); // Select target to modify ID by %i:
		pSock.CustomTarget( 12, tempMsg.replace( /%i/gi, rincIDVal ));
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

// Repeated Command: RMOVABLE <value>
function command_RMOVABLE( pSock, execString )
{
	if( !isNaN( execString ))
	{
		pSock.xText = execString;

		var tempMsg = GetDictionaryEntry( 8945, pSock.language ); // Select target for movable status %i:
		pSock.CustomTarget( 13, tempMsg.replace( /%i/gi, execString ));
	}
	else
	{
		pSock.SysMessage( ReqNum );
	}
}
function onCallback13( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var movableVal = pSock.xText;
		var movableVal = Number( movableVal );
		if( !pSock.GetWord( 1 ))
		{
			myTarget.movable = movableVal;
		}

		var tempMsg = GetDictionaryEntry( 8945, pSock.language ); // Select target for movable status %i:
		pSock.CustomTarget( 13, tempMsg.replace( /%i/gi, movableVal ));
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

// Repeated Command: RNODECAY <value>
function command_RNODECAY( pSock, execString )
{
	pSock.CustomTarget( 14, GetDictionaryEntry( 8946, pSock.language )); // Select target to set as non-decayable:
}
function onCallback14( pSock, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		if( !pSock.GetWord( 1 ))
		{
			myTarget.decayable = false;
		}

		pSock.CustomTarget( 14, GetDictionaryEntry( 8946, pSock.language )); // Select target to set as non-decayable:
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 8932, pSock.language )); // Repeating command ended.
	}
}

function _restorecontext_() {}