/// <reference path="../../definitions.d.ts" />
// @ts-check
// Gourd-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)gourds are double-clicked, it's setup with
// gourds ripe for picking. When it is harvested it turns into "harvested gourds",
// and a "growth" process happens, where the gourds go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown gourds again.
const gourdGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const gourdGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const gourdGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const gourdResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless gourds have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Gourd", 1 ); 		// If set to 1, there is gourd to be picked, if 0 there is no gourd left
	}
	var gourd = iUsed.GetTag( "Gourd" );
	if( gourd == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 9500, pUser.socket.language )); // This gourd is not ready for harvesting yet.
		return false;
	}
	if( gourd == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 9501, pUser.socket.language )); // You fail to pick any gourds.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 9502, pUser.socket.language )); // You harvest a gourd.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c66", 1, "ITEM", true );
			if( gourdResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = gourdResourceDecay;
			}
			iUsed.id = RandomNumber( 0, 1 ) ? 0x0c5f : 0x0c60;
			iUsed.name = "gourd patch";
			iUsed.SetTag( "Gourd", 0 );
			iUsed.StartTimer( gourdGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of gourd growth
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x0c64 : 0x0c65;
		iUsed.name = "baby gourd";
		iUsed.StartTimer( RandomNumber( gourdGrowthIntervalMin, gourdGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Gourd growth finished!
	{
		if( iUsed.id == 0x0c65 )
		{
			iUsed.id = 0x0c66;
		}
		else if( iUsed.id == 0x0c64 )
		{
			iUsed.id = 0x0c67;
		}
		iUsed.name = "gourd";
		iUsed.SetTag( "Gourd", 1 );
	}
}