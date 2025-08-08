// This script creates and decorates an orc camp when an item with the script attached is created
// Associated Orc NPCs (or eventual Prisoner escortees) are spawned separately using regional spawn system
function onCreateDFN( baseCampItem, objType )
{
	// Start a timer to make sure item has been placed correctly in the world before we try to fetch its coordinates
	baseCampItem.StartTimer( 10, 1, 5501 );
}

function onTimer( baseCampItem, timerID )
{
	var multiID = baseCampItem.morex;
	/*var campMulti = CreateBaseMulti( 0x1f6d, baseCampItem.x, baseCampItem.y, baseCampItem.z, baseCampItem.worldnumber, baseCampItem.instanceID );
	if( !ValidateObject( campMulti ) || !campMulti.IsMulti() )
	{
		Console.Error( "Unable to create camp multi from base camp item with section ID '" + baseCampItem.sectionID + "'!" ); // Unable to camp multi!
		return;
	}

	// Keep track of multi on baseCampItem
	baseCampItem.SetTag( "campMultiSer", campMulti.serial.toString() );*/

	// Multi seems to be valid, let's continue decorating it
	// First, add some garbage on the ground, and a firepit
	var groundGarbage = CreateDFNItem( null, null, "0x10ee", 1, "ITEM", false, 0, baseCampItem.worldnumber, baseCampItem.instanceID );
	var firepit = CreateDFNItem( null, null, "0x0fac", 1, "ITEM", false, 0, baseCampItem.worldnumber, baseCampItem.instanceID );
	if( !ValidateObject( groundGarbage ) || !ValidateObject( firepit ))
	{
		Console.Error( "Unable to create camp decorative item for camp with section ID '" + baseCampItem.sectionID + "'!" ); // Unable to create camp decorative item
		return;
	}

	// Keep track of the decorative item on the baseCampItem
	baseCampItem.SetTag( "groundGarbageSer", groundGarbage.serial.toString() );
	baseCampItem.SetTag( "firepitSer", firepit.serial.toString() );

	// Position the decorative items correctly relative to baseCampItem's position, and make them immovable
	groundGarbage.x = baseCampItem.x;
	groundGarbage.y = baseCampItem.y;
	groundGarbage.z = GetMapElevation( groundGarbage.x, groundGarbage.y, groundGarbage.worldnumber ); //baseCampItem.z;
	groundGarbage.movable = 2;
	firepit.x = baseCampItem.x;
	firepit.y = baseCampItem.y + 7;
	firepit.z = GetMapElevation( firepit.x, firepit.y, firepit.worldnumber ); //baseCampItem.z;
	firepit.movable = 2;

	// Second, add some items to the firepit
	var decoID1 = "";
	var decoID2 = "";
	switch( RandomNumber( 1, 3 ))
	{
		default:
		case 1: // Campfire + Cauldron
			decoID1 = "0x0974";
			decoID2 = "0x0de3";
			break;
		case 2: // Rabbit on a spit
			decoID1 = "0x1e95";
			break;
		case 3: // Chicken on a spit
			decoID1 = "0x1e94";
			break;
	}

	if( decoID1 != "" )
	{
		var decoItem1 = CreateDFNItem( null, null, decoID1, 1, "ITEM", false, 0, baseCampItem.worldnumber, baseCampItem.instanceID );
		if( !ValidateObject( decoItem1 ))
		{
			Console.Error( "Unable to create item for camp with section ID '" + baseCampItem.sectionID + "'!" ); // Unable to create camp item
			return;
		}

		// Keep track of the decorative item on the baseCampItem
		baseCampItem.SetTag( "decoItem1Ser", decoItem1.serial.toString() );

		// Position the item correctly relative to baseCampItem's position
		decoItem1.x = firepit.x;
		decoItem1.y = firepit.y;
		decoItem1.z = firepit.z + 1;
		decoItem1.movable = 2;
	}

	if( decoID2 != "" )
	{
		var decoItem2 = CreateDFNItem( null, null, decoID2, 1, "ITEM", false, 0, baseCampItem.worldnumber, baseCampItem.instanceID );
		if( !ValidateObject( decoItem2 ))
		{
			Console.Error( "Unable to create item for camp with section ID '" + baseCampItem.sectionID + "'!" ); // Unable to create camp item
			return;
		}

		// Keep track of the decorative item on the baseCampItem
		baseCampItem.SetTag( "decoItem2Ser", decoItem2.serial.toString() );

		// Position the item correctly relative to baseCampItem's position
		decoItem2.x = firepit.x;
		decoItem2.y = firepit.y;
		decoItem2.z = firepit.z;
		decoItem2.movable = 2;
	}

	// Add gruesome standard
	var gruesomeStandard = CreateDFNItem( null, null, "0x0428", 1, "ITEM", false, 0, baseCampItem.worldnumber, baseCampItem.instanceID );
	if( !ValidateObject( gruesomeStandard ))
	{
		Console.Error( "Unable to create camp decorative item for camp with section ID '" + baseCampItem.sectionID + "'!" ); // Unable to create camp decorative item
		return;
	}

	// Keep track of the decorative item on the baseCampItem
	baseCampItem.SetTag( "gruesomeStandardSer", gruesomeStandard.serial.toString() );

	// Position the decorative items correctly relative to baseCampItem's position
	gruesomeStandard.x = baseCampItem.x - 5;
	gruesomeStandard.y = baseCampItem.y - 4;
	gruesomeStandard.z = GetMapElevation( gruesomeStandard.x, gruesomeStandard.y, gruesomeStandard.worldnumber ); //baseCampItem.z;
	gruesomeStandard.movable = 2;
}

// If base camp item is deleted, delete all associated decorative items as well
function onDelete( baseCampItem, objType )
{
	// Delete the actual multi
	/*var campMulti = CalcItemFromSer( parseInt( baseCampItem.GetTag( "campMultiSer" )));
	if( ValidateObject( campMulti ))
	{
		campMulti.Delete();
	}*/

	// Delete the ground garbage
	var groundGarbage = CalcItemFromSer( parseInt( baseCampItem.GetTag( "groundGarbageSer" )));
	if( ValidateObject( groundGarbage ))
	{
		groundGarbage.Delete();
	}

	// Delete the firepit
	var firepit = CalcItemFromSer( parseInt( baseCampItem.GetTag( "firepitSer" )));
	if( ValidateObject( firepit ))
	{
		firepit.Delete();
	}

	// Delete the first decorative item
	var decoItem1 = CalcItemFromSer( parseInt( baseCampItem.GetTag( "decoItem1Ser" )));
	if( ValidateObject( decoItem1 ))
	{
		decoItem1.Delete();
	}

	// Delete the second decorative item
	var decoItem2 = CalcItemFromSer( parseInt( baseCampItem.GetTag( "decoItem2Ser" )));
	if( ValidateObject( decoItem2 ))
	{
		decoItem2.Delete();
	}

	// Delete the gruesome standard
	var gruesomeStandard = CalcItemFromSer( parseInt( baseCampItem.GetTag( "gruesomeStandardSer" )));
	if( ValidateObject( gruesomeStandard ))
	{
		gruesomeStandard.Delete();
	}

	return true;
}