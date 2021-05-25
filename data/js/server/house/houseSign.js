// House menu that appears when double-clicking the sign on a house
// Uses script trigger 15001, applied via jse_typeassociations.scp based on type 203 - IT_HOUSESIGN
var lockdowns = 0;
var maxLockdowns = 0;
var secureContainers = 0;
var maxSecureContainers = 0;
var trashContainers = 0;
var maxTrashContainers = 0;
var vendors = 0;
var maxVendors = 0;
var bans = 0;
var maxBans = 0;
var friends = 0;
var maxFriends = 0;
var guests = 0;
var maxGuests = 0;
var owners = 0;
var maxOwners = 0;
var visitCount = 0;
var iMulti;
var iSign;
var houseOwner = "";
var houseIsPublic = false;
var houseCoOwnerList = [];
var houseFriendList = [];
var houseGuestList = [];
var houseBanList = [];
var houseBuildTime = "";
var houseTradeTime = "";

// Fetch uox.ini settings on whether characters on same account as house owner will be
// treated as if they were co-owners of the house
const coOwnHousesOnSameAccount = GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ); //CoOwnHousesOnSameAccount();

function onUseUnChecked( pUser, iUsed )
{
	// Grab reference to multi stored on sign
	iMulti = iUsed.multi;

	if( ValidateObject( iMulti ))
	{
		// Perform a range check, making sure player can only use it within reasonable distance from the sign
		if( !pUser.InRange( iUsed, 18 ))
		{
			pUser.SysMessage( GetDictionaryEntry( 393, pUser.socket.language ) ); // That is too far away.
		}
		else
		{
			// Get public/private status of house
			houseIsPublic = iMulti.isPublic;

			// Let's keep track of some data for use in the house menu gump!
			// First, get an up-to-date reference to owner of house
			var houseOwnerSerial = iMulti.owner.serial & 0x00FFFFFF;
			houseOwner = (CalcCharFromSer(houseOwnerSerial)).name;

			// The double-clicked item is the house sign!
			iSign = iUsed;

			// Check if player is on owner list of house
			// TODO - OR, if option is enabled, simply check if player is holding a KEY
			// This key-based option would do away with lists of owners/co-owners. Keyholder is owner!
			// If multiple people hold key to house - well, multiple people own the house.
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ) || iMulti.IsOnFriendList( pUser ) || pUser.isGM )
			{
				// Store serial of sign in multi's MORE property
				if( iMulti.more == 0 )
					iMulti.more = iUsed.serial;

				// Set scripttrigger 15000 on houses that were created prior to the version that
				// automatically adds this based on tag in houses.dfn
				var scriptFound = false;
				var scriptTriggers = iMulti.scriptTriggers;
				for( var i = 0; i < scriptTriggers.length; i++ )
				{
					if( scriptTriggers[i] == 15000 )
						scriptFound = true;
				}
				if( !scriptFound )
					iMulti.scripttrigger = 15000;

				// Then grab some other data and store them in global
				// (across this instance of the script only) variables
				lockdowns = iMulti.lockdowns;
				maxLockdowns = iMulti.maxLockdowns;
				secureContainers = iMulti.secureContainers;
				maxSecureContainers = iMulti.maxSecureContainers;
				trashContainers = iMulti.trashContainers;
				maxTrashContainers = iMulti.maxTrashContainers;
				vendors = iMulti.vendors;
				maxVendors = iMulti.maxVendors;
				owners = iMulti.owners;
				maxOwners = iMulti.maxOwners;
				friends = iMulti.friends;
				maxFriends = iMulti.maxFriends;
				bans = iMulti.bans;
				maxBans = iMulti.maxBans;
				houseBuildTime = iMulti.buildTimestamp;
				houseTradeTime = iMulti.tradeTimestamp;
				visitCount = iMulti.GetTag( "visitCount" );

				// Reset lists
				houseCoOwnerList.length = 0;
				houseFriendList.length = 0;
				houseBanList.length = 0;

				// Open house menu gump
				houseOwnerGump( pUser );
			}
			else
			{
				visitCount = iMulti.GetTag( "visitCount" );
				houseGuestGump( pUser );
			}
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 1820, pUser.socket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
	}
	return false;
}

// Handle button presses in the house menu
function onGumpPress( pSocket, pButton, gumpData )
{
	var pUser = pSocket.currentChar;
	switch( pButton )
	{
		case 0: //Closes gump, stops refreshing
			break;
		case 1: // Open main house gump
			houseOwnerGump( pUser );
			break;
		case 2: // Open house sign selection gump
			if( houseIsPublic )
			{
				if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
				{
					houseSignSelectionGump( pUser );
				}
				else
				{
					pSocket.SysMessage( GetDictionaryEntry( 1940, pSocket.language )); // Only the house owner and co-owners can change the sign type for this house!
				}
			}
			else
				pSocket.SysMessage( GetDictionaryEntry( 1941, pSocket.language )); // You cannot change the sign on a private house!
			break;
		case 3: // Change house sign
			if( houseIsPublic )
			{
				if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
				{
					if( pSocket.GetWord( 21 ) > 0 )
					{
						var newID = pSocket.GetWord( 21 );
						if( iSign.id % 2 == 0 )
							iSign.id = newID;
						else
						{
							newID -= 1;
							iSign.id = newID;
						}
						pSocket.SysMessage( GetDictionaryEntry( 556, pUser.socket.language )); // House sign changed.
					}
				}
				else
				{
					pSocket.SysMessage( GetDictionaryEntry( 1940, pSocket.language )); // Only the house owner and co-owners can change the sign type for this house!
				}
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1941, pSocket.language )); // You cannot change the sign on a private house!
			}
			houseOwnerGump( pUser );
			break;
		case 4: // Change House Name
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
			{
				houseSignNameInputGump( pUser );
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1942, pSocket.language )); // Only the house owner and co-owners can change the name of the sign for this house!
			}
			break;
		case 5: // Confirm New Name
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
			{
				var newName = gumpData.getEdit(0);
				if( newName.length <= 60 ) // Estimated longest name that fits inside sign gump
				{
					iSign.name = newName;
					pSocket.SysMessage( GetDictionaryEntry( 1943, pSocket.language ), newName ); // House name changed to %s.
				}
				else
					pSocket.SysMessage( GetDictionaryEntry( 1944, pSocket.language )); // That name is too long.
			}
			break;
		case 10: // List co-owners
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
			{
				displayPlayerList( pUser, "owner" );
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1945, pSocket.language )); // Only the house owner and co-owners can view the list of co-owners for this house!
			}
			break;
		case 11: // Add player as co-owner - only actual owner should be able to do this!
			if( iMulti.IsOwner( pUser ))
				TriggerEvent( 15002, "AddOwner", pSocket, iMulti );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1832, pSocket.language )); // Only the primary house owner may add co-owners!
			break;
		case 12: // Remove player as co-owner
			if( iMulti.IsOwner( pUser ) || iMulti.IsOnOwnerList( pUser ))
				TriggerEvent( 15002, "RemoveOwnerTrigger", pSocket, iMulti );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1839, pSocket.language )); // Only the primary house owner can remove co-owners!
			break;
		case 13: // Clear co-owner list - only actual owner should be able to do this!
			if( iMulti.IsOwner( pUser ))
			{
				var confirmButtonID = 14;
				var confirmString = GetDictionaryEntry( 1946, pSocket.language ); // Are you sure you want to <I>clear the co-owner list</I> for this house?
				confirmActionGump( pUser, confirmString, confirmButtonID );
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1947, pSocket.language )); // Only the house owner can clear the co-owners list!
			}
			break;
		case 14: // Confirm clear co-owner list
			if( iMulti.IsOwner( pUser ))
			{
				iMulti.ClearOwnerList();
				pSocket.SysMessage( GetDictionaryEntry( 1948, pSocket.language )); // Owner list for this house has been cleared!
			}
			break;
		case 20: // List friends
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ) || iMulti.IsOnFriendList( pUser ))
				displayPlayerList( pUser, "friend" );
			break;
		case 21: // Add player as friend of multi
			if( iMulti.IsOnOwnerList( pUser )|| ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
				TriggerEvent( 15002, "AddFriend", pSocket, iMulti );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1848, pSocket.language )); // Only house owners and co-owners can add someone to the friend list!
			break;
		case 22: // Remove player as friend of multi
			if( iMulti.IsOnOwnerList( pUser )|| ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
				TriggerEvent( 15002, "RemoveFriendTrigger", pSocket, iMulti );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1853, pSocket.language )); // Only house owners and co-owners can remove someone from the friend list!
			break;
		case 23: // Clear friend list - only actual owner should be able to do this!
			if( iMulti.IsOwner( pUser ))
			{
				var confirmButtonID = 24;
				var confirmString = GetDictionaryEntry( 1949, pSocket.language ); // Are you sure you want to <I>clear the friend list</I> for this house?
				confirmActionGump( pUser, confirmString, confirmButtonID );
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1950, pSocket.language )); // Only the house owner can clear the friend list!
			}
			break;
		case 24: // Confirm clear friend list
			if( iMulti.IsOwner( pUser ))
			{
				iMulti.ClearFriendList();
				pSocket.SysMessage( GetDictionaryEntry( 1951, pSocket.language )); // Friend list for this house has been cleared!
			}
			break;
		case 30: // Ban player from house
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ) || iMulti.IsOnFriendList( pUser ))
				TriggerEvent( 15002, "BanPlayer", pSocket, iMulti );
			break;
		case 31: // Eject someone from house
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ) || iMulti.IsOnFriendList( pUser ))
				TriggerEvent( 15002, "EjectPlayer", pSocket, iMulti );
			break;
		case 32: // List banned players
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ) || iMulti.IsOnFriendList( pUser ))
				displayPlayerList( pUser, "banned" );
			break;
		case 33: // Remove someone from ban list
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ) || iMulti.IsOnFriendList( pUser ))
				TriggerEvent( 15002, "UnbanPlayerTrigger", pSocket, iMulti );
			break;
		case 40: // List guests
			if( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ) || iMulti.IsOnFriendList( pUser ))
				displayPlayerList( pUser, "guest" );
			break;
		case 41: // Add player as guest of multi
			if( iMulti.IsOnOwnerList( pUser )|| ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
				TriggerEvent( 15002, "AddGuest", pSocket, iMulti );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1859, pSocket.language )); // Only house owners and co-owners can add someone to the guest list!
			break;
		case 42: // Remove player as guest of multi
			if( iMulti.IsOnOwnerList( pUser )|| ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))
				TriggerEvent( 15002, "RemoveGuestTrigger", pSocket, iMulti );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1867, pSocket.language )); // Only house owners and co-owners can remove someone from the guest list!
			break;
		case 43: // Clear guest list - only actual owner should be able to do this!
			if( iMulti.IsOwner( pUser ))
			{
				var confirmButtonID = 44;
				var confirmString = GetDictionaryEntry( 1952, pSocket.language ); // Are you sure you want to <I>clear the guest list</I> for this house?
				confirmActionGump( pUser, confirmString, confirmButtonID );
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1953, pSocket.language )); // Only the house owner can clear the guest list!
			}
			break;
		case 44: // Confirm clear guest list
			if( iMulti.IsOwner( pUser ))
			{
				iMulti.ClearGuestList();
				pSocket.SysMessage( GetDictionaryEntry( 1954, pSocket.language )); // Friend list for this house has been cleared!
			}
			break;
		case 50: // Transfer primary ownership of house
			if( iMulti.IsOwner( pUser ))
			{
				var confirmButtonID = 55;
				var confirmString = GetDictionaryEntry( 1955, pSocket.language ); // Are you sure you want to <I>transfer ownership</I> of this house?
				confirmActionGump( pUser, confirmString, confirmButtonID );
			}
			else
				pSocket.SysMessage( GetDictionaryEntry( 1822, pSocket.language )); // Only the primary house owner can transfer ownership of the house!
			break;
		case 51: // Demolish house and get a deed back
			if( iMulti.IsOwner( pUser ))
			{
				var confirmButtonID = 56;
				var confirmString = GetDictionaryEntry( 1956, pSocket.language ); // Are you sure you want to <I>demolish</I> this house?
				confirmActionGump( pUser, confirmString, confirmButtonID );
			}
			else
				pSocket.SysMessage( GetDictionaryEntry( 1957, pSocket.language )); // Only the primary house owner can demolish the house!
			break;
		case 52: // Change the house locks
			if( iMulti.IsOwner( pUser ))
			{
				var confirmButtonID = 57;
				var confirmString = GetDictionaryEntry( 1958, pSocket.language ); // Are you sure you want to <I>change the locks</I> on this house?
				confirmActionGump( pUser, confirmString, confirmButtonID );
			}
			else
				pSocket.SysMessage( GetDictionaryEntry( 1959, pSocket.language )); // Only the primary house owner can change the house locks!
			break;
		case 53: // Declare building to be public
			if( iMulti.IsOwner( pUser ))
			{
				var confirmButtonID = 58;
				var confirmString = GetDictionaryEntry( 1960, pSocket.language ); // Are you sure you want to make this house <I>Public</I>?
				confirmActionGump( pUser, confirmString, confirmButtonID );
			}
			else
				pSocket.SysMessage( GetDictionaryEntry( 1961, pSocket.language )); // Only the primary house owner can declare the house as public!
			break;
		case 54: // Declare building to be private
			if( iMulti.IsOwner( pUser ))
			{
				var confirmButtonID = 59;
				var confirmString = GetDictionaryEntry( 1962, pSocket.language ); // Are you sure you want to make this house <I>Private</I>?
				confirmActionGump( pUser, confirmString, confirmButtonID );
			}
			else
				pSocket.SysMessage( GetDictionaryEntry( 1963, pSocket.language )); // Only the primary house owner can declare the house as private!
			break;
		case 55: // Confirm transfer of ownership
			if( iMulti.IsOwner( pUser ))
				TriggerEvent( 15002, "TransferOwnership", pSocket, iMulti );
			break;
		case 56: // Confirm demolish house
			if( iMulti.IsOwner( pUser ))
				TriggerEvent( 15002, "DemolishHouse", pSocket, iMulti );
			break;
		case 57: // Confirm change locks
			if( iMulti.IsOwner( pUser ))
				TriggerEvent( 15002, "ChangeHouseLocks", pSocket, iMulti );
			break;
		case 58: // Confirm turn house public
			if( iMulti.IsOwner( pUser ))
				TriggerEvent( 15002, "DeclareHousePublic", pSocket, iMulti );
			break;
		case 59: // Confirm turn house private
			if( iMulti.IsOwner( pUser ))
				TriggerEvent( 15002, "DeclareHousePrivate", pSocket, iMulti );
			break;
		default:
			break;
	}

	// Handle owner/friend/ban-list removal buttons
	if( pButton >= 100 && pButton <= 250 && iMulti.IsOwner( pUser ))
		TriggerEvent( 15002, "RemoveOwner", pSocket, houseCoOwnerList[(pButton-100)], iMulti );
	else if( pButton >= 300 && pButton <= 450 && ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum )))
		TriggerEvent( 15002, "RemoveFriend", pSocket, houseFriendList[(pButton-300)], iMulti );
	else if( pButton >= 500 && pButton <= 650 && ( iMulti.IsOnFriendList( pUser ) || ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum ))))
		TriggerEvent( 15002, "UnbanPlayer", pSocket, houseBanList[(pButton-500)], iMulti );
	else if( pButton >= 700 && pButton <= 750 && ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == pUser.accountNum )))
		TriggerEvent( 15002, "RemoveGuest", pSocket, houseGuestList[(pButton-700)], iMulti );
}

// Display list of co-owners, friends or banned players
function displayPlayerList( pUser, listType )
{
	var playerListCount = 0;
	var playerIndex = 0;
	var playerIndexStart = 0;

	if( listType == "owner" ) // co-owners
	{
		playerListCount = iMulti.owners;
		playerIndex = 100;
		playerIndexStart = 100;
	}
	else if( listType == "friend" ) // friends
	{
		playerListCount = iMulti.friends;
		playerIndex = 300;
		playerIndexStart = 300;
	}
	else if( listType == "guest" ) // friends
	{
		playerListCount = iMulti.guests;
		playerIndex = 700;
		playerIndexStart = 700;
	}
	else // "banned" - bans
	{
		playerListCount = iMulti.bans;
		playerIndex = 500;
		playerIndexStart = 500;
	}

	var pageNumTotal = Math.ceil(playerListCount / 8);
	var yGumpOffset = 140;
	var pageOffset = 1;
	var houseChar;

	// Start of gump!
	var housePlayerListGump = new Gump;

	// Page 0 - Shared with all other pages
	housePlayerListGump.AddPage( 0 );
	housePlayerListGump.AddBackground( 0, 0, 250, 360, 2620 );  		// Tile Dark Gray Background
	housePlayerListGump.AddBackground( 10, 10, 230, 340, 3000 );  	// Tile White Background
	housePlayerListGump.AddButton( 205, 15, 4017, 4018, 1, 0, 1 ); 	// Exit
	housePlayerListGump.AddHTMLGump( 5, 15, 245, 20, 10, 0, "<CENTER>House Owner</CENTER>");
	housePlayerListGump.AddHTMLGump( 5, 30, 245, 40, 10, 0, "<CENTER>" + houseOwner + "</CENTER>" );
	housePlayerListGump.AddGump( 35, 50, 10452 ); 				// Sign Image
	if( listType == "owner" )
	{
		housePlayerListGump.AddHTMLGump( 35, 115, 170, 20, 0, 0, "CO-OWNERS (" + owners + "/" + maxOwners + ")" );
		housePlayerListGump.AddButton( 195, 115, 5402, 5402, 1, 0, 11);	// Button - Add a co-owner
	}
	else if( listType == "friend" )
	{
		housePlayerListGump.AddHTMLGump( 35, 115, 170, 20, 0, 0, "FRIENDS (" + friends + "/" + maxFriends + ")" );
		housePlayerListGump.AddButton( 195, 115, 5402, 5402, 1, 0, 21);	// Button - Add a Friend
	}
	else if( listType == "guest" )
	{
		housePlayerListGump.AddHTMLGump( 35, 113, 170, 20, 0, 0, "GUESTS (" + guests + "/" + maxGuests + ")" );
		housePlayerListGump.AddButton( 195, 115, 5402, 5402, 1, 0, 41);	// Button - Add a Guest
	}
	else // "banned"
	{
		housePlayerListGump.AddHTMLGump( 35, 115, 170, 20, 0, 0, "BANS (" + bans + "/" + maxBans + ")" );
		housePlayerListGump.AddButton( 195, 115, 5402, 5402, 1, 0, 30);	// Button - Ban someone from the house
	}

	for( houseChar = iMulti.FirstChar( listType ); !iMulti.FinishedChars( listType ); houseChar = iMulti.NextChar( listType ) )
	{
		// Don't attempt to add invalid characters or the actual house owner to the list
		if( !ValidateObject( houseChar ))
			continue;

		if( iMulti.IsOwner( houseChar ))
			continue;

		// Set page 1 first time around
		if( pageOffset == 1 )
		{
			housePlayerListGump.AddPage( pageOffset );
			if( pageNumTotal > 1 )
				housePlayerListGump.AddHTMLGump( 80, 300, 150, 20, 0, 0, ("Page " + pageOffset + " of " + pageNumTotal) );
		}

		if( playerIndex >= ( playerIndexStart + (4 * pageOffset) ))
		{
			// Reset y position, start a new page
			yGumpOffset = 140;
			pageOffset += 1;

			// Before changing page, add a NEXT button to the old page
			housePlayerListGump.AddButton( 150, 323, 4005, 4006, 0, pageOffset, 0 ); 		// Next page
			housePlayerListGump.AddHTMLGump( 190, 325, 40, 20, 0, 0, "NEXT");


			// Start of next page, with a PREV button
			housePlayerListGump.AddPage( pageOffset );
			housePlayerListGump.AddHTMLGump( 80, 300, 150, 20, 0, 0, ("Page " + pageOffset + " of " + pageNumTotal) );
			housePlayerListGump.AddButton( 30, 323, 4014, 4015, 0, (pageOffset - 1), 0 ); 		// Prev page
			housePlayerListGump.AddHTMLGump( 70, 325, 40, 20, 0, 0, "PREV");
		}

		// Add button to remove a co-owner... but do we know which co-owner this is?
		housePlayerListGump.AddButton( 25, yGumpOffset, 2710, 2711, 1, 0, playerIndex);	// Button - Remove a co-owner
		housePlayerListGump.AddHTMLGump( 50, yGumpOffset, 170, 20, 0, 0, "(Remove) - " + houseChar.name );

		if( listType == "owner" ) // owners
			houseCoOwnerList.push( houseChar );
		else if( listType == "friend" ) // friends
			houseFriendList.push( houseChar );
		else if( listType == "guest" ) // guests
			houseGuestList.push( houseChar );
		else // "banned" - bans
			houseBanList.push( houseChar );

		yGumpOffset += 20;
		playerIndex += 1;
	}

	// All done! Send gump to client
	housePlayerListGump.Send( pUser.socket );
	housePlayerListGump.Free();
}

// Gump displayed to guests
function houseGuestGump( pUser )
{
	var houseGuestGump = new Gump;
	// Page 0 - Shared with all other pages
	houseGuestGump.AddPage( 0 );
	houseGuestGump.AddBackground( 0, 0, 210, 200, 2620 );	// Tile Dark Gray Background
	houseGuestGump.AddBackground( 10, 10, 190, 180, 3000 );	// Tile White Background
	houseGuestGump.AddGump( 30, 0, 100 ); 					// Sign Image

	houseGuestGump.AddHTMLGump( 45, Math.max((60 - iSign.name.length),10), 115, 75, 26, 0, ("<CENTER><BIG>" + iSign.name + "</BIG></CENTER>") );
	houseGuestGump.AddHTMLGump( 30, 105, 140, 20, 0, 0, "<CENTER>Owned by: " + houseOwner + "</CENTER>" );
	if( !houseIsPublic )
		houseGuestGump.AddHTMLGump( 20, 125, 150, 40, 26, 0, "<CENTER>This house is <BASEFONT color=#FF0000>private</BASEFONT></CENTER>");
	else
	{
		houseGuestGump.AddHTMLGump( 20, 125, 170, 20, 26, 0, "<CENTER>This house is </CENTER>");
		houseGuestGump.AddHTMLGump( 20, 145, 170, 20, 26, 0, "<CENTER><BASEFONT color=#00FF00>open to the public</BASEFONT></CENTER>");
		houseGuestGump.AddHTMLGump( 20, 165, 170, 20, 26, 0, "<CENTER>"+visitCount+" visitor(s)</CENTER>");
	}

	houseGuestGump.AddButton( 175, 10, 4017, 4018, 1, 0, 0 ); 		// Exit
	houseGuestGump.Send( pUser.socket );
	houseGuestGump.Free();
}

// Gump displayed to house owners
function houseOwnerGump( pUser )
{
	var houseOwnerGump = new Gump;
	// Page 0 - Shared with all other pages
	houseOwnerGump.AddPage( 0 );
	houseOwnerGump.AddBackground( 0, 0, 420, 400, 2620 );	// Tile Dark Gray Background
	houseOwnerGump.AddBackground( 10, 10, 400, 380, 3000 );	// Tile White Background
	houseOwnerGump.AddGump( 140, 0, 100 ); 					// Sign Image

	houseOwnerGump.AddHTMLGump( 155, Math.max((60 - iSign.name.length),10), 115, 75, 26, 0, ("<CENTER><BIG>" + iSign.name + "</BIG></CENTER>") );
	houseOwnerGump.AddButton( 35, 107, 4011, 4012, 0, 1, 0 ); 		// Info
	houseOwnerGump.AddButton( 35, 352, 4005, 4006, 1, 0, 2 ); 		// Change house sign
	houseOwnerGump.AddButton( 165, 352, 4005, 4006, 1, 0, 4 ); 		// Change house name
	houseOwnerGump.AddButton( 165, 107, 4008, 4009, 0, 2, 0 ); 		// Friends
	houseOwnerGump.AddButton( 300, 107, 4029, 4030, 0, 3, 0 ); 		// Options
	houseOwnerGump.AddButton( 300, 352, 4017, 4018, 1, 0, 0 ); 		// Exit
	houseOwnerGump.AddHTMLGump( 70, 109, 90, 20, 0, 0, "INFO");				// Info
	houseOwnerGump.AddHTMLGump( 200, 109, 65, 20, 0, 0, "FRIENDS");			// Friends
	houseOwnerGump.AddHTMLGump( 335, 109, 75, 20, 0, 0, "OPTIONS");			// options
	houseOwnerGump.AddHTMLGump( 70, 354, 175, 20, 0, 0, "Change Sign");		// Change Sign
	houseOwnerGump.AddHTMLGump( 200, 354, 175, 20, 0, 0, "Change Name");	// Change Name
	houseOwnerGump.AddHTMLGump( 335, 354, 40, 20, 0, 0, "EXIT");			// Exit

	// Page 1 - Information
	houseOwnerGump.AddPage( 1 );
	houseOwnerGump.AddHTMLGump( 35, 145, 350,  20, 0, 0, "Owned by: " + houseOwner );
	if( !houseIsPublic )
		houseOwnerGump.AddHTMLGump( 35, 165, 350, 20, 26, 0, "This house is <BASEFONT color=#FF0000>private</BASEFONT>");
	else
		houseOwnerGump.AddHTMLGump( 35, 165, 350, 20, 26, 0, "This house is <BASEFONT color=#00FF00>open to the public</BASEFONT> ("+visitCount.toString()+" visitors) ");

	houseOwnerGump.AddHTMLGump( 35, 200, 350, 20, 0, 0, "Number of locked down items:");
	houseOwnerGump.AddHTMLGump( 35, 220, 350, 20, 0, 0, "Number of secure containers:");
	houseOwnerGump.AddHTMLGump( 35, 240, 350, 20, 0, 0, "Number of trash barrels:");
	houseOwnerGump.AddHTMLGump( 35, 260, 350, 20, 0, 0, "Number of player vendors:");

	houseOwnerGump.AddHTMLGump( 35, 300, 350, 20, 0, 0, "Built On:");
	houseOwnerGump.AddHTMLGump( 35, 320, 350, 20, 0, 0, "Last Traded:");

	houseOwnerGump.AddHTMLGump( 300, 200, 100, 20, 0, 0, " " + lockdowns + " / " + maxLockdowns);
	houseOwnerGump.AddHTMLGump( 300, 220, 100, 20, 0, 0, " " + secureContainers + " / " + maxSecureContainers);
	houseOwnerGump.AddHTMLGump( 300, 240, 100, 20, 0, 0, " " + trashContainers + " / " + maxTrashContainers);
	houseOwnerGump.AddHTMLGump( 300, 260, 100, 20, 0, 0, " " + vendors + " / " + maxVendors);

	houseOwnerGump.AddHTMLGump( 180, 300, 200, 20, 0, 0, '<div align="right">' + houseBuildTime + '</div>');
	houseOwnerGump.AddHTMLGump( 180, 320, 200, 20, 0, 0, '<div align="right">' + houseTradeTime + '</div>');

	// Page 2 - Friends, Co-owners and Banlist
	houseOwnerGump.AddPage( 2 );
	houseOwnerGump.AddButton( 65, 145, 2714, 2715, 1, 0, 10);	// Button - List of co-owners
	houseOwnerGump.AddButton( 65, 165, 2714, 2715, 1, 0, 11);	// Button - Add a co-owner
	houseOwnerGump.AddButton( 65, 185, 2710, 2711, 1, 0, 12);	// Button - Remove a co-owner
	houseOwnerGump.AddButton( 65, 205, 2708, 2709, 1, 0, 13);	// Button - Clear co-owner list
	houseOwnerGump.AddHTMLGump( 90, 145, 170, 20, 0, 0, "List of co-owners");
	houseOwnerGump.AddHTMLGump( 90, 165, 170, 20, 0, 0, "Add a co-owner");
	houseOwnerGump.AddHTMLGump( 90, 185, 170, 20, 0, 0, "Remove a co-owner");
	houseOwnerGump.AddHTMLGump( 90, 205, 170, 20, 0, 0, "Clear co-owner list");
	houseOwnerGump.AddButton( 225, 145, 2714, 2715, 1, 0, 20);	// Button - List of Friends
	houseOwnerGump.AddButton( 225, 165, 2714, 2715, 1, 0, 21);	// Button - Add a Friend
	houseOwnerGump.AddButton( 225, 185, 2710, 2711, 1, 0, 22);	// Button - Remove a Friend
	houseOwnerGump.AddButton( 225, 205, 2708, 2709, 1, 0, 23);	// Button - Clear Friends list
	houseOwnerGump.AddHTMLGump( 250, 145, 170, 20, 15, 0, "List of Friends");
	houseOwnerGump.AddHTMLGump( 250, 165, 170, 20, 16, 0, "Add a Friend");
	houseOwnerGump.AddHTMLGump( 250, 185, 170, 20, 17, 0, "Remove a Friend");
	houseOwnerGump.AddHTMLGump( 250, 205, 170, 20, 18, 0, "Clear Friends list");

	if( !houseIsPublic )
	{
		houseOwnerGump.AddButton( 65, 245, 2714, 2715, 1, 0, 40);	// Button - List of guests
		houseOwnerGump.AddButton( 65, 265, 2714, 2715, 1, 0, 41);	// Button - Add a guest
		houseOwnerGump.AddButton( 65, 285, 2710, 2711, 1, 0, 42);	// Button - Remove a guest
		houseOwnerGump.AddButton( 65, 305, 2708, 2709, 1, 0, 43);	// Button - Clear guest list
		houseOwnerGump.AddHTMLGump( 90, 245, 170, 20, 0, 0, "List of guests");
		houseOwnerGump.AddHTMLGump( 90, 265, 170, 20, 0, 0, "Add a guest");
		houseOwnerGump.AddHTMLGump( 90, 285, 170, 20, 0, 0, "Remove a guest");
		houseOwnerGump.AddHTMLGump( 90, 305, 170, 20, 0, 0, "Clear guest list");
	}

	houseOwnerGump.AddButton( 225, 245, 2714, 2715, 1, 0, 32);	// Button - View a list of banned people
	houseOwnerGump.AddButton( 225, 265, 2714, 2715, 1, 0, 30);	// Button - Ban someone from the house
	houseOwnerGump.AddButton( 225, 285, 2710, 2711, 1, 0, 33);	// Button - Lift a ban
	houseOwnerGump.AddButton( 225, 305, 2708, 2709, 1, 0, 31);	// Button - Eject someone from the house
	houseOwnerGump.AddHTMLGump( 250, 245, 200, 20, 21, 0, "List of banned players");
	houseOwnerGump.AddHTMLGump( 250, 265, 200, 20, 19, 0, "<BASEFONT COLOR=#FF0000>Ban player</BASEFONT>");
	houseOwnerGump.AddHTMLGump( 250, 285, 200, 20, 22, 0, "Lift a ban");
	houseOwnerGump.AddHTMLGump( 250, 305, 200, 20, 20, 0, "Eject player");

	// Page 3 - Options
	houseOwnerGump.AddPage( 3 );
	houseOwnerGump.AddButton( 35, 155, 2714, 2715, 1, 0, 50);		// Button - Transfer ownership of the house
	houseOwnerGump.AddButton( 35, 185, 2714, 2715, 1, 0, 51);		// Button - Demolish the house and get a deed back
	houseOwnerGump.AddButton( 35, 215, 2714, 2715, 1, 0, 52);	// Button - Change the house locks
	if( !houseIsPublic )
		houseOwnerGump.AddButton( 35, 245, 2714, 2715, 1, 0, 53);	// Button - Declare this building to be public. This will make your doors unlockable
	else
		houseOwnerGump.AddButton( 35, 245, 2714, 2715, 1, 0, 54);	// Button - Declare this building to be private.
	houseOwnerGump.AddHTMLGump( 60, 155, 350, 20, 23, 0, "Transfer ownership of the house");
	houseOwnerGump.AddHTMLGump( 60, 185, 350, 20, 24, 0, "Demolish the house and get a deed back");
	houseOwnerGump.AddHTMLGump( 60, 215, 350, 20, 25, 0, "Change the house locks");
	if( !houseIsPublic )
		houseOwnerGump.AddHTMLGump( 60, 245, 350, 40, 26, 0, "Declare this building to be public. This will make your front door unlockable.");
	else
		houseOwnerGump.AddHTMLGump( 60, 245, 350, 40, 26, 0, "Declare this building to be private.");

	houseOwnerGump.Send( pUser.socket );
	houseOwnerGump.Free();
}

function houseSignNameInputGump( pUser )
{
	var signNameInputGump = new Gump;
	signNameInputGump.AddPage( 0 );
	signNameInputGump.AddBackground( 0, 0, 350, 180, 2620 );  	// Tile Dark Gray Background
	signNameInputGump.AddBackground( 10, 10, 330, 160, 3000 );  // Tile White Background
	signNameInputGump.AddButton( 305, 15, 4017, 4018, 1, 0, 1 ); 	// Exit
	signNameInputGump.AddButton( 175, 125, 4023, 4024, 1, 0, 5 ); // Okay, confirm
	signNameInputGump.AddGump( 25, 45, 100 ); 						// Sign Image
	signNameInputGump.AddHTMLGump( 25, 20, 280, 20, 0, 0, "<BIG>Enter new name for sign:</BIG>" );
	signNameInputGump.AddHTMLGump( 175, 60, 150, 60, 0, 0, "(Note: Name will be centered when viewed)" );
	signNameInputGump.AddHTMLGump( 210, 127, 130, 20, 0, 0, "Confirm New Name" );
	signNameInputGump.AddTextEntryLimited( 50, 60, 100, 200, 0, 1, 3, iSign.name, 60 );
	signNameInputGump.Send( pUser.socket );
	signNameInputGump.Free();
}

function confirmActionGump( pUser, confirmString, confirmButtonID )
{
	var confirmGump = new Gump;
	confirmGump.AddPage( 0 );
	confirmGump.AddBackground( 0, 0, 300, 140, 2620 ); // Tile Dark Gray Background
	confirmGump.AddBackground( 10, 10, 280, 120, 3000 ); // Tile White Background
	confirmGump.AddHTMLGump( 10, 20, 280, 60, 0, 0, "<CENTER><BIG>Confirm Action?</BIG></CENTER>" );
	confirmGump.AddButton( 100, 100, 4023, 4024, 1, 0, confirmButtonID ); // Okay, confirm
	confirmGump.AddButton( 175, 100, 4020, 4021, 1, 0, 1 ); // Cancel
	confirmGump.AddText( 25, 100, 0, "CONFIRM >" );
	confirmGump.AddText( 210, 100, 0, "< CANCEL" );
	confirmGump.AddHTMLGump( 25, 50, 250, 60, 0, 0, confirmString );
	confirmGump.Send( pUser.socket );
	confirmGump.Free();
}

// Gump with sign options - radioIDs matches itemID of sign in question,
// so the radioID can be used to set the ID of the sign directly when player confirms selection!
function houseSignSelectionGump( pUser )
{
	var signSelectGump = new Gump;
	signSelectGump.AddPage( 0 );
	signSelectGump.AddBackground( 0, 0, 350, 370, 2620 );
	signSelectGump.AddBackground( 10, 10, 330, 350, 3000 );
	signSelectGump.AddHTMLGump( 0, 13, 350, 20, 0, 0, "<CENTER><BIG>Select new house sign:</BIG></CENTER>" );
	signSelectGump.AddHTMLGump( 0, 315, 350, 20, 0, 0, "<CENTER>Confirm with OK</CENTER>" );
	signSelectGump.AddButton( 305, 15, 4017, 4018, 1, 0, 1 ); 	// Exit
	signSelectGump.AddButton( 150, 335, 4023, 4024, 1, 0, 3 ); // Change house sign OK
	signSelectGump.AddPage( 1 );
	signSelectGump.AddRadio( 22, 40, 9722, (iSign.id == 2966? 1 : 0), 2966 );
	signSelectGump.AddPicture( 45, 40, 2966 );
	signSelectGump.AddRadio( 22, 80, 9722, (iSign.id == 2980? 1 : 0), 2980 );
	signSelectGump.AddPicture( 45, 80, 2980 );
	signSelectGump.AddRadio( 22, 120, 9722, (iSign.id == 2982? 1 : 0), 2982 );
	signSelectGump.AddPicture( 45, 115, 2982 );
	signSelectGump.AddRadio( 22, 160, 9722, (iSign.id == 2984? 1 : 0), 2984 );
	signSelectGump.AddPicture( 45, 155, 2984 );
	signSelectGump.AddRadio( 22, 200, 9722, (iSign.id == 2986? 1 : 0), 2986 );
	signSelectGump.AddPicture( 45, 195, 2986 );
	signSelectGump.AddRadio( 22, 240, 9722, (iSign.id == 2988? 1 : 0), 2988 );
	signSelectGump.AddPicture( 45, 235, 2988 );
	signSelectGump.AddRadio( 22, 280, 9722, (iSign.id == 2990? 1 : 0), 2990 );
	signSelectGump.AddPicture( 45, 275, 2990 );

	signSelectGump.AddRadio( 98, 40, 9722, (iSign.id == 2992? 1 : 0), 2992 );
	signSelectGump.AddPicture( 120, 35, 2992 );
	signSelectGump.AddRadio( 98, 80, 9722, (iSign.id == 2994? 1 : 0), 2994 );
	signSelectGump.AddPicture( 120, 80, 2994 );
	signSelectGump.AddRadio( 98, 120, 9722, (iSign.id == 2996? 1 : 0), 2996 );
	signSelectGump.AddPicture( 120, 120, 2996 );
	signSelectGump.AddRadio( 98, 160, 9722, (iSign.id == 2998? 1 : 0), 2998 );
	signSelectGump.AddPicture( 120, 160, 2998 );
	signSelectGump.AddRadio( 98, 200, 9722, (iSign.id == 3000? 1 : 0), 3000 );
	signSelectGump.AddPicture( 120, 195, 3000 );
	signSelectGump.AddRadio( 98, 240, 9722, (iSign.id == 3002? 1 : 0), 3002 );
	signSelectGump.AddPicture( 120, 235, 3002 );
	signSelectGump.AddRadio( 98, 280, 9722, (iSign.id == 3004? 1 : 0), 3004 );
	signSelectGump.AddPicture( 120, 280, 3004 );

	signSelectGump.AddRadio( 178, 40, 9722, (iSign.id == 3006? 1 : 0), 3006 );
	signSelectGump.AddPicture( 200, 35, 3006 );
	signSelectGump.AddRadio( 178, 80, 9722, (iSign.id == 3008? 1 : 0), 3008 );
	signSelectGump.AddPicture( 200, 80, 3008 );
	signSelectGump.AddRadio( 178, 120, 9722, (iSign.id == 3010? 1 : 0), 3010 );
	signSelectGump.AddPicture( 200, 115, 3010 );
	signSelectGump.AddRadio( 178, 160, 9722, (iSign.id == 3012? 1 : 0), 3012 );
	signSelectGump.AddPicture( 200, 160, 3012 );
	signSelectGump.AddRadio( 178, 200, 9722, (iSign.id == 3014? 1 : 0), 3014 );
	signSelectGump.AddPicture( 200, 200, 3014 );
	signSelectGump.AddRadio( 178, 240, 9722, (iSign.id == 3016? 1 : 0), 3016 );
	signSelectGump.AddPicture( 200, 235, 3016 );
	signSelectGump.AddRadio( 178, 280, 9722, (iSign.id == 3018? 1 : 0), 3018 );
	signSelectGump.AddPicture( 200, 280, 3018 );

	signSelectGump.AddRadio( 258, 45, 9722, (iSign.id == 3020? 1 : 0), 3020 );
	signSelectGump.AddPicture( 280, 40, 3020 );
	signSelectGump.AddRadio( 258, 85, 9722, (iSign.id == 3022? 1 : 0), 3022 );
	signSelectGump.AddPicture( 280, 85, 3022 );
	signSelectGump.AddRadio( 258, 125, 9722, (iSign.id == 3024? 1 : 0), 3024 );
	signSelectGump.AddPicture( 280, 120, 3024 );
	signSelectGump.AddRadio( 258, 165, 9722, (iSign.id == 3026? 1 : 0), 3026 );
	signSelectGump.AddPicture( 280, 160, 3026 );
	signSelectGump.AddRadio( 258, 205, 9722, (iSign.id == 3028? 1 : 0), 3028 );
	signSelectGump.AddPicture( 280, 200, 3028 );
	signSelectGump.AddRadio( 258, 245, 9722, (iSign.id == 3030? 1 : 0), 3030 );
	signSelectGump.AddPicture( 280, 240, 3030 );
	signSelectGump.AddRadio( 258, 285, 9722, (iSign.id == 3032? 1 : 0), 3032 );
	signSelectGump.AddPicture( 280, 280, 3032 );

	signSelectGump.AddText( 265, 335, 0, "NEXT" );
	signSelectGump.AddButton( 305, 335, 4005, 4006, 0, 2, 0 );

	signSelectGump.AddPage( 2 );
	signSelectGump.AddRadio( 22, 40, 9722, (iSign.id == 3034 ? 1 : 0), 3034 );
	signSelectGump.AddPicture( 45, 35, 3034 );
	signSelectGump.AddRadio( 22, 80, 9722, (iSign.id == 3036 ? 1 : 0), 3036 );
	signSelectGump.AddPicture( 45, 75, 3036 );
	signSelectGump.AddRadio( 22, 120, 9722, (iSign.id == 3038 ? 1 : 0), 3038 );
	signSelectGump.AddPicture( 45, 115, 3038 );
	signSelectGump.AddRadio( 22, 160, 9722, (iSign.id == 3040 ? 1 : 0), 3040 );
	signSelectGump.AddPicture( 45, 155, 3040 );
	signSelectGump.AddRadio( 22, 200, 9722, (iSign.id == 3042 ? 1 : 0), 3042 );
	signSelectGump.AddPicture( 45, 195, 3042 );
	signSelectGump.AddRadio( 22, 240, 9722, (iSign.id == 3044 ? 1 : 0), 3044 );
	signSelectGump.AddPicture( 45, 235, 3044 );
	signSelectGump.AddRadio( 22, 280, 9722, (iSign.id == 3046 ? 1 : 0), 3046 );
	signSelectGump.AddPicture( 45, 275, 3046 );

	signSelectGump.AddRadio( 98, 40, 9722, (iSign.id == 3048 ? 1 : 0), 3048 );
	signSelectGump.AddPicture( 120, 35, 3048 );
	signSelectGump.AddRadio( 98, 80, 9722, (iSign.id == 3048 ? 1 : 0), 3048 );
	signSelectGump.AddPicture( 120, 75, 3048 );
	signSelectGump.AddRadio( 98, 120, 9722, (iSign.id == 3052 ? 1 : 0), 3052 );
	signSelectGump.AddPicture( 120, 115, 3052 );
	signSelectGump.AddRadio( 98, 160, 9722, (iSign.id == 3054 ? 1 : 0), 3054 );
	signSelectGump.AddPicture( 120, 155, 3054 );
	signSelectGump.AddRadio( 98, 200, 9722, (iSign.id == 3056 ? 1 : 0), 3056 );
	signSelectGump.AddPicture( 120, 195, 3056 );
	signSelectGump.AddRadio( 98, 240, 9722, (iSign.id == 3058 ? 1 : 0), 3058 );
	signSelectGump.AddPicture( 120, 235, 3058 );
	signSelectGump.AddRadio( 98, 280, 9722, (iSign.id == 3060 ? 1 : 0), 3060 );
	signSelectGump.AddPicture( 120, 275, 3060 );

	signSelectGump.AddRadio( 178, 40, 9722, (iSign.id == 3062 ? 1 : 0), 3062 );
	signSelectGump.AddPicture( 200, 35, 3062 );
	signSelectGump.AddRadio( 178, 80, 9722, (iSign.id == 3064 ? 1 : 0), 3064 );
	signSelectGump.AddPicture( 200, 75, 3064 );
	signSelectGump.AddRadio( 178, 120, 9722, (iSign.id == 3066 ? 1 : 0), 3066 );
	signSelectGump.AddPicture( 200, 115, 3066 );
	signSelectGump.AddRadio( 178, 160, 9722, (iSign.id == 3068 ? 1 : 0), 3068 );
	signSelectGump.AddPicture( 200, 155, 3068 );
	signSelectGump.AddRadio( 178, 200, 9722, (iSign.id == 3070 ? 1 : 0), 3070 );
	signSelectGump.AddPicture( 200, 195, 3070 );
	signSelectGump.AddRadio( 178, 240, 9722, (iSign.id == 3072 ? 1 : 0), 3072 );
	signSelectGump.AddPicture( 200, 235, 3072 );
	signSelectGump.AddRadio( 178, 280, 9722, (iSign.id == 3074 ? 1 : 0), 3074 );
	signSelectGump.AddPicture( 200, 275, 3074 );

	signSelectGump.AddRadio( 258, 45, 9722, (iSign.id == 3076 ? 1 : 0), 3076 );
	signSelectGump.AddPicture( 280, 40, 3076 );
	signSelectGump.AddRadio( 258, 85, 9722, (iSign.id == 3078 ? 1 : 0), 3078 );
	signSelectGump.AddPicture( 280, 80, 3078 );
	signSelectGump.AddRadio( 258, 125, 9722, (iSign.id == 3080 ? 1 : 0), 3080 );
	signSelectGump.AddPicture( 280, 120, 3080 );
	signSelectGump.AddRadio( 258, 165, 9722, (iSign.id == 3082 ? 1 : 0), 3082 );
	signSelectGump.AddPicture( 280, 160, 3082 );
	signSelectGump.AddRadio( 258, 205, 9722, (iSign.id == 3084 ? 1 : 0), 3084 );
	signSelectGump.AddPicture( 280, 205, 3084 );
	signSelectGump.AddRadio( 258, 245, 9722, (iSign.id == 3086 ? 1 : 0), 3086 );
	signSelectGump.AddPicture( 280, 245, 3086 );
	signSelectGump.AddRadio( 258, 285, 9722, (iSign.id == 3140 ? 1 : 0), 3140 );
	signSelectGump.AddPicture( 280, 285, 3140 );

	signSelectGump.AddText( 60, 335, 0, "PREV" );
	signSelectGump.AddButton( 19, 335, 4014, 4015, 0, 1, 0 );

	signSelectGump.Send( pUser.socket );
	signSelectGump.Free();
}