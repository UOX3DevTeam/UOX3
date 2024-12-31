// Handle NPC kills for quests
function onDeathBlow( victim, player )
{
	if( !ValidateObject( player ) || !ValidateObject( victim ))
		return true;

	TriggerEvent( 5800, "onCreatureKilled", victim, player );

	return true;
}

