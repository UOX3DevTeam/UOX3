function CommandRegistration()
{
	RegisterCommand( "adddoor", 2, true );
}

function command_ADDDOOR( socket, cmdString )
{
	var pUser = socket.currentChar;
	switch ( pUser.GetTag( "getdoor" ) )
	{
		case 1:
			DoorGump1( pUser );
			break;
		case 2:
			DoorGump2( pUser );
			break;
		case 3:
			DoorGump3( pUser );
			break;
		case 4:
			DoorGump4( pUser );
			break;
		case 5:
			DoorGump5( pUser );
			break;
		case 6:
			DoorGump6( pUser );
			break;
		case 7:
			DoorGump7( pUser );
			break;
		case 8:
			DoorGump8( pUser );
			break;
		default:
			DoorGump1( pUser );
			break;
	}
}

const tooltipproperty = 1042971;
var width = 570;
var height = 165;

function DoorGump1( pUser )
{
	var DoorGump = new Gump;
	var socket = pUser.socket;
	pUser.SetTag( "getdoor", 1 );
	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	DoorGump.AddButton( 30, 20, 0x2624, 0x2625, 1, 0, 1 );
	DoorGump.AddPictureColor( 30, 35, 0x0675, 0 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9900, pSock.language ) + "</basefont > " ); // Metal Door

    DoorGump.AddButton( 75, 20, 0x2624, 0x2625, 1, 0, 2 );
	DoorGump.AddPictureColor( 70, 35, 0x0677, 0 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9900, pSock.language ) + "</basefont>" ); // Metal Door

	DoorGump.AddButton( 120, 20, 0x2624, 0x2625, 1, 0, 3 );
	DoorGump.AddPictureColor( 115, 35, 0x0679, 0 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9900, pSock.language ) + "</basefont>" ); // Metal Door

	DoorGump.AddButton( 165, 20, 0x2624, 0x2625, 1, 0, 4 );
	DoorGump.AddPictureColor( 160, 35, 0x067B, 0 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9900, pSock.language ) + "</basefont>" ); // Metal Door

	DoorGump.AddButton( 230, 20, 0x2624, 0x2625, 1, 0, 5 );
	DoorGump.AddPictureColor( 200, 35, 0x067D, 0 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9901, pSock.language ) + "</basefont>" ); // Metal Door

	DoorGump.AddButton( 275, 20, 0x2624, 0x2625, 1, 0, 6 );
	DoorGump.AddPictureColor( 245, 35, 0x067F, 0 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9901, pSock.language ) + "</basefont>" ); // Metal Door

	DoorGump.AddButton( 320, 20, 0x2624, 0x2625, 1, 0, 7 );
	DoorGump.AddPictureColor( 290, 35, 0x0681, 0 );
    DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9901, pSock.language ) + "</basefont>" ); // Metal Door

	DoorGump.AddButton( 365, 20, 0x2624, 0x2625, 1, 0, 8 );
	DoorGump.AddPictureColor( 330, 35, 0x0683, 0 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9901, pSock.language ) + "</basefont>" ); // Metal Door

	DoorGump.AddHTMLGump( 500, 20, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9916, pSock.language ) + "1</basefont>" );
	DoorGump.AddHTMLGump( 500, 45, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, pSock.language ) + "</basefont>" ); // Next
    DoorGump.AddButton( 500, 60, 0xFA5, 0xFA7, 1, 0, 9 );
	DoorGump.Send( pUser );
	DoorGump.Free();
	return;
}

function DoorGump2( pUser )
{
	var socket = pUser.socket;
	var DoorGump = new Gump;
	pUser.SetTag( "getdoor", 2 );
	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	DoorGump.AddButton( 35, 23, 9764, 9765, 1, 0, 10 );
	DoorGump.AddPicture( 29, 35, 1669 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9902, pSock.language ) + "</basefont>" ); //Barred Metal Door

	DoorGump.AddButton( 78, 23, 9764, 9765, 1, 0, 11 );
	DoorGump.AddPicture( 71, 35, 1671 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9902, pSock.language ) + "</basefont>" ); //Barred Metal Door

	DoorGump.AddButton( 118, 23, 9764, 9765, 1, 0, 12 );
	DoorGump.AddPicture( 112, 35, 1673 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9902, pSock.language ) + "</basefont>" ); //Barred Metal Door

	DoorGump.AddButton( 162, 23, 9764, 9765, 1, 0, 13 );
	DoorGump.AddPicture( 149, 35, 1675 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9902, pSock.language ) + "</basefont>" ); //Barred Metal Door

	DoorGump.AddButton( 328, 25, 9764, 9765, 1, 0, 14 );
	DoorGump.AddPicture( 310, 35, 1683 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9903, pSock.language ) + "</basefont>" ); //Barred Metal Door

	DoorGump.AddButton( 289, 25, 9764, 9765, 1, 0, 15 );
	DoorGump.AddPicture( 270, 35, 1681 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9903, pSock.language ) + "</basefont>" ); //Barred Metal Door

	DoorGump.AddButton( 253, 25, 9764, 9765, 1, 0, 16 );
	DoorGump.AddPicture( 230, 35, 1679 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9903, pSock.language ) + "</basefont>" ); //Barred Metal Door

	DoorGump.AddButton( 219, 25, 9764, 9765, 1, 0, 17 );
	DoorGump.AddPicture( 190, 35, 1677 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9903, pSock.language ) + "</basefont>" ); //Barred Metal Door

	DoorGump.AddHTMLGump( 500, 20, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9916, pSock.language ) + "2</basefont>" );
	DoorGump.AddHTMLGump( 500, 45, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, pSock.language ) + "</basefont>" ); // Next
    DoorGump.AddButton( 500, 60, 0xFA5, 0xFA7, 1, 0, 18 );

	DoorGump.AddHTMLGump( 500, 85, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, pSock.language ) + "</basefont>" ); // Back
    DoorGump.AddButton( 500, 100, 0xFAE, 0xFB0, 1, 0, 19 );
	DoorGump.Send( pUser );
	DoorGump.Free();
}

function DoorGump3( pUser )
{
	var socket = pUser.socket;
	var DoorGump = new Gump;
	pUser.SetTag( "getdoor", 3 );
	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	DoorGump.AddButton( 35, 23, 9764, 9765, 1, 0, 20 );
	DoorGump.AddPicture( 30, 35, 1685 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9904, pSock.language ) + "</basefont>" ); //Rotten Door

	DoorGump.AddButton( 78, 23, 9764, 9765, 1, 0, 21 );
	DoorGump.AddPicture( 70, 35, 1687 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9904, pSock.language ) + "</basefont>" ); //Rotten Door

	DoorGump.AddButton( 118, 23, 9764, 9765, 1, 0, 22 );
	DoorGump.AddPicture( 110, 35, 1689 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9904, pSock.language ) + "</basefont>" ); //Rotten Door

	DoorGump.AddButton( 162, 23, 9764, 9765, 1, 0, 23 );
	DoorGump.AddPicture( 150, 35, 1691 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9904, pSock.language ) + "</basefont>" ); //Rotten Door

	DoorGump.AddButton( 328, 25, 9764, 9765, 1, 0, 24 );
	DoorGump.AddPicture( 310, 35, 1699 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9905, pSock.language ) + "</basefont>" ); //Rotten Door

	DoorGump.AddButton( 289, 25, 9764, 9765, 1, 0, 25 );
	DoorGump.AddPicture( 270, 35, 1697 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9905, pSock.language ) + "</basefont>" ); //Rotten Door

	DoorGump.AddButton( 253, 25, 9764, 9765, 1, 0, 26 );
	DoorGump.AddPicture( 230, 35, 1695 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9905, pSock.language ) + "</basefont>" ); //Rotten Door

	DoorGump.AddButton( 219, 25, 9764, 9765, 1, 0, 27 );
	DoorGump.AddPicture( 190, 35, 1693 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9905, pSock.language ) + "</basefont>" ); //Rotten Door

	DoorGump.AddHTMLGump( 500, 20, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9916, pSock.language ) + "3</basefont>" );
	DoorGump.AddHTMLGump( 500, 45, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 60, 4005, 4007, 1, 0, 28 );
	DoorGump.AddHTMLGump( 500, 85, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 100, 4014, 4016, 1, 0, 29 );
	
	DoorGump.Send( pUser );
	DoorGump.Free();
}

function DoorGump4( pUser )
{
	var socket = pUser.socket;
	var DoorGump = new Gump;
	pUser.SetTag( "getdoor", 4 );
	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	DoorGump.AddButton( 35, 23, 9764, 9765, 1, 0, 30 );
	DoorGump.AddPicture( 30, 35, 1701 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9906, pSock.language ) + "</basefont>" ); //Wooden Door

	DoorGump.AddButton( 78, 23, 9764, 9765, 1, 0, 31 );
	DoorGump.AddPicture( 70, 35, 1703 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9906, pSock.language ) + "</basefont>" ); //Wooden Door

	DoorGump.AddButton( 118, 23, 9764, 9765, 1, 0, 32 );
	DoorGump.AddPicture( 110, 35, 1705 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9906, pSock.language ) + "</basefont>" ); //Wooden Door

	DoorGump.AddButton( 162, 23, 9764, 9765, 1, 0, 33 );
	DoorGump.AddPicture( 150, 35, 1707 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9906, pSock.language ) + "</basefont>" ); //Wooden Door

	DoorGump.AddButton( 328, 25, 9764, 9765, 1, 0, 34 );
	DoorGump.AddPicture( 305, 35, 1715 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9907, pSock.language ) + "</basefont>" ); //Wooden Door

	DoorGump.AddButton( 289, 25, 9764, 9765, 1, 0, 35 );
	DoorGump.AddPicture( 265, 35, 1713 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9907, pSock.language ) + "</basefont>" ); //Wooden Door

	DoorGump.AddButton( 253, 25, 9764, 9765, 1, 0, 36 );
	DoorGump.AddPicture( 230, 35, 1711 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9907, pSock.language ) + "</basefont>" ); //Wooden Door

	DoorGump.AddButton( 219, 25, 9764, 9765, 1, 0, 37 );
	DoorGump.AddPicture( 198, 35, 1709 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9907, pSock.language ) + "</basefont>" ); //Wooden Door


	DoorGump.AddHTMLGump( 500, 20, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9916, pSock.language ) + "4</basefont>" );
	DoorGump.AddHTMLGump( 500, 45, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 60, 4005, 4007, 1, 0, 38 );
	DoorGump.AddHTMLGump( 500, 85, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 100, 4014, 4016, 1, 0, 39 );
	
	DoorGump.Send( pUser );
	DoorGump.Free();
}

function DoorGump5( pUser )
{
	var socket = pUser.socket;
	var DoorGump = new Gump;
	pUser.SetTag( "getdoor", 5 );
	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	DoorGump.AddButton( 35, 23, 9764, 9765, 1, 0, 40 );
	DoorGump.AddPicture( 30, 35, 1717 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9908, pSock.language ) + "</basefont>" ); //Tall Wooden Door

	DoorGump.AddButton( 78, 23, 9764, 9765, 1, 0, 41 );
	DoorGump.AddPicture( 70, 35, 1719 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9908, pSock.language ) + "</basefont>" ); //Tall Wooden Door

	DoorGump.AddButton( 118, 23, 9764, 9765, 1, 0, 42 );
	DoorGump.AddPicture( 110, 35, 1721 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9908, pSock.language ) + "</basefont>" ); //Tall Wooden Door

	DoorGump.AddButton( 162, 23, 9764, 9765, 1, 0, 43 );
	DoorGump.AddPicture( 150, 35, 1723 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9908, pSock.language ) + "</basefont>" ); //Tall Wooden Door

	DoorGump.AddButton( 328, 25, 9764, 9765, 1, 0, 44 );
	DoorGump.AddPicture( 300, 35, 1731 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9909, pSock.language ) + "</basefont>" ); //Tall Wooden Door

	DoorGump.AddButton( 289, 25, 9764, 9765, 1, 0, 45 );
	DoorGump.AddPicture( 265, 35, 1729 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9909, pSock.language ) + "</basefont>" ); //Tall Wooden Door

	DoorGump.AddButton( 253, 25, 9764, 9765, 1, 0, 46 );
	DoorGump.AddPicture( 230, 35, 1727 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9909, pSock.language ) + "</basefont>" ); //Tall Wooden Door

	DoorGump.AddButton( 219, 25, 9764, 9765, 1, 0, 47 );
	DoorGump.AddPicture( 190, 35, 1725 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9909, pSock.language ) + "</basefont>" ); //Tall Wooden Door

	DoorGump.AddHTMLGump( 500, 20, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9916, pSock.language ) + "5</basefont>" );
	DoorGump.AddHTMLGump( 500, 45, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 60, 4005, 4007, 1, 0, 48 );
	DoorGump.AddHTMLGump( 500, 85, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 100, 4014, 4016, 1, 0, 49 );
	
	DoorGump.Send( pUser );
	DoorGump.Free();
}

function DoorGump6( pUser )
{
	var socket = pUser.socket;
	var DoorGump = new Gump;
	pUser.SetTag( "getdoor", 6 );
	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	DoorGump.AddButton( 35, 23, 9764, 9765, 1, 0, 50 );
	DoorGump.AddPicture( 30, 35, 0x06C5 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9910, pSock.language ) + "</basefont>" ); //Tall Metal Door

	DoorGump.AddButton( 78, 23, 9764, 9765, 1, 0, 51 );
	DoorGump.AddPicture( 70, 35, 0x06C7 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9910, pSock.language ) + "</basefont>" ); //Tall Metal Door

	DoorGump.AddButton( 118, 23, 9764, 9765, 1, 0, 52 );
	DoorGump.AddPicture( 110, 35, 0x06C9 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9910, pSock.language ) + "</basefont>" ); //Tall Metal Door

	DoorGump.AddButton( 162, 23, 9764, 9765, 1, 0, 53 );
	DoorGump.AddPicture( 150, 35, 0x06CB );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9910, pSock.language ) + "</basefont>" ); //Tall Metal Door

	DoorGump.AddButton( 328, 25, 9764, 9765, 1, 0, 54 );
	DoorGump.AddPicture( 300, 35, 0x06CD );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9911, pSock.language ) + "</basefont>" ); //Tall Metal Door

	DoorGump.AddButton( 289, 25, 9764, 9765, 1, 0, 55 );
	DoorGump.AddPicture( 265, 35, 0x06CF );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9911, pSock.language ) + "</basefont>" ); //Tall Metal Door

	DoorGump.AddButton( 253, 25, 9764, 9765, 1, 0, 56 );
	DoorGump.AddPicture( 230, 35, 0x06D1 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9911, pSock.language ) + "</basefont>" ); //Tall Metal Door

	DoorGump.AddButton( 219, 25, 9764, 9765, 1, 0, 57 );
	DoorGump.AddPicture( 190, 35, 0x06D3 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9911, pSock.language ) + "</basefont>" ); //Tall Metal Door

	DoorGump.AddHTMLGump( 500, 20, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9916, pSock.language ) + "6</basefont>" );
	DoorGump.AddHTMLGump( 500, 45, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 60, 4005, 4007, 1, 0, 58 );
	DoorGump.AddHTMLGump( 500, 85, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 100, 4014, 4016, 1, 0, 59 );
	
	DoorGump.Send( pUser );
	DoorGump.Free();
}

function DoorGump7( pUser )
{
	var socket = pUser.socket;
	var DoorGump = new Gump;
	pUser.SetTag( "getdoor", 7 );
	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	DoorGump.AddButton( 35, 23, 9764, 9765, 1, 0, 60 );
	DoorGump.AddPicture( 30, 35, 0x06D5 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9911, pSock.language ) + "</basefont>" ); //Light Wooden Door

	DoorGump.AddButton( 78, 23, 9764, 9765, 1, 0, 61 );
	DoorGump.AddPicture( 70, 35, 0x06D7 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9911, pSock.language ) + "</basefont>" ); //Light Wooden Door

	DoorGump.AddButton( 118, 23, 9764, 9765, 1, 0, 62 );
	DoorGump.AddPicture( 110, 35, 0x06D9 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9911, pSock.language ) + "</basefont>" ); //Light Wooden Door

	DoorGump.AddButton( 162, 23, 9764, 9765, 1, 0, 63 );
	DoorGump.AddPicture( 150, 35, 0x06DB );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9911, pSock.language ) + "</basefont>" ); //Light Wooden Door

	DoorGump.AddButton( 328, 25, 9764, 9765, 1, 0, 64 );
	DoorGump.AddPicture( 300, 35, 0x06DD );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9912, pSock.language ) + "</basefont>" ); //Light Wooden Door

	DoorGump.AddButton( 289, 25, 9764, 9765, 1, 0, 65 );
	DoorGump.AddPicture( 265, 35, 0x06DF );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9912, pSock.language ) + "</basefont>" ); //Light Wooden Door

	DoorGump.AddButton( 253, 25, 9764, 9765, 1, 0, 66 );
	DoorGump.AddPicture( 230, 35, 0x06E1 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9912, pSock.language ) + "</basefont>" ); //Light Wooden Door

	DoorGump.AddButton( 219, 25, 9764, 9765, 1, 0, 67 );
	DoorGump.AddPicture( 190, 35, 0x06E3 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9912, pSock.language ) + "</basefont>" ); //Light Wooden Door

	DoorGump.AddHTMLGump( 500, 20, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9916, pSock.language ) + "7</basefont>" );
	DoorGump.AddHTMLGump( 500, 45, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 60, 4005, 4007, 1, 0, 68 );
	DoorGump.AddHTMLGump( 500, 85, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 100, 4014, 4016, 1, 0, 69 );
	
	DoorGump.Send( pUser );
	DoorGump.Free();
}

function DoorGump8( pUser )
{
	var socket = pUser.socket;
	var DoorGump = new Gump;
	pUser.SetTag( "getdoor", 8 );
	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	DoorGump.AddButton( 35, 23, 9764, 9765, 1, 0, 70 );
	DoorGump.AddPicture( 30, 35, 0x06E5 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9913, pSock.language ) + "</basefont>" ); //Wooden Metal Brace Door

	DoorGump.AddButton( 78, 23, 9764, 9765, 1, 0, 71 );
	DoorGump.AddPicture( 70, 35, 0x06E7 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9913, pSock.language ) + "</basefont>" ); //Wooden Metal Brace Door

	DoorGump.AddButton( 118, 23, 9764, 9765, 1, 0, 72 );
	DoorGump.AddPicture( 110, 35, 0x06E9 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9913, pSock.language ) + "</basefont>" ); //Wooden Metal Brace Door

	DoorGump.AddButton( 162, 23, 9764, 9765, 1, 0, 73 );
	DoorGump.AddPicture( 150, 35, 0x06EB );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9913, pSock.language ) + "</basefont>" ); //Wooden Metal Brace Door

	DoorGump.AddButton( 328, 25, 9764, 9765, 1, 0, 74 );
	DoorGump.AddPicture( 300, 35, 0x06ED );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9914, pSock.language ) + "</basefont>" ); //Wooden Metal Brace Door

	DoorGump.AddButton( 289, 25, 9764, 9765, 1, 0, 75 );
	DoorGump.AddPicture( 265, 35, 0x06EF );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9914, pSock.language ) + "</basefont>" ); //Wooden Metal Brace Door

	DoorGump.AddButton( 253, 25, 9764, 9765, 1, 0, 76 );
	DoorGump.AddPicture( 230, 35, 0x06F1 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9914, pSock.language ) + "</basefont>" ); //Wooden Metal Brace Door

	DoorGump.AddButton( 219, 25, 9764, 9765, 1, 0, 77 );
	DoorGump.AddPicture( 190, 35, 0x06F3 );
	DoorGump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( 9914, pSock.language ) + "</basefont>" ); //Wooden Metal Brace Door

	DoorGump.AddHTMLGump( 500, 20, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9916, pSock.language ) + "8</basefont>" );
	DoorGump.AddHTMLGump( 500, 85, 60, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, pSock.language ) + "</basefont>" );
	DoorGump.AddButton( 500, 100, 4014, 4016, 1, 0, 79 );
	
	DoorGump.Send( pUser );
	DoorGump.Free();
}

function AddBlueBack( DoorGump )
{
	DoorGump.AddBackground( 0, 0, width - 00, height - 00, 0xE10 );
	DoorGump.AddBackground( 8, 5, width - 16, height - 11, 0x053 );
	DoorGump.AddTiledGump( 15, 14, width - 29, height - 29, 0xE14 );
	DoorGump.AddCheckerTrans( 15, 14, width - 29, height - 29 );
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	switch ( pButton )
	{
		case 0:
			break;
		case 1:
			pUser.ExecuteCommand( "radd 0x0675" );
			TriggerEvent(  1403, "DoorGump1", pUser  );
			break;
		case 2:
			pUser.ExecuteCommand( "radd 0x0677" );
			TriggerEvent(  1403, "DoorGump1", pUser  );
			break;
		case 3:
			pUser.ExecuteCommand( "radd 0x0679" );
			TriggerEvent(  1403, "DoorGump1", pUser  );
			break;
		case 4:
			pUser.ExecuteCommand( "radd 0x067B" );
			TriggerEvent(  1403, "DoorGump1", pUser  );
			break;
		case 5:
			pUser.ExecuteCommand( "radd 0x067D" );
			TriggerEvent(  1403, "DoorGump1", pUser  );
			break;
		case 6:
			pUser.ExecuteCommand( "radd 0x067F" );
			TriggerEvent(  1403, "DoorGump1", pUser  );
			break;
		case 7:
			pUser.ExecuteCommand( "radd 0x0681" );
			TriggerEvent(  1403, "DoorGump1", pUser  );
			break;
		case 8:
			pUser.ExecuteCommand( "radd 0x0683" );
			TriggerEvent(  1403, "DoorGump1", pUser  );
			break;
		case 9:
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 10:
			pUser.ExecuteCommand( "radd 1669" );
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 11:
			pUser.ExecuteCommand( "radd 1671" );
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 12:
			pUser.ExecuteCommand( "radd 1673" );
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 13:
			pUser.ExecuteCommand( "radd 1675" );
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 14:
			pUser.ExecuteCommand( "radd 1683" );
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 15:
			pUser.ExecuteCommand( "radd 1681" );
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 16:
			pUser.ExecuteCommand( "radd 1679" );
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 17:
			pUser.ExecuteCommand( "radd 1677" );
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 18:
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 19:
			TriggerEvent(  1403, "DoorGump1",  pUser  );
			break;
		case 20:
			pUser.ExecuteCommand( "radd 1685" );
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 21:
			pUser.ExecuteCommand( "radd 1687" );
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 22:
			pUser.ExecuteCommand( "radd 1689" );
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 23:
			pUser.ExecuteCommand( "radd 1691" );
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 24:
			pUser.ExecuteCommand( "radd 1699" );
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 25:
			pUser.ExecuteCommand( "radd 1697" );
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 26:
			pUser.ExecuteCommand( "radd 1693" );
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 27:
			pUser.ExecuteCommand( "radd 1695" );
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 28:
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 29:
			TriggerEvent(  1403, "DoorGump2", pUser  );
			break;
		case 30:
			pUser.ExecuteCommand( "radd 1701" );
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 31:
			pUser.ExecuteCommand( "radd 1703" );
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 32:
			pUser.ExecuteCommand( "radd 1705" );
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 33:
			pUser.ExecuteCommand( "radd 1707" );
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 34:
			pUser.ExecuteCommand( "radd 1715" );
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 35:
			pUser.ExecuteCommand( "radd 1713" );
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 36:
			pUser.ExecuteCommand( "radd 1711" );
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 37:
			pUser.ExecuteCommand( "radd 1719" );
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 38:
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 39:
			TriggerEvent(  1403, "DoorGump3", pUser  );
			break;
		case 40:
			pUser.ExecuteCommand( "radd 1717" );
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 41:
			pUser.ExecuteCommand( "radd 1719" );
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 42:
			pUser.ExecuteCommand( "radd 1721" );
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 43:
			pUser.ExecuteCommand( "radd 1723" );
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 44:
			pUser.ExecuteCommand( "radd 1731" );
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 45:
			pUser.ExecuteCommand( "radd 1729" );
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 46:
			pUser.ExecuteCommand( "radd 1727" );
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 47:
			pUser.ExecuteCommand( "radd 1725" );
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 48:
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 49:
			TriggerEvent(  1403, "DoorGump4", pUser  );
			break;
		case 50:
			pUser.ExecuteCommand( "radd 0x06C5" );
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 51:
			pUser.ExecuteCommand( "radd 0x06C7" );
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 52:
			pUser.ExecuteCommand( "radd 0x06C9" );
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 53:
			pUser.ExecuteCommand( "radd 0x06CB" );
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 54:
			pUser.ExecuteCommand( "radd 0x06CD" );
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 55:
			pUser.ExecuteCommand( "radd 0x06CF" );
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 56:
			pUser.ExecuteCommand( "radd 0x06D1" );
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 57:
			pUser.ExecuteCommand( "radd 0x06D3" );
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 58:
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 59:
			TriggerEvent(  1403, "DoorGump5", pUser  );
			break;
		case 60:
			pUser.ExecuteCommand( "radd 0x06D5" );
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 61:
			pUser.ExecuteCommand( "radd 0x06D7" );
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 62:
			pUser.ExecuteCommand( "radd 0x06D9" );
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 63:
			pUser.ExecuteCommand( "radd 0x06DB" );
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 64:
			pUser.ExecuteCommand( "radd 0x06DD" );
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 65:
			pUser.ExecuteCommand( "radd 0x06DF" );
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 66:
			pUser.ExecuteCommand( "radd 0x06E1" );
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 67:
			pUser.ExecuteCommand( "radd 0x06E3" );
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
		case 68:
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 69:
			TriggerEvent(  1403, "DoorGump6", pUser  );
			break;
		case 70:
			pUser.ExecuteCommand( "radd 0x06E5" );
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 71:
			pUser.ExecuteCommand( "radd 0x06E7" );
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 72:
			pUser.ExecuteCommand( "radd 0x06E9" );
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 73:
			pUser.ExecuteCommand( "radd 0x06EB" );
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 74:
			pUser.ExecuteCommand( "radd 0x06ED" );
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 75:
			pUser.ExecuteCommand( "radd 0x06EF" );
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 76:
			pUser.ExecuteCommand( "radd 0x06F1" );
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 77:
			pUser.ExecuteCommand( "radd 0x06F3" );
			TriggerEvent(  1403, "DoorGump8", pUser  );
			break;
		case 79:
			TriggerEvent(  1403, "DoorGump7", pUser  );
			break;
	}
}