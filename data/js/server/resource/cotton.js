// resources script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// pick cotton and consume the plant

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
	iUsed.SoundEffect( 0x004f, true );

	iUsed.Delete();

	// give some resources
	srcSock.SysMessage( "You harvest some cotton." );
	var itemMade = SpawnItem( srcSock, pUser, "0x0df9", false );	// makes an item and puts in Char's pack
}

