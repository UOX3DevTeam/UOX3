// This command lets GMs apply certain other commands to all objects within an area

function CommandRegistration()
{
	RegisterCommand( "areacommand", 2, true );
}

function command_AREACOMMAND( socket, cmdString )
{
	if( cmdString )
	{
		socket.xText = cmdString;
		var targMsg = GetDictionaryEntry( 9104, socket.language ); // Select first corner of areacommand box:
		socket.CustomTarget( 0, targMsg );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 8076, socket.language )); // AREACOMMAND requires a subcommand
	}
}

function onCallback0( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var targMsg	= GetDictionaryEntry( 9105, socket.language ); // Select second corner of areacommand box:
		socket.clickX = socket.GetWord( 11 );
		socket.clickY = socket.GetWord( 13 );
		socket.CustomTarget( 1, targMsg );
	}
}

function onCallback1( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		x1 = socket.clickX;
		y1 = socket.clickY;
		x2 = socket.GetWord( 11 );
		y2 = socket.GetWord( 13 );
		var tmpLoc;
		if( x1 > x2 )
		{
			tmpLoc 	= x1;
			x1 	= x2;
			x2	= tmpLoc;
		}
		if( y1 > y2 )
		{
			tmpLoc 	= y1;
			y1 	= y2;
			y2 	= tmpLoc;
		}

		var splitString = socket.xText.split( " " );
		if( splitString[0] )
		{
			key = splitString[0].toUpperCase();
			if( splitString[1] )
			{
				value = parseInt( splitString[1] );
			}
			else
			{
				value = 0;
			}
			var iCount = IterateOver( "ITEM" );
			var tempMsg = GetDictionaryEntry( 9103, socket.language ); // %i items affected by %s %d
			tempMsg = ( tempMsg.replace( /%s/gi, key ));
			tempMsg = ( tempMsg.replace( /%d/gi, value ));
			socket.SysMessage( tempMsg.replace( /%i/gi, iCount ));
		}

		socket.clickX = -1;
		socket.clickY = -1;
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		key = "";
		value = 0;
	}
}

function onIterate( toCheck )
{
	if( toCheck && toCheck.isItem && toCheck.container == null )
	{
		if( toCheck.x >= x1 && toCheck.x <= x2 && toCheck.y >= y1 && toCheck.y <= y2 )
		{
			switch( key )
			{
			case "DYE":			toCheck.colour = value;			break;
			case "WIPE":		toCheck.Delete();				break;
			case "INCX":		toCheck.x = toCheck.x + value;	break;
			case "INCY":		toCheck.y = toCheck.y + value;	break;
			case "INCZ":		toCheck.z = toCheck.z + value;	break;
			case "SETX":		toCheck.x = value;				break;
			case "SETY":		toCheck.y = value;				break;
			case "SETZ":		toCheck.z = value;				break;
			case "SETTYPE":		toCheck.type = value;			break;
			case "SETEVENT":	toCheck.event = value;			break;
//			case "NEWBIE":		toCheck.newbie = value;			break;
			case "SETSCPTRIG":	toCheck.scripttrigger = value;	break;
			case "ADDSCPTRIG": 	toCheck.AddScriptTrigger( value ); break;
			case "REMOVESCPTRIG":	toCheck.RemoveScriptTrigger( value );	break;
			case "MOVABLE":		toCheck.movable = value;		break;
			case "DECAYABLE":   toCheck.decayable = value; 		break;
			default:						return false;
			}
			return true;
		}
	}
	return false;
}
