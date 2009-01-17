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
		pUser.SysMessage( "You are too far away to reach that." );
		return false;
	}

	if( !iUsed.GetTag("initialized")) // Unless pears have been picked before, initialize settings
	{
		iUsed.SetTag("initialized",1); 	// Marks tree as initialized
		iUsed.SetTag("Pears",1); 		// If set to 1, there are pears to be picked, if 0 there are no ripe pears
		iUsed.SetTag("PearCounter", maxResource); 	// Add 5 pears to the tree initially
	}
	var Pears = iUsed.GetTag("Pears");
	var PearCount = iUsed.GetTag("PearCounter");
	if (Pears == 0)
	{	
		pUser.SysMessage( "You find no ripe pears to pick. Try again later." );
	}
	if( Pears == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
			pUser.SysMessage( "You fail to pick any pears." );
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( "You pick a pear from the tree." );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0994", 1, "ITEM", true );
			PearCount--;
			iUsed.SetTag( "PearCounter", PearCount );
			if( PearCount == 1)
				pUser.SysMessage( "There is "+PearCount+" ripe pear left on the tree." );
			else
				pUser.SysMessage( "There are "+PearCount+" ripe pears left on the tree." );
		    	if( PearCount == 0 )
			{
				if( iUsed.id == 0x0da6 )
					iUsed.id = 0x0da5;
				else if( iUsed.id == 0x0daa )
					iUsed.id = 0x0da9; 
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
		iUsed.SetTag("PearCounter", maxResource);
		iUsed.SetTag("Pears", 1);
		if( iUsed.id == 0x0da5 )
			iUsed.id = 0x0da6;
		else if( iUsed.id == 0x0da9 )
			iUsed.id = 0x0daa; 
	}
}