var AIR_MS = 300000; // 5 minutes

function isDead( item )
{
	return (( item.GetTag( "dead" ) === 1 ) || ( item.id === 0x3B0F ));
}

function killFish( item )
{
	if( !ValidateObject( item ))
		return;

	if( isDead( item )) 
		return;

	item.id = 0x3B0C;
	item.color = item.color | 0;
	item.SetTag( "dead", 1 );

	item.SetTag( "fishAirActive", null );
}

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( objMade, objType )
{
	if( objType == 0 )
	{
		objMade.SetTag( "fishAirActive", 1 );
		objMade.StartTimer( AIR_MS, 1, true );
	}
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( iOwner, timerID )
{
	if(( iOwner.GetTag( "fishAirActive" ) == null ))
		return;

	killFish( iOwner );
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	var live = !isDead( myObj );
	var unusualByID = ( myObj.id > 0x3B0F );
	var hasHue = (( myObj.color | 0 ) !== 0 );

	var desc;
	if( unusualByID )
		desc = live ? "A very unusual aquarium creature" : "A very unusual dead aquarium creature";
	else if( hasHue )
		desc = live ? "An aquarium creature of unusual color" : "A dead aquarium creature of unusual color";
	else
		desc = live ? "An aquarium creature" : "A dead aquarium creature";

	var tooltipText = [desc];

	if( live && ( myObj.GetTag( "fishAirActive" ) === 1 ))
		tooltipText.push( "Gasping for air" );

	myObj.SetTempTag( "clilocTooltip", 1042971 ); // ~1_NOTHING~
	return tooltipText.join( "\n" );
}
