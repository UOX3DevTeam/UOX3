// This script is not functioning correctly and is not used
// It is thought solely for educational purposes

// destroy furniture script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// use axe : target furniture : destroy it when its not locked down
// 09/02/2003 Updated by Xuri; xuri@sensewave.com

function onUseChecked( pUser, iUsed ) 
{
	// is it in users pack?
	var iPackOwner = GetPackOwner( iUsed, 0 );
	if( iPackOwner.serial != pUser.serial )
	{
		pUser.SysMessage( "This has to be in your backpack!" );
	}
	else
	{
		pUser.CustomTarget( 0, "What do you want to use the axe on?" );
	}
}

function onCallback0( tSock, tItem )
{
	var tChar = tSock.currentChar;
	if( !ValidateObject( tItem ) )
	{
		tSock.SysMessage( "You didn't target anything." );
		return;
	}
	if( !tItem.isItem )
		return;

    // check if its in range
	var isInRange = tChar.InRange( tItem, 4 );
	if( !isInRange )
	{
		tSock.SysMessage( "You are too far away to reach that." );
		return;
	}

    // check if its in someone elses house
	var persMulti = FindMulti( tChar );
	var itemMulti = FindMulti( tItem );
	if( persMulti.owner.serial != itemMulti.owner.serial )	// not in the same house
	{
		tSock.SysMessage( "You cannot reach that from here!" );
		return;
	}

   	// !! check if its locked down !!
   	var iMov = tItem.movable;
	if(( iMov == 3 ) && ( tItem.type != 87 ))
	{
		tSock.SysMessage( "You cannot destroy an item that is locked down!" );
		return;
	}

	tChar.SoundEffect( 0x139, true );
	tItem.Delete();
}
