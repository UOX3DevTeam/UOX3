function CommandRegistration()
{
	RegisterCommand( "spawnchamps", 8, true );
}

function command_SPAWNCHAMPS( socket, cmdString )
{
	let pChar = socket.currentChar;
	SetupChampionAltars( pChar );
}

function SetupChampionAltars( pUser )
{
	let socket = pUser.socket;


	let altarData = [
		{ type: "Unholy", x: 5178, y: 708, z: 0 },
		{ type: "Vermin", x: 5557, y: 824, z: 45 },
		{ type: "Cold",   x: 5259, y: 837, z: 38 },
		{ type: "Abyss",  x: 5814, y: 1350, z: 23 },
		{ type: "Arachnid", x: 5190, y: 1605, z: 0 },
		{ type: "Forest", x: 5559, y: 3757, z: 1 }
	];

	for( let i = 0; i < altarData.length; ++i )
	{
		let data = altarData[i];
		CreateHouse( 1000, data.x, data.y, data.z,  pUser.worldNumber, pUser.instanceID, 0, false );	
	}
}
