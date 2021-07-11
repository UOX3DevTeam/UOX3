var enableHPLOSS = 0;       // Disable or enable hp on tools to break
var enableTOOLBREAK = 0;    // Disable or enable tools to break

const scriptID = 4013;
const scriptID2 = 4014;
const scriptID3 = 4015;
const scriptID4 = 4016;
const scriptID5 = 4017;
const scriptID6 = 4018;
const scriptID7 = 4019;
const scriptID8 = 4020;
const scriptID9 = 4021;

function onUseChecked( pUser, iUsed )
{
    var socket = pUser.socket;
    if ( socket && iUsed && iUsed.isItem )
    {
        if ( enableHPLOSS == 1 )
        {
            iUsed.health -= 1;
        }
        else if ( enableTOOLBREAK = 1 && iUsed.health == 0 )
        {
            iUsed.Delete();
            pUser.SysMessage( GetDictionaryEntry( 2202, socket.Language ) ); // You have worn out your tool!
            return false;
        }
        else
        {
            socket.tempObj = iUsed;
            var targMsg = GetDictionaryEntry( 445, socket.Language );
            socket.CustomTarget( 1, targMsg );
        }
    }
    return false;
}

function onCallback1( socket, ourObj )
{
    var bItem = socket.tempObj;
    var mChar = socket.currentChar;
    var anvil = AreaItemFunction( "findNearbyItems", mChar, 2, mChar.socket );

    if ( mChar && mChar.isChar && bItem && bItem.isItem )
    {
        if ( !ourObj || !ourObj.isItem )
        {
            socket.SysMessage( GetDictionaryEntry( 979, socket.Language ) );
            return;
        }

        if ( anvil > 0 )
        {
            var resID = ourObj.id;
            if ( resID == 0x1BF2 || resID == 0x1BEF || resID == 0x26B4 ) 	// Is it a valid blacksmithing resource?
            {
                var ownerObj = GetPackOwner( ourObj, 0 );
                if ( ownerObj && mChar.serial == ownerObj.serial )
                {
                    BlackSmithGump( socket, mChar );
                }
                else
                    socket.SysMessage( "You can't smith " + ourObj.name + " outside your backpack." );
            }
            else
            {
                socket.SysMessage( GetDictionaryEntry( 2190, socket.language ) );// You can't make anything from that material.
                return;
            }
        }
        else
        {
            socket.SysMessage( GetDictionaryEntry( 2201, socket.Language ) ); // You must be near an anvil and a forge to smith items.
        }
    }
}

function findNearbyItems( pUser, trgItem, pSock )
{
    var isAnvil = ( trgItem.id == 0x0faf || trgItem.id == 0x0fb0 || trgItem.id == 0x2DD5 || trgItem.id == 0x2DD6 )
    var isForge = ( trgItem.id == 0x197A || trgItem.id == 0x0FB1 || trgItem.id == 0x19A9 || trgItem.id == 0x2DD8 )

    if ( trgItem && trgItem.isItem && isAnvil || isForge )
        return true;
    else
        return false;
}

function BlackSmithGump( socket, pUser )
{
    if ( socket == null )
        return;

    var gumpID = scriptID + 0xffff;
    var gumpID2 = scriptID2 + 0xffff;
    var gumpID3 = scriptID3 + 0xffff;
    var gumpID4 = scriptID4 + 0xffff;
    var gumpID5 = scriptID5 + 0xffff;
    var gumpID6 = scriptID6 + 0xffff;
    var gumpID7 = scriptID7 + 0xffff;
    var gumpID8 = scriptID8 + 0xffff;
    var gumpID9 = scriptID9 + 0xffff;
    socket.CloseGump( gumpID, 0 );
    socket.CloseGump( gumpID2, 0 );
    socket.CloseGump( gumpID3, 0 );
    socket.CloseGump( gumpID4, 0 );
    socket.CloseGump( gumpID5, 0 );
    socket.CloseGump( gumpID6, 0 );
    socket.CloseGump( gumpID7, 0 );
    socket.CloseGump( gumpID8, 0 );
    socket.CloseGump( gumpID9, 0 );
    var bsGump = new Gump;

    bsGump.AddPage( 0 );
    bsGump.AddBackground( 0, 0, 585, 393, 5054 );                             		            // Tile Gray Background
    bsGump.AddBackground( 194, 36, 386, 275, 3000 );		                                        // Tile White Bacground
    bsGump.AddHTMLGump( 60, 339, 160, 25, 0, 0, GetDictionaryEntry( 2189, socket.language ) );      // OKAY
    bsGump.AddButton( 26, 335, 0xfa5, 1, 0, 1 );		                                            // Button - OKAY
    bsGump.AddHTMLGump( 60, 359, 160, 25, 0, 0, GetDictionaryEntry( 2090, socket.language ) );      // CANCEL
    bsGump.AddButton( 26, 356, 0xfa5, 1, 0, 0 );                                                    // Button - CANCEL
    bsGump.AddHTMLGump( 60, 318, 160, 25, 0, 0, GetDictionaryEntry( 2212, socket.language ) );      // REPAIR
    bsGump.AddButton( 26, 315, 0xfa5, 1, 0, 11 );		                                            // Button - REPAIR
    bsGump.AddHTMLGump( 178, 4, 160, 25, 0, 0, GetDictionaryEntry( 2188, socket.language ) );       // Blacksmithy Selection Menu
    bsGump.AddBackground( 2, 32, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 35, 149, 25, 0, 0, GetDictionaryEntry( 2191, socket.language ) );        // Ringmail
    bsGump.AddButton( 156, 36, 0xfa5, 0, 1, 0 );		                                            // Button - Ringmail
    bsGump.AddBackground( 2, 57, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 60, 149, 25, 0, 0, GetDictionaryEntry( 2192, socket.language ) );        // Chainmail
    bsGump.AddButton( 156, 61, 0xfa5, 0, 2, 0 );		                                            // Button - Chainmail
    bsGump.AddBackground( 2, 82, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 85, 149, 25, 0, 0, GetDictionaryEntry( 2193, socket.language ) );        // Platemail
    bsGump.AddButton( 156, 86, 0xfa5, 0, 3, 0 );		                                            // Button - Platemail
    bsGump.AddBackground( 2, 107, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 110, 149, 25, 0, 0, GetDictionaryEntry( 2194, socket.language ) );       // Helmets
    bsGump.AddButton( 156, 111, 0xfa5, 0, 4, 0 );		                                            // Button - Helmets
    bsGump.AddBackground( 2, 132, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 135, 149, 25, 0, 0, GetDictionaryEntry( 2195, socket.language ) );       // Shields
    bsGump.AddButton( 156, 136, 0xfa5, 0, 5, 0 );		                                            // Button - Shields
    bsGump.AddBackground( 2, 157, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 160, 149, 25, 0, 0, GetDictionaryEntry( 2196, socket.language ) );       // Bladed
    bsGump.AddButton( 156, 161, 0xfa5, 0, 6, 0 );		                                            // Button - Bladed
    bsGump.AddBackground( 2, 182, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 185, 149, 25, 0, 0, GetDictionaryEntry( 2197, socket.language ) );       // Axes
    bsGump.AddButton( 156, 186, 0xfa5, 0, 7, 0 );		                                            // Button - Axes
    bsGump.AddBackground( 2, 207, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 210, 149, 25, 0, 0, GetDictionaryEntry( 2198, socket.language ) );       // Pole Arms
    bsGump.AddButton( 156, 211, 0xfa5, 0, 8, 0 );		                                            // Button - Pole Arms
    bsGump.AddBackground( 2, 232, 149, 25, 3000 );
    bsGump.AddHTMLGump( 5, 235, 149, 25, 0, 0, GetDictionaryEntry( 2199, socket.language ) );       // Bashing
    bsGump.AddButton( 156, 236, 0xfa5, 0, 9, 0 );		                                            // Button - Bashing

    if ( pUser.skills[7] >= 800 )
    {
        bsGump.AddHTMLGump( 228, 315, 160, 25, 0, 0, GetDictionaryEntry( 2203, socket.language ) );     // BRONZE
        bsGump.AddButton( 196, 315, 0xfa5, 1, 0, 2 );		                                            // Button - Bronze
    }
    if ( pUser.skills[7] >= 750 )
    {
        bsGump.AddHTMLGump( 228, 335, 160, 25, 0, 0, GetDictionaryEntry( 2204, socket.language ) );     // COPPER
        bsGump.AddButton( 196, 335, 0xfa5, 1, 0, 3 );		                                            // Button - Copper
    }
    if ( pUser.skills[7] >= 900 )
    {
        bsGump.AddHTMLGump( 228, 355, 160, 25, 0, 0, GetDictionaryEntry( 2205, socket.language ) );     // AGAPITE
        bsGump.AddButton( 196, 355, 0xfa5, 1, 0, 4 );		                                            // Button - Agapite
    }
    if ( pUser.skills[7] >= 650 )
    {
        bsGump.AddHTMLGump( 312, 315, 160, 25, 0, 0, GetDictionaryEntry( 2206, socket.language ) );     // DULL COPPER
        bsGump.AddButton( 280, 315, 0xfa5, 1, 0, 5 );		                                            // Button -DULL COPPER 
    }
    if ( pUser.skills[7] >= 850 )
    {
        bsGump.AddHTMLGump( 312, 335, 160, 25, 0, 0, GetDictionaryEntry( 2207, socket.language ) );     // GOLD
        bsGump.AddButton( 280, 335, 0xfa5, 1, 0, 6 );		                                            // Button - GOLD
    }
    if ( pUser.skills[7] >= 700 )
    {
        bsGump.AddHTMLGump( 312, 355, 160, 25, 0, 0, GetDictionaryEntry( 2208, socket.language ) );     // SHADOW IRON
        bsGump.AddButton( 280, 355, 0xfa5, 1, 0, 7 );		                                            // Button - SHADOW IRON
    }
    if ( pUser.skills[7] >= 990 )
    {
        bsGump.AddHTMLGump( 432, 315, 160, 25, 0, 0, GetDictionaryEntry( 2209, socket.language ) );     // VALORITE
        bsGump.AddButton( 400, 315, 0xfa5, 1, 0, 8 );		                                            // Button - VALORITE
    }
    if ( pUser.skills[7] >= 950 )
    {
        bsGump.AddHTMLGump( 432, 335, 160, 25, 0, 0, GetDictionaryEntry( 2210, socket.language ) );     // VERITE
        bsGump.AddButton( 400, 335, 0xfa5, 1, 0, 9 );		                                            // Button - VERITE
    }

    bsGump.AddPage( 1 );																			// Ringmail Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2217, socket.language ) );      // Ringmail Gloves
    bsGump.AddPicture( 210, 62, 0x13f2 );			                                                // Ringmail Gloves Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + "10 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 12.0 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 2 );															// Radio for Ringmail Gloves
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2218, socket.language ) );      // Ringmail Legs
    bsGump.AddPicture( 338, 64, 0x13f0 );			                                                // Ringmail Legs Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + "16 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 19.4 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 3 );															// Radio for Ringmail Legs
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2219, socket.language ) );      // Ringmail Arms
    bsGump.AddPicture( 480, 60, 0x13ee );			                                                // Ringmail Arms Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 16.9 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 4 );															// Radio for Ringmail Arms
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2220, socket.language ) );     // Ringmail Chest
    bsGump.AddPicture( 215, 126, 0x13ec );															// Ringmail Chest Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 21.9 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 5 );	                                                    // Radio for Ringmail Chest

    bsGump.AddPage( 2 );																			// Chainmail Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2221, socket.language ) );      // Chain Coif
    bsGump.AddPicture( 210, 62, 0x13bb );			                                                // Chain Coif Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 14.5 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 6 );															// Radio for Chain Coif
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2222, socket.language ) );      // Chain Legs
    bsGump.AddPicture( 348, 62, 0x13be );			                                                // Chain Legs Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 36.7 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 7 );															// Radio for Chain Legs
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2223, socket.language ) );      // Chain Chest
    bsGump.AddPicture( 470, 62, 0x13bf );			                                                // Chain Chest Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 20 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 39.1 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 8 );															// Radio for Chain Chest

    bsGump.AddPage( 3 );																			// Platemail Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2224, socket.language ) );      // Plate Arms
    bsGump.AddPicture( 213, 62, 0x1410 );			                                                // PlateArms Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 66.3 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 9 );															// Radio for PlateArms
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2225, socket.language ) );      // Plate Gloves
    bsGump.AddPicture( 342, 67, 0x1414 );			                                                // PlateGloves Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 58.9 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 10 );	                                                    // Radio for PlateGloves
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2226, socket.language ) );      // Plate Gorget
    bsGump.AddPicture( 476, 67, 0x1413 );			                                                // PlateGorget Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 56.4 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 11 );	                                                    // Radio for PlateGorget
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2227, socket.language ) );     // Plate Legs
    bsGump.AddPicture( 216, 130, 0x1411 );															// Plate Legs Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 20 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 68.8 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 12 );	                                                    // Radio for Plate Legs
    bsGump.AddHTMLGump( 327, 109, 149, 25, 0, 0, GetDictionaryEntry( 2228, socket.language ) );     // Plate Chest
    bsGump.AddPicture( 347, 130, 0x1415 );															// Plate Chest Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 25 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 75.0 <\center>" )
    bsGump.AddRadio( 327, 129, 0xd2, 0, 13 );	                                                    // Radio for Plate Chest
    bsGump.AddHTMLGump( 456, 109, 149, 25, 0, 0, GetDictionaryEntry( 2229, socket.language ) );     // Female Plate Chest
    bsGump.AddPicture( 474, 128, 0x1c04 );	 														// Female Plate Chest Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 20 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 44.1 <\center>" )
    bsGump.AddRadio( 456, 129, 0xd2, 0, 14 );	                                                    // Radio for FemalePlateChest

    bsGump.AddPage( 4 );																			// Helmets Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2230, socket.language ) );      // Bascinet
    bsGump.AddPicture( 217, 62, 0x140c );			                                                // Bascinet Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 8.3 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 15 );	                                                    // Radio for Bascinet
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2231, socket.language ) );      // Close Helm
    bsGump.AddPicture( 345, 62, 0x1408 );			                                                // CloseHelm Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 37.9 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 16 );	                                                    // Radio for CloseHelm
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2232, socket.language ) );      // Helmet
    bsGump.AddPicture( 470, 62, 0x140a );			                                                // Helmet Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 37.9 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 17 );	                                                    // Radio for Helmet
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2233, socket.language ) );     // Norse Helm
    bsGump.AddPicture( 220, 130, 0x140e );															// Norse Helm Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 37.9 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 18 );														// Radio for Norse Helm
    bsGump.AddHTMLGump( 327, 109, 149, 25, 0, 0, GetDictionaryEntry( 2234, socket.language ) );     // Plate Helm
    bsGump.AddPicture( 353, 130, 0x1419 );															// Plate Helm Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 62.6 <\center>" )
    bsGump.AddRadio( 327, 129, 0xd2, 0, 19 );	                                                    // Radio for Plate Helm

    bsGump.AddPage( 5 );																			// Shields Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2235, socket.language ) );      // Buckler
    bsGump.AddPicture( 210, 64, 0x1b73 );			                                                // Buckler Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 0.0 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 20 );	                                                    // Radio for Buckler
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2236, socket.language ) );      // Bronze Shield
    bsGump.AddPicture( 350, 60, 0x1b72 );			                                                // Bronze Shield Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 0.0 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 21 );	                                                    // Radio for Bronze Shield
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2237, socket.language ) );      // Heater Shield
    bsGump.AddPicture( 475, 60, 0x1b76 );			                                                // Heater Shield Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 24.3 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 22 );	                                                    // Radio for Heater Shield
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2238, socket.language ) );     // Metal Shield
    bsGump.AddPicture( 210, 130, 0x1b7b );															// Metal Shield Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 0.0 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 23 );	                                                    // Radio for Metal Shield
    //MetalKiteShield 0x1b74 24 GetDictionaryEntry( 2239, socket.language )

    bsGump.AddPage( 6 );																			// Bladed Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2240, socket.language ) );      // Broadsword
    bsGump.AddPicture( 190, 67, 0x0f61 );			                                                // Broadsword Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 35.4 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 26 );	                                                    // Radio for Broadsword
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2241, socket.language ) );      // Cutlass
    bsGump.AddPicture( 340, 60, 0x1441 );			                                                // Cutlass Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 8 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 24.3 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 27 );	                                                    // Radio for Cutlass
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2242, socket.language ) );      // Dagger
    bsGump.AddPicture( 480, 62, 0x0f51 );			                                                // Dagger Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 3 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 0.0 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 28 );	                                                    // Radio for Dagger
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2243, socket.language ) );     // Katana
    bsGump.AddPicture( 210, 130, 0x13ff );															// Katana Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 8 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 44.1 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 29 );	                                                    // Radio for Katana
    bsGump.AddHTMLGump( 327, 109, 149, 25, 0, 0, GetDictionaryEntry( 2244, socket.language ) );     // Kryss
    bsGump.AddPicture( 340, 130, 0x1401 );															// Kryss Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 8 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 36.7 <\center>" )
    bsGump.AddRadio( 327, 129, 0xd2, 0, 30 );	                                                    // Radio for Kryss
    bsGump.AddHTMLGump( 454, 109, 149, 25, 0, 0, GetDictionaryEntry( 2245, socket.language ) );     // Longsword
    bsGump.AddPicture( 450, 130, 0x0f61 );	 														// Longsword Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 28.0 <\center>" )
    bsGump.AddRadio( 456, 129, 0xd2, 0, 31 );	                                                    // Radio for Longsword
    bsGump.AddHTMLGump( 197, 178, 149, 25, 0, 0, GetDictionaryEntry( 2246, socket.language ) );     // Scimitar
    bsGump.AddPicture( 220, 200, 0x13b6 );															// Scimitar Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 31.7 <\center>" )
    bsGump.AddRadio( 197, 199, 0xd2, 0, 32 );	                                                    // Radio for Scimitar
    bsGump.AddHTMLGump( 327, 178, 149, 25, 0, 0, GetDictionaryEntry( 2247, socket.language ) );     // Viking Sword
    bsGump.AddPicture( 350, 200, 0x13b9 );															// Viking Sword Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 24.3 <\center>" )
    bsGump.AddRadio( 327, 199, 0xd2, 0, 33 );	                                                    // Radio for Viking Sword

    bsGump.AddPage( 7 );																			// Axes Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2248, socket.language ) );      // Axe
    bsGump.AddPicture( 220, 67, 0x0f49 );															// Axe Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 34.2 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 34 );														// Radio for Axe
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2249, socket.language ) );      // Battle Axe
    bsGump.AddPicture( 352, 67, 0x0f47 );															// BattleAxe Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 30.5 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 35 );														// Radio for Battle Axe
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2250, socket.language ) );      // Double Axe
    bsGump.AddPicture( 480, 67, 0x0f4b );															// Double Axe Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 29.3 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 36 );														// Radio for Double Axe
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2251, socket.language ) );     // Executioner's Axe
    bsGump.AddPicture( 220, 140, 0x0f45 );															// Executioners Axe Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 34.2 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 37 );														// Radio for Executioners Axe
    bsGump.AddHTMLGump( 327, 109, 149, 25, 0, 0, GetDictionaryEntry( 2252, socket.language ) );     // Large Battle Axe
    bsGump.AddPicture( 352, 140, 0x13fb );															// Large Battle Axe Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 28.0 <\center>" )
    bsGump.AddRadio( 327, 129, 0xd2, 0, 38 );														// Radio for Large Battle Axe
    bsGump.AddHTMLGump( 456, 109, 149, 25, 0, 0, GetDictionaryEntry( 2253, socket.language ) );     // Two-Handed Axe
    bsGump.AddPicture( 480, 140, 0x1443 );															// Two-Handed Axe Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 16 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 33.0 <\center>" )
    bsGump.AddRadio( 456, 129, 0xd2, 0, 39 );														// Radio for Two-Handed Axe
    bsGump.AddHTMLGump( 197, 178, 149, 25, 0, 0, GetDictionaryEntry( 2254, socket.language ) );     // War Axe
    bsGump.AddPicture( 220, 207, 0x13b0 );															// War Axe Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 16 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 39.1 <\center>" )
    bsGump.AddRadio( 197, 199, 0xd2, 0, 40 );														// Radio for War Axe

    bsGump.AddPage( 8 );																			// Pole Arms Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2255, socket.language ) );      // Bardiche
    bsGump.AddPicture( 220, 67, 0x0f4d );															// Bardiche Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 31.7 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 41 );														// Radio for Bardiche
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2256, socket.language ) );      // Halberd
    bsGump.AddPicture( 300, 64, 0x143f );															// Halberd Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 20 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 39.1 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 42 );														// Radio for Halberd
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2257, socket.language ) );      // Short Spear
    bsGump.AddPicture( 480, 67, 0x1403 );															// Short Spear Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 6 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 45.3 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 43 );														// Radio for Short Spear
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2258, socket.language ) );     // Spear
    bsGump.AddPicture( 160, 130, 0x0f62 );															// Spear Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 49.0 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 44 );														// Radio for Spear
    bsGump.AddHTMLGump( 327, 109, 149, 25, 0, 0, GetDictionaryEntry( 2259, socket.language ) );     // War Fork
    bsGump.AddPicture( 352, 140, 0x1405 );															// War Fork Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 42.9 <\center>" )
    bsGump.AddRadio( 327, 129, 0xd2, 0, 45 );														// Radio for War Fork

    bsGump.AddPage( 9 );																			// Bashing Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2260, socket.language ) );      // Hammer Pick
    bsGump.AddPicture( 220, 67, 0x143d );															// Hammer Pick Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 16 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 34.2 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 46 );														// Radio for Hammer Pick
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2261, socket.language ) );      // Mace
    bsGump.AddPicture( 352, 67, 0x0f5c );															// Mace Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 6 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 14.5 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 47 );														// Radio for Mace
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2262, socket.language ) );      // Maul
    bsGump.AddPicture( 480, 67, 0x143b );															// Maul Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 19.4 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 48 );														// Radio for Maul
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2263, socket.language ) );     // War Mace
    bsGump.AddPicture( 220, 140, 0x1407 );															// War Mace Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 28.0 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 49 );														// Radio for War Mace
    bsGump.AddHTMLGump( 327, 109, 149, 25, 0, 0, GetDictionaryEntry( 2264, socket.language ) );     // War Hammer
    bsGump.AddPicture( 352, 140, 0x1439 );															// War Hammer Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 16 Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 34.2 <\center>" )
    bsGump.AddRadio( 327, 129, 0xd2, 0, 50 );														// Radio for War Hammer

    bsGump.Send( pUser );
    bsGump.Free();
    return false;
}

function onGumpPress( pSock, pButton, gumpData )
{
    var pUser = pSock.currentChar;
    var gumpID = scriptID + 0xffff;
    var anvil = AreaItemFunction( "findNearbyItems", pUser, 2, pUser.socket );

    switch ( pButton )
    {
        case 0:
            pSock.CloseGump( gumpID, 0 );
            break;// abort and do nothing
        case 1:
            if ( anvil > 0 )
            {
                var OtherButton = gumpData.getButton( 0 );
                switch ( OtherButton )
                {
                    case 2:
                        MakeItem( pSock, pUser, 7 );
                        BlackSmithGump( pSock, pUser );
                        break;//Ringmail Gloves
                    case 3:
                        MakeItem( pSock, pUser, 9 );
                        BlackSmithGump( pSock, pUser );
                        break;//Ringmail Legs
                    case 4:
                        MakeItem( pSock, pUser, 8 );
                        BlackSmithGump( pSock, pUser );
                        break;//Ringmail Arms
                    case 5:
                        MakeItem( pSock, pUser, 10 );
                        BlackSmithGump( pSock, pUser );
                        break;//Ringmail Chest
                    case 6:
                        MakeItem( pSock, pUser, 11 );
                        BlackSmithGump( pSock, pUser );
                        break;//Chain Coif
                    case 7:
                        MakeItem( pSock, pUser, 12 );
                        BlackSmithGump( pSock, pUser );
                        break;//Chain Legs
                    case 8:
                        MakeItem( pSock, pUser, 13 );
                        BlackSmithGump( pSock, pUser );
                        break;//Chain Chest
                    case 9:
                        MakeItem( pSock, pUser, 16 );
                        BlackSmithGump( pSock, pUser );
                        break;//Plate Arms
                    case 10:
                        MakeItem( pSock, pUser, 15 );
                        BlackSmithGump( pSock, pUser );
                        break;//Plate Gloves
                    case 11:
                        MakeItem( pSock, pUser, 14 );
                        BlackSmithGump( pSock, pUser );
                        break;//Plate Gorget
                    case 12:
                        MakeItem( pSock, pUser, 17 );
                        BlackSmithGump( pSock, pUser );
                        break;//Plate Legs
                    case 13:
                        MakeItem( pSock, pUser, 18 );
                        BlackSmithGump( pSock, pUser );
                        break;//Plate Chest
                    case 14:
                        MakeItem( pSock, pUser, 19 );
                        BlackSmithGump( pSock, pUser );
                        break;//Female Plate Chest
                    case 15:
                        MakeItem( pSock, pUser, 46 );
                        BlackSmithGump( pSock, pUser );
                        break;//Bascinet
                    case 16:
                        MakeItem( pSock, pUser, 48 );
                        BlackSmithGump( pSock, pUser );
                        break;//Close Helm
                    case 17:
                        MakeItem( pSock, pUser, 45 );
                        BlackSmithGump( pSock, pUser );
                        break;//Helmet
                    case 18:
                        MakeItem( pSock, pUser, 47 );
                        BlackSmithGump( pSock, pUser );
                        break;//Norse Helm
                    case 19:
                        MakeItem( pSock, pUser, 49 );
                        BlackSmithGump( pSock, pUser );
                        break;//Plate Helm
                    case 20:
                        MakeItem( pSock, pUser, 367 );
                        BlackSmithGump( pSock, pUser );
                        break;//Buckler
                    case 21:
                        MakeItem( pSock, pUser, 2 );
                        BlackSmithGump( pSock, pUser );
                        break;//Bronze Shield
                    case 22:
                        MakeItem( pSock, pUser, 6 );
                        BlackSmithGump( pSock, pUser );
                        break;//Heater Shield
                    case 23:
                        MakeItem( pSock, pUser, 3 );
                        BlackSmithGump( pSock, pUser );
                        break;//Metal Shield
                    case 24:
                        MakeItem( pSock, pUser, 5 );
                        BlackSmithGump( pSock, pUser );
                        break;//MetalKiteShield
                    case 25: break;
                    case 26:
                        MakeItem( pSock, pUser, 25 );
                        BlackSmithGump( pSock, pUser );
                        break;//Broadsword
                    case 27:
                        MakeItem( pSock, pUser, 21 );
                        BlackSmithGump( pSock, pUser );
                        break;//Cutlass
                    case 28:
                        MakeItem( pSock, pUser, 20 );
                        BlackSmithGump( pSock, pUser );
                        break;//Dagger
                    case 29:
                        MakeItem( pSock, pUser, 22 );
                        BlackSmithGump( pSock, pUser );
                        break;//Katana
                    case 30:
                        MakeItem( pSock, pUser, 23 );
                        BlackSmithGump( pSock, pUser );
                        break;//Kryss
                    case 31:
                        MakeItem( pSock, pUser, 26 );
                        BlackSmithGump( pSock, pUser );
                        break;//Longsword
                    case 32:
                        MakeItem( pSock, pUser, 24 );
                        BlackSmithGump( pSock, pUser );
                        break;//Scimitar
                    case 33:
                        MakeItem( pSock, pUser, 27 );
                        BlackSmithGump( pSock, pUser );
                        break;//Viking Sword
                    case 34:
                        MakeItem( pSock, pUser, 29 );
                        BlackSmithGump( pSock, pUser );
                        break;//Axe
                    case 35:
                        MakeItem( pSock, pUser, 28 );
                        BlackSmithGump( pSock, pUser );
                        break;//Battle Axe
                    case 36:
                        MakeItem( pSock, pUser, 32 );
                        BlackSmithGump( pSock, pUser );
                        break;//Double Axe
                    case 37:
                        MakeItem( pSock, pUser, 30 );
                        BlackSmithGump( pSock, pUser );
                        break;//Executioners Axe
                    case 38:
                        MakeItem( pSock, pUser, 33 );
                        BlackSmithGump( pSock, pUser );
                        break;//Large Battle Axe
                    case 39:
                        MakeItem( pSock, pUser, 31 );
                        BlackSmithGump( pSock, pUser );
                        break;//Two Handed Axe
                    case 40:
                        MakeItem( pSock, pUser, 34 );
                        BlackSmithGump( pSock, pUser );
                        break;//War Axe
                    case 41:
                        MakeItem( pSock, pUser, 38 );
                        BlackSmithGump( pSock, pUser );
                        break;//Bardiche
                    case 42:
                        MakeItem( pSock, pUser, 39 );
                        BlackSmithGump( pSock, pUser );
                        break;//Halberd
                    case 43:
                        MakeItem( pSock, pUser, 35 );
                        BlackSmithGump( pSock, pUser );
                        break;//Short Spear
                    case 44:
                        MakeItem( pSock, pUser, 36 );
                        BlackSmithGump( pSock, pUser );
                        break;//Spear
                    case 45:
                        MakeItem( pSock, pUser, 37 );
                        BlackSmithGump( pSock, pUser );
                        break;//WarFork
                    case 46:
                        MakeItem( pSock, pUser, 44 );
                        BlackSmithGump( pSock, pUser );
                        break;//Hammer Pick
                    case 47:
                        MakeItem( pSock, pUser, 40 );
                        BlackSmithGump( pSock, pUser );
                        break;//Mace
                    case 48:
                        MakeItem( pSock, pUser, 41 );
                        BlackSmithGump( pSock, pUser );
                        break;//Maul
                    case 49:
                        MakeItem( pSock, pUser, 42 );
                        BlackSmithGump( pSock, pUser );
                        break;//War Mace
                    case 50:
                        MakeItem( pSock, pUser, 43 );
                        BlackSmithGump( pSock, pUser );
                        break;//War Hammer
                }
                break;
            }
            else
            {
                pSock.SysMessage( GetDictionaryEntry( 2213, pSock.Language ) ); // You must be near an anvil and a forge to smith items.
                BlackSmithGump( pSock, pUser );
            } break;
        case 2:
            pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4014, "BronzeSmithGump", pSock, pUser );
            break;
        case 3:
            pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4015, "CopperSmithGump", pSock, pUser );
            break;
        case 4:
            pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4016, "AgapiteSmithGump", pSock, pUser );
            break;
        case 5:
            pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4017, "DullCopperSmithGump", pSock, pUser );
            break;
        case 6:
            pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4018, "GoldSmithGump", pSock, pUser );
            break;
        case 7:
            pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4019, "ShadowSmithGump", pSock, pUser );
            break;
        case 8:
            pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4020, "ValoriteSmithGump", pSock, pUser );
            break;
        case 9:
            pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4021, "VeriteSmithGump", pSock, pUser );
            break;
        case 10:break;
        case 11:
            var targMsg = GetDictionaryEntry( 485, pSock.Language );
            pSock.CustomTarget( 2, targMsg );
            BlackSmithGump( pSock, pUser );
            break;
    }
}

function onCallback2( socket, ourObj )
{
    var mChar = socket.currentChar;
    var bItem = socket.tempObj;
    socket.tempObj = null;

    if ( mChar && mChar.isChar && bItem && bItem.isItem )
    {
        if ( !ourObj || !ourObj.isItem )
        {
            socket.SysMessage( GetDictionaryEntry( 986, socket.Language ) );
            return;
        }

        var minSkill = 999;
        var maxSkill = 1000;
        // This needs to use getWeaponType eventually
        var weaponID = ourObj.id;
        var armorID = ourObj.id;
        if ( weaponID == 0x0F43 || weaponID == 0x0F44 || weaponID == 0x0F45 || weaponID == 0x0F46 ||
            weaponID == 0x0F47 || weaponID == 0x0F48 || weaponID == 0x0F49 || weaponID == 0x0F4A ||
            weaponID == 0x0F4B || weaponID == 0x0F4C || weaponID == 0x0F4E || weaponID == 0x0F4E ||
            weaponID == 0x0F51 || weaponID == 0x0F52 || weaponID == 0x0F5C || weaponID == 0x0F5D ||
            weaponID == 0x0F5E || weaponID == 0x0F5F || weaponID == 0x0F60 || weaponID == 0x0F61 ||
            weaponID == 0x0F62 || weaponID == 0x0F63 || weaponID == 0x13AF || weaponID == 0x13B0 ||
            weaponID == 0x13B5 || weaponID == 0x13B6 || weaponID == 0x13B7 || weaponID == 0x13B8 ||
            weaponID == 0x13B9 || weaponID == 0x13BA || weaponID == 0x13F6 || weaponID == 0x13F7 ||
            weaponID == 0x13FA || weaponID == 0x13FB || weaponID == 0x13FC || weaponID == 0x13FD ||
            weaponID == 0x13FE || weaponID == 0x13FF || weaponID == 0x1400 || weaponID == 0x1401 ||
            weaponID == 0x1402 || weaponID == 0x1403 || weaponID == 0x1404 || weaponID == 0x1405 ||
            weaponID == 0x1406 || weaponID == 0x1407 || weaponID == 0x1438 || weaponID == 0x1439 ||
            weaponID == 0x143A || weaponID == 0x143B || weaponID == 0x143C || weaponID == 0x143D ||
            weaponID == 0x143E || weaponID == 0x143F || weaponID == 0x1440 || weaponID == 0x1441 ||
            weaponID == 0x1442 || weaponID == 0x1443 || armorID == 0x13BB || armorID == 0x13BC ||
            armorID == 0x13BD || armorID == 0x13BE || armorID == 0x13BF || armorID == 0x13C0 ||
            armorID == 0x13C1 || armorID == 0x13C2 || armorID == 0x13C3 || armorID == 0x13C4 ||
            armorID == 0x13E5 || armorID == 0x13E6 || armorID == 0x13E7 || armorID == 0x13E8 ||
            armorID == 0x13E9 || armorID == 0x13EA || armorID == 0x13EB || armorID == 0x13EC ||
            armorID == 0x13ED || armorID == 0x13EE || armorID == 0x13EF || armorID == 0x13F0 ||
            armorID == 0x13F1 || armorID == 0x13F2 || armorID == 0x1408 || armorID == 0x1409 ||
            armorID == 0x140A || armorID == 0x140B || armorID == 0x140C || armorID == 0x140D ||
            armorID == 0x140E || armorID == 0x140F || armorID == 0x1410 || armorID == 0x1411 ||
            armorID == 0x1412 || armorID == 0x1413 || armorID == 0x1414 || armorID == 0x1415 ||
            armorID == 0x1416 || armorID == 0x1417 || armorID == 0x1418 || armorID == 0x1419 ||
            armorID == 0x141A )	// Is it a armor or weapon?
        {
            var ownerObj = GetPackOwner( ourObj, 0 );
            if ( ownerObj && mChar.serial == ownerObj.serial )
            {
                if ( ourObj.health < ourObj.maxhp )
                {
                    if ( ourObj.hidamage > 0 )
                    {
                        var offset = ( ( ourObj.lodamage + ourObj.hidamage ) / 2 );
                        if ( offset <= 25 )
                        {
                            minSkill = ( 510 + ( ( offset - 1 ) / 5 ) * 100 );
                            maxSkill = ( 799 + ( ( offset - 1 ) / 5 ) * 50 );
                        }
                    }
                    else
                    {
                        socket.SysMessage( GetDictionaryEntry( 987, socket.Language ) );
                        return;
                    }

                    if ( mChar.CheckSkill( 8, minSkill, maxSkill ) )
                    {
                        ourObj.health = ourObj.maxhp;
                        socket.SysMessage( GetDictionaryEntry( 989, socket.Language ) );
                        socket.SoundEffect( 0x002A, true );
                    }
                    else
                        socket.SysMessage( GetDictionaryEntry( 990, socket.Language ) );
                }
                else
                    socket.SysMessage( GetDictionaryEntry( 988, socket.Language ) );
            }
            else
                socket.SysMessage( GetDictionaryEntry( 2265, socket.Language ) );//You must have that item in your pack to repair it!
        }
        else
            socket.SysMessage( GetDictionaryEntry( 986, socket.Language ) );
    }
}