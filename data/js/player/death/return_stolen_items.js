// This script handles the return of stolen items to their owners in the event of
// player dying within X seconds (see stealing.js) of stealing the items

function onDeath( pDead, iCorpse )
{
	// Loop through items on the corpse, and return stolen items to their original
	// owners if they have a reference to original owner and it's been
	// less than X seconds since they were stolen
	for( var toCheck = iCorpse.FirstItem(); !iCorpse.FinishedItems(); toCheck = iCorpse.NextItem() )
	{
		var origOwnerSerial = toCheck.GetTempTag( "stolenItemOwner" );
		if( origOwnerSerial )
		{
			var origOwner = CalcCharFromSer( parseInt( origOwnerSerial ));
			if( ValidateObject( origOwner ))
			{
				var stolenTime = parseInt( toCheck.GetTempTag( "stolenAtTime" ));
				if( stolenTime && GetCurrentClock() - stolenTime < 30000 )
				{
					// Return stolen item to original owner
					var origOwnerPack = origOwner.pack;
					if( ValidateObject( origOwnerPack ))
					{
						toCheck.container = origOwnerPack;
						toCheck.PlaceInPack();
					}
				}
			}
		}
	}

	// Allow other scripts with onDeath to also run
	return false;
}