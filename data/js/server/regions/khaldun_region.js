/// <reference path="../../definitions.d.ts" />
// @ts-check
function onEnterRegion( pEntering, regionEntered )
{
	if( regionEntered == 42 ) // Khaldun
	{
		// Attach script that prevents use of resurrection spells/abilities
		// and teleportation spells to all players who enter Khaldun dungeon
		if( ValidateObject( pEntering ) && pEntering.isChar && !pEntering.npc )
		{
			pEntering.AddScriptTrigger( 4776 );
		}
	}
}

function onLeaveRegion( pLeaving, regionLeft )
{
	if( regionLeft == 42 )
	{
		// Remove the script that prevents resurrection and/or teleportation
		// When player leaves the Khaldun dungeon
		if( ValidateObject( pLeaving ) && pLeaving.isChar && !pLeaving.npc )
		{
			pLeaving.RemoveScriptTrigger( 4776 );
		}
	}
}
