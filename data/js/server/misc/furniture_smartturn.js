// turn furniture script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// if you use/move a piece of furniture it should rotate accordingly

function onUse( pUser, iUsed ) 
{ 
	// get users socket
	var srcSock = CalcSockFromChar( pUser );

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
        SysMessage( srcSock, "item east of player" );
    }
    if( ( absX > absY ) && ( relX > 0 ) ) 
	{
        direction = 2; // west
        SysMessage( srcSock, "item west of player" );
    }
    if( ( absX < absY ) && ( relY < 0 ) ) 
	{
        direction = 3; // south
        SysMessage( srcSock, "item south of player" );
    }
    if( ( absX < absY ) && ( relY > 0 ) ) 
	{
        direction = 4; // north
        SysMessage( srcSock, "item north of player" );
    }
    
	var iUsedID = iUsed.id;
    // straw chair    
    if( iUsedID == 0x0b5A || iUsedID == 0x0B5B || iUsedID == 0x0B5D )
    { 
        SysMessage( srcSock, "straw chair" );
        if( direction == 1 )
        {
            SysMessage( srcSock, "rotate east" );
            //SetID( iUsed, 0x0b5d, 1 ); // east
        }
        if( direction == 2 )
        {
            SysMessage( srcSock, "rotate west" );
            //SetID( iUsed, 0x0b5a, 1 ); // west
        }
        if( direction == 3 )
        {
            SysMessage( srcSock, "rotate south" );
            //SetID( iUsed, 0x0b5c, 1 ); // south
        }
        if( direction == 4 )
        {
            // north
            SysMessage( srcSock, "rotate north" );
            iUsed.id = 0x0B5B;
        }
    }
}

function onDrop( iDropped, pDropper ) 
{
    // get users socket
	var srcSock = CalcSockFromChar( pDropper );

    // onDrop not working atm
    // I coded the stuff into onUse, once onDrop is working it can just be switched over
    SysMessage( srcSock, "You droppsored the chair." );
   
}