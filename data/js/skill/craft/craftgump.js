const textHue = 0x480;				// Color of the text.
const LabelColor = 0x7FFF;			// Second Color of text.
const scriptID = 4025;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
const resourceDelay = 200;			// Timer for the gump to reapear after selecting a resource.
const blacksmithID = 4023;
const Carpentry = 4025;
const Alchemy = 4028;
const Bowcraft = 4029;
const Tailoring = 4030;
const Tinkering = 4032;
const Cooking = 4034;

 // If enabled, players can craft coloured variants of weapons using Blacksmithing skill, though
 // unless the craftItems array in blacksmithing.js is updated with specific create entries for the
 // coloured weapon variants, they'll just be regular weapons with ore colour applied
const allowColouredWeapons = GetServerSetting( "CraftColouredWeapons" );

function CraftingGumpMenu( myGump, socket )
{
	var pUser = socket.currentChar;

	// Get player's resource counts
	var iron = pUser.ResourceCount( 0x1BF2 );
	var dullcopper = pUser.ResourceCount( 0x1BF2, 0x0973 );
	var shadowiron = pUser.ResourceCount( 0x1BF2, 0x0966 );
	var copper = pUser.ResourceCount( 0x1BF2, 0x07dd );
	var bronze = pUser.ResourceCount( 0x1BF2, 0x06d6 );
	var gold = pUser.ResourceCount( 0x1BF2, 0x08a5 );
	var agapite = pUser.ResourceCount( 0x1BF2, 0x0979 );
	var verite = pUser.ResourceCount( 0x1BF2, 0x089f );
	var valorite = pUser.ResourceCount( 0x1BF2, 0x08ab );
	var logs = pUser.ResourceCount( 0x1BE0 );
	var boards = pUser.ResourceCount( 0x1bd7 );
	var leather = pUser.ResourceCount( 0x1067 );
	var leather1 = pUser.ResourceCount( 0x1068 );
	var leather2 = pUser.ResourceCount( 0x1081 );
	var leather3 = pUser.ResourceCount( 0x1082 );
	var hides = pUser.ResourceCount( 0x1078 );
	var hides1 = pUser.ResourceCount( 0x1079 );

	var resourcename = 10291;
	var resource = iron;
	var groupList;
	var gumpMenuName = "";
	var repair = 51;
	var craftingSkillUsed = pUser.GetTempTag( "CRAFT" );

	switch( craftingSkillUsed )
	{
		case 1: // Carpentry
			grouplist = [10601, 10602, 10603, 10604, 10605, 10606, 10607, 10608, 10609, 10610]; //CATEGORIES
			resourcename = 10687;
			resource = ( logs + boards );
			gumpMenuName = 10600;//Carpentry
			repair = 51;
			break;
		case 2: // Alchemy
			grouplist = [10902, 10903, 10904, 10905]; //CATEGORIES
			gumpMenuName = 10901;//Alchemy Menu
			break;
		case 3: // Bowcraft/Fletching
			grouplist = [11202, 11203, 11204]; //CATEGORIES
			gumpMenuName = 11201;//Bowcraft Menu
			resourcename = 10687;
			resource = ( logs + boards );
			//repair = 51;
			break;
		case 4: // Tailoring
			grouplist = [11404, 11405, 11406, 11407, 11408, 11410, 11411, 11412]; //CATEGORIES
			gumpMenuName = 11401;//Tailoring Menu
			resourcename = 11402;
			resource = ( leather + leather1 + leather2 + leather3 + hides + hides1 );
			//repair = 51;
			break;
		case 5: // Blacksmithing
			grouplist = [10279, 10280, 10281, 10282, 10283, 10284, 10285] //CATEGORIES
			gumpMenuName = 10188;//Blacksmithing Menu
			switch( pUser.GetTempTag( "ORE" ))
			{
				case 0: // Iron
					resourcename = 10291;
					resource = iron;
					break;
				case 1: // Dull Copper
					resourcename = 10203;
					resource = dullcopper;
					break;
				case 2: // Shadow Iron
					resourcename = 10204;
					resource = shadowiron;
					break;
				case 3: // Copper
					resourcename = 10205;
					resource = copper;
					break;
				case 4: // Bronze
					resourcename = 10206;
					resource = bronze;
					break;
				case 5: // Gold
					resourcename = 10207;
					resource = gold;
					break;
				case 6: // Agapite
					resourcename = 10208;
					resource = agapite;
					break;
				case 7: // Verite
					resourcename = 10209;
					resource = verite;
					break;
				case 8: // Valorite
					resourcename = 10210;
					resource = valorite;
					break;
				default: // Iron
					resourcename = 10291;
					resource = iron;
					break;
			}
			repair = 49;
			break;
		case 6: // Cooking
			grouplist = [11602, 11603, 11604, 11605]; //CATEGORIES
			gumpMenuName = 11601;//Cooking Menu
			break;
		case 7: // Tinkering
			grouplist = [11991, 11992, 11993, 11994, 11995, 11996, 11997, 11998, 11999]; // CATEGORIES
			gumpMenuName = 11990; // Tinkering Menu
			break;
	}

	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 530, 437, 5054 );
	myGump.AddTiledGump( 10, 10, 510, 22, 2624 );
	myGump.AddTiledGump( 10, 292, 150, 45, 2624 );
	myGump.AddTiledGump( 165, 292, 355, 45, 2624 );
	myGump.AddTiledGump( 10, 342, 510, 85, 2624 );
	myGump.AddTiledGump( 10, 37, 200, 250, 2624 );
	myGump.AddTiledGump( 215, 37, 305, 250, 2624 );
	myGump.AddCheckerTrans( 10, 10, 510, 417 );

	myGump.AddHTMLGump( 0, 12, 530, 20, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( gumpMenuName, socket.language ) + "</basefont> </center>" ); // MENU NAME
	myGump.AddHTMLGump( 0, 37, 220, 22, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10286, socket.language ) + "</basefont> </center>" ); // CATEGORIES
	myGump.AddHTMLGump( 230, 37, 280, 22, 0, 0, "<center><basefont color=#ffffff>" + GetDictionaryEntry( 10287, socket.language ) + "</basefont></center>" ); // SELECTIONS

	myGump.AddHTMLGump( 10, 302, 150, 25, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10288, socket.language ) + "</basefont> </center>" ); // NOTICES

	// Fetch result of previous crafting action, and null out temp tag
	var previousActionResult = pUser.GetTempTag( "prevActionResult" );
	pUser.SetTempTag( "prevActionResult", null );

	switch( previousActionResult )
	{
		case "FAILED":
			myGump.AddHTMLGump(170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10292, socket.language ) + "</basefont>"); // You have no idea how to work this metal.
			socket.SysMessage( GetDictionaryEntry( 10292, socket.language ));
			break;
		case "NOANVIL":
			myGump.AddHTMLGump(170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10213, socket.language ) + "</basefont>"); // You must be near an anvil to smith items.
			socket.SysMessage( GetDictionaryEntry( 10213, socket.language ));
			break;
		case "NOFORGEORANVIL":
			myGump.AddHTMLGump(170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10201, socket.language ) + "</basefont>"); // You must be near an anvil and a forge to smelt items.
			socket.SysMessage( GetDictionaryEntry( 10201, socket.language ));
			break;
		case "CANTREPAIR":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 987, socket.language ) + "</basefont>" ); // You cannot repair this item.
			socket.SysMessage( GetDictionaryEntry( 987, socket.language ));
			break;
		case "REPAIRSUCCESS":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 989, socket.language ) + "</basefont>" ); // You repair the item successfully.
			socket.SysMessage( GetDictionaryEntry( 989, socket.language ));
			break;
		case "FAILREPAIR":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 990, socket.language ) + "</basefont>" ); // You fail to repair the item.
			socket.SysMessage( GetDictionaryEntry( 990, socket.language ));
			break;
		case "FULLREPAIR":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 988, socket.language ) + "</basefont>" ); // That item is already fully repaired.
			socket.SysMessage( GetDictionaryEntry( 988, socket.language ));
			break;
		case "CHECKPACK":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10265, socket.language ) + "</basefont>" ); // You must have that item in your pack to repair it!
			socket.SysMessage( GetDictionaryEntry( 10265, socket.language ));
			break;
		case "CANTSMELT":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 1114, socket.language ) + "</basefont>" ); // You have no knowledge on how to melt that.
			socket.SysMessage( GetDictionaryEntry( 1114, socket.language ));
			break;
		case "NOSMELTSKILL":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 1115, socket.language ) + "</basefont>" ); // You aren't skilled enough to melt this.
			socket.SysMessage( GetDictionaryEntry( 1115, socket.language ));
			break;
		case "SMELTITEMSUCCESS":
		{
			var ingotsAdded = pUser.GetTempTag( "ingotsFromSmelting" );
			pUser.SetTempTag( "ingotsFromSmelting", null );
			var tmpText = GetDictionaryEntry( 1116, socket.language ); // You melt the item and place %i ingots in your pack.
			tmpText = tmpText.replace(/%i/gi, ingotsAdded.toString() );
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + tmpText + "</basefont>" );
			socket.SysMessage( tmpText );
			break;
		}
		case "CANTUNRAVEL":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10296, socket.language ) + "</basefont>" ); // You have no knowledge on how to unravel that.
			socket.SysMessage( GetDictionaryEntry( 10296, socket.language ));
			break;
		case "NOUNRAVELSKILL":
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 20298, socket.language ) + "</basefont>" ); // You aren't skilled enough to unravel this.
			socket.SysMessage( GetDictionaryEntry( 20298, socket.language ));
			break;
		case "UNRAVELSUCCESS":
		{
			var resourcesAdded = pUser.GetTempTag( "resourceFromUnravelling" );
			pUser.SetTempTag( "resourceFromUnravelling", null );
			var tmpText = GetDictionaryEntry( 10297, socket.language ); // You unravel the item and place 5 resources in your pack.
			tmpText = tmpText.replace(/%i/gi, resourcesAdded.toString() );
			myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + tmpText + "</basefont>" );
			socket.SysMessage( tmpText );
			break;
		}
		case "NOHEATSOURCE":
			myGump.AddHTMLGump(170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 11654, socket.language ) + "</basefont>"); // You must be near a heat source to cook food.
			socket.SysMessage( GetDictionaryEntry( 11654, socket.language ));
			break;
		case "NOMILL":
			myGump.AddHTMLGump(170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 11655, socket.language ) + "</basefont>"); // You must be near a mill to create flour.
			socket.SysMessage( GetDictionaryEntry( 11655, socket.language ));
			break;
		case "NOOVEN":
			myGump.AddHTMLGump(170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 11656, socket.language ) + "</basefont>"); // You must be near an oven to bake food.
			socket.SysMessage( GetDictionaryEntry( 11656, socket.language ));
			break;
		default:
			break;
	}

	if( craftingSkillUsed != 2 && craftingSkillUsed != 6 )
	{
		myGump.AddText( 50, 362, textHue, GetDictionaryEntry( resourcename, socket.language ) + " (" + resource.toString() + ")" );

		// No material selection for these crafting skills yet
		if( craftingSkillUsed != 1 && craftingSkillUsed != 3 && craftingSkillUsed != 4 && craftingSkillUsed != 7 )
		{
			myGump.AddButton(15, 362, 4005, 4007, 1, 0, 50); // Material Selection Button
		}

		if( craftingSkillUsed == 5 )
		{
			// Blacksmithing
			myGump.AddButton(270, 342, 0xfa5, 1, 0, repair); // Repair Button
			myGump.AddHTMLGump(305, 345, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10212, socket.language ) + "</basefont>" );// REPAIR ITEM
			myGump.AddButton( 15, 342, 0xfa5, 1, 0, 52 ); // Smelt Item Button
			myGump.AddHTMLGump( 50, 345, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10289, socket.language ) + "</basefont>" );// SMELT ITEM
		}
		else if( craftingSkillUsed == 4 )
		{
			// Tailoring
			myGump.AddButton( 15, 342, 0xfa5, 1, 0, 52 ); // Unravel Item Button
			myGump.AddHTMLGump( 50, 345, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10294, socket.language ) + "</basefont>" );// UNRAVEL ITEM
		}
	}

	myGump.AddButton( 15, 402, 0x0fb1, 1, 0, 0 ); // Exit Button
	myGump.AddHTMLGump( 50, 405, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10290, socket.language ) + "</basefont>" );// EXIT

	myGump.AddButton( 270, 402, 0xfab, 1, 0, 5000 ); // Make Last Button
	myGump.AddHTMLGump( 305, 405, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10277, socket.language ) + "</basefont>" );// MAKE LAST

	for ( var i = 0; i < grouplist.length; i++ )
	{
		// Don't show weapon categories for blacksmithing menu if player has coloured ingots selected
		if( !allowColouredWeapons && craftingSkillUsed == 5 && pUser.GetTempTag( "resourceHue" ) > 0 && i > 2 )
			continue;

		//myGump.AddButton( 15, 60, 0xfa5, 0, 4, 0 );
		//myGump.AddXMFHTMLGumpColor( 50, 63, 150, 18, 1044014, false, false, LabelColor ); // LAST TEN
		myGump.AddButton( 15, 80 + ( i * 20 ), 4005, 4007, 1, 0, 1 + i );
		myGump.AddText( 50, 80 + ( i * 20 ), textHue, GetDictionaryEntry( grouplist[i], socket.language ));
	}
	return;
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	var bItem = pSock.tempObj;
	var gumpID = scriptID + 0xffff;
	switch( pButton )
	{
		case 0:
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;// abort and do nothing
		case 1:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 1 );
					break;
				case 2:
					TriggerEvent( Alchemy, "PageX", pSock, pUser, 1 );
					break;
				case 3:
					TriggerEvent( Bowcraft, "PageX", pSock, pUser, 1 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 1 );
					break;
				case 5:
					if( pUser.GetTempTag( "ORE" ) >= 0 && pUser.GetTempTag( "ORE" ) <= 8 )
					{
						TriggerEvent( blacksmithID, "PageX", pSock, pUser, 1 );
					}
					break;
				case 6:
					TriggerEvent( Cooking, "PageX", pSock, pUser, 1 );
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 1 );
					break;
				default:
					break;
			}
		case 2:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 2 );
					break;
				case 2:
					TriggerEvent( Alchemy, "PageX", pSock, pUser, 2 );
					break;
				case 3:
					TriggerEvent( Bowcraft, "PageX", pSock, pUser, 2 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 2 );
					break;
				case 5:
					if( pUser.GetTempTag( "ORE" ) >= 0 && pUser.GetTempTag( "ORE" ) <= 8 )
					{
						TriggerEvent( blacksmithID, "PageX", pSock, pUser, 2 );
					}
					break;
				case 6:
					TriggerEvent( Cooking, "PageX", pSock, pUser, 2 );
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 2 );
					break;
				default:
					break;
			}
		case 3:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 3 );
					break;
				case 2:
					TriggerEvent( Alchemy, "PageX", pSock, pUser, 3 );
					break;
				case 3:
					TriggerEvent( Bowcraft, "PageX", pSock, pUser, 3 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 3 );
					break;
				case 5:
					if( pUser.GetTempTag( "ORE" ) >= 0 && pUser.GetTempTag( "ORE" ) <= 8 )
					{
						TriggerEvent( blacksmithID, "PageX", pSock, pUser, 3 );
					}
					break;
				case 6:
					TriggerEvent( Cooking, "PageX", pSock, pUser, 3 );
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 3 );
					break;
				default:
					break;
			}
		case 4:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 4 );
					break;
				case 2:
					TriggerEvent( Alchemy, "PageX", pSock, pUser, 4 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 4 );
					break;
				case 5:
					if( pUser.GetTempTag( "ORE" ) >= 0 && pUser.GetTempTag( "ORE" ) <= 8 )
					{
						TriggerEvent( blacksmithID, "PageX", pSock, pUser, 4 );
					}
					break;
				case 6:
					TriggerEvent( Cooking, "PageX", pSock, pUser, 4 );
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 4 );
					break;
				default:
					break;
			}
		case 5:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 5 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 5 );
					break;
				case 5:
					if( pUser.GetTempTag( "ORE" ) >= 0 && pUser.GetTempTag( "ORE" ) <= 8 )
					{
						TriggerEvent( blacksmithID, "PageX", pSock, pUser, 5 );
					}
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 5 );
					break;
				default:
					break;
			}
		case 6:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 6 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 6 );
					break;
				case 5:
					if( pUser.GetTempTag( "ORE" ) >= 0 && pUser.GetTempTag( "ORE" ) <= 8 )
					{
						TriggerEvent( blacksmithID, "PageX", pSock, pUser, 6 );
					}
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 6 );
					break;
				default:
					break;
			}
		case 7:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 7 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 7 );
					break;
				case 5:
					if( pUser.GetTempTag( "ORE" ) >= 0 && pUser.GetTempTag( "ORE" ) <= 8 )
					{
						TriggerEvent( blacksmithID, "PageX", pSock, pUser, 7 );
					}
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 7 );
					break;
				default:
					break;
			}
		case 8:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 8 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 8 );
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 8 );
					break;
			}
			break;
		case 9:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 9 );
					break;
				case 4:
					TriggerEvent( Tailoring, "PageX", pSock, pUser, 9 );
					break;
				case 7:
					TriggerEvent( Tinkering, "PageX", pSock, pUser, 9 );
					break;
			}
			break;
		case 10:
			pSock.CloseGump( gumpID, 0 );
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					TriggerEvent( Carpentry, "PageX", pSock, pUser, 10 );
					break;
			}
			break;
		case 49: // Repair Item - Blacksmithing
			TriggerEvent( blacksmithID, "RepairTarget", pSock );
			break;
		case 51: // Repair Item - Carpentry
			TriggerEvent( Carpentry, "RepairTarget", pSock );
			break;
		case 52: // Smelt Item
			break;
	}
}