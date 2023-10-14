function CommandRegistration()
{
	RegisterCommand( "gumpmenu", 2, true );
	RegisterCommand( "nacct", 2, true );
}

// This command is used to open specific gumps from dfndata/misc/gumps.dfn
function command_GUMPMENU( socket, cmdString )
{
	if( cmdString )
	{
		socket.OpenGump( parseInt( cmdString ));
	}
}

// This command is used to open a proof-of-concept (nonfunctional) account management gump
function command_NACCT( socket, cmdString )
{
	if( cmdString )
	{
		var splitString = cmdString.split( " " );
		var uKey = splitString[0].toUpperCase();
		switch( uKey )
		{
		case "ADD":		socket.OpenGump( 8 );		break;
		case "BAN":		break;
		case "EDIT":	socket.OpenGump( 10 );		break;
		case "VIEW":	socket.OpenGump( 9 );		break;
		case "LIST":
		default:		socket.OpenGump( 7 );		break;
		}
	}
	else
	{
		socket.OpenGump( 7 );
	}
}
