// Alternative castle gate opener
// Last Updated: 12/11/2021
//
// The script will do a search for gates (item type 3 - IT_CASTLEGATE)
// with same MORE (part 1) value as switch, within the specified distance
// of the switch/lever, as determined by the switch's MORE (part 1) value,
// and will open (or close) each gate it finds.
//
// The gates use MOREX (part 1) value of 1 to indicate that they are raised,
// a MOREX (part 1) value of 0 to indicate that they are lowered.
//
// The gates are animated up/down to max/min heights based on MOREX (part 2, for min)
// and MOREX (part 3, for max) values.

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	switch( iUsed.id )
	{
		case 0x108f: // Switch east, up
		case 0x1091: // Switch south, down
		case 0xa2ba: // Fancy switch east, down
		case 0xa2bc: // Fancy switch south, down
			iUsed.id++;
			break;
		case 0x1090: // Switch east, down
		case 0x1092: // Switch south, up
		case 0xa2bb: // Fancy switch east, up
		case 0xa2bd: // Fancy switch south, up
			iUsed.id--;
			break;
		case 0x108c: // Lever, west
		case 0x1093: // Lever, north
			iUsed.id += 2;
			break;
		case 0x108e: // Lever, east
		case 0x1095: // Lever, south
			iUsed.id -= 2;
			break;
	}
	iUsed.SoundEffect( 0x04a, true );

	var searchRange = parseInt( iUsed.GetMoreVar( "morex", 1 ));
	var iCount = AreaItemFunction( "SearchForGate", iUsed, searchRange );
	if( iCount > 0 && socket )
	{
		socket.SysMessage( 9106, socket.language ); // You hear the sound of a gate moving nearby.
	}
} 

function SearchForGate( iUsed, trgItem )
{
	if( !ValidateObject( trgItem ) || trgItem.GetTempTag( "isBusy" ))
		return false;

	var iUsedMore = iUsed.more;
	var trgItemMore = trgItem.more;
	if( trgItem.type == 3 && iUsedMore == trgItemMore )
	{
		iUsed.TextMessage( trgItem.type );
		iUsed.TextMessage( trgItem.more );
		if( parseInt( trgItem.GetMoreVar( "morex", 1 )) == 0 )
		{
			trgItem.SetTempTag( "isBusy", true );
			trgItem.SetMoreVar( "morex", 1, 1 );
			trgItem.StartTimer( 50, 1, true );
			trgItem.SoundEffect( 0x0f0, true );
		}
		else
		{
			trgItem.SetTempTag( "isBusy", true );
			trgItem.SetMoreVar( "morex", 1, 0 );
			trgItem.StartTimer( 50, 2, true );
			trgItem.SoundEffect( 0x0ef, true );
		}
		return true;
	}

	return false;
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	var minHeight = parseInt( timerObj.GetMoreVar( "morex", 2 ));
	var maxHeight = parseInt( timerObj.GetMoreVar( "morex", 3 ));
	if( timerID == 1 && timerObj.z < maxHeight )
	{
		timerObj.z++
		timerObj.StartTimer( 50, 1, true );
		return;
	}
	else if( timerID == 2 && timerObj.z > minHeight )
	{
		timerObj.z--;
		timerObj.StartTimer( 50, 2, true );
		return;
	}
	timerObj.SetTempTag( "isBusy", false );
}