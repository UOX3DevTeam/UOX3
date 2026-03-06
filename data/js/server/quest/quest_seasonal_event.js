/// <reference path="../../../definitions.d.ts" />
// @ts-check

// =========================================================
// Seasonal Event Quests (Full Replacement)
// =========================================================
// - Registers yearly holiday events with Event Manager (script 5)
// - Uses holiday packs so shard owners can enable only the cultures/themes they want
// - Supports built-in holidays + custom holidays
// - Spawns seasonal questgiver NPCs when an event starts
// - Removes them when the event ends
// - Supports GM commands to list/enable/disable packs and add/remove custom events
//
// Recommended folder:
//   js/server/quests/seasonal/seasonal_event_quests.js
//
// Recommended uox.ini setting:
//   EVENTMANAGERSYSTEM=1
//
// Notes:
// - Built-in holidays are grouped into packs such as western, east_asian, islamic, etc.
// - Enabled packs are stored in a shared JSON file so admins do not have to edit script code
// - Custom events are independent and always loaded unless removed
// - Built-in movable holidays like Easter are computed automatically
// - Other movable holidays such as Ramadan, Eid, Diwali, Hanukkah, Lunar New Year, etc.
//   are supported through optional per-year override tables in this script
// =========================================================

const EVENT_MANAGER_SCRIPT_ID = 5;

// Shared files
const SEASONAL_CONFIG_FILE = "seasonal_config.json";
const SEASONAL_CONFIG_SECTION = "Seasonal";

const SEASONAL_CUSTOM_EVENTS_FILE = "seasonal_custom_events.json";
const SEASONAL_CUSTOM_EVENTS_SECTION = "SeasonalCustom";

const SEASONAL_ACTIVE_FILE = "seasonal_active.json";
const SEASONAL_ACTIVE_SECTION = "SeasonalActive";

// Event Manager unique ID prefix
const SEASONAL_EVENT_PREFIX = "seasonal_";

// ---------------------------------------------------------
// Script Registration
// ---------------------------------------------------------
function CommandRegistration()
{
	RegisterCommand( "season", 8, true );
}

// ---------------------------------------------------------
// Startup
// ---------------------------------------------------------
function onScriptLoad()
{
	RegisterConfiguredSeasonalEvents();
	RestoreActiveSeasonalSpawns();
}

// =========================================================
// CONFIG
// =========================================================

function GetDefaultSeasonalConfig()
{
	return {
		enabledPacks: [
			"western",
			"shard_fun"
		]
	};
}

/** @type { () => any } */
function LoadSeasonalConfig()
{
	var file = new UOXCFile();
	var defaultConfig = GetDefaultSeasonalConfig();

	file.Open( SEASONAL_CONFIG_FILE, "r", SEASONAL_CONFIG_SECTION );
	if( file && file.Length() > 0 )
	{
		var rawText = "";
		while( !file.EOF() )
			rawText += String( file.ReadUntil( "\n" ) );

		file.Close();
		file.Free();

		var parsedConfig = SafeParseJson( rawText );
		if( parsedConfig )
		{
			if( !parsedConfig.enabledPacks || !IsArray( parsedConfig.enabledPacks ) )
				parsedConfig.enabledPacks = defaultConfig.enabledPacks.slice( 0 );

			return parsedConfig;
		}
	}

	if( file )
		file.Free();

	SaveSeasonalConfig( defaultConfig );
	return defaultConfig;
}

/** @type { ( config: any ) => void } */
function SaveSeasonalConfig( config )
{
	var file = new UOXCFile();
	file.Open( SEASONAL_CONFIG_FILE, "w", SEASONAL_CONFIG_SECTION );
	if( file )
	{
		file.Write( JSON.stringify( config || GetDefaultSeasonalConfig() ) + "\n" );
		file.Close();
		file.Free();
	}
}

// =========================================================
// BUILT-IN HOLIDAY CATALOG
// =========================================================

/** @type { () => any } */
function GetBuiltInHolidayCatalog()
{
	return {
		new_years_day: {
			key: "new_years_day",
			name: "New Year’s Day",
			type: "FIXED",
			month: 1,
			day: 1,
			hour: 0,
			minute: 5,
			durationHours: 48,
			spawns: [
				{ npcSectionID: "dl_newyears_questgiver", x: 1455, y: 1220, z: 0, world: 0, instance: 0 }
			]
		},

		lunar_new_year: {
			key: "lunar_new_year",
			name: "Lunar New Year",
			type: "LUNAR_NEWYEAR",
			hour: 12,
			minute: 0,
			durationHours: 120,
			spawns: [
				{ npcSectionID: "dl_lunarnewyear_questgiver", x: 1456, y: 1221, z: 0, world: 0, instance: 0 }
			]
		},

		valentines_day: {
			key: "valentines_day",
			name: "Valentine’s Day",
			type: "FIXED",
			month: 2,
			day: 14,
			hour: 12,
			minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_valentines_questgiver", x: 1457, y: 1222, z: 0, world: 0, instance: 0 }
			]
		},

		st_patricks_day: {
			key: "st_patricks_day",
			name: "St. Patrick’s Day",
			type: "FIXED",
			month: 3,
			day: 17,
			hour: 12,
			minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_stpatricks_questgiver", x: 1458, y: 1222, z: 0, world: 0, instance: 0 }
			]
		},

		holi: {
			key: "holi",
			name: "Holi",
			type: "HOLI",
			hour: 12,
			minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_holi_questgiver", x: 1458, y: 1223, z: 0, world: 0, instance: 0 }
			]
		},

		easter: {
			key: "easter",
			name: "Easter Festival",
			type: "EASTER",
			hour: 10,
			minute: 0,
			durationHours: 96,
			spawns: [
				{ npcSectionID: "dl_easter_questgiver", x: 1458, y: 1224, z: 0, world: 0, instance: 0 }
			]
		},

		ramadan: {
			key: "ramadan",
			name: "Ramadan",
			type: "RAMADAN",
			hour: 12,
			minute: 0,
			durationHours: 240,
			spawns: [
				{ npcSectionID: "dl_ramadan_questgiver", x: 1459, y: 1224, z: 0, world: 0, instance: 0 }
			]
		},

		eid_al_fitr: {
			key: "eid_al_fitr",
			name: "Eid al-Fitr",
			type: "EID_AL_FITR",
			hour: 12,
			minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_eidfitr_questgiver", x: 1460, y: 1224, z: 0, world: 0, instance: 0 }
			]
		},

		vesak: {
			key: "vesak",
			name: "Vesak",
			type: "VESAK",
			hour: 12,
			minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_vesak_questgiver", x: 1461, y: 1224, z: 0, world: 0, instance: 0 }
			]
		},

		beltane: {
			key: "beltane",
			name: "Beltane / May Day",
			type: "FIXED",
			month: 5,
			day: 1,
			hour: 12,
			minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_beltane_questgiver", x: 1459, y: 1223, z: 0, world: 0, instance: 0 }
			]
		},

		dragon_boat_festival: {
			key: "dragon_boat_festival",
			name: "Dragon Boat Festival",
			type: "DRAGON_BOAT",
			hour: 12,
			minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_dragonboat_questgiver", x: 1460, y: 1223, z: 0, world: 0, instance: 0 }
			]
		},

		midsummer: {
			key: "midsummer",
			name: "Midsummer Festival",
			type: "FIXED",
			month: 6,
			day: 21,
			hour: 12,
			minute: 0,
			durationHours: 96,
			spawns: [
				{ npcSectionID: "dl_midsummer_questgiver", x: 1460, y: 1224, z: 0, world: 0, instance: 0 }
			]
		},

		july4: {
			key: "july4",
			name: "Founders’ Day / July 4th",
			type: "FIXED",
			month: 7,
			day: 4,
			hour: 12,
			minute: 0,
			durationHours: 72,
			spawns: [
				{ npcSectionID: "dl_july4_questgiver", x: 1462, y: 1218, z: 0, world: 0, instance: 0 }
			]
		},

		obon: {
			key: "obon",
			name: "Obon Festival",
			type: "FIXED",
			month: 8,
			day: 15,
			hour: 12,
			minute: 0,
			durationHours: 96,
			spawns: [
				{ npcSectionID: "dl_obon_questgiver", x: 1461, y: 1220, z: 0, world: 0, instance: 0 }
			]
		},

		mid_autumn_festival: {
			key: "mid_autumn_festival",
			name: "Mid-Autumn Festival",
			type: "MID_AUTUMN",
			hour: 12,
			minute: 0,
			durationHours: 96,
			spawns: [
				{ npcSectionID: "dl_midautumn_questgiver", x: 1462, y: 1220, z: 0, world: 0, instance: 0 }
			]
		},

		pirates_day: {
			key: "pirates_day",
			name: "Talk Like a Pirate Day",
			type: "FIXED",
			month: 9,
			day: 19,
			hour: 12,
			minute: 0,
			durationHours: 48,
			spawns: [
				{ npcSectionID: "dl_piratesday_questgiver", x: 1463, y: 1220, z: 0, world: 0, instance: 0 }
			]
		},

		harvest_festival: {
			key: "harvest_festival",
			name: "Harvest Festival",
			type: "FIXED",
			month: 9,
			day: 22,
			hour: 12,
			minute: 0,
			durationHours: 96,
			spawns: [
				{ npcSectionID: "dl_harvest_questgiver", x: 1464, y: 1220, z: 0, world: 0, instance: 0 }
			]
		},

		halloween: {
			key: "halloween",
			name: "Halloween (All Hallows)",
			type: "FIXED",
			month: 10,
			day: 31,
			hour: 12,
			minute: 0,
			durationHours: 168,
			spawns: [
				{ npcSectionID: "dl_halloween_questgiver", x: 1460, y: 1225, z: 0, world: 0, instance: 0 }
			]
		},

		day_of_the_dead: {
			key: "day_of_the_dead",
			name: "Día de los Muertos",
			type: "FIXED",
			month: 11,
			day: 2,
			hour: 12,
			minute: 0,
			durationHours: 96,
			spawns: [
				{ npcSectionID: "dl_dayofthedead_questgiver", x: 1461, y: 1226, z: 0, world: 0, instance: 0 }
			]
		},

		bonfire_night: {
			key: "bonfire_night",
			name: "Bonfire Night",
			type: "FIXED",
			month: 11,
			day: 5,
			hour: 12,
			minute: 0,
			durationHours: 48,
			spawns: [
				{ npcSectionID: "dl_bonfire_questgiver", x: 1462, y: 1226, z: 0, world: 0, instance: 0 }
			]
		},

		diwali: {
			key: "diwali",
			name: "Diwali",
			type: "DIWALI",
			hour: 12,
			minute: 0,
			durationHours: 120,
			spawns: [
				{ npcSectionID: "dl_diwali_questgiver", x: 1453, y: 1219, z: 0, world: 0, instance: 0 }
			]
		},

		hanukkah: {
			key: "hanukkah",
			name: "Hanukkah",
			type: "HANUKKAH",
			hour: 12,
			minute: 0,
			durationHours: 192,
			spawns: [
				{ npcSectionID: "dl_hanukkah_questgiver", x: 1454, y: 1217, z: 0, world: 0, instance: 0 }
			]
		},

		winter_solstice: {
			key: "winter_solstice",
			name: "Winter Solstice",
			type: "FIXED",
			month: 12,
			day: 21,
			hour: 12,
			minute: 0,
			durationHours: 120,
			spawns: [
				{ npcSectionID: "dl_wintersolstice_questgiver", x: 1455, y: 1217, z: 0, world: 0, instance: 0 }
			]
		},

		christmas: {
			key: "christmas",
			name: "Christmas / Yuletide",
			type: "FIXED",
			month: 12,
			day: 25,
			hour: 12,
			minute: 0,
			durationHours: 216,
			spawns: [
				{ npcSectionID: "dl_christmas_questgiver", x: 1452, y: 1216, z: 0, world: 0, instance: 0 }
			]
		},

		new_years_eve: {
			key: "new_years_eve",
			name: "New Year’s Eve",
			type: "FIXED",
			month: 12,
			day: 31,
			hour: 18,
			minute: 0,
			durationHours: 18,
			spawns: [
				{ npcSectionID: "dl_newyearseve_questgiver", x: 1458, y: 1222, z: 0, world: 0, instance: 0 }
			]
		}
	};
}

// =========================================================
// BUILT-IN HOLIDAY PACKS
// =========================================================

/** @type { () => any } */
function GetBuiltInHolidayPacks()
{
	return {
		western: [
			"new_years_day",
			"valentines_day",
			"easter",
			"halloween",
			"christmas",
			"new_years_eve"
		],

		celtic_pagan: [
			"beltane",
			"midsummer",
			"winter_solstice"
		],

		east_asian: [
			"lunar_new_year",
			"dragon_boat_festival",
			"obon",
			"mid_autumn_festival"
		],

		south_asian: [
			"holi",
			"diwali",
			"vesak"
		],

		jewish: [
			"hanukkah"
		],

		islamic: [
			"ramadan",
			"eid_al_fitr"
		],

		latin_american: [
			"day_of_the_dead"
		],

		irish: [
			"st_patricks_day"
		],

		american: [
			"july4",
			"bonfire_night"
		],

		shard_fun: [
			"july4",
			"pirates_day",
			"harvest_festival"
		],

		all: [
			"new_years_day",
			"lunar_new_year",
			"valentines_day",
			"st_patricks_day",
			"holi",
			"easter",
			"ramadan",
			"eid_al_fitr",
			"vesak",
			"beltane",
			"dragon_boat_festival",
			"midsummer",
			"july4",
			"obon",
			"mid_autumn_festival",
			"pirates_day",
			"harvest_festival",
			"halloween",
			"day_of_the_dead",
			"bonfire_night",
			"diwali",
			"hanukkah",
			"winter_solstice",
			"christmas",
			"new_years_eve"
		]
	};
}

/** @type { () => any[] } */
function GetEnabledBuiltInHolidayDefinitions()
{
	var seasonalConfig = LoadSeasonalConfig();
	var enabledPackNames = seasonalConfig.enabledPacks || [];
	var holidayCatalog = GetBuiltInHolidayCatalog();
	var holidayPacks = GetBuiltInHolidayPacks();

	var seenHolidayKeys = {};
	var enabledHolidayDefinitions = [];

	for( var packIndex = 0; packIndex < enabledPackNames.length; packIndex++ )
	{
		var packName = String( enabledPackNames[packIndex] || "" ).toLowerCase();
		var holidayKeyList = holidayPacks[packName];

		if( !holidayKeyList || !holidayKeyList.length )
			continue;

		for( var holidayIndex = 0; holidayIndex < holidayKeyList.length; holidayIndex++ )
		{
			var holidayKey = String( holidayKeyList[holidayIndex] || "" ).toLowerCase();

			if( seenHolidayKeys[holidayKey] )
				continue;

			if( holidayCatalog[holidayKey] )
			{
				seenHolidayKeys[holidayKey] = true;
				enabledHolidayDefinitions.push( holidayCatalog[holidayKey] );
			}
		}
	}

	return enabledHolidayDefinitions;
}

// =========================================================
// CUSTOM EVENT STORAGE
// =========================================================

/** @type { () => any[] } */
function LoadCustomSeasonalEvents()
{
	var file = new UOXCFile();
	var customEvents = [];

	file.Open( SEASONAL_CUSTOM_EVENTS_FILE, "r", SEASONAL_CUSTOM_EVENTS_SECTION );
	if( file && file.Length() > 0 )
	{
		var rawText = "";
		while( !file.EOF() )
			rawText += String( file.ReadUntil( "\n" ) );

		file.Close();
		file.Free();

		var parsedData = SafeParseJson( rawText );
		if( parsedData && parsedData.events && IsArray( parsedData.events ) )
			return parsedData.events;
	}

	if( file )
		file.Free();

	return customEvents;
}

/** @type { ( data: any ) => void } */
function SaveCustomSeasonalEvents( data )
{
	var file = new UOXCFile();
	file.Open( SEASONAL_CUSTOM_EVENTS_FILE, "w", SEASONAL_CUSTOM_EVENTS_SECTION );
	if( file )
	{
		file.Write( JSON.stringify( data || { events: [] } ) + "\n" );
		file.Close();
		file.Free();
	}
}

/** @type { ( eventDefinition: any ) => void } */
function UpsertCustomSeasonalEvent( eventDefinition )
{
	var data = { events: LoadCustomSeasonalEvents() };
	var targetKey = String( eventDefinition.key || "" ).toLowerCase();
	var foundExisting = false;

	for( var eventIndex = 0; eventIndex < data.events.length; eventIndex++ )
	{
		if( String( data.events[eventIndex].key || "" ).toLowerCase() == targetKey )
		{
			data.events[eventIndex] = eventDefinition;
			foundExisting = true;
			break;
		}
	}

	if( !foundExisting )
		data.events.push( eventDefinition );

	SaveCustomSeasonalEvents( data );
}

/** @type { ( eventKey: string ) => boolean } */
function RemoveCustomSeasonalEvent( eventKey )
{
	var data = { events: LoadCustomSeasonalEvents() };
	var normalizedEventKey = String( eventKey || "" ).toLowerCase();

	var filteredEvents = [];
	var removed = false;

	for( var eventIndex = 0; eventIndex < data.events.length; eventIndex++ )
	{
		if( String( data.events[eventIndex].key || "" ).toLowerCase() == normalizedEventKey )
		{
			removed = true;
			continue;
		}

		filteredEvents.push( data.events[eventIndex] );
	}

	if( removed )
		SaveCustomSeasonalEvents( { events: filteredEvents } );

	return removed;
}

// =========================================================
// ACTIVE STATE STORAGE
// =========================================================

/** @type { () => any } */
function LoadActiveSeasonalState()
{
	var file = new UOXCFile();
	var defaultState = { active: {}, spawns: {} };

	file.Open( SEASONAL_ACTIVE_FILE, "r", SEASONAL_ACTIVE_SECTION );
	if( file && file.Length() > 0 )
	{
		var rawText = "";
		while( !file.EOF() )
			rawText += String( file.ReadUntil( "\n" ) );

		file.Close();
		file.Free();

		var parsedState = SafeParseJson( rawText );
		if( parsedState )
			return parsedState;
	}

	if( file )
		file.Free();

	return defaultState;
}

/** @type { ( state: any ) => void } */
function SaveActiveSeasonalState( state )
{
	var file = new UOXCFile();
	file.Open( SEASONAL_ACTIVE_FILE, "w", SEASONAL_ACTIVE_SECTION );
	if( file )
	{
		file.Write( JSON.stringify( state || { active: {}, spawns: {} } ) + "\n" );
		file.Close();
		file.Free();
	}
}

/** @type { ( holidayKey: string, isActive: boolean ) => void } */
function MarkSeasonalActive( holidayKey, isActive )
{
	var activeState = LoadActiveSeasonalState();
	var normalizedHolidayKey = String( holidayKey || "" ).toLowerCase();

	if( !activeState.active )
		activeState.active = {};

	activeState.active[normalizedHolidayKey] = ( isActive ? 1 : 0 );
	SaveActiveSeasonalState( activeState );
}

// =========================================================
// REGISTRATION / RELOAD
// =========================================================

function RegisterConfiguredSeasonalEvents()
{
	var enabledBuiltInEvents = GetEnabledBuiltInHolidayDefinitions();
	for( var builtInIndex = 0; builtInIndex < enabledBuiltInEvents.length; builtInIndex++ )
	{
		RegisterOrRescheduleHoliday( enabledBuiltInEvents[builtInIndex] );
	}

	var customEvents = LoadCustomSeasonalEvents();
	for( var customIndex = 0; customIndex < customEvents.length; customIndex++ )
	{
		RegisterOrRescheduleHoliday( customEvents[customIndex] );
	}
}

/** @type { ( holidayDefinition: any ) => void } */
function RegisterOrRescheduleHoliday( holidayDefinition )
{
	if( !holidayDefinition || !holidayDefinition.key )
		return;

	var nowMs = Date.now();
	var nextStartMs = ComputeNextHolidayStartMs( holidayDefinition, nowMs );
	if( nextStartMs <= 0 )
		return;

	var durationMs = Math.max( 0, ToIntSafe( holidayDefinition.durationHours ) * 3600 * 1000 );
	var uniqueEventID = BuildUniqueSeasonalEventID( holidayDefinition.key, nextStartMs );

	TriggerEvent(
		EVENT_MANAGER_SCRIPT_ID,
		"RegisterEvent",
		{
			uniqueEventID: uniqueEventID,
			callbackScriptID: SCRIPT.script_id,
			callbackFuncStart: "SeasonalEventStart",
			callbackFuncEnd: "SeasonalEventEnd",
			type: "ONCE",
			date: nextStartMs,
			duration: durationMs,
			name: holidayDefinition.name || ( "Seasonal: " + holidayDefinition.key ),
			desc: "Seasonal quest window: " + holidayDefinition.key
		},
		{
			holidayKey: holidayDefinition.key,
			holidayName: holidayDefinition.name || holidayDefinition.key,
			holidayDefinition: holidayDefinition
		},
		{
			holidayKey: holidayDefinition.key,
			holidayName: holidayDefinition.name || holidayDefinition.key,
			holidayDefinition: holidayDefinition
		}
	);
}

/** @type { ( holidayKey: string, startMs: number ) => string } */
function BuildUniqueSeasonalEventID( holidayKey, startMs )
{
	return SEASONAL_EVENT_PREFIX + String( holidayKey || "" ).toLowerCase() + "_" + String( startMs );
}

// =========================================================
// EVENT MANAGER CALLBACKS
// =========================================================

function SeasonalEventStart( args )
{
	if( !args || !args.holidayDefinition )
		return;

	var holidayKey = String( args.holidayKey || "" ).toLowerCase();
	var holidayName = String( args.holidayName || holidayKey );

	MarkSeasonalActive( holidayKey, true );
	BroadcastMessage( "[Seasonal] " + holidayName + " is now active!" );

	SpawnSeasonalQuestGivers( args.holidayDefinition );
}

function SeasonalEventEnd( args )
{
	if( !args || !args.holidayDefinition )
		return;

	var holidayKey = String( args.holidayKey || "" ).toLowerCase();
	var holidayName = String( args.holidayName || holidayKey );

	BroadcastMessage( "[Seasonal] " + holidayName + " has ended." );

	DespawnSeasonalQuestGivers( args.holidayDefinition );
	MarkSeasonalActive( holidayKey, false );

	RegisterOrRescheduleHoliday( args.holidayDefinition );
}

// =========================================================
// SPAWN / DESPAWN
// =========================================================

/** @type { ( holidayDefinition: any ) => void } */
function SpawnSeasonalQuestGivers( holidayDefinition )
{
	if( !holidayDefinition || !holidayDefinition.spawns || !holidayDefinition.spawns.length )
		return;

	var holidayKey = String( holidayDefinition.key || "" ).toLowerCase();
	var activeState = LoadActiveSeasonalState();

	if( !activeState.spawns )
		activeState.spawns = {};

	if( !activeState.spawns[holidayKey] )
		activeState.spawns[holidayKey] = [];

	for( var spawnIndex = 0; spawnIndex < holidayDefinition.spawns.length; spawnIndex++ )
	{
		var spawnInfo = holidayDefinition.spawns[spawnIndex];
		if( !spawnInfo || !spawnInfo.npcSectionID )
			continue;

		var spawnedNpc = SpawnNPC(
			String( spawnInfo.npcSectionID ),
			ToIntSafe( spawnInfo.x ),
			ToIntSafe( spawnInfo.y ),
			ToIntSafe( spawnInfo.z ),
			ToIntSafe( spawnInfo.world ),
			ToIntSafe( spawnInfo.instance )
		);

		if( ValidateObject( spawnedNpc ) )
		{
			spawnedNpc.SetTag( "SeasonalEventKey", holidayKey );
			spawnedNpc.SetTag( "SeasonalQuestGiver", 1 );

			activeState.spawns[holidayKey].push( spawnedNpc.serial );
		}
	}

	SaveActiveSeasonalState( activeState );
}

/** @type { ( holidayDefinition: any ) => void } */
function DespawnSeasonalQuestGivers( holidayDefinition )
{
	if( !holidayDefinition )
		return;

	var holidayKey = String( holidayDefinition.key || "" ).toLowerCase();
	var activeState = LoadActiveSeasonalState();

	if( !activeState.spawns || !activeState.spawns[holidayKey] )
		return;

	var spawnedSerials = activeState.spawns[holidayKey];
	for( var serialIndex = 0; serialIndex < spawnedSerials.length; serialIndex++ )
	{
		var spawnedNpc = CalcCharFromSer( ToIntSafe( spawnedSerials[serialIndex] ) );
		if( ValidateObject( spawnedNpc ) )
		{
			if( spawnedNpc.GetTag( "SeasonalQuestGiver" ) )
				spawnedNpc.Delete();
		}
	}

	delete activeState.spawns[holidayKey];
	SaveActiveSeasonalState( activeState );
}

function RestoreActiveSeasonalSpawns()
{
	var activeState = LoadActiveSeasonalState();
	if( !activeState || !activeState.active )
		return;

	var allHolidayDefinitions = GetEnabledBuiltInHolidayDefinitions().concat( LoadCustomSeasonalEvents() );

	for( var holidayIndex = 0; holidayIndex < allHolidayDefinitions.length; holidayIndex++ )
	{
		var holidayDefinition = allHolidayDefinitions[holidayIndex];
		if( !holidayDefinition || !holidayDefinition.key )
			continue;

		var holidayKey = String( holidayDefinition.key || "" ).toLowerCase();

		if( activeState.active[holidayKey] == 1 )
		{
			if( !activeState.spawns || !activeState.spawns[holidayKey] || !activeState.spawns[holidayKey].length )
				SpawnSeasonalQuestGivers( holidayDefinition );
		}
	}
}

// =========================================================
// GM COMMANDS
// =========================================================

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_SEASON( socket, cmdString )
{
	var gmChar = socket.currentChar;
	if( !ValidateObject( gmChar ) )
		return;

	cmdString = String( cmdString || "" ).replace( /^\s+|\s+$/g, "" );
	var parts = ( cmdString.length > 0 ? cmdString.split( /\s+/ ) : [] );

	if( parts.length < 1 )
	{
		PrintSeasonHelp( socket );
		return;
	}

	var subCommand = String( parts[0] || "" ).toLowerCase();

	if( subCommand == "list" )
	{
		ListEnabledSeasonalEvents( socket );
		return;
	}

	if( subCommand == "catalog" )
	{
		ListHolidayCatalog( socket );
		return;
	}

	if( subCommand == "packs" )
	{
		ListHolidayPacks( socket );
		return;
	}

	if( subCommand == "active" )
	{
		ListActiveSeasonalEvents( socket );
		return;
	}

	if( subCommand == "enablepack" )
	{
		if( parts.length < 2 )
		{
			socket.SysMessage( "Usage: [season enablepack <packName>" );
			return;
		}

		EnableHolidayPack( socket, parts[1] );
		return;
	}

	if( subCommand == "disablepack" )
	{
		if( parts.length < 2 )
		{
			socket.SysMessage( "Usage: [season disablepack <packName>" );
			return;
		}

		DisableHolidayPack( socket, parts[1] );
		return;
	}

	if( subCommand == "reload" )
	{
		RegisterConfiguredSeasonalEvents();
		socket.SysMessage( "Seasonal events re-registered from current config." );
		return;
	}

	// [season add <key> <month> <day> <hour> <minute> <durationHours> <npcSectionID> <x> <y> <z> <world> <instance>
	if( subCommand == "add" )
	{
		if( parts.length < 13 )
		{
			socket.SysMessage( "Usage: [season add <key> <month> <day> <hour> <minute> <durationHours> <npcSectionID> <x> <y> <z> <world> <instance>" );
			return;
		}

		var eventKey = String( parts[1] || "" ).toLowerCase();
		var month = parseInt( parts[2], 10 );
		var day = parseInt( parts[3], 10 );
		var hour = parseInt( parts[4], 10 );
		var minute = parseInt( parts[5], 10 );
		var durationHours = parseInt( parts[6], 10 );
		var npcSectionID = String( parts[7] || "" );
		var x = parseInt( parts[8], 10 );
		var y = parseInt( parts[9], 10 );
		var z = parseInt( parts[10], 10 );
		var world = parseInt( parts[11], 10 );
		var instance = parseInt( parts[12], 10 );

		if(
			isNaN( month ) || isNaN( day ) || isNaN( hour ) || isNaN( minute ) ||
			isNaN( durationHours ) || isNaN( x ) || isNaN( y ) || isNaN( z ) ||
			isNaN( world ) || isNaN( instance )
		)
		{
			socket.SysMessage( "Invalid numeric values." );
			return;
		}

		var customEventDefinition = {
			key: eventKey,
			name: "Custom Seasonal Event: " + eventKey,
			type: "FIXED",
			month: month,
			day: day,
			hour: hour,
			minute: minute,
			durationHours: durationHours,
			spawns: [
				{
					npcSectionID: npcSectionID,
					x: x,
					y: y,
					z: z,
					world: world,
					instance: instance
				}
			]
		};

		UpsertCustomSeasonalEvent( customEventDefinition );
		RegisterOrRescheduleHoliday( customEventDefinition );

		socket.SysMessage( "Added custom seasonal event '" + eventKey + "' and registered it." );
		return;
	}

	if( subCommand == "remove" )
	{
		if( parts.length < 2 )
		{
			socket.SysMessage( "Usage: [season remove <key>" );
			return;
		}

		var removeKey = String( parts[1] || "" ).toLowerCase();
		if( RemoveCustomSeasonalEvent( removeKey ) )
		{
			socket.SysMessage( "Removed custom seasonal event '" + removeKey + "' from config." );
			socket.SysMessage( "If it is already in Event Manager, use [em list and [em remove <index>." );
		}
		else
		{
			socket.SysMessage( "No custom seasonal event found for '" + removeKey + "'." );
		}
		return;
	}

	PrintSeasonHelp( socket );
}

function PrintSeasonHelp( socket )
{
	socket.SysMessage( "Seasonal Quest Commands:" );
	socket.SysMessage( "  [season list" );
	socket.SysMessage( "  [season catalog" );
	socket.SysMessage( "  [season packs" );
	socket.SysMessage( "  [season active" );
	socket.SysMessage( "  [season enablepack <packName>" );
	socket.SysMessage( "  [season disablepack <packName>" );
	socket.SysMessage( "  [season reload" );
	socket.SysMessage( "  [season add <key> <month> <day> <hour> <minute> <durationHours> <npcSectionID> <x> <y> <z> <world> <instance>" );
	socket.SysMessage( "  [season remove <key>" );
}

/** @type { ( socket: Socket ) => void } */
function ListHolidayCatalog( socket )
{
	var holidayCatalog = GetBuiltInHolidayCatalog();
	socket.SysMessage( "--- Built-In Holiday Catalog ---" );

	for( var holidayKey in holidayCatalog )
	{
		if( holidayCatalog.hasOwnProperty( holidayKey ) )
		{
			socket.SysMessage( holidayKey + " (" + holidayCatalog[holidayKey].name + ")" );
		}
	}
}

/** @type { ( socket: Socket ) => void } */
function ListHolidayPacks( socket )
{
	var holidayPacks = GetBuiltInHolidayPacks();
	var seasonalConfig = LoadSeasonalConfig();
	var enabledPackLookup = {};

	for( var packIndex = 0; packIndex < seasonalConfig.enabledPacks.length; packIndex++ )
	{
		enabledPackLookup[String( seasonalConfig.enabledPacks[packIndex] || "" ).toLowerCase()] = true;
	}

	socket.SysMessage( "--- Holiday Packs ---" );
	for( var packName in holidayPacks )
	{
		if( holidayPacks.hasOwnProperty( packName ) )
		{
			socket.SysMessage(
				packName +
				( enabledPackLookup[String( packName ).toLowerCase()] ? " [ENABLED]" : " [DISABLED]" ) +
				" - " + holidayPacks[packName].length + " holiday(s)"
			);
		}
	}
}

/** @type { ( socket: Socket ) => void } */
function ListEnabledSeasonalEvents( socket )
{
	var builtInEvents = GetEnabledBuiltInHolidayDefinitions();
	var customEvents = LoadCustomSeasonalEvents();

	socket.SysMessage( "--- Enabled Built-In Seasonal Events ---" );
	if( !builtInEvents.length )
	{
		socket.SysMessage( "(none)" );
	}
	else
	{
		for( var builtInIndex = 0; builtInIndex < builtInEvents.length; builtInIndex++ )
		{
			socket.SysMessage( builtInEvents[builtInIndex].key + " (" + builtInEvents[builtInIndex].name + ")" );
		}
	}

	socket.SysMessage( "--- Custom Seasonal Events ---" );
	if( !customEvents.length )
	{
		socket.SysMessage( "(none)" );
	}
	else
	{
		for( var customIndex = 0; customIndex < customEvents.length; customIndex++ )
		{
			socket.SysMessage( customEvents[customIndex].key + " (" + ( customEvents[customIndex].name || "custom" ) + ")" );
		}
	}
}

/** @type { ( socket: Socket ) => void } */
function ListActiveSeasonalEvents( socket )
{
	var activeState = LoadActiveSeasonalState();

	socket.SysMessage( "--- Active Seasonal Events ---" );
	if( !activeState.active )
	{
		socket.SysMessage( "(none)" );
		return;
	}

	var foundAny = false;
	for( var holidayKey in activeState.active )
	{
		if( activeState.active.hasOwnProperty( holidayKey ) && activeState.active[holidayKey] == 1 )
		{
			foundAny = true;
			socket.SysMessage( "ACTIVE: " + holidayKey );
		}
	}

	if( !foundAny )
		socket.SysMessage( "(none)" );
}

/** @type { ( socket: Socket, packName: string ) => void } */
function EnableHolidayPack( socket, packName )
{
	var normalizedPackName = String( packName || "" ).toLowerCase();
	var holidayPacks = GetBuiltInHolidayPacks();

	if( !holidayPacks[normalizedPackName] )
	{
		socket.SysMessage( "Unknown pack: " + normalizedPackName );
		return;
	}

	var seasonalConfig = LoadSeasonalConfig();
	var alreadyEnabled = false;

	for( var index = 0; index < seasonalConfig.enabledPacks.length; index++ )
	{
		if( String( seasonalConfig.enabledPacks[index] || "" ).toLowerCase() == normalizedPackName )
		{
			alreadyEnabled = true;
			break;
		}
	}

	if( alreadyEnabled )
	{
		socket.SysMessage( "Pack already enabled: " + normalizedPackName );
		return;
	}

	seasonalConfig.enabledPacks.push( normalizedPackName );
	SaveSeasonalConfig( seasonalConfig );

	socket.SysMessage( "Enabled holiday pack: " + normalizedPackName );
	socket.SysMessage( "Use [season reload to register newly enabled events immediately." );
}

/** @type { ( socket: Socket, packName: string ) => void } */
function DisableHolidayPack( socket, packName )
{
	var normalizedPackName = String( packName || "" ).toLowerCase();
	var seasonalConfig = LoadSeasonalConfig();

	var filteredPacks = [];
	var removed = false;

	for( var index = 0; index < seasonalConfig.enabledPacks.length; index++ )
	{
		var existingPackName = String( seasonalConfig.enabledPacks[index] || "" ).toLowerCase();
		if( existingPackName == normalizedPackName )
		{
			removed = true;
			continue;
		}
		filteredPacks.push( existingPackName );
	}

	if( !removed )
	{
		socket.SysMessage( "Pack was not enabled: " + normalizedPackName );
		return;
	}

	seasonalConfig.enabledPacks = filteredPacks;
	SaveSeasonalConfig( seasonalConfig );

	socket.SysMessage( "Disabled holiday pack: " + normalizedPackName );
	socket.SysMessage( "Use [em list / [em remove if you also want already-registered Event Manager entries removed now." );
}

// =========================================================
// DATE CALCULATIONS
// =========================================================

/** @type { ( holidayDefinition: any, nowMs: number ) => number } */
function ComputeNextHolidayStartMs( holidayDefinition, nowMs )
{
	var currentDate = new Date( nowMs );
	var targetYear = currentDate.getFullYear();
	var holidayStartDate = ResolveHolidayDateForYear( holidayDefinition, targetYear );

	if( !holidayStartDate )
		return 0;

	if( holidayStartDate.getTime() <= nowMs )
	{
		targetYear = targetYear + 1;
		holidayStartDate = ResolveHolidayDateForYear( holidayDefinition, targetYear );
	}

	if( !holidayStartDate )
		return 0;

	return holidayStartDate.getTime();
}

/** @type { ( holidayDefinition: any, year: number ) => Date | null } */
function ResolveHolidayDateForYear( holidayDefinition, year )
{
	if( !holidayDefinition )
		return null;

	var holidayType = String( holidayDefinition.type || "FIXED" ).toUpperCase();
	var hour = ToIntSafe( holidayDefinition.hour );
	var minute = ToIntSafe( holidayDefinition.minute );

	if( holidayType == "FIXED" )
	{
		return new Date(
			year,
			ToIntSafe( holidayDefinition.month ) - 1,
			ToIntSafe( holidayDefinition.day ),
			hour,
			minute,
			0,
			0
		);
	}

	if( holidayType == "EASTER" )
	{
		var easterInfo = ComputeWesternEasterDate( year );
		return new Date( year, easterInfo.monthIndex, easterInfo.day, hour, minute, 0, 0 );
	}

	if( holidayType == "LUNAR_NEWYEAR" )
	{
		return ResolveMappedHolidayDate( "lunar_new_year", year, hour, minute );
	}

	if( holidayType == "HOLI" )
	{
		return ResolveMappedHolidayDate( "holi", year, hour, minute );
	}

	if( holidayType == "RAMADAN" )
	{
		return ResolveMappedHolidayDate( "ramadan", year, hour, minute );
	}

	if( holidayType == "EID_AL_FITR" )
	{
		return ResolveMappedHolidayDate( "eid_al_fitr", year, hour, minute );
	}

	if( holidayType == "VESAK" )
	{
		return ResolveMappedHolidayDate( "vesak", year, hour, minute );
	}

	if( holidayType == "DRAGON_BOAT" )
	{
		return ResolveMappedHolidayDate( "dragon_boat_festival", year, hour, minute );
	}

	if( holidayType == "MID_AUTUMN" )
	{
		return ResolveMappedHolidayDate( "mid_autumn_festival", year, hour, minute );
	}

	if( holidayType == "DIWALI" )
	{
		return ResolveMappedHolidayDate( "diwali", year, hour, minute );
	}

	if( holidayType == "HANUKKAH" )
	{
		return ResolveMappedHolidayDate( "hanukkah", year, hour, minute );
	}

	return null;
}

/** @type { ( holidayKey: string, year: number, hour: number, minute: number ) => Date | null } */
function ResolveMappedHolidayDate( holidayKey, year, hour, minute )
{
	var holidayDateTable = GetHolidayDateTable();
	var holidayMap = holidayDateTable[String( holidayKey || "" ).toLowerCase()];

	if( !holidayMap || !holidayMap[year] )
		return null;

	var dateInfo = holidayMap[year];
	return new Date( year, dateInfo.month - 1, dateInfo.day, hour, minute, 0, 0 );
}

// ---------------------------------------------------------
// Movable holiday lookup table
// Add more years whenever you want.
// month is 1-12
// ---------------------------------------------------------
function GetHolidayDateTable()
{
	return {
		lunar_new_year: {
			2025: { month: 1, day: 29 },
			2026: { month: 2, day: 17 },
			2027: { month: 2, day: 6 },
			2028: { month: 1, day: 26 },
			2029: { month: 2, day: 13 },
			2030: { month: 2, day: 3 }
		},

		holi: {
			2025: { month: 3, day: 14 },
			2026: { month: 3, day: 3 },
			2027: { month: 3, day: 22 },
			2028: { month: 3, day: 11 },
			2029: { month: 3, day: 1 },
			2030: { month: 3, day: 19 }
		},

		ramadan: {
			2025: { month: 3, day: 1 },
			2026: { month: 2, day: 18 },
			2027: { month: 2, day: 8 },
			2028: { month: 1, day: 28 },
			2029: { month: 1, day: 16 },
			2030: { month: 1, day: 6 }
		},

		eid_al_fitr: {
			2025: { month: 3, day: 30 },
			2026: { month: 3, day: 20 },
			2027: { month: 3, day: 9 },
			2028: { month: 2, day: 26 },
			2029: { month: 2, day: 14 },
			2030: { month: 2, day: 4 }
		},

		vesak: {
			2025: { month: 5, day: 12 },
			2026: { month: 5, day: 31 },
			2027: { month: 5, day: 20 },
			2028: { month: 5, day: 9 },
			2029: { month: 5, day: 27 },
			2030: { month: 5, day: 16 }
		},

		dragon_boat_festival: {
			2025: { month: 5, day: 31 },
			2026: { month: 6, day: 19 },
			2027: { month: 6, day: 9 },
			2028: { month: 5, day: 28 },
			2029: { month: 6, day: 16 },
			2030: { month: 6, day: 5 }
		},

		mid_autumn_festival: {
			2025: { month: 10, day: 6 },
			2026: { month: 9, day: 25 },
			2027: { month: 9, day: 15 },
			2028: { month: 10, day: 3 },
			2029: { month: 9, day: 22 },
			2030: { month: 9, day: 12 }
		},

		diwali: {
			2025: { month: 10, day: 20 },
			2026: { month: 11, day: 8 },
			2027: { month: 10, day: 29 },
			2028: { month: 10, day: 17 },
			2029: { month: 11, day: 5 },
			2030: { month: 10, day: 26 }
		},

		hanukkah: {
			2025: { month: 12, day: 14 },
			2026: { month: 12, day: 4 },
			2027: { month: 12, day: 24 },
			2028: { month: 12, day: 12 },
			2029: { month: 12, day: 2 },
			2030: { month: 12, day: 22 }
		}
	};
}

// Meeus/Jones/Butcher algorithm
/** @type { ( year: number ) => any } */
function ComputeWesternEasterDate( year )
{
	var a = year % 19;
	var b = Math.floor( year / 100 );
	var c = year % 100;
	var d = Math.floor( b / 4 );
	var e = b % 4;
	var f = Math.floor( ( b + 8 ) / 25 );
	var g = Math.floor( ( b - f + 1 ) / 3 );
	var h = ( 19 * a + b - d - g + 15 ) % 30;
	var i = Math.floor( c / 4 );
	var k = c % 4;
	var l = ( 32 + 2 * e + 2 * i - h - k ) % 7;
	var m = Math.floor( ( a + 11 * h + 22 * l ) / 451 );
	var month = Math.floor( ( h + l - 7 * m + 114 ) / 31 );
	var day = ( ( h + l - 7 * m + 114 ) % 31 ) + 1;

	return {
		monthIndex: month - 1,
		day: day
	};
}

// =========================================================
// UTILITIES
// =========================================================

/** @type { ( value: any ) => number } */
function ToIntSafe( value )
{
	var parsedValue = parseInt( value, 10 );
	return ( isNaN( parsedValue ) ? 0 : parsedValue );
}

/** @type { ( raw: any ) => any } */
function SafeParseJson( raw )
{
	try
	{
		return JSON.parse( String( raw || "" ) );
	}
	catch( error )
	{
		return null;
	}
}

/** @type { ( value: any ) => boolean } */
function IsArray( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

/** @type { ( msg: string ) => void } */
function BroadcastMessage( msg )
{
	// Replace or remove if you have your own shard-wide broadcast method.
	Console.Print( String( msg || "" ) + "\n" );
}