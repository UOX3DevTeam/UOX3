// This event runs instead of regular, hard-coded target selection for summoned Energy Vortex

function onAICombatTarget( pChar, pTarget )
{
	if( !ValidateObject( pTarget ))
		return false;

	// Check for Facet Ruleset - don't allow target if facet restricts EV targeting
	if( DoesEventExist( 2507, "FacetRuleEnergyVortexTarget" )
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

	// Energy Vortex is more likely to attack a nearby target with high int/magery than a far away target with low int/magery
	var attackWeight = Math.round(( pTarget.intelligence + ( pTarget.skills.magery / 10 )) / Math.max( pChar.DistanceTo( pTarget ), 1 ));
	if( RandomNumber( 1, 200 ) >= attackWeight )
		return false;

	// Otherwise, assume valid target
	return true;
}
