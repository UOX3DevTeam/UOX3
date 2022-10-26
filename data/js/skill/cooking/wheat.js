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
		{
			srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
		}
		else
		{
			// Not needed, in Pub15 era it takes 1 wheat to get 1 sack of flour
			/*var countOfResource = pUser.ResourceCount( 0x1EBD );	// item ID
			if( countOfResource < 4 )
			{
				srcSock.SysMessage( GetDictionaryEntry( 6080, srcSock.language )); // You do not have enough resources! You need 4 sheaves of wheat!
			}
			else*/
			//{
				srcSock.CustomTarget( 0, GetDictionaryEntry( 6081, srcSock.language )); // Where do you wish to grind the wheat?
			//}
		}
	}
	else
	{
		srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
	}

	return false;
}

function onCallback0( tSock, myTarget )
{
	var pUser = tSock.currentChar;
	var tileID = tSock.GetWord( 17 );
	var strangeByte = tSock.GetWord( 1 );

	if( tileID == 0 || ( strangeByte == 0 && myTarget.isChar ))
	{
		//Target is a MapTile or a Character
		tSock.SysMessage( GetDictionaryEntry( 6082, tSock.language )); // You cannot grind your wheat on that.
		return;
	}

	if( TriggerEvent( 107, "FlourMill", tileID ))
	{
		if( myTarget.GetTag( "millStatus" ) == 0 )
		{
			if( TriggerEvent( 103, "RangeCheck", tSock, pUser ))
			{
				tSock.SysMessage( GetDictionaryEntry( 393, tSock.language )); // That is too far away.
				return;
			}

			// remove 4 sheaves of wheat
			var iMakeResource = pUser.ResourceCount( 0x1EBD );// is there enough resources to use up to make it
			if( iMakeResource < 1 )
			{
				tSock.SysMessage( GetDictionaryEntry( 6080, tSock.language )); // You do not have enough resources! You need 4 sheaves of wheat!
				return;
			}
			pUser.UseResource( 1, 0x1EBD ); // uses up a resource (amount, item ID, item colour)
			pUser.SoundEffect( 0x021e, true );

			if( tileID == 0x1922 || tileID == 0x192e )
			{
				myTarget.SetTag( "millStatus", 1 ); // busy
				AreaItemFunction( "SearchForFlourMills", myTarget, 1 );
				myTarget.StartTimer( 100, 1, true );
			}
		}
		else
		{
			// busy
			tSock.SysMessage( GetDictionaryEntry( 6103, tSock.language )); // The flour mill is currently busy.
		}
	}
}

function SearchForFlourMills( srcChar, trgItem, iUsed )
{
	if( trgItem.id == 0x1920 || trgItem.id == 0x192c )
	{
		trgItem.StartTimer( 100, 4, true );
	}
}

function onTimer( timerObj, timerID )
{
	switch( timerID )
	{
		case 1:
			timerObj.id++; // 0x1922 becomes 0x1923, 0x192e becomes 0x192f
			timerObj.StartTimer( 100, 2, true );
			break;
		case 2:
			if( timerObj.id == 0x1923 )
			{
				timerObj.id = 0x1926;
			}
			else
			{
				timerObj.id = 0x1932;
			}
			var fakeFlourBees = CreateDFNItem( null, null, "fakeflourbees", 1, "ITEM", false, 0, timerObj.worldnumber, timerObj.instanceID );
			fakeFlourBees.x = timerObj.x;
			fakeFlourBees.y = timerObj.y;
			fakeFlourBees.z = 2;
			timerObj.StartTimer( 4500, 3, true );
			timerObj.SetTempTag( "fakeFlourSerial", ( fakeFlourBees.serial ).toString() );
			break;
		case 3:
			if( timerObj.id == 0x1926 )
			{
				timerObj.id = 0x1927;
			}
			else
			{
				timerObj.id = 0x1933;
			}
			timerObj.SetTag( "millStatus", 2 ); // Ready for collection
			var fakeFlourSerial = timerObj.GetTempTag( "fakeFlourSerial" );
			timerObj.SetTempTag( "fakeFlourSerial", null );
			var fakeFlourBees = CalcItemFromSer( parseInt( fakeFlourSerial ));
			if( ValidateObject( fakeFlourBees ))
			{
				fakeFlourBees.Delete();
			}
			timerObj.TextMessage( GetDictionaryEntry( 6104 ), true, 0x096a ); // Flour ready for collection.
			break;
		case 4:
			timerObj.id++;
			timerObj.StartTimer( 4600, 5, true );
			break;
		case 5:
			timerObj.id--;
			break;
	}
}