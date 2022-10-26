// This script runs some ownership checks on houses before allowing a player to place
// additional houses. The actual house-placement part is handled in code.

// Check if tracking of house ownership is done on a per-account (instead of per-char) basis (from uox.ini)
const trackHousesPerAccount = GetServerSetting( "TrackHousesPerAccount" );

// Check if players can own and co-own houses at the same time (from uox.ini)
const canOwnAndCoOwnHouses = GetServerSetting( "CanOwnAndCoOwnHouses" );

// Fetch max amount of houses someone can own/co-own (from uox.ini)
const maxHousesOwnable = GetServerSetting( "MaxHousesOwnable" );
const maxHousesCoOwnable = GetServerSetting( "MaxHousesCoOwnable" );

function onUseChecked( pUser, iUsed )
{
	if( iUsed.morex == 0 )
		return false;

	var pSocket = pUser.socket;

	// Check how many houses player already owns, before allowing placing any more houses
	var housesOwned = pUser.housesOwned;
	var housesCoOwned = pUser.housesCoOwned;
	if( housesOwned >= maxHousesOwnable )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1827, pSocket.language ), pUser.housesOwned, maxHousesOwnable, ( trackHousesPerAccount ? "account" : "character" )); // You already own %i houses, you may not place another (Max %i per %s)!
		return false;
	}
	if( !canOwnAndCoOwnHouses && housesCoOwned > 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1828, pSocket.language ), housesCoOwned ); // You are already a co-owner of %i houses, and you cannot own and co-own
		return false;
	}

	// All good, return true and proceed with house placement in code
	return true;
}