/// <reference path="../../definitions.d.ts" />
// @ts-check
/** @type { ( mChar: Character, oldFacet: number, newFacet: number ) => boolean } */
function onFacetChange( mChar, oldFacet, newFacet )
{
	// Add a specific script to all characters that switch to the Trammel facet,
	// but remove said script if character moves to any other facet
	switch( newFacet )
	{
		case 0: // Felucca
			mChar.RemoveScriptTrigger( 2507 );
			break;
		case 1: // Trammel
		case 2: // Ilsh
		case 3: // Malas
		case 4: // Tokuno
		case 5: // Eodon
			mChar.AddScriptTrigger( 2507 );
			break;
		default:
			mChar.RemoveScriptTrigger( 2507 );
			break;
	}

	// Return true allows the facet change to happen like normal
	return true;
}
