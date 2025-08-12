/// <reference path="../../definitions.d.ts" />
// @ts-check
// Immortality! Character can still take damage, but will never die
/** @type { ( mKilled: Character, mKiller: Character ) => void } */
function onDeathBlow( pKilled, pKiller )
{
	pKilled.health++;
	pKilled.TextMessage( "*This character is immortal, and cannot die*", true, 0x096a );
	return false;
}
