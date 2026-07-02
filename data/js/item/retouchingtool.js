function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( ValidateObject( iUsed ) && iUsed.isItem )
	{
		//Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 774, pSocket.language )); // That is locked down and you cannot use it
			return false;
		}

		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial ) 
		{
			pSocket.SysMessage( GetDictionaryEntry( 6589, pSocket.language )); // You must have the object in your backpack to use it.
			return false;
		}
		else
		{
			pSocket.tempObj = iUsed;
			pSocket.CustomTarget( 0, GetDictionaryEntry( 6590, pSocket.language )); // Target the ethereal mount you wish to retouch.
			iUsed.Refresh();
		}
	}
	return false;
}

function onCallback0( socket, myTarget )
{
	var disallowedMountsRetouch = {
	"etherealwarboarstatuette": true
	};

	if( socket == null )
		return;

	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;
	var toolOwner = GetPackOwner( iUsed, 0 );
	if( toolOwner == null || toolOwner.serial != pUser.serial )
	{
		socket.SysMessage( GetDictionaryEntry( 6591, socket.language )); // That must be in your pack for you to use it.
		return;
	}

	if( !ValidateObject( myTarget ) || !myTarget.isItem )
	{
		socket.SysMessage( GetDictionaryEntry( 6592, socket.language )); // That is not a valid target.
		return;
	}

	var targetOwner = GetPackOwner( myTarget, 0 );
	if( targetOwner == null || targetOwner.serial != pUser.serial )
	{
		socket.SysMessage( GetDictionaryEntry( 6593, socket.language )); // You must have the item in your backpack to target it.
		return;
	}

	var sectionID = myTarget.sectionID;
	if( disallowedMountsRetouch[sectionID] )
	{
		socket.SysMessage( GetDictionaryEntry( 6594, socket.language )); // You cannot use this item for it.
		return;
	}

	var currentState = myTarget.GetTag( "retouching" );
	var savedHue = parseInt( myTarget.GetTag( "saveColor" ));
	if( currentState == "transparent" )
	{
		socket.SysMessage( GetDictionaryEntry( 6595, socket.language )); // Your ethereal mount's body has been solidified.
		myTarget.SetTag( "retouching", "normal" );

		// Restore custom hue if one was saved
		if( savedHue != 0 )
		{
			myTarget.color = savedHue;
		}
		else
		{
			myTarget.color = 0;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 6596, socket.language )); // Your ethereal mount's transparency has been restored.
		myTarget.SetTag( "retouching", "transparent" );
		myTarget.color = 0;
	}

	myTarget.Refresh();
}
