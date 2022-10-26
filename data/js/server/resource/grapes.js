// Grape-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When (dynamic) grapevines are double-clicked, they're setup with
// 5 grapes ripe for picking. After they've been picked, a timer starts,
// and until it's up no more grapes can be picked. Once the timer is over,
// new grapes are added. The apperance of the tree indicates whether or
// not there are any grapes left to pick.
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

	if( !iUsed.GetTag( "initialized" )) // Unless grapes have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks tree as initialized
		iUsed.SetTag( "Grapes",1 ); 		// If set to 1, there are grapes to be picked, if 0 there are no ripe grapes
		iUsed.SetTag( "GrapeCounter", maxResource ); 	// Add 5 grapes to the tree initially
	}
	var grapes = iUsed.GetTag( "Grapes" );
	var grapeCount = iUsed.GetTag( "GrapeCounter" );
	if( grapes == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2534, pUser.socket.language )); // You find no ripe grapebunches to pick. Try again later.
	}
	if( grapes == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2535, pUser.socket.language )); // You fail to pick any grapebunches.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2536, pUser.socket.language )); // You pick a grapebunch from the tree.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09d1", 1, "ITEM", true );
			grapeCount--;
			iUsed.SetTag( "GrapeCounter", grapeCount );
			if( grapeCount == 1)
			{
				pUser.SysMessage( GetDictionaryEntry( 2537, pUser.socket.language )); // There is 1 ripe grapebunch left on the tree.
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 2538, pUser.socket.language )); // There are %i ripe grapebunches left on the tree.
			}
	    	if( grapeCount == 0 )
			{
				iUsed.SetTag( "Grapes", 0 );
				iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay of 30 seconds until next time grapes respawn
			}
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag( "GrapeCounter", maxResource );
		iUsed.SetTag( "Grapes", 1 );
	}
}