// Honey/Wax-Picking Script
// Originally written by Cav
// Rewritten by Xuri 24/01/2005 ;xuri@uox3.org
// When a (dynamic) bee-hive is double-clicked, there are a number of possible results.
// The user may or may not manage to harvest wax or honey, and he/she may or may not
// manage to avoid being stung by bees =)
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

	if( !iUsed.GetTag( "initialized")) // Unless honey has been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks hive as initialized
		iUsed.SetTag( "Honey", 1 ); 			// If set to 1, there is honey to be harvested, if 0 there is no honey ready for harvesting
		iUsed.SetTag( "HoneyCounter", maxResource ); 	// Add 5 honey to the hive initially
	}

	var honey = iUsed.GetTag( "Honey" );
	var honeyCount = iUsed.GetTag( "HoneyCounter" );

	if( honey == 0 )
	{
		pUser.SysMessage( GetDictionaryEntry( 2515, pUser.socket.language )); // You find no ripe honey to collect. Try again later.
	}
	else
	{
		var loot = RollDice( 1, 4, 0 );
		if( loot == 1 )
		{
			pUser.socket.SysMessage( GetDictionaryEntry( 2511, pUser.socket.language )); // You fail to grab anything in the beehive, but you avoid being stung.
		}
		else if( loot == 2 )
		{
			pUser.socket.SysMessage( GetDictionaryEntry( 2512, pUser.socket.language )); // You fail to grab anything in the beehive, and you get some nice new bee-sting-marks!
			pUser.Damage( 10 );
			pUser.DoAction( 0x0014 );
			iUsed.SoundEffect( 0x0231, true );
		}
		else if( loot == 3 )
	 	{
			pUser.socket.SysMessage( GetDictionaryEntry( 2513, pUser.socket.language )); // You manage to grab some wax and honey from the beehive without getting stung.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1422", 1, "ITEM", true );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09ec", 1, "ITEM", true );
			honeyCount--;
			iUsed.SetTag( "HoneyCounter", honeyCount );
			if( honeyCount == 1)
			{
				pUser.SysMessage( GetDictionaryEntry( 2516, pUser.socket.language )); // There is very few collectable resources left in the beehive.
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 2517, pUser.socket.language )); // There are still some collectable resources left in the beehive.
			}
		}
		else if( loot == 4 )
		{
			pUser.socket.SysMessage(GetDictionaryEntry( 2514, pUser.socket.language )); // You manage to grab some wax and honey from the beehive, but fail to avoid getting stung.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1422", 1, "ITEM", true );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09ec", 1, "ITEM", true );
			honeyCount--;
			iUsed.SetTag( "HoneyCounter", honeyCount );
			if( honeyCount == 1)
			{
				pUser.SysMessage( GetDictionaryEntry( 2516, pUser.socket.language )); // There is very few collectable resources left in the beehive.
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 2517, pUser.socket.language )); // There are still some collectable resources left in the beehive.
			}
			pUser.Damage( 10 );
			pUser.DoAction( 0x0014 );
			iUsed.SoundEffect( 0x0231, true );
		}

		if( honeyCount == 0 )
		{
			iUsed.SetTag( "Honey", 0 );
			iUsed.StartTimer( resourceGrowthDelay, 1, true ); // Puts in a delay until next time honey respawn
		}
	}
	return false;
}


function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		iUsed.SetTag( "HoneyCounter", maxResource );
		iUsed.SetTag( "Honey", 1 );
	}
}