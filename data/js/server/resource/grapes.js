// Grape-Picking Script
// 19/02/2003 Xuri; xuri@sensewave.com
// When (dynamic) grapevines are double-clicked, they're setup with
// 5 grapes ripe for picking. After they've been picked, a timer starts,
// and until it's up no more grapes can be picked. Once the timer is over,
// new grapes are added. The apperance of the tree indicates whether or
// not there are any grapes left to pick.

function onUse( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( "You are too far away to reach that." );
		return;
	}

	if( !iUsed.GetTag("initialized")) // Unless grapes have been picked before, initialize settings
	{
		iUsed.SetTag("initialized",true); 	// Marks tree as initialized
		iUsed.SetTag("Grapes",1); 		// If set to 1, there are grapes to be picked, if 0 there are no ripe grapes
		iUsed.SetTag("GrapeCounter", 5); 	// Add 5 grapes to the tree initially
	}
	var Grapes = iUsed.GetTag("Grapes");
	var GrapeCount = iUsed.GetTag("GrapeCounter");
	if (Grapes == 0)
	{	
		pUser.SysMessage( "You find no ripe grapebunches to pick. Try again later." );
	}
	if( Grapes == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
			pUser.SysMessage( "You fail to pick any grapebunches." );
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( "You pick a grapebunch from the tree." );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09d1", 1, "ITEM", true );
			GrapeCount--;
			iUsed.SetTag( "GrapeCounter", GrapeCount );
			if( GrapeCount == 1)
				pUser.SysMessage( "There is "+GrapeCount+" ripe grapebunch left on the tree." );
			else
				pUser.SysMessage( "There are "+GrapeCount+" ripe grapebunches left on the tree." );
		    	if( GrapeCount == 0 )
			{
				iUsed.SetTag( "Grapes", 0 );
				iUsed.StartTimer( 30000, 1, true ); // Puts in a delay of 30 seconds until next time grapes respawn
			}
		}
	}
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag("GrapeCounter", 5);
		iUsed.SetTag("Grapes", 1);
	}
}