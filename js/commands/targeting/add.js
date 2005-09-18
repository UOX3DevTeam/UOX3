function CommandRegistration()
{
	RegisterCommand( "add", 2, true );
	RegisterCommand( "itemmenu", 2, true );
	RegisterCommand( "addx", 2, true );
}

function command_ADD( socket, cmdString )
{
	if( cmdString )
	{
		var splitString = cmdString.split( " " );
		switch( splitString[0].toUpperCase() )
		{
		case "NPC":
			if( splitString[1] )
			{
				socket.xText = splitString[1];
				socket.CustomTarget( 0, "Select location for NPC: " + splitString[1] );
			}
			break;
		case "ITEM":
			if( splitString[2] )
			{
				var a1 = StringToNum( splitString[1] );
				var a2 = StringToNum( splitString[2] );
				socket.tempint = (a1<<8) + a2;
				socket.CustomTarget( 2, "Select location for Item: 0x" + NumToHexString( socket.tempint ) );
			}
			else if( splitString[1] )
			{
				socket.xText = splitString[1];
				socket.CustomTarget( 2, "Select location for Item: " + splitString[1] );
			}
			break;
		case "SPAWNER":
			if( splitString[2] )
			{
				var a1 = StringToNum( splitString[1] );
				var a2 = StringToNum( splitString[2] );
				socket.tempint = (a1<<8) + a2;
				socket.xText = NumToHexString((a1<<8) + a2);
				socket.CustomTarget( 3, "Select location for Spawner: 0x" + NumToHexString( socket.xText ) );
			}
			else if( splitString[1] )
			{
				socket.xText = splitString[1];
				socket.CustomTarget( 4, "Select location for Spawner: " + splitString[1] );
			}
			break;
		default:
			if( splitString[0] )
			{
				socket.tempint = StringToNum( splitString[0] );
				socket.CustomTarget( 1, "Select location for Item: " + splitString[1] );
			}
			break;
		}
	}
	else
		socket.SendAddMenu( 1 );
}

function onCallback0( socket, ourObj )
{
	var mChar = socket.currentChar;
	if( mChar )
	{
		var x 		= socket.GetWord( 11 );
		var y 		= socket.GetWord( 13 );
		var z 		= socket.GetByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
		var npcSection 	= socket.xText;

		var newChar 	= SpawnNPC( npcSection, x, y, z, mChar.worldnumber );
		if( newChar && newChar.isChar )
			newChar.InitWanderArea();
	}
}

function onCallback1( socket, ourObj )
{
	var mChar = socket.currentChar;
	if( mChar )
	{
		var itemID 	= socket.tempint;
		var StrangeByte = socket.GetWord( 1 );
		if( StrangeByte == 0 && ourObj.isChar  )
		{ //If target is a character, add item to backpack
			var backpack = ourObj.FindItemLayer(21);
			if( backpack != null )
				var newItem = CreateBlankItem( socket, mChar, 1, "#", itemID, 0, "ITEM", true );
			else
				mChar.SysMessage( "That character has no backpack, no item added" );
			return;
		}
		var x 		= socket.GetWord( 11 );
		var y 		= socket.GetWord( 13 );
		var z 		= socket.GetByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
		var newItem 	= CreateBlankItem( socket, mChar, 1, "#", itemID, 0, "ITEM", false );
		if( newItem )
			newItem.SetLocation( x, y, z );
	}
}

function onCallback2( socket, ourObj )
{
	var mChar = socket.currentChar;
	if( mChar )
	{
		var iSection 	= socket.xText;
		var StrangeByte = socket.GetWord( 1 );
		if( StrangeByte == 0 && ourObj.isChar  )
		{ //If target is a character, add item to backpack
			var backpack = ourObj.FindItemLayer(21);
			if( backpack != null )
				var newItem = CreateDFNItem( socket, mChar, iSection, 1, "ITEM", true );
			else
				mChar.SysMessage( "That character has no backpack, no item added" );
			return;
		}
		var x 		= socket.GetWord( 11 );
		var y 		= socket.GetWord( 13 );
		var z 		= socket.GetByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
		var newItem 	= CreateDFNItem( socket, mChar, iSection, 1, "ITEM", false );
		if( newItem )
			newItem.SetLocation( x, y, z );
	}
}

function onCallback3( socket, ourObj )
{
	var mChar = socket.currentChar;
	if( mChar )
	{
		var x 		= socket.GetWord( 11 );
		var y 		= socket.GetWord( 13 );
		var z 		= socket.GetByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
		var itemID 	= socket.tempint;
		var newItem 	= CreateBlankItem( socket, mChar, 1, "#", itemID, 0, "SPAWNER", false );
		if( newItem )
			newItem.SetLocation( x, y, z );
	}
}

function onCallback4( socket, ourObj )
{
	var mChar = socket.currentChar;
	if( mChar )
	{
		var x 		= socket.GetWord( 11 );
		var y 		= socket.GetWord( 13 );
		var z 		= socket.GetByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
		var iSection 	= socket.xText;
		var newItem 	= CreateDFNItem( socket, mChar, iSection, 1, "SPAWNER", false );
		if( newItem )
			newItem.SetLocation( x, y, z );
	}
}

function command_ITEMMENU( socket, cmdString )
{
	if( cmdString )
		socket.SendAddMenu( StringToNum( cmdString ) );
}

function command_ADDX( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( cmdString && mChar )
	{
		var targID;
		var targZ = mChar.z;
		var splitString = cmdString.split( " " );
		if( splitString[2] )
			targZ = StringToNum( splitString[2] );
		if( splitString[1] )
		{
			var a1 = StringToNum( splitString[0] );
			var a2 = StringToNum( splitString[1] );
			targID = (a1<<8) + a2;
		}
		else if( splitString[0] )
		{
			targID = StringToNum( splitString[0] );
		}
		var newItem = CreateBlankItem( socket, mChar, 1, "#", targID, 0, "ITEM", false );
		if( newItem )
			newItem.SetLocation( mChar.x, mChar.y, targZ );
	}
}

