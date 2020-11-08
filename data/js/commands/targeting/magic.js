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
		if( ourObj.morex <= 200 && ourObj.morey <= 200 )
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
	}
}
