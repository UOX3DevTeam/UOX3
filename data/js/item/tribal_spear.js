// Very high chance of tribal spear breaking upon being picked up from Savage's corpse
function onPickup( iPickedUp, pGrabber )
{
	var pSock = pGrabber.socket;
	if( pSock && pSock.pickupSpot == 3 )
	{
		var iCont = iPickedUp.container;
		if( ValidateObject( iCont ) && iCont.corpse && ( iCont.amount == 0x00b9 || iCont.amount == 0x00ba ))
		{
			if( RandomNumber( 1, 100 ) <= 95 ) // 95% chance of breaking when being looted
			{
				iCont.StaticEffect( 0x37b9, 10, 16 );
				pGrabber.TextMessage( GetDictionaryEntry( 1682, pSock.language ), false, 0x096a ); // The Tribal Spear breaks apart in your hands as you pick it up!
				iPickedUp.Delete();
				return false;
			}
			else
			{
				// Let's remove the script, no longer needed
				iPickedUp.RemoveScriptTrigger( 5038 );
			}
		}
	}

	return true;
}