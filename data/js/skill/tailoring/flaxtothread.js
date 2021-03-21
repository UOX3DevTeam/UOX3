// tailoring script
// Last Updated: January 24th 2005
// flax bundle : spinning wheel : six spools of thread

function onUseChecked ( pUser, iUsed )
{
	// get users socket
	var srcSock = pUser.socket;

	if( srcSock && iUsed && iUsed.isItem )
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner && pUser.serial == iPackOwner.serial )
		{
			srcSock.tempObj = iUsed;
			srcSock.CustomTarget( 0, "What spinning wheel do you want to spin the flax on?" ); // Select spinning wheel to spin flax on
		}
		else
			pUser.SysMessage( GetDictionaryEntry( 775, srcSock.language ) ); //You can't use material outside your backpack.
	}
	return false;
}

function onCallback0( tSock, myTarget )
{
	var iUsed = tSock.tempObj;
	var pUser = tSock.currentChar;
	var StrangeByte   = tSock.GetWord( 1 );
	var tileID	= tSock.GetWord( 17 );

	if( myTarget == null || myTarget.isChar )
	{ //Target is a Maptile/Character
		pUser.SysMessage("You may only spin flax on a spinning wheel.");
		return;
	}
	if( !(tileID == 0x1015 || tileID == 0x1019 || tileID == 0x101C )) //only allow first IDs in the spinning wheel anims
	{
		if( tileID == 0x1015+1 || tileID == 0x1019+1 || tileID == 0x101c+1 )
			pUser.SysMessage("That is already in use.");
		else
			pUser.SysMessage("You may only spin flax on a spinning wheel.");
		return;
	}
	if( !pUser.InRange( myTarget, 3 ) )
	{
		pUser.SysMessage( GetDictionaryEntry( 393, tSock.language ) ); //That is too far away
		return;
	}
    var iMakeResource = pUser.ResourceCount( iUsed.id );	// is there enough resources to use up to make it
    if( iMakeResource < 1 )
    {
		pUser.SysMessage( "You don't seem to have enough flax bundles!" );
  		return;
	}
	if( pUser.CheckSkill( 34, 0, 1000 ) )
	{
		pUser.UseResource( 1, iUsed.id ); 	// remove some flax
		pUser.SoundEffect( 0x021A, true );
		myTarget.id++;
		myTarget.StartTimer( 2000, 1, true );
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0fA0", 1, "ITEM", true ); // makes spools of thread
		pUser.SysMessage( "You spin some spools of thread, and put them in your backpack." );
	}
	else
		pUser.SysMessage( GetDictionaryEntry( 821, tSock.language ) ); //You failed to spin your material.
        return;
}

function onTimer( spinWheel, timerID )
{
	if( timerID == 1 )
	{
		spinWheel.id = spinWheel.id - 1;
	}
}
