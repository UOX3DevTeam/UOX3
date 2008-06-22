// script for a deed for carpentered multi item
// 17/06/2001 Yeshe; yeshe@manofmystery.org

// TESTSCRIPT for positioning an item

function onUseChecked( pUser, iUsed )
{
	// get users socket
	var srcSock = CalcSockFromChar( pUser );

	// is the item within range?
	var isInRange = InRange( pUser, iUsed, 0, 1, 4 );
	if( !isInRange ) 
	{
		SysMessage( srcSock, "You are too far away to reach that." );
		return;
	}

	// find out if the item is in someone elses pack
	var iPackOwner = GetPackOwner( iUsed, 0 );
	if( iPackOwner != -1 && iPackOwner != pUser )
	{
		SysMessage( srcSock, "You cannot use things in other people's packs!" );
		return;
	}

	if( iPackOwner == -1 )	// it's on the ground
	{
		// Not the most elegant solution, but it'll work
		var persMulti = FindMulti( pUser.x, pUser.y, pUser.z );
		var itemMulti = FindMulti( iUsed.x, iUsed.y, iUsed.z );

		if( persMulti != itemMulti )	// not in the same house
		{
			SysMessage( srcSock, "You cannot reach that from here!" );
			return;
		}
	}

	// do some damage
	DoDamage( pUser, 5, 0 );
	pUser.DoAction( 0x0014 );
	// make a sound
	DoSoundEffect( iUsed, 1, 0x0231, true );

	// temporary disable the item
	DoTempEffect( 1, pUser, iUsed, 25, 1000, 0, 0, iUsed );

	// give some resources
	var loot = RollDice( 1, 3, 0 ); 
	if( loot == 1 )
	{
		SysMessage( srcSock, "You don't manage to steal someting from the bees." );
		return;
	}
	if( loot == 2 )
	{
		SysMessage( srcSock, "You manage to steal some wax from the bees." );
		var itemMade = SpawnItem( srcSock, pUser, 0x1422, false );	// makes an item and puts in tChar's pack
		return;
	}
	if( loot == 3 ) 
	{
		SysMessage( srcSock, "You steal wax and some honey from the bees." );
		var itemMade = SpawnItem( srcSock, pUser, 0x1422, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( srcSock, pUser, 0x1422, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( srcSock, pUser, 0x09ec, false );	// makes an item and puts in tChar's pack
		return;
	}

}

