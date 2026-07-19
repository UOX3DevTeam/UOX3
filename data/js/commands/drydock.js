// ServUO-style High Seas dry docking. Use 'drydock and target the vessel.

function CommandRegistration()
{
	RegisterCommand( "drydock", 0, true );
}

function command_DRYDOCK( socket, cmdString )
{
	socket.CustomTarget( 0, "Target the High Seas vessel you wish to dry dock." );
}

function onCallback0( socket, target )
{
	var boat = ResolveDryDockBoat( target );
	BeginHighSeasDryDock( socket, boat );
}

function BeginHighSeasDryDock( socket, boat )
{
	var user = socket.currentChar;
	if( !ValidateDryDockOwnerAndRange( socket, user, boat )) return;
	var result = boat.CheckDryDock();
	if( !ReportDryDockResult( socket, result )) return;
	user.SetTempTag( "hsDryDockBoat", boat.serial );
	var g = new Gump();
	g.AddPage( 0 );
	g.AddBackground( 0, 0, 390, 190, 0x0A28 );
	g.AddHTMLGump( 25, 25, 340, 65, false, false,
		"<CENTER><BIG>Dry Dock Vessel</BIG><BR>This will convert the vessel into a deed. Unloaded deployed cannons will be preserved.</CENTER>" );
	g.AddButton( 80, 125, 0x0FA5, 0x0FA7, 1, 0, 1 );
	g.AddHTMLGump( 115, 127, 90, 25, false, false, "Confirm" );
	g.AddButton( 225, 125, 0x0FA5, 0x0FA7, 1, 0, 0 );
	g.AddHTMLGump( 260, 127, 90, 25, false, false, "Cancel" );
	g.Send( user );
	g.Free();
}

function onGumpPress( socket, button, gumpData )
{
	if( button != 1 ) return;
	var user = socket.currentChar;
	var boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsDryDockBoat" )));
	if( !ValidateDryDockOwnerAndRange( socket, user, boat )) return;
	var result = boat.CheckDryDock();
	if( !ReportDryDockResult( socket, result )) return;
	var deedSection = DryDockDeedSection( boat );
	if( deedSection == "" ) { socket.SysMessage( "That vessel cannot be converted into a High Seas deed." ); return; }
	var deed = CreateDFNItem( socket, user, deedSection, 1, "ITEM", true );
	if( !ValidateObject( deed )) return;
	if( !boat.DeleteForDryDock( deed ))
	{
		deed.Delete();
		socket.SysMessage( "The vessel changed before dry docking could finish." );
		return;
	}
	user.SetTempTag( "hsDryDockBoat", null );
	socket.SysMessage( "The vessel has been dry docked and returned to your backpack." );
}

function ResolveDryDockBoat( target )
{
	if( !ValidateObject( target )) return null;
	if( target.isItem && target.IsBoat() ) return target;
	if( ValidateObject( target.multi ) && target.multi.IsBoat() ) return target.multi;
	return null;
}

function ValidateDryDockOwnerAndRange( socket, user, boat )
{
	if( !ValidateObject( boat ) || !boat.IsBoat() ) { socket.SysMessage( "That is not a High Seas vessel." ); return false; }
	var servUOOwner = user.isGM || boat.IsOwner( user ) ||
		( ValidateObject( boat.owner ) && boat.owner.accountNum == user.accountNum );
	if( !servUOOwner ) { socket.SysMessage( "Only the ship's owner may dry dock it." ); return false; }
	if( user.dead ) { socket.SysMessage( "You cannot dry dock a vessel while dead." ); return false; }
	if( user.multi == boat ) { socket.SysMessage( "You must disembark before dry docking the vessel." ); return false; }
	if( !boat.InRange( user, 12 )) { socket.SysMessage( "You are too far away from the vessel." ); return false; }
	return true;
}

function ReportDryDockResult( socket, result )
{
	if( result == 0 ) return true;
	if( result == 1 ) socket.SysMessage( "The ship must be fully repaired before it can be docked." );
	else if( result == 2 ) socket.SysMessage( "Make sure the ship's hold is empty and try again." );
	else if( result == 3 ) socket.SysMessage( "You cannot dock the ship with beings aboard." );
	else if( result == 4 ) socket.SysMessage( "You cannot dock the ship with a cluttered deck." );
	else if( result == 5 ) socket.SysMessage( "You cannot dock the ship with loaded weapons on deck." );
	else socket.SysMessage( "That vessel cannot be dry docked." );
	return false;
}

function DryDockDeedSection( boat )
{
	var multiID = parseInt( boat.id ) - 0x4000;
	if( multiID >= 0x18 && multiID <= 0x1B ) return "orcishgalleondeed";
	if( multiID >= 0x24 && multiID <= 0x27 ) return "gargishgalleondeed";
	if( multiID >= 0x30 && multiID <= 0x33 ) return "tokunogalleondeed";
	if( multiID >= 0x40 && multiID <= 0x43 ) return "britannianshipdeed";
	if( multiID >= 0x50 && multiID <= 0x53 ) return "pumpkinrowboatdeed";
	return "";
}
