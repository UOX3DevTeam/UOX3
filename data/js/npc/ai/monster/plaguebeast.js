/** @type { ( objMade: BaseObject, objType: number ) => void } */
function onCreateDFN( objMade, objType )
{
	if( objType == 1 )
		objMade.StartTimer( 5000, 1, true );
}

/** @type { ( plagueBeast: Character, timerID: number ) => void } */
function onTimer( plagueBeast, timerID )
{
	if( !ValidateObject( plagueBeast ))
		return;

	if( timerID == 1 )
	{
		var numCorpsesFound = AreaItemFunction( "DevourCorpse", plagueBeast, 3, null );
		if( numCorpsesFound > 0 )
			plagueBeast.TextMessage( "* The plague beast absorbs the fleshy remains of the corpse *" );

		plagueBeast.StartTimer( 5000, 1, true );
	}
}

/** @type { ( creature: Character, corpseItem: Item, socket: Socket ) => boolean } */
function DevourCorpse( creature, corpseItem, socket )
{
	if( !ValidateObject( creature ) || !ValidateObject( corpseItem ))
		return false;

	if( !corpseItem.corpse )
		return false;

	if( corpseItem.GetTag( "devoured" ))
		return false;

	var devouredCount = creature.GetTag( "corpsesDevoured" );
	if( devouredCount >= 25 )
		return false;

	corpseItem.SetTag( "devoured", 1 );
	creature.health = Math.min( creature.health + 100, 2000 );

	var corpseOwner = corpseItem.owner;
	if( ValidateObject( corpseOwner ) && !corpseOwner.npc )
	{
		corpseItem.decaytime = GetServerSetting( "CORPSEDECAYTIMER" );
		transformToBones( corpseItem );
	}

	creature.SetTag( "corpsesDevoured", devouredCount + 1 );
	return true;
}

/** @type { ( corpseItem: Item ) => void } */
function transformToBones( corpseItem )
{
	if( !ValidateObject( corpseItem ))
		return;

	// only standard corpse item + only player bodies
	var playerCorpseIDs = [ 0x0190, 0x0191, 0x025D, 0x025E, 0x029A, 0x029B ];
	if( corpseItem.id != 0x2006 || playerCorpseIDs.indexOf( corpseItem.amount ) == -1 )
		return;

	switch( corpseItem.dir )
	{
		case 4:
		case 128: corpseItem.id = 0x0ECD; break; // North
		case 5:
		case 129: corpseItem.id = 0x0ECF; break; // NE
		case 6:
		case 130: corpseItem.id = 0x0ECB; break; // East
		case 7:
		case 131: corpseItem.id = 0x0ECE; break; // SE
		case 8:
		case 132: corpseItem.id = 0x0ECA; break; // South
		case 1:
		case 133: corpseItem.id = 0x0ED0; break; // SW
		case 2:
		case 134: corpseItem.id = 0x0ECC; break; // West
		case 3:
		case 135: corpseItem.id = 0x0ECE; break; // NW
		default: break;
	}
}

/** @type { ( pAttacker: Character, pDefender: Character ) => boolean } */
function onDefense( pAttacker, pDefender )
{
	if( !ValidateObject( pAttacker ) || !ValidateObject( pDefender ))
		return true;

	if( pDefender.sectionID != "plaguebeast" )
		return true;

	// 20% chance to spawn during defense
	if( RandomNumber( 1, 5 ) == 1 )
	{
		pDefender.TextMessage( "* The plague beast creates another beast from its flesh! *" );

		var spawnTypes = [ "earthele", "headless", "pirate", "gorilla", "giantserpent", "slime" ];
		var spawnType = spawnTypes[Math.floor( Math.random() * spawnTypes.length )];

		// Offsets: -2..+2
		var ox = RandomNumber( -2, 2 );
		var oy = RandomNumber( -2, 2 );

		var plagueSpawn = SpawnNPC( spawnType, pDefender.x + ox, pDefender.y + oy, pDefender.z, pDefender.worldnumber, pDefender.instanceID );

		if( ValidateObject( plagueSpawn ))
		{
			plagueSpawn.colour = 0x11;
			plagueSpawn.name = "a plague spawn";
		}
	}

	// poison spit, 30s cooldown
	var iTime = GetCurrentClock();
	var NextUse = pDefender.GetTempTag( "poisonDelayed" );
	var Delay = 30000;

	if( (iTime - NextUse) >= Delay )
	{
		DoMovingEffect( pDefender, pAttacker, 0x36D4, 0x10, 0x00, false );
		pAttacker.SetPoisoned( 4, 10000 );
		if( pAttacker.socket )
		{
			pAttacker.socket.SysMessage( pDefender.name + " spits a poisonous substance at you!" );
		}
		pDefender.SetTempTag( "poisonDelayed", iTime );
	}

	return true;
}