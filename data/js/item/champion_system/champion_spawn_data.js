/// <reference path="../definitions.d.ts" />
// @ts-check
// Champion Spawn System
// v61 - Updated 7th Feb, 2026

// ============================================================================
// ChampionSpawnTable
// ---------------------------------------------------------------------------
// Key
//  - Numeric key (1, 2, 3, ...) is the "championType" value stored on the
//    altar via SetTag("championType", type). Used by ChampionSpawnData() and
//    any system that needs to look up spawn info.
// 
// Per-entry properties:
//  name              - Display name for this champion spawn type. Used in
//                      messages, logs, or UI.
// 
//  boss              - DFN sectionID (name) of the champion boss NPC that
//                      will be summoned at the final stage. This must match
//                      the NPC definition in your DFNs.
//	miniBoss          - Optional DFN sectionID (name) of a mini-boss NPC.
//                      If set, this NPC is spawned at the same time the final boss spawns.
//                      Use this for "boss + lieutenant" style finales.
//                      (Leave blank/undefined to disable mini-boss for that champion type.)
// 
//  rewards           - Array of DFN sectionIDs (items) that can be rolled as
//                      normal rewards for the top damagers when the champion
//                      dies. RewardTopDamagers() picks one random entry from
//                      this list per rewarded player.
//   uniqueList		  - Rare, unique style reward items. At most one per champ
//                      kill. Weighted as rare in logic script.
//   sharedList       - Common / standard reward items. Can drop for multiple
//                      top damagers. Weighted as the main pool.
//   decorativeList   - Decorative / vanity items (statues, banners, hue cloth,
//                      etc). Weighted as uncommon.
// 
//  stageNames        - Array of strings for the 3 title steps shown on the
//                      altar as the spawn advances. Example for Abyss:
//                        "Foe", "Assassin", "Conqueror"
//                      Each index corresponds to a spawn level (see levels).
// 
//  levels            - Array of spawn-level definitions. Each entry in this
//                      outer array is one "wave" (level) of the champion
//                      spawn. Inside each level is an array of DFN sectionIDs
//                      (creature types) that may be spawned at that level.
//                      Example:
//                        levels[0] = ["greatermongbat", "imp"]
//                        levels[1] = ["gargoyle", "harpy"]
//                      Your spawn logic picks creatures from the correct
//                      level array depending on current stage.
// 
//  powerScrollSkills - Array of skill property names (matching character
//                      .skills / .skillCaps property names) that this
//                      champion is allowed to roll power scrolls for. The
//                      power scroll reward logic reads this array and picks
//                      one skill at random when generating a scroll:
// 
//                        "swordsmanship", "fencing", "magery", etc.
// 
//                      The power scroll system then builds a DFN sectionID
//                      like "powerscroll_<skill>_<bonus>" using one of these
//                      skill names and a rolled bonus value (5/10/15/20).
// ============================================================================

const ChampionSpawnTable = {
	1: { // Abyss
		name: "Abyss Champion",
		boss: "semidar",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		uniqueList:      [ "artifact_gladiators_collar" ],
		sharedList:      [ "artifact_lieutenant_of_the_britannian_royal_guard" ],
		decorativeList:  [ "artifact_demon_skull" ],
		stageNames: ["Foe", "Assassin", "Conqueror"],
		levels: [
			["greatermongbat", "imp"],
			["gargoyle", "harpy"],
			["firegargoyle", "stonegargoyle"],
			["daemon", "succubus"]
		],
		powerScrollSkills: [
			"swordsmanship", "fencing", "macefighting", "archery", "wrestling",
			"parrying", "tactics", "anatomy", "healing",
			"magery", "meditation", "evaluatingintel", "magicresistance",
			"stealing", "throwing", "stealth",
			"taming", "animallore", "veterinary",
			"musicianship", "provocation", "discordance", "peacemaking",
			"chivalry", "focus", "necromancy", "bushido", "ninjitsu",
			"spellweaving", "spiritspeak", "mysticism"
		]
	},
	2: { // Arachnid
		name: "Arachnid Champion",
		boss: "mephitis",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		uniqueList:      [ "artifact_calm" ],
		sharedList:      [ "artifact_oblivions_needle", "artifact_necromancer_shroud" ],
		decorativeList:  [ "artifact_web" ],
		stageNames: ["bane", "killer", "vanquisher"],
		levels: [
			["scorpion", "giantspider"],
			["teradrone", "terawarrior"],
			["dreadspider", "teramatriarch"],
			["poisonelemental", "teraavenger"]
		],
		powerScrollSkills: [
			"swordsmanship", "fencing", "macefighting", "archery", "wrestling",
			"parrying", "tactics", "anatomy", "healing",
			"magery", "meditation", "evaluatingintel", "magicresistance",
			"stealing", "throwing", "stealth",
			"taming", "animallore", "veterinary",
			"musicianship", "provocation", "discordance", "peacemaking",
			"chivalry", "focus", "necromancy", "bushido", "ninjitsu",
			"spellweaving", "spiritspeak", "mysticism"
		]
	},
	3: { // Cold
		name: "Cold Blood Champion",
		boss: "rikktor",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		uniqueList:      [ "artifact_crown_of_talkeesh" ],
		sharedList:      [ "artifact_most_knowledge_person_5", "artifact_most_knowledge_person_4", "artifact_most_knowledge_person_2", 
		"artifact_lieutenant_of_the_britannian_royal_guard", "artifact_brave_knight_of_the_britannia" ],
		decorativeList:  [ "artifact_lava_tile" ],
		stageNames: ["Blight", "Slayer", "Destroyer"],
		levels: [
			["lizardman", "snake"],
			["lavalizard", "ophwarrior", "ophenforcer"],
			["drake", "ophjusticar", "ophzealot"],
			["dragon", "ophavenger", "ophapprentice"]
		],
		powerScrollSkills: [
			"swordsmanship", "fencing", "macefighting", "archery", "wrestling",
			"parrying", "tactics", "anatomy", "healing",
			"magery", "meditation", "evaluatingintel", "magicresistance",
			"stealing", "throwing", "stealth",
			"taming", "animallore", "veterinary",
			"musicianship", "provocation", "discordance", "peacemaking",
			"chivalry", "focus", "necromancy", "bushido", "ninjitsu",
			"spellweaving", "spiritspeak", "mysticism"
		]
	},
	4: { // Forest
		name: "Forest Champion",
		boss: "lordoaks",
		//miniBoss: "silvani",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		uniqueList:      [ "artifact_orc_chieftain_helm", ],
		sharedList:      [ "artifact_royal_guard_survival_knife", "artifact_djinnis_ring", "artifact_lieutenant_of_the_britannian_royal_guard", 
		"artifact_good_samaritan_of_britannia", "artifact_legendary_detective_boots", "artifact_elder_detective_boots", "artifact_mythical_detective_boots", 
		"artifact_most_knowledge_person_5", "artifact_most_knowledge_person_4", "artifact_most_knowledge_person_3"  ],
		decorativeList:  [ "artifact_water_tile", "artifact_wind_spirit", "artifact_pier" ],
		stageNames: ["Enemy", "Curse", "Slaughter"],
		levels: [
			["pixie", "shadowwisp"],
			["kirin", "wisp"],
			["centaur", "unicorn"],
			["etherealwarrior", "serptinedragon"]
		],
		powerScrollSkills: [
			"swordsmanship", "fencing", "macefighting", "archery", "wrestling",
			"parrying", "tactics", "anatomy", "healing",
			"magery", "meditation", "evaluatingintel", "magicresistance",
			"stealing", "throwing", "stealth",
			"taming", "animallore", "veterinary",
			"musicianship", "provocation", "discordance", "peacemaking",
			"chivalry", "focus", "necromancy", "bushido", "ninjitsu",
			"spellweaving", "spiritspeak", "mysticism"
		]
	},
	5: { // Unholy
		name: "Unholy Terror Champion",
		boss: "neira",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		uniqueList:      [ "artifact_shroud_of_deceit" ],
		sharedList:      [ "artifact_captain_johns_hat", "artifact_legendary_detective_boots", "artifact_elder_detective_boots", "artifact_mythical_detective_boots", "artifact_necromancer_shroud" ],
		decorativeList:  [ "artifact_tattered_mummy_wrapping", "artifact_wall_blood" ],
		stageNames: ["Scourge", "Punisher", "Nemesis"],
		levels: [
			["ghoul", "shade", "spectre", "wraith"],
			["bonemage", "mummy", "skeletalmage"],
			["boneknight", "skeletalknight", "liche"],
			["lichlord", "rottingcorpse"]
		],
		powerScrollSkills: [
			"swordsmanship", "fencing", "macefighting", "archery", "wrestling",
			"parrying", "tactics", "anatomy", "healing",
			"magery", "meditation", "evaluatingintel", "magicresistance",
			"stealing", "throwing", "stealth",
			"taming", "animallore", "veterinary",
			"musicianship", "provocation", "discordance", "peacemaking",
			"chivalry", "focus", "necromancy", "bushido", "ninjitsu",
			"spellweaving", "spiritspeak", "mysticism"
		]
	},
	6: { // Vermin
		name: "Vermin Horde Champion",
		boss: "barracoon",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		uniqueList:      [ "artifact_fang_of_ractus" ],
		sharedList:      [ "artifact_gauntlets_of_anger", "artifact_embroidered_oak_leaf_cloak", "artifact_djinnis_ring", "artifact_legendary_detective_boots", "artifact_elder_detective_boots", "artifact_mythical_detective_boots" ],
		decorativeList:  [ "artifact_swamp_tile", "0x20E8" ],
		stageNames: ["adversary", "subjugator", "eradictor"],
		levels: [
			["giantrat", "slime"],
			["direwolf", "ratman"],
			["hellhound", "ratmanmage"],
			["ratmanarcher", "silverserpent"]
		],
		powerScrollSkills: [
			"swordsmanship", "fencing", "macefighting", "archery", "wrestling",
			"parrying", "tactics", "anatomy", "healing",
			"magery", "meditation", "evaluatingintel", "magicresistance",
			"stealing", "throwing", "stealth",
			"taming", "animallore", "veterinary",
			"musicianship", "provocation", "discordance", "peacemaking",
			"chivalry", "focus", "necromancy", "bushido", "ninjitsu",
			"spellweaving", "spiritspeak", "mysticism"
		]
	},
	7: { // Dragon Turtle
		name: "Dragon Turtle Champion",
		boss: "dragonturtle",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		uniqueList:      [ "artifact_fang_of_ractus" ],
		sharedList:      [ "artifact_gauntlets_of_anger", "artifact_embroidered_oak_leaf_cloak", "artifact_djinnis_ring", "artifact_legendary_detective_boots", "artifact_elder_detective_boots", "artifact_mythical_detective_boots" ],
		decorativeList:  [ "artifact_swamp_tile", "0x20E8" ],
		stageNames: ["Explorer", "Huntsman", "Msafiri"],
		levels: [
			["lizardman", "snake"],
			["lavalizard", "ophwarrior", "ophenforcer"],
			["drake", "ophjusticar", "ophzealot"],
			["dragon", "ophavenger", "ophapprentice"]
		],
		powerScrollSkills: [
			"swordsmanship", "fencing", "macefighting", "archery", "wrestling",
			"parrying", "tactics", "anatomy", "healing",
			"magery", "meditation", "evaluatingintel", "magicresistance",
			"stealing", "throwing", "stealth",
			"taming", "animallore", "veterinary",
			"musicianship", "provocation", "discordance", "peacemaking",
			"chivalry", "focus", "necromancy", "bushido", "ninjitsu",
			"spellweaving", "spiritspeak", "mysticism"
		]
	}
};

function ChampionSpawnData( spawnType )
{
	if( ChampionSpawnTable.hasOwnProperty( spawnType ))
		return ChampionSpawnTable[spawnType];
	return null;
}