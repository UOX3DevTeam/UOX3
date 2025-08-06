/// <reference path="../../definitions.d.ts" />
// @ts-check
// Carrot-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)carrots are double-clicked, it's setup with
// carrots ripe for picking. When it is harvested it turns into "harvested carrots",
// and a "growth" process happens, where the carrots go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown carrots again.
const carrotGrowthDelay = 120000; // Delay in milliseconds before resource regrowth kicks in
const carrotGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const carrotGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const carrotResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless carrots have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Carrot", 1 ); 		// If set to 1, there is carrot to be picked, if 0 there is no carrot left
	}
	var carrot = iUsed.GetTag( "Carrot" );
	if( carrot == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 5588, pUser.socket.language )); // These carrots are not ready for harvesting yet.
		return false;
	}
	if( carrot == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2589, pUser.socket.language )); // You fail to pick any carrots.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2590, pUser.socket.language )); // You harvest a carrot.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c77", 1, "ITEM", true );
			if( carrotResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = carrotResourceDecay;
			}
			iUsed.id = 0x0c68;
			iUsed.name = "carrot sprouts";
			iUsed.SetTag( "Carrot", 0 );
			iUsed.StartTimer( carrotGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of carrot growth
	{
		iUsed.id = 0x0c69;
		iUsed.StartTimer( RandomNumber( carrotGrowthIntervalMin, carrotGrowthIntervalMax ), 2, true );
	}
	if( timerID == 2 ) // Carrot growth finished!
	{
		iUsed.id = 0x0c76;
		iUsed.name = "carrots";
		iUsed.SetTag( "Carrot", 1 );
	}
}