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
	var nSpawned = SpawnNPC( "packhorse", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
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
