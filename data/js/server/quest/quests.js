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
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
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
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
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
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
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
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
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
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
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
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
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
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
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
		"8": {
			title: "The Inner Warrior",
			description: "Head East out of town to Old Haven. Expend stamina and mana until you have raised your Focus skill to 50. Well, hello there. " +
						 "Don't you like quite the adventureer! You want to learn more about Focus, do you? I can teach you something about that, but " +
						 "first you should know that not everyone can be a disciplined enough to excel at it. Focus is the ability to achive inner balance " +
						 "in both body and spirit, so that you recover from physical and mental exertion faster than you other wise would.If you want to " +
						 "practice Focus, the best place to do that is east of here, in Old Haven, where you'll find an undead infestation, Exert yourself " +
						 "physically by engaging in combat and moving quickly.For testing your mental balance, expend mana in whatever way you find most " +
						 "suitable to your abilites.Casting spells and using abilites work well for consuming your mana.Go.Train hard, and you will find " +
						 "that your concentration will imporve naturally.When you've improved your ability to focus yourself at an Apprentice level, come " +
						 "back to me and i shall give you something worthy of your new ability.",
			uncomplete: "Hell again. I see you've returned, but it seems that your Focus skill hasn't improved as much as it could have. Just head east, " +
						"to Old Haven, and exert yourself physically and mentally as much as possible. To do this physically engage in combat and move as " +
						"quickly as you can.For exerting yourself mentally, expend mana in whatever way you find most suitable to your abilites.Casting " +
						"spells and using abilites work well for consuming your mana.",
			complete: "Look Who it is! I knew you could do it if you just had the discipline to apply yourself. It feels good to recover from battle " +
					  "so quickly, doesn't it? Just wait until you become a Grandmaster, It's amazing!	Please take this gift, as you've more than earned " +
					  "it with your hard work.It will help you recover even faster during battle, and provides a bit of protection aswell.You have so " +
					  "much more potential, so don't stop trying to improve your Focus now! Safe travels!",
			refuse: "I'm disappointed. You have alot of inner potential, and it would pain me greatly to see you waste that. Oh well. If you change your mind ill be right here.",
			oncomplete: "You have achieved the rank of Apprentice Miner. Return to Jacob Waltz in at his camp in the hills above New Haven as soon as you can to claim your reward.",
			trainingarea: "You feel much more attuned to yourself. Your ability to improve Focus skill is enhanced in this area.",
			npcPhrases: [
				"Know yourself, and you will become a true warrior."
			],
			type: "skillgain",
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0, // 0 gold drops in pack if 1 gold goes to bank.
			targetRegion: 195, // Example region ID for sparring grounds
			regionName: "Old Haven Training",
			targetSkill: 50, // Skill ID 50
			maxSkillPoints: 500, // 50.0 in-game skill points
			minPoint: 2,   //min skill point 0.2
			maxPoint: 5,	//max skill point 0.5
			rewards: [
				{ type: "item", itemID: "claspofconcentration", amount: 1, name: "Clasp of Concentration" }
			]
		},
		"9": {
			"title": "It's Hammer Time!",
			"description": "Create new daggers and maces using the forge and anvil in George's shop. Try making daggers up to 45 skill, then switch to making maces until 50 skill. Hail, and welcome to my humble shop. I am George Hephaestus, New Haven's blacksmith. I assume that you're here to ask me to train you to be an Apprentice Blacksmith. I certainly can do that, but you're going to have to supply your own ingots. You can always buy them at the market, but I highly suggest that you mine your own. That way, any items you sell will be pure profit! So, once you have a supply of ingots, use my forge and anvil to create items. You'll also need a supply of the proper tools, you can use a smith's hammer, a sledgehammer, or tongs. You can either make them yourself if you have the tinkering skill, or buy them from a tinker at the market. Since I'll be around to give you advice, you'll learn faster here than anywhere else. Start off making daggers until you reach 45 blacksmithing skill, then switch to maces until you've achieved 50. Once you've done that, come talk to me and I'll give you something for your hard work.",
			"uncomplete": "You're doing well, but you're not quite there yet. Remember that the quickest way to learn is to make daggers up to 45 skill, and then switch to maces. Also, don't forget that using my forge and anvil will help you learn faster.",
			"complete": "I've been watching you get better and better as you've been smithing, and I have to say, you're a natural! It's a long road to being a Grandmaster Blacksmith, but I have no doubt that if you put your mind to it you'll get there someday. Let me give you one final piece of advice. If you're smithing just to practice and improve your skill, make items that are moderately difficult (60%-80% success chance), and try to stick to ones that use fewer ingots. Now that you're an Apprentice Blacksmith, I have something for you. While you were busy practicing, I was crafting this hammer for you. It's finely balanced and has a bit of magic imbued within that will help you craft better items. However, that magic needs to restore itself over time, so you can only use it so many times per day. I hope you find it useful.",
			"refuse": "You're not interested in learning to be a smith, eh? I thought for sure that's why you were here. Oh well, if you change your mind, you can always come back and talk to me.",
			"oncomplete": "You have achieved the rank of Apprentice Blacksmith. Return to George Hephaestus in New Haven to claim your reward.",
			"trainingarea": "You feel much more attuned to blacksmithing. Your ability to improve blacksmithing skill is enhanced in this area.",
			"npcPhrases": [
				"A steady hand creates a perfect blade."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 195,
			"regionName": "New Haven Blacksmith Shop",
			"targetSkill": 45,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "apprenticeblacksmithhammer", "amount": 1, "name": "Apprentice Blacksmith Hammer" }
			]
		},
		"10": {
			"title": "The Right Tool for the Job",
			"description": "Create new scissors and hammers while inside Amelia's workshop. Try making scissors up to 45 skill, then switch to making hammers until 50 skill. Hello! I guess you're here to learn something about Tinkering, eh? You've come to the right place, as Tinkering is what I've dedicated my life to. You'll need two things to get started: a supply of ingots and the right tools for the job. You can either buy ingots from the market or go mine them yourself. As for tools, you can try making your own set of Tinker's Tools, or if you'd prefer to buy them, I have some for sale. Working here in my shop will let me give you pointers as you go, so you'll be able to learn faster than anywhere else. Start off making scissors until you reach 45 tinkering skill, then switch to hammers until you've achieved 50. Once you've done that, come talk to me and I'll give you something for your hard work.",
			"uncomplete": "Nice going! You're not quite at Apprentice Tinkering yet, though, so you better get back to work. Remember that the quickest way to learn is to make scissors up until 45 skill, and then switch to hammers. Also, don't forget that working here in my shop will let me give you tips so you can learn faster.",
			"complete": "You've done it! Look at our brand-new Apprentice Tinker! You've still got quite a lot to learn if you want to be a Grandmaster Tinker, but I believe you can do it! Just keep in mind that if you're tinkering just to practice and improve your skill, make items that are moderately difficult (60-80% success chance), and try to stick to ones that use fewer ingots. Come here, my brand-new Apprentice Tinker, I want to give you something special. I created this just for you, so I hope you like it. It's a set of Tinker's Tools that contains a bit of magic. These tools have more charges than any Tinker's Tools a Tinker can make. You can even use them to make a normal set of tools, so that way you won't ever find yourself stuck somewhere with no tools!",
			"refuse": "I’m disappointed that you aren’t interested in learning more about Tinkering. It’s really such a useful skill! *Amelia smiles* At least you know where to find me if you change your mind, since I rarely spend time outside of this shop.",
			"oncomplete": "You have achieved the rank of Apprentice Tinker. Return to Amelia at her workshop to claim your reward.",
			"trainingarea": "By tinkering inside of Amelia’s workshop, she is able to give you advice. This helps you hone your Tinkering skill faster than normal.",
			"npcPhrases": [
				"Every tool has a purpose; find yours.",
				"Tinkering is very useful for a blacksmith.You can make your own tools."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 196,
			"regionName": "Springs And Things Workshop",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "ameliastoolbox", "amount": 1, "name": "Amelia's Toolbox" }
			]
		},
		"11": {
			"title": "Knowing Thine Enemy",
			"description": "Head East out of town to Old Haven. Battle monsters there, or heal yourself and other players, until you have raised your Anatomy skill to 50. Hail and well met. You must be here to improve your knowledge of Anatomy. Well, you've come to the right place because I can teach you what you need to know. At least all you'll need to know for now. Haha! Knowing about how living things work inside can be a very useful skill. Not only can you learn where to strike an opponent to hurt him the most, but you can use what you learn to heal wounds better as well. Just walking around town, you can even tell if someone is strong or weak or if they happen to be particularly dexterous or not. If you're interested in learning more, I'd advise you to head out to Old Haven, just to the east, and jump into the fray. You'll learn best by engaging in combat while keeping you and your fellow adventurers healed, or you can even try sizing up your opponents. While you're gone, I'll dig up something you may find useful.",
			"uncomplete": "I'm surprised to see you back so soon. You've still got a ways to go if you want to really understand the science of Anatomy. Head out to Old Haven and practice combat and healing yourself or other adventurers.",
			"complete": "By the Virtues, you've done it! Congratulations mate! You still have quite a ways to go if you want to perfect your knowledge of Anatomy, but I know you'll get there someday. Just keep at it. In the meantime, here's a piece of armor that you might find useful. It's not fancy, but it'll serve you well if you choose to wear it. Happy adventuring, and remember to keep your cranium separate from your clavicle!",
			"refuse": "It's your choice, but I wouldn't head out there without knowing what makes those things tick inside! If you change your mind, you can find me right here dissecting frogs, cats or even the occasional unlucky adventurer.",
			"oncomplete": "You have achieved the rank of Apprentice Anatomist. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel very willing to learn more about the body. Your ability to hone your Anatomy skill is enhanced in this area.",
			"npcPhrases": [
				"Know thy enemy, and victory shall be yours.",
				"Learning of the body will allow you to excel in combat."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 197,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "tunicoffguarding", "amount": 1, "name": "Tunic of Guarding" }
			]
		},
		"12": {
			"title": "Bruises, Bandages, and Blood",
			"description": "Head East out of town and go to Old Haven. Heal yourself and other players until you have raised your Healing skill to 50. Ah, welcome to my humble practice. I am Avicenna, New Haven's resident Healer. A lot of adventurers head out into the wild from here, so I keep rather busy when they come back bruised, bleeding, or worse. I can teach you how to bandage a wound, sure, but it's not a job for the queasy! For some folks, the mere sight of blood is too much for them, but it's something you'll get used to over time. It is one thing to cut open a living thing, but it's quite another to sew it back up and save it from sure death. 'Tis noble work, healing. Best way for you to practice fixing up wounds is to head east out to Old Haven and either practice binding up your own wounds, or practice on someone else. Surely they'll be grateful for the assistance. Make sure to take enough bandages with you! You don't want to run out in the middle of a tough fight.",
			"uncomplete": "Hail! 'Tis good to see you again. Unfortunately, you're not quite ready to call yourself an Apprentice Healer quite yet. Head back out to Old Haven, due east from here, and bandage up some wounds. Yours or someone else's, it doesn't much matter.",
			"complete": "Hello there, friend. I see you've returned in one piece, and you're an Apprentice Healer to boot! You should be proud of your accomplishment, as not everyone has 'the touch' when it comes to healing. I can't stand to see such good work go unrewarded, so I have something I'd like you to have. It's not much, but it'll help you heal just a little faster, and maybe keep you alive. Good luck out there, friend, and don't forget to help your fellow adventurer whenever possible!",
			"refuse": "No? Are you sure? Well, when you feel that you're ready to practice your healing, come back to me. I'll be right here, fixing up adventurers and curing the occasional cold!",
			"oncomplete": "You have achieved the rank of Apprentice Healer. Return to Avicenna in New Haven to claim your reward.",
			"trainingarea": "You feel fresh and are eager to heal wounds. Your ability to improve your Healing skill is enhanced in this area.",
			"npcPhrases": [
				"A healer's touch saves lives, even your own.",
				" A warrior needs to learn how to apply bandages to wounds."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 198,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "healerstouch", "amount": 1, "name": "Healer's Touch" }
			]
		},
		"13": {
			"title": "Becoming One With The Shadows",
			"description": "Practice hiding in the Ninja Dojo until you reach 50 Hiding skill. Come closer. Don't be afraid. The shadows will not harm you. To be a successful Ninja, you must learn to become one with the shadows. The Ninja Dojo is the ideal place to learn the art of concealment. Practice hiding here. Talk to me once you have achieved the rank of Apprentice Rogue (for Hiding), and I shall reward you.",
			"uncomplete": "You have not achieved the rank of Apprentice Rogue (for Hiding). Talk to me when you feel you have accomplished this.",
			"complete": "Not bad at all. You have learned to control your fear of the dark and you are becoming one with the shadows. If you haven't already talked to Jun, I advise you do so. Jun can teach you how to stealth undetected. Hiding and Stealth are essential skills to master when becoming a Ninja. As promised, I have a reward for you. Here are some smokebombs. As long as you are an Apprentice Ninja and have mana available, you will be able to use them. They will allow you to hide while in the middle of combat. I hope these serve you well.",
			"refuse": "If you wish to become one with the shadows, come back and talk to me.",
			"oncomplete": "You have achieved the rank of Apprentice Rogue. Return to your instructor at the Ninja Dojo to claim your reward.",
			"trainingarea": "You feel you can easily slip into the shadows here. Your ability to hide is enhanced in this area.",
			"npcPhrases": [
				"Fear not the shadows; become one with them.",
				"The shadows are your allies, not your enemies.",
				"To be undetected means you cannot be harmed."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 199,
			"regionName": "Haven Dojo",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "bagofsmokebombs", "amount": 1, "name": "Bag of Smoke Bombs" }
			]
		},
		"14": {
			"title": "Walking Silently",
			"description": "Head East out of town and go to Old Haven. While wearing normal clothes, practice Stealth there until you reach 50 Stealth skill. You there. You're not very quiet in your movements. I can help you with that. Not only must you learn to become one with the shadows, but also you must learn to quiet your movements. Old Haven is the ideal place to learn how to Stealth. Head East out of town and go to Old Haven. While wearing normal clothes, practice Stealth there. Stealth becomes more difficult as you wear heavier pieces of armor, so for now, only wear clothes while practicing Stealth. You can only Stealth once you are hidden. If you become visible, use your Hiding skill, and begin slowly walking. Come back to me once you have achieved the rank of Apprentice Rogue (for Stealth), and I will reward you with something useful.",
			"uncomplete": "You have not achieved the rank of Apprentice Rogue (for Stealth). Talk to me when you feel you have accomplished this.",
			"complete": "Good. You have learned to quiet your movements. If you haven't already talked to Chiyo, I advise you do so. Chiyo can teach you how to become one with the shadows. Hiding and Stealth are essential skills to master when becoming a Ninja. Here is your reward. This leather Ninja jacket is called 'Twilight Jacket.' It will offer greater protection to you. I hope this will serve you well.",
			"refuse": "If you want to learn to quiet your movements, talk to me and I will help you.",
			"oncomplete": "You have achieved the rank of Apprentice Rogue. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel you can easily slip into the shadows and walk silently here. Your ability to Stealth is enhanced in this area.",
			"npcPhrases": [
				"Quiet steps lead to survival in the shadows.",
				"Walk Silently. Remain unseen. I can teach you."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 200,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "twilightjacket", "amount": 1, "name": "Twilight Jacket" }
			]
		},
		"15": {
			"title": "The Art of Stealth",
			"description": "Head East out of town and go to Old Haven. While wielding your fencing weapon, battle monsters with focus attack and summon mirror images up to 40 Ninjitsu skill, and continue practicing focus attack on monsters until 50 Ninjitsu skill. Welcome, young one. You seek to learn Ninjitsu. With it, and the book of Ninjitsu, a Ninja can invoke a number of special abilities including transforming into a variety of creatures that give unique bonuses, using stealth to attack unsuspecting opponents, or just plain disappearing into thin air! If you do not have a book of Ninjitsu, you can purchase one from me. I have an assignment for you. Head East out of town and go to Old Haven. While wielding your fencing weapon, battle monsters with focus attack and summon mirror images up to the Novice rank, and continue focusing your attack for greater damage on monsters until you become an Apprentice Ninja. Each image will absorb one attack. The art of deception is a strong defense. Use it wisely. Come back to me once you have achieved the rank of Apprentice Ninja, and I shall reward you with something useful.",
			"uncomplete": "You have not achieved the rank of Apprentice Ninja. Come back to me once you have done so.",
			"complete": "You have done well, young one. Please accept this kryss as a gift. It is called the 'Silver Serpent Blade.' With it, you will strike with precision and power. This should aid you in your journey as a Ninja. Farewell.",
			"refuse": "Come back to me if you want to learn Ninjitsu in the future.",
			"oncomplete": "You have achieved the rank of Apprentice Ninja. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel a greater sense of awareness here. Your ability to hone your Ninjitsu skill is enhanced in this area.",
			"npcPhrases": [
				"Master the art of deception, for it is a Ninja's greatest weapon.",
				"I can teach you Ninjitsu. The Art of Stealth."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 201,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "silverserpentblade", "amount": 1, "name": "Silver Serpent Blade" }
			]
		},
		"16": {
			"title": "Eyes of a Ranger",
			"description": "Track animals, monsters, and people on Haven Island until you have raised your Tracking skill to 50. Hello, Friend. I am Walker, Grandmaster Ranger. An adventurer needs to keep alive in the wilderness. Being able to track those around you is essential to surviving in dangerous places. Certain Ninja abilities are more potent when the Ninja possesses Tracking knowledge. If you want to be a Ninja, or if you simply want to get a leg up on the creatures that inhabit these parts, I advise you to learn how to track them. You can track any animals, monsters, or people on Haven Island. Clear your mind, focus, and note any tracks in the ground or sounds in the air that can help you find your mark. You can do it, friend. I have faith in you. Come back to me once you have achieved the rank of Apprentice Ranger (for Tracking), and I will give you something that may help in your travels. Take care, friend.",
			"uncomplete": "So far so good, kid. You are still alive, and you are getting the hang of Tracking. There are many more animals, monsters, and people to track. Come back to me once you have tracked them.",
			"complete": "I knew you could do it! You have become a fine Ranger. Just keep practicing, and one day you will become a Grandmaster Ranger, just like me. I have a little something for you that will hopefully aid you in your journeys. These leggings offer some resistances that will hopefully protect you from harm. I hope these serve you well. Farewell, friend.",
			"refuse": "Farewell, friend. Be careful out here. If you change your mind and want to learn Tracking, come back and talk to me.",
			"oncomplete": "You have achieved the rank of Apprentice Ranger. Return to Walker on Haven Island to claim your reward.",
			"trainingarea": "You feel you can track creatures here with ease. Your Tracking skill is enhanced in this area.",
			"npcPhrases": [
				"The wilderness holds secrets for those who can track them.",
				"There is no theory of evolution. Just a list of creatures I allow to live.",
				"I don't sleep. I wait.",
				"I can lead a horse to water and make it drink."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 202,
			"regionName": "Haven Island",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "walkersleggings", "amount": 1, "name": "Walker's Leggings" }
			]
		},
		"17": {
			"title": "The Way of the Samurai",
			"description": "Head East out of town and go to Old Haven. Use the Confidence defensive stance and attempt to honorably execute monsters there until you have raised your Bushido skill to 50. Greetings. I see you wish to learn the Way of the Samurai. Wielding a blade is easy. Anyone can grasp a sword's hilt. Learning how to fight properly and skillfully is to become an Armsman. Learning how to master weapons, and even more importantly when not to use them, this is the Way of the Warrior. The Way of the Samurai. The Code of Bushido. That is why you are here. Adventure East to Old Haven. Use the Confidence defensive stance and attempt to honorably execute the undead that inhabit there. You will need a book of Bushido. If you do not possess a book of Bushido, you can purchase one from me. If you fail to honorably execute the undead, your defenses will be greatly weakened. Resistances will suffer and Resisting Spells will suffer. A successful parry instantly ends the weakness. If you succeed, however, you will be infused with strength and healing. Your swing speed will also be boosted for a short duration. With practice, you will learn how to master your Bushido abilities. Return to me once you feel that you have become an Apprentice Samurai.",
			"uncomplete": "You have not yet proven yourself ready to become an Apprentice Samurai. There are still many more undead to lay to rest. Return to me once you have done so.",
			"complete": "You have proven yourself, young one. You will continue to improve as your skills are honed with age. You are an honorable warrior, worthy of the rank Apprentice Samurai. Please accept this no-dachi as a gift. It is called 'The Dragon's Tail.' Upon a successful strike in combat, there is a chance this mighty weapon will replenish your stamina equal to the damage of your attack. I hope 'The Dragon's Tail' serves you well. You have earned it. Farewell for now.",
			"refuse": "Good journey to you. Return to me if you wish to live the life of a Samurai.",
			"oncomplete": "You have achieved the rank of Apprentice Samurai. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your Bushido potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"An honorable warrior lives by the Code of Bushido.",
				"Seek me to learn the way of the samurai."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 203,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "thedragonstail", "amount": 1, "name": "The Dragon's Tail" }
			]
		},
		"18": {
			"title": "Defying the Arcane",
			"description": "Head East out of town and go to Old Haven. Battle spell-casting monsters there until you have raised your Resisting Spells skill to 50. Hail and well met! To become a true master of the arcane art of Magery, I suggest learning the complementary skill known as Resisting Spells. While the name of this skill may suggest that it helps with resisting all spells, this is not the case. This skill helps you lessen the severity of spells that lower your stats or ones that last for a specific duration of time. It does not lessen damage from spells such as Energy Bolt or Flamestrike. The Magery spells that can be resisted are Clumsy, Curse, Feeblemind, Mana Drain, Mana Vampire, Paralyze, Paralyze Field, Poison, Poison Field, and Weaken. The Necromancy spells that can be resisted are Blood Oath, Corpse Skin, Mind Rot, and Pain Spike. At higher ranks, the Resisting Spells skill also benefits you by adding a bonus to your minimum elemental resists. This bonus is only applied after all other resist modifications - such as from equipment - have been calculated. It's also not cumulative. It compares the number of your minimum resists to the calculated value of your modifications and uses the higher of the two values. As you can see, Resisting Spells is a difficult skill to understand, and even more difficult to master. This is because in order to improve it, you will have to put yourself in harm's way - as in the path of one of the above spells. Undead have plagued the town of Old Haven. We need your assistance in cleansing the town of this evil influence. Old Haven is located east of here. Battle the undead spell casters that inhabit there. Come back to me once you feel that you are worthy of the rank of Apprentice Mage and I will reward you with an arcane prize.",
			"uncomplete": "You have not achieved the rank of Apprentice Mage. Come back to me once you feel that you are worthy of the rank of Apprentice Mage and I will reward you with an arcane prize.",
			"complete": "You have successfully begun your journey in becoming a true master of Magery. On behalf of the New Haven Mage Council, I wish to present you with this bracelet. When worn, the Bracelet of Resilience will enhance your resistances vs. the elements, physical, and poison harm. The Bracelet of Resilience also magically enhances your ability to fend off ranged and melee attacks. I hope it serves you well.",
			"refuse": "The ability to resist powerful spells is a taxing experience. I understand your resistance in wanting to pursue it. If you wish to reconsider, feel free to return to me for Resisting Spells training. Good journey to you!",
			"oncomplete": "You have achieved the rank of Apprentice Mage. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your Resisting Spells potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"Resist the arcane, and master your defenses.",
				"A mage should learn how to resist spells."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 204,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "braceletofresilience", "amount": 1, "name": "Bracelet of Resilience" }
			]
		},
		"19": {
			"title": "Stopping the World",
			"description": "Head East out of town and go to Old Haven. Use spells and abilities to deplete your mana and meditate there until you have raised your Meditation skill to 50. Well met! I can teach you how to 'Stop the World' around you and focus your inner energies on replenishing your mana. What is mana? Mana is the life force for everyone who practices arcane arts. When a practitioner of magic invokes a spell or scribes a scroll, it consumes mana. Having an abundant supply of mana is vital to excelling as a practitioner of the arcane. Those of us who study the art of Meditation are also known as stoics. The Meditation skill allows stoics to increase the rate at which they regenerate mana. A Stoic needs to perform abilities or cast spells to deplete mana before meditating to replenish it. Meditation can occur passively or actively. Active Meditation is more difficult to master but allows for the stoic to replenish mana at a significantly faster rate. Metal armor interferes with the regenerative properties of Meditation. It is wise to wear leather or cloth protection when meditating. Head east out of town and go to Old Haven. Use spells and abilities to deplete your mana and actively meditate to replenish it. Come back once you feel you are at the worthy rank of Apprentice Stoic and I will reward you with an arcane prize.",
			"uncomplete": "You have not achieved the rank of Apprentice Stoic. Come back to me once you feel that you are worthy of the rank Apprentice Stoic and I will reward you with an arcane prize.",
			"complete": "Splendid! On behalf of the New Haven Mage Council, I wish to present you with this hat. When worn, the Philosopher's Hat will protect you somewhat from physical attacks. The Philosopher's Hat also enhances the potency of your offensive spells, lowers the mana cost of your arcane spells and abilities, and passively increases your mana regeneration rate. Ah yes, almost forgot. The Philosopher's Hat also grants one other special ability to its wearer. It allows a chance for the wearer to cast spells without using any reagents. I hope the Philosopher's Hat serves you well.",
			"refuse": "Seek me out if you ever wish to study the art of Meditation. Good journey.",
			"oncomplete": "You have achieved the rank of Apprentice Stoic. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your Meditation potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"Quiet the mind and the world will follow.",
				"Meditation allows a mage to replenish mana quickly. I can teach you."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 205,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "philosophershat", "amount": 1, "name": "Philosopher's Hat" }
			]
		},
		"20": {
			"title": "Scribing Arcane Knowledge",
			"description": "While here at the New Haven Magery Library, use a scribe's pen and scribe 3rd and 4th circle Magery scrolls that you have in your spellbook. Remember, you will need blank scrolls as well. Do this until you have raised your Inscription skill to 50. Greetings and welcome to the New Haven Magery Library! You wish to learn how to scribe spell scrolls? You have come to the right place! Inscribed with a steady hand and imbued with the power of reagents, a scroll can mean the difference between life and death in a perilous situation. Those knowledgeable in Inscription can transcribe spells to create useful and valuable magical scrolls. Before you can inscribe a spell, you must first be able to cast the spell without the aid of a scroll. This means that you need the appropriate level of proficiency as a mage, the required mana, and the required reagents. Second, you will need a blank scroll to write on and a scribe's pen. Then, you will need to decide which particular spell you wish to scribe. As with the development of all skills, you need to practice Inscription of lower-level spells before you can move onto the more difficult ones. Inscribing a scroll with a magic spell drains your mana. I suggest you begin scribing any 3rd and 4th circle spells that you know. If you don't possess any, you can always barter with one of the local mage merchants or a fellow adventurer that is a seasoned Scribe. Come back to me once you feel that you are worthy of the rank of Apprentice Scribe and I will reward you with an arcane prize.",
			"uncomplete": "You have not achieved the rank of Apprentice Scribe. Come back to me once you feel that you are worthy of the rank of Apprentice Scribe and I will reward you with an arcane prize.",
			"complete": "Scribing is a very fulfilling pursuit. I am pleased to see you embark on this journey. You sling a pen well! On behalf of the New Haven Mage Council, I wish to present you with this spellbook. When equipped, the Hallowed Spellbook greatly enhances the potency of your offensive spells when used against Undead. Be mindful, though. While this book is equipped, your spells and abilities used against Humanoids, such as other humans, orcs, ettins, and trolls, will diminish in effectiveness. I suggest unequipping the Hallowed Spellbook when battling Humanoids. I hope this spellbook serves you well.",
			"refuse": "I understand. When you are ready, feel free to return to me for Inscription training. Thanks for stopping by!",
			"oncomplete": "You have achieved the rank of Apprentice Scribe. Return to your instructor in New Haven Magery Library to claim your reward.",
			"trainingarea": "Your Inscription potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"A steady hand and a focused mind create wonders.",
				"I can teach you how to scribe magic scrolls."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 206,
			"regionName": "Haven Library",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "hallowedspellbook", "amount": 1, "name": "Hallowed Spellbook" }
			]
		},
		"21": {
			"title": "The Mage's Apprentice",
			"description": "Head East out of town and go to Old Haven. Cast Fireball and Lightning Bolt against monsters there until you have raised your Magery skill to 50. Greetings. You seek to unlock the secrets of the arcane art of Magery. The New Haven Mage Council has an assignment for you. Undead have plagued the town of Old Haven. We need your assistance in cleansing the town of this evil influence. Old Haven is located east of here. I suggest using your offensive Magery spells such as Fireball and Lightning Bolt against the Undead that inhabit there. Make sure you have plenty of reagents before embarking on your journey. Reagents are required to cast Magery spells. You can purchase extra reagents at the nearby Reagent shop, or you can find reagents growing in the nearby wooded areas. You can see which reagents are required for each spell by looking in your spellbook. Come back to me once you feel that you are worthy of the rank of Apprentice Mage and I will reward you with an arcane prize.",
			"uncomplete": "You have not achieved the rank of Apprentice Mage. Come back to me once you feel that you are worthy of the rank of Apprentice Mage and I will reward you with an arcane prize.",
			"complete": "Well done! On behalf of the New Haven Mage Council, I wish to present you with this staff. Normally a mage must unequip weapons before spell casting. While wielding your new Ember Staff, however, you will be able to invoke your Magery spells. Even if you do not currently possess skill in Mace Fighting, the Ember Staff will allow you to fight as if you do. However, your Magery skill will be temporarily reduced while doing so. Finally, the Ember Staff occasionally smites a foe with a Fireball while wielding it in melee combat. I hope the Ember Staff serves you well.",
			"refuse": "Very well, come back to me when you are ready to practice Magery. You have so much arcane potential. 'Tis a shame to see it go to waste. The New Haven Mage Council could really use your help.",
			"oncomplete": "You have achieved the rank of Apprentice Mage. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your Magery potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"Harness the arcane, and wield it with precision.",
				"Want to unlock the secrets of magery ?"
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 207,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "emberstaff", "amount": 1, "name": "Ember Staff" }
			]
		},
		"22": {
			"title": "A Scholarly Task",
			"description": "Head East out of town and go to Old Haven. Use Evaluating Intelligence on all creatures you see there. You can also cast Magery spells as well to raise Evaluating Intelligence. Do these activities until you have raised your Evaluating Intelligence skill to 50. Hello. Truly knowing your opponent is essential for landing your offensive spells with precision. I can teach you how to enhance the effectiveness of your offensive spells, but first you must learn how to size up your opponents intellectually. I have a scholarly task for you. Head East out of town and go to Old Haven. Use Evaluating Intelligence on all creatures you see there. You can also cast Magery spells as well to raise Evaluating Intelligence. Come back to me once you feel that you are worthy of the rank Apprentice Scholar, and I will reward you with an arcane prize.",
			"uncomplete": "You have not achieved the rank of Apprentice Scholar. Come back to me once you feel that you are worthy of the rank Apprentice Scholar, and I will reward you with an arcane prize.",
			"complete": "You have completed the task. Well done. On behalf of the New Haven Mage Council, I wish to present you with this ring. When worn, the Ring of the Savant enhances your intellectual aptitude and increases your mana pool. Your spell casting abilities will take less time to invoke, and recovering from such spell casting will be hastened. I hope the Ring of the Savant serves you well.",
			"refuse": "Return to me if you reconsider and wish to become an Apprentice Scholar.",
			"oncomplete": "You have achieved the rank of Apprentice Scholar. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your Evaluating Intelligence potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"Understanding your opponent is the key to victory.",
				"Want to maximize your spell damage ? I have a scholarly task for you!"
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 208,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "ringofthesavant", "amount": 1, "name": "Ring of the Savant" }
			]
		},
		"23": {
			"title": "The Art of War",
			"description": "Head East out of town to Old Haven. Battle monsters there until you have raised your Tactics skill to 50. Knowing how to hold a weapon is only half of the battle. The other half is knowing how to use it against an opponent. It's one thing to kill a few bunnies now and then for fun, but a true warrior knows that the right moves to use against a lich will pretty much get your arse fried by a dragon. I'll help teach you how to fight so that when you do come up against that dragon, maybe you won't have to walk out of there 'OooOOooOOOooOO'ing' and looking for a healer. There are some undead that need cleaning out in Old Haven towards the east. Why don't you head on over there and practice killing things? When you feel like you've got the basics down, come back to me and I'll see if I can scrounge up an item to help you in your adventures later on.",
			"uncomplete": "You're making some progress, that I can tell, but you're not quite good enough to last for very long out there by yourself. Head back to Old Haven, to the east, and kill some more undead.",
			"complete": "Hey, good job killing those undead! Hopefully someone will come along and clean up the mess. All that blood and guts tends to stink after a few days, and when the wind blows in from the east, it can raise a mighty stink! Since you performed valiantly, please take these arms and use them well. I've seen a few too many harvests to be running around out there myself, so you might as well take it. There is a lot left for you to learn, but I think you'll do fine. Remember to keep your elbows in and stick 'em where it hurts the most!",
			"refuse": "That's too bad. I really thought you had it in you. Well, I'm sure those undead will still be there later, so if you change your mind, feel free to stop on by and I'll help you the best I can.",
			"oncomplete": "You have achieved the rank of Apprentice Warrior. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel like practicing combat here would really help you learn to fight better. Your ability to raise your Tactics skill is enhanced in this area.",
			"npcPhrases": [
				"Master the art of battle, and the field shall be yours.",
				"There is an art to slaying your enemies swiftly. It's called tactics, and I can teach it to you."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 209,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "armsofarmstrong", "amount": 1, "name": "Arms of Armstrong" }
			]
		},
		"24": {
			"title": "Crushing Bones and Taking Names",
			"description": "Head East out of town and go to Old Haven. While wielding your mace, battle monsters there until you have raised your Mace Fighting skill to 50. I see you want to learn a real weapon skill and not that toothpick training Jockles has to offer. Real warriors are called Armsmen, and they wield mace weapons. No doubt about it. Nothing is more satisfying than knocking the wind out of your enemies, smashing their armor, crushing their bones, and taking their names. Want to learn how to wield a mace? Well, I have an assignment for you. Head East out of town and go to Old Haven. Undead have plagued the town, so there are plenty of bones for you to smash there. Come back to me after you have achieved the rank of Apprentice Armsman, and I will reward you with a real weapon.",
			"uncomplete": "Listen, kid. There are a lot of undead in Old Haven, and you haven't smashed enough of them yet. So get back there and do some more cleansing.",
			"complete": "Now that's what I'm talking about! Well done! Don't you like crushing bones and taking names? As I promised, here is a war mace for you. It hits hard. It swings fast. It hits often. What more do you need? Now get out of here and crush some more enemies!",
			"refuse": "I thought you wanted to be an Armsman and really make something of yourself. You have potential, kid, but if you want to play with toothpicks, run to Jockles and he will teach you how to clean your teeth with a sword. If you change your mind, come back to me, and I will show you how to wield a real weapon.",
			"oncomplete": "You have achieved the rank of Apprentice Armsman. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel much more attuned to your mace. Your ability to hone your Mace Fighting skill is enhanced in this area.",
			"npcPhrases": [
				"Nothing beats the power of a good mace in battle.",
				"Don't listen to Jockles. Real warriors wield mace weapons!"
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 210,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "churchillswarmace", "amount": 1, "name": "Churchill's War Mace" }
			]
		},
		"25": {
			"title": "The Way of the Blade",
			"description": "Head East out of town and go to Old Haven. While wielding your sword, battle monsters there until you have raised your Swordsmanship skill to 50. As you approach, you notice Jockles sizing you up with a skeptical look on his face. 'I can see you want to learn how to handle a blade. It's a lot harder than it looks, and you're going to have to put a lot of time and effort if you ever want to be half as good as I am. I'll tell you what, kid, I'll help you get started, but you're going to have to do all the work if you want to learn something. East of here, outside of town, is Old Haven. It's been overrun with the nastiest of undead you've seen, which makes it a perfect place for you to turn that sloppy grin on your face into actual skill at handling a sword. Make sure you have a sturdy Swordsmanship weapon in good repair before you leave. 'Tis no fun to travel all the way down there just to find out you forgot your blade! When you feel that you've cut down enough of those foul-smelling things to learn how to handle a blade without hurting yourself, come back to me. If I think you've improved enough, I'll give you something suited for a real warrior.'",
			"uncomplete": "Jockles looks you up and down. 'Come on! You've got to work harder than that to get better. Now get out of here, go kill some more of those undead to the east in Old Haven, and don't come back till you've got real skill.'",
			"complete": "Well, well, look at what we have here! You managed to do it after all. I have to say, I'm a little surprised that you came back in one piece, but since you did, I've got a little something for you. This is a fine blade that served me well in my younger days. Of course, I've got much better swords at my disposal now, so I'll let you go ahead and use it under one condition: take good care of it and treat it with the respect that a fine sword deserves. You're one of the quickest learners I've seen, but you still have a long way to go. Keep at it, and you'll get there someday. Happy hunting, kid.",
			"refuse": "Ha! I had a feeling you were a lily-livered pansy. You might have potential, but if you're scared by a few smelly undead, maybe it's better that you stay away from sharp objects. After all, you wouldn't want to hurt yourself swinging a sword. If you change your mind, I might give you another chance...maybe.",
			"oncomplete": "You have achieved the rank of Apprentice Swordsman. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel much more attuned to your blade. Your ability to hone your Swordsmanship skill is enhanced in this area.",
			"npcPhrases": [
				"A true warrior wields their blade with skill and respect.",
				"Talk to me to learn the way of the blade."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 211,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "jocklesquicksword", "amount": 1, "name": "Jockles' Quick Sword" }
			]
		},
		"26": {
			"title": "En Guarde!",
			"description": "Head East out of town and go to Old Haven. Battle monsters there until you have raised your Fencing skill to 50. Well hello there, lad. Fighting with elegance and precision is far more enriching than slugging an enemy with a club or butchering an enemy with a sword. Learn the art of Fencing if you want to master combat and look good doing it! The key to being a successful fencer is to be the complement and not the opposition to your opponent's strength. Watch for your opponent to become off balance. Then finish him off with finesse and flair. There are some undead that need cleansing out in Old Haven towards the East. Head over there and slay them, but remember, do it with style! Come back to me once you have achieved the rank of Apprentice Fencer, and I will reward you with a prize.",
			"uncomplete": "You're doing well so far, but you're not quite ready yet. Head back to Old Haven, to the East, and kill some more undead.",
			"complete": "Excellent! You are beginning to appreciate the art of Fencing. I told you fighting with elegance and precision is more enriching than fighting like an ogre. Since you have returned victorious, please take this war fork and use it well. The war fork is a finesse weapon, and this one is magical! I call it 'Recaro's Riposte.' With it, you will be able to parry and counterstrike with ease! Your enemies will bask in your greatness and glory! Good luck to you, lad, and keep practicing!",
			"refuse": "I understand, lad. Being a hero isn't for everyone. Run along, then. Come back to me if you change your mind.",
			"oncomplete": "You have achieved the rank of Apprentice Fencer. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel more dexterous and quick-witted while practicing combat here. Your ability to raise your Fencing skill is enhanced in this area.",
			"npcPhrases": [
				"Precision and style turn the tide of battle.",
				"The art of fencing requires a dexterous hand, a quick wit and fleet feet."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 212,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "recarosriposte", "amount": 1, "name": "Recaro's Riposte" }
			]
		},
		"27": {
			"title": "Swift as an Arrow",
			"description": "Head East out of town and go to Old Haven. While wielding your bow or crossbow, battle monsters there until you have raised your Archery skill to 50. Well met, friend. Imagine yourself in a distant grove of trees. You raise your bow, take slow, careful aim, and with the twitch of a finger, you impale your prey with a deadly arrow. You look like you would make an excellent archer, but you will need practice. There is no better way to practice Archery than when your life is on the line. I have a challenge for you. Head East out of town and go to Old Haven. While wielding your bow or crossbow, battle the undead that reside there. Make sure you bring a healthy supply of arrows (or bolts if you prefer a crossbow). If you wish to purchase a bow, crossbow, arrows, or bolts, you can purchase them from me or the Archery shop in town. You can also make your own arrows with the Bowcraft/Fletching skill. You will need fletcher's tools, wood to turn into shafts, and feathers to make arrows or bolts. Come back to me after you have achieved the rank of Apprentice Archer, and I will reward you with a fine Archery weapon.",
			"uncomplete": "You're doing great as an Archer! However, you need more practice. Head East out of town and go to Old Haven. Come back to me after you have achieved the rank of Apprentice Archer.",
			"complete": "Congratulations! I want to reward you for your accomplishment. Take this composite bow. It is called 'Heartseeker.' With it, you will shoot with swiftness, precision, and power. I hope 'Heartseeker' serves you well.",
			"refuse": "I understand that Archery may not be for you. Feel free to visit me in the future if you change your mind.",
			"oncomplete": "You have achieved the rank of Apprentice Archer. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel more steady and dexterous here. Your Archery skill is enhanced in this area.",
			"npcPhrases": [
				"Master the bow, and let your arrows fly true.",
				"Archery requires a steady aim and dexterous fingers."
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 213,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "heartseeker", "amount": 1, "name": "Heartseeker" }
			]
		},
		"28": {
			"title": "Thou and Thine Shield",
			"description": "Head East out of town and go to Old Haven. Battle monsters, or simply let them hit you, while holding a shield or a weapon until you have raised your Parrying skill to 50. Oh, hello. You probably want me to teach you how to parry, don't you? Very well. First, you'll need a weapon or a shield. Obviously, shields work best of all, but you can parry with a 2-handed weapon. Or if you're feeling particularly brave, a 1-handed weapon will do in a pinch. I'd advise you to go to Old Haven, which you'll find to the East, and practice blocking incoming blows from the undead there. You'll learn quickly if you have more than one opponent attacking you at the same time to practice parrying lots of blows at once. That's the quickest way to master the art of parrying. If you manage to improve your skill enough, I have a shield that you might find useful. Come back to me when you've trained to an apprentice level.",
			"uncomplete": "You're doing well, but in my opinion, I don't think you really want to continue on without improving your parrying skill a bit more. Go to Old Haven, to the East, and practice blocking blows with a shield.",
			"complete": "Well done! You're much better at parrying blows than you were when we first met. You should be proud of your new ability, and I bet your body is grateful to you as well. *Tyl Ariadne laughs loudly at his own (mostly lame) joke* Oh yes, I did promise you a shield if I thought you were worthy of having it, so here you go. My father made these shields for the guards who served my father faithfully for many years, and I just happen to have one that I can part with. You should find it useful as you explore the lands. Good luck, and may the Virtues be your guide.",
			"refuse": "It's your choice, obviously, but I'd highly suggest that you learn to parry before adventuring out into the world. Come talk to me again when you get tired of being beaten on by your opponents.",
			"oncomplete": "You have achieved the rank of Apprentice Parrier. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "You feel as light as a butterfly, as if you could block incoming blows easily. Your ability to hone your Parrying skill is enhanced in this area.",
			"npcPhrases": [
				"A shield isn't just defense; it's your partner in battle.",
				"Want to learn how to parry blows ?"
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 214,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "escutcheondeariadne", "amount": 1, "name": "Escutcheon de Ariadne" }
			]
		},
		"29": {
			"title": "Cleansing Old Haven",
			"description": "Head East out of town to Old Haven. Consecrate your weapon, cast Divine Fury, and battle monsters there until you have raised your Chivalry skill to 50. Hail, friend. The life of a Paladin is a life of much sacrifice, humility, bravery, and righteousness. If you wish to pursue such a life, I have an assignment for you. Adventure east to Old Haven, consecrate your weapon, and lay to rest the undead that inhabit there. Each ability a Paladin wishes to invoke will require a certain amount of 'tithing points' to use. A Paladin can earn these tithing points by donating gold at a shrine or holy place. You may tithe at this shrine. Return to me once you feel that you are worthy of the rank of Apprentice Paladin.",
			"uncomplete": "There are still more undead to lay to rest. You still have more to learn. Return to me once you have done so.",
			"complete": "Well done, friend. While I know you understand Chivalry is its own reward, I would like to reward you with something that will protect you in battle. It was passed down to me when I was a lad. Now, I am passing it on to you. It is called the Bulwark Leggings. Thank you for your service.",
			"refuse": "Farewell to you, my friend. Return to me if you wish to live the life of a Paladin.",
			"oncomplete": "You have achieved the rank of Apprentice Paladin. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your Chivalry potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"The righteous path is not easy, but it is always worth walking.",
				"Hail, friend. Want to live the life of a paladin?"
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 215,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "bulwarkleggings", "amount": 1, "name": "Bulwark Leggings" }
			]
		},
		"30": {
			"title": "The Rudiments of Self Defense",
			"description": "Head East out of town and go to Old Haven. Battle monsters there until you have raised your Wrestling skill to 50. Listen up! If you want to learn the rudiments of self-defense, you need toughening up, and there's no better way to toughen up than engaging in combat. Head East out of town to Old Haven and battle the undead there in hand-to-hand combat. Afraid of dying, you say? Well, you should be! Being an adventurer isn't a bed of posies, or roses, or however that saying goes. If you take a dirt nap, go to one of the nearby wandering healers and they'll get you back on your feet. Come back to me once you feel that you are worthy of the rank Apprentice Wrestler and I will reward you with a prize.",
			"uncomplete": "You have not achieved the rank of Apprentice Wrestler. Come back to me once you feel that you are worthy of the rank Apprentice Wrestler and I will reward you with something useful.",
			"complete": "It's about time! Looks like you managed to make it through your self-defense training. As I promised, here's a little something for you. When worn, these Gloves of Safeguarding will increase your awareness and resistances to most elements except poison. Oh yeah, they also increase your natural health regeneration as well. Pretty handy gloves, indeed. Oh, if you are wondering if your meditation will be hindered while wearing these gloves, it won't be. Mages can wear cloth and leather items without needing to worry about that. Now get out of here and make something of yourself.",
			"refuse": "Ok, featherweight. Come back to me if you want to learn the rudiments of self-defense.",
			"oncomplete": "You have achieved the rank of Apprentice Wrestler. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your Wrestling potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"The best defense is a strong offense... and a lot of practice.",
				"You there! Wanna master hand to hand defense? Of course you do!"
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 216,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "glovesofsafeguarding", "amount": 1, "name": "Gloves of Safeguarding" }
			]
		},
		"31": {
			"title": "Channeling the Supernatural",
			"description": "Head East out of town and go to Old Haven. Use Spirit Speak and channel energy from either yourself or nearby corpses there. You can also cast Necromancy spells to raise Spirit Speak. How do you do? Channeling the supernatural through Spirit Speak allows you to heal your wounds. Such channeling expends your mana, so be mindful of this. Spirit Speak enhances the potency of your Necromancy spells. The channeling powers of a Medium are quite useful when practicing the dark magic of Necromancy. It is best to practice Spirit Speak where there are a lot of corpses. Head East out of town and go to Old Haven. Undead currently reside there. Use Spirit Speak and channel energy from either yourself or nearby corpses. You can also cast Necromancy spells to raise Spirit Speak. Come back to me once you feel that you are worthy of the rank of Apprentice Medium, and I will reward you with something useful.",
			"uncomplete": "Back so soon? You have not achieved the rank of Apprentice Medium. Come back to me once you feel that you are worthy of the rank of Apprentice Medium, and I will reward you with something useful.",
			"complete": "Well done! Channeling the supernatural is taxing, indeed. As promised, I will reward you with this bag of Necromancer reagents. You will need these if you wish to also pursue the dark magic of Necromancy. Good journey to you.",
			"refuse": "Channeling the supernatural isn't for everyone. It is a dark art. See me if you ever wish to pursue the life of a Medium.",
			"oncomplete": "You have achieved the rank of Apprentice Medium. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your ability to channel the supernatural is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"The line between life and death is thin; learn to walk it wisely.",
				"Want to learn how to channel the supernatural?"
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 217,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "bagofnecroreagents", "amount": 1, "name": "Bag of Necromancer Reagents" }
			]
		},
		"32": {
			"title": "The Allure of Dark Magic",
			"description": "Head East out of town and go to Old Haven. Cast Evil Omen and Pain Spike against monsters there until you have raised your Necromancy skill to 50. Welcome! I see you are allured by the dark magic of Necromancy. First, you must prove yourself worthy of such knowledge. Undead currently occupy the town of Old Haven. Practice your harmful Necromancy spells on them such as Evil Omen and Pain Spike. Make sure you have plenty of reagents before embarking on your journey. Reagents are required to cast Necromancy spells. You can purchase extra reagents from me, or you can find reagents growing in the nearby wooded areas. You can see which reagents are required for each spell by looking in your spellbook. Come back to me once you feel that you are worthy of the rank of Apprentice Necromancer and I will reward you with the knowledge you desire.",
			"uncomplete": "You have not achieved the rank of Apprentice Necromancer. Come back to me once you feel that you are worthy of the rank of Apprentice Necromancer and I will reward you with the knowledge you desire.",
			"complete": "You have done well, my young apprentice. Behold! I now present to you the knowledge you desire. This spellbook contains all the Necromancer spells. The power is intoxicating, isn't it?",
			"refuse": "You are weak after all. Come back to me when you are ready to practice Necromancy.",
			"oncomplete": "You have achieved the rank of Apprentice Necromancer. Return to your instructor in Old Haven to claim your reward.",
			"trainingarea": "Your Necromancy potential is greatly enhanced while questing in this area.",
			"npcPhrases": [
				"True power lies in the mastery of the dark arts.",
				"Allured by dark magic, aren't you?"
			],
			"type": "skillgain",
			"category": "Side Quests",
			"oneTimeQuest": 1,
			"questTurnIn": 1,
			"bankgold": 0,
			"targetRegion": 218,
			"regionName": "Old Haven Training",
			"targetSkill": 50,
			"maxSkillPoints": 500,
			"minPoint": 2,
			"maxPoint": 5,
			"rewards": [
				{ "type": "item", "itemID": "completenecromancerspellbook", "amount": 1, "name": "Complete Necromancer Spellbook" }
			]
		},
		"33": {
			title: "Split Ends",
			description: "*sighs* I think bowcrafting is a might beyond my talents. Say there, you look a bit more confident with tools. Can I persuade thee to make a few arrows? You could have my satchel in return... 'tis useless to me! You'll need a fletching kit to start, some feathers, and a few arrow shafts.Just use the fletching kit while you have the other things, and I'm sure you'll figure out the rest.",
			uncomplete: "You're not quite done yet.  Get back to work!",
			complete: "Thanks for helping me out.  Here's the reward I promised you.",
			refuse: "Oh. Well. I'll just keep trying alone, I suppose...",
			type: "collect",
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
			targetItems: [
				{ itemID: "arrow", amount: 20, name: "arrow" }
			],
			rewards: [
				{ type: "item", itemID: "fletcherssatchel", amount: 1, name: "Craftsman's Satchel" }
			]
		},
		"34": {
			title: "Split Ends",
			description: "*sighs* I think bowcrafting is a might beyond my talents. Say there, you look a bit more confident with tools. Can I persuade thee to make a few arrows? You could have my satchel in return... 'tis useless to me! You'll need a fletching kit to start, some feathers, and a few arrow shafts.Just use the fletching kit while you have the other things, and I'm sure you'll figure out the rest.",
			uncomplete: "You're not quite done yet.  Get back to work!",
			complete: "Thanks for helping me out.  Here's the reward I promised you.",
			refuse: "Oh. Well. I'll just keep trying alone, I suppose...",
			type: "collect",
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
			targetItems: [
				{ itemID: "arrow", amount: 20, name: "arrow" }
			],
			rewards: [
				{ type: "item", itemID: "fletcherssatchel", amount: 1, name: "Craftsman's Satchel" }
			]
		},
		"35": {
			title: "I Shot an Arrow Into the Air...",
			description: "Truth be told, the only way to get a feel for the bow is to shoot one and there's no better practice target than a sheep. If ye can shoot ten of them I think ye will have proven yer abilities. Just grab a bow and make sure to take enough ammunition. Bows tend to use arrows and crossbows use bolts.Ye can buy 'em or have someone craft 'em. How about it then? Come back here when ye are done.",
			uncomplete: "Return once ye have killed ten sheep with a bow and not a moment before.",
			complete: "Well done!  I prepared this bag for thee. Take it, and I wish thee the best of luck in thy endeavors.",
			refuse: "Fair enough, the bow isn't for everyone. Good day then.",
			type: "kill",
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
			targetKills: [
				{ npcID: "sheep", amount: 10, name: "sheep" }
			],
			rewards: [
				{ type: "item", itemID: "trinketbag", amount: 1, name: "A bag of trinkets." }
			]
		},
		"36": {
			title: "Comfortable Seating",
			description: "Hail friend, hast thou a moment? A mishap with a saw hath left me in a sorry state, for it shall be a while before I canst return to carpentry.In the meantime, I need a comfortable chair that I may rest. Could thou craft a straw chair?  Only a tool, such as a dovetail saw, a few boards, and some skill as a carpenter is needed. Remember, this is a piece of furniture, so please pay attention to detail.",
			uncomplete: "Is all going well? I look forward to the simple comforts in my very own home.",
			complete: "This is perfect!",
			refuse: "I quite understand your reluctance.  If you reconsider, I'll be here.",
			type: "collect",
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
			targetItems: [
				{ itemID: "BambooChair", amount: 1, name: "Straw Chair" }
			],
			rewards: [
				{ type: "item", itemID: "carpenterssatchel", amount: 1, name: "Craftsman's Satchel" }
			]
		},
		"37": {
			title: "The Pen is Mightier",
			description: "Do you know anything about 'Inscription?' I've been trying to get my hands on some hand crafted Recall scrolls for a while now, and I could really use some help. I don't have a scribe's pen, let alone a spellbook with Recall in it, or blank scrolls, so there's no way I can do it on my own. How about you though ? I could trade you one of my old leather bound books for some.",
			uncomplete: "Inscribing... yes, you'll need a scribe's pen, some reagents, some blank scroll, and of course your own magery book.You might want to visit the magery shop if you're lacking some materials.",
			complete: "Ha! Finally! I've had a rune to the waterfalls near Justice Isle that I've been wanting to use for the longest time, and now I can visit at last. Here's that book I promised you... glad to be rid of it, to be honest.",
			refuse: "Hmm, thought I had your interest there for a moment. It's not everyday you see a book made from real daemon skin, after all!",
			type: "collect",
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
			targetItems: [
				{ itemID: "recallscroll", amount: 5, name: "Recall Scroll" }
			],
			rewards: [
				{ type: "item", itemID: "redleatherbook", amount: 1, name: "a book bound in red leather" }
			]
		},
		"38": {
			title: "Flee and Fatigue",
			description: "I was just *coughs* ambushed near the moongate. *wheeze* Why do I pay my taxes? Where were the guards? You then, you an Alchemist? If you can make me a few Refresh potions, I will be back on my feet and can give those lizards the what for! Find a mortar and pestle, a good amount of black pearl, and ten empty bottles to store the finished potions in. Just use the mortar and pestle and the rest will surely come to you.When you return, the favor will be repaid.",
			uncomplete: "Just remember you need to use your mortar and pestle while you have empty bottles and some black pearl. Refresh potions are what I need.",
			complete: "*glug* *glug* Ahh... Yes! Yes! That feels great! Those lizardmen will never know what hit 'em! Here, take this, I can get more from the lizards.",
			refuse: "Fine fine, off with *cough* thee then! The next time you see a lizardman though, give him a whallop for me, eh?",
			type: "collect",
			category: "Side Quests", // New field for quest categorization
			oneTimeQuest: 1, // Indicates this quest can only be completed once
			questTurnIn: 1, // 0 = auto-complete
			bankgold: 0,
			targetItems: [
				{ itemID: "refreshpotion", amount: 10, name: "Refresh Potion" }
			],
			rewards: [
				{ type: "item", itemID: "alchemistssatchel", amount: 1, name: "a book bound in red leather" }
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
		},
		"1012": {
			title: "Daily Mining Challenge",
			description: "Mine 50 ore in the designated area.",
			type: "collect",
			category: "Daily Quests",
			questTurnIn: 1, // 0 = auto-complete
			dailyQuest: 1,
			resetDailyTime: 24, // Reset after 24 hours
			targetItems: [{ itemID: "ore", amount: 50, name: "Iron Ore"}], // Ore ID
			rewards: [
				{ type: "gold", amount: 2000, name: "Gold"},
				{ type: "item", itemID: "0x1F4C", amount: 1, name: "Recall Scroll" }
			]
		}
	};
	// Return the full quest list if no questID is provided
	if( !questID )
	{
		return questList;
	}

	// Return a specific quest if questID is provided
	return questList[questID] || null;
}