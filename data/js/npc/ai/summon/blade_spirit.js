// This event runs instead of regular, hard-coded target selection for summoned Blade Spirits

function onAICombatTarget( pChar, pTarget )
{
	if( !ValidateObject( pTarget ))
		return false;

	// Check for Facet Ruleset - don't allow target if facet restricts BS targeting
	if( DoesEventExist( 2507, "FacetRuleBladeSpiritTarget" ))
	{
		if( !TriggerEvent( 2507, "FacetRuleBladeSpiritTarget", pChar, pTarget ))
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

	// Blade Spirits are more likely to attack a nearby target with high str/tactics than a far away target with low str/tactics
	var attackWeight = Math.round(( pTarget.strength + ( pTarget.skills.tactics / 10 )) / Math.max( pChar.DistanceTo( pTarget ), 1 ));
	if( RandomNumber( 1, 200 ) >= attackWeight )
		return false;

	// Otherwise, assume valid target
	return true;
}
