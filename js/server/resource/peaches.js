// Peach-Picking Script
// 20/02/2006 Xuri; xuri@sensewave.com
// When a (dynamic) peach tree is double-clicked, it's setup with
// 5 peaches ripe for picking. After they've been picked, a timer starts,
// and until it's up no more peaches can be picked. Once the timer is over,
// new peaches are added. The apperance of the tree indicates whether or
// not there are any peaches left to pick.
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

	if( !iUsed.GetTag("initialized")) // Unless peaches have been picked before, initialize settings
	{
		iUsed.SetTag("initialized", 1); 	// Marks tree as initialized
		iUsed.SetTag("Peaches",1); 		// If set to 1, there are peaches to be picked, if 0 there are no ripe peaches
		iUsed.SetTag("PeachCounter", maxResource); 	// Add 5 peaches to the tree initially
	}
	var Peaches = iUsed.GetTag("Peaches");
	var PeachCount = iUsed.GetTag("PeachCounter");
	if (Peaches == 0)
	{	
		pUser.SysMessage( "You find no ripe peaches to pick. Try again later." );
	}
	if( Peaches == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
			pUser.SysMessage( "You fail to pick any peaches." );
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( "You pick a peach from the tree." );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09d2", 1, "ITEM", true );
			PeachCount--;
			iUsed.SetTag( "PeachCounter", PeachCount );
			if( PeachCount == 1)
				pUser.SysMessage( "There is "+PeachCount+" ripe peach left on the tree." );
			else
				pUser.SysMessage( "There are "+PeachCount+" ripe peaches left on the tree." );
		    	if( PeachCount == 0 )
			{
				if( iUsed.id == 0x0d9e )
					iUsed.id = 0x0d9d;
				else if( iUsed.id == 0x0da2 )
					iUsed.id = 0x0da1; 
				iUsed.SetTag( "Peaches", 0 );
				iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay of 30 seconds until next time peaches respawn
			}
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag("PeachCounter", maxResource);
		iUsed.SetTag("Peaches", 1);
		if( iUsed.id == 0x0d9d )
			iUsed.id = 0x0d9e;
		else if( iUsed.id == 0x0da1 )
			iUsed.id = 0x0da2; 
	}
}