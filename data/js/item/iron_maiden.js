// Iron Maiden
// Giver of very intimate, spiky hugs

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket == null || !ValidateObject( iUsed ) || !iUsed.isItem )
		return false;

	if( pUser.CanSee( iUsed ))
	{
		if( pUser.InRange( iUsed, 1 ))
		{
			if( iUsed.id == 0x124a ) // open iron maiden
			{
				pUser.Teleport( iUsed );
				pUser.frozen = true;
				iUsed.id++;
				iUsed.more = pUser.serial;
				pUser.Damage( Math.round( pUser.health / 3 ));
				iUsed.SoundEffect( 0x0231, true );
				iUsed.StartTimer( 1000, 0, true );
			}
			else if( iUsed.more != 0 )
			{
				var targChar = CalcCharFromSer( iUsed.more );
				if( ValidateObject( targChar ) && targChar.online && !targChar.dead )
				{
					socket.SysMessage( "That is already occupied by someone else..." );
				}
				else
				{
					iUsed.id = 0x124a;
				}
			}
			else
			{
				iUsed.id = 0x124a;
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1183, socket.language )); // That is too far away
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 1646, socket.language )); // You cannot see that
	}
	return false;
}

function onTimer( timerObj, timerID )
{
	if( timerID < 2 )
	{
		timerObj.id++;
		timerObj.SoundEffect( 0x0231, true );
		var pUser = CalcCharFromSer( timerObj.more );
		if( ValidateObject( pUser ) && pUser.online && !pUser.dead )
		{
			pUser.Damage( Math.round( pUser.health / 3 ));
		}
		timerObj.StartTimer( 1000, timerID + 1, true );
	}
	else if( timerID == 2 )
	{
		timerObj.id = 0x124a;
		var pUser = CalcCharFromSer( timerObj.more );
		if( ValidateObject( pUser ) && pUser.online && !pUser.dead )
		{
			pUser.frozen = false;
			pUser.Refresh();
		}
		timerObj.more = 0;
	}
}