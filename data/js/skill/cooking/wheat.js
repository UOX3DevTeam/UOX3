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
	var tileID = tSock.GetWord(17);

	if( tileID == 0 || ( StrangeByte == 0 && targSerial.isChar ))
	{ //Target is a MapTile or a Character
		tSock.SysMessage( GetDictionaryEntry( 6082, tSock.language )); // You cannot grind your wheat on that.
		return;
	}

	if(TriggerEvent( 107, "FlourMill", tileID))
	{
		if (tileID == 0x1922)
		{
			AreaItemFunction("myAreaFunc", pUser, 4, targSerial);
			targSerial.StartTimer(100, 1, true);
		}

		if(TriggerEvent( 103, "RangeCheck", tSock, pUser ))
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
		pUser.SoundEffect(0x021e, true);
	}
}

function myAreaFunc(srcChar, trgItem, iUsed)
{
	if (trgItem.id == 0x1920)
		trgItem.StartTimer(100, 4, true);
}

function onTimer(targSerial, timerID)
{
	switch (timerID)
	{
		case 1:
			targSerial.id = 0x1923;
			targSerial.StartTimer(100, 2, true);
			break;
		case 2:
			targSerial.id = 0x1926;
			targSerial.StartTimer(2100, 3, true);
			break;
		case 3:
			targSerial.id = 0x1927;
			break;
		case 4:
			targSerial.id = 0x1921;
			targSerial.StartTimer(100, 5, true);
			break;
		case 5:
			targSerial.id = 0x1921;
			targSerial.StartTimer(2100, 6, true);
			break;
		case 6:
			targSerial.id = 0x1921;
			break;
	}
}