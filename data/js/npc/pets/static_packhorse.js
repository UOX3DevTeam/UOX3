// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// Updated by Xuri

var maxControlSlots = GetServerSetting( "MaxControlSlots" );

// maxFollowers only comes into play if maxControlSlots is set to 0 in UOX.INI
var maxFollowers = GetServerSetting( "MaxFollowers" );

// Runs before purchase is validated by server
function onBuyFromVendor( pSock, vendor, iBought, numItemsBought )
{
	var pUser = pSock.currentChar;
	if( maxControlSlots > 0 )
	{
		if( pUser.controlSlotsUsed + ( numItemsBought * iBought.morez ) > maxControlSlots )
		{
			pSock.SysMessage( GetDictionaryEntry( 2390, pSock.language )); // That would exceed your maximum pet control slots.
			if( ValidateObject( vendor ))
			{
				vendor.TextMessage( GetDictionaryEntry( 2399, pSock.language )); // Sorry, I cannot sell thee this item!
			}
			return false;
		}
	}
	else if( maxFollowers > 0 && ( pUser.followerCount + numItemsBought > maxFollowers ))
	{
		pSock.SysMessage( GetDictionaryEntry( 2400, pSock.language )); // You have too many followers already!
		if( ValidateObject( vendor ))
		{
			vendor.TextMessage( GetDictionaryEntry( 2399, pSock.language )); // Sorry, I cannot sell thee this item!
		}
		return false;
	}

	return true;
}

// Runs after purchase has been completed
function onBoughtFromVendor( pSock, Vendor, iBought, numItemsBought )
{
	if( pSock && iBought )
	{
		onUseChecked( pSock.currentChar, iBought );
	}
}

function onUseChecked( pUser, iUsed )
{
	var nSpawned = SpawnNPC( "packhorse", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( nSpawned )
	{
		// set owner to the envoker
		nSpawned.owner = pUser;

		// Increase pet control slots in use for owner, if feature is enabled
		if( maxControlSlots > 0 )
		{
			pUser.controlSlotsUsed = pUser.controlSlotsUsed + nSpawned.controlSlots;
		}

		// Set nSpawned as an active follower of pUser
		pUser.AddFollower( nSpawned );

		// make pet follow owner by default
		nSpawned.Follow( pUser );

		// make a sound
		pUser.SoundEffect( 0x0215, true );

		// remove the static
		iUsed.Delete();
	}
	else
	{
		pUser.SysMessage( "Creature failed to spawn, reason unknown." );
	}

	return false;
}
