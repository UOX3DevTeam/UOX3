function CommandRegistration()
{
	RegisterCommand( "gumpmenu", 2, true );
	RegisterCommand( "nacct", 2, true );
}

function command_GUMPMENU( socket, cmdString )
{
	if( cmdString )
		socket.OpenGump( parseInt( cmdString ) );
}

function command_NACCT( socket, cmdString )
{
	if( cmdString )
	{
		var splitString = cmdString.split( " " );
		var uKey = splitString[0].toUpperCase();
		switch( uKey )
		{
		case "ADD":	socket.OpenGump( 8 );		break;
		case "BAN":					break;
		case "EDIT":	socket.OpenGump( 10 );		break;
		case "VIEW":	socket.OpenGump( 9 );		break;
		case "LIST":
		default:	socket.OpenGump( 7 );		break;
		}
	}
	else
		socket.OpenGump( 7 );
}
