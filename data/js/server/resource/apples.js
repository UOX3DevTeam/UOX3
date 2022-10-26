// Apple-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When a (dynamic) apple tree is double-clicked, it's setup with
// 5 apples ripe for picking. After they've been picked, a timer starts,
// and until it's up no more apples can be picked. Once the timer is over,
// new apples are added. The appearance of the tree indicates whether or
// not there are any apples left to pick.
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
	if( !iUsed.GetTag( "initialized" )) // Unless apples have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 			 // Marks tree as initialized
		iUsed.SetTag( "Apples", 1 ); 				 // If set to 1, there are apples to be picked, if 0 there are no ripe apples
		iUsed.SetTag( "AppleCounter", maxResource ); // Add 5 apples to the tree initially
	}
	var apples = iUsed.GetTag( "Apples" );
	var appleCount = iUsed.GetTag( "AppleCounter" );
	if( apples == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2501, pUser.socket.language )); // You find no ripe apples to pick. Try again later.
	}
	if( apples == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2502, pUser.socket.language )); // You fail to pick any apples.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2503, pUser.socket.language )); // You pick an apple from the tree.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09d0", 1, "ITEM", true );
			appleCount--;
			iUsed.SetTag( "AppleCounter", appleCount );
			if( appleCount == 1)
			{
				pUser.SysMessage( GetDictionaryEntry( 2504, pUser.socket.language )); // There is 1 ripe apple left on the tree.
			}
			else
			{
				var appleCountMsg = GetDictionaryEntry( 2505, pUser.socket.language ); // There are %i ripe apples left on the tree.
				pUser.SysMessage( appleCountMsg.replace( /%i/gi, appleCount ));
			}
		    if( appleCount == 0 )
			{
				if( iUsed.id == 0x0d96 )
				{
					iUsed.id = 0x0d95;
				}
				else if( iUsed.id == 0x0d9a )
				{
					iUsed.id = 0x0d99; 
				}
				iUsed.SetTag( "Apples", 0 );
				iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay until next time apples respawn
			}
		}
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag( "AppleCounter", maxResource );
		iUsed.SetTag( "Apples", 1 );
		if( iUsed.id == 0x0d95 )
		{
			iUsed.id = 0x0d96;
		}
		else if( iUsed.id == 0x0d99 )
		{
			iUsed.id = 0x0d9a; 
		}
	}
}