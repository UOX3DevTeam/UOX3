// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// use wheat : target flour mill : get flour

function onUse( pUser, iUsed ) 
{
	// get users socket
	var srcSock = pUser.socket;

	// is it in users pack?
	var iPackOwner = GetPackOwner( iUsed, 0 );
	if( iPackOwner != pUser )
	{
		srcSock.SysMessage( "This has to be in your backpack!" );
		return;
	}

	var countOfResource = pUser.ResourceCount( 0x1EBD );	// item ID
	if( countOfResource < 4 )
	{
		srcSock.SysMessage( "You do not have enough resources!" );
		return;
	}
	// let the user target the mill
	srcSock.CustomTarget( 0, "Where do you wish to grind the wheat?" );
}

function onCallback0( tSock, targSerial )
{
	var tItem = CalcTargetedItem( tSock );
	var tChar = tSock.currentChar;

	if( tItem == null )
	{
		tSock.SysMessage( "You didn't target anything." );
		return;
	}

	var isInRange = tChar.InRange( tItem, 6 );

	if( !isInRange ) 
	{
		tSock.SysMessage( "You are too far away to reach that!" );
		return;
	}

	// find out if the item is in someone elses pack
	var iPackOwner = GetPackOwner( tItem, 0 );
	if( iPackOwner != null && iPackOwner != tChar )
	{
		tSock.SysMessage( "You cannot use things in other people's packs!" );
		return;
	}

	if( iPackOwner == null )	// on the ground
	{
		// if in a house check if its the one the player is in
		var persMulti = FindMulti( tChar );
		var itemMulti = FindMulti( tItem );

		if( persMulti != itemMulti )	// not in the same house
		{
			tSock.SysMessage( "You cannot reach that from here!" );
			return;
		}
	}

	var iID = tItem.id;
	var iColour = tItem.colour;
	if( iID != 0x1920 && iID != 0x1922 && iID != 0x192c && iID != 0x192E ) // is the item of the right type?
	{
		tSock.SysMessage( "That is not the right thing to use your resources on." );
		return;
	}

	var iMakeResource = tChar.ResourceCount( 0x1EBD );	// is there enough resources to use up to make it
	if( iMakeResource < 4 )
	{
		tSock.Sysmessage( "There is not enough base resources in your pack!" );
		return;
	}

	tItem.SoundEffect( 0x021D, true );

	tChar.UseResource( 4, 0x1EBD ); // uses up a resource (amount, item ID, item colour)

	var itemMade = SpawnItem( tSock, tChar, 0x1045, false );	// makes an item and puts in tChar's pack

	tSock.SysMessage( "You mill some wheat and put a sack of flour in your pack!" );
}
