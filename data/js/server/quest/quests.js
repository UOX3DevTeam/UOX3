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
			npcPhrases: [
				"Bring me iron, and I'll reward you handsomely.",
				"Have you collected the ore yet?",
				"Don't keep me waiting for those materials!"
			],
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
		"5": {
			title: "Battered Bucklers",
			description: "Hey there! Yeah... you! Ya' any good with a hammer? Tell ya what, if yer thinking about tryin' some metal work, " +
						 "and have a bit of skill, I can show ya how to bend it into shape.Just get some of those ingots there, and grab a " +
						 "hammer and use it over here at this forge.I need a few more bucklers hammered out to fill this here order with..." +
						 "hmmm about ten more.that'll give some taste of how to work the metal.",
			uncomplete: "Come On! Whats that... a bucket? We need ten bucklers... not spitoons.",
			complete: "Thanks for the help. Here's something for ya to remember me by.",
			refuse: "Not enough muscle on yer bones to use it? hmph, probably afraid of the sparks markin' up yer loverly skin... to" +
					 "good for some honest labor...ha!...off with ya!",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [
				{ itemID: "Buckler", amount: 10, name: "Buckler" }
			],
			rewards: [
				{ type: "item", itemID: "smithssatchel", amount: 1, name: "Craftsman's Satchel" }
			]
		},
		"6": {
			title: "A clockwork puzzle",
			description: "'Tis a riddle, you see! What kind of clock is only right twice per day? A broken one! laughs heartily " +
						 "Ah, yes *wipes eye*, that's one of my favorites! Ah... to business. Could you fashion me some clock parts? " +
						 "I wish my own clocks to be right all the day long! You'll need some tinker's tools and some iron ingots, I " +
						 "think, but from there it should be just a matter of working the metal.",
			uncomplete: "You're not quite done yet.  Get back to work!",
			complete: "Wonderful! Tick tock, tick tock, soon all shall be well with grandfather's clock!",
			refuse: "Or perhaps you'd rather not.",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [
				{ itemID: "0x104F", amount: 5, name: "Clock Parts" }
			],
			rewards: [
				{ type: "item", itemID: "tinkerssatchel", amount: 1, name: "Craftsman's Satchel" }
			]
		},
		"7": {
			title: "The Delucian’s Lost Mine",
			description: "Find Jacob's Lost Mine and mine iron ore there, using a pickaxe or shovel. Bring it back to Jacob's forge and " +
						 "smelt the ore into ingots, until you have raised your Mining skill to 50. You may find a packhorse useful for " +
						 "hauling the ore around.The animal trainer in New Haven has packhorses for sale.< br ><center>-----</center><br>Howdy! " +
						 "Welcome to my camp. It's not much, I know, but it's all I'll be needin' up here. I don't need them fancy things those " +
						 "townspeople have down there in New Haven. Nope, not one bit. Just me, Bessie, my pick and a thick vein 'o valorite. " +
						 "<br><br>Anyhows, I'm guessin' that you're up here to ask me about minin', aren't ya? Well, don't be expectin' me to " +
						 "tell you where the valorite's at, cause I ain't gonna tell the King of Britannia, much less the likes of you. But I " + 
						 "will show ya how to mine and smelt iron, cause there certainly is a 'nough of up in these hills.<br><br>*Jacob looks " +
						 "around, with a perplexed look on his face*<br><br>Problem is, I can't remember where my iron mine's at, so you'll " +
						 "have to find it yourself. Once you're there, have at it with a pickaxe or shovel, then haul it back to camp and I'll " +
						 "show ya how to smelt it. Ya look a bit wimpy, so you might wanna go buy yourself a packhorse in town from the animal " +
						 "trainer to help you haul around all that ore.<br><br>When you're an Apprentice Miner, talk to me and I'll give ya a " +
						 "little somethin' I've got layin' around here... somewhere.",
			uncomplete: "Where ya been off a gallivantin’ all day, pilgrim? You ain’t seen no hard work yet! Get yer arse back out there to " +
						"my mine and dig up some more iron.Don’t forget to take a pickaxe or shovel, and if you’re so inclined, a packhorse too.",
			complete: "Dang gun it! If that don't beat all! Ya went and did it, didn’t ya? What we got ourselves here is a mighty fine brand " +
					  "spankin’ new Apprentice Miner!<br>< br > I can see ya put some meat on them bones too while you were at it!<br>< br > Here’s " +
					  "that little somethin’ I told ya I had for ya.It’s a pickaxe with some high falutin’ magic inside that’ll help you find " +
					  "the good stuff when you’re off minin’. It wears out fast, though, so you can only use it a few times a day.< br > <br>Welp," +
					  "I’ve got some smeltin’ to do, so off with ya.Good luck, pilgrim !",
			refuse: "Couldn’t find my iron mine, could ya? Well, neither can I!<br><br>*Jacob laughs*<br><br>Oh, ya don’t wanna find it? " +
					"Well, allrighty then, ya might as well head on back down to town then and stop cluttering up my camp.Come back and " +
					"talk to me if you’re interested in learnin’ ‘bout minin’.",
			oncomplete: "You have achieved the rank of Apprentice Miner. Return to Jacob Waltz in at his camp in the hills above New Haven as soon as you can to claim your reward.",
			trainingarea: "You can almost smell the ore in the rocks here! Your ability to improve your Mining skill is enhanced in this area.",
			npcPhrases: [
				"You there! I can use some help mining these rocks!"
			],
			type: "skillgain",
			questTurnIn: 1, // 0 = auto-complete
			targetRegion: 194, // Example region ID for sparring grounds
			regionName: "Haven Mountains",
			targetSkill: 45, // Skill ID 45
			maxSkillPoints: 500, // 50.0 in-game skill points
			minPoint: 2,   //min skill point 0.2
			maxPoint: 5,	//max skill point 0.5
			rewards: [
				{ type: "item", itemID: "jacobspickaxe", amount: 1, name: "Jacob's Pickaxe" }
			]
		},
		//Custom Example Quests
		"1000": {
			title: "Collect Fruits",
			description: "Collect 2 apples, 3 oranges, and 1 banana.",
			uncomplete: "You have not collected all the fruits yet.",
			complete: "You have collected all the fruits! Here is your reward.",
			refuse: "You have refused to collect the fruits.",
			npcPhrases: [
				"Bring me iron, and I'll reward you handsomely.",
				"Have you collected the ore yet?",
				"Don't keep me waiting for those materials!"
			],
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