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
	"731,1452": [5656, 1795],

	// Inside Solen Hive Teleporters
	//Area A
	//B leads to level 2
	"5757,1794": [5725, 1894],
	"5755,1795": [5725, 1894],
	"5756,1794": [5725, 1894],
	"5757,1796": [5725, 1894],
	"5757,1795": [5725, 1894],
	"5758,1795": [5725, 1894],

	//B leads back level 1
	"5727,1894": [5757, 1798],
	"5728,1896": [5757, 1798],
	"5728,1895": [5757, 1798],
	"5728,1894": [5757, 1798],

	//C leads to center area
	"5657,1898": [5750, 1894],
	"5658,1898": [5750, 1894],
	"5658,1897": [5750, 1894],
	"5659,1898": [5750, 1894],
	"5657,1899": [5750, 1894],
	"5658,1899": [5750, 1894],
	"5659,1899": [5750, 1894],
	"5658,1900": [5750, 1894],
	"5659,1900": [5750, 1894],
	"5660,1899": [5750, 1894],

	//C leads back to area A
	"5749,1895": [5757, 1798],
	"5748,1895": [5757, 1798],
	"5747,1895": [5757, 1798],
	"5746,1895": [5757, 1798],
	"5749,1896": [5757, 1798],
	"5748,1896": [5757, 1798],
	"5747,1896": [5757, 1798],
	"5746,1896": [5757, 1798],
	"5747,1897": [5757, 1798],

	//Area B
	//B leads to level 2
	"5898,1877": [5875, 1867],
	"5897,1877": [5875, 1867],
	"5898,1878": [5875, 1867],
	"5898,1879": [5875, 1867],

	//B leads back to level 1
	"5871,1867": [5899, 1875],
	"5871,1868": [5899, 1875],
	"5872,1868": [5899, 1875],
	"5873,1868": [5899, 1875],

	//D leads to center area
	"5852,1850": [5771, 1871],
	"5852,1849": [5771, 1871],
	"5852,1848": [5771, 1871],
	"5853,1850": [5771, 1871],
	"5853,1849": [5771, 1871],
	"5853,1848": [5771, 1871],

	//D leads back to area B
	"5771,1868": [5899, 1875],
	"5772,1868": [5899, 1875],
	"5773,1868": [5899, 1875],
	"5771,1867": [5899, 1875],
	"5772,1867": [5899, 1875],
	"5773,1867": [5899, 1875],

	//Area C
	//B leads to level 2
	"5657,1955": [5709, 1953],
	"5658,1955": [5709, 1953],
	"5659,1955": [5709, 1953],
	"5657,1956": [5709, 1953],
	"5658,1956": [5709, 1953],
	"5659,1956": [5709, 1953],

	//B leads back to level 1
	"5711,1952": [5657, 1957],
	"5712,1952": [5657, 1957],
	"5713,1952": [5657, 1957],
	"5711,1953": [5657, 1957],
	"5712,1953": [5657, 1957],
	"5713,1953": [5657, 1957],
	"5711,1954": [5657, 1957],
	"5712,1954": [5657, 1957],
	"5713,1954": [5657, 1957],

	//E leads to center area
	"5700,1929": [5784, 1927],
	"5701,1929": [5784, 1927],
	"5702,1929": [5784, 1927],
	"5700,1930": [5784, 1927],
	"5701,1930": [5784, 1927],
	"5702,1930": [5784, 1927],
	"5700,1931": [5784, 1927],
	"5701,1931": [5784, 1927],
	"5702,1931": [5784, 1927],

	//E leads back to area C
	"5784,1929": [5657, 1957],
	"5785,1929": [5657, 1957],
	"5786,1929": [5657, 1957],
	"5784,1930": [5657, 1957],
	"5785,1930": [5657, 1957],
	"5786,1930": [5657, 1957],
	"5784,1931": [5657, 1957],
	"5785,1931": [5657, 1957],
	"5786,1931": [5657, 1957],

	//Area D
	//B leads to level 2
	"5913,1893": [5815, 2013],
	"5914,1893": [5815, 2013],
	"5915,1893": [5815, 2013],
	"5913,1894": [5815, 2013],
	"5914,1894": [5815, 2013],
	"5915,1894": [5815, 2013],
	"5913,1895": [5815, 2013],
	"5914,1895": [5815, 2013],
	"5915,1895": [5815, 2013],

	//B leads back to level 1
	"5814,2015": [5914, 1891],
	"5815,2015": [5914, 1891],
	"5816,2015": [5914, 1891],
	"5814,2016": [5914, 1891],
	"5815,2016": [5914, 1891],
	"5816,2016": [5914, 1891],
	"5814,2017": [5914, 1891],
	"5815,2017": [5914, 1891],
	"5816,2017": [5914, 1891],

	//F leads to center area
	"5876,1891": [5808, 1908],
	"5877,1891": [5808, 1908],
	"5878,1891": [5808, 1908],
	"5876,1892": [5808, 1908],
	"5877,1892": [5808, 1908],
	"5878,1892": [5808, 1908],
	"5876,1893": [5808, 1908],
	"5877,1893": [5808, 1908],
	"5878,1893": [5808, 1908],

	//F leads back to area D
	"5809,1905": [5914, 1891],
	"5810,1905": [5914, 1891],
	"5811,1905": [5914, 1891],
	"5809,1906": [5914, 1891],
	"5810,1906": [5914, 1891],
	"5811,1906": [5914, 1891],
	"5809,1907": [5914, 1891],
	"5810,1907": [5914, 1891],
	"5811,1907": [5914, 1891],
	"5809,1908": [5914, 1891],
	"5810,1908": [5914, 1891],
	"5811,1908": [5914, 1891],
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

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( pUser.visible == 1 || pUser.visible == 2 )
		pUser.visible = 0;

	if( socket && iUsed && iUsed.isItem )
	{
		if( !iUsed.InRange( pUser, 3 ))
		{
			pUser.SysMessage( "I can't reach that." );
			return false;
		}
		
		// Use the tile coords as the lookup key
		var key = iUsed.x + "," + iUsed.y;

		// Try entrance map first
		if( teleportFromMap( pUser, key, entranceMap ))
			return true;

		// Then try exit map
		if( teleportFromMap( pUser, key, exitMap ))
			return true;
	}
	return false;
}

function teleportFromMap( pUser, key, map )
{
	var coords = map[key];
	if( coords )
	{
		TeleportWithPets( pUser, coords[0], coords[1], 0 );
		return true;
	}
	return false;
}

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

function onTimer( iObject, timerID )
{
	if( timerID == 1 )
	{
		iObject.SetTag( "Spawned", false );
	}
}

function SpawnAnt( iObject )
{
	var type = ( RandomNumber( 1, 3 ) < 2 ) ? "worker" : "warrior";
	var color = ( iObject.worldnumber == 0 ) ? "black" : "red";

	var ant = SpawnNPC( color + "_solen_" + type, iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
	if( ValidateObject( ant ))
	{
		ant.AddScriptTrigger( 3214 );
	}

	return ant;
}