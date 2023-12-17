var maxControlSlots = GetServerSetting("MaxControlSlots");
var maxFollowers = GetServerSetting("MaxFollowers");
function onCreateDFN( objMade, objType )
{
	const maxCharges = 30;
	if( objType == 0 ) 
	{
		var Charges = 0;
		switch( RandomNumber( 0, 2 ))
		{
			case 0: Charges = 3; break;
			case 1: Charges = 6; break;
			case 2: Charges = 9; break;
		}
		objMade.SetTag( "charges", Charges );
		objMade.SetTag( "maxCharges", maxCharges );
	}
}

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var Charges = iUsed.GetTag( "charges" );

	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( socket && iUsed && iUsed.isItem )
	{
		//Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); // That is locked down and you cannot use it
			return false;
		}

		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial ) 
		{
			pUser.SysMessage( GetDictionaryEntry( 18748, socket.language )); // That must be in your pack for you to use it.
			return false;
		}
		else if( Charges == 0 )
		{
			pUser.SysMessage( GetDictionaryEntry( 0, socket.language )); // This item is out of charges.
			return false;
		}
		else
		{
			if( iUsed.GetTag( "linked" ) == 0 )
			{
				socket.tempObj = iUsed;
				socket.CustomTarget( 0, GetDictionaryEntry( 19050, socket.language ));// Target your pet that you wish to link to this Crystal Ball of Pet Summoning.
			}
			else
			{
				SummonPet( pUser, iUsed );
			}
		}
	}
	return false;
}

function onCallback0( socket, myTarget)
{
	var pUser = socket.currentChar;
	var requiredCharges = 1;
	var iUsed = socket.tempObj;
	var Charges = parseInt( iUsed.GetTag( "charges" ));

	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pUser.SysMessage( GetDictionaryEntry( 18748, socket.language )); // That must be in your pack for you to use it.
		return false;
	}
	else if( Charges == 0 )
	{
		pUser.SysMessage( GetDictionaryEntry( 0, socket.language )); // This item is out of charges.
		return false;
	}

	if( myTarget.tamed )
	{
		if( myTarget.owner != pUser )
		{
			pUser.SysMessage( GetDictionaryEntry( 19051, socket.language )); // You may only link your own pets to a Crystal Ball of Pet Summoning.
			return false;
		}
		else if( requiredCharges > Charges )
		{
			pUser.SysMessage( GetDictionaryEntry( 19052, socket.language )); // The Crystal Ball darkens. It must be charged before it can be used again.
			return false;
		}
		else
		{
			iUsed.SetTag( "petSerial", myTarget.serial );
			iUsed.SetTag( "linked", 1 );
			socket.SysMessage( GetDictionaryEntry( 19053, socket.language )); // Your pet is now linked to this Crystal Ball of Pet Summoning.
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 19054, socket.language )); // Only pets can be linked to this Crystal Ball of Pet Summoning.
		return false;
	}
}

function SummonPet( pUser, iUsed )
{
	var socket = pUser.socket;
	var requiredCharges = 1;
	var Pet = CalcCharFromSer( parseInt( iUsed.GetTag( "petSerial" )));
	var Charges = parseInt( iUsed.GetTag( "charges" ));

	if( Charges == 0)
	{
		pUser.SysMessage( GetDictionaryEntry( 19055, socket.language )); // The Crystal Ball darkens. It must be charged before it can be used again.
		return false;
	}
	else if( ValidateObject( Pet ) && Pet.mounted == true)
	{
		socket.SysMessage( GetDictionaryEntry( 19056, socket.language )); // The Crystal Ball fills with a yellow mist. Why would you summon your pet while riding it?
		return false;
	}
	else if( maxControlSlots > 0 && (pUser.controlSlotsUsed + iUsed.morez > maxControlSlots) || pUser.petCount >= maxFollowers ) 
	{
		socket.SysMessage( GetDictionaryEntry( 19057, socket.language )); // The Crystal Ball fills with a blue mist. Your pet is not responding to the summons.
		return false;
	}
	else if( ValidateObject( Pet ) && Pet.owner != pUser )
	{
		socket.SysMessage( GetDictionaryEntry( 19058, socket.language )); // The Crystal Ball fills with a grey mist. You are not the owner of the pet you are attempting to summon.
		return false;
	}
	else if( iUsed.GetTempTag( "delay" ) == 1)
	{
		socket.SysMessage( GetDictionaryEntry( 19059, socket.language )); // You must wait a few seconds before you can summon your pet.
		return false;
	}
	else
	{
		if( ValidateObject( Pet ))
		{
			var totalStabledPets = parseInt( pUser.GetTag( "totalStabledPets" ));
			var petFound = false;
			if( totalStabledPets > 0 )
			{
				var maxStabledPets = parseInt( pUser.GetTag( "maxStabledPets" ));
				var i = 0;
				for( i = 0; i < maxStabledPets; i++ )
				{
					var tempPetSerial = pUser.GetTag( "stabledPet" + i );
					if( tempPetSerial != null && tempPetSerial != "0" && tempPetSerial == Pet.serial )
					{
						ReleasePet( Pet, i, pUser );
						petFound = true;
						return true;
					}
				}
			}
			else if( petFound == false )
			{
				Pet.Teleport( pUser );
			}
			iUsed.SetTag( "charges", Charges - requiredCharges);
			iUsed.SetTempTag( "delay", 1 );
			iUsed.StartTimer( 15000, 0, true );
			socket.SysMessage( GetDictionaryEntry( 19060, socket.language )); // The Crystal Ball fills with a green mist. Your pet has been summoned.
		}
	}
}

function ReleasePet( petObj, petNum, pUser )
{
	// Reset some values for the pet, teleport it back to owner's location
	petObj.stabled = 0;
	petObj.visible = 0;
	petObj.frozen = false;
	petObj.owner = pUser;
	petObj.hungerstatus = true;
	petObj.willthirst = true;
	petObj.vulnerable = true;
	petObj.Teleport( pUser );
	var totalStabledPets = parseInt( pUser.GetTag( "totalStabledPets" ));
	totalStabledPets--;
	pUser.SetTag( "totalStabledPets", totalStabledPets );
	pUser.SetTag( "stabledPet" + petNum, null );
	pUser.SetTag( "stableMasterSerial", null );
	pUser.controlSlotsUsed = pUser.controlSlotsUsed + petObj.controlSlots;
	pUser.AddFollower( petObj );
	petObj.Follow( pUser );
}

function onTooltip( iUsed, pSocket )
{
	var tooltipText = "";
	var Charges = parseInt( iUsed.GetTag( "charges" ));
	var Pet = CalcCharFromSer( parseInt( iUsed.GetTag( "petSerial" )));
	tooltipText = GetDictionaryEntry( 9252, pSocket.language )+ " " + Charges;// Charges:
	if( ValidateObject( Pet ))
	{
		tooltipText += "\n" + GetDictionaryEntry( 19061, pSocket.language ) + " " + Pet.name;
	}
	return tooltipText;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 0 )
	{
		iUsed.SetTempTag( "delay", null );
	}
}

function onContextMenuRequest( socket, targObj )
{
	var Pet = CalcCharFromSer(parseInt( targObj.GetTag( "petSerial" )));
	// handle your own packet with context menu here
	var pUser = socket.currentChar;
	var offset = 12;
	if( ValidateObject( Pet ))
	{
		var numEntries = 3;
	}
	else
	{
		var numEntries = 1;
	}

	// Prepare packet
	var toSend = new Packet();
	var packetLen = ( 12 + ( numEntries * 8 ));
	toSend.ReserveSize( packetLen );
	toSend.WriteByte( 0, 0xBF );
	toSend.WriteShort( 1, packetLen );
	toSend.WriteShort( 3, 0x14); // subCmd
	toSend.WriteShort( 5, 0x0001); // 0x0001 for 2D client, 0x0002 for KR (maybe this needs to be 0x0002?)
	toSend.WriteLong( 7, targObj.serial );
	toSend.WriteByte( 11, numEntries ); // Number of entries

	if( ValidateObject( Pet ))
	{
		toSend.WriteShort( offset, 0x000a );    // Unique ID
		toSend.WriteShort( offset += 2, 6181 ); // summon pet
		toSend.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
		toSend.WriteShort( offset += 2, 0x03E0 ); // Hue of text

		offset += 2; // for each additional entry

		toSend.WriteShort( offset, 0x000b );    // Unique ID
		toSend.WriteShort( offset += 2, 6183 ); // update pet name
		toSend.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
		toSend.WriteShort( offset += 2, 0x03E0 ); // Hue of text

		offset += 2; // for each additional entry

		toSend.WriteShort( offset, 0x000c );    // Unique ID
		toSend.WriteShort( offset += 2, 6182 ); //unlink
		toSend.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
		toSend.WriteShort( offset += 2, 0x03E0 ); // Hue of text
	}
	else
	{

		toSend.WriteShort( offset, 0x000a );    // Unique ID
		toSend.WriteShort( offset += 2, 6180 ); // summon pet
		toSend.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
		toSend.WriteShort( offset += 2, 0x03E0 ); // Hue of text
	}

	//Send packet
	socket.Send( toSend );
	toSend.Free();

	return false;
}

function onContextMenuSelect( socket, targObj, popupEntry )
{
	var Pet = CalcCharFromSer( parseInt( targObj.GetTag( "petSerial" )));
	var pUser = socket.currentChar;
	// Handle checking popupEntry here, to make sure it matches one of the context menu options sent in custom packet in onContextMenuRequest
	switch( popupEntry ) 
	{
		case 10:
			if( ValidateObject( Pet ))
			{
				SummonPet( pUser, targObj );
			}
			else
			{
				socket.tempObj = targObj;
				socket.CustomTarget( 0, GetDictionaryEntry( 19050, socket.language ));// Target your pet that you wish to link to this Crystal Ball of Pet Summoning.
			}
			break;
		case 11:
			pUser.SpeechInput( 1, Pet );
			targObj.Refresh();
			break;
		case 12:
			socket.SysMessage( GetDictionaryEntry( 19062, socket.language )); // This crystal ball is no longer linked to a pet.
			targObj.SetTag( "petSerial", null );
			targObj.SetTag( "linked", null );
			break;
	}
	return false;
}

function onSpeechInput( pUser, targObj, pSpeech, pSpeechID )
{
	var pSocket = pUser.socket;
	if( pSpeech == null || pSpeech == " " )
	{
		pSocket.SysMessage( GetDictionaryEntry( 9270, pSocket.language )); // That name is too short, or no name was entered.
		return;
	}

	switch( pSpeechID )
	{
		case 1: // Rename Pet
			if( pSpeech.length > 50 )
			{
				pSocket.SysMessage( GetDictionaryEntry( 9271, pSocket.language )); // That name is too long. Maximum 50 chars.
				return;
			}

			if( ValidateObject( targObj ))
			{
				targObj.name = pSpeech;
				var tempMsg = GetDictionaryEntry( 19063, socket.language ); // The new name of the Pet is: %s
				pSocket.SysMessage( tempMsg.replace( /%s/gi, targObj.name ));
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 19064, socket.language )); // That does not seem to be a valid pet.
			}
			break;
		default:
			break;
	}
}

function _restorecontext_() { }