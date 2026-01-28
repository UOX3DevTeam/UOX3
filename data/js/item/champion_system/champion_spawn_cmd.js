/// <reference path="../definitions.d.ts" />
// @ts-check

function CommandRegistration()
{
    RegisterCommand( "champspawn", 8, true );
    RegisterCommand( "champenable", 8, true );
    RegisterCommand( "champdisable", 8, true );
    RegisterCommand( "champremove", 8, true );
	RegisterCommand( "champmenu", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_CHAMPSPAWN( socket, cmdString )
{
    var pChar = socket.currentChar;
    if( !ValidateObject( pChar ))
        return;

    cmdString = ("" + cmdString).replace(/^\s+|\s+$/g, "");
    var parts = (cmdString.length > 0) ? cmdString.split(/\s+/) : [];

    if( parts.length < 1 )
    {
        socket.SysMessage( "Usage: [champspawn all] or [champspawn <type>]" );
        socket.SysMessage( "Types: abyss, arachnid, cold, forest, unholy, vermin" );
        return;
    }

    var arg = NormalizeTypeString( parts[0] );
    var all = ( arg == "all" );

    if( !all && !ChampionNameToID.hasOwnProperty( arg ))
    {
        socket.SysMessage( "Unknown type: " + parts[0] );
        socket.SysMessage( "Types: abyss, arachnid, cold, forest, unholy, vermin or all" );
        return;
    }

    SetupChampionAltars( pChar, all ? "all" : arg );
}

var ChampPlatformIDs = {
    0x0751: 1,
    0x03F7: 1,
    0x03F8: 1,
    0x03F9: 1,
    0x03FA: 1,
    0x03EE: 1,
    0x03EF: 1,
    0x03F0: 1,
    0x03F1: 1,
    0x03F2: 1,
	0x0FE7: 1,
	0x0FE8: 1,
	0x0FE9: 1,
	0x0FEB: 1,
	0x0FEC: 1,
	0x0FED: 1,
	0x0FEE: 1
};

function SetupChampionAltars( pUser, spawnFilter )
{
    if( !ValidateObject( pUser ))
        return;

    var socket = pUser.socket;
    if( socket == null )
        return;

    var filter = NormalizeTypeString( spawnFilter );
    if( filter == "" )
        filter = "all";

    var altarData = [
        { type: "Unholy",   x: 5178, y: 708,  z: 0  },
        { type: "Vermin",   x: 5557, y: 824,  z: 45 },
        { type: "Cold",     x: 5258, y: 829,  z: 42 },
        { type: "Abyss",    x: 5814, y: 1350, z: -19 },
        { type: "Arachnid", x: 5190, y: 1605, z: 0  },
        { type: "Forest",   x: 5559, y: 3757, z: 1  },
		{ type: "Habitat",  x: 7042, y: 1889, z: 40  }
    ];

    var created = 0;
    var skipped = 0;
    var failed  = 0;

    var worldNum = pUser.worldnumber;
    var instID   = pUser.instanceID;

    var MARKER_ID = 0x1F14;

    for( var i = 0; i < altarData.length; ++i )
    {
        var d = altarData[i];
        var dType = NormalizeTypeString( d.type );

        // Type filter
        if( filter != "all" && dType != filter )
            continue;

        // Already spawned?
        var existingMarker = FindItem( d.x, d.y, d.z, worldNum, MARKER_ID, instID );
        if( ValidateObject( existingMarker ) && existingMarker.GetTag( "ChampAltarMarker" ) == 1 )
        {
            skipped++;
            continue;
        }

        // Place the multi (CreateHouse does NOT return the platform step items)
        CreateHouse( 1000, d.x, d.y, d.z, worldNum, instID, 0, false );

        // Marker = proof of success + anchor for scans/removal
        var marker = CreateBlankItem( null, null, 1, "champ altar marker", MARKER_ID, 0, "ITEM", false );
        if( !ValidateObject( marker ))
        {
            failed++;
            socket.SysMessage( "FAIL " + d.type + " (marker creation failed)" );
			Console.Log( "FAIL " + d.type + " (marker creation failed)" );
            continue;
        }

        marker.SetLocation( d.x, d.y, d.z, worldNum, instID );
        marker.movable = 2;
        marker.decayable = false;

        if( marker.visible != null )
            marker.visible = 3;

        marker.SetTag( "ChampAltarMarker", 1 );
        marker.SetTag( "ChampAltarType", d.type );

        // Tag platform step items around marker so champremove can delete safely
        socket._tagPlatformTypeStr = dType;

        // If your environment needs it, keep the refresh guard:
        marker.Refresh();
        if( ValidateObject( marker ))
            AreaItemFunction( "TagChampPlatformAround", marker, 12, socket );

        created++;
    }

    socket.SysMessage( "Champion altars setup complete (" + filter + "). Created: " + created + ", Skipped: " + skipped + ", Failed: " + failed + "." );

    if( created > 0 && failed == 0 )
        socket.SysMessage( "All requested champion altars spawned successfully." );
}

function TagChampPlatformAround( src, item, pSock )
{
    if( !ValidateObject( item ))
        return false;

    if( ChampPlatformIDs[item.id] != 1 )
        return false;

    // Safety: only tag your platform hue (COLOUR=0x455)
    if( item.colour != 0x0455 )
        return false;

    item.SetTag( "ChampAltarPlatform", 1 );
    item.SetTag( "ChampAltarType", pSock._tagPlatformTypeStr );
    return true;
}

// ------------------------------------------------------------
// Enable / Disable commands (mirror the gump behavior)
// ------------------------------------------------------------

var ChampionNameToID = {
    "abyss": 1,
    "arachnid": 2,
    "cold": 3,
    "forest": 4,
    "unholy": 5,
    "vermin": 6,
	"habitat": 7
};

function NormalizeTypeString( s )
{
    if( s == null )
        return "";
    s = ( "" + s ).toLowerCase();
    return s;
}

function GetAltarDataList()
{
    return [
        { type: "Unholy",   x: 5178, y: 708,  z: 0  },
        { type: "Vermin",   x: 5557, y: 824,  z: 45 },
        { type: "Cold",     x: 5258, y: 829,  z: 42 },
        { type: "Abyss",    x: 5814, y: 1350, z: -19 },
        { type: "Arachnid", x: 5190, y: 1605, z: 0  },
        { type: "Forest",   x: 5559, y: 3757, z: 1  },
		{ type: "Habitat",  x: 7042, y: 1889, z: 40  }
    ];
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_CHAMPENABLE( socket, cmdString )
{
    RunChampToggleCommand( socket, cmdString, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_CHAMPDISABLE( socket, cmdString )
{
    RunChampToggleCommand( socket, cmdString, false );
}

function RunChampToggleCommand( socket, cmdString, enabling )
{
    var pUser = socket.currentChar;
    if( !ValidateObject( pUser ))
        return;

    cmdString = ("" + cmdString).replace(/^\s+|\s+$/g, "");
    var parts = (cmdString.length > 0) ? cmdString.split(/\s+/) : [];

    if (parts.length < 1)
    {
        socket.SysMessage("Usage: [" + (enabling ? "champenable" : "champdisable") + " all] or [" + (enabling ? "champenable" : "champdisable") + " <type>]");
        socket.SysMessage("Types: abyss, arachnid, cold, forest, unholy, vermin");
        return;
    }

    var arg = NormalizeTypeString(parts[0]);
    var all = ( arg == "all" );

    if( !all && !ChampionNameToID.hasOwnProperty( arg ))
    {
        socket.SysMessage( "Unknown type: " + parts[0] );
        socket.SysMessage( "Types: abyss, arachnid, cold, forest, unholy, vermin or all" );
        return;
    }

    socket._champToggleAll = all ? 1 : 0;
    socket._champToggleID  = all ? 0 : ChampionNameToID[arg];
    socket._champToggleEnable = enabling ? 1 : 0;
    socket._champToggleCount = 0;
    socket._champToggleSkipped = 0;

    var altarData = GetAltarDataList();
    var MARKER_ID = 0x1F14;

    var worldNum = pUser.worldnumber;
    var instID   = pUser.instanceID;

    for( var i = 0; i < altarData.length; ++i )
    {
        var d = altarData[i];

        if( !all )
        {
            if( NormalizeTypeString( d.type ) != arg )
                continue;
        }

        var marker = FindItem( d.x, d.y, d.z, worldNum, MARKER_ID, instID );
        if( ValidateObject( marker ) && marker.GetTag( "ChampAltarMarker" ) == 1 )
        {
            AreaItemFunction( "ChampToggleAltarsAround", marker, 12, socket );
        }
        else
        {
            AreaItemFunction( "ChampToggleAltarsAround", pUser, 12, socket );
        }
    }

    socket.SysMessage(( enabling ? "Enabled" : "Disabled" ) + " champion spawns: " + socket._champToggleCount + " (skipped: " + socket._champToggleSkipped + ")" );
}

// AreaItemFunction callback: find altar items by presence of championType tag
function ChampToggleAltarsAround( src, item, pSock )
{
    if( !ValidateObject( item ))
        return false;

    var ct = item.GetTag( "championType" );
    if( ct == null )
        return false;

    var champID = parseInt( ct, 10 ) || 0;
    if( champID <= 0 )
        return false;

    if( pSock._champToggleAll != 1 )
    {
        if( champID != pSock._champToggleID )
            return false;
    }

    if( pSock._champToggleEnable == 1 )
    {
        if( EnableOneChampionAltar( item, pSock ))
            pSock._champToggleCount++;
        else
            pSock._champToggleSkipped++;
    }
    else
    {
        if( DisableOneChampionAltar( item, pSock ))
            pSock._champToggleCount++;
        else
            pSock._champToggleSkipped++;
    }

    return true;
}

// Mirrors champion_spawn_alter.js gump button 2
function EnableOneChampionAltar( altar, socket )
{
    if( !ValidateObject( altar ))
        return false;

    if( altar.GetTag( "spawnActive" ))
        return false;

    var type = altar.GetTag( "championType" ) || 0;
    if( parseInt( type, 10 ) == 0 )
    {
        socket.SysMessage( "Cannot enable: championType is not set on altar at " + altar.x + "," + altar.y + "." );
		Console.Log( "Cannot enable: championType is not set on altar at " + altar.x + "," + altar.y + "." )
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

// Mirrors champion_spawn_alter.js gump button 3
function DisableOneChampionAltar( altar, socket )
{
    if( !ValidateObject( altar ))
        return false;

    if( altar.GetTag( "spawnActive" ) != 1 )
        return false;

    altar.SetTag( "spawnActive", 0 );
    altar.SetTag( "killCount", 0 );
    altar.SetTag( "whiteSkullCount", 0 );
    altar.SetTag( "redSkullCount", 0 );
    altar.SetTag( "spawnStage", 1 );

    altar.KillJSTimer( 1, 7500 );
    altar.KillJSTimer( 10, 7500 );

    TriggerEvent( 7500, "RemoveRedSkulls", altar );
    TriggerEvent( 7500, "RemoveWhiteSkulls", altar );

	socket.tempObj = altar;
	// Scan around the altar, not the player
	AreaCharacterFunction( "RemoveSpawn", altar, 80, socket );

    altar.Refresh();
    return true;
}

// ------------------------------------------------------------
// champremove (marker-driven): remove controller + platform, marker last
// ------------------------------------------------------------
/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_CHAMPREMOVE( socket, cmdString )
{
    var pUser = socket.currentChar;
    if( !ValidateObject( pUser ))
        return;

    cmdString = ("" + cmdString).replace(/^\s+|\s+$/g, "");
    var parts = (cmdString.length > 0) ? cmdString.split(/\s+/) : [];

    if (parts.length < 1)
    {
        socket.SysMessage("Usage: [champremove all] or [champremove <type>]");
        socket.SysMessage("Types: abyss, arachnid, cold, forest, unholy, vermin, habitat");
        return;
    }

    var arg = NormalizeTypeString( parts[0] );
    var all = ( arg == "all" );

    if( !all && !ChampionNameToID.hasOwnProperty( arg ))
    {
        socket.SysMessage( "Unknown type: " + parts[0] );
        socket.SysMessage( "Types: abyss, arachnid, cold, forest, unholy, vermin, habitat or all" );
        return;
    }

    socket._champRemoveAll = all ? 1 : 0;
    socket._champRemoveTypeStr = arg; // string filter for marker/platform
    socket._champRemoveCount = 0;
    socket._champRemoveSkipped = 0;
	socket._champRemoveNoMarker = 0;
	socket._champRemoveNoAltar = 0;
	socket._champRemoveFound = 0;
	socket._champRemoveRemoved = 0;

    var altarData = GetAltarDataList();
    var MARKER_ID = 0x1F14;

    var worldNum = pUser.worldnumber;
    var instID   = pUser.instanceID;

    for( var i = 0; i < altarData.length; ++i )
    {
        var d = altarData[i];

        if( !all )
        {
            if( NormalizeTypeString( d.type ) != arg )
                continue;
        }

		var marker = FindItem(d.x, d.y, d.z, worldNum, MARKER_ID, instID);
		if (ValidateObject(marker) && marker.GetTag("ChampAltarMarker") == 1)
		{
			socket._champRemoveFound++;

			if (ChampRemove_DoAtMarker(marker, socket))
				socket._champRemoveRemoved++;
		}
		else
		{
			socket._champRemoveNoMarker++;
		}
    }

	socket.SysMessage( "Removed champ altars: " + socket._champRemoveRemoved + " | FoundMarkers: " + socket._champRemoveFound +
	" | NoMarker: " + socket._champRemoveNoMarker + " | NoAltarFound: " + socket._champRemoveNoAltar );
}

function ChampRemove_DoAtMarker( marker, pSock )
{
    if( !ValidateObject( marker ) || pSock == null )
        return;

    if( marker.id != 0x1F14 || marker.GetTag( "ChampAltarMarker" ) != 1 )
        return;

    var t = NormalizeTypeString( marker.GetTag( "ChampAltarType" ));
    if( pSock._champRemoveAll != 1 && t != pSock._champRemoveTypeStr )
        return;

    // Ensure anchor is valid before scans
    marker.Refresh();
    if( !ValidateObject( marker ))
        return;

    // Count how many controller altars we actually touched
    pSock._champRemoveFoundControllers = 0;

	// 1) Platform first (uses marker anchor while it is still stable)
	AreaItemFunction( "ChampRemovePlatformAround", marker, 12, pSock );

    AreaItemFunction( "ChampRemove_CleanupControllersAround", marker, 12, pSock );

    AreaItemFunction( "ChampRemove_DeleteControllersAround", marker, 12, pSock );

    SafeDeleteItem( marker );

    // If no altar controller was found, track it (marker/platform removed, but no altar item existed)
    if( pSock._champRemoveFoundControllers <= 0 )
        pSock._champRemoveNoAltar++;

    pSock._champRemoveCount++;
}

function ChampRemove_CleanupControllersAround( src, item, pSock )
{
    if( !ValidateObject( item ))
        return false;

    if( item.GetTag( "championType" ) == null )
        return false;

    // Count controller found for this marker
    pSock._champRemoveFoundControllers = ( pSock._champRemoveFoundControllers || 0 ) + 1;

    ChampRemove_CleanupAltar( item, pSock );
    return true;
}

function ChampRemove_DeleteControllersAround( src, item, pSock )
{
    if( !ValidateObject( item ))
        return false;

    if( item.GetTag( "championType" ) == null )
        return false;

    SafeDeleteItem( item );
    return true;
}

function ChampRemovePlatformAround( src, item, pSock )
{
    if( !ValidateObject( item ))
        return false;

    if( item.GetTag( "ChampAltarPlatform" ) != 1 )
        return false;

    var t = NormalizeTypeString( item.GetTag( "ChampAltarType" ));
    if( pSock._champRemoveAll != 1 && t != pSock._champRemoveTypeStr )
        return false;

    SafeDeleteItem( item );
    return true;
}

function ChampRemove_CleanupAltar( altar, socket )
{
    if( !ValidateObject( altar ))
        return;

    altar.SetTag( "spawnActive", 0 );
    altar.SetTag( "killCount", 0 );
    altar.SetTag( "whiteSkullCount", 0 );
    altar.SetTag( "redSkullCount", 0 );
    altar.SetTag( "spawnStage", 1 );

    altar.KillJSTimer( 1, 7500 );
    altar.KillJSTimer( 10, 7500 );

    TriggerEvent( 7500, "RemoveRedSkulls", altar );
    TriggerEvent( 7500, "RemoveWhiteSkulls", altar );

	socket.tempObj = altar;

	// Scan around the altar, not the player
	AreaCharacterFunction( "RemoveSpawn", altar, 80, socket );

    altar.Refresh();
}

function SafeDeleteItem( item )
{
    if( !ValidateObject( item ))
        return;

    if( typeof item.Delete == "function" )
    {
        item.Delete();
        return;
    }
}

function RemoveSpawn( srcChar, trgChar, pSock )
{
    if( !ValidateObject( trgChar ) || pSock == null )
        return true;

    var altar = pSock.tempObj;
    if( !ValidateObject( altar ))
        return true;

    if( trgChar.GetTag( "championSpawnID" ) == altar.serial )
    {
        trgChar.Delete();
    }
    return true;
}

// Title-case list for display, and numeric IDs matching your champion system
var ChampMenuTypes = [
	{ name: "Abyss",    id: 1 },
	{ name: "Arachnid", id: 2 },
	{ name: "Cold",     id: 3 },
	{ name: "Forest",   id: 4 },
	{ name: "Unholy",   id: 5 },
	{ name: "Vermin",   id: 6 },
	{ name: "Habitat",   id: 7 }
];

// command handler
/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_CHAMPMENU( socket, cmdString )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( socket._champMenuTypeID == null )
		socket._champMenuTypeID = 5; // Unholy default

	socket._champMenuX = pUser.x;
	socket._champMenuY = pUser.y;
	socket._champMenuZ = pUser.z;
	socket._champMenuWorld = pUser.worldnumber;
	socket._champMenuInst = pUser.instanceID;

	ChampMenu_Open( socket );
}

function ChampMenu_Open( socket )
{
	var pUser = socket.currentChar;
	var champMenu = new Gump;

	champMenu.AddPage( 0 );
	champMenu.AddBackground( 0, 0, 360, 240, 5054 );
	champMenu.AddCheckerTrans( 0, 0, 360, 240 );

	champMenu.AddHTMLGump( 15, 10, 330, 20, 0, 0, "<basefont color=#EECD8B><center><big>Champion Spawn Setup</big></center></basefont>" );
	champMenu.AddButton( 330, 8, 4017, 4018, 1, 0, 0 ); // close

	// Location display
	var locStr = "X=" + socket._champMenuX + " Y=" + socket._champMenuY + " Z=" + socket._champMenuZ;
	champMenu.AddHTMLGump( 20, 40, 320, 18, false, false, "<basefont color=#ffffff>Location: " + locStr + "</basefont>" );

	// Set Location button
	champMenu.AddButton( 20, 62, 4005, 4007, 1, 0, 2 );
	champMenu.AddHTMLGump( 55, 64, 200, 18, false, false, "<basefont color=#ffffff>Set Location (target)</basefont>" );

	// Type header
	champMenu.AddHTMLGump( 20, 95, 320, 18, false, false, "<basefont color=#ffffff>Champion Type:</basefont>" );

	// Type radios
	champMenu.AddGroup( 9 );

	var startX = 20;
	var startY = 118;
	var colW = 110;

	for( var i = 0; i < ChampMenuTypes.length; ++i )
	{
		var t = ChampMenuTypes[i];
		var col = ( i % 3 );
		var row = Math.floor( i / 3 );

		var x = startX + ( col * colW );
		var y = startY + ( row * 24 );

		var selected = ( socket._champMenuTypeID == t.id ) ? 1 : 0;

		champMenu.AddRadio( x, y, 2472, 2153, selected, t.id );
		champMenu.AddHTMLGump( x + 30, y, 90, 18, false, false, "<basefont color=#00ff00>" + t.name + "</basefont>" );
	}

	champMenu.EndGroup();

	// Create + Enable
	champMenu.AddButton( 20, 190, 2122, 2124, 1, 0, 3 );
	champMenu.AddHTMLGump( 80, 192, 220, 18, false, false, "<basefont color=#ffffff>Create + Enable</basefont>" );

	champMenu.AddButton( 190, 62, 4005, 4007, 1, 0, 4 );
	champMenu.AddHTMLGump( 220, 64, 220, 18, false, false, "<basefont color=#ffffff>X:</basefont>" );
	champMenu.AddHTMLGump( 270, 64, 220, 18, false, false, "<basefont color=#ffffff>Y:</basefont>" );
	champMenu.AddHTMLGump( 310, 64, 220, 18, false, false, "<basefont color=#ffffff>Z:</basefont>" );
	champMenu.AddTextEntryLimited( 233, 64, 120, 25, 10, 0, 14, pUser.x.toString(), 4 ); //TextID 11
	champMenu.AddTextEntryLimited( 283, 64, 120, 25, 10, 0, 15, pUser.y.toString(), 4 ); //TextID 12
	champMenu.AddTextEntryLimited( 323, 64, 120, 25, 10, 0, 16, pUser.z.toString(), 2 ); //TextID 13

	champMenu.Send( socket );
	champMenu.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( socket, buttonID, gumpData )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( buttonID == 0 )
		return;

	var selectedTypeID = parseInt( gumpData.getButton( 0 ));
	if( selectedTypeID > 0 )
		socket._champMenuTypeID = selectedTypeID;

	switch( buttonID )
	{
		case 2:
		{
			socket.CustomTarget( 0, "Target the location for the champion altar." );
			break;
		}
		case 3:
		{
			ChampMenu_CreateAndEnable( socket );
			break;
		}
		case 4:
			var TextX = parseInt( gumpData.getEdit(0), 10 );
			var TextY = parseInt( gumpData.getEdit(1), 10 );
			var TextZ = parseInt( gumpData.getEdit(2), 10 );

			if( isNaN( TextX ) || isNaN( TextY ) || isNaN( TextZ ))
			{
				socket.SysMessage( "Invalid coordinates. Please enter numeric values only." );
				Console.Log( "Invalid coordinates entered in champ menu by " + pUser.name );
				ChampMenu_Open( socket );
				return;
			}
			socket.SysMessage( "You set the Champion Alter Spawn at X:" + TextX + " Y:" + TextY + " Z:" + TextZ);
			socket._champMenuX = TextX;
			socket._champMenuY = TextY;
			socket._champMenuZ = TextZ;
			socket._champMenuWorld = pUser.worldnumber;
			socket._champMenuInst  = pUser.instanceID;
			ChampMenu_Open( socket );
			break;
		default:
			break;
	}
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var targZ = socket.GetSByte( 16 );

	if( targX <= 0 || targY <= 0 )
	{
		socket.SysMessage( "No valid location selected." );
		Console.Log( "No valid location selected in champ menu by " + pUser.name );
		ChampMenu_Open( socket );
		return;
	}

	socket._champMenuX = targX;
	socket._champMenuY = targY;
	socket._champMenuZ = targZ;

	socket._champMenuWorld = pUser.worldnumber;
	socket._champMenuInst  = pUser.instanceID;

	ChampMenu_Open( socket );
}

function ChampMenu_CreateAndEnable( socket )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var x = socket._champMenuX;
	var y = socket._champMenuY;
	var z = socket._champMenuZ;
	var worldNum = socket._champMenuWorld;
	var instID = socket._champMenuInst;

	var champID = parseInt( socket._champMenuTypeID, 10 ) || 0;
	if( champID <= 0 )
	{
		socket.SysMessage( "Invalid champion type selected." );
		Console.Log( "Invalid champion type selected in champ menu by " + pUser.name );
		return;
	}

	// Resolve typeName
	var typeName = "Unknown";
	for( var i = 0; i < ChampMenuTypes.length; ++i )
	{
		if( ChampMenuTypes[i].id == champID )
		{
			typeName = ChampMenuTypes[i].name;
			break;
		}
	}

	CreateHouse( 1000, x, y, z, worldNum, instID, 0, false );

	var cx = x + 0;
	var cy = y + 0;
	var cz = z + 15;

	var ctrl = FindItem( cx, cy, cz, worldNum, 0x0BD1, instID );

	if( !ValidateObject( ctrl ))
	{
		// store pending data and retry shortly
		socket._champMenuPending = 1;
		socket._champMenuPendingCX = cx;
		socket._champMenuPendingCY = cy;
		socket._champMenuPendingCZ = cz;
		socket._champMenuPendingTypeName = typeName;

		pUser.StartTimer( 1000, 9102, true );
		socket.SysMessage( "Placed altar. Finalizing controller setup..." );
		return;
	}

	ChampMenu_ConfigureController( socket, ctrl, champID, typeName );
	socket.SysMessage( "Champion altar created and enabled: " + typeName + " at " + x + "," + y + "," + z + "." );
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( pUser, timerID )
{
	if( timerID != 9102 )
		return;

	if( !ValidateObject( pUser ) || pUser.socket == null )
		return;

	var socket = pUser.socket;

	if( socket._champMenuPending != 1 )
		return;

	socket._champMenuPending = 0;

	var worldNum = socket._champMenuWorld;
	var instID = socket._champMenuInst;

	var cx = socket._champMenuPendingCX;
	var cy = socket._champMenuPendingCY;
	var cz = socket._champMenuPendingCZ;

	var typeName = socket._champMenuPendingTypeName;
	var champID = parseInt( socket._champMenuTypeID, 10 ) || 0;

	var ctrl = FindItem( cx, cy, cz, worldNum, 0x0BD1, instID );

	if( !ValidateObject( ctrl ))
	{
		socket.SysMessage( "Could not locate controller sign at " + cx + "," + cy + "," + cz + " (id 0x0BD1)." );
		socket.SysMessage( "Check DFN offsets or controller item id." );
		return;
	}

	ChampMenu_ConfigureController( socket, ctrl, champID, typeName );

	socket.SysMessage( "Champion altar created and enabled: " + typeName + " at " +
	socket._champMenuX + "," + socket._champMenuY + "," + socket._champMenuZ + "." );
}

function ChampMenu_ConfigureController( socket, ctrl, champID, typeName )
{
	if( !ValidateObject( ctrl ))
		return;

	ctrl.SetTag( "ChampAltarMarker", 1 );
	ctrl.SetTag( "ChampAltarType", typeName );
	ctrl.SetTag( "championType", champID );

	ctrl.movable = 2;
	ctrl.decayable = false;
	if( ctrl.visible != null ) ctrl.visible = 3;

	ctrl.AddScriptTrigger( 7500 );

	socket._tagPlatformTypeStr = ( "" + typeName ).toLowerCase();
	AreaItemFunction( "TagChampPlatformAround", ctrl, 30, socket );

	ChampMenu_EnableController( ctrl );
}

function ChampMenu_EnableController( altarSign )
{
	if( !ValidateObject( altarSign ))
		return;

	// Core state
	altarSign.SetTag( "spawnActive", 1 );
	altarSign.SetTag( "killCount", 0 );
	altarSign.SetTag( "spawnStage", 1 );
	altarSign.SetTag( "redSkullCount", 1 );
	altarSign.SetTag( "whiteSkullCount", 0 );

	altarSign.StartTimer( 30000, 1, 7500 );     // wave/spawn tick
	altarSign.StartTimer( 600000, 10, 7500 );   // long tick / decay / progress

	// Update skull visuals (if your altar script supports these events)
	TriggerEvent( 7500, "PlaceRedSkulls", altarSign, 1 );
	TriggerEvent( 7500, "PlaceWhiteSkulls", altarSign, 0, 1 );
	TriggerEvent( 7500, "StartChampionWave", altarSign, 1 );
}
