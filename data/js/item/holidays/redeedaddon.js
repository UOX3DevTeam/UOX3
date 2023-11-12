function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	ReDeedAddon( pUser, iUsed );
}

function ReDeedAddon( pUser, iUsed )
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	var wreathaddongump = new Gump;

	wreathaddongump.AddPage( 0 );
	wreathaddongump.AddBackground(0, 0, 220, 170, 0x13BE);
	wreathaddongump.AddBackground(10, 10, 200, 150, 0xBB8);

	wreathaddongump.AddHTMLGump(20, 30, 180, 60, 0, 0, "<basefont color=#ffffff>Do you wish to re - deed this decoration ?</basefont>");
	wreathaddongump.AddHTMLGump(55, 100, 160, 25, 0, 0, "<basefont color=#ffffff>CONTINUE</basefont>");
	wreathaddongump.AddButton(20, 100, 0xFA5, 0xFA7, 1, 0, 1);
	wreathaddongump.AddHTMLGump(55, 125, 160, 25, 0, 0, "<basefont color=#ffffff>CANCEL</basefont>");
	wreathaddongump.AddButton(20, 125, 0xFA5, 0xFA7, 1, 0, 0);

	wreathaddongump.Send( pUser );
	wreathaddongump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var iUsed = pSock.tempObj;
	var iMulti = FindMulti(iUsed.x, iUsed.y, iUsed.z, pSock.currentChar.worldnumber);
	var deedName = "";
	switch( pButton ) 
	{
		case 0: break;
		case 1:
			switch( iUsed.sectionID )
			{
				case "0x232C":
				case "0x232D":
				case "wreathaddon": deedName = "WreathDeed"; break;
				default:
					pSock.SysMessage( "Error Report to GM" );
					break;
			}
			break;
	}
	iMulti.ReleaseItem( iUsed );
	var lockdownsLeft = iMulti.maxLockdowns - iMulti.lockdowns;
	pSock.SysMessage( GetDictionaryEntry( 1902, pSock.language ), lockdownsLeft ); // You release the item (%i lockdown(s) remaining).

	if( deedName != "" ) 
	{
		var deeditem = CreateDFNItem(pSock, pSock.currentChar, deedName, 1, "ITEM", true );
		if( ValidateObject( deeditem ))
		{
			iUsed.Delete();
		}
	}
}