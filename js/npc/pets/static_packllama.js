// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// Updated by Xuri
// untill stablekeepers are changed to sell living animals these scripts create NPCs from sold statics
// pack llama

function onUse( pUser, iUsed ) 
{ 
	var nSpawned = SpawnNPC( null, "packllama", pUser.x, pUser.y, pUser.z, pUser.worldnumber );
	if( nSpawned == null )
	{
		pUser.SysMessage( "Creature couldn't be created." );
		return;
	}

	// set owner to the envoker
	nSpawned.owner = pUser;   

	// make a sound
	pUser.SoundEffect( 0x0215, true );

	// remove the static
	iUsed.Delete();
}
