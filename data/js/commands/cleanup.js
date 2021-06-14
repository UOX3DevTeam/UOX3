function CommandRegistration()
{
	RegisterCommand( "cleanup", 2, true );
}

function command_CLEANUP( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 83, socket.language ) );
	var count = IterateOver( "ITEM" );
	socket.SysMessage( GetDictionaryEntry( 84, socket.language ) );
	socket.SysMessage( count.toString() + " corpses or gates have been deleted" );
}

var bloodIDs = [ 0x1645, 0x122A, 0x122B, 0x122C, 0x122D, 0x122E, 0x122F ];
function onIterate( toCheck )
{
	if( toCheck && toCheck.isItem )
	{
		// Clean up corpses
		if( toCheck.corpse || toCheck.type == 51 || toCheck.type == 52 )
		{
			toCheck.Delete();
			return true;
		}

		// Also clean up blood splatter
		if( bloodIDs.indexOf( toCheck.id ) > -1 )
		{
			toCheck.Delete();
			return true;
		}
	}
	return false;
}