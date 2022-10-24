// This command is a shortcut to add house signs - brings up a menu to select from

function CommandRegistration()
{
	RegisterCommand( "addsign", 2, true );
}

function command_ADDSIGN( socket, cmdString )
{
	var pUser = socket.currentChar;
	SignGump( pUser );
}

const signtypes = [ 0xB95, 0xB96, 0xBA3, 0xBA4, 0xBA5, 0xBA6, 0xBA7, 0xBA8, 0xBA9, 0xBAA, 0xBAB, 0xBAC, 0xBAD, 0xBAE, 0xBAF, 0xBB0, 0xBB1, 0xBB2, 0xBB3, 0xBB4,
	0xBB5, 0xBB6, 0xBB7, 0xBB8, 0xBB9, 0xBBA, 0xBBB, 0xBBC, 0xBBD, 0xBBE, 0xBBF, 0xBC0, 0xBC1, 0xBC2, 0xBC3, 0xBC4, 0xBC5, 0xBC6, 0xBC7, 0xBC8, 0xBC9, 0xBCA,
	0xBCB, 0xBCC, 0xBCD, 0xBCE, 0xBCF, 0xBD0, 0xBD1, 0xBD2, 0xBD3, 0xBD4, 0xBD5, 0xBD6, 0xBD7, 0xBD8, 0xBD9, 0xBDA, 0xBDB, 0xBDC, 0xBDD, 0xBDE, 0xBDF, 0xBE0,
	0xBE1, 0xBE2, 0xBE3, 0xBE4, 0xBE5, 0xBE6, 0xBE7, 0xBE8, 0xBE9, 0xBEA, 0xBEB, 0xBEC, 0xBED, 0xBEE, 0xBEF, 0xBF0, 0xBF1, 0xBF2, 0xBF3, 0xBF4, 0xBF5, 0xBF6,
	0xBF7, 0xBF8, 0xBF9, 0xBFA, 0xBFB, 0xBFC, 0xBFD, 0xBFE, 0xBFF, 0xC00, 0xC01, 0xC02, 0xC03, 0xC04, 0xC05, 0xC06, 0xC07, 0xC08, 0xC09, 0xC0A, 0xC0B, 0xC0C,
	0xC0D, 0xC0E, 0x1297, 0x1298, 0x1299, 0x129A, 0x129B, 0x129C, 0x129D, 0x129E, 0x1F28, 0x1F29, 0x4B20, 0x4B21, 0x9A0C, 0x9A0D, 0x9A0E, 0x9A0F, 0x9A10, 0x9A11,
	0x9A12, 0x9A13 ];

const signGumpWidth = 588;
const signGumpHeight = 165;

function SignGump( pUser )
{
	var pSock = pUser.socket;
	var myGump = new Gump;
	myGump.AddPage( 0 );

	AddBlueBack( myGump );
	for( var i = 0; i < signtypes.length; i++ )
	{
		var pages = signtypes[i] / 20 + 1;
		var page = i / 20 + 1;
		var xpos = ( i / 2 ) % 10;
		var ypos = i % 2;

		if( xpos == 0 && ypos == 0 )
		{
			myGump.AddPage( page );
			myGump.AddHTMLGump( 30, 20, 60, 20, false, false, "<basefont color=#ffffff>" + page + "</basefont >" ); // #

			myGump.AddHTMLGump( 30, 45, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, pSock.language ) + "</basefont>" ); // Next
			if( page <= 6 )
			{
				myGump.AddButton( 30, 60, 0xFA5, 0xFA7, 0, page + 1, 0 );
			}
			else
			{
				myGump.AddButton( 30, 60, 0xFA5, 0xFA7, 0, 1, 0 );
			}

			myGump.AddHTMLGump( 30, 85, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, pSock.language ) + "</basefont>" ); // Back
			if( page >= 1 )
			{
				myGump.AddButton( 30, 100, 0xFAE, 0xFB0, 0, page - 1, 0 );
			}
			else
			{
				myGump.AddButton( 30, 100, 0xFAE, 0xFB0, 0, pages, 0 );
			}
		}

		var x = ( xpos + 1 ) * 50;
		var y = ( ypos * 75 );
		myGump.AddButton( 30 + x, 20 + y, 0x2624, 0x2625, 1, 0, i + 1 );
		myGump.AddPicture( 15 + x, 40 + y, signtypes[i] );
	}

	myGump.Send( pUser );
	myGump.Free();
}

function AddBlueBack( myGump )
{
	myGump.AddBackground( 0, 0, signGumpWidth, signGumpHeight, 0xE10 );
	myGump.AddBackground( 8, 5, signGumpWidth - 16, signGumpHeight - 11, 0x053 );
	myGump.AddTiledGump( 15, 14, signGumpWidth - 29, signGumpHeight - 29, 0xE14 );
	myGump.AddCheckerTrans( 15, 14, signGumpWidth - 29, signGumpHeight - 29 );
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	switch( pButton )
	{
		case 0:
			break;
		default:
			if( pButton > 0 && pButton <= signtypes.length )
			{
				pUser.ExecuteCommand( "radd " + signtypes[pButton-1] );
				TriggerEvent( 1060, "SignGump", pUser );
			}
			break;
	}
}