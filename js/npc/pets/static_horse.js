// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// Updated by Xuri
// untill stablekeepers are changed to sell living animals these scripts create NPCs from sold statics
// horse

function onUseChecked( pUser, iUsed ) 
{ 
	// randomize the horse given
	var look = RollDice( 1, 4, 0 ); 
	if( look == 1 ) 
	{
		var nSpawned = SpawnNPC( "horse", pUser.x, pUser.y, pUser.z, pUser.worldnumber );
	}
	if( look == 2 ) 
	{
		var nSpawned = SpawnNPC( "brownhorse", pUser.x, pUser.y, pUser.z, pUser.worldnumber );
	}
	if( look == 3 ) 
	{
		var nSpawned = SpawnNPC( "grayhorse", pUser.x, pUser.y, pUser.z, pUser.worldnumber );
	}
	if( look == 4 ) 
	{
		var nSpawned = SpawnNPC( "darkhorse", pUser.x, pUser.y, pUser.z, pUser.worldnumber );
	}
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

