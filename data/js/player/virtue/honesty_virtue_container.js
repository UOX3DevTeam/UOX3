function onDropItemOnItem( iDropped, cDropper, iDroppedOn )
{
	if( !ValidateObject( iDropped ) ||
		!ValidateObject( cDropper ) ||
		!ValidateObject( iDroppedOn ))
	{
		// Invalid objects; do not block normal handling
		return 1;
	}

	// IMPORTANT: only care about Honesty quest items
	var questFlag = iDropped.GetTag( "HonestyQuest" );
	if( questFlag !== 1 && questFlag !== "1" )
	{
		// Normal item – let default container / pack code handle it
		return 1;
	}

	// Check if the *target* item is a Honesty Lost and Found box
	var boxTown = iDroppedOn.GetTag( "HonestyTownBox" );
	if( boxTown === null || boxTown === undefined )
	{
		// Dropped on a normal pack/container/ground/etc – do NOT
		// process Honesty, just allow the drop
		return 1;
	}

	// At this point we know:
	//   - iDropped is a HonestyQuest item
	//   - iDroppedOn is a HonestyTownBox
	TriggerEvent( 8010, "Honesty_TurnInLostItem", cDropper, iDropped, boxTown.toString() );

	// 2 = we handled it and consumed the dropped item
	return 2;
}