/// <reference path="../definitions.d.ts" />
// @ts-check

// Provides ship access-level, party/guild, ownership and tillerman controls.

/** @type { ( wheel: Item ) => any } */
function ResolveSecurityBoat( wheel )
{
	if( !ValidateObject( wheel ) || !ValidateObject( wheel.multi ) || !wheel.multi.IsBoat() )
	{
		return null;
	}
	return wheel.multi;
}

/** @type { ( boat: Multi, user: Character ) => any } */
function IsCaptain( boat, user ) { return ValidateObject( boat ) && ValidateObject( user ) && boat.GetSecurityLevel( user ) >= 5; }

/** @type { ( boat: Multi ) => any } */
function IsRowBoat( boat )
{
	if( !ValidateObject( boat ) )
	{
		return false;
	}
	let multiID = parseInt( boat.id ) - 0x4000;
	return ( multiID >= 0x3C && multiID <= 0x3F ) || ( multiID >= 0x50 && multiID <= 0x53 );
}

/** @type { ( socket: Socket, wheel: Item ) => any } */
function onContextMenuRequest( socket, wheel )
{
	let user = socket.currentChar;
	let boat = ResolveSecurityBoat( wheel );
	if( !ValidateObject( boat ) || !ValidateObject( user ) )
	{
		return true;
	}
	let entries = [];
	let aboard = user.multi == boat;
	let level = parseInt( boat.GetSecurityLevel( user ) );
	// Rowboats have no hull damage, repair, manifest, rename, or movable-pilot
	// controls. Their tiller menu contains only Dry Dock, and only while the
	// owner is standing off the boat.
	if( IsRowBoat( boat ) )
	{
		if( !aboard && level >= 5 )
		{
			entries.push( { id : 37, text : 1116520, flags : 0x0000, hue : 0x03E0 } );
		}
		else
		{
			return true;
		}
		TriggerEvent( 18001, "modifyContextMenu", socket, wheel, entries, true );
		return false;
	}
	if( aboard )
	{
		if( level >= 3 )
		{
			entries.push( { id : 31, text : 1116589, flags : 0x0000, hue : 0x03E0 } );
			entries.push( { id : 32, text : 1116590, flags : 0x0000, hue : 0x03E0 } );
		}
		if( level >= 5 )
		{
			entries.push( { id : 33, text : 1111680, flags : 0x0000, hue : 0x03E0 } );
			entries.push( { id : 34, text : 1116729, flags : 0x0000, hue : 0x03E0 } );
			entries.push( { id : 30, text : 1149786, flags : 0x0000, hue : 0x03E0 } );
			entries.push( { id : 36, text : 1060700, flags : 0x0000, hue : 0x03E0 } );
		}
	}
	else if( level >= 5 )
	{
		entries.push( { id : 37, text : 1116520, flags : 0x0000, hue : 0x03E0 } );
	}
	if( entries.length == 0 )
	{
		return true;
	}

	TriggerEvent( 18001, "modifyContextMenu", socket, wheel, entries, true );
	return false;
}

/** @type { ( socket: Socket, wheel: Item, popupEntry: number ) => any } */
function onContextMenuSelect( socket, wheel, popupEntry )
{
	let boat = ResolveSecurityBoat( wheel );
	let user = socket.currentChar;
	if( !ValidateObject( boat ) || !ValidateObject( user ) )
	{
		return false;
	}
	let aboard = user.multi == boat;
	let level = parseInt( boat.GetSecurityLevel( user ) );
	if( IsRowBoat( boat ) )
	{
		if( popupEntry == 37 && !aboard && level >= 5 )
		{
			TriggerEvent( 5098, "BeginHighSeasDryDock", socket, boat );
		}
		return false;
	}
	if( popupEntry == 31 && aboard && level >= 3 )
	{
		TriggerEvent( 5099, "BeginEmergencyRepairs", user, boat );
	}
	else if( popupEntry == 32 && aboard && level >= 3 )
	{
		TriggerEvent( 5099, "RepairShipHull", user, boat );
	}
	else if( popupEntry == 33 && aboard && level >= 5 )
	{
		user.SetTempTag( "hsRenameBoat", boat.serial );
		user.SpeechInput( 1, wheel );
		socket.SysMessage( "What do you wish to name your ship?" );
	}
	else if( popupEntry == 34 && aboard && level >= 5 )
	{
		user.SetTempTag( "hsMoveTillermanBoat", boat.serial );
		socket.CustomTarget( 1, "Where do you wish to station the tillerman?" );
	}
	else if( popupEntry == 30 && aboard && level >= 5 )
	{
		ShowSecurityGump( user, boat );
	}
	else if( popupEntry == 36 && aboard && level >= 5 )
	{
		ShowResetSecurityGump( user, boat );
	}
	else if( popupEntry == 37 && !aboard && level >= 5 )
	{
		TriggerEvent( 5098, "BeginHighSeasDryDock", socket, boat );
	}
	return false;
}

/** @type { ( user: Character, boat: Multi ) => any } */
function ShowResetSecurityGump( user, boat )
{
	user.SetTempTag( "hsResetSecurityBoat", boat.serial );
	let g = new Gump;
	g.AddPage( 0 );
	g.AddBackground( 0, 0, 390, 180, 0x0A28 );
	g.AddHTMLGump( 25, 25, 340, 65, false, false,
		"<BASEFONT COLOR=#FFFFFF><CENTER>Are you sure you wish to clear your ship's access list and restore the default security settings?</CENTER></BASEFONT>" );
	g.AddButton( 80, 120, 0x0FA5, 0x0FA7, 1, 0, 3001 );
	g.AddHTMLGump( 115, 122, 90, 20, false, false, "<BASEFONT COLOR=#FFFFFF>Confirm</BASEFONT>" );
	g.AddButton( 225, 120, 0x0FA5, 0x0FA7, 1, 0, 3002 );
	g.AddHTMLGump( 260, 122, 90, 20, false, false, "<BASEFONT COLOR=#FFFFFF>Cancel</BASEFONT>" );
	g.Send( user );
	g.Free();
}

/** @type { ( level: number ) => any } */
function LevelName( level )
{
	if( level == 1 )
	{
		return "Denied";
	}
	if( level == 2 )
	{
		return "Passenger";
	}
	if( level == 3 )
	{
		return "Crewman";
	}
	if( level == 4 )
	{
		return "Officer";
	}
	if( level == 5 )
	{
		return "Captain";
	}
	return "N/A";
}

/** @type { ( level: number ) => any } */
function LevelCliloc( level )
{
	if( level == 1 )
	{
		return 1149726;
	}
	if( level == 2 )
	{
		return 1149727;
	}
	if( level == 3 )
	{
		return 1149728;
	}
	if( level == 4 )
	{
		return 1149729;
	}
	if( level == 5 )
	{
		return 1149730;
	}
	return 1149725;
}

/** @type { ( level: number ) => any } */
function LevelHue( level )
{
	if( level == 1 )
	{
		return 0x7CE7;
	}
	if( level == 2 )
	{
		return 0x1CFF;
	}
	if( level == 3 )
	{
		return 0x1FE7;
	}
	if( level == 4 )
	{
		return 0x7FE7;
	}
	if( level == 5 )
	{
		return 0x7DE7;
	}
	return 0x5EF7;
}

/** @type { ( g: Gump, x: number, y: number, width: number, cliloc: number, hue: number ) => any } */
function AddLocalized( g, x, y, width, cliloc, hue ) { g.AddXMFHTMLGumpColor( x, y, width, 18, cliloc, false, false, hue ); }

/** @type { ( g: Gump, x: number, y: number, width: number, level: number, selected: boolean ) => any } */
function AddLevel( g, x, y, width, level, selected ) { AddLocalized( g, x, y, width, LevelCliloc( level ), selected ? LevelHue( level ) : 0x7FFF ); }

/** @type { ( g: Gump, x: number, y: number, selected: boolean, button: number ) => any } */
function AddSelectedButton( g, x, y, selected, button )
{
	if( selected )
	{
		g.AddPicture( x, y, 0x0FA6 );
	}
	else
	{
		g.AddButton( x, y, 0x0FA5, 0x0FA7, 1, 0, button );
	}
}

/** @type { ( g: Gump, page: number, x: number, titleCliloc: BaseObject, current: number, buttonBase: number ) => any } */
function AddAccessPanel( g, page, x, titleCliloc, current, buttonBase )
{
	g.AddPage( page );
	g.AddBackground( x, 215, 190, 130, 0x0A3C );
	AddLocalized( g, x + 50, 220, 100, titleCliloc, 0x7FFF );
	let levels = [ 0, 2, 3, 4, 1 ];
	for( let i = 0; i < levels.length; ++i )
	{
		AddSelectedButton( g, x + 10, 243 + i * 18, current == levels[i], buttonBase + levels[i] );
		AddLevel( g, x + 45, 245 + i * 18, 140, levels[i], current == levels[i] );
	}
}

/** @type { ( user: Character, boat: Multi ) => any } */
function ShowSecurityGump( user, boat )
{
	if( !IsCaptain( boat, user ) )
	{
		return;
	}
	user.SetTempTag( "hsSecurityBoat", boat.serial );
	let g = new Gump;
	g.AddPage( 0 );
	g.AddBackground( 0, 0, 320, 385, 0x0A3C );
	AddLocalized( g, 10, 10, 300, 1149724, 0x7FEF );
	AddLocalized( g, 10, 38, 75, 1149761, 0x7FFF );
	g.AddText( 80, 38, 0x53, boat.name && boat.name != "#" ? boat.name : "unnamed ship" );
	AddLocalized( g, 10, 56, 75, 1149762, 0x7FFF );
	g.AddText( 80, 56, 0x53, ValidateObject( boat.owner ) ? boat.owner.name : "Unknown" );
	AddLocalized( g, 10, 79, 300, 1149743, 0x7FFF );
	let partyMode = parseInt( boat.GetShipAccessSetting( 3 ) );
	AddSelectedButton( g, 55, 97, partyMode == 0, 1001 );
	AddLocalized( g, 90, 99, 210, 1149778, 0x7FFF );
	AddSelectedButton( g, 55, 115, partyMode == 1, 1002 );
	AddLocalized( g, 90, 117, 210, 1149744, 0x7FFF );
	AddSelectedButton( g, 55, 133, partyMode == 2, 1003 );
	AddLocalized( g, 90, 135, 210, 1149745, 0x7FFF );
	let publicLevel = parseInt( boat.GetShipAccessSetting( 0 ) );
	let partyLevel = parseInt( boat.GetShipAccessSetting( 1 ) );
	let guildLevel = parseInt( boat.GetShipAccessSetting( 2 ) );
	AddLocalized( g, 10, 158, 125, 1149731, 0x7FFF );
	g.AddButton( 140, 156, 0x0FA5, 0x0FA7, 0, 2, 0 );
	AddLevel( g, 175, 158, 125, publicLevel, true );
	AddLocalized( g, 10, 175, 150, 1149732, 0x7FFF );
	g.AddButton( 140, 173, 0x0FA5, 0x0FA7, 0, 3, 0 );
	AddLevel( g, 175, 175, 125, partyLevel, true );
	AddLocalized( g, 10, 193, 150, 1149733, 0x7FFF );
	g.AddButton( 140, 191, 0x0FA5, 0x0FA7, 0, 4, 0 );
	AddLevel( g, 175, 193, 125, guildLevel, true );
	if( GetServerSetting( "HIGHSEASSHIPANCHORS" ) )
	{
		g.AddText( 10, 220, 0x7FFF, "Anchor" );
		g.AddButton( 140, 218, 0x0FA5, 0x0FA7, 1, 0, 1400 );
		g.AddText( 175, 220, boat.moveType == -1 ? 0x44 : 0x53, boat.moveType == -1 ? "Lowered" : "Raised" );
	}
	g.AddButton( 160, 355, 0x0FA5, 0x0FA7, 1, 0, 2000 );
	AddLocalized( g, 195, 357, 100, 1149734, 0x7FFF );
	AddAccessPanel( g, 2, 30, 1149731, publicLevel, 1100 );
	AddAccessPanel( g, 3, 70, 1149732, partyLevel, 1200 );
	AddAccessPanel( g, 4, 110, 1149733, guildLevel, 1300 );
	g.Send( user );
	g.Free();
}

/** @type { ( socket: Socket, button: number, gumpData: GumpData ) => any } */
function onGumpPress( socket, button, gumpData )
{
	let user = socket.currentChar;
	if( button == 3001 || button == 3002 )
	{
		let resetBoat = CalcItemFromSer( parseInt( user.GetTempTag( "hsResetSecurityBoat" ) ) );
		user.SetTempTag( "hsResetSecurityBoat", null );
		if( button == 3001 && IsCaptain( resetBoat, user ) && user.multi == resetBoat )
		{
			resetBoat.ResetShipSecurity();
			socket.SysMessage( "The ship's access list and default security settings have been reset." );
		}
		return;
	}
	let boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsSecurityBoat" ) ) );
	if( !IsCaptain( boat, user ) )
	{
		return;
	}
	if( button >= 1001 && button <= 1003 )
	{
		boat.SetShipAccessSetting( 3, button - 1001 );
		ShowSecurityGump( user, boat );
	}
	else if( button >= 1100 && button <= 1104 )
	{
		boat.SetShipAccessSetting( 0, button - 1100 );
		ShowSecurityGump( user, boat );
	}
	else if( button >= 1200 && button <= 1204 )
	{
		boat.SetShipAccessSetting( 1, button - 1200 );
		ShowSecurityGump( user, boat );
	}
	else if( button >= 1300 && button <= 1304 )
	{
		boat.SetShipAccessSetting( 2, button - 1300 );
		ShowSecurityGump( user, boat );
	}
	else if( button == 1400 && GetServerSetting( "HIGHSEASSHIPANCHORS" ) )
	{
		if( user.multi != boat )
		{
			socket.SysMessage( "You must be aboard the ship to use its anchor." );
		}
		else if( boat.moveType == -1 )
		{
			boat.moveType = 0;
			socket.SysMessage( "You raise the anchor. The unattended ship can now drift forward." );
		}
		else
		{
			boat.moveType = -1;
			socket.SysMessage( "You lower the anchor." );
		}
		ShowSecurityGump( user, boat );
	}
	else if( button == 2000 )
	{
		user.SetTempTag( "hsSecurityGrant", -1 );
		socket.CustomTarget( 0, "Target a player to view or change their ship access." );
	}
	else if( button >= 2100 && button <= 2105 )
	{
		let target = CalcCharFromSer( parseInt( user.GetTempTag( "hsSecurityTarget" ) ) );
		if( ValidateObject( target ) )
		{
			boat.SetSecurityLevel( target, button - 2100 );
			socket.SysMessage( button == 2100 ? "The player's explicit ship access entry was removed."
											  : "The player's ship access is now " + LevelName( button - 2100 ) + "." );
		}
		ShowSecurityGump( user, boat );
	}
	else if( button == 2199 )
	{
		ShowSecurityGump( user, boat );
	}
}

/** @type { ( user: Character, boat: Multi, target: Character | Item | null ) => any } */
function ShowGrantAccessGump( user, boat, target )
{
	user.SetTempTag( "hsSecurityTarget", target.serial );
	let current = parseInt( boat.GetSecurityLevel( target ) );
	let publicLevel = parseInt( boat.GetShipAccessSetting( 0 ) );
	let partyLevel = parseInt( boat.GetShipAccessSetting( 1 ) );
	let guildLevel = parseInt( boat.GetShipAccessSetting( 2 ) );
	let partyMode = parseInt( boat.GetShipAccessSetting( 3 ) );
	let owner = boat.owner;
	let isPublic = publicLevel != 1;
	let inParty = ValidateObject( owner ) && partyMode != 0 && owner.party != null && target.party == owner.party;
	let inGuild = ValidateObject( owner ) && owner.guild != null && target.guild == owner.guild;
	let g = new Gump;
	g.AddPage( 0 );
	g.AddBackground( 0, 0, 320, 385, 0x0A3C );
	AddLocalized( g, 10, 10, 300, 1149724, 0x7FEF );
	AddLocalized( g, 10, 38, 75, 1149761, 0x7FFF );
	g.AddText( 80, 38, 0x53, boat.name && boat.name != "#" ? boat.name : "unnamed ship" );
	AddLocalized( g, 10, 56, 75, 1149762, 0x7FFF );
	g.AddText( 80, 56, 0x53, ValidateObject( owner ) ? owner.name : "Unknown" );
	AddLocalized( g, 10, 79, 125, 1149763, 0x7FFF );
	g.AddText( 140, 79, 0x30, target.name );
	AddLocalized( g, 10, 97, 125, 1149768, 0x7FFF );
	AddLevel( g, 140, 97, 160, current, true );
	AddLocalized( g, 10, 120, 125, 1149731, 0x7FFF );
	AddLocalized( g, 140, 120, 50, isPublic ? 1149756 : 1149757, isPublic ? 0x1FE7 : 0x3DEF );
	if( isPublic )
	{
		AddLevel( g, 200, 120, 100, publicLevel, true );
	}
	AddLocalized( g, 10, 138, 125, 1149769, 0x7FFF );
	AddLocalized( g, 140, 138, 50, inParty ? 1149756 : 1149757, inParty ? 0x1FE7 : 0x3DEF );
	if( inParty )
	{
		AddLevel( g, 200, 138, 100, partyLevel, true );
	}
	AddLocalized( g, 10, 156, 125, 1149770, 0x7FFF );
	AddLocalized( g, 140, 156, 50, inGuild ? 1149756 : 1149757, inGuild ? 0x1FE7 : 0x3DEF );
	if( inGuild )
	{
		AddLevel( g, 200, 156, 100, guildLevel, true );
	}
	AddLocalized( g, 10, 179, 300, 1149747, 0x7FFF );
	let levels = [ 0, 1, 2, 3, 4, 5 ];
	for( let i = 0; i < levels.length; ++i )
	{
		AddSelectedButton( g, 65, 197 + i * 18, i > 0 && current == levels[i], 2100 + levels[i] );
		if( i == 0 )
		{
			AddLocalized( g, 100, 199, 200, current == 0 ? 1149775 : 1149776, 0x3DEF );
		}
		else
		{
			AddLevel( g, 100, 199 + i * 18, 100, levels[i], current == levels[i] );
		}
	}
	g.AddButton( 10, 355, 0x0FA5, 0x0FA7, 1, 0, 2199 );
	AddLocalized( g, 45, 357, 100, 1149777, 0x7FFF );
	g.Send( user );
	g.Free();
}

/** @type { ( socket: Socket, target: Character | Item | null ) => any } */
function onCallback0( socket, target )
{
	let user = socket.currentChar;
	let boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsSecurityBoat" ) ) );
	if( !IsCaptain( boat, user ) || !ValidateObject( target ) || !target.isChar || target.npc )
	{
		socket.SysMessage( "You must target a player character." );
		return;
	}
	if( boat.IsOwner( target ) || ( ValidateObject( boat.owner ) && boat.owner.accountNum == target.accountNum ) )
	{
		socket.SysMessage( "The ship owner always has Captain access." );
		return;
	}
	let level = parseInt( user.GetTempTag( "hsSecurityGrant" ) );
	if( level == -1 )
	{
		ShowGrantAccessGump( user, boat, target );
		return;
	}
	if( level < 0 || level > 5 )
	{
		return;
	}
	boat.SetSecurityLevel( target, level );
	socket.SysMessage( level == 0 ? "The player's explicit ship access entry was removed." : "The player's ship access is now " + LevelName( level ) + "." );
	ShowSecurityGump( user, boat );
}

/** @type { ( socket: Socket, target: Character | Item | null ) => any } */
function onCallback1( socket, target )
{
	let user = socket.currentChar;
	let boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsMoveTillermanBoat" ) ) );
	user.SetTempTag( "hsMoveTillermanBoat", null );
	if( !IsCaptain( boat, user ) || user.multi != boat )
	{
		return;
	}
	let x = socket.GetWord( 11 );
	let y = socket.GetWord( 13 );
	let z = socket.GetSByte( 16 );
	if( boat.RelocateTillerman( x, y, z ) )
	{
		socket.SysMessage( "The ship wheel has been moved." );
	}
	else
	{
		socket.SysMessage( "That is not a clear location on this ship's deck." );
	}
}

/** @type { ( user: Character, wheel: Item, speech: string, speechID: number ) => any } */
function onSpeechInput( user, wheel, speech, speechID )
{
	if( speechID != 1 || !ValidateObject( user ) || !user.socket )
	{
		return;
	}
	let boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsRenameBoat" ) ) );
	user.SetTempTag( "hsRenameBoat", null );
	if( !IsCaptain( boat, user ) || user.multi != boat )
	{
		return;
	}
	if( speech == null )
	{
		return;
	}
	let newName = speech.replace( /^\s+|\s+$/g, "" );
	if( newName.length == 0 )
	{
		boat.name = "#";
		user.socket.SysMessage( "This ship now has no name." );
		return;
	}
	if( newName.length > 40 )
	{
		newName = newName.substring( 0, 40 );
	}
	boat.name = newName;
	boat.Refresh();
	user.socket.SysMessage( "This ship is now called the " + newName + "." );
}
