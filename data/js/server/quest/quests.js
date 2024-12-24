function getQuests() {
	return [
		{
			id: "1",
			name: "Hero's Journey",
			steps: [
				{
					description: "Collect 5 apples you can buy these or find them",
					objectives: [{ type: "collect", target: "0x09d0", count: 5, progress: 0, displayName: "Apple's" }],
					rewards: { gold: 150, items: ["apple"] }
				}
			]
		},
		{
			id: "2",
			name: "Monster Hunt",
			steps: [
				{
					description: "Kill 3 orcs",
					objectives: [{ type: "kill", target: "orc", count: 3, progress: 0, displayName: "Orc", }],
					rewards: { gold: 300, items: ["orc_helmet"] }
				}
			]
		}];
}