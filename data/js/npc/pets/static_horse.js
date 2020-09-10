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
	// randomize the horse given
	var look = RollDice( 1, 4, 0 );
	if( look == 1 )
		var nSpawned = SpawnNPC( "horse", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	else if( look == 2 )
		var nSpawned = SpawnNPC( "brownhorse", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	else if( look == 3 )
		var nSpawned = SpawnNPC( "grayhorse", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	else if( look == 4 )
		var nSpawned = SpawnNPC( "darkhorse", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
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
