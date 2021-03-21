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
		socket.CustomTarget( 0, "Select container to remove item (" + cmdString + ") from: " );
	}
	else
	{
		var targMsg = GetDictionaryEntry( 188, socket.language );
		socket.CustomTarget( 0, targMsg );
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
					socket.SysMessage( GetDictionaryEntry( 1015 ) );
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
						socket.SysMessage( "Unable to find any items of that ID to remove from the container!" );
					}
				}
				else
				{
					// TODO - If targeted object is a house sign on a multi, or a tillerman on a boat,
					// cause the multi to be demolished and run through the process of releasing
					// any items locked down inside?

					socket.SysMessage( GetDictionaryEntry( 1013 ) );
					ourObj.Delete();
				}
			}
		}
		else
			socket.SysMessage( "Invalid target" );
	}
}

function command_DELETECHAR( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 1618, socket.language );
	socket.CustomTarget( 1, targMsg );
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
				targSock.SysMessage( GetDictionaryEntry( 1659 ) );
				targSock.Disconnect();
			}
		}
		ourObj.Delete();
	}
}
