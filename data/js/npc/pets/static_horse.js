// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// untill stablekeepers are changed to sell living animals these scripts create NPCs from sold statics
// horse

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
    nSpawned = SpawnNPC( pUser, 7, 0, userX, userY, userZ );

    // randomize the horses look
   	var look = RollDice( 1, 4, 0 ); 
    if( look == 1 ) 
	{
        nSpawned.id = 0x00CC;
    }
    if( look == 2 ) 
	{
        nSpawned.id = 0x00E4;
    }
    if( look == 3 ) 
	{
        nSpawned.id = 0x00C8;
    }
    if( look == 4 ) 
	{
        nSpawned.id = 0x00E2;
    }

    // set owner to the envoker
    SetOwner( nSpawned, pUser, 0 );
    
}

