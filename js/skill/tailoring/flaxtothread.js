// Tailoring Script
// 15/09/2002 Cav; tbacavalier@bigpond.com
// 21/07/2003 Xuri; Updated/rewrote the script
// For use only with the UOX3 emulator or any other with the authors permission
// use flax, target spinning wheel, get thread

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
			srcSock.CustomTarget( 0, "What do you want to spin the flax on?" );
	}
	else
		pUser.SysMessage( "This has to be in your backpack!" );
}

function onCallback0( tSock, targSerial )
{
	var tItem = CalcTargetedItem( tSock );
	var pUser = tSock.currentChar;
	var StrangeByte   = tSock.GetWord( 1 );
	var targX	= tSock.GetWord( 11 );
	var targY	= tSock.GetWord( 13 );
	var targZ	= tSock.GetByte( 16 );
	var tileID	= tSock.GetWord( 17 );
	if( tileID == 0 || ( StrangeByte == 0 && targSerial.isChar ))
	{ //Target is a Maptile/Character
		pUser.SysMessage("You may only spin flax on a spinning wheel.");
	}
	//Target is a dynamic/static spinningwheel
	if( tileID == 0x1015 || tileID == 0x1019 || tileID == 0x101C || tileID == 0x10A4 )
	{
		if(( pUser.x > targX + 4 ) || ( pUser.x < targX - 4 ) || ( pUser.y > targY + 4 ) || ( pUser.y < targY - 4 ) || ( pUser.z > targZ + 10 ) || ( pUser.z < targZ - 10 ))
		{
			pUser.SysMessage( "You are too far away from the target!" );
			return;
		}
		var iMakeResource1 = pUser.ResourceCount( 0x1A9C );
		if( iMakeResource1 < 1 )
		{
			var iMakeResource2 = pUser.ResourceCount( 0x1A9D );
			if( iMakeResource2 < 1 )
			{
				pUser.SysMessage( "You don't seem to have any flax bundles!" );
				return;
			}
			pUser.UseResource( 1, 0x1a9d );
		}
		else
			pUser.UseResource( 1, 0x1a9c );
		pUser.SoundEffect( 0x021A, true );
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0fa0", false, 6, true, true ); //spin some spools of thread
		pUser.SysMessage( "You spin some spools of thread." );
		return;
	}
}