function CommandRegistration()
{
	RegisterCommand( "telestuff", 1, true );
	RegisterCommand( "tele", 1, true );
	RegisterCommand( "teleport", 1, true );
}

function command_TELESTUFF( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 250, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	var targMsg;
	if( !socket.GetWord( 1 ) )
	{
		if( ourObj.isChar )
			targMsg = GetDictionaryEntry( 1045, socket.language );
		else
			targMsg = GetDictionaryEntry( 1046, socket.language );

		socket.tempObj = ourObj;
		socket.CustomTarget( 1, targMsg );
	}
}

function onCallback1( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var dictMsg;
		var toTele = socket.tempObj;
		if( toTele )
		{
			if( toTele.isChar )
				dictMsg = GetDictionaryEntry( 1047, socket.language );
			else
				dictMsg = GetDictionaryEntry( 1048, socket.language );

			var x = socket.GetWord( 11 );
			var y = socket.GetWord( 13 );
			var z = socket.GetSByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
			doTele( toTele, x, y, z );

			socket.SysMessage( dictMsg );
		}
	}
	socket.tempObj = null;
}

function command_TELE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 185, socket.language );
	socket.CustomTarget( 2, targMsg );
}

// Alias of TELE
function command_TELEPORT( socket, cmdString )
{
	command_TELE( socket, cmdString );
}

function onCallback2( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var mChar = socket.currentChar;
		if( mChar )
		{
			var targX;
			var targY;
			var targZ;
			if( !socket.GetWord( 1 ) && ourObj )
			{
				targX = ourObj.x;
				targY = ourObj.y;
				targZ = ourObj.z;
			}
			else
			{
				targX = socket.GetWord( 11 );
				targY = socket.GetWord( 13 );
				targZ = socket.GetSByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
			}
			doTele( mChar, targX, targY, targZ );
		}
	}
}

function doTele( toTele, targX, targY, targZ )
{
	toTele.SoundEffect( 0x01FE, true );
	toTele.SetLocation( targX, targY, targZ );
	toTele.StaticEffect( 0x372A, 0x09, 0x06 );
}
