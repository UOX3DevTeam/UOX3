function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( pUser.skillsused.alchemy )
		{
			socket.SysMessage( GetDictionaryEntry( 1631, socket.language )); // You must wait till you have finished making your other potion
		}
		else
		{
			socket.tempObj = iUsed;
			socket.CustomTarget( 1, GetDictionaryEntry( 470, socket.language )); // What do you wish to grind with your mortar and pestle?
		}
	}
	return false;
}

function onCallback1( socket, ourObj )
{
	var bItem = socket.tempObj;
	var mChar = socket.currentChar;

	socket.tempObj = null;

	if( mChar && mChar.isChar && bItem && bItem.isItem )
	{
		if( !ourObj || !ourObj.isItem )
		{
			socket.SysMessage( GetDictionaryEntry( 6001, socket.language )); // You can't make anything useful from that.
			return;
		}

		var resID = ourObj.id;
		var ownerObj = GetPackOwner( ourObj, 0 );
		if( ownerObj && mChar.serial == ownerObj.serial )
		{
			switch( resID )
			{
				case 0x0F7B:	socket.MakeMenu( 90, 0 );	break;
				case 0x0F84:	socket.MakeMenu( 91, 0 );	break;
				case 0x0F8C:	socket.MakeMenu( 92, 0 );	break;
				case 0x0F85:	socket.MakeMenu( 93, 0 );	break;
				case 0x0F88:	socket.MakeMenu( 94, 0 );	break;
				case 0x0F7A:	socket.MakeMenu( 95, 0 );	break;
				case 0x0F86:	socket.MakeMenu( 96, 0 );	break;
				case 0x0F8D:	socket.MakeMenu( 97, 0 );	break;
				default:
					socket.SysMessage( GetDictionaryEntry( 6001, socket.language )); // You can't make anything useful from that.
					break;
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 6002, socket.language )); // That must be in your pack.
		}
	}
}

