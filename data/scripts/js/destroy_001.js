// This script is not functioning correctly and is not used
// It is thought solely for educational purposes

// destroy furniture script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// use axe : target furniture : destroy it when its not locked down

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
	CustomTarget( srcSock, 0, "What do you want to use the axe on?" );
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

    // !! check if its locked down !!
    var iMov = tItem.movable;
    if( iMov == 3 )
	{
		SysMessage( tSock, "You cannot destroy an item that is locked down!" );
		return;
	}


    if( tItem.id == 0x0B3A ) 
	{
		SysMessage( tSock, "Identified small table" );
    }
    
    DoSoundEffect( tChar, 0, 0x0134, true );

}
