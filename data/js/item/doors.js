doorTypes = new Array (
	0x00E8, 0x0314, 0x0324, 0x0334, 0x0344, 0x0354, 0x0675, 0x0685,
	0x0695, 0x06A5, 0x06B5, 0x06C5, 0x06D5, 0x06E5, 0x0824, 0x0839,
	0x084C, 0x0866, 0x190E, 0x190f, 0x1FED
);

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket )
	{
		if( !pUser.InRange( iUsed, 3 ))
		{
			socket.SysMessage( GetDictionaryEntry( 1183, socket.language ));
			return false;
		}
	}

	if( iUsed.isDoorOpen )
	{
		if( !CloseDoor( iUsed ))
		{
			if( socket )
			{
				socket.SysMessage( GetDictionaryEntry( 1661, socket.language ));
			}
		}
		return false;
	}

	if( iUsed.type == 13 ) // Locked
	{
		if( !FindKey( pUser, iUsed ))
		{
			if( socket )
			{
				socket.SysMessage( GetDictionaryEntry( 406, socket.language ));
			}
			return false;
		}
	}

	var isDoor = false;
	var canOpen = false;
	var doorID = iUsed.id;

	if( doorID < 0x241F )
	{
		for( var i = 0; i < 21; ++i )
		{
			var doorType = doorTypes[i];
			if( doorID == doorType )
			{
				if( doorID != 0x190E && doorID != 0x190f )
				{
					canOpen = OpenDoor( iUsed, 1, -1, 1 );
				}
				else
				{
					if( doorID == 0x190e )
					{
						canOpen = OpenDoor( iUsed, 1, 0, 0 );
					}
					else
					{
						canOpen = OpenDoor( iUsed, -1, 0, 0 );
					}
				}
				isDoor = true;
				break;
			}
			else if( doorID == doorType + 2 || doorID == doorType + 8 )
			{
				canOpen = OpenDoor( iUsed, 1, 1, 1 );
				isDoor = true;
				break;
			}
			else if( doorID == doorType + 4 )
			{
				canOpen = OpenDoor( iUsed, 1, -1, 0 );
				isDoor = true;
				break;
			}
			else if( doorID == doorType + 6 || doorID == doorType + 10 )
			{
				canOpen = OpenDoor( iUsed, 1, 1, -1 );
				isDoor = true;
				break;
			}
			else if( doorID == doorType + 12 )
			{
				canOpen = OpenDoor( iUsed, 1, 0, 0 );
				isDoor = true;
				break;
			}
			else if( doorID == doorType + 14 )
			{
				canOpen = OpenDoor( iUsed, 1, 0, -1 );
				isDoor = true;
				break;
			}
		}
	}
	else if( doorID == 0x2D46 || doorID == 0x319E || doorID == 0x31A2 || doorID == 0x31A6 ||
		doorID == 0x31AC || doorID == 0x46e3 || doorID == 0x4d20 || doorID == 0x4d28 || doorID == 0x50ce ||
		doorID == 0x50d6 || doorID == 0x5148 || doorID == 0x9ae5 || doorID == 0x9b4a )
	{
		canOpen = OpenDoor( iUsed, 1, 0, -1 );
		isDoor = true;
	}
	else if( doorID == 0x2422 || doorID == 0x2424 )
	{
		canOpen = OpenDoor( iUsed, -1, 0, -1 );
		isDoor = true;
	}
	else if( doorID == 0x26F4 )
	{
		canOpen = OpenDoor( iUsed, 2, 0, 0 );
		isDoor = true;
	}
	else if( doorID == 0x2990 || doorID == 0x299E || doorID == 0x2A05 || doorID == 0x2A07 ||
		 doorID == 0x2A09 || doorID == 0x2A0B || doorID == 0x2A0D || doorID == 0x2A0F ||
		 doorID == 0x2A11 || doorID == 0x2A13 || doorID == 0x2A15 || doorID == 0x2A17 ||
		 doorID == 0x2A19 || doorID == 0x2A1B || doorID == 0x2D48 || doorID == 0x2D63 ||
		 doorID == 0x2D67 || doorID == 0x2D6B || doorID == 0x2FE2 || doorID == 0x46e1 ||
		 doorID == 0x4d1e || doorID == 0x4d26 || doorID == 0x50cc || doorID == 0x50d4 ||
		 doorID == 0x5146 || doorID == 0x9ae3 || doorID == 0x9b48 )
	{
		canOpen = OpenDoor( iUsed, 1, 0, 0 );
		isDoor = true;
	}
	else if( doorID == 0x2D65 || doorID == 0x2D69 || doorID == 0x2D6D || doorID == 0x2FE4 ||
		doorID == 0x46dd || doorID == 0x4d1a || doorID == 0x4d22 || doorID == 0x50c8 ||
		doorID == 0x50d0 || doorID == 0x5142 || doorID == 0x9adb || doorID == 0x9b40 )
	{
		canOpen = OpenDoor( iUsed, 1, -1, 0 );
		isDoor = true;
	}
	else if( doorID == 0x319C )
	{
		canOpen = OpenDoor( iUsed, 1, 1, 0 );
		isDoor = true;
	}
	else if( doorID == 0x241F || doorID == 0x31AA || doorID == 0x31A0 || doorID == 0x31A4 ||
		doorID == 0x31A8 || doorID == 0x31AE || doorID == 0x46df || doorID == 0x4d1c ||
		doorID == 0x4d24 || doorID == 0x50ca || doorID == 0x50d2 || doorID == 0x5144 ||
		doorID == 0x9add || doorID == 0x9ae1 || doorID == 0x9b42 || doorID == 0x9b46 )
	{
		canOpen = OpenDoor( iUsed, 1, 1, -1 );
		isDoor = true;
	}
	else if( doorID == 0x9ad7 || doorID == 0x9b3c )
	{
		canOpen = OpenDoor( iUsed, 1, -1, 1 );
		isDoor = true;
	}
	else if( doorID == 0x9ad9 || doorID == 0x9adf || doorID == 0x9b3e || doorID == 0x9b44 )
	{
		canOpen = OpenDoor( iUsed, 1, 1, 1 );
		isDoor = true;
	}
	else if( doorID == 0x2421 )
	{
		canOpen = OpenDoor( iUsed, 1, -1, 1 );
		isDoor = true;
	}

	if( isDoor )
	{
		if( !canOpen )
		{
			if( socket )
			{
				socket.SysMessage( GetDictionaryEntry( 1661, socket.language ));
			}
		}
		else if( iUsed.type == 13 )
		{
			if( socket )
			{
				pUser.TextMessage( GetDictionaryEntry( 405, socket.language ), false, 0x047e );
			}
		}
	}

	// The below block of code allows "linked" double-doors
	// to be opened at the same time, if they've been setup
	// using the 'LINKDOORS command
	if( iUsed.GetTag( "linked" ) == true )
	{
		if( pUser.socket.clickY == -1 )
		{
			var linkedDoor = CalcItemFromSer( iUsed.GetTag( "linkSer1"), iUsed.GetTag( "linkSer2"), iUsed.GetTag( "linkSer3"),  iUsed.GetTag( "linkSer4"));
			if( linkedDoor )
			{
				pUser.socket.clickY = 1;
				onUseChecked( pUser, linkedDoor );
			}
		}
		else if( pUser.socket.clickY == 1 )
		{
			pUser.socket.clickY = -1;
		}
	}

	return false;
}

function OpenDoor( iUsed, idChange, xChange, yChange )
{
	var doorType = GetDoorType( iUsed );
	SetDoorValues( iUsed, idChange, xChange, yChange, DoorOpenSound( doorType ), DoorCloseSound( doorType ));

	if( IsDoorBlocked( iUsed, false ))
	{
		SetDoorValues( iUsed, 0, 0, 0, 0, 0 );
		return false;
	}
	else
	{
		UseDoor( iUsed, false );
	}

	return true;
}

function CloseDoor( iUsed )
{
	if( IsDoorBlocked( iUsed, true ))
	{
		return false;
	}
	else
	{
		UseDoor( iUsed, true );
	}

	return true;
}

function SetDoorValues( iUsed, idChange, xChange, yChange, openSound, closeSound )
{
	iUsed.SetTag( "DOOR_ID", idChange );
	iUsed.SetTag( "DOOR_X", xChange );
	iUsed.SetTag( "DOOR_Y", yChange );
	iUsed.SetTag( "DOOR_OPEN", openSound );
	iUsed.SetTag( "DOOR_CLOSE", closeSound );
}

function UseDoor( iUsed, isOpen )
{
	if( !isOpen )
	{
		iUsed.id 	= iUsed.id + iUsed.GetTag( "DOOR_ID" );
		iUsed.x		= iUsed.x  + iUsed.GetTag( "DOOR_X" );
		iUsed.y		= iUsed.y  + iUsed.GetTag( "DOOR_Y" );
		iUsed.isDoorOpen = true;
		if( iUsed.GetTag( "DOOR_OPEN" ))
		{
			iUsed.SoundEffect( iUsed.GetTag( "DOOR_OPEN" ), true );
		}
		iUsed.StartTimer( 10000, 0, true );
	}
	else
	{
		iUsed.id	= iUsed.id - iUsed.GetTag( "DOOR_ID" );
		iUsed.x		= iUsed.x  - iUsed.GetTag( "DOOR_X" );
		iUsed.y		= iUsed.y  - iUsed.GetTag( "DOOR_Y" );
		iUsed.isDoorOpen = false;
		if( iUsed.GetTag( "DOOR_CLOSE" ))
		{
			iUsed.SoundEffect( iUsed.GetTag( "DOOR_CLOSE" ), true );
		}

		iUsed.KillTimers();

		SetDoorValues( iUsed, 0, 0, 0, 0, 0 );
	}
}

function onTimer( iUsed, timerID )
{
	if( !iUsed.isDoorOpen )
		return;

	if( IsDoorBlocked( iUsed, true ))
	{
		iUsed.StartTimer( 10000, 0, true );
	}
	else
	{
		UseDoor( iUsed, true );
	}
}

function IsDoorBlocked( iUsed, isOpen )
{
	var blockCount = 0;

	if( !isOpen )
	{
		blockCount = AreaCharacterFunction( "FindOpenBlockers", iUsed, 2 );
	}
	else
	{
		blockCount = AreaCharacterFunction( "FindCloseBlockers", iUsed, 2 );
	}

	if( blockCount > 0 )
		return true;

	return false;
}

function FindOpenBlockers( iUsed, tmpChar )
{
	return FindBlockers( tmpChar, iUsed.x+iUsed.GetTag( "DOOR_X" ) ,iUsed.x+iUsed.GetTag( "DOOR_Y" ));
}

function FindCloseBlockers( iUsed, tmpChar )
{
	return FindBlockers( tmpChar, iUsed.x-iUsed.GetTag( "DOOR_X" ) ,iUsed.y-iUsed.GetTag( "DOOR_Y" ));
}

function FindBlockers( tmpChar, newX, newY )
{
	if( tmpChar.x == newX && tmpChar.y == newY )
	{
		if( !tmpChar.dead && ( tmpChar.npc || tmpChar.online ))
		{
			return true;
		}
	}
	return false;
}

function DoorOpenSound( doorType )
{
	var retVal = 0x0000;
	switch( doorType )
	{
		case 1:	retVal = 0x00EA;	break;	// Open wooden / ratan
		case 2:	retVal = 0x00EB;	break;	// Open gate
		case 3:	retVal = 0x00EC;	break;	// Open metal
		case 4:	retVal = 0x00ED;	break;	// Open secret
	}
	return retVal;
}

function DoorCloseSound( doorType )
{
	var retVal = 0x0000;
	switch( doorType )
	{
		case 1:	retVal = 0x00F1;	break;	// Close wooden / ratan
		case 2:	retVal = 0x00F2;	break;	// Close gate
		case 3:	retVal = 0x00F3;	break;	// Close metal
		case 4:	retVal = 0x00F4;	break;	// Close secret
	}
	return retVal;
}

function GetDoorType( iUsed )
{
	var retVal = 1; // Default to wooden door
	var doorID = iUsed.id;
	if( ( doorID >= 0x0695 && doorID < 0x06C5 ) || 		// wooden / ratan door
	    ( doorID >= 0x06D5 && doorID <= 0x06F4 ) ||
	    ( doorID >= 0x50c8 && doorID <= 0x50d6 ) ||
	    ( doorID >= 0x9ad7 && doorID <= 0x9ae5 ) ||
	    ( doorID >= 0x9b3c && doorID <= 0x9b4a ) ||
	    ( doorID >= 0x46dd && doorID <= 0x46e3 ) ||
	    ( doorID >= 0x4d1a && doorID <= 0x4d28 ))
	{
			retVal = 1;
	}
	else if( ( doorID >= 0x0839 && doorID <= 0x0848 ) || 	// gate
		 ( doorID >= 0x084C && doorID <= 0x085B ) ||
		 ( doorID >= 0x0866 && doorID <= 0x0875 ))
	{
			retVal = 2;
	}
	else if( ( doorID >= 0x0675 && doorID < 0x0695 ) || 	// metal
		 ( doorID >= 0x06C5 && doorID < 0x06D5 ) ||
		 ( doorID >= 0x5142 && doorID <= 0x5148 ))
	{
			retVal = 3;
	}
	else if(( doorID >= 0x00e8 && doorID <= 0x00f7 ) || ( doorID >= 0x0314 && doorID <= 0x0365 )) 	// secret
	{
			retVal = 4;
	}
	return retVal;
}

function FindKey( pUser, iUsed )
{
	var foundKey = false;
	if( iUsed.more > 0 )
	{
		var pPack = pUser.pack;
		if( pPack != null )
		{
			foundKey = FindKeyInPack( pUser, pPack, iUsed );
		}
	}

	return foundKey;
}

function FindKeyInPack( pUser, pPack, iUsed )
{
	for( var toCheck = pPack.FirstItem(); !pPack.FinishedItems(); toCheck = pPack.NextItem() )
	{
		if( toCheck.isContType )
		{
			if( FindKeyInPack( pUser, toCheck, iUsed ))
			{
				return true;
			}
		}
		else if( toCheck.more == iUsed.more )
		{
			return true;
		}
		else if( toCheck.id >= 0x1769 && toCheck.id <= 0x176b )
		{
			//If it's a keyring, check to see if any of the keys there match the lock on the door
			var keyAmount = toCheck.GetTag( "keys" );
			var i = 1;
			for( i = 1; i < keyAmount + 1; i++ )
			{
				var keyItemMore = toCheck.GetTag( "key" + i + "more" );
				if( keyItemMore == iUsed.more || keyItemMore == 255 )
				{
					return true;
				}
			}
		}
	}
	return false;
}
