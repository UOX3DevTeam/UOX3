// This script can be used for long term JS timers (max 49 days!) for objects
// Script will be automatically removed from object when no long term timers are active
const scriptID = 3;

function StartLongTermTimer( timerObj, timerID, duration )
{
	if( !ValidateObject( timerObj ))
		return;

	timerObj.StartTimer( duration, timerID, true );
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	switch( timerID )
	{
		case 5000: // Tribal Paint effect
		{
			RevertTribalPaintEffect( timerObj );
			break;
		}
		default:
			break;
	}

	// Run a check to see if there are still active long term timers
	CheckTimers( timerObj );
}

function CheckTimers( timerObj )
{
	// Check if any long term timers mentioned in onTimer are still active on object
	if( timerObj.GetJSTimer( 5000, scriptID ) != 0 )
	{
		return;
	}

	// If none are active, remove script from object
	timerObj.RemoveScriptTrigger( scriptID );
}

function RevertTribalPaintEffect( timerObj )
{
	// Revert the effects of tribal paint
	if( timerObj.id == 0x00b7 || timerObj.id == 0x00b8 )
	{
		timerObj.id = timerObj.orgID;
		timerObj.colour = timerObj.orgSkin;
		var socket = timerObj.socket;
		if( socket )
		{
			timerObj.SysMessage( GetDictionaryEntry( 1676, socket.language )); // Your body paint has worn off!
		}
	}
}