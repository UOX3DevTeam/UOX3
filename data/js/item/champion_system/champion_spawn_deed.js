function onCreateDFN( objMade, objType )
{
	objMade.StartTimer( 100, 1, 7502 );
}

function onTimer( objMade, timerID )
{
	var houseID = objMade.morex || 1000;

	var x = objMade.x;
	var y = objMade.y;
	var z = objMade.z;
	var world = objMade.worldNumber;
	var instance = objMade.instanceID;
	CreateHouse( houseID, x, y, z, world, instance, 0, false );
}