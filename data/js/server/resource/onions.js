/// <reference path="../../definitions.d.ts" />
// @ts-check
// Onion-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)onions are double-clicked, it's setup with
// onions ripe for picking. When it is harvested it turns into "harvested onions",
// and a "growth" process happens, where the onions go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown onions again.
const onionGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const onionGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const onionGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const onionResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

/** @type {( pUser: Character, iUsed: Item ) => boolean} */
function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless onions have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Onion", 1 ); 		// If set to 1, there is onion to be picked, if 0 there is no onion left
	}
	var onion = iUsed.GetTag( "Onion" );
	if( onion == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2591, pUser.socket.language )); // These onions are not ready for harvesting yet.
		return false;
	}
	if( onion == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2592, pUser.socket.language )); // You fail to pick any onions.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2593, pUser.socket.language )); // You harvest an onion.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c6d", 1, "ITEM", true );
			if( onionResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = onionResourceDecay;
			}
			iUsed.id = 0x0c68;
			iUsed.name = "onion sprouts";
			iUsed.SetTag( "Onion", 0 );
			iUsed.StartTimer( onionGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of onion growth
	{
		iUsed.id = 0x0c69;
		iUsed.StartTimer( RandomNumber( onionGrowthIntervalMin, onionGrowthIntervalMax ), 2, true );
	}
	if( timerID == 2 ) // Onion growth finished!
	{
		iUsed.id = 0x0c6f;
		iUsed.name = "onions";
		iUsed.SetTag( "Onion", 1 );
	}
}