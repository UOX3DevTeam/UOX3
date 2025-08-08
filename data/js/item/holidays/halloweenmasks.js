function onCreateDFN( objMade, objType ) 
{
	if( objType == 0 )
	{
		var maskname = "";
		switch( objMade.GetTag( "paintedmask" )) 
		{
			case 1: maskname = "A Evil Clown Mask"; break;
			case 2: maskname = "A Daemon Mask"; break;
			case 3: maskname = "A Plague Mask"; break;
			case 4: maskname = "An Evil Jester Mask"; break;
			case 5: maskname = "A Porcelain Mask"; break;
			default: maskname = "A Mask"; break;
		}
		objMade.SetRandomName( 2009 );
		var newName = objMade.name;
		objMade.name = maskname + " hand painted by " + newName;
	}
}