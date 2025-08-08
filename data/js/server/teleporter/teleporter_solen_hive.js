// ================================
// Solen Hive Teleport & Spawn Script
// ================================

// Entrance coordinates: key = "x,y", value = [destX, destY]
const entranceMap = {
    // Minoc
    "2608,763": [5916, 1793],
    "2609,763": [5921, 1793],
    "2608,764": [5917, 1797],
    "2609,764": [5921, 1797],
    // Trinsic
    "1690,2789": [5653, 2018],
    "1691,2789": [5658, 2017],
    "1690,2790": [5655, 2021],
    "1691,2790": [5658, 2021],
    // Compassion Shrine
    "1724,814": [5919, 2019],
    "1725,814": [5922, 2020],
    "1724,815": [5917, 2023],
    "1725,815": [5922, 2024],
    // Yew Orc Fort
    "730,1451": [5654, 1789],
    "731,1451": [5658, 1791],
    "730,1452": [5652, 1793],
    "731,1452": [5656, 1795]
};

// Exit coordinates: key = "x,y", value = [destX, destY]
const exitMap = {
    // Minoc Exit
    "5918,1794": [2607, 763],
    "5919,1794": [2607, 763],
    "5918,1795": [2607, 763],
    "5919,1795": [2607, 763],
    // Trinsic Exit
    "5655,2018": [1689, 2789],
    "5656,2018": [1689, 2789],
    "5655,2019": [1689, 2789],
    "5656,2019": [1689, 2789],
    // Compassion Exit
    "5919,2021": [1723, 814],
    "5920,2021": [1723, 814],
    "5919,2022": [1723, 814],
    "5920,2022": [1723, 814],
    // Yew Orc Fort Exit
    "5654,1791": [729, 1451],
    "5655,1791": [729, 1451],
    "5654,1792": [729, 1451],
    "5655,1792": [729, 1451]
};

// =====================================================
// Solen Hive Teleport & Spawn Script (UOX3 Format)
// =====================================================

// Entrance coordinates
var entranceMap = {
	// Minoc
	"2608,763": [5916, 1793],
	"2609,763": [5921, 1793],
	"2608,764": [5917, 1797],
	"2609,764": [5921, 1797],
	// Trinsic
	"1690,2789": [5653, 2018],
	"1691,2789": [5658, 2017],
	"1690,2790": [5655, 2021],
	"1691,2790": [5658, 2021],
	// Compassion Shrine
	"1724,814": [5919, 2019],
	"1725,814": [5922, 2020],
	"1724,815": [5917, 2023],
	"1725,815": [5922, 2024],
	// Yew Orc Fort
	"730,1451": [5654, 1789],
	"731,1451": [5658, 1791],
	"730,1452": [5652, 1793],
	"731,1452": [5656, 1795]
};

// Exit coordinates
var exitMap = {
	// Minoc Exit
	"5918,1794": [2607, 763],
	"5919,1794": [2607, 763],
	"5918,1795": [2607, 763],
	"5919,1795": [2607, 763],
	// Trinsic Exit
	"5655,2018": [1689, 2789],
	"5656,2018": [1689, 2789],
	"5655,2019": [1689, 2789],
	"5656,2019": [1689, 2789],
	// Compassion Exit
	"5919,2021": [1723, 814],
	"5920,2021": [1723, 814],
	"5919,2022": [1723, 814],
	"5920,2022": [1723, 814],
	// Yew Orc Fort Exit
	"5654,1791": [729, 1451],
	"5655,1791": [729, 1451],
	"5654,1792": [729, 1451],
	"5655,1792": [729, 1451]
};

// =====================================================
// Main Use Handler
// =====================================================
function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( pUser.visible == 1 || pUser.visible == 2 )
		pUser.visible = 0;

	if( socket && iUsed && iUsed.isItem )
	{
		if( !iUsed.InRange( pUser, 3 ) )
		{
			pUser.SysMessage( "I can't reach that." );
			return false;
		}

		if( iUsed.sectionID == "solenentrancehole" || iUsed.sectionID == "solenentrancehole2" )
		{
			if( !teleportFromMap( pUser, iUsed, entranceMap ) )
			{
				var examine = iUsed.GetTag( "Examined" );
				var rand = RandomNumber( 1, 100 );

				if( rand < 50 && examine == 0 )
				{
					pUser.TextMessage( "* You examine the solen hole, but discover it does not lead anywhere *" );
					iUsed.SetTag( "Examined", 1 );
					return;
				}

				pUser.TextMessage( "* " + pUser.name + " dives into the hole and disappears! *" );
				TeleportWithPets( pUser, 5922, 2024, 0 ); // Random location placeholder
			}
		}
		else if( iUsed.sectionID == "shexitteleporter" )
		{
			teleportFromMap( pUser, iUsed, exitMap );
		}
	}
	return false;
}

// =====================================================
// Helper - Teleport from coordinate map
// =====================================================
function teleportFromMap( pUser, iUsed, map )
{
	var coords = map[iUsed.x + "," + iUsed.y];
	if( coords )
	{
		TeleportWithPets( pUser, coords[0], coords[1], 0 );
		return true;
	}
	return false;
}

// =====================================================
// Unified Teleport with Pets
// =====================================================
function TeleportWithPets( pUser, x, y, z )
{
	var followerList = pUser.GetFollowerList();
	for( var i = 0; i < followerList.length; i++ )
	{
		var tempFollower = followerList[i];
		if( ValidateObject( tempFollower ) && tempFollower.wandertype == 1 && tempFollower.InRange( pUser, 24 ))
		{
			tempFollower.Teleport( x, y, z );
			tempFollower.Follow( pUser );
		}
	}
	pUser.Teleport( x, y, z );
}

// =====================================================
// Collision Handler - Spawn Ants
// =====================================================
function onCollide( pSock, pUser, iObject )
{
	if( !ValidateObject( pUser ) || !pUser.isChar || pUser.npc )
		return false;

	if( iObject.sectionID == "solenentrancehole" || iObject.sectionID == "solenentrancehole2" ||
		iObject.sectionID == "shexitteleporter" || iObject.GetTag( "Examined" ) == 1 )
		return false;

	if( iObject.GetTag( "Spawned" ) == true )
		return false;

	var spawnCount = iObject.GetTag( "spawnCount" );
	if( spawnCount < 5 )
	{
		var count = 1 + RandomNumber( 1, 4 );

		for( var i = 0; i < count && spawnCount < 5; ++i )
		{
			var ant = SpawnAnt( iObject );
			if( ValidateObject( ant ) )
			{
				spawnCount++;
				iObject.SetTag( "spawnCount", spawnCount );
				iObject.SetTag( "Spawned", true );
				iObject.StartTimer( 300000, 1, true ); // 5 min cooldown
				ant.SetTag( "parentSerial", ( iObject.serial ).toString() );
			}
		}
	}
	return true;
}

// =====================================================
// Timer Handler
// =====================================================
function onTimer( iObject, timerID )
{
	if( timerID == 1 )
		iObject.SetTag( "Spawned", false );
}

// =====================================================
// Spawn Ant Helper
// =====================================================
function SpawnAnt( iObject )
{
	var type = ( RandomNumber( 1, 3 ) < 2 ) ? "worker" : "warrior";
	var color = ( iObject.worldnumber == 0 ) ? "black" : "red";

	var ant = SpawnNPC( color + "solen" + type, iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
	if( ValidateObject( ant ) )
		ant.AddScriptTrigger( 3214 );

	return ant;
}