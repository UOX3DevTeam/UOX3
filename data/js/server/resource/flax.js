// Flax-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When a (dynamic) flax plant is double-clicked, it may yield some flax.
// Then a timer will start, and no more flax can be picked until it runs out.
var resourceGrowthDelay = 120000; //Delay in milliseconds before resources respawns

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
			iUsed.SetTag( "Flax", 0 );
			iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay of 30 seconds until next time more flax respawns
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag( "Flax", 1 );
	}
}