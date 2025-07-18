function onEquipAttempt( pEquipper, iEquipped )
{
	var blockEquip = pEquipper.GetTempTag( "blockEquip" );
	if( blockEquip == true )
	{
		iEquipped.SetTempTag( "charSer", pEquipper.serial.toString() );
		iEquipped.StartTimer( 10 ,1, true );
		return false;
	}

	return true;
}

function onTimer( timerObj, timerID )
{
	var mChar = CalcCharFromSer(parseInt( timerObj.GetTempTag( "charSer" )))
	timerObj.container = mChar.pack;
	timerObj.Refresh();
	timerObj.SetTempTag( "charSer", null );
}