// Date-Picking Script
// 20/02/2006 Xuri; xuri@sensewave.com
// When a (dynamic) date tree is double-clicked, it's setup with
// 5 dates ripe for picking. After they've been picked, a timer starts,
// and until it's up no more dates can be picked. Once the timer is over,
// new dates are added. The apperance of the tree indicates whether or
// not there are any dates left to pick.
var resourceGrowthDelay = 120000; //Delay in milliseconds before resources respawns
var maxResource = 5; //maximum amount of resources on a given item

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( "You are too far away to reach that." );
		return false;
	}

	if( !iUsed.GetTag("initialized")) // Unless dates have been picked before, initialize settings
	{
		iUsed.SetTag("initialized",1); 	// Marks tree as initialized
		iUsed.SetTag("Dates",1); 		// If set to 1, there are dates to be picked, if 0 there are no ripe dates
		iUsed.SetTag("DateCounter", maxResource); 	// Add 5 dates to the tree initially
	}
	var Dates = iUsed.GetTag("Dates");
	var DateCount = iUsed.GetTag("DateCounter");
	if (Dates == 0)
	{	
		pUser.SysMessage( "You find no ripe dates to pick. Try again later." );
	}
	if( Dates == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
			pUser.SysMessage( "You fail to pick any dates." );
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( "You pick a bunch of dates from the date-palm." );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1727", 1, "ITEM", true );
			DateCount--;
			iUsed.SetTag( "DateCounter", DateCount );
			if( DateCount == 1)
				pUser.SysMessage( "There is "+DateCount+" ripe bunch of dates left on the tree." );
			else
				pUser.SysMessage( "There are "+DateCount+" ripe dates left on the tree." );
		    	if( DateCount == 0 )
			{
				if( iUsed.id == 0x0d96 )
					iUsed.id = 0x0d95;
				else if( iUsed.id == 0x0d9a )
					iUsed.id = 0x0d99; 
				iUsed.SetTag( "Dates", 0 );
				iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay of 30 seconds until next time dates respawn
			}
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag("DateCounter", maxResource);
		iUsed.SetTag("Dates", 1);
		if( iUsed.id == 0x0d95 )
			iUsed.id = 0x0d96;
		else if( iUsed.id == 0x0d99 )
			iUsed.id = 0x0d9a; 
	}
}