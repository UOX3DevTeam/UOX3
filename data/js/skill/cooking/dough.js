// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// use flour : target pitcher of water : get dough

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

	// let the user target the water pitcher
	srcSock.CustomTarget( 0, "Which pitcher of water to use?" );
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

	// is it in users pack?
	var iPackOwner = GetPackOwner( tItem, 0 );
	if( iPackOwner != tChar )
	{
		tSock.SysMessage( "This has to be in your backpack!" );
		return;
	}

	var iID = tItem.id;
	var iColour = tItem.colour;
	if( iID != 0x0FF8 && iID != 0x0FF9 ) // is the item of the right type?
	{
		tSock.SysMessage( "That is not a pitcher of water." );
		return;
	}

	var iMakeResource = tChar.ResourceCount( 0x1045 );   // is there enough resources to use up to make it
	if( iMakeResource < 1 )
	{
		tSock.SysMessage( "There is not enough flour in your pack!" );
		return;
	}

 	tChar.UseResource( 1, 0x1045 ); // uses up a resource (amount, item ID, item colour)

	if( !tChar.CheckSkill( 13, 1, 200 ) )	// character to check, skill #, minimum skill, and maximum skill
	{
		tSock.SysMessage( "You tried to make dough but failed." );
		return;
	}
	// CheckSkill automatically raises skill levels

	if( iID == 0x0FF8 )
	{
		tItem.id = 0x0FF7;
	}
	if( iID == 0x0ff9 )
	{
		tItem.id = 0x0FF6;
	}

	tChar.SoundEffect( 0x0134, true );

	var itemMade = SpawnItem( tSock, tChar, 0x103d, false );	// makes an item and puts in tChar's pack
}
