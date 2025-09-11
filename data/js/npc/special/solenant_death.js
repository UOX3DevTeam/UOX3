function onDeath( myNPC )
{
	var parentItem = CalcItemFromSer( parseInt( myNPC.GetTag( "parentSerial" )));
	if( ValidateObject( parentItem ))
	{
		// Reduce spawn count on parent item, since NPC died
		parentItem.SetTag( "spawnCount", parentItem.GetTag( "spawnCount" ) - 1 );
	}
}