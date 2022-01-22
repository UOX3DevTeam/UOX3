var syllableCount = ["bu", "du", "fu", "ju", "gu", "ulg", "gug", "gub", "gur", "oog", "gub", "log", "ru", "stu", "glu", "ug", "ud", "og", "log", "ro", "flu",
	"bo", "duf", "fun", "nog", "dun", "bog", "dug", "gh", "ghu", "gho", "nug", "ig", "igh", "ihg", "luh", "duh", "bug", "dug",
	"dru", "urd", "gurt", "grut", "grunt","snarf", "urgle", "igg", "glu", "glug", "foo", "bar", "baz", "ghat", "ab", "ad",
	"gugh", "guk", "ag", "alm", "thu", "log", "bilge", "augh", "gha", "gig", "goth", "zug", "pig", "auh", "gan", "azh", "bag",
	"hig", "oth", "dagh", "gulg", "ugh", "ba", "bid", "gug", "bug", "rug", "hat", "brui", "gagh", "buad", "buil", "buim", "bum",
	"hug", "hug", "buo", "ma", "buor", "ghed", "buu", "ca", "guk", "clog", "thurg", "car", "cro", "thu", "da", "cuk", "gil", "cur", "dak",
	"dar", "deak", "der", "dil", "dit", "at", "ag", "dor", "gar", "dre", "tk", "dri", "gka", "rim", "eag", "egg", "ha", "rod", "eg", "lat", "eichel",
	"ek", "ep", "ka", "it", "ut", "ewk", "ba", "dagh", "faugh", "foz", "fog", "fid", "fruk", "gag", "fub", "fud", "fur", "bog", "fup", "hagh", "gaa", "kt",
	"rekk", "lub", "lug", "tug", "gna", "urg", "l", "gno", "gnu", "gol", "gom", "kug", "ukk", "jak", "jek", "rukk", "jja", "akt", "nuk", "hok", "hrol",
	"olm", "natz", "i", "i", "o", "u", "ikk", "ign", "juk", "kh", "kgh", "ka", "hig", "ke", "ki", "klap", "klu", "knod", "kod", "knu", "thnu",
	"krug", "nug", "nar", "nag", "neg", "neh", "oag", "ob", "ogh", "oh", "om", "dud", "oo", "pa", "hrak", "qo", "quad", "quil", "ghig", "rur", "sag", "sah", "sg"];

function onSpeech( myString, myPlayer, myNPC )
{
	if ( !myNPC.InRange( myPlayer, 2 ) )
		return;

	var mySocket = myPlayer.socket;
	if ( mySocket == null )
		return;

	HandleCustomKeyWords( myNPC );
}
	
function onAttack( pAttacker, pDefender )
{
	var Random = RandomNumber( 1, 100 )
	if ( Random >= 20 && Random <= 40 ) // 20% - 40 chance to talk
	{
		Speech( pAttacker );
	}
}

function onDefense( pAttacker, pDefender )
{
	var Random = RandomNumber( 1, 99 )
	if ( Random >= 20 && Random <= 40 ) // 20% chance to talk
	{
		Speech( pDefender );
	}
}

// Handle custom speech keywords
function Speech( myNPC )
{
	var mySentence = ""; // Prepare a variable for our sentence
	var sentenceLength = RandomNumber( 2, 6 ); // Randomize amount of words in sentence
	for ( var i = 0; i < sentenceLength; i++ )
	{
		// Build up a sentence from the randomly selected syllables
		mySentence += GetRandomSyllable();

		// Add a space between each syllable, except after the last one
		if ( i < sentenceLength - 1 )
			mySentence += " ";
	}

	myNPC.SoundEffect( 432, true );
	return myNPC.TextMessage( mySentence, true, 34, 2 );
}

function GetRandomSyllable()
{
	return syllableCount[RandomNumber(  0, syllableCount.length  )];
}

