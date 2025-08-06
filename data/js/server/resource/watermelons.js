/// <reference path="../../definitions.d.ts" />
// @ts-check
// Watermelon-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)watermelons are double-clicked, it's setup with
// watermelons ripe for picking. When it is harvested it turns into "harvested watermelons",
// and a "growth" process happens, where the watermelons go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown watermelons again.
const watermelonGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const watermelonGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const watermelonGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const watermelonResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless watermelons have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Watermelon", 1 ); 		// If set to 1, there is watermelon to be picked, if 0 there is no watermelon left
	}
	var watermelon = iUsed.GetTag( "Watermelon" );
	if( watermelon == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 9509, pUser.socket.language )); // This watermelon is not ready for harvesting yet.
		return false;
	}
	if( watermelon == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 9510, pUser.socket.language )); // You fail to pick any watermelons.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 9511, pUser.socket.language )); // You harvest a watermelon.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c5c", 1, "ITEM", true );
			if( watermelonResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = watermelonResourceDecay;
			}
			iUsed.id = RandomNumber( 0, 1 ) ? 0x0c5f : 0x0c60;
			iUsed.name = "watermelon patch";
			iUsed.SetTag( "Watermelon", 0 );
			iUsed.StartTimer( watermelonGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of watermelon growth
	{
		iUsed.id = 0x0c5d;
		iUsed.name = "baby watermelon";
		iUsed.StartTimer( RandomNumber( watermelonGrowthIntervalMin, watermelonGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Watermelon growth finished!
	{
		iUsed.id = 0x0c5c;
		iUsed.name = "watermelon";
		iUsed.SetTag( "Watermelon", 1 );
	}
}