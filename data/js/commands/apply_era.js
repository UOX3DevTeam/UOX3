// This command is used to load pre-defined .INI settings for specific UO eras
// Server restart and/or additional changes in other files might be needed
// to fully embrace specific eras
function CommandRegistration()
{
	RegisterCommand( "applyera", 8, true );
}

const eraSettings_validEras = [ "uo", "t2a", "uor", "td", "lbr", "aos", "se", "ml", "sa", "hs", "tol" ];

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_APPLYERA( socket, cmdString )
{
	if( cmdString )
	{
		var splitString = cmdString.split( " " );
		if( splitString[0] && eraSettings_validEras.indexOf( splitString[0].toLowerCase() ) !== -1 )
		{
			switch( splitString[0].toLowerCase() )
			{
				case "uo": 	ApplyEra_UO( socket ); 	break;
				case "t2a": ApplyEra_T2A( socket ); break;
				case "uor": ApplyEra_UOR( socket ); break;
				case "td": 	ApplyEra_TD( socket ); 	break;
				case "lbr": ApplyEra_LBR( socket ); break;
				case "aos": ApplyEra_AOS( socket ); break;
				case "se": 	ApplyEra_SE( socket ); 	break;
				case "ml": 	ApplyEra_ML( socket ); 	break;
				case "sa": 	ApplyEra_SA( socket ); 	break;
				case "hs": 	ApplyEra_HS( socket ); 	break;
				case "tol": ApplyEra_TOL( socket ); break;
			}
		}
		else
		{
			socket.SysMessage( "This command requires a valid parameter that specifies which UO Era to load: uo, t2a, uor, td, lbr, aos, se, ml, sa, hs, tol" );
		}
	}
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_UO( socket )
{
	// Apply settings for Ultima Online (at launch)
	// [system]
	SetServerSetting( "ContextMenus", 0 );

	// [skill & stats]
	SetServerSetting( "SkillCap", 7000 );
	SetServerSetting( "SkillCapSingle", 1000 );
	SetServerSetting( "StatCap", 225 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ) || GetServerSetting( "ClientSupport7090" ))
	{
		// These client versions don't support extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 0 );
		SetServerSetting( "ExtendedStartingSkills", 0 );
	}
	else
	{
		// Clients from 7.0.16.0 and up require extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 1 );
		SetServerSetting( "ExtendedStartingSkills", 1 );
	}
	SetServerSetting( "ArmorAffectManaRegen", 0 );
	SetServerSetting( "HealthRegenMode", 1 );
	SetServerSetting( "StaminaRegenMode", 1 );
	SetServerSetting( "ManaRegenMode", 1 );
	SetServerSetting( "HealthRegenCap", 18 );
	SetServerSetting( "StaminaRegenCap", 24 );
	SetServerSetting( "ManaRegenCap", 18 );

	// [timers]
	SetServerSetting( "CorpseDecayTimer", 420 );
	SetServerSetting( "NpcCorpseDecayTimer", 420 );
	SetServerSetting( "DecayTimer", 3600 );
	SetServerSetting( "HitPointRegenTimer", 8 );
	SetServerSetting( "StaminaRegenTimer", 2 );
	SetServerSetting( "ManaRegenTimer", 7 );

	// [expansion settings]
	SetServerSetting( "CorseShardEra", "uo" );
	SetServerSetting( "ArmorCalculation", "core" );
	SetServerSetting( "StrengthDamageBonus", "core" );
	SetServerSetting( "TacticsDamageBonus", "core" );
	SetServerSetting( "AnatomyDamageBonus", "core" );
	SetServerSetting( "LumberjackDamageBonus", "core" );
	SetServerSetting( "RacialDamageBonus", "core" );
	SetServerSetting( "DamageBonusCap", "core" );
	SetServerSetting( "ShieldParry", "core" );
	SetServerSetting( "WeaponParry", "core" );
	SetServerSetting( "WrestlingParry", "core" );
	SetServerSetting( "CombatHitChance", "core" );

	// [default race bonuses]
	SetServerSetting( "HumanHealthRegenBonus", 0 );
	SetServerSetting( "HumanStaminaRegenBonus", 0 );
	SetServerSetting( "HumanManaRegenBonus", 0 );
	SetServerSetting( "HumanMaxWeightBonus", 0 );
	SetServerSetting( "ElfHealthRegenBonus", 0 );
	SetServerSetting( "ElfStaminaRegenBonus", 0 );
	SetServerSetting( "ElfManaRegenBonus", 0 );
	SetServerSetting( "ElfMaxWeightBonus", 0 );
	SetServerSetting( "GargoyleHealthRegenBonus", 0 );
	SetServerSetting( "GargoyleStaminaRegenBonus", 0 );
	SetServerSetting( "GargoyleManaRegenBonus", 0 );
	SetServerSetting( "GargoyleMaxWeightBonus", 0 );

	// [settings]
	SetServerSetting( "LootDecaysWithPlayerCorpse", 0 );
	SetServerSetting( "LootDecaysWithNpcCorpse", 1 );
	SetServerSetting( "SellMaxItems", 5 );
	SetServerSetting( "GlobalRestockMultiplier", 1 );
	SetServerSetting( "BankBuyThreshold", 2000 );
	SetServerSetting( "RankSystem", 1 );
	SetServerSetting( "CutScrollRequirement", 1 );
	SetServerSetting( "DisplayMakersMark", 0 );
	SetServerSetting( "ClientFeatures", 0 );
	SetServerSetting( "ServerFeatures", 0 );
	SetServerSetting( "SpawnRegionsFacets", 1 );
	SetServerSetting( "MoongateFacets", 1 );
	SetServerSetting( "BasicTooltipsOnly", 1 );
	SetServerSetting( "ShowReputationTitleInTooltip", 0 );
	SetServerSetting( "ShowGuildInfoInTooltip", 0 );
	SetServerSetting( "ShowNpcTitlesInTooltips", 0 );
	SetServerSetting( "ShowNpcTitlesOverhead", 0 );
	SetServerSetting( "ShowInvulnerableTagOverhead", 1 );
	SetServerSetting( "PaperdollGuildButton", 0 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" ) || GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" ))
	{
		SetServerSetting( "ForceNewAnimationPacket", 0 ); // If client version used is below 7.0
	}
	else
	{
		SetServerSetting( "ForceNewAnimationPacket", 1 ); // If client version used is v7.0 or above
	}
	SetServerSetting( "MapDiffsEnabled", 0 );
	SetServerSetting( "ToolUseLimit", 1 );
	SetServerSetting( "ToolUseBreak", 1 );
	SetServerSetting( "ItemRepairDurabilityLoss", 1 );
	SetServerSetting( "CraftColouredWeapons", 0 );
	SetServerSetting( "MaxSafeTeleportsPerDay", 1 );
	SetServerSetting( "TeleportToNearestSafeLocation", 0 );
	SetServerSetting( "YoungPlayerSystem", 0 );
	SetServerSetting( "KarmaLocking", 0 );

	// [pets and followers]
	SetServerSetting( "MaxControlSlots", 0 );
	SetServerSetting( "MaxFollowers", 5 );
	SetServerSetting( "MaxPetOwners", 100 );
	SetServerSetting( "CheckPetControlDifficulty", 0 );
	SetServerSetting( "PetLoyaltyGainOnSuccess", 1 );
	SetServerSetting( "PetLoyaltyLossOnFailure", 1 );
	SetServerSetting( "PetLoyaltyRate", 600 );
	SetServerSetting( "PetBondingEnabled", 0 );

	// [combat]
	SetServerSetting( "DisplayDamageNumbers", 0 );
	SetServerSetting( "NPCDamageRate", 2 );
	SetServerSetting( "AttackSpeedFromStamina", 0 );
	SetServerSetting( "ArcheryHitBonus", 0 );
	SetServerSetting( "ArcheryShootDelay", 1.0 );
	SetServerSetting( "ShootOnAnimalBack", 1 );
	SetServerSetting( "WeaponDamageChance", 8 );
	SetServerSetting( "ArmorDamageChance", 16 );
	SetServerSetting( "ParryDamageChance", 10 );
	SetServerSetting( "AlchemyBonusEnabled", 0 );
	SetServerSetting( "ItemsInterruptCasting", 0 );
	SetServerSetting( "ShowItemResistStats", 0 );
	SetServerSetting( "ShowWeaponDamageTypes", 0 );
	SetServerSetting( "WeaponDamageBonusType", 2 );
	SetServerSetting( "PoisonCorrosionSystem", 0 );

	// [magic]
	SetServerSetting( "TravelSpellsFromBoatKeys", 1 );
	SetServerSetting( "TravelSpellsWhileOverweight", 0 );
	SetServerSetting( "MarkRunesInMultis", 0 );
	SetServerSetting( "TravelSpellsWhileAggressor", 0 );
	SetServerSetting( "HideStatsForUnknownMagicItems", 1 );

	// [startup]
	SetServerSetting( "StartGold", 100 );

	// [houses]
	SetServerSetting( "TrackHousesPerAccount", 0 );
	SetServerSetting( "CanOwnAndCoOwnHouses", 0 );
	SetServerSetting( "CoOwnHousesOnSameAccount", 0 );
	SetServerSetting( "ItemDecayInHouses", 1 );
	SetServerSetting( "MaxHousesOwnable", 1 );
	SetServerSetting( "MaxHousesCoOwnable", 1 );
	SetServerSetting( "SafeCoOwnerLogout", 1 );
	SetServerSetting( "SafeFriendLogout", 1 );
	SetServerSetting( "SafeGuestLogout", 0 );
	SetServerSetting( "KeylessOwnerAccess", 1 );
	SetServerSetting( "KeylessCoOwnerAccess", 0 );
	SetServerSetting( "KeylessFriendAccess", 0 );
	SetServerSetting( "KeylessGuestAccess", 0 );

	// [bulk order deeds]
	SetServerSetting( "OfferBODsFromItemSales", 0 );
	SetServerSetting( "OfferBODsFromContextMenu", 0 );

	// Recommend server restart after applying changes
	// With confirmation from admin
	RecommendServerRestart( socket );
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_T2A( socket )
{
	// Apply settings for The Second Age
	// Apply settings for Ultima Online (at launch)
	// [system]
	SetServerSetting( "ContextMenus", 0 );

	// [skill & stats]
	SetServerSetting( "SkillCap", 7000 );
	SetServerSetting( "SkillCapSingle", 1000 );
	SetServerSetting( "StatCap", 225 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ) || GetServerSetting( "ClientSupport7090" ))
	{
		// These client versions don't support extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 0 );
		SetServerSetting( "ExtendedStartingSkills", 0 );
	}
	else
	{
		// Clients from 7.0.16.0 and up require extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 1 );
		SetServerSetting( "ExtendedStartingSkills", 1 );
	}
	SetServerSetting( "ArmorAffectManaRegen", 1 );
	SetServerSetting( "HealthRegenMode", 1 );
	SetServerSetting( "StaminaRegenMode", 1 );
	SetServerSetting( "ManaRegenMode", 1 );
	SetServerSetting( "HealthRegenCap", 18 );
	SetServerSetting( "StaminaRegenCap", 24 );
	SetServerSetting( "ManaRegenCap", 18 );

	// [timers]
	SetServerSetting( "CorpseDecayTimer", 900 );
	SetServerSetting( "NpcCorpseDecayTimer", 900 );
	SetServerSetting( "DecayTimer", 3600 );
	SetServerSetting( "HitPointRegenTimer", 8 );
	SetServerSetting( "StaminaRegenTimer", 2 );
	SetServerSetting( "ManaRegenTimer", 7 );

	// [expansion settings]
	SetServerSetting( "CorseShardEra", "t2a" );
	SetServerSetting( "ArmorCalculation", "core" );
	SetServerSetting( "StrengthDamageBonus", "core" );
	SetServerSetting( "TacticsDamageBonus", "core" );
	SetServerSetting( "AnatomyDamageBonus", "core" );
	SetServerSetting( "LumberjackDamageBonus", "core" );
	SetServerSetting( "RacialDamageBonus", "core" );
	SetServerSetting( "DamageBonusCap", "core" );
	SetServerSetting( "ShieldParry", "core" );
	SetServerSetting( "WeaponParry", "core" );
	SetServerSetting( "WrestlingParry", "core" );
	SetServerSetting( "CombatHitChance", "core" );

	// [default race bonuses]
	SetServerSetting( "HumanHealthRegenBonus", 0 );
	SetServerSetting( "HumanStaminaRegenBonus", 0 );
	SetServerSetting( "HumanManaRegenBonus", 0 );
	SetServerSetting( "HumanMaxWeightBonus", 0 );
	SetServerSetting( "ElfHealthRegenBonus", 0 );
	SetServerSetting( "ElfStaminaRegenBonus", 0 );
	SetServerSetting( "ElfManaRegenBonus", 0 );
	SetServerSetting( "ElfMaxWeightBonus", 0 );
	SetServerSetting( "GargoyleHealthRegenBonus", 0 );
	SetServerSetting( "GargoyleStaminaRegenBonus", 0 );
	SetServerSetting( "GargoyleManaRegenBonus", 0 );
	SetServerSetting( "GargoyleMaxWeightBonus", 0 );

	// [settings]
	SetServerSetting( "LootDecaysWithPlayerCorpse", 0 );
	SetServerSetting( "LootDecaysWithNpcCorpse", 1 );
	SetServerSetting( "SellMaxItems", 5 );
	SetServerSetting( "GlobalRestockMultiplier", 1 );
	SetServerSetting( "BankBuyThreshold", 2000 );
	SetServerSetting( "RankSystem", 1 );
	SetServerSetting( "CutScrollRequirement", 1 );
	SetServerSetting( "DisplayMakersMark", 1 );
	SetServerSetting( "ClientFeatures", 1 );
	SetServerSetting( "ServerFeatures", 0 );
	SetServerSetting( "SpawnRegionsFacets", 1 );
	SetServerSetting( "MoongateFacets", 1 );
	SetServerSetting( "BasicTooltipsOnly", 1 );
	SetServerSetting( "ShowReputationTitleInTooltip", 0 );
	SetServerSetting( "ShowGuildInfoInTooltip", 0 );
	SetServerSetting( "ShowNpcTitlesInTooltips", 0 );
	SetServerSetting( "ShowNpcTitlesOverhead", 0 );
	SetServerSetting( "ShowInvulnerableTagOverhead", 1 );
	SetServerSetting( "PaperdollGuildButton", 0 );
	SetServerSetting( "MaxPlayerBankWeight", 2147483647 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" ) || GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" ))
	{
		SetServerSetting( "ForceNewAnimationPacket", 0 ); // If client version used is below 7.0
	}
	else
	{
		SetServerSetting( "ForceNewAnimationPacket", 1 ); // If client version used is v7.0 or above
	}
	SetServerSetting( "MapDiffsEnabled", 0 );
	SetServerSetting( "ToolUseLimit", 1 );
	SetServerSetting( "ToolUseBreak", 1 );
	SetServerSetting( "ItemRepairDurabilityLoss", 1 );
	SetServerSetting( "CraftColouredWeapons", 0 );
	SetServerSetting( "MaxSafeTeleportsPerDay", 1 );
	SetServerSetting( "TeleportToNearestSafeLocation", 0 );
	SetServerSetting( "YoungPlayerSystem", 0 );
	SetServerSetting( "KarmaLocking", 0 );

	// [pets and followers]
	SetServerSetting( "MaxControlSlots", 0 );
	SetServerSetting( "MaxFollowers", 5 );
	SetServerSetting( "MaxPetOwners", 100 );
	SetServerSetting( "CheckPetControlDifficulty", 1 );
	SetServerSetting( "PetLoyaltyGainOnSuccess", 1 );
	SetServerSetting( "PetLoyaltyLossOnFailure", 1 );
	SetServerSetting( "PetLoyaltyRate", 900 );
	SetServerSetting( "PetBondingEnabled", 0 );

	// [combat]
	SetServerSetting( "DisplayDamageNumbers", 0 );
	SetServerSetting( "NPCDamageRate", 1 );
	SetServerSetting( "AttackSpeedFromStamina", 1 );
	SetServerSetting( "ArcheryHitBonus", 0 );
	SetServerSetting( "ArcheryShootDelay", 1.0 );
	SetServerSetting( "ShootOnAnimalBack", 1 );
	SetServerSetting( "WeaponDamageChance", 17 );
	SetServerSetting( "ArmorDamageChance", 33 );
	SetServerSetting( "ParryDamageChance", 20 );
	SetServerSetting( "AlchemyBonusEnabled", 0 );
	SetServerSetting( "ItemsInterruptCasting", 1 );
	SetServerSetting( "ShowItemResistStats", 0 );
	SetServerSetting( "ShowWeaponDamageTypes", 0 );
	SetServerSetting( "WeaponDamageBonusType", 2 );
	SetServerSetting( "PoisonCorrosionSystem", 0 );

	// [magic]
	SetServerSetting( "TravelSpellsFromBoatKeys", 0 );
	SetServerSetting( "TravelSpellsWhileOverweight", 1 );
	SetServerSetting( "MarkRunesInMultis", 0 );
	SetServerSetting( "TravelSpellsWhileAggressor", 1 );
	SetServerSetting( "HideStatsForUnknownMagicItems", 1 );

	// [startup]
	SetServerSetting( "StartGold", 100 );

	// [houses]
	SetServerSetting( "TrackHousesPerAccount", 0 );
	SetServerSetting( "CanOwnAndCoOwnHouses", 0 );
	SetServerSetting( "CoOwnHousesOnSameAccount", 0 );
	SetServerSetting( "ItemDecayInHouses", 0 );
	SetServerSetting( "MaxHousesOwnable", 1 );
	SetServerSetting( "MaxHousesCoOwnable", 1 );
	SetServerSetting( "SafeCoOwnerLogout", 1 );
	SetServerSetting( "SafeFriendLogout", 1 );
	SetServerSetting( "SafeGuestLogout", 0 );
	SetServerSetting( "KeylessOwnerAccess", 1 );
	SetServerSetting( "KeylessCoOwnerAccess", 0 );
	SetServerSetting( "KeylessFriendAccess", 0 );
	SetServerSetting( "KeylessGuestAccess", 0 );

	// [bulk order deeds]
	SetServerSetting( "OfferBODsFromItemSales", 0 );
	SetServerSetting( "OfferBODsFromContextMenu", 0 );

	// Recommend server restart after applying changes
	// With confirmation from admin
	RecommendServerRestart( socket );
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_UOR( socket )
{
	// Apply settings for Renaissance
	// [system]
	SetServerSetting( "ContextMenus", 0 );

	// [skill & stats]
	SetServerSetting( "SkillCap", 7000 );
	SetServerSetting( "SkillCapSingle", 1000 );
	SetServerSetting( "StatCap", 225 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ) || GetServerSetting( "ClientSupport7090" ))
	{
		// These client versions don't support extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 0 );
		SetServerSetting( "ExtendedStartingSkills", 0 );
	}
	else
	{
		// Clients from 7.0.16.0 and up require extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 1 );
		SetServerSetting( "ExtendedStartingSkills", 1 );
	}
	SetServerSetting( "ArmorAffectManaRegen", 1 );
	SetServerSetting( "HealthRegenMode", 1 );
	SetServerSetting( "StaminaRegenMode", 1 );
	SetServerSetting( "ManaRegenMode", 1 );
	SetServerSetting( "HealthRegenCap", 18 );
	SetServerSetting( "StaminaRegenCap", 24 );
	SetServerSetting( "ManaRegenCap", 18 );

	// [timers]
	SetServerSetting( "CorpseDecayTimer", 900 );
	SetServerSetting( "NpcCorpseDecayTimer", 900 );
	SetServerSetting( "DecayTimer", 3600 );
	SetServerSetting( "DecayTimerInHouse", 3600 );
	SetServerSetting( "HitPointRegenTimer", 8 );
	SetServerSetting( "StaminaRegenTimer", 2 );
	SetServerSetting( "ManaRegenTimer", 7 );

	// [expansion settings]
	SetServerSetting( "CorseShardEra", "uor" );
	SetServerSetting( "ArmorCalculation", "core" );
	SetServerSetting( "StrengthDamageBonus", "core" );
	SetServerSetting( "TacticsDamageBonus", "core" );
	SetServerSetting( "AnatomyDamageBonus", "core" );
	SetServerSetting( "LumberjackDamageBonus", "core" );
	SetServerSetting( "RacialDamageBonus", "core" );
	SetServerSetting( "DamageBonusCap", "core" );
	SetServerSetting( "ShieldParry", "core" );
	SetServerSetting( "WeaponParry", "core" );
	SetServerSetting( "WrestlingParry", "core" );
	SetServerSetting( "CombatHitChance", "core" );

	// [default race bonuses]
	SetServerSetting( "HumanHealthRegenBonus", 0 );
	SetServerSetting( "HumanStaminaRegenBonus", 0 );
	SetServerSetting( "HumanManaRegenBonus", 0 );
	SetServerSetting( "HumanMaxWeightBonus", 0 );
	SetServerSetting( "ElfHealthRegenBonus", 0 );
	SetServerSetting( "ElfStaminaRegenBonus", 0 );
	SetServerSetting( "ElfManaRegenBonus", 0 );
	SetServerSetting( "ElfMaxWeightBonus", 0 );
	SetServerSetting( "GargoyleHealthRegenBonus", 0 );
	SetServerSetting( "GargoyleStaminaRegenBonus", 0 );
	SetServerSetting( "GargoyleManaRegenBonus", 0 );
	SetServerSetting( "GargoyleMaxWeightBonus", 0 );

	// [settings]
	SetServerSetting( "LootDecaysWithPlayerCorpse", 0 );
	SetServerSetting( "LootDecaysWithNpcCorpse", 1 );
	SetServerSetting( "SellMaxItems", 5 );
	SetServerSetting( "GlobalRestockMultiplier", 1 );
	SetServerSetting( "BankBuyThreshold", 2000 );
	SetServerSetting( "RankSystem", 1 );
	SetServerSetting( "CutScrollRequirement", 1 );
	SetServerSetting( "DisplayMakersMark", 1 );
	SetServerSetting( "ClientFeatures", 3 );
	SetServerSetting( "ServerFeatures", 0 );
	SetServerSetting( "SpawnRegionsFacets", 3 );
	SetServerSetting( "MoongateFacets", 3 );
	SetServerSetting( "BasicTooltipsOnly", 1 );
	SetServerSetting( "ShowReputationTitleInTooltip", 0 );
	SetServerSetting( "ShowGuildInfoInTooltip", 0 );
	SetServerSetting( "ShowNpcTitlesInTooltips", 0 );
	SetServerSetting( "ShowNpcTitlesOverhead", 0 );
	SetServerSetting( "ShowInvulnerableTagOverhead", 1 );
	SetServerSetting( "PaperdollGuildButton", 0 );
	SetServerSetting( "MaxPlayerBankWeight", 2147483647 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" ))
	{
		SetServerSetting( "ForceNewAnimationPacket", 0 ); // If client version used is below 7.0
	}
	else
	{
		SetServerSetting( "ForceNewAnimationPacket", 1 ); // If client version used is v7.0 or above
	}
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ))
	{
		// MapDiffs enabled to display Old Haven in Trammel
		SetServerSetting( "MapDiffsEnabled", 1 );
	}
	else
	{
		// MapDiffs disabled to display New Haven in Trammel
		SetServerSetting( "MapDiffsEnabled", 0 );
	}
	SetServerSetting( "ToolUseLimit", 1 );
	SetServerSetting( "ToolUseBreak", 1 );
	SetServerSetting( "ItemRepairDurabilityLoss", 1 );
	SetServerSetting( "CraftColouredWeapons", 0 );
	SetServerSetting( "MaxSafeTeleportsPerDay", 1 );
	SetServerSetting( "TeleportToNearestSafeLocation", 0 );
	SetServerSetting( "YoungPlayerSystem", 0 );
	SetServerSetting( "KarmaLocking", 1 );

	// [pets and followers]
	SetServerSetting( "MaxControlSlots", 0 );
	SetServerSetting( "MaxFollowers", 5 );
	SetServerSetting( "MaxPetOwners", 10 );
	SetServerSetting( "CheckPetControlDifficulty", 1 );
	SetServerSetting( "PetLoyaltyGainOnSuccess", 1 );
	SetServerSetting( "PetLoyaltyLossOnFailure", 1 );
	SetServerSetting( "PetLoyaltyRate", 900 );
	SetServerSetting( "PetBondingEnabled", 0 );

	// [combat]
	SetServerSetting( "DisplayDamageNumbers", 0 );
	SetServerSetting( "NPCDamageRate", 1 );
	SetServerSetting( "AttackSpeedFromStamina", 1 );
	SetServerSetting( "ArcheryHitBonus", 10 );
	SetServerSetting( "ArcheryShootDelay", 0.5 );
	SetServerSetting( "ShootOnAnimalBack", 1 );
	SetServerSetting( "WeaponDamageChance", 17 );
	SetServerSetting( "ArmorDamageChance", 33 );
	SetServerSetting( "ParryDamageChance", 20 );
	SetServerSetting( "AlchemyBonusEnabled", 0 );
	SetServerSetting( "ItemsInterruptCasting", 0 );
	SetServerSetting( "ShowItemResistStats", 0 );
	SetServerSetting( "ShowWeaponDamageTypes", 0 );
	SetServerSetting( "WeaponDamageBonusType", 2 );
	SetServerSetting( "PoisonCorrosionSystem", 1 );

	// [magic]
	SetServerSetting( "TravelSpellsFromBoatKeys", 1 );
	SetServerSetting( "TravelSpellsWhileOverweight", 0 );
	SetServerSetting( "MarkRunesInMultis", 0 );
	SetServerSetting( "TravelSpellsBetweenWorlds", 0 );
	SetServerSetting( "TravelSpellsWhileAggressor", 0 );
	SetServerSetting( "HideStatsForUnknownMagicItems", 1 );

	// [startup]
	SetServerSetting( "StartGold", 1000 );

	// [houses]
	SetServerSetting( "TrackHousesPerAccount", 1 );
	SetServerSetting( "CanOwnAndCoOwnHouses", 0 );
	SetServerSetting( "CoOwnHousesOnSameAccount", 0 );
	SetServerSetting( "ItemDecayInHouses", 1 );
	SetServerSetting( "MaxHousesOwnable", 1 );
	SetServerSetting( "MaxHousesCoOwnable", 1 );
	SetServerSetting( "SafeCoOwnerLogout", 1 );
	SetServerSetting( "SafeFriendLogout", 1 );
	SetServerSetting( "SafeGuestLogout", 0 );
	SetServerSetting( "KeylessOwnerAccess", 1 );
	SetServerSetting( "KeylessCoOwnerAccess", 0 );
	SetServerSetting( "KeylessFriendAccess", 0 );
	SetServerSetting( "KeylessGuestAccess", 0 );

	// [bulk order deeds]
	SetServerSetting( "OfferBODsFromItemSales", 0 );
	SetServerSetting( "OfferBODsFromContextMenu", 0 );

	// Recommend server restart after applying changes
	// With confirmation from admin
	RecommendServerRestart( socket );
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_TD( socket )
{
	// Apply settings for Third Dawn
	// [system]
	SetServerSetting( "ContextMenus", 0 );

	// [skill & stats]
	SetServerSetting( "SkillCap", 7000 );
	SetServerSetting( "SkillCapSingle", 1000 );
	SetServerSetting( "StatCap", 225 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ) || GetServerSetting( "ClientSupport7090" ))
	{
		// These client versions don't support extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 0 );
		SetServerSetting( "ExtendedStartingSkills", 0 );
	}
	else
	{
		// Clients from 7.0.16.0 and up require extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 1 );
		SetServerSetting( "ExtendedStartingSkills", 1 );
	}
	SetServerSetting( "ArmorAffectManaRegen", 1 );
	SetServerSetting( "HealthRegenMode", 1 );
	SetServerSetting( "StaminaRegenMode", 1 );
	SetServerSetting( "ManaRegenMode", 1 );
	SetServerSetting( "HealthRegenCap", 18 );
	SetServerSetting( "StaminaRegenCap", 24 );
	SetServerSetting( "ManaRegenCap", 18 );

	// [timers]
	SetServerSetting( "CorpseDecayTimer", 900 );
	SetServerSetting( "NpcCorpseDecayTimer", 900 );
	SetServerSetting( "DecayTimer", 3600 );
	SetServerSetting( "DecayTimerInHouse", 3600 );
	SetServerSetting( "HitPointRegenTimer", 8 );
	SetServerSetting( "StaminaRegenTimer", 2 );
	SetServerSetting( "ManaRegenTimer", 7 );

	// [expansion settings]
	SetServerSetting( "CorseShardEra", "td" );
	SetServerSetting( "ArmorCalculation", "core" );
	SetServerSetting( "StrengthDamageBonus", "core" );
	SetServerSetting( "TacticsDamageBonus", "core" );
	SetServerSetting( "AnatomyDamageBonus", "core" );
	SetServerSetting( "LumberjackDamageBonus", "core" );
	SetServerSetting( "RacialDamageBonus", "core" );
	SetServerSetting( "DamageBonusCap", "core" );
	SetServerSetting( "ShieldParry", "core" );
	SetServerSetting( "WeaponParry", "core" );
	SetServerSetting( "WrestlingParry", "core" );
	SetServerSetting( "CombatHitChance", "core" );

	// [default race bonuses]
	SetServerSetting( "HumanHealthRegenBonus", 0 );
	SetServerSetting( "HumanStaminaRegenBonus", 0 );
	SetServerSetting( "HumanManaRegenBonus", 0 );
	SetServerSetting( "HumanMaxWeightBonus", 0 );
	SetServerSetting( "ElfHealthRegenBonus", 0 );
	SetServerSetting( "ElfStaminaRegenBonus", 0 );
	SetServerSetting( "ElfManaRegenBonus", 0 );
	SetServerSetting( "ElfMaxWeightBonus", 0 );
	SetServerSetting( "GargoyleHealthRegenBonus", 0 );
	SetServerSetting( "GargoyleStaminaRegenBonus", 0 );
	SetServerSetting( "GargoyleManaRegenBonus", 0 );
	SetServerSetting( "GargoyleMaxWeightBonus", 0 );

	// [settings]
	SetServerSetting( "LootDecaysWithPlayerCorpse", 0 );
	SetServerSetting( "LootDecaysWithNpcCorpse", 1 );
	SetServerSetting( "SellMaxItems", 5 );
	SetServerSetting( "GlobalRestockMultiplier", 1 );
	SetServerSetting( "BankBuyThreshold", 2000 );
	SetServerSetting( "RankSystem", 1 );
	SetServerSetting( "CutScrollRequirement", 1 );
	SetServerSetting( "DisplayMakersMark", 1 );
	SetServerSetting( "ClientFeatures", 7 );
	SetServerSetting( "ServerFeatures", 8 );
	SetServerSetting( "SpawnRegionsFacets", 7 );
	SetServerSetting( "MoongateFacets", 7 );
	SetServerSetting( "BasicTooltipsOnly", 1 );
	SetServerSetting( "ShowReputationTitleInTooltip", 0 );
	SetServerSetting( "ShowGuildInfoInTooltip", 0 );
	SetServerSetting( "ShowNpcTitlesInTooltips", 0 );
	SetServerSetting( "ShowNpcTitlesOverhead", 0 );
	SetServerSetting( "ShowInvulnerableTagOverhead", 1 );
	SetServerSetting( "PaperdollGuildButton", 0 );
	SetServerSetting( "MaxPlayerBankWeight", 2147483647 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" ))
	{
		SetServerSetting( "ForceNewAnimationPacket", 0 ); // If client version used is below 7.0
	}
	else
	{
		SetServerSetting( "ForceNewAnimationPacket", 1 ); // If client version used is v7.0 or above
	}
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ))
	{
		// MapDiffs enabled to display Old Haven in Trammel
		SetServerSetting( "MapDiffsEnabled", 1 );
	}
	else
	{
		// MapDiffs disabled to display New Haven in Trammel
		SetServerSetting( "MapDiffsEnabled", 0 );
	}
	SetServerSetting( "ToolUseLimit", 1 );
	SetServerSetting( "ToolUseBreak", 1 );
	SetServerSetting( "ItemRepairDurabilityLoss", 1 );
	SetServerSetting( "CraftColouredWeapons", 0 );
	SetServerSetting( "MaxSafeTeleportsPerDay", 1 );
	SetServerSetting( "TeleportToNearestSafeLocation", 0 );
	SetServerSetting( "YoungPlayerSystem", 0 );
	SetServerSetting( "KarmaLocking", 1 );

	// [pets and followers]
	SetServerSetting( "MaxControlSlots", 0 );
	SetServerSetting( "MaxFollowers", 5 );
	SetServerSetting( "MaxPetOwners", 10 );
	SetServerSetting( "CheckPetControlDifficulty", 1 );
	SetServerSetting( "PetLoyaltyGainOnSuccess", 1 );
	SetServerSetting( "PetLoyaltyLossOnFailure", 1 );
	SetServerSetting( "PetLoyaltyRate", 900 );
	SetServerSetting( "PetBondingEnabled", 0 );

	// [combat]
	SetServerSetting( "DisplayDamageNumbers", 0 );
	SetServerSetting( "NPCDamageRate", 1 );
	SetServerSetting( "AttackSpeedFromStamina", 1 );
	SetServerSetting( "ArcheryHitBonus", 10 );
	SetServerSetting( "ArcheryShootDelay", 0.5 );
	SetServerSetting( "ShootOnAnimalBack", 1 );
	SetServerSetting( "WeaponDamageChance", 17 );
	SetServerSetting( "ArmorDamageChance", 33 );
	SetServerSetting( "ParryDamageChance", 20 );
	SetServerSetting( "AlchemyBonusEnabled", 0 );
	SetServerSetting( "ItemsInterruptCasting", 0 );
	SetServerSetting( "ShowItemResistStats", 0 );
	SetServerSetting( "ShowWeaponDamageTypes", 0 );
	SetServerSetting( "WeaponDamageBonusType", 2 );
	SetServerSetting( "PoisonCorrosionSystem", 1 );
	SetServerSetting( "AutoUnequippedCasting", 1 );

	// [magic]
	SetServerSetting( "TravelSpellsFromBoatKeys", 1 );
	SetServerSetting( "TravelSpellsWhileOverweight", 0 );
	SetServerSetting( "MarkRunesInMultis", 0 );
	SetServerSetting( "TravelSpellsBetweenWorlds", 0 );
	SetServerSetting( "TravelSpellsWhileAggressor", 0 );
	SetServerSetting( "HideStatsForUnknownMagicItems", 1 );

	// [startup]
	SetServerSetting( "StartGold", 1000 );

	// [houses]
	SetServerSetting( "TrackHousesPerAccount", 1 );
	SetServerSetting( "CanOwnAndCoOwnHouses", 0 );
	SetServerSetting( "CoOwnHousesOnSameAccount", 0 );
	SetServerSetting( "ItemDecayInHouses", 1 );
	SetServerSetting( "MaxHousesOwnable", 1 );
	SetServerSetting( "MaxHousesCoOwnable", 1 );
	SetServerSetting( "SafeCoOwnerLogout", 1 );
	SetServerSetting( "SafeFriendLogout", 1 );
	SetServerSetting( "SafeGuestLogout", 0 );
	SetServerSetting( "KeylessOwnerAccess", 1 );
	SetServerSetting( "KeylessCoOwnerAccess", 0 );
	SetServerSetting( "KeylessFriendAccess", 0 );
	SetServerSetting( "KeylessGuestAccess", 0 );

	// [bulk order deeds]
	SetServerSetting( "OfferBODsFromItemSales", 0 );
	SetServerSetting( "OfferBODsFromContextMenu", 0 );

	// Recommend server restart after applying changes
	// With confirmation from admin
	RecommendServerRestart( socket );
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_LBR( socket )
{
	// Apply settings for Lord Blackthorn's Revenge
	// [system]
	SetServerSetting( "ContextMenus", 1 );

	// [skill & stats]
	SetServerSetting( "SkillCap", 7000 );
	SetServerSetting( "SkillCapSingle", 1000 );
	SetServerSetting( "StatCap", 225 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ) || GetServerSetting( "ClientSupport7090" ))
	{
		// These client versions don't support extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 0 );
		SetServerSetting( "ExtendedStartingSkills", 0 );
	}
	else
	{
		// Clients from 7.0.16.0 and up require extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 1 );
		SetServerSetting( "ExtendedStartingSkills", 1 );
	}
	SetServerSetting( "ArmorAffectManaRegen", 1 );
	SetServerSetting( "HealthRegenMode", 1 );
	SetServerSetting( "StaminaRegenMode", 1 );
	SetServerSetting( "ManaRegenMode", 1 );
	SetServerSetting( "HealthRegenCap", 18 );
	SetServerSetting( "StaminaRegenCap", 24 );
	SetServerSetting( "ManaRegenCap", 18 );

	// [timers]
	SetServerSetting( "CorpseDecayTimer", 420 );
	SetServerSetting( "NpcCorpseDecayTimer", 420 );
	SetServerSetting( "DecayTimer", 1800 );
	SetServerSetting( "DecayTimerInHouse", 3600 );
	SetServerSetting( "HitPointRegenTimer", 8 );
	SetServerSetting( "StaminaRegenTimer", 2 );
	SetServerSetting( "ManaRegenTimer", 7 );

	// [expansion settings]
	SetServerSetting( "CorseShardEra", "lbr" );
	SetServerSetting( "ArmorCalculation", "core" );
	SetServerSetting( "StrengthDamageBonus", "core" );
	SetServerSetting( "TacticsDamageBonus", "core" );
	SetServerSetting( "AnatomyDamageBonus", "core" );
	SetServerSetting( "LumberjackDamageBonus", "core" );
	SetServerSetting( "RacialDamageBonus", "core" );
	SetServerSetting( "DamageBonusCap", "core" );
	SetServerSetting( "ShieldParry", "core" );
	SetServerSetting( "WeaponParry", "core" );
	SetServerSetting( "WrestlingParry", "core" );
	SetServerSetting( "CombatHitChance", "core" );

	// [default race bonuses]
	SetServerSetting( "HumanHealthRegenBonus", 0 );
	SetServerSetting( "HumanStaminaRegenBonus", 0 );
	SetServerSetting( "HumanManaRegenBonus", 0 );
	SetServerSetting( "HumanMaxWeightBonus", 0 );
	SetServerSetting( "ElfHealthRegenBonus", 0 );
	SetServerSetting( "ElfStaminaRegenBonus", 0 );
	SetServerSetting( "ElfManaRegenBonus", 0 );
	SetServerSetting( "ElfMaxWeightBonus", 0 );
	SetServerSetting( "GargoyleHealthRegenBonus", 0 );
	SetServerSetting( "GargoyleStaminaRegenBonus", 0 );
	SetServerSetting( "GargoyleManaRegenBonus", 0 );
	SetServerSetting( "GargoyleMaxWeightBonus", 0 );

	// [settings]
	SetServerSetting( "LootDecaysWithPlayerCorpse", 1 );
	SetServerSetting( "LootDecaysWithNpcCorpse", 1 );
	SetServerSetting( "SellMaxItems", 5 );
	SetServerSetting( "GlobalRestockMultiplier", 1 );
	SetServerSetting( "BankBuyThreshold", 2000 );
	SetServerSetting( "RankSystem", 1 );
	SetServerSetting( "CutScrollRequirement", 1 );
	SetServerSetting( "DisplayMakersMark", 1 );
	SetServerSetting( "ClientFeatures", 1021183 );
	SetServerSetting( "ServerFeatures", 4584 );
	SetServerSetting( "SpawnRegionsFacets", 7 );
	SetServerSetting( "MoongateFacets", 7 );
	SetServerSetting( "BasicTooltipsOnly", 0 );
	SetServerSetting( "ShowReputationTitleInTooltip", 1 );
	SetServerSetting( "ShowGuildInfoInTooltip", 1 );
	SetServerSetting( "ShowNpcTitlesInTooltips", 1 );
	SetServerSetting( "ShowNpcTitlesOverhead", 1 );
	SetServerSetting( "ShowInvulnerableTagOverhead", 0 );
	SetServerSetting( "PaperdollGuildButton", 1 );
	SetServerSetting( "MaxPlayerBankWeight", 2147483647 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" ))
	{
		SetServerSetting( "ForceNewAnimationPacket", 0 ); // If client version used is below 7.0
	}
	else
	{
		SetServerSetting( "ForceNewAnimationPacket", 1 ); // If client version used is v7.0 or above
	}
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ))
	{
		// MapDiffs enabled to display Old Haven in Trammel
		SetServerSetting( "MapDiffsEnabled", 1 );
	}
	else
	{
		// MapDiffs disabled to display New Haven in Trammel
		SetServerSetting( "MapDiffsEnabled", 0 );
	}
	SetServerSetting( "ToolUseLimit", 1 );
	SetServerSetting( "ToolUseBreak", 1 );
	SetServerSetting( "ItemRepairDurabilityLoss", 1 );
	SetServerSetting( "CraftColouredWeapons", 0 );
	SetServerSetting( "MaxSafeTeleportsPerDay", 1 );
	SetServerSetting( "TeleportToNearestSafeLocation", 0 );
	SetServerSetting( "YoungPlayerSystem", 1 );
	SetServerSetting( "KarmaLocking", 1 );

	// [pets and followers]
	SetServerSetting( "MaxControlSlots", 5 );
	SetServerSetting( "MaxFollowers", 5 );
	SetServerSetting( "MaxPetOwners", 10 );
	SetServerSetting( "CheckPetControlDifficulty", 1 );
	SetServerSetting( "PetLoyaltyGainOnSuccess", 1 );
	SetServerSetting( "PetLoyaltyLossOnFailure", 3 );
	SetServerSetting( "PetLoyaltyRate", 900 );
	SetServerSetting( "PetBondingEnabled", 0 );

	// [combat]
	SetServerSetting( "DisplayDamageNumbers", 1 );
	SetServerSetting( "NPCDamageRate", 1 );
	SetServerSetting( "AttackSpeedFromStamina", 1 );
	SetServerSetting( "ArcheryHitBonus", 10 );
	SetServerSetting( "ArcheryShootDelay", 0.5 );
	SetServerSetting( "ShootOnAnimalBack", 1 );
	SetServerSetting( "WeaponDamageChance", 17 );
	SetServerSetting( "ArmorDamageChance", 33 );
	SetServerSetting( "ParryDamageChance", 20 );
	SetServerSetting( "AlchemyBonusEnabled", 1 );
	SetServerSetting( "AlchemyBonusModifier", 5 );
	SetServerSetting( "ItemsInterruptCasting", 1 );
	SetServerSetting( "ShowItemResistStats", 0 );
	SetServerSetting( "ShowWeaponDamageTypes", 1 );
	SetServerSetting( "WeaponDamageBonusType", 2 );
	SetServerSetting( "PoisonCorrosionSystem", 1 );
	SetServerSetting( "AutoUnequippedCasting", 1 );

	// [magic]
	SetServerSetting( "TravelSpellsFromBoatKeys", 1 );
	SetServerSetting( "TravelSpellsWhileOverweight", 0 );
	SetServerSetting( "MarkRunesInMultis", 1 );
	SetServerSetting( "TravelSpellsBetweenWorlds", 0 );
	SetServerSetting( "TravelSpellsWhileAggressor", 0 );
	SetServerSetting( "HideStatsForUnknownMagicItems", 1 );

	// [startup]
	SetServerSetting( "StartGold", 1000 );

	// [houses]
	SetServerSetting( "TrackHousesPerAccount", 1 );
	SetServerSetting( "CanOwnAndCoOwnHouses", 1 );
	SetServerSetting( "CoOwnHousesOnSameAccount", 1 );
	SetServerSetting( "ItemDecayInHouses", 1 );
	SetServerSetting( "MaxHousesOwnable", 1 );
	SetServerSetting( "MaxHousesCoOwnable", 10 );
	SetServerSetting( "SafeCoOwnerLogout", 1 );
	SetServerSetting( "SafeFriendLogout", 1 );
	SetServerSetting( "SafeGuestLogout", 0 );
	SetServerSetting( "KeylessOwnerAccess", 1 );
	SetServerSetting( "KeylessCoOwnerAccess", 1 );
	SetServerSetting( "KeylessFriendAccess", 1 );
	SetServerSetting( "KeylessGuestAccess", 0 );

	// [bulk order deeds]
	SetServerSetting( "OfferBODsFromItemSales", 1 );
	SetServerSetting( "OfferBODsFromContextMenu", 1 );

	// Recommend server restart after applying changes
	// With confirmation from admin
	RecommendServerRestart( socket );
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_AOS( socket )
{
	// Apply settings for Age of Shadows
	// [system]
	SetServerSetting( "ContextMenus", 1 );

	// [skill & stats]
	SetServerSetting( "SkillCap", 7000 );
	SetServerSetting( "SkillCapSingle", 1000 );
	SetServerSetting( "StatCap", 225 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ) || GetServerSetting( "ClientSupport7090" ))
	{
		// These client versions don't support extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 0 );
		SetServerSetting( "ExtendedStartingSkills", 0 );
	}
	else
	{
		// Clients from 7.0.16.0 and up require extended starting stats/skills
		SetServerSetting( "ExtendedStartingStats", 1 );
		SetServerSetting( "ExtendedStartingSkills", 1 );
	}
	SetServerSetting( "ArmorAffectManaRegen", 1 );
	SetServerSetting( "HealthRegenMode", 1 );
	SetServerSetting( "StaminaRegenMode", 2 );
	SetServerSetting( "ManaRegenMode", 2 );
	SetServerSetting( "HealthRegenCap", 18 );
	SetServerSetting( "StaminaRegenCap", 24 );
	SetServerSetting( "ManaRegenCap", 18 );

	// [timers]
	SetServerSetting( "CorpseDecayTimer", 420 );
	SetServerSetting( "NpcCorpseDecayTimer", 420 );
	SetServerSetting( "DecayTimer", 1800 );
	SetServerSetting( "DecayTimerInHouse", 3600 );
	SetServerSetting( "HitPointRegenTimer", 8 );
	SetServerSetting( "StaminaRegenTimer", 2 );
	SetServerSetting( "ManaRegenTimer", 7 );

	// [expansion settings]
	SetServerSetting( "CorseShardEra", "lbr" );
	SetServerSetting( "ArmorCalculation", "core" );
	SetServerSetting( "StrengthDamageBonus", "core" );
	SetServerSetting( "TacticsDamageBonus", "core" );
	SetServerSetting( "AnatomyDamageBonus", "core" );
	SetServerSetting( "LumberjackDamageBonus", "core" );
	SetServerSetting( "RacialDamageBonus", "core" );
	SetServerSetting( "DamageBonusCap", "core" );
	SetServerSetting( "ShieldParry", "core" );
	SetServerSetting( "WeaponParry", "core" );
	SetServerSetting( "WrestlingParry", "core" );
	SetServerSetting( "CombatHitChance", "core" );

	// [default race bonuses]
	SetServerSetting( "HumanHealthRegenBonus", 0 );
	SetServerSetting( "HumanStaminaRegenBonus", 0 );
	SetServerSetting( "HumanManaRegenBonus", 0 );
	SetServerSetting( "HumanMaxWeightBonus", 0 );
	SetServerSetting( "ElfHealthRegenBonus", 0 );
	SetServerSetting( "ElfStaminaRegenBonus", 0 );
	SetServerSetting( "ElfManaRegenBonus", 0 );
	SetServerSetting( "ElfMaxWeightBonus", 0 );
	SetServerSetting( "GargoyleHealthRegenBonus", 0 );
	SetServerSetting( "GargoyleStaminaRegenBonus", 0 );
	SetServerSetting( "GargoyleManaRegenBonus", 0 );
	SetServerSetting( "GargoyleMaxWeightBonus", 0 );

	// [settings]
	SetServerSetting( "LootDecaysWithPlayerCorpse", 1 );
	SetServerSetting( "LootDecaysWithNpcCorpse", 1 );
	SetServerSetting( "SellMaxItems", 5 );
	SetServerSetting( "GlobalRestockMultiplier", 1 );
	SetServerSetting( "BankBuyThreshold", 2000 );
	SetServerSetting( "RankSystem", 1 );
	SetServerSetting( "CutScrollRequirement", 1 );
	SetServerSetting( "DisplayMakersMark", 1 );
	SetServerSetting( "ClientFeatures", 1021183 );
	SetServerSetting( "ServerFeatures", 4584 );
	SetServerSetting( "SpawnRegionsFacets", 7 );
	SetServerSetting( "MoongateFacets", 7 );
	SetServerSetting( "BasicTooltipsOnly", 0 );
	SetServerSetting( "ShowReputationTitleInTooltip", 1 );
	SetServerSetting( "ShowGuildInfoInTooltip", 1 );
	SetServerSetting( "ShowNpcTitlesInTooltips", 1 );
	SetServerSetting( "ShowNpcTitlesOverhead", 1 );
	SetServerSetting( "ShowInvulnerableTagOverhead", 0 );
	SetServerSetting( "PaperdollGuildButton", 1 );
	SetServerSetting( "MaxPlayerBankWeight", 2147483647 );
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" ))
	{
		SetServerSetting( "ForceNewAnimationPacket", 0 ); // If client version used is below 7.0
	}
	else
	{
		SetServerSetting( "ForceNewAnimationPacket", 1 ); // If client version used is v7.0 or above
	}
	if( GetServerSetting( "ClientSupport4000" ) || GetServerSetting( "ClientSupport5000" )
		|| GetServerSetting( "ClientSupport6000" ) || GetServerSetting( "ClientSupport6050" )
		|| GetServerSetting( "ClientSupport7000" ))
	{
		// MapDiffs enabled to display Old Haven in Trammel
		SetServerSetting( "MapDiffsEnabled", 1 );
	}
	else
	{
		// MapDiffs disabled to display New Haven in Trammel
		SetServerSetting( "MapDiffsEnabled", 0 );
	}
	SetServerSetting( "ToolUseLimit", 1 );
	SetServerSetting( "ToolUseBreak", 1 );
	SetServerSetting( "ItemRepairDurabilityLoss", 1 );
	SetServerSetting( "CraftColouredWeapons", 0 );
	SetServerSetting( "MaxSafeTeleportsPerDay", 1 );
	SetServerSetting( "TeleportToNearestSafeLocation", 0 );
	SetServerSetting( "YoungPlayerSystem", 1 );
	SetServerSetting( "KarmaLocking", 1 );

	// [pets and followers]
	SetServerSetting( "MaxControlSlots", 5 );
	SetServerSetting( "MaxFollowers", 5 );
	SetServerSetting( "MaxPetOwners", 10 );
	SetServerSetting( "CheckPetControlDifficulty", 1 );
	SetServerSetting( "PetLoyaltyGainOnSuccess", 1 );
	SetServerSetting( "PetLoyaltyLossOnFailure", 3 );
	SetServerSetting( "PetLoyaltyRate", 900 );
	SetServerSetting( "PetBondingEnabled", 0 );

	// [combat]
	SetServerSetting( "DisplayDamageNumbers", 1 );
	SetServerSetting( "NPCDamageRate", 1 );
	SetServerSetting( "AttackSpeedFromStamina", 1 );
	SetServerSetting( "ArcheryHitBonus", 10 );
	SetServerSetting( "ArcheryShootDelay", 0.5 );
	SetServerSetting( "ShootOnAnimalBack", 1 );
	SetServerSetting( "WeaponDamageChance", 17 );
	SetServerSetting( "ArmorDamageChance", 33 );
	SetServerSetting( "ParryDamageChance", 20 );
	SetServerSetting( "AlchemyBonusEnabled", 1 );
	SetServerSetting( "AlchemyBonusModifier", 5 );
	SetServerSetting( "ItemsInterruptCasting", 1 );
	SetServerSetting( "ShowItemResistStats", 0 );
	SetServerSetting( "ShowWeaponDamageTypes", 1 );
	SetServerSetting( "WeaponDamageBonusType", 2 );
	SetServerSetting( "PoisonCorrosionSystem", 1 );
	SetServerSetting( "AutoUnequippedCasting", 1 );

	// [magic]
	SetServerSetting( "TravelSpellsFromBoatKeys", 1 );
	SetServerSetting( "TravelSpellsWhileOverweight", 0 );
	SetServerSetting( "MarkRunesInMultis", 1 );
	SetServerSetting( "TravelSpellsBetweenWorlds", 0 );
	SetServerSetting( "TravelSpellsWhileAggressor", 0 );
	SetServerSetting( "HideStatsForUnknownMagicItems", 1 );

	// [startup]
	SetServerSetting( "StartGold", 1000 );

	// [houses]
	SetServerSetting( "TrackHousesPerAccount", 1 );
	SetServerSetting( "CanOwnAndCoOwnHouses", 1 );
	SetServerSetting( "CoOwnHousesOnSameAccount", 1 );
	SetServerSetting( "ItemDecayInHouses", 1 );
	SetServerSetting( "MaxHousesOwnable", 1 );
	SetServerSetting( "MaxHousesCoOwnable", 10 );
	SetServerSetting( "SafeCoOwnerLogout", 1 );
	SetServerSetting( "SafeFriendLogout", 1 );
	SetServerSetting( "SafeGuestLogout", 0 );
	SetServerSetting( "KeylessOwnerAccess", 1 );
	SetServerSetting( "KeylessCoOwnerAccess", 1 );
	SetServerSetting( "KeylessFriendAccess", 1 );
	SetServerSetting( "KeylessGuestAccess", 0 );

	// [bulk order deeds]
	SetServerSetting( "OfferBODsFromItemSales", 1 );
	SetServerSetting( "OfferBODsFromContextMenu", 1 );

	// Recommend server restart after applying changes
	// With confirmation from admin
	RecommendServerRestart( socket );
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_SE( socket )
{
	// Apply settings for Samurai Empire
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_ML( socket )
{
	// Apply settings for Mondain's Legacy
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_SA( socket )
{
	// Apply settings for Stygian Abyss
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_HS( socket )
{
	// Apply settings for High Seas
}

/** @type { ( socket: Socket ) => void } */
function ApplyEra_TOL( socket )
{
	// Apply settings for Time of Legends
}

/** @type { ( socket: Socket ) => void } */
function RecommendServerRestart( socket )
{
	// Recommend that shard admin restart server
}
