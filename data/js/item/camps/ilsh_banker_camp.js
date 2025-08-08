// This script creates and decorates a banker gypsy camp when an item with the script attached is created
// Associated Banker NPCs are spawned using regional spawn system
function onCreateDFN( baseCampItem, objType )
{
	// Start a timer to make sure item has been placed correctly in the world before we try to fetch its coordinates
	baseCampItem.StartTimer( 10, 1, 5502 );
}

function onTimer( baseCampItem, timerID )
{
	var multiID = baseCampItem.morex;
	var campMulti = CreateBaseMulti( 0x1f6, baseCampItem.x, baseCampItem.y, baseCampItem.z, baseCampItem.worldnumber, baseCampItem.instanceID );
	if( !ValidateObject( campMulti ) || !campMulti.IsMulti() )
	{
		Console.Error( "Unable to create camp multi from base camp item with section ID '" + baseCampItem.sectionID + "'!" ); // Unable to camp multi!
		return;
	}

	// Keep track of multi on baseCampItem
	baseCampItem.SetTag( "campMultiSer", campMulti.serial.toString() );

	// Multi seems to be valid, let's continue decorating it
	// First, add two gates to the wagon
	var gateWest = CreateDFNItem( null, null, "0x0839", 1, "ITEM", false, 0, baseCampItem.worldnumber, baseCampItem.instanceID );
	var gateEast = CreateDFNItem( null, null, "0x083b", 1, "ITEM", false, 0, baseCampItem.worldnumber, baseCampItem.instanceID );
	if( !ValidateObject( gateWest ) || !ValidateObject( gateEast ))
	{
		Console.Error( "Unable to create camp decorative item for camp with section ID '" + baseCampItem.sectionID + "'!" ); // Unable to create camp decorative item
		return;
	}

	// Keep track of the decorative item on the baseCampItem
	baseCampItem.SetTag( "gateWestSer", gateWest.serial.toString() );
	baseCampItem.SetTag( "gateEastSer", gateEast.serial.toString() );

	// Position the gates correctly relative to baseCampItem's position
	gateWest.x = baseCampItem.x - 4;
	gateWest.y = baseCampItem.y + 4;
	gateWest.z = baseCampItem.z + 7;
	gateWest.movable = 2;
	gateEast.x = baseCampItem.x - 3;
	gateEast.y = baseCampItem.y + 4;
	gateEast.z = baseCampItem.z + 7;
	gateEast.movable = 2;

	// Second, add the bank sign
	var bankSign = CreateDFNItem( null, null, "0x0c0c", 1, "ITEM", false, 0, baseCampItem.worldnumber, baseCampItem.instanceID );
	if( !ValidateObject( bankSign ))
	{
		Console.Error( "Unable to create sign for camp with section ID '" + baseCampItem.sectionID + "'!" ); // Unable to create camp sign
		return;
	}

	// Keep track of the decorative sign on the baseCampItem
	baseCampItem.SetTag( "bankSignSer", bankSign.serial.toString() );

	// Position the sign correctly relative to baseCampItem's position
	bankSign.x = baseCampItem.x - 5;
	bankSign.y = baseCampItem.y + 5;
	bankSign.z = baseCampItem.z -4;
	bankSign.movable = 2;
}

// If base camp item is deleted, delete all associated decorative items as well
function onDelete( baseCampItem, objType )
{
	// Delete the actual multi
	var campMulti = CalcItemFromSer( parseInt( baseCampItem.GetTag( "campMultiSer" )));
	if( ValidateObject( campMulti ))
	{
		campMulti.Delete();
	}

	// Delete the west gate
	var gateWest = CalcItemFromSer( parseInt( baseCampItem.GetTag( "gateWestSer" )));
	if( ValidateObject( gateWest ))
	{
		gateWest.Delete();
	}

	// Delete the east gate
	var gateEast = CalcItemFromSer( parseInt( baseCampItem.GetTag( "gateEastSer" )));
	if( ValidateObject( gateEast ))
	{
		gateEast.Delete();
	}

	// Delete the bank sign
	var bankSign = CalcItemFromSer( parseInt( baseCampItem.GetTag( "bankSignSer" )));
	if( ValidateObject( bankSign ))
	{
		bankSign.Delete();
	}

	return true;
}