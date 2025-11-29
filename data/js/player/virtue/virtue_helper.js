/// <reference path="../../definitions.d.ts" />
// @ts-check
//
// Virtue helper for UOX3
// Centralizes virtue storage and logic so all scripts use the same code.

// Enum-ish indices for virtues
var VirtueName = {
	Humility:     0,
	Sacrifice:    1,
	Compassion:   2,
	Spirituality: 3,
	Valor:        4,
	Honor:        5,
	Justice:      6,
	Honesty:      7
};

var VirtueLevel = {
	None:     0,
	Seeker:   1,
	Follower: 2,
	Knight:   3
};

function Virtue_ClampIndex( idx )
{
	if( idx < 0 ) return 0;
	if( idx > 7 ) return 7;
	return idx;
}

// Storage: virtue0..virtue7 tags on the character
function Virtue_GetValue( pChar, virtueIndex )
{
	if( !ValidateObject( pChar ))
		return 0;

	virtueIndex = Virtue_ClampIndex( virtueIndex );

	var tagName = "virtue" + virtueIndex;
	var raw = pChar.GetTag( tagName );
	if( raw === null || raw === undefined )
		return 0;

	var num = Number( raw );
	if( isNaN( num ))
		return 0;

	return num;
}

function Virtue_SetValue( pChar, virtueIndex, value )
{
	if( !ValidateObject( pChar ))
		return;

	virtueIndex = Virtue_ClampIndex( virtueIndex );
	if( value < 0 )
		value = 0;

	var tagName = "virtue" + virtueIndex;
	pChar.SetTag( tagName, value );
}

function Virtue_GetMaxAmount( virtueIndex )
{
	virtueIndex = Virtue_ClampIndex( virtueIndex );

	if( virtueIndex === VirtueName.Honor )
		return 20000;

	if( virtueIndex === VirtueName.Sacrifice )
		return 22000;

	return 21000;
}

function Virtue_GetLevel( pChar, virtueIndex )
{
	var v    = Virtue_GetValue( pChar, virtueIndex );
	var vmax = Virtue_GetMaxAmount( virtueIndex );
	var vl;

	if( v < 4000 )
	{
		vl = VirtueLevel.None;
	}
	else if( v >= vmax )
	{
		vl = VirtueLevel.Knight;
	}
	else
	{
		vl = Math.floor(( v + 10000 ) / 10000 ); // 1 or 2 here
		if( vl < VirtueLevel.Seeker )
			vl = VirtueLevel.Seeker;
		if( vl > VirtueLevel.Follower )
			vl = VirtueLevel.Follower;
	}

	return vl;
}

function Virtue_HasAny( pChar, virtueIndex )
{
	return Virtue_GetValue( pChar, virtueIndex ) > 0;
}

function Virtue_IsHighestPath( pChar, virtueIndex )
{
	return Virtue_GetValue( pChar, virtueIndex ) >= Virtue_GetMaxAmount( virtueIndex );
}

function Virtue_IsSeeker( pChar, virtueIndex )
{
	return Virtue_GetLevel( pChar, virtueIndex ) >= VirtueLevel.Seeker;
}

function Virtue_IsFollower( pChar, virtueIndex )
{
	return Virtue_GetLevel( pChar, virtueIndex ) >= VirtueLevel.Follower;
}

function Virtue_IsKnight( pChar, virtueIndex )
{
	return Virtue_GetLevel( pChar, virtueIndex ) >= VirtueLevel.Knight;
}

function Virtue_GetHue( pChar, virtueIndex )
{
	var virtueHueTable = [
		0x0481, 0x0963, 0x0965, // Humility
		0x060A, 0x060F, 0x002A, // Sacrifice
		0x08A4, 0x08A7, 0x0034, // Compassion
		0x0965, 0x08FD, 0x0480, // Spirituality
		0x00EA, 0x0845, 0x0020, // Valor
		0x0011, 0x0269, 0x013D, // Honor
		0x08A1, 0x08A3, 0x0042, // Justice
		0x0543, 0x0547, 0x0061  // Honesty
	];

	var value = Virtue_GetValue( pChar, virtueIndex );

	if( value === 0 )
		return 2402;

	if( value < 4000 )
		return 2402;

	// sanity cap like RunUO
	if( value >= 30000 )
		value = 30000;

	var vl;

	if( value < 10000 )
	{
		vl = 0;
	}
	else if( value >= 20000 && virtueIndex === VirtueName.Honor )
	{
		vl = 2;
	}
	else if( value >= 21000 && virtueIndex !== VirtueName.Sacrifice )
	{
		vl = 2;
	}
	else if( value >= 22000 && virtueIndex === VirtueName.Sacrifice )
	{
		vl = 2;
	}
	else
	{
		vl = 1;
	}

	var tableIndex = (virtueIndex * 3) + vl;
	return virtueHueTable[tableIndex] || 2402;
}

function Virtue_Award( pChar, virtueIndex, amount )
{
	var result = {
		success:    false,
		gainedPath: false,
		oldLevel:   VirtueLevel.None,
		newLevel:   VirtueLevel.None,
		oldValue:   0,
		newValue:   0
	};

	if( !ValidateObject( pChar ))
		return result;

	virtueIndex = Virtue_ClampIndex( virtueIndex );

	var current   = Virtue_GetValue( pChar, virtueIndex );
	var maxAmount = Virtue_GetMaxAmount( virtueIndex );

	if( current >= maxAmount )
		return result; // already capped

	// If you add a VirtueShield later, modify amount here.

	if( (current + amount) >= maxAmount )
		amount = maxAmount - current;

	var oldLevel = Virtue_GetLevel( pChar, virtueIndex );
	var newValue = current + amount;

	Virtue_SetValue( pChar, virtueIndex, newValue );

	var newLevel = Virtue_GetLevel( pChar, virtueIndex );
	var gained   = (newLevel !== oldLevel);

	result.success    = true;
	result.gainedPath = gained;
	result.oldLevel   = oldLevel;
	result.newLevel   = newLevel;
	result.oldValue   = current;
	result.newValue   = newValue;

	// If you want an event:
	// if( gained ) TriggerEvent( 7605, "OnVirtueLevelChange", pChar, oldLevel, newLevel, virtueIndex );

	return result;
}

// Atrophy
function Virtue_Atrophy( pChar, virtueIndex, amount )
{
	if( !ValidateObject( pChar ))
		return false;

	virtueIndex = Virtue_ClampIndex( virtueIndex );

	var current = Virtue_GetValue( pChar, virtueIndex );
	var hadAny  = (current > 0);
	var newVal  = current - amount;

	if( newVal < 0 )
		newVal = 0;

	Virtue_SetValue( pChar, virtueIndex, newVal );
	return hadAny;
}