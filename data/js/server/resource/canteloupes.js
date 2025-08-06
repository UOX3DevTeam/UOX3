/// <reference path="../../definitions.d.ts" />
// @ts-check
// Canteloupe-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)canteloupe are double-clicked, it's setup with
// canteloupe ripe for picking. When it is harvested it turns into "harvested canteloupe",
// and a "growth" process happens, where the canteloupe go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown canteloupe again.
const canteloupeGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const canteloupeGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const canteloupeGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const canteloupeResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

/** @type {( pUser: Character, iUsed: Item ) => boolean} */
function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless canteloupe have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Canteloupe", 1 ); 	// If set to 1, there is canteloupe to be picked, if 0 there is no canteloupe left
	}
	var canteloupe = iUsed.GetTag( "Canteloupe" );
	if( canteloupe == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 9515, pUser.socket.language )); // This canteloupe is not ready for harvesting yet.
		return false;
	}
	if( canteloupe == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 9516, pUser.socket.language )); // You fail to pick any canteloupe.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 9517, pUser.socket.language )); // You harvest a canteloupe.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c79", 1, "ITEM", true );
			if( canteloupeResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = canteloupeResourceDecay;
			}
			iUsed.id = RandomNumber( 0, 1 ) ? 0x0c5f : 0x0c60;
			iUsed.name = "canteloupe patch";
			iUsed.SetTag( "Canteloupe", 0 );
			iUsed.StartTimer( canteloupeGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of canteloupe growth
	{
		iUsed.StartTimer( RandomNumber( canteloupeGrowthIntervalMin, canteloupeGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Canteloupe growth finished!
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x0c79 : 0x0c7a;
		iUsed.name = "canteloupe";
		iUsed.SetTag( "Canteloupe", 1 );
	}
}