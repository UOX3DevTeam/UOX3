function CommandRegistration()
{
	RegisterCommand( "spawnchamps", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_SPAWNCHAMPS( socket, cmdString )
{
	var pChar = socket.currentChar;
	if( !ValidateObject( pChar ))
		return;

	SetupChampionAltars( pChar );
}

function SetupChampionAltars( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

	var altarData = [
		{ type: "Unholy",   x: 5178, y: 708,  z: 0  },
		{ type: "Vermin",   x: 5557, y: 824,  z: 45 },
		{ type: "Cold",     x: 5258, y: 829,  z: 42 },
		{ type: "Abyss",    x: 5814, y: 1350, z: 23 },
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

		// Already spawned?
		var existingMarker = FindItem( d.x, d.y, d.z, worldNum, MARKER_ID, instID );
		if( ValidateObject( existingMarker ) && existingMarker.GetTag( "ChampAltarMarker" ) == 1 )
		{
			skipped++;
			continue;
		}

		CreateHouse( 1000, d.x, d.y, d.z, worldNum, instID, 0, false );

		// Marker = proof of success
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

		created++;
	}

	socket.SysMessage( "Champion altars setup complete. Created: " + created + ", Skipped: " + skipped + ", Failed: " + failed + "." );

	if( created > 0 && failed == 0 )
		socket.SysMessage( "All champion altars spawned successfully." );
}