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
			areaCmdKey = splitString[0].toUpperCase();
			if( splitString[1] )
			{
				if( !isNaN( splitString[1] ) && splitString[1].match( /^\d+$/ ))
				{
					areaCmdValue = parseInt( splitString[1] ); // int
				}
				else
				{
					areaCmdValue = splitString[1]; // string

					if( splitString.length > 2 )
					{
						areaCmdValue = splitString.slice( 1 ).join( " " );
					}
				}
			}
			else
			{
				areaCmdValue = 0;
			}
			var iCount = IterateOver( "ITEM" );
			var tempMsg = GetDictionaryEntry( 9103, socket.language ); // %i items affected by %s %d
			tempMsg = ( tempMsg.replace( /%s/gi, areaCmdKey ));
			tempMsg = ( tempMsg.replace( /%d/gi, areaCmdValue ));
			socket.SysMessage( tempMsg.replace( /%i/gi, iCount ));
		}

		socket.clickX = -1;
		socket.clickY = -1;
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		areaCmdKey = "";
		areaCmdValue = 0;
	}
}

function onIterate( toCheck )
{
	if( toCheck && toCheck.isItem && toCheck.container == null )
	{
		if( toCheck.x >= x1 && toCheck.x <= x2 && toCheck.y >= y1 && toCheck.y <= y2 )
		{
			switch( areaCmdKey )
			{
			case "DYE":			toCheck.colour = areaCmdValue;			break;
			case "WIPE":		toCheck.Delete();				break;
			case "INCX":		toCheck.x = toCheck.x + areaCmdValue;	break;
			case "INCY":		toCheck.y = toCheck.y + areaCmdValue;	break;
			case "INCZ":		toCheck.z = toCheck.z + areaCmdValue;	break;
			case "NAME": 		toCheck.name = areaCmdValue;			break;
			case "SETX":		toCheck.x = areaCmdValue;				break;
			case "SETY":		toCheck.y = areaCmdValue;				break;
			case "SETZ":		toCheck.z = areaCmdValue;				break;
			case "SETTYPE":		toCheck.type = areaCmdValue;			break;
			case "SETEVENT":	toCheck.event = areaCmdValue;			break;
//			case "NEWBIE":		toCheck.newbie = areaCmdValue;			break;
			case "SETSCPTRIG":	toCheck.scripttrigger = areaCmdValue;	break;
			case "ADDSCPTRIG": 	toCheck.AddScriptTrigger( areaCmdValue ); break;
			case "REMOVESCPTRIG":	toCheck.RemoveScriptTrigger( areaCmdValue );	break;
			case "MOVABLE":		toCheck.movable = areaCmdValue;		break;
			case "DECAYABLE":   toCheck.decayable = areaCmdValue; 		break;
			default:						return false;
			}
			return true;
		}
	}
	return false;
}

function _restorecontext_() {}
