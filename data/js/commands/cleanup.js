function CommandRegistration(contextID)
{
	RegisterCommand( "cleanup", 2, true,contextID );
}

function command_CLEANUP( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 83, socket.Language ) );
	var count = IterateOver( "ITEM" );
	socket.SysMessage( GetDictionaryEntry( 84, socket.Language ) );
	socket.SysMessage( NumToString( count ) + " corpses or gates have been deleted" );
}

function onIterate( toCheck )
{
	if( toCheck && toCheck.isItem )
	{
		if( toCheck.corpse || toCheck.type == 51 || toCheck.type == 52 )
		{
			toCheck.Delete();
			return true;
		}
	}
	return false;
}