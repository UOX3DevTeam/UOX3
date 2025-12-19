function onDropItemOnItem( iDropped, cDropper, iDroppedOn )
{
	if( !ValidateObject( iDropped ) || !ValidateObject( cDropper ) || !ValidateObject( iDroppedOn ))
	{
		return 1;
	}

	var questFlag = iDropped.GetTag( "HonestyQuest" );
	if( questFlag !== 1 && questFlag !== "1" )
	{
		return 1;
	}

	var boxTown = iDroppedOn.GetTag( "HonestyTownBox" );
	if( boxTown === null || boxTown === undefined )
	{
		return 1;
	}

	TriggerEvent( 8010, "Honesty_TurnInLostItem", cDropper, iDropped, boxTown.toString() );
	return 2;
}
