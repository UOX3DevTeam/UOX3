/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	if( myObj.GetTag( "isAquariumDecor" ) === 1 )
	{
		var tooltipText = "An aquarium decoration";
		return tooltipText;
	}
	else if( myObj.GetTag( "isAquariumVac" ) === 1 )
	{
		var tooltipText = "Vacation days 7";
		return tooltipText;
	}
}
