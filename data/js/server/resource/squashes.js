/// <reference path="../../definitions.d.ts" />
// @ts-check
// Squash-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)squashes are double-clicked, it's setup with
// squashes ripe for picking. When it is harvested it turns into "harvested squashes",
// and a "growth" process happens, where the squashes go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown squashes again.
const squashGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const squashGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const squashGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const squashResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless squashes have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Squash", 1 ); 		// If set to 1, there is squash to be picked, if 0 there is no squash left
	}
	var squash = iUsed.GetTag( "Squash" );
	if( squash == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 9506, pUser.socket.language )); // This squash is not ready for harvesting yet.
		return false;
	}
	if( squash == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 9507, pUser.socket.language )); // You fail to pick any squashes.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 9508, pUser.socket.language )); // You harvest a squash.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c72", 1, "ITEM", true );
			if( squashResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = squashResourceDecay;
			}
			iUsed.id = RandomNumber( 0, 1 ) ? 0x0c5f : 0x0c60;
			iUsed.name = "squash patch";
			iUsed.SetTag( "Squash", 0 );
			iUsed.StartTimer( squashGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of squash growth
	{
		iUsed.StartTimer( RandomNumber( squashGrowthIntervalMin, squashGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Squash growth finished!
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x0c72 : 0x0c73;
		iUsed.name = "squash";
		iUsed.SetTag( "Squash", 1 );
	}
}