// These commands are used to remove/delete objects

function CommandRegistration()
{
	RegisterCommand( "remove", 2, true );
	RegisterCommand( "removemulti", 3, true );
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
		socket.CustomTarget( 0, tempMsg.replace( /%s/gi, cmdString ) );
	}
	else
	{
		var targMsg = GetDictionaryEntry( 188, socket.language ); // Select item to remove.
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
	if( !socket.GetWord( 1 ))
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

					if( ourObj.type == 1 && ourObj.id == 0x0e43 && ValidateObject( ourObj.multi ) && ourObj.more == ourObj.multi.serial )
					{
						// Chest of a tent multi. Destroy the multi too!
						var objMulti = ourObj.multi;
						objMulti.Delete();
					}
					ourObj.Delete();
				}
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 2353, socket.language )); // Invalid target
		}
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

function command_REMOVEMULTI( socket, cmdString )
{
	socket.tempint = null;
	var targMsg = GetDictionaryEntry( 188, socket.language ); // Select item to remove.
	socket.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{
	if( !ValidateObject( ourObj ) && socket.GetWord( 1 ))
	{
		// Find multi at target location
		var targX = socket.GetWord( 11 );
		var targY = socket.GetWord( 13 );
		var targZ = socket.GetSByte( 16 );
		var iMulti = FindMulti( targX, targY, targZ, socket.currentChar.worldnumber, socket.currentChar.instanceID )
		if( iMulti && iMulti.IsMulti() )
		{
			// Iterate through characters in house, delete player vendors and eject all other characters from house
			for( var charInHouse = iMulti.FirstChar(); !iMulti.FinishedChars(); charInHouse = iMulti.NextChar() )
			{
				if( !ValidateObject( charInHouse ))
					continue;

				if( !ValidateObject( charInHouse.multi ))
					continue;

				if( charInHouse.aitype == 17 ) // player vendor AI
				{
					charInHouse.Delete();
				}
				else
				{
					// Eject character from house
					TriggerEvent( 15002, "EjectPlayerActual", iMulti, charInHouse );
				}
			}

			// Iterate through items in the house and remove those too
			var itemInHouse;
			for( itemInHouse = iMulti.FirstItem(); !iMulti.FinishedItems(); itemInHouse = iMulti.NextItem() )
			{
				if( !ValidateObject( itemInHouse ))
					continue;

				if( !ValidateObject( itemInHouse.multi ))
					continue;

				itemInHouse.Delete();
			}

			// Finally, delete the multi itself
			iMulti.Delete();
		}
	}
}