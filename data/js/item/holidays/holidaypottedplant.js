function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial) 
	{
		socket.SysMessage( GetDictionaryEntry( 1763, socket.language )); // The item must be in your backpack to use it.
		return false;
	}

	PottedPlantGump( pUser, iUsed );
}

function PottedPlantGump( pUser, iUsed )
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	var pottedPlant = new Gump;

	pottedPlant.AddPage( 0 );
	pottedPlant.AddBackground( 0, 0, 360, 195, 0xA28 );

	pottedPlant.AddPage( 1 );
	pottedPlant.AddText( 45, 15, 0, "Choose a Potted Plant:" );

	var plantIds = [0x11C8, 0x11C9, 0x11CA, 0x11CB, 0x11CC];
	for( var i = 0; i < 5; i++ )
	{
		var x = 45 + i * 60;
		pottedPlant.AddPicture( x, 75, plantIds[i] );
		pottedPlant.AddButton( x + 10, 50, 0x845, 0x846, 1, 0, i + 1 );
	}

	pottedPlant.Send( pUser );
	pottedPlant.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var iUsed = pSock.tempObj;
	var itemOwner = GetPackOwner( iUsed, 0 );

	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pSock.SysMessage( GetDictionaryEntry(1763, pSock.language )); // The item must be in your backpack to use it.
		return false;
	}

	var pottedplant = "";
	if( pButton >= 1 && pButton <= 5 ) 
	{
		var plantIds = [0x11C8, 0x11C9, 0x11CA, 0x11CB, 0x11CC];
		pottedplant = "0x" + ( plantIds[pButton - 1] ).toString( 16 );
	}

	if( pottedplant != "" )
	{
		var pottedplant = CreateDFNItem( pUser.socket, pUser, pottedplant, 1, "ITEM", true );
		pottedplant.movable = 1;
		pottedplant.decay = 1;
		iUsed.Delete();
	}
}