const scriptID = 4017;

function DullCopperSmithGump( socket, pUser )
{
    if ( socket == null )
        return;

    var gumpID = scriptID + 0xffff;
    socket.CloseGump( gumpID, 0 );
	var bsGump = new Gump; // create a new gump 
    bsGump.AddPage( 0 );
    bsGump.AddBackground( 0, 0, 585, 393, 5054 );                             						// Tile Gray Background
    bsGump.AddBackground( 194, 36, 386, 275, 3000 );		                                        // Tile White Bacground
    bsGump.AddHTMLGump( 60, 339, 160, 25, 0, 0, GetDictionaryEntry( 2189, socket.language ) );      // OKAY
    bsGump.AddButton( 26, 335, 0xfa5, 1, 0, 1 );		                                            // Button - OKAY
    bsGump.AddHTMLGump( 60, 359, 160, 25, 0, 0, GetDictionaryEntry( 2090, socket.language ) );      // CANCEL
    bsGump.AddButton( 26, 356, 0xfa5, 1, 0, 0 );                                                    // Button - CANCEL
    bsGump.AddHTMLGump(60, 318, 160, 25, 0, 0, GetDictionaryEntry( 2267, socket.language ) );		// BACK
    bsGump.AddButton(26, 315, 0xfa5, 1, 0, 2);													    // Button - BACK
    bsGump.AddHTMLGump( 178, 4, 160, 25, 0, 0, GetDictionaryEntry( 2270, socket.language ) );       // Dull Copper Blacksmithy Selection Menu
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

    bsGump.AddPage( 1 );																			// Ringmail Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2217, socket.language ) );      // Ringmail Gloves
    bsGump.AddPicture( 210, 62, 0x13f2 );			                                                // Ringmail Gloves Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + "10 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 12.0 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 2 );															// Radio for Ringmail Gloves
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2218, socket.language ) );      // Ringmail Legs
    bsGump.AddPicture( 338, 64, 0x13f0 );			                                                // Ringmail Legs Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + "16 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 19.4 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 3 );															// Radio for Ringmail Legs
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2219, socket.language ) );      // Ringmail Arms
    bsGump.AddPicture( 480, 60, 0x13ee );			                                                // Ringmail Arms Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 16.9 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 4 );															// Radio for Ringmail Arms
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2220, socket.language ) );     // Ringmail Chest
    bsGump.AddPicture( 215, 126, 0x13ec );															// Ringmail Chest Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 21.9 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 5 );	                                                    // Radio for Ringmail Chest

    bsGump.AddPage( 2 );																			// Chainmail Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2221, socket.language ) );      // Chain Coif
    bsGump.AddPicture( 210, 62, 0x13bb );			                                                // Chain Coif Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 14.5 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 6 );															// Radio for Chain Coif
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2222, socket.language ) );      // Chain Legs
    bsGump.AddPicture( 348, 62, 0x13be );			                                                // Chain Legs Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 36.7 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 7 );															// Radio for Chain Legs
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2223, socket.language ) );      // Chain Chest
    bsGump.AddPicture( 470, 62, 0x13bf );			                                                // Chain Chest Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 20 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 39.1 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 8 );															// Radio for Chain Chest

    bsGump.AddPage( 3 );																			// Platemail Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2224, socket.language ) );      // Plate Arms
    bsGump.AddPicture( 213, 62, 0x1410 );			                                                // PlateArms Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 66.3 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 9 );															// Radio for PlateArms
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2225, socket.language ) );      // Plate Gloves
    bsGump.AddPicture( 342, 67, 0x1414 );			                                                // PlateGloves Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 58.9 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 10 );	                                                    // Radio for PlateGloves
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2226, socket.language ) );      // Plate Gorget
    bsGump.AddPicture( 476, 67, 0x1413 );			                                                // PlateGorget Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 56.4 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 11 );	                                                    // Radio for PlateGorget
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2227, socket.language ) );     // Plate Legs
    bsGump.AddPicture( 216, 130, 0x1411 );															// Plate Legs Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 20 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 68.8 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 12 );	                                                    // Radio for Plate Legs
    bsGump.AddHTMLGump( 327, 109, 149, 25, 0, 0, GetDictionaryEntry( 2228, socket.language ) );     // Plate Chest
    bsGump.AddPicture( 347, 130, 0x1415 );															// Plate Chest Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 25 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 75.0 <\center>" )
    bsGump.AddRadio( 327, 129, 0xd2, 0, 13 );	                                                    // Radio for Plate Chest
    bsGump.AddHTMLGump( 456, 109, 149, 25, 0, 0, GetDictionaryEntry( 2229, socket.language ) );     // Female Plate Chest
    bsGump.AddPicture( 474, 128, 0x1c04 );	 														// Female Plate Chest Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 20 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 44.1 <\center>" )
    bsGump.AddRadio( 456, 129, 0xd2, 0, 14 );	                                                    // Radio for FemalePlateChest

    bsGump.AddPage( 4 );																			// Helmets Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2230, socket.language ) );      // Bascinet
    bsGump.AddPicture( 217, 62, 0x140c );			                                                // Bascinet Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 8.3 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 15 );	                                                    // Radio for Bascinet
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2231, socket.language ) );      // Close Helm
    bsGump.AddPicture( 345, 62, 0x1408 );			                                                // CloseHelm Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 37.9 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 16 );	                                                    // Radio for CloseHelm
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2232, socket.language ) );      // Helmet
    bsGump.AddPicture( 470, 62, 0x140a );			                                                // Helmet Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 37.9 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 17 );	                                                    // Radio for Helmet
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2233, socket.language ) );     // Norse Helm
    bsGump.AddPicture( 220, 130, 0x140e );															// Norse Helm Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 37.9 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 18 );														// Radio for Norse Helm
    bsGump.AddHTMLGump( 327, 109, 149, 25, 0, 0, GetDictionaryEntry( 2234, socket.language ) );     // Plate Helm
    bsGump.AddPicture( 353, 130, 0x1419 );															// Plate Helm Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 15 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 62.6 <\center>" )
    bsGump.AddRadio( 327, 129, 0xd2, 0, 19 );	                                                    // Radio for Plate Helm

    bsGump.AddPage( 5 );																			// Shields Menu
    bsGump.AddHTMLGump( 197, 41, 149, 25, 0, 0, GetDictionaryEntry( 2235, socket.language ) );      // Buckler
    bsGump.AddPicture( 210, 64, 0x1b73 );			                                                // Buckler Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 10 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 0.0 <\center>" )
    bsGump.AddRadio( 197, 62, 0xd2, 0, 20 );	                                                    // Radio for Buckler
    bsGump.AddHTMLGump( 327, 41, 149, 25, 0, 0, GetDictionaryEntry( 2236, socket.language ) );      // Bronze Shield
    bsGump.AddPicture( 350, 60, 0x1b72 );			                                                // Bronze Shield Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 12 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 0.0 <\center>" )
    bsGump.AddRadio( 327, 62, 0xd2, 0, 21 );	                                                    // Radio for Bronze Shield
    bsGump.AddHTMLGump( 456, 41, 149, 25, 0, 0, GetDictionaryEntry( 2237, socket.language ) );      // Heater Shield
    bsGump.AddPicture( 475, 60, 0x1b76 );			                                                // Heater Shield Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 18 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 24.3 <\center>" )
    bsGump.AddRadio( 456, 62, 0xd2, 0, 22 );	                                                    // Radio for Heater Shield
    bsGump.AddHTMLGump( 197, 109, 149, 25, 0, 0, GetDictionaryEntry( 2238, socket.language ) );     // Metal Shield
    bsGump.AddPicture( 210, 130, 0x1b7b );															// Metal Shield Image
    bsGump.AddToolTip( 1114778, socket, "<center> <basefont color=#FFFF00>" + GetDictionaryEntry( 2214, socket.language ) + "</basefont> \n" + GetDictionaryEntry( 2215, socket.language ) + " 14 Dull Copper Ingots \n" + GetDictionaryEntry( 2216, socket.language ) + " 0.0 <\center>" )
    bsGump.AddRadio( 197, 129, 0xd2, 0, 23 );	                                                    // Radio for Metal Shield
    //MetalKiteShield 0x1b74 24 GetDictionaryEntry( 2239, socket.language )
    bsGump.Send( pUser );
    bsGump.Free();
    return false;
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
                        MakeItem( pSock, pUser, 506 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Ringmail Gloves
					case 3:
                        MakeItem( pSock, pUser, 508 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Ringmail Legs
					case 4:
                        MakeItem( pSock, pUser, 507 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Ringmail Arms
					case 5:
                        MakeItem( pSock, pUser, 509 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Ringmail Chest
					case 6:
                        MakeItem( pSock, pUser, 510 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Chain Coif
					case 7:
                        MakeItem( pSock, pUser, 511 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Chain Legs
					case 8:
                        MakeItem( pSock, pUser, 512 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Chain Chest
					case 9:
                        MakeItem( pSock, pUser, 515 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Plate Arms
					case 10:
                        MakeItem( pSock, pUser, 514 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Plate Gloves
					case 11:
                        MakeItem( pSock, pUser, 513 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Plate Gorget
					case 12:
                        MakeItem( pSock, pUser, 516 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Plate Legs
					case 13:
                        MakeItem( pSock, pUser, 517 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Plate Chest
					case 14:
                        MakeItem( pSock, pUser, 518 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Female Plate Chest
					case 15:
                        MakeItem( pSock, pUser, 520 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Bascinet
					case 16:
                        MakeItem( pSock, pUser, 522 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Close Helm
					case 17:
                        MakeItem( pSock, pUser, 519 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Helmet
					case 18:
                        MakeItem( pSock, pUser, 521 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Norse Helm
					case 19:
                        MakeItem( pSock, pUser, 523 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Plate Helm
					case 20:
                        MakeItem( pSock, pUser, 500 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Buckler
					case 21:
                        MakeItem( pSock, pUser, 501 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Bronze Shield
					case 22:
                        MakeItem( pSock, pUser, 505 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Heater Shield
					case 23:
                        MakeItem( pSock, pUser, 502 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//Metal Shield
					case 24:
                        MakeItem( pSock, pUser, 504 );
                        DullCopperSmithGump( pSock, pUser );
              			break;//MetalKiteShield
                }
                break;
            }
            else
            {
                pSock.SysMessage( GetDictionaryEntry( 2213, pSock.Language ) ); // You must be near an anvil and a forge to smith items.
                DullCopperSmithGump( pSock, pUser );
            } break;
        case 2: pSock.CloseGump( gumpID, 0 );
            TriggerEvent( 4013, "BlackSmithGump", pSock, pUser );//Return to Blacksmith Gump
            break;
    }
}