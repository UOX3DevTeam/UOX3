// tailoring script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// flax bundle : spinning wheel : six spools of thread

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

	// let the user target the spinning wheel
	CustomTarget( srcSock, 0, "Where do you want to spin the flax on?" );
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

    // In case its a spinning wheel
	var iID = tItem.id;
	if( iID == 0x1015 || iID == 0x1019 || iID == 0x101C || iID == 0x10A4 )
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
		// remove one flax bundle
		var iMakeResource1 = GetResourceCount( tChar, 0x1A9C, 0 );	// is there enough resources to use up to make it
		var iMakeResource2 = GetResourceCount( tChar, 0x1A9D, 0 );	// is there enough resources to use up to make it
		if( (iMakeResource1+iMakeResource2) < 1 )
		{
			SysMessage( tSock, "You dont seem to have any flax!" );
			return;
		}
		if( iMakeResource1 > 0 )
		{
			UseResource( tChar, 0x1A9C, 0, 1 );
		}
		else
		{
			UseResource( tChar, 0x1A9D, 0, 1 );
		}
		DoSoundEffect( tItem, 1, 0x021A, true );
		// add spools of thread
		var itemMade = SpawnItem( tSock, tChar, 0x0fA0, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( tSock, tChar, 0x0fA0, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( tSock, tChar, 0x0fA0, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( tSock, tChar, 0x0fA0, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( tSock, tChar, 0x0fA0, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( tSock, tChar, 0x0fA0, false );	// makes an item and puts in tChar's pack
		SysMessage( tSock, "You spin some spools of thread." );
		return;
	}

	SysMessage( tSock, "That is not a thing to use flax on." );

}
