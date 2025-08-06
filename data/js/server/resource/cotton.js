/// <reference path="../../definitions.d.ts" />
// @ts-check
// Cotton-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When a (dynamic) cotton plant is double-clicked, it may yield some cotton.
// Then a timer will start, and no more cotton can be picked until it runs out.
const cottonGrowthDelay = 120000; // Delay in milliseconds before resource regrowth kicks in
const cottonGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const cottonGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const cottonResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless cotton have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks tree as initialized
		iUsed.SetTag( "Cotton", 1 ); 		// If set to 1, there is cotton to be picked
	}
	var cotton = iUsed.GetTag( "Cotton" );
	if( cotton == 0 )
	{
		pUser.SysMessage( GetDictionaryEntry( 2523, pUser.socket.language )); // You find no cotton to pick. Try again later.
	}
	if( cotton == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2524, pUser.socket.language )); // You fail to pick any cotton.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2525, pUser.socket.language )); // You harvest some cotton.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0df9", 1, "ITEM", true );
			if( cottonResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = cottonResourceDecay;
			}
			iUsed.id = RandomNumber( 0, 1 ) ? 0x0c51 : 0x0c52;
			iUsed.name = "cotton seedling";
			iUsed.SetTag( "Cotton", 0 );
			iUsed.StartTimer( cottonGrowthDelay, 1, true ); // Puts in a delay of 60 seconds until next time more cotton respawns
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of cottong growth
	{
		iUsed.StartTimer( RandomNumber( cottonGrowthIntervalMin, cottonGrowthIntervalMax ), 2, true );
	}
	if( timerID == 2 ) // Cotton growth finished!
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x0c4f : 0x0c50;
		iUsed.name = "cotton";
		iUsed.SetTag( "Cotton", 1 );
	}
}