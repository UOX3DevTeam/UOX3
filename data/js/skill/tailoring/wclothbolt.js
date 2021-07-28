// tailoring script
// Last Updated: July 24th 2021
// five balls of yarn : loom : bolt of cloth

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
			srcSock.CustomTarget( 0, GetDictionaryEntry( 452, srcSock.language ) ); // Select loom to make your cloth.
		}
		else
			srcSock.SysMessage( GetDictionaryEntry( 775, srcSock.language ) ); // You can't use material outside your backpack.
	}
	return false;
}

function onCallback0( tSock, myTarget )
{
	var iUsed = tSock.tempObj;
	var pUser = tSock.currentChar;

	var StrangeByte   = tSock.GetWord( 1 );
	var tileID	= tSock.GetWord( 17 );

	if(( myTarget == null ) || ( myTarget.isChar ) || !( tileID >= 0x105f && tileID <= 0x1066 ))
	{ //Target is a Maptile/Character/wrong item
		tSock.SysMessage( GetDictionaryEntry( 6037, tSock.language ) ); // You cannot weave your yarn on that!
		return;
	}
	else
	{
		if( !pUser.InRange( myTarget, 3 ) )
		{
			tSock.SysMessage( GetDictionaryEntry( 393, tSock.language ) ); // That is too far away
			return;
		}
	}
    	var iMakeResource = pUser.ResourceCount( iUsed.id );	// is there enough resources to use up to make it
    	if( iMakeResource < 5 )
  	{
    		tSock.SysMessage( GetDictionaryEntry( 6038, tSock.language ) ); // You don't seem to have enough yarn!
		return;
        }
	if( pUser.CheckSkill( 34, 0, 1000 ) )
	{
		tSock.SysMessage( GetDictionaryEntry( 822, tSock.language ) ); // You have successfully spun your material.
        pUser.UseResource( 5, iUsed.id ); 	// remove five balls of yarn
		pUser.SoundEffect( 0x0190, true );
	    var itemMade = CreateDFNItem( tSock, pUser, "0x175f", 30, "ITEM", true ); // makes folded cloth
		tSock.SysMessage( GetDictionaryEntry( 6039, tSock.language ) ); // You weave some folded cloth.
	}
	else
		tSock.SysMessage( GetDictionaryEntry( 821, tSock.language ) ); // You failed to spin your material.
        return;
}
