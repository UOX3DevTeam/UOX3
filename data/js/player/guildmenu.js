function GuildCreation( pUser )
{
	var guildMenu = new Gump;
	var socket = pUser.socket;

	guildMenu.AddBackground( 0, 0, 380, 380, 0x6DB );
	guildMenu.AddCheckerTrans( 8, 8, 360, 360 );
	guildMenu.AddHTMLGump( 140, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Creation</basefont>" );
	guildMenu.AddPicture( 80, 15, 0x0ED4 );

	guildMenu.AddHTMLGump( 30, 100, 300, 140, false, false, "<basefont color=#ffffff>Thou art not bound to any guild. To establish thine own, provide a unique name and tender the standard guild registration fee.</basefont>" );
	
	guildMenu.AddPicture( 180, 170, 0x0EEF );
	guildMenu.AddHTMLGump( 30, 180, 300, 140, false, false, "<basefont color=#ffffff>Registration Fee:</basefont> <basefont color=#0fffff>25000</basefont>" );

	guildMenu.AddGump( 140, 220, 1803 );
	guildMenu.AddGump( 140, 260, 1803 );

	guildMenu.AddHTMLGump( 30, 220, 110, 70, false, false, "<basefont color=#ffffff>Enter Guild Name:</basefont>" );
	guildMenu.AddHTMLGump( 30, 240, 300, 140, false, false, "<SMALL><basefont color=#0fffff>Guild names may be up to 33 characters.</basefont></SMALL>" );

	guildMenu.AddHTMLGump( 30, 260, 110, 70, false, false, "<basefont color=#ffffff>Abbrevation:</basefont>" );
	guildMenu.AddHTMLGump( 30, 280, 300, 140, false, false, "<SMALL><basefont color=#0fffff>Guild Abbrevation may be up to 3 characters.</basefont></SMALL>" );

	guildMenu.AddTextEntryLimited( 140, 220, 200, 20, 0, 1, 9, pUser.name + "'s Guild", 33 );
	guildMenu.AddTextEntryLimited( 140, 260, 200, 20, 0, 2, 10, "NEW", 3 );

	guildMenu.AddButton( 210, 340, 0xF1, 0xF3, 1, 0, 0 );
	guildMenu.AddButton( 100, 340, 0xF7, 0xF8, 1, 0, 1 );

	guildMenu.Send(socket);
	guildMenu.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pChar = pSock.currentChar;

	switch( pButton )
	{
		case 0:// abort and do nothing
			break;
		case 1:
		{
			var Text1 = gumpData.getEdit(0);
			var Text2 = gumpData.getEdit(1);
			if( pChar.guild == null )
			{
				pChar.TextMessage( "Not currently in a guild... Creating new guild...", false, 0x3b2, 0, pChar.serial );
				var newGuild = CreateNewGuild();
				if( newGuild )
				{
					newGuild.name = Text1;
					newGuild.abbreviation = Text2;
					newGuild.type = 0;
					newGuild.AddMember( pChar );
					newGuild.master = pChar;
					pChar.guildTitle = "Guild Master";
					pChar.TextMessage( "Guild automatically created: " + newGuild.name, false, 0x3b2, 0, pChar.serial );
				}
				break;
			}
			else
			{
			   pChar.TextMessage( "Currently member of: " + pChar.guild.name, false, 0x3b2, 0, pChar.serial );
			   break;
			}
		}
	}
}

