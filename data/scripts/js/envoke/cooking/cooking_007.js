// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// Raw bird : Heat source : Cooked bird

function onUse ( pUser, iUsed ) 
{
	// get users socket
	var srcSock = CalcSockFromChar( pUser );

	// is it in users pack?
	var iPackOwner = GetPackOwner( iUsed, 0 );
	if( iPackOwner != pUser )
	{
		SysMessage( srcSock, "This has to be in your backpack!" );
		return;
	}

	// let the user target the heat source
	CustomTarget( srcSock, 0, "What do you want to use the raw bird with?" );
}

function onCallback0( tSock, targSerial )
{
	var tItem = CalcTargetedItem( tSock );
	var tChar = CalcCharFromSock( tSock );

	if( tItem == -1 )
	{
		SysMessage( tSock, "You didn't target anything." );
		return;
	}

	// In case its an oven or a fireplace
   	var iID = tItem.id;
	if( iID == 0x0461 || iID == 0x0462 || iID == 0x046b || iID == 0x046f || iID == 0x0475 || iID == 0x047b || iID == 0x092b || iID == 0x092c || iID == 0x0930 || iID == 0x0931 || iID == 0x0937 || iID == 0x0945 || iID == 0x0953 || iID == 0x0961 || iID == 0x0de3 || iID == 0x0fac || iID == 0x0fb1 || iID == 0x0e31 || iID == 0x019bb )
	{
		// check if its in range
		var isInRange = InRange( tChar, tItem, 0, 1, 4 );
		if( !isInRange ) 
		{
			SysMessage( tSock, "You are too far away to reach that!" );
			return;
		}
		// check if its in someone elses house
		var persMulti = FindMulti( tChar.x, tChar.y, tChar.z );
		var itemMulti = FindMulti( tItem.x, tItem.y, tItem.z );

		if( persMulti != itemMulti )	// not in the same house
		{
			SysMessage( tSock, "You cannot reach that from here!" );
			return;
		}
		// remove one raw bird
		var iMakeResource = GetResourceCount( tChar, 0x09b9, 0 );	// is there enough resources to use up to make it
		if( iMakeResource < 1 )
		{
			SysMessage( tSock, "You dont seem to have any raw birds!" );
			return;
		}
		UseResource( tChar, 0x09b9, 0, 1); // uses up a resource (character, item ID, item colour, amount)
		DoSoundEffect( tItem, 1, 0x0021, true);
		// check the skill
		if( GetSkill( srcChar, 13 ) < 200 ) 
		{
			SysMessage( tSock, "You are not skilled enough to do that." );
			return;
		}
		if( !CheckSkill( tChar, 13, 200, 600 ) )	// character to check, skill #, minimum skill, and maximum skill
		{
			SysMessage( tSock, "You burnt the bird to crisp." );
			return;
		}
		// add one cooked bird if skill is ok
		var itemMade = SpawnItem( tSock, tChar, 0x09b8, false );	// makes an item and puts in tChar's pack
		SysMessage( tSock, "You cook a bird. Smells good!" );
		return;
	}

	SysMessage( tSock, "That is not a thing to use raw birds on." );

}
