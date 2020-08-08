// Syntax: 'RNDTILE <startID> <endID> <hue> <Z>(optional) <movable true/false>(optional)
// Example 1: 'RNDTILE 0x04a9 0x04ac 0x0			//random floor tiles of hue 0x0
// Example 2: 'RNDTILE 0x04a9 0x04ac 0x0 20 		//random floor tiles of hue 0x0 at Z 20
// Example 3: 'RNDTILE 0x04a9 0x04ac 0x32 20 true 	//random floortiles of hue 0x32 at Z 20, frozen/immovable
//
// If no Z is specified, the Z level of the second target of the boundary box will be used

function CommandRegistration()
{
	RegisterCommand( "rndtile", 2, true );
}

function command_RNDTILE( pSock, cmdString )
{
	if( !pSock )
		return;

	pSock.tempint = null;
	pSock.xText = null;

	if( cmdString )
	{
		var splitString = cmdString.split( " " );
		if( splitString[0] && splitString[1] && splitString[2] )
		{
			//if startID is < than endID
			if( parseInt( splitString[0] ) < parseInt( splitString[1] ))
			{
				pSock.xText = splitString[0] + " " + splitString[1] + " " + splitString[2];
				if( splitString[3] && splitString[3] != "true" && splitString[3] != "false" )
					pSock.tempint = parseInt( splitString[3] );
				else
					pSock.xText = pSock.xText + " " + splitString[3];
				if( splitString[4] )
					pSock.xText = pSock.xText + " " + splitString[4];
				pSock.CustomTarget( 0, "Select first corner of bounding box:" );
			}
			else
				pSock.SysMessage( "<startID> must be lower than <endID>!" );
		}
		else
			pSock.SysMessage( "A minimum of three arguments must be included; <startID> <endID> and <hue>" );
	}
	else
		pSock.SysMessage( "Syntax: 'RNDTILE <startID> <endID> <hue> <Z>(optional) <movable true/false>(optional)" );
}

//First target location in boundary box
function onCallback0( pSock, myTarget )
{
	if( !pSock )
		return;

	var targX = pSock.GetWord( 11 );
	var targY = pSock.GetWord( 13 );
	var targZ = pSock.tempint;
	var splitString = pSock.xText.split( " " );
	var startID = splitString[0];
	var endID = splitString[1];
	var itemHue = splitString[2];
	var toFreeze = splitString[3];
	if( toFreeze != "true" ) // if movable status hasn't been set to true, set it to false
		toFreeze = "false";
	pSock.xText = targX + " " + targY + " " + startID + " " + endID + " " + itemHue + " " + toFreeze;
	pSock.CustomTarget( 1, "Select second corner of bounding box:" );
}

//Second target location in boundary box
function onCallback1( pSock, myTarget )
{
	if( !pSock )
		return;

	var splitString = pSock.xText.split( " " );
	var targX = splitString[0];
	var targY = splitString[1];
	var targZ = pSock.tempint;
	if( !targZ && targZ != 0 ) //if no target Z has been specified, use the Z level of the target location
		targZ = pSock.GetSByte( 16 ) + GetTileHeight( pSock.GetWord( 17 ) );
	var startID = parseInt( splitString[2] );
	var endID = parseInt( splitString[3] );
	var itemHue = splitString[4];
	var toFreeze = splitString[5];
	var targX2 = pSock.GetWord( 11 );
	var targY2 = pSock.GetWord( 13 );
	if( targX2 < targX )
	{
		targX2 = parseInt( targX );
		targX = pSock.GetWord( 11 );
	}
	if( targY2 < targY )
	{
		targY2 = parseInt( targY );
		targY = pSock.GetWord( 13 );
	}
	var i = 0, j = 0;
	for( i = targX; i < targX2 +1; i++ )
	{
		for( j = targY; j < targY2 +1; j++ )
		{
			var rndID = RandomNumber( startID, endID );
			var newItem = CreateBlankItem( pSock, pSock.currentChar, 1, "#", rndID, 0x0, "ITEM", false );
			if( newItem )
			{
				if( toFreeze == "true" ) //if movable status has been set to true, make item immovable
					newItem.movable = 2;
				if( itemHue ) //if a hue has been specified, set hue of new item
					newItem.colour = itemHue;
				newItem.SetLocation( i, j, targZ, pSock.currentChar.worldnumber, pSock.currentChar.instanceID );
				newItem.decayable = false;
			}
		}
	}
}

