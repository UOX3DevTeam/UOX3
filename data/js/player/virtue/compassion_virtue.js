/// <reference path="../definitions.d.ts" />
// @ts-check
//
// Compassion Virtue ability for UOX3
//
// Player resurrect:
//   - Seeker   -> resurrected at 20% max HP
//   - Follower -> resurrected at 40% max HP
//   - Knight   -> resurrected at 80% max HP
//
// Pet resurrect (bonded):
//   - Up to 20% of pet max HP from Veterinary / Animal Lore
//       (we approximate using internal skill values)
//   - Plus Compassion bonus:
//       Seeker:   +5%
//       Follower: +10%
//       Knight:   +20%
//   - Final HP bonus capped at 200 HP
//
// Young players cannot GAIN Compassion, but they CAN still
// use the HP bonus if you want (OSI does not block usage, only gain).
//
// This script expects the virtue helper at scriptID 8003, providing:
//   Virtue_GetLevel( pChar, virtueIndex )
//   Virtue_Award( pChar, virtueIndex, amount )
//   Virtue_Atrophy( pChar, virtueIndex, amount )
//   VirtueName.Compassion == 2
//
// Events used from other scripts:
//   TriggerEvent( 8009, "Compassion_OnPlayerResurrect", healer, target );
//   TriggerEvent( 8009, "Compassion_OnPetResurrect", healer, pet );
//   TriggerEvent( 8009, "Compassion_AwardEscort", pChar, isPrisonerQuest, isNewHavenEscort );
//


// Reuse shared enums if already defined
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

// How much Compassion we consume per resurrection use.
// You can tweak this to feel right.
var Compassion_UseCost = 400;

function Compassion_IsYoung( pChar )
{
	if( !ValidateObject( pChar ))
		return false;

	// If the Young system is disabled, just return false
	if( typeof youngPlayerSystem !== "undefined" && !youngPlayerSystem )
		return false;

	if( pChar.account && pChar.account.isYoung )
		return true;

	return false;
}

function Compassion_GetLevel( pChar )
{
	if( !ValidateObject( pChar ))
		return VirtueLevel.None;

	return TriggerEvent( 8003, "Virtue_GetLevel", pChar, VirtueName.Compassion ) | 0;
}

function Compassion_OnPlayerResurrect( healer, target )
{
	if( !ValidateObject( healer ) || !ValidateObject( target ))
		return;

	// Must be a player resurrecting another player (or self)
	if( target.npc )
		return;

	var level = Compassion_GetLevel( healer );
	if( level <= VirtueLevel.None )
		return;

	// Determine HP percent based on level
	var percent = 0;
	if( level >= VirtueLevel.Knight )
		percent = 80;
	else if( level >= VirtueLevel.Follower )
		percent = 40;
	else if( level >= VirtueLevel.Seeker )
		percent = 20;

	if( percent <= 0 )
		return;

	var maxHP = target.maxhp | 0;
	if( maxHP <= 0 )
		return;

	var newHP = Math.floor( maxHP * percent / 100 );
	if( newHP < 1 )
		newHP = 1;
	if( newHP > maxHP )
		newHP = maxHP;

	// Set HP after Resurrect() has set them to base value
	target.health = newHP;

	// Consume Compassion points for the healer
	TriggerEvent( 8003, "Virtue_Atrophy", healer, VirtueName.Compassion, Compassion_UseCost );
}

function Compassion_OnPetResurrect( healer, pet )
{
	if( !ValidateObject( healer ) || !ValidateObject( pet ))
		return;

	// Must be a pet, not a player
	if( !pet.npc )
		return;

	var level = Compassion_GetLevel( healer );

	// Base % from Veterinary + Animal Lore
	// Internal skills usually 0..1000 or 0..1200.
	var vetSkill  = healer.skills.veterinary | 0;
	var loreSkill = healer.skills.animallore | 0;

	// Clamp at 120.0 internally (1200)
	if( vetSkill > 1200 )  vetSkill = 1200;
	if( loreSkill > 1200 ) loreSkill = 1200;

	// 0..20% from combined Vet/Lore
	var basePercent = Math.floor((( vetSkill + loreSkill ) / 2400 ) * 20 );
	if( basePercent < 0 )
		basePercent = 0;
	if( basePercent > 20 )
		basePercent = 20;

	// Compassion bonus: Seeker 5, Follower 10, Knight 20
	var compBonus = 0;
	if( level >= VirtueLevel.Knight )
		compBonus = 20;
	else if( level >= VirtueLevel.Follower )
		compBonus = 10;
	else if( level >= VirtueLevel.Seeker )
		compBonus = 5;

	var totalPercent = basePercent + compBonus;

	if( totalPercent <= 0 )
		return;

	var maxHP = pet.maxhp | 0;
	if( maxHP <= 0 )
		return;

	// Calculate HP from percent, but cap at 200 HP total
	var hpFromPercent = Math.floor( maxHP * totalPercent / 100 );
	if( hpFromPercent < 1 )
		hpFromPercent = 1;
	if( hpFromPercent > 200 )
		hpFromPercent = 200;

	pet.health = hpFromPercent;

	// Consume Compassion points for the healer
	TriggerEvent( 8003, "Virtue_Atrophy", healer, VirtueName.Compassion, Compassion_UseCost );
}

function Compassion_CanGain( pChar )
{
	var result = { canGain: true, reason: "" };

	if( !ValidateObject( pChar ))
	{
		result.canGain = false;
		result.reason = "Invalid character.";
		return result;
	}

	// Young players cannot earn Compassion
	if( Compassion_IsYoung( pChar ))
	{
		result.canGain = false;
		result.reason = "Young players cannot gain Compassion.";
		return result;
	}

	var now = GetCurrentClock() | 0; // ms
	if( now <= 0 )
	{
		result.canGain = false;
		result.reason = "Invalid server time.";
		return result;
	}

	// Parse tag
	var gainTag = pChar.GetTag( "compassion_gain" );
	var lastGainTime = 0;
	var dayStamp = 0;
	var gainsToday = 0;

	if( gainTag && typeof gainTag === "string" )
	{
		var parts = gainTag.split( "," );
		if( parts.length >= 3 )
		{
			lastGainTime = Number( parts[0] ) | 0;
			dayStamp     = Number( parts[1] ) | 0;
			gainsToday   = Number( parts[2] ) | 0;
		}
	}

	// Day stamp: convert ms to days
	var curDay = Math.floor( now / ( 24 * 60 * 60 * 1000 ));

	// If new day, reset counter
	if( curDay !== dayStamp )
	{
		gainsToday = 0;
		dayStamp = curDay;
	}

	// Max 5 gains per day
	if( gainsToday >= 5 )
	{
		result.canGain = false;
		result.reason = "You have already gained Compassion too many times today.";
		return result;
	}

	// Require 5 minutes between gains
	var delayMs = 5 * 60 * 1000;
	if( now < ( lastGainTime + delayMs ))
	{
		result.canGain = false;
		result.reason = "You must wait a few minutes before gaining more Compassion.";
		return result;
	}

	// All good
	result.canGain = true;
	result.reason = "";
	return result;
}

function Compassion_AwardEscort( pChar, isPrisonerQuest, isNewHavenEscort )
{
	if( !ValidateObject( pChar ))
		return;

	if( isNewHavenEscort )
		return; // no Compassion from New Haven escorts

	var can = Compassion_CanGain( pChar );
	if( !can.canGain )
	{
		// Optional: message if you want
		// pChar.SysMessage( can.reason );
		return;
	}

	// Escort vs prisoner gain amounts
	var baseGain = 0;

	if( isPrisonerQuest )
	{
		// Prisoner quests give more
		baseGain = 1500;
	}
	else
	{
		// Town escort
		baseGain = 800;
	}

	// Actually award virtue
	var result = TriggerEvent( 8003, "Virtue_Award", pChar, VirtueName.Compassion, baseGain );

	if( result && result.success )
	{
		var msg = "";

		if( result.gainedPath )
			msg = "You have gained a path in Compassion!";
		else
			msg = "You have gained in Compassion!";

		pChar.SysMessage( msg );

		// Update gain tracking
		var now = GetCurrentClock() | 0;
		var gainTag = pChar.GetTag( "compassion_gain" );
		var lastGainTime = 0;
		var dayStamp = 0;
		var gainsToday = 0;

		if( gainTag && typeof gainTag === "string" )
		{
			var parts = gainTag.split( "," );
			if( parts.length >= 3 )
			{
				dayStamp   = Number( parts[1] ) | 0;
				gainsToday = Number( parts[2] ) | 0;
			}
		}

		// Recompute day stamp for now
		var curDay = Math.floor( now / ( 24 * 60 * 60 * 1000 ));
		if( curDay !== dayStamp )
		{
			dayStamp = curDay;
			gainsToday = 0;
		}

		gainsToday++;

		pChar.SetTag( "compassion_gain", now.toString() + "," + dayStamp.toString() + "," + gainsToday.toString() );
	}
}