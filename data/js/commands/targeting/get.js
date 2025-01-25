// This command allows GM to fetch

function CommandRegistration()
{
	RegisterCommand( "get", 2, true );
}

function command_GET( socket, cmdString )
{
	if( cmdString )
	{
		socket.xText = cmdString;
		var tempMsg = GetDictionaryEntry( 8079, socket.language ); // Choose target to get %s property for:
		socket.CustomTarget( 0, tempMsg.replace( /%s/gi, cmdString ));
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 8080, socket.language )); // No property was specified for the GET command.
	}
}

// Common Object Properties
function onCallback0( socket, ourObj )
{
	if( socket.GetWord( 1 ) && !ValidateObject( ourObj ))
	{
		socket.SysMessage( "'Get': " + GetDictionaryEntry( 2353, socket.language )); // Invalid target.
		return;
	}

	var splitString = socket.xText.split( " ", 1 );
	var uKey = splitString[0].toUpperCase();
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
	case "ISAGGRESSOR":
		socket.SysMessage( )
		break;
	case "HUE":
	case "SKIN":
	case "COLOR":
	case "COLOUR":
		socket.SysMessage( "0x" + ( ourObj.colour ).toString( 16 ) + " ( " + ourObj.colour + ")" );
		break;
	case "ORGHUE":
	case "ORGSKIN":
	case "ORGCOLOR":
	case "ORGCOLOUR":
		socket.SysMessage( "0x" + ( ourObj.orgSkin ).toString( 16 ) + " ( " + ourObj.orgSkin + ")" );
		break;
	case "MAXHP":
		socket.SysMessage( ourObj.maxhp );
		break;
	case "MANA":
		socket.SysMessage( ourObj.mana );
		break;
	case "MULTI":
		socket.SysMessage( ourObj.multi );
		break;
	case "OWNER":
		if( ourObj.owner )
		{
			socket.SysMessage( ourObj.owner.serial + " - " + ourObj.owner.name );
		}
		else
		{
			socket.SysMessage( "null" );
		}
		break;
	case "ORIGIN":
		socket.SysMessage( ourObj.origin );
		break;
	case "PERMANENTMAGICREFLECT":
		socket.SysMessage( ourObj.permanentMagicReflect );
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
		{
			socket.SysMessage( ourObj.race.id + " - " + ourObj.race.name );
		}
		else
		{
			socket.SysMessage( "null" );
		}
		break;
	case "INSTANCEID":
		socket.SysMessage( ourObj.instanceID );
		break;
	case "WORLDNUMBER":
		socket.SysMessage( ourObj.worldnumber );
		break;
	case "DAMAGEABLE":
		socket.SysMessage( ourObj.isDamageable );
		break;
	case "REGION":
		socket.SysMessage( ourObj.region.id + " ( " + ourObj.region.name + ")" );
		break;
	case "SCRIPTTRIGGER":
	{
		// Get last script trigger in list of triggers on object
		var scriptTriggers = ourObj.scriptTriggers;
		var size = scriptTriggers.length;
		socket.SysMessage( scriptTriggers[size-1] );
		break;
	}
	case "SCRIPTTRIGGERS":
	{
		// Get list of script triggers from object
		var scriptTriggers = ourObj.scriptTriggers;
		var size = scriptTriggers.length;
		for( var i = 0; i < size; i++ )
		{
			socket.SysMessage( scriptTriggers[i] );
		}
		break;
	}
	case "SECTIONID":
		socket.SysMessage( ourObj.sectionID );
		break;
	case "SHOULDSAVE":
		socket.SysMessage( ourObj.shouldSave );
		break;
	default:
		if( ourObj.isChar )
		{
			HandleGetChar( socket, ourObj, uKey );
		}
		else if( ourObj.isItem )
		{
			HandleGetItem( socket, ourObj, uKey );
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 8081, socket.language ) + " " + uKey ); // Invalid property provided for get command:
		}
		break;
	}
}

// Item-specific Properties
function HandleGetItem( socket, ourItem, uKey )
{
	switch( uKey )
	{
	case "ID":
		var ourID = ( ourItem.id ).toString( 16 );
		while( ourID.length < 4 )
		{
			ourID = "0" + ourID;
		}
		ourID = "0x" + ourID;
		socket.SysMessage( ourID + " ( " + ourItem.id + ")" );
		break;
	case "AMOUNT":
		socket.SysMessage( ourItem.amount );
		break;
	case "MOVEABLE":
	case "MOVABLE":
		socket.SysMessage( ourItem.movable );
		break;
	case "BASERANGE":
		socket.SysMessage( ourItem.baseRange );
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
	case "EVENT":
		socket.SysMessage( ourItem.event );
		break;
	case "MAXITEMS":
		socket.SysMessage( ourItem.maxItems );
		break;
	case "MAXRANGE":
		socket.SysMessage( ourItem.maxRange );
		break;
	case "MORE":
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.more ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.more + " ( " + hexVal + ")" );
		break;
	case "MORE0":
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.more ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.more0 + " ( " + hexVal + ")" );
		break;
	case "MORE1":
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.more ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.more1 + " ( " + hexVal + ")" );
		break;
	case "MORE2":
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.more ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.more2 + " ( " + hexVal + ")" );
		break;
	case "MOREX":
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.morex ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.morex + " ( " + hexVal + ")" );
		break;
	case "MOREY":
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.morey ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.morey + " ( " + hexVal + ")" );
		break;
	case "MOREZ":
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.morez ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.morez + " ( " + hexVal + ")" );
		break;
	case "MOREXYZ":
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.morex ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.morex + " ( " + hexVal + ")" );
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.morey ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.morey + " ( " + hexVal + ")" );
		var hexVal = "0x" + ( "00000000" + ( Number( ourItem.morez ).toString( 16 ))).slice( -8 )
		socket.SysMessage( ourItem.morez + " ( " + hexVal + ")" );
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
	case "RESISTARMOR":
		socket.SysMessage( ourItem.Resist( 1 ));
		break;
	case "RESISTLIGHT":
		socket.SysMessage( ourItem.Resist( 2 ));
		break;
	case "RESISTWATER":
		socket.SysMessage( ourItem.Resist( 3 ));
		break;
	case "RESISTCOLD":
		socket.SysMessage( ourItem.Resist( 4 ));
		break;
	case "RESISTFIRE":
		socket.SysMessage( ourItem.Resist( 5 ));
		break;
	case "RESISTENERGY":
		socket.SysMessage( ourItem.Resist( 6 ));
		break;
	case "RESISTPOISON":
		socket.SysMessage( ourItem.Resist( 7 ));
		break;
	case "ARMORCLASS":
	case "ARMOURCLASS":
	case "AC":
		socket.SysMessage( ourItem.ac );
		break;
	case "LAYER":
		socket.SysMessage( "0x" + ( ourItem.layer ).toString( 16 ) + "( " + ourItem.layer + ")" );
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
	case "MAXWEIGHT":
		socket.SysMessage( "Fetching value of property: .weightMax" );
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
		{
			socket.SysMessage( ourItem.race.id + " - " + ourItem.race.name );
		}
		else
		{
			socket.SysMessage( "null" );
		}
		break;
	case "MAXUSES":
		socket.SysMessage( ourItem.maxUses );
		break;
	case "USESLEFT":
		socket.SysMessage( ourItem.usesLeft );
		break;
	case "STEALABLE":
		socket.SysMessage( ourItem.stealable );
		break;
	default:
		if( ourItem.isSpawner )
		{
			HandleGetSpawner( socket, ourItem, uKey );
		}
		else
		{
			if( uKey == "SPAWNSECTION" || uKey == "MININTERVAL" || uKey == "MAXINTERVAL" )
			{
				socket.SysMessage( GetDictionaryEntry( 8082, socket.language )); // These properties are only found on objects added as spawner objects via the 'ADD SPAWNER # command!
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 8081, socket.language ) + " " + uKey ); // Invalid property provided for get command:
			}
		}
		break;
	}
}

// Spawner-specific properties
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
		socket.SysMessage( GetDictionaryEntry( 8081, socket.language ) + " " + uKey ); // Invalid property provided for get command:
	}
}

// Character-specific properties
function HandleGetChar( socket, ourChar, uKey )
{
	switch( uKey )
	{
	case "ID":
		socket.SysMessage( "0x" + ( ourChar.id ).toString( 16 ) + " ( " + ourChar.id + ")" );
		break;
	case "ORGID":
		socket.SysMessage( "0x" + ( ourChar.orgID ).toString( 16 ) + " ( " + ourChar.orgID + ")" );
		break;
	case "ACTUALSTR":
	case "ACTUALSTRENGTH":
		socket.SysMessage( ourChar.actualStrength );
		break;
	case "ACTUALDEX":
	case "ACTUALDEXTERITY":
		socket.SysMessage( ourChar.actualDexterity );
		break;
	case "ACTUALINT":
	case "ACTUALINTELLIGENCE":
		socket.SysMessage( ourChar.actualIntelligence );
		break;
	case "CONTROLSLOTS":
		socket.SysMessage( ourChar.controlSlots );
		break;
	case "CONTROLSLOTSUSED":
		socket.SysMessage( ourChar.controlSlotsUsed );
		break;
	case "CRIMINAL":
		socket.SysMessage( ourChar.criminal );
		break;
	case "INNOCENT":
		socket.SysMessage( ourChar.innocent );
		break;
	case "MURDERER":
		socket.SysMessage( ourChar.murderer );
		break;
	case "PERMAGREY":
		socket.SysMessage( ourChar.IsPermaGrey() );
		break;
	case "HASSTOLEN":
		socket.SysMessage( ourChar.hasStolen );
		break;
	case "AGGRESSOR":
		socket.SysMessage( ourChar.IsAggressor() );
		break;
	case "PETCOUNT":
		socket.SysMessage( ourChar.petCount );
		break;
	case "FOLLOWERCOUNT":
		socket.SysMessage( ourChar.followerCount );
		break;
	case "MAXLOYALTY":
		socket.SysMessage( ourChar.maxLoyalty );
		break;
	case "LOYALTY":
		socket.SysMessage( ourChar.loyalty );
		break;
	case "LOYALTYRATE":
		socket.SysMessage( ourChar.loyaltyRate );
		break;
	case "DEATHS":
		socket.SysMessage( ourChar.deaths );
		break;
	case "OWNERCOUNT":
		socket.SysMessage( ourChar.ownerCount );
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
		socket.SysMessage( ourChar.oldWandertype );
		break;
	case "NPCGUILD":
		socket.SysMessage( ourChar.npcGuild );
		break;
	case "NPCGUILDJOINED":
		if( !ourChar.npc )
		{
			var npcGuildJoined = ourChar.npcGuildJoined;
			if( npcGuildJoined > 0 )
			{
				// Output raw value of timestamp
				socket.SysMessage( npcGuildJoined );

				// Calculate date of timestamp, and output as a formatted date
				var joinedDate = new Date( npcGuildJoined * 60000 );
				var options = { year: 'numeric', month: 'numeric', day: 'numeric', hour: 'numeric', minute: 'numeric', second: 'numeric' };
				var formattedDateTime = joinedDate.toLocaleString( undefined, options );
				socket.SysMessage( "Character joined guild on " + formattedDateTime );

				// Calculate the duration in days, hours, and minutes, and output
				var durationInMinutes = Math.floor( Date.now() / ( 60 * 1000 )) - npcGuildJoined;
				var days = Math.floor(durationInMinutes / (24 * 60));
				var hours = Math.floor((durationInMinutes % (24 * 60)) / 60);
				var minutes = durationInMinutes % 60;
				socket.SysMessage( "(" + days + " days, " + hours + " hours, " + minutes + " minutes ago)" );
			}
			else
			{
				socket.SysMessage( "Character has not joined an NPC Guild yet." );
			}
		}
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
	case "HAIRSTYLE":
		socket.SysMessage( ourChar.hairStyle );
		break;
	case "HAIRCOLOUR":
	case "HAIRCOLOR":
		socket.SysMessage( ourChar.hairColour );
		break;
	case "BEARDSTYLE":
		socket.SysMessage( ourChar.beardStyle );
		break;
	case "BEARDCOLOUR":
	case "BEARDCOLOR":
		socket.SysMessage( ourChar.beardColour );
		break;
	case "Z":
		socket.SysMessage( ourChar.z );
		break;
	case "FX1":
		socket.SysMessage( ourChar.fx1 );
		break;
	case "FY1":
		socket.SysMessage( ourChar.fy1 );
		break;
	case "FX2":
		socket.SysMessage( ourChar.fx2 );
		break;
	case "FY2":
		socket.SysMessage( ourChar.fy2 );
		break;
	case "FZ":
		socket.SysMessage( ourChar.fz );
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
	case "RESISTARMOR":
		socket.SysMessage( ourChar.Resist( 1 ));
		break;
	case "RESISTLIGHT":
		socket.SysMessage( ourChar.Resist( 2 ));
		break;
	case "RESISTWATER":
		socket.SysMessage( ourChar.Resist( 3 ));
		break;
	case "RESISTCOLD":
		socket.SysMessage( ourChar.Resist( 4 ));
		break;
	case "RESISTFIRE":
		socket.SysMessage( ourChar.Resist( 5 ));
		break;
	case "RESISTENERGY":
		socket.SysMessage( ourChar.Resist( 6 ));
		break;
	case "RESISTPOISON":
		socket.SysMessage( ourChar.Resist( 7 ));
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
	case "HUNGERRATE":
		socket.SysMessage( ourChar.hungerRate );
		break;
	case "AWAKE":
		socket.SysMessage( ourChar.isAwake );
		break;
	case "LANGUAGE":
		if( !ourChar.npc );
		{
			socket.SysMessage( ourChar.socket.language );
		}
		break;
	case "THIRST":
		socket.SysMessage( ourChar.thirst );
		break;
	case "THIRSTRATE":
		socket.SysMessage( ourChar.thirstRate );
		break;
	case "TOWNPRIV":
		socket.SysMessage( ourChar.townPriv );
		break;
	case "CREATEDON":
		if( !ourChar.npc )
		{
			if( ourChar.createdOn > 0 )
			{
				socket.SysMessage( ourChar.createdOn );
				var createdDate = new Date( ourChar.createdOn * 60 * 1000 );
				socket.SysMessage( "Character was created on " + createdDate );

				var currentDate = new Date();

				// Calculate the time difference between character creation and now
				var timeDifference = currentDate.getTime() - createdDate.getTime();

				// Calculate the amount of days
				var daysDifference = timeDifference / ( 1000 * 3600 * 24 );

				// Display final number of days
				socket.SysMessage( "Character is " + daysDifference.toFixed( 2 ) + " days old." );
			}
			else
			{
				socket.SysMessage( "Character was created at an unknown date." );
			}
		}
		break;

	// Account Properties
	case "USERNAME":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "Username: " + myAccount.username );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET USERNAME> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "FLAGS":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.sysMessage( "Flags: " + myAccount.flags );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET FLAGS> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "CURRENTCHAR":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var currentChar = myAccount.currentChar;
			if( ValidateObject( currentChar ))
			{
				socket.SysMessage( "currentChar: " + myAccount.currentChar.name );
			}
			else
			{
				socket.SysMessage( "Invalid character" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET CURRENTCHAR> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "COMMENT":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "Comment: " + myAccount.comment );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET COMMENT> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISBANNED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isBanned: " + myAccount.isBanned );
			if( myAccount.isBanned && myAccount.timeban == 0 )
			{
				socket.SysMessage( "User is permanently banned." );
			}
			else if( myAccount.isBanned && myAccount.timeban > 0 )
			{
				var bannedUntil = new Date( myAccount.timeban * 60 * 1000 )
				socket.SysMessage( "User is banned until " + bannedUntil );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISBANNED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSUSPENDED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSuspended: " + myAccount.suspended );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSUSPENDED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISPUBLIC":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isPublic: " + myAccount.public );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISPUBLIC> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISONLINE":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isOnline: " + myAccount.isOnline );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISONLINE> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSLOT1BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSlot1Blocked: " + myAccount.isSlot1Blocked );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSLOT1BLOCKED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSLOT2BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSlot2Blocked: " + myAccount.isSlot2Blocked );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSLOT2BLOCKED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSLOT3BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSlot3Blocked: " + myAccount.isSlot3Blocked );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSLOT3BLOCKED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSLOT4BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSlot4Blocked: " + myAccount.isSlot4Blocked );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSLOT4BLOCKED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSLOT5BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSlot5Blocked: " + myAccount.isSlot5Blocked );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSLOT5BLOCKED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSLOT6BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSlot6Blocked: " + myAccount.isSlot6Blocked );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSLOT6BLOCKED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSLOT7BLOCKED":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSlot7Blocked: " + myAccount.isSlot7Blocked );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSLOT7BLOCKED> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISYOUNG":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isYoung: " + myAccount.isYoung );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISYOUNG> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "UNUSED10":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "unused10: " + myAccount.unused10 );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET UNUSED10> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISSEER":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isSeer: " + myAccount.isSeer );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISSEER> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISCOUNSELOR":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isCounselor: " + myAccount.isCounselor );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISCOUNSELOR> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "ISGM":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "isGM: " + myAccount.isGM );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET ISGM> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "CHARACTER1":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var accountChar = myAccount.character1;
			if( ValidateObject( accountChar ))
			{
				socket.SysMessage( "character1: " + accountChar.name );
			}
			else
			{
				socket.SysMessage( "No character in slot 1!" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET CHARACTER1> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "CHARACTER2":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var accountChar = myAccount.character2;
			if( ValidateObject( accountChar ))
			{
				socket.SysMessage( "character2: " + accountChar.name );
			}
			else
			{
				socket.SysMessage( "No character in slot 2!" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET CHARACTER2> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "CHARACTER3":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var accountChar = myAccount.character3;
			if( ValidateObject( accountChar ))
			{
				socket.SysMessage( "character3: " + accountChar.name );
			}
			else
			{
				socket.SysMessage( "No character in slot 3!" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET CHARACTER3> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "CHARACTER4":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var accountChar = myAccount.character4;
			if( ValidateObject( accountChar ))
			{
				socket.SysMessage( "character4: " + accountChar.name );
			}
			else
			{
				socket.SysMessage( "No character in slot 4!" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET CHARACTER4> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "CHARACTER5":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var accountChar = myAccount.character5;
			if( ValidateObject( accountChar ))
			{
				socket.SysMessage( "character5: " + accountChar.name );
			}
			else
			{
				socket.SysMessage( "No character in slot 5!" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET CHARACTER5> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "CHARACTER6":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var accountChar = myAccount.character6;
			if( ValidateObject( accountChar ))
			{
				socket.SysMessage( "character6: " + accountChar.name );
			}
			else
			{
				socket.SysMessage( "No character in slot 6!" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET CHARACTER6> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "CHARACTER7":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var accountChar = myAccount.character7;
			if( ValidateObject( accountChar ))
			{
				socket.SysMessage( "character7: " + accountChar.name );
			}
			else
			{
				socket.SysMessage( "No character in slot 7!" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET CHARACTER7> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "LASTIP":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			socket.SysMessage( "Last IP: " + myAccount.lastIP );
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET LASTIP> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "TIMEBAN":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var timeban = myAccount.timeban;
			var currentTime = new Date();
			var currentTimeMin = Math.ceil( currentTime.getTime() / 1000 / 60 );
			var timebanDelta = timeban - currentTimeMin;
			if( timebanDelta > 0 )
			{
				socket.SysMessage( "Timeban: " + timebanDelta + " minutes" );
				var bannedUntil = new Date( timeban * 60 * 1000 )
				socket.SysMessage( "User is banned until " + bannedUntil );
			}
			else
			{
				socket.SysMessage( "Timeban: 0 minutes" );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET TIMEBAN> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	case "FIRSTLOGIN":
		if( !ourChar.npc )
		{
			var myAccount = ourChar.account;
			var firstLogin = myAccount.firstLogin;
			if( firstLogin > 0 )
			{
				var firstLoginDate = new Date( firstLogin * 60 * 1000 );
				socket.SysMessage( "Account saw it's first login on " + firstLoginDate );

				var currentDate = new Date();

				// Calculate the time difference between first login and now
				var timeDifference = currentDate.getTime() - firstLoginDate.getTime();

				// Calculate the amount of days
				var daysDifference = timeDifference / (1000 * 3600 * 24);

				// Display final number of days
				socket.SysMessage( "Account is " + daysDifference.toFixed( 2 ) + " days old." );
			}
			else
			{
				socket.SysMessage( "Account has had no login activity yet." );
			}
			Console.Log( socket.currentChar.name + " (serial: " + socket.currentChar.serial + ") used command <GET FIRSTLOGIN> on account #" + myAccount.id + ". Extra Info: Cleared", "command.log" );
		}
		break;
	default:
		if( !HandleGetSocket( socket, uKey ))
		{
			socket.SysMessage( GetDictionaryEntry( 8081, socket.language ) + " " + uKey ); // Invalid property provided for get command:
		}
		break;
	}
}

function HandleGetSocket( socket, uKey )
{
	switch( uKey )
	{
		case "TEMPINT":
			socket.SysMessage( socket.tempInt );
			break;
		case "TEMPINT2":
			socket.SysMessage( socket.tempInt2 );
			break;
		case "XTEXT":
			socket.SysMessage( socket.xText );
			break;
		case "CLICKX":
			socket.SysMessage( socket.clickX );
			break;
		case "CLICKY":
			socket.SysMessage( socket.clickY );
			break;
		case "CLICKZ":
			socket.SysMessage( socket.clickZ );
			break;
		case "WALKSEQUENCE":
			socket.SysMessage( socket.walkSequence );
			break;
		case "CURRENTSPELLTYPE":
			socket.SysMessage( socket.currentSpellType );
			break;
		case "LOGGING":
			socket.SysMessage( socket.logging );
			break;
		case "PICKUPX":
			socket.SysMessage( socket.pickupX );
			break;
		case "PICKUPY":
			socket.SysMessage( socket.pickupY );
			break;
		case "PICKUPZ":
			socket.SysMessage( socket.pickupZ );
			break;
		case "PICKUPSPOT":
			socket.SysMessage( socket.pickupSpot );
			break;
		default:
			socket.SysMessage( GetDictionaryEntry( 8081, socket.language ) + " " + uKey ); // Invalid property provided for get command:
			break;
	}

	return true;
}