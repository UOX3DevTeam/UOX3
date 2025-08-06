/// <reference path="../../definitions.d.ts" />
// @ts-check
// Flax-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When a (dynamic) flax plant is double-clicked, it may yield some flax.
// Then a timer will start, and no more flax can be picked until it runs out.
const flaxGrowthDelay = 120000; //Delay in milliseconds before resources respawns
const flaxGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const flaxGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const flaxWiltingDelay = 300000; // Delay in milliseconds before fully grown flax wilts
const flaxResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless flax have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks tree as initialized
		iUsed.SetTag( "Flax", 1 ); 		// If set to 1, there is flax to be picked
	}
	var flax = iUsed.GetTag( "Flax" );
	if( flax == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2531, pUser.socket.language )); // You find no flax to pick. Try again later.
	}
	if( flax == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2532, pUser.socket.language )); // You fail to pick any flax.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2533, pUser.socket.language )); // You harvest some flax.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1a9c", 1, "ITEM", true );
			if( flaxResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = flaxResourceDecay;
			}
			iUsed.id = 0x0cb0;
			iUsed.name = "flax stalk";
			iUsed.SetTag( "Flax", 0 );
			iUsed.StartTimer( flaxGrowthDelay, 1, true ); // Puts in a delay of 30 seconds until next time more flax respawns
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of flax growth
	{
		iUsed.id = 0x1a99;
		iUsed.StartTimer( RandomNumber( flaxGrowthIntervalMin, flaxGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Starts phase 2 of flax growth
	{
		iUsed.id = 0x1a9a;
		iUsed.StartTimer( RandomNumber( flaxGrowthIntervalMin, flaxGrowthIntervalMax ), 3, true );
	}
	if( timerID == 3 ) // Flax growth finished!
	{
		iUsed.id = 0x1a9b;
		iUsed.name = "flax";
		iUsed.SetTag( "Flax", 1 );
		iUsed.StartTimer( RandomNumber( flaxGrowthIntervalMin, flaxGrowthIntervalMax ), 4, true );
	}
	if( timerID == 4 ) // Flax is wilting
	{
		iUsed.id = 0x0cb0;
		iUsed.name = "whithered flax stalk";
		iUsed.SetTag( "Flax", 0 );
		iUsed.StartTimer( flaxGrowthDelay, 1, true); // Restart the growth process
	}
}
