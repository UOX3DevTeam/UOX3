// tailoring script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/Rewrote the script
// unspun wool : spinning wheel : three balls of yarn

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
			srcSock.CustomTarget( 0, "What do you want to spin the wool on?" );
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
		pUser.SysMessage("You cannot spin your wool on that!");
	}
	if( tileID == 0x1015 || tileID == 0x1019 || tileID == 0x101C || tileID == 0x10A4 )
	{ // In case its a spinning wheel
		// check if its in range
		if(( pUser.x > targX + 3 ) || ( pUser.x < targX - 3 ) || ( pUser.y > targY + 3 ) || ( pUser.y < targY - 3 ) || ( pUser.z > targZ + 10 ) || ( pUser.z < targZ - 10 ))
		{
			pUser.SysMessage( "You are too far away from the target!" );
			return;
		}	
		// remove one bale of wool
		var iMakeResource =  pUser.ResourceCount( 0x0DF8 );
		if( iMakeResource < 1 )
		{
			pUser.SysMessage( "You dont seem to have any cotton!" );
			return;
		}
		pUser.UseResource( 1, 0x0DF8 ); // uses up a resource 
		pUser.SoundEffect( 0x021b, true );
		// add spools of thread
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0e1e", 3, "ITEM", true ); //makes some balls of yarn
		pUser.SysMessage(  "You spin some balls of yarn." );
		return;
	}
}
