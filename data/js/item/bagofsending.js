const maxCharges = 30;		// Default maximum amount of charges a bag of sending can hold
function onCreateDFN( objMade, objType )
{
	if ( objType == 0 ) 
	{
		var Charges = 0;
		switch ( RandomNumber( 0, 2 ))
		{
			case 0: Charges = 3; break;
			case 1: Charges = 6; break;
			case 2: Charges = 9; break;
		}
		objMade.SetTag( "Charges", Charges )
		objMade.SetTag( "maxCharges", maxCharges )
	}
}

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var Charges = iUsed.GetTag( "Charges" )
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
			pUser.SysMessage( "The bag of sending must be in your backpack." ); // The bag of sending must be in your backpack.
			return false;
		}
		else if( Charges == 0 )
		{
			pUser.SysMessage( "This item is out of charges." ); // This item is out of charges.
			return false;
		}
		else
		{
			socket.tempObj = iUsed;
			socket.CustomTarget( 0 );
			iUsed.Refresh();
		}
	}
	return false;
}

function onCallback0( socket, myTarget)
{
	var pUser = socket.currentChar;
	var requiredCharges = 1;
	var iUsed = socket.tempObj;
	var Charges = parseInt( iUsed.GetTag( "Charges" ));
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pUser.SysMessage( "The bag of sending must be in your backpack" ); // The bag of sending must be in your backpack
		return false;
	}
	else if( Charges == 0 )
	{
		pUser.SysMessage( "This item is out of charges." ); // This item is out of charges.
		return false;
	}

	if( !socket.GetWord( 1 ) && myTarget.isItem )
	{
		var bankBox = pUser.FindItemLayer( 29 );
		if( !ValidateObject( bankBox ))
		{
			socket.SysMessage( "Error: No valid bankbox found! Please contact a GM for assistance." );
			Console.Error( "No valid bankbox found for character " + pUser.name + " with serial " + ( pUser.serial ).toString() + "!" );
			return false;
		}
		else if( !ValidateObject( myTarget.container ) || myTarget.container != pUser.pack )
		{
			socket.SysMessage( "You may only send items from your backpack to your bank box." ); // You may only send items from your backpack to your bank box.
			return false;
		}
		else if( myTarget.type == 1 || myTarget == iUsed )
		{
			socket.SysMessage( "You cannot send a container through the bag of sending." ); // You may only send items from your backpack to your bank box.
			return false;
		}
		else if( bankBox.totalItemCount >= bankBox.maxItems )
		{
			socket.SysMessage( "Your bank box is full." ); // Your bank box is full.
			return false;
		}
		else if( bankBox.weight > bankBox.weightMax )
		{
			socket.SysMessage( "Your bank box is full." ); // Your bank box is full.
			return false;
		}
		else if( requiredCharges > Charges )
		{
			pUser.SysMessage( "You don't have enough charges to send that much weight" ); // You don't have enough charges to send that much weight
			return false;
		}
		else
		{
			iUsed.SetTag( "Charges", Charges - requiredCharges );
			socket.SysMessage( "The item was placed in your bank box." ); // The item was placed in your bank box.
			myTarget.container = bankBox;
			iUsed.Refresh();
		}
	}
	else
	{
		socket.SysMessage( "The bag of sending rejects that item." ); // The bag of sending rejects that item.
		return false;
	}
}

function onTooltip( bagOfSending )
{
	var tooltipText = "";
	var Charges = parseInt( bagOfSending.GetTag( "Charges" ));
	tooltipText = "charges: " + Charges;
	return tooltipText;
}