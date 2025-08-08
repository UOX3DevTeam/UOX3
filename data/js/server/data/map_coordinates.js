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
				return resultArray;
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
				return resultArray;
			}
			break;
		default:
			break;
	}

	// Calculate the distance from the center in tiles
	let yDistance = yCoord - yCenter;
	let xDistance = xCoord - xCenter;

	// Adjust for the wrap-around
	yDistance = (( yDistance + yHeight / 2 + yHeight ) % yHeight ) - yHeight / 2;
	xDistance = (( xDistance + xWidth / 2 + xWidth ) % xWidth ) - xWidth / 2;

	// Convert the distance to a value between -0.5 and 0.5
	let latitudeValue = yDistance / yHeight;
	let longitudeValue = xDistance / xWidth;

	// Convert the value into degrees
	let latitude = latitudeValue * 360;
	let longitude = longitudeValue * 360;

	// Determine the cardinal directions
	let latitudeCardinal = ( latitude < 0 ) ? true : false; // true = N, false = S
	let longitudeCardinal = ( longitude < 0 ) ? true : false; // true = W, false = E

	// Make the degrees positive and less than or equal to 180
	latitude = Math.abs( latitude );
	longitude = Math.abs( longitude );

    // Normalize the degrees and find the minutes
	let latitudeMinutes = Math.round(( latitude % 1 ) * 60 );
	let longitudeMinutes = Math.round(( longitude % 1 ) * 60 );

	let latitudeDegrees = Math.floor( latitude );
	let longitudeDegrees = Math.floor( longitude );

	return [longitudeDegrees, longitudeMinutes, longitudeCardinal, latitudeDegrees, latitudeMinutes, latitudeCardinal];
}