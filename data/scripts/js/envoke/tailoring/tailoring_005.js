// tailoring script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// five balls of yarn : loom : bolt of cloth

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
	CustomTarget( srcSock, 0, "Where do you want to weave the yarn on?" );
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
		var persMulti = FindMulti( GetX( tChar, 0 ), GetY( tChar, 0 ), GetZ( tChar, 0 ) );
		var itemMulti = FindMulti( GetX( tItem, 1 ), GetY( tItem, 1 ), GetZ( tItem, 1 ) );

		if( persMulti != itemMulti )	// not in the same house
		{
			SysMessage( tSock, "You cannot reach that from here!" );
			return;
		}
		// remove five balls of yarn
		var iMakeResource1 = GetResourceCount( tChar, 0x0E1D, 0 );	// is there enough resources to use up to make it
		var iMakeResource2 = GetResourceCount( tChar, 0x0E1E, 0 );	// is there enough resources to use up to make it
		var iMakeResource3 = GetResourceCount( tChar, 0x0E1F, 0 );	// is there enough resources to use up to make it
		if( (iMakeResource1 < 5) && (iMakeResource2 < 5) && (iMakeResource3 < 5) )
		{
			SysMessage( tSock, "You dont seem to have enough yarn!" );
			return;
		}
		if( iMakeResource1 > 4 )
		{
			UseResource( tChar, 0x0E1D, 0, 5 );
		}
		else if( iMakeResource2 > 4 )
		{
			UseResource( tChar, 0x0E1E, 0, 5 );
		}
		else
		{
			UseResource( tChar, 0x0E1F, 0, 5 );
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

	SysMessage( tSock, "That is not a thing to use yarn on." );

}
