// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/rewrote the script
// Updated to use dictionary messages
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
			srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
		else
		{
			var countOfResource = pUser.ResourceCount( 0x1EBD );	// item ID
			if( countOfResource < 4 )
			{
				srcSock.SysMessage( GetDictionaryEntry( 6080, srcSock.language )); // You do not have enough resources! You need 4 sheaves of wheat!
			}
			else
				srcSock.CustomTarget( 0, GetDictionaryEntry( 6081, srcSock.language )); // Where do you wish to grind the wheat?
		}
	}
	else
		srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.

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
		tSock.SysMessage( GetDictionaryEntry( 6082, tSock.language )); // You cannot grind your wheat on that.
		return;
	}
	// Target is a Dynamic or Static Item
	if( tileID == 0x188b || tileID == 0x188c || ( tileID >= 0x1920 && tileID <= 0x1923 ) || ( tileID >= 0x1925 && tileID <= 0x1927 ) ||
	( tileID >= 0x192c && tileID <= 0x192f ) || ( tileID >= 0x1931 && tileID <= 0x1933 ))
	{	// In case its a flour mill
		// check if its in range
		if(( pUser.x > targX + 3 ) || ( pUser.x < targX - 3 ) || ( pUser.y > targY + 3 ) || ( pUser.y < targY - 3 ) || ( pUser.z > targZ + 10 ) || ( pUser.z < targZ - 10 ))
		{
			tSock.SysMessage( GetDictionaryEntry( 393, tSock.language )); // That is too far away.
			return;
		}
		// remove 4 sheaves of wheat
		var iMakeResource = pUser.ResourceCount( 0x1EBD );// is there enough resources to use up to make it
		if( iMakeResource < 4 )
		{
			tSock.SysMessage( GetDictionaryEntry( 6080, tSock.language )); // You do not have enough resources! You need 4 sheaves of wheat!
			return;
		}
		pUser.UseResource( 4, 0x1EBD ); // uses up a resource (amount, item ID, item colour)
		pUser.SoundEffect( 0x021e, true );
		var itemMade = CreateDFNItem( tSock, pUser, "0x1045", 1, "ITEM", true ); // makes a sack of flour
		tSock.SysMessage( GetDictionaryEntry( 6083, tSock.language )); // You grind some wheat and put a sack of flour in your pack!
		return;
	}
}
