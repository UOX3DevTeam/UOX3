function CommandRegistration()
{
	RegisterCommand( "set", 2, true );
	RegisterCommand( "setpoisoned", 2, true );
}

function command_SET( socket, cmdString )
{
	if( cmdString )
	{
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
		break;
	case "STR":
	case "STRENGTH":
		ourObj.strength = StringToNum( splitString[1] );
		break;
	case "DEX":
	case "DEXTERITY":
		ourObj.dexterity = StringToNum( splitString[1] );
		break;
	case "INT":
	case "INTELLIGENCE":
		ourObj.intelligence = StringToNum( splitString[1] );
		break;
	case "FAME":
		ourObj.fame = StringToNum( splitString[1] );
		break;
	case "KARMA":
		ourObj.karma = StringToNum( splitString[1] );
		break;
	case "KILLS":
		ourObj.murdercount = StringToNum( splitString[1] );
		break;
	case "COLOR":
	case "COLOUR":
		ourObj.colour = StringToNum( splitString[1] );
		break;
	case "OWNER":
		socket.tempObj = ourObj;
		socket.CustomTarget( 1, "Choose character to own this object" );
		break;
	case "X":
		ourObj.x = StringToNum( splitString[1] );
		break;
	case "Y":
		ourObj.y = StringToNum( splitString[1] );
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
	switch( uKey )
	{
	case "ID":
		ourItem.id = StringToNum( value );
		break;
	case "AMOUNT":
		ourItem.amount = StringToNum( value );
		break;
	case "MOVABLE":
		ourItem.movable = StringToNum( value );
		break;
	case "WIPABLE":
		ourItem.wipable = (StringToNum( value ) == 1);
		break;
	case "BUYVALUE":
		ourItem.buyvalue = StringToNum( value );
		break;
	case "SELLVALUE":
		ourItem.sellvalue = StringToNum( value );
		break;
	case "RESTOCK":
		ourItem.restock = StringToNum( value );
		break;
	case "MORE":
		ourItem.more = StringToNum( value );
		break;
	case "MOREX":
		ourItem.morex = StringToNum( value );
		break;
	case "MOREY":
		ourItem.morey = StringToNum( value );
		break;
	case "MOREZ":
		ourItem.morez = StringToNum( value );
		break;
	case "DEVINELOCK":
		ourItem.devinelock = (StringToNum( value ) == 1);
		break;
	case "DIR":
		ourItem.dir = StringToNum( value );
		break;
	case "NAME2":
		ourItem.name2 = socket.xText.substring( 6 );
		break;
	case "TYPE":
		ourItem.type = StringToNum( value );
		break;
	case "Z":
		ourItem.z = StringToNum( value );
		break;
	case "DECAYABLE":
		ourItem.decayable = (StringToNum( value ) == 1);
		break;
	case "VISIBLE":
		ourItem.visible = StringToNum( value );
		break;
	default:
		if( ourObj.isSpawner )
			HandleSetSpawner( socket, ourObj, uKey, splitString[1] );
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
		break;
	case "MININTERVAL":
		ourSpawn.mininterval = StringToNum( value );
		break;
	case "MAXINTERVAL":
		ourSpawn.maxinterval = StringToNum( value );
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
		break;
	case "ID":
		var idVal = StringToNum( value );
		if( idVal <= 0x7CF )
			ourChar.id = idVal;
		break;
	case "FONT":
		ourChar.font = StringToNum( value );
		break;
	case "SPATTACK":
		ourChar.spattack = StringToNum( value );
		break;
	case "SPDELAY":
		ourChar.spdelay = StringToNum( value );
		break;
	case "POISON":
		ourChar.poison = StringToNum( value );
		break;
	case "TITLE":
		ourChar.title = socket.xText.substring( 6 );
		break;
	case "NPCWANDER":
		ourChar.wantertype = StringToNum( value );
		break;
	case "DIR":
		ourChar.dir = StringToNum( value );
		break;
	case "NPCAI":
		ourChar.aitype = StringToNum( value );
		break;
	case "VULNERABLE":
		ourChar.vulnerable = (StringToNum( value ) == 1);
		break;
	case "SPLIT":
		ourChar.split = StringToNum( value );
		break;
	case "SPLITCHANCE":
		ourChar.splitchance = StringToNum( value );
		break;
	case "COMMANDLEVEL":
		ourChar.commandlevel = StringToNum( value );
		break;
	case "Z":
		ourChar.z = StringToNum( value );
		break;
	case "CANTRAIN":
		if( ourChar.npc )
			ourChar.trainer = (StringToNum( value ) == 1);
		break;
	case "FROZEN":
		ourChar.frozen = (StringToNum( value ) == 1);
		break;
	case "VISIBLE":
		ourChar.visible = StringToNum( value );
		break;
	default:
		if( !ourChar.SetSkillByName( uKey, StringToNum( value ) ) )
			socket.SysMessage( "Invalid set command " + uKey );
		break;
	}
}

function onCallBack1( socket, ourObj )
{
	var toOwn = socket.tempObj;
	if( !socket.GetWord( 1 ) && ourObj.isChar && toOwn )
		toOwn.owner = ourObj;

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
	}
	socket.tempint = 0;
}
