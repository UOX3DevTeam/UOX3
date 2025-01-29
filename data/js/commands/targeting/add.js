// These commands let GMs add items in a variety of ways

function CommandRegistration()
{
	RegisterCommand( "add", 2, true );
	RegisterCommand( "itemmenu", 2, true );
	RegisterCommand( "addx", 2, true );
	RegisterCommand( "addxitem", 2, true );
	RegisterCommand( "addxspawner", 2, true );
}

function command_ADD( socket, cmdString )
{
	if( cmdString )
	{
		var stringID = "";
		var splitString = cmdString.split( " " );
		socket.tempInt2 = 0;
		switch( splitString[0].toUpperCase() )
		{
			case "NPCLIST":
				socket.tempInt2 = 1;
				// fallthrough
			case "NPC":
				if( splitString[1] )
				{
					socket.xText = splitString[1];
					socket.CustomTarget( 0, GetDictionaryEntry( 8068, socket.language ) + " " + splitString[1] ); // Select location for NPC:
				}
				break;
			case "ITEM":
				if( splitString[1] )
				{
					// .add item itemID
					socket.xText = splitString[1];
					socket.CustomTarget( 2, GetDictionaryEntry( 8069, socket.language ) + " " + splitString[1] ); // Select location for scripted item:
				}
				break;
			case "SPAWNER":
				if( splitString[1] )
				{
					socket.xText = splitString[1];
					socket.CustomTarget( 4, GetDictionaryEntry( 8070, socket.language ) + " " + splitString[1] ); // Select location for Spawner:
				}
				break;
			case "TREE":
				if( splitString[1] && splitString[2] )
				{
					socket.tempint = parseInt( splitString[1] );
					socket.tempInt2 = parseInt( splitString[2] );
					stringID = splitString[1];
					socket.CustomTarget( 1, GetDictionaryEntry( 8071, socket.language ) + " " + stringID ); // Select location for base item:
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 9108, socket.language )); // IDs for both tree trunk AND leaves are required by this command. Syntax: cmd tree [trunkID] [leafID]
				}
				break;
			default:
				if( splitString[2] )
				{
					// .add static itemID rndValue
					stringID = splitString[1];
					socket.tempint = parseInt( stringID ) + RandomNumber( 0, parseInt( splitString[2] ));
				}
				else if( splitString[1] )
				{
					if( splitString[0].toUpperCase() == "STATIC" )
					{
						// .add static itemID
						stringID = splitString[1];
						socket.tempint = parseInt( stringID );
					}
					else
					{
						// .add itemID rndValue
						stringID = splitString[0];
						socket.tempint = parseInt( stringID ) + RandomNumber( 0, parseInt( splitString[1] ));
					}
				}
				else if( splitString[0] )
				{
					// .add itemID
					stringID = splitString[0];
					socket.tempint = parseInt( stringID );
				}

				if( stringID != "" )
				{
					socket.CustomTarget( 1, GetDictionaryEntry( 8071, socket.language ) + " " + stringID ); // Select location for base item:
				}
				break;
		}
	}
	else
	{
		socket.SendAddMenu( 1 );
	}
}

function onCallback0( socket, ourObj )
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

		var npcSection = socket.xText;
		socket.xText = null;

		var useNpcList = false;
		if( socket.tempInt2 )
		{
			useNpcList = true;
			socket.tempInt2 = null;
		}

		var newChar = SpawnNPC( npcSection, x, y, z, mChar.worldnumber, mChar.instanceID, useNpcList );
		if( ValidateObject( newChar ) && newChar.isChar )
		{
			newChar.InitWanderArea();
		}
		else
		{
			mChar.SysMessage( GetDictionaryEntry( 8072, socket.language ) + " " + npcSection ); // NPC-section not found in DFNs:
		}
	}
}

function onCallback1( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	var mChar = socket.currentChar;
	if( mChar )
	{
		var itemID 	= socket.tempint;
		var itemID2 = socket.tempInt2;
		socket.tempint = null;
		socket.tempInt2 = null;
		var StrangeByte = socket.GetWord( 1 );
		if( StrangeByte == 0 && ourObj.isChar  )
		{
			//If target is a character, add item to backpack
			var backpack = ourObj.FindItemLayer( 21 );
			if( backpack != null )
			{
				var newItem = CreateBlankItem( socket, ourObj, 1, "", itemID, 0, "ITEM", true );
			}
			else
			{
				mChar.SysMessage( GetDictionaryEntry( 8073, socket.language )); // That character has no backpack, no item added
			}
		}
		else if( StrangeByte == 0 && ourObj.isItem && ourObj.type == 1 )
		{
			// If target is an item, and a container, add item to the container
			var newItem = CreateBlankItem( socket, mChar, 1, "", itemID, 0, "ITEM", false );
			if( ValidateObject( newItem ))
			{
				newItem.container = ourObj;
				newItem.PlaceInPack();
			}
		}
		else
		{
			var x = socket.GetWord( 11 );
			var y = socket.GetWord( 13 );
			var z = socket.GetSByte( 16 );

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if ((StrangeByte == 0 && ourObj.isItem) || (socket.clientMajorVer <= 7 && socket.clientSubVer < 9))
			{
				z += GetTileHeight( socket.GetWord( 17 ));
			}

			var newItem = CreateBlankItem( socket, mChar, 1, "", itemID, 0, "ITEM", false );
			if( newItem )
			{
				newItem.SetLocation( x, y, z );
				if( itemID2 )
				{
					var newItem2 = CreateBlankItem( socket, mChar, 1, "", itemID2, 0, "ITEM", false );
					if( newItem2 )
					{
						newItem2.SetLocation( x, y, z );
					}
				}
			}
		}
		if( newItem )
		{
			if( newItem.id != itemID )
			{
				//If itemid of newly created item differs from specified id, delete item - it's a default one only
				mChar.SysMessage( GetDictionaryEntry( 8074, socket.language )); // Specified item-ID does not exist.
				mChar.SysMessage( "Hex: 0x" + itemID.toString( 16 ) + " Dec: " + itemID );
				newItem.Delete();
			}
		}
	}
}

function onCallback2( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	var mChar = socket.currentChar;
	if( mChar )
	{
		var iSection = socket.xText;
		socket.xText = null;
		var StrangeByte = socket.GetWord( 1 );
		if( StrangeByte == 0 && ourObj.isChar  )
		{
			//If target is a character, add item to backpack
			var backpack = ourObj.FindItemLayer( 21 );
			if( backpack != null )
			{
				var newItem = CreateDFNItem( socket, ourObj, iSection, 1, "ITEM", true );
			}
			else
			{
				mChar.SysMessage( GetDictionaryEntry( 8073, socket.language )); // That character has no backpack, no item added
			}
		}
		else if( StrangeByte == 0 && ourObj.isItem && ourObj.type == 1 )
		{
			// If target is an item, and a container, add item to the container
			var newItem = CreateDFNItem( socket, ourObj, iSection, 1, "ITEM", false );
			if( ValidateObject( newItem ))
			{
				newItem.container = ourObj;
				newItem.PlaceInPack();
			}
		}
		else
		{
			var x = socket.GetWord( 11 );
			var y = socket.GetWord( 13 );
			var z = socket.GetSByte( 16 );

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if ((StrangeByte == 0 && ourObj.isItem) || (socket.clientMajorVer <= 7 && socket.clientSubVer < 9))
			{
				z += GetTileHeight( socket.GetWord( 17 ));
			}

			var newItem = CreateDFNItem( socket, mChar, iSection, 1, "ITEM", false );
			if( newItem )
			{
				newItem.SetLocation( x, y, z );
			}
		}
		if( !newItem )
		{
			mChar.SysMessage( GetDictionaryEntry( 8074, socket.language ) + " " + iSection ); // Item-section not found in DFNs:
		}
	}
}

function onCallback3( socket, ourObj )
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

		var itemID 	= socket.tempint;
		socket.tempint = null;
		var newItem = CreateBlankItem( socket, mChar, 1, "#", itemID, 0, "SPAWNER", false );
		if( newItem )
		{
			newItem.SetLocation( x, y, z );
			newItem.decayable = false;
		}
		if( newItem.id != itemID )
		{
			//If itemid of newly created item differs from specified id, delete item - it's a default one only
			mChar.SysMessage( GetDictionaryEntry( 8074, socket.language )); // Specified item-ID does not exist.
			mChar.SysMessage( "Hex: 0x" + itemID.toString( 16 ) + " Dec: " + itemID );
			newItem.Delete();
		}
	}
}

function onCallback4( socket, ourObj )
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

		var iSection = socket.xText;
		socket.xText = null;
		var newItem = CreateDFNItem( socket, mChar, iSection, 1, "SPAWNER", false );
		if( newItem )
		{
			newItem.SetLocation( x, y, z );
			newItem.decayable = false;
		}
		else
		{
			mChar.SysMessage( GetDictionaryEntry( 8075, socket.language ) + " " + iSection ); // Item-section not found in DFNs:
		}
	}
}

function command_ITEMMENU( socket, cmdString )
{
	if( cmdString )
	{
		socket.SendAddMenu( parseInt( cmdString ));
	}
}

function command_ADDX( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( cmdString && mChar )
	{
		var targID;
		var targZ = mChar.z;
		var splitString = cmdString.split( " " );
		if( splitString[1] )
		{
			targZ = parseInt( splitString[1] );
		}
		if( splitString[0] )
		{
			targID = parseInt( splitString[0] );
		}
		var newItem = CreateBlankItem( socket, mChar, 1, "#", targID, 0, "ITEM", false );
		if( newItem )
		{
			newItem.SetLocation( mChar.x, mChar.y, targZ );
		}

		if( newItem.id != targID )
		{
			// If itemid of newly created item differs from specified id, delete item - it's a default one only
			mChar.SysMessage( GetDictionaryEntry( 8074, socket.language )); // Specified item-ID does not exist.
			mChar.SysMessage( "Hex: 0x" + targID.toString( 16 ) + " Dec: " + targID );
			newItem.Delete();
		}
	}
}

function command_ADDXITEM( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( cmdString && mChar )
	{
		AddXItemSpawner( socket, cmdString, "ITEM" );
	}
}

function command_ADDXSPAWNER( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( cmdString && mChar )
	{
		AddXItemSpawner( socket, cmdString, "SPAWNER" );
	}
}

function AddXItemSpawner( socket, cmdString, itemType )
{
	var mChar = socket.currentChar;
	if( cmdString && mChar )
	{
		var itemID;
		var compareID = false;
		var targZ = mChar.z;
		var splitString = cmdString.split( " " );
		if( splitString[1] )
		{
			targZ = parseInt( splitString[1] );
		}
		if( splitString[0] )
		{
			itemID = splitString[0];
			if( !isNaN( parseInt( itemID )))
			{
				itemID = parseInt( splitString[0] ).toString( 16 );
				while( itemID.length < 4 )
				{
					itemID = "0" + itemID;
				}
				itemID = "0x" + itemID;
				compareID = true;
			}
		}

		var newItem = CreateDFNItem( socket, mChar, itemID, 1, itemType, false );
		if( !ValidateObject( newItem ))
		{
			socket.SysMessage( GetDictionaryEntry( 9109, socket.language )); // Item definition not found, attempting to add blank item using decimal ID...
			newItem = CreateBlankItem( socket, mChar, 1, "#", parseInt( itemID ), 0x0, itemType, false );
			compareID = true;
		}

		if( newItem )
		{
			if( compareID && newItem.id != parseInt( itemID ))
			{
				//If itemid of newly created item differs from specified id, delete item - it's a default one only
				mChar.SysMessage( GetDictionaryEntry( 8074, socket.language )); // Specified item-ID does not exist.
				mChar.SysMessage( "Hex: 0x" + itemID.toString( 16 ) + " Dec: " + itemID );
				newItem.Delete();
			}
			else
			{
				newItem.SetLocation( mChar.x, mChar.y, targZ );
				newItem.movable = 2;
				newItem.decayable = false;

				if( itemType != "SPAWNER" && ( newItem.type == 61 || newItem.type == 63 || newItem.type == 64 || newItem.type == 65 ))
				{
					// It's a spawner, but was added as an item!
					newItem.TextMessage( GetDictionaryEntry( 9110, socket.language )); // [ERROR] Spawner Object added as ITEM, but needs to be SPAWNER!
					socket.SysMessage( GetDictionaryEntry( 9110, socket.language )); // [ERROR] Spawner Object added as ITEM, but needs to be SPAWNER!
				}
			}
		}
	}
}

function _restorecontext_() {}