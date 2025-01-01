function QuestList(questID)
{
	var questList = {
		//New Player Quests
		"1": {
			title: "More Ore Please",
			description: "Have a pickaxe? My supplier is late and I need some iron ore so I can complete a bulk order for another " +
						 "merchant. If you can get me some soon I'll pay you double what it's worth on the market. Just find a cave " +
						 "or mountainside and try to use your pickaxe there, maybe you'll strike a good vein! 5 large pieces should do it.",
			uncomplete: "Hmmm… we need some more Ore. Try finding a mountain or cave, and give it a whack.",
			complete: "I see you found a good vien! Great!  This will help get this order out on time. Good work!",
			refuse: "Not feeling strong enough today? Its alright, I didn't need a bucket of rocks anyway.",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [
				{ itemID: "ore", amount: 5, name: "Iron Ore" }
			],
			rewards: [
				{ type: "item", itemID: "minerssatchel", amount: 1, name: "Craftsman's Satchel" }
			]
		},
		"2": {
			title: "A Stitch in Time",
			description: "Oh how I wish I had a fancy dress like the noble ladies of Castle British! I don't have much... but I " +
						 "have a few trinkets I might trade for it. It would mean the world to me to go to a fancy ball and dance " +
						 "the night away. Oh, and I could tell you how to make one! You just need to use your sewing kit on enough " +
						 "cut cloth, that's all.",
			uncomplete: "Hello again! Do you need anything? You may want to visit the tailor's shop for cloth and a sewing kit, " +
						"if you don't already have them.",
			complete: "It's gorgeous! I only have a few things to give you in return, but I can't thank you enough! Maybe I'll " +
					  "even catch Uzeraan's eye at the, er, *blushes* I mean, I can't wait to wear it to the next town dance!",
			refuse: "Won't you reconsider? It'd mean the world to me, it would!",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [
				{ itemID: "0x1eff", amount: 1, name: "Fancy Dress" }
			],
			rewards: [
				{ type: "item", itemID: "oldring", amount: 1, name: "an old ring" },
				{ type: "item", itemID: "oldnecklace", amount: 1, name: "an old necklace" }
			]
		},
		"3": {
			title: "Baker's Dozen",
			description: "You there! Do you know much about the ways of cooking? If you help me out, I'll show you a thing or two about " +
						 "how it's done. Bring me some cookie mix, about 5 batches will do it, and I will reward you. Although, I don't " +
						 "think you can buy it, you can make some in a snap! First get a rolling pin or frying pan or even a flour sifter. " +
						 "Then you mix one pinch of flour with some water and you've got some dough! Take that dough and add one dollop of " +
						 "honey and you've got sweet dough. add one more drop of honey and you've got cookie mix.See ? Nothing to it! Now " +
						 "get to work",
			uncomplete: "You're not quite done yet.  Get back to work!",
			complete: "Thank you! I haven't been this excited about food in months!",
			refuse: "Argh, I absolutely must have more of these 'cookies!' Come back if you change your mind.",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [
				{ itemID: "0x103F", amount: 5, name: "Cookie Mix" }
			],
			rewards: [
				{ type: "item", itemID: "chefssatchel", amount: 1, name: "Craftsman's Satchel" }
			]
		},
		"4": {
			title: "Chop Chop, On The Double!",
			description: "That's right, move it! I need sixty logs on the double, and they need to be freshly cut! If you can get them to " +
						 "me fast I'll have your payment in your hands before you have the scent of pine out from beneath your nostrils. Just " +
						 "get a sharp axe and hack away at some of the trees in the land and your lumberjacking skill will rise in no time.",
			uncomplete: "You're not quite done yet.  Get back to work!",
			complete: "Ahhh! The smell of fresh cut lumber. And look at you, all strong and proud, as if you had done an honest days work!",
			refuse: "Or perhaps you'd rather not.",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [
				{ itemID: "0x1BE0", amount: 60, name: "Log" }
			],
			rewards: [
				{ type: "item", itemID: "lumberjackssatchel", amount: 1, name: "Craftsman's Satchel" }
			]
		},
		//Custom Example Quests
		"1000": {
			title: "Collect Fruits",
			description: "Collect 2 apples, 3 oranges, and 1 banana.",
			uncomplete: "You have not collected all the fruits yet.",
			complete: "You have collected all the fruits! Here is your reward.",
			refuse: "You have refused to collect the fruits.",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [
				{ itemID: "0x13e3", amount: 2, name: "Apples" }, // Apples
				{ itemID: "0x13e4", amount: 3, name: "Oranges" }, // Oranges
				{ itemID: "0x13e5", amount: 1, name: "Bananas" }  // Bananas
			],
			rewards: [
				{ type: "gold", amount: 500, name: "Gold Coins" },
				{ type: "item", itemID: "0x1f4c", amount: 1, name: "Magical Scroll" },
				{ type: "karma", amount: 50, name: "Karma Points" },
				{ type: "fame", amount: 30, name: "Fame Points" }
			]
		},
		"1001": {
			title: "Hunt Dangerous Creatures",
			description: "Kill 1 dog and 1 cat.",
			type: "kill",
			targetKills: [
				{ npcID: "dog", amount: 1 }, // Kill 5 orcs
				{ npcID: "cat", amount: 1 }  // Kill 3 cats
			],
			rewards: [
				{ type: "gold", amount: 1000 },
				{ type: "karma", amount: 50 },
				{ type: "fame", amount: 30 }
			]
		},
		"1002": {
			title: "Collect Fruits",
			type: "collect",
			targetItems: [
				{ itemID: "0x13e3", amount: 2 }, // Apples
			],
			rewards: [
				{ type: "gold", amount: 500 }, // Give 500 gold
				{ type: "item", itemID: "0x1f4c", amount: 1 }, // Give a magical scroll
				{ type: "karma", amount: 50 }, // Add 50 karma
				{ type: "fame", amount: 30 }   // Add 30 fame
			]
		},
		"1003": {
			title: "Gather Fruits and Hunt Animals",
			type: "multi", // New type for combined objectives
			targetItems: [
				{ itemID: "0x13e3", amount: 1 } // Collect 3 apples
			],
			targetKills: [
				{ npcID: "dog", amount: 1 }, // Kill 1 dog
			],
			rewards: [
				{ type: "gold", amount: 500 }, // Give 500 gold
				{ type: "item", itemID: "0x1f4c", amount: 1 }, // Give a magical scroll
				{ type: "karma", amount: 50 }, // Add 50 karma
				{ type: "fame", amount: 30 }   // Add 30 fame
			]
		},
		"1004": {
			title: "Timed Fruit Collection",
			type: "timecollect",
			targetItems: [
				{ itemID: "0x13e3", amount: 5 } // Collect 5 apples
			],
			timeLimit: 300, // Time limit in seconds
			rewards: [
				{ type: "gold", amount: 100 },
				{ type: "karma", amount: 10 }
			]
		},
		"1005": {
			title: "Timed Hunt Animals",
			type: "timekills",
			targetKills: [
				{ npcID: "dog", amount: 1 }, // Kill 1 dog
			],
			timeLimit: 100, // Time limit in seconds
			rewards: [
				{ type: "gold", amount: 100 },
				{ type: "karma", amount: 10 }
			]
		},
		"1006": {
			title: "Gather Apples",
			description: "Collect 10 apples from the orchard.",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [{ itemID: "0x13e3", amount: 1, name: "Apple" }],
			rewards: [{ type: "gold", amount: 500 }],
			nextQuestID: "1007" // Chain to the next quest
		},
		"1007": {
			title: "Defeat the Bandits",
			description: "Defeat 5 bandits in the nearby forest.",
			type: "kill",
			questTurnIn: 1, // 0 = auto-complete
			targetKills: [{ npcID: "cat", amount: 1 }],
			rewards: [{ type: "gold", amount: 1000 }],
			nextQuestID: "1008" // Chain to the next quest
		},
		"1008": {
			title: "Deliver Supplies to the Guard",
			description: "Bring the supplies to the guard at the eastern gate.",
			type: "delivery",
			targetDelivery: { npcID: "nail", location: { x: 1000, y: 2000, z: 0 } },
			deliveryItem: { itemID: "0x1F4C", amount: 1 },
			rewards: [
				{ type: "gold", amount: 1000 },
				{ type: "item", itemID: "0x1B7A", amount: 1 }
			],
			nextQuestID: null
		},
		"1011": {
			"title": "Deliver the Package to Nail",
			"type": "delivery",
			"description": "Deliver the package to Nail, the receiver, located at his usual spot.",
			"deliveryItem": {
				"itemID": "0x0E21", // Package item ID
				"name": "Sealed Package", // Friendly name for the item
				"amount": 1 // Quantity
			},
			"targetDelivery": {
				"sectionID": "nail", // Section ID of Nail
				"name": "Nail the Receiver", // Name of the NPC
				"location": {
					"x": 1234, // X coordinate
					"y": 5678, // Y coordinate
					"z": 0,    // Z coordinate
					"world": 1 // World number
				}
			},
			"rewards": [
				{ "type": "gold", "amount": 500 },
				{ "type": "item", "itemID": "0x1F4C", "amount": 1 }
			]
		}
	};
	// Return the full quest list if no questID is provided
	if (!questID)
	{
		return questList;
	}

	// Return a specific quest if questID is provided
	return questList[questID] || null;
}