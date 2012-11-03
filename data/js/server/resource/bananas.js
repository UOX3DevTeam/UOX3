// Banana-Picking Script
// 20/02/2006 Xuri; xuri@sensewave.com
// When a (dynamic) banana tree is double-clicked, it's setup with
// 5 banans ripe for picking. After they've been picked, a timer starts,
// and until it's up no more bananas can be picked. Once the timer is over,
// new bananas are added.
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

	if( !iUsed.GetTag("initialized")) // Unless bananas have been picked before, initialize settings
	{
		iUsed.SetTag("initialized", 1); 	// Marks tree as initialized
		iUsed.SetTag("Bananas",1); 		// If set to 1, there are bananas to be picked, if 0 there are no ripe bananas
		iUsed.SetTag("BananaCounter", maxResource); 	// Add 5 bananas to the tree initially
	}
	var Bananas = iUsed.GetTag("Bananas");
	var BananaCount = iUsed.GetTag("BananaCounter");
	if (Bananas == 0)
	{	
		pUser.SysMessage( "You find no ripe bananas to pick. Try again later." );
	}
	if( Bananas == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
			pUser.SysMessage( "You fail to pick any bananas." );
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( "You pick a banana from the tree." );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x171f", 1, "ITEM", true );
			BananaCount--;
			iUsed.SetTag( "BananaCounter", BananaCount );
			if( BananaCount == 1)
				pUser.SysMessage( "There is "+BananaCount+" ripe bananas left on the tree." );
			else
				pUser.SysMessage( "There are "+BananaCount+" ripe bananas left on the tree." );
		    	if( BananaCount == 0 )
			{
				iUsed.SetTag( "Bananas", 0 );
				iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay of 30 seconds until next time bananas respawn
			}
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag("BananaCounter", maxResource);
		iUsed.SetTag("Bananas", 1);
	}
}