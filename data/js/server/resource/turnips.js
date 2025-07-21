// Turnip-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)turnips are double-clicked, it's setup with
// turnips ripe for picking. When it is harvested it turns into "harvested turnips",
// and a "growth" process happens, where the turnips go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown turnips again.
const turnipGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const turnipGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const turnipGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const turnipResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless turnips have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Turnip", 1 ); 		// If set to 1, there is turnip to be picked, if 0 there is no turnip left
	}
	var turnip = iUsed.GetTag( "Turnip" );
	if( turnip == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2594, pUser.socket.language )); // This turnip is not ready for harvesting yet.
		return false;
	}
	if( turnip == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2595, pUser.socket.language )); // You fail to pick any turnips.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2596, pUser.socket.language )); // You harvest a turnip.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0d39", 1, "ITEM", true );
			if( turnipResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = turnipResourceDecay;
			}
			iUsed.id = 0x0cb6;
			iUsed.name = "turnip sprout";
			iUsed.SetTag( "Turnip", 0 );
			iUsed.StartTimer( turnipGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of turnip growth
	{
		iUsed.id = 0x0c61;
		iUsed.StartTimer( RandomNumber( turnipGrowthIntervalMin, turnipGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Starts phase 1 of turnip growth
	{
		iUsed.id = 0x0c62;
		iUsed.StartTimer( RandomNumber( turnipGrowthIntervalMin, turnipGrowthIntervalMax ), 3, true);
	}
	if( timerID == 3 ) // Turnip growth finished!
	{
		iUsed.id = 0x0c63;
		iUsed.name = "turnip";
		iUsed.SetTag( "Turnip", 1 );
	}
}