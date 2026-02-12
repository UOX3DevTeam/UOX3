/// <reference path="../definitions.d.ts" />
// @ts-check

// UOX3 Event Manager
// For registering and managing events that need to trigger on a schedule
// Example: Holiday celebrations, invasion systems, champion spawns, etc

// Configuration
const eventManagerEnabled = GetServerSetting( "EventManagerSystem" );
const eventDataFolder	= "events";
const eventDataFile		= "event_list.json";
const eventManagerDef	= "eventmanagerobj";
const eventManagerTimer	= 1000;

// Global variables
/** @type { Item | null } */
var gEventManagerObj	= null;
var gEventManagerSerial	= 0;

/** @type { any[] } */
var gEventList			= [];

// Loads on server startup after everything else has loaded
// and after reload of this/all JS scripts
/** @type { ( ) => void } */
function onScriptLoad()
{
	if( eventManagerEnabled )
	{
		InitEventManager();
	}
}

// Check for Event Manager's heartbeat
function CheckForHeartbeat()
{
	if( !ValidateObject( gEventManagerObj ))
		return false;

	return ( gEventManagerObj.GetJSTimer( 0, SCRIPT.script_id ) > 0 );
}

// Wake up the Event Manager
function StartHeartbeat()
{
	if( ValidateObject( gEventManagerObj ))
	{
		// Only start if not already running
		if( !CheckForHeartbeat() )
		{
			gEventManagerObj.StartTimer( eventManagerTimer, 0, SCRIPT.script_id );
			Console.Print( GetDictionaryEntry( 6500 ) + "\n" ); // Event Manager: Heartbeat started
		}
	}
}

// One-time Initialization of Event Manager
function InitEventManager()
{
	let oldEventManager = FindItemBySection( 1000, 1000, -50, 0, eventManagerDef );
	if( ValidateObject( oldEventManager ))
	{
		// We found an existing event manager object in the world, use it!
		gEventManagerObj = oldEventManager;
		gEventManagerSerial = oldEventManager.serial;
		if( UOX_DEBUG_MODE )
		{
			Console.Print( GetDictionaryEntry( 6501 ) + "\n" ); // Event Manager: Reusing existing Event Manager object.
		}
	}
	else
	{
		// No event manager object found, let's create a new one!
		let newObj = CreateDFNItem( null, null, eventManagerDef, 1, "ITEM", false );
		if( ValidateObject( newObj ))
		{
			// Let's stuff it somewhere safe, away from players' eyes
			// It will get hidden and non-decayable/wipeable via the DFN tags
			newObj.Teleport( 1000, 1000, -50 );

			// Keep reference to it
			gEventManagerObj = newObj;
			gEventManagerSerial = newObj.serial;

			if( UOX_DEBUG_MODE )
			{
				Console.Print( GetDictionaryEntry( 6502 ) + "\n" ); // Event Manager: Old Event Manager object no longer exists. A new one will be created.
			}
		}
		else
		{
			let consoleMsg = GetDictionaryEntry( 6503 ); // Event Manager: Failed to spawn %s
			Console.Error( consoleMsg.replace( /%s/gi, eventManagerDef ) + "\n" );
			return;
		}
	}

	// Load all existing events from .json file
	LoadEvents();

	// Kick off the heartbeat timer if any events were loaded
	if( gEventList.length > 0 )
	{
		StartHeartbeat();
		let consoleMsg = GetDictionaryEntry( 6504 ); // Event Manager: Initialization complete. Events loaded: %i
		Console.Print( consoleMsg.replace( /%i/gi, gEventList.length ) + "\n" );
	}
	else
	{
		Console.Print( GetDictionaryEntry( 6505 ) + "\n" ); // Event Manager: Initialization complete. No events loaded, entering standby mode.
	}
}

// Event Manager "heartbeat"
/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( eventManager, timerID )
{
	if( timerID !== 0 )
		return;

	if( !ValidateObject( gEventManagerObj ) || eventManager.serial != gEventManagerSerial )
		return;

	let now = new Date().getTime();
	let modified = false;
	let toRemove = [];

	// Iterate through all tracked events, check time, etc.
	for( let i = 0; i < gEventList.length; i++ )
	{
		let event = gEventList[i];

		// If event is not enabled, ignore it
		if( event.enabled === false )
			continue;

		// Check if event is due to run
		if( event.nextRun <= now )
		{
			if( event.isRunning )
			{
				// Event is already running, this means it's time to stop it
				event.isRunning = false;

				if( event.callbackFuncEnd != "" )
				{
					// Call the event's registered END function
					ExecuteCallback( event.callbackScriptID, event.callbackFuncEnd, event.argsEnd );
				}

				if( event.type == "ONCE" || ( event.maxTriggerCount > 0 && event.triggerCount >= event.maxTriggerCount ))
				{
					// Event is one-time only, or max trigger count has been reached, let's remove it
					toRemove.push( i );
				}
				else
				{
					// Repeating event. Calculate next start time!
					CalculateNextRun( event );
				}
				modified = true;
			}
			else
			{
				// Event is not already running. Does it meet the conditions to run?
				if( CheckConditions( event ))
				{
					// Conditions met, time to run the event!
					ExecuteCallback( event.callbackScriptID, event.callbackFuncStart, event.argsStart );
					event.triggerCount++;

					// Check duration of event
					if( event.duration > 0 )
					{
						// Switch to "running" state
						event.isRunning = true;
						event.nextRun = now + event.duration;
						modified = true;
						continue;
					}

					// If event has exceeded max trigger count, let's remove it
					if( event.maxTriggerCount > 0 && event.triggerCount >= event.maxTriggerCount )
					{
						toRemove.push( i );
						modified = true;
						continue;
					}
				}

				// Check if event is one-time only
				if( event.type == "ONCE" )
				{
					// Remove event
					toRemove.push( i );
					modified = true;
				}
				else
				{
					// Schedule next run of event
					CalculateNextRun( event );
					modified = true;
				}
			}

		}
	}

	// Cleanup of tracked events
	for( let j = toRemove.length - 1; j >= 0; j-- )
	{
		gEventList.splice( toRemove[j], 1 );
	}

	if( modified )
	{
		SaveEvents();
	}

	// Keep heartbeat going, if there are still events left
	if( gEventList.length > 0 )
	{
		eventManager.StartTimer( eventManagerTimer, 0, SCRIPT.script_id );
	}
	else
	{
		Console.Print( GetDictionaryEntry( 6506 ) + "\n" ); // Event Manager: All events finished. Entering standby mode.
	}
}

// Trigger the event via callback function
/** @type { ( callbackScriptID: number, callbackFunc: any, argsStart: any[]  ) => void } */
function ExecuteCallback( callbackScriptID, callbackFunc, argsStart )
{
	// Make sure callback function exists in callback script
	if( !DoesEventExist( callbackScriptID, callbackFunc ))
	{
		let consoleMsg = GetDictionaryEntry( 6507 ); // Event Manager: WARNING - Skipped event, callback function %s missing in script %i
		consoleMsg = consoleMsg.replace( /%s/gi, callbackFunc );
		consoleMsg = consoleMsg.replace( /%i/gi, callbackScriptID );
		Console.Warning( consoleMsg );
		return;
	}

	TriggerEvent( callbackScriptID, callbackFunc, argsStart );
}

// Validate conditions for event to run
/** @type { ( event: any  ) => boolean } */
function CheckConditions( event )
{
	// Does event only trigger on specific days of the week?
	if( event.daysOfWeek && event.daysOfWeek.length > 0 )
	{
		// Is today one of those days?
		let today = new Date().getDay(); // 0 = Sunday ... 6 = Saturday
		for( let i = 0; i < event.daysOfWeek.length; i++ )
		{
			if( event.daysOfWeek[i] == today )
			{
				// Today's the day!
				return true;
			}
		}

		// Today's not the day. Try again next day
		return false;
	}

	// No specific conditions are needed for the event to run
	return true;
}

// Calculate when event should run next (for ones that run multiple times)
/** @type { ( event: any  ) => void } */
function CalculateNextRun( event )
{
	let now = new Date().getTime();

	if( event.type == "INTERVAL" ) // Fixed interval
	{
		event.nextRun = now + ( event.interval || 60000 );
	}
	else if( event.type == "RECURRING" ) // Recurring event on specific day(s) of the week
	{
		let nextDate = new Date();
		nextDate.setHours( event.hour || 0, event.minute || 0, 0, 0 );

		// If the time for the event has already passed for today, start checking from tomorrow
		if( nextDate.getTime() <= now )
		{
			nextDate.setDate( nextDate.getDate() + 1 );
		}

		// If set to run on specific days, keep adding days until we find a match
		if( event.daysOfWeek && event.daysOfWeek.length > 0 )
		{
			let i = 0;

			// While the current day (0-6) is NOT in our allowed list...
			while( event.daysOfWeek.indexOf( nextDate.getDay() ) == -1 && i < 8 )
			{
				nextDate.setDate( nextDate.getDate() + 1 );
				i++;
			}
		}

		event.nextRun = nextDate.getTime();
	}
}

// Commands triggered via commands/em.js
// Example usage: 'em list
/** @type { ( socket: Socket, command: string ) => boolean } */
function EventManagerCommands( socket, command )
{
	if( socket == null )
		return false;

	if( !eventManagerEnabled )
	{
		socket.SysMessage( GetDictionaryEntry( 6508 )); // Event Manager is currently disabled. Enable it via uox.ini
		return false;
	}

	let args = command.split( " " );
	let subCmd = args[0].toLowerCase();
	let param = ( args.length > 1 ) ? args[1] : null;

	switch( subCmd )
	{
		case "list":
		{
			// List out all events (should be a gump, ideally)
			let sysMsg = GetDictionaryEntry( 6509, socket.language ); // Active Events: %i
			socket.SysMessage( sysMsg.replace( /%i/gi, gEventList.length ));
			for( let i = 0; i < gEventList.length; i++ )
			{
				let event = gEventList[i];
				let timeLeft = Math.floor(( event.nextRun - new Date().getTime()) / 1000 );

				let statusStr = ( event.enabled ? "[ENABLED]" : "[DISABLED]" );
				socket.SysMessage( statusStr + "[" + i + "]" + event.name + " (Script: " + event.callbackScriptID + ", " + event.callbackFuncStart + ") in " + timeLeft + "s" );
			}
			return true;
		}
		case "enable":
		case "disable":
		{
			if( param == null || isNaN( parseInt( param )))
			{
				let sysMsg = GetDictionaryEntry( 6510, socket.language ); // Usage: 'em %s <index number from event list>
				socket.SysMessage( sysMsg.replace( /%s/gi, subCmd ));
			}
			else
			{
				let index = parseInt( param );
				if( index >= 0 && index < gEventList.length )
				{
					let newState = ( subCmd == "enable" );
					let event = gEventList[index];
					event.enabled = newState;

					// Reschedule event to next valid window if it missed previous cycle
					if( newState === true && event.nextRun < new Date().getTime() )
					{
						if( !event.isRunning )
						{
							socket.SysMessage( GetDictionaryEntry( 6520, socket.language )); // Event reenabled and rescheduled for next valid activation window.
							CalculateNextRun( event );
						}
						else
						{
							socket.SysMessage( "Event reenabled and resumed." );
						}
					}
					SaveEvents();

					let sysMsg = GetDictionaryEntry( 6511, socket.language ); // Event %i is now %s
					sysMsg = sysMsg.replace( /%i/gi, index );
					sysMsg = sysMsg.replace( /%s/gi, ( newState ? "ENABLED" : "DISABLED" ));
					socket.SysMessage( sysMsg );

					let consoleMsg = GetDictionaryEntry( 6512 ); // Event Manager: %s changed event %i to %s
					consoleMsg = consoleMsg.replace( /%s/gi, socket.currentChar.name );
					consoleMsg = consoleMsg.replace( /%i/gi, index );
					consoleMsg = consoleMsg.replace( /%s/gi, newState );
					Console.Print( consoleMsg + "\n" );
					return true;
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 6513, socket.language )); // Invalid event index. To see full event list: 'em list
				}
			}
			break;
		}
		case "remove":
		{
			if( param == null || isNaN( parseInt( param )))
			{
				socket.SysMessage( GetDictionaryEntry( 6521, socket.language )); // Usage: 'em remove <index from event list>
			}
			else
			{
				let index = parseInt( param );
				if( index >= 0 && index < gEventList.length )
				{
					let event = gEventList[index];
					if( event.isRunning )
					{
						if( event.callbackFuncEnd != "" )
						{
							socket.SysMessage( GetDictionaryEntry( 6522, socket.language )); // Event halted and removed.
							ExecuteCallback( event.callbackScriptID, event.callbackFuncEnd, event.argsEnd );
						}
					}

					// Log removal
					let consoleMsg = "Event Manager: %s removed event %i (%s)";
					consoleMsg = consoleMsg.replace( /%s/gi, socket.currentChar.name );
					consoleMsg = consoleMsg.replace( /%i/gi, index );
					consoleMsg = consoleMsg.replace( /%s/gi, event.name );
					Console.Print( consoleMsg + "\n" );

					// Remove event from list and save
					gEventList.splice( index, 1 );
					SaveEvents();
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 6513, socket.language )); // Invalid event index. To see full event list: 'em list
				}
			}
			return true;
		}
		case "reset":
		{
			socket.SysMessage( GetDictionaryEntry( 6514, socket.language )); // Forcing reset of Event Manager...
			Console.Warning( GetDictionaryEntry( 6514, socket.language )); // Forcing reset of Event Manager...
			InitEventManager();
			return true;
		}
		default:
			socket.SysMessage( GetDictionaryEntry( 6515, socket.language )); // Supported Event Manager commands: list, enable <#>, disable <#>, remove <#>, reset
			break;
	}
	return false;
}

// Register a new Event
// Usage from other scripts: TriggerEvent( [ManagerID], "RegisterEvent", { callbackScriptID: script_id, callbackFuncStart: "MyFunc", type: "INTERVAL", ... }, { argsStart }, { argsEnd } )
/** @type { ( config: any, argsStart: any, argsEnd: any  ) => null | string } */
function RegisterEvent( config, argsStart, argsEnd )
{
	if( !eventManagerEnabled )
	{
		Console.Warning( GetDictionaryEntry( 6516 )); // Event Manager: RegisterEvent failed because Event Manager is disabled.
		return null;
	}

	if( !config.callbackScriptID || !config.callbackFuncStart || !config.uniqueEventID )
	{
		Console.Error( GetDictionaryEntry( 6517 )); // Event Manager: RegisterEvent failed due to missing config data.
		return null;
	}

	// Check for duplicate events, based on uniqueEventID provided with registration
	for( let i = 0; i < gEventList.length; i++ )
	{
		let eventToCheck = gEventList[i];
		if( eventToCheck.callbackScriptID == config.callbackScriptID && eventToCheck.uniqueEventID == config.uniqueEventID )
		{
			// Event already exists in list, don't double-register, just return internal id of existing event:
			return eventToCheck.id;
		}
	}

	let newEvent = {
		id: "event_" + ( new Date().getTime() ) + "_" + Math.floor( Math.random() * 1000 ),

		// Used for GM event management purposes
		uniqueEventID: config.uniqueEventID,
		name: config.name || config.callbackFuncStart,
		desc: config.desc || "",
		category: config.category || "",

		// Callback options for start/end of event
		callbackScriptID: config.callbackScriptID,
		callbackFuncStart: config.callbackFuncStart,
		callbackFuncEnd: config.callbackFuncEnd || "",
		argsStart: argsStart || {},
		argsEnd: argsEnd || {},

		// Event type and duration
		type: config.type || "ONCE",
		duration: config.duration || 0,

		// Misc
		enabled: true,
		isRunning: false,
		triggerCount: 0,
		maxTriggerCount: config.maxTriggerCount || -1,

		// Event scheduling configuration
		daysOfWeek: config.days || [],
		interval: config.interval || 0,
		hour: config.hour || 0,
		minute: config.minute || 0,

		created: new Date().getTime()
	}

	// Initial calculation for next run
	if( newEvent.type == "ONCE" )
	{
		// Run once at a very specific date/time
		newEvent.nextRun = config.date || ( new Date().getTime() + 1000 );
	}
	else
	{
		// Run based on fixed interval ("INTERVAL") or specific day(s) of the week ("RECURRING")
		CalculateNextRun( newEvent );
	}

	gEventList.push( newEvent );
	SaveEvents();

	let consoleMsg = GetDictionaryEntry( 6518 ); // Event Manager: New Event registered - %s (ID: %i)
	consoleMsg = consoleMsg.replace( /%s/gi, newEvent.callbackFuncStart );
	consoleMsg = consoleMsg.replace( /is/gi, newEvent.id );
	Console.Print( consoleMsg + "\n" );

	// Wake up Event Manager (in case it's in standby)
	StartHeartbeat();

	return newEvent.id;
}

// Load events from .json file
function LoadEvents()
{
	gEventList = [];

	var mFile = new UOXCFile();
	mFile.Open( eventDataFile, "r", eventDataFolder );
	if( mFile && mFile.Length() >= 0 )
	{
		let jsonStr = "";

		// Read until end of file
		while( !mFile.EOF() )
		{
			let line = mFile.ReadUntil( "\n" );

			line = line.trim();
			if( !line || line.length < 1 || line == "" )
				continue;

			jsonStr += line;
		}

		// Remove trailing EOF/Garbage character (255) if present
		if( jsonStr.length > 0 && jsonStr.charCodeAt(jsonStr.length - 1) === 255 )
		{
		    jsonStr = jsonStr.slice( 0, -1 );
		}

		if( jsonStr.length > 1 )
		{
			gEventList = JSON.parse( jsonStr );
		}

		mFile.Close();
		mFile.Free();
	}
}

// Save all events to a .json file
function SaveEvents()
{
	let mFile = new UOXCFile();
	mFile.Open( eventDataFile, "w", eventDataFolder );
	if( mFile != null )
	{
		if( gEventList.length > 0 )
		{
			mFile.Write( JSON.stringify( gEventList, null, 4 ));
		}

		mFile.Close();
		mFile.Free();
	}
}
