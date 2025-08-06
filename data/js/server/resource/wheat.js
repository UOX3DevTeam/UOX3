/// <reference path="../../definitions.d.ts" />
// @ts-check
// Wheat-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When (dynamic)wheat is double-clicked, it's setup with
// wheat ripe for picking. When it is harvested it turns into "harvested wheat",
// and a "growth" process happens, where the wheat goes through various stages
// like "sprouts", short wheat and finally becomes harvestable tall wheat again.
const wheatGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const wheatGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const wheatGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const wheatWiltingDelay = 300000; // Delay in milliseconds before fully grown wheat wilts
const wheatResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

/** @type {( pUser: Character, iUsed: Item ) => boolean} */
function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless wheats have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Wheat", 1 ); 		// If set to 1, there is wheat to be picked, if 0 there is no wheat left
	}
	var wheat = iUsed.GetTag( "Wheat" );
	if( wheat == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2549, pUser.socket.language )); // This wheat is not ready for harvesting yet.
		return false;
	}
	if( wheat == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2550, pUser.socket.language )); // You fail to pick any wheat.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2551, pUser.socket.language )); // You harvest some wheat.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1ebd", 1, "ITEM", true );
			if( wheatResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = wheatResourceDecay;
			}
			iUsed.id = RandomNumber( 0, 1 ) ? 0x0daf : 0x0dae;
			iUsed.name = "wheat stalks";
			iUsed.SetTag( "Wheat", 0 );
			iUsed.StartTimer( wheatGrowthDelay, 1, true ); // Let's do some timers! Whee!
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of wheat growth
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x1ebe : 0x1ebf;
		iUsed.StartTimer( RandomNumber( wheatGrowthIntervalMin, wheatGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Starts phase 2 of wheat growth
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0xc55 : 0xc56;
		iUsed.StartTimer( RandomNumber( wheatGrowthIntervalMin, wheatGrowthIntervalMax ), 3, true );
	}
	if( timerID == 3 ) // Starts phase 3 of wheat growth
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0xc57 : 0xc59;
		iUsed.StartTimer( RandomNumber( wheatGrowthIntervalMin, wheatGrowthIntervalMax ), 4, true );
	}
	if( timerID == 4 ) // Wheat growth finished!
	{
		var sprout = RollDice( 1, 3, 0 );
		if( sprout == 1 )
		{
			iUsed.id = 0xc58;
		}
		if( sprout == 2 )
		{
			iUsed.id = 0xc5a;
		}
		if( sprout == 3 )
		{
			iUsed.id = 0xc5b;
		}
		iUsed.name = "wheat";
		iUsed.SetTag( "Wheat", 1 );
		iUsed.StartTimer( Math.round( RandomNumber( wheatWiltingDelay / 2, wheatWiltingDelay * 1.5 )), 5, true );
	}
	if( timerID == 5 ) // Wheat is wilting
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x0daf : 0x0dae;
		iUsed.name = "whithered wheat stalks";
		iUsed.SetTag( "Wheat", 0 );
		iUsed.StartTimer( wheatGrowthDelay, 1, true); // Restart the growth process
	}
}