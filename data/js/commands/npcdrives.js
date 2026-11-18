/// <reference path="../definitions.d.ts" />
// @ts-check

const NPC_DRIVE_SCRIPT_ID = 6;
const AI_ANIMAL = 6;
const AI_ANIMAL_SCARED = 12;
const WT_FROZEN = 5;

function CommandRegistration()
{
	RegisterCommand( "npcdrives", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_NPCDRIVES( socket, cmdString )
{
	var commandText = cmdString ? cmdString.toString().trim().toLowerCase() : "";
	var commandParts = commandText.split( /\s+/ );
	var action = commandParts[0];
	if( action == "diagnose" && commandParts.length > 1 )
	{
		var serial = parseInt( commandParts[1] );
		var npc = isNaN( serial ) ? null : CalcCharFromSer( serial );
		if( !ValidateObject( npc ) || !npc.npc )
		{
			socket.SysMessage( "That serial does not identify a live NPC." );
			return;
		}
		socket.npcDriveAdminAction = "";
		socket.npcDriveAdminTarget = npc.serial.toString();
		ReportPickupDiagnostic( socket, npc );
		ShowNpcDriveAdminGump( socket );
		return;
	}
	socket.npcDriveAdminAction = action == "diagnose" ? "diagnose" : "";
	socket.CustomTarget( 0, "Select the NPC whose drive settings you wish to inspect." );
}

/** @type { ( socket: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, target )
{
	if( socket.GetWord( 1 ) || !ValidateObject( target ) || !target.isChar || !target.npc )
	{
		socket.SysMessage( "You must target an NPC." );
		return;
	}

	socket.npcDriveAdminTarget = target.serial.toString();
	if( socket.npcDriveAdminAction == "diagnose" )
	{
		socket.npcDriveAdminAction = "";
		ReportPickupDiagnostic( socket, target );
	}
	ShowNpcDriveAdminGump( socket );
}

/** @type { ( socket: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, target )
{
	if( socket.GetWord( 1 ) || !ValidateObject( target ) || ( !target.isChar && !target.isItem ))
	{
		socket.SysMessage( "You must target an item or character." );
		return;
	}
	socket.npcDriveResourceTarget = target.serial.toString();
	socket.npcDriveResourceTargetType = target.isChar ? "char" : "item";
	ShowNpcDriveResourceGump( socket );
}

/** @type { ( socket: Socket, target: Character | Item | null ) => void } */
function onCallback2( socket, target )
{
	var npc = GetSelectedNpc( socket );
	if( socket.GetWord( 1 ) || !ValidateObject( npc ) || !ValidateObject( target ) || !target.isChar || target.serial == npc.serial )
	{
		socket.SysMessage( "You must target another character as the drive master." );
		return;
	}
	npc.SetTag( "NpcDriveMasterSerial", target.serial.toString() );
	ForceReevaluation( npc );
	ShowNpcDriveSocialGump( socket );
}

/** @type { ( socket: Socket, target: Character | Item | null ) => void } */
function onCallback3( socket, target )
{
	var npc = GetSelectedNpc( socket );
	if( socket.GetWord( 1 ) || !ValidateObject( npc ) || !ValidateObject( target ) || !target.isItem || target.type != 1 || target.container != null )
	{
		socket.SysMessage( "You must target a chest or other container placed on the ground." );
		return;
	}
	npc.SetTag( "NpcDriveHoardContainerSerial", target.serial.toString() );
	ForceReevaluation( npc );
	socket.SysMessage( "The NPC's hoard chest has been set." );
	ShowNpcDriveSocialGump( socket );
}

/** @param {Socket} socket */
function ShowNpcDriveAdminGump( socket )
{
	var npc = GetSelectedNpc( socket );
	if( !ValidateObject( npc ))
	{
		socket.SysMessage( "That NPC is no longer available." );
		return;
	}

	var profile = GetProfile( npc );
	var eligibility = GetEligibility( npc, profile );
	var mode = GetMode( npc );
	var reason = npc.GetTempTag( "npcDriveReason" );
	var driveTarget = GetDriveTarget( npc );
	var healthPercent = npc.maxhp > 0 ? Math.floor(( npc.health * 100 ) / npc.maxhp ) : 100;
	var globalEnabled = GetServerSetting( "NpcDriveSystem" );

	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 620, 412, 5054 );
	gump.AddCheckerTrans( 8, 8, 604, 396 );
	gump.AddHTMLGump( 20, 12, 580, 24, false, false, "<basefont color=#EECD8B><center><big>NPC Drive Administration</big></center></basefont>" );
	gump.AddButton( 585, 14, 4017, 4018, 1, 0, 0 );

	var summary = "<basefont color=#FFFFFF><b>" + EscapeHtml( npc.name ) + "</b> [0x" + npc.serial.toString( 16 ).toUpperCase() + "]" +
		"<br>Global system: " + ColourValue( globalEnabled ? "ENABLED" : "DISABLED", globalEnabled ) +
		" | Eligibility: " + ColourValue( eligibility.eligible ? "YES" : "NO", eligibility.eligible ) + " (" + EscapeHtml( eligibility.reason ) + ")" +
		"<br>AI: " + npc.aitype + " | Wander: " + npc.wandertype + " | Profile: " + EscapeHtml( profile ) +
		" | Combat: " + ( npc.atWar ? "yes" : "no" ) +
		"<br>Health: " + npc.health + "/" + npc.maxhp + " (" + healthPercent + "%) | Hunger: " + npc.hunger + "/6 | Food: " + EscapeHtml( npc.foodList || "none" ) +
		"<br>Drive: " + EscapeHtml( mode || "none" ) + " | Reason: " + EscapeHtml( reason || "none" ) + " | Target: " + DescribeTarget( npc, driveTarget ) +
		"<br>Home: " + EscapeHtml( DescribeHome( npc )) + "</basefont>";
	gump.AddHTMLGump( 20, 42, 580, 116, true, false, summary );

	AddSectionTitle( gump, 20, 166, "Profile" );
	AddChoiceButton( gump, 20, 188, 20, "Auto", profile == "auto" );
	AddChoiceButton( gump, 130, 188, 21, "Animal", profile == "animal" );
	AddChoiceButton( gump, 240, 188, 22, "Humanoid", profile == "humanoid" );
	AddChoiceButton( gump, 355, 188, 23, "Ambient", profile == "ambient" );
	AddChoiceButton( gump, 475, 188, 24, "Off", profile == "none" || profile == "off" );

	AddSectionTitle( gump, 20, 220, "Individual drives" );
	AddCompactToggleButton( gump, 20, 242, 30, "Food", !DriveDisabled( npc, "Food" ));
	AddCompactToggleButton( gump, 135, 242, 31, "Shelter", !DriveDisabled( npc, "Shelter" ));
	AddCompactToggleButton( gump, 250, 242, 32, "Survival", !DriveDisabled( npc, "Survival" ));
	AddCompactToggleButton( gump, 375, 242, 33, "Desire", !DriveDisabled( npc, "Desire" ));
	AddCompactToggleButton( gump, 490, 242, 34, "Defense", !DriveDisabled( npc, "Defense" ));

	AddSectionTitle( gump, 20, 274, "Settings and actions" );
	AddWideButton( gump, 20, 298, 12, "Thresholds, shelter and home" );
	AddWideButton( gump, 310, 298, 9, "Desires and aversions" );
	AddWideButton( gump, 20, 334, 10, "Territory and hoard defense" );
	AddWideButton( gump, 310, 334, 11, "Pack, hoard chest and idle" );
	AddWideButton( gump, 20, 370, 2, "Target another NPC" );
	AddWideButton( gump, 310, 370, 14, "Force immediate reevaluation" );

	gump.Send( socket );
	gump.Free();
}

/** @param {Socket} socket */
function ShowNpcDriveCoreGump( socket )
{
	var npc = GetSelectedNpc( socket );
	if( !ValidateObject( npc ))
	{
		socket.SysMessage( "That NPC is no longer available." );
		return;
	}

	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 620, 430, 5054 );
	gump.AddCheckerTrans( 8, 8, 604, 414 );
	gump.AddHTMLGump( 20, 12, 580, 24, false, false, "<basefont color=#EECD8B><center><big>NPC Thresholds, Shelter and Home</big></center></basefont>" );
	AddHeaderBackButton( gump, 13, "Overview" );
	gump.AddButton( 585, 14, 4017, 4018, 1, 0, 0 );
	gump.AddHTMLGump( 20, 42, 580, 42, true, false, "<basefont color=#FFFFFF><b>" + EscapeHtml( npc.name ) + "</b> [0x" + npc.serial.toString( 16 ).toUpperCase() + "]<br>Home: " + EscapeHtml( DescribeHome( npc )) + "</basefont>" );

	AddSectionTitle( gump, 20, 92, "Thresholds and ranges" );
	AddNumberSetting( gump, 20, 116, "Food threshold (1-5)", 0, GetTagValue( npc, "NpcDriveFoodThreshold", 3 ));
	AddNumberSetting( gump, 310, 116, "Food radius (1-24)", 1, GetTagValue( npc, "NpcDriveSearchRadius", 12 ));
	AddNumberSetting( gump, 20, 150, "Shelter radius (1-64)", 2, GetTagValue( npc, "NpcDriveShelterRadius", 24 ));
	AddNumberSetting( gump, 310, 150, "Leash radius (1-64)", 3, GetTagValue( npc, "NpcDriveLeashRadius", 24 ));
	AddNumberSetting( gump, 20, 184, "Rest health % (1-99)", 4, GetTagValue( npc, "NpcDriveRestHealth", 25 ));
	AddNumberSetting( gump, 310, 184, "Resume health % (2-100)", 5, GetTagValue( npc, "NpcDriveResumeHealth", 75 ));
	AddNumberSetting( gump, 20, 218, "Shelter start (0-23)", 6, GetTagValue( npc, "NpcDriveShelterStartHour", 22 ));
	AddNumberSetting( gump, 310, 218, "Shelter end (0-23)", 7, GetTagValue( npc, "NpcDriveShelterEndHour", 6 ));

	AddSectionTitle( gump, 20, 252, "Actions" );
	AddWideButton( gump, 20, 276, 3, "Apply threshold/range values" );
	AddWideButton( gump, 310, 276, 4, "Force immediate reevaluation" );
	AddWideButton( gump, 20, 312, 5, "Cancel current drive goal" );
	AddWideButton( gump, 310, 312, 2, "Target another NPC" );
	AddWideButton( gump, 20, 348, 6, "Set NPC position as home" );
	AddWideButton( gump, 310, 348, 7, "Reset home to spawn" );
	AddWideButton( gump, 20, 384, 8, "Create shelter at NPC" );
	AddWideButton( gump, 310, 384, 13, "Back to overview" );

	gump.Send( socket );
	gump.Free();
}

/** @param {Socket} socket */
function ShowNpcDriveDesireGump( socket )
{
	var npc = GetSelectedNpc( socket );
	if( !ValidateObject( npc ))
	{
		socket.SysMessage( "That NPC is no longer available." );
		return;
	}

	var desires = npc.GetTag( "NpcDriveDesires" );
	var aversions = npc.GetTag( "NpcDriveAversions" );
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 620, 535, 5054 );
	gump.AddCheckerTrans( 8, 8, 604, 519 );
	gump.AddHTMLGump( 20, 12, 580, 24, false, false, "<basefont color=#EECD8B><center><big>NPC Desires and Aversions</big></center></basefont>" );
	AddHeaderBackButton( gump, 42, "Overview" );
	gump.AddButton( 585, 14, 4017, 4018, 1, 0, 0 );
	gump.AddHTMLGump( 20, 42, 580, 38, true, false, "<basefont color=#FFFFFF><b>" + EscapeHtml( npc.name ) + "</b> | Desire drive: " + ColourValue( DriveDisabled( npc, "Desire" ) ? "DISABLED" : "ENABLED", !DriveDisabled( npc, "Desire" )) + "<br>Entries are evaluated from left to right; the first reachable match wins.</basefont>" );

	AddSectionTitle( gump, 20, 88, "Desires" );
	gump.AddHTMLGump( 20, 110, 580, 34, false, false, "<basefont color=#FFFFFF>Format: resource:action:cooldownSeconds | Actions: pickup, nearby, follow<br>Example: gold:pickup:60,honey:nearby:30,human:follow:0</basefont>" );
	AddTextSetting( gump, 20, 148, 580, 42, 0, 200, desires, 240 );

	AddSectionTitle( gump, 20, 198, "Aversions" );
	gump.AddHTMLGump( 20, 220, 580, 20, false, false, "<basefont color=#FFFFFF>Format: resource:safeRadius | Example: orc:10,carnivore:8</basefont>" );
	AddTextSetting( gump, 20, 244, 580, 42, 1, 201, aversions, 240 );

	AddNumberSetting( gump, 20, 302, "Desire search (1-24)", 2, GetTagValue( npc, "NpcDriveDesireSearchRadius", 12 ));
	AddNumberSetting( gump, 310, 302, "Aversion search (1-24)", 3, GetTagValue( npc, "NpcDriveAversionSearchRadius", 12 ));
	AddNumberSetting( gump, 20, 338, "Nearby range (1-8)", 4, GetTagValue( npc, "NpcDriveDesireStayRange", 2 ));
	AddNumberSetting( gump, 310, 338, "Interaction secs (1-300)", 5, GetTagValue( npc, "NpcDriveDesireDuration", 15 ));

	AddWideButton( gump, 20, 386, 40, "Apply desire settings" );
	AddWideButton( gump, 310, 386, 41, "Edit world resource tags" );
	AddWideButton( gump, 20, 422, 43, "Force immediate reevaluation" );
	AddWideButton( gump, 310, 422, 45, "Cancel current drive goal" );
	AddWideButton( gump, 20, 458, 2, "Target another NPC" );
	AddWideButton( gump, 310, 458, 42, "Back to overview" );
	AddWideButton( gump, 20, 494, 44, "Toggle desire drive" );
	AddWideButton( gump, 310, 494, 46, "Diagnose nearby pickup" );

	gump.Send( socket );
	gump.Free();
}

/** @param {Socket} socket */
function ShowNpcDriveDefenseGump( socket )
{
	var npc = GetSelectedNpc( socket );
	if( !ValidateObject( npc ))
	{
		socket.SysMessage( "That NPC is no longer available." );
		return;
	}

	var territoryTargets = npc.GetTag( "NpcDriveTerritoryTargets" );
	var hoardResources = npc.GetTag( "NpcDriveHoardResources" );
	var defenseReason = npc.GetTempTag( "npcDriveDefenseReason" );
	var defenseTarget = GetDefenseTarget( npc );
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 620, 470, 5054 );
	gump.AddCheckerTrans( 8, 8, 604, 454 );
	gump.AddHTMLGump( 20, 12, 580, 24, false, false, "<basefont color=#EECD8B><center><big>NPC Territory and Hoard Defense</big></center></basefont>" );
	AddHeaderBackButton( gump, 62, "Overview" );
	gump.AddButton( 585, 14, 4017, 4018, 1, 0, 0 );
	var status = "<basefont color=#FFFFFF><b>" + EscapeHtml( npc.name ) + "</b> | Defense drive: " + ColourValue( DriveDisabled( npc, "Defense" ) ? "DISABLED" : "ENABLED", !DriveDisabled( npc, "Defense" )) +
		"<br>Last trigger: " + EscapeHtml( defenseReason || "none" ) + " | Target: " + DescribeTarget( npc, defenseTarget ) + "</basefont>";
	gump.AddHTMLGump( 20, 42, 580, 52, true, false, status );

	AddSectionTitle( gump, 20, 104, "Territorial intruders" );
	gump.AddHTMLGump( 20, 126, 580, 34, false, false, "<basefont color=#FFFFFF>Comma-separated character resources that may trigger native combat.<br>Example: player,human | Leave empty to disable aggression.</basefont>" );
	AddTextSetting( gump, 20, 164, 580, 38, 0, 400, territoryTargets, 240 );

	AddSectionTitle( gump, 20, 210, "Protected hoard resources" );
	gump.AddHTMLGump( 20, 232, 580, 20, false, false, "<basefont color=#FFFFFF>Ground resources to guard inside the territory. Example: gold,gem</basefont>" );
	AddTextSetting( gump, 20, 256, 580, 38, 1, 401, hoardResources, 240 );

	AddNumberSetting( gump, 20, 306, "Territory radius (1-24)", 2, GetTagValue( npc, "NpcDriveTerritoryRadius", 12 ));
	AddNumberSetting( gump, 310, 306, "Hoard guard radius (1-24)", 3, GetTagValue( npc, "NpcDriveHoardGuardRadius", 4 ));
	AddWideButton( gump, 20, 346, 60, "Apply defense settings" );
	AddWideButton( gump, 310, 346, 61, "Edit world resource tags" );
	AddWideButton( gump, 20, 382, 63, "Force immediate reevaluation" );
	AddWideButton( gump, 310, 382, 64, "Toggle defense drive" );
	AddWideButton( gump, 20, 418, 2, "Target another NPC" );
	AddWideButton( gump, 310, 418, 62, "Back to overview" );

	gump.Send( socket );
	gump.Free();
}

/** @param {Socket} socket */
function ShowNpcDriveSocialGump( socket )
{
	var npc = GetSelectedNpc( socket );
	if( !ValidateObject( npc ))
	{
		socket.SysMessage( "That NPC is no longer available." );
		return;
	}

	var packId = npc.GetTag( "NpcDrivePackId" );
	var packRole = npc.GetTag( "NpcDrivePackRole" );
	var returnResources = npc.GetTag( "NpcDriveReturnResources" );
	var master = GetTaggedCharacter( npc, "NpcDriveMasterSerial" );
	var hoardContainer = GetTaggedItem( npc, "NpcDriveHoardContainerSerial" );
	var socialTarget = GetTempTaggedCharacter( npc, "npcDriveSocialTarget" );
	var pendingItem = GetTempTaggedItem( npc, "npcDrivePendingReturnItem" );
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 620, 458, 5054 );
	gump.AddCheckerTrans( 8, 8, 604, 442 );
	gump.AddHTMLGump( 20, 12, 580, 24, false, false, "<basefont color=#EECD8B><center><big>NPC Pack and Hoard Chest</big></center></basefont>" );
	AddHeaderBackButton( gump, 79, "Overview" );
	gump.AddButton( 585, 14, 4017, 4018, 1, 0, 0 );
	var status = "<basefont color=#FFFFFF><b>" + EscapeHtml( npc.name ) + "</b> | Social trigger: " + EscapeHtml( npc.GetTempTag( "npcDriveSocialReason" ) || "none" ) +
		"<br>Social target: " + DescribeTarget( npc, socialTarget ) + " | Pending return: " + DescribeObject( pendingItem ) + "</basefont>";
	gump.AddHTMLGump( 20, 42, 580, 42, true, false, status );

	AddSectionTitle( gump, 20, 92, "Hoard chest for returned resources" );
	gump.AddHTMLGump( 20, 116, 190, 20, false, false, "<basefont color=#FFFFFF>Resources (or all)</basefont>" );
	AddTextSetting( gump, 210, 112, 390, 26, 2, 502, returnResources, 240 );
	gump.AddHTMLGump( 20, 146, 580, 22, true, false, "<basefont color=#FFFFFF>Hoard chest: " + DescribeObject( hoardContainer ) + " (optional; otherwise items remain in the NPC pack)</basefont>" );
	AddWideButton( gump, 20, 174, 73, "Set hoard chest - target" );
	AddWideButton( gump, 310, 174, 74, "Clear hoard chest" );

	AddSectionTitle( gump, 20, 208, "Pack and master relationship" );
	gump.AddHTMLGump( 20, 232, 120, 20, false, false, "<basefont color=#FFFFFF>Pack ID</basefont>" );
	AddTextSetting( gump, 140, 228, 160, 26, 0, 500, packId, 32 );
	gump.AddHTMLGump( 320, 232, 120, 20, false, false, "<basefont color=#FFFFFF>Role</basefont>" );
	AddTextSetting( gump, 440, 228, 160, 26, 1, 501, packRole || "member", 8 );
	gump.AddHTMLGump( 20, 260, 580, 22, true, false, "<basefont color=#FFFFFF>Master: " + DescribeObject( master ) + "</basefont>" );
	AddWideButton( gump, 20, 288, 71, "Target master" );
	AddWideButton( gump, 310, 288, 72, "Clear master" );
	AddNumberSetting( gump, 20, 324, "Assist radius (1-24)", 3, GetTagValue( npc, "NpcDrivePackRadius", 12 ));
	AddNumberSetting( gump, 310, 324, "Regroup range (1-12)", 4, GetTagValue( npc, "NpcDrivePackFollowDistance", 4 ));

	AddSectionTitle( gump, 20, 358, "Actions" );
	AddWideButton( gump, 20, 382, 70, "Apply pack/hoard settings" );
	AddWideButton( gump, 310, 382, 75, "Force immediate reevaluation" );
	AddCompactToggleButton( gump, 20, 418, 76, "Social", !DriveDisabled( npc, "Social" ));
	AddCompactToggleButton( gump, 160, 418, 77, "Return", !DriveDisabled( npc, "Return" ));
	AddWideButton( gump, 310, 418, 80, "Boredom and idle settings" );

	gump.Send( socket );
	gump.Free();
}

/** @param {Socket} socket */
function ShowNpcDriveIdleGump( socket )
{
	var npc = GetSelectedNpc( socket );
	if( !ValidateObject( npc ))
	{
		socket.SysMessage( "That NPC is no longer available." );
		return;
	}

	var idleActivities = npc.GetTag( "NpcDriveIdleActivities" );
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 620, 348, 5054 );
	gump.AddCheckerTrans( 8, 8, 604, 332 );
	gump.AddHTMLGump( 20, 12, 580, 24, false, false, "<basefont color=#EECD8B><center><big>NPC Boredom and Idle Activity</big></center></basefont>" );
	AddHeaderBackButton( gump, 79, "Overview" );
	gump.AddButton( 585, 14, 4017, 4018, 1, 0, 0 );
	gump.AddHTMLGump( 20, 42, 580, 42, true, false, "<basefont color=#FFFFFF><b>" + EscapeHtml( npc.name ) + "</b> | Boredom drive: " + ColourValue( DriveDisabled( npc, "Boredom" ) ? "DISABLED" : "ENABLED", !DriveDisabled( npc, "Boredom" )) + "<br>Idle activities are tried from left to right when the NPC becomes bored.</basefont>" );

	AddSectionTitle( gump, 20, 92, "Idle activity order" );
	gump.AddHTMLGump( 20, 116, 180, 20, false, false, "<basefont color=#FFFFFF>home, roam, pause</basefont>" );
	AddTextSetting( gump, 200, 112, 400, 26, 0, 503, idleActivities, 64 );
	AddNumberSetting( gump, 20, 152, "Boredom delay (10-3600s)", 1, GetTagValue( npc, "NpcDriveBoredomDelay", 60 ));
	AddNumberSetting( gump, 310, 152, "Activity duration (1-300s)", 2, GetTagValue( npc, "NpcDriveIdleDuration", 10 ));
	AddNumberSetting( gump, 20, 186, "Roam radius (1-24)", 3, GetTagValue( npc, "NpcDriveIdleRadius", 8 ));

	AddSectionTitle( gump, 20, 226, "Actions" );
	AddWideButton( gump, 20, 250, 82, "Apply idle settings" );
	AddWideButton( gump, 310, 250, 83, "Force immediate reevaluation" );
	AddWideButton( gump, 20, 286, 78, "Toggle boredom drive" );
	AddWideButton( gump, 310, 286, 81, "Back to pack and hoard chest" );
	AddWideButton( gump, 20, 322, 2, "Target another NPC" );

	gump.Send( socket );
	gump.Free();
}

/** @param {Socket} socket */
function ShowNpcDriveResourceGump( socket )
{
	var target = GetSelectedResourceObject( socket );
	if( !ValidateObject( target ))
	{
		socket.SysMessage( "That resource target is no longer available." );
		if( socket.npcDriveResourceReturn == "defense" )
			ShowNpcDriveDefenseGump( socket );
		else
			ShowNpcDriveDesireGump( socket );
		return;
	}
	var resources = target.GetTag( "NpcDriveResource" );
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 620, 260, 5054 );
	gump.AddCheckerTrans( 8, 8, 604, 244 );
	gump.AddHTMLGump( 20, 12, 580, 24, false, false, "<basefont color=#EECD8B><center><big>NPC Drive Resource Tags</big></center></basefont>" );
	AddHeaderBackButton( gump, 52, "Settings" );
	gump.AddButton( 585, 14, 4017, 4018, 1, 0, 0 );
	gump.AddHTMLGump( 20, 48, 580, 48, true, false, "<basefont color=#FFFFFF><b>" + EscapeHtml( target.name || ( target.isChar ? "character" : "item" )) + "</b> [0x" + target.serial.toString( 16 ).toUpperCase() + "]<br>Assign one or more comma-separated resource names.</basefont>" );
	AddTextSetting( gump, 20, 110, 580, 42, 0, 300, resources, 240 );
	gump.AddHTMLGump( 20, 158, 580, 20, false, false, "<basefont color=#FFFFFF>Example: gold,coin | Resource names use letters, numbers, underscore or hyphen.</basefont>" );
	AddWideButton( gump, 20, 194, 50, "Apply resource tags" );
	AddWideButton( gump, 310, 194, 51, "Clear resource tags" );
	AddWideButton( gump, 20, 228, 52, "Back to drive settings" );
	gump.Send( socket );
	gump.Free();
}

/** @type { ( socket: Socket, buttonID: number, gumpData: GumpData ) => void } */
function onGumpPress( socket, buttonID, gumpData )
{
	if( buttonID == 0 )
		return;

	if( buttonID == 2 )
	{
		socket.CustomTarget( 0, "Select another NPC to inspect." );
		return;
	}

	var npc = GetSelectedNpc( socket );
	if( !ValidateObject( npc ))
	{
		socket.SysMessage( "That NPC is no longer available." );
		return;
	}
	if( buttonID == 9 )
	{
		ShowNpcDriveDesireGump( socket );
		return;
	}
	if( buttonID == 10 )
	{
		ShowNpcDriveDefenseGump( socket );
		return;
	}
	if( buttonID == 11 )
	{
		ShowNpcDriveSocialGump( socket );
		return;
	}
	if( buttonID == 12 )
	{
		ShowNpcDriveCoreGump( socket );
		return;
	}
	if( buttonID == 13 )
	{
		ShowNpcDriveAdminGump( socket );
		return;
	}
	if( buttonID == 42 )
	{
		ShowNpcDriveAdminGump( socket );
		return;
	}
	if( buttonID == 52 )
	{
		if( socket.npcDriveResourceReturn == "defense" )
			ShowNpcDriveDefenseGump( socket );
		else
			ShowNpcDriveDesireGump( socket );
		return;
	}
	if( buttonID == 41 )
	{
		socket.npcDriveResourceReturn = "desire";
		socket.CustomTarget( 1, "Select an item or character whose NPC drive resources you wish to edit." );
		return;
	}
	if( buttonID == 61 )
	{
		socket.npcDriveResourceReturn = "defense";
		socket.CustomTarget( 1, "Select an item or character whose NPC drive resources you wish to edit." );
		return;
	}
	if( buttonID == 62 )
	{
		ShowNpcDriveAdminGump( socket );
		return;
	}
	if( buttonID == 71 )
	{
		socket.CustomTarget( 2, "Select the character this NPC should treat as its master." );
		return;
	}
	if( buttonID == 73 )
	{
		socket.CustomTarget( 3, "Select a chest or other ground container for the NPC's hoard." );
		return;
	}
	if( buttonID == 79 )
	{
		ShowNpcDriveAdminGump( socket );
		return;
	}
	if( buttonID == 80 )
	{
		ShowNpcDriveIdleGump( socket );
		return;
	}
	if( buttonID == 81 )
	{
		ShowNpcDriveSocialGump( socket );
		return;
	}

	switch( buttonID )
	{
		case 1:
			return;
		case 3:
			if( !ApplySettings( socket, npc, gumpData ))
				return;
			ForceReevaluation( npc );
			socket.SysMessage( "NPC drive settings updated." );
			ShowNpcDriveCoreGump( socket );
			return;
		case 4:
			ForceReevaluation( npc );
			socket.SysMessage( "The NPC will reevaluate its drives on its next AI slice." );
			ShowNpcDriveCoreGump( socket );
			return;
		case 5:
			TriggerEvent( NPC_DRIVE_SCRIPT_ID, "AdminCancelDriveGoal", npc );
			socket.SysMessage( "The NPC's current drive goal was cancelled." );
			ShowNpcDriveCoreGump( socket );
			return;
		case 6:
			SetHome( npc );
			ForceReevaluation( npc );
			socket.SysMessage( "Drive home set to the NPC's current position." );
			ShowNpcDriveCoreGump( socket );
			return;
		case 7:
			ClearHome( npc );
			ForceReevaluation( npc );
			socket.SysMessage( "Drive home reset to the NPC's spawn position." );
			ShowNpcDriveCoreGump( socket );
			return;
		case 8:
			CreateShelter( socket, npc );
			ShowNpcDriveCoreGump( socket );
			return;
		case 14:
			ForceReevaluation( npc );
			socket.SysMessage( "The NPC will reevaluate its drives on its next AI slice." );
			break;
		case 20:
			SetProfile( npc, null );
			ForceReevaluation( npc );
			break;
		case 21:
			SetProfile( npc, "animal" );
			ForceReevaluation( npc );
			break;
		case 22:
			SetProfile( npc, "humanoid" );
			ForceReevaluation( npc );
			break;
		case 23:
			SetProfile( npc, "ambient" );
			ForceReevaluation( npc );
			break;
		case 24:
			SetProfile( npc, "none" );
			ForceReevaluation( npc );
			break;
		case 30:
			ToggleDrive( npc, "Food" );
			ForceReevaluation( npc );
			break;
		case 31:
			ToggleDrive( npc, "Shelter" );
			ForceReevaluation( npc );
			break;
		case 32:
			ToggleDrive( npc, "Survival" );
			ForceReevaluation( npc );
			break;
		case 33:
			ToggleDrive( npc, "Desire" );
			ForceReevaluation( npc );
			break;
		case 34:
			ToggleDrive( npc, "Defense" );
			ForceReevaluation( npc );
			break;
		case 43:
			ForceReevaluation( npc );
			socket.SysMessage( "The NPC will reevaluate its drives on its next AI slice." );
			ShowNpcDriveDesireGump( socket );
			return;
		case 44:
			ToggleDrive( npc, "Desire" );
			ForceReevaluation( npc );
			ShowNpcDriveDesireGump( socket );
			return;
		case 45:
			TriggerEvent( NPC_DRIVE_SCRIPT_ID, "AdminCancelDriveGoal", npc );
			socket.SysMessage( "The NPC's current drive goal was cancelled." );
			ShowNpcDriveDesireGump( socket );
			return;
		case 46:
			ReportPickupDiagnostic( socket, npc );
			ShowNpcDriveDesireGump( socket );
			return;
		case 40:
			if( ApplyDesireSettings( socket, npc, gumpData ))
			{
				ForceReevaluation( npc );
				socket.SysMessage( "NPC desire and aversion settings updated." );
			}
			ShowNpcDriveDesireGump( socket );
			return;
		case 50:
			ApplyResourceTags( socket, gumpData.getEdit( 0 ));
			ShowNpcDriveResourceGump( socket );
			return;
		case 51:
			var resourceTarget = GetSelectedResourceObject( socket );
			if( ValidateObject( resourceTarget ))
				resourceTarget.SetTag( "NpcDriveResource", null );
			ShowNpcDriveResourceGump( socket );
			return;
		case 60:
			if( ApplyDefenseSettings( socket, npc, gumpData ))
			{
				ForceReevaluation( npc );
				socket.SysMessage( "NPC territory and hoard settings updated." );
			}
			ShowNpcDriveDefenseGump( socket );
			return;
		case 63:
			ForceReevaluation( npc );
			socket.SysMessage( "The NPC will reevaluate its drives on its next AI slice." );
			ShowNpcDriveDefenseGump( socket );
			return;
		case 64:
			ToggleDrive( npc, "Defense" );
			ForceReevaluation( npc );
			ShowNpcDriveDefenseGump( socket );
			return;
		case 70:
			if( ApplyPackHoardSettings( socket, npc, gumpData ))
			{
				ForceReevaluation( npc );
				socket.SysMessage( "NPC pack and hoard settings updated." );
			}
			ShowNpcDriveSocialGump( socket );
			return;
		case 72:
			npc.SetTag( "NpcDriveMasterSerial", null );
			ForceReevaluation( npc );
			ShowNpcDriveSocialGump( socket );
			return;
		case 74:
			npc.SetTag( "NpcDriveHoardContainerSerial", null );
			ForceReevaluation( npc );
			socket.SysMessage( "The NPC's hoard chest has been cleared." );
			ShowNpcDriveSocialGump( socket );
			return;
		case 75:
			ForceReevaluation( npc );
			socket.SysMessage( "The NPC will reevaluate its drives on its next AI slice." );
			ShowNpcDriveSocialGump( socket );
			return;
		case 76:
			ToggleDrive( npc, "Social" );
			ForceReevaluation( npc );
			ShowNpcDriveSocialGump( socket );
			return;
		case 77:
			ToggleDrive( npc, "Return" );
			ForceReevaluation( npc );
			ShowNpcDriveSocialGump( socket );
			return;
		case 78:
			ToggleDrive( npc, "Boredom" );
			ForceReevaluation( npc );
			ShowNpcDriveIdleGump( socket );
			return;
		case 82:
			if( ApplyIdleSettings( socket, npc, gumpData ))
			{
				ForceReevaluation( npc );
				socket.SysMessage( "NPC boredom and idle settings updated." );
			}
			ShowNpcDriveIdleGump( socket );
			return;
		case 83:
			ForceReevaluation( npc );
			socket.SysMessage( "The NPC will reevaluate its drives on its next AI slice." );
			ShowNpcDriveIdleGump( socket );
			return;
		default:
			return;
	}

	ShowNpcDriveAdminGump( socket );
}

/** @param {Socket} socket @returns {Character | null} */
function GetSelectedNpc( socket )
{
	var serial = parseInt( socket.npcDriveAdminTarget );
	var npc = isNaN( serial ) ? null : CalcCharFromSer( serial );
	return ValidateObject( npc ) && npc.npc ? npc : null;
}

/** @param {Socket} socket @param {Character} npc */
function ReportPickupDiagnostic( socket, npc )
{
	var diagnostic = TriggerEvent( NPC_DRIVE_SCRIPT_ID, "AdminDescribePickup", npc );
	socket.SysMessage( diagnostic ? diagnostic.toString() : "Pickup diagnostic did not return a result. Reload JS script 6." );
}

/** @param {Socket} socket @returns {Character | Item | null} */
function GetSelectedResourceObject( socket )
{
	var serial = parseInt( socket.npcDriveResourceTarget );
	if( isNaN( serial ))
		return null;
	var target = socket.npcDriveResourceTargetType == "char" ? CalcCharFromSer( serial ) : CalcItemFromSer( serial );
	return ValidateObject( target ) ? target : null;
}

/** @param {Character} npc @returns {string} */
function GetProfile( npc )
{
	var profile = npc.GetTag( "NpcDriveProfile" );
	return profile ? profile.toString().toLowerCase() : "auto";
}

/** @param {Character} npc @returns {string} */
function GetMode( npc )
{
	var mode = npc.GetTempTag( "npcDriveMode" );
	if( !mode )
		return "";
	mode = mode.toString().toLowerCase();
	return mode == "1" ? "food" : mode;
}

/** @param {Character} npc @returns {Character | Item | null} */
function GetDriveTarget( npc )
{
	var serial = ParseStoredSerial( npc.GetTempTag( "npcDriveTarget" ));
	if( isNaN( serial ))
		serial = ParseStoredSerial( npc.GetTempTag( "npcDriveFoodTarget" ));
	if( isNaN( serial ))
		return null;
	return npc.GetTempTag( "npcDriveTargetType" ).toString() == "char" ? CalcCharFromSer( serial ) : CalcItemFromSer( serial );
}

/** @param {Character} npc @returns {Character | null} */
function GetDefenseTarget( npc )
{
	var serial = ParseStoredSerial( npc.GetTempTag( "npcDriveDefenseTarget" ));
	if( isNaN( serial ))
		return null;
	var target = CalcCharFromSer( serial );
	return ValidateObject( target ) ? target : null;
}

/** @param {Character} npc @param {string} tagName @returns {Character | null} */
function GetTaggedCharacter( npc, tagName )
{
	var serial = ParseStoredSerial( npc.GetTag( tagName ));
	if( isNaN( serial ))
		return null;
	var target = CalcCharFromSer( serial );
	return ValidateObject( target ) ? target : null;
}

/** @param {Character} npc @param {string} tagName @returns {Item | null} */
function GetTaggedItem( npc, tagName )
{
	var serial = ParseStoredSerial( npc.GetTag( tagName ));
	if( isNaN( serial ))
		return null;
	var target = CalcItemFromSer( serial );
	return ValidateObject( target ) ? target : null;
}

/** @param {Character} npc @param {string} tagName @returns {Character | null} */
function GetTempTaggedCharacter( npc, tagName )
{
	var serial = ParseStoredSerial( npc.GetTempTag( tagName ));
	if( isNaN( serial ))
		return null;
	var target = CalcCharFromSer( serial );
	return ValidateObject( target ) ? target : null;
}

/** @param {Character} npc @param {string} tagName @returns {Item | null} */
function GetTempTaggedItem( npc, tagName )
{
	var serial = ParseStoredSerial( npc.GetTempTag( tagName ));
	if( isNaN( serial ))
		return null;
	var target = CalcItemFromSer( serial );
	return ValidateObject( target ) ? target : null;
}

/**
 * Missing UOX3 integer tags read back as numeric zero. Drive serials are always
 * stored as strings so a real serial zero remains distinguishable as "0".
 * @param {string | number | boolean} rawValue
 * @returns {number}
 */
function ParseStoredSerial( rawValue )
{
	if( rawValue === null || rawValue === undefined || rawValue === false || rawValue === "" || ( rawValue === 0 && typeof rawValue != "string" ))
		return NaN;
	return parseInt( rawValue );
}

/** @param {Character} npc @param {string} profile @returns {{ eligible: boolean, reason: string }} */
function GetEligibility( npc, profile )
{
	if( npc.dead ) return { eligible: false, reason: "dead" };
	if( npc.tamed ) return { eligible: false, reason: "tamed" };
	if( npc.mounted ) return { eligible: false, reason: "mounted" };
	if( npc.stabled ) return { eligible: false, reason: "stabled" };
	if( npc.frozen || npc.wandertype == WT_FROZEN ) return { eligible: false, reason: "frozen" };
	if( profile == "none" || profile == "off" ) return { eligible: false, reason: "profile disabled" };
	if( profile == "animal" || profile == "humanoid" || profile == "ambient" ) return { eligible: true, reason: "explicit profile" };
	if( npc.aitype == AI_ANIMAL || npc.aitype == AI_ANIMAL_SCARED ) return { eligible: true, reason: "automatic animal AI" };
	return { eligible: false, reason: "requires an explicit profile" };
}

/** @param {Character} npc @param {string} name @returns {boolean} */
function DriveDisabled( npc, name )
{
	return parseInt( npc.GetTag( "NpcDriveDisable" + name )) == 1;
}

/** @param {Character} npc @param {string} name */
function ToggleDrive( npc, name )
{
	npc.SetTag( "NpcDriveDisable" + name, DriveDisabled( npc, name ) ? null : 1 );
}

/** @param {Character} npc @param {string | null} profile */
function SetProfile( npc, profile )
{
	npc.SetTag( "NpcDriveProfile", profile );
}

/** @param {Character} npc */
function ForceReevaluation( npc )
{
	TriggerEvent( NPC_DRIVE_SCRIPT_ID, "AdminForceDriveReevaluation", npc );
}

/** @param {Character} npc */
function SetHome( npc )
{
	npc.SetTag( "NpcDriveHomeX", npc.x.toString() );
	npc.SetTag( "NpcDriveHomeY", npc.y.toString() );
	npc.SetTag( "NpcDriveHomeZ", npc.z.toString() );
	npc.SetTag( "NpcDriveHomeWorld", npc.worldnumber.toString() );
	npc.SetTag( "NpcDriveHomeInstance", npc.instanceID.toString() );
	ClearHomeCache( npc );
}

/** @param {Character} npc */
function ClearHome( npc )
{
	npc.SetTag( "NpcDriveHomeX", null );
	npc.SetTag( "NpcDriveHomeY", null );
	npc.SetTag( "NpcDriveHomeZ", null );
	npc.SetTag( "NpcDriveHomeWorld", null );
	npc.SetTag( "NpcDriveHomeInstance", null );
	ClearHomeCache( npc );
}

/** @param {Character} npc */
function ClearHomeCache( npc )
{
	npc.SetTempTag( "npcDriveHomeX", null );
	npc.SetTempTag( "npcDriveHomeY", null );
	npc.SetTempTag( "npcDriveHomeZ", null );
	npc.SetTempTag( "npcDriveHomeWorld", null );
	npc.SetTempTag( "npcDriveHomeInstance", null );
	npc.SetTempTag( "npcDriveHomeSet", null );
}

/** @param {Socket} socket @param {Character} npc */
function CreateShelter( socket, npc )
{
	var marker = CreateDFNItem( socket, npc, "npc_drive_shelter_marker", 1, "ITEM", false );
	if( !ValidateObject( marker ))
	{
		socket.SysMessage( "Unable to create the shelter marker." );
		return;
	}
	marker.SetLocation( npc.x, npc.y, npc.z, npc.worldnumber, npc.instanceID );
	socket.SysMessage( "A GM-visible NPC drive shelter marker was created at the NPC's position." );
}

/** @param {Socket} socket @param {Character} npc @param {GumpData} gumpData @returns {boolean} */
function ApplyDesireSettings( socket, npc, gumpData )
{
	var desireResult = NormalizeDesireDefinitions( gumpData.getEdit( 0 ).toString() );
	if( desireResult.error )
	{
		socket.SysMessage( desireResult.error );
		return false;
	}
	var aversionResult = NormalizeAversionDefinitions( gumpData.getEdit( 1 ).toString() );
	if( aversionResult.error )
	{
		socket.SysMessage( aversionResult.error );
		return false;
	}

	var numberDefinitions = [
		[ "NpcDriveDesireSearchRadius", 1, 24 ],
		[ "NpcDriveAversionSearchRadius", 1, 24 ],
		[ "NpcDriveDesireStayRange", 1, 8 ],
		[ "NpcDriveDesireDuration", 1, 300 ]
	];
	var values = [];
	for( var i = 0; i < numberDefinitions.length; ++i )
	{
		var textValue = gumpData.getEdit( i + 2 ).toString().trim();
		if( !/^\d+$/.test( textValue ))
		{
			socket.SysMessage( "Every desire setting must contain a whole number." );
			return false;
		}
		var value = parseInt( textValue );
		if( value < numberDefinitions[i][1] || value > numberDefinitions[i][2] )
		{
			socket.SysMessage( numberDefinitions[i][0] + " must be between " + numberDefinitions[i][1] + " and " + numberDefinitions[i][2] + "." );
			return false;
		}
		values.push( value );
	}

	npc.SetTag( "NpcDriveDesires", desireResult.value ? desireResult.value : null );
	npc.SetTag( "NpcDriveAversions", aversionResult.value ? aversionResult.value : null );
	for( var j = 0; j < numberDefinitions.length; ++j )
		npc.SetTag( numberDefinitions[j][0], values[j].toString() );
	return true;
}

/** @param {Socket} socket @param {Character} npc @param {GumpData} gumpData @returns {boolean} */
function ApplyDefenseSettings( socket, npc, gumpData )
{
	var territoryResult = NormalizeResourceList( gumpData.getEdit( 0 ).toString() );
	if( territoryResult.error )
	{
		socket.SysMessage( territoryResult.error );
		return false;
	}
	var hoardResult = NormalizeResourceList( gumpData.getEdit( 1 ).toString() );
	if( hoardResult.error )
	{
		socket.SysMessage( hoardResult.error );
		return false;
	}

	var definitions = [
		[ "NpcDriveTerritoryRadius", 1, 24 ],
		[ "NpcDriveHoardGuardRadius", 1, 24 ]
	];
	var values = [];
	for( var i = 0; i < definitions.length; ++i )
	{
		var textValue = gumpData.getEdit( i + 2 ).toString().trim();
		if( !/^\d+$/.test( textValue ))
		{
			socket.SysMessage( "Every defense setting must contain a whole number." );
			return false;
		}
		var value = parseInt( textValue );
		if( value < definitions[i][1] || value > definitions[i][2] )
		{
			socket.SysMessage( definitions[i][0] + " must be between " + definitions[i][1] + " and " + definitions[i][2] + "." );
			return false;
		}
		values.push( value );
	}

	npc.SetTag( "NpcDriveTerritoryTargets", territoryResult.value ? territoryResult.value : null );
	npc.SetTag( "NpcDriveHoardResources", hoardResult.value ? hoardResult.value : null );
	for( var j = 0; j < definitions.length; ++j )
		npc.SetTag( definitions[j][0], values[j].toString() );
	return true;
}

/** @param {Socket} socket @param {Character} npc @param {GumpData} gumpData @returns {boolean} */
function ApplyPackHoardSettings( socket, npc, gumpData )
{
	var packId = gumpData.getEdit( 0 ).toString().trim().toLowerCase();
	if( packId && !IsResourceName( packId ))
	{
		socket.SysMessage( "Pack IDs may contain up to 32 letters, numbers, underscores or hyphens." );
		return false;
	}
	var packRole = gumpData.getEdit( 1 ).toString().trim().toLowerCase();
	if( packRole != "leader" && packRole != "member" )
	{
		socket.SysMessage( "Pack role must be leader or member." );
		return false;
	}
	var returnResult = NormalizeResourceList( gumpData.getEdit( 2 ).toString() );
	if( returnResult.error )
	{
		socket.SysMessage( returnResult.error );
		return false;
	}
	var definitions = [
		[ "NpcDrivePackRadius", 1, 24 ],
		[ "NpcDrivePackFollowDistance", 1, 12 ]
	];
	var values = [];
	for( var i = 0; i < definitions.length; ++i )
	{
		var textValue = gumpData.getEdit( i + 3 ).toString().trim();
		if( !/^\d+$/.test( textValue ))
		{
			socket.SysMessage( "Every pack setting must contain a whole number." );
			return false;
		}
		var value = parseInt( textValue );
		if( value < definitions[i][1] || value > definitions[i][2] )
		{
			socket.SysMessage( definitions[i][0] + " must be between " + definitions[i][1] + " and " + definitions[i][2] + "." );
			return false;
		}
		values.push( value );
	}

	npc.SetTag( "NpcDrivePackId", packId ? packId : null );
	npc.SetTag( "NpcDrivePackRole", packRole );
	npc.SetTag( "NpcDriveReturnResources", returnResult.value ? returnResult.value : null );
	for( var j = 0; j < definitions.length; ++j )
		npc.SetTag( definitions[j][0], values[j].toString() );
	return true;
}

/** @param {Socket} socket @param {Character} npc @param {GumpData} gumpData @returns {boolean} */
function ApplyIdleSettings( socket, npc, gumpData )
{
	var idleResult = NormalizeIdleActivities( gumpData.getEdit( 0 ).toString() );
	if( idleResult.error )
	{
		socket.SysMessage( idleResult.error );
		return false;
	}

	var definitions = [
		[ "NpcDriveBoredomDelay", 10, 3600 ],
		[ "NpcDriveIdleDuration", 1, 300 ],
		[ "NpcDriveIdleRadius", 1, 24 ]
	];
	var values = [];
	for( var i = 0; i < definitions.length; ++i )
	{
		var textValue = gumpData.getEdit( i + 1 ).toString().trim();
		if( !/^\d+$/.test( textValue ))
		{
			socket.SysMessage( "Every idle setting must contain a whole number." );
			return false;
		}
		var value = parseInt( textValue );
		if( value < definitions[i][1] || value > definitions[i][2] )
		{
			socket.SysMessage( definitions[i][0] + " must be between " + definitions[i][1] + " and " + definitions[i][2] + "." );
			return false;
		}
		values.push( value );
	}

	npc.SetTag( "NpcDriveIdleActivities", idleResult.value ? idleResult.value : null );
	for( var j = 0; j < definitions.length; ++j )
		npc.SetTag( definitions[j][0], values[j].toString() );
	return true;
}

/** @param {Socket} socket @param {string} rawValue @returns {boolean} */
function ApplyResourceTags( socket, rawValue )
{
	var target = GetSelectedResourceObject( socket );
	if( !ValidateObject( target ))
	{
		socket.SysMessage( "That resource target is no longer available." );
		return false;
	}
	var result = NormalizeResourceList( rawValue );
	if( result.error )
	{
		socket.SysMessage( result.error );
		return false;
	}
	target.SetTag( "NpcDriveResource", result.value ? result.value : null );
	var npc = GetSelectedNpc( socket );
	if( ValidateObject( npc ))
		ForceReevaluation( npc );
	socket.SysMessage( result.value ? "NPC drive resource tags updated." : "NPC drive resource tags cleared." );
	return true;
}

/** @param {string} rawValue @returns {{ value: string, error: string }} */
function NormalizeDesireDefinitions( rawValue )
{
	var normalized = [];
	var raw = rawValue.trim().toLowerCase();
	if( !raw )
		return { value: "", error: "" };
	var definitions = raw.split( "," );
	for( var i = 0; i < definitions.length; ++i )
	{
		var parts = definitions[i].split( ":" );
		if( parts.length < 1 || parts.length > 3 )
			return { value: "", error: "Invalid desire entry: " + definitions[i] };
		var resource = parts[0].trim();
		var action = parts.length > 1 ? parts[1].trim() : "nearby";
		var cooldownText = parts.length > 2 ? parts[2].trim() : "0";
		if( !IsResourceName( resource ))
			return { value: "", error: "Invalid desire resource name: " + resource };
		if( action != "pickup" && action != "nearby" && action != "follow" )
			return { value: "", error: "Desire action must be pickup, nearby or follow." };
		if( !/^\d+$/.test( cooldownText ) || parseInt( cooldownText ) > 86400 )
			return { value: "", error: "Desire cooldown must be between 0 and 86400 seconds." };
		normalized.push( resource + ":" + action + ":" + parseInt( cooldownText ));
	}
	return { value: normalized.join( "," ), error: "" };
}

/** @param {string} rawValue @returns {{ value: string, error: string }} */
function NormalizeAversionDefinitions( rawValue )
{
	var normalized = [];
	var raw = rawValue.trim().toLowerCase();
	if( !raw )
		return { value: "", error: "" };
	var definitions = raw.split( "," );
	for( var i = 0; i < definitions.length; ++i )
	{
		var parts = definitions[i].split( ":" );
		if( parts.length < 1 || parts.length > 2 )
			return { value: "", error: "Invalid aversion entry: " + definitions[i] };
		var resource = parts[0].trim();
		var radiusText = parts.length > 1 ? parts[1].trim() : "8";
		if( !IsResourceName( resource ))
			return { value: "", error: "Invalid aversion resource name: " + resource };
		if( !/^\d+$/.test( radiusText ) || parseInt( radiusText ) < 1 || parseInt( radiusText ) > 24 )
			return { value: "", error: "Aversion radius must be between 1 and 24 tiles." };
		normalized.push( resource + ":" + parseInt( radiusText ));
	}
	return { value: normalized.join( "," ), error: "" };
}

/** @param {string} rawValue @returns {{ value: string, error: string }} */
function NormalizeResourceList( rawValue )
{
	var normalized = [];
	var raw = rawValue.trim().toLowerCase();
	if( !raw )
		return { value: "", error: "" };
	var resources = raw.split( "," );
	for( var i = 0; i < resources.length; ++i )
	{
		var resource = resources[i].trim();
		if( !IsResourceName( resource ))
			return { value: "", error: "Resource names may contain up to 32 letters, numbers, underscores or hyphens." };
		if( normalized.indexOf( resource ) == -1 )
			normalized.push( resource );
	}
	return { value: normalized.join( "," ), error: "" };
}

/** @param {string} rawValue @returns {{ value: string, error: string }} */
function NormalizeIdleActivities( rawValue )
{
	var result = NormalizeResourceList( rawValue );
	if( result.error || !result.value )
		return result;
	var activities = result.value.split( "," );
	for( var i = 0; i < activities.length; ++i )
	{
		if( activities[i] != "home" && activities[i] != "roam" && activities[i] != "pause" )
			return { value: "", error: "Idle activities must be home, roam or pause." };
	}
	return result;
}

/** @param {string} resource @returns {boolean} */
function IsResourceName( resource )
{
	return resource.length <= 32 && /^[a-z0-9_-]+$/.test( resource );
}

/** @param {Socket} socket @param {Character} npc @param {GumpData} gumpData @returns {boolean} */
function ApplySettings( socket, npc, gumpData )
{
	var definitions = [
		[ "NpcDriveFoodThreshold", 1, 5 ],
		[ "NpcDriveSearchRadius", 1, 24 ],
		[ "NpcDriveShelterRadius", 1, 64 ],
		[ "NpcDriveLeashRadius", 1, 64 ],
		[ "NpcDriveRestHealth", 1, 99 ],
		[ "NpcDriveResumeHealth", 2, 100 ],
		[ "NpcDriveShelterStartHour", 0, 23 ],
		[ "NpcDriveShelterEndHour", 0, 23 ]
	];
	var values = [];
	for( var i = 0; i < definitions.length; ++i )
	{
		var textValue = gumpData.getEdit( i ).toString().trim();
		if( !/^-?\d+$/.test( textValue ))
		{
			socket.SysMessage( "Every drive setting must contain a whole number." );
			ShowNpcDriveAdminGump( socket );
			return false;
		}
		var value = parseInt( textValue );
		if( value < definitions[i][1] || value > definitions[i][2] )
		{
			socket.SysMessage( definitions[i][0] + " must be between " + definitions[i][1] + " and " + definitions[i][2] + "." );
			ShowNpcDriveAdminGump( socket );
			return false;
		}
		values.push( value );
	}

	if( values[5] <= values[4] )
	{
		socket.SysMessage( "Resume health must be greater than rest health." );
		ShowNpcDriveAdminGump( socket );
		return false;
	}

	for( var j = 0; j < definitions.length; ++j )
		npc.SetTag( definitions[j][0], values[j].toString() );
	return true;
}

/** @param {Character} npc @param {string} tagName @param {number} defaultValue @returns {number} */
function GetTagValue( npc, tagName, defaultValue )
{
	var rawValue = npc.GetTag( tagName );
	var value = parseInt( rawValue );
	return isNaN( value ) || ( value == 0 && typeof rawValue != "string" ) ? defaultValue : value;
}

/** @param {Character} npc @returns {string} */
function DescribeHome( npc )
{
	var homeX = parseInt( npc.GetTag( "NpcDriveHomeX" ));
	var homeY = parseInt( npc.GetTag( "NpcDriveHomeY" ));
	if( !isNaN( homeX ) && !isNaN( homeY ))
		return homeX + ", " + homeY + " (custom)";
	if( npc.spawnX != 0 || npc.spawnY != 0 )
		return npc.spawnX + ", " + npc.spawnY + " (spawn)";
	return npc.x + ", " + npc.y + " (initial position)";
}

/** @param {Character} npc @param {Character | Item | null} target @returns {string} */
function DescribeTarget( npc, target )
{
	if( parseInt( npc.GetTempTag( "npcDriveRestInPlace" )) == 1 )
		return "current building";
	if( !ValidateObject( target ))
		return "none";
	return EscapeHtml( target.name || "item" ) + " [0x" + target.serial.toString( 16 ).toUpperCase() + "] at " + DistanceBetween( npc, target, true ) + " tiles";
}

/** @param {Character | Item | null} target @returns {string} */
function DescribeObject( target )
{
	if( !ValidateObject( target ))
		return "none";
	return EscapeHtml( target.name || ( target.isChar ? "character" : "item" )) + " [0x" + target.serial.toString( 16 ).toUpperCase() + "]";
}

/** @param {Gump} gump @param {number} x @param {number} y @param {string} text */
function AddSectionTitle( gump, x, y, text )
{
	gump.AddHTMLGump( x, y, 580, 20, false, false, "<basefont color=#EECD8B><b>" + text + "</b></basefont>" );
}

/** @param {Gump} gump @param {number} x @param {number} y @param {number} buttonId @param {string} text @param {boolean} selected */
function AddChoiceButton( gump, x, y, buttonId, text, selected )
{
	gump.AddButton( x, y, 4005, 4007, 1, 0, buttonId );
	gump.AddHTMLGump( x + 34, y + 2, 76, 20, false, false, "<basefont color=" + ( selected ? "#00FF00" : "#FFFFFF" ) + ">" + text + "</basefont>" );
}

/** @param {Gump} gump @param {number} x @param {number} y @param {number} buttonId @param {string} text @param {boolean} enabled */
function AddToggleButton( gump, x, y, buttonId, text, enabled )
{
	gump.AddButton( x, y, 4005, 4007, 1, 0, buttonId );
	gump.AddHTMLGump( x + 34, y + 2, 110, 20, false, false, "<basefont color=" + ( enabled ? "#00FF00" : "#FF6060" ) + ">" + text + " " + ( enabled ? "ON" : "OFF" ) + "</basefont>" );
}

/** @param {Gump} gump @param {number} x @param {number} y @param {number} buttonId @param {string} text @param {boolean} enabled */
function AddCompactToggleButton( gump, x, y, buttonId, text, enabled )
{
	gump.AddButton( x, y, 4005, 4007, 1, 0, buttonId );
	gump.AddHTMLGump( x + 30, y + 2, 90, 20, false, false, "<basefont color=" + ( enabled ? "#00FF00" : "#FF6060" ) + ">" + text + " " + ( enabled ? "ON" : "OFF" ) + "</basefont>" );
}

/** @param {Gump} gump @param {number} x @param {number} y @param {number} width @param {number} height @param {number} relay @param {number} textId @param {*} value @param {number} maxLength */
function AddTextSetting( gump, x, y, width, height, relay, textId, value, maxLength )
{
	gump.AddBackground( x, y, width, height, 2620 );
	gump.AddTextEntryLimited( x + 8, y + 3, width - 16, height - 6, 55, relay, textId, EntryText( value ), maxLength );
}

/** @param {Gump} gump @param {number} x @param {number} y @param {string} label @param {number} relay @param {number} value */
function AddNumberSetting( gump, x, y, label, relay, value )
{
	gump.AddHTMLGump( x, y + 2, 180, 20, false, false, "<basefont color=#FFFFFF>" + label + "</basefont>" );
	gump.AddBackground( x + 182, y - 2, 82, 26, 2620 );
	gump.AddTextEntryLimited( x + 190, y + 2, 66, 20, 55, relay, 100 + relay, value.toString(), 5 );
}

/** @param {Gump} gump @param {number} x @param {number} y @param {number} buttonId @param {string} text */
function AddWideButton( gump, x, y, buttonId, text )
{
	gump.AddButton( x, y, 4005, 4007, 1, 0, buttonId );
	gump.AddHTMLGump( x + 34, y + 2, 245, 20, false, false, "<basefont color=#FFFFFF>" + text + "</basefont>" );
}

/** @param {Gump} gump @param {number} buttonId @param {string} text */
function AddHeaderBackButton( gump, buttonId, text )
{
	gump.AddButton( 20, 14, 4005, 4007, 1, 0, buttonId );
	gump.AddHTMLGump( 52, 16, 100, 20, false, false, "<basefont color=#EECD8B>" + text + "</basefont>" );
}

/** @param {*} value @returns {string} */
function EntryText( value )
{
	if( value === null || value === undefined || value.toString().length == 0 )
		return " ";
	return value.toString();
}

/** @param {string} text @returns {string} */
function EscapeHtml( text )
{
	return text.toString().replace( /&/g, "&amp;" ).replace( /</g, "&lt;" ).replace( />/g, "&gt;" );
}

/** @param {string} text @param {boolean} positive @returns {string} */
function ColourValue( text, positive )
{
	return "<basefont color=" + ( positive ? "#00FF00" : "#FF6060" ) + ">" + text + "</basefont>";
}
