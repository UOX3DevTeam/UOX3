// Wheat-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When (dynamic)wheat is double-clicked, it's setup with
// wheat ripe for picking. When it is harvested it turns into "harvested wheat",
// and a "growth" process happens, where the wheat goes through various stages
// like "sprouts", short wheat and finally becomes harvestable tall wheat again.
var resourceGrowthDelay = 120000; //Delay in milliseconds before resources respawns

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
			var itemMade = CreateBlankItem( pUser.socket, pUser, 1, "#", 0x1ebd, 0x0, "ITEM", true );
			var loot2 = RollDice( 1, 2, 0 );
			if( loot2 == 1 )
			{
				iUsed.id = 0x0daf;
			}
			if( loot2 == 2 )
			{
				iUsed.id = 0x0dae;
			}
			iUsed.SetTag( "Wheat", 0 );
			iUsed.StartTimer( resourceGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts faze 1 of wheat growth
	{
		var sprout = RollDice( 1, 2, 0 );
		if( sprout == 1 )
		{
			iUsed.id = 0x1ebe;
		}
		if( sprout == 2 )
		{
			iUsed.id = 0x1ebf;
		}
		iUsed.StartTimer( 60000, 2, true); 
	}
	if( timerID == 2 ) // Starts faze 2 of wheat growth
	{
		var sprout = RollDice( 1, 2, 0 );
		if( sprout == 1 )
		{
			iUsed.id = 0xc55;
		}
		if( sprout == 2 )
		{
			iUsed.id = 0xc56;
		}
		iUsed.StartTimer( 60000, 3, true ); 
	}
	if( timerID == 3 ) // Starts faze 3 of wheat growth
	{
		var sprout = RollDice( 1, 2, 0 );
		if( sprout == 1 )
		{
			iUsed.id = 0xc57;
		}
		if( sprout == 2 )
		{
			iUsed.id = 0xc59;
		}
		iUsed.StartTimer( 60000, 4, true ); 
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
		iUsed.SetTag( "Wheat", 1 );
	}

}