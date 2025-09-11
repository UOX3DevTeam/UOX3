function onCreateDFN( objMade, objType ) 
{
	if( objType == 0 ) 
	{
		// Start a timer to replenish charges every 5 minutes
		objMade.StartTimer( 5000, 0, 5063 ); // 300000 ms = 5 minutes
	}
}

function onTimer( timerObj, timerID )
{
	if( timerID == 0 )
	{
		// Ensure the pickaxe regains a charge only if it has fewer than 20 uses left
		if( timerObj.usesLeft < 20 )
		{
			timerObj.usesLeft += 1; // Regain one charge
		}
		// Restart the timer to continue regenerating charges
		timerObj.StartTimer( 5000, 0, 5063 );
	}
}