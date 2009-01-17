// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// Updated by Xuri
// untill stablekeepers are changed to sell living animals these scripts create NPCs from sold statics
// ostard

function onUseChecked( pUser, iUsed ) 
{ 
	// randomize the ostard given
	var look = RollDice( 1, 3, 0 ); 
	if( look == 1 ) 
	{
		var nSpawned = SpawnNPC( "forestostard", pUser.x, pUser.y, pUser.z, pUser.worldnumber );
	}
	if( look == 2 ) 
	{
		var nSpawned = SpawnNPC( "desertostard", pUser.x, pUser.y, pUser.z, pUser.worldnumber );
	}
	if( look == 3 ) 
	{
		var nSpawned = SpawnNPC( "frenziedostard", pUser.x, pUser.y, pUser.z, pUser.worldnumber );
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

