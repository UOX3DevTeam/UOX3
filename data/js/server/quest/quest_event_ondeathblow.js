// Handle NPC kills for quests
/** @type { ( mKilled: Character, mKiller: Character ) => boolean } */
function onDeathBlow( victim, player )
{
	if( !ValidateObject( player ) || !ValidateObject( victim ))
		return true;

	TriggerEvent( 5800, "CreatureKilled", victim, player );

	return true;
}
