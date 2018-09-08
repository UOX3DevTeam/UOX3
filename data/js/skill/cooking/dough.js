// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/rewrote the script
// 14/06/2005 Xuri; Fixed the script :P
// use flour : target water pitcher : get dough

function onUseChecked ( pUser, iUsed ) 
{
	var srcSock = pUser.socket;	// get users socket
	if( iUsed.container != null )	// is it in users pack?
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner.serial != pUser.serial )
		{
			pUser.SysMessage( "This has to be in your backpack!" );
		}
		else
		{
			if( iUsed.id == 0x1039 || iUsed.id == 0x1045 )
			{
				iUsed.id++;
			}
			else
			{
				srcSock.tempObj = iUsed;
				srcSock.CustomTarget( 0, "Which pitcher of water to use?" );// let the user target a pitcher of water to use
			}
		}
	}
	else
		pUser.SysMessage( "This has to be in your backpack!" );
	return false;
}

function onCallback0( tSock, targSerial )
{
	var pUser = tSock.currentChar;
	var iUsed = tSock.tempObj;	
	var StrangeByte   = tSock.GetWord( 1 );
	var targX	= tSock.GetWord( 11 );
	var targY	= tSock.GetWord( 13 );
	var targZ	= tSock.GetSByte( 16 );
	var tileID	= tSock.GetWord( 17 );
	if( tileID == 0 )
	{ //Target is a Maptile
		pUser.SysMessage("That is not a pitcher of water.");
	}
	else if( StrangeByte == 0 && targSerial.isChar )
	{ //Target is a Character
		pUser.SysMessage("That is not a pitcher of water.");
	}
	// Target is a Dynamic Item
	if( StrangeByte == 0 )
	{
		//If target self, close the flour bag
		if( targSerial == iUsed )
		{
			targSerial.id--;
			return;
		}
		if( targSerial.id != 0x0FF8 && targSerial.id != 0x0FF9 && targSerial.id != 0x1f9d && targSerial.id != 0x1f9e ) // is the item of the right type?
		{
			tSock.SysMessage( "That is not a pitcher of water." );
			return;
		}
		// check if its in range
		if( iUsed.container != null )
		{
			var iPackOwner = GetPackOwner( iUsed, 0 );
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
		// remove one flour
		var iMakeResource = pUser.ResourceCount( 0x1045 );	// is there enough resources to use up to make it
		if( iMakeResource < 1 )
		{
			var iMakeResource = pUser.ResourceCount( 0x1046 );	// is there enough resources to use up to make it
			if( iMakeResource < 1 )
			{
				var iMakeResource = pUser.ResourceCount( 0x1039 );	// is there enough resources to use up to make it
				if( iMakeResource <1 )
				{
					var iMakeResource = pUser.ResourceCount( 0x103a );	// is there enough resources to use up to make it			
					if( iMakeResource < 1 )
					{
						pUser.SysMessage( "There is not enough flour in your pack!" );
						return;
					}
					else
						var iID = 0x103a;
				}
				else
					var iID = 0x1039;
			}
			else
				var iID = 0x1046;
		}
		else
			var iID = 0x1045;
		pUser.SysMessage( "Resource to use: " + iID );
		//pUser.UseResource( 1, iID ); // uses up a resource (amount, item ID, item colour)
		iUsed.Delete();
		pUser.SoundEffect( 0x0134, true );
		// check the skill
		if( !pUser.CheckSkill( 13, 1, 1000 ) )	// character to check, skill #, minimum skill, and maximum skill
		{
			pUser.SysMessage( "You tried to make dough but failed." );
			return;
		}
		if( targSerial.id == 0x0FF8 || targSerial.id == 0x1f9e)
			targSerial.id = 0x0FF7;
		if( targSerial.id == 0x0ff9 || targSerial.id == 0x1f9d )
			targSerial.id = 0x0FF6;
		targSerial.SetTag( "ContentsType", 1 );
		targSerial.SetTag( "EmptyGlass", 3 );
		targSerial.SetTag( "UsesLeft", 0 );
		targSerial.SetTag( "ContentsName", "nothing" );

		var itemMade = CreateBlankItem( pUser.socket, pUser, 1, "#", 0x103D, 0x0, "ITEM", true ); // makes a dough
		pUser.SysMessage( "You make some dough." );
	}
	else // Target is a static item
		pUser.SysMessage( "You cannot use that for making dough." );
}
