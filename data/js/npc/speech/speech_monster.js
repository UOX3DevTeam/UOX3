// This script handles speech and speech responses by humanoid monsters such as orcs, lizardmen and
// ratmen, as well as intelligent creatures such as wisps

// Orc Speech
const orcSyllables = [ "ab", "ad", "ag", "akt", "alm", "at", "augh", "auh", "azh", "ba", "bag", "bar",
	"baz", "bid", "bilge", "bo", "bog", "brui", "bu", "buad", "bug", "buil", "buim", "bum", "buo",
	"buor", "buu", "ca", "car", "clog", "cro", "cuk", "cur", "da", "dagh", "dak", "dar", "deak",
	"der", "dil", "dit", "dor", "dre", "dri", "dru", "du", "dud", "duf", "dug", "duh", "dun", "eag",
	"eg", "egg", "eichel", "ek", "ep", "ewk", "faugh", "fid", "flu", "fog", "foo", "foz", "fruk",
	"fu", "fub", "fud", "fun", "fup", "fur", "gaa", "gag", "gagh", "gan", "gar", "gh", "gha",
	"ghat", "ghed", "ghid", "ghig", "gho", "ghu", "gig", "gil", "gka", "glu", "glug", "gna", "gno",
	"gnu", "gol", "gom", "goth", "grunt", "grut", "gu", "gub", "gug", "gugh", "guk", "gulg", "gur",
	"gurt", "ha", "hagh", "hat", "hig", "hok", "hrak", "hrol", "hug", "i", "ig", "igg", "igh",
	"ign", "ihg", "ikk", "it", "jak", "jek", "jja", "ju", "juk", "ka", "ke", "kgh", "kh", "ki",
	"klap", "klu", "knod", "knu", "kod", "krug", "kt", "kug", "lat", "log", "lub", "lug", "luh",
	"ma", "nag", "nar", "natz", "neg", "neh", "nog", "nug", "nuk", "o", "oag", "ob", "og", "ogh",
	"oh", "olm", "om", "oo", "oog", "oth", "pa", "pig", "qo", "quad", "quil", "rekk", "rim", "ro",
	"rod", "ru", "rug", "rukk", "rur", "sag", "sah", "sg", "snarf", "stu", "thu", "thurg", "tk",
	"tug", "u", "ud", "ug", "ugh", "ukk", "ulg", "urd", "urg", "urgle", "ut", "zug" ];

// Lizardman Speech
const lizardmanSyllables = [ "als", "asa", "asi", "asth", "ath", "ce", "ci", "cy", "is", "isa", "iss",
	"isth", "ith", "its", "kth", "las", "les", "lil", "lis", "lish", "lith", "lyl", "lys", "lysh",
	"lyth", "miss", "myss", "ra", "sal", "sas", "sath", "ses", "shash", "shi", "shy", "sia", "sil",
	"sis", "sit", "sith", "sla", "ss", "ssi", "ssy", "sth", "stha", "sthi", "sthi", "sthy", "sthy",
	"syl", "sys", "syt", "syth", "tha", "tha", "thes", "thil", "this", "thth", "thyl", "thys",
	"tis", "tsi", "tsy", "tys", "ys", "ysa", "yss", "ysth", "yth", "yts" ];

// Ratman Speech
const ratmanSyllables = [ "ack", "ak", "at", "att", "ccka", "ccke", "ccki", "ccko", "ccku", "ccky",
	"ch", "cha", "che", "chi", "cho", "chu", "chy", "cka", "cke", "cki", "cko", "cku", "cky", "ech",
	"eck", "ek", "et", "ett", "ich", "ick", "ik", "it", "itt", "ka", "ke", "ki", "ko", "ku", "ky",
	"och", "ock", "ok", "ot", "ott", "rach", "rak", "rech", "rek", "rich", "rik", "roch", "rok",
	"rrap", "rrep", "rrip", "rrop", "rrup", "rryp", "ruch", "ruk", "rych", "ryk", "skrit", "tak",
	"tat", "tcha", "tche", "tchi", "tcho", "tchu", "tchy", "tek", "tet", "ti", "tik", "tit", "tok",
	"tot", "tta", "tte", "tti", "tto", "ttu", "tty", "tuk", "tut", "tyk", "tyt", "uch", "uck", "uk",
	"ut", "utt", "ych", "yck", "yk", "yt", "ytt" ];

// Wisp Speech
const wispSyllables = ["b", "d", "f", "g", "h", "j", "k", "l", "n", "p", "r", "s", "t", "v", "w",
	"x", "z"];

const wispVowels = [ "c", "i", "m", "y" ];

const keywords = [ "meat", "gold", "kill", "killing", "slay", "sword", "axe", "spell",
	"magic", "spells", "swords", "axes", "mace", "maces", "monster", "monsters", "food", "run",
	"escape", "away", "help", "dead", "die", "dying", "lose", "losing", "life", "lives", "death",
	"ghost", "ghosts", "british", "blackthorn", "guild", "guilds", "dragon", "dragons", "game",
	"games", "ultima", "silly", "stupid", "dumb", "idiot", "idiots", "cheesy", "cheezy", "crazy",
	"dork", "jerk", "fool", "foolish", "ugly", "insult", "scum" ];

const responses = [ "meat", "kill", "pound", "crush", "yum yum", "crunch", "destroy",
	"murder", "eat", "munch", "massacre", "food", "monster", "evil", "run", "die", "lose", "dumb",
	"idiot", "fool", "crazy", "dinner", "lunch", "breakfast", "fight", "battle", "doomed",
	"rip apart", "tear apart", "smash", "edible?", "shred", "disembowel", "ugly", "smelly",
	"stupid", "hideous", "smell", "tasty", "invader", "attack", "raid", "plunder", "pillage",
	"treasure", "loser", "lose", "scum" ];

const deathWords = [ "Revenge!", "NOOooo!", "I... I...", "Me no die!", "Me die!",
	"Must... not die...", "Oooh, me hurt...", "Me dying?" ];

const killWords = [ "Ha! Thou dead!", "Thou not attack me! ", "Die!", "Die! Die!", "There!",
	"Thou, die!", "Fight me not!", "Ha! Be ghost now!"];

const hitWordsNone = [ "Ha! Bad fighter!", "You miss!", "Bad aim!", "Enemy fight bad!",
	"Me fight better!", "Thou art doomed..." ];

const hitWordsLow = [ "Ouch!", "Me not hurt bad!", "Thou fight bad.", "Thy blows soft!",
	"You bad with weapon!" ];

const hitWordsHigh = [ "Ouch! Me hurt!", "No, kill me not!", "Me hurt!", "Away with thee!",
	"Oof! That hurt!", "Aaah! That hurt...", "Good blow!" ];

const wordEndings = [ "!", ".", "?" ];

const searchAmount = 4;
const searchInterval = 5000;
const searchRange = 8;
const searchTimer = 15000;

function onSpeech( myString, myPlayer, myNPC )
{
	// Don't react to NPCs
	if( myPlayer.npc )
		return;

	// Don't react to dead players
	if( myPlayer.dead )
		return;

	// Don't react to offline (?) players
	if( !myPlayer.online )
		return;

	// Don't react while in combat
	if( myNPC.atWar )
		return;

	// Only react to speech from characters within 2 tiles range
	if( !myNPC.InRange( myPlayer, 2 ))
		return;

	Speech( myNPC, myString );
}

function inRange( myNPC, objInRange )
{
	if( !ValidateObject( objInRange ) || objInRange.npc || !objInRange.online || objInRange.dead || objInRange.isGM || objInRange.isCounselor )
		return;

	// Get the current server clock, and if it exists, the time for when the last search was started
	// Compare the two, and the script will see if enough time has passed to initiate a new search
	// This also ensures that the script stays working even if the server saves in the middle of a
	// search, but crashes before the next save.
	var iTime = GetCurrentClock();
	var initSearchTime = parseInt( myNPC.GetTempTag( "initSearchTime" ));

	// If search has already been initiated, don't start a new search, unless an abnormal amount of time has passed
	if(( initSearchTime != null && initSearchTime != 0 ) && (((( iTime - initSearchTime ) / 1000 ) < searchTimer ) && !( initSearchTime > iTime )))
	{
		return;
	}

	if(((( iTime - initSearchTime ) / 1000 ) > searchTimer ) || initSearchTime > iTime )
	{
		myNPC.SetTempTag( "initSearchTime", iTime.toString() );
		myNPC.StartTimer( searchInterval, 1, true );
	}
}

function onTimer( srcChar, timerID )
{
	if( timerID < searchAmount )
	{
		//Search for nearby characters the specified amount of times
		AreaCharacterFunction( "searchForPlayers", srcChar, searchRange );
		srcChar.StartTimer( searchInterval, timerID + 1, true );
	}
}

// This function iterates through all characters within the specified radius in AreaCharacterFunction
// It then checks to make sure they are valid for msgs
function searchForPlayers( srcChar, trgChar, pSock )
{
	if( srcChar == trgChar )
		return false;

	if( !ValidateObject( trgChar ) || trgChar.npc || !trgChar.online || trgChar.dead || trgChar.isGM || trgChar.isCounselor )
		return false;

	Speech( srcChar, null );
}

// Triggers when NPC attacks
function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt )
{
	// Random chance to speak when killing someone
	if( ValidateObject( pDefender ) && pDefender.dead )
	{
		if( RandomNumber( 1, 100 ) <= 33 )  // 33% chance to talk
		{
			var speechString = killWords[RandomNumber( 0, killWords.length - 1 )];
			pAttacker.TextMessage( speechString, true, GetTextHue( pAttacker ), 2 );
			pAttacker.SoundEffect( GetTextSound( pAttacker ), true );
		}
	}
}

// Triggers when NPC takes damage
function onDamage( damaged, attacker, damageValue, damageType )
{
	// Random chance to speak when defending
	if( RandomNumber( 1, 100 ) <= 10 ) // 10% chance to talk when taking damage
	{
		var speechString = "";
		if( damageValue <= 1 )
		{
			// No damage
			speechString = hitWordsNone[RandomNumber( 0, hitWordsNone.length - 1 )];
		}
		else if( damageValue <= 5 )
		{
			// Low damage
			speechString = hitWordsLow[RandomNumber( 0, hitWordsLow.length - 1 )];
		}
		else
		{
			// High damage
			speechString = hitWordsHigh[RandomNumber( 0, hitWordsHigh.length - 1 )];
		}

		damaged.TextMessage( speechString, true, GetTextHue( damaged ), 2 );
		damaged.SoundEffect( GetTextSound( damaged ), true );
	}
	return true;
}

// Triggers when NPC dies
function onDeathBlow( pDead, pKiller )
{
	// Only do this for orcs, lizardmen and ratmen
	if( pDead.race.id != 19 && RandomNumber( 1, 100 ) <= 20 ) // 20% chance
	{
		var lastWords = deathWords[RandomNumber( 0, deathWords.length - 1 )];
		pDead.TextMessage( lastWords, true, GetTextHue( pDead ), 2 );
		pDead.SoundEffect( GetTextSound( pDead ), true );
	}
	return true;
}

// Handle the NPC Speech
function Speech( myNPC, myString )
{
	if( myString != null && RandomNumber( 1, 100 ) <= 10 ) // 10% chance to respond to keywords
	{
		// Detect keyword
		var keywordFound = false;
		var keywordEntry = "";
		var speechArray = myString.split( " " );
		for( let i = 0; i < keywords.length; i++ )
		{
			if( keywordFound )
				break;

			for( let j = 0; j < speechArray.length; j++ )
			{
				if( keywords[i].toLowerCase() == speechArray[j].toLowerCase() )
				{
					// Keyword detected!
					keywordFound = true;
					keywordEntry = keywords[i].toLowerCase();
					break;
				}
			}
		}

		if( keywordFound )
		{
			// Keyword detected! Let's respond to it
			let rndResponse = RandomNumber( 0, 5 );
			let responseString = "";
			let responseWord = "";
			switch( rndResponse )
			{
				case 0: // Me [word]?
					responseString = "Me " + keywordEntry + "?";
					break;
				case 1: // [word] thee!
					responseString = keywordEntry + " thee!";
					responseString = responseString[0].toUpperCase() + responseString.slice( 1 ); // Capitalize 1st letter
					break;
				case 2: // [word]?
					responseString = keywordEntry + "?";
					responseString = responseString[0].toUpperCase() + responseString.slice( 1 ); // Capitalize 1st letter
					break;
				case 3: // [word]! [word].
					responseWord = responses[RandomNumber( 0, responses.length - 1 )];
					responseWord = responseWord[0].toUpperCase() + responseWord.slice( 1 ); // Capitalize 1st letter
					responseString = keywordEntry + "! " + responseWord + ".";
					responseString = responseString[0].toUpperCase() + responseString.slice( 1 ); // Capitalize 1st letter
					break;
				case 4: // [word].
					responseString = keywordEntry + ".";
					responseString = responseString[0].toUpperCase() + responseString.slice( 1 ); // Capitalize 1st letter
					break;
				case 5: // [word]? [word].
					responseWord = responses[RandomNumber( 0, responses.length - 1 )];
					responseWord = responseWord[0].toUpperCase() + responseWord.slice( 1 ); // Capitalize 1st letter
					responseString = keywordEntry + "? " + responseWord + ".";
					responseString = responseString[0].toUpperCase() + responseString.slice( 1 ); // Capitalize 1st letter
					break;
				default:
					break;
			}

			myNPC.TextMessage( responseString, true, GetTextHue( myNPC ), 2 );
		}
	}

	if( RandomNumber( 1, 100 ) > 10 ) // 90% chance to do nothing, 10% to say some gibberish
		return;

	var mySentence = ""; // Prepare a variable for our sentence
	var sentenceLength = RandomNumber( 2, 6 ); // Randomize amount of words in sentence

	// Construct sentences for Orcs, Lizardmen and Ratmen
	if( myNPC.race.id == 22 || myNPC.race.id == 5 || myNPC.race.id == 23 )
	{
		for( let i = 0; i < sentenceLength; i++ )
		{
			// Build up a sentence from the randomly selected syllables
			mySentence += GetRandomSyllable( myNPC );

			if( i == sentenceLength - 1 && RandomNumber( 0, 5 ) == 0 )
			{
				mySentence += GetRandomWordEnding( myNPC );
			}

			// Add a space between each syllable, except after the last one
			if( i < sentenceLength - 1 )
				mySentence += " ";
		}
	}
	else if( myNPC.race.id == 19 )
	{
		// Construct words and sentences for Wisps
		for( let i = 0; i < sentenceLength; i++ )
		{
			var myWord = "";
			var wordLength = RandomNumber( 1, 5 ); // Random word length for Wisp-speech
			// Build up each word of the sentence from randomly selected syllables
			for( let j = 0; j < wordLength; j++ )
			{
				if( wordLength > 1 && j == wordLength - 1 && RandomNumber( 0, 5 ) == 0 )
				{
					// Insert word ending
					myWord += GetRandomWordEnding( myNPC );
				}
				else if( j == 1 && RandomNumber( 0, 2 ) < 2 )
				{
					// Insert vowel
					myWord += GetRandomVowel( myNPC );
				}
				else
				{
					// Insert syllable
					myWord += GetRandomSyllable( myNPC );
				}
			}

			mySentence += myWord;

			// Add a space between each syllable, except after the last one
			if( i < sentenceLength - 1 )
				mySentence += " ";
		}
	}

	myNPC.SoundEffect( GetTextSound( myNPC ), true );
	myNPC.TextMessage( mySentence, true, GetTextHue( myNPC ), 2 );
}

function GetRandomSyllable( myNPC )
{
	switch( myNPC.race.id )
	{
		case 5: // Lizardmen
			return lizardmanSyllables[RandomNumber( 0, lizardmanSyllables.length - 1 )];
		case 19: // Wisps
			return wispSyllables[RandomNumber( 0, wispSyllables.length - 1 )];
		case 22: // Orcs
			return orcSyllables[RandomNumber( 0, orcSyllables.length - 1 )];
		case 23: // Ratmen
			return ratmanSyllables[RandomNumber( 0, ratmanSyllables.length - 1 )];
		default:
			return "";
	}
}

function GetRandomVowel( myNPC )
{
	switch( myNPC.race.id )
	{
		case 19: // Wisps
			return wispVowels[RandomNumber( 0, wispVowels.length - 1 )];
		default:
			return "";
	}
}

function GetRandomWordEnding( myNPC )
{
	switch( myNPC.race.id )
	{
		case 5: // Lizardmen
		case 19: // Wisps
		case 22: // Orcs
		case 23: // Ratmen
			return wordEndings[RandomNumber( 0, wordEndings.length - 1 )];
		default:
			return "";
	}
}

function GetTextHue( myNPC )
{
	switch( myNPC.race.id )
	{
		case 5: // Lizardmen
			return 0x3A;
		case 19: // Wisps
			return 0x59;
		case 22: // Orcs
			return 0x22;
		case 23: // Ratmen
			return 0x95;
		default:
			return 0x00;
	}
}

function GetTextSound( myNPC )
{
	switch( myNPC.race.id )
	{
		case 5: // Lizardmen
			return 0x1A2;
		case 19: // Wisps
			return 0x1D2;
		case 22: // Orcs
			return 0x1B0;
		case 23: // Ratmen
			return 0x1B6;
		default:
			return 0x00;
	}
}

function _restorecontext_() {}
