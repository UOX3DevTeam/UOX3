function CommandRegistration()
{
	RegisterCommand( "remove", 2, true );
	RegisterCommand( "delete", 2, true );
	RegisterCommand( "deletechar", 3, true );
}

function command_REMOVE( socket, cmdString )
{
	socket.tempint = null;
	if( cmdString )
	{
		socket.tempint = parseInt( cmdString );
		var tempMsg = GetDictionaryEntry( 8095, socket.language ); // Select container to remove item (%s) from:
		socket.CustomTarget( 0, tempMsg.replace(/%s/gi, cmdString ), 1 );
	}
	else
	{
		var targMsg = GetDictionaryEntry( 188, socket.language, 1 ); // Select item to remove.
		// socket.CustomTarget( 0, targMsg, 1 );
		socket.CustomTarget( 0, targMsg, 3 );
	}
}

// Alias for REMOVE command
function command_DELETE( socket, cmdString )
{
	command_REMOVE( socket, cmdString );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
	{
		if( ourObj )
		{
			if( ourObj.isChar )
			{
				if( ourObj.npc )
				{
					socket.SysMessage( GetDictionaryEntry( 1015, socket.language )); // Removing character.
					ourObj.Delete();
				}
			}
			else if( ourObj.isItem )
			{
				var itemIdToRemove = socket.tempint;
				socket.tempint = null;
				if( itemIdToRemove )
				{
					if( ourObj.itemsinside > 0 )
					{
						for( mItem = ourObj.FirstItem(); !ourObj.FinishedItems(); mItem = ourObj.NextItem() )
						{
							if( ValidateObject( mItem ) && mItem.id == itemIdToRemove )
							{
								mItem.Delete();
							}
						}
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 8096, socket.language )); // Unable to find any items of that ID to remove from the container!
					}
				}
				else
				{
					// TODO - If targeted object is a house sign on a multi, or a tillerman on a boat,
					// cause the multi to be demolished and run through the process of releasing
					// any items locked down inside?

					socket.SysMessage( GetDictionaryEntry( 1013, socket.language )); // Removing item.
					ourObj.Delete();
				}
			}
		}
		else
			socket.SysMessage( GetDictionaryEntry( 2353, socket.language )); // Invalid target
	}
}

function command_DELETECHAR( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 1618, socket.language ); // Which player do you wish to delete?
	socket.CustomTarget( 1, targMsg, 1 );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar && ourObj != socket.currentChar )
	{
		if( !ourObj.npc && ourObj.online )
		{
			var targSock = ourObj.socket;
			if( targSock )
			{
				targSock.SysMessage( GetDictionaryEntry( 1659, targSock.language )); // You are being removed
				targSock.Disconnect();
			}
		}
		ourObj.Delete();
	}
}
