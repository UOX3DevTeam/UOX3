// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/rewrote the script
// knives : fish : raw fish steak

function onUse ( pUser, iUsed ) 
{
	// get users socket
	var srcSock = pUser.socket;

	// is it in users pack?
	if( iUsed.container != null )
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner.serial != pUser.serial )
		{
			pUser.SysMessage( "This has to be in your backpack!" );
			return;
		}
		else
			// let the user target the heat source
			srcSock.CustomTarget( 0, "What do you want to use the knife on?" );
	}
	else
		pUser.SysMessage( "This has to be in your backpack!" );
}

function onCallback0( tSock, targSerial )
{
	var pUser = tSock.currentChar;
	var StrangeByte   = tSock.GetWord( 1 );
	var targX	= tSock.GetWord( 11 );
	var targY	= tSock.GetWord( 13 );
	var targZ	= tSock.GetByte( 16 );
	var tileID	= tSock.GetWord( 17 );
	if( tileID == 0 )
	{ //Target is a Maptile
		pUser.SysMessage("You cannot use your knife on that.");
	}
	else if( StrangeByte == 0 && targSerial.isChar )
	{ //Target is a Character
		pUser.SysMessage("You cannot use your knife on that.");
	}
	if( StrangeByte == 0 )
	{
		// Target is a DynamicItem
		if( tileID >= 0x09CC && tileID <= 0x09CF )
		{	// In case its a fish
			// check if its in range
			if( targSerial.container != null )
			{
				var iPackOwner = GetPackOwner( targSerial, 0 );
				if( iPackOwner.serial != pUser.serial )
				{
					pUser.SysMessage( "The target is out of reach." );
				}
			}
			else
			{
				if(( pUser.x > targX + 3 ) || ( pUser.x < targX - 3 ) || ( pUser.y > targY + 3 ) || ( pUser.y < targY - 3 ) || ( pUser.z > targZ + 10 ) || ( pUser.z < targZ - 10 ))
			{
					pUser.SysMessage( "You are too far away from the target!" );
					return;
				}	
			}
			targSerial.amount = targSerial.amount -1;
			if( targSerial.amount < 1 )
				targSerial.Delete();
			pUser.SoundEffect( 0x018E, true );
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x097a", 4, "ITEM", true ); // makes 4 raw fish steaks
			pUser.SysMessage( "You slice a fish to steaks." );
			return;
		}
	}
	else // Target is a static item
		pUser.SysMessage("You cannot use your knife on that." );
}
