// UOX3 Speech File (JavaScript)
// Converted by Dark-Storm
// Date: 29.12.2001 2:07 am
// Tools used: PERL :)
//
// Rewritten by Xuri, July 2004
//
// This script uses regular expressions in it's searches. this allows to for instance include "hi", "hi!", and "hi?" as positive search results,
// while leaving out words where "hi" is simply a part of the word, like "hiking" or "hinder" or "hill".
// The regular expression is placed within two forward slashes, for instance /hello/. To make sure that the script only searches for the whole
// word "hello" and leaves off any preceeding or trailing letters/numbers/symbols, enclose the trigger-word in \b \b. Example: /\bhello\b/
// By adding an additional "i" behind the last forward slash, you specify that the search is case-insensitive, example: /\bhello\b/i

function onSpeech( myString, myPlayer, myNPC )
{
	if( !myNPC.InRange( myPlayer, 2 ) )
		return;
	var Speech_Array = myString.split(" ");
	var i = 0, currObj = 0;
	for( i = 1; i <= Speech_Array.length; i++ ) 
	{
/*		if( Speech_Array[currObj].match( /pUser.name/ ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "A pleasure to meet you, "+pUser.name+"." );
			return;
		}*/
		if( Speech_Array[currObj].match( /\bName\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "My name is "+myNPC.name+". A pleasure, I'm sure." );
			return;
		}
		if( Speech_Array[currObj].match( /\bHail\b/i ) || Speech_Array[currObj].match( /\bHi\b/i ) || Speech_Array[currObj].match( /\bHello\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Hello there!" );
			return;
		}
		else if( Speech_Array[currObj].match( /\bBritish\b/i ) || Speech_Array[currObj].match( /\bKing\b/i ) || Speech_Array[currObj].match( /\bRuler\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Lord British is a leader who believes in order." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bBlackthorn\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Blackthorn is Lord British's closest friend." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bBritannia\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Britannia is the world in which thou art standing in." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bShrine\b/i ) || Speech_Array[currObj].match( /\bVirtue\b/i ) || Speech_Array[currObj].match( /\bTemple\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "There are many shrines in Britannia. Some say they have mystical powers!" );
			return;
		}
		else if( Speech_Array[currObj].match( /\bAnkh\b/i ) || Speech_Array[currObj].match( /\bHealer\b/i ) || Speech_Array[currObj].match( /\bResurrect\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Ankh's represent eternal life.  Some healers say that they have special powers." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bBard\b/i ) || Speech_Array[currObj].match( /\bGleeman\b/i ) || Speech_Array[currObj].match( /\bMusician\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Bards sing of adventures of great heroes! But I think they can be really annoying sometimes." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bBoat\b/i ) || Speech_Array[currObj].match( /\bShip\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "A boat is great tool for thee to get around and visit the lands of Britannia with. Get one at the shipwright's!" );
			return;
		}
		else if( Speech_Array[currObj].match( /\bCemetery\b/i ) || Speech_Array[currObj].match( /\bCrypt\b/i ) || Speech_Array[currObj].match( /\bGraves\b/i )  || Speech_Array[currObj].match( /\bGraveyard\b/i ) || Speech_Array[currObj].match( /\bUndead\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Some people just won't let the dead rest in peace." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bLost\b/i ) || Speech_Array[currObj].match( /\bMap\b/i ) || Speech_Array[currObj].match( /\bAdvice\b/i )  || Speech_Array[currObj].match( /\bWhere\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Just remember, where ever thou goest - There thou art!" );
			return;
		}
		else if( Speech_Array[currObj].match( /\bOrc\b/i ) || Speech_Array[currObj].match( /\bLizardman\b/i ) || Speech_Array[currObj].match( /\bMonster\b/i )  || Speech_Array[currObj].match( /\bSkeleton\b/i ) || Speech_Array[currObj].match( /\bZombie\b/i ) || Speech_Array[currObj].match( /\bDungeon\b/i ) || Speech_Array[currObj].match( /\bGraveyard\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Monsters are everywhere... Especially in dungeons and graveyards." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bStable\b/i ) || Speech_Array[currObj].match( /\bHorse\b/i ) || Speech_Array[currObj].match( /\bRide\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "If thou hast some gold to spare, thou canst seek out the stables to buy thyself a horse." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bSteal\b/i ) || Speech_Array[currObj].match( /\bThief\b/i ) || Speech_Array[currObj].match( /\bThiev/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "A thief is annoying, but perhaps they need things more then we do." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bArmor\b/i ) || Speech_Array[currObj].match( /\bArmour\b/i ) || Speech_Array[currObj].match( /\bShield\b/i ) || Speech_Array[currObj].match( /\bPlate\b/i ) || Speech_Array[currObj].match( /\bChain/i ) || Speech_Array[currObj].match( /\bLeather\b/i ) || Speech_Array[currObj].match( /\bStudded\b/i ) || Speech_Array[currObj].match( /\bHelm\b/i ) || Speech_Array[currObj].match( /\bGloves\b/i ) || Speech_Array[currObj].match( /\bLeggings\b/i ) || Speech_Array[currObj].match( /\bGorget\b/i ) || Speech_Array[currObj].match( /\bBone\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Sounds like thou needst to find thyself an Armorer." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bWeapon\b/i ) || Speech_Array[currObj].match( /\bScimitar\b/i ) || Speech_Array[currObj].match( /\bKatana\b/i ) || Speech_Array[currObj].match( /\bLongsword\b/i ) || Speech_Array[currObj].match( /\bSword/i ) || Speech_Array[currObj].match( /\bViking\b/i ) || Speech_Array[currObj].match( /\bKryss\b/i ) || Speech_Array[currObj].match( /\bDagger\b/i ) || Speech_Array[currObj].match( /\bKnife\b/i ) || Speech_Array[currObj].match( /\bCleaver\b/i ) || Speech_Array[currObj].match( /\bHalberd\b/i ) || Speech_Array[currObj].match( /\bBardiche\b/i ) || Speech_Array[currObj].match( /\bHatchet\b/i ) || Speech_Array[currObj].match( /\bAxe\b/i ) || Speech_Array[currObj].match( /\bSpear\b/i ) || Speech_Array[currObj].match( /\bStaff\b/i ) || Speech_Array[currObj].match( /\bPitchfork\b/i ) || Speech_Array[currObj].match( /\bFork\b/i ) || Speech_Array[currObj].match( /\bCrook\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Those things may be bought from a Weaponsmith." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bEgg\b/i ) || Speech_Array[currObj].match( /\bFlour\b/i ) || Speech_Array[currObj].match( /\bHoney\b/i ) || Speech_Array[currObj].match( /\bLoaf\b/i ) || Speech_Array[currObj].match( /\bLoaves/i ) || Speech_Array[currObj].match( /\bCake\b/i ) || Speech_Array[currObj].match( /\bCookie\b/i ) || Speech_Array[currObj].match( /\bPizza\b/i ) || Speech_Array[currObj].match( /\bDough\b/i ) || Speech_Array[currObj].match( /\bPie\b/i ) || Speech_Array[currObj].match( /\bMuffin\b/i ) || Speech_Array[currObj].match( /\bDonout\b/i ) || Speech_Array[currObj].match( /\bBake\b/i ) || Speech_Array[currObj].match( /\bOven\b/i ) || Speech_Array[currObj].match( /\bCook\b/i ) || Speech_Array[currObj].match( /\bChicken\b/i ) || Speech_Array[currObj].match( /\bHungry\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "You want food? Try the baker's. Or find a tavern and get a drink to go with thy food." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bLiquor\b/i ) || Speech_Array[currObj].match( /\bBeer\b/i ) || Speech_Array[currObj].match( /\bWine\b/i ) || Speech_Array[currObj].match( /\bAle\b/i ) || Speech_Array[currObj].match( /\bAlcohol/i ) || Speech_Array[currObj].match( /\bSpirit\b/i ) || Speech_Array[currObj].match( /\bBeverage\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Try a tavern - they have both drinks and food and games aplenty!" );
			return;
		}
		else if( Speech_Array[currObj].match( /\bReagents\b/i ) || Speech_Array[currObj].match( /\bRegs\b/i ) || Speech_Array[currObj].match( /\bReags\b/i ) || Speech_Array[currObj].match( /\bAsh\b/i ) || Speech_Array[currObj].match( /\bBlood/i ) || Speech_Array[currObj].match( /\bGarlic\b/i ) || Speech_Array[currObj].match( /\bMoss\b/i ) || Speech_Array[currObj].match( /\bMandrake\b/i ) || Speech_Array[currObj].match( /\bRoot\b/i ) || Speech_Array[currObj].match( /\bBlack\b/i ) || Speech_Array[currObj].match( /\bPearl\b/i ) || Speech_Array[currObj].match( /\bNightshade\b/i ) || Speech_Array[currObj].match( /\bGinseng\b/i ) || Speech_Array[currObj].match( /\bSulfurous\b/i ) || Speech_Array[currObj].match( /\bSilk\b/i ) || Speech_Array[currObj].match( /\bSceptre\b/i ) || Speech_Array[currObj].match( /\bSpellbook\b/i ) || Speech_Array[currObj].match( /\bPotionk\b/i ) || Speech_Array[currObj].match( /\bMortar\b/i ) || Speech_Array[currObj].match( /\bMagic\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "A Mage or an Alchemist may sell thee such mystical artifacts." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bCloth\b/i ) || Speech_Array[currObj].match( /\bShirt\b/i ) || Speech_Array[currObj].match( /\bClothing\b/i ) || Speech_Array[currObj].match( /\bKilt\b/i ) || Speech_Array[currObj].match( /\bCape/i ) || Speech_Array[currObj].match( /\bRobe\b/i ) || Speech_Array[currObj].match( /\bSash\b/i ) || Speech_Array[currObj].match( /\bTunic\b/i ) || Speech_Array[currObj].match( /\bDress\b/i ) || Speech_Array[currObj].match( /\bLoom\b/i ) || Speech_Array[currObj].match( /\bWeave\b/i ) || Speech_Array[currObj].match( /\bCotton\b/i ) || Speech_Array[currObj].match( /\bWool\b/i ) || Speech_Array[currObj].match( /\bDye\b/i ) || Speech_Array[currObj].match( /\bGarment\b/i ) || Speech_Array[currObj].match( /\bPants\b/i ) || Speech_Array[currObj].match( /\bSew\b/i ) || Speech_Array[currObj].match( /\bTailork\b/i ) || Speech_Array[currObj].match( /\bThread\b/i ) || Speech_Array[currObj].match( /\bScissors\b/i ) || Speech_Array[currObj].match( /\bTunic\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "For some fine clothing, I suggest thou seekest a tailor." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bArchitect\b/i ) || Speech_Array[currObj].match( /\bBuilding\b/i ) || Speech_Array[currObj].match( /\bHome\b/i ) || Speech_Array[currObj].match( /\bHouse\b/i ) || Speech_Array[currObj].match( /\bEstate/i ) || Speech_Array[currObj].match( /\bTower\b/i ) || Speech_Array[currObj].match( /\bCastle\b/i ) || Speech_Array[currObj].match( /\bKeep\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "An architect might be able to grant thee a house deed." );
			return;
		}
		else if( Speech_Array[currObj].match( /\bBye\b/i ) || Speech_Array[currObj].match( /\bGoodbye\b/i ) || Speech_Array[currObj].match( /\bFarewell\b/i ) || Speech_Array[currObj].match( /\bLater\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Goodbye to thee!" );
			return;
		}
		else if( Speech_Array[currObj].match( /\bStupid\b/i ) || Speech_Array[currObj].match( /\bDumb\b/i ) || Speech_Array[currObj].match( /\bIdiot\b/i ) || Speech_Array[currObj].match( /\bFool\b/i ))	
		{
			myNPC.TurnToward( myPlayer );
			myNPC.TextMessage( "Ack! It pains my heart to hear such words!" );
			return;
		}
		currObj++;
	}
}

// [EOF]

