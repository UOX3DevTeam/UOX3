function CommandRegistration()
{
	RegisterCommand( "get", 2, true );
}

function command_GET( socket, cmdString )
{
	if( cmdString )
	{
		socket.xText = cmdString;
		socket.CustomTarget( 0, "Choose target to get " + cmdString + " property for:" );
	}
	else
		socket.SysMessage( "No property was specified for the GET command." );
}

function onCallback0( socket, ourObj )
{
	if( socket.GetWord( 1 ) )
	{
		socket.SysMessage( "'Get': Invalid target" );
		return;
	}

	var splitString = socket.xText.split( " ", 1 );
	var uKey 	= splitString[0].toUpperCase();
	switch( uKey )
	{
	case "NAME":
		socket.SysMessage( ourObj.name );
		break;
	case "STR":
	case "STRENGTH":
		socket.SysMessage( ourObj.strength );
		break;
	case "DEX":
	case "DEXTERITY":
		socket.SysMessage( ourObj.dexterity );
		break;
	case "INT":
	case "INTELLIGENCE":
		socket.SysMessage( ourObj.intelligence );
		break;
	case "FAME":
		socket.SysMessage( ourObj.fame );
		break;
	case "HP":
	case "HEALTH":
		socket.SysMessage( ourObj.health );
		break;
	case "KARMA":
		socket.SysMessage( ourObj.karma );
		break;
	case "KILLS":
		socket.SysMessage( ourObj.murdercount );
		break;
	case "COLOR":
	case "COLOUR":
		socket.SysMessage( "0x" + (ourObj.colour).toString(16) + " (" + ourObj.colour + ")" );
		break;
	case "MAXHP":
		socket.SysMessage( ourObj.maxhp );
		break;
	case "MANA":
		socket.SysMessage( ourObj.mana );
		break;
	case "OWNER":
		if( ourObj.owner )
			socket.SysMessage( ourObj.owner.serial + " - " + ourObj.owner.name );
		else
			socket.SysMessage( "null" );
		break;
	case "POISON":
		socket.SysMessage( ourObj.poison );
		break;
	case "X":
		socket.SysMessage( ourObj.x );
		break;
	case "Y":
		socket.SysMessage( ourObj.y );
		break;
	case "TEMPSTR":
	case "TEMPSTRENGTH":
		socket.SysMessage( ourObj.tempstr );
		break;
	case "TEMPINT":
	case "TEMPINTELLIGENCE":
		socket.SysMessage( ourObj.tempint );
		break;
	case "TEMPDEX":
	case "TEMPDEXTERITY":
		socket.SysMessage( ourObj.tempdex );
		break;
	case "WIPABLE":
	case "WIPEABLE":
		socket.SysMessage( ourObj.wipable );
		break;
	case "RACE":
		if( ourObj.race )
			socket.SysMessage( ourObj.race.id + " - " + ourObj.race.name );
		else
			socket.SysMessage( "null" );
		break;
	case "INSTANCEID":
		socket.SysMessage( ourObj.instanceID );
		break;
	case "WORLDNUMBER":
		socket.SysMessage( ourObj.worldnumber );
		break;
	default:
		if( ourObj.isChar )
			HandleGetChar( socket, ourObj, uKey );
		else if( ourObj.isItem )
			HandleGetItem( socket, ourObj, uKey );
		else
			socket.SysMessage( "Invalid property provided for get command: " + uKey );
		break;
	}
}

function HandleGetItem( socket, ourItem, uKey )
{
	switch( uKey )
	{
	case "ID":
		socket.SysMessage( "0x" + (ourItem.id).toString(16) + " (" + ourItem.id + ")" );
		break;
	case "AMOUNT":
		socket.SysMessage( ourItem.amount );
		break;
	case "MOVEABLE":
	case "MOVABLE":
		socket.SysMessage( ourItem.movable );
		break;
	case "BUYVALUE":
		socket.SysMessage( ourItem.buyvalue );
		break;
	case "SELLVALUE":
		socket.SysMessage( ourItem.sellvalue );
		break;
	case "RESTOCK":
		socket.SysMessage( ourItem.restock );
		break;
	case "MAXITEMS":
		socket.SysMessage( ourItem.maxItems );
		break;
	case "MORE":
		socket.SysMessage( ourItem.more );
		break;
	case "MOREX":
		socket.SysMessage( ourItem.morex );
		break;
	case "MOREY":
		socket.SysMessage( ourItem.morey );
		break;
	case "MOREZ":
		socket.SysMessage( ourItem.morez );
		break;
	case "MOREXYZ":
		socket.SysMessage( ourItem.morex );
		socket.SysMessage( ourItem.morey );
		socket.SysMessage( ourItem.morez );
		break;
	case "NEWBIE":
		socket.SysMessage( ourItem.isNewbie );
		break;
	case "DIVINELOCK":
		socket.SysMessage( ourItem.divinelock );
		break;
	case "DIR":
		socket.SysMessage( ourItem.dir );
		break;
	case "NAME2":
		socket.SysMessage( ourItem.name2 );
		break;
	case "TYPE":
		socket.SysMessage( ourItem.type );
		break;
	case "Z":
		socket.SysMessage( ourItem.z );
		break;
	case "DECAYABLE":
		socket.SysMessage( ourItem.decayable );
		break;
	case "VISIBLE":
		socket.SysMessage( ourItem.visible );
		break;
	case "DESC":
		socket.SysMessage( ourItem.desc );
		break;
	case "DEF":
		socket.SysMessage( ourItem.Resist( 1 ));
		break;
	case "ARMORCLASS":
	case "ARMOURCLASS":
	case "AC":
		socket.SysMessage( ourItem.ac );
		break;
	case "LAYER":
		socket.SysMessage( "0x" + (ourItem.layer).toString(16) + "(" + ourItem.layer + ")" );
		break;
	case "LODAMAGE":
		socket.SysMessage( ourItem.lodamage );
		break;
	case "HIDAMAGE":
		socket.SysMessage( ourItem.hidamage );
		break;
	case "WEIGHT":
		socket.SysMessage( ourItem.weight );
		break;
	case "WEIGHTMAX":
		socket.SysMessage( ourItem.weightmax );
		break;
	case "SPEED":
	case "SPD":
		socket.SysMessage( ourItem.speed );
		break;
	case "STACKABLE":
	case "PILEABLE":
		socket.SysMessage( ourItem.pileable );
		break;
	case "RACE":
		if( ourItem.race )
			socket.SysMessage( ourItem.race.id + " - " + ourItem.race.name );
		else
			socket.SysMessage( "null" );
		break;
	default:
		if( ourItem.isSpawner )
			HandleGetSpawner( socket, ourItem, uKey );
		else
		{
			if( uKey == "SPAWNSECTION" || uKey == "MININTERVAL" || uKey == "MAXINTERVAL" )
			{
				socket.SysMessage( "These properties are only found on objects added as spawner objects via the 'ADD SPAWNER # command!" );
			}
			else
			{
				socket.SysMessage( "Invalid property provided for get command: " + uKey );
			}
		}
		break;
	}
}

function HandleGetSpawner( socket, ourSpawn, uKey )
{
	switch( uKey )
	{
	case "SPAWNSECTION":
		socket.SysMessage( ourSpawn.spawnsection );
		break;
	case "SECTIONALIST":
		socket.SysMessage( ourSpawn.sectionalist );
		break;
	case "MININTERVAL":
		socket.SysMessage( ourSpawn.mininterval );
		break;
	case "MAXINTERVAL":
		socket.SysMessage( ourSpawn.maxinterval );
		break;
	default:
		socket.SysMessage( "Invalid property provided for get command: " + uKey );
	}
}

function HandleGetChar( socket, ourChar, uKey )
{
	switch( uKey )
	{
	case "ID":
		socket.SysMessage( "0x" + (ourChar.id).toString(16) + " (" + ourChar.id + ")" );
		break;
	case "ORGID":
		socket.SysMessage( "0x" + (ourChar.orgID).toString(16) + " (" + ourChar.orgID + ")" );
		break;
	case "FONT":
		socket.SysMessage( ourChar.font );
		break;
	case "SPATTACK":
		socket.SysMessage( ourChar.spattack );
		break;
	case "SPDELAY":
		socket.SysMessage( ourChar.spdelay );
		break;
	case "TITLE":
		socket.SysMessage( ourChar.title );
		break;
	case "TOTAME":
		socket.SysMessage( ourChar.skillToTame );
		break;
	case "TOPROV":
		socket.SysMessage( ourChar.skillToProv );
		break;
	case "TOPEACE":
		socket.SysMessage( ourChar.skillToPeace );
		break;
	case "NPCWANDER":
		socket.SysMessage( ourChar.wandertype );
		break;
	case "OLDWANDERTYPE":
		socket.SysMessage( ourChar.oldwandertype );
		break;
	case "DIR":
	case "DIRECTION":
		socket.SysMessage( ourChar.direction );
		break;
	case "NPCAI":
		socket.SysMessage( ourChar.aitype );
		break;
	case "VULNERABLE":
		socket.SysMessage( ourChar.vulnerable );
		break;
	case "SPLIT":
		socket.SysMessage( ourChar.split );
		break;
	case "SPLITCHANCE":
		socket.SysMessage( ourChar.splitchance );
		break;
	case "COMMANDLEVEL":
		socket.SysMessage( ourChar.commandlevel );
		break;
	case "Z":
		socket.SysMessage( ourChar.z );
		break;
	case "CANTRAIN":
		socket.SysMessage( ourChar.trainer );
		break;
	case "FROZEN":
		socket.SysMessage( ourChar.frozen );
		break;
	case "VISIBLE":
		socket.SysMessage( ourChar.visible );
		break;
	case "ARMOUR":
	case "ARMOR":
		socket.SysMessage( ourChar.Resist( 1 ));
		break;
	case "MAXHP":
		socket.SysMessage( ourChar.maxhp );
		break;
	case "MAXMANA":
		socket.SysMessage( ourChar.maxmana );
		break;
	case "MAXSTAMINA":
		socket.SysMessage( ourChar.maxstamina );
		break;
	case "HUNGER":
		socket.SysMessage( ourChar.hunger );
		break;
	case "LANGUAGE":
		if( !ourChar.npc );
		{
			socket.SysMessage( ourChar.socket.language );
		}
		break;
	default:
		socket.SysMessage( "Invalid property provided for get command: " + uKey );
		break;
	}
}

