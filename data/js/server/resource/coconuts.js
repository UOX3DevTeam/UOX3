// Coconut-Picking Script
// 19/02/2003 Xuri; xuri@sensewave.com
// When a (dynamic) coconut palm is double-clicked, it's setup with
// 5 coconuts ripe for picking. After they've been picked, a timer starts,
// and until it's up no more coconuts can be picked. Once the timer is over,
// new coconuts are added.

function onUse( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( "You are too far away to reach that." );
		return;
	}

	if( !iUsed.GetTag("initialized")) // Unless apples have been picked before, initialize settings
	{
		iUsed.SetTag("initialized",true); 	// Marks tree as initialized
		iUsed.SetTag("Coconuts",1); 		// If set to 1, there are apples to be picked, if 0 there are no ripe apples
		iUsed.SetTag("CoconutCounter", 5); 	// Add 5 apples to the tree initially
	}
	var Coconuts = iUsed.GetTag("Coconuts");
	var CoconutCount = iUsed.GetTag("CoconutCounter");
	if (Coconuts == 0)
	{	
		pUser.SysMessage( "You find no coconuts to pick. Try again later." );
	}
	if( Coconuts == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
			pUser.SysMessage( "You fail to pick any coconuts." );
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( "You pick a coconut from the palm-tree." );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1726", 1, "ITEM", true );
			CoconutCount--;
			iUsed.SetTag( "CoconutCounter", CoconutCount );
			if( CoconutCount == 1)
				pUser.SysMessage( "There is "+CoconutCount+" coconut left on the tree." );
			else
				pUser.SysMessage( "There are "+CoconutCount+" coconuts left on the tree." );
		    	if( CoconutCount == 0 )
			{
				iUsed.SetTag( "Coconuts", 0 );
				iUsed.StartTimer( 30000, 1, true ); // Puts in a delay of 30 seconds until next time coconuts respawn
			}
		}
	}
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag("CoconutCounter", 5);
		iUsed.SetTag("Coconuts", 1);
	}
}