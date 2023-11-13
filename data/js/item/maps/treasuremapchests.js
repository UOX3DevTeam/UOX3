function onCreateDFN( objMade, objType )
{
	if( !ValidateObject( objMade ))
		return;

	// Decay the container 10 minutes after spawning
	objMade.decayable = 1;
	objMade.decaytime = 60 * 10; // 10 minutes
}

const possibleNPCs = {
1: ["Mongbat", "Ratman", "Headless", "Skeleton", "Zombie"],
2: ["OrcMage", "Gargoyle", "Gazer", "Hellhound", "EarthElemental"],
3: ["Liche", "OgreLord", "DreadSpider", "AirElemental", "FireElemental"],
4: ["DreadSpider", "LichLord", "Daemon", "ElderGazer", "OgreLord"],
5: ["LichLord", "GreaterDaemon", " ElderGazer", "PoisonElemental", "BloodElemental"],
6: ["LichLord", "GreaterDaemon", " ElderGazer", "PoisonElemental", "BloodElemental"],};

function onContRemoveItem( iCont, iItem, pChar )
{
	if( !ValidateObject( iCont ) || !ValidateObject( iItem ))
		return;

		if (0.1 >= Math.random())
		{
			// 10% chance to spawn a new monster
  
			var level = iCont.GetTag("Level"); // set the level to a static value using the tag from the chest.
			var randomNPCs = possibleNPCs[level]; // selects the array of possible NPCs for the given level
			var randomNPC = randomNPCs[Math.floor(Math.random() * randomNPCs.length)]; // selects a random NPC from the array
  
			var nSpawned = SpawnNPC(randomNPC, iCont.x, iCont.y, iCont.z, iCont.worldnumber, iCont.instanceID);
		}

	// Check if there are other items left in the container
	if( iCont.itemsinside > 1 ) // The last item is iItem; this property doesn't update instantly
	{
		// There are still items inside, prevent it from decaying...
		var timeLeft = Math.floor(( iCont.decaytime - GetCurrentClock() ) / 1000 );
		if( timeLeft < 60 )
		{
			iCont.decaytime = 180; // Set new decay time to 3 minutes
		}
		return;
	}

	// When last item has been removed from the chest, let decay kick in
	iCont.decaytime = 30; // Set it to decay in 60 seconds, so it decays before item respawns kick in
}