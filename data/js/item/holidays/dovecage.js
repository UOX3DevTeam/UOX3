/// <reference path="../../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
		return false;

	if( pUser.InRange( iUsed, 3 ))
	{
		pUser.SysMessage( "I cannot reach that." );
		return false;
	}

	pUser.SoundEffect( 1663, true );
	return false;
}

function onCreateDFN( objMade, objType ) 
{
	if( objType == 0 )
	{
		objMade.SetRandomName( 2014 );
		var newName = objMade.name;

		objMade.name = "A cage with a single " + newName + " dove";
	}
}
