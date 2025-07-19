const ChampionSpawnTable = {
	1: { // Abyss
		name: "Abyss Champion",
		boss: "semidar",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		stageNames: ["Foe", "Assassin", "Conqueror"],
		levels: [
			["greatermongbat", "imp"],
			["gargoyle", "harpy"],
			["firegargoyle", "stonegargoyle"],
			["daemon", "succubus"]
		]
	},
	2: { // Arachnid
		name: "Arachnid Champion",
		boss: "mephitis",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		stageNames: ["bane", "killer", "vanquisher"],
		levels: [
			["scorpion", "giantspider"],
			["terathdrone", "terathanwarrior"],
			["dreadspider", "terathanmatriarch"],
			["poisonelemental", "terathanavenger"]
		]
	},
	3: { // Cold
		name: "Cold Blood Champion",
		boss: "Rikktor",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		stageNames: ["Blight", "Slayer", "Destroyer"],
		levels: [
			["lizardman", "snake"],
			["lavalizard", "ophidianwarrior", "ophidianenforcer"],
			["drake", "ophidianjusticar", "ophidianenzealot"],
			["dragon", "ophidianavenger", "ophidianknighterrant"]
		]
	},
	4: { // Forest
		name: "Forest Champion",
		boss: "lordoaks",
		//miniBoss: "silvani",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		stageNames: ["Enemy", "Curse", "Slaughter"],
		levels: [
			["pixie", "shadowwisp"],
			["kirin", "wisp"],
			["centaur", "unicorn"],
			["etherealwarrior", "serptinedragon"]
		]
	},
	5: { // Unholy
		name: "Unholy Terror Champion",
		boss: "neira",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		stageNames: ["Scourge", "Punisher", "Nemesis"],
		levels: [
			["ghoul", "shade", "spectre", "wraith"],
			["bonemagi", "mummie", "skeletalmage"],
			["boneknight", "skeletalknight", "liche"],
			["lichlord", "rottingcorpse"]
		]
	},
	6: { // Vermin
		name: "Vermin Horde Champion",
		boss: "barracoon",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"],
		stageNames: ["adversary", "subjugator", "eradictor"],
		levels: [
			["giantrat", "slime"],
			["direwolf", "ratman"],
			["hellhound", "ratmanmage"],
			["ratmanarcher", "silverserpent"]
		]
	}
};

function ChampionSpawnData( spawnType )
{
	if( ChampionSpawnTable.hasOwnProperty( spawnType ))
		return ChampionSpawnTable[spawnType];
	return null;
}