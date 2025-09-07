/// <reference path="../../../definitions.d.ts" />
// @ts-check
/** @type { ( pDead: Character, iCorpse: Item ) => boolean } */
function onDeath( pDead, iCorpse )
{
	var masterBrute = CalcCharFromSer( parseInt( pDead.GetTempTag( "masterBruteSerial" )));
	if( ValidateObject( masterBrute ))
	{
		var orcLordCount = masterBrute.GetTempTag( "orcLordsThrown" );
		masterBrute.SetTempTag( "orcLordsThrown", orcLordCount - 1 );
	}
}
