// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// use dough : target heat source : get bread

function onUse ( pUser, iUsed ) 
{
	// get users socket
	var srcSock = pUser.socket;

	// is it in users pack?
	var iPackOwner = GetPackOwner( iUsed, 0 );
	if( iPackOwner != pUser )
	{
		srcSock.SysMessage( "This has to be in your backpack!" );
		return;
	}

	// let the user target the heat source
	srcSock.CustomTarget( 0, "What do you want to use the dough with?" );
}

function onCallback0( tSock, targSerial )
{
	var tItem = CalcTargetedItem( tSock );
	var tChar = tSock.currentChar;

	if( tItem == null )
	{
		tSock.SysMessage( "You didn't target anything." );
		return;
	}

	// In case its an oven or a fireplace
   	var iID = tItem.id;
	if( iID == 0x0461 || iID == 0x0462 || iID == 0x046b || iID == 0x046f || iID == 0x0475 || iID == 0x047B || iID == 0x092B || iID == 0x092C || iID == 0x0930 || iID == 0x0931 || iID == 0x0937 || iID == 0x0945 || iID == 0x0953 || iID == 0x0961)
	{
		// check if its in range
		var isInRange = tChar.InRange( tItem, 4 );
		if( !isInRange ) 
		{
			tSock.SysMessage( "You are too far away to reach that!" );
			return;
		}

		// check if its in someone elses house
		var persMulti = FindMulti( tChar );
		var itemMulti = FindMulti( tItem );

		if( persMulti != itemMulti )	// not in the same house
		{
			tSockSysMessage( "You cannot reach that from here!" );
			return;
		}

		// remove one dough
		var iMakeResource = tChar.ResourceCount( 0x103D );	// is there enough resources to use up to make it
		if( iMakeResource < 1 )
		{
			tSock.SysMessage( "You dont seem to have any dough!" );
			return;
		}

		tChar.UseResource( 1, 0x103D ); // uses up a resource (amount, item ID, item colour)
		tItem.SoundEffect( 0x0054, true );

		// check the skill
		if( srcChar.skills[13] < 200 ) 
		{
			SysMessage( tSock, "You are not skilled enough to do that." );
			return;
		}

		if( !tChar.CheckSkill( 13, 200, 500 ) )	// character to check, skill #, minimum skill, and maximum skill
		{
			tSock.SysMessage( "You burnt the dough to crisp." );
			return;
		}

		// add one bread if skill is ok
		var itemMade = SpawnItem( tSock, tChar, 0x103B, false );	// makes an item and puts in tChar's pack
		tSock.SysMessage( "You bake a loaf of bread." );
		return;
	}

	tSock.SysMessage( "That is not a thing to use dough on." );
}
