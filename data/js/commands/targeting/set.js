function CommandRegistration()
{
	RegisterCommand( "set", 2, true );
	RegisterCommand( "setpoisoned", 2, true );
}

function command_SET( socket, cmdString )
{
	if( cmdString )
	{
		var splitString = cmdString.split( " ", 2 );
		if( !splitString[1] )
		{
			var uKey = splitString[0].toUpperCase();
			if( uKey != "OWNER" ) // SET OWNER requires no additional arguments
			{
				socket.SysMessage( GetDictionaryEntry( 1755, socket.Language )); //Additional arguments required
				return;
			}
		}
		var targMsg = GetDictionaryEntry( 1741, socket.Language );
		socket.xText = cmdString;
		socket.CustomTarget( 0, "Choose target to set: " + cmdString );
	}
	else
		socket.SysMessage( "No property was specified for the SET command." );
}

function onCallback0( socket, ourObj )
{
	if( socket.GetWord( 1 ) )
	{
		socket.SysMessage( "'Set': Invalid target" );
		return;
	}

	var splitString = socket.xText.split( " ", 2 );
	var uKey 	= splitString[0].toUpperCase();
	var nVal 	= StringToNum( splitString[1] );
	switch( uKey )
	{
	case "NAME":
		ourObj.name = socket.xText.substring( 5 );
		okMsg( socket );
		break;
	case "STR":
	case "STRENGTH":
		ourObj.strength = nVal;
		okMsg( socket );
		break;
	case "DEX":
	case "DEXTERITY":
		ourObj.dexterity = nVal;
		okMsg( socket );
		break;
	case "INT":
	case "INTELLIGENCE":
		ourObj.intelligence = nVal;
		okMsg( socket );
		break;
	case "FAME":
		ourObj.fame = nVal;
		okMsg( socket );
		break;
	case "KARMA":
		ourObj.karma = nVal;
		okMsg( socket );
		break;
	case "KILLS":
		ourObj.murdercount = nVal;
		okMsg( socket );
		break;
	case "COLOR":
	case "COLOUR":
		ourObj.colour = nVal;
		okMsg( socket );
		break;
	case "OWNER":
		socket.tempObj = ourObj;
		socket.CustomTarget( 1, "Choose character to own this object" );
		break;
	case "X":
		ourObj.x = nVal;
		okMsg( socket );
		break;
	case "Y":
		ourObj.y = nVal;
		okMsg( socket );
		break;
	default:
		if( ourObj.isChar )
			HandleSetChar( socket, ourObj, uKey, nVal );
		else if( ourObj.isItem )
			HandleSetItem( socket, ourObj, uKey, nVal );
		else
			socket.SysMessage( "Invalid set command " + uKey );
		break;
	}
}

function HandleSetItem( socket, ourItem, uKey, nVal )
{
	switch( uKey )
	{
	case "ID":
		ourItem.id = nVal;
		okMsg( socket );
		break;
	case "AMOUNT":
		ourItem.amount = nVal;
		okMsg( socket );
		break;
	case "MOVABLE":
		ourItem.movable = nVal;
		okMsg( socket );
		break;
	case "WIPABLE":
		ourItem.wipable = (nVal == 1);
		okMsg( socket );
		break;
	case "BUYVALUE":
		ourItem.buyvalue = nVal;
		okMsg( socket );
		break;
	case "SELLVALUE":
		ourItem.sellvalue = nVal;
		okMsg( socket );
		break;
	case "RESTOCK":
		ourItem.restock = nVal;
		okMsg( socket );
		break;
	case "MORE":
		ourItem.more = nVal;
		okMsg( socket );
		break;
	case "MOREX":
		ourItem.morex = nVal;
		okMsg( socket );
		break;
	case "MOREY":
		ourItem.morey = nVal;
		okMsg( socket );
		break;
	case "MOREZ":
		ourItem.morez = nVal;
		okMsg( socket );
		break;
	case "MOREXYZ":
		var splitValues = socket.xText.split( " " )
		if( splitValues[3] )
		{
			ourItem.morex = StringToNum( splitValues[1] );
			ourItem.morey = StringToNum( splitValues[2] );
			ourItem.morez = StringToNum( splitValues[3] );
			okMsg( socket );
		}
		break;
	case "DEVINELOCK":
		ourItem.devinelock = (nVal == 1);
		okMsg( socket );
		break;
	case "DIR":
		ourItem.dir = nVal;
		okMsg( socket );
		break;
	case "NAME2":
		ourItem.name2 = socket.xText.substring( 6 );
		okMsg( socket );
		break;
	case "TYPE":
		ourItem.type = nVal;
		okMsg( socket );
		break;
	case "Z":
		ourItem.z = nVal;
		okMsg( socket );
		break;
	case "DECAYABLE":
		ourItem.decayable = (nVal == 1);
		okMsg( socket );
		break;
	case "VISIBLE":
		ourItem.visible = nVal;
		okMsg( socket );
		break;
	case "DESC":
		ourItem.desc = nVal;
		okMsg( socket );
		break;
	default:
		if( ourItem.isSpawner )
			HandleSetSpawner( socket, ourItem, uKey, nVal );
		else
			socket.SysMessage( "Invalid set command " + uKey );
		break;
	}
}

function HandleSetSpawner( socket, ourSpawn, uKey, nVal )
{
	switch( uKey )
	{
	case "SPAWNSECTION":
		var splitValues = socket.xText.split( " " )
		if( splitValues[2] )
		{
			ourSpawn.spawnsection = splitValues[1];
			ourSpawn.sectionalist = (splitValues[2] == "TRUE");
		}
		else
		{
			ourSpawn.spawnsection = value;
			ourSpawn.sectionalist = false;
		}
		okMsg( socket );
		break;
	case "MININTERVAL":
		ourSpawn.mininterval = nVal;
		okMsg( socket );
		break;
	case "MAXINTERVAL":
		ourSpawn.maxinterval = nVal;
		okMsg( socket );
		break;
	default:
		socket.SysMessage( "Invalid set command " + uKey );
	}
}

function HandleSetChar( socket, ourChar, uKey, nVal )
{
	switch( uKey )
	{
	case "ALLSKILLS":
		ourChar.baseskills.allskills = nVal;
		okMsg( socket );
		break;
	case "ID":
		if( nVal <= 0x7CF )
		{
			ourChar.id = nVal;
			okMsg( socket );
		}
		break;
	case "FONT":
		ourChar.font = nVal;
		okMsg( socket );
		break;
	case "SPATTACK":
		ourChar.spattack = nVal;
		okMsg( socket );
		break;
	case "SPDELAY":
		ourChar.spdelay = nVal;
		okMsg( socket );
		break;
	case "POISON":
		ourChar.poison = nVal;
		okMsg( socket );
		break;
	case "TITLE":
		ourChar.title = socket.xText.substring( 6 );
		okMsg( socket );
		break;
	case "NPCWANDER":
		ourChar.wandertype = nVal;
		okMsg( socket );
		break;
	case "DIR":
		ourChar.dir = nVal;
		okMsg( socket );
		break;
	case "NPCAI":
		ourChar.aitype = nVal;
		okMsg( socket );
		break;
	case "VULNERABLE":
		ourChar.vulnerable = (nVal == 1);
		okMsg( socket );
		break;
	case "SPLIT":
		ourChar.split = nVal;
		okMsg( socket );
		break;
	case "SPLITCHANCE":
		ourChar.splitchance = nVal;
		okMsg( socket );
		break;
	case "COMMANDLEVEL":
		ourChar.commandlevel = nVal;
		okMsg( socket );
		break;
	case "Z":
		ourChar.z = nVal;
		okMsg( socket );
		break;
	case "CANTRAIN":
		if( ourChar.npc )
		{
			ourChar.trainer = (nVal == 1);
			okMsg( socket );
		}
		break;
	case "FROZEN":
		ourChar.frozen = (nVal == 1);
		okMsg( socket );
		break;
	case "VISIBLE":
		ourChar.visible = nVal;
		okMsg( socket );
		break;
	default:
		if( ourChar.SetSkillByName( uKey, nVal ) )
			okMsg( socket );
		else
			socket.SysMessage( "Invalid set command " + uKey );
		break;
	}
}

function onCallback1( socket, ourObj )
{
	var toOwn = socket.tempObj;
	if( !socket.GetWord( 1 ) && ourObj.isChar && toOwn )
	{
		toOwn.owner = ourObj;
		okMsg( socket );
	}
	socket.tempObj = null;
}

function command_SETPOISONED( socket, cmdString )
{
	if( cmdString )
	{
		var targMsg = GetDictionaryEntry( 240, socket.Language );
		socket.tempint = StringToNum( cmdString );
		socket.CustomTarget( 2, targMsg );
	}
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var poisonStrength = socket.tempint;
		ourObj.SetPoisoned( poisonStrength, 180000 );
		okMsg( socket );
	}
	socket.tempint = 0;
}

function okMsg( socket )
{ //Sends verification to the player that the specified value was successfully set.
	socket.SysMessage( GetDictionaryEntry( 1756, socket.Language ));
}
