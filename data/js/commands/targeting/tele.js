// GM commands for teleporting user and/or other objects around

function CommandRegistration()
{
	RegisterCommand( "telestuff", 1, true );
	RegisterCommand( "move", 1, true );
	RegisterCommand( "tele", 1, true );
	RegisterCommand( "teleport", 1, true );
}

// Alias of TELESTUFF
function command_MOVE( socket, cmdString )
{
	command_TELESTUFF( socket, cmdString );
}

function command_TELESTUFF( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 250, socket.language ); // Select player/object to teleport.
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	var targMsg;
	if( !socket.GetWord( 1 ))
	{
		if( ourObj.isChar )
		{
			targMsg = GetDictionaryEntry( 1045, socket.language ); // Select location to put this character.
		}
		else
		{
			targMsg = GetDictionaryEntry( 1046, socket.language ); // Select location to put this item.
		}

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
			{
				dictMsg = GetDictionaryEntry( 1047, socket.language ); // Moving character...
			}
			else
			{
				dictMsg = GetDictionaryEntry( 1048, socket.language ); // Moving item...
			}

			var x = socket.GetWord( 11 );
			var y = socket.GetWord( 13 );
			var z = socket.GetSByte( 16 );
			var StrangeByte = socket.GetWord(1);

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if ((StrangeByte == 0 && ourObj.isItem) || (socket.clientMajorVer <= 7 && socket.clientSubVer < 9))
			{
				z += GetTileHeight( socket.GetWord( 17 ));
			}

			doTele( toTele, x, y, z );

			socket.SysMessage( dictMsg );
		}
	}
	socket.tempObj = null;
}

function command_TELE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 185, socket.language ); // Select teleport target.
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
				targZ = socket.GetSByte( 16 );
				var StrangeByte = socket.GetWord(1);

				// If connected with a client lower than v7.0.9, manually add height of targeted tile
				if ((StrangeByte == 0 && ourObj.isItem) || (socket.clientMajorVer <= 7 && socket.clientSubVer < 9))
				{
					targZ += GetTileHeight( socket.GetWord( 17 ));
				}
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

function _restorecontext_() {}
