/// <reference path="../../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onCreateDFN( objMade, objType ) 
{
	if( objType == 0 )
	{
		objMade.SetRandomName( 2013 );
		var newName = objMade.name;

		objMade.name = "Jolly Holiday Tree from " + newName;
	}
}