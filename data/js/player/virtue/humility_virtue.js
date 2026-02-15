/// <reference path="../definitions.d.ts" />
// @ts-check

var virtueEnums = TriggerEvent( 8003, "Virtue_GetEnums" );
var VirtueName  = virtueEnums ? virtueEnums.VirtueName  : null;
var VirtueLevel = virtueEnums ? virtueEnums.VirtueLevel : null;

/** @type { ( pUser: Character ) => void } */
function Humility_StartPetTarget( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null)
		return;

	var level = TriggerEvent( 8003, "Virtue_GetLevel", pUser, VirtueName.Humility );
	if( level < VirtueLevel.Seeker )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30003, pSocket.language )); // You must be at least a Seeker of Humility to invoke this ability.
		return;
	}
	var msg = GetDictionaryEntry( 30004, pSocket.language ); // Target the pet you wish to embrace with your Humility.
	pSocket.CustomTarget( 0, msg );
}

/** @type { ( pUser: Character, pet: Character ) => void } */
function Humility_OnVirtueUsed( pUser, pet )
{
	if( !ValidateObject( pUser ) || !ValidateObject( pet ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	// Check virtue level (cheap double-check)
	var level = TriggerEvent( 8003, "Virtue_GetLevel", pUser, VirtueName.Humility );
	if( level < VirtueLevel.Seeker )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30003, pSocket.language )); // You must be at least a Seeker of Humility to invoke this ability.
		return;
	}

	// Reject dead pets
	if( pet.GetTag( "isPetDead" ) == true)
	{
		pSocket.SysMessage( GetDictionaryEntry( 30005, pSocket.language )); // You cannot embrace Humility on the dead!
		return;
	}

	// Already buffed?
	if( pet.GetTag( "humility_pet_owner" ) !== null )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30006, pSocket.language )); // That pet has already embraced Humility.
		return;
	}

	// Spend some Humility points
	TriggerEvent( 8003, "Virtue_Atrophy", pUser, VirtueName.Humility, 3200 );
	pSocket.SysMessage( GetDictionaryEntry( 30007, pSocket.language )); // You have lost some Humility.

	// Compute HPR bonus based on path
	var hprBonus = 0;
	if( level >= VirtueLevel.Knight )
		hprBonus = 30;
	else if( level >= VirtueLevel.Follower )
		hprBonus = 20;
	else if( level >= VirtueLevel.Seeker )
		hprBonus = 10;

	// Store current HPR so we can restore later
	var curHpr = pet.healthRegenBonus;
	if( typeof curHpr !== "number" )
		curHpr = 0;

	Humility_WritePetData( pet, pUser.serial, curHpr );

	// Apply new bonus (additive)
	pet.healthRegenBonus = curHpr + hprBonus;

	pUser.SetTag( "humility_pet_active", 1 );

	// Visual / text feedback
	pet.TextMessage( "*Your pet surges with the power of your Humility!*" ); // *Your pet surges with the power of your Humility!*
	// Start 20 minute buff timer on the pet (20 * 60 * 1000 = 1,200,000 ms)
	pet.StartTimer( 1200000, 5002, 8005 );
}

/** @type { ( pUser: Character ) => void } */
function Humility_ToggleHunt( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	// Check alive state if you have a way; for now just proceed.
	var state = pUser.GetTag( "humility_hunt_state" ); // "active", "expiring", or null

	var data = Humility_ReadHuntData( pUser );

	// state: 0=none, 1=active, 2=expiring
	if( data.state === 1 )
	{
		// First time saying it -> start expiring
		if( data.expiring === 0 )
		{
			data.state    = 2; // expiring
			data.expiring = 1;

			Humility_WriteHuntData( pUser, data.state, data.expiring, data.cooldownUntil );

			pSocket.SysMessage( GetDictionaryEntry( 30009, pSocket.language )); // You have ended your journey on the Path of Humility.

			// After 30 seconds, fully clear the hunt data
			pUser.StartTimer( 30000, 5001, 8005 );
		}
		else
		{
			//You have already ended your journey on the Path of Humility. You must wait before you restart your path.
			pSocket.SysMessage( GetDictionaryEntry( 30010, pSocket.language ));
		}
	}
	else if( data.state === 2 )
	{
		// You have already ended your journey on the Path of Humility. You must wait before you restart your path.
		pSocket.SysMessage( GetDictionaryEntry( 30010, pSocket.language ));
	}
	else
	{
		// Before starting a new hunt, check cooldown
		var now = GetCurrentClock();
		if( data.cooldownUntil && now < data.cooldownUntil )
		{
			pSocket.SysMessage( GetDictionaryEntry( 30011, pSocket.language )); // You must wait before you can begin another Humility Hunt.
			return;
		}

		Humility_StartHunt( pUser );
	}
}

/** @type { ( pUser: Character ) => void } */
function Humility_StartHunt( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	// state=1 (active), expiring=0, cooldownUntil=0
	Humility_WriteHuntData( pUser, 1, 0, 0 );

	// Apply resist debuff to the player
	Humility_ApplyResistDebuff( pUser );
	pUser.AddScriptTrigger(8006); // humility_registerkill.js

	pSocket.SysMessage( GetDictionaryEntry( 30012, pSocket.language ) ); // You have begun your journey on the Path of Humility. Your resists have been debuffed by 70.
	//You are now on a Humility Hunt. For each kill while you forgo the protection of resists, you shall continue on your path to Humility. You may end your Hunt by speaking \"Lum Lum Lum\" at any time.
	pSocket.SysMessage( GetDictionaryEntry( 30013, pSocket.language ));

	// Optional: visual indicator
	pUser.TextMessage( "*You feel your defenses weaken as you follow the path of Humility.*" ); // *You feel your defenses weaken as you follow the path of Humility.*
}

/** @type { ( pUser: Character ) => void } */
function Humility_OnHuntExpired( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	Humility_ClearResistDebuff( pUser );
	pUser.RemoveScriptTrigger(8006); // humility_registerkill.js

	var now = GetCurrentClock(); // ms timestamp
	var cooldownUntil = now + 60000; // 60s cooldown

	// state back to 0 (none), expiring=0, set cooldown
	Humility_WriteHuntData( pUser, 0, 0, cooldownUntil );

	pSocket.SysMessage( GetDictionaryEntry( 30015, pSocket.language )); // Your time on the Path of Humility has ended.
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( timerObj, timerID )
{
	if( !timerObj )
		return;

	// 5001: Humility Hunt expire
	if( timerID === 5001 )
	{
		// timerObj should be the player who had their hunt expiring
		Humility_OnHuntExpired( timerObj );
		return;
	}

	// 5002: Pet Humility buff expire
	if( timerID === 5002 )
	{
		var pet = timerObj;
		var data = Humility_ReadPetData( pet );

		// Restore original healthRegenBonus
		var baseHpr = data.baseHpr;
		if( typeof baseHpr !== "number" || isNaN( baseHpr ))
			baseHpr = 0;

		pet.healthRegenBonus = baseHpr;

		// Clear combined pet tag
		pet.SetTag( "HumilityPet", null );

		if( data.ownerSerial && data.ownerSerial > 0 )
		{
			var owner = CalcCharFromSer( Number( data.ownerSerial ));
			if( ValidateObject( owner ))
			{
				var pSocket = owner.socket;
				if( !pSocket )
					return;
				pSocket.SysMessage( GetDictionaryEntry( 30016, pSocket.language )); // Your pet's power returns to normal.
				owner.SetTag( "humility_pet_active", null );
			}
		}

		pet.TextMessage( "*Your humility-empowered vigor fades away.*" ); // *Your humility-empowered vigor fades away.*
		return;
	}
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, ourObj )
{
	if( !socket || !socket.currentChar )
		return;

	var pUser = socket.currentChar;

	// Ensure valid target
	// socket.GetWord(1) is "cancel" flag in the docs example:
	// if 0 = OK, non-zero = cancelled.
	if( socket.GetWord && socket.GetWord( 1 ))
	{
		// Target cancelled
		return;
	}

	if( !ValidateObject( ourObj ) || !ourObj.isChar )
	{
		socket.SysMessage( GetDictionaryEntry( 30018, socket.language )); // You can only embrace your Humility on a pet.
		return;
	}

	var pet = ourObj;
	var data = Humility_ReadPetData( pet );
	if( data.ownerSerial && data.ownerSerial > 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 30019, socket.language )); //That pet has already embraced Humility.
		return;
	}

	// Delegate to the actual ability logic
	Humility_OnVirtueUsed( pUser, pet );
}

// Which resist indices to debuff for Humility Hunt
// Based on your snippet: 1=armor, 4=cold, 5=fire, 6=energy, 7=poison
var HUMILITY_RESIST_INDICES = [ 1 ];
var HUMILITY_DEBUFF_AMOUNT  = 70;
/** @type { ( pChar: Character ) => void } */
function Humility_ApplyResistDebuff( pChar )
{
	if( !ValidateObject( pChar ))
		return;

	// If already debuffed, do not stack
	if( pChar.GetTag( "humility_resist_debuffed" ) === 1 )
		return;

	for( var i = 0; i < HUMILITY_RESIST_INDICES.length; i++ )
	{
		var idx = HUMILITY_RESIST_INDICES[i];

		// Get current resist value
		var curVal = pChar.Resist( idx ); // getter
		if( typeof curVal !== "number" )
			curVal = 0;

		// Store original in tag so we can restore later
		pChar.SetTag( "humility_resist_orig_" + idx, curVal );

		// Apply debuff
		var newVal = curVal - HUMILITY_DEBUFF_AMOUNT;
		if( newVal < 0 )
			newVal = 0;

		pChar.Resist( idx, newVal ); // setter
	}

	pChar.SetTag( "humility_resist_debuffed", 1 );
	pChar.TextMessage( "*Your defenses are weakened by your Humility Hunt.*" ); // *Your defenses are weakened by your Humility Hunt.*
}

/** @type { ( pChar: Character ) => void } */
function Humility_ClearResistDebuff( pChar )
{
	if( !ValidateObject( pChar ))
		return;

	if( pChar.GetTag( "humility_resist_debuffed" ) !== 1 )
		return;

	for( var i = 0; i < HUMILITY_RESIST_INDICES.length; i++ )
	{
		var idx = HUMILITY_RESIST_INDICES[i];

		var orig = pChar.GetTag( "humility_resist_orig_" + idx );
		if( orig !== null && orig !== undefined )
		{
			var val = Number( orig );
			if( isNaN( val ))
				val = 0;

			pChar.Resist( idx, val );
		}

		// Clear per-resist tags
		pChar.SetTag( "humility_resist_orig_" + idx, null );
	}

	pChar.SetTag( "humility_resist_debuffed", null );
	pChar.TextMessage( "*Your defenses return to normal.*" ); // *Your defenses return to normal.*
}

/** @type { ( pet: Character ) => { ownerSerial: number, baseHpr: number } } */
function Humility_ReadPetData( pet )
{
	var data = { ownerSerial: 0, baseHpr: 0 };

	if( !ValidateObject( pet ))
		return data;

	var raw = pet.GetTag( "HumilityPet" );
	if( raw && raw.length > 0 )
	{
		var parts = raw.split( "," );

		if( parts.length > 0 )
		{
			var s = Number( parts[0] );
			if( !isNaN( s ) && s > 0 )
				data.ownerSerial = s;
		}

		if( parts.length > 1 )
		{
			var h = Number( parts[1] );
			if( !isNaN( h ))
				data.baseHpr = h;
		}
	}
	else
	{
		// Backwards compat: read old tags once if they exist
		var oldOwner = Number( pet.GetTag( "humility_pet_owner" ));
		if( !isNaN( oldOwner ) && oldOwner > 0 )
			data.ownerSerial = oldOwner;

		var oldHpr = Number( pet.GetTag( "humility_hpr_base" ));
		if( !isNaN( oldHpr ))
			data.baseHpr = oldHpr;
	}

	if( data.baseHpr < 0 )
		data.baseHpr = 0;

	return data;
}

/** @type { ( pet: Character, ownerSerial: number, baseHpr: number ) => void } */
function Humility_WritePetData( pet, ownerSerial, baseHpr )
{
	if( !ValidateObject( pet ))
		return;

	if( ownerSerial < 0 )
		ownerSerial = 0;
	if( baseHpr < 0 )
		baseHpr = 0;

	var val = ownerSerial.toString() + "," + baseHpr.toString();
	pet.SetTag( "HumilityPet", val );
}

// Humility hunt data stored on the player:
// HumilityHunt = "state,expiring,cooldownUntil"
//   state: 0=none, 1=active, 2=expiring
//   expiring: 0/1
//   cooldownUntil: ms timestamp, 0 if none
/** @type { ( pUser: Character ) => { state: number, expiring: number, cooldownUntil: number } } */
function Humility_ReadHuntData( pUser )
{
	var data = { state: 0, expiring: 0, cooldownUntil: 0 };

	if( !ValidateObject( pUser ))
		return data;

	var raw = pUser.GetTag( "HumilityHunt" );
	if( raw && raw.length > 0 )
	{
		var parts = raw.split( "," );

		if( parts.length > 0 )
		{
			var st = Number( parts[0] );
			if( !isNaN( st ) && st >= 0 )
				data.state = st;
		}

		if( parts.length > 1 )
		{
			var ex = Number( parts[1] );
			if( !isNaN( ex ) && ex >= 0 )
				data.expiring = ex;
		}

		if( parts.length > 2 )
		{
			var cd = Number( parts[2] );
			if( !isNaN( cd ) && cd > 0 )
				data.cooldownUntil = cd;
		}
	}

	// Clamp
	if( data.state < 0 || data.state > 2 )
		data.state = 0;
	if( data.expiring < 0 )
		data.expiring = 0;
	if( data.expiring > 1 )
		data.expiring = 1;
	if( data.cooldownUntil < 0 )
		data.cooldownUntil = 0;

	return data;
}

/** @type { ( pUser: Character, state: number, expiring: number, cooldownUntil: number ) => void } */
function Humility_WriteHuntData( pUser, state, expiring, cooldownUntil )
{
	if( !ValidateObject( pUser ))
		return;

	if( state < 0 || state > 2 )
		state = 0;
	if( expiring < 0 )
		expiring = 0;
	if( expiring > 1 )
		expiring = 1;
	if( cooldownUntil < 0 )
		cooldownUntil = 0;

	var val = state.toString() + "," + expiring.toString() + "," + cooldownUntil.toString();
	pUser.SetTag( "HumilityHunt", val );
}
