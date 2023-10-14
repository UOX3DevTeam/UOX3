// tailoring script
// Last Updated: October 31st 2021
// five balls of yarn/spools of thread : loom : bolt of cloth

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
			pSocket.CustomTarget( 0, GetDictionaryEntry( 452, pSocket.language )); // Select loom to make your cloth.
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
	var tileID 		= pSocket.GetWord( 17 );

	if( !ValidateObject( myTarget ))
	{
		// Map/statictile targeted
		if( tileID < 0x105f || tileD > 0x1066 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 6037, pSocket.language )); // You cannot weave your yarn on that!
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

		if( !myTarget.isItem || tileID < 0x105f || tileID > 0x1066 )
		{
			// Non-item targeted
			pSocket.SysMessage( GetDictionaryEntry( 6037, pSocket.language )); // You cannot weave your yarn on that!
			return;
		}

		if( !pUser.InRange( myTarget, 3 ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away
			return;
		}
	}

    var iMakeResource = pUser.ResourceCount( iUsed.id );	// is there enough resources to use up to make it
    if( iMakeResource < 5 )
  	{
    	pSocket.SysMessage( GetDictionaryEntry( 6038, pSocket.language )); // You don't seem to have enough yarn!
		return;
	}

	if( pUser.CheckSkill( 34, 0, 1000 ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 822, pSocket.language )); // You have successfully spun your material.
        pUser.UseResource( 5, iUsed.id ); 	// remove five balls of yarn/spool of thread
		pUser.SoundEffect( 0x0190, true );
	    var itemMade = CreateDFNItem( pSocket, pUser, "0x175f", 30, "ITEM", true ); // makes folded cloth
		pSocket.SysMessage( GetDictionaryEntry( 6039, pSocket.language )); // You weave some folded cloth.
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 821, pSocket.language )); // You failed to spin your material.
	}
}
