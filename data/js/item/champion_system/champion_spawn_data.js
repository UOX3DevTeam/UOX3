const ChampionSpawnTable = {
	"Abyss": {
		name: "Abyss Champion",
		boss: "semidar",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"], // Example DFN sections
		stageNames: ["Foe", "Assassin", "Conqueror"],
		levels: [
			["greatermongbat", "imp"],
			["gargoyle", "harpy"],
			["firegargoyle", "stonegargoyle"],
			["daemon", "succubus"]
		]
	},
	"Arachnid": {
		name: "Arachnid Champion",
		boss: "mephitis",
				rewards: ["0x0F0E", "0x0F0F", "0x0F10"], // Example DFN sections
		stageNames: ["bane", "killer", "vanquisher"],
		levels: [
			["scorpion", "giantspider"],
			["terathdrone", "terathanwarrior"],
			["dreadspider", "terathanmatriarch"],
			["poisonelemental", "terathanavenger"]
		]
	},
	"Cold": {
		name: "Cold Blood Champion",
		boss: "Rikktor",
				rewards: ["0x0F0E", "0x0F0F", "0x0F10"], // Example DFN sections
		stageNames: ["Blight", "Slayer", "Destroyer"],
		levels: [
			["lizardman", "snake"],
			["lavalizard", "ophidianwarrior", "ophidianenforcer"],
			["drake", "ophidianjusticar", "ophidianenzealot"],
			["dragon", "ophidianavenger", "ophidianknighterrant"]
		]
	},
	"Forest": {
		name: "Forest Champion",
		boss: "lordoaks",
		miniBoss: "silvani",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"], // Example DFN sections
		stageNames: ["Enemy", "Curse", "Slaughter"],
		levels: [
			["pixie", "shadowwisp"],
			["kirin", "wisp"],
			["centaur", "unicorn"],
			["etherealwarrior", "serptinedragon"]
		]
	},
	"Unholy": {
		name: "Unholy Terror Champion",
		boss: "neira",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"], // Example DFN sections
		stageNames: ["Scourge", "Punisher", "Nemesis"],
		levels: [
			["ghoul","shade","spectre", "wraith"],
			["bonemagi", "mummie", "skeletalmage"],
			["boneknight", "skeletalknight", "liche"],
			["lichlord", "rottingcorpse"]
		]
	},
	"Vermin": {
		name: "Vermin Horde Champion",
		boss: "barracoon",
		rewards: ["0x0F0E", "0x0F0F", "0x0F10"], // Example DFN sections
		stageNames: ["adversary", "subjugator", "eradictor"],
		levels: [
			["giantrat","slime"],
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