// This event runs instead of regular, hard-coded target selection for AI_EVIL

const monstersVsAnimals = GetServerSetting( "MonstersVsAnimals" );
const animalAttackChance = GetServerSetting( "AnimalAttackChance" );

function onAICombatTarget( pChar, pTarget )
{
	if( !ValidateObject( pTarget ))
		return false;

	// Don't allow if target has AI_HEALER_G, or is an animal and monsters vs animals is disabled
	if( pTarget.aitype == 1
		|| ( pTarget.isAnimal && !ValidateObject( pTarget.owner ) && ( !monstersVsAnimals || animalAttackChance < RandomNumber( 1, 1000 ))))
		return false;

	// 1% chance to turn on own race
	if( !ValidateObject( pTarget.owner ) && pChar.race.id == pTarget.race.id && RandomNumber( 1, 100 ) > 1 )
		return false;

	// Don't attack racial allies
	var raceCompare = RaceCompareByRace( pChar.race.id, pTarget.race.id );
	if( !ValidateObject( pTarget.owner ) && raceCompare >= 1 )
		return false;

	// Don't attack other monsters, unless racial enemies
	if( !ValidateObject( pTarget.owner ) && (( pChar.aitype == 2 || pChar.aitype == 11 ) && ( pTarget.aitype == 2 || pTarget.aitype == 11 ) && raceCompare >= 0 ))
		return false;

	// See if pTarget is wearing an orcish kin mask
	var headWear = pTarget.FindItemLayer( 0x06 );
	if( ValidateObject( headWear ) )
	{
		if( headWear.id == 0x141c && headWear.colour == 0x08a4 )
			return false;
	}

	// 85% chance to attack current target, 15% chance to pick another
	if( RandomNumber( 1, 100 ) >= 85 )
		return false;

	// Otherwise, assume valid target
	return true;
}

function onDamage( pDefender, pAttacker, damageValue, damageType )
{
	if( !ValidateObject( pAttacker ))
		return false;

	// Check if the attacker is actually a pet/hireling/summoned creature
	var pOwner = pAttacker.owner;
	if( !ValidateObject( pOwner ))
	{
		pOwner = pAttacker;
	}

	ExplodeOrcishMask( pOwner );

	return true;
}

function ExplodeOrcishMask( pChar )
{
	var headWear = pChar.FindItemLayer( 0x06 );
	if( ValidateObject( headWear ))
	{
		if( headWear.id == 0x141c && headWear.colour == 0x08a4 )
		{
			// Punish pChar for negative actions towards orc while wearing a mask of orcish kin!
			pChar.ExplodeItem( headWear, 50, 5, false );
		}
	}
}