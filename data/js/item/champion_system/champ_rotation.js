/// <reference path="../definitions.d.ts" />
// @ts-check
//
// champ_rotation.js
// - Uses Event Manager (script ID 5) to rotate champion spawns daily
// - Disables all champ altars, then enables 2 random champ types

// How many champs to keep active each rotation
var activeChampionSpawns = 2;

// Daily schedule (RECURRING)
var rotateHour = 3;   // 3 AM shard time
var rotateMin  = 0;   // :00
var rotateDays = [ 0, 1, 2, 3, 4, 5, 6 ]; // every day

// -------------------------------
// Champ type mapping (matches champion_spawn_cmd.js)
// -------------------------------
var ChampionNameToID = {
	"abyss": 1,
	"arachnid": 2,
	"cold": 3,
	"forest": 4,
	"unholy": 5,
	"vermin": 6,
	"habitat": 7
};

var gRotateMode = "DISABLE_ALL";
var gSelectedIDs = null;
var gDisableCount = 0;
var gEnableCount = 0;
var gDespawnAltarSerial = 0;

// Set these to the facet/instance where your champ markers exist
var champWorld = 0;  // example: Tram
var champInstance = 0;  // usually 0

/** @type { ( string: any ) => string } */
function NormalizeTypeString( string )
{
	if( string == null )
		return "";
	return ( "" + string ).toLowerCase();
}

/** @type { () => { type: string, x: number, y: number, z: number }[] } */
function GetAltarDataList()
{
	return [
		{ type: "Unholy",   x: 5178, y: 708,  z: 0  },
		{ type: "Vermin",   x: 5557, y: 824,  z: 45 },
		{ type: "Cold",     x: 5259, y: 803,  z: 0 },
		{ type: "Abyss",    x: 5814, y: 1350, z: -19 },
		{ type: "Arachnid", x: 5190, y: 1605, z: 0  },
		{ type: "Forest",   x: 5559, y: 3757, z: 1  },
		{ type: "Habitat",  x: 7042, y: 1889, z: 40 }
	];
}

/** @type { () => void } */
function onScriptLoad()
{
	RegisterChampRotationEvent();
}

/** @type { () => void } */
function RegisterChampRotationEvent()
{
	var cfg = {
		uniqueEventID: "champ_rotation_daily",
		callbackScriptID: SCRIPT.script_id,
		callbackFuncStart: "ChampRotation_Tick",

		type: "RECURRING",
		days: rotateDays,
		hour: rotateHour,
		minute: rotateMin,

		name: "Champion Rotation (Daily)",
		desc: "Disables all champ spawns, enables 2 random champ types daily"
	};

	// argsStart can be empty; keep here in case you want to pass settings later
	TriggerEvent( 5, "RegisterEvent", cfg, { }, { } );

	Console.Log( "[champ_rotation] Registered daily rotation event (RECURRING @ " + rotateHour + ":" + ( rotateMin < 10 ? ( "0" + rotateMin ) : rotateMin ) + ")" );
}

/** @type { ( argsStart: EventArgs ) => void } */
function ChampRotation_Tick( argsStart )
{
	// 1) Pick 2 random champ types
	var pick = PickRandomChampionTypes( activeChampionSpawns );

	// 2) Disable everything, then enable the picked types
	RotateChampions( pick );

	// 3) Log what we picked
	Console.Log( "[champ_rotation] Active champs: " + pick.join(", ") );
}

/** @type { ( count: number ) => string[] } */
function PickRandomChampionTypes( count )
{
	var keys = [];
	for( var k in ChampionNameToID )
	{
		if( ChampionNameToID.hasOwnProperty( k ) )
			keys.push( k );
	}

	ShuffleArrayInPlace( keys );

	if( count <= 0 )
		count = 1;
	if( count > keys.length )
		count = keys.length;

	var out = [];
	for( var i = 0; i < count; ++i )
		out.push( keys[i] );

	return out;
}

/** @type { ( a: any[] ) => void } */
function ShuffleArrayInPlace( a )
{
	for( var i = a.length - 1; i > 0; --i )
	{
		var j = RandomNumber( 0, i );
		var t = a[i];
		a[i] = a[j];
		a[j] = t;
	}
}

/** @type { ( selectedKeys: string[] ) => void } */
function RotateChampions( selectedKeys )
{
	var selectedIDs = {};
	for( var i = 0; i < selectedKeys.length; ++i )
	{
		var key = NormalizeTypeString( selectedKeys[i] );
		if( ChampionNameToID.hasOwnProperty( key ) )
			selectedIDs[ ChampionNameToID[key] ] = 1;
	}

	gSelectedIDs = selectedIDs;
	gDisableCount = 0;
	gEnableCount = 0;

	// Step A: disable all
	gRotateMode = "DISABLE_ALL";
	ScanAllMarkersAndApply();

	// Step B: enable selected
	gRotateMode = "ENABLE_SELECTED";
	ScanAllMarkersAndApply();

	Console.Log( "[champ_rotation] Disabled: " + gDisableCount + ", Enabled: " + gEnableCount );
}

/** @type { () => void } */
function ScanAllMarkersAndApply()
{
	var altarData = GetAltarDataList();
	var markerID = 0x1F14;

	for( var i = 0; i < altarData.length; ++i )
	{
		var d = altarData[i];

		var marker = FindItem( d.x, d.y, d.z, champWorld, markerID, champInstance );
		if( !ValidateObject( marker ) || marker.GetTag( "ChampAltarMarker" ) != 1 )
		{
			// debug once you confirm firing
			// Console.Log("[champ_rotation] No marker at " + d.x + "," + d.y + "," + d.z);
			continue;
		}

		// This matches the command behavior: scan around marker radius 12
		AreaItemFunction( "ChampRotation_ScanAltarsAround", marker, 12, null );
	}
}

/** @type { ( src: Item, item: Item, ctx: any ) => boolean } */
function ChampRotation_ScanAltarsAround( src, item, ctx )
{
	if( !ValidateObject( item ) )
		return false;

	var ct = item.GetTag( "championType" );
	if( ct == null )
		return false;

	var champID = parseInt( ct, 10 ) || 0;
	if( champID <= 0 )
		return false;

	if( gRotateMode == "DISABLE_ALL" )
	{
		if( DisableOneChampionAltar( item ) )
			gDisableCount++;
		return true;
	}

	// ENABLE_SELECTED
	if( gSelectedIDs && gSelectedIDs[ champID ] == 1 )
	{
		if( EnableOneChampionAltar( item ) )
			gEnableCount++;
	}

	return true;
}

/** @type { ( altar: Item ) => boolean } */
function EnableOneChampionAltar( altar )
{
	if( !ValidateObject( altar ) )
		return false;

	if( altar.GetTag( "spawnActive" ) )
		return false;

	var championType = altar.GetTag( "championType" ) || 0;
	if(( parseInt( championType, 10 ) || 0) == 0 )
	{
		Console.Log( "[champ_rotation] Cannot enable: championType missing at " + altar.x + "," + altar.y );
		return false;
	}

	altar.SetTag( "spawnActive", 1 );
	altar.SetTag( "killCount", 0 );
	altar.SetTag( "spawnStage", 1 );
	altar.SetTag( "redSkullCount", 1 );
	altar.SetTag( "whiteSkullCount", 0 );

	TriggerEvent( 7500, "PlaceRedSkulls", altar, 1 );
	TriggerEvent( 7500, "PlaceWhiteSkulls", altar, 0, 1 );
	TriggerEvent( 7500, "StartChampionWave", altar, 1 );

	altar.StartTimer( 30000, 1, 7500 );
	altar.StartTimer( 600000, 10, 7500 );

	altar.Refresh();
	return true;
}

/** @type { ( altar: Item ) => boolean } */
function DisableOneChampionAltar( altar )
{
	if( !ValidateObject( altar ) )
		return false;

	if( !altar.GetTag( "spawnActive" ) )
		return false;

	altar.SetTag( "spawnActive", 0 );
	altar.SetTag( "killCount", 0 );
	altar.SetTag( "whiteSkullCount", 0 );
	altar.SetTag( "redSkullCount", 0 );
	altar.SetTag( "spawnStage", 1 );

	// Stop champ timers
	altar.KillJSTimer( 1, 7500 );
	altar.KillJSTimer( 10, 7500 );

	// Remove skulls (matches champion_spawn_cmd.js)
	TriggerEvent( 7500, "RemoveRedSkulls", altar );
	TriggerEvent( 7500, "RemoveWhiteSkulls", altar );

	// Despawn mobs that belong to this altar (no socket required)
	gDespawnAltarSerial = altar.serial;
	AreaCharacterFunction( "RemoveSpawn_Rotation", altar, 80, null );
	gDespawnAltarSerial = 0;

	altar.Refresh();
	return true;
}

/** @type { ( srcChar: Character, trgChar: Character, pSock: Socket | null ) => boolean } */
function RemoveSpawn_Rotation( srcChar, trgChar, pSock )
{
	// pSock is ignored on purpose (Event Manager has no socket)
	if( !ValidateObject( trgChar ) )
		return true;

	if( gDespawnAltarSerial != 0 && trgChar.GetTag( "championSpawnID" ) == gDespawnAltarSerial )
	{
		trgChar.Delete();
	}
	return true;
}