function CommandRegistration()
{
    RegisterCommand( "champspawn", 8, true );
    RegisterCommand( "champenable", 8, true );
    RegisterCommand( "champdisable", 8, true );
    RegisterCommand( "champremove", 8, true );
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
        { type: "Abyss",    x: 5814, y: 1350, z: -23 },
        { type: "Arachnid", x: 5190, y: 1605, z: 0  },
        { type: "Forest",   x: 5559, y: 3757, z: 1  }
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
    "vermin": 6
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
        { type: "Abyss",    x: 5814, y: 1350, z: -23 },
        { type: "Arachnid", x: 5190, y: 1605, z: 0  },
        { type: "Forest",   x: 5559, y: 3757, z: 1  }
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

    socket.SysMessage(
        ( enabling ? "Enabled" : "Disabled" ) +
        " champion spawns: " + socket._champToggleCount +
        " (skipped: " + socket._champToggleSkipped + ")"
    );
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
        socket.SysMessage("Types: abyss, arachnid, cold, forest, unholy, vermin");
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