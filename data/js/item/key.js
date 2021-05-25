// Keys and Keyrings
// Version 1.6
// Last updated: October 25. 2020
//
// Can add up to 5 keys to each key-ring, and can use keyring
// to unlock any of the locks that it holds keys for. To release
// keys from keyring, target the keyring itself.
//
// Rename keys and/or keyrings(empty only) by targeting key/keyring itself
// Copy keys by targeting a usable key with a blank
// A key with more value of 0xFF 0xFF 0xFF 0xFF is usable on any lock.

var keyID1 = 0x100e; //copper key
var keyID2 = 0x100f; //gold key
var keyID3 = 0x1010; //iron key
var keyID4 = 0x1012; //magic key
var keyID5 = 0x1013; //rusty iron key
var emptyKeyringID = 0x1011;
var KeyringID1 = 0x1769; //single key
var KeyringID2 = 0x176a; //many keys
var KeyringID3 = 0x176b; //full keyring

var KeyScriptID = 5013; //Script-ID from JSE_FILEASSOCIATIONS.SCP! Change to match your own setting.

const coOwnHousesOnSameAccount = GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ); //CoOwnHousesOnSameAccount();

// Adding keys to keyrings
function onUseChecked( pUser, iUsed )
{
	if( iUsed.movable == 3 )
	{
		// Key is locked down, make sure only owners, co-owners or friends can use it
		var iMulti = iUsed.multi;
		if( !ValidateObject( iMulti ) || ( !iMulti.IsOnOwnerList( pUser ) && !iMulti.IsOnFriendList( pUser )))
		{
			if( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum != pUser.accountNum ))
			{
				pUser.socket.SysMessage( GetDictionaryEntry( 1032, pUser.socket.language )); // That is not yours!
				return false;
			}
		}
	}

	pUser.socket.tempObj = iUsed;
	if(( iUsed.id >= keyID1 && iUsed.id <= keyID3 ) || iUsed.id == keyID4 || iUsed.id == keyID5 )
	{
		if( !iUsed.more )
		{
			pUser.CustomTarget( 3, "This is a key blank. Which key would you like to make a copy of?" );
		}
		else
		{
			pUser.CustomTarget( 0, "What do you want to use this key on?" );
		}
	}
	else if( iUsed.id >= KeyringID1 && iUsed.id <= KeyringID3 )
	{
		pUser.CustomTarget( 1, "What do you want to use this keyring on?" );
	}
	else if( iUsed.id == emptyKeyringID )
	{
		pUser.CustomTarget( 2, "Select a key to add to this keyring." );
	}
	return false;
}

//Key targetting
function onCallback0( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var Key = pSock.tempObj;
	pSock.tempObj = null;

	// If player is now out of range of the key, disallow it!
	if(	!pUser.InRange( Key, 3 ))
	{
		pUser.SysMessage( "Key is out of range!" );
		return;
	}

	var StrangeByte = pSock.GetWord( 1 );
	if( !StrangeByte && myTarget.isItem )
	{
		// If targeting an item that is out of range, return
		if( !pUser.InRange( myTarget, 3 ))
		{
			pUser.SysMessage( GetDictionaryEntry( 393, pSock.language )); //That is too far away.
		}

		//If targeting a full keyring
		var targetType = myTarget.type;
		if( myTarget.id == KeyringID3 )
		{
 			pUser.SysMessage("Sorry, that keyring is full ");
 			return;
		}
		//if targeting a keyring, add key to it
		else if( myTarget.id == KeyringID1 || myTarget.id == KeyringID2 || myTarget.id == emptyKeyringID)
		{
			if( myTarget.container.serial != pUser.pack.serial )
			{
				pUser.SysMessage( GetDictionaryEntry( 1763, pSock.language )); //That item must be in your backpack before it can be used.
			}
			else
			{
				if( myTarget.id == emptyKeyringID )
				{
					myTarget.id = KeyringID1;
				}
				var keys = myTarget.GetTag( "keys" );
				if( !keys )
				{
					keys = 0;
				}
				keys++;
				myTarget.SetTag( "keys", keys );
				myTarget.SetTag( "key"+keys+"more", (Key.more).toString() );
				myTarget.SetTag( "key"+keys+"name", Key.name );
				myTarget.SetTag( "keyid"+keys, Key.id );
				pUser.SysMessage( "You've added the key to the keyring." );
				pUser.SysMessage( "There are now "+keys+" keys on this keyring." );
				Key.Delete();
				if( keys > 1 && keys < 5 )
				{
					myTarget.id = KeyringID2;
				}
				if( keys == 5 )
				{
					myTarget.id = KeyringID3;
				}
			}
			return;
		}
		//If targeting a lock
		else if( targetType == 1 || targetType == 8 || targetType == 12 || targetType == 13 || targetType == 117 || targetType == 63 || targetType == 64 || targetType == 203 )
		{
			if( myTarget.more == Key.more || Key.more == 255 ) //Key.more == 255 is a universal key
			{
				if( targetType == 12 || targetType == 13 )
				{
					// Don't allow changing lock status on FRONT doors in public houses
					var lockMulti = myTarget.multi
					if( ValidateObject( lockMulti ) && lockMulti.isPublic && myTarget.GetTag( "DoorType" ) == "front" )
					{
						pSock.SysMessage( "You cannot lock the front door of a public house!" );
						return;
					}
				}
				changeLockStatus( pUser, myTarget );
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 1026, pSock.language )); //The key does not fit into that lock.
			}
			return;
		}
		//If targeting key itself, rename key
		else if( myTarget.serial == Key.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1019, pSock.language )); //Enter new name for key.
			pUser.socket.tempObj2 = Key;
			pUser.SpeechInput(1);
			return;
		}
	}
	pUser.SysMessage( GetDictionaryEntry( 1025, pSock.language )); //That does not have a lock.
}

function onCallback1( pSock, myTarget )
{ //Keyring targeting
	var pUser = pSock.currentChar;
	var Keyring = pSock.tempObj;
	pSock.tempObj = null;
	var StrangeByte = pSock.GetWord( 1 );
	var keys, i;

	if( !StrangeByte && myTarget.isItem )
	{
		// If targeting an item that is out of range, return
		if( myTarget.type != 203 && !pUser.InRange( myTarget, 3 ))
		{
			pUser.SysMessage( GetDictionaryEntry( 393, pSock.language )); //That is too far away.
		}

		if( myTarget.serial == Keyring.serial )
		{ //If targeting keyring itself, release all keys
			keys = Keyring.GetTag( "keys" );
			var totalKeys = keys;
			var KeyringCont = Keyring.container;
			i = 1;
			for( i = 1; i < totalKeys + 1; i++ )
			{
				if( ValidateObject( KeyringCont ))
				{
					if( KeyringCont.totalItemCount >= KeyringCont.maxItems )
					{
						pSock.SysMessage( GetDictionaryEntry( 1819 ), pSock.language ); // Your backpack cannot hold any more items!
						break;
					}
				}
				var keyID = Keyring.GetTag( "keyid"+i );
				var HexKeyID = keyID.toString(16);
				var newKey = CreateDFNItem( pUser.socket, pUser, "0x"+HexKeyID, 1, "ITEM", true );
				keys--;
			}
			if( keys <= 0 )
			{
				Keyring.id = emptyKeyringID;
				Keyring.SetTag( "keys", 0 );
				pUser.SysMessage( "You release all keys from the keyring." );
			}
			else
			{
				Keyring.SetTag( "keys", keys );
				if( keys == 1 )
					Keyring.id == KeyringID1;
				else if( keys > 1 && keys < 5 )
					Keyring.id = KeyringID2;
				else if( keys >= 5 )
					Keyring.id = KeyringID3;
			}
			return;
		}
		else if( myTarget.type == 1 || myTarget.type == 8 || myTarget.type == 12 || myTarget.type == 13 || myTarget.type == 117 || myTarget.type == 63 || myTarget.type == 64 || myTarget.type == 203 )
		{ //If key & lock has same MORE value, i.e. they are a match
			keys = Keyring.GetTag( "keys" );
			i = 1;
			for( i = 1; i < keys + 1; i++ )
			{
				var KeyItemMore = Keyring.GetTag( "key"+i+"more" );
				if( KeyItemMore == myTarget.more || KeyItemMore == 255 )
				{
					changeLockStatus( pUser, myTarget );
					return;
				}
			}
			pUser.SysMessage( "None of your keys fit the lock." );
			return;
		}
	}
	pUser.SysMessage( GetDictionaryEntry( 1025, pSock.language )); //That does not have a lock.
}

function onCallback2( pSock, myTarget )
{ //Empty keyring targeting
	var pUser = pSock.currentChar;
	var emptyKeyring = pSock.tempObj;
	pSock.tempObj = null;
	var StrangeByte = pSock.GetWord( 1 );
	if( !StrangeByte && myTarget.isItem )
	{
		// If targeting an item that is out of range, return
		if( !pUser.InRange( myTarget, 3 ))
		{
			pUser.SysMessage( GetDictionaryEntry( 393, pSock.language )); //That is too far away.
		}

		if(( myTarget.id >= keyID1 && myTarget.id <= keyID3 ) || myTarget.id == keyID4 || myTarget.id == keyID5 )
		{
			var itemOwner = GetPackOwner( myTarget, 0 );
			if( !itemOwner || itemOwner != pUser )
			{
				pUser.SysMessage( GetDictionaryEntry( 1763, pSock.language )); //That item must be in your backpack before it can be used.
			}
			if( !myTarget.more )
			{ //If key isn't associated with any key, cancel
				pUser.SysMessage( "That key has no lock defined for it!" );
			}
			else
			{
				var scriptFound = false;
				var scriptTriggers = myTarget.scriptTriggers;
				for( var i = 0; i < scriptTriggers.length; i++ )
				{
					if( scriptTriggers[i] == KeyScriptID )
						scriptFound = true;
				}
				if( !scriptFound ) // If targetted key doesn't use this script, make it :)
				{
					myTarget.AddScriptTrigger( KeyScriptID );
				}
				emptyKeyring.id = KeyringID1;
				emptyKeyring.SetTag( "keys", 1 );
				emptyKeyring.SetTag( "keys1more", (myTarget.more).toString() );
				emptyKeyring.SetTag( "keys1name", myTarget.name );
				emptyKeyring.SetTag( "keyid1", myTarget.id );
				pUser.SysMessage( "You've added the key to the keyring." );
				myTarget.Delete();
			}
			return;
		}
		else if( myTarget.serial == emptyKeyring.serial )
		{ //If targeting empty keyring, rename it
			pUser.SysMessage( "Enter the new name for this keyring:" );
			pUser.socket.tempObj2 = emptyKeyring;
			pUser.SpeechInput(2);
			return;
		}
	}
	pUser.SysMessage( "That is not a key." );
}

//Using a blank key, target a key to copy
function onCallback3( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var keyBlank = pSock.tempObj;
	pSock.tempObj = null;
	var StrangeByte = pSock.GetWord( 1 );
	if( !StrangeByte && myTarget.isItem )
	{
		var itemOwner = GetPackOwner( myTarget, 0 );
		if( itemOwner && itemOwner != pUser )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, pSock.language )); //That item must be in your backpack before it can be used.
			return;
		}
		else
		{
			if( !pUser.InRange( myTarget, 3 ))
			{
				pUser.SysMessage( GetDictionaryEntry( 393, pSock.language )); //That is too far away.
				return;
			}
			else
			{
				if(( myTarget.id >= keyID1 && myTarget.id <= keyID3 ) || myTarget.id == keyID4 || myTarget.id == keyID5 )
				{
					if( myTarget.more )
					{
						if( pUser.CheckSkill( 37, 400, 1000 ) )
						{
							pUser.SysMessage( GetDictionaryEntry( 1017, pSock.language )); //You copy the key.
							keyBlank.more = myTarget.more;
						}
						else
						{
							pUser.SysMessage( GetDictionaryEntry( 1016, pSock.language )); //You fail and destroy the key blank.
							keyBlank.Delete();
						}
					}
					else
					{
						pUser.SysMessage( "This key is also a blank." );
					}
					return;
				}
			}
		}
	}
	pUser.SysMessage( "You can't make a copy of that." );
}

function onSpeechInput(pUser, pItem, pSpeech, pSpeechID)
{
	var KeyItem = pUser.socket.tempObj2;
	pUser.socket.tempObj2 = null;
	if( pSpeech )
	{
    	switch(pSpeechID)
    	{
			case 1: //Rename Keys
				KeyItem.name = pSpeech;
				pUser.SysMessage( "The new name for this key is: "+pSpeech );
				break;
			case 2: //Rename Keyrings
				KeyItem.name = pSpeech;
				pUser.SysMessage( "The new name for this keyring is: "+pSpeech );
				break;
			case 3: //Rename house-sign
			{
				var keyItemMulti = KeyItem.multi;
				if( keyItemMulti.IsOwner( pUser ))
				{
					if( pSpeech.length > 60 )
					{
						pUser.SysMessage( "House name must shorter than 60 characters long!" );
					}
					else
					{
						KeyItem.name = pSpeech;
						pUser.SysMessage( "Renamed to: " + pSpeech );
					}
				}
				else
				{
					pUser.SysMessage( "Only the house owner can rename the house!" );
				}
				break;
			}
		}
	}
}

function changeLockStatus( pUser, myTarget )
{
	var pSock = pUser.socket;
	switch( myTarget.type )
	{
		case 1: //Unlocked container
			myTarget.type = 8;
			pUser.SysMessage( GetDictionaryEntry( 1018, pSock.language )); //You lock the container.
			break;
		case 8: //Locked container
			myTarget.type = 1;
			pUser.SysMessage( GetDictionaryEntry( 1020, pSock.language )); //You unlock the container.
			break;
		case 12: //Unlocked door
			pUser.SysMessage( GetDictionaryEntry( 1021, pSock.language )); //You lock the door.
			myTarget.type = 13;
			if( myTarget.isDoorOpen )
			{
				//If the door is open when you lock it, close the door!
				TriggerEvent( 4500, "UseDoor", myTarget, true );
			}
			break;
		case 13: //Locked door
			pUser.SysMessage( GetDictionaryEntry( 1022, pSock.language )); //You unlock the door.
			myTarget.type = 12;
			break;
		case 63: //spawn-container
			pUser.SysMessage( GetDictionaryEntry( 1018, pSock.language )); //You lock the container.
			myTarget.type = 64;
			break;
		case 64: //locked spawn-container
			pUser.SysMessage( GetDictionaryEntry( 1020, pSock.language )); //You unlock the container.
			myTarget.type = 63;
			break;
		case 117: //The plank on a boat
			var plankStatus;
			switch( myTarget.id )
			{
				//Open plank->
				case 0x3EE9: myTarget.id = 0x3E84; plankStatus = 1; break;
				case 0x3EB1: myTarget.id = 0x3ED5; plankStatus = 1; break;
				case 0x3EB2: myTarget.id = 0x3ED4; plankStatus = 1; break;
				case 0x3E8A: myTarget.id = 0x3E89; plankStatus = 1; break;
				case 0x3E85: myTarget.id = 0x3E84; plankStatus = 1; break;
				//Close Plank->
				case 0x3E84: myTarget.id = 0x3EE9; plankStatus = 0; break;
				case 0x3ED5: myTarget.id = 0x3EB1; plankStatus = 0; break;
				case 0x3ED4: myTarget.id = 0x3EB2; plankStatus = 0; break;
				case 0x3E89: myTarget.id = 0x3E8A; plankStatus = 0; break;
				default:break;
			}
			if( !plankStatus )
			{
				pUser.SysMessage( GetDictionaryEntry( 2032, pSock.language )); //You close the plank.
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 2031, pSock.language )); //You open the plank.
			}
			break;
		case 203: //A house sign
		{
			var signMulti = myTarget.multi;
			if( signMulti.IsOwner( pUser ))
			{
				pUser.socket.tempObj2 = myTarget;
				pUser.SysMessage( GetDictionaryEntry( 1023, pSock.language )); //What do you wish the sign to say?
				pUser.SpeechInput(3);
			}
			else
			{
				pUser.SysMessage( "Only the house owner can rename the house!" );
			}
			break;
		}
	}
	pUser.SoundEffect( 0x241, 1 );
}
