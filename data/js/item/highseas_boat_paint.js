// ServUO pub57 High Seas boat paint, permanent paint, and paint remover.
const PAINT_MODE_TEMPORARY = 1;
const PAINT_MODE_PERMANENT = 2;
const PAINT_MODE_REMOVER = 3;

function onUseChecked( user, used )
{
	if( !ValidateObject( user ) || !ValidateObject( used ) || !IsInPack( used, user.pack ))
	{
		if( ValidateObject( user ) && user.socket )
			user.socket.SysMessage( "That must be in your backpack to use it." );
		return false;
	}
	user.socket.tempObj = used;
	user.socket.CustomTarget( 0, used.morex == PAINT_MODE_REMOVER ?
		"Select the ship whose paint you wish to remove." :
		"Select the main mast of the ship you wish to paint." );
	return false;
}

function onCallback0( socket, target )
{
	var paint = socket.tempObj;
	socket.tempObj = null;
	var user = socket.currentChar;
	if( !ValidateObject( user ) || !ValidateObject( paint ) || !IsInPack( paint, user.pack ))
		return;

	var boat = ResolveBoat( target );
	if( !ValidateObject( boat ) || !boat.IsBoat() || user.multi != boat )
	{
		socket.SysMessage( "You must be aboard the ship you wish to paint." );
		return;
	}
	if( boat.GetSecurityLevel( user ) < 5 )
	{
		socket.SysMessage( "You must be the captain to paint this ship!" );
		return;
	}
	if( !ValidateObject( target ) || !target.InRange( user, 5 ) || ( target != boat && target.multi != boat ))
	{
		socket.SysMessage( "You must target the main mast of the ship you wish to paint." );
		return;
	}

	var mode = paint.morex;
	if( mode == PAINT_MODE_REMOVER )
	{
		if( boat.RemoveShipPaint() )
		{
			socket.SysMessage( "You remove the temporary paint from the ship." );
			paint.Delete();
		}
		else
			socket.SysMessage( "This ship has no removable paint." );
		return;
	}

	var result = boat.PaintShip( paint.color, mode == PAINT_MODE_PERMANENT );
	if( result == 1 )
	{
		socket.SysMessage( "You apply a coat of paint to the ship." );
		paint.Delete();
	}
	else if( result == 2 )
	{
		socket.SysMessage( "The ship has been permanently painted." );
		paint.Delete();
	}
	else if( result == -1 )
		socket.SysMessage( "That paint is incompatible with the ship's current paint. Use paint remover first." );
	else if( result == -2 )
		socket.SysMessage( "This ship already has the maximum of four coats of paint." );
	else
		socket.SysMessage( "That ship cannot be painted." );
}

function ResolveBoat( target )
{
	if( !ValidateObject( target ))
		return null;
	if( target.IsBoat && target.IsBoat() )
		return target;
	return target.multi;
}

function IsInPack( item, pack )
{
	var parent = item.container;
	while( ValidateObject( parent ))
	{
		if( parent == pack )
			return true;
		parent = parent.container;
	}
	return false;
}
