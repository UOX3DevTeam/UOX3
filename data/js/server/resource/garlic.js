// Garlic-Picking Script
// 14/06/2025 Xuri; xuri@uox3.org
// When (dynamic)garlic are double-clicked, it's setup with
// garlic ripe for picking. When it is harvested it turns into "harvested garlic",
// and a "growth" process happens, where the garlic go through various stages
// like "sprouts", slightly larger sprouts and finally becomes harvestable fully grown garlic again.
const garlicGrowthDelay = 120000; // Delay in milliseconds before resources respawns
const garlicGrowthIntervalMin = 30000; // Min delay in milliseconds between each growth phase
const garlicGrowthIntervalMax = 90000; // Max delay in milliseconds between each growth phase
const garlicResourceDecay = 3600; // Time (in seconds) it takes for resource to decay, allowing another to spawn in its place if spawned using spawn regions. 0 to disable decay

function onUseChecked( pUser, iUsed )
{
	var isInRange = pUser.InRange( iUsed, 3 );
	if( !isInRange )
 	{
		pUser.SysMessage( GetDictionaryEntry( 2500, pUser.socket.language )); // You are too far away to reach that.
		return false;
	}

	if( !iUsed.GetTag( "initialized" )) // Unless garlic have been picked before, initialize settings
	{
		iUsed.SetTag( "initialized", 1 ); 	// Marks what as initialized
		iUsed.SetTag( "Garlic", 1 ); 	// If set to 1, there is garlic to be picked, if 0 there is no garlic left
	}
	var garlic = iUsed.GetTag( "Garlic" );
	if( garlic == 0 )
	{	
		pUser.SysMessage( GetDictionaryEntry( 9518, pUser.socket.language )); // This garlic is not ready for harvesting yet.
		return false;
	}
	if( garlic == 1 )
	{
		iUsed.SoundEffect( 0x0050, true );
		var loot = RollDice( 1, 3, 0 );
		if( loot == 2 )
		{
			pUser.SysMessage( GetDictionaryEntry( 9519, pUser.socket.language )); // You fail to pick any garlic.
		}
		if( loot == 3 || loot == 1 )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 9520, pUser.socket.language )); // You harvest a garlic.
			var itemMade = CreateDFNItem( pUser.socket, pUser, "0x0c79", 1, "ITEM", true );
			if( garlicResourceDecay > 0 )
			{
				iUsed.decayable = true;
				iUsed.decaytime = garlicResourceDecay;
			}
			iUsed.id = 0x0cb5;
			iUsed.name = "garlic sprout";
			iUsed.SetTag( "Garlic", 0 );
			iUsed.StartTimer( garlicGrowthDelay, 1, true); // Let's do some timers! Whee!
		}
		return false;
	}
	return false;
}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 ) // Starts phase 1 of garlic growth
	{
		iUsed.StartTimer( RandomNumber( garlicGrowthIntervalMin, garlicGrowthIntervalMax ), 2, true);
	}
	if( timerID == 2 ) // Garlic growth finished!
	{
		iUsed.id = 0x18e1;
		iUsed.name = "garlic";
		iUsed.SetTag( "Garlic", 1 );
	}
}