// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// untill stablekeepers are changed to sell living animals these scripts create NPCs from sold statics
// ostard

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
    nSpawned = SpawnNPC( pUser, 427, 0, userX, userY, userZ );

    // randomize the horses look
   	var look = RollDice( 1, 3, 0 ); 
    if( look == 1 ) 
	{
        nSpawned.id = 0x00D2;
    }
    if( look == 2 ) 
	{
        nSpawned.id = 0x00DA;
    }
    if( look == 3 ) 
	{
        nSpawned.id = 0x00DB;
    }

    // set owner to the envoker
    SetOwner( nSpawned, pUser, 0 );
    
}

