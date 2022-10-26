// Keys and Keyrings
// Version 1.7
// Last updated: September 12. 2022
//
// Can add up to 5 keys to each key-ring, and can use keyring
// to unlock any of the locks that it holds keys for. To release
// keys from keyring, target the keyring itself.
//
// Rename keys and/or keyrings(empty only) by targeting key/keyring itself
// Copy keys by targeting a usable key with a blank
// A key with more value of 0xFF 0xFF 0xFF 0xFF is usable on any lock.

const keyID1 = 0x100e; // copper key
const keyID2 = 0x100f; // gold key
const keyID3 = 0x1010; // iron key
const keyID4 = 0x1012; // magic key
const keyID5 = 0x1013; // rusty iron key
const emptyKeyringID = 0x1011;
const keyringID1 = 0x1769; // single key
const keyringID2 = 0x176a; // many keys
const keyringID3 = 0x176b; // full keyring

const keyScriptID = 5013; // Script-ID from JSE_FILEASSOCIATIONS.SCP! Change to match your own setting.

const coOwnHousesOnSameAccount = GetServerSetting( "CoOwnHousesOnSameAccount" );

// Adding keys to keyrings
function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( iUsed.movable == 3 )
	{
		// Key is locked down, make sure only owners, co-owners or friends can use it
		var iMulti = iUsed.multi;
		if( !ValidateObject( iMulti ) || ( !iMulti.IsOnOwnerList( pUser ) && !iMulti.IsOnFriendList( pUser )))
		{
			if( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum != pUser.accountNum ))
			{
				pSock.SysMessage( GetDictionaryEntry( 1032, pSock.language )); // That is not yours!
				return false;
			}
		}
	}

	pSock.tempObj = iUsed;
	if(( iUsed.id >= keyID1 && iUsed.id <= keyID3 ) || iUsed.id == keyID4 || iUsed.id == keyID5 )
	{
		if( !iUsed.more )
		{
			pUser.CustomTarget( 3, GetDictionaryEntry( 2727, pSock.language )); // This is a key blank. Which key would you like to make a copy of?
		}
		else
		{
			pUser.CustomTarget( 0, GetDictionaryEntry( 2728, pSock.language )); // What do you want to use this key on?
		}
	}
	else if( iUsed.id >= keyringID1 && iUsed.id <= keyringID3 )
	{
		pUser.CustomTarget( 1, GetDictionaryEntry( 2729, pSock.language )); // What do you want to use this keyring on?
	}
	else if( iUsed.id == emptyKeyringID )
	{
		pUser.CustomTarget( 2, GetDictionaryEntry( 2730, pSock.language )); // Select a key to add to this keyring.
	}
	return false;
}

// Key targetting
function onCallback0( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var targetKey = pSock.tempObj;
	pSock.tempObj = null;

	// If player is now out of range of the key, disallow it!
	if(	!pUser.InRange( targetKey, 3 ))
	{
		pSock.SysMessage( GetDictionaryEntry( 2731, pSock.language )); // Key is out of range!
		return;
	}

	var strangeByte = pSock.GetWord( 1 );
	if( !strangeByte && myTarget.isItem )
	{
		// If targeting an item that is out of range, return
		if( !pUser.InRange( myTarget, 3 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
		}

		// If targeting a full keyring
		var targetType = myTarget.type;
		if( myTarget.id == keyringID3 )
		{
 			pSock.SysMessage( GetDictionaryEntry( 2732, pSock.language )); // Sorry, that keyring is full.
 			return;
		}
		// if targeting a keyring, add key to it
		else if( myTarget.id == keyringID1 || myTarget.id == keyringID2 || myTarget.id == emptyKeyringID )
		{
			if( myTarget.container.serial != pUser.pack.serial )
			{
				pSock.SysMessage( GetDictionaryEntry( 1763, pSock.language )); //That item must be in your backpack before it can be used.
			}
			else
			{
				if( myTarget.id == emptyKeyringID )
				{
					myTarget.id = keyringID1;
				}
				var keys = myTarget.GetTag( "keys" );
				if( !keys )
				{
					keys = 0;
				}
				keys++;
				myTarget.SetTag( "keys", keys );
				myTarget.SetTag( "key" + keys + "more", ( targetKey.more ).toString() );
				myTarget.SetTag( "key" + keys + "name", targetKey.name );
				myTarget.SetTag( "keyid" + keys, targetKey.id );
				pSock.SysMessage( GetDictionaryEntry( 2733, pSock.language )); // You've added the key to the keyring.
				var tempMsg = GetDictionaryEntry( 2734, pSock.language ) // There are now %i keys on this keyring.
				pSock.SysMessage( tempMsg.replace( /%i/gi, keys ));
				targetKey.Delete();
				if( keys > 1 && keys < 5 )
				{
					myTarget.id = keyringID2;
				}
				if( keys == 5 )
				{
					myTarget.id = keyringID3;
				}
			}
			return;
		}
		//If targeting a lock
		else if( targetType == 1 || targetType == 8 || targetType == 12 || targetType == 13 || targetType == 117 || targetType == 63 || targetType == 64 || targetType == 203 )
		{
			if( myTarget.more == targetKey.more || targetKey.more == 255 ) //targetKey.more == 255 is a universal key
			{
				if( targetType == 12 || targetType == 13 )
				{
					// Don't allow changing lock status on FRONT doors in public houses
					var lockMulti = myTarget.multi
					if( ValidateObject( lockMulti ) && lockMulti.isPublic && myTarget.GetTag( "DoorType" ) == "front" )
					{
						pSock.SysMessage( GetDictionaryEntry( 2735, pSock.language )); // You cannot lock the front door of a public house!
						return;
					}
				}
				ChangeLockStatus( pUser, myTarget );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1026, pSock.language )); //The key does not fit into that lock.
			}
			return;
		}
		//If targeting key itself, rename key
		else if( myTarget.serial == targetKey.serial )
		{
			pSock.SysMessage( GetDictionaryEntry( 1019, pSock.language )); //Enter new name for key.
			pSock.tempObj2 = targetKey;
			pUser.SpeechInput( 1 );
			return;
		}
	}
	pSock.SysMessage( GetDictionaryEntry( 1025, pSock.language )); //That does not have a lock.
}

function onCallback1( pSock, myTarget )
{
	// keyring targeting
	var pUser = pSock.currentChar;
	var keyring = pSock.tempObj;
	pSock.tempObj = null;
	var strangeByte = pSock.GetWord( 1 );
	var keys, i;

	if( !strangeByte && myTarget.isItem )
	{
		// If targeting an item that is out of range, return
		if( myTarget.type != 203 && !pUser.InRange( myTarget, 3 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); //That is too far away.
		}

		if( myTarget.serial == keyring.serial )
		{
			// If targeting keyring itself, release all keys
			keys = keyring.GetTag( "keys" );
			var totalKeys = keys;
			var keyringCont = keyring.container;
			i = 1;
			for( i = 1; i < totalKeys + 1; i++ )
			{
				if( ValidateObject( keyringCont ))
				{
					if( keyringCont.totalItemCount >= keyringCont.maxItems )
					{
						pSock.SysMessage( GetDictionaryEntry( 1819 ), pSock.language ); // Your backpack cannot hold any more items!
						break;
					}
				}
				var keyID = keyring.GetTag( "keyid"+i );
				var hexKeyID = keyID.toString( 16 );
				var newKey = CreateDFNItem( pSock, pUser, "0x" + hexKeyID, 1, "ITEM", true );
				keys--;
			}
			if( keys <= 0 )
			{
				keyring.id = emptyKeyringID;
				keyring.SetTag( "keys", 0 );
				pSock.SysMessage( GetDictionaryEntry( 2736, pSock.language )); // You release all keys from the keyring.
			}
			else
			{
				keyring.SetTag( "keys", keys );
				if( keys == 1 )
				{
					keyring.id == keyringID1;
				}
				else if( keys > 1 && keys < 5 )
				{
					keyring.id = keyringID2;
				}
				else if( keys >= 5 )
				{
					keyring.id = keyringID3;
				}
			}
			return;
		}
		else if( myTarget.type == 1 || myTarget.type == 8 || myTarget.type == 12 || myTarget.type == 13 || myTarget.type == 117 || myTarget.type == 63 || myTarget.type == 64 || myTarget.type == 203 )
		{
			// If key & lock has same MORE value, i.e. they are a match
			keys = keyring.GetTag( "keys" );
			i = 1;
			for( i = 1; i < keys + 1; i++ )
			{
				var keyItemMore = keyring.GetTag( "key" + i + "more" );
				if( keyItemMore == myTarget.more || keyItemMore == 255 )
				{
					ChangeLockStatus( pUser, myTarget );
					return;
				}
			}
			pSock.SysMessage( GetDictionaryEntry( 2737, pSock.language )); // None of your keys fit the lock.
			return;
		}
	}
	pSock.SysMessage( GetDictionaryEntry( 1025, pSock.language )); // That does not have a lock.
}

function onCallback2( pSock, myTarget )
{
	// Empty keyring targeting
	var pUser = pSock.currentChar;
	var emptyKeyring = pSock.tempObj;
	pSock.tempObj = null;
	var strangeByte = pSock.GetWord( 1 );
	if( !strangeByte && myTarget.isItem )
	{
		// If targeting an item that is out of range, return
		if( !pUser.InRange( myTarget, 3 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
		}

		if(( myTarget.id >= keyID1 && myTarget.id <= keyID3 ) || myTarget.id == keyID4 || myTarget.id == keyID5 )
		{
			var itemOwner = GetPackOwner( myTarget, 0 );
			if( !itemOwner || itemOwner != pUser )
			{
				pSock.SysMessage( GetDictionaryEntry( 1763, pSock.language )); // That item must be in your backpack before it can be used.
			}
			if( !myTarget.more )
			{
				// If key isn't associated with any key, cancel
				pSock.SysMessage( GetDictionaryEntry( 2738, pSock.language )); // That key has no lock defined for it!
			}
			else
			{
				var scriptFound = false;
				var scriptTriggers = myTarget.scriptTriggers;
				for( var i = 0; i < scriptTriggers.length; i++ )
				{
					if( scriptTriggers[i] == keyScriptID )
					{
						scriptFound = true;
					}
				}
				if( !scriptFound ) // If targetted key doesn't use this script, make it :)
				{
					myTarget.AddScriptTrigger( keyScriptID );
				}
				emptyKeyring.id = keyringID1;
				emptyKeyring.SetTag( "keys", 1 );
				emptyKeyring.SetTag( "keys1more", ( myTarget.more ).toString() );
				emptyKeyring.SetTag( "keys1name", myTarget.name );
				emptyKeyring.SetTag( "keyid1", myTarget.id );
				pSock.SysMessage( GetDictionaryEntry( 2739, pSock.language )); // You've added the key to the keyring.
				myTarget.Delete();
			}
			return;
		}
		else if( myTarget.serial == emptyKeyring.serial )
		{
			//If targeting empty keyring, rename it
			pSock.SysMessage( GetDictionaryEntry( 2740, pSock.language )); // Enter the new name for this keyring:
			pUser.socket.tempObj2 = emptyKeyring;
			pUser.SpeechInput( 2 );
			return;
		}
	}
	pSock.SysMessage( GetDictionaryEntry( 2741, pSock.language )); // That is not a key.
}

// Using a blank key, target a key to copy
function onCallback3( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var keyBlank = pSock.tempObj;
	pSock.tempObj = null;
	var strangeByte = pSock.GetWord( 1 );
	if( !strangeByte && myTarget.isItem )
	{
		var itemOwner = GetPackOwner( myTarget, 0 );
		if( itemOwner && itemOwner != pUser )
		{
			pSock.SysMessage( GetDictionaryEntry( 1763, pSock.language )); // That item must be in your backpack before it can be used.
			return;
		}
		else
		{
			if( !pUser.InRange( myTarget, 3 ))
			{
				pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
				return;
			}
			else
			{
				if(( myTarget.id >= keyID1 && myTarget.id <= keyID3 ) || myTarget.id == keyID4 || myTarget.id == keyID5 )
				{
					if( myTarget.more )
					{
						if( pUser.CheckSkill( 37, 400, 1000 ))
						{
							pSock.SysMessage( GetDictionaryEntry( 1017, pSock.language )); //You copy the key.
							keyBlank.more = myTarget.more;
						}
						else
						{
							pSock.SysMessage( GetDictionaryEntry( 1016, pSock.language )); //You fail and destroy the key blank.
							keyBlank.Delete();
						}
					}
					else
					{
						pSock.SysMessage( GetDictionaryEntry( 2742, pSock.language )); // This key is also a blank.
					}
					return;
				}
			}
		}
	}
	pSock.SysMessage( GetDictionaryEntry( 2743, pSock.language )); // You can't make a copy of that.
}

function onSpeechInput( pUser, pItem, pSpeech, pSpeechID )
{
	var pSock = pUser.socket;
	var keyItem = pSock.tempObj2;
	pSock.tempObj2 = null;
	if( pSpeech )
	{
    	switch( pSpeechID )
    	{
			case 1: //Rename Keys
			{
				keyItem.name = pSpeech;
				var tempMsg = GetDictionaryEntry( 2744, pSock.language ); // The new name for this key is: %s
				pSock.SysMessage( tempMsg.replace( /%s/gi, pSpeech ));
				break;
			}
			case 2: //Rename Keyrings
			{
				keyItem.name = pSpeech;
				var tempMsg2 = GetDictionaryEntry( 2745, pSock.language ); // The new name for this keyring is: %s
				pSock.SysMessage( tempMsg2.replace( /%s/gi, pSpeech ));
				break;
			}
			case 3: //Rename house-sign
			{
				var keyItemMulti = keyItem.multi;
				if( keyItemMulti.IsOwner( pUser ))
				{
					if( pSpeech.length > 60 )
					{
						pSock.SysMessage( GetDictionaryEntry( 2746, pSock.language )); // House name must shorter than 60 characters long!
					}
					else
					{
						keyItem.name = pSpeech;
						var tempMsg3 = GetDictionaryEntry( 2747, pSock.language ); // Renamed to: %s
						pSock.SysMessage( tempMsg3.replace( /%s/gi, pSpeech ));
					}
				}
				else
				{
					pSock.SysMessage( GetDictionaryEntry( 2748, pSock.language )); // Only the house owner can rename the house!
				}
				break;
			}
		}
	}
}

function ChangeLockStatus( pUser, myTarget )
{
	var pSock = pUser.socket;
	switch( myTarget.type )
	{
		case 1: // Unlocked container
			myTarget.type = 8;
			pSock.SysMessage( GetDictionaryEntry( 1018, pSock.language )); //You lock the container.
			break;
		case 8: // Locked container
			myTarget.type = 1;
			pSock.SysMessage( GetDictionaryEntry( 1020, pSock.language )); //You unlock the container.
			break;
		case 12: // Unlocked door
			pSock.SysMessage( GetDictionaryEntry( 1021, pSock.language )); //You lock the door.
			myTarget.type = 13;
			if( myTarget.isDoorOpen )
			{
				// If the door is open when you lock it, close the door!
				TriggerEvent( 4500, "UseDoor", myTarget, true );
			}
			break;
		case 13: // Locked door
			pSock.SysMessage( GetDictionaryEntry( 1022, pSock.language )); //You unlock the door.
			myTarget.type = 12;
			break;
		case 63: // spawn-container
			pSock.SysMessage( GetDictionaryEntry( 1018, pSock.language )); //You lock the container.
			myTarget.type = 64;
			break;
		case 64: // locked spawn-container
			pSock.SysMessage( GetDictionaryEntry( 1020, pSock.language )); //You unlock the container.
			myTarget.type = 63;
			break;
		case 117: // The plank on a boat
			var plankStatus;
			var plankLocked = parseInt( myTarget.GetTag( "plankLocked" ));
			switch( plankLocked )
			{
				// Unlock plank
				case 0: plankStatus = 0; myTarget.SetTag( "plankLocked", 1 ); break;
				// Lock plank
				case 1: plankStatus = 1; myTarget.SetTag( "plankLocked", 0 ); break;
				default:break;
			}
			if( !plankStatus )
			{
				switch( myTarget.id )
				{
					case 0x3E84: myTarget.id = 0x3EE9; break;
					case 0x3ED5: myTarget.id = 0x3EB1; break;
					case 0x3ED4: myTarget.id = 0x3EB2; break;
					case 0x3E89: myTarget.id = 0x3E8A; break;
				}
				pSock.SysMessage( GetDictionaryEntry( 9143, pSock.language )); //You lock the plank.
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 9142, pSock.language )); //You unlock the plank.
			}
			break;
		case 203: //A house sign
		{
			var signMulti = myTarget.multi;
			if( signMulti.IsOwner( pUser ))
			{
				pSock.tempObj2 = myTarget;
				pSock.SysMessage( GetDictionaryEntry( 1023, pSock.language )); //What do you wish the sign to say?
				pUser.SpeechInput( 3 );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 2748, pSock.language )); // Only the house owner can rename the house!
			}
			break;
		}
	}
	pUser.SoundEffect( 0x241, 1 );
}
