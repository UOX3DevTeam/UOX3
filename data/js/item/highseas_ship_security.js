// ServUO pub57 High Seas ship security controls.
// Security levels: 0 NA, 1 Denied, 2 Passenger, 3 Crewman, 4 Officer, 5 Captain.
const SECURITY_ENTRY = 30;
const EMERGENCY_REPAIR_ENTRY = 31;
const PERMANENT_REPAIR_ENTRY = 32;
const RENAME_SHIP_ENTRY = 33;
const MOVE_TILLERMAN_ENTRY = 34;
const RESET_SECURITY_ENTRY = 36;
const DRY_DOCK_ENTRY = 37;
const NA_HUE = 0x5EF7;
const PASSENGER_HUE = 0x1CFF;
const CREW_HUE = 0x1FE7;
const OFFICER_HUE = 0x7FE7;
const DENY_HUE = 0x7CE7;
const CAPTAIN_HUE = 0x7DE7;
const LABEL_HUE = 0x7FFF;
const NO_HUE = 0x3DEF;

function ResolveSecurityBoat( wheel )
{
	if( !ValidateObject( wheel ) || !ValidateObject( wheel.multi ) || !wheel.multi.IsBoat() ) return null;
	return wheel.multi;
}

function IsCaptain( boat, user )
{
	return ValidateObject( boat ) && ValidateObject( user ) && boat.GetSecurityLevel( user ) >= 5;
}

function IsRowBoat( boat )
{
	if( !ValidateObject( boat )) return false;
	var multiID = parseInt( boat.id ) - 0x4000;
	return ( multiID >= 0x3C && multiID <= 0x3F ) ||
		( multiID >= 0x50 && multiID <= 0x53 );
}

function onContextMenuRequest( socket, wheel )
{
	var user = socket.currentChar;
	var boat = ResolveSecurityBoat( wheel );
	if( !ValidateObject( boat ) || !ValidateObject( user )) return true;
	var entries = [];
	var aboard = user.multi == boat;
	var level = parseInt( boat.GetSecurityLevel( user ));
	// Rowboats have no hull damage, repair, manifest, rename, or movable-pilot
	// controls. Their tiller menu contains only Dry Dock, and only while the
	// owner is standing off the boat.
	if( IsRowBoat( boat ))
	{
		if( !aboard && level >= 5 )
			entries.push({ id: DRY_DOCK_ENTRY, text: 1116520, flags: 0x0000, hue: 0x03E0 });
		else
			return true;
		TriggerEvent( 18001, "modifyContextMenu", socket, wheel, entries, true );
		return false;
	}
	if( aboard )
	{
		if( level >= 3 )
		{
			entries.push({ id: EMERGENCY_REPAIR_ENTRY, text: 1116589, flags: 0x0000, hue: 0x03E0 });
			entries.push({ id: PERMANENT_REPAIR_ENTRY, text: 1116590, flags: 0x0000, hue: 0x03E0 });
		}
		if( level >= 5 )
		{
			entries.push({ id: RENAME_SHIP_ENTRY, text: 1111680, flags: 0x0000, hue: 0x03E0 });
			entries.push({ id: MOVE_TILLERMAN_ENTRY, text: 1116729, flags: 0x0000, hue: 0x03E0 });
			entries.push({ id: SECURITY_ENTRY, text: 1149786, flags: 0x0000, hue: 0x03E0 });
			entries.push({ id: RESET_SECURITY_ENTRY, text: 1060700, flags: 0x0000, hue: 0x03E0 });
		}
	}
	else if( level >= 5 )
		entries.push({ id: DRY_DOCK_ENTRY, text: 1116520, flags: 0x0000, hue: 0x03E0 });
	if( entries.length == 0 ) return true;

	TriggerEvent( 18001, "modifyContextMenu", socket, wheel, entries, true );
	return false;
}

function onContextMenuSelect( socket, wheel, popupEntry )
{
	var boat = ResolveSecurityBoat( wheel );
	var user = socket.currentChar;
	if( !ValidateObject( boat ) || !ValidateObject( user )) return false;
	var aboard = user.multi == boat;
	var level = parseInt( boat.GetSecurityLevel( user ));
	if( IsRowBoat( boat ))
	{
		if( popupEntry == DRY_DOCK_ENTRY && !aboard && level >= 5 )
			TriggerEvent( 5098, "BeginHighSeasDryDock", socket, boat );
		return false;
	}
	if( popupEntry == EMERGENCY_REPAIR_ENTRY && aboard && level >= 3 )
		TriggerEvent( 5099, "BeginEmergencyRepairs", user, boat );
	else if( popupEntry == PERMANENT_REPAIR_ENTRY && aboard && level >= 3 )
		TriggerEvent( 5099, "RepairShipHull", user, boat );
	else if( popupEntry == RENAME_SHIP_ENTRY && aboard && level >= 5 )
	{
		user.SetTempTag( "hsRenameBoat", boat.serial );
		user.SpeechInput( 1, wheel );
		socket.SysMessage( "What do you wish to name your ship?" );
	}
	else if( popupEntry == MOVE_TILLERMAN_ENTRY && aboard && level >= 5 )
	{
		user.SetTempTag( "hsMoveTillermanBoat", boat.serial );
		socket.CustomTarget( 1, "Where do you wish to station the tillerman?" );
	}
	else if( popupEntry == SECURITY_ENTRY && aboard && level >= 5 ) ShowSecurityGump( user, boat );
	else if( popupEntry == RESET_SECURITY_ENTRY && aboard && level >= 5 ) ShowResetSecurityGump( user, boat );
	else if( popupEntry == DRY_DOCK_ENTRY && !aboard && level >= 5 )
		TriggerEvent( 5098, "BeginHighSeasDryDock", socket, boat );
	return false;
}

function ShowResetSecurityGump( user, boat )
{
	user.SetTempTag( "hsResetSecurityBoat", boat.serial );
	var g = new Gump();
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

function LevelName( level )
{
	if( level == 1 ) return "Denied";
	if( level == 2 ) return "Passenger";
	if( level == 3 ) return "Crewman";
	if( level == 4 ) return "Officer";
	if( level == 5 ) return "Captain";
	return "N/A";
}

function LevelCliloc( level )
{
	if( level == 1 ) return 1149726;
	if( level == 2 ) return 1149727;
	if( level == 3 ) return 1149728;
	if( level == 4 ) return 1149729;
	if( level == 5 ) return 1149730;
	return 1149725;
}

function LevelHue( level )
{
	if( level == 1 ) return DENY_HUE;
	if( level == 2 ) return PASSENGER_HUE;
	if( level == 3 ) return CREW_HUE;
	if( level == 4 ) return OFFICER_HUE;
	if( level == 5 ) return CAPTAIN_HUE;
	return NA_HUE;
}

function AddLocalized( g, x, y, width, cliloc, hue )
{
	g.AddXMFHTMLGumpColor( x, y, width, 18, cliloc, false, false, hue );
}

function AddLevel( g, x, y, width, level, selected )
{
	AddLocalized( g, x, y, width, LevelCliloc( level ), selected ? LevelHue( level ) : LABEL_HUE );
}

function PartyModeName( mode )
{
	if( mode == 1 ) return "Leader Only";
	if( mode == 2 ) return "All Members";
	return "Never";
}

function AddSelectedButton( g, x, y, selected, button )
{
	if( selected ) g.AddPicture( x, y, 0x0FA6 );
	else g.AddButton( x, y, 0x0FA5, 0x0FA7, 1, 0, button );
}

function AddAccessPanel( g, page, x, titleCliloc, current, buttonBase )
{
	g.AddPage( page );
	g.AddBackground( x, 215, 190, 130, 0x0A3C );
	AddLocalized( g, x + 50, 220, 100, titleCliloc, LABEL_HUE );
	var levels = [0, 2, 3, 4, 1];
	for( var i = 0; i < levels.length; ++i )
	{
		AddSelectedButton( g, x + 10, 243 + i * 18, current == levels[i], buttonBase + levels[i] );
		AddLevel( g, x + 45, 245 + i * 18, 140, levels[i], current == levels[i] );
	}
}

function ShowSecurityGump( user, boat )
{
	if( !IsCaptain( boat, user )) return;
	user.SetTempTag( "hsSecurityBoat", boat.serial );
	var g = new Gump();
	g.AddPage( 0 );
	g.AddBackground( 0, 0, 320, 385, 0x0A3C );
	AddLocalized( g, 10, 10, 300, 1149724, 0x7FEF );
	AddLocalized( g, 10, 38, 75, 1149761, LABEL_HUE );
	g.AddText( 80, 38, 0x53, boat.name && boat.name != "#" ? boat.name : "unnamed ship" );
	AddLocalized( g, 10, 56, 75, 1149762, LABEL_HUE );
	g.AddText( 80, 56, 0x53, ValidateObject( boat.owner ) ? boat.owner.name : "Unknown" );
	AddLocalized( g, 10, 79, 300, 1149743, LABEL_HUE );
	var partyMode = parseInt( boat.GetShipAccessSetting( 3 ));
	AddSelectedButton( g, 55, 97, partyMode == 0, 1001 );
	AddLocalized( g, 90, 99, 210, 1149778, LABEL_HUE );
	AddSelectedButton( g, 55, 115, partyMode == 1, 1002 );
	AddLocalized( g, 90, 117, 210, 1149744, LABEL_HUE );
	AddSelectedButton( g, 55, 133, partyMode == 2, 1003 );
	AddLocalized( g, 90, 135, 210, 1149745, LABEL_HUE );
	var publicLevel = parseInt( boat.GetShipAccessSetting( 0 ));
	var partyLevel = parseInt( boat.GetShipAccessSetting( 1 ));
	var guildLevel = parseInt( boat.GetShipAccessSetting( 2 ));
	AddLocalized( g, 10, 158, 125, 1149731, LABEL_HUE );
	g.AddButton( 140, 156, 0x0FA5, 0x0FA7, 0, 2, 0 );
	AddLevel( g, 175, 158, 125, publicLevel, true );
	AddLocalized( g, 10, 175, 150, 1149732, LABEL_HUE );
	g.AddButton( 140, 173, 0x0FA5, 0x0FA7, 0, 3, 0 );
	AddLevel( g, 175, 175, 125, partyLevel, true );
	AddLocalized( g, 10, 193, 150, 1149733, LABEL_HUE );
	g.AddButton( 140, 191, 0x0FA5, 0x0FA7, 0, 4, 0 );
	AddLevel( g, 175, 193, 125, guildLevel, true );
	g.AddButton( 160, 355, 0x0FA5, 0x0FA7, 1, 0, 2000 );
	AddLocalized( g, 195, 357, 100, 1149734, LABEL_HUE );
	AddAccessPanel( g, 2, 30, 1149731, publicLevel, 1100 );
	AddAccessPanel( g, 3, 70, 1149732, partyLevel, 1200 );
	AddAccessPanel( g, 4, 110, 1149733, guildLevel, 1300 );
	g.Send( user );
	g.Free();
}

function onGumpPress( socket, button, gumpData )
{
	var user = socket.currentChar;
	if( button == 3001 || button == 3002 )
	{
		var resetBoat = CalcItemFromSer( parseInt( user.GetTempTag( "hsResetSecurityBoat" )));
		user.SetTempTag( "hsResetSecurityBoat", null );
		if( button == 3001 && IsCaptain( resetBoat, user ) && user.multi == resetBoat )
		{
			resetBoat.ResetShipSecurity();
			socket.SysMessage( "The ship's access list and default security settings have been reset." );
		}
		return;
	}
	var boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsSecurityBoat" )));
	if( !IsCaptain( boat, user )) return;
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
	else if( button == 2000 )
	{
		user.SetTempTag( "hsSecurityGrant", -1 );
		socket.CustomTarget( 0, "Target a player to view or change their ship access." );
	}
	else if( button >= 2100 && button <= 2105 )
	{
		var target = CalcCharFromSer( parseInt( user.GetTempTag( "hsSecurityTarget" )));
		if( ValidateObject( target ))
		{
			boat.SetSecurityLevel( target, button - 2100 );
			socket.SysMessage( button == 2100 ? "The player's explicit ship access entry was removed." :
				"The player's ship access is now " + LevelName( button - 2100 ) + "." );
		}
		ShowSecurityGump( user, boat );
	}
	else if( button == 2199 ) ShowSecurityGump( user, boat );
}

function ShowGrantAccessGump( user, boat, target )
{
	user.SetTempTag( "hsSecurityTarget", target.serial );
	var current = parseInt( boat.GetSecurityLevel( target ));
	var publicLevel = parseInt( boat.GetShipAccessSetting( 0 ));
	var partyLevel = parseInt( boat.GetShipAccessSetting( 1 ));
	var guildLevel = parseInt( boat.GetShipAccessSetting( 2 ));
	var partyMode = parseInt( boat.GetShipAccessSetting( 3 ));
	var owner = boat.owner;
	var isPublic = publicLevel != 1;
	var inParty = ValidateObject( owner ) && partyMode != 0 && owner.party != null && target.party == owner.party;
	var inGuild = ValidateObject( owner ) && owner.guild != null && target.guild == owner.guild;
	var g = new Gump();
	g.AddPage( 0 );
	g.AddBackground( 0, 0, 320, 385, 0x0A3C );
	AddLocalized( g, 10, 10, 300, 1149724, 0x7FEF );
	AddLocalized( g, 10, 38, 75, 1149761, LABEL_HUE );
	g.AddText( 80, 38, 0x53, boat.name && boat.name != "#" ? boat.name : "unnamed ship" );
	AddLocalized( g, 10, 56, 75, 1149762, LABEL_HUE );
	g.AddText( 80, 56, 0x53, ValidateObject( owner ) ? owner.name : "Unknown" );
	AddLocalized( g, 10, 79, 125, 1149763, LABEL_HUE );
	g.AddText( 140, 79, 0x30, target.name );
	AddLocalized( g, 10, 97, 125, 1149768, LABEL_HUE );
	AddLevel( g, 140, 97, 160, current, true );
	AddLocalized( g, 10, 120, 125, 1149731, LABEL_HUE );
	AddLocalized( g, 140, 120, 50, isPublic ? 1149756 : 1149757, isPublic ? CREW_HUE : NO_HUE );
	if( isPublic ) AddLevel( g, 200, 120, 100, publicLevel, true );
	AddLocalized( g, 10, 138, 125, 1149769, LABEL_HUE );
	AddLocalized( g, 140, 138, 50, inParty ? 1149756 : 1149757, inParty ? CREW_HUE : NO_HUE );
	if( inParty ) AddLevel( g, 200, 138, 100, partyLevel, true );
	AddLocalized( g, 10, 156, 125, 1149770, LABEL_HUE );
	AddLocalized( g, 140, 156, 50, inGuild ? 1149756 : 1149757, inGuild ? CREW_HUE : NO_HUE );
	if( inGuild ) AddLevel( g, 200, 156, 100, guildLevel, true );
	AddLocalized( g, 10, 179, 300, 1149747, LABEL_HUE );
	var levels = [0, 1, 2, 3, 4, 5];
	for( var i = 0; i < levels.length; ++i )
	{
		AddSelectedButton( g, 65, 197 + i * 18, i > 0 && current == levels[i], 2100 + levels[i] );
		if( i == 0 ) AddLocalized( g, 100, 199, 200, current == 0 ? 1149775 : 1149776, NO_HUE );
		else AddLevel( g, 100, 199 + i * 18, 100, levels[i], current == levels[i] );
	}
	g.AddButton( 10, 355, 0x0FA5, 0x0FA7, 1, 0, 2199 );
	AddLocalized( g, 45, 357, 100, 1149777, LABEL_HUE );
	g.Send( user );
	g.Free();
}

function onCallback0( socket, target )
{
	var user = socket.currentChar;
	var boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsSecurityBoat" )));
	if( !IsCaptain( boat, user ) || !ValidateObject( target ) || !target.isChar || target.npc )
	{
		socket.SysMessage( "You must target a player character." );
		return;
	}
	if( boat.IsOwner( target ) || ( ValidateObject( boat.owner ) && boat.owner.accountNum == target.accountNum ))
	{
		socket.SysMessage( "The ship owner always has Captain access." );
		return;
	}
	var level = parseInt( user.GetTempTag( "hsSecurityGrant" ));
	if( level == -1 )
	{
		ShowGrantAccessGump( user, boat, target );
		return;
	}
	if( level < 0 || level > 5 ) return;
	boat.SetSecurityLevel( target, level );
	socket.SysMessage( level == 0 ? "The player's explicit ship access entry was removed." :
		"The player's ship access is now " + LevelName( level ) + "." );
	ShowSecurityGump( user, boat );
}

function onCallback1( socket, target )
{
	var user = socket.currentChar;
	var boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsMoveTillermanBoat" )));
	user.SetTempTag( "hsMoveTillermanBoat", null );
	if( !IsCaptain( boat, user ) || user.multi != boat ) return;
	var x = socket.GetWord( 11 );
	var y = socket.GetWord( 13 );
	var z = socket.GetSByte( 16 );
	if( boat.RelocateTillerman( x, y, z ))
		socket.SysMessage( "The ship wheel has been moved." );
	else
		socket.SysMessage( "That is not a clear location on this ship's deck." );
}

function onSpeechInput( user, wheel, speech, speechID )
{
	if( speechID != 1 || !ValidateObject( user ) || !user.socket ) return;
	var boat = CalcItemFromSer( parseInt( user.GetTempTag( "hsRenameBoat" )));
	user.SetTempTag( "hsRenameBoat", null );
	if( !IsCaptain( boat, user ) || user.multi != boat ) return;
	if( speech == null ) return;
	var newName = speech.replace( /^\s+|\s+$/g, "" );
	if( newName.length == 0 )
	{
		boat.name = "#";
		user.socket.SysMessage( "This ship now has no name." );
		return;
	}
	if( newName.length > 40 ) newName = newName.substring( 0, 40 );
	boat.name = newName;
	boat.Refresh();
	user.socket.SysMessage( "This ship is now called the " + newName + "." );
}
