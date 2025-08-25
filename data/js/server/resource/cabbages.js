/// <reference path="../../definitions.d.ts" />
// @ts-check
// Cabbage-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)cabbages are double-clicked, it's setup with
// cabbages ripe for picking. When it is harvested it turns into "harvested cabbages",
// and a "growth" process happens, where the cabbages go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown cabbages again.
const cabbageGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const cabbageGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const cabbageGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const cabbageResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless cabbages have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Cabbage", 1 ); 		// If set to 1, there is cabbage to be picked, if 0 there is no cabbage left
	}
	var cabbage = iUsed.GetTag( "Cabbage" );
	if( cabbage == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2597, pUser.socket.language )); // This cabbage is not ready for harvesting yet.
		return false;
	}
	if( cabbage == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2598, pUser.socket.language )); // You fail to pick any cabbages.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2599, pUser.socket.language )); // You harvest a cabbage.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c7b", 1, "ITEM", true );
			if( cabbageResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = cabbageResourceDecay;
			}
			iUsed.id = 0x0cb6;
			iUsed.name = "cabbage sprout";
			iUsed.SetTag( "Cabbage", 0 );
			iUsed.StartTimer( cabbageGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of cabbage growth
	{
		iUsed.StartTimer( RandomNumber( cabbageGrowthIntervalMin, cabbageGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Cabbage growth finished!
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x0c7b : 0x0c7c;
		iUsed.name = "cabbage";
		iUsed.SetTag( "Cabbage", 1 );
	}
}
