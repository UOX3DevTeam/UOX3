// These commands show item or character properties for targeted objects
function CommandRegistration()
{
	RegisterCommand( "istats", 1, true );
	RegisterCommand( "cstats", 1, true );
}

function command_ISTATS( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 182, socket.language ); // Select item to inspect.
	socket.CustomTarget( 0, targMsg );
}

// Known needed entries:
// Privs
// Carve
// Multi
// Creator
// Madewith
// Decaytime
// Good
// Random Value Rate
function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj && ourObj.isItem )
	{
		position 	= 40;
		var myGump 	= new Gump;
		myGump.AddBackground( 0, 0, 300, 350, 2600 );
		myGump.AddButton( 260, 15, 4017, 1, 0, 1 );
		myGump.AddText( 45, 15, 0, GetDictionaryEntry( 8083, socket.language )); // Dynamic Item Stats
		myGump.AddPage( 1 );
		addHexEntry( myGump, "Serial:", ourObj.serial );
		addHexEntry( myGump, "ID:", ourObj.id );
		addStringEntry( myGump, "Name:", ourObj.name );
		addStringEntry( myGump, "Name2:", ourObj.name2 );
		addHexEntry( myGump, "Colour:", ourObj.colour );
		var contObj = ourObj.container;
		if( contObj )
		{
			addHexEntry( myGump, "Container:", contObj.serial );
		}
		else
		{
			addStringEntry( myGump, "Container:", "0xffffffff" );
		}
		addHexEntry( myGump, "Layer:", ourObj.layer );
		addEntry( myGump, "Type:", ourObj.type );
		addEntry( myGump, "Movable:", ourObj.movable );
		addHexEntry( myGump, "More", ourObj.more );
		myGump.AddPageButton( 260, 260, 4005, 2 );
		myGump.AddPage( 2 );
		position = 40;
		addEntry( myGump, "X coord:", ourObj.x );
		addEntry( myGump, "Y coord:", ourObj.y );
		addEntry( myGump, "Z coord:", ourObj.z );
		addEntry( myGump, "Amount:", ourObj.amount );
		var ownerObj = ourObj.owner;
		if( ownerObj )
		{
			addHexEntry( myGump, "Owner:", ownerObj.serial );
		}
		else
		{
			addStringEntry( myGump, "Owner:", "0xffffffff" );
		}
		addEntry( myGump, "Strength:", ourObj.strength );
		var hpmax = ourObj.health.toString() + " " + ourObj.maxhp.toString();
		addStringEntry( myGump, "HP/Max:", hpmax );
		var damage = ourObj.lodamage.toString() + " " + ourObj.hidamage.toString();
		addStringEntry( myGump, "Damage:", damage );
		addEntry( myGump, "Defense:", ourObj.Resist( 1 ) );
		addEntry( myGump, "Rank:", ourObj.rank );
		myGump.AddPageButton( 10, 260, 4014, 1 );
		myGump.AddPageButton( 260, 260, 4005, 3 );
		myGump.AddPage( 3 );
		position = 40;
		addHexEntry( myGump, "More X:", ourObj.morex );
		addHexEntry( myGump, "More Y:", ourObj.morey );
		addHexEntry( myGump, "More Z:", ourObj.morez );
		addEntry( myGump, "Poisoned:", ourObj.poison );
		addEntry( myGump, "Weight:", ourObj.weight );
		addEntry( myGump, "BaseWeight:", ourObj.baseWeight );
		addEntry( myGump, "Decay:", ourObj.decayable ? 1 : 0 );
		addEntry( myGump, "Buy Value:", ourObj.buyvalue );
		addEntry( myGump, "Sell Value:", ourObj.sellvalue );

		addEntry( myGump, "Is Corpse:", ourObj.corpse ? 1 : 0 );
		var scriptTriggers = ourObj.scriptTriggers;
		for( var i = 0; i < scriptTriggers.length; i++ )
		{
			addEntry( myGump, "Script ID:", scriptTriggers[i] );
		}
		addEntry( myGump, "Direction:", ourObj.dir );
		if( ourObj.isSpawner )
		{
			if( ourObj.spawnsection )
			{
				addStringEntry( myGump, "SpawnSection:", ourObj.spawnsection );
			}
			else
			{
				addStringEntry( myGump, "SpawnSection:", "0" );
			}
			addEntry( myGump, "IsASpawnList:", ourObj.sectionalist ? 1 : 0 );
			addEntry( myGump, "Min Spawn Time:", ourObj.mininterval );
			addEntry( myGump, "Max Spawn Time:", ourObj.maxinterval );
		}
		myGump.AddPageButton( 10, 260, 4014, 2 );
		myGump.Send( socket );
		myGump.Free;
	}
	else
	{
		SendStaticStats( socket );
	}
}

function command_CSTATS( socket, cmdString )
{
	var target = CalcCharFromSer( socket.GetDWord( 7 ));
	if( ValidateObject( target ))
	{
		onCallback1( socket, target );
	}
	else
	{
		var targMsg = GetDictionaryEntry( 183, socket.language ); // Select char to inspect.
		socket.CustomTarget( 1, targMsg );
	}
}

// Known needed entries:
// Account
// Privs
// PoisonStrength
// Carve
// RaceGate
// LastOn
// Multi
// Region
function onCallback1( socket, ourObj )
{
	// if( !socket.GetWord( 1 ) && ourObj && ourObj.isChar )
	if( ValidateObject( ourObj ) && ourObj.isChar)
	{
		position 	= 40;
		var myGump 	= new Gump;
		myGump.AddBackground( 0, 0, 300, 300, 2600 );
		myGump.AddButton( 260, 15, 4017, 1, 0, 1 );
		myGump.AddText( 45, 15, 0, GetDictionaryEntry( 8084, socket.language )); // Character Stats
		myGump.AddPage( 1 );
		addHexEntry( myGump, "Serial:", ourObj.serial );
		addHexEntry( myGump, "Body Type:", ourObj.id );
		addStringEntry( myGump, "Name:", ourObj.name );
		addHexEntry( myGump, "Skin:", ourObj.colour );
		addEntry( myGump, "Strength:", ourObj.strength );
		addEntry( myGump, "Dexterity:", ourObj.dexterity );
		addEntry( myGump, "Intelligence:", ourObj.intelligence );
		addEntry( myGump, "Mana:", ourObj.mana );
		addEntry( myGump, "Hitpoints:", ourObj.health );
		addEntry( myGump, "Stamina:", ourObj.stamina );
		myGump.AddPageButton( 260, 260, 4005, 2 );
		myGump.AddPage( 2 );
		position = 40;
		addEntry( myGump, "X coord:", ourObj.x );
		addEntry( myGump, "Y coord:", ourObj.y );
		addEntry( myGump, "Z coord:", ourObj.z );
		addEntry( myGump, "Fame:", ourObj.fame );
		addEntry( myGump, "Karma:", ourObj.karma );
		addEntry( myGump, "Deaths:", ourObj.deaths );
		addEntry( myGump, "Kills:", ourObj.murdercount );
		addEntry( myGump, "NPC Wander:", ourObj.wandertype );
		addEntry( myGump, "NPC AI:", ourObj.aitype );
		addEntry( myGump, "Weight:", ourObj.weight );
		addEntry( myGump, "Poisoned:", ourObj.poison );
		myGump.AddPageButton( 10, 260, 4014, 1 );
		myGump.AddPageButton( 260, 260, 4005, 3 );
		myGump.AddPage( 3 );
		position = 40;
		addEntry( myGump, "Hunger:", ourObj.hunger );
		var attChar = ourObj.attacker;
		var targChar = ourObj.target;
		if( attChar )
		{
			addHexEntry( myGump, "Attacker:", attChar.serial );
		}
		if( targChar )
		{
			addHexEntry( myGump, "Target:", targChar.serial );
		}
		var ourRace = ourObj.race;
		if( ourRace )
		{
			addEntry( myGump, "Race:", ourRace.id );
		}
		else
		{
			addEntry( myGump, "Race:", 0 );
		}
		addEntry( myGump, "CommandLevel:", ourObj.commandlevel );
		var scriptTriggers = ourObj.scriptTriggers;
		for( var i = 0; i < scriptTriggers.length; i++ )
		{
			addEntry( myGump, "Script ID:", scriptTriggers[i] );
		}
		addEntry( myGump, "Direction:", ourObj.direction );
		myGump.AddPageButton( 10, 260, 4014, 2 );
		myGump.Send( socket );
		myGump.Free;
	}
}

function onGumpPress( socket, button, myGump )
{
	switch( button )
	{
	case 1:			break;
	default:		break;
	}
}

function addHexEntry( myGump, stringToAdd, dataToAdd )
{
	var hexString = NumToHexString( dataToAdd );

	if( dataToAdd > 0 && dataToAdd <= 9999 )
	{
		hexString = ( "000" + hexString ).slice( -4 );
	}

	myGump.AddText( 50, position, 0, stringToAdd );
	myGump.AddText( 150, position, 0, "0x" + hexString );
	position += 20;
}

function addEntry( myGump, stringToAdd, dataToAdd )
{
	myGump.AddText( 50, position, 0, stringToAdd );
	myGump.AddText( 150, position, 0, dataToAdd.toString() );
	position += 20;
}

function addStringEntry( myGump, stringToAdd, dataToAdd )
{
	myGump.AddText( 50, position, 0, stringToAdd );
	myGump.AddText( 150, position, 0, dataToAdd );
	position += 20;
}