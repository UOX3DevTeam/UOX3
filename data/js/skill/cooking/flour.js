// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/rewrote the script
// 14/06/2005 Xuri; Fixed the script :P
// 25/07/2021 Updated to use dictionary messages
// use flour : target water pitcher : get dough

function onUseChecked ( pUser, iUsed )
{
	var srcSock = pUser.socket;	// get users socket
	if( iUsed.container != null )	// is it in users pack?
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner.serial != pUser.serial )
		{
			srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
		}
		else
		{
			if( iUsed.id == 0x1039 || iUsed.id == 0x1045 )
			{
				iUsed.id++;
				if( iUsed.usesLeft == 0 )
				{
					iUsed.usesLeft = 20;
					iUsed.AddScriptTrigger( 2200 ); // UsesLeft tooltip
				}
			}
			else
			{
				srcSock.tempObj = iUsed;
				srcSock.CustomTarget( 0, GetDictionaryEntry( 6074, srcSock.language )); // Which pitcher of water to use?
			}
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
	var iUsed = tSock.tempObj;
	var strangeByte = tSock.GetWord( 1 );
	var targX = tSock.GetWord( 11 );
	var targY = tSock.GetWord( 13 );
	var targZ = tSock.GetSByte( 16 );

	var tileID = tSock.GetWord( 17 );
	if( tileID == 0 || ( strangeByte == 0 && myTarget.isChar ))
	{
		//Target is a MapTile, or a Character
		tSock.SysMessage( GetDictionaryEntry( 6075, tSock.language )); // That is not a pitcher of water
		return;
	}

	// Target is a Dynamic Item
	if( strangeByte == 0 )
	{
		// If target self, close the flour bag
		if( myTarget == iUsed )
		{
			myTarget.id--;
			return;
		}

		// Is player targeting tribal berry in attempt to create tribal paint?
		if( myTarget.sectionID == "tribalberry" && ( iUsed.id == 0x1046 || iUsed.id == 0x103a ))
		{
			if( pUser.skills.cooking >= 800 )
			{
				// Delete the sack of flour, and reduce amount of berries in pile by 1`
				iUsed.Delete();
				if( myTarget.amount > 1 )
				{
					myTarget.amount--;
				}
				else
				{
					myTarget.Delete();
				}

				// Great! Let's make some tribal paint
				var tribalPaint = CreateDFNItem( tSock, pUser, "tribalpaint", 1, "ITEM", true );
				tSock.SysMessage( GetDictionaryEntry( 6275, tSock.language )); // You combine the berry and the flour into the tribal paint worn by the savages.
			}
			else
			{
				// Not enough skill
				tSock.SysMessage( GetDictionaryEntry( 6276, tSock.language )); // You don't have the cooking skill to create the body paint.
			}
			return;
		}
		else
		{
			if( myTarget.id != 0x0FF8 && myTarget.id != 0x0FF9 && myTarget.id != 0x1f9d && myTarget.id != 0x1f9e ) // is the item of the right type?
			{
				tSock.SysMessage( GetDictionaryEntry( 6075, tSock.language )); // That is not a pitcher of water
				return;
			}
		}

		// Check if its in range
		if( iUsed.container != null )
		{
			var iPackOwner = GetPackOwner( iUsed, 0 );
			if( iPackOwner.serial != pUser.serial )
			{
				tSock.SysMessage( GetDictionaryEntry( 393, tSock.language )); // That is too far away.
			}
		}
		else if( myTarget.isItemHeld )
		{
			tSock.SysMessage( GetDictionaryEntry( 393, tSock.language )); // That is too far away.
			return;
		}
		else
		{
			if( TriggerEvent( 103, "RangeCheck", tSock, pUser ))
			{
				tSock.SysMessage( GetDictionaryEntry( 393, tSock.language )); // That is too far away.
				return;
			}
		}

		// remove one flour
		if( iUsed.usesLeft > 0 )
		{
			iUsed.usesLeft--;
			iUsed.Refresh();
		}
		else
		{
			iUsed.Delete();
		}

		pUser.SoundEffect( 0x0134, true );

		// Reduce uses left in water source
		if( myTarget.amount > 1 )
		{
			myTarget.amount--;
		}
		else
		{
			myTarget.Delete();
		}
		return;

		if( myTarget.usesLeft > 0 )
		{
			myTarget.usesLeft--;
			myTarget.Refresh();
		}
		else
		{
			if( myTarget.id == 0x0FF8 || myTarget.id == 0x1f9e )
			{
				myTarget.id = 0x0FF7;
			}
			else if( myTarget.id == 0x0ff9 || myTarget.id == 0x1f9d )
			{
				myTarget.id = 0x0FF6;
			}
			myTarget.SetTag( "ContentsType", 1 );
			myTarget.SetTag( "EmptyGlass", 3 );
			myTarget.usesLeft = 0;
			myTarget.SetTag( "ContentsName", "nothing" );
			TriggerEvent( 2100, "switchPitcherID", tSock, myTarget );
		}

		var itemMade = CreateDFNItem( tSock, pUser, "0x103d", 1, "ITEM", true );
		tSock.SysMessage( GetDictionaryEntry( 6078, tSock.language )); // You make some dough.
	}
	else // Target is a static item
	{
		tSock.SysMessage( GetDictionaryEntry( 6079, tSock.language )); // You cannot use that for making dough.
	}
}
