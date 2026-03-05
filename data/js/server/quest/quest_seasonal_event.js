/// <reference path="../../../definitions.d.ts" />
// @ts-check

// =========================================================
// Seasonal Event Quests (Event Manager Integration)
// - Registers yearly holiday events with Event Manager (script 5)
// - Spawns seasonal questgiver NPCs when event starts
// - Removes them when event ends
// - Supports GM commands to add/remove custom seasonal events
// =========================================================

const EVENT_MANAGER_SCRIPT_ID = 5;

// Where we store admin-added seasonal events (shared/persistent)
const CUSTOM_EVENTS_FILE = "seasonal_events.json";
const CUSTOM_EVENTS_SECTION = "Seasonal";

// Where we store currently active events (so reloads keep state)
const ACTIVE_EVENTS_FILE = "seasonal_active.json";
const ACTIVE_EVENTS_SECTION = "SeasonalActive";

// Unique prefix for events registered with Event Manager
const EVENT_ID_PREFIX = "seasonal_";

// ---------------------------------------------------------
// Built-in Holidays (month/day/hour/minute + durationHours)
// Notes:
// - Easter is computed (Western/US Easter) per year
// - Everything else is fixed date
// - These are server-local time (same as Date())
// ---------------------------------------------------------
function GetBuiltInHolidayDefinitions()
{
	return [
		{
			key: "new_years",
			name: "New Year’s Celebration",
			type: "FIXED",
			month: 1, day: 1, hour: 0, minute: 5,
			durationHours: 48,
			spawns: [
				// Example spawn. Replace coords/sectionID with your shard’s.
				{ npcSectionID: "dl_newyears_questgiver", x: 1455, y: 1220, z: 0, world: 0, instance: 0 }
			]
		},
		{
			key: "easter",
			name: "Easter Festival",
			type: "EASTER",
			hour: 10, minute: 0,
			durationHours: 96,
			spawns: [
				{ npcSectionID: "dl_easter_questgiver", x: 1458, y: 1222, z: 0, world: 0, instance: 0 }
			]
		},
		{
			key: "july4",
			name: "Founders’ Day (July 4th)",
			type: "FIXED",
			month: 7, day: 4, hour: 12, minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_july4_questgiver", x: 1462, y: 1218, z: 0, world: 0, instance: 0 }
			]
		},
		{
			key: "halloween",
			name: "Halloween (All Hallows)",
			type: "FIXED",
			month: 10, day: 31, hour: 12, minute: 0,
			durationHours: 168, // 7 days
			spawns: [
				{ npcSectionID: "dl_halloween_questgiver", x: 1460, y: 1225, z: 0, world: 0, instance: 0 }
			]
		},
		{
			key: "christmas",
			name: "Christmas (Yuletide)",
			type: "FIXED",
			month: 12, day: 25, hour: 12, minute: 0,
			durationHours: 216, // 9 days
			spawns: [
				{ npcSectionID: "dl_christmas_questgiver", x: 1452, y: 1216, z: 0, world: 0, instance: 0 }
			]
		}
	];
}

// =========================================================
// Script Registration (GM command)
// =========================================================
function CommandRegistration()
{
	// Admin-level recommended. Adjust as needed.
	RegisterCommand( "season", 8, true );
}

// =========================================================
// Startup hook
// =========================================================
function onScriptLoad()
{
	// Register built-ins
	var builtIns = GetBuiltInHolidayDefinitions();
	for( var i = 0; i < builtIns.length; i++ )
	{
		RegisterOrRescheduleHoliday( builtIns[i] );
	}

	// Register custom events from file
	var custom = LoadCustomSeasonalEvents();
	for( var k = 0; k < custom.length; k++ )
	{
		RegisterOrRescheduleHoliday( custom[k] );
	}

	// If events are active (e.g. after reload), re-spawn their NPCs if missing
	RestoreActiveSeasonalSpawns();
}

// =========================================================
// GM Command: [season ...
// =========================================================
/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_SEASON( socket, cmdString )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	cmdString = String( cmdString || "" ).replace(/^\s+|\s+$/g, "");
	var parts = cmdString.length ? cmdString.split(/\s+/) : [];

	if( parts.length < 1 )
	{
		PrintSeasonHelp( socket );
		return;
	}

	var sub = String( parts[0] ).toLowerCase();

	if( sub == "list" )
	{
		ListSeasonalEvents( socket );
		return;
	}

	if( sub == "active" )
	{
		ListActiveSeasonalEvents( socket );
		return;
	}

	// Add a custom event:
	// [season add <key> <month> <day> <hour> <minute> <durationHours> <npcSectionID> <x> <y> <z> <world> <instance>
	// Example:
	// [season add myevent 3 14 18 0 24 dl_custom_questgiver 1450 1220 0 0 0
	if( sub == "add" )
	{
		if( parts.length < 13 )
		{
			socket.SysMessage( "Usage: [season add <key> <month> <day> <hour> <minute> <durationHours> <npcSectionID> <x> <y> <z> <world> <instance>" );
			return;
		}

		var key = String( parts[1] ).toLowerCase();

		var month = parseInt( parts[2], 10 );
		var day = parseInt( parts[3], 10 );
		var hour = parseInt( parts[4], 10 );
		var minute = parseInt( parts[5], 10 );
		var durationHours = parseInt( parts[6], 10 );

		var npcSectionID = String( parts[7] );
		var x = parseInt( parts[8], 10 );
		var y = parseInt( parts[9], 10 );
		var z = parseInt( parts[10], 10 );
		var world = parseInt( parts[11], 10 );
		var instance = parseInt( parts[12], 10 );

		if( isNaN(month)||isNaN(day)||isNaN(hour)||isNaN(minute)||isNaN(durationHours)||isNaN(x)||isNaN(y)||isNaN(z)||isNaN(world)||isNaN(instance) )
		{
			socket.SysMessage( "Invalid numeric values." );
			return;
		}

		var customEvent =
		{
			key: key,
			name: "Custom Seasonal Event: " + key,
			type: "FIXED",
			month: month, day: day,
			hour: hour, minute: minute,
			durationHours: durationHours,
			spawns: [
				{ npcSectionID: npcSectionID, x: x, y: y, z: z, world: world, instance: instance }
			]
		};

		UpsertCustomSeasonalEvent( customEvent );
		RegisterOrRescheduleHoliday( customEvent );

		socket.SysMessage( "Added seasonal event '" + key + "' and registered it with Event Manager." );
		return;
	}

	// Remove a custom event:
	// [season remove <key>
	if( sub == "remove" )
	{
		if( parts.length < 2 )
		{
			socket.SysMessage( "Usage: [season remove <key>" );
			return;
		}

		var removeKey = String( parts[1] ).toLowerCase();
		var ok = RemoveCustomSeasonalEvent( removeKey );

		// Note: Event Manager has its own removal command, but we can also just tell the admin what to do
		if( ok )
		{
			socket.SysMessage( "Removed custom seasonal event '" + removeKey + "' from config file." );
			socket.SysMessage( "If it is already registered, use: [em list then [em remove <index> to remove it from Event Manager." );
		}
		else
		{
			socket.SysMessage( "No custom seasonal event found for key '" + removeKey + "'." );
		}
		return;
	}

	PrintSeasonHelp( socket );
}

function PrintSeasonHelp( socket )
{
	socket.SysMessage( "Seasonal Quests Commands:" );
	socket.SysMessage( "  [season list          - list built-in + custom holiday configs" );
	socket.SysMessage( "  [season active        - show active seasonal events (from shared state)" );
	socket.SysMessage( "  [season add <key> <month> <day> <hour> <minute> <durationHours> <npcSectionID> <x> <y> <z> <world> <instance>" );
	socket.SysMessage( "  [season remove <key>  - remove custom config (does not auto-remove from EM; use [em remove)" );
}

// =========================================================
// Core Registration Logic
// =========================================================
function RegisterOrRescheduleHoliday( holidayDef )
{
	if( !holidayDef || !holidayDef.key )
		return;

	var nowMs = Date.now();

	var nextStartMs = ComputeNextHolidayStartMs( holidayDef, nowMs );
	if( nextStartMs <= 0 )
		return;

	var durationMs = Math.max( 0, ( (holidayDef.durationHours || 24) * 3600 * 1000 ) );

	var uniqueEventID = BuildUniqueEventID( holidayDef.key, nextStartMs );

	TriggerEvent( EVENT_MANAGER_SCRIPT_ID, "RegisterEvent",
		{
			uniqueEventID: uniqueEventID,
			callbackScriptID: SCRIPT.script_id,
			callbackFuncStart: "SeasonalEventStart",
			callbackFuncEnd: "SeasonalEventEnd",
			type: "ONCE",
			date: nextStartMs,
			duration: durationMs,
			name: holidayDef.name || ( "Seasonal: " + holidayDef.key ),
			desc: "Seasonal quest window: " + holidayDef.key
		},
		{
			holidayKey: holidayDef.key,
			holidayName: holidayDef.name || holidayDef.key,
			def: holidayDef
		},
		{
			holidayKey: holidayDef.key,
			holidayName: holidayDef.name || holidayDef.key,
			def: holidayDef
		}
	);
}

function BuildUniqueEventID( holidayKey, startMs )
{
	// Keep ID stable + unique per occurrence
	// e.g. seasonal_halloween_1761931200000
	return EVENT_ID_PREFIX + String( holidayKey ).toLowerCase() + "_" + String( startMs );
}

// =========================================================
// Event Manager Callbacks
// =========================================================
function SeasonalEventStart( args )
{
	if( !args || !args.def )
		return;

	var holidayKey = String( args.holidayKey || "" ).toLowerCase();
	var holidayName = String( args.holidayName || holidayKey );

	MarkSeasonalActive( holidayKey, true );

	BroadcastMessage( "[Seasonal] " + holidayName + " is now active!" );

	SpawnSeasonalQuestGivers( args.def );
}

function SeasonalEventEnd( args )
{
	if( !args || !args.def )
		return;

	var holidayKey = String( args.holidayKey || "" ).toLowerCase();
	var holidayName = String( args.holidayName || holidayKey );

	BroadcastMessage( "[Seasonal] " + holidayName + " has ended." );

	DespawnSeasonalQuestGivers( args.def );
	MarkSeasonalActive( holidayKey, false );

	// Reschedule next year/next occurrence immediately
	RegisterOrRescheduleHoliday( args.def );
}

// =========================================================
// Spawn / Despawn NPCs
// =========================================================
function SpawnSeasonalQuestGivers( holidayDef )
{
	if( !holidayDef || !holidayDef.spawns || !holidayDef.spawns.length )
		return;

	var holidayKey = String( holidayDef.key || "" ).toLowerCase();

	var activeState = LoadActiveSeasonalState();
	if( !activeState.spawns )
		activeState.spawns = {};

	if( !activeState.spawns[holidayKey] )
		activeState.spawns[holidayKey] = [];

	for( var i = 0; i < holidayDef.spawns.length; i++ )
	{
		var s = holidayDef.spawns[i];
		if( !s || !s.npcSectionID )
			continue;

		var spawned = SpawnNPC(
			String( s.npcSectionID ),
			ToIntSafe( s.x ), ToIntSafe( s.y ), ToIntSafe( s.z ),
			ToIntSafe( s.world ),
			ToIntSafe( s.instance )
		);

		if( ValidateObject( spawned ) )
		{
			// Tag for cleanup + identification
			spawned.SetTag( "SeasonalEventKey", holidayKey );
			spawned.SetTag( "SeasonalQuestGiver", 1 );

			activeState.spawns[holidayKey].push( spawned.serial );
		}
	}

	SaveActiveSeasonalState( activeState );
}

function DespawnSeasonalQuestGivers( holidayDef )
{
	if( !holidayDef )
		return;

	var holidayKey = String( holidayDef.key || "" ).toLowerCase();

	var activeState = LoadActiveSeasonalState();
	if( !activeState.spawns || !activeState.spawns[holidayKey] )
		return;

	var serials = activeState.spawns[holidayKey];

	for( var i = 0; i < serials.length; i++ )
	{
		var npc = CalcCharFromSer( ToIntSafe( serials[i] ) );
		if( ValidateObject( npc ) )
		{
			// Safety: only delete if it's ours
			if( npc.GetTag( "SeasonalQuestGiver" ) )
				npc.Delete();
		}
	}

	delete activeState.spawns[holidayKey];
	SaveActiveSeasonalState( activeState );
}

// If script reload happens mid-event, respawn missing NPCs (optional)
function RestoreActiveSeasonalSpawns()
{
	var activeState = LoadActiveSeasonalState();
	if( !activeState || !activeState.active )
		return;

	var builtIns = GetBuiltInHolidayDefinitions();
	var custom = LoadCustomSeasonalEvents();

	var allDefs = builtIns.concat( custom );

	for( var i = 0; i < allDefs.length; i++ )
	{
		var def = allDefs[i];
		if( !def || !def.key )
			continue;

		var holidayKey = String( def.key ).toLowerCase();

		if( activeState.active[holidayKey] )
		{
			// If spawns list missing, respawn
			if( !activeState.spawns || !activeState.spawns[holidayKey] || !activeState.spawns[holidayKey].length )
			{
				SpawnSeasonalQuestGivers( def );
			}
		}
	}
}

// =========================================================
// Active State Storage
// =========================================================
function MarkSeasonalActive( holidayKey, isActive )
{
	var state = LoadActiveSeasonalState();
	if( !state.active )
		state.active = {};

	state.active[String(holidayKey).toLowerCase()] = ( isActive ? 1 : 0 );
	SaveActiveSeasonalState( state );
}

function LoadActiveSeasonalState()
{
	var file = new UOXCFile();
	var out = { active: {}, spawns: {} };

	file.Open( ACTIVE_EVENTS_FILE, "r", ACTIVE_EVENTS_SECTION );
	if( file && file.Length() > 0 )
	{
		var raw = "";
		while( !file.EOF() )
			raw += String( file.ReadUntil( "\n" ) );

		file.Close();
		file.Free();

		out = SafeParseJson( raw ) || out;
		return out;
	}

	if( file ) file.Free();
	return out;
}

function SaveActiveSeasonalState( state )
{
	var file = new UOXCFile();
	file.Open( ACTIVE_EVENTS_FILE, "w", ACTIVE_EVENTS_SECTION );
	if( file )
	{
		file.Write( JSON.stringify( state || {} ) + "\n" );
		file.Close();
		file.Free();
	}
}

// =========================================================
// Custom Event Storage
// =========================================================
function LoadCustomSeasonalEvents()
{
	var file = new UOXCFile();
	var list = [];

	file.Open( CUSTOM_EVENTS_FILE, "r", CUSTOM_EVENTS_SECTION );
	if( file && file.Length() > 0 )
	{
		var raw = "";
		while( !file.EOF() )
			raw += String( file.ReadUntil( "\n" ) );

		file.Close();
		file.Free();

		var parsed = SafeParseJson( raw );
		if( parsed && parsed.events && parsed.events.length )
			return parsed.events;
		return [];
	}

	if( file ) file.Free();
	return [];
}

function UpsertCustomSeasonalEvent( evt )
{
	var data = { events: LoadCustomSeasonalEvents() };

	var key = String( evt.key || "" ).toLowerCase();
	var replaced = false;

	for( var i = 0; i < data.events.length; i++ )
	{
		if( String( data.events[i].key ).toLowerCase() == key )
		{
			data.events[i] = evt;
			replaced = true;
			break;
		}
	}

	if( !replaced )
		data.events.push( evt );

	SaveCustomSeasonalEvents( data );
}

function RemoveCustomSeasonalEvent( key )
{
	var data = { events: LoadCustomSeasonalEvents() };
	key = String( key || "" ).toLowerCase();

	var out = [];
	var removed = false;

	for( var i = 0; i < data.events.length; i++ )
	{
		if( String( data.events[i].key ).toLowerCase() == key )
		{
			removed = true;
			continue;
		}
		out.push( data.events[i] );
	}

	if( removed )
	{
		SaveCustomSeasonalEvents( { events: out } );
	}

	return removed;
}

function SaveCustomSeasonalEvents( data )
{
	var file = new UOXCFile();
	file.Open( CUSTOM_EVENTS_FILE, "w", CUSTOM_EVENTS_SECTION );
	if( file )
	{
		file.Write( JSON.stringify( data || { events: [] } ) + "\n" );
		file.Close();
		file.Free();
	}
}

// =========================================================
// Listing helpers
// =========================================================
function ListSeasonalEvents( socket )
{
	socket.SysMessage( "--- Built-in Holidays ---" );
	var builtIns = GetBuiltInHolidayDefinitions();
	for( var i = 0; i < builtIns.length; i++ )
	{
		var d = builtIns[i];
		socket.SysMessage( d.key + " (" + d.name + ")" );
	}

	socket.SysMessage( "--- Custom Holidays ---" );
	var custom = LoadCustomSeasonalEvents();
	if( !custom.length )
	{
		socket.SysMessage( "(none)" );
		return;
	}

	for( var j = 0; j < custom.length; j++ )
	{
		var c = custom[j];
		socket.SysMessage( c.key + " (" + (c.name || "custom") + ")" );
	}
}

function ListActiveSeasonalEvents( socket )
{
	var state = LoadActiveSeasonalState();
	if( !state.active )
	{
		socket.SysMessage( "(no active-state file)" );
		return;
	}

	socket.SysMessage( "--- Active Seasonal Events ---" );
	var any = false;
	for( var k in state.active )
	{
		if( state.active.hasOwnProperty( k ) && state.active[k] == 1 )
		{
			any = true;
			socket.SysMessage( "ACTIVE: " + k );
		}
	}
	if( !any )
		socket.SysMessage( "(none active)" );
}

// =========================================================
// Date calculations
// =========================================================
function ComputeNextHolidayStartMs( holidayDef, nowMs )
{
	var now = new Date( nowMs );
	var year = now.getFullYear();

	var startDate = null;

	if( holidayDef.type == "EASTER" )
	{
		var easter = ComputeWesternEasterDate( year );
		startDate = new Date( year, easter.monthIndex, easter.day, holidayDef.hour || 0, holidayDef.minute || 0, 0, 0 );
	}
	else
	{
		startDate = new Date(
			year,
			ToIntSafe( holidayDef.month ) - 1,
			ToIntSafe( holidayDef.day ),
			ToIntSafe( holidayDef.hour ),
			ToIntSafe( holidayDef.minute ),
			0, 0
		);
	}

	// If already passed, schedule next year
	if( startDate.getTime() <= nowMs )
	{
		year = year + 1;

		if( holidayDef.type == "EASTER" )
		{
			var easter2 = ComputeWesternEasterDate( year );
			startDate = new Date( year, easter2.monthIndex, easter2.day, holidayDef.hour || 0, holidayDef.minute || 0, 0, 0 );
		}
		else
		{
			startDate = new Date(
				year,
				ToIntSafe( holidayDef.month ) - 1,
				ToIntSafe( holidayDef.day ),
				ToIntSafe( holidayDef.hour ),
				ToIntSafe( holidayDef.minute ),
				0, 0
			);
		}
	}

	return startDate.getTime();
}

// Meeus/Jones/Butcher algorithm (Gregorian Western Easter)
/** @returns {{monthIndex:number, day:number}} */
function ComputeWesternEasterDate( year )
{
	var a = year % 19;
	var b = Math.floor( year / 100 );
	var c = year % 100;
	var d = Math.floor( b / 4 );
	var e = b % 4;
	var f = Math.floor( (b + 8) / 25 );
	var g = Math.floor( (b - f + 1) / 3 );
	var h = (19 * a + b - d - g + 15) % 30;
	var i = Math.floor( c / 4 );
	var k = c % 4;
	var l = (32 + 2 * e + 2 * i - h - k) % 7;
	var m = Math.floor( (a + 11 * h + 22 * l) / 451 );
	var month = Math.floor( (h + l - 7 * m + 114) / 31 ); // 3=March, 4=April
	var day = ((h + l - 7 * m + 114) % 31) + 1;

	return { monthIndex: (month - 1), day: day };
}

// =========================================================
// Utilities
// =========================================================
function ToIntSafe( v )
{
	var n = parseInt( v, 10 );
	return ( isNaN( n ) ? 0 : n );
}

function SafeParseJson( raw )
{
	try
	{
		return JSON.parse( String( raw || "" ) );
	}
	catch( e )
	{
		return null;
	}
}