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

	guildMenu.Send( socket );
	guildMenu.Free();
}

function GuildMenu( pUser )
{
	var guildMenu = new Gump;
	var socket = pUser.socket;
	var guildinfo = pUser.guild;

	guildMenu.AddPage( 1 );
	guildMenu.AddBackground( 0, 0, 600, 600, 0x6DB );
	guildMenu.AddBackground( 0, 0, 600, 100, 0x6DB );
	guildMenu.AddHTMLGump( 280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu</basefont>" );
	guildMenu.AddPicture( 240, 15, 0x0ED4 );
	guildMenu.AddBackground( 0, 0, 180, 600, 0x6DB );
	guildMenu.AddHTMLGump( 30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>" );
	guildMenu.AddPageButton( 135, 20, 0xFA5, 2 );
	guildMenu.AddPicture( -5, 20, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 60, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>" );
	guildMenu.AddPageButton( 135, 60, 0xFA5, 3 );
	guildMenu.AddPicture( -5, 60, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 100, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>" );
	guildMenu.AddPageButton( 135, 100, 0xFA5, 4 );
	guildMenu.AddPicture( -5, 100, 0x0FC0 );

	guildMenu.AddHTMLGump( 350, 120, 300, 35, false, false, "<basefont color=#ffffff>Guild News</basefont>" );
	guildMenu.AddBackground( 230, 150, 320, 300, 0x2486 );
	guildMenu.AddHTMLGump( 240, 160, 300, 35, false, false, "<basefont color=#1111111>news Test hey look i posted news</basefont>" );
	guildMenu.AddHTMLGump( 310, 500, 300, 35, false, false, "<basefont color=#ffffff>Guild Message of the Day</basefont>" );
	guildMenu.AddBackground( 230, 520, 320, 50, 0x2486 );
	guildMenu.AddHTMLGump( 240, 530, 300, 35, false, false, "<basefont color=#1111111>I own you today my slaves</basefont>" );

	guildMenu.AddPage( 2 );
	guildMenu.AddBackground( 0, 0, 600, 600, 0x6DB );
	guildMenu.AddBackground( 0, 0, 600, 100, 0x6DB );
	guildMenu.AddHTMLGump( 280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu Page 2</basefont>" );
	guildMenu.AddPicture( 240, 15, 0x0ED4 );
	guildMenu.AddBackground( 0, 0, 180, 600, 0x6DB );
	guildMenu.AddHTMLGump( 30, 20, 300, 35, false, false, "<basefont color=#0fffff>Guild Roster</basefont>" );
	guildMenu.AddPageButton( 135, 20, 0xFA5, 1 );
	guildMenu.AddPicture( -5, 20, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 60, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>" );
	guildMenu.AddPageButton( 135, 60, 0xFA5, 3 );
	guildMenu.AddPicture( -5, 60, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 100, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>" );
	guildMenu.AddPageButton( 135, 100, 0xFA5, 4 );
	guildMenu.AddPicture( -5, 100, 0x0FC0 );
	guildMenu.AddHTMLGump( 240, 120, 300, 35, false, false, "<basefont color=#ffffff>Name</basefont>" );
	guildMenu.AddHTMLGump( 380, 120, 300, 35, false, false, "<basefont color=#ffffff>Rank</basefont>" );
	guildMenu.AddHTMLGump( 500, 120, 300, 35, false, false, "<basefont color=#ffffff>Status</basefont>" );

	guildMenu.AddPage( 3 );
	guildMenu.AddBackground( 0, 0, 600, 600, 0x6DB );
	guildMenu.AddBackground( 0, 0, 600, 100, 0x6DB );
	guildMenu.AddHTMLGump( 280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu Page 3</basefont>" );
	guildMenu.AddPicture( 240, 15, 0x0ED4 );
	guildMenu.AddBackground( 0, 0, 180, 600, 0x6DB );
	guildMenu.AddHTMLGump( 30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>" );
	guildMenu.AddPageButton( 135, 20, 0xFA5, 2 );
	guildMenu.AddPicture( -5, 20, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 60, 300, 35, false, false, "<basefont color=#0fffff>Guild Information</basefont>" );
	guildMenu.AddPageButton( 135, 60, 0xFA5, 1 );
	guildMenu.AddPicture( -5, 60, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 100, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>" );
	guildMenu.AddPageButton( 135, 100, 0xFA5, 4 );
	guildMenu.AddPicture( -5, 100, 0x0FC0 );
	guildMenu.AddPicture( 220, 120, 0x0FBD );
	guildMenu.AddHTMLGump( 280, 120, 300, 35, false, false, "<basefont color=#ffffff>Guild Name: " + guildinfo.name.toString() +  "</basefont>" );
	guildMenu.AddHTMLGump( 280, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Abbreviation: " + guildinfo.abbreviation.toString() +  "</basefont>" );
	var guildType = guildinfo.type;
	var type = "";
	var typeID = 0;
	switch( guildType )
	{
		case 0: type = "Standard"; typeID = 0x1BC7; break;
		case 1: type = "Order"; typeID = 0x1BC4; break;
		case 2: type = "Chaos"; typeID = 0x1BC3; break;
	}
	guildMenu.AddPicture( 220, 160, typeID );
	guildMenu.AddHTMLGump( 280, 160, 300, 35, false, false, "<basefont color=#ffffff>Guild Type: " + type +  "</basefont>" );
	guildMenu.AddHTMLGump( 280, 180, 300, 35, false, false, "<basefont color=#ffffff>Guild Member Count: " + guildinfo.numMembers.toString() +  "</basefont>" );
	guildMenu.AddHTMLGump( 280, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruit Count: " + guildinfo.numRecruits.toString() +  "</basefont>" );

	guildMenu.AddPage( 4 );
	guildMenu.AddBackground( 0, 0, 600, 600, 0x6DB );
	guildMenu.AddBackground( 0, 0, 600, 100, 0x6DB );
	guildMenu.AddHTMLGump( 280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu Page 2</basefont>" );
	guildMenu.AddPicture( 240, 15, 0x0ED4 );
	guildMenu.AddBackground( 0, 0, 180, 600, 0x6DB );
	guildMenu.AddHTMLGump( 30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>" );
	guildMenu.AddPageButton( 135, 20, 0xFA5, 2 );
	guildMenu.AddPicture( -5, 20, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 60, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>" );
	guildMenu.AddPageButton( 135, 60, 0xFA5, 3 );
	guildMenu.AddPicture( -5, 60, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 100, 300, 35, false, false, "<basefont color=#0fffff>Guild Recruitment</basefont>" );
	guildMenu.AddPageButton( 135, 100, 0xFA5, 1 );
	guildMenu.AddPicture( -5, 100, 0x0FC0 );
	guildMenu.AddHTMLGump( 200, 120, 300, 35, false, false, "<basefont color=#ffffff>View recruits</basefont>" );
	guildMenu.AddHTMLGump( 350, 120, 300, 35, false, false, "<basefont color=#ffffff>Accept/Reject</basefont>" );
	guildMenu.AddButton( 350, 140, 0xFB7, 0xFB9, 1, 0, 2 );//Accept
	guildMenu.AddButton( 400, 140, 0xFB4, 0xFB6, 1, 0, 3 );//Reject
	guildMenu.AddHTMLGump( 480, 120, 300, 35, false, false, "<basefont color=#ffffff>Invite new recruit</basefont>" );
	guildMenu.AddButton( 520, 140, 0xFAE, 0xFB0, 1, 0, 3 );//invite
	guildMenu.Send( socket );
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

