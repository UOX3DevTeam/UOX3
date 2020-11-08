function CommandRegistration()
{
	RegisterCommand( "hide", 2, true );
	RegisterCommand( "unhide", 2, true );
}

function command_HIDE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 244, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
	{
		if( ourObj.visible )
			socket.SysMessage( GetDictionaryEntry( 833, socket.language ) );
		else
			ourObj.visible = 3;
	}
}

function command_UNHIDE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 245, socket.language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
	{
		if( ourObj.visible == 0 )
			socket.SysMessage( "That object is already visible!" );
		else
		{
			if( ourObj.isChar )
			{
				var mChar = socket.currentChar;
				if( ourObj.commandlevel > mChar.commandlevel )
				{
					socket.SysMessage( "You cannot unhide that person!" );
					return;
				}
			}
			ourObj.visible = 0;
		}
	}
}

