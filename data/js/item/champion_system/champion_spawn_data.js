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
// 
//  rewards           - Array of DFN sectionIDs (items) that can be rolled as
//                      normal rewards for the top damagers when the champion
//                      dies. RewardTopDamagers() picks one random entry from
//                      this list per rewarded player.
//   UniqueList		  - Rare, unique style reward items. At most one per champ
//                      kill. Weighted as rare in logic script.
//   SharedList       - Common / standard reward items. Can drop for multiple
//                      top damagers. Weighted as the main pool.
//   DecorativeList   - Decorative / vanity items (statues, banners, hue cloth,
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
		UniqueList:      [ /* "abyss_unique_artifact_1", ... */ ],
		SharedList:      [ /* "abyss_shared_reward_1", ... */ ],
		DecorativeList:  [ /* "abyss_deco_statue_1", ... */ ],
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
		UniqueList:      [ /* "abyss_unique_artifact_1", ... */ ],
		SharedList:      [ /* "abyss_shared_reward_1", ... */ ],
		DecorativeList:  [ /* "abyss_deco_statue_1", ... */ ],
		stageNames: ["bane", "killer", "vanquisher"],
		levels: [
			["scorpion", "giantspider"],
			["terathdrone", "terathanwarrior"],
			["dreadspider", "terathanmatriarch"],
			["poisonelemental", "terathanavenger"]
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
		boss: "Rikktor",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		UniqueList:      [ /* "abyss_unique_artifact_1", ... */ ],
		SharedList:      [ /* "abyss_shared_reward_1", ... */ ],
		DecorativeList:  [ /* "abyss_deco_statue_1", ... */ ],
		stageNames: ["Blight", "Slayer", "Destroyer"],
		levels: [
			["lizardman", "snake"],
			["lavalizard", "ophidianwarrior", "ophidianenforcer"],
			["drake", "ophidianjusticar", "ophidianenzealot"],
			["dragon", "ophidianavenger", "ophidianknighterrant"]
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
		UniqueList:      [ /* "abyss_unique_artifact_1", ... */ ],
		SharedList:      [ /* "abyss_shared_reward_1", ... */ ],
		DecorativeList:  [ /* "abyss_deco_statue_1", ... */ ],
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
		UniqueList:      [ /* "abyss_unique_artifact_1", ... */ ],
		SharedList:      [ /* "abyss_shared_reward_1", ... */ ],
		DecorativeList:  [ /* "abyss_deco_statue_1", ... */ ],
		stageNames: ["Scourge", "Punisher", "Nemesis"],
		levels: [
			["ghoul", "shade", "spectre", "wraith"],
			["bonemagi", "mummie", "skeletalmage"],
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
		UniqueList:      [ /* "abyss_unique_artifact_1", ... */ ],
		SharedList:      [ /* "abyss_shared_reward_1", ... */ ],
		DecorativeList:  [ /* "abyss_deco_statue_1", ... */ ],
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
	}
};

function ChampionSpawnData( spawnType )
{
	if( ChampionSpawnTable.hasOwnProperty( spawnType ))
		return ChampionSpawnTable[spawnType];
	return null;
}