// Flax-Picking Script
// 19/02/2003 Xuri; xuri@sensewave.com
// When a (dynamic) flax plant is double-clicked, it may yield some flax.
// Then a timer will start, and no more flax can be picked until it runs out.

function onUse( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( "You are too far away to reach that." );
		return;
	}

	if( !iUsed.GetTag("initialized")) // Unless flax have been picked before, initialize settings
	{
		iUsed.SetTag("initialized",true); 	// Marks tree as initialized
		iUsed.SetTag("Flax",1); 		// If set to 1, there is flax to be picked
	}
	var Flax = iUsed.GetTag("Flax");
	if (Flax == 0)
	{	
		pUser.SysMessage( "You find no flax to pick. Try again later." );
	}
	if( Flax == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
			pUser.SysMessage( "You fail to pick any flax." );
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( "You harvest some flax." );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1a9c", false, 1, true, true );
			iUsed.SetTag( "Flax", 0 );
			iUsed.StartTimer( 30000, 1, true ); // Puts in a delay of 30 seconds until next time more flax respawns
		}
	}
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag("Flax", 1);
	}
}