const monstersVsAnimals = GetServerSetting( "MonstersVsAnimals" );
const animalAttackChance = GetServerSetting( "AnimalAttackChance" );

// Override target selection to make sure they don't attack players with tribal paint applied
function onAICombatTarget( savageNpc, pTarget )
{
	if( !ValidateObject( pTarget ))
		return false;

	// Don't allow if target has AI_HEALER_G, or is an animal and monsters vs animals is disabled
	if( pTarget.aitype == 1
		|| ( pTarget.isAnimal && ( !monstersVsAnimals || animalAttackChance < RandomNumber( 1, 1000 ))))
		return false;

	// Don't attack racial allies
	var raceCompare = RaceCompareByRace( savageNpc.race.id, pTarget.race.id );
	if( raceCompare >= 1 )
		return false;

	// Don't attack other monsters, unless racial enemies
	if(( savageNpc.aitype == 2 || savageNpc.aitype == 11 ) && ( pTarget.aitype == 2 || pTarget.aitype == 11 ) && raceCompare >= 0 )
		return false;

	// See if pTarget is wearing savage kin paint kit
	if( pTarget.id == 0x00b7 || pTarget.id == 0x00b8 )
	{
		return false;
	}

	// 85% chance to attack current target, 15% chance to pick another
	if( RandomNumber( 1, 100 ) >= 85 )
		return false;

	// Otherwise, assume valid target
	return true;
}

// Remove tribal paint from attacker if applicable
function onDamage( savageNpc, pAttacker, damageValue, damageType )
{
	if( !ValidateObject( pAttacker ))
		return true;

	// Check if the attacker is actually a pet/hireling/summoned creature
	var pOwner = pAttacker.owner;
	if( !ValidateObject( pOwner ))
	{
		pOwner = pAttacker;
	}

	// See if character (or their owner) is wearing savage paint
	if( pOwner.id == 0x00b7 || pOwner.id == 0x00b8 )
	{
		// Punish character (or their owner) for attacking a Savage while wearing savage paint
		// Play FX of attacker's savage paint burning
		pOwner.StaticEffect( 0x3709, 9, 9 );
		pOwner.SoundEffect( 0x307, true );

		// Deal damage to attacker
		pOwner.Damage( 50, 5 );

		// Reset attacker's body to its original ID and hue
		pOwner.id = pOwner.orgID;
		pOwner.colour = pOwner.orgSkin;

		if( pOwner.socket )
		{
			pOwner.socket.SysMessage( GetDictionaryEntry( 1675, pOwner.socket.language )); // Your skin is scorched as the tribal paint burns away!
		}
	}
	return true;
}

// List of creatures that Savages will deal extra damage to
const bonusDamageList = [ "graydragon", "reddragon", "graydrake", "reddrake", "shadowwyrm",
		"whitewyrm", "ancientwyrm", "wyvern", "serpentinedragon", "skeletaldragon", "swampdragon",
		"nightmare", "darknightmare", "manenightmare", "purenightmare", "daemon-summon" ];

// Override combat damage calculations to ensure Savages can deal more damage against certain creatures
function onCombatDamageCalc( pAttacker, pDefender, fightSkill, hitLoc )
{
	// Get attacker's base damage
	var baseDamage = pAttacker.attack;

	if( baseDamage == -1 )  // No damage if weapon breaks
		return 0;

	// Apply hard-coded damage bonuses based on race weakness, character skills and stats, etc
	var damage = ApplyDamageBonuses( 1, pAttacker, pDefender, fightSkill, hitLoc, baseDamage );

	if( damage < 1 )
		return 0;

	// Apply hard-coded defense modifiers based on armor, resistances etc
	damage = ApplyDefenseModifiers( 1, pAttacker, pDefender, fightSkill, hitLoc, damage, true);

	// If damage after defense modifiers is below 0, do a small random amount of damage still
	if( damage <= 0 )
	{
		damage = RandomNumber( 0, 4 );
	}

	// Half remaining damage by 2 if PUB15 or earlier
	damage /= 2;

	// If defender is a player, damage is divided by this modifier from uox.ini (modifier defaults to 1)
	if( !pDefender.npc )
	{
		damage /= GetServerSetting( "NpcDamageRate" );
	}

	// Apply Savage damage bonuses against large tamed animals
	var sectID = pDefender.sectionID;
	if( bonusDamageList.indexOf( pDefender.sectionID ))
	{
		damage *= 3;
	}

	return damage;
}