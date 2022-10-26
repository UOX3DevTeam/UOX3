// Water Resource Script
// 17/08/2002 Cav; tbacavalier@bigpond.com
// For use only with the UOX3 emulator or any other with the authors permission
// Target resource, fill water pitcher

function onUseChecked( pUser, iUsed )
{
	//var iPackOwner = GetPackOwner( iUsed, 0 );
	//if( iPackOwner != pUser )
	//{
	//	pUser.socket.SysMessage( "This has to be in your backpack!" );
	//	return;
	//}
	pUser.socket.CustomTarget( 0, "Where do you want to fill the pitcher from?" );
	return false;
}

function onCallback0( tSock, targSerial )
{
	var tItem = CalcItemFromSer( targSerial );
	var tChar = CalcCharFromSock( tSock );
	if( tItem == -1 )
	{
		tSock.SysMessage( "You didn't target anything." );
		return;
	}
	tSock.SysMessage( tItem );
   	var iID =  tItem.id;
	tSock.SysMessage( iID );
    if( iID == 0x0b41 || iID == 0x0b42 || iID == 0x0b43 || iID == 0x0b44 || iID == 0x0e7b || iID == 0x1008 || iID == 0x154d || iID == 0x1738 || iID == 0x1739 || iID == 0x173b || iID == 0x173c || iID == 0x173d || iID == 0x173e || iID == 0x173f || iID == 0x1740 || iID == 0x19ca || iID == 0x19cb || iID == 0x19cd || iID == 0x19ce || iID == 0x19cf || iID == 0x19d0 || iID == 0x19d1 || iID == 0x19d2 )
	{
		var isInRange = tChar.InRange( tItem, 4 );
		if( !isInRange )
 		{
			tSock.SysMessage( "You are too far away to reach that." );
			return;
		}
		var persMulti = FindMulti( tChar );
		var itemMulti = FindMulti( tItem );
		if( persMulti != itemMulti )
		{
			tSock.SysMessage( "You cannot reach that from here!" );
			return;
		}
		tItem.DoSoundEffect( 0x004E, true );
		tChar.UseResource( 0x0ff6, 0, 1 );
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0ff9", 1, "ITEM", true );
		tSock.SysMessage( "You fill the pitcher with water." );
		return;
	}
	tSock.SysMessage( "That is not a thing to fill pitchers of." );
}