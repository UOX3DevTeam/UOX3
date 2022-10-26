// Banana-Picking Script
// 20/02/2006 Xuri; xuri@uox3.org
// When a (dynamic) banana tree is double-clicked, it's setup with
// 5 bananas ripe for picking. After they've been picked, a timer starts,
// and until it's up no more bananas can be picked. Once the timer is over,
// new bananas are added.
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

	if( !iUsed.GetTag( "initialized" )) // Unless bananas have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks tree as initialized
		iUsed.SetTag( "Bananas", 1 ); 		// If set to 1, there are bananas to be picked, if 0 there are no ripe bananas
		iUsed.SetTag( "BananaCounter", maxResource ); 	// Add 5 bananas to the tree initially
	}
	var bananas = iUsed.GetTag("Bananas");
	var bananaCount = iUsed.GetTag("BananaCounter");
	if( bananas == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 2506, pUser.socket.language )); // You find no ripe bananas to pick. Try again later.
	}
	if( bananas == 1 )
	{
		iUsed.SoundEffect( 0x004F, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2507, pUser.socket.language )); // You fail to pick any bananas.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 2508, pUser.socket.language )); // You pick a banana from the tree.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x171f", 1, "ITEM", true );
			bananaCount--;
			iUsed.SetTag( "BananaCounter", bananaCount );
			if( bananaCount == 1 )
			{
				pUser.SysMessage( GetDictionaryEntry( 2509, pUser.socket.language )); // There is 1 ripe bananas left on the tree.
			}
			else
			{
				var bananaCountMsg = GetDictionaryEntry( 2510, pUser.socket.language ); // There are %i ripe bananas left on the tree.
				pUser.SysMessage( bananaCountMsg.replace( /%i/gi, bananaCount ));
			}
		    if( bananaCount == 0 )
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
		iUsed.SetTag( "BananaCounter", maxResource );
		iUsed.SetTag( "Bananas", 1 );
	}
}