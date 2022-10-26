// Returns an array with sextant coordinates
// Can be called from another script using
// 	var coordsArray = TriggerEvent( 2503, "GetMapCoordinates", xCoord, yCoord, worldNum )
function GetMapCoordinates( xCoord, yCoord, worldNum )
{
	var resultArray = new Array();
	var xCenter = 0;
	var yCenter = 0;
	var xWidth = 5120;
	var yHeight = 4096;

	switch( worldNum )
	{
		case 0: // Trammel
		case 1: // Felucca
			if( xCoord >= 0 && yCoord >= 0 && xCoord < 5120 && yCoord < 4096 )
			{
				xCenter = 1323;
				yCenter = 1624;
			}
			else if( xCoord >= 5120 && yCoord >= 0 && xCoord < 7168 && yCoord < 4096)
			{
				xCenter = 5936;
				yCenter = 3112;
			}
			else
			{
				resultArray[0] = "-1";
				resultArray[1] = "-1";
				resultArray[2] = true;
				resultArray[3] = "-1";
				resultArray[4] = "-1";
				resultArray[5] = true;
				return xLongitude;
			}
			break;
		case 2: // Ilshenar
		case 3: // Tokuno
		case 4: // Malas
		case 5: // Termur
			if( xCoord >= 0 && yCoord >= 0 && xCoord < 7168 && yCoord < 4096 )
			{
				xCenter = 1323;
				yCenter = 1624;
			}
			else
			{
				resultArray[0] = "-1";
				resultArray[1] = "-1";
				resultArray[2] = true;
				resultArray[3] = "-1";
				resultArray[4] = "-1";
				resultArray[5] = true;
				return xLongitude;
			}
			break;
		default:
			break;
	}

	// Longitude
	var absLong = (( xCoord - xCenter ) * 360 ) / xWidth;
	if( absLong > 180 )
	{
		absLong = -180 + ( absLong % 180 );
	}

	var xEast = ( absLong >= 0 );

	if( absLong < 0 )
	{
		absLong = -absLong;
	}

	var xLongDeg = Math.round( absLong );
	var xLongMin = Math.round(( absLong % 1 ) * 60 );

	// Latitude
	var absLat = (( yCoord - yCenter ) * 360 ) / yHeight;
	if( absLat > 180 )
	{
		absLat = -180 + ( absLat % 180 );
	}

	var ySouth = ( absLat >= 0 );

	if( absLat < 0 )
	{
		absLat = -absLat;
	}

	var yLatDeg = Math.round( absLat );
	var yLatMin = Math.round(( absLat % 1 ) * 60 );

	// Build return array
	resultArray[0] = xLongDeg;
	resultArray[1] = xLongMin;
	resultArray[2] = xEast;
	resultArray[3] = yLatDeg;
	resultArray[4] = yLatMin;
	resultArray[5] = ySouth;

	return resultArray;
}