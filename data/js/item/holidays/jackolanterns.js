/// <reference path="../../definitions.d.ts" />
// @ts-check
/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( objMade, objType ) 
{
	if( objType == 0 )
	{
		objMade.SetRandomName( 2009 );
		var newName = objMade.name;

		objMade.name = newName + " Jack O' Lantern";
	}
}
