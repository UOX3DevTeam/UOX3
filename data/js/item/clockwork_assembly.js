// Tool used by Tinkerers to craft Iron Golems
const maxControlSlots = GetServerSetting( "MaxControlSlots" );
const maxFollowers = GetServerSetting( "MaxFollowers" );
const golemControlSlots = 4;

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( ValidateObject( iUsed ))
		return;

	// Check to see if it's locked down
	if( iUsed.movable == 2 || iUsed.movable == 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 774, socket.language )); // That is locked down and you cannot use it
		return false;
	}

	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
		return false;
	}

	if( pUser.skills.tinkering < 60 )
	{
		socket.SysMessage( GetDictionaryEntry( X, socket.language )); // You must be a Journeyman or higher Tinker to construct a golem
		return false;
	}

	// Check to see if player actually has space for ANY more pets
	if( maxControlSlots > 0 && pUser.controlSlotsUsed >= maxControlSlots )
	{
		pSock.SysMessage( GetDictionaryEntry( X, pSock.language )); // You have too many followers to control that creature.
		return;
	}

	if( maxControlSlots > 0 && ( pUser.controlSlotsUsed + golemControlSlots > maxControlSlots ))
	{
		pSock.SysMessage( GetDictionaryEntry( 2390, pSock.language )); // That would exceed your maximum number of pet control slots.
		return;
	}

	if( pUser.petCount >= maxFollowers )
	{
		var tempMsg = GetDictionaryEntry( 2346, pSock.language ); // You can maximum have %i pets/followers active at the same time.
		pSock.SysMessage( tempMsg.replace( /%i/gi, maxFollowers ));
		return;
	}

	// Check if player has enough resources to construct the golem
	// 1 power crystal, 50 iron ingots, 50 bronze ingots and 5 gears
	if( pUser.ResourceCount( 0x1f1c ) < 1 )
	{
		socket.SysMessage(); // You need a power crystal to construct a golem.
	}
	else if( pUser.ResourceCount( 0x1bf2 ) < 50 )
	{
		socket.SysMessage(); // You need more iron ingots to construct a golem.
	}
	else if( pUser.ResourceCount( 0x1bf2, 0x06d6 ) < 50 )
	{
		socket.SysMessage(); // You need more bronze ingots to construct a golem.
	}
	else if( pUser.ResourceCount( ) < 5 )
	{
		socket.SysMessage(); // You need more gears to construct a golem.
	}
	else
	{
		// Construct the golem!
		var golemNPC = SpawnNPC( "crafted_golem", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
		if( ValidateObject( nSpawned ))
 		{
 			// Delete the clockwork assembly
 			iUsed.Delete();

 			// Set player as owner of the golem
 			golemNPC.owner = pUser;

 			// Start following the player
 			golemNPC.Follow( pUser );

 			// Play a sound effect
 			golemNPC.SoundEffect( 0x241, true );

 			// Apply power-modifications based on player's Tinkering skill
 			ApplyGolemPower( golemNPC, pUser );
 			return true;
 		}
	}

	return false;
}

function ApplyGolemPower( golemNPC, pUser )
{
	// Calculate power-scale value
	var powerScale;
	var tinkerSkill = pUser.skills.tinkering;
	if( tinkerSkill >= 1000 )
	{
		powerScale = 1.0;
	}
	else if( tinkerSkill >= 900 )
	{
		powerScale = 0.9;
	}
	else if( tinkerSkill >= 800 )
	{
		powerScale = 0.8;
	}
	else if( tinkerSkill >= 700 )
	{
		powerScale = 0.7;
	}
	else
	{
		powerScale = 0.6;
	}

	// Scale golem NPC's attributes and skills
	golemNPC.strength *= powerScale;
	golemNPC.dexterity *= powerScale;
	golemNPC.intelligence *= powerScale;
	golemNPC.skills.magicresistance *= powerScale;
	golemNPC.skills.tactics *= powerScale;
	golemNPC.skills.wrestling *= powerScale;
}