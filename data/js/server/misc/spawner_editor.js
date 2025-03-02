var spawnEditorTooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var gumpID = this.script_id + 0xffff;

	if( socket && iUsed && iUsed.isItem && pUser.isGM )
	{
		socket.tempObj = iUsed;
		socket.CloseGump( gumpID, 0 );
		spawnerGump( socket, pUser, iUsed );
	}

	return false;
}

function spawnerGump( socket, pUser, iUsed )
{
	var spawner = new Gump;

	spawner.AddPage( 0 );

	// Main Background and Header
	spawner.AddBackground( 0, 0, 360, 295, 5054 );
	spawner.AddCheckerTrans( 0, 0, 360, 295 );
	spawner.AddHTMLGump(15, 7, 320, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>" + GetDictionaryEntry( 9800, socket.language ) + "</BASEFONT></BIG></CENTER>" );// Spawner Object Editor
	spawner.AddButton( 320, 1, 4017, 4018, 1, 0, 0 ); // Close Menu Button
	spawner.AddPicture( 50, 10, 7956 ); // Image of a "spawn rune"

	// Spawner Name
	spawner.AddHTMLGump( 20, 35, 140, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9801, socket.language ) + ":</basefont >" );// Spawner Name
	spawner.AddBackground( 115, 30, 235, 28, 5120 ); // Spawner Name Background

	// Spawner Type
	spawner.AddHTMLGump( 20, 68, 140, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9802, socket.language ) + ":</basefont>" );// Spawner Type
	spawner.AddHTMLGump( 55, 100, 140, 22, false, false, "<basefont color=#00ff00><big>" + GetDictionaryEntry( 9803, socket.language ) + "</big></basefont>" );// Item
	spawner.AddHTMLGump( 135, 100, 140, 22, false, false, "<basefont color=#00ff00><big>" + GetDictionaryEntry( 9804, socket.language ) + "</big></basefont>" );// Npc
	spawner.AddHTMLGump( 215, 100, 140, 22, false, false, "<basefont color=#00ff00><big>" + GetDictionaryEntry( 9805, socket.language ) + "</big></basefont>" );// Area
	spawner.AddHTMLGump( 295, 100, 140, 22, false, false, "<basefont color=#00ff00><big>" + GetDictionaryEntry( 9806, socket.language ) + "</big></basefont>" );// Escort
	spawner.AddGroup( 1 );
	spawner.AddRadio( 20, 95, 2472, 2153, ( iUsed.type == 61 ? 1 : 0 ), 0 );
	spawner.AddRadio( 100, 95, 2472, 2153, ( iUsed.type == 62 ? 1 : 0 ), 1 );
	spawner.AddRadio( 180, 95, 2472, 2153, ( iUsed.type == 69 ? 1 : 0 ), 2 );
	spawner.AddRadio( 260, 95, 2472, 2153, ( iUsed.type == 125 ? 1 : 0 ), 3 );
	spawner.EndGroup();

	// Spawn Section/List
	spawner.AddHTMLGump( 20, 135, 150, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9807, socket.language ) + ":</basefont>" );// Spawn Section/List
	spawner.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9808, socket.language ));// Section ID or NPClist to spawn from DFNs
	spawner.AddBackground( 20, 160, 205, 28, 5120 );

	// Spawnlist?
	spawner.AddHTMLGump( 285, 155, 55, 44, false, false, "<center><basefont color=#ffffff>Use<br>" + GetDictionaryEntry( 9809, socket.language ) + "</basefont></center>" );// Spawnlist
	spawner.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9810, socket.language ));// If checked, spawner uses spawnlist instead of spawn section
	spawner.AddCheckbox( 250, 160, 2472, 2153, ( iUsed.sectionalist == true ? 1 : 0 ), 1 );

	// Amount
	spawner.AddHTMLGump( 35, 205, 120, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9811, socket.language ) + ":</basefont>" );// Amount
	spawner.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9812, socket.language ));// Maximum amount to respawn
	spawner.AddBackground( 30, 230, 50, 28, 5120 );

	// Min / Max Time
	spawner.AddHTMLGump( 105, 205, 140, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9813, socket.language ) + ":</basefont>" );// Min Time
	spawner.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9814, socket.language ));// Minimum time in min to respawn
	spawner.AddHTMLGump( 180, 205, 140, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9815, socket.language ) + ":</basefont>" );// Max Time
	spawner.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9816, socket.language ));// Maximum time in min to respawn
	spawner.AddBackground( 105, 230, 50, 28, 5120 );
	spawner.AddBackground( 185, 230, 50, 28, 5120 );

	// Radius
	spawner.AddHTMLGump( 260, 205, 170, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9817, socket.language ) + ":</basefont>" );// Radius (Area)
	spawner.AddToolTip( spawnEditorTooltipClilocID, socket, GetDictionaryEntry( 9818, socket.language ));// Radius to spawn NPCs in (Area-spawner only!)
	spawner.AddBackground( 280, 230, 50, 28, 5120 );

	// Apply!
	//spawner.AddButton( 300, 190, 2122, 2124, 1, 0, 1 );
	spawner.AddButton( 230, 265, 4005, 4007, 1, 0, 1 );
	spawner.AddHTMLGump( 265, 267, 140, 22, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9819, socket.language ) + "</basefont>" );// Apply Changes

	// Text Entry fields
	spawner.AddTextEntry( 125, 35, 140, 25, 1153, 0, 14, iUsed.name);
	spawner.AddTextEntry( 30, 165, 178, 20, 1153, 0, 15, iUsed.spawnsection );
	spawner.AddTextEntry( 45, 235, 115, 20, 1153, 0, 16, iUsed.amount );
	spawner.AddTextEntry( 115, 235, 40, 20, 1153, 0, 17, iUsed.mininterval );
	spawner.AddTextEntry( 195, 235, 40, 20, 1153, 0, 18, iUsed.maxinterval );
	spawner.AddTextEntry( 295, 235, 115, 20, 1153, 0, 19, iUsed.GetMoreVar( "more", 3 ));

	spawner.Send( socket );
	spawner.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;

	// Fetch data from text-fields
	var spawnerName = gumpData.getEdit( 0 );
	var spawnSection = gumpData.getEdit( 1 );
	var spawnAmount = parseInt( gumpData.getEdit( 2 ));
	var minInterval = parseInt( gumpData.getEdit( 3 ));
	var maxInterval = parseInt( gumpData.getEdit( 4 ));
	var spawnRadius = parseInt( gumpData.getEdit( 5 ));

	// Fetch data from radio/checkbox-buttons
	var radiobtnGroup1 = gumpData.getButton( 0 );
	var checkBtnSpawnlist = gumpData.getButton( 1 );

	switch( pButton )
	{
		case 0: // Close gump, no changes
			break;
		case 1:
			// Handle Spawner Type Radiobuttons
			switch( radiobtnGroup1 )
			{
				case 0: // item
					if( iUsed.type != 61 )
					{
						iUsed.TextMessage( GetDictionaryEntry( 9820, socket.language ), false, 0x3b2, 0, pUser.serial );// Changed Spawner Type to Item (61), amount reset to 1.
						iUsed.type = 61;
						iUsed.amount = 1;
						spawnAmount = 1;
						spawnRadius = 0;
					}
					break
				case 1: // npc
					if( iUsed.type != 62 )
					{
						iUsed.TextMessage( GetDictionaryEntry( 9821, socket.language ), false, 0x3b2, 0, pUser.serial );// Changed Spawner Type to NPC (62).
						iUsed.type = 62;
						spawnRadius = 0;
					}
					break;
				case 2: // arena
					if( iUsed.type != 69 )
					{
						iUsed.TextMessage( GetDictionaryEntry( 9822, socket.language ), false, 0x3b2, 0, pUser.serial );// Changed Spawner Type to Area (69), spawn radius updated (defaults to 1)
						iUsed.type = 69;
						if( spawnRadius == 0 )
						{
							spawnRadius = 1;
						}
					}
					break;
				case 3: // escort
					if( iUsed.type != 125 )
					{
						iUsed.TextMessage( GetDictionaryEntry( 9823, socket.language ), false, 0x3b2, 0, pUser.serial );// Changed Spawner Type to Escort (125).
						iUsed.type = 125;
						spawnRadius = 0;
					}
					break;
				default:
					break;
			}

			// Update spawner properties
			if( spawnSection != iUsed.spawnsection )
			{
				iUsed.spawnsection = spawnSection;
				iUsed.TextMessage( GetDictionaryEntry( 9824, socket.language ) + spawnSection, false, 0x3b2, 0, pUser.serial );// Spawn Section updated to 
			}
			if( minInterval != iUsed.mininterval )
			{
				iUsed.mininterval = minInterval;
				iUsed.TextMessage( GetDictionaryEntry( 9825, socket.language ) + minInterval, false, 0x3b2, 0, pUser.serial );// Min Time updated to 
			}
			if( maxInterval != iUsed.maxinterval )
			{
				iUsed.maxinterval = maxInterval;
				iUsed.TextMessage( GetDictionaryEntry( 9826, socket.language ) + maxInterval, false, 0x3b2, 0, pUser.serial );// Max Time updated to 
			}
			if( spawnAmount != iUsed.amount )
			{
				iUsed.amount = spawnAmount;
				iUsed.TextMessage( GetDictionaryEntry( 9827, socket.language ) + spawnAmount, false, 0x3b2, 0, pUser.serial );// Amount updated to 
			}
			if( spawnRadius != iUsed.GetMoreVar( "more", 3 ))
			{
				iUsed.SetMoreVar( "more", 3, spawnRadius );
				iUsed.SetMoreVar( "more", 4, spawnRadius );
				iUsed.TextMessage( GetDictionaryEntry( 9828, socket.language ) + spawnRadius, false, 0x3b2, 0, pUser.serial );// Spawn Radius updated to 
			}

			// Update spawner name
			if( spawnerName == null || spawnerName == "" )
			{
				socket.SysMessage( GetDictionaryEntry( 9270, socket.language )); // That name is too short, or no name was entered.
			}
			else if( spawnerName.length > 50 )
			{
				pSocket.SysMessage( GetDictionaryEntry( 9271, pSocket.language )); // That name is too long. Maximum 50 chars.
			}
			else if( spawnerName != iUsed.name )
			{
				iUsed.name = spawnerName;
				iUsed.TextMessage( GetDictionaryEntry( 9829, socket.language ) + spawnerName, false, 0x3b2, 0, pUser.serial );// Spawner name updated to 
			}

			// Handle Spawnlist Checkbox
			if( checkBtnSpawnlist == -1 )
			{
				if( iUsed.sectionalist )
				{
					iUsed.TextMessage( GetDictionaryEntry( 9830, socket.language ), false, 0x3b2, 0, pUser.serial );// No longer using spawn list - assuming spawn section now refers to individual NPC definition.
					iUsed.sectionalist = false;
				}
			}
			else
			{
				if( !iUsed.sectionalist )
				{
					iUsed.TextMessage( GetDictionaryEntry( 9831, socket.language ), false, 0x3b2, 0, pUser.serial );// Now using spawn list - assuming spawn section now refers to an NPC list.
					iUsed.sectionalist = true;
				}
			}

			spawnerGump( socket, pUser, iUsed );
			break;
		default:
			break;
	}
}

function onTooltip( spawner )
{
	var typeName = "";
	switch( spawner.type )
	{
		case 61: typeName = "<basefont color=#00ff00><big>Item</big></basefont>"; break
		case 62: typeName = "<basefont color=#00ff00><big>Npc</big></basefont>"; break
		case 69: typeName = "<basefont color=#00ff00><big>Area</big></basefont>"; break
		case 125: typeName = "<basefont color=#00ff00><big>Escort</big></basefont>"; break
	}
	var spawnList = "";
	if( spawner.sectionalist )
	{
		spawnList = "<basefont color=#00ff00><big>Enabled</big></basefont>";
	}
	else
	{
		spawnList = "<basefont color=#ff2800><big>Disabled</big></basefont>";
	}

	var tooltipText = "";
	tooltipText = "Spawn Section: " + spawner.spawnsection;
	tooltipText += "\n" + "Spawn Type: " + typeName;
	tooltipText += "\n" + "Spawn List: " + spawnList;
	tooltipText += "\n" + "Amount: " + "<basefont color=#00ff00>" + spawner.amount + "</big></basefont >";
	tooltipText += "\n" + "Min Interval: " + "<basefont color=#00ff00>" + spawner.mininterval + "</big></basefont>" + " Max Interval: " + "<basefont color=#00ff00>" + spawner.maxinterval + "</big></basefont>";
	if( spawner.type == 69 )
	{
		tooltipText += "\n" + "Radius: " + "<basefont color=#00ff00>" + spawner.GetMoreVar( "more", 3 ) + "</big></basefont >";
	}
	tooltipText += "\n" + "Region: " + "<basefont color=#00ff00>" + spawner.region.name + "</big></basefont>";
	return tooltipText;
}