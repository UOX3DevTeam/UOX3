// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// knives : fish : raw fish steak

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

	// let the user target something
	CustomTarget( srcSock, 0, "What do you want to use the knife on?" );
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

	// In case its a fish
	if( iID >= 0x09CC && iID <= 0x09CF )
	{
		// is it in users pack?
		var iPackOwner = GetPackOwner( tItem, 0 );
		if( iPackOwner != tChar )
		{
			SysMessage( tSock, "This has to be in your backpack!" );
			return;
		}

		// remove one fish
		if( iID == 0x09CC )
		{
			UseResource( tChar, 0x09CC, 0, 1 ); // uses up a resource (character, item ID, item colour, amount)
		}
		if( iID == 0x09CD )
		{
			UseResource( tChar, 0x09CD, 0, 1 ); // uses up a resource (character, item ID, item colour, amount)
		}
		if( iID == 0x09CE )
		{
			UseResource( tChar, 0x09CE, 0, 1 ); // uses up a resource (character, item ID, item colour, amount)
		}
		if( iID == 0x09CF )
		{
			UseResource( tChar, 0x09CF, 0, 1 ); // uses up a resource (character, item ID, item colour, amount)
		}
		DoSoundEffect( tChar, 0, 0x018E, true );

		var itemMade = SpawnItem( tSock, tChar, 0x097a, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( tSock, tChar, 0x097a, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( tSock, tChar, 0x097a, false );	// makes an item and puts in tChar's pack
		var itemMade = SpawnItem( tSock, tChar, 0x097a, false );	// makes an item and puts in tChar's pack
		SysMessage( tSock, "You slice a fish to steaks." );
		return;
	}

	SysMessage( tSock, "You dont know how to use your knife on that." );
}
