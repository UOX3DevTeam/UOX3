function onDeath( myNPC )
{
	var parentItem = CalcItemFromSer( parseInt( myNPC.GetTag( "parentSerial" )));
	if( ValidateObject( parentItem ))
	{
		// Reduce spawn count on parent item, since NPC died
		parentItem.SetTag( "spawnCount", parentItem.GetTag( "spawnCount" ) - 1 );
	}
}

function onDelete( objDestroyed, objType )
{
	var parentItem = CalcItemFromSer( parseInt( objDestroyed.GetTag( "parentSerial" )));
	if( objType == 1 && ValidateObject( objDestroyed ) && !objDestroyed.dead )
	{
		// Reduce spawn count on parent item, since NPC died
		parentItem.SetTag( "spawnCount", parentItem.GetTag( "spawnCount" ) - 1 );
	}
}
