// Scissor-usage; Sheep-shearing, cloth-cutting
// 24/07/2021 Xuri; xuri@uox3.org
// This script contains all the default functions of the scissors,
// from sheep-shearing to cloth cutting.
var aosEnabled = 0;

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var isInRange = pUser.InRange( iUsed, 3 );
		if( !isInRange )
	 	{
			socket.SysMessage( GetDictionaryEntry( 389, socket.language )); // That is too far away and you cannot reach it.
		}
		else
		{
			if( pUser.isRunning ) // Taunt player about using scissors while running
			{
				socket.SysMessage( GetDictionaryEntry( 2759, socket.language )); // Didn't your parents ever tell you not to run with scissors in your hand?!
			}

			socket.tempObj = iUsed;
			socket.CustomTarget( 0, GetDictionaryEntry( 6029, socket.language )); // What do you want to use these scissors on?
		}
	}
	return false;
}

function onCallback0( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	var StrangeByte	= pSock.GetWord( 1 );
	var tileID = pSock.GetWord( 17 );

	if( !ValidateObject( myTarget ) || tileID == 0 )
	{
		//Target is invalid or a Maptile
		pSock.SysMessage( GetDictionaryEntry( 6030, pSock.language )); // You can't use the scissors on that.
		return;
	}

	// Verify that the scissors the user used still exist
	var myScissors = pSock.tempObj;
	if( !ValidateObject( myScissors ))
	{
		pSock.SysMessage( GetDictionaryEntry( 8803, pSock.language )); // Object no longer exists.
		return;
	}

	// Verify that user is still within range of the scissors
	if( !pUser.InRange( myScissors, 3 ))
	{
		pSock.SysMessage( GetDictionaryEntry( 2749, pSock.language )); // You have moved too far away to use this.
		return;
	}

	if( StrangeByte == 0 )
	{
		if( myTarget.isChar )
		{
			//Target is a Character
			var isInRange = pUser.InRange( myTarget, 3 );
			if( !isInRange )
		 	{
				pSock.SysMessage( GetDictionaryEntry( 461, pSock.language )); // You are too far away.
				return;
			}
			else if( myTarget.id == 0x00df || myTarget.id == 0x00cf )
			{
				TriggerEvent( 2012, "ShearSheep", pUser, myTarget );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6030, pSock.language )); // You can't use the scissors on that.
			}
			return;
		}
		else
		{
			//Target is a Dynamic Item
			if( !pUser.InRange( myTarget, 3 ))
		 	{
				pSock.SysMessage( GetDictionaryEntry( 461, pSock.language )); // You are too far away.
				return;
			}
			if( myTarget.movable == 3 )
			{
				pSock.SysMessage( GetDictionaryEntry( 6031, pSock.language )); // Locked down resources cannot be used!
				return;
			}

			var iPackOwner = GetPackOwner( myTarget, 0 );
			if( ValidateObject( iPackOwner )) // Is the target item in a backpack?
			{
				if( iPackOwner.serial != pUser.serial ) //And if so does the pack belong to the user?
				{
					pSock.SysMessage( GetDictionaryEntry( 6032, pSock.language )); // That resource is in someone else's backpack!
					return;
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6022, pSock.language )); // This has to be in your backpack before you can use it.
				return;
			}

			if( myTarget.isNewbie )
			{
				pSock.SysMessage( GetDictionaryEntry( 6036, pSock.language )); // Scissors cannot be used on that to produce anything.
				return;
			}

			// Keep track of the color of the source material, so we can apply it to the end-product
			var resourceColor = myTarget.colour;

			if( CutBolts( tileID ))
			{
				//Cut bolts of cloth into cut cloth
				pUser.SoundEffect( 0x248, true );
				pSock.SysMessage( GetDictionaryEntry( 6033, pSock.language )); // You cut the material and place it into your backpack.
				CreateDFNItem( pSock, pUser, "0x1766", myTarget.amount * 50, "ITEM", true, resourceColor );  //give the player some cut cloth
				myTarget.Delete();
				return;
			}
			else if( CutCloth( tileID ))
			{
				//Cut folded cloth and cut cloth into bandages
				pUser.SoundEffect( 0x248, true );
				pSock.SysMessage( GetDictionaryEntry( 6034, pSock.language )); // You cut the material into bandage and place it in your backpack.
				CreateDFNItem( pSock, pUser, "0x0e21", myTarget.amount, "ITEM", true, resourceColor );  //give the player some bandages
				myTarget.Delete();
				return;
			}
			else if( CutLeather( tileID ))
			{
				//Cut up hides into leather
				pUser.SoundEffect( 0x248, true );
				pSock.SysMessage( GetDictionaryEntry( 6035, pSock.language )); // You cut the material into leather and place it in your backpack.
				CreateDFNItem( pSock, pUser, "0x1081", myTarget.amount, "ITEM", true, resourceColor );
				myTarget.Delete();
				return;
			}
			else if( CutClothing( tileID ))
			{
				//Cut Clothing into cut cloth
				pUser.SoundEffect( 0x248, true );
				var resourceAmount = GetResourcesReturned( pUser, myTarget, 0x1081, 0x0, 34 ); // Skill 34 = Tailoring, 0x1081 == cut leather
				CreateDFNItem( pSock, pUser, "0x1766", myTarget.amount * resourceAmount, "ITEM", true, resourceColor );
				myTarget.Delete();
				return;
			}
			else if( CutLeatherArmor( tileID ))
			{
				//Cut Leather Armor into leather
				pUser.SoundEffect( 0x248, true );
				var resourceAmount = GetResourcesReturned( pUser, myTarget, 0x1081, 0x0, 34 ); // Skill 34 = Tailoring, 0x1081 == cut leather
				CreateDFNItem( pSock, pUser, "0x1081", myTarget.amount * resourceAmount, "ITEM", true, resourceColor );
				myTarget.Delete();
				return;
			}
			else if( CutStuddedLeatherArmor( tileID ))
			{
				//Cut Studded Leather Armor into leather
				pUser.SoundEffect( 0x248, true );
				var resourceAmount = GetResourcesReturned( pUser, myTarget, 0x1081, 0x0, 34 ); // Skill 34 = Tailoring, 0x1081 == cut leather
				CreateDFNItem( pSock, pUser, "0x1081", myTarget.amount * resourceAmount, "ITEM", true, resourceColor );
				myTarget.Delete();
				return;
			}
			else if( aosEnabled == 1  && CutBones( tileID ))
			{
				//Cut Bones Parts into Bone
				pUser.SoundEffect( 0x248, true );
				CreateDFNItem( pSock, pUser, "0x0f7e", myTarget.amount * 10, "ITEM", true, resourceColor );
				myTarget.Delete();
				return;
			}
		}
		pSock.SysMessage( GetDictionaryEntry( 6036, pSock.language )); // Scissors cannot be used on that to produce anything.
	}
}

function CutClothing( tileID )
{
	return ( ( tileID >= 5397 && tileID <= 5400 ) || ( tileID >= 5422 && tileID <= 5444 ) || ( tileID >= 7933 && tileID <= 7940 )
		|| ( tileID >= 5899 && tileID <= 5916 ) || ( tileID >= 8095 && tileID <= 8098 ) || ( tileID == 8189 || tileID == 8190 )
		|| ( tileID >= 8973 && tileID <= 8976 ) || ( tileID >= 9706 && tileID <= 9715 ) || ( tileID >= 9801 && tileID <= 9802 )
		|| ( tileID >= 9805 && tileID <= 9814 ) || ( tileID >= 9823 && tileID <= 9832 ) || ( tileID >= 9835 && tileID <= 9838 )
		|| ( tileID >= 9851 && tileID <= 9863 ) || ( tileID == 9885 || tileID == 9886 ) || ( tileID >= 9889 && tileID <= 9892 )
		|| ( tileID >= 9901 && tileID <= 9904 ) || ( tileID == 10114 || tileID == 10115 ) || tileID == 10132 || ( tileID >= 10137 && tileID <= 10140 )
		|| ( tileID == 10144 || tileID == 10145 ) || ( tileID == 10189 || tileID == 10190 ) || tileID == 10207 || ( tileID >= 10212 && tileID <= 10215 )
		|| ( tileID == 10219 || tileID == 10220 ) || ( tileID >= 12659 && tileID <= 12664 ) || ( tileID >= 16384 && tileID <= 16387 )
		|| ( tileID >= 16479 && tileID <= 16486 ) || ( tileID >= 19357 && tileID <= 19360 ) || tileID == 30742
		|| ( tileID >= 30745 && tileID <= 30753 ) || ( tileID == 40687 || tileID == 40688 ) || ( tileID >= 40695 && tileID <= 40700 )
		|| ( tileID >= 41131 && tileID <= 41135 ));
}

function CutLeatherArmor( tileID )
{
	return ( ( tileID >= 5061 && tileID <= 5063 ) || ( tileID >= 5067 && tileID <= 5070 ) || ( tileID >= 5074 && tileID <= 5075 )
		|| ( tileID >= 769 && tileID <= 782 ) || ( tileID == 784 || tileID == 785 ) || ( tileID == 7168 || tileID == 7169 )
		|| ( tileID >= 7174 && tileID <= 7179 ) || ( tileID == 7609 || tileID == 7610 ) || ( tileID == 9873 || tileID == 9874 )
		|| ( tileID == 10102 || tileID == 10106 || tileID == 10107 || tileID == 10118 || tileID == 10122 )
		|| ( tileID >= 10126 && tileID <= 10131 ) || ( tileID == 10181 || tileID == 10182 || tileID == 10193 || tileID == 10197 )
		|| ( tileID >= 10201 && tileID <= 10206 ) || ( tileID == 16856 || tileID == 16857 || tileID == 17790 || tileID == 17791 ));
}

function CutStuddedLeatherArmor( tileID )
{
	return ( ( tileID >= 5076 && tileID <= 5078 ) || ( tileID >= 5082 && tileID <= 5085 ) || ( tileID >= 5089 && tileID <= 5090 )
		|| ( tileID == 7170 || tileID == 7171 || tileID == 7180 || tileID == 7181 || tileID == 10108 || tileID == 10119 || tileID == 10123 )
		|| ( tileID == 10141 || tileID == 10183 || tileID == 10194 || tileID == 10198 || tileID == 10216 ));
}

function CutLeather( tileID )
{
	return ( tileID >= 4216 && tileID <= 4217 );
}

function CutCloth( tileID )
{
	return ( tileID >= 5981 && tileID <= 5992 );
}

function CutBolts( tileID )
{
	return ( tileID >= 3989 && tileID <= 3996 );
}

function CutBones( tileID )
{
	return ( tileID >= 6921 && tileID <= 6929 || ( tileID >= 6935 && tileID <= 6937 ));
}

function GetResourcesReturned( pUser, myTarget, resourceID, resourceColor, skillID )
{
	// Verify that item was crafted by player
	var entryMadeFrom = myTarget.entryMadeFrom;
	if( entryMadeFrom == null || entryMadeFrom == 0 )
		return 1;

	// Verify that the entry ID returned is a valid create DFN entry
	var createEntry = CreateEntries[entryMadeFrom];
	if( createEntry == null )
		return 1;

	// Verify that the create DFN entry matches the ID of the item we're getting resource amount for
	if( createEntry.id != myTarget.id )
		return 1;

	// Loop through list of resources needed to create item, see if ANY of them matches up to the
	// resources we're trying to get amount for
	var resourceAmount = -1;
	var resourcesUsed = createEntry.resources;

	for( var i = 0; i < resourcesUsed.length; i++ )
	{
		var resource = resourcesUsed[i];
		var amountNeeded = resource[0];
		var colorNeeded = resource[1];
		var resourceIDs = resource[2];

		// Loop through list of resourceIDs that were valid for crafting this item, see if ANY
		// were a match for the resource we're trying to return
		for( var j = 0; j <= resourceIDs.length; j++ )
		{
			// If both resource needed matches up, and resource color matches up, go for it
			if( resourceIDs[j] == resourceID && colorNeeded == resourceColor )
			{
				resourceAmount = amountNeeded;
			}
		}
	}

	if( resourceAmount == -1 )
	{
		pUser.socket.SysMessage( GetDictionaryEntry( 6277, pUser.socket.language )); // Resource ID not found in list of resources used to craft item.
		return 1;
	}

	// Calculate amount of resources returned based on player's skill
	var maxResourceAmount = Math.floor( resourceAmount / 2 ); // Max half returned of what was invested

	// Fetch player skill
	var playerSkill = pUser.skills[skillID];

	// Based on player's skill, return between 1 to maxResourceAmount
	var amountReturned = Math.min( Math.max( Math.floor( maxResourceAmount * ( playerSkill / 1000 )), 1 ), maxResourceAmount );
	return amountReturned;
}