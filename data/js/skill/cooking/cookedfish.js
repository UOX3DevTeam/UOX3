// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// Raw fish steak  : Heat source : Cooked fish steak

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
	CustomTarget( srcSock, 0, "What do you want to use the raw fish steak with?" );
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
	if( iID == 0x0461 || iID == 0x0462 || iID == 0x046B || iID == 0x046F || iID == 0x0475 || iID == 0x047B || iID == 0x092B || iID == 0x092C || iID == 0x0930 || iID == 0x0931 || iID == 0x0937 || iID == 0x0945 || iID == 0x0953 || iID == 0x0961 || iID == 0x0DE3 || iID == 0x0FAC || iID == 0x0FB1 || iID == 0x0E31 || iID == 0x019BB )
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
		// remove one fish steak
		var iMakeResource = GetResourceCount( tChar, 0x097A, 0 );	// is there enough resources to use up to make it
		if( iMakeResource < 1 )
		{
			SysMessage( tSock, "You dont seem to have any fish steak!" );		
			return;
		}
		UseResource( tChar, 0x097A, 0, 1); // uses up a resource (character, item ID, item colour, amount)
		DoSoundEffect( tItem, 1, 0x0021, true);

		// check the skill
		if( !CheckSkill( tChar, 13, 1, 300 ) )	// character to check, skill #, minimum skill, and maximum skill
		{
			SysMessage( tSock, "You burnt the fish to crisp." );
			return;
		}
		// add one cooked fish steak if skill is ok
		var itemMade = SpawnItem( tSock, tChar, 0x097B, false );	// makes an item and puts in tChar's pack
		SysMessage( tSock, "You cook a fish steak." );
		return;
	}
	SysMessage( tSock, "That is not a thing to use raw fish steaks on." );
}
