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
	var uox3gump = new Gump();

	// Create Page 0
	function createPage0()
	{
		uox3gump.AddPage( 0 );
		uox3gump.AddBackground( 140, 40, 100, 568, 9270 );
		uox3gump.AddBackground( 240, 40, 644, 568, 9270 );
		page0(pUser, uox3gump);
	}

	createPage0();

	uox3gump.Send( pUser );
	uox3gump.Free();
}

function page0( pUser, uox3gump )
{
	const buttonLabels = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "R"];
	const startY = 60;

	for( let i = 1; i <= 16; i++ ) 
	{
		const yPos = startY + ( i - 1 ) * 30;
		uox3gump.AddPageButton( 160, yPos, 2443, 2444, i );
		uox3gump.AddText( 190, yPos, 0, buttonLabels[i - 1] );
	}

	uox3gump.AddButton( 210, 570, 1543, 1544, 1, 0, 20 );
	uox3gump.AddText( 170, 570, 0, "Next" );

	uox3gump.AddPage( 1 );
	TriggerEvent( 9000, "InfoA", uox3gump );

	uox3gump.AddPage( 2 );
	TriggerEvent( 9001, "InfoB", uox3gump );

	uox3gump.AddPage( 3 );
	TriggerEvent(9002, "InfoC", uox3gump);
}

function page20( pUser )
{
	var uox3gump = new Gump;
	var socket = pUser.socket;
	uox3gump.AddPage( 0 );
	uox3gump.AddBackground( 140, 40, 100, 568, 9270 );
	uox3gump.AddBackground( 240, 40, 644, 568, 9270 );

	const startChar = 'S';
	const startY = 60;

	for( let i = 0; i < 8; i++ ) 
	{
		const char = String.fromCharCode(startChar.charCodeAt(0) + i );
		const yPos = startY + i * 30;

		uox3gump.AddPageButton( 160, yPos, 2443, 2444, i + 1 );
		uox3gump.AddText( 190, yPos, 0, char );
	}

	uox3gump.AddButton( 210, 570, 1545, 1546, 1, 0, 1 );
	uox3gump.AddText( 170, 570, 0, "Back" );

	uox3gump.Send( pUser );
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
		case 20: page20(pUser);
		break;
	}
}

function _restorecontext_() {}