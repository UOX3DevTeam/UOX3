// resources script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// pick apples

function onUse( pUser, iUsed ) 
{ 
	// get users socket
	var srcSock = pUser.socket;

	// is the item within range?
	var isInRange = pUser.InRange( iUsed, 4 );
	if( !isInRange ) 
	{
		srcSock.SysMessage( "You are too far away to reach that." );
		return;
	}

	// find out if the item is in someone elses pack
	var iPackOwner = GetPackOwner( iUsed, 0 );
	if( iPackOwner != null && iPackOwner != pUser )
	{
		srcSock.SysMessage( "You cannot use things in other people's packs!" );
		return;
	}

	if( iPackOwner == null )	// it's on the ground
	{
		// Not the most elegant solution, but it'll work
		var persMulti = FindMulti( pUser );
		var itemMulti = FindMulti( iUsed );

		if( persMulti != itemMulti )	// not in the same house
		{
			srcSock.SysMessage( "You cannot reach that from here!" );
			return;
		}
	}

	// make a sound
	iUsed.SoundEffect( 0x004F, true );
	// temporary disable the item
	DoTempEffect( 1, pUser, iUsed, 25, 10, 0, 0, iUsed );
	// give some resources
	var loot = RollDice( 1, 3, 0 ); 
	if( loot == 1 || loot == 2 )
	{
		srcSock.SysMessage( "You don't manage to pick apples." );
		return;
	}
	if( loot == 3 ) 
	{
		srcSock.SysMessage( "You pick an apple from the tree." );
		var itemMade = SpawnItem( srcSock, pUser, "0x09d0", false );	// makes an item and puts in Char's pack
		// disable item needs to be put in here
		return;
	}

}

