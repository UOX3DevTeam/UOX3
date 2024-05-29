const disguiseDuration = 1000 * 60 * 120; // 2 hours

const humanMaleHair = [
	// itemID, gumpID, xOffset, yOffset, name
	[0x2045, 1871, -20, -15, 2080], // Pageboy
	[0x203B, 1878, -20, -15, 2084], // Short
	[0x203C, 1876, -20, -15, 2085], // Long
	[0x2048, 1880, -20, -15, 2081], // Receding
	[0x203D, 1879, -20, -15, 2086], // Ponytail
	[0x2049, 1870, -20, -15, 2082], // 2-tails
	[0x2044, 1877, -20, -5, 2087], // Mohawk
	[0x204A, 1874, -20, -15, 2083], // Topknot
	[0x2047, 1873, -20, -15, 2088], // Curly
	[0, 0, 0, 0, 2200] // None/Bald
];
const humanMaleBeard = [
	// itemID, gumpID, xOffset, yOffset, name
	[0x204D, 50906, 0, 0, 18000], // Vandyke
	[0x2041, 50808, 0, -2, 18001], // Mustache
	[0x203F, 50802, 0, 0, 18002], // Short Beard
	[0x204C, 50905, 0, -10, 18003], // Long Beard
	[0x204B, 50904, 0, 0, 18004], // Short Beard
	[0x203E, 50801, 0, -10, 18005], // Long Beard
	[0, 0, 0, 0, 2200] // None
];
const humanFemaleHair = [
	// itemID, gumpID, xOffset, yOffset, name
	[0x2045, 1844, -20, -15, 2080], // Pageboy
	[0x203B, 1847, -20, -15, 2084], // Short
	[0x203C, 1842, -20, -15, 2085], // Long
	[0x2048, 1846, -20, -15, 2081], // Receding
	[0x203D, 1845, -20, -15, 2086], // Ponytail
	[0x2049, 1836, -20, -15, 2082], // 2-tails
	[0x2044, 1843, -20, -15, 2087], // Mohawk
	[0x204A, 1840, -20, -15, 2083], // Topknot
	[0x2046, 1841, -20, -15, 2089], // Buns
	[0x2047, 1839, -20, -15, 2088], // Curly
	[0, 0, 0, 0, 2200] // None/Bald
];
const elfMaleHair = [
	// itemID, gumpID, xOffset, yOffset, name
	[0x2FBF, 1774, -20, -15, 18027], // Mid Long
	[0x2FC0, 1785, -20, -15, 18028], // Long Feather
	[0x2FC1, 1786, -20, -15, 18029], // Short
	[0x2FC2, 1787, -20, -15, 18030], // Mullet
	[0x2FCD, 1789, -20, -15, 18031], // Long
	[0x2FCE, 1790, -20, -15, 18032], // Topknot
	[0x2FCF, 1791, -20, -5, 18033], // Long Braid
	[0x2FD1, 1793, -20, -15, 1803], // Spiked
	[0, 0, 0, 0, 2200] // None/Bald
];
const elfFemaleHair = [
	// itemID, gumpID, xOffset, yOffset, name
	[0x2FC0, 1775, -20, -15, 18028], // Long Feather
	[0x2FC1, 1776, -20, -15, 18029], // Short
	[0x2FC2, 1777, -20, -15, 18030], // Mullet
	[0x2FCC, 1778, -20, -15, 18035], // Flower
	[0x2FCE, 1780, -20, -15, 18032], // Topknot
	[0x2FCF, 1781, -20, -5, 18033], // Long Braid
	[0x2FD0, 1782, -20, -5, 18036], // Buns
	[0x2FD1, 1783, -20, -15, 18034], // Spiked
	[0, 0, 0, 0, 2200] // None/Bald
];
const gargoyleMaleHorns = [
	// itemID, gumpID, xOffset, yOffset, name
	[0x4258, 1900, -20, -15, 18019], // Horn Style 1
	[0x4259, 1901, -20, -15, 18020], // Horn Style 2
	[0x425A, 1907, -20, -15, 18021], // Horn Style 3
	[0x425B, 1902, -20, -15, 18022], // Horn Style 4
	[0x425C, 1908, -20, -15, 18023], // Horn Style 5
	[0x425D, 1909, -20, -15, 18024], // Horn Style 6
	[0x425E, 1910, -20, -15, 18025], // Horn Style 7
	[0x425F, 1911, -20, -15, 18026], // Horn Style 8
	[0, 0, 0, 0, 2200] // None/Bald
];
const gargoyleMaleFacialHorns = [
	// itemID, gumpID, xOffset, yOffset, name
	[0x42AD, 1903, -20, -15, 18019], // Horn Style 1
	[0x42AE, 1904, -20, -15, 18020], // Horn Style 2
	[0x42AF, 1905, -20, -15, 18021], // Horn Style 3
	[0x42B0, 1906, -20, -15, 18022], // Horn Style 4
	[0, 0, 0, 0, 2200] // None/Bald
];
const gargoyleFemaleHorns = [
	// itemID, gumpID, xOffset, yOffset, name
	[0x4261, 1952, -20, -15, 18019], // Horn Style 1
	[0x4262, 1953, -20, -15, 18020], // Horn Style 2
	[0x4273, 1950, -20, -15, 18021], // Horn Style 3
	[0x4274, 1954, -20, -15, 18022], // Horn Style 4
	[0x4275, 1951, -20, -15, 18023], // Horn Style 5
	[0x4276, 1916, -20, -15, 18024], // Horn Style 6
	[0x42AA, 1917, -20, -15, 18025], // Horn Style 7
	[0x42AB, 1918, -20, -15, 18026], // Horn Style 8
	[0, 0, 0, 0, 2200] // None/Bald
];

function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner != pUser )
	{
		pSock.SysMessage( GetDictionaryEntry( 1763, pSock.language )); // That item must be in your backpack before it can be used.
	}
	else if( pUser.npcGuild != 3 ) // Only members of the Thieves Guild can use this
	{
		pSock.SysMessage( GetDictionaryEntry( 18010, pSock.language )); // Only Members of the thieves guild are trained to use this item.
	}
	else if( pUser.murdercount > 0 )
	{
		pSock.SysMessage( GetDictionaryEntry( 18011, pSock.language )); // You are currently suspended from the thieves guild.  They would frown upon your actions.
	}
	else if( pUser.isIncognito )
	{
		pSock.SysMessage( GetDictionaryEntry( 1677, pSock.language )); // You cannot disguise yourself while incognito.
	}
	else if( pUser.isPolymorphed )
	{
		pSock.SysMessage( GetDictionaryEntry( 1678, pSock.language )); // You cannot disguise yourself while polymorphed.
	}
	else if( pUser.id == 0x00b8 || pUser.id == 0x00b7 )
	{
		pSock.SysMessage( GetDictionaryEntry( 1670, pSock.language )); // You cannot disguise yourself while wearing body paint
	}
	else
	{
		var helmLayerItem = pUser.FindItemLayer( 0x06 );
		if( ValidateObject( helmLayerItem ) && helmLayerItem.sectionID == "orcishkinmask" )
		{
			pSock.SysMessage( GetDictionaryEntry( 1679, pSock.language )); // You are already disguised.
		}
		else
		{
			pUser.disguiseKit = iUsed;
			ShowDisguiseKitMenu( pSock, pUser );
			return false;
		}
	}

	return true;
}

function ShowDisguiseKitMenu( socket, pUser )
{
	var dGump = new Gump;
	dGump.AddPage( 0 );
	dGump.AddBackground( 100, 10, 400, 430, 2600 );
	dGump.AddHTMLGump( 100, 25, 400, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( 18012, socket.language ) + "</CENTER>" ); // THIEF DISGUISE KIT
	dGump.AddButton( 140, 393, 0x0fa5, 0x0fa7, 1, 0, 0 ); // Okay (done applying changes)
	dGump.AddHTMLGump( 172, 395, 90, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( 18013, socket.language ) + "</CENTER>" ); // OKAY
	dGump.AddButton( 257, 393, 0x0fa5, 0x0fa7, 1, 0, 1 ); // Apply (apply selected options)
	dGump.AddHTMLGump( 289, 395, 90, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( 18014, socket.language ) + "</CENTER>" ); // APPLY

	// Hairs/Horns
	dGump.AddPage( 1 );
	if( pUser.gender == 0 || pUser.gender == 4 ) // Male Human/Gargoyle, show button to display facial hairs
	{
		dGump.AddButton( 155, 350, 0x0fa5, 0x0fa7, 0, 2, 0 ); // Facial Hair Selections
		var facialHairHeaderDict = "";
		if( pUser.gender == 0 )
		{
			facialHairHeaderDict = 18015;
		}
		else
		{
			facialHairHeaderDict = 18017;
		}

		dGump.AddHTMLGump( 180, 350, 150, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( facialHairHeaderDict, socket.language ) + "</CENTER>" ); // Facial Hair/Horn Selections
	}

	var hairEntries;
	var beardEntries;
	switch( pUser.gender )
	{
		case 0: // Male
		default:
			hairEntries = humanMaleHair;
			beardEntries = humanMaleBeard;
			break;
		case 1: // Female
			hairEntries = humanFemaleHair;
			break;
		case 2: // Male Elf
			hairEntries = elfMaleHair;
			break;
		case 3: // Female Elf
			hairEntries = elfFemaleHair;
			break;
		case 4: // Male Gargoyle
			hairEntries = gargoyleMaleHorns;
			beardEntries = gargoyleMaleFacialHorns;
			break;
		case 5: // Female Gargoyle
			hairEntries = gargoyleFemaleHorns;
			break;
	}

	for( var i = 0; i < hairEntries.length; i++ )
	{
		var x = Math.round(( i % 2 ) * 205 );
		var y = Math.round(( i / 2 ) * 55 );

		if( hairEntries[i][1] != 0 )
		{
			dGump.AddBackground( 220 + x, 60 + y, 50, 50, 2620 );
			dGump.AddGump( 153 + x + hairEntries[i][2], 15 + y + hairEntries[i][3], parseInt( hairEntries[i][1] ));
		}

		dGump.AddHTMLGump( 140 + x, 72 + y, 80, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( hairEntries[i][4], socket.language ) + "</CENTER>" ); // Hair name
		dGump.AddRadio( 118 + x, 73 + y, 208, 0, ( i * 2 )  );
	}

	// Beards/Facial Horns
	if( pUser.gender == 0 || pUser.gender == 4 ) // Male Human/Gargoyle, show facial hair entries in another page
	{
		dGump.AddPage( 2 );
		dGump.AddButton( 155, 320, 0x0fae, 0x0fb0, 0, 1, 0 ); // Hair Selections
		dGump.AddHTMLGump( 180, 320, 150, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( 18016, socket.language ) + "</CENTER>" ); // Hair Selections

		for( var i = 0; i < beardEntries.length; i++ )
		{
			var x = Math.round(( i % 2 ) * 205 );
			var y = Math.round(( i / 2 ) * 55 );

			if( beardEntries[i][1] != 0 )
			{
				dGump.AddBackground( 220 + x, 60 + y, 50, 50, 2620 );
				dGump.AddGump( 153 + x + beardEntries[i][2], 15 + y + beardEntries[i][3], beardEntries[i][1] );
			}

			dGump.AddHTMLGump( 140 + x, 72 + y, 80, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( beardEntries[i][4], socket.language ) + "</CENTER>" ); // Hair name
			dGump.AddRadio( 118 + x, 73 + y, 208, 0, ( i * 2 ) + 1 );
		}
	}

	dGump.Send( socket ); // send this gump to client now
	dGump.Free(); // clear this gump from uox-memory
}

function onGumpPress( pSock, buttonID, gumpData )
{
	if( pSock == null )
		return;

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var disguiseKit = pUser.disguiseKit;
	if( !ValidateObject( disguiseKit ))
		return;

	var hairEntries;
	var beardEntries;
	switch( pUser.gender )
	{
		case 0: // Male
		default:
			hairEntries = humanMaleHair;
			beardEntries = humanMaleBeard;
			break;
		case 1: // Female
			hairEntries = humanFemaleHair;
			break;
		case 2: // Male Elf
			hairEntries = elfMaleHair;
			break;
		case 3: // Female Elf
			hairEntries = elfFemaleHair;
			break;
		case 4: // Male Gargoyle
			hairEntries = gargoyleMaleHorns;
			beardEntries = gargoyleMaleFacialHorns;
			break;
		case 5: // Female Gargoyle
			hairEntries = gargoyleFemaleHorns;
			break;
	}

	switch( buttonID )
	{
		case 0: // Okay
			break;
		case 1: // Apply changes
			var radioBtnID = gumpData.getButton( 0 );
			var index = Math.floor( radioBtnID / 2 );

			// Randomize player's name, after storing the original name for later
			if( pUser.origName == "" )
			{
				// Only store name if not already storing name! Or could be storing a fake name
				pUser.origName = pUser.name;
			}
			if( pUser.gender == 0 )
			{
				pUser.SetRandomName( "1" ); // Random human male names from dfndata/npc/namelists.dfn
			}
			else
			{
				pUser.SetRandomName( "2" ); // Random human female names from dfndata/npc/namelists.dfn
			}

			if( parseInt( radioBtnID % 2 ))
			{
				if( pUser.gender == 0 || pUser.gender == 4 )
				{
					// Apply beard style/color modifications
					var playerBeard = pUser.FindItemLayer( 0x10 );
					if( ValidateObject( playerBeard ))
					{
						playerBeard.id = beardEntries[index][0];
						playerBeard.color = 0x0;
					}
					else
					{
						pUser.TextMessage( "No beard found!" );
					}
				}
			}
			else
			{
				// Apply hair style mods
				var playerHair = pUser.FindItemLayer( 0x0b );
				if( ValidateObject( playerHair ))
				{
					playerHair.id = hairEntries[index][0];
					playerHair.color = 0x0;
				}
			}

			// Player's titles should also be hidden? But then again, titles of players already
			// members of thieves guild - a requirement to use disguise kit should not be showing
			// any titles in any case

			// Mark player as disguised
			pUser.isDisguised = true;

			// Start Disguise timer
			pUser.KillJSTimer( 1, 5023 );

			TriggerEvent(50104, "RemoveBuff", pUser, 1033);// Remove Current Disguise Buff

			pUser.StartTimer( disguiseDuration, 1, 5023 );

			pSock.SysMessage( GetDictionaryEntry( 18037, pSock.language )); // Disguises wear off after 2 hours. : You're looking good.

			TriggerEvent(50104, "AddBuff", pUser, 1033, 1075821, 1075820, 7200, ""); // Add Disguise Buff

			// Resend Disguise Kit Gump
			ShowDisguiseKitMenu( pSock, pUser );
			break;
		default:
			pUser.TextMessage( "Button pressed: " + buttonID );
			break;
	}
}

function onTimer( pUser, timerID )
{
	if( timerID == 1 )
	{
		// Disguise has expired, return player to their normal self!
		pUser.name = pUser.origName;
		pUser.origName = "";

		// Restore hair style and color
		var playerHair = pUser.FindItemLayer( 0x0b );
		if( ValidateObject( playerHair ))
		{
			playerHair.id = pUser.hairStyle;
			playerHair.color = pUser.hairColor;
		}

		if( pUser.gender == 0 )
		{
			var playerBeard = pUser.FindItemLayer( 0x10 );
			if( ValidateObject( playerBeard ))
			{
				playerBeard.id = pUser.beardStyle;
				playerBeard.color = pUser.beardColor;
			}
		}

		// Remove disguised marker
		pUser.isDisguised = false;
	}
}

function _restorecontext_() {}
