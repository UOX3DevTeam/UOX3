// tailoring script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// five spools of thread : loom : bolt of cloth

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

	// let the user target the loom
	CustomTarget( srcSock, 0, "Where do you want to weave the thread on?" );
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

    // In case its a loom
   	var iID = tItem.id;
	if( iID >= 0x105F && iID <= 0x1066 )
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
        // remove five spools of thread
    	var iMakeResource1 = GetResourceCount( tChar, 0x0fA0, 0 );	// is there enough resources to use up to make it
    	var iMakeResource2 = GetResourceCount( tChar, 0x0fA1, 0 );	// is there enough resources to use up to make it
    	if( (iMakeResource1<5) && (iMakeResource2<5) )
	    {
		    SysMessage( tSock, "You dont seem to have enough thread!" );
    		return;
	    }
	    if( iMakeResource1 > 4 )
	    {
        	UseResource( tChar, 0x0fa0, 0, 5 );
        }
        else
        {
        	UseResource( tChar, 0x0fa1, 0, 5 );
        }
        
        DoSoundEffect( tItem, 1, 0x018F, true );
        // add bolt of cloth
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
	    var itemMade = SpawnItem( tSock, tChar, 0x175F, false );	// makes an item and puts in tChar's pack
        SysMessage( tSock, "You weave some folded cloth." );
        return;
	}

	SysMessage( tSock, "That is not a thing to use thread on." );

}
