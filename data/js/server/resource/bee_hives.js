// Honey/Wax-Picking Script
// Originally written by Cav
// Rewritten by Xuri 19/02/2003 ;xuri@sensewave.com
// When a (dynamic) bee-hive is double-clicked, there are a number of possible results.
// The user may or may not manage to harvest wax or honey, and he/she may or may not
// manage to avoid being stung by bees =)

function onUse( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( "You are too far away to reach that." );
		return;
	}

	var loot = RollDice( 1, 4, 0 );
	if( loot == 1 ) 
	{
		pUser.socket.SysMessage( "You fail to grab anything in the beehive, but you avoid being stung." );
		return;
	}
	if( loot == 2 ) 
	{
		pUser.socket.SysMessage( "You fail to grab anything in the beehive, and you get some nice new bee-sting-marks!" );
		//DoDamage( pUser, 40, 1 );
		pUser.DoAction( 0x0014 );
		iUsed.SoundEffect( 0x0231, true );
		return;
	}
	if( loot == 3 )
 	{
		pUser.socket.SysMessage( "You manage to grab some wax and honey from the behive without getting stung." );
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1422", false, 1, true, true );
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09ec", false, 1, true, true );
		
		return;
	}
	if( loot == 4 )
	{
		pUser.socket.SysMessage( "You manage to grab some wax and honey from the beehive, but fail to avoid getting stung." );
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1422", false, 1, true, true );
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x09ec", false, 1, true, true );
		//DoDamage( pUser, 40, 1 );
		pUser.DoAction( 0x0014 );
		iUsed.SoundEffect( 0x0231, true );
		return;
	}

}