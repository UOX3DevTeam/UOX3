// This command is used to toggle houseicons for multis
// NOTE: Currently does not work, because multis no longer have separate "items" that can
// be displayed!

function CommandRegistration()
{
	RegisterCommand( "houseicons", 2, true );
}

function command_HOUSEICONS( socket, cmdString )
{
	var pUser = socket.currentChar;
	var value = ( cmdString.toUpperCase() == "ON" );
	var sysMessage;

	pUser.houseicons = value;
	pUser.Teleport();
	if( value )
	{
		sysMessage = GetDictionaryEntry( 41, socket.language ); // House icons visible. (Houses invisible)
	}
	else
	{
		sysMessage = GetDictionaryEntry( 42, socket.language ); // House icons hidden. (Houses visible)
	}
	socket.SysMessage( sysMessage );
}
