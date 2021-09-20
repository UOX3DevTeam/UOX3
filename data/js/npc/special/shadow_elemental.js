// Shadow Iron Elementals are immune to magic
function onSpellTarget( myTarget, pCaster, spellID )
{
	var socket = pCaster.socket;
	if( socket != null )
		pCaster.SysMessage( GetDictionaryEntry( 9056, socket.language )); // That target is immune to magic!
	return 2;
}