function SkillRegistration()
{
	RegisterSkill( 36, true );	// Taste Identification
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		// Fetch skill-specific skill delay to use on failure
		var pSkillDelay = Skills[36].skillDelay;
		pSock.CustomTarget( 0, GetDictionaryEntry( 6200, pSock.language )); // What would you like to taste?
		pSock.SetTimer( Timer.SOCK_SKILLDELAY, pSkillDelay * 1000 );
	}
	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ValidateObject( ourObj ) && ourObj.isItem && ValidateObject( pUser ))
	{
		if( ourObj.corpse )
		{
			pSock.SysMessage( GetDictionaryEntry( 6201, pSock.language )); // That's not something you can taste.
			return;
		}
		else if( ourObj.isItemHeld || ourObj.worldnumber != pUser.worldnumber || ourObj.instanceID != pUser.instanceID )
		{
			pSock.SysMessage( GetDictionaryEntry( 6202, pSock.language )); // You are too far away to taste that.
			return;
		}
		else if( !pUser.InRange( ourObj, 3 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 6202, pSock.language )); // You are too far away to taste that.
			return;
		}

		if( ourObj.type == 14 ) // Food Type
	    {
	    	// Player can gain skill up to 60.0 by taste identifying food
			if( pUser.CheckSkill( 36, 0, 600 ))
			{
				if( ourObj.poison )
				{
					var tempMsg = GetDictionaryEntry( 6203, pSock.language );
					pSock.SysMessage( tempMsg.replace( /%s/gi, ourObj.name )); // You sense a hint of foulness about %s.
				}
				else
                {
                    pSock.SysMessage( GetDictionaryEntry( 6204, pSock.language )); // You detect nothing unusual about this substance.
                }
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6205, pSock.language )); // You cannot discern anything about this substance.
				return;
			}
		}
		else if( ourObj.type == 19 ) // Potions
		{
			IdentifyPotion( pUser, ourObj, true );
		}
		else if( ourObj.sectionID == "potionkeg" ) // Potion Kegs
		{
			IdentifyPotionKeg( pUser, ourObj, true );
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 6201, pSock.language )); // That's not something you can taste.
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 2094, pSock.language )); // You feel that such an action would be inappropriate.
	}
	return;
}

function IdentifyPotion( pUser, iPotion, showInfo )
{
	var pSock = pUser.socket;

	// Player can gain skill up to 75.0 by taste identifying potions
	if( pUser.CheckSkill( 36, 0, 750 ))
	{
		TasteItem( pUser, iPotion, true );
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 6205, pSock.language )); // You cannot discern anything about this substance.
	}
}

function IdentifyPotionKeg( pUser, potionKeg )
{
	var pSock = pUser.socket;

	// Check if keg actually contains any potions
	if( potionKeg.GetTag( "potionCount" ) == 0 )
	{
		pSock.SysMessage( GetDictionaryEntry( 17224, pSock.language )); // There is nothing in the keg to taste!
	}
	else
	{
		// Player can gain skill up to 100.0 by taste identifying potion kegs
		if( pUser.CheckSkill( 36, 0, 1000 ))
		{
			TasteItem( pUser, potionKeg, true );
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 6205, pSock.language )); // You cannot discern anything about this substance.
		}
	}
}

function TasteItem( pUser, itemToTaste, showInfo )
{
	var pSock = pUser.socket;

	// Set up default message
	var tmpMsg = "";
	if( itemToTaste.sectionID == "potionkeg" )
	{
		// Potion Keg
		tmpMsg = GetDictionaryEntry( 17225, pSock.language ); // You are already familiar with this keg's contents.
	}
	else
	{
		// Potion
		tmpMsg = GetDictionaryEntry( 6215, pSock.language ); // You already know what kind of potion that is: %s
		tmpMsg.replace( /%s/gi, itemToTaste.name2 );
	}

	// Only check list of tasters if item has a secondary hidden name
	var listOfTasters = new Array();
	if( itemToTaste.name2 != "#" )
	{
		// Check if player is on list of tasters for item
		var listOfTastersTemp = itemToTaste.GetTag( "listOfTasters" )
		if( listOfTastersTemp.size > 0 )
		{
			listOfTasters = listOfTastersTemp.split( "," ).map( Number ); // convert to numbers
			if( !listOfTasters || listOfTasters.indexOf( pUser.serial ) == -1 )
			{
				tmpMsg = ""; // Player has not taste identified item, clear default message
			}
		}
		else
		{
			tmpMsg = ""; // Player has not taste identified item, clear default message
		}
	}

	if( tmpMsg != "" )
	{
		// Default message intact, player already knows what item tastes like
		if( showInfo )
		{
			pSock.SysMessage( tmpMsg );
		}
	}
	else
	{
		// Since user has not already identified item, add them to list
		if( listOfTasters.length >= 5 )
		{
			// Remove the oldest entry in the list - max 5 people are tracked at any given time
			listOfTasters.shift();
		}

		listOfTasters.push( pUser.serial );
		itemToTaste.SetTag( "listOfTasters", listOfTasters.toString() );

		// Refresh potion so tooltip is updated
		itemToTaste.Refresh();
	}

	if( showInfo && itemToTaste.type == 19 ) // Potions
	{
		var potionIngredients = "";
		switch( itemToTaste.id )
		{
			case 0x0f06: // Black Potion
				potionIngredients = GetDictionaryEntry( 6206, pSock.language ); // This potion may have been made from garlic.
				break;
			case 0x0f07: // Orange Potion
				potionIngredients = GetDictionaryEntry( 6207, pSock.language ); // This potion may have been made from spider silk.
				break;
			case 0x0f08: // Blue Potion
				potionIngredients = GetDictionaryEntry( 6208, pSock.language ); // This potion may have been made from bloodmoss.
				break;
			case 0x0f09: // White Potion
				potionIngredients = GetDictionaryEntry( 6209, pSock.language ); // This potion may have been made from mandrake.
				break;
			case 0x0f0a: // Green Potion
				var tempMsg = GetDictionaryEntry( 6203, pSock.language ); // You sense a hint of foulness about %s.
				pSock.SysMessage( tempMsg.replace( /%s/gi, itemToTaste.name ));
				potionIngredients = GetDictionaryEntry( 6210, pSock.language ); // This potion may have been made from nightshade.
				break;
			case 0x0f0b: // Red Potion
				potionIngredients = GetDictionaryEntry( 6211, pSock.language ); // This potion may have been made from black pearl.
				break;
			case 0x0f0c: // Yellow Potion
				potionIngredients = GetDictionaryEntry( 6212, pSock.language ); // This potion may have been made from ginseng.
				break;
			case 0x0f0d: // Purle Potion
				potionIngredients = GetDictionaryEntry( 6213, pSock.language ); // This potion may have been made from sulfurous ash.
				break;
			default:
				pSock.SysMessage( GetDictionaryEntry( 6214, pSock.language )); // This potion's ingredients are unknown to you.
				return;
		}

		pSock.SysMessage( "The potion tastes familiar to you." ); // The potion tastes familiar to you.
		pSock.SysMessage( potionIngredients );
		pSock.SysMessage( itemToTaste.name2 );
	}
	else if( showInfo && itemToTaste.sectionID == "potionkeg" )
	{
		pSock.SysMessage( itemToTaste.name2 );
	}
}