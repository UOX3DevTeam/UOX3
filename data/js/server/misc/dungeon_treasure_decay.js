// This script ensures dungeon treasures will decay after a certain amount of time, so they
// can respawn elsewhere.
function onCreateDFN( objMade, objType )
{
	if( !ValidateObject( objMade ))
		return;

	// Decay the container 10 minutes after spawning
	objMade.decayable = 1;
	objMade.decaytime = 60 * 10; // 10 minutes
}

function onContRemoveItem( iCont, iItem, pChar )
{
	if( !ValidateObject( iCont ) || !ValidateObject( iItem ))
		return;

	// Check if there are other items left in the container
	if( iCont.itemsinside > 1 ) // The last item is iItem; this property doesn't update instantly
	{
		// There are still items inside, prevent it from decaying...
		var timeLeft = Math.floor(( iCont.decaytime - GetCurrentClock() ) / 1000 );
		if( timeLeft < 60 )
		{
			iCont.decaytime = 180; // Set new decay time to 3 minutes
		}
		return;
	}

	// When last item has been removed from the chest, let decay kick in
	iCont.decaytime = 30; // Set it to decay in 60 seconds, so it decays before item respawns kick in
}
