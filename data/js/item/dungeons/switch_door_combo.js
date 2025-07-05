// Switch Door Combo
//
// This script can be used to link up switches to doors, which will be
// unlocked/opened when the switch is flipped.
//
// 1 switch can be linked with up to multiple doors via serials of those doors
// stored as a comma-separated string in the 'doorSerials' tag on the switch
// (This must be done in-game, as DFN entries have no serials - they're just blueprints)
//
// 'switchState' Tag value (set by script) on the switch determines whether it has been flipped (2)
// or not (1), while 'switchType' Tag determines whether the changes triggered by the switch are
// toggleable (1) or lasts a certain amount of time (2). If the latter, switch cannot be
// interacted with again until the amount of time in milliseconds defined in 'switchTimer' Tag has
// passed and both switch and door have reset. If the former, 'switchTimer' Tag instead defines
// amount of time in milliseconds until switch can be toggled again.
// If the tags 'switchActiveID' and 'switchInactiveID' are present, they will be used to
// set the ID of the switch when flipped/resetting. If not, script will just increment
// ID by 1 when activated, and reduce it by 1 again when resetting.
//
// MORE (part 1) value on door determines type of interaction switch triggers
//	1 = unlock, changes a locked door to an unlocked one
//	2 = raise, raises door by total height set in MORE (part 2), over interval (1 = 100 ms) set in MORE (part 3)
//	3 = lower, lowers door by total height set in MORE (part 2), over interval (1 = 100 ms) set in MORE (part 3)
//	4 = shift door west along X axis by number of tiles set in MORE (part 2)
//	5 = shift door east along X axis by number of tiles set in MORE (part 2)
//	6 = shift door north along Y axis by number of tiles set in MORE (part 2)
//	7 = shift door south along Y axis by number of tiles set in MORE (part 2)
//	8 = hide, hides the door, blinks it out of existence
// For all interactions, an optional SFX can be played on the door, as defined
// by the 'doorActivateSFX' and 'doorResetSFX' Tags.
//
// For all interactions, an optional VFX can be played on the door, as defined
// by the 'doorActivateVFX' and 'doorResetVFX' Tags.
//
// When the switch is flipped, it will display a text message above the switch, visible only to
// the player who flipped the switch. The message displayed will either be a couple of default
// ones, or one determined by optional 'switchActivateMsg' or 'switchResetMsg' Tags on the switch.

const switchScriptID = 4761; // ID of this script in jse_fileassociations.scp

function onUseChecked( pUser, iSwitch )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iSwitch ))
		return;

	if( !pUser.CanSee( iSwitch ))
		return;

	// Perform initialization first of all, to perform initial
	// required link-up with doors based on a unique ID
	if( !iSwitch.GetTag( "isInitialized" ))
	{
		let pSock = pUser.socket;
		if( pSock == null || !InitializeSwitchDoorCombo( iSwitch, pSock ))
		{
			Console.Warning( "Failed to Initialize Switch/Door combo for switch with serial: " + iSwitch.serial.toString() );
			iSwitch.TextMessage( "You try to flip the switch, but nothing happens.", false, 0x3b2, 0, pUser.serial );
			return;
		}
	}

	// Validate the type of switch we are dealing with
	let switchType = iSwitch.GetTag( "switchType" );
	if( switchType != 1 && switchType != 2 )
	{
		Console.Warning( "Switch (serial: " + iSwitch.serial.toString() + ") has unknown switch type (" + switchType + "), aborting script execution!" );
		return;
	}

	// Check if switch has already been flipped
	let hasBeenFlipped = iSwitch.GetTag( "switchState" );
	if( hasBeenFlipped == 2 ) // Switch has already been flipped
	{
		if( switchType == 2 )
		{
			// Timer based. Player needs to wait to use switch again!
			iSwitch.TextMessage( "This mechanism cannot be engaged again so soon.", false, 0x3b2, 0, pUser.serial );
			return;
		}
		else if( switchType == 1 )
		{
			let switchOnCooldown = iSwitch.GetTag( "switchOnCooldown" );
			if( switchOnCooldown )
			{
				iSwitch.TextMessage( "That seems stuck for the moment. Try again later!", false, 0x3b2, 0, pUser.serial );
			}
			else
			{
				// Toggle the switch, reset back to original
				ResetSwitchAndDoors( iSwitch );
			}
			return;
		}
	}

	// Trigger the door effects
	TriggerSwitchAndDoorEffects( iSwitch );

	// Set Switch Timer
	let switchTimer = parseInt( iSwitch.GetTag( "switchTimer" ));
	if( switchTimer > 0 )
	{
		if( switchType == 1 )
		{
			iSwitch.SetTag( "switchOnCooldown", 1 );
			iSwitch.StartTimer( switchTimer, 0, switchScriptID );
		}
		else if( switchType == 2 )
		{
			iSwitch.StartTimer( switchTimer, 1, switchScriptID );
		}
	}
}

function TriggerSwitchAndDoorEffects( iSwitch )
{
	// Iterate over each door associated with switch, and execute
	// the appropriate "action" for each door
	let doorList = [];
	let doorSerialList = [];
	let doorSerials = iSwitch.GetTag( "doorSerials" );
	if( doorSerials == 0 || typeof( doorSerials ) !== 'string' || doorSerials.length == 0 )
	{
		Console.Warning( "No doors associated with switch (" + iSwitch.serial.toString() + ") were found. Aborting script execution, and resetting switch init state." );
		iSwitch.SetTag( "isInitialized", null );
		iSwitch.KillJSTimer( 1, switchScriptID );
		return;
	}
	else
	{
		doorSerialList = doorSerials.split( "," );
		if( doorSerialList.length > 0 )
		{
			for( let i = 0; i < doorSerialList.length; i++ )
			{
				let tempDoor = CalcItemFromSer( parseInt( doorSerialList[i] ));
				if( ValidateObject( tempDoor ))
				{
					doorList.push( tempDoor );
				}
			}
		}
	}

	// Let's proceed to flip the switch
	let switchActiveID = parseInt( iSwitch.GetTag( "switchActiveID" ));
	if( switchActiveID )
	{
		iSwitch.id = switchActiveID;
	}
	else if( iSwitch.id == 0x1091 || iSwitch.id == 0x108f )
	{
		iSwitch.id++;
	}
	else
	{
		iSwitch.id--;
	}

	// Set switch to flipped
	iSwitch.SetTag( "switchState", 2 );

	// Play a switch flipping sound
	iSwitch.SoundEffect( 0x004a, true );

	// Display switch message to player
	let switchMsg = iSwitch.GetTag( "switchActivateMsg" );
	if( switchMsg == 0 )
	{
		switchMsg = "You hear a grinding noise echoing in the distance.";
	}
	iSwitch.TextMessage( switchMsg, true, 0x3b2 );

	let switchFlipTimer = iSwitch.GetTag( "switchTimer" );

	for( let i = 0; i < doorList.length; i++ )
	{
		let doorObj = doorList[i];
		if( !ValidateObject( doorObj ))
			continue;

		if( doorObj.GetTag( "swOrigX" ) == 0 )
		{
			doorObj.SetTag( "swOrigX", doorObj.x );
		}
		if( doorObj.GetTag( "swOrigY" ) == 0 )
		{
			doorObj.SetTag( "swOrigY", doorObj.y );
		}
		if( doorObj.GetTag( "swOrigZ" ) == 0 )
		{
			doorObj.SetTag( "swOrigZ", doorObj.z );
		}
		if( doorObj.GetTag( "swOrigType" ) == 0 )
		{
			doorObj.SetTag( "swOrigType", doorObj.type );
		}
		if( doorObj.GetTag( "swOrigID" ) == 0 )
		{
			doorObj.SetTag( "swOrigID", doorObj.id );
		}

		let doorEffect = doorObj.GetMoreVar( "more", 1 );
		switch( doorEffect )
		{
			case 1: // Unlock Door
				if( doorObj.type == 13 )
				{
					doorObj.type = 12;
				}
				break;
			case 2: // Raise Door
				let raiseTimer = doorObj.GetMoreVar( "more", 3 ) * 100;
				doorObj.StartTimer( raiseTimer, 2, switchScriptID );
				break;
			case 3: // Lower Door
				let lowerTimer = doorObj.GetMoreVar( "more", 3 ) * 100;
				doorObj.StartTimer( lowerTimer, 4, switchScriptID );
				break;
			case 4: // Slide Door West
				{
					let xDiff = doorObj.GetMoreVar( "more", 2 );
					doorObj.x -= xDiff;
					break;
				}
			case 5: // Slide Door East
				{
					let xDiff = doorObj.GetMoreVar( "more", 2 );
					doorObj.x += xDiff;
					break;
				}
			case 6: // Slide Door North
				{
					let yDiff = doorObj.GetMoreVar( "more", 2 );
					doorObj.y -= yDiff;
					break;
				}
			case 7: // Slide Door South
				{
					let yDiff = doorObj.GetMoreVar( "more", 2 );
					doorObj.y += yDiff;
					break;
				}
			case 8: // Hide door
				doorObj.id = 0x1; // No draw
				//doorObj.visible = 3;
				doorObj.Refresh();
				break;
			default:
				break;
		}

		// Play optional SFX associated with activating door
		let doorActivateSFX = parseInt( doorObj.GetTag( "doorActivateSFX" ));
		if( doorActivateSFX != 0 )
		{
			doorObj.SoundEffect( doorActivateSFX, true );
		}

		// Play optional VFX associated with activating door
		let doorActivateVFX = parseInt( doorObj.GetTag( "doorActivateVFX" ));
		if( doorActivateVFX != 0 )
		{
			DoStaticEffect( doorObj.x, doorObj.y, doorObj.z, doorActivateVFX, 0x2, 0x2, false );
		}
	}
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	if( timerID == 0 )
	{
		// Set switch off cooldown
		timerObj.SetTag( "switchOnCooldown", null );
	}
	else if( timerID == 1 )
	{
		// Reset switch and associated doors
		ResetSwitchAndDoors( timerObj );
	}
	else if( timerID == 2 )
	{
		// Raise Door - Initial
		let origZ = timerObj.z;

		// Raise door by 1
		timerObj.z++;

		// Let's start another timer if door has not reached dynamic position targZ yet
		let targZ = timerObj.GetMoreVar( "more", 2 );
		if( timerObj.z < origZ + targZ )
		{
			let raiseTimer = timerObj.GetMoreVar( "more", 3 ) * 100;
			timerObj.StartTimer( raiseTimer, 3, switchScriptID );
		}
	}
	else if( timerID == 3 )
	{
		// Raise Door - Continuous
		timerObj.z++;

		let origZ = timerObj.GetTag( "swOrigZ" );
		let targZ = timerObj.GetMoreVar( "more", 2 );
		if( timerObj.z < origZ + targZ )
		{
			let raiseTimer = timerObj.GetMoreVar( "more", 3 ) * 100;
			timerObj.StartTimer( raiseTimer, 3, switchScriptID );
		}
	}
	else if( timerID == 4 )
	{
		// Lower Door - Initial
		let origZ = timerObj.z;

		// Lower door by 1
		timerObj.z--;

		// Let's start another timer if door has not reached dynamic position targZ yet
		let targZ = timerObj.GetMoreVar( "more", 2 );
		if( timerObj.z > origZ - targZ )
		{
			let lowerTimer = timerObj.GetMoreVar( "more", 3 ) * 100;
			timerObj.StartTimer( lowerTimer, 5, switchScriptID );
		}
	}
	else if( timerID == 5 )
	{
		// Lower Door - Continuous
		timerObj.z--;

		let origZ = timerObj.GetTag( "swOrigZ" );
		let targZ = timerObj.GetMoreVar( "more", 2 );
		if( timerObj.z > origZ - targZ )
		{
			let lowerTimer = timerObj.GetMoreVar( "more", 3 ) * 100;
			timerObj.StartTimer( lowerTimer, 5, switchScriptID );
		}
	}
}

function ResetSwitchAndDoors( iSwitch )
{
	// Reset switch state
	iSwitch.SetTag( "switchState", 1 );

	// Reset switch ID
	let switchInactiveID = parseInt( iSwitch.GetTag( "switchInactiveID" ));
	if( switchInactiveID )
	{
		iSwitch.id = switchInactiveID;
	}
	else if( iSwitch.id == 0x1092 || iSwitch.id == 0x1090 )
	{
		iSwitch.id--;
	}
	else
	{
		iSwitch.id++;
	}

	// Play a switch flipping sound
	iSwitch.SoundEffect( 0x004a, true );

	// Display switch message to player
	let switchMsg = iSwitch.GetTag( "switchResetMsg" );
	if( switchMsg == 0 )
	{
		switchMsg = "You flip the switch back to its original position. A faint noise echoes from somewhere nearby.";
	}
	iSwitch.TextMessage( switchMsg, true, 0x3b2 );

	//	Reset doors
	let doorList = [];
	let doorSerialList = [];
	let doorSerials = iSwitch.GetTag( "doorSerials" );
	if( doorSerials == 0 || typeof( doorSerials ) !== 'string' || doorSerials.length == 0 )
	{
		Console.Warning( "No doors associated with switch (" + iSwitch.serial.toString() + ") were found. Aborting script execution." );
		return;
	}
	else
	{
		doorSerialList = doorSerials.split( "," );
		if( doorSerialList.length > 0 )
		{
			for( let i = 0; i < doorSerialList.length; i++ )
			{
				let tempDoor = CalcItemFromSer( parseInt( doorSerialList[i] ));
				if( ValidateObject( tempDoor ))
				{
					doorList.push( tempDoor );
				}
			}
		}
	}

	for( let i = 0; i < doorList.length; i++ )
	{
		let doorObj = doorList[i];
		if( !ValidateObject( doorObj ))
			continue;

		let doorEffect = doorObj.GetMoreVar( "more", 1 );
		let origX = doorObj.GetTag( "swOrigX" );
		let origY = doorObj.GetTag( "swOrigY" );
		let origZ = doorObj.GetTag( "swOrigZ" );
		let origType = doorObj.GetTag( "swOrigType" );
		let origID = doorObj.GetTag( "swOrigID" );

		switch( doorEffect )
		{
			case 1: // Re-lock Door
				if( origType != 0 )
				{
					doorObj.type = origType;
				}
				break;
			case 2: // Lower Door to original position
			case 3: // Raise Door to original position
				doorObj.KillJSTimer( 2, switchScriptID );
				doorObj.KillJSTimer( 3, switchScriptID );
				doorObj.KillJSTimer( 4, switchScriptID );
				doorObj.KillJSTimer( 5, switchScriptID );
				doorObj.z = origZ;
				break;
			case 4: // Slide Door East to original position
			case 5: // Slide Door West to original position
				if( origX != 0 )
				{
					doorObj.x = origX;
				}
				break;
			case 6: // Slide Door South to original position
			case 7: // Slide Door North to original position
				if( origY != 0 )
				{
					doorObj.y = origY;
				}
				break;
			case 8: // Show door again
				doorObj.id = origID;
				doorObj.visible = 0;
				doorObj.Refresh();
				break;
			default:
				break;
		}

		// Play optional SFX associated with resetting door
		let doorResetSFX = parseInt( doorObj.GetTag( "doorResetSFX" ));
		if( doorResetSFX != 0 )
		{
			doorObj.SoundEffect( doorResetSFX, true );
		}

		// Play optional VFX associated with resetting door
		let doorResetVFX = parseInt( doorObj.GetTag( "doorResetVFX" ));
		if( doorResetVFX != 0 )
		{
			DoStaticEffect( doorObj.x, doorObj.y, doorObj.z, doorResetVFX, 0x2, 0x2, false );
		}
	}
}

// This function runs one time the first ever time a switch is used
// It will trigger an iteration through all items to find the specific
// doors that should be linked up with this switch. This is done by a
// unique ID stored on both switch and doors, since item-serials are
// not kept in world templates, which are the basis of decorating the game world
function InitializeSwitchDoorCombo( iSwitch, pSock )
{
	// We use pSock to bring iSwitch into onIterate...
	pSock.iSwitch = iSwitch;
	let doorsFound = IterateOver( "ITEM", pSock );
	if( doorsFound > 0 )
	{
		iSwitch.SetTag( "isInitialized", true );
		return true;
	}

	return false;
}

function onIterate( myObject, pSock )
{
	let iSwitch = pSock.iSwitch;
	if( ValidateObject( myObject ) && myObject.isItem )
	{
		if( iSwitch.morez > 0 && iSwitch.morez == myObject.morez && myObject.more > 0 )
		{
			// This item is linked with the switch! Store its serial in iSwitch's list of door serials
			let doorSerials = iSwitch.GetTag( "doorSerials" );
			if( doorSerials == 0 )
			{
				doorSerials = myObject.serial.toString();
			}
			else
			{
				doorSerials += "," + myObject.serial.toString();
			}
			iSwitch.SetTag( "doorSerials", doorSerials );
			return true;
		}
	}

	return false;
}

function _restorecontext_() {}
