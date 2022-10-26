function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( socket && iUsed && iUsed.isItem )
	{
		//Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); // That is locked down and you cannot use it
			return false;
		}
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 2712, socket.language )); // The bola must be in your pack to use it.
			return false;
		}
		else if( pUser.isonhorse )
		{
			pUser.SysMessage( GetDictionaryEntry( 2713, socket.language )); // You cannot use this while riding a mount.
			return false;
		}
		if( pUser.GetTag( "bola" ) == null || pUser.GetTag( "bola" ) == 0)
		{
			socket.tempObj = iUsed;
			pUser.SetTag( "bola", 1 );

			//the addition of the false-flag in the EmoteMessage below tells the server to only
			//send the message to this character
			pUser.EmoteMessage( GetDictionaryEntry( 2714, socket.language ), false ); // * You begin to swing the bola...*
			pUser.StartTimer( 3000, 0, true );
		}
		else if( pUser.GetTag( "bola" ) == 1 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2715, socket.language )); // You have to wait a few moments before you can use another bola!
		}
		else
		{
			return true;
		}
	}
	return false;
}

function onCallback0( socket, myTarget)
{
	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( !socket.GetWord( 1 ) && myTarget.isChar )
	{
		if( pUser.InRange( myTarget, 8 ))
		{
			if( myTarget.isonhorse )
			{
				myTarget.Dismount();
				pUser.DoAction( 0x9 );
				DoMovingEffect( pUser, myTarget, 0x26AC, 0x10, 0x00, false );
				if( myTarget.socket != null )
				{
					myTarget.SysMessage( GetDictionaryEntry( 2716, myTarget.socket.language )); // You have been knocked off your mount!
				}
				iUsed.Delete();
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 2717, socket.language )); // Your target isn't mounted.
			}
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 2718, socket.language )); // Your target is out of range.
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 2719, socket.language )); // You cannot throw a bola at that.
	}
}

function onTimer( pUser, timerID )
{
	var socket = pUser.socket;
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( timerID == 0 )
	{
		var tempMsg = GetDictionaryEntry( 2720, socket.language ); // %s begins to menacingly swing a bola...
		pUser.EmoteMessage( tempMsg.replace( /%s/gi, pUser.name ));
		pUser.StartTimer( 1000, 1, true );
	}

	if( timerID == 1 )
	{
		pUser.SetTag( "bola", null );
		socket.CustomTarget( 0 );
	}
}