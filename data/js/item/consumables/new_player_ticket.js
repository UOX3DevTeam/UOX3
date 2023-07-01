// This script handles New Player Tickets given to all newly created characters on Young accounts

// Rewards enabled/disabled based on CoreShardEra setting in UOX.INI
const rangerArmorReward = ( EraStringToNum( GetServerSetting( "CoreShardEra" )) <= EraStringToNum( "t2a" )); // T2A and earlier
const spellBookReward = ( EraStringToNum( GetServerSetting( "CoreShardEra" )) >= EraStringToNum( "uor" )); // UOR and later

// Modify these to override above settings from CoreShardEra
const rangerArmorOverride = false;
const spellBookRewardOverride = false;

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket == null || !ValidateObject( iUsed ))
		return false;


	var itemPack = GetPackOwner( iUsed, 0 );
	if( iUsed.owner != pUser || parseInt( iUsed.GetTag( "ticketUsableBy" )) != pUser.serial )
	{
		pUser.SysMessage( GetDictionaryEntry( 18747, socket.language )); // This isn't your ticket! Shame on you! You have to use YOUR ticket.
	}
	else if( itemPack == null || itemPack.serial != pUser.serial )
	{
		pUser.SysMessage( GetDictionaryEntry( 18748, socket.language )); // That must be in your pack for you to use it.
	}
	else
	{
		// Keep a reference to the ticket used
		pUser.SetTempTag( "youngTicketSerial", ( iUsed.serial ).toString() );
		socket.tempObj = iUsed;
		pUser.socket.CustomTarget( 0, GetDictionaryEntry( 18749, socket.language )); // Target any other ticket marked NEW PLAYER to win a prize.
	}
	return false;
}

function onCallback0( socket, myTarget )
{
	if( socket == null )
		return;

	var newPlayerTicket = socket.tempObj;
	if( !ValidateObject( newPlayerTicket ))
		return;

	var itemOwner = GetPackOwner( newPlayerTicket, 0 );
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( !socket.GetWord( 1 ))
	{
		if( ValidateObject( myTarget ) && myTarget.isItem )
		{
			if( myTarget == newPlayerTicket )
			{
				// Don't allow player targeting the same ticket
				socket.SysMessage( GetDictionaryEntry( 18750, socket.language ));// You can't target the same ticket!
			}
			else if( itemOwner == null || itemOwner.serial != pUser.serial )
			{
				// Don't allow using a ticket outside of backpack
				pUser.SysMessage( GetDictionaryEntry( 18751, socket.language )); // The item must be in your backpack to use it.
				return;
			}
			else if( myTarget.sectionID == "newplayerticket" )
			{
				TicketGump( socket, pUser, newPlayerTicket ); // Send gump to the user initiating the exchange

				// Fetch reference to who owns other ticket
				var otherUser = CalcCharFromSer( parseInt( myTarget.GetTag( "ticketUsableBy" )));
				if( !ValidateObject( otherUser ) || otherUser.socket == null )
					return;

				otherUser.SetTempTag( "youngTicketSerial", ( myTarget.serial).toString() );
				TicketGump( otherUser.socket, otherUser, myTarget );// Send gump to other player
			}
			else if( myTarget.id == 0x14F0 ) // trying to target any other item that looks like a new player ticket
			{
				socket.SysMessage( GetDictionaryEntry( 18752, socket.language )); // You need to find another ticket marked NEW PLAYER.
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 18753, socket.language )); // You will need to select a ticket.
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 18754, socket.language )); // That is not a valid ticket.
			return;
		}
	}

	return;
}

function TicketGump( socket, pUser, newPlayerTicket )
{
	var ticketGump = new Gump();

	var baseGumpHeight = 175;
	var numRewards = 6;
	if(( rangerArmorReward || rangerArmorOverride ) && ( spellBookReward || spellBookRewardOverride ))
	{
		numRewards = 7;
	}
	baseGumpHeight += ( numRewards * 35 );

	var yOffsetGump = 130;
	var yOffsetBtn = 128;
	ticketGump.AddPage( 0 );
	ticketGump.AddBackground( 0, 0, 400, baseGumpHeight, 0xA28 );
	ticketGump.AddHTMLGump( 30, 45, 340, 70, false, false, GetDictionaryEntry( 18755, socket.language )); // Choose the gift you prefer. WARNING: if you cancel, and your partner does not, you will need to find another matching ticket!

	ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 1 );
	ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18756, socket.language )); // A sextant

	yOffsetBtn += 35;
	yOffsetGump += 35;
	ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 2 );
	ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18757, socket.language )); // A coupon for a single hair restyling

	yOffsetBtn += 35;
	yOffsetGump += 35;
	if(( rangerArmorReward || rangerArmorOverride ) && ( spellBookReward || spellBookRewardOverride ))
	{
		// Both ranger armor and spellbook rewards (custom)
		ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 7 );
		ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18771, socket.language )); // A full set of ranger armor.

		yOffsetBtn += 35;
		yOffsetGump += 35;
		ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 3 );
		ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18758, socket.language )); // A spellbook with all 1st - 4th spells.
	}
	else if(( rangerArmorReward || rangerArmorOverride ) && ( !spellBookReward && !spellBookRewardOverride ))
	{
		// Ranger Armor only (T2A and earlier)
		ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 7 );
		ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18771, socket.language )); // A full set of ranger armor.
	}
	else
	{
		// Spellbook only (UOR and later)
		ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 3 );
		ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18758, socket.language )); // A spellbook with all 1st - 4th spells.
	}

	yOffsetBtn += 35;
	yOffsetGump += 35;
	ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 4 );
	ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18759, socket.language )); // A wand of fireworks

	yOffsetBtn += 35;
	yOffsetGump += 35;
	ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 5 );
	ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18760, socket.language )); // A spyglass

	yOffsetBtn += 35;
	yOffsetGump += 35;
	ticketGump.AddButton( 46, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 6 );
	ticketGump.AddHTMLGump( 80, yOffsetGump, 320, 35, false, false, GetDictionaryEntry( 18761, socket.language )); // Dyes and a dye tub

	yOffsetBtn += 37;
	yOffsetGump += 37;
	ticketGump.AddButton( 120, yOffsetBtn, 0xFA5, 0xFA7, 1, 0, 0 );
	ticketGump.AddHTMLGump( 154, yOffsetGump, 100, 35, false, false, GetDictionaryEntry( 18762, socket.language )); // CANCEL

	ticketGump.Send( pUser );
	ticketGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	if( socket == null || !ValidateObject( pUser ))
		return;

	var newPlayerTicket = CalcItemFromSer( parseInt( pUser.GetTempTag( "youngTicketSerial" )));
	if( !ValidateObject( newPlayerTicket ))
		return;

	var sysMsg = "";
	var freeItem1 = "";
	var freeItem2 = "";
	switch( pButton )
	{
		case 0: // close gump
			return;
		case 1: // sextant
			freeItem1 = "sextant";
			sysMsg = GetDictionaryEntry( 18763, socket.language ); // A sextant has been placed in your backpack.
			break;
		case 2: // hair restyling coupon
			freeItem1 = "hairrestyledeed";
			sysMsg = GetDictionaryEntry( 18764, socket.language ); // A coupon for a free hair restyling has been placed in your backpack.
			break;
		case 3: // Spellbook pre-filled with 1st to 4th circle spells
			freeItem1 = "spellbook1to4";
			sysMsg = GetDictionaryEntry( 18766, socket.language ); // A spellbook with all 1st to 4th circle spells has been placed in your backpack.
			break;
		case 4: // Wand of fireworks
			freeItem1 = "fireworks_wand";
			sysMsg = GetDictionaryEntry( 18767, socket.language ); // A wand of fireworks has been placed in your backpack.
			break;
		case 5: // Spyglass
			freeItem1 = "0x14F5";
			sysMsg = GetDictionaryEntry( 18768, socket.language ); // A spyglass has been placed in your backpack.
			break;
		case 6: // Dyes and dye tub
			freeItem1 = "0x0fab";
			freeItem2 = "0x0fa9";
			sysMsg = GetDictionaryEntry( 18769, socket.language ); // The dyes and dye tub have been placed in your backpack.
			break;
		case 7:	// Ranger armor set
			freeItem1 = "rangerArmorSet";
			sysMsg = sysMsg = GetDictionaryEntry( 18765, socket.language ); // A bag with a set of Ranger armor has been placed in your backpack.
			break;
		default:
			break;
	}

	socket.SysMessage( sysMsg );
	var newItem1 = CreateDFNItem( socket, pUser, freeItem1, 1, "ITEM", true );
	if( freeItem2 != "" )
	{
		var newItem2 = CreateDFNItem( socket, pUser, freeItem2, 1, "ITEM", true );
	}

	// Delete the new player ticket!
	newPlayerTicket.Delete();
}

function onTooltip( iUsed, pSocket )
{
	var tooltipText = "";
	tooltipText = GetDictionaryEntry( 18770, pSocket.language ); // This is half a prize ticket! Double-click this ticket and target any other ticket marked NEW PLAYER and get a prize! This ticket will only work for YOU, so don’t give it away!
	return tooltipText;
}