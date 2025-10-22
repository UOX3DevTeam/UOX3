/// <reference path="../../definitions.d.ts" />
// @ts-check
// These commands let GMs add items in a variety of ways

function CommandRegistration()
{
	RegisterCommand( "add", 8, true );
	RegisterCommand( "itemmenu", 8, true );
	RegisterCommand( "addx", 8, true );
	RegisterCommand( "addxitem", 8, true );
	RegisterCommand( "addxspawner", 8, true );
}

// Collapse spaces/underscores/dashes and lowercase, e.g. "Brown Horse" -> "brownhorse"
function normalizeSectionID( str )
{
	return ( str || "" ).replace( /[\s_\-]+/g, "" ).toLowerCase();
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_ADD( socket, cmdString )
{
	if( cmdString )
	{
		var stringID = "";
		var splitString = cmdString.split( " " );
		socket.tempInt2 = 0;
		socket.addAmount = 1; // default amount
		switch( splitString[0].toUpperCase() )
		{
			case "NPCLIST":
				socket.tempInt2 = 1;
				// fallthrough
			case "NPC":
				if( splitString[1] )
				{
					// Join tokens after "npc"
					var tokens = splitString.slice( 1 );
					// Trailing number => amount
					var maybeAmount = parseInt( tokens[tokens.length - 1] );
					if( !isNaN( maybeAmount ) && maybeAmount > 0 )
					{
						socket.addAmount = maybeAmount;
						tokens.pop();
					}
					else
						socket.addAmount = 1;

					// Keep underscores/dashes/spaces; just lowercase
					var rawNpc = tokens.join(" ").toLowerCase();

					socket.xText = rawNpc;
					socket.CustomTarget( 0, GetDictionaryEntry( 8068, socket.language ) + " " + rawNpc ); // Select location for NPC:
				}
				break;

			case "ITEM":
				if( splitString[1] )
				{
					var tokens = splitString.slice(1);
					var maybeAmount = parseInt( tokens[tokens.length - 1] );
					if( !isNaN( maybeAmount ) && maybeAmount > 0 )
					{
						socket.addAmount = maybeAmount;
						tokens.pop();
					}

					// Keep underscores/dashes/spaces; just lowercase
					var rawItem = tokens.join(" ").toLowerCase();

					socket.xText = rawItem;
					socket.CustomTarget( 2, GetDictionaryEntry( 8069, socket.language ) + " " + rawItem ); // Select location for scripted item:
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

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	var mChar = socket.currentChar;
	if( !mChar ) return;

	var x = socket.GetWord( 11 );
	var y = socket.GetWord( 13 );
	var z = socket.GetSByte( 16 );
	var StrangeByte = socket.GetWord( 1 );

	// Pre-7.0.9 client: add tile height if needed
	if(( StrangeByte == 0 && ourObj.isItem ) || ( socket.clientMajorVer <= 7 && socket.clientSubVer < 9 ))
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

	// Amount defaults to 1. If NPCLIST is used, we ignore amount to avoid repeated pickers
	var amount = socket.addAmount;
	if( amount == null || amount < 1 ) amount = 1;
	if( useNpcList ) amount = 1;

	// Try to spawn one; if not found, retry with normalized fallback
	function trySpawnOnce(section)
	{
		var c = SpawnNPC( section, x, y, z, mChar.worldnumber, mChar.instanceID, useNpcList );
		if( ValidateObject( c ) && c.isChar )
		{
			c.InitWanderArea();
			return c;
		}
		return null;
	}

	// First spawn (with fallback to normalized form of what we already normalized—harmless)
	var first = trySpawnOnce( npcSection );
	if( !first )
	{
		var altSection = normalizeSectionID( npcSection );
		if( altSection && altSection !== npcSection )
		{
			first = trySpawnOnce( altSection );
			if( first ) npcSection = altSection;
		}
	}

	if( !first )
	{
		mChar.SysMessage( GetDictionaryEntry( 8072, socket.language ) + " " + npcSection ); // NPC-section not found in DFNs:
		return;
	}

	// Spawn remaining (amount-1) at the same ground location
	for( var i = 1; i < amount; i++ )
	{
		var extra = trySpawnOnce( npcSection );
		if( !extra )
		{
			// If one of the later spawns fails, just stop quietly
			break;
		}
	}
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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
			if(( StrangeByte == 0 && ourObj.isItem ) || ( socket.clientMajorVer <= 7 && socket.clientSubVer < 9 ))
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

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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

		// Get amount to add (default 1)
		var itemAmount = socket.addAmount;
		if( itemAmount == null || itemAmount < 1 )
			itemAmount = 1;

		var StrangeByte = socket.GetWord( 1 );

		// Helper to spawn N items into a character's pack
		function spawnToPack(targetChar, section, count)
		{
			var made = 0;
			// Try once with 'count' to allow stacking DFNs to do it in one go
			var first = CreateDFNItem( socket, targetChar, section, count, "ITEM", true );
			if( ValidateObject(first) )
			{
				made += (first.amount && first.amount > 0) ? first.amount : 1;

				// If it didn't stack up to requested amount, add the rest one by one
				while( made < count )
				{
					var extra = CreateDFNItem( socket, targetChar, section, 1, "ITEM", true );
					if( ValidateObject(extra) )
					{
						made += (extra.amount && extra.amount > 0) ? extra.amount : 1;
					}
					else
					{
						break;
					}
				}
				return true;
			}
			return false;
		}

		// Helper to spawn N items into a container item
		function spawnToContainer(containerItem, section, count)
		{
			var made = 0;

			// First try a single spawn with 'count' (in case DFN stacks)
			var first = CreateDFNItem( socket, mChar, section, count, "ITEM", false );
			if( ValidateObject(first) )
			{
				first.container = containerItem;
				first.PlaceInPack();

				made += (first.amount && first.amount > 0) ? first.amount : 1;

				while( made < count )
				{
					var extra = CreateDFNItem( socket, mChar, section, 1, "ITEM", false );
					if( ValidateObject(extra) )
					{
						extra.container = containerItem;
						extra.PlaceInPack();
						made += (extra.amount && extra.amount > 0) ? extra.amount : 1;
					}
					else
					{
						break;
					}
				}
				return true;
			}
			return false;
		}

		// Helper to spawn N items on the ground at x,y,z
		function spawnToWorld(x, y, z, section, count)
		{
			var made = 0;

			var first = CreateDFNItem( socket, mChar, section, count, "ITEM", false );
			if( ValidateObject(first) )
			{
				first.SetLocation( x, y, z );
				made += (first.amount && first.amount > 0) ? first.amount : 1;

				while( made < count )
				{
					var extra = CreateDFNItem( socket, mChar, section, 1, "ITEM", false );
					if( ValidateObject(extra) )
					{
						extra.SetLocation( x, y, z );
						made += (extra.amount && extra.amount > 0) ? extra.amount : 1;
					}
					else
					{
						break;
					}
				}
				return true;
			}
			return false;
		}

		// Target is a character? -> put in their backpack
		if( StrangeByte == 0 && ourObj.isChar )
		{
			var backpack = ourObj.FindItemLayer( 21 );
			if( backpack != null )
			{
				var ok = spawnToPack( ourObj, iSection, itemAmount );
				if( !ok )
				{
					// Fallback: try normalized section ID (handles "long sword" -> "longsword")
					var alt = normalizeSectionID( iSection );
					if( alt && alt != iSection )
					{
						ok = spawnToPack( ourObj, alt, itemAmount );
						if( ok ) iSection = alt;
					}
				}
				if( !ok )
					mChar.SysMessage( GetDictionaryEntry( 8074, socket.language ) + " " + iSection ); // Item-section not found in DFNs:
			}
			else
			{
				mChar.SysMessage( GetDictionaryEntry( 8073, socket.language )); // That character has no backpack, no item added
			}
			return;
		}

		// Target is a container item? -> drop into that container
		if( StrangeByte == 0 && ourObj.isItem && ourObj.type == 1 )
		{
			var ok2 = spawnToContainer( ourObj, iSection, itemAmount );
			if( !ok2 )
			{
				var alt2 = normalizeSectionID( iSection );
				if( alt2 && alt2 != iSection )
				{
					ok2 = spawnToContainer( ourObj, alt2, itemAmount );
					if( ok2 ) iSection = alt2;
				}
			}
			if( !ok2 )
				mChar.SysMessage( GetDictionaryEntry( 8074, socket.language ) + " " + iSection );
			return;
		}

		// Otherwise, world location
		var x = socket.GetWord( 11 );
		var y = socket.GetWord( 13 );
		var z = socket.GetSByte( 16 );

		// Pre-7.0.9 clients need tile height added
		if(( StrangeByte == 0 && ourObj.isItem ) || ( socket.clientMajorVer <= 7 && socket.clientSubVer < 9 ))
			z += GetTileHeight( socket.GetWord( 17 ));

		var ok3 = spawnToWorld( x, y, z, iSection, itemAmount );
		if( !ok3 )
		{
			var alt3 = normalizeSectionID( iSection );
			if( alt3 && alt3 != iSection )
			{
				ok3 = spawnToWorld( x, y, z, alt3, itemAmount );
				if( ok3 ) iSection = alt3;
			}
		}
		if( !ok3 )
			mChar.SysMessage( GetDictionaryEntry( 8074, socket.language ) + " " + iSection );
	}
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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
		if(( StrangeByte == 0 && ourObj.isItem ) || ( socket.clientMajorVer <= 7 && socket.clientSubVer < 9 ))
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

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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
		if(( StrangeByte == 0 && ourObj.isItem ) || ( socket.clientMajorVer <= 7 && socket.clientSubVer < 9 ))
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

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_ITEMMENU( socket, cmdString )
{
	if( cmdString )
	{
		socket.SendAddMenu( parseInt( cmdString ));
	}
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
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

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_ADDXITEM( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( cmdString && mChar )
	{
		AddXItemSpawner( socket, cmdString, "ITEM" );
	}
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
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
