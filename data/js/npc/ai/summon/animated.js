// This event runs instead of regular, hard-coded target selection for summoned Energy Vortex

function onAICombatTarget( pChar, pTarget )
{
	var getOwner = CalcCharFromSer(parseInt(pChar.GetTag("ownerSerial")));
	if( !ValidateObject( pTarget ))
		return false;

	// Check for Facet Ruleset - don't allow target if facet restricts EV targeting
	if( DoesEventExist( 2507, "FacetRuleEnergyVortexTarget" ))
	{
		if( !TriggerEvent( 2507, "FacetRuleEnergyVortexTarget", pChar, pTarget ))
		{
			return false;
		}
	}

	// Ignore targets that are in safe zones (Are these already excluded in code?)
	var targetRegion = pTarget.region;
	if( targetRegion.isSafeZone )
	{
		return false;
	}
	
	if (pChar.GetTag("animated") && pTarget.getOwner)
		return false;

	// Otherwise, assume valid target
	return true;
}

function onSpeech(strSaid, pTalking, pTalkingTo)
{
	if (pTalkingTo.GetTag("animated")) 
	{
		return 1;
	}
}

function onTalk(myChar, mySpeech)
{

	if (myChar.GetTag("animated"))
		return false;
	else
		return true;
}

function onTimer(mChar, timerID)
{
	if (!ValidateObject(mChar) || mChar.dead)
		return;

	switch (timerID) 
	{
		case 1: mChar.Delete();
	}
}
