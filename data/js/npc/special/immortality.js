// Immortality! Character can still take damage, but will never die
function onDeathBlow( pKilled, pKiller )
{
	pKilled.health++;
	pKilled.TextMessage( "*This character is immortal, and cannot die*", true, 0x096a );
	return false;
}