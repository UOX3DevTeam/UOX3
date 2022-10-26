function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
		}
		else
		{
			var iMulti = pUser.multi;
			if( ValidateObject( iMulti ) && ( iMulti.IsOnOwnerList( pUser )
				|| ( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ) && iMulti.owner.accountNum == pUser.accountNum )))
			{
				var ret = displaygump(iUsed, pUser);
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 2067, socket.language )); // You must be in your house to do this.
			}
		}
	}
	return false;
}

function displaygump( socket, pUser )
{
	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 200, 200, 2600 );
	myGump.AddButton( 50, 45, 0x868, 1, 0, 1 );
	myGump.AddXMFHTMLGump( 90, 50, 70, 40, 1018323, false, false );//turn
	myGump.AddButton( 50, 95, 0x868, 1, 0, 2 );
	myGump.AddXMFHTMLGump( 90, 100, 70, 40, 1018324, false, false );//up
	myGump.AddButton( 50, 145, 0x868, 1, 0, 3 );
	myGump.AddXMFHTMLGump( 90, 150, 70, 40, 1018325, false, false );//down
	myGump.Send( pUser );
	myGump.Free();
	return false;
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var iMulti = pUser.multi;
	if( ValidateObject( iMulti ) && ( iMulti.IsOnOwnerList( pUser )
		|| ( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ) && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
	{
		switch( pButton )
		{
			case 0:
				break;//close
			case 1: // Turn
	  			var targMsg3 = GetDictionaryEntry( 2068, socket.language ); // Select an object to turn.
	    		socket.CustomTarget( 3, targMsg3 );
				var ret = displaygump( socket, pUser );
				break;
			case 2: // Up
				var targMsg2 = GetDictionaryEntry( 2069, socket.language ); // Select an object to increase its height.
	    		socket.CustomTarget( 2, targMsg2 );
				var ret = displaygump( socket, pUser );
				break;
			case 3: // Down
				var targMsg1 = GetDictionaryEntry( 2070, socket.language ); // Select an object to lower its height.
	    		socket.CustomTarget( 1, targMsg1 );
				var ret = displaygump( socket, pUser );
				break;
			default:
				break;
		}
	}
    else
        pUser.SysMessage( GetDictionaryEntry( 2067, socket.language )); // You must be in your house to do this.
}

function onCallback1( socket, ourObj )
{
	var tChar = socket.currentChar;
	if( !ValidateObject( ourObj ) || !ourObj.isItem || ourObj.movable != 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 2072, socket.language )); // You can only use the interior decorator on locked down items.
		return;
	}

	var iMulti = tChar.multi;
	if( !ValidateObject( iMulti )
		|| ( !iMulti.IsOnOwnerList( tChar )
			&& (( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ) && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != tChar.accountNum ))
				|| ( !GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" )))))
	{
		tChar.SysMessage( GetDictionaryEntry( 2067, socket.language )); // You must be in your house to do this.
		return;
	}

	var objHeight = ourObj.z + GetTileHeight( ourObj.id );
	var iMultiZ = iMulti.z;
	if( ourObj.z == -128 || ourObj.z == ( iMultiZ + 7 ) || ourObj.z == ( iMultiZ + 27 ) || ourObj.z == ( iMultiZ + 47 ) || ourObj.z == ( iMultiZ + 67 )
		|| objHeight == -128 || objHeight == ( iMultiZ + 7 ) || objHeight == ( iMultiZ + 27 ) || objHeight == ( iMultiZ + 47 ) || objHeight == ( iMultiZ + 67 ))
	{
		socket.SysMessage( GetDictionaryEntry( 2073, socket.language )); // You cannot lower it down any further.
		return;
	}

	if( ourObj.weight >= 10000 )
	{
		socket.SysMessage( GetDictionaryEntry( 1551, socket.language )); // That is too heavy.
		return;
	}

	socket.CustomTarget( 1 );
	if( !socket.GetWord( 1 ))
	{
		ourObj.z -= 1;
	}
}

function onCallback2( socket, ourObj )
{
	var tChar = socket.currentChar;
	if( !ValidateObject( ourObj ) || !ourObj.isItem || ourObj.movable != 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 2072, socket.language )); // You can only use the interior decorator on locked down items.
		return;
	}

	var iMulti = ourObj.multi;
	if( !ValidateObject( iMulti )
		|| ( !iMulti.IsOnOwnerList( tChar )
			&& (( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ) && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != tChar.accountNum ))
				|| ( !GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" )))))
	{
		tChar.SysMessage( GetDictionaryEntry( 2067, socket.language )); // You must be in your house to do this.
		return;
	}

	var objHeight = ourObj.z + GetTileHeight( ourObj.id );
	var iMultiZ = iMulti.z;
	if( ourObj.z == 127 || ourObj.z == ( iMultiZ + 22 ) || ourObj.z == ( iMultiZ + 44 ) || ourObj.z == ( iMultiZ + 66 ) || ourObj.z == ( iMultiZ + 88 )
		|| objHeight == 127 || objHeight == ( iMultiZ + 22 ) || objHeight == ( iMultiZ + 44 ) || objHeight == ( iMultiZ + 66 ) || objHeight == ( iMultiZ + 88 ))
	{
		socket.SysMessage( GetDictionaryEntry( 2074, socket.language )); // You cannot raise it up any higher.
		return;
	}

	if( ourObj.weight >= 10000 )
	{
		socket.SysMessage( GetDictionaryEntry( 1551, socket.language )); // That is too heavy.
		return;
	}

	socket.CustomTarget( 2 );
	if( !socket.GetWord( 1 ))
	{
		ourObj.z += 1;
	}
}

function onCallback3( socket, ourObj )
{
	var tChar = socket.currentChar;
	if( !ValidateObject( ourObj ) || !ourObj.isItem || ourObj.movable != 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 2072, socket.language )); // You can only use the interior decorator on locked down items.
		return;
	}

	var iMulti = ourObj.multi;
	if( !ValidateObject( iMulti )
		|| ( !iMulti.IsOnOwnerList( tChar )
			&& (( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ) && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != tChar.accountNum ))
				|| ( !GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" )))))
	{
		tChar.SysMessage( GetDictionaryEntry( 2067, socket.language )); // You must be in your house to do this.
		return;
	}

	if( ourObj.weight >= 10000 )
	{
		socket.SysMessage( GetDictionaryEntry( 1551, socket.language )); // That is too heavy.
		return;
	}

	socket.CustomTarget( 3 );
	TriggerEvent( 50, "SmartTurn", ourObj ); // Trigger SmartTurn() function in furniture_smartturn.js
}