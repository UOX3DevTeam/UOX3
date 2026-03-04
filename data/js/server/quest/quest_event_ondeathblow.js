/// <reference path="../../../definitions.d.ts" />
// @ts-check

// Handle NPC kills for quests
/** @type { ( mKilled: Character, mKiller: Character ) => boolean } */
function onKill( player ,victim )
{
	TriggerEvent( 5800, "CreatureKilled", victim, player );
	return true;
}
