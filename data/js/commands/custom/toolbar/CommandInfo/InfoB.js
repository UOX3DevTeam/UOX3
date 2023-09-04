function InfoB( uox3gump )
{
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 120 );
	uox3gump.AddText( 330, 60, 0, "Bolt" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 121 );
	uox3gump.AddText( 330, 90, 0, "Browse" );
	uox3gump.AddPageButton( 270, 120, 2446, 2445, 122 );
	uox3gump.AddText( 330, 120, 0, "BrightLight" );
	uox3gump.AddPageButton( 270, 150, 2446, 2445, 123 );
	uox3gump.AddText( 330, 150, 0, "Buy" );

	uox3gump.AddPage( 120 );
	uox3gump.AddBackground( 270, 90, 592, 128, 9300 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 120 );
	uox3gump.AddText( 330, 60, 0, "Bolt" );
	uox3gump.AddPageButton(270, 230, 2446, 2445, 121 );
	uox3gump.AddText( 330, 230, 0, "Browse" );
	uox3gump.AddPageButton(270, 260, 2446, 2445, 122 );
	uox3gump.AddText( 330, 260, 0, "BrightLight" );
	uox3gump.AddPageButton( 270, 290, 2446, 2445, 123 );
	uox3gump.AddText( 330, 290, 0, "Buy" );

	uox3gump.AddHTMLGump( 280, 100, 550, 102, false, false, "Animates a harmless lightning bolt on target.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'bolt</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>(target)</BASEFONT><BR>" );

	uox3gump.AddPage( 121 );
	uox3gump.AddBackground( 270, 120, 592, 128, 9300 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 120 );
	uox3gump.AddText( 330, 60, 0, "Bolt" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 121 );
	uox3gump.AddText( 330, 90, 0, "Browse" );
	uox3gump.AddPageButton( 270, 260, 2446, 2445, 122 );
	uox3gump.AddText( 330, 260, 0, "BrightLight" );
	uox3gump.AddPageButton( 270, 290, 2446, 2445, 123 );
	uox3gump.AddText( 330, 290, 0, "Buy" );
	uox3gump.AddHTMLGump( 280, 130, 550, 102, false, false, "Opens a specified website in user's default browser.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'browse</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>(s)</BASEFONT><BR>" );

	uox3gump.AddPage( 122 );
	uox3gump.AddBackground( 270, 150, 592, 128, 9300 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 120 );
	uox3gump.AddText( 330, 60, 0, "Bolt" );
	uox3gump.AddHTMLGump( 280, 160, 550, 102, false, false, "Sets the brightest lightlevel in the day/night-cycles (Day: 0x00. Traditional OSI night: 0x09. Pitch black: 0x1F)<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'brightLight</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>(d / h)</BASEFONT><BR>" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 121 );
	uox3gump.AddText( 330, 90, 0, "Browse" );
	uox3gump.AddPageButton(270, 120, 2446, 2445, 122 );
	uox3gump.AddText( 330, 120, 0, "BrightLight" );
	uox3gump.AddPageButton( 270, 290, 2446, 2445, 123 );
	uox3gump.AddText( 330, 290, 0, "Buy" );

	uox3gump.AddPage( 123 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 120 );
	uox3gump.AddText( 330, 60, 0, "Bolt" );
	uox3gump.AddBackground( 270, 180, 592, 128, 9300 );
	uox3gump.AddHTMLGump( 280, 190, 550, 102, false, false, "Opens the 'buy'-gump for specified vendor.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'buy</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>(target)</BASEFONT><BR>" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 121 );
	uox3gump.AddText( 330, 90, 0, "Browse" );
	uox3gump.AddPageButton( 270, 120, 2446, 2445, 122 );
	uox3gump.AddText( 330, 120, 0, "BrightLight" );
	uox3gump.AddPageButton( 270, 150, 2446, 2445, 123 );
	uox3gump.AddText( 330, 150, 0, "Buy" );
}

function _restorecontext_() {}