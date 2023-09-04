function CommandRegistration()
{
	RegisterCommand( "toolbar", 2, true );
}

function command_TOOLBAR( socket, cmdString )
{
	var pUser = socket.currentChar;
	toolbar( pUser );
}

function toolbar( pUser ) 
{
	var socket = pUser.socket;
	var uox3gump = new Gump;
	uox3gump.NoClose();
	uox3gump.AddPage( 0 );

	uox3gump.AddBackground( 180, 260, 524, 32, 9200 );
	uox3gump.AddButton( 190, 265, 2445, 2443, 1, 0, 1 );
	uox3gump.AddButton( 300, 265, 2445, 2443, 1, 0, 2 );
	uox3gump.AddButton( 410, 265, 2445, 2443, 1, 0, 3 );
	uox3gump.AddButton( 520, 265, 2445, 2443, 1, 0, 4 );
	uox3gump.AddButton( 630, 265, 2443, 2445, 1, 0, 5 );
	uox3gump.AddText( 210, 267, 0, "COMMANDS" );
	uox3gump.AddText( 310, 267, 0, "JS DOCUMENT" );
	uox3gump.AddText( 530, 267, 0, "WHOLIST OFF" );
	uox3gump.AddText( 430, 267, 0, "WHOLIST ON" );
	uox3gump.AddText( 650, 267, 0, "ADD" );

	uox3gump.Send( pUser );
	uox3gump.Free();
}

function CommandInfo( pUser )
{
	var socket = pUser.socket;
	var uox3gump = new Gump;

	uox3gump.AddPage( 0 );
	uox3gump.AddBackground( 140, 40, 100, 568, 9270 );
	uox3gump.AddBackground( 240, 40, 644, 568, 9270 );

	uox3gump.AddPageButton( 160, 60, 2443, 2444, 1 );
	uox3gump.AddText( 190, 60, 0, "A" );

	uox3gump.AddPageButton( 160, 90, 2443, 2444, 2 );
	uox3gump.AddText( 190, 90, 0, "B" );

	uox3gump.AddPageButton( 160, 120, 2443, 2444, 3 );
	uox3gump.AddText( 190, 120, 0, "C" );

	uox3gump.AddPageButton( 160, 150, 2443, 2444, 4 );
	uox3gump.AddText( 190, 150, 0, "D" );

	uox3gump.AddPageButton( 160, 180, 2443, 2444, 5 );
	uox3gump.AddText( 190, 180, 0, "E" );

	uox3gump.AddPageButton( 160, 210, 2443, 2444, 6 );
	uox3gump.AddText( 190, 210, 0, "F" );

	uox3gump.AddPageButton( 160, 240, 2443, 2444, 7 );
	uox3gump.AddText( 190, 240, 0, "G" );

	uox3gump.AddPageButton( 160, 270, 2443, 2444, 8 );
	uox3gump.AddText( 190, 270, 0, "H" );

	uox3gump.AddPageButton( 160, 300, 2443, 2444, 9 );
	uox3gump.AddText( 190, 300, 0, "I" );

	uox3gump.AddPageButton( 160, 330, 2443, 2444, 10 );
	uox3gump.AddText( 190, 330, 0, "J" );

	uox3gump.AddPageButton( 160, 360, 2443, 2444, 11 );
	uox3gump.AddText( 190, 360, 0, "K" );

	uox3gump.AddPageButton( 160, 390, 2443, 2444, 11 );
	uox3gump.AddText( 190, 390, 0, "L" );

	uox3gump.AddPageButton( 160, 420, 2443, 2444, 12 );
	uox3gump.AddText( 190, 420, 0, "M" );

	uox3gump.AddPageButton( 160, 450, 2443, 2444, 13 );
	uox3gump.AddText( 190, 450, 0, "N" );

	uox3gump.AddPageButton( 160, 480, 2443, 2444, 14 );
	uox3gump.AddText( 190, 480, 0, "O" );

	uox3gump.AddPageButton( 160, 510, 2443, 2444, 15 );
	uox3gump.AddText( 190, 510, 0, "P" );

	uox3gump.AddPageButton( 160, 540, 2443, 2444, 16 );
	uox3gump.AddText( 190, 540, 0, "R" );

	uox3gump.AddPageButton( 160, 570, 2443, 2444, 17 );
	uox3gump.AddText( 190, 570, 0, "S" );

	uox3gump.AddPage( 1 );
	TriggerEvent( 9000, "InfoA", uox3gump );

	uox3gump.AddPage( 2 );
	TriggerEvent( 9001, "InfoB", uox3gump );

	uox3gump.Send( pUser);
	uox3gump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	switch ( pButton )
	{
		case 0:
			toolbar( pUser );
			break;
		case 1:
			CommandInfo( pUser );
			break;
		case 2:
			toolbar( pUser );
			break;
		case 3:
			pUser.ExecuteCommand( "wholist on" );
			pUser.ExecuteCommand( "toolbar" );
			break;
		case 4:
			pUser.ExecuteCommand( "wholist off" );
			pUser.ExecuteCommand( "toolbar" );
			break;
		case 5:
			pUser.ExecuteCommand( "Add" );
			pUser.ExecuteCommand( "toolbar" );
			break;
	}
}

function _restorecontext_() {}

























