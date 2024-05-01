// This command is used to set values for various properties on objects

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
				socket.SysMessage( GetDictionaryEntry( 1755, socket.language )); //Additional arguments required
				return;
			}
		}

		socket.xText = cmdString;
		socket.CustomTarget( 0, GetDictionaryEntry( 8102, socket.language ) + " " + cmdString ); // Choose target to set:
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 8103, socket.language )); // No property was specified for the SET command.
	}
}

// Common Object properties
function onCallback0( socket, ourObj )
{
	if( socket.GetWord( 1 ) && !ValidateObject( ourObj ))
	{
		socket.SysMessage( "'Set': " + GetDictionaryEntry( 2353, socket.language )); // Invalid target
		return;
	}

	var splitString = socket.xText.split( " " );
	var uKey = splitString[0].toUpperCase();
	var nVal = splitString[1];
	nVal = nVal === "true" ? 1 : nVal === "false" ? 0 : parseInt( nVal );

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
	case "DEF":
	case "RESISTARMOR":
		ourObj.Resist( 1, nVal );
		okMsg( socket );
		break;
	case "RESISTLIGHT":
		ourObj.Resist( 2, nVal );
		okMsg( socket );
		break;
	case "RESISTWATER":
		ourObj.Resist( 3, nVal );
		okMsg( socket );
		break;
	case "RESISTCOLD":
		ourObj.Resist( 4, nVal );
		okMsg( socket );
		break;
	case "RESISTFIRE":
		ourObj.Resist( 5, nVal );
		okMsg( socket );
		break;
	case "RESISTENERGY":
		ourObj.Resist( 6, nVal );
		okMsg( socket );
		break;
	case "RESISTPOISON":
		ourObj.Resist( 7, nVal );
		okMsg( socket );
		break;
	case "LOWERSTATREQ":
		ourItem.lowerStatReq = nVal;
		okMsg( socket );
		break;
	case "HP":
	case "HEALTH":
		ourObj.health = nVal;
		if( ourObj.isDamageable )
		{
			// Refresh potential health bar for damageable objects
			ourObj.UpdateStats( 0 );
		}
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
	case "HUE":
	case "COLOR":
	case "COLOUR":
		ourObj.colour = nVal;
		okMsg( socket );
		break;
	case "MAXHP":
		ourObj.maxhp = nVal;
		okMsg( socket );
		break;
	case "MANA":
		ourObj.mana = nVal;
		okMsg( socket );
		break;
	case "ORIGIN":
		ourObj.origin = socket.xText.substring( 7 );
		okMsg( socket );
		break;
	case "OWNER":
		socket.tempObj = ourObj;
		socket.CustomTarget( 1, GetDictionaryEntry( 8104, socket.language )); // Choose character to own this object
		break;
	case "PERMANENTMAGICREFLECT":
		ourObj.permanentMagicReflect = ( nVal == 1 );
		okMsg( socket );
		break;
	case "POISON":
		ourObj.poison = nVal;
		okMsg( socket );
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
		ourObj.wipable = ( nVal == 1 );
		okMsg( socket );
		break;
	case "RACE":
		ourObj.race = nVal;
		okMsg( socket );
		break;
	case "INSTANCEID":
		ourObj.instanceID = nVal;
		okMsg( socket );
		break;
	case "WORLDNUMBER":
		ourObj.worldnumber = nVal;
		okMsg( socket );
		break;
	case "DAMAGEABLE":
		ourObj.isDamageable = nVal;
		okMsg( socket );
		break;
	case "SCRIPTTRIGGER":
		// Add a single script trigger to object; overwrites existing list of scripts
		if( parseInt( nVal ) == 0 )
		{
			ourObj.RemoveScriptTrigger( 0 );
			socket.SysMessage( GetDictionaryEntry( 2044, socket.language )); // All script triggers have been cleared from target!
		}
		else
		{
			ourObj.scripttrigger = nVal;
			okMsg( socket );
		}
		break;
	case "SCRIPTTRIGGERS":
		// Add script trigger to list of triggers on objects
		if( parseInt( nVal ) == 0 )
		{
			ourObj.RemoveScriptTrigger( 0 );
			socket.SysMessage( GetDictionaryEntry( 2044, socket.language )); // All script triggers have been cleared from target!
		}
		else
		{
			ourObj.AddScriptTrigger( nVal );
			okMsg( socket );
		}
		break;
	case "SECTIONID":
		ourObj.sectionID = socket.xText.substring( 10 );
		okMsg( socket );
		break;
	case "SHOULDSAVE":
		ourObj.shouldSave = ( nVal == 1 );
		okMsg( socket );
		break;
	default:
		if( ourObj.isChar )
		{
			HandleSetChar( socket, ourObj, uKey, splitString );
		}
		else if( ourObj.isItem )
		{
			HandleSetItem( socket, ourObj, uKey, splitString );
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 8105, socket.language ) + " " + uKey ); // Invalid set command + uKey
		}
		break;
	}
}

// Item-specific properties
function HandleSetItem( socket, ourItem, uKey, splitString )
{
	var nVal = splitString[1];
	nVal = nVal === "true" ? 1 : nVal === "false" ? 0 : parseInt( nVal );

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
		ourItem.Refresh(); // Refresh so movable state updates for nearby clients!
		okMsg( socket );
		break;
	case "BASERANGE":
		ourItem.baseRange = nVal;
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
	case "EVENT":
		ourItem.event = socket.xText.substring( 6 );
		okMsg( socket );
		break;
	case "MAXITEMS":
		ourItem.maxItems = nVal;
		okMsg( socket );
		break;
	case "MAXRANGE":
		ourItem.maxRange = nVal;
		okMsg( socket );
		break;
	case "MORE":
		if( splitString[4] )
		{
			ourItem.more = splitString[1] + " " + splitString[2] + " " + splitString[3] + " " + splitString[4];
		}
		else
		{
			ourItem.more = nVal;
		}
		okMsg( socket );
		break;
	case "MORE0":
		if( splitString[4] )
		{
			ourItem.more0 = splitString[1] + " " + splitString[2] + " " + splitString[3] + " " + splitString[4];
		}
		else
		{
			ourItem.more0 = nVal;
		}
		okMsg( socket );
		break;
	case "MORE1":
		if( splitString[4] )
		{
			ourItem.more1 = splitString[1] + " " + splitString[2] + " " + splitString[3] + " " + splitString[4];
		}
		else
		{
			ourItem.more1 = nVal;
		}
		okMsg( socket );
		break;
	case "MORE2":
		if( splitString[4] )
		{
			ourItem.more2 = splitString[1] + " " + splitString[2] + " " + splitString[3] + " " + splitString[4];
		}
		else
		{
			ourItem.more2 = nVal;
		}
		okMsg( socket );
		break;
	case "MOREX":
		if( splitString[4] )
		{
			ourItem.morex = splitString[1] + " " + splitString[2] + " " + splitString[3] + " " + splitString[4];
		}
		else
		{
			ourItem.morex = nVal;
		}
		okMsg( socket );
		break;
	case "MOREY":
		if( splitString[4] )
		{
			ourItem.morey = splitString[1] + " " + splitString[2] + " " + splitString[3] + " " + splitString[4];
		}
		else
		{
			ourItem.morey = nVal;
		}
		okMsg( socket );
		break;
	case "MOREZ":
		if( splitString[4] )
		{
			ourItem.morez = splitString[1] + " " + splitString[2] + " " + splitString[3] + " " + splitString[4];
		}
		else
		{
			ourItem.morez = nVal;
		}
		okMsg( socket );
		break;
	case "MOREXYZ":
		if( splitString[3] )
		{
			ourItem.morex = parseInt( splitString[1] );
			ourItem.morey = parseInt( splitString[2] );
			ourItem.morez = parseInt( splitString[3] );
			okMsg( socket );
		}
		break;
	case "NEWBIE":
		ourItem.isNewbie = ( nVal == 1 );
		okMsg( socket );
		break;
	case "DIVINELOCK":
		ourItem.divinelock = ( nVal == 1 );
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
				socket.SysMessage( GetDictionaryEntry( 8106, socket.language )); // Only objects added using the 'ADD SPAWNER # command can be assigned spawner item types!
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
		ourItem.decayable = ( nVal == 1 );
		okMsg( socket );
		break;
	case "VISIBLE":
		ourItem.visible = nVal;
		ourItem.Refresh();
		okMsg( socket );
		break;
	case "DESC":
		ourItem.desc = socket.xText.substring( 5 );
		okMsg( socket );
		break;
	case "DEF":
		ourItem.Resist( 1, nVal );
		okMsg( socket );
		break;
	case "ARMORCLASS":
	case "ARMOURCLASS":
	case "AC":
		ourItem.ac( nVal );
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
	case "MAXWEIGHT":
		socket.SysMessage( "Setting value of property: .weightMax" );
	case "WEIGHTMAX":
		ourItem.weightMax = nVal;
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
	case "MAXUSES":
		ourItem.maxUses = nVal;
		okMsg( socket );
		break;
	case "USESLEFT":
		ourItem.usesLeft = nVal;
		okMsg( socket );
		break;
	case "STEALABLE":
		ourItem.stealable = nVal;
		okMsg( socket );
		break;
	default:
		if( ourItem.isSpawner )
		{
			HandleSetSpawner( socket, ourItem, uKey, splitString );
		}
		else
		{
			if( uKey == "SPAWNSECTION" || uKey == "MININTERVAL" || uKey == "MAXINTERVAL" )
			{
				socket.SysMessage( GetDictionaryEntry( 8107, socket.language )); // This can only be set on objects added as spawner objects through the 'ADD SPAWNER # command!
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 8105, socket.language ) + " " + uKey ); // Invalid set command + uKey
			}
		}
		break;
	}
}

// Spawner-specific properties
function HandleSetSpawner( socket, ourSpawn, uKey, splitString )
{
	var nVal = splitString[1];
	nVal = nVal === "true" ? 1 : nVal === "false" ? 0 : parseInt( nVal );

	switch( uKey )
	{
	case "SPAWNSECTION":
		var splitValues = socket.xText.split( " " );
		if( splitValues[2] )
		{
			ourSpawn.spawnsection = splitValues[1];
			ourSpawn.sectionalist = ( splitValues[2] == "TRUE" );
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
		socket.SysMessage( GetDictionaryEntry( 8105, socket.language ) + " " + uKey ); // Invalid set command + uKey
	}
}

// Character-specific properties
function HandleSetChar( socket, ourChar, uKey, splitString )
{
	var nVal = splitString[1];
	nVal = nVal === "true" ? 1 : nVal === "false" ? 0 : parseInt( nVal );

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
	case "CONTROLSLOTS":
		ourChar.controlSlots = nVal;
		okMsg( socket );
		break;
	case "CONTROLSLOTSUSED":
		ourChar.controlSlotsUsed = nVal;
		okMsg( socket );
		break;
	case "DEATHS":
		ourChar.deaths = nVal;
		okMsg( socket );
		break;
	case "LOYALTY":
		ourChar.loyalty = nVal;
		okMsg( socket );
		break;
	case "MAXLOYALTY":
		ourChar.maxLoyalty = nVal;
		okMsg( socket );
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
		ourChar.oldWandertype = nVal;
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
	case "NPCGUILD":
		ourChar.npcGuild = nVal;
		okMsg( socket );
		break;
	case "NPCGUILDJOINED":
		ourChar.npcGuildJoined = nVal;
		okMsg( socket );
		break;
	case "VULNERABLE":
		ourChar.vulnerable = ( nVal == 1 );
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
	case "HAIRSTYLE":
		ourChar.hairStyle = nVal;
		okMsg( socket );
		break;
	case "HAIRCOLOUR":
	case "HAIRCOLOR":
		ourChar.hairColour = nVal;
		okMsg( socket );
		break;
	case "BEARDSTYLE":
		ourChar.beardStyle = nVal;
		okMsg( socket );
		break;
	case "BEARDCOLOUR":
	case "BEARDCOLOR":
		ourChar.beardColour = nVal;
		okMsg( socket );
		break;
	case "Z":
		ourChar.z = nVal;
		okMsg( socket );
		break;
	case "FX1":
		ourChar.fx1 = nVal;
		okMsg( socket );
		break;
	case "FY1":
		ourChar.fy1 = nVal;
		okMsg( socket );
		break;
	case "FX2":
		ourChar.fx2 = nVal;
		okMsg( socket );
		break;
	case "FY2":
		ourChar.fy2 = nVal;
		okMsg( socket );
		break;
	case "FZ":
		ourChar.fz = nVal;
		okMsg( socket );
		break;
	case "CANTRAIN":
		if( ourChar.npc )
		{
			ourChar.trainer = ( nVal == 1 );
			okMsg( socket );
		}
		break;
	case "FROZEN":
		ourChar.frozen = ( nVal == 1 );
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
	case "HUNGER":
		ourChar.hunger = nVal;
		okMsg( socket );
		break;
	case "AWAKE":
		ourChar.isAwake = nVal;
		okMsg( socket );
		break;
	case "LANGUAGE":
		if( !ourChar.npc && ourChar.online )
		{
			ourChar.socket.language = nVal;
			okMsg ( socket );
		}
		break;
	case "THIRST":
		ourChar.thirst = nVal;
		okMsg( socket );
		break;
	// Account Properties
	case "USERNAME":
	case "CURRENTCHAR":
	case "ISONLINE":
	case "CHARACTER1":
	case "CHARACTER2":
	case "CHARACTER3":
	case "CHARACTER4":
	case "CHARACTER5":
	case "CHARACTER6":
	case "CHARACTER7":
	case "LASTIP":
		socket.SysMessage( GetDictionaryEntry( 8108, socket.language )); // This is a read-only property.
		break;
	case "PASSWORD":
		{
			if( !ourChar.npc )
			{
				var newPass = socket.xText.substring( 9 );
				if( newPass.length > 3 )
				{
					var myAccount = ourChar.account;
					myAccount.password = newPass;
   					Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET PASSWORD> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
					okMsg( socket );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 8109, socket.language ) + " " + newPass.length ); // Password must be longer than 3 characters! Current length:
				}
			}
			break;
		}
	case "COMMENT":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.comment = socket.xText.substring( 8 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET COMMENT> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISBANNED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isBanned = ( nVal == 1 );
			if( nVal == 1 )
			{
				// Also set a default timeban of 24 hours (60 * 24), and disconnect user, if online
				myAccount.timeban = 60 * 24;
				if( myAccount.isOnline && ValidateObject( myAccount.currentChar ) && myAccount.currentChar.socket != null )
				{
					myAccount.currentChar.Disconnect();
				}
			}
			else
			{
				// Remove timeban as well
				myAccount.timeban = 0;
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISBANNED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSUSPENDED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSuspended = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSUSPENDED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISPUBLIC":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isPublic = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISPUBLIC " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSLOT1BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSlot1Blocked = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSLOT1BLOCKED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSLOT2BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSlot2Blocked = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSLOT2BLOCKED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSLOT3BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSlot3Blocked = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSLOT3BLOCKED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSLOT4BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSlot4Blocked = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSLOT4BLOCKED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSLOT5BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSlot5Blocked = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSLOT5BLOCKED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSLOT6BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSlot6Blocked = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSLOT6BLOCKED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSLOT7BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSlot7Blocked = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSLOT6BLOCKED " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISYOUNG":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isYoung = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISYOUNG " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "UNUSED10":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.unused10 = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET UNUSED10 " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISSEER":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isSeer = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISSEER " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISCOUNSELOR":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isCounselor = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISCOUNSELOR " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "ISGM":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.isGM = ( nVal == 1 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <SET ISGM " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	case "TIMEBAN":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			myAccount.timeban = nVal;
			if( nVal > 0 )
			{
				// Ban player if timeban is over zero
				myAccount.isBanned = 1;
				if( myAccount.isOnline && ValidateObject( myAccount.currentChar ) && myAccount.currentChar.socket != null )
				{
					// Disconnect currently logged in character on account, if they're online
					myAccount.currentChar.socket.Disconnect();
				}
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") useD command <SET TIMEBAN " + nVal + "> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
			okMsg( socket );
		}
		break;
	default:
		if( ourChar.SetSkillByName( uKey, nVal ))
		{
			okMsg( socket );
		}
		else if( HandleSetSocket( socket, uKey, splitString ))
		{
			okMsg( socket );
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 8105, socket.language ) + " " + uKey ); // Invalid set command + uKey
		}
		break;
	}
}

function HandleSetSocket( socket, uKey, splitString )
{
	var nVal = splitString[1];
	nVal = nVal === "true" ? 1 : nVal === "false" ? 0 : parseInt( nVal );

	switch( uKey )
	{
		case "TEMPINT":
			socket.tempInt = parseInt( nVal );
			break;
		case "TEMPINT2":
			socket.tempInt2 = parseInt( nVal );
			break;
		case "XTEXT":
			socket.xText = nVal;
			break;
		case "CLICKX":
			socket.clickX = parseInt( nVal );
			break;
		case "CLICKY":
			socket.clickY = parseInt( nVal );
			break;
		case "CLICKZ":
			socket.clickZ = parseInt( nVal );
			break;
		case "WALKSEQUENCE":
			socket.walkSequence = parseInt( nVal );
			break;
		case "CURRENTSPELLTYPE":
			socket.currentSpellType = parseInt( nVal );
			break;
		case "LOGGING":
			socket.logging = parseInt( nVal );
			break;
		case "PICKUPX":
			socket.pickupX = parseInt( nVal );
			break;
		case "PICKUPY":
			socket.pickupY = parseInt( nVal );
			break;
		case "PICKUPZ":
			socket.pickupZ = parseInt( nVal );
			break;
		case "PICKUPSPOT":
			socket.pickupSpot = parseInt( nVal );
			break;
		default:
			socket.SysMessage( GetDictionaryEntry( 8105, socket.language ) + " " + uKey ); // Invalid set command + uKey
			return false;
	}

	return true;
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
		var targMsg = GetDictionaryEntry( 240, socket.language ); // Select creature to set POISONED.
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
{
	// Sends verification to the player that the specified value was successfully set.
	socket.SysMessage( GetDictionaryEntry( 1756, socket.language )); // Value successfully set.
}

function _restorecontext_() {}
