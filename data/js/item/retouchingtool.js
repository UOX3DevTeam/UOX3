function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;

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
			socket.SysMessage( "You must have the object in your backpack to use it." ); // You must have the object in your backpack to use it.
			return false;
		}
		else
		{
			socket.tempObj = iUsed;
			socket.CustomTarget( 0, "Target the ethereal mount you wish to retouch." );
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

	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;
	var toolOwner = GetPackOwner( iUsed, 0 );

	if( toolOwner == null || toolOwner.serial != pUser.serial )
	{
		pUser.SysMessage( "That must be in your pack for you to use it." );
		return;
	}

	if( !myTarget || !myTarget.isItem )
	{
		pUser.SysMessage( "That is not a valid target." );
		return;
	}

	var targetOwner = GetPackOwner( myTarget, 0 );
	if( targetOwner == null || targetOwner.serial != pUser.serial )
	{
		pUser.SysMessage( "You must have the item in your backpack to target it." );
		return;
	}

	var sectionID = myTarget.sectionID;
	if( disallowedMountsRetouch[sectionID] )
	{
		pUser.SysMessage( "You cannot use this item for it." );
		return;
	}

	var currentState = myTarget.GetTag( "retouching" );
	var savedHue = parseInt( myTarget.GetTag( "saveColor" ));
	if( currentState == "transparent" )
	{
		pUser.SysMessage( "Your ethereal mount's body has been solidified." );
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
		pUser.SysMessage( "Your ethereal mount's transparency has been restored." );
		myTarget.SetTag( "retouching", "transparent" );
		myTarget.color = 0;
	}

	myTarget.Refresh();
}

function onTooltip( etherealStatuette )
{
	var tooltipText = "";
	var vetReward = etherealStatuette.GetTag( "vetRewardYear" );

	if( vetReward )
	{
		var indicator = "th";
		switch( vetReward )
		{
			case 1: indicator = "st"; break;
			case 2: indicator = "nd"; break;
			case 3: indicator = "rd"; break;
		}
		tooltipText = vetReward + indicator + " Year Veteran Reward";
	}

	if( tooltipText != "" )
		etherealStatuette.SetTempTag( "tooltipSortOrder", 21 );

	return tooltipText;
}

function _restorecontext_() {}