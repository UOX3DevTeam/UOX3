// Coconut-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When a (dynamic) coconut palm is double-clicked, it's setup with
// 5 coconuts ripe for picking. After they've been picked, a timer starts,
// and until it's up no more coconuts can be picked. Once the timer is over,
// new coconuts are added.
var resourceGrowthDelay = 120000; //Delay in milliseconds before resources respawns
var maxResource = 5; //maximum amount of resources on a given item

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); //You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless apples have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks tree as initialized
		iUsed.SetTag( "Coconuts", 1 ); 		// If set to 1, there are apples to be picked, if 0 there are no ripe apples
		iUsed.SetTag( "CoconutCounter", maxResource ); 	// Add 5 apples to the tree initially
	}
	var coconuts = iUsed.GetTag( "Coconuts" );
	var coconutCount = iUsed.GetTag( "CoconutCounter" );
	if( coconuts == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2518, pUser.socket.language )); // You find no coconuts to pick. Try again later.
	}
	if( coconuts == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2519, pUser.socket.language )); // You fail to pick any coconuts.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2520, pUser.socket.language )); // You pick a coconut from the palm-tree.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1726", 1, "ITEM", true );
			coconutCount--;
			iUsed.SetTag( "CoconutCounter", coconutCount );
			if( coconutCount == 1)
			{
				pUser.SysMessage( GetDictionaryEntry( 2521, pUser.socket.language )); // There is 1 coconut left on the tree.
			}
			else
			{
				var coconutCountMsg = GetDictionaryEntry( 2522, pUser.socket.language ); // There are %i coconuts left on the tree.
				pUser.SysMessage( coconutCountMsg.replace( /%i/gi, coconutCount ));
			}
		    if( coconutCount == 0 )
			{
				iUsed.SetTag( "Coconuts", 0 );
				iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay of 30 seconds until next time coconuts respawn
			}
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag( "CoconutCounter", maxResource );
		iUsed.SetTag( "Coconuts", 1 );
	}
}