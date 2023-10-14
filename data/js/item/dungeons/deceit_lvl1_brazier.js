// Brazier spawns lich

function onUseChecked( pUser, iUsed )
{
	var iTime = GetCurrentClock();
	var lastSpawnTime = iUsed.GetTempTag( "lastSpawnTime" );

	if( lastSpawnTime && (( iTime - lastSpawnTime < 900000 ))) // 15 minutes
		return;

	// Play a switch flipping sound
	iUsed.SoundEffect( 0x004a, true );

	// Store a timestamp for when last spawn took place
	iUsed.SetTempTag( "lastSpawnTime", iTime );

	// Spawn a reward lich lord!
	var useNpcList = true;
	var lichLord = SpawnNPC( "randomallmonster", iUsed.x, iUsed.y, iUsed.z, iUsed.worldnumber, iUsed.instanceID, useNpcList );
}
