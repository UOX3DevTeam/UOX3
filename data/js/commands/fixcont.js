// This ADMIN-only command will iterate through all items in the worldfiles and
// update - for containers only - the...
// 		weight (if container is empty and does no weight-value is already defined)
// 		baseWeight (forced on all containers except character backpacks/bankboxes)
// 		weightMax (if no weightMax-value is already defined)
//	...properties

function CommandRegistration()
{
	RegisterCommand( "fixcont", 3, true );
}

function command_FIXCONT( socket, cmdString )
{
	var confirmGump = new Gump;
	confirmGump.AddPage( 0 );
	confirmGump.AddBackground( 0, 130, 410, 250, 9550 );
	confirmGump.AddBackground( 5, 135, 400, 240, 83 );
	confirmGump.AddText( 30, 140, 1259, "WARNING!" );
	confirmGump.AddText( 100, 140, 160, "Executing 'fixcont'-script" );
	confirmGump.AddText( 15, 165, 80, "Are you certain? This will iterate through ALL items" );
	confirmGump.AddText( 15, 185, 80, "on your shard and force-update the baseWeight (forced)," );
	confirmGump.AddText( 15, 205, 80, "weight (conditional) and weightMax (conditional) properties" );
	confirmGump.AddText( 15, 225, 80, "for all non-bank/non-backpack containers to match values" );
	confirmGump.AddText( 15, 245, 80, "defined in this script. It will also update maxItems for all" );
	confirmGump.AddText( 15, 265, 80, "containers that don't already have a value for that." );
	
	confirmGump.AddText( 15, 285, 1259, "NOTE: This will NOT overwrite any custom weight-related" );
	confirmGump.AddText( 15, 305, 1259, "values you might have applied to your containers." );
	confirmGump.AddText( 15, 325, 1259, "You should rarely have to use this command." );
	confirmGump.AddButton( 75, 355, 241, 1, 0, 1 ); // Cancel
	confirmGump.AddButton( 200, 355, 247, 1, 0, 2 ); // Continue
    confirmGump.Send( socket );
	confirmGump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	//Check if any buttons were pressed
	switch( pButton )
	{
		case 0:	//Closes gump
		case 1: //Closes gump
			break;
		case 2: //Action confirmed, iterate through objects and fix weight
			pSock.SysMessage( "Iterating over all items, checking for containers..." );
			var count = IterateOver( "ITEM" );
			pSock.SysMessage( "A total of " + count + " containers were updated." );
			break;
		default:
			break;
	}
}

function onIterate( toCheck )
{
	if( toCheck.type == 1 ) // Is it a container?
	{
		if( toCheck.container && toCheck.container.isChar ) // Is the container contained on a character (backpack, bankbox)
		{
			return false; // We don't want to change these.
		}
		else // Otherwise, the other containers are what we're looking for
		{
			var countThis = true;
			switch( toCheck.id ) // Let's check the id of the item against a list of container-ids
			{
				case 0x0E75:	// backpack
				case 0x0E79:	// pouch
				case 0x09B0:	// pouch
				case 0x09B2:	// backpack
				case 0x0E76:	// leather bag
				case 0x0E7A:	// square picnic basket
				case 0x24D5:	// SE basket
				case 0x24D6:	// SE basket
				case 0x24D9:	// SE basket
				case 0x24DA:	// SE basket
				case 0x24D7:	// SE basket
				case 0x24D8:	// SE basket
				case 0x24DD:	// SE basket
				case 0x0A35:	// dresser drawer
				case 0x232A:	// giftbox
				case 0x232B:	// giftbox
				case 0x24DB:	// SE basket
				case 0x24DC:	// SE basket
				case 0x46A5:	// SA giftbox
				case 0x46A6:	// SA giftbox
				case 0x46A2:	// SA giftbox
				case 0x46A3:	// SA giftbox
				case 0x46A4:	// SA giftbox
				case 0x46A7:	// SA giftbox
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 100
					}
					toCheck.baseWeight = 100;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0990:	// round basket
				case 0x09AC:	// round bushel
				case 0x09B1:	// round basket
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 200
					}
					toCheck.baseWeight = 200;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0E83:	// empty tub
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 300
					}
					toCheck.baseWeight = 300;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0E7F:	// keg
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 400
					}
					toCheck.baseWeight = 400;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0E77:	// barrel
				case 0x0FAE:	// barrel with lids
				case 0x0E3E:	// small wooden create
				case 0x0E3F:	// small wooden crate
				case 0x0E7E:	// wooden crate
				case 0x09A9:	// small wooden crate
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 500
					}
					toCheck.baseWeight = 500;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0E3C:	// large wooden crate
				case 0x0E3D:	// large wooden crate
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 600
					}
					toCheck.baseWeight = 600;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0E7D:	// wooden box
				case 0x09AA:	// wooden box
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 700
					}
					toCheck.baseWeight = 700;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x1AD7:	// potion keg
				case 0x1940:	// potion keg
				case 0x2857:	// SE armoire
				case 0x2858:	// SE armoire
				case 0x285B:	// SE armoire
				case 0x285C:	// SE armoire
				case 0x285D:	// SE armoire
				case 0x285E:	// SE armoire
				case 0x2859:	// SE armoire
				case 0x285A:	// SE armoire
				case 0x2815:	// SE cabinet
				case 0x2816:	// SE cabinet
				case 0x2817:	// SE cabinet
				case 0x2818:	// SE cabinet
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 1000
					}
					toCheck.baseWeight = 1000;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x280B:	// SE chest
				case 0x280C:	// SE chest
				case 0x280D:	// SE chest
				case 0x280E:	// SE chest
				case 0x280F:	// SE chest
				case 0x2810:	// SE chest
				case 0x2811:	// SE chest
				case 0x2812:	// SE chest
				case 0x2813:	// SE chest
				case 0x2814:	// SE chest
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 1500
					}
					toCheck.baseWeight = 1500;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0A30:   	// chest of drawers (fancy)
				case 0x0A38:   	// chest of drawers (fancy)
				case 0x0E80:	// brass box
				case 0x09A8:	// metal box
				case 0x0E7C:	// silver chest
				case 0x09AB:	// silver chest
				case 0x0A97:	// bookcase
				case 0x0A98:	// bookcase
				case 0x0A99:	// bookcase
				case 0x0A9A:	// bookcase
				case 0x0A9B:	// bookcase
				case 0x0A9C:	// bookcase
				case 0x0A9D:	// bookcase (empty)
				case 0x0A9E:	// bookcase (empty)
				case 0x0A4C:	// fancy armoire (open)
				case 0x0A4D:	// fancy armoire
				case 0x0A50:	// fancy armoire (open)
				case 0x0A51:	// fancy armoire
				case 0x0A4E:	// wooden armoire (open)
				case 0x0A4F:	// wooden armoire
				case 0x0A52:	// wooden armoire (open)
				case 0x0A53:	// wooden armoire
				case 0x0A2C:	// chest of drawers (wood)
				case 0x0A34:	// chest of drawers (wood)
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 2000
					}
					toCheck.baseWeight = 2000;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0E40:	// gold chest
				case 0x0E41:	// gold chest
				case 0x0E42:	// wooden chest
				case 0x0E43:	// wooden chest
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 3000
					}
					toCheck.baseWeight = 3000;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0A3C:	// dresser
				case 0x0A44:	// dresser
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 5000
					}
					toCheck.baseWeight = 5000;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				case 0x0ECA:	// bones
				case 0x0ECB:	// bones
				case 0x0ECC:	// bones
				case 0x0ECD:	// bones
				case 0x0ECE:	// bones
				case 0x0ECF:	// bones
				case 0x0ED0:	// bones
				case 0x0ED1:	// bones
				case 0x0ED2:	// bones
					if( toCheck.weightMax == 0 )
					{
						toCheck.weightMax = 40000;
					}
					if( toCheck.itemsinside == 0 && toCheck.weight == 0 )
					{
						toCheck.weight = 2500
					}
					toCheck.baseWeight = 2500;
					if( toCheck.maxItems == 0 )
					{
						toCheck.maxItems = 125;
					}
					break;
				default:
					countThis = false; // The id didn't match any of the entries
			}
			if( countThis ) // A matching entry was found and updated
			{
				return true;
			}
		}
	}
	return false;
}