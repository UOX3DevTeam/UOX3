function onCreateDFN( objMade, objType ) 
{
	if( objType == 0 )
	{
		objMade.SetRandomName( 2009 );
		var newName = objMade.name;

		objMade.name = newName + " Jack O' Lantern";
	}
}