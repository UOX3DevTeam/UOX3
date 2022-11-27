// This script handles functionality of potion kegs
const maxPotion = 100; // Max potions in a potion keg
const limitLockedDownKegUsage = false; // If set to true, only owner/co-owners/friends can use locked down potion kegs
const limitLockedDownPotionDrop = false; // If set to true, only owner/co-owner/friends can drop potions on locked down potion kegs

function onUseChecked( pUser, potionKeg )
{
	var socket = pUser.socket;
	var potionCount = parseInt( potionKeg.GetTag( "potionCount" ));
	if( pUser.InRange( pUser, 2 ) && ( potionKeg.container != null || pUser.CanSee( potionKeg )))
	{
		// Check to see if it's locked down
		if( potionKeg.movable == 3 && limitLockedDownKegUsage )
		{
			var iMulti = potionKeg.multi;
			if( ValidateObject( iMulti ))
			{
				if( !iMulti.IsOnFriendList( pUser ) && !iMulti.IsOnOwnerList( pUser ))
				{
					socket.SysMessage( GetDictionaryEntry( 774, socket.language )); // That is locked down and you cannot use it
					return false;
				}
			}
		}

		// Check to make sure you are not invisible.
		if( pUser.visible == 1 || pUser.visible == 2 )
		{
			pUser.visible = 0;
		}

		// Checking potion Count
		if( potionCount > 0 )
		{
			var emptyBottles = pUser.ResourceCount( 0x0f0e ); // Checking Make sure we have a bottle.
			if( emptyBottles > 0 )
			{
				var potionSectionID = potionKeg.GetTag( "potionSectionID" );
				var newPotion = CreateDFNItem( socket, pUser, potionSectionID, 1, "ITEM", true );
				if( ValidateObject( newPotion )) // Always validate that the item got created properly
				{
					socket.SysMessage( GetDictionaryEntry( 17200, socket.language )); // You pour some of the keg's contents into an empty bottle...

					// Reduce potionCount in keg
					var potionCount = parseInt( potionKeg.GetTag( "potionCount" )) - 1;
					potionKeg.SetTag( "potionCount", potionCount );
					pUser.UseResource( 1, 0x0f0e );
					socket.SysMessage( GetDictionaryEntry( 17201, socket.language )); // ...and place it into your backpack.
					pUser.SoundEffect( 0x240, true );
					potionKeg.weight -= Math.round(( newPotion.weight / 5 ) * 4 ); // Four fifths of a potion's weight

					// Identify potion if player is owner of keg, or if they know (via taste id) the contents of keg
					if(( ValidateObject( potionKeg.owner ) && potionKeg.owner == pUser && potionKeg.GetTag( "idByOwner" )) || HasUserTastedItem( pUser, potionKeg ))
					{
						TriggerEvent( 819, "IdentifyPotion", pUser, newPotion, false ); // false flag = Don't output details of identification to player
					}

					if( potionCount == 0 )
					{
						// Keg is now empty
						potionKeg.name = "keg specially lined for potions";
						potionKeg.name2 = "#"; // no more mystery

						// Reset tags on keg
						potionKeg.SetTag( "potionSectionID", null );
						potionKeg.SetTag( "idByOwner", null );
						potionKeg.SetTag( "listOfTasters", null );
					}
					potionKeg.Refresh();
				}
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 17202, socket.language )); // you have no empty bottles.
			}
		}
		else 
		{
			socket.SysMessage( GetDictionaryEntry( 17211, socket.language )); // The keg is empty.
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 17204, socket.language )); // I can't reach that.
	}
	return false;
}

function HasUserTastedItem( pUser, potionItem )
{
	var listOfTastersTemp = potionItem.GetTag( "listOfTasters" );
	if( listOfTastersTemp.length > 0 )
	{
		listOfTasters = listOfTastersTemp.split( "," ).map( Number );
		if( listOfTasters && listOfTasters.indexOf( pUser.serial ) != -1 )
		{
			return true;
		}
	}
	return false;
}

function onDropItemOnItem( iDropped, pUser, potionKeg )
{
	var socket = pUser.socket;
	if( iDropped.type != 19 ) // prevent script from running when moving the potion keg itself
	{
		if( iDropped.sectionID != "potionkeg" )
		{
			pUser.TextMessage( GetDictionaryEntry( 17210, socket.language ), false, 0x03b2 ); // The keg is not designed to hold that type of object.
		}
		return 1;
	}

	var potionCount = parseInt( potionKeg.GetTag( "potionCount" ));
	var potionSectionID = potionKeg.GetTag( "potionSectionID" );

    // Make sure there's space in the potionKeg for the new potion
	if( potionCount >= maxPotion )
	{
		socket.SysMessage( GetDictionaryEntry( 17206, socket.language )); // The keg will not hold any more!
		return 0;
	}
	else if( potionKeg.sectionID == "potionKeg" && potionKeg.movable == 3 ) // Locked down
	{
		if( potionCount == 0 || limitLockedDownPotionDrop )
		{
			// Only house owner, co-owners and friends can fill an empty, locked-down potion keg
			var iMulti = potionKeg.multi;
			if( ValidateObject( iMulti ))
			{
				if( !iMulti.IsOnFriendList( pUser ) && !iMulti.IsOnOwnerList( pUser ))
				{
					// Player is not a friend nor an owner/co-owner
					socket.SysMessage( GetDictionaryEntry( 17205, socket.language )); // Only the owner can drop potions in this keg.
					return 0;
				}
			}
		}
		else if(( ValidateObject( potionKeg.owner ) && potionKeg.owner != pUser ) && !HasUserTastedItem( pUser, potionKeg ) && !pUser.isGM )
		{
			// Anyone can continue filling up a partially filled locked-down potion keg, as long as they
			// A) are the owner of the keg or B) have used Taste ID to identify contents of keg
			socket.SysMessage( GetDictionaryEntry( 17205, socket.language )); // Only the owner can drop potions in this keg.
			return 0;
		}
	}

	if( iDropped.type == 19 ) // If the item being dropped is a potion
	{
		if( potionSectionID == "" || iDropped.sectionID == potionSectionID ) // Check to see if it is the same potion.
		{
			// Check that the player's backpack can contain a empty bottle
			var pPack = pUser.pack;
			if( pPack.totalItemCount >= pPack.maxItems || pPack.weight >= pPack.weightMax ) 
			{
				socket.SysMessage( GetDictionaryEntry( 17207, socket.language )); // You don't have room for the empty bottle in your backpack.
				return 0;
			}
			else if( iDropped.name2 != "#" && potionCount > 0 && potionKeg.name2 == "#" )
			{
				socket.SysMessage( "Unidentified potions can only be placed in unidentified potion kegs" ); // Unidentified potions can only be placed in unidentified potion kegs
				return 0;
			}
			else if( iDropped.name2 == "#" && potionCount > 0 && potionKeg.name2 != "#" )
			{
				socket.SysMessage( "Identified potions can only be placed in identified potion kegs" ); // Identified potions can only be placed in identified potion kegs
				return 0;
			}
			else
			{
				if( potionCount == 0 )
				{
					// Default keg as not identified when first potion is dropped in
					potionKeg.SetTag( "idByOwner", null );

					// Was the potion dropped on the keg already been identified?
					if( HasUserTastedItem( pUser, iDropped ))
					{
						// Potion was identified, tag keg as identified too!
						potionKeg.SetTag( "idByOwner", true );
					}

					// Potion keg is empty, let's add some identifiers for what potion was added
					potionKeg.name = "a keg of " + iDropped.name;
					if( iDropped.name2 != "#" )
					{
						// Set real name of keg to match real name of unidentified potion
						var shardEra = GetServerSetting( "CoreShardEra" );
						if( shardEra != "t2a" && shardEra != "uor" && shardEra != "td" && shardEra != "lbr" )
						{
							// In eras newer than LBR, true name of keg contents always shows
							potionKeg.name = "a keg of " + iDropped.name2;
						}
						potionKeg.name2 = potionKeg.name;
					}
					potionKeg.SetTag( "potionSectionID", iDropped.sectionID );
				}

				// Add potion to keg and delete the bottle.
				potionKeg.SetTag( "potionCount", potionCount + 1 );
				potionKeg.weight += Math.round(( iDropped.weight / 5 ) * 4 ); // Four fifths of potion weight

				if( iDropped.amount > 1 )
				{
					iDropped.amount = ( iDropped.amount - 1 );
				}
				else
				{
					iDropped.Delete();
				}

				// Creates a empty bottle make the potion keg be owned by the dropper and now refresh keg.
				var emptyBottle = CreateDFNItem( socket, pUser, "0x0f0e", 1, "ITEM", true )
				if( ValidateObject( emptyBottle ))
				{
					pUser.TextMessage( GetDictionaryEntry( 17208, socket.language ), false, 0x03b2 ); // You place the empty bottle in your backpack.
					pUser.SoundEffect( 0x240, true );

					// Only change owner if the keg is not locked down
					if( potionKeg.movable != 3 )
					{
						potionKeg.owner = pUser;
					}
					potionKeg.Refresh();
				}
				return 2;
			}
		}
		else 
		{
			pUser.TextMessage( GetDictionaryEntry( 17209, socket.language ), false, 0x03b2 ); // You decide that it would be a bad idea to mix different types of potions.
			return 0;
		}
	}
	return 1;
}

// Show real name to owner, and to those who have identified potion keg
function onNameRequest( potionKeg, pUser )
{
	// Default name
	var nameString = potionKeg.name;

	var shardEra = GetServerSetting( "CoreShardEra" );
	if( shardEra == "t2a" || shardEra == "uor" || shardEra == "td" || shardEra == "lbr" )
	{
		// Prior to Publish 15, players only see true contents of potion kegs if they're the owner, or have
		// used taste identification on it
		if( potionKeg.name2 != "#" )
		{
			if( ValidateObject( potionKeg.owner ) && potionKeg.owner == pUser )
			{
				nameString = potionKeg.name2;
			}
			else if( HasUserTastedItem( pUser, potionKeg ))
			{
				// Show identified name of potion
				nameString = potionKeg.name2;
			}
			else
			{
				switch( potionKeg.GetTag( "potionSectionID" ).toUpperCase() )
				{
					case "0X0F0D":
					case "LESSEREXPLOSION":
					case "0X0F0D-B":
					case "EXPLOSIONPOTION":
					case "0X0F0D-C":
					case "GREATEREXPLOSIONPOTION":
						nameString = "a keg of purple liquid";
						break;
					case "0X0F0B":
					case "REFRESHMENTPOTION":
					case "0X0F0B-B":
					case "TOTALREFRESHMENTPOTION":
						nameString = "a keg of red liquid";
						break;
					case "0X0F06":
					case "NIGHTSIGHTPOTION":
						nameString = "a keg of black liquid";
						break;
					case "0X0F07":
					case "LESSERCUREPOTION":
					case "0X0F07-B":
					case "CUREPOTION":
					case "0X0F07-C":
					case "GREATERCUREPOTION":
						nameString = "a keg of orange liquid";
						break;
					case "0X0F08":
					case "AGILITYPOTION":
					case "0X0F08-B":
					case "GREATERAGILITYPOTION":
						nameString = "a keg of blue liquid";
						break;
					case "0X0F09":
					case "STRENGTHPOTION":
					case "0X0F09-B":
					case "GREATERSTRENGTHPOTION":
						nameString = "a keg of white liquid";
						break;
					case "0X0F0A":
					case "LESSERPOISONPOTION":
					case "0X0F0A-B":
					case "POISONPOTION":
					case "0X0F0A-C":
					case "GREATERPOISONPOTION":
					case "0X0F0A-D":
					case "DEADLYPOISONPOTION":
						nameString = "a keg of green liquid";
						break;
					case "0X0F0C":
					case "LESSERHEALPOTION":
					case "0X0F0C-B":
					case "HEALPOTION":
					case "0X0F0C-C":
					case "GREATERHEALPOTION":
						nameString = "a keg of yellow liquid";
						break;
				}
			}
		}
	}
	else
	{
		// Show identified name for all eras post lbr
		nameString = potionKeg.name2;
	}

	return nameString;
}

function onTooltip( potionKeg, pSocket )
{
	var pUser = null;
	if( pSocket != null )
	{
		pUser = pSocket.currentChar;
	}

	var tooltipText = "";
	var potionCount = parseInt( potionKeg.GetTag( "potionCount" ));
	if( potionCount <= 0 )
	{
		tooltipText = GetDictionaryEntry( 17211, pSocket.language ); // The keg is empty.
	}
	else
	{
		var shardEra = GetServerSetting( "CoreShardEra" );
		if( shardEra == "t2a" || shardEra == "uor" || shardEra == "td" || shardEra == "lbr" )
		{
			// Show unidentified tag to players who don't know the contents of the keg
			if( potionKeg.name2 != "#" && ( !ValidateObject( potionKeg.owner ) || ( ValidateObject( potionKeg.owner ) && potionKeg.owner != pUser )))
			{
				if( !HasUserTastedItem( pUser, potionKeg ))
				{
					tooltipText = GetDictionaryEntry( 9402, pSocket.language ); // [Unidentified]
				}
			}
		}

		// Add an empty line if tooltip text already contains something
		if( tooltipText != "" )
		{
			tooltipText += "\n";
		}

		// Show description for how full potion keg is
		if( potionCount < 5 )
		{
			tooltipText += GetDictionaryEntry( 17212, pSocket.language ); // The keg is nearly empty.
		}
		else if( potionCount < 20 )
		{
			tooltipText += GetDictionaryEntry( 17213, pSocket.language ); // The keg is not very full.
		}
		else if( potionCount < 30 )
		{
			tooltipText += GetDictionaryEntry( 17214, pSocket.language ); // The keg is about one quarter full.
		}
		else if( potionCount < 40 )
		{
			tooltipText += GetDictionaryEntry( 17215, pSocket.language ); // The keg is about one third full.
		}
		else if( potionCount < 47 )
		{
			tooltipText += GetDictionaryEntry( 17216, pSocket.language ); // The keg is almost half full.
		}
		else if( potionCount < 54 )
		{
			tooltipText += GetDictionaryEntry( 17217, pSocket.language ); // The keg is approximately half full.
		}
		else if( potionCount < 70 )
		{
			tooltipText += GetDictionaryEntry( 17218, pSocket.language ); // The keg is more than half full.
		}
		else if( potionCount < 80 )
		{
			tooltipText += GetDictionaryEntry( 17219, pSocket.language ); // The keg is about three quarters full.
		}
		else if( potionCount < 96 )
		{
			tooltipText += GetDictionaryEntry( 17220, pSocket.language ); // The keg is very full.
		}
		else if( potionCount < 100 )
		{
			tooltipText += GetDictionaryEntry( 17221, pSocket.language ); // The liquid is almost to the top of the keg.
		}
		else
		{
			tooltipText += GetDictionaryEntry( 17222, pSocket.language ); // The keg is completely full.
		}
	}
	return tooltipText;
}
