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
	var nVal 	= parseInt( splitString[1] );
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
	case "HP":
	case "HEALTH":
		ourObj.health = nVal;
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
	case "MAXHP":
		ourObj.maxhp = nVal;
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
	case "TEMPSTR":
	case "TEMPSTRENGTH":
		ourObj.tempstr = nVal;
		okMsg( socket );
		break;
	case "TEMPINT":
	case "TEMPINTELLIGENCE":
		ourObj.tempint = nVal;
		okMsg( socket );
		break;
	case "TEMPDEX":
	case "TEMPDEXTERITY":
		ourObj.tempdex = nVal;
		okMsg( socket );
		break;
	case "WIPABLE":
	case "WIPEABLE":
		ourObj.wipable = (nVal == 1);
		okMsg( socket );
		break;
	case "RACE":
		ourObj.race = nVal;
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
	case "MOVEABLE":
	case "MOVABLE":
		ourItem.movable = nVal;
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
		var splitValues = socket.xText.split( " " );
		if( splitValues[3] )
		{
			ourItem.morex = parseInt( splitValues[1] );
			ourItem.morey = parseInt( splitValues[2] );
			ourItem.morez = parseInt( splitValues[3] );
			okMsg( socket );
		}
		break;
	case "NEWBIE":
		ourItem.isNewbie = (nVal == 1);
		okMsg( socket );
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
		if(( nVal >= 61 && nVal <= 65 ) || nVal == 69 || nVal == 125 )
		{
			// User is trying to set type 62 on an item. This should only work if the item is of type OT_SPAWNER
			if( ourItem.isSpawner )
			{
				ourItem.type = nVal;
				okMsg( socket );
			}
			else
			{
				socket.SysMessage( "Only objects added using the 'ADD SPAWNER # command can be assigned spawner item types!" );
			}
		}
		else
		{
			ourItem.type = nVal;
			okMsg( socket );
		}
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
	case "DEF":
		ourItem.Resist( 1, nVal );
		okMsg( socket );
		break;
	case "LAYER":
		ourItem.layer = nVal;
		okMsg( socket );
		break;
	case "LODAMAGE":
		ourItem.lodamage = nVal;
		okMsg( socket );
		break;
	case "HIDAMAGE":
		ourItem.hidamage = nVal;
		okMsg( socket );
		break;
	case "WEIGHT":
		ourItem.weight = nVal;
		okMsg( socket );
		break;
	case "WEIGHTMAX":
		ourItem.weightmax = nVal;
		okMsg( socket );
		break;
	case "SPEED":
	case "SPD":
		ourItem.speed = nVal;
		okMsg( socket );
		break;
	case "STACKABLE":
	case "PILEABLE":
		ourItem.pileable = nVal;
		okMsg( socket );
		break;
	case "RACE":
		ourItem.race = nVal;
		okMsg( socket );
		break;
	default:
		if( ourItem.isSpawner )
			HandleSetSpawner( socket, ourItem, uKey, nVal );
		else
		{
			if( uKey == "SPAWNSECTION" || uKey == "MININTERVAL" || uKey == "MAXINTERVAL" )
			{
				socket.SysMessage( "This can only be set on objects added as spawner objects through the 'ADD SPAWNER # command!" );
			}
			else
			{
			socket.SysMessage( "Invalid set command " + uKey );
			}
		}
		break;
	}
}

function HandleSetSpawner( socket, ourSpawn, uKey, nVal )
{
	switch( uKey )
	{
	case "SPAWNSECTION":
		var splitValues = socket.xText.split( " " );
		if( splitValues[2] )
		{
			ourSpawn.spawnsection = splitValues[1];
			ourSpawn.sectionalist = (splitValues[2] == "TRUE");
		}
		else
		{
			ourSpawn.spawnsection = splitValues[1];
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
			ourChar.orgID = nVal;
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
	case "TOTAME":
		if( ourChar.npc )
		{
			ourChar.skillToTame = nVal;
			okMsg( socket );
		}
		break;
	case "TOPROV":
		if( ourChar.npc )
		{
			ourChar.skillToProv = nVal;
			okMsg( socket );
		}
		break;
	case "TOPEACE":
		if( ourChar.npc )
		{
			ourChar.skillToPeace = nVal;
			okMsg( socket );
		}
		break;
	case "NPCWANDER":
		ourChar.wandertype = nVal;
		okMsg( socket );
		break;
	case "DIR":
	case "DIRECTION":
		ourChar.direction = nVal;
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
	case "ARMOUR":
	case "ARMOR":
		ourChar.Resist( 1, nVal );
		okMsg( socket );
		break;
	case "MAXHP":
		ourChar.maxhp = nVal;
		okMsg( socket );
		break;
	case "MAXMANA":
		ourChar.maxmana = nVal;
		okMsg( socket );
		break;
	case "MAXSTAMINA":
		ourChar.maxstamina = nVal;
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
		socket.tempint = parseInt( cmdString );
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
