// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// Updated by Xuri

function onBoughtFromVendor( pSock, Vendor, iBought )
{
	if( pSock && iBought )
		onUseChecked( pSock.currentChar, iBought );
}

function onUseChecked( pUser, iUsed )
{
	// randomize the ostard given
	var look = RollDice( 1, 3, 0 );
	if( look == 1 )
		var nSpawned = SpawnNPC( "forestostard", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( look == 2 )
		var nSpawned = SpawnNPC( "desertostard", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( look == 3 )
		var nSpawned = SpawnNPC( "frenziedostard", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( nSpawned )
	{
		// set owner to the envoker
		nSpawned.owner = pUser;
		// make pet follow owner by default
		nSpawned.Follow( pUser );
		nSpawned.wandertype = 1;
		// make a sound
		pUser.SoundEffect( 0x0215, true );
		// remove the static
		iUsed.Delete();
	}
	else
		pUser.SysMessage( "Creature failed to spawn, reason unknown." );
	return false;
}

