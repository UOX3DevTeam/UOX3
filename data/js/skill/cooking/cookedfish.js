// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/rewrote the script
// Raw fish steak  : Heat source : Cooked fish steak

function onUseChecked ( pUser, iUsed )
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
		}
		else
			// let the user target the heat source
			srcSock.CustomTarget( 0, "What do you want to use the raw fish steak with?" );
	}
	else
		pUser.SysMessage( "This has to be in your backpack!" );

	return false;
}

function onCallback0( tSock, targSerial )
{
	var pUser = tSock.currentChar;
	var StrangeByte   = tSock.GetWord( 1 );
	var targX	= tSock.GetWord( 11 );
	var targY	= tSock.GetWord( 13 );
	var targZ	= tSock.GetSByte( 16 );
	var tileID	= tSock.GetWord( 17 );
	if( tileID == 0 || ( StrangeByte == 0 && targSerial.isChar ))
	{ //Target is a MapTile or a Character
		pUser.SysMessage("You cannot cook your raw fish steak on that.");
		return;
	}
	// Target is a Dynamic or Static Item
	if(( tileID >= 0x0461 && tileID <= 0x0480 ) || ( tileID >= 0x092B && tileID <= 0x0933 ) || ( tileID >= 0x0937 && tileID <= 0x0942 ) ||
	( tileID >= 0x0945 && tileID <= 0x0950 ) || ( tileID >= 0x0953 && tileID <= 0x095e ) || ( tileID >= 0x0961 && tileID <= 0x096c ) ||
	( tileID >= 0x0de3 && tileID <= 0x0de8 ) || tileID == 0x0fac )
	{	// In case its an oven, fireplace, campfire or fire pit
		// check if its in range
		if(( pUser.x > targX + 3 ) || ( pUser.x < targX - 3 ) || ( pUser.y > targY + 3 ) || ( pUser.y < targY - 3 ) || ( pUser.z > targZ + 10 ) || ( pUser.z < targZ - 10 ))
		{
			pUser.SysMessage( "You are too far away from the target!" );
			return;
		}
		// remove one raw fish steak
		var iMakeResource = pUser.ResourceCount( 0x097A );	// is there enough resources to use up to make it
		if( iMakeResource < 1 )
		{
			pUser.SysMessage( "You don't seem to have any fish steaks!" );
			return;
		}
		pUser.UseResource( 1, 0x097A ); // uses up a resource (amount, item ID, item colour)
		pUser.SoundEffect( 0x0021, true );
		// check the skill
		if( !pUser.CheckSkill( 13, 0, 300 ) )	// character to check, skill #, minimum skill, and maximum skill
		{
			pUser.SysMessage( "You burnt the fish to crisp." );
			return;
		}
		var itemMade = CreateDFNItem( pUser.socket, pUser, "0x097B", 1, "ITEM", true ); // makes a fish steak
		pUser.SysMessage( "You cook a fish steak." );
	}
}
