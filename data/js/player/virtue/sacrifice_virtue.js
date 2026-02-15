/// <reference path="../../definitions.d.ts" />
// @ts-check

var virtueEnums = TriggerEvent( 8003, "Virtue_GetEnums" );
var VirtueName  = virtueEnums ? virtueEnums.VirtueName  : null;
var VirtueLevel = virtueEnums ? virtueEnums.VirtueLevel : null;

const Sacrifice_GainDelayMs = 24 * 60 * 60 * 1000;      // 1 day
const Sacrifice_LossDelayMs = 7 * Sacrifice_GainDelayMs; // 7 days
const Sacrifice_LossAmount  = 500;                      // Atrophy amount when decay kicks in

/** @type { ( pUser: Character ) => void } */
function Sacrifice_OnVirtueUsed( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	// visible: 0 = visible, 1 = hidden, 2 = magically invisible, 3 = GM only
	if( pUser.visible !== 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30022, pSocket.language )); // You cannot do that while hidden.
		return;
	}

	if( pUser.dead )
	{
		// Dead: use Sacrifice to self-res
		Sacrifice_Resurrect( pUser );
	}
	else
	{
		// Alive: target a creature to sacrifice your fame for
		Sacrifice_StartTarget( pUser );
	}
}

/** @type { ( pUser: Character ) => void } */
function Sacrifice_Resurrect( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null)
		return;

	if( !pUser.dead )
		return;

	// Criminals cannot use this ability
	if( pUser.criminal )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30023, pSocket.language )); // You cannot use this ability while flagged as a criminal.
		return;
	}

	// Must be at least Seeker of Sacrifice
	var level = TriggerEvent( 8003, "Virtue_GetLevel", pUser, VirtueName.Sacrifice );
	if( level < VirtueLevel.Seeker )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30024, pSocket.language )); // You cannot use this ability.
		return;
	}

	// Check available resurrects (0..3)
	var data = Sacrifice_ReadData( pUser );
	if( data.resCount <= 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30025, pSocket.language )); // You do not have any resurrections left.
		return;
	}

	pUser.Resurrect();

	data.resCount -= 1;
	if( data.resCount < 0 )
		data.resCount = 0;

	Sacrifice_WriteData( pUser, data.lastGain, data.resCount, data.lastLoss );

	pSocket.SysMessage( GetDictionaryEntry( 30026, pSocket.language )); // You use the power of Sacrifice to return to life.
}

/** @type { ( pUser: Character ) => void } */
function Sacrifice_StartTarget( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	var level = TriggerEvent( 8003, "Virtue_GetLevel", pUser, VirtueName.Sacrifice );
	if( level >= VirtueLevel.Knight )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30027, pSocket.language )); // You have already attained the highest path in this virtue.
		return;
	}

	// Too little fame
	if( pUser.fame < 2500 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30028, pSocket.language )); // You do not have enough fame to sacrifice.
		return;
	}

	// Gain delay: approximately one day between sacrifices
	var now  = GetCurrentClock();
	var data = Sacrifice_ReadData( pUser );
	var lastGain = data.lastGain;

	if( now < ( lastGain + Sacrifice_GainDelayMs ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 30029, pSocket.language )); // You must wait approximately one day before sacrificing again.
		return;
	}

	var msg = GetDictionaryEntry( 30030, pSocket.language ); // Target the creature you wish to set free so that you may sacrifice your fame.
	pSocket.CustomTarget( 1, msg );
}

var Sacrifice_ValidSectionMap = {
	"lich": 1,
	"succubus": 1,
	"daemon": 1,
	"evil_mage": 1,
	"enslaved_gargoyle": 1,
	"gargoyle_enforcer": 1
	// add more here as needed
};

/** @type { ( targ: Character ) => boolean } */
function Sacrifice_ValidateCreature( targ )
{
	if( !ValidateObject( targ ) || !targ.isChar )
		return false;

	if( targ.tamed )
		return false;

	var sec = targ.sectionID;
	if( !sec )
		return false;

	sec = sec.toLowerCase();

	// Look up in the whitelist map
	return Sacrifice_ValidSectionMap[sec] === 1;
}

/** @type { ( pUser: Character, targ: Character ) => void } */
function Sacrifice_HandleTarget( pUser, targ )
{
	if( !ValidateObject( pUser ) || !ValidateObject( targ ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	// Alive check
	if( pUser.dead )
		return;

	// Hidden check
	if( pUser.visible !== 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30022, pSocket.language )); // You cannot do that while hidden.
		return;
	}

	// Already at highest path?
	var level = TriggerEvent( 8003, "Virtue_GetLevel", pUser, VirtueName.Sacrifice );
	if( level >= VirtueLevel.Knight )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30027, pSocket.language )); //You have already attained the highest path in this virtue.
		return;
	}

	// Fame check
	if( pUser.fame < 2500 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30028, pSocket.language )); // You do not have enough fame to sacrifice.
		return;
	}

	// Gain delay check
	var now  = GetCurrentClock();
	var data = Sacrifice_ReadData( pUser );
	var lastGain = data.lastGain;
	var resCount = data.resCount;

	if( now < ( lastGain + Sacrifice_GainDelayMs ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 30029, pSocket.language )); // You must wait approximately one day before sacrificing again.
		return;
	}

	// Validate appropriate creature
	if( !Sacrifice_ValidateCreature( targ ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 30031, pSocket.language )); // You cannot sacrifice your fame for that creature.
		return;
	}

	// Target must be mostly undamaged: hits >= 90% of max
	var hpPercent = 0;
	if( targ.maxhp > 0 )
		hpPercent = ( targ.health * 100 ) / targ.maxhp;

	if( hpPercent < 90 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30032, pSocket.language )); // You cannot sacrifice for this monster because it is too damaged.
		return;
	}

	// All checks passed: compute virtue gain based on fame
	var toGain;
	if( pUser.fame < 5000 )
		toGain = 500;
	else if( pUser.fame < 10000 )
		toGain = 1000;
	else
		toGain = 2000;

	// Sacrifice all fame
	pUser.fame = 0;

	// Flavor messages
	targ.TextMessage( "I have seen the error of my ways!" );
	pSocket.SysMessage( GetDictionaryEntry( 30033, pSocket.language )); // You have set the creature free.

	// Remove the creature
	targ.Delete();

	// Record last gain time
	data.lastGain = now;

	// Award Sacrifice virtue
	var result = TriggerEvent( 8003, "Virtue_Award", pUser, VirtueName.Sacrifice, toGain );

	if( result && result.success )
	{
		if( result.gainedPath )
		{
			pSocket.SysMessage( GetDictionaryEntry( 30034, pSocket.language )); // You have gained a path in Sacrifice!

			if( resCount < 3 )
				resCount += 1;
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 30035, pSocket.language )); // You have gained in Sacrifice.
		}
	}

	// Save updated data
	data.resCount = resCount;
	Sacrifice_WriteData( pUser, data.lastGain, data.resCount, data.lastLoss );

	// Same message as OSI after a successful sacrifice
	pSocket.SysMessage( GetDictionaryEntry( 30029, pSocket.language )); // You must wait approximately one day before sacrificing again.
}

/** @type { ( pUser: Character ) => void } */
function Sacrifice_CheckAtrophy( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	var now  = GetCurrentClock();
	var data = Sacrifice_ReadData( pUser );

	// Not time to decay yet
	if( now < ( data.lastLoss + Sacrifice_LossDelayMs ))
		return;

	// Time to decay Sacrifice
	var lost = TriggerEvent( 8003, "Virtue_Atrophy", pUser, VirtueName.Sacrifice, Sacrifice_LossAmount );
	if( lost )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30036, pSocket.language )); // You have lost some Sacrifice.
	}

	// Update available resurrects to match current path (max 3)
	var level = TriggerEvent( 8003, "Virtue_GetLevel", pUser, VirtueName.Sacrifice );
	var newResCount = level;
	if( newResCount > 3 )
		newResCount = 3;
	if( newResCount < 0 )
		newResCount = 0;

	data.resCount = newResCount;
	data.lastLoss = now;

	Sacrifice_WriteData( pUser, data.lastGain, data.resCount, data.lastLoss );
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, ourObj )
{
	if( socket == null )
		return;

	var pUser = socket.currentChar;

	// Cancelled target?
	if( socket.GetWord && socket.GetWord( 1 ))
	{
		return;
	}

	if( !ValidateObject( ourObj ) || !ourObj.isChar )
	{
		socket.SysMessage( GetDictionaryEntry( 30037, socket.language )); // You cannot sacrifice your fame for that.
		return;
	}

	Sacrifice_HandleTarget( pUser, ourObj );
}

/** @type { ( pUser: Character ) => { lastGain: number, resCount: number, lastLoss: number } } */
function Sacrifice_ReadData( pUser )
{
	var data = { lastGain: 0, resCount: 0, lastLoss: 0 };

	if( !ValidateObject( pUser ))
		return data;

	var raw = pUser.GetTag( "SacrificeGain" );

	if( raw && raw.length > 0 )
	{
		var parts = raw.split( "," );

		if( parts.length > 0 )
		{
			var g = Number( parts[0] );
			if( !isNaN( g ) && g > 0 )
				data.lastGain = g;
		}

		if( parts.length > 1 )
		{
			var r = Number( parts[1] );
			if( !isNaN( r ) && r >= 0 )
				data.resCount = r;
		}

		if( parts.length > 2 )
		{
			var l = Number( parts[2] );
			if( !isNaN( l ) && l > 0 )
				data.lastLoss = l;
		}
	}

	// Clamp resCount 0..3
	if( data.resCount < 0 )
		data.resCount = 0;
	if( data.resCount > 3 )
		data.resCount = 3;

	return data;
}

/** @type { ( pUser: Character, lastGain: number, resCount: number, lastLoss: number ) => void } */
function Sacrifice_WriteData( pUser, lastGain, resCount, lastLoss )
{
	if( !ValidateObject( pUser ))
		return;

	if( lastGain < 0 )
		lastGain = 0;
	if( lastLoss < 0 )
		lastLoss = 0;

	if( resCount < 0 )
		resCount = 0;
	if( resCount > 3 )
		resCount = 3;

	var val = lastGain.toString() + "," + resCount.toString() + "," + lastLoss.toString();
	pUser.SetTag( "SacrificeGain", val );
}
