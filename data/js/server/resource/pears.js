// Pear-Picking Script
// 20/02/2006 Xuri; xuri@sensewave.com
// When a (dynamic) pear tree is double-clicked, it's setup with
// 5 pears ripe for picking. After they've been picked, a timer starts,
// and until it's up no more pears can be picked. Once the timer is over,
// new pears are added. The apperance of the tree indicates whether or
// not there are any pears left to pick.
var resourceGrowthDelay = 120000; //Delay in milliseconds before resources respawns
var maxResource = 5; //maximum amount of resources on a given item

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless pears have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks tree as initialized
		iUsed.SetTag( "Pears", 1 ); 		// If set to 1, there are pears to be picked, if 0 there are no ripe pears
		iUsed.SetTag( "PearCounter", maxResource ); 	// Add 5 pears to the tree initially
	}
	var pears = iUsed.GetTag( "Pears" );
	var pearCount = iUsed.GetTag( "PearCounter" );
	if( pears == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2544, pUser.socket.language )); // You find no ripe pears to pick. Try again later.
	}
	if( pears == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2545, pUser.socket.language )); // You fail to pick any pears.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2546, pUser.socket.language )); // You pick a pear from the tree.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0994", 1, "ITEM", true );
			pearCount--;
			iUsed.SetTag( "PearCounter", pearCount );
			if( pearCount == 1 )
			{
				pUser.SysMessage(yEntry( 2547, pUser.socket.language )); // There is 1 ripe pear left on the tree.
			}
			else
			{
				var pearCountMsg = GetDictionaryEntry( 2548, pUser.socket.language ); // There are %i ripe pears left on the tree.
				pUser.SysMessage( pearCountMsg.replace( /%i/gi, pearCount ));
			}
		    if( pearCount == 0 )
			{
				if( iUsed.id == 0x0da6 )
				{
					iUsed.id = 0x0da5;
				}
				else if( iUsed.id == 0x0daa )
				{
					iUsed.id = 0x0da9; 
				}
				iUsed.SetTag( "Pears", 0 );
				iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay of 30 seconds until next time pears respawn
			}
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag( "PearCounter", maxResource );
		iUsed.SetTag( "Pears", 1 );
		if( iUsed.id == 0x0da5 )
		{
			iUsed.id = 0x0da6;
		}
		else if( iUsed.id == 0x0da9 )
		{
			iUsed.id = 0x0daa; 
		}
	}
}