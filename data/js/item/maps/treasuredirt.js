/// <reference path="../../definitions.d.ts" />
// @ts-check
/** @type { ( sourceObj: BaseObject, CharInRange: Character, rangeToChar: number, oldCharX: number, oldCharY: number ) => boolean } */
function onMoveDetect( iDirt, pChar, rangeToChar, oldCharX, oldCharY ) 
{
	var socket = pChar.socket;
	var dirtDiggerChar = CalcCharFromSer(parseInt(iDirt.GetTempTag( "diggerCharSer" )));
	if( !ValidateObject( pChar ) || !ValidateObject( dirtDiggerChar ) || pChar != dirtDiggerChar )
		return false;

	if( pChar.x != oldCharX || pChar.y != oldCharY )
	{
		dirtDiggerChar.SysMessage( GetDictionaryEntry( 5719, socket.language ) ); // You cannot move around while digging up treasure. You will need to start digging anew.
		KillTreasureEvent( iDirt, dirtDiggerChar );
		return;
	}
}

function KillTreasureEvent( iDirt, dirtDiggerChar )
{
	var dirtChestItem = CalcItemFromSer( parseInt( iDirt.GetTag( "tChestSer" )));
	iDirt.KillTimers( 5400 );
	dirtDiggerChar.SetTempTag( "digging", null );
	if( ValidateObject( dirtChestItem ))
	{
		dirtChestItem.Delete();
	}
	iDirt.Delete();
}
