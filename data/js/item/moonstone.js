// Moonstone Script

// invalidTiles contains hex-id's of various maptiles that count as "invalid" if the user
// either stands on one when attempting to use the moonstone, or if the user would get teleported
// to one after walking through/doubleclicking on the moongate.

// Moonstone-ID: 0x0F8B
// Moongate-ID: 0x0F6C

var invalidTiles = new Array( "0x00a8", "0x00a9", "0x00aa", "0x00ab", "0x0136", "0x0137", "0x01f4", "0x01f5", "0x01f6", "0x01f7" );

function onCreateDFN( objMade, objType )
{
	// If the moonstone is spawned in Felucca, it becomes a Trammel Moonstone
	if( objMade.worldnumber == 0 )
	{
		objMade.name = "Trammel Moonstone";
		objMade.SetTag( "TargetWorld", 1 );
	}

	// If the moonstone is spawned in Trammel, it becomes a Felucca Moonstone
	if( objMade.worldnumber == 1 )
	{
		objMade.name = "Felucca Moonstone";
		objMade.SetTag( "TargetWorld", 0 );	
	}
}

function onUseChecked( pUser, iUsed )
{
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	// If moonstone has become a moongate already, act as if user has collided with moongate upon doubleclick
	if( iUsed.id == 0x0f6c )
	{
		onCollide( pUser.socket, pUser, iUsed );
		return false;
	}

	// Numberify the value for target world defined on moonstone
	var targetWorld = parseInt( iUsed.GetTag( "TargetWorld" ));
	
	// Check for generic failure reasons, generate failure messages
	var itemContainer = iUsed.container;
	var failureMsg = "";
	if( itemContainer == null ) //|| itemContainer.serial != pUser.pack.serial )
	{
		failureMsg = "This must be in your backpack before it can be used.";
	}
	else if( pUser.region.isGuarded )
	{
		failureMsg = "You cannot bury the stone in a guarded area.";
	}
	else if( pUser.isonhorse )
	{
		failureMsg = "You cannot bury a stone while you sit on a mount.";
	}
	else if( pUser.criminal )
	{
		failureMsg = "The magic of the stone cannot be envoked by the lawless.";
	}
	else if( pUser.murderer )
	{
		failureMsg = "The magic of the stone cannot be envoked by someone with blood on their hands.";
	}
	else if( targetWorld == 0 && pUser.worldnumber != 1 )
	{
		failureMsg = "You must be in Trammel to use this.";
	}
	else if( targetWorld == 1 && pUser.worldnumber != 0 )
	{
		failureMsg = "You must be in Felucca to use this.";
	}

	// Change color of moonstone depending on moon phase
	// Which moon? Need JS function/method to read moon phases! How?
	if( itemContainer && itemContainer.serial == pUser.pack.serial )
	{
		// For random reasons that need no explaining, we check the moonphase
		// for the Trammel moon if the player is in Trammel, and for Felucca moon if in Felucca,
		// and change the color of the moonstone depending on what phase the moon is in.
		switch( Moon( pUser.worldnumber ))
		{
			case 0: // Full Moon
				iUsed.colour = 0x017e;
				break;
			case 1: // Waning Gibbous
				iUsed.colour = 0x0544;
				break;
			case 2: // Last Quarter
				iUsed.colour = 0x0011;
				break;
			case 3: // Waning Crescent
				iUsed.colour = 0x084e;
				break;
			case 4: // New Moon
				iUsed.colour = 0x035a;
				break;
			case 5: // Waxing Crescent
				iUsed.colour = 0x021b;
				break;
			case 6: // First Quarter
				iUsed.colour = 0x0025;
				break;
			case 7: // Waxing Gibbous
				iUsed.colour = 0x000c;
				break;
			default:
		}
	}
	
	// If no failureMsg exists so far, check if source & target locations are valid
	if( !failureMsg || failureMsg == "" )
	{
		if( StaticAt( pUser.x, pUser.y, targetWorld ) || GetItem( pUser.x, pUser.y, pUser.z, targetWorld ))
		{
			var failureMsg = "Something is blocking the facet gate exit.";
		}
		else
		{
			var InitMapTile = GetTileIDAtMapCoord( pUser.x, pUser.y, pUser.worldnumber );
			var TargMapTile = GetTileIDAtMapCoord( pUser.x, pUser.y, targetWorld );
			for( var i = 0; i < invalidTiles.length; i++ )
			{
				if( InitMapTile == invalidTiles[i] )
				{
					var failureMsg = "You cannot open a facet gate from this location.";
					break;	
				}
			   	if( TargMapTile == invalidTiles[i] )
			   	{
			        var failureMsg = "Something is blocking the facet gate exit.";
			       	break;
				}
		    }			
		}
	}
	// If a failureMsg has happened, let the player know and halt the script
	if( failureMsg && failureMsg != "" )
	{
		pUser.SysMessage( failureMsg );
		return false;
	}
	
	// If no failureMsg has been generated the moonstone is successfully used
	pUser.DoAction( 0x20 );
	iUsed.container = null;
	iUsed.Teleport( pUser.x, pUser.y, pUser.z, pUser.worldnumber );
	iUsed.morex = 1;
	//iUsed.movable = 2;
	iUsed.owner = pUser;
	iUsed.tempObj = pUser;
	iUsed.TextMessage( "The stone settles into the ground" );
	iUsed.StartTimer( 5000, 1, true );
	return false;	
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		// If the player has picked up the item since it was settled in the ground, don't do anything
		if( iUsed.morex == 0 )
			return;

		var pUser = iUsed.tempObj;
		// Change moonstone gfx to moongate gfx, and enable decayable status
		iUsed.id = 0x0f6c;
		iUsed.dir = 29;
		iUsed.decayable = true;
		iUsed.decaytime = 30;
		//Update the name of the item
		switch( iUsed.worldnumber )
		{
			case 0:
				iUsed.name = "Trammel Moongate";
				break;
			case 1:
				iUsed.name = "Felucca Moongate";
				break;
		}
		// Teleport item to it's own location to update name to nearby players
		iUsed.Teleport( iUsed.x, iUsed.y, iUsed.z, iUsed.worldnumber );
		// Add moongate at target location as well, for visual purposes only
		var newGate = CreateBlankItem( pUser.socket, pUser, 1, "Moongate", 0x0f6c, 0, "ITEM", false );
		newGate.dir = 29;
		newGate.colour = iUsed.colour;
		newGate.decayable = true;
		newGate.decaytime = 30;
		newGate.Teleport( iUsed.x, iUsed.y, iUsed.z, parseInt( iUsed.GetTag( "TargetWorld" )) );
	}
}

function onCollide( trgSock, srcChar, trgItem )
{
	// If user collides with item after it changes to moongate form, teleport him. 
	if( trgItem.id == 0x0f6c )
	{
		//Only the placer of the moonstone and anyone in his party can enter through the gate
		if( trgItem.owner != srcChar && srcChar.party != trgItem.owner.party )
		{
			srcChar.TextMessage( "Only the placer of the moonstone or someone in his party may go through this gate." );
			return;	
		}
		else if( !CanUseGate( trgSock, srcChar ))
		{
			return;	
		}
		
		var tempVal = trgItem.GetTag( "TargetWorld" );
		var worldVal = parseInt( tempVal );

		// Teleport character's pets to target world and location
		var petList = srcChar.GetPetList();
		for( var i = 0; i < petList.length; i++ )
		{
			var tempPet = petList[i];
			if( ValidateObject( tempPet ) && tempPet.InRange( srcChar, 24 ))
			{
				tempPet.Teleport( trgItem.x , trgItem.y, trgItem.z, worldVal );
				tempPet.Follow( srcChar );
			}
		}

		// Teleport our character to the target world and location
		srcChar.Teleport( trgItem.x , trgItem.y, trgItem.z, worldVal );
	}
}

function onPickup( iPickedUp, pGrabber )
{
	if( iPickedUp.id == 0x0f6c )
	{
		return 0; //Bounce the item!
	}
	else if( iPickedUp.morex == 1 && iPickedUp.owner == pGrabber )
	{
		iPickedUp.morex = 0; //Reset the moonstone so it doesn't turn into a moongate
	}
	else if( iPickedUp.owner && pGrabber != iPickedUp.owner )
	{
		pGrabber.SysMessage( "I don't think so! Only the owner may pick this up." );
		return 0; //Bounce the item!
	}	
}

function CanUseGate( srcSock, pUser )
{
	if( pUser.criminal )
	{
		// Disallow moongate travel for players flagged as criminals
		if( srcSock )
		{
			srcSock.SysMessage( GetDictionaryEntry( 9112, srcSock.language )); // Thou'rt a criminal and cannot escape so easily.
		}
		return false;
	}
	else if( pUser.atWar && pUser.attackFirst)
	{
		// Disallow moongate travel for players flagged as aggressors
		var pTarget = pUser.target;
		if( ValidateObject( pTarget ))
		{
			if( !pTarget.npc && !pTarget.dead && pTarget.online && !pTarget.criminal && !pTarget.murderer )
			{
				if( srcSock )
				{
					srcSock.SysMessage( GetDictionaryEntry( 9113, srcSock.language )); // Wouldst thou flee during the heat of battle??
				}
				return false;
			}
		}
	}
	else
	{
		// Disallow moongate travel for players busy casting a spell
		if( pUser.GetTimer( Timer.SPELLTIME ) != 0 )
		{
			if( pUser.isCasting )
			{
				if( srcSock )
				{
					srcSock.SysMessage( GetDictionaryEntry( 9114, srcSock.language )); // You are too busy to do that at the moment.
				}
				return false;
			}
		}
	}

	return true;
}