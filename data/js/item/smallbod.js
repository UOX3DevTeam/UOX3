// Can also be triggered by creating BOD from admin menu or add command: 'add item smallbod
function onCreateDFN( objMade, objType )
{
	if( !ValidateObject( objMade ))
		return;

	var bodEntry 	= TriggerEvent( 3214, "SelectBodEntry", null, false, 3 ); // 1 - weapon BODs, 2 - armor BODs, 3 - Either
	var itemName	= bodEntry.name; // name of the create entry
	var graphicID	= bodEntry.id; // graphical id of item to craft
	var sectionID 	= bodEntry.addItem; // section ID of item to craft
	var materialColor = bodEntry.resources[0][1]; // colour of primary resource required to craft item

	var init		= objMade.GetTag( "init" )
	if( objType == 0 && init == false )
	{
		objMade.SetTag( "itemName", itemName );
		objMade.SetTag( "graphicID", graphicID );
		objMade.SetTag( "sectionID", sectionID );
		objMade.SetTag( "materialColor", materialColor );
		objMade.Refresh();
	}
}

function onUseChecked( pUser, smallBOD )
{
	var socket = pUser.socket;
	if( socket == null )
		return false;

	if( pUser.InRange( pUser, 2 ))
	{
		// Check to make sure you are not invisible.
		if( pUser.visible == 1 || pUser.visible == 2 )
		{
			pUser.visible = 0;
		}

		// Check to see if it's locked down
		if( smallBOD.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); //That is locked down and you cannot use it
			return false;
		}
		else 
		{
			// Store iUsed as a custom property on pUser
			pUser.bodItem = smallBOD;

			var init = smallBOD.GetTag( "init" ) // just to make sure we dont set the bod over
			if( init == false ) // Keep from resetting the amount needed.
			{
				var amountMax = 0;
				if( pUser.skills.blacksmithing >= 701 )
				{
					if((( pUser.skills.blacksmithing + 800 ) / 2 ) > RandomNumber( 0, 1000 ))
					{
						smallBOD.SetTag( "reqExceptional", true );
					}

					var values = [ 10, 15, 20, 20 ];
					amountMax = values[Math.floor( Math.random() * values.length )];
				}
				else if( pUser.skills.blacksmithing >= 501 )
				{					
					var values = [ 10, 15, 15, 20 ];
					amountMax = values[Math.floor( Math.random() * values.length )];
				}
				else
				{
					var values = [ 10, 10, 15, 20 ];
					amountMax = values[Math.floor( Math.random() * values.length )];					
				}
				smallBOD.SetTag( "init", true );
				smallBOD.SetTag( "amountMax", amountMax );
			}

			smallBOD.Refresh();
			SmallBODGump( pUser, smallBOD );
		}
	}
}

function SmallBODGump( pUser, smallBOD )
{
	var amountMax 		= smallBOD.GetTag( "amountMax" ); // amount you have to make of the item
	var amountCur 		= smallBOD.GetTag( "amountCur" ); // amount you have combined to the bod
	var itemName 		= smallBOD.GetTag( "itemName" ); // itemName of the item set when item is created
	var graphicID 		= smallBOD.GetTag( "graphicID" ); // graphicID of the item set when item is created
	var reqExceptional 	= smallBOD.GetTag( "reqExceptional" ); // flag that specifies whether item has to be exceptional or not
	var materialColor	= smallBOD.GetTag( "materialColor" ); // color of primary resource required to craft item
	var socket = pUser.socket;
	var bodGump = new Gump;

	bodGump.AddPage( 0 );

	bodGump.AddBackground( 50, 10, 455, 260, 5054 );
	bodGump.AddTiledGump( 58, 20, 438, 241, 2624 );
	bodGump.AddCheckerTrans( 58, 20, 438, 241 );
	bodGump.AddGump( 45, 5, 10460 );
	bodGump.AddGump( 480, 5, 10460 );
	bodGump.AddGump( 45, 245, 10460 );
	bodGump.AddGump( 480, 245, 10460 );

	bodGump.AddHTMLGump( 225, 25, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17250, socket.language ) + "</basefont>" ); // A bulk order

	bodGump.AddHTMLGump( 75, 48, 250, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17251, socket.language ) + "</basefont>"); // Amount to make:
	bodGump.AddText( 275, 48, 1152, amountMax );

	bodGump.AddHTMLGump( 275, 76, 200, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17252, socket.language ) + "</basefont>"); // Amount finished:
	bodGump.AddText( 275, 96, 0x480, amountCur );

	bodGump.AddHTMLGump( 75, 72, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17253, socket.language ) + "</basefont>" ); // Item requested:

	if( reqExceptional )
	{
		bodGump.AddHTMLGump( 75, 96, 210, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17254, socket.language ) + " " + itemName + "</basefont>");// exceptional
	}
	else
	{
		bodGump.AddHTMLGump( 75, 96, 210, 20, 0, 0, "<basefont color=#ffffff>" + itemName + "</basefont>");
	}

	bodGump.AddPicture( 410, 72, graphicID );// image of the item requested
	if( reqExceptional || materialColor > 0 )
	{
		bodGump.AddHTMLGump( 75, 120, 200, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17255, socket.language ) + "</basefont>" ); // Special requirements to meet:
	}

	if( reqExceptional )
	{
		bodGump.AddHTMLGump( 75, 144, 300, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17256, socket.language ) + "</basefont>"); // All items must be exceptional.
	}

	if( materialColor > 0 ) // checks to see if a material is required for the bod.
	{
		if( reqExceptional )
		{
			var y = 168;
		}
		else
		{
			var y = 144;
		}
		var materialName = "";
		switch( materialColor )
		{
			case 0:
				materialName = "iron";
				break;
			case 2419:
				materialName = "dull copper";
				break;
			case 2406:
				materialName = "shadow iron";
				break;
			case 2414:
				materialName = "copper";
				break;
			case 1750:
				materialName = "bronze";
				break;
			case 2213:
				materialName = "gold";
				break;
			case 2425:
				materialName = "agapite";
				break;
			case 2207:
				materialName = "verite";
				break;
			case 2219:
				materialName = "valorite";
				break;
			default:
				materialName = "iron";
				break;
		}
		if( materialName != "" )
		{
			bodGump.AddHTMLGump( 75, y, 300, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17257, socket.language ) + " " + materialName + " material</basefont>" ); // All items must be made with 
		}
	}

	bodGump.AddButton( 125, 192, 4005, 4007, 1, 0, 1 );
	bodGump.AddHTMLGump( 160, 192, 300, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17258, socket.language ) + "</basefont>" ); // Combine this deed with the item requested.

	bodGump.AddButton( 125, 216, 4005, 4007, 1, 0, 0 );
	bodGump.AddHTMLGump( 160, 216, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10290, socket.language ) + "</basefont>" ); // EXIT

	bodGump.Send( pUser );
	bodGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var smallBOD = pUser.bodItem;
	switch( pButton )
	{
		case 0:// Exit
			delete pUser.bodItem; // Remove bodItem as a temporary property on pUser
			break;
		case 1:
			CombineItemWithBod( pUser, smallBOD );
			break;
	}
}

function CombineItemWithBod( pUser, smallBOD )
{
	var socket = pUser.socket;
	var amountMax = smallBOD.GetTag( "amountMax" ); // amount you have to make of the item
	var amountCur = smallBOD.GetTag( "amountCur" ); // amount you have combined

	if( amountCur >= amountMax )
	{
		pUser.TextMessage( GetDictionaryEntry( 17260, socket.language ), false, 0x3b2, 0, pUser.serial ); // The maximum amount of requested items have already been combined to this deed.
		return;
	}
	else
	{
		pUser.CustomTarget( 0 );
		return;
	}
}


function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	var smallBOD = pUser.bodItem;
	var bodSectionID 	= smallBOD.GetTag( "bodSectionID" ); // sectionID of item required by BOD
	var amountMax 		= smallBOD.GetTag( "amountMax" ); // amount you have to make of the item
	var amountCur 		= smallBOD.GetTag( "amountCur" ); // amount you have combined
	var reqExceptional 	= smallBOD.GetTag( "reqExceptional" );
	var materialColor 	= smallBOD.GetTag( "materialColor" );
	var bodItemName 	= smallBOD.GetTag( "itemName" );
	var bodItemID 		= smallBOD.GetTag( "graphicID" );
	delete pUser.bodItem; // Remove bodItem as a temporary property on pUser

	// Abort if player cancels target cursor
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	if( !socket.GetWord( 1 ) && myTarget.isItem && ( myTarget.sectionID == bodSectionID ) || ( myTarget.id == bodItemID && myTarget.name == bodItemName ))
	{
		// That must be in your pack for you to use it.
		var itemOwner = GetPackOwner( myTarget, 0 );
		if( itemOwner == null || itemOwner != pUser ) 
		{
			socket.SysMessage( GetDictionaryEntry( 17259, socket.language )); // You must have the item in your backpack to target it.
			return;
		}
		else if( amountCur >= amountMax )
		{
			pUser.TextMessage( GetDictionaryEntry( 17260, socket.language ), false, 0x3b2, 0, pUser.serial ); // The maximum amount of requested items have already been combined to this deed.
			return;
		}
		else if( materialColor != myTarget.colour )
		{
			socket.SysMessage( GetDictionaryEntry( 17261, socket.language )); // The item is not made from the requested ore.
			return;
		}
		else
		{
			if( reqExceptional && myTarget.rank != 10 )
			{
				socket.SysMessage( GetDictionaryEntry( 17262, socket.language )); // The item must be exceptional.
				SmallBODGump( pUser, smallBOD );
				return;
			}
			else 
			{
				smallBOD.SetTag( "amountCur", amountCur + 1 );

				// Continuously update the gold and fame reward values of the BOD with each item added to it:
				var fameValue = smallBOD.GetTag( "fameValue" );
				var goldValue = smallBOD.GetTag( "goldValue" );
				var qualityValue = smallBOD.GetTag( "qualityValue" );

				// Add to the gold value of the BOD with each crafted item added to the BOD,
				// based on the sell-value of the item modified by the rank system
				var targetGoldValue = myTarget.sellvalue;
				var targetFameValue = myTarget.sellvalue;
				var targetQualityValue = myTarget.rank;
				if( GetServerSetting( "RankSystem" ))
				{
					if( myTarget.rank > 0 && myTarget.rank < 10 )
					{
						// If rank is lower than exceptional, use sell value divided by 2, modified based on rank
						targetGoldValue = Math.round(( myTarget.rank * ( targetGoldValue / 2 )) / 10 );

						// For fame, divide by 2.75 instead, and again by 10 at the end
						targetFameValue = Math.round((( myTarget.rank * ( targetFameValue / 2.75 )) / 10 ) / 10 );
					}
					else
					{
						// If rank is exceptional, use sell value divided by 1.5 (bonus gold)
						targetGoldValue = Math.round( targetGoldValue / 1.5 );

						// For fame, divide by 2.5 instead,
						targetFameValue = Math.round(( targetFameValue / 2.5 ) / 10 );
					}
				}
				else
				{
					// Rank system not enabled
					targetQualityValue = 10; // Default is best quality without rank system
					targetGoldValue = Math.round( targetGoldValue / 2 );
					targetFameValue = Math.round(( targetFameValue / 2.5 ) / 10 );
				}

/*				// From stratics archive: https://web.archive.org/web/20021002074641/http://uo.stratics.com/content/reputation/bulkorders.shtml
				// Every bulk order weapon deed gives "a lot" of fame regardless of your level. This means
				// that if you are eminent you get the same fame from 15 normal daggers as from killing a dragon.
				// If you already have a little fame, it takes 2 weapons bods to get to level 2 fame.
				// 		From level 2 to level 3 : 16 bulk order deeds
				//		From level 3 to level 4 : 21 bulk order deeds
				//		From level 4 to level 5 : 34 bulk order deeds
				// 		5 - 5000, 4 - 2500, 3 - 1250, 2 - 675, 1 - 0

				// The following setup grants fame value per item added to BOD based on
				// the player's existing fame and an estimated amount of BOD hand-ins expected
				// to go from one fame level to the next. In this case, the player would need to
				// hand in 223 completed BODs to reach max fame level
				if( pDropper.fame < 675 )
				{
					targetFameValue = 675 / 16 / 20;
				}
				else if( pDropper.fame < 1250 )
				{
					targetFameValue = 1250 / 21 / 20;
				}
				else if( pDropper.fame < 2500 )
				{
					targetFameValue = 2500 / 34 / 20;
				}
				else if( pDropper.fame < 5000 )
				{
					targetFameValue = 5000 / 57 / 20;
				}
				else if( pDropper.fame < 10000 )
				{
					targetFameValue = 10000 / 95 / 20;
				}
*/
				smallBOD.SetTag( "goldValue", goldValue + targetGoldValue );
				smallBOD.SetTag( "fameValue", fameValue + targetFameValue );
				smallBOD.SetTag( "qualityValue", qualityValue + targetQualityValue );
				smallBOD.Refresh();

				myTarget.Delete();
				pUser.TextMessage( GetDictionaryEntry( 17263, socket.language ), false, 0x3b2, 0, pUser.serial ); // The item has been combined with the deed.
				if( amountCur < amountMax )
				{
					pUser.CustomTarget( 0 );
					SmallBODGump( pUser, smallBOD );
				}
			}
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 17264, socket.language )); // The item is not in the request.
		SmallBODGump( pUser, smallBOD );
	}
}

function onTooltip( smallBOD )
{
	var tooltipText = "";
	var amountMax = smallBOD.GetTag( "amountMax" ); // amount you have to make of the item
	var amountCur = smallBOD.GetTag( "amountCur" ); // amount you have combined
	var itemName = smallBOD.GetTag( "itemName" ); // name of the item set when item is created
	if( amountMax == 0 || itemName == 0 )
		return "";

	tooltipText = "amount to make : " + amountMax;
	tooltipText += "\n" + itemName + " : " + amountCur;
	return tooltipText;
}