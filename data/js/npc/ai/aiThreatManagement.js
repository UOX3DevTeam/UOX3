/// <reference path="../../definitions.d.ts" />
// @ts-check
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( !ValidateObject( attacker ) || damaged == attacker )
		return;

	// Initialize threat table if it doesn't already exist
	if( !damaged.threatTable )
	{
		damaged.threatTable = {};
	}

	// Initialize threat from current attacker, if not already in threat table
	if( !damaged.threatTable[attacker.serial] )
	{
		damaged.threatTable[attacker.serial] = 0;
	}

	// Add attacker's damage to threat table at a 1:1 ratio
	damaged.threatTable[attacker.serial] += damageValue;

	Console.Print( attacker.name + " generated " + damageValue + " threat against " + damaged.name + "\n" );
}