// This command lets GMs pick up and move even items that are marked as non-movable
function CommandRegistration()
{
	RegisterCommand( "allmove", 2, true );
}

function command_ALLMOVE( socket, cmdString )
{
	var pUser = socket.currentChar;
	var value = ( cmdString.toUpperCase() == "ON" );
	var sysMessage;

	pUser.allmove = value;
	pUser.Teleport();
	if( value )
	{
		sysMessage = GetDictionaryEntry( 39, socket.language ); // ALLMOVE enabled.
	}
	else
	{
		sysMessage = GetDictionaryEntry( 40, socket.language ); // ALLMOVE disabled
	}
	socket.SysMessage( sysMessage );
}
