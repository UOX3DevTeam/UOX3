// tailoring script
// Last Updated: October 31st 2021
// bale of cotton : spinning wheel : six spools of thread

function onUseChecked ( pUser, iUsed )
{
	// get users socket
	var pSocket = pUser.socket;

	if( pSocket && iUsed && iUsed.isItem )
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner && pUser.serial == iPackOwner.serial )
		{
			pSocket.tempObj = iUsed;
			pSocket.CustomTarget( 0, GetDictionaryEntry( 449, pSocket.language )); // Select spinning wheel to spin cotton.
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 775, pSocket.language )); // You can't use material outside your backpack.
		}
	}
	return false;
}

function onCallback0( pSocket, myTarget )
{
	if( pSocket == null )
		return;

	var iUsed 		= pSocket.tempObj;
	var pUser 		= pSocket.currentChar;
	var tileID		= pSocket.GetWord( 17 );

	if( !ValidateObject( myTarget ))
	{
		// Map/statictile targeted
		if( !( tileID == 0x1015 || tileID == 0x1019 || tileID == 0x101C || tileID == 0x10a4 )) //only allow first IDs in the spinning wheel anims
		{
			pSocket.SysMessage( GetDictionaryEntry( 6040, pSocket.language )); // You can only spin cotton on a spinning wheel.
			return;
		}

		// if too far away from the static loom, disallow
		var targX = socket.GetWord( 11 );
		var targY = socket.GetWord( 13 );
		var targZ = socket.GetSByte( 16 );

		if( Math.abs( pUser.x - targX ) > 3 || Math.abs( pUser.y - targY ) > 3 || Math.abs( pUser.z - targZ ) > 20 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away
			return;
		}
	}
	else if( ValidateObject( myTarget ))
	{
		// Get tileID from myTarget, in case this function was called from the houseAddonUse.js script
		tileID = myTarget.id;

		if( !myTarget.isItem || !( tileID == 0x1015 || tileID == 0x1019 || tileID == 0x101C || tileID == 0x10a4 )) //only allow first IDs in the spinning wheel anims
		{
			if( tileID == 0x1015 + 1 || tileID == 0x1019 + 1 || tileID == 0x101c + 1 || tileID == 0x10a4 + 1 )
			{
				pSocket.SysMessage( GetDictionaryEntry( 6026, pSocket.language )); // That is already in use.
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 6040, pSocket.language )); // You can only spin cotton on a spinning wheel.
			}
			return;
		}

		if( !pUser.InRange( myTarget, 3 ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away
			return;
		}
	}

	var iMakeResource = pUser.ResourceCount( iUsed.id );	// is there enough resources to use up to make it
	if( iMakeResource < 1 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6041, pSocket.language )); // You don't seem to have any cotton!
		return;
	}

	if( pUser.CheckSkill( 34, 0, pUser.skillCaps.tailoring ))
	{
		pUser.UseResource( 1, iUsed.id ); 	// remove cotton
		pUser.SoundEffect( 0x021b, true );
		myTarget.id++;
		myTarget.StartTimer( 2000, 0, 132 );
		var itemMade = CreateDFNItem( pSocket, pUser, "0x0fa0", 6, "ITEM", true );  // makes some spools of thread
		pSocket.SysMessage( GetDictionaryEntry( 6042, pSocket.language )); // You spin some spools of thread, and put them in your backpack.
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 821, pSocket.language )); // You failed to spin your material.
	}
}

function onTimer( spinWheel, timerID )
{
	if( timerID == 0 )
	{
		spinWheel.id = spinWheel.id - 1;
	}
}