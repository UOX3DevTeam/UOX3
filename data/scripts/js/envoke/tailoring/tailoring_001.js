// tailoring script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// scissors : unshorn sheep : unspun wool

function onUse ( pUser, iUsed ) 
{
	// get users socket
	var srcSock = CalcSockFromChar( pUser );

	// is it in users pack?
	var iCont = GetCont( iUsed );
	var iPackOwner = GetPackOwner( iCont, 0 );
	if( iPackOwner != pUser )
	{
		SysMessage( srcSock, "This has to be in your backpack!" );
		return;
	}

	// let the user target the heat source
	CustomTarget( srcSock, 0, "Where do you want to use the scissors on?" );
}

function onCallback0( tSock, targSerial )
{
	//var tItem = CalcTargetedItem( tSock );
	var tChar = CalcCharFromSock( tSock );
    //var tItem = GetTarget( tChar );
    var tItem = CalcCharFromSer( targSerial );
	
	if( tItem == -1 )
	{
		SysMessage( tSock, "You didn't target anything." );
		return;
	}

   	var iID = tItem.id;

    // In case its a sheep
    
	if( iID == 0x00cf )
	{
        // check if its in range
    	var isInRange = InRange( tChar, tItem, 0, 0, 4 );
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
        DoSoundEffect( tItem, 0, 0x0248, true );
        // set to sheered sheep
        tItem.id = 0x00DF;
        // set a timer on the sheep to let the wool grow again
        tItem.scripttrigger = 5001;
        StartTimer( tItem, 300, 0, 0 );
        // give the player some wool
	    var itemMade = SpawnItem( tSock, tChar, 0x0df8, false );	// makes an item and puts in tChar's pack
        SysMessage( tSock, "You shear some wool form the sheep." );
        return;
	}

	SysMessage( tSock, "That is not a thing to use scissors on." );
}

function onTimer( tChar, timerID )
{
    if( timerID == 0 )
    {
        tChar.id = 0x00CF;
    }
}

