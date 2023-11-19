function onCreateDFN( objMade, objType )
{
	if( !ValidateObject( objMade ))
		return;

	// Decay the container 10 minutes after spawning
	objMade.decayable = 1;
	objMade.decaytime = 60 * 10; // 10 minutes
	objMade.SetTag( "spawnCount", 15 );
}

const spawnOnItemPickup = {
	0: ["mongbat", "ratman", "headless", "skeleton", "zombie"],
	1: ["mongbat", "ratman", "headless", "skeleton", "zombie"],
	2: ["orcmage", "gargoyle", "gazer", "hellhound", "earthele"],
	3: ["lich", "ogrelord", "dreadspider", "airele", "fireele"],
	4: ["dreadspider", "lichlord", "daemon", "eldergazer", "ogrelord"],
	5: ["lichlord", "daemon", " eldergazer", "poisonele", "bloodele"],
	6: ["LichLord", "daemon", " eldergazer", "poisonele", "bloodele"],};

function onContRemoveItem( iCont, iItem, pChar )
{
	var forceSpawnLimit = iCont.GetTag( "spawnCount" );
	if( !ValidateObject( iCont ) || !ValidateObject( iItem ))
		return;

	if( forceSpawnLimit > 0 )
	{
		if( 0.1 >= Math.random() )
		{
			// 10% chance to spawn a new monster

			var mapLevel = iCont.GetTag( "Level" ); // set the level to a static value using the tag from the chest.
			var spawnList = spawnOnItemPickup[mapLevel]; // selects the array of possible NPCs for the given level
			var randomlySelectedNPC = spawnList[Math.floor( Math.random() * spawnList.length )]; // selects a random NPC from the array

			var nSpawned = SpawnNPC( randomlySelectedNPC, iCont.x, iCont.y, iCont.z, iCont.worldnumber, iCont.instanceID );
			iCont.SetTag( "spawnCount", forceSpawnLimit - 1 );
		}
	}
	// Check if there are other items left in the container
	if( iCont.itemsinside > 1 ) // The last item is iItem; this property doesn't update instantly
	{
		// There are still items inside, prevent it from decaying...
		var timeLeft = Math.floor( ( iCont.decaytime - GetCurrentClock() ) / 1000 );
		if( timeLeft < 60 )
		{
			iCont.decaytime = 180; // Set new decay time to 3 minutes
		}
		return;
	}

	// When last item has been removed from the chest, let decay kick in
	iCont.decaytime = 30; // Set it to decay in 60 seconds, so it decays before item respawns kick in
}

function onDropItemOnItem( iDropped, cDropper, iDroppedOn )
{
	var socket = cDropper.socket;
	socket.SysMessage( GetDictionaryEntry( 5713, socket.language ));// The chest refuses to be filled with any more treasure.
	return false;
}