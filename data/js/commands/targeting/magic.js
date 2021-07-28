function CommandRegistration()
{
	RegisterCommand( "gate", 1, true );
	RegisterCommand( "recall", 1, true );
	RegisterCommand( "mark", 1, true );
}

function command_GATE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 226, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	var mChar 	= socket.currentChar;
	if( !socket.GetWord( 1 ) && ourObj.isItem && mChar )
	{
		if( ourObj.morex <= 200 && ourObj.morey <= 200 )
		{
			var txtMessage = GetDictionaryEntry( 679, socket.language );
			socket.SysMessage( txtMessage );
		}
		else
			mChar.Gate( ourObj );
	}
}

function command_RECALL( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 224, socket.language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	var mChar 	= socket.currentChar;
	if( !socket.GetWord( 1 ) && ourObj.isItem && mChar )
	{
		var txtMessage;
		if( ourObj.type == 7 )
		{
			// We targeted a key. For a house, or a boat?
			var shipSerial = ourObj.more;
			var serialPart1 = ( shipSerial >> 24 );
			var serialPart2 = ( shipSerial >> 16 );
			var serialPart3 = ( shipSerial >> 8 );
			var serialPart4 = ( shipSerial % 256 );
			var shipMulti = CalcMultiFromSer( serialPart1, serialPart2, serialPart3, serialPart4 );

			if( ValidateObject( shipMulti ) && shipMulti.isBoat() )
			{
				if( shipMulti.worldnumber == socket.currentChar.worldnumber && shipMulti.instanceID == socket.currentChar.instanceID )
				{
					socket.currentChar.SetLocation( shipMulti.x + 1, shipMulti.y, shipMulti.z + 3 );
				}
				else
					socket.SysMessage( GetDictionaryEntry( 8093, socket.language )); // You are unable to recall to your ship - it might be in another world!
			}
			else
				socket.SysMessage( GetDictionaryEntry( 8094, socket.language )); // You can only recall off of valid ship keys.
		}
		else if( ourObj.morex <= 200 && ourObj.morey <= 200 )
		{
			txtMessage = GetDictionaryEntry( 431, socket.language );
			socket.SysMessage( txtMessage );
		}
		else
		{
			txtMessage = GetDictionaryEntry( 682, socket.language );
			socket.SysMessage( txtMessage );
			mChar.Recall( ourObj );
		}
	}
}

function command_MARK( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 225 );
	socket.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{
	var mChar 	= socket.currentChar;
	if( !socket.GetWord( 1 ) && ourObj.isItem && mChar )
	{
		var txtMessage = GetDictionaryEntry( 686 );
		socket.SysMessage( txtMessage );
		mChar.Mark( ourObj );

		var worldNum = mChar.worldnumber;
		switch( worldNum )
		{
			case 1: // Trammel
				ourObj.colour = ( mChar.multi != null ? 1151 : 50 );
				break;
			case 2: // Ilshenar
				ourObj.colour = ( mChar.multi != null ? 1375 : 1102 );
				break;
			case 3: // Malas
				ourObj.colour = ( mChar.multi != null ? 1375 : 1102 );
				break;
			case 4: // Tokuno
				ourObj.colour = ( mChar.multi != null ? 7956 : 1154 );
				break;
			case 5: // TerMur
				ourObj.colour = 1162;
				break;
			default: // Felucca
				ourObj.colour = ( mChar.multi != null ? 1645 : 0 );
				break;
		}

	}
}
