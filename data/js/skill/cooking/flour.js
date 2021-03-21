// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/rewrote the script
// use wheat : target flour mill : get flour

function onUseChecked ( pUser, iUsed )
{
	// get users socket
	var srcSock = pUser.socket;

	// is it in users pack?
	if( iUsed.container != null )
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner.serial != pUser.serial )
			pUser.SysMessage( "This has to be in your backpack!" );
		else
		{
			var countOfResource = pUser.ResourceCount( 0x1EBD );	// item ID
			if( countOfResource < 4 )
			{
				srcSock.SysMessage( "You do not have enough resources! You need 4 sheaves of wheat!" );
			}
			else
				srcSock.CustomTarget( 0, "Where do you wish to grind the wheat?" );
		}
	}
	else
		pUser.SysMessage( "This has to be in your backpack!" );

	return false;
}

function onCallback0( tSock, targSerial )
{
	var pUser = tSock.currentChar;
	var StrangeByte   = tSock.GetWord( 1 );
	var targX	= tSock.GetWord( 11 );
	var targY	= tSock.GetWord( 13 );
	var targZ	= tSock.GetSByte( 16 );
	var tileID	= tSock.GetWord( 17 );
	if( tileID == 0 || ( StrangeByte == 0 && targSerial.isChar ))
	{ //Target is a MapTile or a Character
		pUser.SysMessage("You cannot grind your wheat on that.");
		return;
	}
	// Target is a Dynamic or Static Item
	if( tileID == 0x188b || tileID == 0x188c || ( tileID >= 0x1920 && tileID <= 0x1923 ) || ( tileID >= 0x1925 && tileID <= 0x1927 ) ||
	( tileID >= 0x192c && tileID <= 0x192f ) || ( tileID >= 0x1931 && tileID <= 0x1933 ))
	{	// In case its a flour mill
		// check if its in range
		if(( pUser.x > targX + 3 ) || ( pUser.x < targX - 3 ) || ( pUser.y > targY + 3 ) || ( pUser.y < targY - 3 ) || ( pUser.z > targZ + 10 ) || ( pUser.z < targZ - 10 ))
		{
			pUser.SysMessage( "You are too far away from the target!" );
			return;
		}
		// remove 4 sheaves of wheat
		var iMakeResource = pUser.ResourceCount( 0x1EBD );// is there enough resources to use up to make it
		if( iMakeResource < 4 )
		{
			pUser.SysMessage( "You do not have enough resources! You need 4 sheaves of wheat!" );
			return;
		}
		pUser.UseResource( 4, 0x1EBD ); // uses up a resource (amount, item ID, item colour)
		pUser.SoundEffect( 0x021e, true );
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x1045", 1, "ITEM", true ); // makes a sack of flour
		pUser.SysMessage( "You grind some wheat and put a sack of flour in your pack!" );
		return;
	}
}
