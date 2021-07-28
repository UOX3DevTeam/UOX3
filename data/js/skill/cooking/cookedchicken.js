// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/rewrote the script
// 25/07/2021 Updated to use dictionary messages
// Raw chicken leg : Heat source : Chicken Leg

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
			srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
		}
		else
			// let the user target the heat source
			srcSock.CustomTarget( 0, GetDictionaryEntry( 6054, srcSock.language )); // What do you want to use the raw chicken leg with?
	}
	else
		srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
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
	{ //Target is a Character
		tSock.SysMessage( GetDictionaryEntry( 6055, tSock.language )); // You cannot cook your chicken on that.
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
			tSock.SysMessage( GetDictionaryEntry( 393, tSock.language )); // That is too far away.
			return;
			}
		// remove one raw chicken
		var iMakeResource = pUser.ResourceCount( 0x1607 );	// is there enough resources to use up to make it
		if( iMakeResource < 1 )
		{
			tSock.SysMessage( GetDictionaryEntry( 6056, tSock.language )); // You don't seem to have any raw chicken legs!
			return;
		}
		if( pUser.skills[13] < 200 )
		{
			tSock.SysMessage( GetDictionaryEntry( 6051, tSock.language )); // You are not skilled enough to do that.
			return;
		}
		pUser.UseResource( 1, 0x1607 ); // uses up a resource (amount, item ID, item colour)
		pUser.SoundEffect( 0x0021, true );
		// check the skill
		if( !pUser.CheckSkill( 13, 100, 500 ) )	// character to check, skill #, minimum skill, and maximum skill
		{
			tSock.SysMessage( GetDictionaryEntry( 6057, tSock.language )); // You burnt the chicken leg to crisp
			return;
		}
		var itemMade = CreateDFNItem( tSock, pUser, "0x1608", 1, "ITEM", true ); // makes a cooked chicken leg
		tSock.SysMessage( GetDictionaryEntry( 6058, tSock.language )); // You cook a chicken leg. Smells good!
		return;
	}
}
