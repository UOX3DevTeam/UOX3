// resources script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// empty pitcher : watersource : filled waterpitcher

function onUse ( pUser, iUsed ) 
{
	// get users socket
	var srcSock = CalcSockFromChar( pUser );

	// is it in users pack?
	var iPackOwner = GetPackOwner( iUsed, 0 );
	if( iPackOwner != pUser )
	{
		SysMessage( srcSock, "This has to be in your backpack!" );
		return;
	}

	// let the user target the heat source
	CustomTarget( srcSock, 0, "Where do you want to fill the pitcher from?" );
}

function onCallback0( tSock, targSerial )
{
	var tItem = CalcTargetedItem( tSock );
	var tChar = CalcCharFromSock( tSock );

	if( tItem == -1 )
	{
		SysMessage( tSock, "You didn't target anything." );
		return;
	}

   	var iID = tItem.id;

    // In case its a watersource
    
	if( iID == 0x0b41 || iID == 0x0b42 || iID == 0x0b43 || iID == 0x0b44 || iID == 0x0e7b || iID == 0x1008 || iID == 0x154d || iID == 0x1738 || iID == 0x1739 || iID == 0x173b || iID == 0x173c || iID == 0x173d || iID == 0x173e || iID == 0x173f || iID == 0x1740 || iID == 0x19ca || iID == 0x19cb || iID == 0x19cd || iID == 0x19ce || iID == 0x19cf || iID == 0x19d0 || iID == 0x19d1 || iID == 0x19d2 )
	{
        // check if its in range
    	var isInRange = InRange( tChar, tItem, 0, 1, 4 );
	    if( !isInRange ) 
    	{
	    	SysMessage( tSock, "You are too far away to reach that!" );
		    return;
    	}
        // check if its in someone elses house
		var persMulti = FindMulti( tChar.x, tChar.y, tChar.z );
		var itemMulti = FindMulti( tItem.x, tItem.y, tItem.z );

		if( persMulti != itemMulti )	// not in the same house
		{
			SysMessage( tSock, "You cannot reach that from here!" );
			return;
		}
        DoSoundEffect( tItem, 1, 0x004E, true );
        UseResource( tChar, 0x0ff6, 0, 1 ); // uses up a resource (character, item ID, item colour, amount)
	    var itemMade = SpawnItem( tSock, tChar, 0x0ff9, false );	// makes an item and puts in tChar's pack
        SysMessage( tSock, "You fill the pitcher with water." );
        return;
	}

	SysMessage( tSock, "That is not a thing to fill pitchers of." );

}
