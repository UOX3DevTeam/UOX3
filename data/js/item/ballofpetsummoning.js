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
					var tempPet = pUser.GetTag( "stabledPet" + i );
					if( tempPet != null && tempPet != "0" )
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

function _restorecontext_() { }