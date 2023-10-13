// Admin Welcome Gump
// Triggered for characters logging in with admin account (account 0) until a
// choice has been made in these gumps, regarding starting with a blank world or loading default UOX3 world templates
//
// Last updated: 10th of October, 2023

// Backgrounds
const gumpMainBackground = 5054;
const gumpMainBackgroundWidth = 400;
const gumpMainBackgroundHeight = 300;
const gumpSecondaryBackground = 5120;

// Buttons
const defaultWorldButtonOff = 1147;
const defaultWorldButtonOn = 1148;
const blankSlateButtonOff = 1144;
const blankSlateButtonOn = 1145;
const okayButtonOff = 2128;
const okayButtonOn = 2129;
const previousPageButtonOff = 2119;
const previousPageButtonOn = 2120;
const exitButtonOff = 4017;
const exitButtonOn = 4018;

const facetFelucca = 1;
const facetTrammel = 1;
const facetIlshenar = 1;
const facetMalas = 0;
const facetTokuno = 0;
const facetTermur = 0;

// Transparency for background gumps
const enableTransparentGump = true;

// Tooltips for extra info
var enableTooltips = true;
const tooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day

// script ID assigned to script in jse_fileassocations.scp
const scriptID = 1;

function DisplayAdminWelcomeGump( socket, pChar )
{
	if( socket == null )
		return;

	// Close existing instance of gump that's open, if any
	var gumpID = scriptID + 0xffff;
	socket.CloseGump( gumpID, 0 );

	// Display welcome gump, and assign it a specific gumpID
	var adminWelcome = new Gump();
	adminWelcome.NoClose();
	adminWelcome.NoDispose();

	// Add the first page, with greeting and basic options
	adminWelcome.AddPage( 1 );
	adminWelcome.AddBackground( 0, 0, gumpMainBackgroundWidth, gumpMainBackgroundHeight, gumpMainBackground );
	adminWelcome.AddButton( 345, 5, exitButtonOff, exitButtonOn, 1, 0, 0 ); 	// Exit Button
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Close Gump" ); // Close Gump menu
	}

	// Add a checkered overlay over the background we just added
	if( enableTransparentGump )
	{
		adminWelcome.AddCheckerTrans( 0, 5, gumpMainBackgroundWidth, gumpMainBackgroundHeight - 10 );
	}

	// Add a header at the top of the page
	adminWelcome.AddHTMLGump( 0, 5, 400, 60, 0, 0, "<CENTER><h1><BASEFONT color=#FFFFFF>Welcome to UOX3</BASEFONT></h1></CENTER>" );

	// Add a background behind the htmlGump
	// adminWelcome.AddBackground( 15, 30, 370, 80, 3000 );
	adminWelcome.AddHTMLGump( 20, 30, 350, 80, true, false, "<p>Greetings, Administrator, and welcome to your brand new <basefont color=#2D61D6>Ultima Offline eXperiment 3</basefont> shard! If this is your first login, please take a moment to look through the following options:</p>" );

	// adminWelcome.AddBackground( 15, 117, 260, 80, 3000 );
	adminWelcome.AddHTMLGump( 20, 117, 240, 80, true, false, "<p>Start with the <basefont color=#3D9A2B>default world</basefont> to get a world already decorated with doors, signs and other items, and a variety of NPCs!</p>" );
	adminWelcome.AddHTMLGump( 120, 117, 120, 80, false, false, " " );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Okay, load default UOX3 world, please!" ); // Okay, load worldfiles
	}
	adminWelcome.AddGump( -30, 150, 30065 );

	// adminWelcome.AddBackground( 15, 205, 260, 80, 3000 );
	adminWelcome.AddHTMLGump( 20, 205, 240, 80, true, false, "<p>Alternatively, start out from scratch with <basefont color=#AD3C3C>a blank slate</basefont>, with no pre-defined decorations added to the world.</p>" );
	adminWelcome.AddGump( -30, 230, 30070 );

	adminWelcome.AddGump( 367, 0, 10410 );
	adminWelcome.AddGump( 367, 200, 10412 );

	adminWelcome.AddButton( 290, 145, defaultWorldButtonOff, defaultWorldButtonOn, 0, 2, 0 ); 	// Okay Button
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Okay, load default UOX3 world, please!" ); // Okay, load worldfiles
	}
	adminWelcome.AddHTMLGump( 285, 125, 160, 30, false, false, "<basefont color=#5ADC40>Default World</basefont>" );

	adminWelcome.AddButton( 290, 230, blankSlateButtonOff, blankSlateButtonOn, 0, 20, 0 ); 	// Cancel Button
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "I want to start with a blank slate!" ); // Cancel Gump menu
	}
	adminWelcome.AddHTMLGump( 290, 210, 85, 30, false, false, "<basefont color=#AD3C3C>Blank Slate</basefont>" );

	////////// PAGE 2 //////////
	if( facetFelucca )
	{
		adminWelcome = AddPageDetails( socket, adminWelcome, 2, 0 );
	}

	////////// PAGE 3 //////////
	if( facetTrammel )
	{
		adminWelcome = AddPageDetails( socket, adminWelcome, 3, 20 );
	}

	////////// PAGE 4 //////////
	if( facetIlshenar )
	{
		adminWelcome = AddPageDetails( socket, adminWelcome, 4, 40 );
	}

	////////// PAGE 5 //////////
	if( facetMalas )
	{
		adminWelcome = AddPageDetails( socket, adminWelcome, 5, 60 );
	}

	////////// PAGE 6 //////////
	if( facetTokuno )
	{
		adminWelcome = AddPageDetails( socket, adminWelcome, 6, 80 );
	}

	////////// PAGE 7 //////////
	if( facetTermur )
	{
		adminWelcome = AddPageDetails( socket, adminWelcome, 7, 100 );
	}

	////////// PAGE 20 - Blank Slate //////////
	// Add the first page, with greeting and basic options
	adminWelcome.AddPage( 20 );
	adminWelcome.AddBackground( 0, 0, gumpMainBackgroundWidth, gumpMainBackgroundHeight, gumpMainBackground );
	adminWelcome.AddButton( 345, 5, exitButtonOff, exitButtonOn, 1, 0, 0 ); 	// Exit Button

	adminWelcome.AddBackground( 10, 25, 140, 230, gumpSecondaryBackground );
	// Add a checkered overlay over the background we just added
	if( enableTransparentGump )
	{
		adminWelcome.AddCheckerTrans( 0, 5, gumpMainBackgroundWidth, gumpMainBackgroundHeight - 10 );
	}

	// Add a header at the top of the page
	adminWelcome.AddHTMLGump( 0, 5, 400, 60, 0, 0, "<CENTER><h1><BASEFONT color=#AD3C3C>UOX3 - A Blank Slate</BASEFONT></h1></CENTER>" );

	// Add buttons to cancel/get things going
	adminWelcome.AddButton( 150, 265, previousPageButtonOff, previousPageButtonOn, 0, 1, 0 ); 	// Previous Page Button
	adminWelcome.AddButton( 245, 265, okayButtonOff, okayButtonOn, 1, 0, 1 ); 	// Okay Button


	// Add some helpful text
	adminWelcome.AddHTMLGump( 170, 30, 230, 225, true, false, "<p>Select this option to start with <basefont color=#AD3C3C>a blank slate</basefont>, an empty world in which you are in control of what is added, and where. Note that you can return to this menu at any time via the <basefont color=#2D61D6>'welcome <basefont color=BLACK>command!<br><br>Also, be sure to check out the <basefont color=#2D61D6>resources <basefont color=black>on the left to get started! Have fun!</p>" );

	adminWelcome.AddHTMLGump( 20, 30, 135, 225, false, false, '<center><h1><basefont color=#ffffff>Docs</basefont></h1></center><a href="https://www.uox3.org/docs">UOX3 Docs</a><br><a href="https://www.uox3.org/docs/jsdocs.html">UOX3 JS Docs</a><br><a href="https://www.uox3.org/docs/index.html#commandList">Command List</a><br><a href="https://www.uox3.org/docs/index.html#useFirstGMCommands">First GM Commands</a><br><center><h1><basefont color=#ffffff>Community</basefont></h1></center><a href="https://www.uox3.org">UOX3.org</a><br><a href="https://www.uox3.org/forums">UOX3 Forums</a><br><a href="https://discord.gg/uBAXxhF">Discord</a>');

	adminWelcome.AddGump( 367, 0, 10410 );
	adminWelcome.AddGump( 367, 200, 10412 );

	////////// Finalize Gump and send to socket //////////

	adminWelcome.Send( socket );
	adminWelcome.Free();
}

function AddPageDetails( socket, adminWelcome, pageNum, checkboxStartID )
{
	// Add another page
	adminWelcome.AddPage( pageNum );

	// Add background, and checkered overlay
	//adminWelcome.AddBackground( 0, 0, 530, gumpMainBackgroundHeight + 130, gumpMainBackground );
	adminWelcome.AddBackground( 0, 0, 800, gumpMainBackgroundHeight + 130, gumpMainBackground );

	if( enableTransparentGump )
	{
		//adminWelcome.AddCheckerTrans( 0, 5, 530, gumpMainBackgroundHeight + 120 );
		adminWelcome.AddCheckerTrans( 0, 5, 800, gumpMainBackgroundHeight + 120 );
	}

	//adminWelcome.AddButton( 460, 5, exitButtonOff, exitButtonOn, 1, 0, 0 ); 	// Exit Button
	adminWelcome.AddButton( 720, 5, exitButtonOff, exitButtonOn, 1, 0, 0 ); 	// Exit Button

	// Add a header at top of page
	adminWelcome.AddHTMLGump( 0, 5, 500, 60, 0, 0, "<CENTER><h1><BASEFONT color=#5ADC40>Default UOX3 World - Options</BASEFONT></h1></CENTER>" );

	// Add section with facet buttons
	adminWelcome.AddBackground( 225, 30, 115, 275, gumpSecondaryBackground );
	adminWelcome.AddHTMLGump( 230, 40, 100, 20, 0, 0, "<CENTER><BIG><BASEFONT color=#3D9A2B>Facets</BASEFONT></BIG></CENTER>" );

	// If Felucca facet is enabled
	if( facetFelucca )
	{
		adminWelcome.AddButton( 230, 60, ( pageNum == 2 ? 10820 : 10800 ), ( pageNum == 2 ? 10800 : 10820 ), 0, 2, 0 );
	}
	else
	{
		adminWelcome.AddGump( 230, 60, 10840 );
	}

	// If Trammel facet is enabled
	if( facetTrammel )
	{
		adminWelcome.AddButton( 230, 100, ( pageNum == 3 ? 10820 : 10800 ), ( pageNum == 3 ? 10800 : 10820 ), 0, 3, 0 );
	}
	else
	{
		adminWelcome.AddGump( 230, 100, 10840 );
	}

	// If Ilshenar facet is enabled
	if( facetIlshenar )
	{
		adminWelcome.AddButton( 230, 140, ( pageNum == 4 ? 10820 : 10800 ), ( pageNum == 4 ? 10800 : 10820 ), 0, 4, 0 );
	}
	else
	{
		adminWelcome.AddGump( 230, 140, 10840 );
	}

	// If Malas facet is enabled
	if( facetMalas )
	{
		adminWelcome.AddButton( 230, 180, ( pageNum == 5 ? 10820 : 10800 ), ( pageNum == 5 ? 10800 : 10820 ), 0, 5, 0 );
	}
	else
	{
		adminWelcome.AddGump( 230, 180, 10840 );
	}

	// If Tokuno facet is enabled
	if( facetTokuno )
	{
		adminWelcome.AddButton( 230, 220, ( pageNum == 6 ? 10820 : 10800 ), ( pageNum == 6 ? 10800 : 10820 ), 0, 6, 0 );
	}
	else
	{
		adminWelcome.AddGump( 230, 220, 10840 );
	}

	// If Termur facet is enabled
	if( facetTermur )
	{
		adminWelcome.AddButton( 230, 260, ( pageNum == 7 ? 10820 : 10800 ), ( pageNum == 7 ? 10800 : 10820 ), 0, 7, 0 );
	}
	else
	{
		adminWelcome.AddGump( 230, 260, 10840 );
	}

	adminWelcome.AddHTMLGump( 260, 65, 70, 20, 0, 0, "<BASEFONT color=#" + ( pageNum == 2 ? "EEEEEE" : "BEBEBE") + ">FELUCCA</BASEFONT>" );
	adminWelcome.AddHTMLGump( 260, 105, 70, 20, 0, 0, "<BASEFONT color=#" + ( pageNum == 3 ? "EEEEEE" : "BEBEBE") + ">TRAMMEL</BASEFONT>" );
	adminWelcome.AddHTMLGump( 260, 145, 70, 20, 0, 0, "<BASEFONT color=#" + ( pageNum == 4 ? "EEEEEE" : "BEBEBE") + ">ILSHENAR</BASEFONT>" );
	adminWelcome.AddHTMLGump( 260, 185, 70, 20, 0, 0, "<BASEFONT color=#" + ( pageNum == 5 ? "EEEEEE" : "BEBEBE") + ">MALAS</BASEFONT>" );
	adminWelcome.AddHTMLGump( 260, 225, 70, 20, 0, 0, "<BASEFONT color=#" + ( pageNum == 6 ? "EEEEEE" : "BEBEBE") + ">TOKUNO</BASEFONT>" );
	adminWelcome.AddHTMLGump( 260, 265, 70, 20, 0, 0, "<BASEFONT color=#" + ( pageNum == 7 ? "EEEEEE" : "BEBEBE") + ">TER MUR</BASEFONT>" );

	// Add section with object category checkboxes
	adminWelcome.AddBackground( 350, 30, 115, 275, gumpSecondaryBackground );
	adminWelcome.AddHTMLGump( 360, 40, 100, 20, 0, 0, "<CENTER><BIG><BASEFONT color=#3D9A2B>Types</BASEFONT></BIG></CENTER>" );
	if( pageNum == 2 || pageNum == 3 || pageNum == 4 )
	{
		// Only add checkboxes for pages that actually have decorations
		adminWelcome.AddCheckbox( 360, 60, 9722, 2153, 1, checkboxStartID );
		adminWelcome.AddCheckbox( 360, 90, 9722, 2153, 1, checkboxStartID + 1 );
		adminWelcome.AddCheckbox( 360, 120, 9722, 2153, 1, checkboxStartID + 2 );
		adminWelcome.AddCheckbox( 360, 150, 9722, 2153, 1, checkboxStartID + 3 );
		adminWelcome.AddCheckbox( 360, 180, 9722, 2153, 1, checkboxStartID + 4 );
		adminWelcome.AddCheckbox( 360, 210, 9722, 2153, 1, checkboxStartID + 5 );
		adminWelcome.AddCheckbox( 360, 240, 9722, 2153, 1, checkboxStartID + 6 );
		adminWelcome.AddCheckbox( 360, 270, 9722, 2153, 1, checkboxStartID + 7 );
	}
	adminWelcome.AddHTMLGump( 395, 65, 70, 20, 0, 0, "<BASEFONT color=#EEEEEE>Doors</BASEFONT>" );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Doors!<br> Almost every building has one - or more!" );
	}
	adminWelcome.AddHTMLGump( 395, 95, 70, 20, 0, 0, "<BASEFONT color=#EEEEEE>Signs</BASEFONT>" );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Shops signs, roadsigns, etc." );
	}
	adminWelcome.AddHTMLGump( 395, 125, 70, 20, 0, 0, "<BASEFONT color=#EEEEEE>Lights</BASEFONT>" );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Lights!<br> lampposts, candles, wall-mounted torches, etc." );
	}
	adminWelcome.AddHTMLGump( 395, 155, 70, 20, 0, 0, "<BASEFONT color=#EEEEEE>Moongates</BASEFONT>" );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Moongates at fixed locations around the world" );
	}
	adminWelcome.AddHTMLGump( 395, 185, 70, 20, 0, 0, "<BASEFONT color=#EEEEEE>Containers</BASEFONT>" );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Containers! Bookcases, drawers, crates, etc." );
	}
	adminWelcome.AddHTMLGump( 395, 215, 70, 20, 0, 0, "<BASEFONT color=#EEEEEE>Teleporters</BASEFONT>" );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Teleporters that teleport players<br> from one location to another" );
	}
	adminWelcome.AddHTMLGump( 395, 245, 70, 20, 0, 0, "<BASEFONT color=#EEEEEE>Misc</BASEFONT>" );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Misc items like plants,<br> and smaller decorations in houses, etc" );
	}
	adminWelcome.AddHTMLGump( 395, 275, 70, 20, 0, 0, "<BASEFONT color=#EEEEEE>Spawns</BASEFONT>" );
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Spawn Regions with NPCs" );
	}

	// Add section with facet-specific additions
	adminWelcome.AddBackground( 475, 30, 310, 380, gumpSecondaryBackground );
	switch( pageNum )
	{
		case 2: // Felucca
			adminWelcome.AddHTMLGump( 480, 40, 250, 20, 0, 0, "<CENTER><BIG><BASEFONT color=#3D9A2B>Felucca Decoration Addons</BASEFONT></BIG></CENTER>" );

			// Magincia, client v6.0.3.1 or older
			adminWelcome.AddCheckbox( 480, 60, 9722, 2153, 0, checkboxStartID + 10 );
			adminWelcome.AddHTMLGump( 515, 65, 250, 20, 0, 0, "<BASEFONT color=#EEEEEE>Magincia</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 85, 250, 40, true, false, "Original city of Magincia, prior to destruction (pre-v6.0.3.1)" );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for original city of Magincia, prior to its destruction.<br>Only exists in client versions earlier than 6.0.3.1!" );
			}

			// Sea Market
			adminWelcome.AddCheckbox( 480, 130, 9722, 2153, 0, checkboxStartID + 11 );
			adminWelcome.AddHTMLGump( 515, 135, 250, 20, 0, 0, "<BASEFONT color=#EEEEEE>Sea Market</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 155, 250, 40, true, false, "Floating wharf/village at sea, added in High Seas (v7.0.9.0 and later)" );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for floating wharf/village in the middle of the sea, only accessible by boat.<br> Added in client v7.0.9.0!" );
			}
			break;
		case 3: // Trammel
			adminWelcome.AddHTMLGump( 480, 40, 250, 20, 0, 0, "<CENTER><BIG><BASEFONT color=#3D9A2B>Trammel Decoration Addons</BASEFONT></BIG></CENTER>" );

			// Castle Blackthorn in Britain
			adminWelcome.AddCheckbox( 480, 60, 9722, 2153, 0, checkboxStartID + 10 );
			adminWelcome.AddHTMLGump( 515, 65, 200, 20, 0, 0, "<BASEFONT color=#EEEEEE>Castle Blackthorn - New</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 85, 250, 40, true, false, "New revamped castle in Britain (v7.0.37 and later)" );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for royal residence of Lord Blackthorn.<br> Revamped and rebuilt version of castle; original was destroyed in 2012." );
			}

			// Castle Blackthorn in Britain
			adminWelcome.AddCheckbox( 480, 130, 9722, 2153, 0, checkboxStartID + 11 );
			adminWelcome.AddHTMLGump( 515, 135, 250, 20, 0, 0, "<BASEFONT color=#EEEEEE>Castle Blackthorn - Original</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 155, 250, 40, true, false, "Original castle in Britain (pre-v7.0.37?)." );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for royal residence of Lord Blackthorn.<br> Original castle destroyed and rebuilt in 2012" );
			}

			// Magincia, client v6.0.3.1 or older
			adminWelcome.AddCheckbox( 480, 200, 9722, 2153, 0, checkboxStartID + 12 );
			adminWelcome.AddHTMLGump( 515, 205, 250, 20, 0, 0, "<BASEFONT color=#EEEEEE>Magincia</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 225, 250, 40, true, false, "Original city of Magincia, prior to destruction (pre-v6.0.3.1)" );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for original city of Magincia, prior to its destruction.<br>Only exists in client versions earlier than 6.0.3.1!" );
			}

			// Sea Market
			adminWelcome.AddCheckbox( 480, 270, 9722, 2153, 0, checkboxStartID + 13 );
			adminWelcome.AddHTMLGump( 515, 275, 250, 20, 0, 0, "<BASEFONT color=#EEEEEE>New Haven</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 295, 250, 40, true, false, "Young Player city, replaces Ocllo on Trammel (v6.0.0 and later)" );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for New Haven, that exists in place of Ocllo on Trammel facet.<br>Replaced the earlier Ocllo-replacement Haven!" );
			}

			// New Haven
			adminWelcome.AddCheckbox( 480, 340, 9722, 2153, 0, checkboxStartID + 14 );
			adminWelcome.AddHTMLGump( 515, 345, 250, 20, 0, 0, "<BASEFONT color=#EEEEEE>Sea Market</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 365, 250, 40, true, false, "Floating wharf/village at sea, added in High Seas (v7.0.9.0 and later)" );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for floating wharf/village in the middle of the sea, only accessible by boat.<br> Added in client v7.0.9.0!" );
			}
			break;

		case 4: // Ilshenar
			adminWelcome.AddHTMLGump( 480, 40, 250, 20, 0, 0, "<CENTER><BIG><BASEFONT color=#3D9A2B>Ilshenar Decoration Addons</BASEFONT></BIG></CENTER>" );

			// Gargoyle City
			adminWelcome.AddCheckbox( 480, 60, 9722, 2153, 0, checkboxStartID + 10 );
			adminWelcome.AddHTMLGump( 515, 65, 200, 20, 0, 0, "<BASEFONT color=#EEEEEE>Gargoyle City</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 85, 250, 40, true, false, "Ver Lor Reg - the city of Gargoyles, prior to destruction (pre-v7.0.30)" );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for Ver Lor Reg, the city of Gargoyles, prior to its destruction during The Awakening story arc in 2012" );
			}

			// Castle Blackthorn (Ilshenar) - Golems and Jukas
			adminWelcome.AddCheckbox( 480, 130, 9722, 2153, 0, checkboxStartID + 11 );
			adminWelcome.AddHTMLGump( 515, 135, 250, 20, 0, 0, "<BASEFONT color=#EEEEEE>Castle Blackthorn (Ilshenar)</BASEFONT>" );
			adminWelcome.AddHTMLGump( 515, 155, 250, 40, true, false, "Castle Blackthorn in Ilshenar, prior to destruction (pre-v7.0.30)." );
			if( enableTooltips )
			{
				adminWelcome.AddToolTip( tooltipClilocID, socket, "Decorations for Blackthorn's castle in Ilshenar, filled with golems and jukas, prior to its destruction during The Awakening story arc in 2012." );
			}
			break;
		default:
			break;
	}

	adminWelcome.AddGump( 753, 0, 10410 );
	adminWelcome.AddGump( 753, 200, 10412 );

	// Add some helpful text
	//adminWelcome.AddHTMLGump( 15, 30, 250, 275, true, false, "Select which parts of the default UOX3 world and Spawns to load for each facet. By default everything is enabled for supported facets" );
	adminWelcome.AddHTMLGump( 15, 30, 200, 275, true, false, "<p>Select which parts of the <basefont color=#3D9A2B>default UOX3 world</basefont> and Spawns to load for each facet. Facets <basefont color=#AD3C3C>in red</basefont><basefont color=black> have no decorations available yet.<br><br>This process might take a minute.<br><br>Return to this menu at any time via the <basefont color=#2D61D6>'welcome <basefont color=BLACK>command!</p>" );
	//adminWelcome.AddHTMLGump( 15, 30, 250, 275, true, false, "Select which parts of the<br>default UOX3 world and spawns<br>to load for each facet." );

	// Add buttons to cancel/get things going
	adminWelcome.AddButton( 308, 313, previousPageButtonOff, previousPageButtonOn, 0, 1, 0 ); 	// Previous Page Button
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Go back!" );
	}
	adminWelcome.AddButton( 400, 305, defaultWorldButtonOff, defaultWorldButtonOn, 1, 0, 2 ); 	// Okay Button
	if( enableTooltips )
	{
		adminWelcome.AddToolTip( tooltipClilocID, socket, "Load default UOX3 world" );
	}

	var facetText = "";
	switch( pageNum )
	{
		case 2: // Felucca
			facetText = "<p><basefont color=#4B4B4B>Felucca - Original world of Britannia. Home of cities like Britain, Trinsic and Yew. Numerous dungeons like Covetous, Deceit and Shame can be found here.</basefont></p>";
			break;
		case 3: // Trammel
			facetText = "<p><basefont color=#4B4B4B>Trammel - Alternate version of Britannia, where non-consensual PvP and hostile actions against fellow players is traditionally both frowned upon and - in many cases - not even possible. Introduced with the UO: Renaissance expansion.</basefont></p>";
			break;
		case 4: // Ilshenar
			facetText = "<p><basefont color=#4B4B4B>Ilshenar - New landmass added with Third Dawn expansion. Originally only available to players with the Third Dawn 3D client, Ilshenar added new dungeons and monsters to the UO world.</basefont></p>";
			break;
		case 5: // Malas
			facetText = "<p><basefont color=#4B4B4B>Malas - A new landmass introduced with the Age of Shadows expansion, with lots of space for player housing, a PvP arena, the dungeon Doom - one of the largest in UO, and two cities - Luna and Umbra, the homes of Paladins and Necromancers respectively.</basefont></p>";
			break;
		case 6: // Tokuno
			facetText = "<p><basefont color=#4B4B4B>Tokuno - The Tokuno islands, based on feudal era Japan, were added with the Samurai Empire expansion, and consist of three main islands (Isamu, Makoto and Homare) and many smaller ones. There's only one city - Zento - and the facet traditionally follows the Trammel ruleset.</basefont></p>";
			break;
		case 7: // Ter Mur
			facetText = "<p><basefont color=#4B4B4B>Ter Mur - Land of the Gargoyles, added with the UO: Stygian Abyss expansion. Comes with three new towns (Royal City, Holy City, Fishing Village), and three new dungeons (Tomb of Kings, Stygian Abyss and Underworld). With the Time of Legends expansion, a new area called Valley of Eodon was added to this facet.</basefont></p>";
			break;
		default:
			facetText = "Invalid";
			break;
	}

	adminWelcome.AddBackground( 15, gumpMainBackgroundHeight + 20, 140, 20, gumpSecondaryBackground );
	adminWelcome.AddHTMLGump( 30, gumpMainBackgroundHeight + 20, 150, 20, false, false, "<p><basefont color=#ffffff>Facet Info:</basefont></p>" );

	adminWelcome.AddHTMLGump( 15, gumpMainBackgroundHeight + 40, 450, 80, true, true, facetText );

	return adminWelcome;
}

function DisplayLoadingWorldTips( socket )
{
	// Display tips while world decorations load
	var worldLoadTips = new Gump;
	worldLoadTips.NoClose();
	worldLoadTips.NoDispose();

	worldLoadTips.AddPage( 0 );
	worldLoadTips.AddBackground( 0, 0, gumpMainBackgroundWidth, gumpMainBackgroundHeight + 120, gumpMainBackground );
	worldLoadTips.AddButton( 345, 5, exitButtonOff, exitButtonOn, 1, 0, 0 ); 	// Exit Button
	if( enableTooltips )
	{
		worldLoadTips.AddToolTip( tooltipClilocID, socket, "Close Gump" ); // Close Gump menu
	}
	worldLoadTips.AddBackground( 265, 95, 125, 290, gumpSecondaryBackground );

	// Add a checkered overlay over the background we just added
	if( enableTransparentGump )
	{
		worldLoadTips.AddCheckerTrans( 0, 5, gumpMainBackgroundWidth, gumpMainBackgroundHeight + 110 );
	}

	// Add a header at the top of the page
	worldLoadTips.AddHTMLGump( 0, 5, 400, 60, 0, 0, "<CENTER><h1><BASEFONT color=#FFFFFF>Loading Default UOX3 World...</BASEFONT></h1></CENTER>" );

	// Add a background behind the htmlGump
	worldLoadTips.AddHTMLGump( 20, 30, 370, 60, true, false, "<p>While you wait for the <basefont color=#3D9A2B>default UOX3 world</basefont> to get loaded, here's some helpful tips and links to get you started on your journey with <basefont color=#2D61D6>UOX3<basefont color=black>!</p>" );

	worldLoadTips.AddHTMLGump( 20, 97, 240, 140, true, false, "<p><basefont color=#2D61D6>Tip #1</basefont> - Don't be afraid to ask, if you're stumped on how to do something! The <basefont color=#2D61D6>UOX3 forums<basefont color=BLACK> and <basefont color=#2D61D6>Discord server <basefont color=BLACK>are there to be used, so don't hesitate to post on the forums or pop your head into #uox3 on Discord if you need help!</p>" );

	worldLoadTips.AddHTMLGump( 20, 245, 240, 140, true, false, "<p><basefont color=#2D61D6>Tip #2</basefont> - Early on, don't be afraid to break stuff! <basefont color=#2D61D6>Tweak each setting<basefont color=BLACK>, <basefont color=#2D61D6>explore every command<basefont color=BLACK>, and learn about the functionality and possibilities UOX3 offers before you bunker down to create your own dream shard in earnest!</p>" );

	worldLoadTips.AddHTMLGump( 278, 100, 105, 240, false, false, '<center><h1><basefont color=#ffffff>Docs</basefont></h1></center><a href="https://www.uox3.org/docs">UOX3 Docs</a><br><a href="https://www.uox3.org/jsdocs">UOX3 JS Docs</a><br><a href="https://www.uox3.org/docs/index.html#commandList">Command List</a><br><a href="https://www.uox3.org/docs/index.html#useFirstGMCommands">First Commands</a><br><center><h1><basefont color=#ffffff>Community</basefont></h1></center><a href="https://www.uox3.org">UOX3.org</a><br><a href="https://www.uox3.org/forums">UOX3 Forums</a><br><a href="https://discord.gg/uBAXxhF">Discord</a>');

	// Some dragon decorations!
	worldLoadTips.AddGump( 367, 0, 10410 );
	worldLoadTips.AddGump( 367, 200, 10412 );

	worldLoadTips.AddButton( 150, 393, 12006, 12007, 1, 0, 0 ); 	// Close Button
	if( enableTooltips )
	{
		worldLoadTips.AddToolTip( tooltipClilocID, socket, "Sounds good. Let me have at it!" );
	}

	worldLoadTips.Send( socket );
	worldLoadTips.Free();
}

function onGumpPress( pSocket, pButton, gumpData )
{
	totalCheckBoxes = 36;
	numCheckedBoxes = 0;
	tempGumpData = gumpData;

	switch( pButton )
	{
		case 0:
			break;
		case 1:
			SaveSeenWelcomeGump( pSocket );
			break;
		case 2:
			SaveSeenWelcomeGump( pSocket );
			numCheckedBoxes = gumpData.buttons;
			if( numCheckedBoxes == 0 )
			{
				pSocket.SysMessage( "No boxes checked! Nothing will spawn. Note that you can return to the previous menu using the 'welcome command!" );
				return;
			}
			else
			{
				DisplayLoadingWorldTips( pSocket );
				pSocket.currentChar.StartTimer( 100, 1, true );
			}
			break;
		default:
			break;
	}
}

function SaveSeenWelcomeGump( socket )
{
	// Create a new file object
	var mFile = new UOXCFile();
	var fileName = "welcomeGumpChoiceMade.jsdata";

	// Open file to store that admin has seen and made a choice in welcome gump
	mFile.Open( fileName, "w", "jsWorldData" );
	if( mFile != null )
	{
		// Write to file
		mFile.Write( "seenWelcome=1" + "\n" );

		// Close file and free up memory allocated by file object
		mFile.Close();

		// Free memory allocated by file object
		mFile.Free();
	}
}

function onTimer( timerObj, timerID )
{
	var pSocket = timerObj.socket;
	if( timerID == 1 )
	{
		if( numCheckedBoxes == totalCheckBoxes )
		{
			timerObj.ExecuteCommand( "decorate load" );
		}
		else
		{
			var facetName = "";
			var buttonID = -1;
			var doSpawnRegionRespawn = false;

			for( var i = 0; i < numCheckedBoxes; i++ )
			{
				buttonID = tempGumpData.getButton( i );
				if( buttonID >= 0 && buttonID <= 19 )
				{
					facetName = "felucca";
				}
				else if( buttonID >= 20 && buttonID <= 39 )
				{
					facetName = "trammel";
				}
				else if( buttonID >= 40 && buttonID <= 59 )
				{
					facetName = "ilshenar";
				}
				else if( buttonID >= 60 && buttonID <= 79 )
				{
					facetName = "malas";
				}
				else if( buttonID >= 80 && buttonID <= 99 )
				{
					facetName = "tokuno";
				}
				else if( buttonID >= 100 && buttonID <= 119 )
				{
					facetName = "termur";
				}

				switch( buttonID )
				{
					case 0: // Doors
					case 20:
					case 40:
					case 60:
					case 80:
					case 100:
						pSocket.currentChar.ExecuteCommand( "decorate load doors " + facetName + "|silent multiple" );
						break;
					case 1: // Signs
					case 21:
					case 41:
					case 61:
					case 81:
					case 101:
						pSocket.currentChar.ExecuteCommand( "decorate load signs " + facetName + "|silent multiple" );
						break;
					case 2: // Lights
					case 22:
					case 42:
					case 62:
					case 82:
					case 102:
						pSocket.currentChar.ExecuteCommand( "decorate load lights " + facetName + "|silent multiple" );
						break;
					case 3: // Containers
					case 23:
					case 43:
					case 63:
					case 83:
					case 103:
						pSocket.currentChar.ExecuteCommand( "decorate load containers " + facetName + "|silent multiple" );
						break;
					case 4: // Moongates
					case 24:
					case 44:
					case 64:
					case 84:
					case 104:
						pSocket.currentChar.ExecuteCommand( "decorate load moongates " + facetName + "|silent multiple" );
						break;
					case 5: // Teleporters
					case 25:
					case 45:
					case 65:
					case 85:
					case 105:
						pSocket.currentChar.ExecuteCommand( "decorate load teleporters " + facetName + "|silent multiple" );
						break;
					case 6: // Misc
					case 26:
					case 46:
					case 66:
					case 86:
					case 106:
						pSocket.currentChar.ExecuteCommand( "decorate load misc " + facetName + "|silent multiple" );
						break;
					// Spawns
					case 7: // Spawns, Felucca
						pSocket.currentChar.ExecuteCommand( "enablespawns 0" );
						doSpawnRegionRespawn = true;
						break;
					case 27: // Spawns, Trammel
						pSocket.currentChar.ExecuteCommand( "enablespawns 1" );
						doSpawnRegionRespawn = true;
						break;
					case 47: // Spawns, Ilshenar
						pSocket.currentChar.ExecuteCommand( "enablespawns 2" );
						doSpawnRegionRespawn = true;
						break;
					case 67: // Spawns, Malas
						pSocket.currentChar.ExecuteCommand( "enablespawns 3" );
						doSpawnRegionRespawn = true;
						break;
					case 87: // Spawns, Tokuno
						pSocket.currentChar.ExecuteCommand( "enablespawns 4" );
						doSpawnRegionRespawn = true;
						break;
					case 107: // Spawns, Ter Mur
						pSocket.currentChar.ExecuteCommand( "enablespawns 5" );
						doSpawnRegionRespawn = true;
						break;
					// Felucca Addons
					case 10: // Original Magincia
						pSocket.currentChar.ExecuteCommand( "decorate load felucca_magincia_original|silent multiple" );
						break;
					case 11: // Sea Market
						pSocket.currentChar.ExecuteCommand( "decorate load felucca_seamarket|silent multiple" );
						break;
					// Trammel Addons
					case 30: // Revamped Castle Blackthorn
						pSocket.currentChar.ExecuteCommand( "decorate load trammel_blackthorncastle|silent multiple" );
						break;
					case 31: // Original Magincia
						pSocket.currentChar.ExecuteCommand( "decorate load trammel_magincia_original|silent multiple" );
						break;
					case 32: // New Haven
						pSocket.currentChar.ExecuteCommand( "decorate load trammel_newhaven|silent multiple" );
						break;
					case 33: // Sea Market
						pSocket.currentChar.ExecuteCommand( "decorate load trammel_seamarket|silent multiple" );
						break;
					// Ilshenar Addons
					case 50: // Ver Lor Reg - Gargoyle City
						pSocket.currentChar.ExecuteCommand( "decorate load ilshenar_gargoyle_city|silent multiple" );
						break;
					case 51: // Castle Blackthorn (Ilshenar) - Golems and Jukas
						pSocket.currentChar.ExecuteCommand( "decorate load ilshenar_blackthorncastle|silent multiple" );
						break;
					default:
						break;
				}
			}

			if( doSpawnRegionRespawn )
			{
				pSocket.SysMessage( "Triggering spawn cycle in regional spawn system..." );
				pSocket.currentChar.ExecuteCommand( "regspawn all" );
			}
		}
	}
}