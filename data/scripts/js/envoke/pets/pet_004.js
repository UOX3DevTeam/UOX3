// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// untill stablekeepers are changed to sell living animals these scripts create NPCs from sold statics
// pack horse

function onUse( pUser, iUsed ) 
{ 
	// get users socket
	var srcSock = CalcSockFromChar( pUser );

    // remove the static
    iUsed.Delete();
    
    // make a sound
    DoSoundEffect( pUser, 0, 0x0215, true );
    
    // create the NPC at Users coords
    var userX = pUser.x;
    var userY = pUser.y;
    var userZ = pUser.z;
    //!!!! needs the new NPCnumbers
    nSpawned = SpawnNPC( pUser, 60, 0, userX, userY, userZ );

    // set owner to the envoker
    SetOwner( nSpawned, pUser, 0 );
    
}

