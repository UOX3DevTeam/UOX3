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
			socket.SysMessage( GetDictionaryEntry( 1755, socket.Language )); //Additional arguments required
			return;
		}
		var targMsg = GetDictionaryEntry( 1741, socket.Language );
		socket.xText = cmdString;
		socket.CustomTarget( 0, "Choose target to set: " + cmdString );
	}
}

function onCallback0( socket, ourObj )
{
	if( socket.GetWord( 1 ) )
	{
		socket.SysMessage( "'Set': Invalid target" );
		return;
	}

	var splitString = socket.xText.split( " ", 2 );
	uKey = splitString[0].toUpperCase();
	switch( uKey )
	{
	case "NAME":
		ourObj.name = socket.xText.substring( 5 );
		okMsg( socket );
		break;
	case "STR":
	case "STRENGTH":
		ourObj.strength = StringToNum( splitString[1] );
		break;
	case "DEX":
	case "DEXTERITY":
		ourObj.dexterity = StringToNum( splitString[1] );
		okMsg( socket );
		break;
	case "INT":
	case "INTELLIGENCE":
		ourObj.intelligence = StringToNum( splitString[1] );
		okMsg( socket );
		break;
	case "FAME":
		ourObj.fame = StringToNum( splitString[1] );
		okMsg( socket );
		break;
	case "KARMA":
		ourObj.karma = StringToNum( splitString[1] );
		break;
	case "KILLS":
		ourObj.murdercount = StringToNum( splitString[1] );
		okMsg( socket );
		break;
	case "COLOR":
	case "COLOUR":
		ourObj.colour = StringToNum( splitString[1] );
		okMsg( socket );
		break;
	case "OWNER":
		socket.tempObj = ourObj;
		socket.CustomTarget( 1, "Choose character to own this object" );
		break;
	case "X":
		ourObj.x = StringToNum( splitString[1] );
		okMsg( socket );
		break;
	case "Y":
		ourObj.y = StringToNum( splitString[1] );
		okMsg( socket );
		break;
	default:
		if( ourObj.isChar )
			HandleSetChar( socket, ourObj, uKey, splitString[1] );
		else if( ourObj.isItem )
			HandleSetItem( socket, ourObj, uKey, splitString[1] );
		else
			socket.SysMessage( "Invalid set command " + uKey );
		break;
	}
}

function HandleSetItem( socket, ourItem, uKey, value )
{
	var splitString = socket.xText.split( " ", 2 );
	uKey = splitString[0].toUpperCase();
	switch( uKey )
	{
	case "ID":
		ourItem.id = StringToNum( value );
		okMsg( socket );
		break;
	case "AMOUNT":
		ourItem.amount = StringToNum( value );
		okMsg( socket );
		break;
	case "MOVABLE":
		ourItem.movable = StringToNum( value );
		okMsg( socket );
		break;
	case "WIPABLE":
		ourItem.wipable = (StringToNum( value ) == 1);
		okMsg( socket );
		break;
	case "BUYVALUE":
		ourItem.buyvalue = StringToNum( value );
		okMsg( socket );
		break;
	case "SELLVALUE":
		ourItem.sellvalue = StringToNum( value );
		okMsg( socket );
		break;
	case "RESTOCK":
		ourItem.restock = StringToNum( value );
		okMsg( socket );
		break;
	case "MORE":
		ourItem.more = StringToNum( value );
		okMsg( socket );
		break;
	case "MOREX":
		ourItem.morex = StringToNum( value );
		okMsg( socket );
		break;
	case "MOREY":
		ourItem.morey = StringToNum( value );
		okMsg( socket );
		break;
	case "MOREZ":
		ourItem.morez = StringToNum( value );
		okMsg( socket );
		break;
	case "DEVINELOCK":
		ourItem.devinelock = (StringToNum( value ) == 1);
		okMsg( socket );
		break;
	case "DIR":
		ourItem.dir = StringToNum( value );
		okMsg( socket );
		break;
	case "NAME2":
		ourItem.name2 = socket.xText.substring( 6 );
		okMsg( socket );
		break;
	case "TYPE":
		ourItem.type = StringToNum( value );
		okMsg( socket );
		break;
	case "Z":
		ourItem.z = StringToNum( value );
		okMsg( socket );
		break;
	case "DECAYABLE":
		ourItem.decayable = (StringToNum( value ) == 1);
		okMsg( socket );
		break;
	case "VISIBLE":
		ourItem.visible = StringToNum( value );
		okMsg( socket );
		break;
	case "DESC":
		ourItem.desc = StringToNum( value );
		okMsg( socket );
		break;
	default:
		if( ourItem.isSpawner )
			HandleSetSpawner( socket, ourItem, uKey, splitString[1] );
		else
			socket.SysMessage( "Invalid set command " + uKey );
		break;
	}
}

function HandleSetSpawner( socket, ourSpawn, uKey, value )
{
	switch( uKey )
	{
	case "SPAWNSECTION":
		splitValues = socket.xText.split( " " )
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
		ourSpawn.mininterval = StringToNum( value );
		okMsg( socket );
		break;
	case "MAXINTERVAL":
		ourSpawn.maxinterval = StringToNum( value );
		okMsg( socket );
		break;
	default:
		socket.SysMessage( "Invalid set command " + uKey );
	}
}

function HandleSetChar( socket, ourChar, uKey, value )
{
	switch( uKey )
	{
	case "ALLSKILLS":
		ourChar.baseskills.allskills = StringToNum( value );
		okMsg( socket );
		break;
	case "ID":
		var idVal = StringToNum( value );
		if( idVal <= 0x7CF )
		{
			ourChar.id = idVal;
			okMsg( socket );
		}
		break;
	case "FONT":
		ourChar.font = StringToNum( value );
		okMsg( socket );
		break;
	case "SPATTACK":
		ourChar.spattack = StringToNum( value );
		okMsg( socket );
		break;
	case "SPDELAY":
		ourChar.spdelay = StringToNum( value );
		okMsg( socket );
		break;
	case "POISON":
		ourChar.poison = StringToNum( value );
		okMsg( socket );
		break;
	case "TITLE":
		ourChar.title = socket.xText.substring( 6 );
		okMsg( socket );
		break;
	case "NPCWANDER":
		ourChar.wantertype = StringToNum( value );
		okMsg( socket );
		break;
	case "DIR":
		ourChar.dir = StringToNum( value );
		okMsg( socket );
		break;
	case "NPCAI":
		ourChar.aitype = StringToNum( value );
		okMsg( socket );
		break;
	case "VULNERABLE":
		ourChar.vulnerable = (StringToNum( value ) == 1);
		okMsg( socket );
		break;
	case "SPLIT":
		ourChar.split = StringToNum( value );
		okMsg( socket );
		break;
	case "SPLITCHANCE":
		ourChar.splitchance = StringToNum( value );
		okMsg( socket );
		break;
	case "COMMANDLEVEL":
		ourChar.commandlevel = StringToNum( value );
		okMsg( socket );
		break;
	case "Z":
		ourChar.z = StringToNum( value );
		break;
	case "CANTRAIN":
		if( ourChar.npc )
		{
			ourChar.trainer = (StringToNum( value ) == 1);
			okMsg( socket );
		}
		break;
	case "FROZEN":
		ourChar.frozen = (StringToNum( value ) == 1);
		okMsg( socket );
		break;
	case "VISIBLE":
		ourChar.visible = StringToNum( value );
		okMsg( socket );
		break;
	default:
		if( !ourChar.SetSkillByName( uKey, StringToNum( value ) ) )
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
