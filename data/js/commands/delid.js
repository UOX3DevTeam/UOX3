function CommandRegistration(contextID)
{
	RegisterCommand( "delid", 2, true,contextID );
}

function command_DELID( socket, cmdString )
{
	if( cmdString )
	{
		idToDelete = StringToNum( cmdString );
		if( idToDelete )
			var count = IterateOver( "ITEM" );
		socket.SysMessage( "Deleted " + NumToString( count ) + " items" );
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