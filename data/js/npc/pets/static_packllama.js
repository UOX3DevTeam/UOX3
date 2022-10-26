// create pet from static script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// Updated by Xuri

var maxControlSlots = GetServerSetting( "MaxControlSlots" );
var maxFollowers = GetServerSetting( "MaxFollowers" );

// Runs before purchase is validated by server
function onBuyFromVendor( pSock, vendor, iBought )
{
	var pUser = pSock.currentChar;
	if( maxControlSlots > 0 && ( pUser.controlSlotsUsed + iBought.morez > maxControlSlots ))
	{
		pSock.SysMessage( GetDictionaryEntry( 2390, pSock.language )); // That would exceed your maximum pet control slots.
		if( ValidateObject( vendor ))
		{
			vendor.TextMessage( GetDictionaryEntry( 2399, pSock.language )); // Sorry, I cannot sell thee this item!
		}
		return false;
	}
	if( pUser.petCount >= maxFollowers )
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
function onBoughtFromVendor( pSock, Vendor, iBought )
{
	if( pSock && iBought )
	{
		onUseChecked( pSock.currentChar, iBought );
	}
}

function onUseChecked( pUser, iUsed )
{
	var nSpawned = SpawnNPC( "packllama", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( nSpawned )
	{
		// set owner to the envoker
		nSpawned.owner = pUser;

		// Increase pet control slots in use for owner, if feature is enabled
		if( maxControlSlots > 0 )
		{
			pUser.controlSlotsUsed = pUser.controlSlotsUsed + nSpawned.controlSlots;
		}

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
