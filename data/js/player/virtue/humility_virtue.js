/// <reference path="../../definitions.d.ts" />
// @ts-check

var VirtueName = VirtueName || {
	Humility:     0,
	Sacrifice:    1,
	Compassion:   2,
	Spirituality: 3,
	Valor:        4,
	Honor:        5,
	Justice:      6,
	Honesty:      7
};

var VirtueLevel = VirtueLevel || {
	None:     0,
	Seeker:   1,
	Follower: 2,
	Knight:   3
};

function Humility_StartPetTarget( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( !pSocket )
		return;

	var level = TriggerEvent( 8003, "Virtue_GetLevel", pUser, VirtueName.Humility );
	if( level < VirtueLevel.Seeker )
	{
		pUser.SysMessage( "You must be at least a Seeker of Humility to invoke this ability." );
		return;
	}
	var msg = "Target the pet you wish to embrace with your Humility.";
	pSocket.CustomTarget( 0, msg );
}

function Humility_OnVirtueUsed( pUser, pet )
{
	if( !ValidateObject( pUser ) || !ValidateObject( pet ))
		return;

	// Check virtue level (cheap double-check)
	var level = TriggerEvent( 8003, "Virtue_GetLevel", pUser, VirtueName.Humility );
	if( level < VirtueLevel.Seeker )
	{
		pUser.SysMessage( "You must be at least a Seeker of Humility to invoke this ability." );
		return;
	}

	// Reject dead pets
	if( pet.GetTag( "isPetDead" ) == true)
	{
		pUser.SysMessage( "You cannot embrace Humility on the dead!" );
		return;
	}

	// Already buffed?
	if( pet.GetTag( "humility_pet_owner" ) !== null )
	{
		pUser.SysMessage( "That pet has already embraced Humility." );
		return;
	}

	// Spend some Humility points
	TriggerEvent( 8003, "Virtue_Atrophy", pUser, VirtueName.Humility, 3200 );
	pUser.SysMessage( "You have lost some Humility." );

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

	pet.SetTag( "humility_hpr_base", curHpr );

	// Apply new bonus (additive)
	pet.healthRegenBonus = curHpr + hprBonus;

	// Mark pet as embraced and owner linked
	pet.SetTag( "humility_pet_owner", pUser.serial );
	pUser.SetTag( "humility_pet_active", 1 );

	// Visual / text feedback
	pet.TextMessage( "*Your pet surges with the power of your Humility!*" );
	// Start 20 minute buff timer on the pet (20 * 60 * 1000 = 1,200,000 ms)
	pet.StartTimer( 1200000, 5002, 8005 );
}

function Humility_ToggleHunt( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	// Check alive state if you have a way; for now just proceed.
	var state = pUser.GetTag( "humility_hunt_state" ); // "active", "expiring", or null

	if( state === "active" )
	{
		// First time saying it -> start expiring
		var exp = pUser.GetTag( "humility_hunt_expiring" ) | 0;
		if( exp === 0 )
		{
			pUser.SetTag( "humility_hunt_expiring", 1 );
			pUser.SetTag( "humility_hunt_state", "expiring" );

			pUser.SysMessage( "You have ended your journey on the Path of Humility." );

			// After 30 seconds, fully clear the hunt data
			pUser.StartTimer( 30000, 5001, 8005 );
		}
		else
		{
			pUser.SysMessage( "You have already ended your journey on the Path of Humility. You must wait before you restart your path." );
		}
	}
	else if( state === "expiring" )
	{
		// Already expiring; tell them to wait
		pUser.SysMessage( "You have already ended your journey on the Path of Humility. You must wait before you restart your path." );
	}
	else
	{
		// Before starting a new hunt, check cooldown
		var cd = pUser.GetTag("humility_hunt_cooldown");
		if (cd !== null && cd !== undefined)
		{
			var now = GetCurrentClock();
			if (now < Number(cd))
			{
				pUser.SysMessage("You must wait before you can begin another Humility Hunt.");
				return;
			}
		}

		Humility_StartHunt(pUser);
	}
}

function Humility_StartHunt( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	pUser.SetTag( "humility_hunt_state", "active" );
	pUser.SetTag( "humility_hunt_expiring", 0 );

	// Apply resist debuff to the player
	Humility_ApplyResistDebuff( pUser );

	pUser.SysMessage( "You have begun your journey on the Path of Humility. Your resists have been debuffed by 70." );
	pUser.SysMessage( "You are now on a Humility Hunt. For each kill while you forgo the protection of resists, you shall continue on your path to Humility. You may end your Hunt by speaking \"Lum Lum Lum\" at any time." );

	// Optional: visual indicator
	pUser.TextMessage( "*You feel your defenses weaken as you follow the path of Humility.*" );
}

function Humility_OnHuntExpired( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	// Clear hunt tags
	// Restore original resists
	Humility_ClearResistDebuff( pUser );

	pUser.SetTag( "humility_hunt_state", null );
	pUser.SetTag( "humility_hunt_expiring", null );

	// set cooldown: store "next allowed" time in ms
	var now = GetCurrentClock(); // whatever you use for ms timestamp
	pUser.SetTag( "humility_hunt_cooldown", now + 60000 ); // 60s
	pUser.SysMessage( "Your time on the Path of Humility has ended." );
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
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
		var ownerSerial = pet.GetTag( "humility_pet_owner" );

		// Restore original healthRegenBonus
		var baseHpr = pet.GetTag( "humility_hpr_base" );
		if( baseHpr !== null && baseHpr !== undefined )
		{
			var val = Number( baseHpr );
			if( isNaN( val ))
				val = 0;

			pet.healthRegenBonus = val;
		}

		// Clear tags
		pet.SetTag( "humility_hpr_base", null );
		pet.SetTag( "humility_pet_owner", null );

		if( ownerSerial !== null && ownerSerial !== undefined )
		{
			var owner = CalcCharFromSer( Number( ownerSerial ));
			if( ValidateObject( owner ))
			{
				owner.SysMessage( "Your pet's power returns to normal." );
				owner.SetTag( "humility_pet_active", null );
			}
		}

		pet.TextMessage( "*Your humility-empowered vigor fades away.*" );
		return;
	}
}

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
		pUser.SysMessage( "You can only embrace your Humility on a pet." );
		return;
	}

	var pet = ourObj;

	// Already embraced?
	if( pet.GetTag( "humility_pet_owner" ) !== null )
	{
		pUser.SysMessage( "That pet has already embraced Humility." );
		return;
	}

	// Delegate to the actual ability logic
	Humility_OnVirtueUsed( pUser, pet );
}

// Which resist indices to debuff for Humility Hunt
// Based on your snippet: 1=armor, 4=cold, 5=fire, 6=energy, 7=poison
var HUMILITY_RESIST_INDICES = [ 1 ];
var HUMILITY_DEBUFF_AMOUNT  = 70;

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
	pChar.TextMessage( "*Your defenses are weakened by your Humility Hunt.*" );
}

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
	pChar.TextMessage( "*Your defenses return to normal.*" );
}