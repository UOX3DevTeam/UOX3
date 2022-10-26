// Cotton-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When a (dynamic) cotton plant is double-clicked, it may yield some cotton.
// Then a timer will start, and no more cotton can be picked until it runs out.
var resourceGrowthDelay = 120000; //Delay in milliseconds before resources respawns

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
			iUsed.SetTag( "Cotton", 0 );
			iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay of 60 seconds until next time more cotton respawns
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag( "Cotton", 1 );
	}
}