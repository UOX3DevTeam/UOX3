// UOX3 Speech File (JavaScript)
// Converted by Dark-Storm
// Date: 29.12.2001 2:07 am
// Tools used: PERL :)

function onSpeech( myString, myPlayer, myNPC )
{
	if(SubStringSearch( myString, "Hi" ) || SubStringSearch( myString, "Greetings" ) || SubStringSearch( myString, "Hello" ) || SubStringSearch( myString, "Hail" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Hello!" );
	}
	else if(SubStringSearch( myString, "buy" ) || SubStringSearch( myString, "sell" ) || SubStringSearch( myString, "British" ) || SubStringSearch( myString, "King" ) || SubStringSearch( myString, "Ruler" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Lord British is a leader who believes in order." );
	}
	else if(SubStringSearch( myString, "Blackthorn" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Blackthorn is Lord British's closest friend." );
	}
	else if(SubStringSearch( myString, "Britannia" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Britannia is the world in which thou art standing in." );
	}
	else if(SubStringSearch( myString, "Shrine" ) || SubStringSearch( myString, "Virtue" ) || SubStringSearch( myString, "Temple" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "There are many shrines in Britannia. Some say they have mystical powers!" );
	}
	else if(SubStringSearch( myString, "Ankh" ) || SubStringSearch( myString, "Healer" ) || SubStringSearch( myString, "Resurrect" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Ankh's represent eternal life.  Some healers say that they have special powers." );
	}
	else if(SubStringSearch( myString, "Bard" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Bards sing of adventures of great heroes! But I think they can be really annoying sometimes." );
	}
	else if(SubStringSearch( myString, "Boat" ) || SubStringSearch( myString, "Ship" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "A boat is great tool for thee to get around and visit the lands of Britannia with." );
	}
	else if(SubStringSearch( myString, "Cemetery" ) || SubStringSearch( myString, "Crypt" ) || SubStringSearch( myString, "Death" ) || SubStringSearch( myString, "Graves" ) || SubStringSearch( myString, "Graveyard" ) || SubStringSearch( myString, "Undead" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Some people just won't let the dead rest in peace." );
	}
	else if(SubStringSearch( myString, "Lost" ) || SubStringSearch( myString, "Map" ) || SubStringSearch( myString, "Advice" ) || SubStringSearch( myString, "Where" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Just remember, where ever thou dost go - There thou art." );
	}
	else if(SubStringSearch( myString, "Orc" ) || SubStringSearch( myString, "Lizardman" ) || SubStringSearch( myString, "Monster" ) || SubStringSearch( myString, "Skeleton" ) || SubStringSearch( myString, "Zombie" ) || SubStringSearch( myString, "Dungeon" ) || SubStringSearch( myString, "Graveyard" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Monsters are everywhere... Especially in dungeons and graveyards." );
	}
	else if(SubStringSearch( myString, "Stable" ) || SubStringSearch( myString, "Horse" ) || SubStringSearch( myString, "Ride" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "If thou hast some gold to spare, thou canst seek out the stables to buy thyself a horse." );
	}
	else if(SubStringSearch( myString, "Steal" ) || SubStringSearch( myString, "Thief" ) || SubStringSearch( myString, "Thiev" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "A thief is annoying, but perhaps they need things more then we do." );
	}
	else if(SubStringSearch( myString, "Want" ) || SubStringSearch( myString, "Well" ) || SubStringSearch( myString, "Need" ) || SubStringSearch( myString, "Wrong" ) || SubStringSearch( myString, "Require" ) || SubStringSearch( myString, "Fare" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "I am doing fine thank thee." );
	}
	else if(SubStringSearch( myString, "Armor" ) || SubStringSearch( myString, "Armour" ) || SubStringSearch( myString, "Shield" ) || SubStringSearch( myString, "Plate" ) || SubStringSearch( myString, "Chain" ) || SubStringSearch( myString, "Mail" ) || SubStringSearch( myString, "Leather" ) || SubStringSearch( myString, "Studded" ) || SubStringSearch( myString, "Helm" ) || SubStringSearch( myString, "Gloves" ) || SubStringSearch( myString, "Leggings" ) || SubStringSearch( myString, "Breast" ) || SubStringSearch( myString, "Gorget" ) || SubStringSearch( myString, "Bone" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Sounds like thou needst to find thyself an Armorer." );
	}
	else if(SubStringSearch( myString, "Weapon" ) || SubStringSearch( myString, "Scimitar" ) || SubStringSearch( myString, "Katana" ) || SubStringSearch( myString, "Long Sword" ) || SubStringSearch( myString, "Sword" ) || SubStringSearch( myString, "Viking" ) || SubStringSearch( myString, "Kryss" ) || SubStringSearch( myString, "Dagger" ) || SubStringSearch( myString, "Butchers Knife" ) || SubStringSearch( myString, "Skinning Knife" ) || SubStringSearch( myString, "Knife" ) || SubStringSearch( myString, "Cleaver" ) || SubStringSearch( myString, "Halberd" ) || SubStringSearch( myString, "Bardiche" ) || SubStringSearch( myString, "Hatchet" ) || SubStringSearch( myString, "War Axe" ) || SubStringSearch( myString, "Pick Axe" ) || SubStringSearch( myString, "Executioners Axe" ) || SubStringSearch( myString, "Double Axe" ) || SubStringSearch( myString, "Battle Axe" ) || SubStringSearch( myString, "Pick Axe" ) || SubStringSearch( myString, "War Axe" ) || SubStringSearch( myString, "Large Battle Axe" ) || SubStringSearch( myString, "Axe" ) || SubStringSearch( myString, "Spear" ) || SubStringSearch( myString, "Staff" ) || SubStringSearch( myString, "Mace" ) || SubStringSearch( myString, "Maul" ) || SubStringSearch( myString, "Club" ) || SubStringSearch( myString, "Hammer" ) || SubStringSearch( myString, "Pitchfork" ) || SubStringSearch( myString, "Pitch" ) || SubStringSearch( myString, "War Fork" ) || SubStringSearch( myString, "Fork" ) || SubStringSearch( myString, "Staff" ) || SubStringSearch( myString, "Gnarled" ) || SubStringSearch( myString, "Quarter" ) || SubStringSearch( myString, "Shepherds Crook" ) || SubStringSearch( myString, "Crook" ) || SubStringSearch( myString, "Shepherd" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Those things may be bought from a Weaponsmith." );
	}
	else if(SubStringSearch( myString, "Egg" ) || SubStringSearch( myString, "Flour" ) || SubStringSearch( myString, "Honey" ) || SubStringSearch( myString, "Loaf" ) || SubStringSearch( myString, "Loaves" ) || SubStringSearch( myString, "Milk" ) || SubStringSearch( myString, "Pastry" ) || SubStringSearch( myString, "Food" ) || SubStringSearch( myString, "Bread" ) || SubStringSearch( myString, "Cake" ) || SubStringSearch( myString, "Cookie" ) || SubStringSearch( myString, "Pizza" ) || SubStringSearch( myString, "Dough" ) || SubStringSearch( myString, "Pie" ) || SubStringSearch( myString, "Muffin" ) || SubStringSearch( myString, "Donut" ) || SubStringSearch( myString, "Bake" ) || SubStringSearch( myString, "Oven" ) || SubStringSearch( myString, "Cook" ) || SubStringSearch( myString, "Chicken" ) || SubStringSearch( myString, "Hungry" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Food can be bought from many places." );
	}
	else if(SubStringSearch( myString, "Liquor" ) || SubStringSearch( myString, "Beer" ) || SubStringSearch( myString, "Wine" ) || SubStringSearch( myString, "Ale" ) || SubStringSearch( myString, "Alcohol" ) || SubStringSearch( myString, "Spirit" ) || SubStringSearch( myString, "Game" ) || SubStringSearch( myString, "Sleep" ) || SubStringSearch( myString, "Darts" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Such things may be purchased at an Inn or a Tavern." );
	}
	else if(SubStringSearch( myString, "Reagents" ) || SubStringSearch( myString, "Regs" ) || SubStringSearch( myString, "Reags" ) || SubStringSearch( myString, "Ash" ) || SubStringSearch( myString, "Blood" ) || SubStringSearch( myString, "Garlic" ) || SubStringSearch( myString, "Moss" ) || SubStringSearch( myString, "Mandrake" ) || SubStringSearch( myString, "Root" ) || SubStringSearch( myString, "Black" ) || SubStringSearch( myString, "Pearl" ) || SubStringSearch( myString, "Nightshade" ) || SubStringSearch( myString, "Ginseng" ) || SubStringSearch( myString, "Ginsing" ) || SubStringSearch( myString, "Sulfurous" ) || SubStringSearch( myString, "Silk" ) || SubStringSearch( myString, "Sceptre" ) || SubStringSearch( myString, "Staff" ) || SubStringSearch( myString, "Scroll" ) || SubStringSearch( myString, "Spellbook" ) || SubStringSearch( myString, "Potion" ) || SubStringSearch( myString, "Mortar" ) || SubStringSearch( myString, "Pestle" ) || SubStringSearch( myString, "Magic" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "A Mage or an Alchemist may sell thee such mystical artifacts." );
	}
	else if(SubStringSearch( myString, "Wyrm Heart" ) || SubStringSearch( myString, "Serpent Scale" ) || SubStringSearch( myString, "Bone" ) || SubStringSearch( myString, "Pig Iron" ) || SubStringSearch( myString, "Fertile Dirt" ) || SubStringSearch( myString, "Eye of Newt" ) || SubStringSearch( myString, "Daemon Bone" ) || SubStringSearch( myString, "Blood Spawn" ) || SubStringSearch( myString, "Obsidian" ) || SubStringSearch( myString, "Dragon Blood" ) || SubStringSearch( myString, "Vial of Blood" ) || SubStringSearch( myString, "Bat Wing" ) || SubStringSearch( myString, "Executioner Cap" ) || SubStringSearch( myString, "Pumice" ) || SubStringSearch( myString, "Dead Wood" ) || SubStringSearch( myString, "Black Moor" ) || SubStringSearch( myString, "Volcanic Ash" ) || SubStringSearch( myString, "Brimstone" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "I have no knowledge about such things." );
	}
	else if(SubStringSearch( myString, "Cloth" ) || SubStringSearch( myString, "Shirt" ) || SubStringSearch( myString, "Kilt" ) || SubStringSearch( myString, "Cape" ) || SubStringSearch( myString, "Robe" ) || SubStringSearch( myString, "Sash" ) || SubStringSearch( myString, "Tunic" ) || SubStringSearch( myString, "Dress" ) || SubStringSearch( myString, "Cap" ) || SubStringSearch( myString, "Wear" ) || SubStringSearch( myString, "Loom" ) || SubStringSearch( myString, "Weave" ) || SubStringSearch( myString, "Cotton" ) || SubStringSearch( myString, "Wol" ) || SubStringSearch( myString, "Dye" ) || SubStringSearch( myString, "Yarn" ) || SubStringSearch( myString, "Skirt" ) || SubStringSearch( myString, "Apron" ) || SubStringSearch( myString, "Coat" ) || SubStringSearch( myString, "Garment" ) || SubStringSearch( myString, "Pant" ) || SubStringSearch( myString, "Sew" ) || SubStringSearch( myString, "Shawl" ) || SubStringSearch( myString, "Tailor" ) || SubStringSearch( myString, "Thread" ) || SubStringSearch( myString, "Vest" ) || SubStringSearch( myString, "Bandanna" ) || SubStringSearch( myString, "Bolt" ) || SubStringSearch( myString, "Jester cap" ) || SubStringSearch( myString, "Suit" ) || SubStringSearch( myString, "Scissors" ) || SubStringSearch( myString, "Tunic" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "For some fine clothing, I suggest thou seekest a tailor." );
	}
	else if(SubStringSearch( myString, "Architect" ) || SubStringSearch( myString, "Building" ) || SubStringSearch( myString, "Home" ) || SubStringSearch( myString, "Lot" ) || SubStringSearch( myString, "House" ) || SubStringSearch( myString, "Estate" ) || SubStringSearch( myString, "Tower" ) || SubStringSearch( myString, "Castle" ) || SubStringSearch( myString, "Keep" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "An architect might be able to grant thee a house deed." );
	}
	else if(SubStringSearch( myString, "Bye" ) || SubStringSearch( myString, "Goodbye" ) || SubStringSearch( myString, "Farewell" ) || SubStringSearch( myString, "Go Away" ) || SubStringSearch( myString, "Leave Me Alone" ) || SubStringSearch( myString, "Later" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Goodbye to thee!" );
	}
	else if(SubStringSearch( myString, "Stupid" ) || SubStringSearch( myString, "Dumb" ))
	{
		myNPC.TurnToward( myPlayer );
		myNPC.TextMessage( "Such words are not kind to mine heart." );
	}
}

// [EOF]

