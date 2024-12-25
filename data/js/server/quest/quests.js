function getQuests() 
{
	return [
		{
			id: "1",
			name: "More Ore Please",
			steps: [
				{
					cliloc: 1075529,// If set to 0, description will be used instead
					description: "Have a pickaxe? My supplier is late and I need some iron ore so I can complete a bulk order for another merchant. If you can get me some soon I'll pay you double what it's worth on the market. Just find a cave or mountainside and try to use your pickaxe there, maybe you'll strike a good vein! 5 large pieces should do it.",
					refuse: "Not feeling strong enough today? Its alright, I didn't need a bucket of rocks anyway.",
					Uncomplete: "Hmmm… we need some more Ore. Try finding a mountain or cave, and give it a whack.",
					objectives: [{ type: "gather", target: "ore", count: 5, progress: 0, displayName: "Iron Ore" }],
					rewards: { gold: 0, items: [{ id: "pickaxe", amount: 2 }]
					}
				}
			]
		},
		{
			id: "2",
			name: "Battered Bucklers",
			steps: [
				{
					cliloc: 1075512, // If set to 0, description will be used instead
					description: "Hey there! Yeah.. you! Ya' any good with a hammer? Tell ya what, if yer thinking about tryin' some metal work, " +
						"and have a bit of skill, I can show ya how to bend it into shape.Just get some of those ingots there, and grab a " +
						"hammer and use it over here at this forge.I need a few more bucklers hammered out to fill this here order with.. " +
						"hmmm about ten more.that'll give some taste of how to work the metal.",
					refuse: "Not enough muscle on yer bones to use it? hmph, probably afraid of the sparks markin' up yer loverly skin... to good for some honest labor...ha!...off with ya!",
					Uncomplete: "Come On! Whats that... a bucket? We need ten bucklers... not spitoons.",
						objectives: [
						{
							type: "collect",
							target: "0x1b73",
							count: 10,
							progress: 0,
							displayName: "Buckler"
						}
					],
					rewards: {
						gold: 0,
						items: [
							{ id: "0x1bf2", amount: 10, color: 0x084c }, // Blue ingots
							{ id: "0x13e3", amount: 1 } // Blacksmith's hammer
						]
					}
				}
			]
		},
		{
			id: "3",
			name: "Monster Hunt",
			steps: [
				{
					description: "Kill 3 orcs",
					objectives: [{ type: "kill", target: "orc", count: 3, progress: 0, displayName: "Orc", }],
					rewards: { gold: 300, items: ["ironingots", "smithhammer"] }
				}
			]
		}];
}