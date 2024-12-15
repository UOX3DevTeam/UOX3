function ResurrectFamePenalty( pUser )
{
	if( pUser.fame > 0 )
	{
		// Calculate 10% of the fame
		var amount = Math.floor( pUser.fame / 10 );
		pUser.fame -= amount;
	}
}

function ResurrectKarmaPenalty( pUser )
{
	if( pUser.karma > 0 )
	{
		// Calculate 10% of the karma
		var amount = Math.floor( pUser.karma / 10 );
		pUser.karma -= amount;
	}
}

function ResurrectMurderPenalty( pUser )
{
	// Ensure the pUser has committed at least 5 short-term murders
	if( pUser.murdercount >= 5 ) 
	{
		// Calculate the loss percentage
		var loss = ( 100.0 - ( 4.0 + ( pUser.murdercount / 5.0 ))) / 100.0;

		// Clamp the loss percentage between 85% and 95%
		if( loss < 0.85 )
		{
			loss = 0.85; // Minimum 15% reduction
		}
		else if( loss > 0.95 )
		{
			loss = 0.95; // Maximum 5% reduction
		}

		// Apply penalties to strength, intelligence, and dexterity
		if( pUser.strength * loss > 10 )
		{
			pUser.strength = RandomNumber( 10, pUser.strength * loss );
		}
		if( pUser.intelligence * loss > 10 )
		{
			pUser.intelligence = RandomNumber( 10, pUser.intelligence * loss );
		}
		if( pUser.dexterity * loss > 10 )
		{
			pUser.dexterity = RandomNumber( 10, pUser.dexterity * loss );
		}
	}
}

function ResurrectSkillPenalty( pUser )
{
	// Define the reduction range (89% to 91%)
	var reductionMin = 89;
	var reductionMax = 91;

	var originalSkills = []; // Array to store original skill values

	// Apply a 10% reduction to all 64 skills
	for( var skillID = 0; skillID < 64; skillID++ )
	{
		var baseSkill = pUser.Skills[ skillID ]; // Get skill value
		var reducedSkill = RandomNumber(
			(baseSkill * reductionMin) / 100,
			(baseSkill * reductionMax) / 100
		);
		originalSkills.push( baseSkill ); // Store original skill value
		pUser.Skills[ skillID ] = reducedSkill; // Apply the reduced skill value
	}

	pUser.SetTag( "originalSkills", originalSkills.join( "," ));

	// Start the recovery timer
	pUser.StartTimer( 15000, 0, false ); // 15 seconds interval, calls back to the current script
	pUser.SetTag( "recoveryCount", 0 ); // Track recovery intervals
}

// Recovery handler triggered by StartTimer
function onTimer( timerObj, timerID )
{
	if( timerID == 0 )
	{
		var recoveryCount = timerObj.GetTag( "recoveryCount" );
		var maxRecoveryCount = 100; // 100 intervals = 25 minutes

		// Parse the original skills from the tag
		var originalSkills = timerObj.GetTag( "originalSkills" ).split( "," );

		// Iterate through all skills and gradually restore
		for( var skillID = 0; skillID < 64; skillID++ )
		{
			var reducedSkill = timerObj.Skills[skillID];
			var originalSkill = parseInt( originalSkills[skillID] );

			if( reducedSkill < originalSkill )
			{
				// Randomize recovery by adding between 0.1% and 0.11% of the original value
				var recoveryStep = RandomNumber(
					(originalSkill * 0.1) / 100,
					(originalSkill * 0.11) / 100
				);
				timerObj.Skills[ skillID ] = Math.min( originalSkill, reducedSkill + recoveryStep );
			}
		}

		recoveryCount++;
		timerObj.SetTag( "recoveryCount", recoveryCount );

		// If recovery is not complete, restart the timer
		if( recoveryCount < maxRecoveryCount )
		{
			timerObj.StartTimer( 15000, 0, false ); // 15 seconds interval
		}
		else
		{
			// Recovery complete
			timerObj.SetTag( "recoveryCount", null );
			timerObj.SetTag( "originalSkills", null );
		}
	}
}

function _restorecontext_() {}