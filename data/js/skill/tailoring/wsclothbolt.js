// tailoring script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated the script
// five spools of thread : loom : bolt of cloth

function onUse ( pUser, iUsed ) 
{
	// get users socket
	var srcSock = pUser.socket;

	// is it in users pack?
	if( iUsed.container != null )
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner.serial != pUser.serial )
		{
			pUser.SysMessage( "This has to be in your backpack!" );
			return;
		}
		else
			// let the user target the heat source
			srcSock.CustomTarget( 0, "What do you want to weave the thread on?" );
	}
	else
		pUser.SysMessage( "This has to be in your backpack!" );
}

function onCallback0( tSock, targSerial )
{
	var pUser = tSock.currentChar;
	var StrangeByte   = tSock.GetWord( 1 );
	var targX	= tSock.GetWord( 11 );
	var targY	= tSock.GetWord( 13 );
	var targZ	= tSock.GetByte( 16 );
	var tileID	= tSock.GetWord( 17 );
	if( tileID == 0 || ( StrangeByte == 0 && targSerial.isChar ))
	{ //Target is a Maptile/Character
		pUser.SysMessage("You cannot weave your thread on that!");
	}
	if( tileID >= 0x105f && tileID <= 0x1066 )
        // check if its in range
	if(( pUser.x > targX + 3 ) || ( pUser.x < targX - 3 ) || ( pUser.y > targY + 3 ) || ( pUser.y < targY - 3 ) || ( pUser.z > targZ + 10 ) || ( pUser.z < targZ - 10 ))
	{
		pUser.SysMessage( "You are too far away from the target!" );
		return;
	}	
        // remove five spools of thread
    	var iMakeResource1 = pUser.ResourceCount( 0x0fA0 );	// is there enough resources to use up to make it
    	var iMakeResource2 = pUser.ResourceCount( 0x0fA1 );	// is there enough resources to use up to make it
    	if( iMakeResource1 < 5 )
    	{
    		if( iMakeResource2 < 5 )
    		{
		    SysMessage( pUser, "You dont seem to have enough thread!" );
  			return;
	    	}
	    	else
	        	pUser.UseResource( 5, 0x0fa1 );
        }
        else
        	pUser.UseResource( 5, 0x0fa0 );
	pUser.SoundEffect( 0x0190, true );        
        // add bolt of cloth
	var itemMade = CreateDFNItem( pUser.socket, pUser, "0x175f", 30, "ITEM", true ); // makes folded cloth
        pUser.SysMessage( "You weave some folded cloth." );
        return;
}
