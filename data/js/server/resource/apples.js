// Apple-Picking Script
// 19/02/2003 Xuri; xuri@sensewave.com
// When a (dynamic) apple tree is double-clicked, it's setup with
// 5 apples ripe for picking. After they've been picked, a timer starts,
// and until it's up no more apples can be picked. Once the timer is over,
// new apples are added. The apperance of the tree indicates whether or
// not there are any apples left to pick.

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
		iUsed.SetTag("Apples",1); 		// If set to 1, there are apples to be picked, if 0 there are no ripe apples
		iUsed.SetTag("AppleCounter", 5); 	// Add 5 apples to the tree initially
	}
	var Apples = iUsed.GetTag("Apples");
	var AppleCount = iUsed.GetTag("AppleCounter");
	if (Apples == 0)
	{	
		pUser.SysMessage( "You find no ripe apples to pick. Try again later." );
	}
	if( Apples == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
			pUser.SysMessage( "You fail to pick any apples." );
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( "You pick an apple from the tree." );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09d0", false, 1, true, true );
			AppleCount--;
			iUsed.SetTag( "AppleCounter", AppleCount );
			if( AppleCount == 1)
				pUser.SysMessage( "There is "+AppleCount+" ripe apple left on the tree." );
			else
				pUser.SysMessage( "There are "+AppleCount+" ripe apples left on the tree." );
		    	if( AppleCount == 0 )
			{
				if( iUsed.id == 0x0d96 )
					iUsed.id = 0x0d95;
				else if( iUsed.id == 0x0d9a )
					iUsed.id = 0x0d99; 
				iUsed.SetTag( "Apples", 0 );
				iUsed.StartTimer( 30000, 1, true ); // Puts in a delay of 30 seconds until next time apples respawn
			}
		}
	}
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag("AppleCounter", 5);
		iUsed.SetTag("Apples", 1);
		if( iUsed.id == 0x0d95 )
			iUsed.id = 0x0d96;
		else if( iUsed.id == 0x0d99 )
			iUsed.id = 0x0d9a; 
	}
}