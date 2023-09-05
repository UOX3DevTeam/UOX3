function InfoC( uox3gump )
{
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 130 );
	uox3gump.AddText( 330, 60, 0, "cleanup" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 131 );
	uox3gump.AddText( 330, 90, 0, "cnsYell" );
	uox3gump.AddPageButton( 270, 120, 2446, 2445, 132 );
	uox3gump.AddText( 330, 120, 0, "command" );
	uox3gump.AddPageButton( 270, 150, 2446, 2445, 133 );
	uox3gump.AddPageButton( 270, 180, 2446, 2445, 134 );
	uox3gump.AddPageButton( 270, 210, 2446, 2445, 135 );
	uox3gump.AddPageButton( 270, 240, 2446, 2445, 136 );
	uox3gump.AddText( 330, 150, 0, "consolTest" );
	uox3gump.AddText( 330, 180, 0, "cont" );
	uox3gump.AddText( 330, 210, 0, "cq" );
	uox3gump.AddText( 330, 240, 0, "cStats" );

	uox3gump.AddPage( 130 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 130 );
	uox3gump.AddText( 330, 60, 0, "cleanup" );
	uox3gump.AddBackground( 270, 90, 592, 89, 9300 );
	uox3gump.AddHTMLGump( 280, 100, 550, 70, false, false, "Deletes all corpses and temporary moongates in the world.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'cleanup</BASEFONT>" );
	uox3gump.AddPageButton( 270, 190, 2446, 2445, 131 );
	uox3gump.AddText( 330, 190, 0, "cnsYell" );
	uox3gump.AddPageButton( 270, 220, 2446, 2445, 132 );
	uox3gump.AddText( 330, 220, 0, "command" );
	uox3gump.AddPageButton( 270, 250, 2446, 2445, 133 );
	uox3gump.AddPageButton( 270, 280, 2446, 2445, 134 );
	uox3gump.AddPageButton( 270, 310, 2446, 2445, 135 );
	uox3gump.AddPageButton( 270, 340, 2446, 2445, 136 );
	uox3gump.AddText( 330, 250, 0, "consolTest" );
	uox3gump.AddText( 330, 280, 0, "cont" );
	uox3gump.AddText( 330, 310, 0, "cq" );
	uox3gump.AddText( 330, 340, 0, "cStats" );

	uox3gump.AddPage( 131 );
	uox3gump.AddBackground( 270, 120, 592, 166, 9300 );
	uox3gump.AddHTMLGump( 280, 130, 550, 150, false, false, "Counselor Yell - Announces a message to all with Counselor privileges or above.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'cnsYell</BASEFONT>"
		+ "<BR>Parameters:<BASEFONT color=#ff0000>( text)l</BASEFONT><BR>"
		+ "<BR>Example: 'cnsYell Everyone meet up at Counselor HQ in 10 minutes!" );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 130 );
	uox3gump.AddText( 330, 60, 0, "cleanup" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 131 );
	uox3gump.AddText( 330, 90, 0, "cnsYell" );
	uox3gump.AddPageButton( 270, 290, 2446, 2445, 132 );
	uox3gump.AddText( 330, 290, 0, "command" );
	uox3gump.AddPageButton( 270, 320, 2446, 2445, 133 );
	uox3gump.AddPageButton( 270, 350, 2446, 2445, 134 );
	uox3gump.AddPageButton( 270, 380, 2446, 2445, 135 );
	uox3gump.AddPageButton( 270, 410, 2446, 2445, 136 );
	uox3gump.AddText( 330, 320, 0, "consolTest" );
	uox3gump.AddText( 330, 350, 0, "cont" );
	uox3gump.AddText( 330, 380, 0, "cq" );
	uox3gump.AddText( 330, 410, 0, "cStats" );

	uox3gump.AddPage( 132 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 130 );
	uox3gump.AddText( 330, 60, 0, "cleanup" );
	uox3gump.AddBackground( 270, 150, 592, 386, 9300 );
	uox3gump.AddHTMLGump( 280, 160, 597, 376, false, false, "Manually executes commands normally triggered from scripts like menus.dfn, item DFNs, etc. Most are already available as stand-alone commands.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'commandl</BASEFONT>"
		+ "<BR>Parameters:<BASEFONT color=#ff0000>( cmd param)l</BASEFONT><BR>"
		+ "<BR><BASEFONT color=#ff0000>addItem ( itemID)</BASEFONT>"
		+ "<BR><BASEFONT color=#ff0000>addSpawner ( spawner ID)</BASEFONT>"
		+ "<BR><BASEFONT color=#ff0000>cPage ( text)</BASEFONT>"
		+ "<BR><BASEFONT color=#ff0000>gmMenu ( menu ID)</BASEFONT><BR>"
		+ "<BR>Examples:"
		+ "<BR>'command addItem 0x171a ( add a feathered hat in inventory)"
		+ "<BR>'command cPage Hello, anyone there? ( adds a help request in Counselor queue)"
		+ "<BR>'command gmMenu 1 ( opens gmMenu 1 from menus.dfn - same as gmMenu command)"
		+ "<BR>'command guiInformation ( shows same server info gump as from help menu)"
		+ "<BR>'command makeMenu 1 ( opens makeMenu 1 from create DFNs)"
		+ "<BR>'command sysMessage Event starts in 10 minutes, at Castle British! ( sends system message to all players)" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 131 );
	uox3gump.AddText( 330, 90, 0, "cnsYell" );
	uox3gump.AddPageButton( 270, 120, 2446, 2445, 132 );
	uox3gump.AddText( 330, 120, 0, "command" );
	uox3gump.AddPageButton( 270, 540, 2446, 2445, 133 );
	uox3gump.AddPageButton( 270, 570, 2446, 2445, 134 );
	uox3gump.AddPageButton( 470, 540, 2446, 2445, 135 );
	uox3gump.AddPageButton( 470, 570, 2446, 2445, 136 );
	uox3gump.AddText( 330, 540, 0, "consolTest" );
	uox3gump.AddText( 330, 570, 0, "cont" );
	uox3gump.AddText( 540, 540, 0, "cq" );
	uox3gump.AddText( 540, 570, 0, "cStats" );

	uox3gump.AddPage( 133 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 130 );
	uox3gump.AddText( 330, 60, 0, "cleanup" );
	uox3gump.AddBackground( 270, 190, 592, 122, 9300 );
	uox3gump.AddHTMLGump( 280, 200, 564, 100, false, false, "Just displays some colourful stuff in the UOX3 console, just a test command =)<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'consoleTest</BASEFONT>" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 131 );
	uox3gump.AddText( 330, 90, 0, "cnsYell" );
	uox3gump.AddPageButton( 270, 120, 2446, 2445, 132 );
	uox3gump.AddText( 330, 120, 0, "command" );
	uox3gump.AddPageButton( 270, 150, 2446, 2445, 133 );
	uox3gump.AddPageButton( 270, 320, 2446, 2445, 134 );
	uox3gump.AddPageButton( 270, 350, 2446, 2445, 135 );
	uox3gump.AddPageButton( 270, 380, 2446, 2445, 136 );
	uox3gump.AddText( 330, 150, 0, "consolTest" );
	uox3gump.AddText( 330, 320, 0, "cont" );
	uox3gump.AddText( 330, 350, 0, "cq" );
	uox3gump.AddText( 330, 380, 0, "cStats" );

	uox3gump.AddPage( 134 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 130 );
	uox3gump.AddText( 330, 60, 0, "cleanup" );
	uox3gump.AddBackground( 270, 210, 592, 122, 9300 );
	uox3gump.AddHTMLGump( 280, 220, 564, 100, false, false, "Shortcut command to make targeted item a container, and set it to nondecay and movable 2.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'cont</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>( target)</BASEFONT>" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 131 );
	uox3gump.AddText( 330, 90, 0, "cnsYell" );
	uox3gump.AddPageButton( 270, 120, 2446, 2445, 132 );
	uox3gump.AddText( 330, 120, 0, "command" );
	uox3gump.AddPageButton( 270, 150, 2446, 2445, 133 );
	uox3gump.AddPageButton( 270, 180, 2446, 2445, 134 );
	uox3gump.AddPageButton( 270, 340, 2446, 2445, 135 );
	uox3gump.AddPageButton( 270, 370, 2446, 2445, 136 );
	uox3gump.AddText( 330, 150, 0, "consolTest" );
	uox3gump.AddText( 330, 180, 0, "cont" );
	uox3gump.AddText( 330, 340, 0, "cq" );
	uox3gump.AddText( 330, 370, 0, "cStats" );

	uox3gump.AddPage( 135 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 130 );
	uox3gump.AddText( 330, 60, 0, "cleanup" );
	uox3gump.AddBackground( 270, 240, 592, 152, 9300 );
	uox3gump.AddHTMLGump( 280, 250, 575, 134, false, false, "Perform tasks relating to the Counselor Queue; Go to next call in the queue, clear the current call, go to current call, transfer current call to GM queue."

		+ "<BR>If nothing is specified, the Counselor Queue will be displayed.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'cq</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>( nothing) / next / clear / curr / transfer</BASEFONT>" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 131 );
	uox3gump.AddText( 330, 90, 0, "cnsYell" );
	uox3gump.AddPageButton( 270, 120, 2446, 2445, 132 );
	uox3gump.AddText( 330, 120, 0, "command" );
	uox3gump.AddPageButton( 270, 150, 2446, 2445, 133 );
	uox3gump.AddPageButton( 270, 180, 2446, 2445, 134 );
	uox3gump.AddPageButton( 270, 210, 2446, 2445, 135 );
	uox3gump.AddPageButton( 270, 400, 2446, 2445, 136 );
	uox3gump.AddText( 330, 150, 0, "consolTest" );
	uox3gump.AddText( 330, 180, 0, "cont" );
	uox3gump.AddText( 330, 210, 0, "cq" );
	uox3gump.AddText( 330, 400, 0, "cStats" );

	uox3gump.AddPage( 136 );
	uox3gump.AddPageButton( 270, 60, 2446, 2445, 130 );
	uox3gump.AddBackground( 270, 270, 592, 115, 9300 );
	uox3gump.AddHTMLGump( 280, 280, 575, 97, false, false, "Opens a gump displaying the stats of the targeted character.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'cStats</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>( target)</BASEFONT>" );
	uox3gump.AddText( 330, 60, 0, "cleanup" );
	uox3gump.AddPageButton( 270, 90, 2446, 2445, 131 );
	uox3gump.AddText( 330, 90, 0, "cnsYell" );
	uox3gump.AddPageButton( 270, 120, 2446, 2445, 132 );
	uox3gump.AddText( 330, 120, 0, "command" );
	uox3gump.AddPageButton( 270, 150, 2446, 2445, 133 );
	uox3gump.AddPageButton( 270, 180, 2446, 2445, 134 );
	uox3gump.AddPageButton( 270, 210, 2446, 2445, 135 );
	uox3gump.AddPageButton( 270, 240, 2446, 2445, 136 );
	uox3gump.AddText( 330, 150, 0, "consolTest" );
	uox3gump.AddText( 330, 180, 0, "cont" );
	uox3gump.AddText( 330, 210, 0, "cq" );
	uox3gump.AddText( 330, 240, 0, "cStats" );
}

function _restorecontext_() {}