/// <reference path="../../definitions.d.ts" />
// @ts-check
// HoneydewMelon-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)honeydew melons are double-clicked, it's setup with
// honeydew melons ripe for picking. When it is harvested it turns into "harvested honeydew melons",
// and a "growth" process happens, where the honeydew melons go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown honeydew melons again.
const honeydewmelonGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const honeydewmelonGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const honeydewmelonGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const honeydewmelonResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless honeydew melons have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "HoneydewMelon", 1 ); 		// If set to 1, there is honeydew melon to be picked, if 0 there is no honeydew melon left
	}
	var honeydewmelon = iUsed.GetTag( "HoneydewMelon" );
	if( honeydewmelon == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 9512, pUser.socket.language )); // This honeydew melon is not ready for harvesting yet.
		return false;
	}
	if( honeydewmelon == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 9513, pUser.socket.language )); // You fail to pick any honeydew melons.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 9514, pUser.socket.language )); // You harvest a honeydew melon.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c74", 1, "ITEM", true );
			if( honeydewmelonResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = honeydewmelonResourceDecay;
			}
			iUsed.id = RandomNumber( 0, 1 ) ? 0x0c5f : 0x0c60;
			iUsed.name = "honeydew melon patch";
			iUsed.SetTag( "HoneydewMelon", 0 );
			iUsed.StartTimer( honeydewmelonGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of honeydewmelon growth
	{
		iUsed.StartTimer( RandomNumber( honeydewmelonGrowthIntervalMin, honeydewmelonGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // HoneydewMelon growth finished!
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x0c74 : 0x0c75;
		iUsed.name = "honeydew melon";
		iUsed.SetTag( "HoneydewMelon", 1 );
	}
}
