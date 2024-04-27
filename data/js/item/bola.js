function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var iTime = GetCurrentClock();
	var NextUse = iUsed.GetTempTag( "bolaDelayed" );
	var Delay = 10000;

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
		else if( pUser.isflying ) 
		{
			pUser.SysMessage( GetDictionaryEntry( 2797, socket.language )); // You can't use this while flying!
			return false;
		}
		else if(( iTime - NextUse ) < Delay )
		{
			pUser.SysMessage( GetDictionaryEntry( 2715, socket.language )); // You have to wait a few moments before you can use another bola!
			return false;
		}
		else
		{
			socket.tempObj = iUsed;
			pUser.EmoteMessage( GetDictionaryEntry( 2714, socket.language )); // * You begin to swing the bola...*
			var tempMsg = GetDictionaryEntry( 2720, socket.language ); // %s begins to menacingly swing a bola...
			pUser.EmoteMessage( tempMsg.replace( /%s/gi, pUser.name ));
			iUsed.SetTempTag( "bolaDelayed", iTime.toString() );
			pUser.StartTimer( 3000, 0, true );
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
		// Don't allow throwing bolas at Young players, or Young players throwing bolas at other players
		if( GetServerSetting( "YoungPlayerSystem" ))
		{
			if(( !myTarget.npc && myTarget.account.isYoung )
				|| ( myTarget.npc && ValidateObject( myTarget.owner ) && !myTarget.owner.npc && myTarget.owner.account.isYoung ))
			{
				// Disallow throwing bola at Young players
				socket.SysMessage( GetDictionaryEntry( 2719, socket.language )); // You cannot throw a bola at that.
				return;
			}

			if(( !pUser.npc && pUser.account.isYoung && !myTarget.npc )
				|| ( myTarget.npc && ValidateObject( myTarget.owner ) && !myTarget.owner.npc ))
			{
				// Disallow throwing bolas at other players as a Young player
				socket.SysMessage( GetDictionaryEntry( 2719, socket.language ));
				return;
			}
		}

		if( pUser.InRange( myTarget, 8 ))
		{
			if( myTarget.isflying )
			{
				myTarget.isflying = false;
				pUser.DoAction( 0x9 );
				DoMovingEffect( pUser, myTarget, 0x26AC, 0x10, 0x00, false );
				if( myTarget.socket != null ) 
				{
					myTarget.socket.SysMessage( GetDictionaryEntry( 2798, myTarget.socket.language )); // You have been grounded
				}
				iUsed.Delete();
			}
			else if( myTarget.isonhorse )
			{
				myTarget.Dismount();
				pUser.DoAction( 0x9 );
				DoMovingEffect( pUser, myTarget, 0x26AC, 0x10, 0x00, false );
				if( myTarget.socket != null )
				{
					myTarget.socket.SysMessage( GetDictionaryEntry( 2716, myTarget.socket.language )); // You have been knocked off your mount!
				}
				iUsed.Delete();
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 2717, socket.language )); // Your target isn't mounted.
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 2718, socket.language )); // Your target is out of range.
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 2719, socket.language )); // You cannot throw a bola at that.
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
		socket.CustomTarget(0);
	}
}