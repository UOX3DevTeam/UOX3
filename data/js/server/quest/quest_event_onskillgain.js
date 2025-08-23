function onSkillGain( pPlayer, skill, skillGainAmount )
{
	TriggerEvent( 5800, "AccelerateSkillGain", pPlayer, skill, skillGainAmount );
	return false;
}

function _restorecontext_() {}