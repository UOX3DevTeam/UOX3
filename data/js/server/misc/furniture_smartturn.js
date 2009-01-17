// turn furniture script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// if you use/move a piece of furniture it should rotate accordingly

function onUseChecked( pUser, iUsed ) 
{ 
	// get users socket
	var srcSock = pUser.socket;

	// get users position
	var userX = pUser.x;
	var userY = pUser.y;
	// get items position
	var itemX = iUsed.x;
	var itemY = iUsed.y;
	// interpolate items facing
	var relX = userX - itemX;
	var relY = userY - itemY;
    
	var absX, absY;
	if( relX < 0 )
	{
		absX = relX * (-1);
	}
	else
	{
        	absX = relX;
	}
	if( relY < 0 )
	{
		absY = relY * (-1);
	}
	else
	{
		absY = relY;
	}

	var direction = 0;

	if( ( absX > absY ) && ( relX < 0 ) ) 
	{
		direction = 1;
		srcSock.SysMessage( "item east of player" );
	}
	if( ( absX > absY ) && ( relX > 0 ) ) 
	{
		direction = 2; // west
		srcSock.SysMessage( "item west of player" );
	}
	if( ( absX < absY ) && ( relY < 0 ) ) 
	{
		direction = 3; // south
		srcSock.SysMessage( "item south of player" );
	}
	if( ( absX < absY ) && ( relY > 0 ) ) 
	{
		direction = 4; // north
		srcSock.SysMessage( "item north of player" );
	}
    
	// straw chair    
	if( iUsed.id == 0x0b5A || iUsed.id == 0x0B5B || iUsed.id == 0x0B5D || iUsed.id == 0x0B5C )
	{ 
		srcSock.SysMessage( "straw chair" );
		if( direction == 1 )
		{
			srcSock.SysMessage( "rotate east" );
			iUsed.id = 0x0B5D;
		}
		if( direction == 2 )
		{
			srcSock.SysMessage( "rotate west" );
			iUsed.id = 0x0B5A;
		}
		if( direction == 3 )
		{
			srcSock.SysMessage( "rotate south" );
			iUsed.id = 0x0B5C;
		}
		if( direction == 4 )
		{
			srcSock.SysMessage( "rotate north" );
			iUsed.id = 0x0B5B;
		}
	}
}

function onDrop( iDropped, pDropper ) 
{
	// onDrop not working atm
	// I coded the stuff into onUse, once onDrop is working it can just be switched over
	pDropper.SysMessage( "You droppsored the chair." );
   
}