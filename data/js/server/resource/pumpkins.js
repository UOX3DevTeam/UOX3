// Pumpkin-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)pumpkins are double-clicked, it's setup with
// pumpkins ripe for picking. When it is harvested it turns into "harvested pumpkins",
// and a "growth" process happens, where the pumpkins go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown pumpkins again.
const pumpkinGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const pumpkinGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const pumpkinGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const pumpkinResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless pumpkins have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Pumpkin", 1 ); 		// If set to 1, there is pumpkin to be picked, if 0 there is no pumpkin left
	}
	var pumpkin = iUsed.GetTag( "Pumpkin" );
	if( pumpkin == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 9503, pUser.socket.language )); // This pumpkin is not ready for harvesting yet.
		return false;
	}
	if( pumpkin == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 9504, pUser.socket.language )); // You fail to pick any pumpkins.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 9505, pUser.socket.language )); // You harvest a pumpkin.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c6a", 1, "ITEM", true );
			if( pumpkinResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = pumpkinResourceDecay;
			}
			iUsed.id = RandomNumber( 0, 1 ) ? 0x0c5f : 0x0c60;
			iUsed.name = "pumpkin patch";
			iUsed.SetTag( "Pumpkin", 0 );
			iUsed.StartTimer( pumpkinGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of pumpkin growth
	{
		iUsed.id = 0x0c6c;
		iUsed.name = "baby pumpkin";
		iUsed.StartTimer( RandomNumber( pumpkinGrowthIntervalMin, pumpkinGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Pumpkin growth finished!
	{
		iUsed.id = RandomNumber( 0, 1 ) ? 0x0c6a : 0x0c6b;
		iUsed.name = "pumpkin";
		iUsed.SetTag( "Pumpkin", 1 );
	}
}