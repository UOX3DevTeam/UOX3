// This command deletes _ALL_ items in the world that matches specified ID

function CommandRegistration()
{
	RegisterCommand( "delid", 2, true );
}

function command_DELID( socket, cmdString )
{
	if( cmdString )
	{
		var count = 0;
		idToDelete = parseInt( cmdString );
		if( idToDelete )
		{
			count = IterateOver( "ITEM" );
		}

		var tempMsg = GetDictionaryEntry( 8011, socket.language ); // Deleted %i items
		socket.SysMessage( tempMsg.replace( /%i/gi, count.toString() ));
	}
}

function onIterate( toCheck )
{
	if( toCheck && toCheck.isItem )
	{
		if( toCheck.id == idToDelete )
		{
			toCheck.Delete();
			return true;
		}
	}
	return false;
}