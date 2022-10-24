// The 'tweak command lets GMs and Admins modify properties of targeted objects
// This script replaces the previously hard-coded command
//
// TODO: Open a "Tweak Town Properties" menu based on a character's town-property, or via a townstone's item properties
// TODO: Open a "Tweak Guild Properties" menu based on a character's guild-property, or via a guildstone's item properties

// Backgrounds
const gumpMainBackground = 5054;
// const gumpMainBackground = 40000;
const gumpMainBackgroundWidth = 260;
const gumpMainBackgroundHeight = 450;
const gumpSecondaryBackground = 5120;
const gumpSecondaryBackgroundWidth = 125;
// const gumpSecondaryBackgroundWidth = 140;

// Buttons
const gumpMainButtonOff = 5031;
// const gumpMainButtonOff = 40019;
const gumpMainButtonOn = 5037;
// const gumpMainButtonOn = 40029;
const gumpNextButtonOff = 4005;
const gumpNextButtonOn = 4006;
const gumpPrevButtonOff = 4014;
const gumpPrevButtonOn = 4015;

// Text
const propertyValueStart = "<CENTER>"
const propertyValueEnd = "</CENTER>"
const propertyLabelStart = "<BASEFONT color=#ffffff>";
const propertyLabelEnd = "</BASEFONT>";

// Settings
const enableTransparentGump = true;
var enableTooltips = false; // Too much data for regular UO client, but works with ClassicUO client
const tooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day

// If any properties are removed or added, make sure to update the value behind each entry so there
// are no duplicate or skipped values

// List of item properties to handle
const itemProp = {
	ammoFX:20,
	ammoFXHue:21,
	ammoFXRender:22,
	ammoHue:23,
	ammoID:24,
	amount:25,
	baseRange:26,
	baseWeight:27,
	buyvalue:28,
	carveSection:29,
	colour:30,
	container:31,
	corpse:32,
	damageHeat:33,
	damageCold:34,
	damageLight:35,
	damageLightning:36,
	damagePoison:37,
	damageRain:38,
	damageSnow:39,
	decayable:40,
	decaytime:41,
	def:42,
	desc:43,
	divinelock:44,
	dir:45,
	entryMadeFrom:46,
	event:47,
	health:48,
	hidamage:49,
	instanceID:50,
	isDispellable:51,
	isDoorOpen:52,
	isDyeable:53,
	isGuarded:54,
	isNewbie:55,
	isPileable:56,
	isWipeable:57,
	itemsinside:58,
	layer:59,
	lodamage:60,
	madeWith:61,
	maxhp:62,
	maxinterval:63,
	maxItems:64,
	maxRange:65,
	maxUses:66,
	mininterval:67,
	more:68,
	morex:69,
	morey:70,
	morez:71,
	movable:72,
	name2:73,
	origin:74,
	owner:75,
	poison:76,
	race:77,
	rank:78,
	resistHeat:79,
	resistCold:80,
	resistLight:81,
	resistLightning:82,
	resistPoison:83,
	resistRain:84,
	resistSnow:85,
	restock:86,
	scripttrigger:87,
	sectionalist:88,
	sectionID:89,
	sellvalue:90,
	spawnsection:91,
	speed:92,
	strength:93,
	tempTimer:94,
	type:95,
	usesLeft:96,
	visible:97,
	weight:98,
	weightMax:99,
	wipable:100,
	worldnumber:101,
	x:102,
	y:103,
	z:104
};

// List of character properties to handle
var charProp = {
	accountNum:200,
	aitype:201,
	allmove:202,
	attack:203,
	attacker:204,
	attackFirst:205,
	atWar:206,
	baseskills:207,
	brkPeaceChance:208,
	canAttack:209,
	canBroadcast:210,
	canRun:211,
	canSnoop:212,
	cell:213,
	colour:214,
	commandlevel:215,
	criminal:216,
	dead:217,
	deaths:218,
	dexterity:219,
	direction:220,
	emoteColour:221,
	fame:222,
	flag:223,
	fontType:224,
	foodList:225,
	frozen:226,
	fx1:227,
	fy1:228,
	fx2:229,
	fy2:230,
	fz:231,
	gender:232,
	guild:233,
	guildTitle:234,
	health:235,
	hidamage:236,
	houseicons:237,
	housesCoOwned:238,
	housesOwned:239,
	hunger:240,
	hungerWildChance:241,
	innocent:242,
	instanceID:243,
	intelligence:244,
	isAnimal:245,
	isCasting:246,
	isCounselor:247,
	isDispellable:248,
	isflying:249,
	isGM:250,
	isGMPageable:251,
	isHuman:252,
	isIncognito:253,
	isJailed:254,
	isMeditating:255,
	isonhorse:256,
	isPolymorphed:257,
	isShop:258,
	isUsingPotion:259,
	karma:260,
	lightlevel:261,
	lodamage:262,
	magicReflect:263,
	mana:264,
	maxhp:265,
	maxmana:266,
	maxstamina:267,
	mounted:268,
	multi:269,
	murdercount:270,
	murderer:271,
	neutral:272,
	nextAct:273,
	noNeedMana:274,
	noNeedReags:275,
	noSkillTitles:276,
	npc:277,
	npcFlag:278,
	oldWandertype:279,
	online:280,
	orgID:281,
	orgSkin:282,
	ownedItemsCount:283,
	owner:284,
	pack:285,
	party:286,
	partyLootable:287,
	petCount:288,
	poison:289,
	poisonStrength:290,
	race:291,
	raceGate:292,
	region:293,
	sayColour:294,
	scripttrigger:295,
	sectionID:296,
	singClickSer:297,
	skillLock:298,
	skills:299,
	skillsused:300,
	skillToPeace:301,
	skillToProv:302,
	skillToTame:303,
	spattack:304,
	spdelay:305,
	spellCast:306,
	split:307,
	splitchance:308,
	squelch:309,
	stabled:310,
	stamina:311,
	stealth:312,
	strength:313,
	tamed:314,
	tamedHungerRate:315,
	tamedThirstRate:316,
	target:317,
	tempdex:318,
	tempint:319,
	tempstr:320,
	thirst:321,
	thirstWildChance:322,
	title:323,
	town:324,
	townPriv:325,
	trainer:326,
	visible:327,
	vulnerable:328,
	wandertype:329,
	weight:330,
	willhunger:331,
	willthirst:332,
	worldnumber:333,
	x:334,
	y:335,
	z:336
}

// List of character skills to handle
var charSkills = {
	alchemy:400,
	anatomy:401,
	animallore:402,
	itemid:403,
	armslore:404,
	parrying:405,
	begging:406,
	blacksmithing:407,
	bowcraft:408,
	peacemaking:409,
	camping:410,
	carpentry:411,
	cartography:412,
	cooking:413,
	detectinghidden:414,
	enticement:415,
	evaluatingintel:416,
	healing:417,
	fishing:418,
	forensics:419,
	herding:420,
	hiding:421,
	provocation:422,
	inscription:423,
	lockpicking:424,
	magery:425,
	magicresistance:426,
	tactics:427,
	snooping:428,
	musicianship:429,
	poisoning:430,
	archery:431,
	spiritspeak:432,
	stealing:433,
	tailoring:434,
	taming:435,
	tasteid:436,
	tinkering:437,
	tracking:438,
	veterinary:439,
	swordsmanship:440,
	macefighting:441,
	fencing:442,
	wrestling:443,
	lumberjacking:444,
	mining:445,
	meditation:446,
	stealth:447,
	removetrap:448,
	necromancy:449,
	focus:450,
	chivalry:451,
	bushido:452,
	ninjitsu:453,
	spellweaving:454,
	imbuing:455,
	mysticism:456,
	throwing:457
}

// List of multi properties to handle
var multiProp = {
	bans:500,
	banX:501,
	banY:502,
	buildTimestamp:503,
	colour:504,
	deed:505,
	dir:506,
	friends:507,
	guests:508,
	instanceID:509,
	isPublic:510,
	lockdowns:511,
	maxBans:512,
	maxFriends:513,
	maxGuests:514,
	maxLockdowns:515,
	maxOwners:516,
	maxSecureContainers:517,
	maxTrashContainers:518,
	maxVendors:519,
	owner:520,
	owners:521,
	scripttrigger:522,
	secureContainers:523,
	tradeTimestamp:524,
	trashContainers:525,
	vendors:526,
	visible:527,
	worldnumber:528,
	x:529,
	y:530,
	z:531
}

var regionProp = {
	appearance:700,
	canCastAggressive:701,
	canGate:702,
	canMark:703,
	canPlaceHouse:704,
	canRecall:705,
	canTeleport:706,
	chanceBigOre:707,
	health:708,
	instanceID:709,
	isDungeon:710,
	isGuarded:711,
	isSafeZone:712,
	mayor:713,
	music:714,
	numGuards:715,
	numOrePrefs:716,
	owner:717,
	population:718,
	race:719,
	reserves:720,
	scriptTrigger:721,
	tax:722,
	taxes:723,
	taxResource:724,
	weather:725,
	worldNumber:726
}

var accountProp = {
	comment:800,
	character1:801,
	character2:802,
	character3:803,
	character4:804,
	character5:805,
	character6:806,
	character7:807,
	currentChar:808,
	flags:809,
	lastIP:810,

	// Flags
	isBanned:811,
	isCounselor:812,
	isGM:813,
	isOnline:814,
	isPublic:815,
	isSeer:816,
	isSlot1Blocked:817,
	isSlot2Blocked:818,
	isSlot3Blocked:819,
	isSlot4Blocked:820,
	isSlot5Blocked:821,
	isSlot6Blocked:822,
	isSlot7Blocked:823,
	isSuspended:824,
	timeban:825,
	firstLogin:826
}

/*var regionProp2 = {
  appearance : {value: 700, dictionary: 154},
  canCastAggressive: {value: 701, dictionary: 155},
  canGate : {value: 702, dictionary: 156}
};*/

// Remember to update the itemPropCount if adding/removing properties to itemProp!
const itemPropCount = 85;
const charPropCount = 137;
const charSkillCount = 58;
const multiPropCount = 32;
const regionPropCount = 27;
const accountPropCount = 26;

function CommandRegistration()
{
	RegisterCommand( "tweak", 2, true );
	RegisterCommand( "props", 2, true ); // alias
}

function command_TWEAK( pSocket, cmdString )
{
	if( GetServerSetting( "ServerLanguage" ) == 7 || GetServerSetting( "ServerLanguage" ) == 0 && pSocket.language == 24 )
	{
		// Disable tooltips in this menu for Czech language until we can figure out why it crashes client!
		enableTooltips = false;
	}

	// First check if a valid target serial has been set by code, for instance when calling
	// tweak command via the hard-coded wholist gump
	var target = CalcCharFromSer( pSocket.GetDWord( 7 ));
	if( ValidateObject( target ))
	{
		onCallback0( pSocket, target );
	}
	else
	{
		// If not, request a custom target
		pSocket.CustomTarget( 0, GetDictionaryEntry( 229, pSocket.language )); // Select item or character to tweak.
	}
}

// Alias of TWEAK
function command_PROPS( pSocket, cmdString )
{
	command_TWEAK( pSocket, cmdString );
}

// Callback function for targeting an object after using 'tweak command
function onCallback0( pSocket, myTarget )
{
	pSocket.currentChar.SetTag( "tweakRegion", null );
	pSocket.currentChar.SetTag( "tweakAccount", null );
	pSocket.tempObj2 = null;
	var socketLang = pSocket.language;

	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	if( parseInt( pSocket.GetByte( 11 )) == 255 && !ValidateObject( myTarget ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1993, socketLang )); // Aborting tweak request
		return;
	}

	if( !ValidateObject( myTarget ) && pSocket.GetWord( 1 ))
	{
		var targX = pSocket.GetWord( 11 );
		var targY = pSocket.GetWord( 13 );
		var targZ = pSocket.GetSByte( 16 );
		var multi = FindMulti( targX, targY, targZ, pSocket.currentChar.worldnumber, pSocket.currentChar.instanceID )
		if( multi && multi.IsMulti() )
		{
			pSocket.tempObj2 = multi;
			HandleMultiTarget( pSocket, multi );
		}
		else
		{
			var pRegion = pSocket.currentChar.region;
			pSocket.currentChar.SetTag( "tweakRegion", pRegion.id );
			HandleRegionTarget( pSocket, pRegion );
		}
		return;
	}

	if( !ValidateObject( myTarget ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1991, socketLang )); // Invalid object targeted!
		return;
	}

	if( myTarget.isItem )
	{
		pSocket.tempObj2 = myTarget;
		HandleItemTarget( pSocket, myTarget );
	}
	else if( myTarget.isChar )
	{
		pSocket.tempObj2 = myTarget;
		HandleCharTarget( pSocket, myTarget );
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1992, socketLang )); // Unknown object targeted!
	}
}

// Callback function for selection of new target for object properties
function onCallback1( pSocket, myTarget )
{
	var socketLang = pSocket.language;

	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	if( parseInt( pSocket.GetByte( 11 )) == 255 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1993, socketLang )); // Aborting tweak request
		return;
	}

	if( pSocket.GetWord( 1 ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1994, socketLang )); // Targeted map or static item! Only dynamic items or characters can be modified.
		return;
	}

	if( !ValidateObject( myTarget ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1991, socketLang )); // Invalid object targeted!
		return;
	}

	var pButton = pSocket.tempInt2;
	var targetObj = pSocket.tempObj2;

	switch( pButton )
	{
		case itemProp.owner:
		case charProp.owner:
			pSocket.SysMessage( GetDictionaryEntry( 1995, socketLang )); // New owner set for object.
			targetObj.owner = myTarget;
			break;
		case charProp.attacker:
			pSocket.SysMessage( GetDictionaryEntry( 1996, socketLang )); // New attacker set for character.
			targetObj.attacker = myTarget;
			break;
		case charProp.target:
			pSocket.SysMessage( GetDictionaryEntry( 1997, socketLang )); // New target set for character.
			targetObj.target = myTarget;
			break;
		default:
			pSocket.SysMessage( GetDictionaryEntry( 1998, socketLang ) + pButton ); // Unhandled button ID in Tweak menu callback1: #
			break;
	}
}

// Shared zeroeth-page setup independent of object-type
function RenderZeroethPage( pSocket, gumpObj, targetObj, tweakSkills, baseSkills )
{
	var socketLang = pSocket.language;
	if( targetObj == null )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1999, socketLang ) + pButton ); // Object not found!
		return;
	}

	var objName = "";
	if( "username" in targetObj )
	{
		objName = targetObj.username;
	}
	else
	{
		objName = targetObj.name;
	}
	var objType = "";
	var nameColor = "#FFFFFF";
	var skillTotal = 0;
	var objPropHeader = GetDictionaryEntry( 2000, socketLang ); // Properties

	if( targetObj.isItem )
	{
		if( targetObj.IsMulti() )
		{
			if( targetObj.IsBoat() )
			{
				objType = GetDictionaryEntry( 2001, socketLang ); // Boat
			}
			else
			{
				objType = GetDictionaryEntry( 2002, socketLang ); // House
			}
		}
		else if( targetObj.isSpawner )
		{
			objType = GetDictionaryEntry( 2003, socketLang ); // Spawner
		}
		else
		{
			objType = GetDictionaryEntry( 2004, socketLang ); // Item
		}

		nameColor = "yellow";
	}
	else if( targetObj.isChar )
	{
		if( !tweakSkills )
		{
			objType = GetDictionaryEntry( 2005, socketLang ); // Char
		}
		else
		{
			objPropHeader = GetDictionaryEntry( 2006, socketLang ); // Skills
			objType = baseSkills ? GetDictionaryEntry( 2007, socketLang ) : GetDictionaryEntry( 2008, socketLang ); // Base vs Effective
			for( var k in charSkills )
			{
				if( baseSkills )
				{
					skillTotal += targetObj.baseskills[k];
				}
				else
				{
					skillTotal += targetObj.skills[k];
				}
			}
		}

		if( targetObj.isGM )
		{
			nameColor = "red";
		}
		else
		{
			if( targetObj.innocent || targetObj.npcFlag == 1 )
			{
				nameColor = "cyan";
			}
			else if( targetObj.criminal || targetObj.npcFlag == 0 )
			{
				nameColor = "grey";
			}
			else if( targetObj.murderer || targetObj.npcFlag == 2 )
			{
				nameColor = "red";
			}
		}
	}
	else if( "username" in targetObj )
	{
		objType = "Account"; // Account
	}
	else
	{
		objType = GetDictionaryEntry( 2009, socketLang ); // Region
	}

	// --------------- Page 0 -------------------
	gumpObj.AddPage( 0 );
	gumpObj.AddBackground( 0, 0, gumpMainBackgroundWidth, gumpMainBackgroundHeight, gumpMainBackground ); // Tile White Background
	gumpObj.AddBackground( 5, 22, 240, 30, 2620 ); // Tile White Background
	if( enableTransparentGump )
	{
		gumpObj.AddCheckerTrans( 0, 0, gumpMainBackgroundWidth, gumpMainBackgroundHeight );
	}

	if( tweakSkills )
	{
		gumpObj.AddButton( 230, 2, gumpPrevButtonOff, gumpPrevButtonOn, 1, 0, 1 ); 	// Exit
	}
	else
	{
		gumpObj.AddButton( 230, 2, 4017, 4018, 1, 0, 0 ); 	// Exit
	}
	if( enableTooltips )
	{
		gumpObj.AddToolTip( tooltipClilocID, pSocket, GetDictionaryEntry( 2010, socketLang )); // Close Tweak menu
	}
	gumpObj.AddHTMLGump( 10, 2, 230, 60, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>" + GetDictionaryEntry( 2011, socketLang ) + " " + objType + " " + objPropHeader + "</BASEFONT></BIG></CENTER>" );
	gumpObj.AddHTMLGump( 10, 27, 230, 20, 0, 0, "<BIG><BASEFONT color=" + nameColor + "><CENTER>" + objName + "</CENTER></BASEFONT></BIG>" );
	if( enableTooltips )
	{
		gumpObj.AddToolTip( tooltipClilocID, pSocket, objName );
	}
	if( tweakSkills )
	{
		gumpObj.AddHTMLGump( 0, 402, 235, 20, 0, 0, '<CENTER><BASEFONT color=#cdcdcd>' + GetDictionaryEntry( 2012, socketLang ) + ' ' + objType + ' ' + GetDictionaryEntry( 2013, socketLang ) + ': ' + (skillTotal/10).toFixed( 1 ).toString() + '</BASEFONT></CENTER>' );
	}

	return gumpObj;
}

// Shared first-page setup independent of object-type
function RenderFirstPage( pSocket, gumpObj, targetObj, objType, propCount, totalPages )
{
	var objName = "";
	if( "username" in targetObj )
	{
		objName = targetObj.username;
	}
	else
	{
		objName = targetObj.name;
	}

	// --------------- Page 1 -------------------
	gumpObj.AddPage( 1 );
	gumpObj.AddBackground( 112, 55, gumpSecondaryBackgroundWidth, 70, gumpSecondaryBackground ); // Tile White Background
	gumpObj.AddBackground( 112, 135, gumpSecondaryBackgroundWidth, 270, gumpSecondaryBackground ); // Tile White Background

	// Main Object Properties
	// Labels
	if( objType != "Region" && objType != "Account" )
	{
		gumpObj.AddHTMLGump( 15, 59, 100, 20, 0, 0, "<BASEFONT color=#ffffff>Serial</BASEFONT>" );
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, objType + " Serial - " + GetDictionaryEntry( 8119, pSocket.language )); // set automatically by UOX3
		}
		gumpObj.AddHTMLGump( 15, 79, 100, 20, 0, 0, "<BASEFONT color=#ffffff>ID</BASEFONT>" );
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, "ID of " + objType );
		}
		gumpObj.AddHTMLGump( 15, 99, 100, 20, 0, 0, "<BASEFONT color=#ffffff>Name</BASEFONT>" );
	}
	else if( objType == "Region" )
	{
		gumpObj.AddHTMLGump( 15, 59, 100, 20, 0, 0, "<BASEFONT color=#ffffff>Region ID</BASEFONT>" );
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, objType + " ID - " + GetDictionaryEntry( 8120, pSocket.language )); // Region number defined in regions.dfn
		}
		gumpObj.AddHTMLGump( 15, 79, 100, 20, 0, 0, "<BASEFONT color=#ffffff>Name</BASEFONT>" );
	}
	else
	{
		// Account
		gumpObj.AddHTMLGump( 15, 59, 100, 20, 0, 0, "<BASEFONT color=#ffffff>Account ID</BASEFONT>" );
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, objType + " ID - " + GetDictionaryEntry( 8121, pSocket.language )); // Account ID defined for this account in accounts.dfn
		}
		gumpObj.AddHTMLGump( 15, 79, 100, 20, 0, 0, "<BASEFONT color=#ffffff>Username</BASEFONT>" );
	}

	if( enableTooltips )
	{
		var tempMsg = GetDictionaryEntry( 8122, pSocket.language ); // Name of %s
		gumpObj.AddToolTip( tooltipClilocID, pSocket, tempMsg.replace(/%s/gi, objType ));
	}

	// Buttons
	if( objType != "Region" && objType != "Account" )
	{
		gumpObj.AddButton( 120, 80, gumpMainButtonOff, gumpMainButtonOn, 1, 0, 2); // ID
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, "0x" + (targetObj.id).toString( 16 ) + " ( " + (targetObj.id).toString() + " )" );
		}
		if( objType == "Item" || objType == "Multi" )
		{
			gumpObj.AddButton( 120, 100, gumpMainButtonOff, gumpMainButtonOn, 1, 0, 10); // Name
		}
		else if( objType == "Character" )
		{
			gumpObj.AddButton( 120, 100, gumpMainButtonOff, gumpMainButtonOn, 1, 0, 11); // Name
		}
	}
	else if( objType == "Region" )
	{
		gumpObj.AddButton( 120, 80, gumpMainButtonOff, gumpMainButtonOn, 1, 0, 12); // Name
	}
	//else // Account
	//{
		//gumpObj.AddButton( 120, 80, gumpMainButtonOff, gumpMainButtonOn, 1, 0, 13); // Username
	//}

	if( enableTooltips )
	{
		gumpObj.AddToolTip( tooltipClilocID, pSocket, objName );
	}

	if( objName.length > 16 )
	{
		objName = objName.substr( 0, 16 - 1 ) + '..';
	}

	// Values
	if( objType != "Region" && objType != "Account" )
	{
		gumpObj.AddHTMLGump( 125, 59, 105, 20, 0, 0, "<BASEFONT color=#EECD8B>" + propertyValueStart + (targetObj.serial).toString() + propertyValueEnd + "</BASEFONT>" );
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, "<BASEFONT color=#EECD8B>" + (targetObj.serial).toString() + "</BASEFONT> (Read-Only)" );
		}
		gumpObj.AddHTMLGump( 125, 79, 105, 20, 0, 0, propertyValueStart + "0x" + (targetObj.id).toString( 16 ) + " ( " + (targetObj.id).toString() + " )" + propertyValueEnd );
		gumpObj.AddHTMLGump( 125, 99, 105, 20, 0, 0, propertyValueStart + objName + propertyValueEnd );
	}
	else
	{
		gumpObj.AddHTMLGump( 125, 59, 105, 20, 0, 0, "<BASEFONT color=#EECD8B>" + propertyValueStart + (targetObj.id).toString() + propertyValueEnd + "</BASEFONT>" );
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, "<BASEFONT color=#EECD8B>" + (targetObj.id).toString() + "</BASEFONT> (Read-Only)" );
		}
		if( objType == "Account" )
		{
			gumpObj.AddHTMLGump( 125, 79, 105, 20, 0, 0, "<BASEFONT color=#EECD8B>" + propertyValueStart + objName + propertyValueEnd + "</BASEFONT>"  );
		}
		else
		{
			gumpObj.AddHTMLGump( 125, 79, 105, 20, 0, 0, propertyValueStart + objName + propertyValueEnd );
		}
	}

	// Initial next-page button
	if( propCount > 12 )
	{
		gumpObj.AddHTMLGump( 100, 420, 80, 20, 0, 0, "<BASEFONT color=#EECD8B>" + GetDictionaryEntry( 8819, pSocket.language ) + " 1/" + totalPages + "</BASEFONT>" );
		gumpObj.AddButton( 210, 420, gumpNextButtonOff, gumpNextButtonOn, 0, 2, 0 );
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, GetDictionaryEntry( 8123, pSocket.language )); // Next page
		}
	}

	return gumpObj;
}

// Shared other-page setup independent of object-type
function RenderOtherPages( pSocket, gumpObj, gumpPage, totalPages )
{
	gumpObj.AddPage( gumpPage );
	gumpObj.AddBackground( 112, 55, gumpSecondaryBackgroundWidth, 350, gumpSecondaryBackground ); // Tile White Background

	gumpObj.AddHTMLGump( 100, 420, 80, 20, 0, 0, "<BASEFONT color=#EECD8B>" + GetDictionaryEntry( 8819, pSocket.language ) + " " + gumpPage + "/" + totalPages + "</BASEFONT>" );

	if( gumpPage < totalPages )
	{
		// Add next page button for all subsequent pages except last one
		gumpObj.AddButton( 210, 420, gumpNextButtonOff, gumpNextButtonOn, 0, gumpPage + 1, 0 );
		if( enableTooltips )
		{
			gumpObj.AddToolTip( tooltipClilocID, pSocket, GetDictionaryEntry( 8123, pSocket.language )); // Next page
		}
	}

	// Add previous page button for all subsequent pages
	gumpObj.AddButton( 14, 420, gumpPrevButtonOff, gumpPrevButtonOn, 0, gumpPage - 1, 0 );
	if( enableTooltips )
	{
		gumpObj.AddToolTip( tooltipClilocID, pSocket, GetDictionaryEntry( 8124, pSocket.language )); // Previous page
	}

	return gumpObj;
}

// Handle properties of item targets
function HandleItemTarget( pSocket, myTarget )
{
	var itemGump = new Gump;
	itemGump = RenderZeroethPage( pSocket, itemGump, myTarget, false, false );

	var propertyName;
	var buttonID = 20;
	var gumpPage = 1;
	var totalPages = Math.ceil( 1 + ( itemPropCount - 13 ) / 17 );

	// First page
	var pageOneLabelStartY = 139;
	var pageOneButtonStartY = 140;
	var pageOneValueStartY = 139;
	// Subsequent pages
	var pageXlabelStartY = 59;
	var pageXbuttonStartY = 60;
	var pageXvalueStartY = 59;

	// Loop over all character properties
	var i = 0;
	for( i = 0; i < itemPropCount; i++ )
	{
		if( i == 0 )
		{
			// Page 1
			itemGump = RenderFirstPage( pSocket, itemGump, myTarget, "Item", itemPropCount, totalPages );
		}
		else // All other pages
		{
			// 17 options can fit on each page, so increase page number as needed
			switch( i )
			{
				case 13:
					gumpPage = 2;
					break;
				case 30:
					gumpPage = 3;
					break;
				case 47:
					gumpPage = 4;
					break;
				case 64:
					gumpPage = 5;
					break;
				case 81:
					gumpPage = 6;
					break;
			}

			// Only add these when it's time for a new page
			if( i == 13 || i == 30 || i == 47 || i == 64 || i == 81 )
			{
				pageXlabelStartY = 59;
				pageXbuttonStartY = 60;
				pageXvalueStartY = 59;
				itemGump = RenderOtherPages( pSocket, itemGump, gumpPage, totalPages );
			}
		}

		var labelStartY = 0;
		var buttonStartY = 0;
		var valueStartY = 0;
		if( gumpPage == 1 )
		{
			labelStartY = pageOneLabelStartY;
			buttonStartY = pageOneButtonStartY;
			valueStartY = pageOneValueStartY;
		}
		else
		{
			labelStartY = pageXlabelStartY;
			buttonStartY = pageXbuttonStartY;
			valueStartY = pageXvalueStartY;
		}

		var index = 20;
		for( var k in itemProp )
		{
		    if( itemProp.hasOwnProperty( k ) && index == i + 20 )
		    {
		        // k is key
				propertyName = k.charAt( 0 ).toUpperCase() + k.slice( 1 );
		        break;
		    }
		    index++;
		}

		var itemLabelTooltip = "";
		var itemValue = "";
		var itemValueTooltip = "";
		var errorFound = false;
		switch( i + 20 )
		{
			case itemProp.ammoFX:
				itemLabelTooltip 	= GetDictionaryEntry( 8125, pSocket.language ); // ID of moving effect played when ranged weapon fires projectile
				itemValue 			= "0x" + ( myTarget.ammoFX ).toString( 16 );
				break;
			case itemProp.ammoFXHue:
				itemLabelTooltip 	= GetDictionaryEntry( 8126, pSocket.language ); // Hue of moving effect played when ranged weapon fires projectile
				itemValue 			= "0x" + ( myTarget.ammoFXHue ).toString( 16 );
				break;
			case itemProp.ammoFXRender:
				itemLabelTooltip 	= GetDictionaryEntry( 8127, pSocket.language ); // Render mode of moving effect played when ranged weapon fires projectile
				itemValue 			= "0x" + ( myTarget.ammoFXRender ).toString( 16 );
				break;
			case itemProp.ammoHue:
				itemLabelTooltip 	= GetDictionaryEntry( 8128, pSocket.language ); // Hue of item used as ammo by ranged weapon
				itemValue 			= "0x" + ( myTarget.ammoHue ).toString( 16 );
				break;
			case itemProp.ammoID:
				itemLabelTooltip 	= GetDictionaryEntry( 8129, pSocket.language ); // ID of item used as ammo by ranged weapon
				itemValue 			= "0x" + ( myTarget.ammoID ).toString( 16 );
				break;
			case itemProp.amount:
				itemLabelTooltip 	= GetDictionaryEntry( 8130, pSocket.language ); // Amount of items in pile, or amount of items restocked on shopkeeper
				itemValue 			= ( myTarget.amount ).toString();
				break;
			case itemProp.baseRange:
				itemLabelTooltip 	= GetDictionaryEntry( 8207, pSocket.language ); // Base range of ranged weapon (SA and later only)
				itemValue 			= ( myTarget.baseRange ).toString();
				break;
			case itemProp.baseWeight:
				itemLabelTooltip 	= GetDictionaryEntry( 8131, pSocket.language ); // Base weight of item - primarily used for tracking the 'true' weight of containers, without adding weight of other items
				itemValue 			= ( myTarget.baseWeight ).toString();
				itemValueTooltip	= (parseFloat( myTarget.baseWeight / 100 ).toFixed( 2 )).toString() + " stones";
				break;
			case itemProp.buyvalue:
				itemLabelTooltip 	= GetDictionaryEntry( 8132, pSocket.language ); // Item's buy value - price player needs to pay to buy item from NPC shopkeeper
				itemValue 			= ( myTarget.buyvalue ).toString();
				break;
			case itemProp.carveSection:
				itemLabelTooltip 	= GetDictionaryEntry( 8133, pSocket.language ); // ID of section in carve DFNs that triggers if this item is carved - used for corpses
				itemValue 			= ( myTarget.carveSection ).toString();
				break;
			case itemProp.colour:
				itemLabelTooltip 	= GetDictionaryEntry( 8134, pSocket.language ); // Colour of item
				itemValue 			= "0x" + ( myTarget.colour ).toString( 16 );
				break;
			case itemProp.container:
				itemLabelTooltip 	= GetDictionaryEntry( 8135, pSocket.language ); // Item/Character the item is contained in/on
				itemValue 			= ( ValidateObject( myTarget.container ) ? "<BASEFONT color=#EECD8B>" + ( myTarget.container ).toString() + "</BASEFONT>" : "-" );
				itemValueTooltip 	= ( ValidateObject( myTarget.container ) ? ( myTarget.container.name ).toString() + " ( " + ( myTarget.container.serial ) + " )": "-" );
				break;
			case itemProp.corpse:
				itemLabelTooltip 	= GetDictionaryEntry( 8136, pSocket.language ); // Marks item as corpse
				itemValue 			= ( myTarget.corpse ? "true" : "false" );
				break;
			case itemProp.damageHeat:
				itemLabelTooltip 	= GetDictionaryEntry( 8137, pSocket.language ); // Weapon deals Heat/Fire elemental damage (true/false)
				itemValue 			= ( myTarget.damageHeat ).toString();
				break;
			case itemProp.damageCold:
				itemLabelTooltip 	= GetDictionaryEntry( 8138, pSocket.language ); // Weapon deals Cold elemental damage (true/false)
				itemValue 			= ( myTarget.damageCold ).toString();
				break;
			case itemProp.damageLight:
				itemLabelTooltip 	= GetDictionaryEntry( 8139, pSocket.language ); // Weapon deals Light elemental damage (true/false)
				itemValue 			= ( myTarget.damageLight ).toString();
				break;
			case itemProp.damageLightning:
				itemLabelTooltip 	= GetDictionaryEntry( 8140, pSocket.language ); // Weapon deals Lightning/Energy elemental damage (true/false)
				itemValue 			= ( myTarget.damageLightning ).toString();
				break;
			case itemProp.damagePoison:
				itemLabelTooltip 	= GetDictionaryEntry( 8141, pSocket.language ); // Weapon deals Poison elemental damage (true/false)
				itemValue 			= ( myTarget.damagePoison ).toString();
				break;
			case itemProp.damageRain:
				itemLabelTooltip 	= GetDictionaryEntry( 8142, pSocket.language ); // Weapon deals Rain elemental damage (true/false)
				itemValue 			= ( myTarget.damageRain ).toString();
				break;
			case itemProp.damageSnow:
				itemLabelTooltip 	= GetDictionaryEntry( 8143, pSocket.language ); // Weapon deals Snow elemental damage (true/false)
				itemValue 			= ( myTarget.damageSnow ).toString();
				break;
			case itemProp.decayable:
				itemLabelTooltip 	= GetDictionaryEntry( 8144, pSocket.language ); // Marks the item as decayable
				itemValue 			= ( myTarget.decayable ? "true" : "false" );
				break;
			case itemProp.decaytime:
				itemLabelTooltip 	= GetDictionaryEntry( 8145, pSocket.language ); // The amount of time left before the item will decay
				itemValue 			=  myTarget.decaytime > 0 ? Math.floor(( myTarget.decaytime - GetCurrentClock() ) / 1000 ).toString() : 0;
				break;
			case itemProp.def:
				itemLabelTooltip 	= GetDictionaryEntry( 8146, pSocket.language ); // Defensive value of item (Physical Resistance post-AoS, AR in older UO)
				itemValue 			= ( myTarget.def ).toString();
				break;
			case itemProp.desc:
				itemLabelTooltip 	= GetDictionaryEntry( 8147, pSocket.language ); // Description of item - used for items sold on player vendors
				itemValue 			= ( myTarget.desc ? ( myTarget.desc ).toString() : "-" );
				break;
			case itemProp.divinelock:
				itemLabelTooltip 	= GetDictionaryEntry( 8148, pSocket.language ); // Marks the item as locked by a GM
				itemValue 			= ( myTarget.divinelock ? "true" : "false" );
				break;
			case itemProp.dir:
				itemLabelTooltip 	= GetDictionaryEntry( 8149, pSocket.language ); // Direction of item - used to determine light type on light sources
				itemValue 			= ( myTarget.dir ).toString();
				break;
			case itemProp.entryMadeFrom:
				itemLabelTooltip 	= GetDictionaryEntry( 8150, pSocket.language ); // The ID of entry from Create DFN that item was crafted from (if any)";
				itemValue 			= ( myTarget.entryMadeFrom ).toString();
				break;
			case itemProp.event:
				itemLabelTooltip 	= GetDictionaryEntry( 8206, pSocket.language ); // Server event item is associated with
				itemValue 			= ( myTarget.event ? ( myTarget.event ).toString() : "-" );
				break;
			case itemProp.health:
				itemLabelTooltip 	= GetDictionaryEntry( 8151, pSocket.language ); // Item's current health/hitpoints (cannot exceed value of maxhp property)
				itemValue 			= ( myTarget.health ).toString();
				break;
			case itemProp.hidamage:
				itemLabelTooltip 	= GetDictionaryEntry( 8152, pSocket.language ); // Max damage item can deal in combat (randomized between lodamage and hidamage)
				itemValue 			= ( myTarget.hidamage ).toString();
				break;
			case itemProp.instanceID:
				itemLabelTooltip 	= GetDictionaryEntry( 8153, pSocket.language ); // ID of instance of world that item exists in. Objects in different instances will not be able to interact with one another!
				itemValue 			= ( myTarget.instanceID ).toString();
				break;
			case itemProp.isDispellable:
				itemLabelTooltip 	= GetDictionaryEntry( 8154, pSocket.language ); // Marks item as dispellable with Magic Dispel
				itemValue 			= ( myTarget.isDispellable ? "true" : "false" );
				break;
			case itemProp.isDoorOpen:
				itemLabelTooltip 	= GetDictionaryEntry( 8155, pSocket.language ); // Marks door as open
				itemValue 			= ( myTarget.isDoorOpen ? "true" : "false" );
				break;
			case itemProp.isDyeable:
				itemLabelTooltip 	= GetDictionaryEntry( 8156, pSocket.language ); // Marks item as dyeable
				itemValue 			= ( myTarget.isDyeable ? "true" : "false" );
				break;
			case itemProp.isGuarded:
				itemLabelTooltip 	= GetDictionaryEntry( 8157, pSocket.language ); // Marks item as guarded by a pet/hireling
				itemValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.isGuarded ? "true" : "false" ) + "</BASEFONT>";
				break;
			case itemProp.isNewbie:
				itemLabelTooltip 	= GetDictionaryEntry( 8158, pSocket.language ); // Marks item as newbiefied/blessed
				itemValue 			= ( myTarget.isNewbie ? "true" : "false" );
				break;
			case itemProp.isPileable:
				itemLabelTooltip 	= GetDictionaryEntry( 8159, pSocket.language ); // Marks item as pileable
				itemValue 			= ( myTarget.isPileable ? "true" : "false" );
				break;
			case itemProp.isWipeable:
				itemLabelTooltip 	= GetDictionaryEntry( 8160, pSocket.language ); // Marks item as wipeable with WIPE command
				itemValue 			= ( myTarget.isWipeable ? "true" : "false" );
				break;
			case itemProp.itemsinside:
				itemLabelTooltip 	= GetDictionaryEntry( 8161, pSocket.language ); // The amount of items contained inside container
				itemValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.itemsinside ).toString() + "</BASEFONT>";
				break;
			case itemProp.layer:
				itemLabelTooltip 	= GetDictionaryEntry( 8162, pSocket.language ); // Layer that characters will equip item on
				itemValue 			= "0x" + ( myTarget.layer ).toString( 16 );
				itemValueTooltip 	= layerName[myTarget.layer];
				break;
			case itemProp.lodamage:
				itemLabelTooltip 	= GetDictionaryEntry( 8163, pSocket.language ); // Lowest damage item can deal in combat
				itemValue 			= ( myTarget.lodamage ).toString();
				break;
			case itemProp.madeWith:
				itemLabelTooltip 	= GetDictionaryEntry( 8164, pSocket.language ); // Skill ID used to create item
				itemValue 			= ( myTarget.madeWith ).toString();
				break;
			case itemProp.maxhp:
				itemLabelTooltip 	= GetDictionaryEntry( 8165, pSocket.language ); // Maximum amount of hitpoints item can have
				itemValue 			= ( myTarget.maxhp ).toString();
				break;
			case itemProp.maxinterval:
				itemLabelTooltip 	= GetDictionaryEntry( 8166, pSocket.language ); // Max interval in seconds between respawns - SpawnObjects only
				if( myTarget.isSpawner )
				{
					itemValue 		= ( myTarget.hasOwnProperty( 'maxinterval' ) ? ( myTarget.maxinterval ).toString() : "-" );
				}
				else
				{
					itemValue 		= ( myTarget.hasOwnProperty( 'maxinterval' ) ? ( myTarget.maxinterval ).toString() : "<BASEFONT color=#EECD8B>n/a</BASEFONT>" );
				}
				break;
			case itemProp.maxItems:
				itemLabelTooltip 	= GetDictionaryEntry( 8167, pSocket.language ); // Max items a container can contain
				itemValue 			= ( myTarget.maxItems ).toString();
				break;
			case itemProp.maxRange:
				itemLabelTooltip 	= GetDictionaryEntry( 8168, pSocket.language ); // Maximum range of ranged weapon
				itemValue 			= ( myTarget.maxRange ).toString();
				break;
			case itemProp.maxUses:
				itemLabelTooltip 	= GetDictionaryEntry( 8209, pSocket.language ); // Maximum amount of uses item can have
				itemValue 			= ( myTarget.maxUses ).toString();
				break;
			case itemProp.mininterval:
				itemLabelTooltip 	= GetDictionaryEntry( 8169, pSocket.language ); // Min interval in seconds between respawns - SpawnObjects only
				if( myTarget.isSpawner )
				{
					itemValue 		= ( myTarget.hasOwnProperty( 'mininterval' ) ? ( myTarget.mininterval ).toString() : "-" );
				}
				else
				{
					itemValue 		= ( myTarget.hasOwnProperty( 'mininterval' ) ? ( myTarget.mininterval ).toString() : "<BASEFONT color=#EECD8B>n/a</BASEFONT>" );
				}
				break;
			case itemProp.more:
				itemLabelTooltip 	= GetDictionaryEntry( 8170, pSocket.language ); // Generic item property used for many different things
				itemValue 			= ( myTarget.more ).toString();
				var hexVal = "0x" + ( "00000000" + ( Number( myTarget.more ).toString( 16 ))).slice( -8 )
				itemValueTooltip = ( myTarget.more ).toString() + " ( " + hexVal + " )";
				break;
			case itemProp.morex:
				itemLabelTooltip 	= GetDictionaryEntry( 8170, pSocket.language ); // Generic item property used for many different things
				itemValue 			= ( myTarget.morex).toString();
				var hexVal = "0x" + ( "00000000" + ( Number( myTarget.morex ).toString( 16 ))).slice( -8 )
				itemValueTooltip = ( myTarget.morex ).toString() + " ( " + hexVal + " )";
				break;
				break;
			case itemProp.morey:
				itemLabelTooltip 	= GetDictionaryEntry( 8170, pSocket.language ); // Generic item property used for many different things
				itemValue 			= ( myTarget.morey).toString();
				var hexVal = "0x" + ( "00000000" + ( Number( myTarget.morey ).toString( 16 ))).slice( -8 )
				itemValueTooltip = ( myTarget.morey ).toString() + " ( " + hexVal + " )";
				break;
				break;
			case itemProp.morez:
				itemLabelTooltip 	= GetDictionaryEntry( 8170, pSocket.language ); // Generic item property used for many different things
				itemValue 			= ( myTarget.morez).toString();
				var hexVal = "0x" + ( "00000000" + ( Number( myTarget.morez ).toString( 16 ))).slice( -8 )
				itemValueTooltip = ( myTarget.morez ).toString() + " ( " + hexVal + " )";
				break;
				break;
			case itemProp.movable:
				itemLabelTooltip 	= GetDictionaryEntry( 8174, pSocket.language ); // Determines who can pick up/move item
				itemValue 			= ( myTarget.movable ).toString();
				break;
			case itemProp.name2:
				itemLabelTooltip 	= GetDictionaryEntry( 8175, pSocket.language ); // Secondary name of object, revealed using item identification
				itemValue 			= myTarget.name2;
				break;
			case itemProp.owner:
				itemLabelTooltip 	= GetDictionaryEntry( 8176, pSocket.language ); // Object registered as owner of this item
				itemValue 			= ( ValidateObject( myTarget.owner ) ? ( myTarget.owner.name ).toString() : "-" );
				itemValueTooltip	= ( ValidateObject( myTarget.owner ) ? ( myTarget.owner.name ).toString() + " ( " + ( myTarget.owner.serial).toString() + " )": "-" );
				break;
			case itemProp.origin:
				itemLabelTooltip 	= GetDictionaryEntry( 8177, pSocket.language ); // Era in which item was added to the game
				itemValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.origin ? ( myTarget.origin ).toString() : "-" ) + "</BASEFONT>";
				break;
			case itemProp.poison:
				itemLabelTooltip 	= GetDictionaryEntry( 8178, pSocket.language ); // Poison level of item from 0 to 5
				itemValue 			= ( myTarget.poison ).toString();
				break;
			case itemProp.race:
				itemLabelTooltip 	= GetDictionaryEntry( 8179, pSocket.language ); // Item deals double damage versus specified race
				itemValue 			= ( myTarget.race != null ? ( myTarget.race.id ).toString() + " ( " + ( myTarget.race.name ).toString() + " )" : "-" );
				break;
			case itemProp.rank:
				itemLabelTooltip 	= GetDictionaryEntry( 8180, pSocket.language ); // Quality of item determined at time of crafting
				itemValue 			= ( myTarget.rank ).toString();
				break;
			case itemProp.resistHeat:
				itemLabelTooltip 	= GetDictionaryEntry( 8181, pSocket.language ); // Item's Heat/Fire Resistance
				itemValue 			= (parseFloat( myTarget.resistHeat / 10 ).toFixed( 2 )).toString() + "%";
				break;
			case itemProp.resistCold:
				itemLabelTooltip 	= GetDictionaryEntry( 8182, pSocket.language ); // Item's Cold Resistance
				itemValue 			= (parseFloat( myTarget.resistCold / 10 ).toFixed( 2 )).toString() + "%";
				break;
			case itemProp.resistLight:
				itemLabelTooltip 	= GetDictionaryEntry( 8183, pSocket.language ); // Item's Light Resistance"
				itemValue 			= (parseFloat( myTarget.resistLight / 10 ).toFixed( 2 )).toString() + "%";
				break;
			case itemProp.resistLightning:
				itemLabelTooltip 	= GetDictionaryEntry( 8184, pSocket.language ); // Item's Lightning/Energy Resistance"
				itemValue 			= (parseFloat( myTarget.resistLightning / 10 ).toFixed( 2 )).toString() + "%";
				break;
			case itemProp.resistPoison:
				itemLabelTooltip 	= GetDictionaryEntry( 8185, pSocket.language ); // Item's Poison Resistance"
				itemValue 			= (parseFloat( myTarget.resistPoison / 10 ).toFixed( 2 )).toString() + "%";
				break;
			case itemProp.resistRain:
				itemLabelTooltip 	= GetDictionaryEntry( 8186, pSocket.language ); // Item's Rain Resistance"
				itemValue 			= (parseFloat( myTarget.resistRain / 10 ).toFixed( 2 )).toString() + "%";
				break;
			case itemProp.resistSnow:
				itemLabelTooltip 	= GetDictionaryEntry( 8187, pSocket.language ); // Item's Snow Resistance"
				itemValue 			= (parseFloat( myTarget.resistSnow / 10 ).toFixed( 2 )).toString() + "%";
				break;
			case itemProp.restock:
				itemLabelTooltip 	= GetDictionaryEntry( 8188, pSocket.language ); // Amount of this item that vendors will restock by default"
				itemValue 			= ( myTarget.restock ).toString();
				break;
			case itemProp.scripttrigger:
				itemLabelTooltip 	= GetDictionaryEntry( 8189, pSocket.language ); // JS Script assigned to item
				itemValue 			= ( myTarget.scripttrigger ).toString();
				break;
			case itemProp.sectionalist:
				itemLabelTooltip 	= GetDictionaryEntry( 8190, pSocket.language ); // True if spawn section is a list - SpawnObject only
				if( myTarget.isSpawner )
				{
					itemValue 		= ( myTarget.hasOwnProperty( 'sectionalist' ) ? ( myTarget.sectionalist ).toString() : "-" );
				}
				else
				{
					itemValue 		=  ( myTarget.hasOwnProperty( 'sectionalist' ) ? ( myTarget.sectionalist ? "true" : "false" ) : "<BASEFONT color=#EECD8B>n/a</BASEFONT>" );
				}
				break;
			case itemProp.sectionID:
				itemLabelTooltip 	= GetDictionaryEntry( 8213, pSocket.language ); // Section ID from DFNs object originated from
				itemValue 			= myTarget.sectionID;
				break;
			case itemProp.sellvalue:
				itemLabelTooltip 	= GetDictionaryEntry( 8191, pSocket.language ); // Item's sell value - price player can sell item to NPC shopkeeper for
				itemValue 			= ( myTarget.sellvalue ).toString();
				break;
			case itemProp.spawnsection:
				itemLabelTooltip 	= GetDictionaryEntry( 8192, pSocket.language ); // SpawnSection used to spawn objects from - SpawnObject only
				if( myTarget.isSpawner )
				{
					itemValue 		= ( myTarget.hasOwnProperty( 'spawnSection' ) ? ( myTarget.spawnSection ).toString() : "-" );
				}
				else
				{
					itemValue 		= ( myTarget.hasOwnProperty( 'spawnSection' ) ? ( myTarget.spawnSection ).toString() : "<BASEFONT color=#EECD8B>n/a</BASEFONT>" );
				}
				break;
			case itemProp.speed:
				itemLabelTooltip 	= GetDictionaryEntry( 8193, pSocket.language ); // Attack speed of item - used by weapons
				itemValue 			= ( myTarget.speed ).toString();
				break;
			case itemProp.strength:
				itemLabelTooltip 	= GetDictionaryEntry( 8194, pSocket.language ); // Strength required to equip item
				itemValue 			= ( myTarget.strength ).toString();
				break;
			case itemProp.tempTimer:
				itemLabelTooltip 	= GetDictionaryEntry( 8195, pSocket.language ); // Temporary timer used by spawners
				itemValue 			= ( myTarget.tempTimer ).toString();
				break;
			case itemProp.type:
				itemLabelTooltip 	= GetDictionaryEntry( 8171, pSocket.language ); // Item type of item - determines double-click behaviour
				itemValue 			= ( myTarget.type ).toString()
				itemValueTooltip 	= itemTypeNames[myTarget.type];
				break;
			case itemProp.usesLeft:
				itemLabelTooltip 	= GetDictionaryEntry( 8208, pSocket.language ); // Item's current remaining uses (cannot exceed value of maxUses property)
				itemValue 			= ( myTarget.usesLeft ).toString();
				break;
			case itemProp.visible:
				itemLabelTooltip 	= GetDictionaryEntry( 8197, pSocket.language ); // Determines who item is visible for (0 = Visible, 1 = Hidden, 2 = Magically Invisible, 3 = Visible to GMs only)
				itemValue 			= ( myTarget.visible ).toString();
				break;
			case itemProp.weight:
				itemLabelTooltip 	= GetDictionaryEntry( 8198, pSocket.language ); // Weight of item (100 = 1.0 stone)
				itemValue 			= ( myTarget.weight ).toString();
				itemValueTooltip 	= (parseFloat( myTarget.weight / 100 ).toFixed( 2 )).toString() + " stones";
				break;
			case itemProp.weightMax:
				itemLabelTooltip 	= GetDictionaryEntry( 8199, pSocket.language ); // Maximum weight a container can hold (100 = 1.0 stone)
				itemValue 			= ( myTarget.weightMax ).toString();
				itemValueTooltip 	= (parseFloat( myTarget.weightMax / 100 ).toFixed( 2 )).toString() + " stones";
				break;
			case itemProp.wipable:
				itemLabelTooltip 	= GetDictionaryEntry( 8200, pSocket.language ); // Marks item as wipable with WIPE command
				itemValue 			= ( myTarget.wipable ? "true" : "false" );
				break;
			case itemProp.worldnumber:
				itemLabelTooltip 	= GetDictionaryEntry( 8201, pSocket.language ); // World that item exists in
				itemValue 			= ( myTarget.worldnumber ).toString();
				break;
			case itemProp.x:
				itemLabelTooltip 	= GetDictionaryEntry( 8202, pSocket.language ); // X coordinate of item in the world - or in container
				itemValue 			= ( myTarget.x ).toString();
				break;
			case itemProp.y:
				itemLabelTooltip 	= GetDictionaryEntry( 8203, pSocket.language ); // Y coordinate of item in the world - or in container
				itemValue 			= ( myTarget.y ).toString();
				break;
			case itemProp.z:
				itemLabelTooltip 	= GetDictionaryEntry( 8204, pSocket.language ); // Z coordinate of item in the world - or in container
				itemValue 			= ( myTarget.z ).toString();
				break;
			default:
				errorFound = true;
				Console.PrintSectionBegin();
  				Console.Print( "Unhandled itemProperty in tweak command script!" );
				Console.PrintDone();
				break;
		}

		if( !errorFound )
		{
			// Labels
			itemGump.AddHTMLGump( 15, labelStartY, 100, 20, 0, 0, propertyLabelStart + propertyName + propertyLabelEnd );
			if( enableTooltips )
			{
				itemGump.AddToolTip( tooltipClilocID, pSocket, itemLabelTooltip.toString() );
			}

			if( itemValue == "-" )
			{
				itemValueTooltip = GetDictionaryEntry( 8205, pSocket.language ); // Value not set
			}

			// Buttons
			if( propertyName != "Itemsinside" && propertyName != "Container" && propertyName != "IsGuarded" && propertyName != "Origin" && ( myTarget.isSpawner || ( !myTarget.isSpawner &&
			 ( propertyName != "Maxinterval" && propertyName != "Mininterval" && propertyName != "Spawnsection" && propertyName != "Sectionalist" ))))
			{
				itemGump.AddButton( 120, buttonStartY, gumpMainButtonOff, gumpMainButtonOn, 1, 0, buttonID);
				if( enableTooltips )
				{
					itemGump.AddToolTip( tooltipClilocID, pSocket, ( itemValueTooltip != "" ? itemValueTooltip : itemValue ));
				}
			}

			// Values
			itemGump.AddHTMLGump( 125, valueStartY, 105, 20, 0, 0, propertyValueStart + itemValue + propertyValueEnd );
			if( enableTooltips && propertyName == "Itemsinside" || propertyName == "Container" || propertyName == "IsGuarded" || propertyName == "Origin" )
			{
				itemGump.AddToolTip( tooltipClilocID, pSocket, ( itemValueTooltip != "" ? itemValueTooltip : itemValue ) + " (Read-Only)" );
			}
			else if( enableTooltips && !myTarget.isSpawner && ( propertyName == "Maxinterval" || propertyName == "Mininterval" || propertyName == "Spawnsection" || propertyName == "Sectionalist" ))
			{
				itemGump.AddToolTip( tooltipClilocID, pSocket, "<BASEFONT color=#EECD8B>n/a</BASEFONT> (SpawnObject Only)" );
			}
		}

		if( gumpPage == 1 )
		{
			pageOneLabelStartY += 20;
			pageOneButtonStartY += 20;
			pageOneValueStartY += 20;
		}
		else
		{
			pageXlabelStartY += 20;
			pageXbuttonStartY += 20;
			pageXvalueStartY += 20;
		}

		buttonID++;
	}

	itemGump.Send( pSocket );
	itemGump.Free();
}

// Handle properties of character targets
function HandleCharTarget( pSocket, myTarget )
{
	var charGump = new Gump;
	charGump = RenderZeroethPage( pSocket, charGump, myTarget, false, false );

	var propertyName;
	var buttonID = 200;
	var gumpPage = 1;
	var totalPages = Math.ceil( 1 + ( charPropCount - 13 ) / 17 );

	// First page
	var pageOneLabelStartY = 139;
	var pageOneButtonStartY = 140;
	var pageOneValueStartY = 139;
	// Subsequent pages
	var pageXlabelStartY = 59;
	var pageXbuttonStartY = 60;
	var pageXvalueStartY = 59;

	// Loop over all character properties
	var i = 0;
	for( i = 0; i < charPropCount; i++ )
	{
		if( i == 0 )
		{
			// Page 1
			charGump = RenderFirstPage( pSocket, charGump, myTarget, "Character", charPropCount, totalPages );
		}
		else // All other pages
		{
			switch( i )
			{
				case 13:
					gumpPage = 2;
					break;
				case 30:
					gumpPage = 3;
					break;
				case 47:
					gumpPage = 4;
					break;
				case 64:
					gumpPage = 5;
					break;
				case 81:
					gumpPage = 6;
					break;
				case 98:
					gumpPage = 7;
					break;
				case 115:
					gumpPage = 8;
					break;
				case 132:
					gumpPage = 9;
					break;
				default:
					break;
			}

			// Only add these when it's time for a new page
			if( i == 13 || i == 30 || i == 47 || i == 64 || i == 81 || i == 98 || i == 115 || i == 132 )
			{
				pageXlabelStartY = 59;
				pageXbuttonStartY = 60;
				pageXvalueStartY = 59;
				charGump = RenderOtherPages( pSocket, charGump, gumpPage, totalPages );
			}
		}

		var labelStartY = 0;
		var buttonStartY = 0;
		var valueStartY = 0;
		if( gumpPage == 1 )
		{
			labelStartY = pageOneLabelStartY;
			buttonStartY = pageOneButtonStartY;
			valueStartY = pageOneValueStartY;
		}
		else
		{
			labelStartY = pageXlabelStartY;
			buttonStartY = pageXbuttonStartY;
			valueStartY = pageXvalueStartY;
		}

		var index = 20;
		for( var k in charProp )
		{
		    if( charProp.hasOwnProperty( k ) && index == i + 20 )
		    {
		        // k is key
				propertyName = k.charAt( 0 ).toUpperCase() + k.slice( 1 );
		        break;
		    }
		    index++;
		}

		var charLabelTooltip = "";
		var charValue = "";
		var charValueTooltip = "";
		var errorFound = false;
		switch( i + 200 )
		{
			case charProp.accountNum:
				charLabelTooltip 	= GetDictionaryEntry( 8300, pSocket.language ); // Account number associated with player. Controlled by server (Read-Only)
				charValue 			= myTarget.npc ? "<BASEFONT color=#EECD8B>" + "n/a"  + "</BASEFONT>" : "<BASEFONT color=#32668A>[click to view]</BASEFONT>"; //myTarget.npc ? "<BASEFONT color=#EECD8B>" + "n/a"  + "</BASEFONT>" : "<BASEFONT color=#EECD8B>" + ( myTarget.accountNum).toString() + "</BASEFONT>";
				charValueTooltip  	= myTarget.accountNum;
				break;
			case charProp.aitype:
				charLabelTooltip 	= GetDictionaryEntry( 8301, pSocket.language ); // NPC AI Type
				charValue 			= ( myTarget.aitype ).toString();
				charValueTooltip 	= aiTypeName[myTarget.aitype];
				break;
			case charProp.allmove:
				charLabelTooltip 	= GetDictionaryEntry( 8302, pSocket.language ); // Toggles being able to move all items regardless of movable status
				charValue 			= ( myTarget.allmove ? "true" : "false" );
				break;
			case charProp.attack:
				charLabelTooltip 	= GetDictionaryEntry( 8303, pSocket.language ); // Gets calculated attack value for character based on skills, equipment, etc. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.attack ).toString() + "</BASEFONT>";
				break;
			case charProp.attacker:
				charLabelTooltip 	= GetDictionaryEntry( 8304, pSocket.language ); // Character's current attacker
				// charValue 			= (( myTarget.hasOwnProperty( 'attacker' ) && myTarget.attacker != null ) ? "0x" + ( myTarget.attacker).toString( 16 ) : "-" );

				charValue 			= ( ValidateObject( myTarget.attacker) ? ( myTarget.attacker.name ).toString() : "-" );
				charValueTooltip	= ( ValidateObject( myTarget.attacker) ? ( myTarget.attacker.name ).toString() + " ( " + ( myTarget.attacker.serial ).toString() + " )": "-" );
				break;
			case charProp.attackFirst:
				charLabelTooltip 	= GetDictionaryEntry( 8305, pSocket.language ); // Did character attack first?
				charValue 			= ( myTarget.attackFirst ? "true" : "false" );
				break;
			case charProp.atWar:
				charLabelTooltip 	= GetDictionaryEntry( 8306, pSocket.language ); // Toggles combat mode for NPC characters (NPC Only)
				charValue 			= ( myTarget.atWar ).toString();
				break;
			case charProp.baseskills:
				charLabelTooltip 	= GetDictionaryEntry( 8307, pSocket.language ); // Base skill values for character
				charValue 			= "<BASEFONT color=#32668A>[click to view]</BASEFONT>";
				break;
			case charProp.brkPeaceChance:
				charLabelTooltip 	= GetDictionaryEntry( 8308, pSocket.language ); // Chance of character affected by peacemaking will break out of peace state
				charValue 			= ( myTarget.brkPeaceChance ).toString() + "%";
				break;
			case charProp.canAttack:
				charLabelTooltip 	= GetDictionaryEntry( 8309, pSocket.language ); // Toggles whether character can attack other characters
				charValue 			= ( myTarget.canAttack ? "true" : "false" );
				break;
			case charProp.canBroadcast:
				charLabelTooltip 	= GetDictionaryEntry( 8310, pSocket.language ); // Toggles whether character can broadcast messages
				charValue 			= ( myTarget.canBroadcast ? "true" : "false" );
				break;
			case charProp.canRun:
				charLabelTooltip 	= GetDictionaryEntry( 8311, pSocket.language ); // Toggles whether character can run
				charValue 			= ( myTarget.canRun ? "true" : "false" );
				break;
			case charProp.canSnoop:
				charLabelTooltip 	= GetDictionaryEntry( 8312, pSocket.language ); // Toggles whether character can snoop in other character's backpacks
				charValue 			= ( myTarget.canSnoop ? "true" : "false" );
				break;
			case charProp.cell:
				charLabelTooltip 	= GetDictionaryEntry( 8313, pSocket.language ); // Current jail character is locked up in, if any
				charValue 			=  ( myTarget.cell ).toString();
				break;
			case charProp.colour:
				charLabelTooltip 	= GetDictionaryEntry( 8314, pSocket.language ); // Colour of character's body
				charValue 			=  "0x" + ( myTarget.colour ).toString( 16 );
				break;
			case charProp.commandlevel:
				if( myTarget.commandlevel >= pSocket.currentChar.commandlevel )
				{
					charLabelTooltip 	= GetDictionaryEntry( 8315, pSocket.language ); // Character's command level (Read-Only, cannot adjust command level of fellow GMs/Admins)
					charValue 			=  "<BASEFONT color=#EECD8B>" + ( myTarget.commandlevel ).toString() + "</BASEFONT>";
				}
				else
				{
					charLabelTooltip 	= GetDictionaryEntry( 8316, pSocket.language ); // Character's command level.
					charValue 			=  ( myTarget.commandlevel ).toString();
				}
				break;
			case charProp.criminal:
				charLabelTooltip 	= GetDictionaryEntry( 8317, pSocket.language ); // Marks character as criminal, instead of innocent, neutral or murderer
				charValue 			= ( myTarget.criminal ? "true" : "false" );
				break;
			case charProp.dead:
				charLabelTooltip 	= GetDictionaryEntry( 8318, pSocket.language ); // Is (player) character dead? Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.dead ? "true" : "false" ) + "</BASEFONT>";
				break;
			case charProp.deaths:
				charLabelTooltip 	= GetDictionaryEntry( 8319, pSocket.language ); // Total amount of times (player) character has died
				charValue 			=  ( myTarget.deaths ).toString();
				break;
			case charProp.dexterity:
				charLabelTooltip 	= GetDictionaryEntry( 8320, pSocket.language ); // Dexterity attribute of character
				charValue 			= ( myTarget.dexterity ).toString();
				break;
			case charProp.direction:
				charLabelTooltip 	= GetDictionaryEntry( 8321, pSocket.language ); // Current direction character is facing
				charValue 			= "0x" + ( myTarget.direction ).toString( 16 );
				charValueTooltip 	= "0x" + ( myTarget.direction ).toString( 16 ) + " ( " + ( myTarget.direction ).toString() + " )";
				break;
			case charProp.emoteColour:
				charLabelTooltip 	= GetDictionaryEntry( 8322, pSocket.language ); // Colour of character's emotes
				charValue 			= "0x" + ( myTarget.emoteColour ).toString( 16 );
				break;
			case charProp.fame:
				charLabelTooltip 	= GetDictionaryEntry( 8323, pSocket.language ); // Character's current fame level
				charValue 			= ( myTarget.fame ).toString();
				break;
			case charProp.flag:
				charLabelTooltip 	= "NPC flag (<BASEFONT COLOR=red>0x1 = murderer</BASEFONT>, <BASEFONT COLOR=grey>0x2 = criminal</BASEFONT>, <BASEFONT COLOR=cyan>0x4 = innocent</BASEFONT>, <BASEFONT COLOR=grey>0x8 = neutral</BASEFONT>)";
				var flagText 	= ( myTarget.flag == 0x1 ? " (<BASEFONT COLOR=red>Murderer</BASEFONT>)" : ( myTarget.flag == 0x2 ? " (<BASEFONT COLOR=grey>Criminal</BASEFONT>)" : ( myTarget.flag == 0x4 ? " (<BASEFONT COLOR=cyan>Innocent</BASEFONT>)" : " (<BASEFONT COLOR=grey>Neutral</BASEFONT>)" )));
				charValue 			= "0x" + ( myTarget.flag ).toString( 16 );
				charValueTooltip	= "0x" + ( myTarget.flag ).toString( 16 ) + flagText;
				break;
			case charProp.fontType:
				charLabelTooltip 	= GetDictionaryEntry( 8324, pSocket.language ); // Font type used by character's speech
				charValue 			= ( myTarget.fontType ).toString();
				break;
			case charProp.foodList:
				charLabelTooltip 	= GetDictionaryEntry( 8325, pSocket.language ); // ID of foodlist that a tamed creature will accept as food
				charValue 			= myTarget.hasOwnProperty( 'foodList' ) ? ( myTarget.foodList ).toString() : "-";
				break;
			case charProp.frozen:
				charLabelTooltip 	= GetDictionaryEntry( 8326, pSocket.language ); // Toggles whether object is frozen (immovable) or not
				charValue 			= ( myTarget.frozen ? "true" : "false" );
				break;
			case charProp.fx1:
				charLabelTooltip 	= GetDictionaryEntry( 8431, pSocket.language ); // Top left X coord of character's wandertype boundary
				charValue 			= ( myTarget.fx1 ).toString();
				break;
			case charProp.fy1:
				charLabelTooltip 	= GetDictionaryEntry( 8432, pSocket.language ); // Top left Y coord of character's wandertype boundary
				charValue 			= ( myTarget.fy1 ).toString();
				break;
			case charProp.fx2:
				charLabelTooltip 	= GetDictionaryEntry( 8433, pSocket.language ); // Bottom right X coord of character's wandertype boundary
				charValue 			= ( myTarget.fx2 ).toString();
				break;
			case charProp.fy2:
				charLabelTooltip 	= GetDictionaryEntry( 8434, pSocket.language ); // Bottom right Y coord of character's wandertype boundary
				charValue 			= ( myTarget.fy2 ).toString();
				break;
			case charProp.fz:
				charLabelTooltip 	= GetDictionaryEntry( 8435, pSocket.language ); // Z coord of character's wandertype boundary
				charValue 			= ( myTarget.fz ).toString();
				break;
			case charProp.gender:
				charLabelTooltip 	= GetDictionaryEntry( 8327, pSocket.language ); // Gender of character (male/female)
				charValue 			= ( myTarget.gender ).toString();
				break;
			case charProp.guild:
				charLabelTooltip 	= GetDictionaryEntry( 8328, pSocket.language ); // Player guild character belongs to, if any
				charValue 			= ( myTarget.guild != null ? ( myTarget.guild.name + " (Guildstone: " + ( myTarget.guild.stone != null && myTarget.guild.stone.serial ) + " )" ) : "Not in Guild" );
				break;
			case charProp.guildTitle:
				charLabelTooltip 	= GetDictionaryEntry( 8329, pSocket.language ); // Guild title of character
				charValue 			= myTarget.guildTitle != "" ? ( myTarget.guildTitle ).toString() : "-";
				break;
			case charProp.health:
				charLabelTooltip 	= GetDictionaryEntry( 8330, pSocket.language ); // Character's current health/hitpoints (cannot exceed value of maxhp property)
				charValue 			= ( myTarget.health ).toString();
				break;
			case charProp.hidamage:
				charLabelTooltip 	= GetDictionaryEntry( 8331, pSocket.language ); // Highest damage character can deal in combat with wrestlin/unarmed attacks
				charValue 			= ( myTarget.hidamage ).toString();
				break;
			case charProp.houseicons:
				charLabelTooltip 	= GetDictionaryEntry( 8332, pSocket.language ); // Toggles whether house icons/deeds are shown instead of multis for character
				charValue 			= ( myTarget.houseicons ).toString();
				break;
			case charProp.housesCoOwned:
				charLabelTooltip 	= GetDictionaryEntry( 8333, pSocket.language ); // Number of houses co-owned by character. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.housesCoOwned ).toString() + "</BASEFONT>";
				break;
			case charProp.housesOwned:
				charLabelTooltip 	= GetDictionaryEntry( 8334, pSocket.language ); // Number of houses owned by character. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.housesOwned ).toString() + "</BASEFONT>";
				break;
			case charProp.hunger:
				charLabelTooltip 	= GetDictionaryEntry( 8335, pSocket.language ); // Character's current hunger status (0-6)
				charValue 			= ( myTarget.hunger ).toString();
				break;
			case charProp.hungerWildChance:
				charLabelTooltip 	= GetDictionaryEntry( 8336, pSocket.language ); // Chance for extremely hungry pet to go wild with every NPC AI loop
				charValue 			= ( myTarget.hungerWildChance ).toString() + "%";
				break;
			case charProp.innocent:
				charLabelTooltip 	= GetDictionaryEntry( 8337, pSocket.language ); // Marks character as innocent, instead of neutral, criminal or murderer
				charValue 			= ( myTarget.innocent ? "true" : "false" );
				break;
			case charProp.instanceID:
				charLabelTooltip 	= GetDictionaryEntry( 8338, pSocket.language ); // ID of instance character is currently in. Objects in different instances will not be able to interact with one another!
				charValue 			= ( myTarget.instanceID ).toString();
				break;
			case charProp.intelligence:
				charLabelTooltip 	= GetDictionaryEntry( 8339, pSocket.language ); // Intelligence attribute of character
				charValue 			= ( myTarget.intelligence ).toString();
				break;
			case charProp.isAnimal:
				charLabelTooltip 	= GetDictionaryEntry( 8340, pSocket.language ); // Is character an animal? Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.isAnimal ? "true" : "false" ) + "</BASEFONT>";
				break;
			case charProp.isCasting:
				charLabelTooltip 	= GetDictionaryEntry( 8341, pSocket.language ); // Is character casting a spell?
				charValue 			= ( myTarget.isCasting ? "true" : "false" );
				break;
			case charProp.isCounselor:
				charLabelTooltip 	= GetDictionaryEntry( 8342, pSocket.language ); // Is character a counselor?
				charValue 			= ( myTarget.isCounselor ? "true" : "false" );
				break;
			case charProp.isDispellable:
				charLabelTooltip 	= GetDictionaryEntry( 8343, pSocket.language ); // Can character be dispelled by the Dispel spell? (NPC only)
				charValue 			= ( myTarget.isDispellable ? "true" : "false" );
				break;
			case charProp.isflying:
				charLabelTooltip 	= GetDictionaryEntry( 8344, pSocket.language ); // Is character flying? Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.isflying ? "true" : "false" ) + "</BASEFONT>";
				break;
			case charProp.isGM:
				if( myTarget.commandlevel >= pSocket.currentChar.commandlevel )
				{
					charLabelTooltip 	= GetDictionaryEntry( 8345, pSocket.language ); // Is character a GM? (Read-Only)
					charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.isGM ? "true" : "false" ) + "</BASEFONT>";
				}
				else
				{
					charLabelTooltip 	= GetDictionaryEntry( 8346, pSocket.language ); // Is character a GM?
					charValue 			= ( myTarget.isGM ? "true" : "false" );
				}
				break;
			case charProp.isGMPageable:
				charLabelTooltip 	= GetDictionaryEntry( 8347, pSocket.language ); // Is character able to see and respond to GM pages?
				charValue 			= ( myTarget.isGMPageable ? "true" : "false" );
				break;
			case charProp.isHuman:
				charLabelTooltip 	= GetDictionaryEntry( 8348, pSocket.language ); // Is character a human? Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.isHuman ? "true" : "false" ) + "</BASEFONT>";
				break;
			case charProp.isIncognito:
				charLabelTooltip 	= GetDictionaryEntry( 8349, pSocket.language ); // Is character incognito?
				charValue 			= ( myTarget.isIncognito ? "true" : "false" );
				break;
			case charProp.isJailed:
				charLabelTooltip 	= GetDictionaryEntry( 8350, pSocket.language ); // Is character jailed? Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.isJailed ? "true" : "false" ) + "</BASEFONT>";
				break;
			case charProp.isMeditating:
				charLabelTooltip 	= GetDictionaryEntry( 8351, pSocket.language ); // Is character meditating? Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.isMeditating ? "true" : "false" ) + "</BASEFONT>";
				break;
			case charProp.isonhorse:
				charLabelTooltip 	= GetDictionaryEntry( 8352, pSocket.language ); // Is character on a mount? Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.isonhorse ? "true" : "false" ) + "</BASEFONT>";
				break;
			case charProp.isPolymorphed:
				charLabelTooltip 	= GetDictionaryEntry( 8353, pSocket.language ); // Is character polymorphed?
				charValue 			= ( myTarget.isPolymorphed ? "true" : "false" );
				break;
			case charProp.isShop:
				charLabelTooltip 	= GetDictionaryEntry( 8354, pSocket.language ); // Is character a vendor? (NPC only)
				charValue 			= ( myTarget.isShop ? "true" : "false" );
				break;
			case charProp.isUsingPotion:
				charLabelTooltip 	= GetDictionaryEntry( 8355, pSocket.language ); // Is character using a potion?
				charValue 			= ( myTarget.isUsingPotion ? "true" : "false" );
				break;
			case charProp.karma:
				charLabelTooltip 	= GetDictionaryEntry( 8356, pSocket.language ); // Character's current karma level
				charValue 			= ( myTarget.karma ).toString();
				break;
			case charProp.lightlevel:
				charLabelTooltip 	= GetDictionaryEntry( 8357, pSocket.language ); // Character's current individual light level
				charValue 			= ( myTarget.lightlevel ).toString();
				break;
			case charProp.lodamage:
				charLabelTooltip 	= GetDictionaryEntry( 8358, pSocket.language ); // Minimum damage dealt by character in combat when using wrestling/unarmed attacks
				charValue 			= ( myTarget.lodamage ).toString();
				break;
			case charProp.magicReflect:
				charLabelTooltip 	= GetDictionaryEntry( 8359, pSocket.language ); // Is magic reflection active for character?
				charValue 			= ( myTarget.magicReflect ? "true" : "false" );
				break;
			case charProp.mana:
				charLabelTooltip 	= GetDictionaryEntry( 8360, pSocket.language ); // Character's current mana
				charValue 			= ( myTarget.mana ).toString();
				break;
			case charProp.maxhp:
				charLabelTooltip 	= GetDictionaryEntry( 8361, pSocket.language ); // Maximum HP character can have (max value that can display properly in player status window is 9999)
				charValue 			= ( myTarget.maxhp ).toString();
				break;
			case charProp.maxmana:
				charLabelTooltip 	= GetDictionaryEntry( 8362, pSocket.language ); // Maximum mana character can have
				charValue 			= ( myTarget.maxmana ).toString();
				break;
			case charProp.maxstamina:
				charLabelTooltip 	= GetDictionaryEntry( 8363, pSocket.language ); // Maximum stamina character can have
				charValue 			= ( myTarget.maxstamina ).toString();
				break;
			case charProp.mounted:
				charLabelTooltip 	= GetDictionaryEntry( 8364, pSocket.language ); // Is mount carrying someone?
				charValue 			= ( myTarget.mounted ? "true" : "false" );
				break;
			case charProp.multi:
				charLabelTooltip 	= GetDictionaryEntry( 8365, pSocket.language ); // Object for multi the character is currently in
				charValue 			= ValidateObject( myTarget.multi) ? "<BASEFONT color=#32668A>[click to view]</BASEFONT>" : "<BASEFONT color=#EECD8B>N/A</BASEFONT>";
				break;
			case charProp.murdercount:
				charLabelTooltip 	= GetDictionaryEntry( 8366, pSocket.language ); // Amount of players character has killed
				charValue 			= ( myTarget.murdercount ).toString();
				break;
			case charProp.murderer:
				charLabelTooltip 	= GetDictionaryEntry( 8367, pSocket.language ); // Is character a murderer?
				charValue 			= ( myTarget.murderer ? "true" : "false" );
				break;
			case charProp.neutral:
				charLabelTooltip 	= GetDictionaryEntry( 8368, pSocket.language ); // Is character neutrally flagged?
				charValue 			= ( myTarget.neutral ? "true" : "false" );
				break;
			case charProp.nextAct:
				charLabelTooltip 	= GetDictionaryEntry( 8369, pSocket.language ); // The next spellcasting action character is going to do. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + "0x" + ( myTarget.nextAct ).toString( 16 ) + "</BASEFONT>";
				break;
			case charProp.noNeedMana:
				charLabelTooltip 	= GetDictionaryEntry( 8370, pSocket.language ); // Mana is not needed for character to cast spells
				charValue 			= ( myTarget.noNeedMana ? "true" : "false" );
				break;
			case charProp.noNeedReags:
				charLabelTooltip 	= GetDictionaryEntry( 8371, pSocket.language ); // Reagents are not needed for character to cast spells
				charValue 			= ( myTarget.noNeedReags ? "true" : "false" );
				break;
			case charProp.noSkillTitles:
				charLabelTooltip 	= GetDictionaryEntry( 8372, pSocket.language ); // Skill titles are not displayed for character
				charValue 			= ( myTarget.noSkillTitles ? "true" : "false" );
				break;
			case charProp.npc:
				charLabelTooltip 	= GetDictionaryEntry( 8373, pSocket.language ); // Whether character is an NPC. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.npc ? "true" : "false" ) + "</BASEFONT>";
				break;
			case charProp.npcFlag:
				charLabelTooltip 	= "NPC flag (<BASEFONT COLOR=grey>0 = Neutral</BASEFONT>, <BASEFONT COLOR=cyan>1 = Innocent</BASEFONT>, <BASEFONT COLOR=red>2 = Evil</BASEFONT>)";
				var npcFlagText 	= ( myTarget.npcFlag == 0 ? " (<BASEFONT COLOR=grey>Neutral</BASEFONT>)" : ( myTarget.npcFlag == 1 ? " (<BASEFONT COLOR=cyan>Innocent</BASEFONT>)" : " (<BASEFONT COLOR=red>Evil</BASEFONT>)" ));
				charValue 			= ( myTarget.npcFlag ).toString();
				charValueTooltip	= ( myTarget.npcFlag ).toString() + npcFlagText;
				break;
			case charProp.oldWandertype:
				charLabelTooltip 	= GetDictionaryEntry( 8374, pSocket.language ); // NPC's old/previous wandertype
				charValue 			= ( myTarget.oldWandertype ).toString();
				break;
			case charProp.online:
				charLabelTooltip 	= GetDictionaryEntry( 8375, pSocket.language ); // Whether player character is online or not. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.online ).toString() + "</BASEFONT>";
				break;
			case charProp.orgID:
				charLabelTooltip 	= GetDictionaryEntry( 8376, pSocket.language ); // Character's original body ID
				charValue 			= "0x" + ( myTarget.orgID ).toString( 16 );
				break;
			case charProp.orgSkin:
				charLabelTooltip 	= GetDictionaryEntry( 8377, pSocket.language ); // Character's original skin (colour)
				charValue 			= "0x" + ( myTarget.orgSkin ).toString( 16 );
				break;
			case charProp.ownedItemsCount:
				charLabelTooltip 	= GetDictionaryEntry( 8378, pSocket.language ); // Number of items owned by character. Controlled by server (Read-Only)
				charValue 			=  "<BASEFONT color=#EECD8B>" + ( myTarget.ownedItemsCount ).toString() + "</BASEFONT>";
				break;
			case charProp.owner:
				charLabelTooltip 	= GetDictionaryEntry( 8379, pSocket.language ); // Character's owner, if any
				charValue 			= ( myTarget.hasOwnProperty( 'owner' ) && myTarget.owner != null ) ? ( myTarget.owner.name ).toString() : "-";
				break;
			case charProp.pack:
				charLabelTooltip 	= GetDictionaryEntry( 8380, pSocket.language ); // Object of character's root backpack
				charValue 			= ( myTarget.hasOwnProperty( 'pack' ) && myTarget.pack != null ) ? ( myTarget.pack ).toString() : "-";
				break;
			case charProp.party:
				charLabelTooltip 	= GetDictionaryEntry( 8381, pSocket.language ); // Object of party character is member of
				charValue 			= myTarget.hasOwnProperty( 'party' ) ? ( myTarget.party ).toString() : "-";
				break;
			case charProp.partyLootable:
				charLabelTooltip 	= GetDictionaryEntry( 8382, pSocket.language ); // Is character lootable by party members when dead?
				charValue 			= ( myTarget.partyLootable ).toString();
				break;
			case charProp.petCount:
				charLabelTooltip 	= GetDictionaryEntry( 8383, pSocket.language ); // Total amount of pets controlled by character. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.petCount ).toString() + "</BASEFONT>";
				break;
			case charProp.poison:
				charLabelTooltip 	= GetDictionaryEntry( 8384, pSocket.language ); // Character's current poisoned state (0-5)
				charValue 			= ( myTarget.poison ).toString();
				break;
			case charProp.poisonStrength:
				charLabelTooltip 	= GetDictionaryEntry( 8385, pSocket.language ); // Strength of poison applied by unarmed/wrestling attacks (0-5)
				charValue 			= ( myTarget.poisonStrength ).toString();
				break;
			case charProp.race:
				charLabelTooltip 	= GetDictionaryEntry( 8386, pSocket.language ); // Race character belongs to
				charValue 			= ( myTarget.race.id ).toString();
				charValueTooltip 	= ( myTarget.race.name ).toString();
				break;
			case charProp.raceGate:
				charLabelTooltip 	= GetDictionaryEntry( 8387, pSocket.language ); // ID of Race for which character has used a Race Gate, if any
				charValue 			= ( myTarget.raceGate == 65535 ? "-" : ( myTarget.raceGate ).toString());
				break;
			case charProp.region:
				charLabelTooltip 	= GetDictionaryEntry( 8388, pSocket.language ); // Object of region character is currently in. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.region ).toString() + "</BASEFONT>";
				charValueTooltip	= "<BASEFONT color=#EECD8B>" + ( myTarget.region.name ).toString() + "</BASEFONT>";
				break;
			case charProp.sayColour:
				charLabelTooltip 	= GetDictionaryEntry( 8389, pSocket.language ); // Colour of character's speech text
				charValue 			= "0x" + ( myTarget.sayColour ).toString( 16 );
				break;
			case charProp.scripttrigger:
				charLabelTooltip 	= GetDictionaryEntry( 8390, pSocket.language ); // JS Script assigned to character
				charValue 			= ( myTarget.scripttrigger ).toString();
				break;
			case charProp.sectionID:
				charLabelTooltip 	= GetDictionaryEntry( 8213, pSocket.language ); // Section ID from DFNs object originated from
				charValue 			= ( myTarget.sectionID ).toString();
				break;
			case charProp.singClickSer:
				charLabelTooltip 	= GetDictionaryEntry( 8391, pSocket.language ); // Toggles whether single-clicks shows serial of clicked object
				charValue 			= ( myTarget.singClickSer ).toString();
				break;
			case charProp.skillLock:
				charLabelTooltip 	= GetDictionaryEntry( 8392, pSocket.language ); // Toggles skill lock for specific character skills (N/A)
				charValue 			= "<BASEFONT color=#EECD8B>N/A</BASEFONT>";
				break;
			case charProp.skills:
				charLabelTooltip 	= GetDictionaryEntry( 8393, pSocket.language ); // Set value for a specific character skill
				charValue 			= "<BASEFONT color=#32668A>[click to view]</BASEFONT>";
				break;
			case charProp.skillsused:
				charLabelTooltip 	= GetDictionaryEntry( 8394, pSocket.language ); // Marks whether specific skills are in use (N/A)
				charValue 			= "<BASEFONT color=#EECD8B>N/A</BASEFONT>";
				break;
			case charProp.skillToPeace:
				charLabelTooltip 	= GetDictionaryEntry( 8395, pSocket.language ); // Peacemaking skill required to peacemake this NPC
				charValue 			= ( myTarget.skillToPeace ).toString();
				break;
			case charProp.skillToProv:
				charLabelTooltip 	= GetDictionaryEntry( 8396, pSocket.language ); // Provocation skill required to provoke this NPC
				charValue 			= ( myTarget.skillToProv ).toString();
				break;
			case charProp.skillToTame:
				charLabelTooltip 	= GetDictionaryEntry( 8397, pSocket.language ); // Animal Taming skill required to tame this NPC
				charValue 			= ( myTarget.skillToTame ).toString();
				break;
			case charProp.spattack:
				charLabelTooltip 	= GetDictionaryEntry( 8398, pSocket.language ); // NPC will cast spells from this spell circle #
				charValue 			= ( myTarget.spattack ).toString();
				break;
			case charProp.spdelay:
				charLabelTooltip 	= GetDictionaryEntry( 8399, pSocket.language ); // Delay between spellcasts for NPC character
				charValue 			= ( myTarget.spdelay ).toString();
				break;
			case charProp.spellCast:
				charLabelTooltip 	= GetDictionaryEntry( 8400, pSocket.language ); // Spell currently being cast by character. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.spellCast ).toString() + "</BASEFONT>";
				break;
			case charProp.split:
				charLabelTooltip 	= GetDictionaryEntry( 8401, pSocket.language ); // Determines how many NPCs character will split into when hit in combat
				charValue 			= ( myTarget.split ).toString();
				break;
			case charProp.splitchance:
				charLabelTooltip 	= GetDictionaryEntry( 8402, pSocket.language ); // Chance of creature to split when hit in combat
				charValue 			= ( myTarget.splitchance ).toString();
				break;
			case charProp.squelch:
				charLabelTooltip 	= GetDictionaryEntry( 8403, pSocket.language ); // Toggles whether character is squelched/muted
				charValue 			= ( myTarget.squelch ).toString();
				break;
			case charProp.stabled:
				charLabelTooltip 	= GetDictionaryEntry( 8404, pSocket.language ); // Toggles whether creature is in a stable
				charValue 			= ( myTarget.stabled ).toString();
				break;
			case charProp.stamina:
				charLabelTooltip 	= GetDictionaryEntry( 8405, pSocket.language ); // Character's current stamina
				charValue 			= ( myTarget.stamina ).toString();
				break;
			case charProp.stealth:
				charLabelTooltip 	= GetDictionaryEntry( 8406, pSocket.language ); // Number of steps character has taken in stealth. Controlled by server (Read-Only)
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.stealth ).toString() + "</BASEFONT>";
				break;
			case charProp.strength:
				charLabelTooltip 	= GetDictionaryEntry( 8407, pSocket.language ); // Strength attribute of character
				charValue 			= ( myTarget.strength ).toString();
				break;
			case charProp.tamed:
				charLabelTooltip 	= GetDictionaryEntry( 8408, pSocket.language ); // Determines if a creature is tame
				charValue 			= ( myTarget.tamed ).toString();
				break;
			case charProp.tamedHungerRate:
				charLabelTooltip 	= GetDictionaryEntry( 8409, pSocket.language ); // The rate at which a pet grows hungry
				charValue 			= ( myTarget.tamedHungerRate ).toString();
				break;
			case charProp.tamedThirstRate:
				charLabelTooltip 	= GetDictionaryEntry( 8410, pSocket.language ); // The rate at which a pet grows thirsty
				charValue 			= ( myTarget.tamedThirstRate ).toString();
				break;
			case charProp.target:
				charLabelTooltip 	= GetDictionaryEntry( 8411, pSocket.language ); // Character's current target
				charValue 			= ( myTarget.hasOwnProperty( 'target' ) && myTarget.target != null ) ? ( myTarget.target ).toString() : "-";
				break;
			case charProp.tempdex:
				charLabelTooltip 	= GetDictionaryEntry( 8412, pSocket.language ); // Character's temporary dex, as affected by equipped items, spells and potions
				charValue 			= ( myTarget.tempdex ).toString();
				break;
			case charProp.tempint:
				charLabelTooltip 	= GetDictionaryEntry( 8413, pSocket.language ); // Character's temporary int, as affected by equipped items, spells and potions
				charValue 			= ( myTarget.tempint ).toString();
				break;
			case charProp.tempstr:
				charLabelTooltip 	= GetDictionaryEntry( 8414, pSocket.language ); // Character's temporary str, as affected by equipped items, spells and potions
				charValue 			= ( myTarget.tempstr ).toString();
				break;
			case charProp.thirst:
				charLabelTooltip 	= GetDictionaryEntry( 8415, pSocket.language ); // Character's current thirst status (0-6)
				charValue 			= ( myTarget.thirst ).toString();
				break;
			case charProp.thirstWildChance:
				charLabelTooltip 	= GetDictionaryEntry( 8416, pSocket.language ); // Chance for extremely thirsty pet to go wild with every NPC AI loop
				charValue 			= ( myTarget.thirstWildChance ).toString() + "%";
				break;
			case charProp.title:
				charLabelTooltip 	= GetDictionaryEntry( 8417, pSocket.language ); // The title of the character
				charValue 			= myTarget.hasOwnProperty( 'title' ) ? ( myTarget.title ).toString() : "-";
				break;
			case charProp.town:
				charLabelTooltip 	= GetDictionaryEntry( 8418, pSocket.language ); // The town the player belongs to (N/A)
				charValue 			= ( myTarget.hasOwnProperty( 'town' ) && myTarget.town ? ( myTarget.town ).toString() : "-" );
				charValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.hasOwnProperty( 'town' ) && myTarget.town ? ( myTarget.town ).toString() : "N/A" ) + "</BASEFONT>";
				charValueTooltip	= "<BASEFONT color=#EECD8B>" + ( myTarget.hasOwnProperty( 'town' ) && myTarget.town ? ( myTarget.town.name ).toString() : "N/A" ) + "</BASEFONT>";
				break;
			case charProp.townPriv:
				charLabelTooltip 	= GetDictionaryEntry( 8419, pSocket.language ); // The privileges the character has with their town (1 = Resident, 2 = Mayor)
				charValue 			= ( myTarget.townPriv ).toString();
				break;
			case charProp.trainer:
				charLabelTooltip 	= GetDictionaryEntry( 8420, pSocket.language ); // Toggles if NPC can train players in skills
				charValue 			= ( myTarget.trainer ).toString();
				break;
			case charProp.visible:
				charLabelTooltip 	= GetDictionaryEntry( 8421, pSocket.language ); // Determines visibility of character to other players (0 = Visible, 1 = Hidden, 2 = Magically Invisible, 3 = Visible to GMs only)
				charValue 			= ( myTarget.visible ).toString();
				break;
			case charProp.vulnerable:
				charLabelTooltip 	= GetDictionaryEntry( 8422, pSocket.language ); // Toggles vulnerability/invulnerability for character
				charValue 			= ( myTarget.vulnerable ).toString();
				break;
			case charProp.wandertype:
				charLabelTooltip 	= GetDictionaryEntry( 8423, pSocket.language ); // Determines wandertype for NPC (0 = None, 2 = Free, 3 = Box, 4 = Circle, 5 = Frozen)
				charValue 			= ( myTarget.wandertype ).toString();
				break;
			case charProp.weight:
				charLabelTooltip 	= GetDictionaryEntry( 8424, pSocket.language ); // Total weight of character (100 = 1.0 stone)
				charValue 			= ( myTarget.weight ).toString();
				break;
			case charProp.willhunger:
				charLabelTooltip 	= GetDictionaryEntry( 8425, pSocket.language ); // Enables (true) or disables (false) hunger-mode
				charValue 			= ( myTarget.willhunger ).toString();
				break;
			case charProp.willthirst:
				charLabelTooltip 	= GetDictionaryEntry( 8426, pSocket.language ); // Enables (true) or disables (false) thirst-mode
				charValue 			= ( myTarget.willthirst ).toString();
				break;
			case charProp.worldnumber:
				charLabelTooltip 	= GetDictionaryEntry( 8427, pSocket.language ); // World that character exists in
				charValue 			= ( myTarget.worldnumber ).toString();
				break;
			case charProp.x:
				charLabelTooltip 	= GetDictionaryEntry( 8428, pSocket.language ); // X coordinate of character in the world
				charValue 			= ( myTarget.x ).toString();
				break;
			case charProp.y:
				charLabelTooltip 	= GetDictionaryEntry( 8429, pSocket.language ); // Y coordinate of character in the world
				charValue 			= ( myTarget.y ).toString();
				break;
			case charProp.z:
				charLabelTooltip 	= GetDictionaryEntry( 8430, pSocket.language ); // Z coordinate of character in the world
				charValue 			= ( myTarget.z ).toString();
				break;
			default:
				errorFound = true;
				Console.PrintSectionBegin();
  				Console.Print( "Unhandled charProperty in tweak command script!" );
				Console.PrintDone();
				break;
		}

		if( !errorFound )
		{
			// Labels
			if( propertyName == "AccountNum" )
			{
				charGump.AddHTMLGump( 15, labelStartY, 100, 20, 0, 0, propertyLabelStart + "Account" + propertyLabelEnd );
			}
			else
			{
				charGump.AddHTMLGump( 15, labelStartY, 100, 20, 0, 0, propertyLabelStart + propertyName + propertyLabelEnd );
			}
			if( enableTooltips )
			{
				charGump.AddToolTip( tooltipClilocID, pSocket, charLabelTooltip.toString() );
			}

			if( charValue == "-" )
			{
				charValueTooltip = "Value not set";
			}

			switch( i + 200 )
			{
				// case charProp.accountNum:
				case charProp.attack:
				case charProp.ownedItemsCount:
				case charProp.petCount:
				case charProp.spellCast:
				case charProp.stealth:
				case charProp.isAnimal:
				case charProp.isHuman:
				case charProp.isMeditating:
				case charProp.dead:
				case charProp.housesOwned:
				case charProp.housesCoOwned:
				case charProp.isflying:
				case charProp.isJailed:
				case charProp.isonhorse:
				case charProp.nextAct:
				case charProp.npc:
				case charProp.online:
				case charProp.region:
				case charProp.skillLock:
				case charProp.skillsused:
				case charProp.town:
					break;
				case charProp.commandlevel:
				case charProp.isGM:
					if( myTarget.commandlevel >= pSocket.currentChar.commandlevel )
						break;
				case charProp.multi:
					if( !(( i + 200 ) == charProp.commandlevel) && !(( i + 200 ) == charProp.isGM ) && myTarget.multi == null )
						break;
				default:
					if((( i + 200 ) == charProp.accountNum) && myTarget.npc )
						break;
					charGump.AddButton( 120, buttonStartY, gumpMainButtonOff, gumpMainButtonOn, 1, 0, buttonID );
					if( enableTooltips )
					{
						charGump.AddToolTip( tooltipClilocID, pSocket, ( charValueTooltip != "" ? charValueTooltip : charValue ));
					}
					break;
			}

			// Values
			if( enableTooltips )
			{
				charGump.AddHTMLGump( 125, valueStartY, 105, 20, 0, 0, propertyValueStart + charValue + propertyValueEnd );
			}
			else
			{
				charGump.AddHTMLGump( 125, valueStartY, 105, 20, 0, 0, charValue );
			}

			switch( i + 200 )
			{
				// case charProp.accountNum:
				// case charProp.accountNum:
				case charProp.attack:
				case charProp.ownedItemsCount:
				case charProp.petCount:
				case charProp.spellCast:
				case charProp.stealth:
				case charProp.isAnimal:
				case charProp.isHuman:
				case charProp.isMeditating:
				case charProp.dead:
				case charProp.housesOwned:
				case charProp.housesCoOwned:
				case charProp.commandlevel:
				case charProp.isGM:
				case charProp.isflying:
				case charProp.isJailed:
				case charProp.isonhorse:
				case charProp.nextAct:
				case charProp.npc:
				case charProp.online:
				case charProp.region:
				case charProp.skillLock:
				case charProp.skillsused:
				case charProp.town:
					if(( propertyName != "Commandlevel" && propertyName != "IsGM" ) || ( myTarget.commandlevel >= pSocket.currentChar.commandlevel ))
					{
						if( enableTooltips )
						{
							charGump.AddToolTip( tooltipClilocID, pSocket, ( charValueTooltip != "" ? charValueTooltip : charValue ) + " (Read-Only)" );
						}
					}
					break;
				case charProp.multi:
					if( myTarget.multi == null && enableTooltips )
					{
						charGump.AddToolTip( tooltipClilocID, pSocket, ( charValueTooltip != "" ? charValueTooltip : charValue ));
					}
					break;
				default:
					break;
			}
		}

		if( gumpPage == 1 )
		{
			pageOneLabelStartY += 20;
			pageOneButtonStartY += 20;
			pageOneValueStartY += 20;
		}
		else
		{
			pageXlabelStartY += 20;
			pageXbuttonStartY += 20;
			pageXvalueStartY += 20;
		}

		buttonID++;
	}

	charGump.Send( pSocket );
	charGump.Free();
}

// Handle properties of character skills
function HandleSkillGump( pSocket, myTarget, baseSkills )
{
	var skillGump = new Gump;
	skillGump = RenderZeroethPage( pSocket, skillGump, myTarget, true, baseSkills );

	var propertyName;
	var buttonID = 400;
	var gumpPage = 1;
	var totalPages = Math.ceil(( 1 + charSkillCount) / 17 );

	// First page
	var pageOneLabelStartY = 59;
	var pageOneButtonStartY = 60;
	var pageOneValueStartY = 59;
	// Subsequent pages
	var pageXlabelStartY = 59;
	var pageXbuttonStartY = 60;
	var pageXvalueStartY = 59;

	// Loop over all character skills
	var i = 0;
	for( i = 0; i < charSkillCount; i++ )
	{
		if( i <= 16 )
		{
			if( i == 0 )
			{
				gumpPage = 1;
				pageXlabelStartY = 59;
				pageXbuttonStartY = 60;
				pageXvalueStartY = 59;
				skillGump.AddPage( gumpPage );
				skillGump.AddBackground( 112, 55, gumpSecondaryBackgroundWidth, 350, gumpSecondaryBackground ); // Tile White Background

				// Next/Prev buttons
				skillGump.AddHTMLGump( 100, 420, 80, 20, 0, 0, "<BASEFONT color=#EECD8B>Page 1/" + totalPages + "</BASEFONT>" );
				skillGump.AddButton( 210, 420, gumpNextButtonOff, gumpNextButtonOn, 0, 2, 0 );
				if( enableTooltips )
				{
					skillGump.AddToolTip( tooltipClilocID, pSocket, GetDictionaryEntry( 8123, pSocket.language )); // Next page
				}
			}
		}
		else // All other pages
		{
			switch( i )
			{
				case 17:
					gumpPage = 2;
					break;
				case 34:
					gumpPage = 3;
					break;
				case 51:
					gumpPage = 4;
					break;
				default:
					break;
			}

			// Only add these when it's time for a new page
			if( i == 17 || i == 34 || i == 51 )
			{
				pageXlabelStartY = 59;
				pageXbuttonStartY = 60;
				pageXvalueStartY = 59;
				skillGump.AddPage( gumpPage );
				skillGump.AddBackground( 112, 55, gumpSecondaryBackgroundWidth, 350, gumpSecondaryBackground ); // Tile White Background

				skillGump.AddHTMLGump( 100, 420, 80, 20, 0, 0, "<BASEFONT color=#EECD8B>Page " + gumpPage + "/" + totalPages + "</BASEFONT>" );

				if( gumpPage < 4 )
				{
					// Add next page button for all subsequent pages except last one
					skillGump.AddButton( 210, 420, gumpNextButtonOff, gumpNextButtonOn, 0, gumpPage + 1, 0 );
					if( enableTooltips )
					{
						skillGump.AddToolTip( tooltipClilocID, pSocket, GetDictionaryEntry( 8123, pSocket.language )); // Next page
					}
				}

				// Add previous page button for all subsequent pages
				skillGump.AddButton( 14, 420, gumpPrevButtonOff, gumpPrevButtonOn, 0, gumpPage - 1, 0 );
				if( enableTooltips )
				{
					skillGump.AddToolTip( tooltipClilocID,  GetDictionaryEntry( 8124, pSocket.language )); // Previous page
				}
			}
		}

		var labelStartY = 0;
		var buttonStartY = 0;
		var valueStartY = 0;
		if( gumpPage == 1 )
		{
			labelStartY = pageOneLabelStartY;
			buttonStartY = pageOneButtonStartY;
			valueStartY = pageOneValueStartY;
		}
		else
		{
			labelStartY = pageXlabelStartY;
			buttonStartY = pageXbuttonStartY;
			valueStartY = pageXvalueStartY;
		}

		var index = 20;
		for( var k in charSkills )
		{
		    if( charSkills.hasOwnProperty( k ) && index == i + 20 )
		    {
		        // k is key
				propertyName = k.charAt( 0 ).toUpperCase() + k.slice( 1 );
		        break;
		    }
		    index++;
		}

		var errorFound = false;
		var charSkillsToolTip = "";
		var charSkillsValue = "";
		var charSkillsValueTooltip = "";
		if( baseSkills )
		{
			charSkillsValueTooltip = ( myTarget.skills[propertyName.toLowerCase()] / 10 ).toFixed( 1 ).toString() + " effective skill after bonuses";
		}
		else
		{
			charSkillsValueTooltip = ( myTarget.baseskills[propertyName.toLowerCase()] / 10 ).toFixed( 1 ).toString() + " base skill before bonuses";
		}

		switch( i + 400 )
		{
			case charSkills.alchemy:
				charSkillsToolTip = "Skill 0 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.alchemy : myTarget.skills.alchemy ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.anatomy:
				charSkillsToolTip = "Skill 1 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.anatomy : myTarget.skills.anatomy ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.animallore:
				charSkillsToolTip = "Skill 2 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.animallore : myTarget.skills.animallore ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.itemid:
				charSkillsToolTip = "Skill 3 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.itemid : myTarget.skills.itemid) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.armslore:
				charSkillsToolTip = "Skill 4 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.armslore : myTarget.skills.armslore) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.parrying:
				charSkillsToolTip = "Skill 5 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.parrying : myTarget.skills.parrying) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.begging:
				charSkillsToolTip = "Skill 6 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.begging : myTarget.skills.begging ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.blacksmithing:
				charSkillsToolTip = "Skill 7 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.blacksmithing : myTarget.skills.blacksmithing ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.bowcraft:
				charSkillsToolTip = "Skill 8 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.bowcraft : myTarget.skills.bowcraft ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.peacemaking:
				charSkillsToolTip = "Skill 9 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.peacemaking : myTarget.skills.peacemaking)/10).toFixed( 1 ).toString();
				break;
			case charSkills.camping:
				charSkillsToolTip = "Skill 10 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.camping : myTarget.skills.camping ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.carpentry:
				charSkillsToolTip = "Skill 11 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.carpentry : myTarget.skills.carpentry ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.cartography:
				charSkillsToolTip = "Skill 12 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.cartography : myTarget.skills.cartography ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.cooking:
				charSkillsToolTip = "Skill 13 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.cooking : myTarget.skills.cooking ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.detectinghidden:
				charSkillsToolTip = "Skill 14 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.detectinghidden : myTarget.skills.detectinghidden ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.enticement:
				charSkillsToolTip = "Skill 15 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.enticement : myTarget.skills.enticement ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.evaluatingintel:
				charSkillsToolTip = "Skill 16 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.evaluatingintel : myTarget.skills.evaluatingintel ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.healing:
				charSkillsToolTip = "Skill 17 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.healing : myTarget.skills.healing ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.fishing:
				charSkillsToolTip = "Skill 18 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.fishing : myTarget.skills.fishing ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.forensics:
				charSkillsToolTip = "Skill 19 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.forensics : myTarget.skills.forensics ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.herding:
				charSkillsToolTip = "Skill 20 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.herding : myTarget.skills.herding ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.hiding:
				charSkillsToolTip = "Skill 21 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.hiding : myTarget.skills.hiding ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.provocation:
				charSkillsToolTip = "Skill 22 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.provocation : myTarget.skills.provocation ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.inscription:
				charSkillsToolTip = "Skill 23 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.inscription : myTarget.skills.inscription ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.lockpicking:
				charSkillsToolTip = "Skill 24 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.lockpicking : myTarget.skills.lockpicking ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.magery:
				charSkillsToolTip = "Skill 25 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.magery : myTarget.skills.magery ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.magicresistance:
				charSkillsToolTip = "Skill 26 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.magicresistance : myTarget.skills.magicresistance ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.tactics:
				charSkillsToolTip = "Skill 27 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.tactics : myTarget.skills.tactics ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.snooping:
				charSkillsToolTip = "Skill 28 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.snooping : myTarget.skills.snooping ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.musicianship:
				charSkillsToolTip = "Skill 29 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.musicianship : myTarget.skills.musicianship ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.poisoning:
				charSkillsToolTip = "Skill 30 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.poisoning : myTarget.skills.poisoning ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.archery:
				charSkillsToolTip = "Skill 31 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.archery : myTarget.skills.archery ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.spiritspeak:
				charSkillsToolTip = "Skill 32 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.spiritspeak : myTarget.skills.spiritspeak ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.stealing:
				charSkillsToolTip = "Skill 33 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.stealing : myTarget.skills.stealing ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.tailoring:
				charSkillsToolTip = "Skill 34 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.tailoring : myTarget.skills.tailoring ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.taming:
				charSkillsToolTip = "Skill 35 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.taming : myTarget.skills.taming ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.tasteid:
				charSkillsToolTip = "Skill 36 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.tasteid : myTarget.skills.tasteid ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.tinkering:
				charSkillsToolTip = "Skill 37 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.tinkering : myTarget.skills.tinkering ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.tracking:
				charSkillsToolTip = "Skill 38 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.tracking : myTarget.skills.tracking ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.veterinary:
				charSkillsToolTip = "Skill 39 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.veterinary : myTarget.skills.veterinary ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.swordsmanship:
				charSkillsToolTip = "Skill 40 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.swordsmanship : myTarget.skills.swordsmanship ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.macefighting:
				charSkillsToolTip = "Skill 41 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.macefighting : myTarget.skills.macefighting ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.fencing:
				charSkillsToolTip = "Skill 42 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.fencing : myTarget.skills.fencing ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.wrestling:
				charSkillsToolTip = "Skill 43 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.wrestling : myTarget.skills.wrestling ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.lumberjacking:
				charSkillsToolTip = "Skill 44 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.lumberjacking : myTarget.skills.lumberjacking ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.mining:
				charSkillsToolTip = "Skill 45 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.mining : myTarget.skills.mining ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.meditation:
				charSkillsToolTip = "Skill 46 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.meditation : myTarget.skills.meditation ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.stealth:
				charSkillsToolTip = "Skill 47 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.stealth : myTarget.skills.stealth ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.removetrap:
				charSkillsToolTip = "Skill 48 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.removetrap : myTarget.skills.removetrap ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.necromancy:
				charSkillsToolTip = "Skill 49 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.necromancy : myTarget.skills.necromancy ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.focus:
				charSkillsToolTip = "Skill 50 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.focus : myTarget.skills.focus ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.chivalry:
				charSkillsToolTip = "Skill 51 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.chivalry : myTarget.skills.chivalry ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.bushido:
				charSkillsToolTip = "Skill 52 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.bushido : myTarget.skills.bushido ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.ninjitsu:
				charSkillsToolTip = "Skill 53 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.ninjitsu : myTarget.skills.ninjitsu ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.spellweaving:
				charSkillsToolTip = "Skill 54 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.spellweaving : myTarget.skills.spellweaving ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.imbuing:
				charSkillsToolTip = "Skill 55 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.imbuing : myTarget.skills.imbuing ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.mysticism:
				charSkillsToolTip = "Skill 56 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.mysticism : myTarget.skills.mysticism ) / 10 ).toFixed( 1 ).toString();
				break;
			case charSkills.throwing:
				charSkillsToolTip = "Skill 57 - " + propertyName;
				charSkillsValue = (( baseSkills ? myTarget.baseskills.throwing : myTarget.skills.throwing ) / 10 ).toFixed( 1 ).toString();
				break;
			default:
				errorFound = true;
				Console.PrintSectionBegin();
  				Console.Print( "Unhandled character skill in tweak command script!" );
				Console.PrintDone();
				break;
		}

		if( !errorFound )
		{
			// Labels
			skillGump.AddHTMLGump( 15, labelStartY, 100, 20, 0, 0, propertyLabelStart + propertyName + propertyLabelEnd );
			if( enableTooltips )
			{
				skillGump.AddToolTip( tooltipClilocID, pSocket, charSkillsToolTip.toString() );
			}

			// Buttons
			skillGump.AddButton( 120, buttonStartY, gumpMainButtonOff, gumpMainButtonOn, 1, 0, buttonID);
			if( enableTooltips )
			{
				skillGump.AddToolTip( tooltipClilocID, pSocket, ( charSkillsValueTooltip != "" ? charSkillsValueTooltip : charSkillsValue ));
			}

			// Values
			skillGump.AddHTMLGump( 125, valueStartY, 105, 20, 0, 0, propertyValueStart + charSkillsValue + propertyValueEnd );
		}

		if( gumpPage == 1 )
		{
			pageOneLabelStartY += 20;
			pageOneButtonStartY += 20;
			pageOneValueStartY += 20;
		}
		else
		{
			pageXlabelStartY += 20;
			pageXbuttonStartY += 20;
			pageXvalueStartY += 20;
		}

		buttonID++;
	}

	skillGump.Send( pSocket );
	skillGump.Free();
}

// Handle properties of Multis
function HandleMultiTarget( pSocket, myTarget )
{
	var multiGump = new Gump;
	multiGump = RenderZeroethPage( pSocket, multiGump, myTarget, false, false );

	var propertyName;
	var buttonID = 500;
	var gumpPage = 1;
	var totalPages = Math.ceil( 1 + ( multiPropCount - 13 ) / 17 );

	// First page
	var pageOneLabelStartY = 139;
	var pageOneButtonStartY = 140;
	var pageOneValueStartY = 139;
	// Subsequent pages
	var pageXlabelStartY = 59;
	var pageXbuttonStartY = 60;
	var pageXvalueStartY = 59;

	// Loop over all character properties
	var i = 0;
	for( i = 0; i < multiPropCount; i++ )
	{
		if( i == 0 )
		{
			// Page 1
			multiGump = RenderFirstPage( pSocket, multiGump, myTarget, "Multi", multiPropCount, totalPages );
		}
		else // All other pages
		{
			// 17 options can fit on each page, so increase page number as needed
			switch( i )
			{
				case 13:
					gumpPage = 2;
					break;
				case 30:
					gumpPage = 3;
					break;
				default:
					break;
			}

			// Only add these when it's time for a new page
			if( i == 13 || i == 30 )
			{
				pageXlabelStartY = 59;
				pageXbuttonStartY = 60;
				pageXvalueStartY = 59;
				multiGump = RenderOtherPages( pSocket, multiGump, gumpPage, totalPages );
			}
		}

		var labelStartY = 0;
		var buttonStartY = 0;
		var valueStartY = 0;
		if( gumpPage == 1 )
		{
			labelStartY = pageOneLabelStartY;
			buttonStartY = pageOneButtonStartY;
			valueStartY = pageOneValueStartY;
		}
		else
		{
			labelStartY = pageXlabelStartY;
			buttonStartY = pageXbuttonStartY;
			valueStartY = pageXvalueStartY;
		}

		var index = 20;
		for( var k in multiProp )
		{
		    if( multiProp.hasOwnProperty( k ) && index == i + 20 )
		    {
		        // k is key
				propertyName = k.charAt( 0 ).toUpperCase() + k.slice( 1 );
		        break;
		    }
		    index++;
		}

		var multiLabelTooltip = "";
		var multiValue = "";
		var multiValueTooltip = "";
		var errorFound = false;

		switch( i + 500 )
		{
			case multiProp.bans:
				multiLabelTooltip 	= GetDictionaryEntry( 8500, pSocket.language ); // Current number of players banned from Multi. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.bans != null ? myTarget.bans : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.banX:
				multiLabelTooltip 	= GetDictionaryEntry( 8501, pSocket.language ); // X coordinate for Multi's ban-location
				multiValue 			= ( myTarget.banX != null ? myTarget.banX : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.banY:
				multiLabelTooltip 	= GetDictionaryEntry( 8502, pSocket.language ); // Y coordinate for Multi's ban-location
				multiValue 			= ( myTarget.banY != null ? myTarget.banY : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.buildTimestamp:
				multiLabelTooltip 	= GetDictionaryEntry( 8503, pSocket.language ); // Timestamp for when Multi was created. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.buildTimestamp != null ? myTarget.buildTimestamp : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.colour:
				multiLabelTooltip 	= GetDictionaryEntry( 8504, pSocket.language ); // Colour of Multi
				multiValue 			= "0x" + ( myTarget.colour).toString( 16 );
				break;
			case multiProp.deed:
				multiLabelTooltip 	= GetDictionaryEntry( 8505, pSocket.language ); // Section Item ID for deed used to build Multi
				multiValue 			= ( myTarget.deed != null ? myTarget.deed : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.dir:
				multiLabelTooltip 	= GetDictionaryEntry( 8506, pSocket.language ); // Direction of Multi (Does this do anything for multis? Maybe boats?)
				multiValue 			= ( myTarget.dir).toString();
				break;
			case multiProp.friends:
				multiLabelTooltip 	= GetDictionaryEntry( 8507, pSocket.language ); // Amount of friends added to Multi. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.friends != null ? myTarget.friends : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.guests:
				multiLabelTooltip 	= GetDictionaryEntry( 8508, pSocket.language ); // Amount of guests added to Multi. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.guests != null ? myTarget.guests : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.isPublic:
				multiLabelTooltip 	= GetDictionaryEntry( 8509, pSocket.language ); // Is the Multi flagged as Public?
				multiValue 			= ( myTarget.isPublic ? "true" : "false" );
				break;
			case multiProp.instanceID:
				multiLabelTooltip 	= GetDictionaryEntry( 8510, pSocket.language ); // ID of instance of world that Multi exists in. Objects in different instances will not be able to interact with one another!
				multiValue 			= ( myTarget.instanceID ).toString();
				break;
			case multiProp.lockdowns:
				multiLabelTooltip 	= GetDictionaryEntry( 8511, pSocket.language ); // Amount of locked down items in Multi. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.lockdowns != null ? myTarget.lockdowns : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.maxBans:
				multiLabelTooltip 	= GetDictionaryEntry( 8512, pSocket.language ); // Maximum amount of bans supported by Multi
				multiValue 			= ( myTarget.maxBans != null ? myTarget.maxBans : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.maxFriends:
				multiLabelTooltip 	= GetDictionaryEntry( 8513, pSocket.language ); // Maximum amount of friends supported by Multi
				multiValue 			= ( myTarget.maxFriends != null ? myTarget.maxFriends : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.maxGuests:
				multiLabelTooltip 	= GetDictionaryEntry( 8514, pSocket.language ); // Maximum amount of guests supported by Multi
				multiValue 			= ( myTarget.maxGuests != null ? myTarget.maxGuests : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.maxLockdowns:
				multiLabelTooltip 	= GetDictionaryEntry( 8515, pSocket.language ); // Maximum amount of lockdowns supported by Multi
				multiValue 			= ( myTarget.maxLockdowns != null ? myTarget.maxLockdowns : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.maxOwners:
				multiLabelTooltip 	= GetDictionaryEntry( 8516, pSocket.language ); // Maximum amount of owners supported by Multi
				multiValue 			= ( myTarget.maxOwners != null ? myTarget.maxOwners : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.maxSecureContainers:
				multiLabelTooltip 	= GetDictionaryEntry( 8517, pSocket.language ); // Maximum amount of secure containers supported by Multi
				multiValue 			= ( myTarget.maxSecureContainers != null ? myTarget.maxSecureContainers : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.maxTrashContainers:
				multiLabelTooltip 	= GetDictionaryEntry( 8518, pSocket.language ); // Maximum amount of trash containers supported by Multi
				multiValue 			= ( myTarget.maxTrashContainers != null ? myTarget.maxTrashContainers : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.maxVendors:
				multiLabelTooltip 	= GetDictionaryEntry( 8519, pSocket.language ); // Maximum amount of vendors supported by Multi
				multiValue 			= ( myTarget.maxVendors != null ? myTarget.maxVendors : '<BASEFONT color=#EECD8B>N/A</BASEFONT>' ).toString();
				break;
			case multiProp.owner:
				multiLabelTooltip 	= GetDictionaryEntry( 8520, pSocket.language ); // Object registered as owner of this Multi
				multiValue 			= ( ValidateObject( myTarget.owner ) ? ( myTarget.owner.name ).toString() : "-" );
				multiValueTooltip	= ( ValidateObject( myTarget.owner ) ? ( myTarget.owner.name ).toString() + " ( " + ( myTarget.owner.serial).toString() + " )": "-" );
				break;
			case multiProp.owners:
				multiLabelTooltip 	= GetDictionaryEntry( 8521, pSocket.language ); // Current amount of owners of Multi. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.owners != null ? myTarget.owners : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.scripttrigger:
				multiLabelTooltip 	= GetDictionaryEntry( 8522, pSocket.language ); // JS Script assigned to Multi
				multiValue 			= ( myTarget.scripttrigger ).toString();
				break;
			case multiProp.secureContainers:
				multiLabelTooltip 	= GetDictionaryEntry( 8523, pSocket.language ); // Current amount of secure containers in Multi. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.secureContainers != null ? myTarget.secureContainers : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.tradeTimestamp:
				multiLabelTooltip 	= GetDictionaryEntry( 8524, pSocket.language ); // Timestamp for last time Multi was traded. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.tradeTimestamp != null ? myTarget.tradeTimestamp : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.trashContainers:
				multiLabelTooltip 	= GetDictionaryEntry( 8525, pSocket.language ); // Current amount of trash containers in Multi. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.trashContainers != null ? myTarget.trashContainers : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.vendors:
				multiLabelTooltip 	= GetDictionaryEntry( 8526, pSocket.language ); // Current amount of vendors in Multi. Controlled by Server (Read-Only)
				multiValue 			= "<BASEFONT color=#EECD8B>" + ( myTarget.vendors != null ? myTarget.vendors : 'N/A' ).toString() + "</BASEFONT>";
				break;
			case multiProp.visible:
				multiLabelTooltip 	= GetDictionaryEntry( 8527, pSocket.language ); // Determines who Multi is visible for (0 = Visible, 1 = Hidden, 2 = Magically Invisible, 3 = Visible to GMs only)
				multiValue 			= ( myTarget.visible ).toString();
				break;
			case multiProp.worldnumber:
				multiLabelTooltip 	= GetDictionaryEntry( 8528, pSocket.language ); // World that Multi exists in
				multiValue 			= ( myTarget.worldnumber ).toString();
				break;
			case multiProp.x:
				multiLabelTooltip 	= GetDictionaryEntry( 8529, pSocket.language ); // X coordinate of Multi in the world
				multiValue 			= ( myTarget.x ).toString();
				break;
			case multiProp.y:
				multiLabelTooltip 	= GetDictionaryEntry( 8530, pSocket.language ); // Y coordinate of Multi in the world
				multiValue 			= ( myTarget.y ).toString();
				break;
			case multiProp.z:
				multiLabelTooltip 	= GetDictionaryEntry( 8531, pSocket.language ); // Z coordinate of Multi in the world
				multiValue 			= ( myTarget.z ).toString();
				break;
			default:
				errorFound = true;
				Console.PrintSectionBegin();
  				Console.Print( "Unhandled multi property in tweak command script!" );
				Console.PrintDone();
				break;
		}

		if( !errorFound )
		{
			// Labels
			multiGump.AddHTMLGump( 15, labelStartY, 100, 20, 0, 0, propertyLabelStart + propertyName + propertyLabelEnd );
			if( enableTooltips )
			{
				multiGump.AddToolTip( tooltipClilocID, pSocket, multiLabelTooltip.toString() );
			}

			if( multiValue == "-" )
			{
				multiValueTooltip = "Value not set";
			}

			switch( i + 500 )
			{
				case multiProp.bans:
				case multiProp.buildTimestamp:
				case multiProp.friends:
				case multiProp.guests:
				case multiProp.lockdowns:
				case multiProp.owners:
				case multiProp.secureContainers:
				case multiProp.tradeTimestamp:
				case multiProp.trashContainers:
				case multiProp.vendors:
					break;
				case multiProp.banX:
				case multiProp.banY:
				case multiProp.deed:
				case multiProp.maxBans:
				case multiProp.maxFriends:
				case multiProp.maxOwners:
				case multiProp.maxTrashContainers:
				case multiProp.maxLockdowns:
				case multiProp.maxSecureContainers:
				case multiProp.maxVendors:
				case multiProp.maxGuests:
					if( myTarget.IsBoat() )
						break;
				default:
					multiGump.AddButton( 120, buttonStartY, gumpMainButtonOff, gumpMainButtonOn, 1, 0, buttonID);
					if( enableTooltips )
					{
						multiGump.AddToolTip( tooltipClilocID, pSocket, ( multiValueTooltip != "" ? multiValueTooltip : multiValue ));
					}
					break;
			}

			// Values
			multiGump.AddHTMLGump( 125, valueStartY, 105, 20, 0, 0, propertyValueStart + multiValue + propertyValueEnd );

			switch( i + 500 )
			{
				case multiProp.bans:
				case multiProp.buildTimestamp:
				case multiProp.friends:
				case multiProp.guests:
				case multiProp.lockdowns:
				case multiProp.owners:
				case multiProp.secureContainers:
				case multiProp.tradeTimestamp:
				case multiProp.trashContainers:
				case multiProp.vendors:
					if( enableTooltips )
					{
						multiGump.AddToolTip( tooltipClilocID, pSocket, ( multiValueTooltip != "" ? multiValueTooltip : multiValue ) + " (Read-Only)" );
					}
					break;
				default:
					break;
			}
		}

		if( gumpPage == 1 )
		{
			pageOneLabelStartY += 20;
			pageOneButtonStartY += 20;
			pageOneValueStartY += 20;
		}
		else
		{
			pageXlabelStartY += 20;
			pageXbuttonStartY += 20;
			pageXvalueStartY += 20;
		}

		buttonID++;
	}

	multiGump.Send( pSocket );
	multiGump.Free();
}

// Handle properties of region targets
function HandleRegionTarget( pSocket, myTarget )
{
	var regionGump = new Gump;
	regionGump = RenderZeroethPage( pSocket, regionGump, myTarget, false, false );

	var propertyName;
	var buttonID = 700;
	var gumpPage = 1;
	var totalPages = Math.ceil( 1 + ( regionPropCount - 13 ) / 17 );

	// First page
	var pageOneLabelStartY = 139;
	var pageOneButtonStartY = 140;
	var pageOneValueStartY = 139;
	// Subsequent pages
	var pageXlabelStartY = 59;
	var pageXbuttonStartY = 60;
	var pageXvalueStartY = 59;

	// Loop over all character properties
	var i = 0;
	for( i = 0; i < regionPropCount; i++ )
	{
		if( i == 0 )
		{
			// Page 1
			regionGump = RenderFirstPage( pSocket, regionGump, myTarget, "Region", regionPropCount, totalPages );
		}
		else // All other pages
		{
			// 17 options can fit on each page, so increase page number as needed
			switch( i )
			{
				case 13:
					gumpPage = 2;
					break;
				case 30:
					gumpPage = 3;
					break;
			}

			// Only add these when it's time for a new page
			if( i == 13 || i == 30 )
			{
				pageXlabelStartY = 59;
				pageXbuttonStartY = 60;
				pageXvalueStartY = 59;
				regionGump = RenderOtherPages( pSocket, regionGump, gumpPage, totalPages );
			}
		}

		var labelStartY = 0;
		var buttonStartY = 0;
		var valueStartY = 0;
		if( gumpPage == 1 )
		{
			labelStartY = pageOneLabelStartY;
			buttonStartY = pageOneButtonStartY;
			valueStartY = pageOneValueStartY;
		}
		else
		{
			labelStartY = pageXlabelStartY;
			buttonStartY = pageXbuttonStartY;
			valueStartY = pageXvalueStartY;
		}

		var index = 20;
		for( var k in regionProp )
		{
		    if( regionProp.hasOwnProperty( k ) && index == i + 20 )
		    {
		        // k is key
				propertyName = k.charAt( 0 ).toUpperCase() + k.slice( 1 );
		        break;
		    }
		    index++;
		}

		var regionLabel = propertyName;
		var regionLabelTooltip = "";
		var regionValue = "";
		var regionValueTooltip = "";
		var errorFound = false;
		switch( i + 700 )
		{
			case regionProp.appearance:
			{
				regionLabelTooltip 	= GetDictionaryEntry( 8626, pSocket.language ) + " (0 = Spring, 1 = Summer, 2 = Autumn, 3 = Winter, 4 = Desolation, 5 = Unknown)"; // Appearance of world within the Region
				regionValue 		= ( myTarget.appearance ).toString();
				var appearanceTooltip = "Unknown";
				switch( myTarget.appearance )
				{
					case 0: appearanceTooltip = "Spring"; break;
					case 1: appearanceTooltip = "Summer"; break;
					case 2: appearanceTooltip = "Autumn"; break;
					case 3: appearanceTooltip = "Winter"; break;
					case 4: appearanceTooltip = "Desolation"; break;
					case 5: appearanceTooltip = "Unknown"; break;
					default: break;
				}
				regionValueTooltip 	= ( myTarget.appearance ).toString() + " ( " + appearanceTooltip + " )";
				break;
			}
			case regionProp.canCastAggressive:
				regionLabelTooltip 	= GetDictionaryEntry( 8600, pSocket.language ); // Can aggressive spells be cast in the Region?
				regionValue 		= ( myTarget.canCastAggressive ? "true" : "false" );
				break;
			case regionProp.canGate:
				regionLabelTooltip 	= GetDictionaryEntry( 8601, pSocket.language ); // Can the Gate spell be used in the Region?
				regionValue 		= ( myTarget.canGate ? "true" : "false" );
				break;
			case regionProp.canMark:
				regionLabelTooltip 	= GetDictionaryEntry( 8602, pSocket.language ); // Can the Mark spell be used in the Region?
				regionValue 		= ( myTarget.canMark ? "true" : "false" );
				break;
			case regionProp.canPlaceHouse:
				regionLabelTooltip 	= GetDictionaryEntry( 8603, pSocket.language ); // Can players place houses in the Region?
				regionValue 		= ( myTarget.canPlaceHouse ? "true" : "false" );
				break;
			case regionProp.canRecall:
				regionLabelTooltip 	= GetDictionaryEntry( 8604, pSocket.language ); // Can the Recall spell be used in the Region?
				regionValue 		= ( myTarget.canRecall ? "true" : "false" );
				break;
			case regionProp.canTeleport:
				regionLabelTooltip 	= GetDictionaryEntry( 8605, pSocket.language ); // Can the Teleport spell be used in the Region?
				regionValue 		= ( myTarget.canTeleport ? "true" : "false" );
				break;
			case regionProp.chanceBigOre:
				regionLabelTooltip 	= GetDictionaryEntry( 8606, pSocket.language ); // Chance to find big ore in the Region
				regionValue 		= ( myTarget.chanceBigOre ).toString() + "%";
				break;
			case regionProp.health:
				regionLabelTooltip 	= GetDictionaryEntry( 8607, pSocket.language ); // Health of Townstone in Region
				regionValue 		= ( myTarget.health ).toString();
				break;
			case regionProp.instanceID:
				regionLabelTooltip 	= GetDictionaryEntry( 8608, pSocket.language ); // Instance ID Region exists in
				regionValue 		= ( myTarget.instanceID ).toString();
				break;
			case regionProp.isDungeon:
				regionLabelTooltip 	= GetDictionaryEntry( 8609, pSocket.language ); // Is Region considered a dungeon?
				regionValue 		= ( myTarget.isDungeon ? "true" : "false" );
				break;
			case regionProp.isGuarded:
				regionLabelTooltip 	= GetDictionaryEntry( 8610, pSocket.language ); // Is Region protected by guards?
				regionValue 		= ( myTarget.isGuarded ? "true" : "false" );
				break;
			case regionProp.isSafeZone:
				regionLabelTooltip 	= GetDictionaryEntry( 8611, pSocket.language ); // Is Region considered safe for players?
				regionValue 		= ( myTarget.isSafeZone ? "true" : "false" );
				break;
			case regionProp.mayor:
				regionLabelTooltip 	= GetDictionaryEntry( 8612, pSocket.language ); // Character voted as Mayor of the Town in this Region (if any)
				regionValue 		= ( ValidateObject( myTarget.mayor ) ? ( myTarget.mayor.name ).toString() : "-" );
				regionValueTooltip	= ( ValidateObject( myTarget.mayor ) ? ( myTarget.mayor.name ).toString() + " ( " + ( myTarget.mayor.serial).toString() + " )": "-" );
				break;
			case regionProp.music:
				regionLabelTooltip 	= GetDictionaryEntry( 8613, pSocket.language ); // Music-list for Region, as specified in regions.dfn
				regionValue 		= ( myTarget.music ).toString();
				break;
			case regionProp.numGuards:
				regionLabelTooltip 	= GetDictionaryEntry( 8614, pSocket.language ); // The number of guards in the Region
				regionValue 		= ( myTarget.numGuards ).toString();
				break;
			case regionProp.numOrePrefs:
				regionLabelTooltip 	= GetDictionaryEntry( 8615, pSocket.language ); // The number of ore preferences present for the Region. Controlled by Server (Read-Only)
				regionValue 		= "<BASEFONT color=#EECD8B>" + ( myTarget.numOrePrefs ).toString() + "</BASEFONT>";
				break;
			case regionProp.owner:
				regionLabel 		= "Owner of Guards";
				regionLabelTooltip 	= GetDictionaryEntry( 8616, pSocket.language ); // The owner of guards in the Region
				regionValue 		= ( myTarget.owner ).toString();
				break;
			case regionProp.population:
				regionLabelTooltip 	= GetDictionaryEntry( 8617, pSocket.language ); // Number of players registered as citizens of the town (if any) in this Region. Controlled by server (Read-Only)
				regionValue 		= "<BASEFONT color=#EECD8B>" + ( myTarget.population ).toString() + "</BASEFONT>";
				break;
			case regionProp.race:
				regionLabelTooltip 	= GetDictionaryEntry( 8618, pSocket.language ); // Race associated with this Region
				regionValue 		= ( myTarget.race ).toString();
				break;
			case regionProp.reserves:
				regionLabelTooltip 	= GetDictionaryEntry( 8619, pSocket.language ); // Resource reserves for town (if any) in this Region
				regionValue 		= ( myTarget.reserves ).toString();
				break;
			case regionProp.scriptTrigger:
				regionLabelTooltip 	= GetDictionaryEntry( 8620, pSocket.language ); // Script-trigger assigned to the Region (if any)
				regionValue 		= ( myTarget.scriptTrigger ).toString();
				break;
			case regionProp.tax:
				regionLabelTooltip 	= GetDictionaryEntry( 8621, pSocket.language ); // The amount of gold taxed from citizens of the town (if any) in this Region
				regionValue 		= ( myTarget.tax ).toString();
				break;
			case regionProp.taxes:
				regionLabelTooltip 	= GetDictionaryEntry( 8622, pSocket.language ); // The gold reserves of the town (if any) in this Region
				regionValue 		= ( myTarget.taxes ).toString();
				break;
			case regionProp.taxResource:
				regionLabelTooltip 	= GetDictionaryEntry( 8623, pSocket.language ); // ID of resource taxed from citizens of the town (if any) in this Region
				regionValue 		= "0x" + ( myTarget.taxResource ).toString( 16 );
				break;
			case regionProp.weather:
				regionLabelTooltip 	= GetDictionaryEntry( 8624, pSocket.language ); // Weather ID from weather.dfn associated with this Region
				regionValue 		= ( myTarget.weather ).toString();
				break;
			case regionProp.worldNumber:
				regionLabelTooltip 	= GetDictionaryEntry( 8625, pSocket.language ); // World number that Region exists in
				regionValue 		= ( myTarget.worldNumber ).toString();
				break;
			default:
				errorFound = true;
				Console.PrintSectionBegin();
  				Console.Print( "Unhandled region property in tweak command script!" );
				Console.PrintDone();
				break;
		}

		if( !errorFound )
		{
			// Labels
			regionGump.AddHTMLGump( 15, labelStartY, 100, 20, 0, 0, propertyLabelStart + regionLabel + propertyLabelEnd );
			if( enableTooltips )
			{
				regionGump.AddToolTip( tooltipClilocID, pSocket, regionLabelTooltip.toString() );
			}

			if( regionValue == "-" )
			{
				regionValueTooltip = "Value not set";
			}

			switch( i + 700 )
			{
				case regionProp.population:
				case regionProp.numOrePrefs:
					break;
				default:
					regionGump.AddButton( 120, buttonStartY, gumpMainButtonOff, gumpMainButtonOn, 1, 0, buttonID);
					if( enableTooltips )
					{
						regionGump.AddToolTip( tooltipClilocID, pSocket, ( regionValueTooltip != "" ? regionValueTooltip : regionValue ));
					}
					break;
			}

			// Values
			regionGump.AddHTMLGump( 125, valueStartY, 105, 20, 0, 0, propertyValueStart + regionValue + propertyValueEnd );

			switch( i + 700 )
			{
				case regionProp.population:
				case regionProp.numOrePrefs:
					if( enableTooltips )
					{
						regionGump.AddToolTip( tooltipClilocID, pSocket, ( regionValueTooltip != "" ? regionValueTooltip : regionValue ) + " (Read-Only)" );
					}
					break;
				default:
					break;
			}

		}

		if( gumpPage == 1 )
		{
			pageOneLabelStartY += 20;
			pageOneButtonStartY += 20;
			pageOneValueStartY += 20;
		}
		else
		{
			pageXlabelStartY += 20;
			pageXbuttonStartY += 20;
			pageXvalueStartY += 20;
		}

		buttonID++;
	}

	regionGump.Send( pSocket );
	regionGump.Free();
}

// Handle properties of user account
function HandleAccountTarget( pSocket, myTarget )
{
	var accountGump = new Gump;
	accountGump = RenderZeroethPage( pSocket, accountGump, myTarget, false, false );

	var propertyName;
	var buttonID = 800;
	var gumpPage = 1;
	var totalPages = Math.ceil( 1 + ( accountPropCount - 13 ) / 17 );

	// First page
	var pageOneLabelStartY = 139;
	var pageOneButtonStartY = 140;
	var pageOneValueStartY = 139;
	// Subsequent pages
	var pageXlabelStartY = 59;
	var pageXbuttonStartY = 60;
	var pageXvalueStartY = 59;

	// Loop over all character properties
	var i = 0;
	for( i = 0; i < accountPropCount + 1; i++ )
	{
		if( i == 0 )
		{
			// Page 1
			accountGump = RenderFirstPage( pSocket, accountGump, myTarget, "Account", accountPropCount, totalPages );
		}
		else // All other pages
		{
			// 17 options can fit on each page, so increase page number as needed
			switch( i )
			{
				case 13:
					gumpPage = 2;
					break;
				case 30:
					gumpPage = 3;
					break;
			}

			// Only add these when it's time for a new page
			if( i == 13 || i == 30 )
			{
				pageXlabelStartY = 59;
				pageXbuttonStartY = 60;
				pageXvalueStartY = 59;
				accountGump = RenderOtherPages( pSocket, accountGump, gumpPage, totalPages );
			}
		}

		var labelStartY = 0;
		var buttonStartY = 0;
		var valueStartY = 0;
		if( gumpPage == 1 )
		{
			labelStartY = pageOneLabelStartY;
			buttonStartY = pageOneButtonStartY;
			valueStartY = pageOneValueStartY;
		}
		else
		{
			labelStartY = pageXlabelStartY;
			buttonStartY = pageXbuttonStartY;
			valueStartY = pageXvalueStartY;
		}

		var index = 20;
		for( var k in accountProp )
		{
		    if( accountProp.hasOwnProperty( k ) && index == i + 20 )
		    {
		        // k is key
				propertyName = k.charAt( 0 ).toUpperCase() + k.slice( 1 );
		        break;
		    }
		    index++;
		}

		var accountLabel = propertyName;
		var accountLabelTooltip = "";
		var accountValue = "";
		var accountValueTooltip = "";
		var errorFound = false;
		switch( i + 800 )
		{
			case accountProp.comment:
				accountLabelTooltip = GetDictionaryEntry( 8700, pSocket.language ); // Contact info/comment for Account
				accountValue 		= ( myTarget.comment ).toString();
				break;
			case accountProp.character1:
				accountLabelTooltip = GetDictionaryEntry( 8701, pSocket.language ); // Character in slot 1 on Account
				accountValue 		= ( ValidateObject( myTarget.character1 ) ? ( myTarget.character1.name ).toString() : "<BASEFONT color=#EECD8B>-</BASEFONT>" );
				break;
			case accountProp.character2:
				accountLabelTooltip = GetDictionaryEntry( 8702, pSocket.language ); // Character in slot 2 on Account
				accountValue 		= ( ValidateObject( myTarget.character2 ) ? ( myTarget.character2.name ).toString() : "<BASEFONT color=#EECD8B>-</BASEFONT>" );
				break;
			case accountProp.character3:
				accountLabelTooltip = GetDictionaryEntry( 8703, pSocket.language ); // Character in slot 3 on Account
				accountValue 		= ( ValidateObject( myTarget.character3 ) ? ( myTarget.character3.name ).toString() : "<BASEFONT color=#EECD8B>-</BASEFONT>" );
				break;
			case accountProp.character4:
				accountLabelTooltip = GetDictionaryEntry( 8704, pSocket.language ); // Character in slot 4 on Account
				accountValue 		= ( ValidateObject( myTarget.character4 ) ? ( myTarget.character4.name ).toString() : "<BASEFONT color=#EECD8B>-</BASEFONT>" );
				break;
			case accountProp.character5:
				accountLabelTooltip = GetDictionaryEntry( 8705, pSocket.language ); // Character in slot 5 on Account
				accountValue 		= ( ValidateObject( myTarget.character5 ) ? ( myTarget.character5.name ).toString() : "<BASEFONT color=#EECD8B>-</BASEFONT>" );
				break;
			case accountProp.character6:
				accountLabelTooltip = GetDictionaryEntry( 8706, pSocket.language ); // Character in slot 6 on Account
				accountValue 		= ( ValidateObject( myTarget.character6 ) ? ( myTarget.character6.name ).toString() : "<BASEFONT color=#EECD8B>-</BASEFONT>" );
				break;
			case accountProp.character7:
				accountLabelTooltip = GetDictionaryEntry( 8707, pSocket.language ); // Character in slot 7 on Account
				accountValue 		= ( ValidateObject( myTarget.character7 ) ? ( myTarget.character7.name ).toString() : "<BASEFONT color=#EECD8B>-</BASEFONT>" );
				break;
			case accountProp.currentChar:
				accountLabelTooltip = GetDictionaryEntry( 8708, pSocket.language ); // Currently logged in character (if any)
				accountValue 		= ( ValidateObject( myTarget.currentChar ) ? ( myTarget.currentChar.name ).toString() : "<BASEFONT color=#EECD8B>-</BASEFONT>" );
				break;
			case accountProp.flags:
				accountLabelTooltip = GetDictionaryEntry( 8709, pSocket.language ); // Flags set on Account (Read-Only)
				accountValue 		= "<BASEFONT color=#EECD8B>0x" + ( myTarget.flags ).toString( 16 ) + "</BASEFONT>";
				break;
			case accountProp.lastIP:
				accountLabelTooltip = GetDictionaryEntry( 8710, pSocket.language ); // Last IP used to login with this Account (Read-Only)
				accountValue 		= "<BASEFONT color=#EECD8B>" + ( myTarget.lastIP ).toString() + "</BASEFONT>";
				break;
			// Flags
			case accountProp.isBanned:
				accountLabelTooltip = GetDictionaryEntry( 8711, pSocket.language ); // Is this Account banned?
				accountValue 		= ( myTarget.isBanned ? "true" : "false" );
				break;
			case accountProp.isCounselor:
				accountLabelTooltip = GetDictionaryEntry( 8712, pSocket.language ); // Is this Account marked as a Counselor account?
				accountValue 		= ( myTarget.isCounselor ? "true" : "false" );
				break;
			case accountProp.isGM:
				accountLabelTooltip = GetDictionaryEntry( 8713, pSocket.language ); // Is this Account marked as a GM account?
				accountValue 		= ( myTarget.isGM ? "true" : "false" );
				break;
			case accountProp.isOnline:
				accountLabelTooltip = GetDictionaryEntry( 8714, pSocket.language ); // Is someone logged on to this Account currently? (Read-Only)
				accountValue 		= "<BASEFONT color=#EECD8B>" + ( myTarget.isOnline ? "true" : "false" ) + "</BASEFONT>";
				break;
			case accountProp.isPublic:
				accountLabelTooltip = GetDictionaryEntry( 8715, pSocket.language ); // Is contact/comments for this account marked as public? (does nothing atm)
				accountValue 		= ( myTarget.isPublic ? "true" : "false" );
				break;
			case accountProp.isSeer:
				accountLabelTooltip = GetDictionaryEntry( 8716, pSocket.language ); // Is this Account marked as a Seer account?
				accountValue 		= ( myTarget.isSeer ? "true" : "false" );
				break;
			case accountProp.isSlot1Blocked:
				accountLabelTooltip = GetDictionaryEntry( 8717, pSocket.language ); // Is slot 1 on this Account blocked?
				accountValue 		= ( myTarget.isSlot1Blocked ? "true" : "false" );
				break;
			case accountProp.isSlot2Blocked:
				accountLabelTooltip = GetDictionaryEntry( 8718, pSocket.language ); // Is slot 2 on this Account blocked?
				accountValue 		= ( myTarget.isSlot2Blocked ? "true" : "false" );
				break;
			case accountProp.isSlot3Blocked:
				accountLabelTooltip = GetDictionaryEntry( 8719, pSocket.language ); // Is slot 3 on this Account blocked?
				accountValue 		= ( myTarget.isSlot3Blocked ? "true" : "false" );
				break;
			case accountProp.isSlot4Blocked:
				accountLabelTooltip = GetDictionaryEntry( 8720, pSocket.language ); // Is slot 4 on this Account blocked?
				accountValue 		= ( myTarget.isSlot4Blocked ? "true" : "false" );
				break;
			case accountProp.isSlot5Blocked:
				accountLabelTooltip = GetDictionaryEntry( 8721, pSocket.language ); // Is slot 5 on this Account blocked?
				accountValue 		= ( myTarget.isSlot5Blocked ? "true" : "false" );
				break;
			case accountProp.isSlot6Blocked:
				accountLabelTooltip = GetDictionaryEntry( 8722, pSocket.language ); // Is slot 6 on this Account blocked?
				accountValue 		= ( myTarget.isSlot6Blocked ? "true" : "false" );
				break;
			case accountProp.isSuspended:
				accountLabelTooltip = GetDictionaryEntry( 8723, pSocket.language ); // Is this Account suspended?
				accountValue 		= ( myTarget.isSuspended ? "true" : "false" );
				break;
			case accountProp.isSlot7Blocked:
				accountLabelTooltip = GetDictionaryEntry( 8724, pSocket.language ); // Is slot 7 on this Account blocked?
				accountValue 		= ( myTarget.isSlot7Blocked ? "true" : "false" );
				break;
			case accountProp.timeban:
				accountLabelTooltip = GetDictionaryEntry( 8725, pSocket.language ); // How long is user banned for, in minutes?
				if( myTarget.timeban > 0 )
				{
					var currentTime = new Date();
					var currentTimeMin = Math.ceil( currentTime.getTime() / 1000 / 60 );
					var timebanDelta = myTarget.timeban - currentTimeMin;
					if( timebanDelta > 0 )
					{
						accountValue = timebanDelta;
						accountValueTooltip = "Banned until: " + new Date( myTarget.timeban * 60 * 1000 );
					}
					else
					{
						accountValue = 0;
					}
				}
				else
				{
					accountValue = 0;
				}
				break;
			case accountProp.firstLogin:
				accountLabelTooltip = GetDictionaryEntry( 8726, pSocket.language ); // When was the account logged into the first time? (Read-Only)
				if( myTarget.firstLogin > 0 )
				{
					accountValue = "<BASEFONT color=#EECD8B>" + myTarget.firstLogin + "</BASEFONT>";
					accountValueTooltip = new Date( myTarget.firstLogin * 60 * 1000 );
				}
				else
				{
					accountValue = 0;
				}
				break;
			default:
				errorFound = true;
				Console.PrintSectionBegin();
  				Console.Print( "Unhandled account property in tweak command script!" );
				Console.PrintDone();
				break;
		}

		if( !errorFound )
		{
			// Labels
			accountGump.AddHTMLGump( 15, labelStartY, 100, 20, 0, 0, propertyLabelStart + accountLabel + propertyLabelEnd );
			if( enableTooltips )
			{
				accountGump.AddToolTip( tooltipClilocID, pSocket, accountLabelTooltip.toString() );
			}

			if( accountValue == "-" )
			{
				accountValueTooltip = "Value not set";
			}

			// Buttons
			switch( i + 800 )
			{
				case accountProp.lastIP:
				case accountProp.lastClientType:
				case accountProp.lastClientVer:
				case accountProp.lastClientVerShort:
				case accountProp.isOnline:
				case accountProp.flags:
				case accountProp.firstLogin:
					break;
				default:
					if(( propertyName == "Character1" && !ValidateObject( myTarget.character1 ))
						|| ( propertyName == "Character2" && !ValidateObject( myTarget.character2 ))
						|| ( propertyName == "Character3" && !ValidateObject( myTarget.character3 ))
						|| ( propertyName == "Character4" && !ValidateObject( myTarget.character4 ))
						|| ( propertyName == "Character5" && !ValidateObject( myTarget.character5 ))
						|| ( propertyName == "Character6" && !ValidateObject( myTarget.character6 ))
						|| ( propertyName == "Character7" && !ValidateObject( myTarget.character7 ))
						|| ( propertyName == "CurrentChar" && !ValidateObject( myTarget.currentChar )))
						break;
					accountGump.AddButton( 120, buttonStartY, gumpMainButtonOff, gumpMainButtonOn, 1, 0, buttonID);
					if( enableTooltips )
					{
						accountGump.AddToolTip( tooltipClilocID, pSocket, ( accountValueTooltip != "" ? accountValueTooltip : accountValue ));
					}
					break;
			}

			// Values
			accountGump.AddHTMLGump( 125, valueStartY, 105, 20, 0, 0, propertyValueStart + accountValue + propertyValueEnd );

			switch( i + 800 )
			{
				case accountProp.lastIP:
				case accountProp.lastClientType:
				case accountProp.lastClientVer:
				case accountProp.lastClientVerShort:
				case accountProp.isOnline:
				case accountProp.flags:
				case accountProp.firstLogin:
					if( enableTooltips )
					{
						accountGump.AddToolTip( tooltipClilocID, pSocket, ( accountValueTooltip != "" ? accountValueTooltip : accountValue ) + " (Read-Only)" );
					}
					break;
				default:
					break;
			}

		}

		if( gumpPage == 1 )
		{
			pageOneLabelStartY += 20;
			pageOneButtonStartY += 20;
			pageOneValueStartY += 20;
		}
		else
		{
			pageXlabelStartY += 20;
			pageXbuttonStartY += 20;
			pageXvalueStartY += 20;
		}

		buttonID++;
	}

	accountGump.Send( pSocket );
	accountGump.Free();
}


// Show input gump for chosen property
function ShowInputGump( pUser, targetObj, propertyName, propertyType, maxLength, maxVal, propertyHint )
{
	var pSocket = pUser.socket;
	var inputBackgroundHeight = ( Math.ceil( maxLength / 40 ) * 15 );
	var inputBackgroundWidth = 285;
	var inputBackgroundLeft = 10;
	var propertyTypeExtra = "";

	if( propertyDesc = "" )
	{
		propertyDesc = GetDictionaryEntry( 8800, pSocket.language ); // Enter new value
	}

	switch( propertyType )
	{
		case "Hexadecimal":
			propertyTypeExtra = ", 0 - 0x" + maxVal.toString( 16 );
			break;
		case "Integer":
			propertyTypeExtra = ", 0 - " + maxVal;
			break;
		case "Text":
			propertyTypeExtra = ", 0 - " + maxVal;
			propertyTypeExtra = ", Max chars: " + maxLength
			break;
		case "Boolean":
			propertyTypeExtra = ", True/False";
			break;
		case "UOXObject":
			inputBackgroundWidth = 200;
			inputBackgroundLeft = 45;
			break;
		case "SkillValue":
			propertyTypeExtra = ", 0.0 - ~" + parseFloat( maxVal ).toFixed( 1 ).toString();
			propertyHint += " <BASEFONT COLOR=red>IMPORTANT:</BASEFONT> " + GetDictionaryEntry( 8801, pSocket.language ); // Changes made to effective skill values are not saved, and will be gone if server restarts. For permanent skill changes, adjust baseskills property instead!
			break;
		case "BaseSkillValue":
			propertyTypeExtra = ", 0.0 - ~" + parseFloat( maxVal ).toFixed( 1 ).toString();
			propertyHint += " " + GetDictionaryEntry( 8802, pSocket.language ); // Traditionally ranges from 0.0 to 100.0, but varies based on UO era, power-scrolls, etc.
			break;
		default:
			break;
	}

	var backgroundHeight = 170;
	if( propertyHint == "" )
	{
		backgroundHeight -= 20;
	}

	var propertyHintHeight = 20 * Math.ceil( propertyHint.length / 42 );

	var inputGump = new Gump;
	inputGump.AddPage( 0 );
	inputGump.AddBackground( 0, 0, 300, backgroundHeight + ( propertyHintHeight - 20 ) + Math.floor( inputBackgroundHeight / 2 ), gumpMainBackground );  	// Tile Dark Gray Background
	if( enableTransparentGump )
	{
		inputGump.AddCheckerTrans( 0, 0, 300, backgroundHeight + ( propertyHintHeight - 20 ) + Math.floor( inputBackgroundHeight / 2 ));
	}
	inputGump.AddHTMLGump( 0, 10, 300, 20, 0, 0, "<BASEFONT color=#EECD8B><CENTER><BIG>" + propertyName.toUpperCase() + "</BIG></CENTER></BASEFONT>" );
	inputGump.AddHTMLGump( 0, 30, 300, 20, 0, 0, "<BASEFONT color=#cdcdcd><CENTER>" + propertyDesc + " ( " + propertyType + propertyTypeExtra + " ):</CENTER></BASEFONT>" );

	// Confirm/Cancel buttons, and propertyHint
	if( propertyType != "UOXObject" )
	{
		inputGump.AddHTMLGump( 70, backgroundHeight - 27 + ( propertyHintHeight - 20 ) + Math.floor( inputBackgroundHeight / 2 ), 50, 20, 0, 0, "<BASEFONT color=#5EED5C>Confirm</BASEFONT>" );
	}
	inputGump.AddHTMLGump( 210, backgroundHeight - 27 + ( propertyHintHeight - 20 ) + Math.floor( inputBackgroundHeight / 2 ), 50, 20, 0, 0, "<BASEFONT color=#FF0000>Cancel</BASEFONT>" );
	inputGump.AddHTMLGump( 10, 110 + Math.floor( inputBackgroundHeight / 2 ), 280, propertyHintHeight, 0, 0, "<CENTER><BASEFONT color=#EECD8B>" + propertyHint + "</BASEFONT></CENTER>" );
	if( propertyType != "UOXObject" )
	{
		inputGump.AddButton( 35, backgroundHeight - 30 + ( propertyHintHeight - 20 ) + Math.floor( inputBackgroundHeight / 2 ), 4023, 4024, 1, 0, 1000 ); // Okay, confirm
	}
	inputGump.AddButton( 175, backgroundHeight - 30 + ( propertyHintHeight - 20 ) + Math.floor( inputBackgroundHeight / 2 ), 4017, 4018, 1, 0, 1 ); 	// Exit

	// True/False radio buttons for Boolean input
	if( propertyType == "Boolean" )
	{
		inputGump.AddBackground( 15, 47, 270, 65, gumpSecondaryBackground ); // Tile White Background
		inputGump.AddRadio( 30, 50, 0x16c3, (targetObj[propertyName] ? 1 : 0), 0 );
		inputGump.AddRadio( 30, 80, 0x16c9, (targetObj[propertyName] ? 0 : 1), 1 );
		inputGump.AddHTMLGump( 70, 50 + Math.floor( inputBackgroundHeight / 2 ), 50, 20, 0, 0, "<BASEFONT color=#EEEEEE>True</BASEFONT>" );
		inputGump.AddHTMLGump( 70, 80 + Math.floor( inputBackgroundHeight / 2 ), 50, 20, 0, 0, "<BASEFONT color=#EEEEEE>False</BASEFONT>" );
	}
	else
	{
		// TextEntry for other inputs
		inputGump.AddBackground( inputBackgroundLeft, 60, inputBackgroundWidth, inputBackgroundHeight + 20, 2620 ); // Tile White Background

		if( propertyType == "Hexadecimal" )
		{
			if( propertyName == "more" || propertyName == "morex" || propertyName == "morey" || propertyName == "morez" )
			{
				var hexVal = "0x" + ( "00000000" + ( Number( targetObj[propertyName] ).toString( 16 ))).slice( -8 )
				inputGump.AddTextEntryLimited( 30, 70, 240, 10 + inputBackgroundHeight, 55, 1, 5, hexVal, maxLength );
				if( enableTooltips )
				{
					inputGump.AddToolTip( tooltipClilocID, pUser.socket, ( targetObj[propertyName] ).toString() );
				}
			}
			else
			{
				inputGump.AddTextEntryLimited( 30, 70, 240, 10 + inputBackgroundHeight, 55, 1, 5, "0x" + ( targetObj[propertyName] ).toString( 16 ), maxLength );
			}
		}
		else if( propertyType == "Text" && ( propertyName in targetObj ) && targetObj[propertyName] == null )
		{
			inputGump.AddTextEntryLimited( 30, 70, 240, 10 + inputBackgroundHeight, 55, 1, 5, "-", maxLength );
		}
		else if( propertyType == "UOXObject" && propertyName != "race" )
		{
			inputGump.AddButton( 20, 66, 0xfae, 0xfaf, 1, 0, 1002 ); // Assign new Object
			if( enableTooltips )
			{
				inputGump.AddToolTip( tooltipClilocID, pUser.socket, GetDictionaryEntry( 8804, pSocket.language )); // Assign new Object
			}
			inputGump.AddButton( 245, 66, 0xfb4, 0xfb5, 1, 0, 1001 ); 	// Clear Object
			if( enableTooltips )
			{
				inputGump.AddToolTip( tooltipClilocID, pUser.socket, GetDictionaryEntry( 8805, pSocket.language )); // Clear Object
			}
			if( targetObj[propertyName] == null )
			{
				inputGump.AddHTMLGump( 50, 70, 195, 20, 0, 0, "<CENTER><BASEFONT color=#EECD8B> - </BASEFONT></CENTER>" );
			}
			else
				inputGump.AddHTMLGump( 50, 70, 195, 20, 0, 0, "<CENTER><BASEFONT color=#EECD8B>" + (targetObj[propertyName].name).toString() + "</BASEFONT></CENTER>" );
		}
		else
		{
			var propertyVal = (( propertyName in targetObj ) && targetObj[propertyName] != null ) ? targetObj[propertyName] : "-";
			if( propertyName == "race" && targetObj[propertyName] != null )
			{
				propertyVal = ( targetObj[propertyName].id ).toString();
			}
			else if( propertyName == "raceGate" && targetObj[propertyName] != null )
			{
				if( targetObj[propertyName] == 65535 )
				{
					propertyVal = "-";
				}
				else
				{
					propertyVal = ( targetObj[propertyName] ).toString();
				}
			}
			else if( propertyName == "decaytime" )
			{
				propertyVal = ( Math.floor(( targetObj[propertyName] - GetCurrentClock() ) / 1000 )).toString();
			}
			else if( propertyName == "timeban" )
			{
				var currentTime = new Date();
				var currentTimeMin = Math.ceil( currentTime.getTime() / 1000 / 60 );
				var timebanDelta = targetObj[propertyName] - currentTimeMin;
				if( timebanDelta > 0 )
				{
					propertyVal = timebanDelta;
				}
				else
				{
					propertyVal = 0;
				}
			}
			else if( propertyType == "SkillValue" )
			{
				pUser.socket.xText = "false";
				propertyVal = parseFloat( targetObj.skills[propertyName] / 10 ).toFixed( 1 ).toString();
			}
			else if( propertyType == "BaseSkillValue" )
			{
				pUser.socket.xText = "true";
				propertyVal = parseFloat( targetObj.baseskills[propertyName] / 10 ).toFixed( 1 ).toString();
			}
			inputGump.AddTextEntryLimited( 30, 70, 240, 10 + inputBackgroundHeight, 55, 1, 5, propertyVal, maxLength );
		}
	}

	inputGump.Send( pUser.socket );
	inputGump.Free();
}

// Handle button-presses in gumps
function onGumpPress( pSocket, pButton, gumpData )
{
	var targetObj = pSocket.tempObj2;
	var modifyData = false;
	var clearObject = false;
	var selectNewObject = false;
	var propertyName = "";
	var propertyType = "";
	var propertyHint = "";
	var maxLength = 0;
	var maxVal = 0;
	var baseSkills = ( pSocket.xText != null && pSocket.xText == "true" ? true : false );
	pSocket.xText = null;

	if( pSocket.currentChar.GetTag( "tweakRegion" ))
	{
		targetObj = GetTownRegion( parseInt( pSocket.currentChar.GetTag( "tweakRegion" )));
	}

	if( pSocket.currentChar.GetTag( "tweakAccount" ))
	{
		if( ValidateObject( targetObj ))
		{
			targetObj = targetObj.account;
		}
	}

	if( targetObj == null )
	{
		pSocket.SysMessage( GetDictionaryEntry( 8803, pSocket.language )); // Object no longer exists.
		pSocket.tempObj2 = null;
		return;
	}

	if( pButton == 1000 || pButton == 1001 || pButton == 1002 )
	{
		// We got here from sub-dialogue! Use button from before
		modifyData = true;
		if( pButton == 1001 )
		{
			clearObject = true;
		}
		else if( pButton == 1002 )
		{
			selectNewObject = true;
		}

		pButton = pSocket.tempInt2;
	}
	else
	{
		// Store button for later so we know which property to change
		pSocket.tempInt2 = pButton;
	}

	switch( pButton )
	{
		case 0: // Close gump
			pSocket.tempObj2 = null;
			break;
		case 1: // Cancel tweak and reopen tweak menu
			if( ValidateObject( targetObj ))
			{
				if( targetObj.isItem && targetObj.IsMulti() )
				{
					HandleMultiTarget( pSocket, targetObj );
				}
				else if( targetObj.isItem )
				{
					HandleItemTarget( pSocket, targetObj );
				}
				else if( targetObj.isChar )
				{
					HandleCharTarget( pSocket, targetObj );
				}
			}
			else if( targetObj != null && "username" in targetObj )
			{
				HandleAccountTarget( pSocket, targetObj );
			}
			else // Region
			{
				HandleRegionTarget( pSocket, targetObj );
			}
			break;

		// Hexadecimal  ------------------------------------------------------
		case 2: // ID
			propertyName = "id";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8806, pSocket.language ); // ID of object
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case itemProp.ammoFX:
			propertyName = "ammoFX";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8125, pSocket.language ); // ID of moving effect played when ranged weapon fires projectile
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case itemProp.ammoFXHue:
			propertyName = "ammoFXHue";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8126, pSocket.language ); // Hue of moving effect played when ranged weapon fires projectile
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case itemProp.ammoHue:
			propertyName = "ammoHue";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8128, pSocket.language ); // Hue of item used as ammo by ranged weapon
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case itemProp.ammoID:
			propertyName = "ammoID";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8129, pSocket.language ); // ID of item used as ammo by ranged weapon
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case itemProp.colour:
			propertyName = "colour";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8134, pSocket.language ); // Colour of item
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case itemProp.layer:
			propertyName = "layer";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8162, pSocket.language ) + ' <BR>(<A HREF="https://www.uox3.org/docs/index.html#itemLayers">See list of Layers in UOX3 Docs</A>)';
			maxLength = 4;
			maxVal = 0x1d;
			break;
		case itemProp.more:
			propertyName = "more";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8170, pSocket.language ); // Generic item property used for many different things. Can enter value as hexadecimal or decimal.
			maxLength = 10;
			maxVal = 0xffffffff;
			break;
		case itemProp.morex:
			propertyName = "morex";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8170, pSocket.language ); // Generic item property used for many different things. Can enter value as hexadecimal or decimal.
			maxLength = 10;
			maxVal = 0xffffffff;
			break;
		case itemProp.morey:
			propertyName = "morey";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8170, pSocket.language ); // Generic item property used for many different things. Can enter value as hexadecimal or decimal.
			maxLength = 10;
			maxVal = 0xffffffff;
			break;
		case itemProp.morez:
			propertyName = "morez";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8170, pSocket.language ); // Generic item property used for many different things. Can enter value as hexadecimal or decimal.
			maxLength = 10;
			maxVal = 0xffffffff;
			break;
		// Character Properties
		case charProp.colour:
			propertyName = "colour";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8314, pSocket.language ); // Colour of character's body
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case charProp.direction:
			propertyName = "direction";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8321, pSocket.language ); // Current direction character is facing
			maxLength = 4;
			maxVal = 0xff;
			break;
		case charProp.emoteColour:
			propertyName = "emoteColour";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8322, pSocket.language ); // Colour of character's emotes
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case charProp.flag:
			propertyName = "flag";
			propertyType = "Hexadecimal";
			propertyHint = "NPC flag (0x1 = murderer, 0x2 = criminal, 0x4 = innocent, 0x8 = neutral)";
			maxLength = 4;
			maxVal = 0xff;
			break;
		case charProp.nextAct:
			propertyName = "nextAct";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8369, pSocket.language ); // The next spellcasting action character is going to do
			maxLength = 4;
			maxVal = 0xff;
			break;
		case charProp.orgID:
			propertyName = "orgID";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8376, pSocket.language ); // Character's original body ID
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case charProp.orgSkin:
			propertyName = "orgSkin";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8377, pSocket.language ); // Character's original skin (colour)
			maxLength = 6;
			maxVal = 0xffff;
			break;
		case charProp.sayColour:
			propertyName = "sayColour";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8389, pSocket.language ); // Colour of character's speech
			maxLength = 6;
			maxVal = 0xffff;
			break;
		// Multi Properties
		case multiProp.colour:
			propertyName = "colour";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8504, pSocket.language ); // Colour of Multi
			maxLength = 6;
			maxVal = 0xffff;
			break;
		// Region Properties
		case regionProp.taxResource:
			propertyName = "taxResource";
			propertyType = "Hexadecimal";
			propertyHint = GetDictionaryEntry( 8623, pSocket.language ); // ID of the type of resource being taxed in the Region
			maxLength = 6;
			maxVal = 0xffff;
			break;

		// Strings/Text ------------------------------------------------------
		case 10: // Item or Multi Name
			propertyName = "name";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8807, pSocket.language ); // Name of Item/Multi, capped at 127 characters
			maxLength = 127;
			break;
		case 11: // Character Name
			propertyName = "name";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8808, pSocket.language ); // Name of character, capped at 30 characters
			maxLength = 30;
			break;
		case 12: // Region Name
			propertyName = "name";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8809, pSocket.language ); // Name of Region, capped at 50 characters
			maxLength = 50;
			break;
		case itemProp.desc:
			propertyName = "desc";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8147, pSocket.language ); // Description of item - used for items sold on player vendors
			maxLength = 127;
			break;
		case itemProp.event:
			propertyName = "event";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8206, pSocket.language ); // Server event item is associated with
			maxLength = 127;
			break;
		case itemProp.name2:
			propertyName = "name2";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8175, pSocket.language ); // Secondary name of object, revealed using item identification
			maxLength = 127;
			break;
		case itemProp.sectionID:
			propertyName = "sectionID";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8213, pSocket.language ); // Section ID from DFNs object originated from
			maxLength = 127;
			break;
		case itemProp.spawnsection:
			propertyName = "spawnSection";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8192, pSocket.language ); // SpawnSection used to spawn objects from - SpawnObject only
			maxLength = 127;
			break;
		// Character properties
		case charProp.foodList:
			propertyName = "foodList";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8325, pSocket.language ); // ID of foodlist that a creature will accept as food, from UOX3/dfndata/items/itemlists/foodlists.dfn
			maxLength = 127;
			break;
		case charProp.sectionID:
			propertyName = "sectionID";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8213, pSocket.language ); // Section ID from DFNs object originated from
			maxLength = 127;
			break;
		case charProp.guildTitle:
			propertyName = "guildTitle";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8329, pSocket.language ); // Guild title of character. Normally set by Guild Master via Guild menu.
			maxLength = 127;
			break;
		case charProp.title:
			propertyName = "title";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8417, pSocket.language ); // The title of the character
			maxLength = 59;
			break;
		// Multi Properties
		case multiProp.deed:
			propertyName = "deed";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8505, pSocket.language ); // Section Item ID of deed used to create Multi
			maxLength = 127;
			break;
		// Region Properties
		case regionProp.owner:
			propertyName = "owner";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8616, pSocket.language ); // The owner of guards in the Region
			maxLength = 30;
			break;
		// Account Properties
		case accountProp.comment:
			propertyName = "comment";
			propertyType = "Text";
			propertyHint = GetDictionaryEntry( 8700, pSocket.language ); // Contact info/comment for Account
			maxLength = 200;
			break;

		// Integer ------------------------------------------------------
		case itemProp.ammoFXRender:
			propertyName = "ammoFXRender";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8127, pSocket.language ) + " (0 - normal, 1 - transparent, 2 - additive, 3 - dark colors transparent, 4 - bright colors transparent, 5 - semi transparent, 6 - subtractive/negative colors, 7 - inverted colors?)";
			maxLength = 1;
			maxVal = 7;
			break;
		case itemProp.amount:
			propertyName = "amount";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8130, pSocket.language ); // Amount of items in pile, or amount of items restocked on shopkeeper
			break;
		case itemProp.baseRange:
			propertyName = "baseRange";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8207, pSocket.language ); // Base range of ranged weapon (SA and later only)
			break;
		case itemProp.baseWeight:
			propertyName = "baseWeight";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8810, pSocket.language ); // Base weight of item (100 = 1.0 stones)
			break;
		case itemProp.buyvalue:
			propertyName = "buyvalue";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8132, pSocket.language ); // Item's buy value - price player needs to pay to buy item from NPC shopkeeper
			break;
		case itemProp.carveSection:
			propertyName = "carveSection";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8133, pSocket.language ); // ID of section in carve DFNs that triggers if this item is carved - used for corpses
			maxLength = 5;
			maxVal = 32767;
			break;
		case itemProp.def:
			propertyName = "def";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8146, pSocket.language ); // Defensive value of item (Physical Resistance post-AoS, AR in older UO)
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.dir:
			propertyName = "dir";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8149, pSocket.language ); // Direction of item - used to determine light type on light sources
			maxLength = 4;
			maxVal = 128;
			break;
		case itemProp.entryMadeFrom:
			propertyName = "entryMadeFrom";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8150, pSocket.language ); // The ID of entry from Create DFN that item was crafted from (if any)
			break;
		case itemProp.health:
			propertyName = "health";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8151, pSocket.language ); // Item's current health/hitpoints (cannot exceed value of maxhp property)
			maxLength = 5;
			maxVal = 32767;
			break;
		case itemProp.hidamage:
			propertyName = "hidamage";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8152, pSocket.language ); // Max damage item can deal in combat (randomized between lodamage and hidamage)
			maxLength = 5;
			maxVal = 32767;
			break;
		case itemProp.instanceID:
			propertyName = "instanceID";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8153, pSocket.language ); // ID of instance of world that item exists in. Objects in different instances will not be able to interact with one another!
			break;
		case itemProp.lodamage:
			propertyName = "lodamage";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8163, pSocket.language ); // Lowest damage item can deal in combat
			maxLength = 5;
			maxVal = 32767;
			break;
		case itemProp.madeWith:
			propertyName = "madeWith";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8164, pSocket.language ); // Skill ID used to create item
			maxLength = 3;
			maxVal = 127;
			break;
		case itemProp.maxhp:
			propertyName = "maxhp";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8165, pSocket.language ); // Maximum amount of hitpoints item can have
			maxVal = 32767;
			break;
		case itemProp.maxinterval:
			propertyName = "maxinterval";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8166, pSocket.language ); // Max interval in seconds between respawns - SpawnObjects only
			maxLength = 3;
			maxVal = 255;
			break;
		case itemProp.maxItems:
			propertyName = "maxItems";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8167, pSocket.language ); // Max items a container can contain
			break;
		case itemProp.maxRange:
			propertyName = "maxRange";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8168, pSocket.language ); // Maximum range of ranged weapon
			break;
		case itemProp.maxUses:
			propertyName = "maxUses";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8209, pSocket.language ); // Maximum amount of uses item can have
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.mininterval:
			propertyName = "mininterval";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8169, pSocket.language ); // Min interval in seconds between respawns - SpawnObjects only
			maxLength = 3;
			maxVal = 255;
			break;
		case itemProp.movable:
			propertyName = "movable";
			propertyHint = GetDictionaryEntry( 8174, pSocket.language ); // Movable state of item (0 - default, 1 - movable, 2 - not movable, 3 - locked down)
			propertyType = "Integer";
			maxLength = 1;
			maxVal = 9;
			break;
		case itemProp.poison:
			propertyName = "poison";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8178, pSocket.language ); // Poison level of item from 0 to 5
			maxLength = 1;
			maxVal = 5;
			break;
		case itemProp.race:
			propertyName = "race";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8179, pSocket.language ); // Item deals double damage versus specified race
			break;
		case itemProp.rank:
			propertyName = "rank";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8180, pSocket.language ); // Quality of item determined at time of crafting
			maxLength = 3;
			maxVal = 127;
			break;
		case itemProp.resistHeat:
			propertyName = "resistHeat";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8181, pSocket.language ) + " (0-1000, where 1000 = 100.0%)"; // Item's Heat/Fire Resistance
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.resistCold:
			propertyName = "resistCold";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8182, pSocket.language ) + " (0-1000, where 1000 = 100.0%)"; // Item's Cold Resistance
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.resistLight:
			propertyName = "resistLight";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8183, pSocket.language ) + " (0-1000, where 1000 = 100.0%)"; // Item's Light resistance
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.resistLightning:
			propertyName = "resistLightning";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8184, pSocket.language ) + " (0-1000, where 1000 = 100.0%)"; // Item's Lightning/Energy Resistance
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.resistPoison:
			propertyName = "resistPoison";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8185, pSocket.language ) + " (0-1000, where 1000 = 100.0%)"; // Item's Poison Resistance
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.resistRain:
			propertyName = "resistRain";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8186, pSocket.language ) + " (0-1000, where 1000 = 100.0%)"; // Item's Rain Resistance
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.resistSnow:
			propertyName = "resistSnow";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8187, pSocket.language ) + " (0-1000, where 1000 = 100.0%)"; // Item's Snow Resistance
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.restock:
			propertyName = "restock";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8188, pSocket.language ); // Amount of this item that vendors will restock by default
			break;
		case itemProp.scripttrigger:
			propertyName = "scripttrigger";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8189, pSocket.language ); // JS Script from jse_fileassociations.scp assigned to Item
			break;
		case itemProp.sellvalue:
			propertyName = "sellvalue";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8191, pSocket.language ); // Item's sell value - price player can sell item to NPC shopkeeper for
			break;
		case itemProp.speed:
			propertyName = "speed";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8193, pSocket.language ); // Attack speed of item - used by weapons
			maxLength = 3;
			maxVal = 127;
			break;
		case itemProp.strength:
			propertyName = "strength";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8194, pSocket.language ); // Strength required to equip item
			maxVal = 32767;
			break;
		case itemProp.tempTimer:
			propertyName = "tempTimer";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8195, pSocket.language ); // Temporary timer used by spawners
			break;
		case itemProp.type:
			propertyName = "type";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8171, pSocket.language ) + '<BR><A HREF="https://www.uox3.org/docs/index.html#itemTypes">See list of Item Types in UOX3 Docs</A>'; // Item type of item - determines double-click
			break;
		case itemProp.usesLeft:
			propertyName = "usesLeft";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8208, pSocket.language ); // Item's current health/hitpoints (cannot exceed value of maxhp property)
			maxLength = 5;
			maxVal = 65535;
			break;
		case itemProp.visible:
			propertyName = "visible";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8197, pSocket.language ); // Determines who item is visible for (0 = Visible, 1 = Hidden, 2 = Magically Invisible, 3 = Visible to GMs only)
			maxLength = 1;
			maxVal = 3;
			break;
		case itemProp.weight:
			propertyName = "weight";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8198, pSocket.language ); // Weight of item (100 = 1.0 stone)
			break;
		case itemProp.weightMax:
			propertyName = "weightMax";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8199, pSocket.language ); // Maximum weight a container can hold (100 = 1.0 stone)
			break;
		case itemProp.worldnumber:
			propertyName = "worldnumber";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8201, pSocket.language ); // World that item exists in
			break;
		case itemProp.x:
			propertyName = "x";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8202, pSocket.language ); // X coordinate of item in the world - or in container
			maxLength = 5;
			maxVal = 32767;
			break;
		case itemProp.y:
			propertyName = "y";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8203, pSocket.language ); // Y coordinate of item in the world - or in container
			maxLength = 5;
			maxVal = 32767;
			break;
		case itemProp.z:
			propertyName = "z";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8204, pSocket.language ); // Z coordinate of item in the world - or in container
			maxLength = 3;
			maxVal = 127;
			break;
		// Character properties
		case charProp.aitype:
			propertyName = "aitype";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8301, pSocket.language ) + '<BR>(<A HREF="https://www.uox3.org/docs/index.html#uox3NPCAI">See more info on NPC AIs in UOX3 Docs</A>)'; // NPC AI Type
			maxLength = 3;
			maxVal = 128;
			break;
		case charProp.brkPeaceChance:
			propertyName = "brkPeaceChance";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8308, pSocket.language ); // Chance of character affected by peacemaking will break out of peace state
			maxLength = 3;
			maxVal = 100;
			break;
		case charProp.cell:
			propertyName = "cell";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8313, pSocket.language ); // Jail cell character is locked up in. If -1, means character is not jailed.
			maxLength = 3;
			maxVal = 127;
			break;
		case charProp.commandlevel:
			propertyName = "commandlevel";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8316, pSocket.language ); // Character's access level to commands. Default setup: 0 = Player, 1 = Counselor, 2 = GM, 5 = Admin
			maxLength = 1;
			maxVal = 9;
			break;
		case charProp.deaths:
			propertyName = "deaths";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8319, pSocket.language ); // Total amount of times (player) character has died
			maxLength = 5;
			maxVal = 65535;
			break;
		case charProp.dexterity:
			propertyName = "dexterity";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8320, pSocket.language ); // Dexterity attribute of character
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.fame:
			propertyName = "fame";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8323, pSocket.language ); // Character's current fame level
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.fontType:
			propertyName = "fontType";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8324, pSocket.language ); // Font type used by character's speech
			maxLength = 2;
			maxVal = 12;
			break;
		case charProp.fx1:
			propertyName = "fx1";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8431, pSocket.language ); // Top left X coord for character's wandertype boundary
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.fy1:
			propertyName = "fy1";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8432, pSocket.language ); // Top left Y coord for character's wandertype boundary
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.fx2:
			propertyName = "fx2";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8433, pSocket.language ); // Bottom right X coord for character's wandertype boundary
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.fy2:
			propertyName = "fy2";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8434, pSocket.language ); // Bottom right Y coord for character's wandertype boundary
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.fz:
			propertyName = "fz";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8435, pSocket.language ); // Z coord for character's wandertype boundary
			maxLength = 3;
			maxVal = 128;
			break;
		case charProp.gender:
			propertyName = "gender";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8327, pSocket.language ); // Gender of character (male/female)
			maxLength = 1;
			maxVal = 1;
			break;
		case charProp.health:
			propertyName = "health";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8330, pSocket.language ); // Character's current health/hitpoints (cannot exceed value of maxhp property)
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.hidamage:
			propertyName = "hidamage";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8331, pSocket.language ); // Highest damage character can deal in combat with wrestling/unarmed attacks
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.housesCoOwned:
			propertyName = "housesCoOwned";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8333, pSocket.language ); // Number of houses co-owned by character (Read-Only)
			maxLength = 3;
			maxVal = 999;
			break;
		case charProp.housesOwned:
			propertyName = "housesOwned";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8334, pSocket.language ); // Number of houses owned by character (Read-Only)
			maxLength = 3;
			maxVal = 999;
			break;
		case charProp.hunger:
			propertyName = "hunger";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8335, pSocket.language ); // Character's current hunger status (0 - 6, where 0 is max hungry and 6 is max full. At 0, characters may start take hunger damage depending on server settings.)
			maxLength = 1;
			maxVal = 6;
			break;
		case charProp.hungerWildChance:
			propertyName = "hungerWildChance";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8336, pSocket.language ); // Chance for extremely hungry pet to go wild with every NPC AI loop
			maxLength = 3;
			maxVal = 100;
			break;
		case charProp.instanceID:
			propertyName = "instanceID";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8338, pSocket.language ); // ID of instance character is currently in. Objects in different instances will not be able to interact with one another!
			maxLength = 5;
			maxVal = 65535;
			break;
		case charProp.intelligence:
			propertyName = "intelligence";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8339, pSocket.language ); // Intelligence attribute of character
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.karma:
			propertyName = "karma";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8356, pSocket.language ); // Character's current karma level
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.lightlevel:
			propertyName = "lightlevel";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8357, pSocket.language ); // Character's current individual light level
			maxLength = 3;
			maxVal = 255;
			break;
		case charProp.lodamage:
			propertyName = "lodamage";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8358, pSocket.language ); // Minimum damage dealt by character in combat when using wrestling/unarmed attacks
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.mana:
			propertyName = "mana";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8360, pSocket.language ); // Character's current mana
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.maxhp:
			propertyName = "maxhp";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8361, pSocket.language ); // Maximum HP character can have (max value that can display properly in player status window is 9999)
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.maxmana:
			propertyName = "maxmana";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8362, pSocket.language ); // Maximum mana character can have
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.maxstamina:
			propertyName = "maxstamina";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8363, pSocket.language ); // Maximum stamina character can have
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.murdercount:
			propertyName = "murdercount";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8366, pSocket.language ); // Amount of players character has killed
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.npcFlag:
			propertyName = "npcFlag";
			propertyType = "Integer";
			propertyHint = "NPC flag (0 = Neutral, 1 = Innocent, 2 = Evil)";
			maxLength = 1;
			maxVal = 2;
			break;
		case charProp.oldWandertype:
			propertyName = "oldWandertype";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8374, pSocket.language ); // NPC's old/previous wandertype
			maxLength = 1;
			maxVal = 7;
			break;
		case charProp.poison:
			propertyName = "poison";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8384, pSocket.language ); // Character's current poisoned state (0-5)
			maxLength = 1;
			maxVal = 5;
			break;
		case charProp.poisonStrength:
			propertyName = "poisonStrength";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8385, pSocket.language ); // Strength of poison applied by unarmed/wrestling attacks (0-5)
			maxLength = 1;
			maxVal = 5;
			break;
		case charProp.race:
			propertyName = "race";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8386, pSocket.language ); // Race character belongs to
			break;
		case charProp.raceGate:
			propertyName = "raceGate";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8387, pSocket.language ); // ID of Race for which character has used a Race Gate, if any
			break;
		case charProp.scripttrigger:
			propertyName = "scripttrigger";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8390, pSocket.language ); // JS Script from jse_fileassociations.scp assigned to Character
			maxLength = 5;
			maxVal = 65535;
			break;
		case charProp.skillToPeace:
			propertyName = "skillToPeace";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8395, pSocket.language ); // Peacemaking skill required to peacemake this NPC
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.skillToProv:
			propertyName = "skillToProv";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8396, pSocket.language ); // Provocation skill required to provoke this NPC
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.skillToTame:
			propertyName = "skillToTame";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8397, pSocket.language ); // Animal Taming skill required to tame this NPC
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.spattack:
			propertyName = "spattack";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8398, pSocket.language ); // NPC will cast spells from this spell circle #
			maxLength = 1;
			maxVal = 8;
			break;
		case charProp.spdelay:
			propertyName = "spdelay";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8399, pSocket.language ); // Delay between spellcasts for NPC character
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.split:
			propertyName = "split";
			propertyHint = GetDictionaryEntry( 8401, pSocket.language ); // Determines how many NPCs character will split into when hit in combat
			propertyType = "Integer";
			maxLength = 3;
			maxVal = 255;
			break;
		case charProp.splitchance:
			propertyName = "splitchance";
			propertyHint = GetDictionaryEntry( 8402, pSocket.language ); // Chance of creature to split when hit in combat
			propertyType = "Integer";
			maxLength = 3;
			maxVal = 100;
			break;
		case charProp.stamina:
			propertyName = "stamina";
			propertyHint = GetDictionaryEntry( 8405, pSocket.language ); // Character's current stamina
			propertyType = "Integer";
			maxLength = 5;
			maxVal = 32737;
			break;
		case charProp.strength:
			propertyName = "strength";
			propertyHint = GetDictionaryEntry( 8407, pSocket.language ); // Strength attribute of character
			propertyType = "Integer";
			maxLength = 5;
			maxVal = 32737;
			break;
		case charProp.tamedHungerRate:
			propertyName = "tamedHungerRate";
			propertyHint = GetDictionaryEntry( 8409, pSocket.language ); // The rate at which a pet grows hungry
			propertyType = "Integer";
			maxLength = 5;
			maxVal = 32737;
			break;
		case charProp.tamedThirstRate:
			propertyName = "tamedThirstRate";
			propertyHint = GetDictionaryEntry( 8410, pSocket.language ); // The rate at which a pet grows thirsty
			propertyType = "Integer";
			maxLength = 5;
			maxVal = 32737;
			break;
		case charProp.tempdex:
			propertyName = "tempdex";
			propertyHint = GetDictionaryEntry( 8412, pSocket.language ); // Character's temporary dex, as affected by equipped items, spells and potions
			propertyType = "Integer";
			maxLength = 5;
			maxVal = 32737;
			break;
		case charProp.tempint:
			propertyName = "tempint";
			propertyHint = GetDictionaryEntry( 8413, pSocket.language ); // Character's temporary int, as affected by equipped items, spells and potions
			propertyType = "Integer";
			maxLength = 5;
			maxVal = 32737;
			break;
		case charProp.tempstr:
			propertyName = "tempstr";
			propertyHint = GetDictionaryEntry( 8414, pSocket.language ); // Character's temporary str, as affected by equipped items, spells and potions
			propertyType = "Integer";
			maxLength = 5;
			maxVal = 32737;
			break;
		case charProp.thirst:
			propertyName = "thirst";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8415, pSocket.language ); // Character's current thirst status (0 - 6, where 0 is max thirsty and 6 is max full. At 0, characters may start take stamina damage depending on server settings.)
			maxLength = 1;
			maxVal = 6;
			break;
		case charProp.thirstWildChance:
			propertyName = "thirstWildChance";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8416, pSocket.language ); // Chance for extremely thirsty pet to go wild with every NPC AI loop
			maxLength = 3;
			maxVal = 100;
			break;
		case charProp.townPriv:
			propertyName = "townPriv";
			propertyHint = GetDictionaryEntry( 8419, pSocket.language ); // The privileges the character has with their town (1 = Resident, 2 = Mayor)
			propertyType = "Integer";
			maxLength = 3;
			maxVal = 127;
			break;
		case charProp.visible:
			propertyName = "visible";
			propertyHint = GetDictionaryEntry( 8421, pSocket.language ); // Determines visibility of character to other players (0 = Visible, 1 = Hidden, 2 = Magically Invisible, 3 = Visible to GMs only)
			propertyType = "Integer";
			maxLength = 1;
			maxVal = 3;
			break;
		case charProp.wandertype:
			propertyName = "wandertype";
			propertyHint = GetDictionaryEntry( 8423, pSocket.language ); // Determines wandertype for NPC (0 = None, 2 = Free, 3 = Box, 4 = Circle, 5 = Frozen)
			propertyType = "Integer";
			maxLength = 1;
			maxVal = 7;
			break;
		case charProp.weight:
			propertyName = "weight";
			propertyHint = GetDictionaryEntry( 8424, pSocket.language ); // Total weight of character (100 = 1.0 stone)
			propertyType = "Integer";
			maxLength = 5;
			maxVal = 65535;
			break;
		case charProp.worldnumber:
			propertyName = "worldnumber";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8427, pSocket.language ); // World that character exists in
			maxLength = 3;
			maxVal = 127;
			break;
		case charProp.x:
			propertyName = "x";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8428, pSocket.language ); // X coordinate of character in the world
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.y:
			propertyName = "y";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8429, pSocket.language ); // Y coordinate of character in the world
			maxLength = 5;
			maxVal = 32767;
			break;
		case charProp.z:
			propertyName = "z";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8430, pSocket.language ); // Z coordinate of character in the world
			maxLength = 3;
			maxVal = 127;
			break;
		// Multi Properties
		case multiProp.banX:
			propertyName = "banX";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8501, pSocket.language ); // X coordinate for Multi's ban-location
			maxLength = 5;
			maxVal = 32767;
			break;
		case multiProp.banY:
			propertyName = "banY";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8502, pSocket.language ); // Y coordinate for Multi's ban-location
			maxLength = 5;
			maxVal = 32767;
			break;
		case multiProp.dir:
			propertyName = "dir";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8506, pSocket.language ); // Direction of Multi - used for direction of boats?
			maxLength = 4;
			maxVal = 128;
			break;
		case multiProp.instanceID:
			propertyName = "instanceID";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8510, pSocket.language ); // ID of instance of world that Multi exists in. Objects in different instances will not be able to interact with one another!
			break;
		case multiProp.maxBans:
			propertyName = "maxBans";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8512, pSocket.language ); // Max amount of bans supported by Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.maxFriends:
			propertyName = "maxFriends";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8513, pSocket.language ); // Max amount of friends supported by Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.maxGuests:
			propertyName = "maxGuests";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8514, pSocket.language ); // Max amount of guests supported by Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.maxLockdowns:
			propertyName = "maxLockdowns";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8515, pSocket.language ); // Max amount of lockdowns supported by Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.maxOwners:
			propertyName = "maxOwners";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8516, pSocket.language ); // Max amount of owners supported by Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.maxSecureContainers:
			propertyName = "maxSecureContainers";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8517, pSocket.language ); // Max amount of secure containers supported by Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.maxTrashContainers:
			propertyName = "maxTrashContainers";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8518, pSocket.language ); // Max amount of trash containers supported by Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.maxVendors:
			propertyName = "maxVendors";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8519, pSocket.language ); // Max amount of vendors supported by Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.scripttrigger:
			propertyName = "scripttrigger";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8522, pSocket.language ); // JS Script from jse_fileassociations.scp assigned to Multi
			maxLength = 5;
			maxVal = 65535;
			break;
		case multiProp.visible:
			propertyName = "visible";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8527, pSocket.language ); // Determines who Multi is visible for (0 = Visible, 1 = Hidden, 2 = Magically Invisible, 3 = Visible to GMs only)
			maxLength = 1;
			maxVal = 3;
			break;
		case multiProp.worldnumber:
			propertyName = "worldnumber";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8528, pSocket.language ); // World that Multi exists in
			maxLength = 3;
			maxVal = 127;
			break;
		case multiProp.x:
			propertyName = "x";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8529, pSocket.language ); // X coordinate of Multi in the world
			maxLength = 5;
			maxVal = 32767;
			break;
		case multiProp.y:
			propertyName = "y";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8530, pSocket.language ); // Y coordinate of Multi in the world
			maxLength = 5;
			maxVal = 32767;
			break;
		case multiProp.z:
			propertyName = "z";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8531, pSocket.language ); // Z coordinate of Multi in the world
			maxLength = 3;
			maxVal = 127;
			break;
		// Region Properties
		case regionProp.appearance:
			propertyName = "appearance";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8626, pSocket.language ); // Appearance of world within the Region (0 = Spring, 1 = Summer, 2 = Autumn, 3 = Winter, 4 = Desolation, 5 = Unknown)
			maxLength = 1;
			maxVal = 5;
			break;
		case regionProp.chanceBigOre:
			propertyName = "chanceBigOre";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8606, pSocket.language ); // Chance to find big ore in the Region
			maxLength = 3;
			maxVal = 100;
			break;
		case regionProp.health:
			propertyName = "health";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8607, pSocket.language ); // Health of townstone (if any) in Region
			maxLength = 5;
			maxVal = 32767;
			break;
		case regionProp.instanceID:
			propertyName = "instanceID";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8608, pSocket.language ); // InstanceID region exists in
			maxLength = 5;
			maxVal = 65535;
			break;
		case regionProp.music:
			propertyName = "music";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8613, pSocket.language ); // Music-list for Region, as specified in regions.dfn
			maxLength = 3;
			maxVal = 255;
			break;
		case regionProp.numGuards:
			propertyName = "numGuards";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8614, pSocket.language ); // The number of guards for the town (if any) in the Region
			maxLength = 5;
			maxVal = 65535;
			break;
		case regionProp.race:
			propertyName = "race";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8618, pSocket.language ); // Race associated with the Region
			maxLength = 5;
			maxVal = 65535;
			break;
		case regionProp.reserves:
			propertyName = "reserves";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8619, pSocket.language ); // Resource reserves for town (if any) in the Region
			maxLength = 10;
			maxVal = 4294967295;
			break;
		case regionProp.scriptTrigger:
			propertyName = "scriptTrigger";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8620, pSocket.language ); // Script-trigger assigned to the Region (if any)
			maxLength = 5;
			maxVal = 65535;
			break;
		case regionProp.tax:
			propertyName = "tax";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8621, pSocket.language ); // The amount of gold taxed from citizens of the Town (if any) in this Region
			maxLength = 5;
			maxVal = 32767;
			break;
		case regionProp.taxes:
			propertyName = "taxes";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8622, pSocket.language ); // The gold reserves of the Town (if any) in this Region
			maxLength = 10;
			maxVal = 4294967295;
			break;
		case regionProp.weather:
			propertyName = "weather";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8624, pSocket.language ); // Weather ID from weather.dfn associated with this Region
			maxLength = 5;
			maxVal = 65535;
			break;
		case regionProp.worldNumber:
			propertyName = "worldNumber";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8625, pSocket.language ); // World number that Region exists in
			maxLength = 3
			maxVal = 255;
			break;
		// Account Properties
		case accountProp.timeban:
			propertyName = "timeban";
			propertyType = "Integer";
			propertyHint = GetDictionaryEntry( 8725, pSocket.language ); // How long is the user banned for, in minutes?
			maxLength = 6;
			maxVal = 999999;
			break;

		// SkillValue ------------------------------------------------------
		// Base Skills
		case charSkills.alchemy:
			propertyName = "alchemy";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.anatomy:
			propertyName = "anatomy";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.animallore:
			propertyName = "animallore";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.itemid:
			propertyName = "itemid";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.armslore:
			propertyName = "armslore";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.parrying:
			propertyName = "parrying";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.begging:
			propertyName = "begging";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.blacksmithing:
			propertyName = "blacksmithing";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.bowcraft:
			propertyName = "bowcraft";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.peacemaking:
			propertyName = "peacemaking";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.camping:
			propertyName = "camping";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.carpentry:
			propertyName = "carpentry";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.cartography:
			propertyName = "cartography";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.cooking:
			propertyName = "cooking";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.detectinghidden:
			propertyName = "detectinghidden";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.enticement:
			propertyName = "enticement";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.evaluatingintel:
			propertyName = "evaluatingintel";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.healing:
			propertyName = "healing";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.fishing:
			propertyName = "fishing";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.forensics:
			propertyName = "forensics";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.herding:
			propertyName = "herding";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.hiding:
			propertyName = "hiding";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.provocation:
			propertyName = "provocation";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.inscription:
			propertyName = "inscription";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.lockpicking:
			propertyName = "lockpicking";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.magery:
			propertyName = "magery";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.magicresistance:
			propertyName = "magicresistance";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.tactics:
			propertyName = "tactics";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.snooping:
			propertyName = "snooping";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.musicianship:
			propertyName = "musicianship";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.poisoning:
			propertyName = "poisoning";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.archery:
			propertyName = "archery";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.spiritspeak:
			propertyName = "spiritspeak";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.stealing:
			propertyName = "stealing";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.tailoring:
			propertyName = "tailoring";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.taming:
			propertyName = "taming";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.tasteid:
			propertyName = "tasteid";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.tinkering:
			propertyName = "tinkering";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.tracking:
			propertyName = "tracking";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.veterinary:
			propertyName = "veterinary";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.swordsmanship:
			propertyName = "swordsmanship";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.macefighting:
			propertyName = "macefighting";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.fencing:
			propertyName = "fencing";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.wrestling:
			propertyName = "wrestling";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.lumberjacking:
			propertyName = "lumberjacking";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.mining:
			propertyName = "mining";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.meditation:
			propertyName = "meditation";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.stealth:
			propertyName = "stealth";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.removetrap:
			propertyName = "removetrap";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.necromancy:
			propertyName = "necromancy";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.focus:
			propertyName = "focus";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.chivalry:
			propertyName = "chivalry";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.bushido:
			propertyName = "bushido";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.ninjitsu:
			propertyName = "ninjitsu";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.spellweaving:
			propertyName = "spellweaving";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.imbuing:
			propertyName = "imbuing";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.mysticism:
			propertyName = "mysticism";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;
		case charSkills.throwing:
			propertyName = "throwing";
			propertyType = ( baseSkills ? "BaseSkillValue" : "SkillValue" );
			break;

		// Object ------------------------------------------------------
		case itemProp.container:
			propertyName = "container";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8135, pSocket.language ); // Item/Character the item is contained in/on
		case itemProp.owner:
			propertyName = "owner";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8176, pSocket.language ); // Owner of item"
			break;
		case charProp.accountNum:
			pSocket.currentChar.SetTag( "tweakAccount", true );
			HandleAccountTarget( pSocket, targetObj.account );
			return;
		case charProp.attacker:
			propertyName = "attacker";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8304, pSocket.language ); // Character's current attacker
			break;
		case charProp.guild:
			propertyName = "guild";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8308, pSocket.language ); // Object for guild character belongs to
			break;
		// case charProp.multi:
		// 	propertyName = "multi";
		// 	propertyType = "UOXObject";
		// 	propertyHint = "Object for multi character is currently in";
		// 	break;
		case charProp.multi:
			var multiObj = targetObj.multi;
			if( multiObj == null )
			{
				pSocket.SysMessage( GetDictionaryEntry( 8811, pSocket.language )); // Multi not found. Is character still inside Multi?
				HandleCharTarget( pSocket, targetObj );
			}
			else
			{
				pSocket.tempObj2 = targetObj.multi;
				HandleMultiTarget( pSocket, targetObj.multi );
				return;
			}
			break;
		case charProp.owner:
			propertyName = "owner";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8379, pSocket.language ); // Character's owner, if any
			break;
		case charProp.pack:
			propertyName = "pack";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8380, pSocket.language ); // Object of character's root backpack
			break;
		case charProp.party:
			propertyName = "party";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8381, pSocket.language ); // Object of party character is member of
			break;
		case charProp.region:
			propertyName = "region";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8388, pSocket.language ); // Object of region character is currently in
			break;
		case charProp.target:
			propertyName = "target";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8411, pSocket.language ); // Character's current target
			break;
		case charProp.town:
			propertyName = "town";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8418, pSocket.language ); // The town the player belongs to
			break;
		case charProp.baseskills:
			HandleSkillGump( pSocket, targetObj, true );
			pSocket.xText = "true";
			return;
		case charProp.skills:
			HandleSkillGump( pSocket, targetObj, false );
			pSocket.xText = "false";
			return;
		// Multi Properties
		case multiProp.owner:
			propertyName = "owner";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8520, pSocket.language ); // Object registered as owner of this Multi
			break;
		// Region Properties
		case regionProp.mayor:
			propertyName = "mayor";
			propertyType = "UOXObject";
			propertyHint = GetDictionaryEntry( 8612, pSocket.language ); // Character voted as Mayor of the Town (if any) in this Region
			break;
		// Account Properties
		case accountProp.character1:
			pSocket.currentChar.SetTag( "tweakAccount", null );
			pSocket.tempObj2 = targetObj.character1;
			HandleCharTarget( pSocket, targetObj.character1 );
			break;
		case accountProp.character2:
			pSocket.currentChar.SetTag( "tweakAccount", null );
			pSocket.tempObj2 = targetObj.character2;
			HandleCharTarget( pSocket, targetObj.character2 );
			break;
		case accountProp.character3:
			pSocket.currentChar.SetTag( "tweakAccount", null );
			pSocket.tempObj2 = targetObj.character3;
			HandleCharTarget( pSocket, targetObj.character3 );
			break;
		case accountProp.character4:
			pSocket.currentChar.SetTag( "tweakAccount", null );
			pSocket.tempObj2 = targetObj.character4;
			HandleCharTarget( pSocket, targetObj.character4 );
			break;
		case accountProp.character5:
			pSocket.currentChar.SetTag( "tweakAccount", null );
			pSocket.tempObj2 = targetObj.character5;
			HandleCharTarget( pSocket, targetObj.character5 );
			break;
		case accountProp.character6:
			pSocket.currentChar.SetTag( "tweakAccount", null );
			pSocket.tempObj2 = targetObj.character6;
			HandleCharTarget( pSocket, targetObj.character6 );
			break;
		case accountProp.character7:
			pSocket.currentChar.SetTag( "tweakAccount", null );
			pSocket.tempObj2 = targetObj.character7;
			HandleCharTarget( pSocket, targetObj.character7 );
			break;
		case accountProp.currentChar:
			pSocket.currentChar.SetTag( "tweakAccount", null );
			pSocket.tempObj2 = targetObj.currentChar;
			HandleCharTarget( pSocket, targetObj.currentChar );
			break;

		// Boolean ------------------------------------------------------
		case itemProp.corpse:
			propertyName = "corpse";
			propertyHint = GetDictionaryEntry( 8136, pSocket.language ); // Marks item as corpse
			propertyType = "Boolean";
			break;
		case itemProp.damageHeat:
			propertyName = "damageHeat";
			propertyType = "Boolean";
			propertyHint = GetDictionaryEntry( 8137, pSocket.language ); // Weapon deals Heat/Fire elemental damage (true/false)
			break;
		case itemProp.damageCold:
			propertyName = "damageCold";
			propertyType = "Boolean";
			propertyHint = GetDictionaryEntry( 8138, pSocket.language ); // Weapon deals Cold elemental damage (true/false)
			break;
		case itemProp.damageLight:
			propertyName = "damageLight";
			propertyType = "Boolean";
			propertyHint = GetDictionaryEntry( 8139, pSocket.language ); // Weapon deals Light elemental damage (true/false)
			break;
		case itemProp.damageLightning:
			propertyName = "damageLightning";
			propertyType = "Boolean";
			propertyHint = GetDictionaryEntry( 8140, pSocket.language ); // Weapon deals Lightning elemental damage (true/false)
			break;
		case itemProp.damagePoison:
			propertyName = "damagePoison";
			propertyType = "Boolean";
			propertyHint = GetDictionaryEntry( 8141, pSocket.language ); // Weapon deals Poison elemental damage (true/false)
			break;
		case itemProp.damageRain:
			propertyName = "damageRain";
			propertyType = "Boolean";
			propertyHint = GetDictionaryEntry( 8142, pSocket.language ); // Weapon deals Rain/Water elemental damage (true/false)
			break;
		case itemProp.damageSnow:
			propertyName = "damageSnow";
			propertyType = "Boolean";
			propertyHint = GetDictionaryEntry( 8143, pSocket.language ); // Weapon deals Snow elemental damage (true/false)
			break;
		case itemProp.decayable:
			propertyName = "decayable";
			propertyHint = GetDictionaryEntry( 8144, pSocket.language ); // Is item decayable?
			propertyType = "Boolean";
			break;
		case itemProp.divinelock:
			propertyName = "divinelock";
			propertyHint = GetDictionaryEntry( 8148, pSocket.language ); // Has item been locked by a GM?
			propertyType = "Boolean";
			break;
		case itemProp.isDispellable:
			propertyName = "isDispellable";
			propertyHint = GetDictionaryEntry( 8154, pSocket.language ); // Is item dispellable with Magic Dispel?
			propertyType = "Boolean";
			break;
		case itemProp.isDoorOpen:
			propertyName = "isDoorOpen";
			propertyHint = GetDictionaryEntry( 8155, pSocket.language ); // Is door open?
			propertyType = "Boolean";
			break;
		case itemProp.isDyeable:
			propertyName = "isDyeable";
			propertyHint = GetDictionaryEntry( 8156, pSocket.language ); // Is item dyable?
			propertyType = "Boolean";
			break;
		case itemProp.isGuarded:
			propertyName = "isGuarded";
			propertyHint = GetDictionaryEntry( 8157, pSocket.language ); // Is item guarded by a pet/hireling?
			propertyType = "Boolean";
			break;
		case itemProp.isNewbie:
			propertyName = "isNewbie";
			propertyHint = GetDictionaryEntry( 8158, pSocket.language ); // Is item marked as newbie/blessed?
			propertyType = "Boolean";
			break;
		case itemProp.isPileable:
			propertyName = "isPileable";
			propertyHint = GetDictionaryEntry( 8159, pSocket.language ); // Is item pileable?
			propertyType = "Boolean";
			break;
		case itemProp.isWipeable:
		case itemProp.wipable:
			propertyName = "isWipeable";
			propertyHint = GetDictionaryEntry( 8160, pSocket.language ); // Is item wipeable with WIPE command?
			propertyType = "Boolean";
			break;
		case itemProp.sectionalist:
			propertyName = "sectionalist";
			propertyHint = GetDictionaryEntry( 8190, pSocket.language ); // True if spawn section is a list - SpawnObject only
			propertyType = "Boolean";
			break;
		// Character Properties
		case charProp.allmove:
			propertyName = "allmove";
			propertyHint = GetDictionaryEntry( 8302, pSocket.language ); // Toggles being able to move all items regardless of movable status
			propertyType = "Boolean";
			break;
		case charProp.attackFirst:
			propertyName = "attackFirst";
			propertyHint = GetDictionaryEntry( 8305, pSocket.language ); // Did character attack first in combat?
			propertyType = "Boolean";
			break;
		case charProp.atWar:
			propertyName = "atWar";
			propertyHint = GetDictionaryEntry( 8306, pSocket.language ); // Toggles combat mode for NPC characters
			propertyType = "Boolean";
			break;
		case charProp.canAttack:
			propertyName = "canAttack";
			propertyHint = GetDictionaryEntry( 8309, pSocket.language ); // Toggles whether character can attack other characters
			propertyType = "Boolean";
			break;
		case charProp.canBroadcast:
			propertyName = "canBroadcast";
			propertyHint = GetDictionaryEntry( 8310, pSocket.language ); // Toggles whether character can broadcast messages
			propertyType = "Boolean";
			break;
		case charProp.canRun:
			propertyName = "canRun";
			propertyHint = GetDictionaryEntry( 8311, pSocket.language ); // Toggles whether NPC can run
			propertyType = "Boolean";
			break;
		case charProp.canSnoop:
			propertyName = "canSnoop";
			propertyHint = GetDictionaryEntry( 8312, pSocket.language ); // Toggles whether character can snoop in other characters' backpacks
			propertyType = "Boolean";
			break;
		case charProp.criminal:
			propertyName = "criminal";
			propertyHint = GetDictionaryEntry( 8317, pSocket.language ); // Is character flagged as a criminal?
			propertyType = "Boolean";
			break;
		case charProp.dead:
			propertyName = "dead";
			propertyHint = GetDictionaryEntry( 8318, pSocket.language ); // Is (player) character dead?
			propertyType = "Boolean";
			break;
		case charProp.frozen:
			propertyName = "frozen";
			propertyHint = GetDictionaryEntry( 8326, pSocket.language ); // Toggles whether object is frozen (immovable) or not
			propertyType = "Boolean";
			break;
		case charProp.houseicons:
			propertyName = "houseicons";
			propertyHint = GetDictionaryEntry( 8332, pSocket.language ); // Toggles whether house icons/deeds are shown instead of multis for character
			propertyType = "Boolean";
			break;
		case charProp.innocent:
			propertyName = "innocent";
			propertyHint = GetDictionaryEntry( 8337, pSocket.language ); // Is character flagged as innocent?
			propertyType = "Boolean";
			break;
		case charProp.isAnimal:
			propertyName = "isAnimal";
			propertyHint = GetDictionaryEntry( 8340, pSocket.language ); // Is character an animal?
			propertyType = "Boolean";
			break;
		case charProp.isCasting:
			propertyName = "isCasting";
			propertyHint = GetDictionaryEntry( 8341, pSocket.language ); // Is character casting a spell? Automatically handled by magic system.
			propertyType = "Boolean";
			break;
		case charProp.isCounselor:
			propertyName = "isCounselor";
			propertyHint = GetDictionaryEntry( 8342, pSocket.language ); // Is character a counselor?
			propertyType = "Boolean";
			break;
		case charProp.isDispellable:
			propertyName = "isDispellable";
			propertyHint = GetDictionaryEntry( 8343, pSocket.language ); // Is character dispellable?
			propertyType = "Boolean";
			break;
		case charProp.isflying:
			propertyName = "isflying";
			propertyHint = GetDictionaryEntry( 8344, pSocket.language ); // Is character flying?
			propertyType = "Boolean";
			break;
		case charProp.isGM:
			propertyName = "isGM";
			propertyHint = GetDictionaryEntry( 8346, pSocket.language ); // Is character a GM?
			propertyType = "Boolean";
			break;
		case charProp.isGMPageable:
			propertyName = "isGMPageable";
			propertyHint = GetDictionaryEntry( 8347, pSocket.language ); // Is character able to respond to GM pages?
			propertyType = "Boolean";
			break;
		case charProp.isHuman:
			propertyName = "isHuman";
			propertyHint = GetDictionaryEntry( 8348, pSocket.language ); // Is character human?
			propertyType = "Boolean";
			break;
		case charProp.isIncognito:
			propertyName = "isIncognito";
			propertyHint = GetDictionaryEntry( 8349, pSocket.language ); // Is character incognito?
			propertyType = "Boolean";
			break;
		case charProp.isJailed:
			propertyName = "isJailed";
			propertyHint = GetDictionaryEntry( 8350, pSocket.language ); // Is character jailed?
			propertyType = "Boolean";
			break;
		case charProp.isMeditating:
			propertyName = "isMeditating";
			propertyHint = GetDictionaryEntry( 8351, pSocket.language ); // Is character meditating?
			propertyType = "Boolean";
			break;
		case charProp.isonhorse:
			propertyName = "isonhorse";
			propertyHint = GetDictionaryEntry( 8352, pSocket.language ); // Is character on a mount?
			propertyType = "Boolean";
			break;
		case charProp.isPolymorphed:
			propertyName = "isPolymorphed";
			propertyHint = GetDictionaryEntry( 8353, pSocket.language ); // Is character polymorphed?
			propertyType = "Boolean";
			break;
		case charProp.isShop:
			propertyName = "isShop";
			propertyHint = GetDictionaryEntry( 8354, pSocket.language ); // Is NPC a shopkeeper?
			propertyType = "Boolean";
			break;
		case charProp.isUsingPotion:
			propertyName = "isUsingPotion";
			propertyHint = GetDictionaryEntry( 8355, pSocket.language ); // Is character using a potion?
			propertyType = "Boolean";
			break;
		case charProp.magicReflect:
			propertyName = "magicReflect";
			propertyHint = GetDictionaryEntry( 8359, pSocket.language ); // Is magic reflection active for character?
			propertyType = "Boolean";
			break;
		case charProp.mounted:
			propertyName = "mounted";
			propertyHint = GetDictionaryEntry( 8364, pSocket.language ); // Is mount carrying someone?
			propertyType = "Boolean";
			break;
		case charProp.murderer:
			propertyName = "murderer";
			propertyHint = GetDictionaryEntry( 8367, pSocket.language ); // Is character flagged as a murderer?
			propertyType = "Boolean";
			break;
		case charProp.neutral:
			propertyName = "neutral";
			propertyHint = GetDictionaryEntry( 8368, pSocket.language ); // Is character neutrally flagged?
			propertyType = "Boolean";
			break;
		case charProp.noNeedMana:
			propertyName = "noNeedMana";
			propertyHint = GetDictionaryEntry( 8370, pSocket.language ); // Mana is not needed for character to cast spells
			propertyType = "Boolean";
			break;
		case charProp.noNeedReags:
			propertyName = "noNeedReags";
			propertyHint = GetDictionaryEntry( 8371, pSocket.language ); // Reagents are not needed for character to cast spells
			propertyType = "Boolean";
			break;
		case charProp.noSkillTitles:
			propertyName = "noSkillTitles";
			propertyHint = GetDictionaryEntry( 8372, pSocket.language ); // Skill titles are not displayed for character
			propertyType = "Boolean";
			break;
		case charProp.npc:
			propertyName = "npc";
			propertyHint = GetDictionaryEntry( 8373, pSocket.language ); // Is character an NPC?
			propertyType = "Boolean";
			break;
		case charProp.online:
			propertyName = "online";
			propertyHint = GetDictionaryEntry( 8375, pSocket.language ); // Is (player) character online?
			propertyType = "Boolean";
			break;
		case charProp.partyLootable:
			propertyName = "partyLootable";
			propertyHint = GetDictionaryEntry( 8382, pSocket.language ); // Is character lootable by party members when dead?
			propertyType = "Boolean";
			break;
		case charProp.singClickSer:
			propertyName = "singClickSer";
			propertyHint = GetDictionaryEntry( 8391, pSocket.language ); // Toggles whether single-clicks shows serial of clicked object
			propertyType = "Boolean";
			break;
		case charProp.squelch:
			propertyName = "squelch";
			propertyHint = GetDictionaryEntry( 8403, pSocket.language ); // Toggles whether character is squelched/muted
			propertyType = "Boolean";
			break;
		case charProp.stabled:
			propertyName = "stabled";
			propertyHint = GetDictionaryEntry( 8404, pSocket.language ); // Is pet stabled?
			propertyType = "Boolean";
			break;
		case charProp.tamed:
			propertyName = "tamed";
			propertyHint = GetDictionaryEntry( 8408, pSocket.language ); // Is NPC tame?
			propertyType = "Boolean";
			break;
		case charProp.trainer:
			propertyName = "trainer";
			propertyHint = GetDictionaryEntry( 8420, pSocket.language ); // Is NPC a skill-trainer?
			propertyType = "Boolean";
			break;
		case charProp.vulnerable:
			propertyName = "vulnerable";
			propertyHint = GetDictionaryEntry( 8422, pSocket.language ); // Is character vulnerable to damage?
			propertyType = "Boolean";
			break;
		case charProp.willhunger:
			propertyName = "willhunger";
			propertyHint = GetDictionaryEntry( 8425, pSocket.language ); // Enables (true) or disables (false) hunger-mode
			propertyType = "Boolean";
			break;
		case charProp.willthirst:
			propertyName = "willthirst";
			propertyHint = GetDictionaryEntry( 8426, pSocket.language ); // Enables (true) or disables (false) thirst-mode
			propertyType = "Boolean";
			break;
		// Multi Properties
		case multiProp.isPublic:
			propertyName = "isPublic";
			propertyHint = GetDictionaryEntry( 8509, pSocket.language ); // Is the Multi flagged as Public?
			propertyType = "Boolean";
			break;
		// Region Properties
		case regionProp.canCastAggressive:
			propertyName = "canCastAggressive";
			propertyHint = GetDictionaryEntry( 8600, pSocket.language ); // Can aggressive spells be cast in the Region?
			propertyType = "Boolean";
			break;
		case regionProp.canGate:
			propertyName = "canGate";
			propertyHint = GetDictionaryEntry( 8601, pSocket.language ); // Can the Gate spell be used in the Region?
			propertyType = "Boolean";
			break;
		case regionProp.canMark:
			propertyName = "canMark";
			propertyHint = GetDictionaryEntry( 8602, pSocket.language ); // Can the Mark spell be used in the Region?
			propertyType = "Boolean";
			break;
		case regionProp.canPlaceHouse:
			propertyName = "canPlaceHouse";
			propertyHint = GetDictionaryEntry( 8603, pSocket.language ); // Can players place houses in the Region?
			propertyType = "Boolean";
			break;
		case regionProp.canRecall:
			propertyName = "canRecall";
			propertyHint = GetDictionaryEntry( 8604, pSocket.language ); // Can the Recall spell be used in the Region?
			propertyType = "Boolean";
			break;
		case regionProp.canTeleport:
			propertyName = "canTeleport";
			propertyHint = GetDictionaryEntry( 8605, pSocket.language ); // Can the Teleport spell be used in the Region?
			propertyType = "Boolean";
			break;
		case regionProp.isDungeon:
			propertyName = "isDungeon";
			propertyHint = GetDictionaryEntry( 8609, pSocket.language ); // Is Region considered a dungeon?
			propertyType = "Boolean";
			break;
		case regionProp.isGuarded:
			propertyName = "isGuarded";
			propertyHint = GetDictionaryEntry( 8610, pSocket.language ); // Is Region protected by guards?
			propertyType = "Boolean";
			break;
		case regionProp.isSafeZone:
			propertyName = "isSafeZone";
			propertyHint = GetDictionaryEntry( 8611, pSocket.language ); // Is Region considered safe for players?
			propertyType = "Boolean";
			break;
		// Account Properties
		case accountProp.isBanned:
			propertyName = "isBanned";
			propertyHint = GetDictionaryEntry( 8711, pSocket.language ); // Is this Account banned?
			propertyType = "Boolean";
			break;
		case accountProp.isCounselor:
			propertyName = "isCounselor";
			propertyHint = GetDictionaryEntry( 8712, pSocket.language ); // Is this Account marked as a Counselor account?
			propertyType = "Boolean";
			break;
		case accountProp.isGM:
			propertyName = "isGM";
			propertyHint = GetDictionaryEntry( 8713, pSocket.language ); // Is this Account marked as a GM account?
			propertyType = "Boolean";
			break;
		case accountProp.isPublic:
			propertyName = "isPublic";
			propertyHint = GetDictionaryEntry( 8715, pSocket.language ); // Is contact/comments for this account marked as public? (does nothing atm)
			propertyType = "Boolean";
			break;
		case accountProp.isSeer:
			propertyName = "isSeer";
			propertyHint = GetDictionaryEntry( 8716, pSocket.language ); // Is this Account marked as a Seer account?
			propertyType = "Boolean";
			break;
		case accountProp.isSlot1Blocked:
			propertyName = "isSlot1Blocked";
			propertyHint = GetDictionaryEntry( 8717, pSocket.language ); // Is slot 1 on this Account blocked?
			propertyType = "Boolean";
			break;
		case accountProp.isSlot2Blocked:
			propertyName = "isSlot2Blocked";
			propertyHint = GetDictionaryEntry( 8718, pSocket.language ); // Is slot 2 on this Account blocked?
			propertyType = "Boolean";
			break;
		case accountProp.isSlot3Blocked:
			propertyName = "isSlot3Blocked";
			propertyHint = GetDictionaryEntry( 8719, pSocket.language ); // Is slot 3 on this Account blocked?
			propertyType = "Boolean";
			break;
		case accountProp.isSlot4Blocked:
			propertyName = "isSlot4Blocked";
			propertyHint = GetDictionaryEntry( 8720, pSocket.language ); // Is slot 4 on this Account blocked?
			propertyType = "Boolean";
			break;
		case accountProp.isSlot5Blocked:
			propertyName = "isSlot5Blocked";
			propertyHint = GetDictionaryEntry( 8721, pSocket.language ); // Is slot 5 on this Account blocked?
			propertyType = "Boolean";
			break;
		case accountProp.isSlot6Blocked:
			propertyName = "isSlot6Blocked";
			propertyHint = GetDictionaryEntry( 8722, pSocket.language ); // Is slot 6 on this Account blocked?
			propertyType = "Boolean";
			break;
		case accountProp.isSlot7Blocked:
			propertyName = "isSlot7Blocked";
			propertyHint = GetDictionaryEntry( 8724, pSocket.language ); // Is slot 7 on this Account blocked?
			propertyType = "Boolean";
			break;
		case accountProp.isSuspended:
			propertyName = "isSuspended";
			propertyHint = GetDictionaryEntry( 8723, pSocket.language ); // Is this Account suspended?
			propertyType = "Boolean";
			break;

		// Timer ------------------------------------------------------
		case itemProp.decaytime:
			propertyName = "decaytime";
			propertyType = "Timer";
			propertyHint = GetDictionaryEntry( 8145, pSocket.language ); // Time in seconds for item to decay
			break;
		default:
			break;
	}

	switch( propertyType )
	{
		case "SkillValue":
		case "BaseSkillValue":
			if( maxLength == 0 )
			{
				maxLength = 5
			}
			if( maxVal == 0 )
			{
				maxVal = 120.0;
			}

			if( baseSkills )
			{
				propertyHint = GetDictionaryEntry( 8812, pSocket.language ); // Base skill, before bonuses from stats and items are applied.
			}
			else
			{
				propertyHint = GetDictionaryEntry( 8813, pSocket.language ); // Effective skill, after bonuses from stats and items are applied.
			}

			if( !modifyData )
			{
				ShowInputGump( pSocket.currentChar, targetObj, propertyName, propertyType, maxLength, maxVal, propertyHint );
			}
			else
			{
				var targetValue = parseFloat( gumpData.getEdit( 0 ));
				if( targetValue !== targetValue )
				{
					pSocket.SysMessage( GetDictionaryEntry( 8814, pSocket.language )); // The entered value is not a valid number.
				}
				else
				{
					if( baseSkills )
					{
						targetObj.baseskills[propertyName] = targetValue * 10;
					}
					else
					{
						targetObj.skills[propertyName] = targetValue * 10;
					}
				}

				if( ValidateObject( targetObj ))
				{
					if( targetObj.isItem )
					{
						HandleItemTarget( pSocket, targetObj );
					}
					else
					{
						HandleCharTarget( pSocket, targetObj );
					}
				}
			}

			break;
		case "Hexadecimal":
			if( maxLength == 0 )
			{
				maxLength = 5;
			}
			if( maxVal == 0 )
			{
				maxVal = "0xffff";
			}

			if( !modifyData )
			{
				ShowInputGump( pSocket.currentChar, targetObj, propertyName, propertyType, maxLength, maxVal, propertyHint );
			}
			else
			{
				targetObj[propertyName] = parseInt( gumpData.getEdit( 0 ));
				if( ValidateObject( targetObj ))
				{
					if( targetObj.isItem && targetObj.IsMulti() )
					{
						HandleMultiTarget( pSocket, targetObj );
					}
					else if( targetObj.isItem )
					{
						HandleItemTarget( pSocket, targetObj );
					}
					else
					{
						HandleCharTarget( pSocket, targetObj );
					}
				}
				else if( targetObj != null && "username" in targetObj )
				{
					HandleAccountTarget( pSocket, targetObj );
				}
				else
				{
					HandleRegionTarget( pSocket, targetObj );
				}
			}
			break;
		case "Integer":
			if( maxLength == 0 )
			{
				maxLength = 5
			}
			if( maxVal == 0 )
			{
				maxVal = 65535;
			}

			if( !modifyData )
			{
				ShowInputGump( pSocket.currentChar, targetObj, propertyName, propertyType, maxLength, maxVal, propertyHint );
			}
			else
			{
				var targetValue = parseInt( gumpData.getEdit( 0 ));
				if( targetValue !== targetValue )
				{
					pSocket.SysMessage( GetDictionaryEntry( 8814, pSocket.language )); // The entered value is not a valid number.
				}
				else
				{
					if( pButton == charProp.commandlevel && targetValue >= pSocket.currentChar.commandlevel )
					{
						pSocket.SysMessage( GetDictionaryEntry( 8815, pSocket.language )); // You cannot give someone a command level higher or equal to your own!
					}
					else if( pButton == itemProp.type && (( targetValue >= 61 && targetValue <= 65 ) || targetValue == 69 || targetValue == 125 ) && !targetObj.isSpawner )
					{
						pSocket.SysMessage( GetDictionaryEntry( 8816, pSocket.language )); // This item type can only be set on spawner objects added with the 'add spawner # command.
					}
					else
					{
						var oldValue = parseInt( targetObj[propertyName] );
						targetObj[propertyName] = targetValue;

						if( propertyName == "isBanned" )
						{
							if( targetValue > 0 )
							{
								// If marked as banned, also apply a default timeban of 24 hours, and disconnect player
								targetObj.timeban = 60 * 24;
								if( targetObj.isOnline && ValidateObject( targetObj.currentChar ) && targetObj.currentChar.socket != null )
								{
									targetObj.currentChar.Disconnect();
								}
							}
							else
							{
								// If marked as not banned, also reset timeban value
								targetObj.timeban = 0;
							}
						}
						else if( propertyName == "timeban" && targetValue > 0 )
						{
							// Also mark user as banned, and disconnect them!
							targetObj.isBanned = 1;
							if( targetObj.isOnline && ValidateObject( targetObj.currentChar ) && targetObj.currentChar.socket != null )
							{
								targetObj.currentChar.Disconnect();
							}
						}
						else if( propertyName == "movable" )
						{
							targetObj.Refresh();
						}

						if( ValidateObject( targetObj ))
						{
							if( targetObj.isItem && targetObj.IsMulti() )
							{
								HandleMultiTarget( pSocket, targetObj );
							}
							else if( targetObj.isItem )
							{
								HandleItemTarget( pSocket, targetObj );
							}
							else
							{
								HandleCharTarget( pSocket, targetObj );
							}
						}
						else if( targetObj != null && "username" in targetObj )
						{
							HandleAccountTarget( pSocket, targetObj );
						}
						else
						{
							HandleRegionTarget( pSocket, targetObj );
						}

						// Somehow, if this section comes before the re-opening of the gump, the context
						// of the object calling the gump changes to the house...
						if( pButton == multiProp.x || pButton == multiProp.y || pButton == multiProp.z || pButton == multiProp.worldnumber || pButton == multiProp.instanceID )
						{
							// Apply any changes made to the location/position in the world for a Multi to all items and characters contained within!
							if( targetObj != null && targetObj.isItem && targetObj.IsMulti() )
							{
								// Move all items inside the Multi along with the Multi
								var itemInMulti;
								for( itemInMulti = targetObj.FirstItem(); !targetObj.FinishedItems(); itemInMulti = targetObj.NextItem() )
								{
									if( !ValidateObject( itemInMulti ))
										continue;

									if( !ValidateObject( itemInMulti.multi ))
										continue;

									if( pButton == multiProp.worldnumber || pButton == multiProp.instanceID )
									{
										itemInMulti[propertyName] = targetValue;
									}
									else
									{
										itemInMulti[propertyName] += ( targetValue - oldValue );
									}
								}

								// Now do the same to any characters within the Multi
								var charInMulti;
								for( charInMulti = targetObj.FirstChar(); !targetObj.FinishedChars(); charInMulti = targetObj.NextChar() )
								{
									if( !ValidateObject( charInMulti ))
										continue;

									if( !ValidateObject( charInMulti.multi ))
										continue;

									if( pButton == multiProp.worldnumber || pButton == multiProp.instanceID )
									{
										charInMulti[propertyName] = targetValue;
									}
									else
									{
										charInMulti[propertyName] += ( targetValue - oldValue );
									}
								}
							}
						}
					}
				}
			}

			break;
		case "Text":
			if( maxLength == 0 )
			{
				maxLength = 127;
			}

			if( !modifyData )
			{
				ShowInputGump( pSocket.currentChar, targetObj, propertyName, propertyType, maxLength, maxVal, propertyHint );
			}
			else
			{
				var newVal = ( gumpData.getEdit( 0 )).toString();
				if( newVal != "-" )
				{
					targetObj[propertyName] = newVal;
				}
				if( ValidateObject( targetObj ))
				{
					if( targetObj.isItem && targetObj.IsMulti() )
					{
						HandleMultiTarget( pSocket, targetObj );
					}
					else if( targetObj.isItem )
					{
						HandleItemTarget( pSocket, targetObj );
					}
					else
					{
						HandleCharTarget( pSocket, targetObj );
					}
				}
				else if( targetObj != null && "username" in targetObj )
				{
					HandleAccountTarget( pSocket, targetObj );
				}
				else
				{
					HandleRegionTarget( pSocket, targetObj );
				}
			}
			break;
		case "Boolean":
			if( maxLength == 0 )
			{
				maxLength = 1
			}
			if( maxVal == 0 )
			{
				maxVal = 1;
			}

			if( !modifyData )
			{
				ShowInputGump( pSocket.currentChar, targetObj, propertyName, propertyType, maxLength, maxVal, propertyHint );
			}
			else
			{
				if( pButton == charProp.isGM && pSocket.currentChar.commandlevel < 5 )
				{
					pSocket.SysMessage( GetDictionaryEntry( 8817, pSocket.language )); // Only Admins can modify someone's GM status!
				}
				else
				{
					targetObj[propertyName] = !gumpData.getButton( 0 );
				}

				if( ValidateObject( targetObj ))
				{
					if( targetObj.isItem && targetObj.IsMulti() )
					{
						HandleMultiTarget( pSocket, targetObj );
					}
					else if( targetObj.isItem )
					{
						HandleItemTarget( pSocket, targetObj );
					}
					else
					{
						HandleCharTarget( pSocket, targetObj );
					}
				}
				else if( targetObj != null && "username" in targetObj )
				{
					HandleAccountTarget( pSocket, targetObj );
				}
				else
				{
					HandleRegionTarget( pSocket, targetObj );
				}
			}
			break;
		case "Timer":
			if( maxLength == 0 )
			{
				maxLength = 10
			}

			if( !modifyData )
			{
				ShowInputGump( pSocket.currentChar, targetObj, propertyName, propertyType, maxLength, maxVal, propertyHint );
			}
			else
			{
				targetObj[propertyName] = parseInt( gumpData.getEdit( 0 ));
				if( ValidateObject( targetObj ))
				{
					if( targetObj.isItem && targetObj.IsMulti() )
					{
						HandleMultiTarget( pSocket, targetObj );
					}
					else if( targetObj.isItem )
					{
						HandleItemTarget( pSocket, targetObj );
					}
					else
					{
						HandleCharTarget( pSocket, targetObj );
					}
				}
				else if( targetObj != null && "username" in targetObj )
				{
					HandleAccountTarget( pSocket, targetObj );
				}
				else
				{
					HandleRegionTarget( pSocket, targetObj );
				}
			}
			break;
		case "UOXObject":
			if( maxLength == 0 )
			{
				maxLength = 20;
			}
			if( !modifyData )
			{
				ShowInputGump( pSocket.currentChar, targetObj, propertyName, propertyType, maxLength, maxVal, propertyHint );
			}
			else
			{
				if( clearObject )
				{
					// Clearing object value
					targetObj[propertyName] = null;
				}
				else if( selectNewObject )
				{
					// Select new object
					pSocket.CustomTarget( 1, GetDictionaryEntry( 8818, pSocket.language )); // Select new object:
					return;
				}
				//targetObj[propertyName] = parseInt(gumpData.getEdit( 0 ));
				if( ValidateObject( targetObj ))
				{
					if( targetObj.isItem && targetObj.IsMulti() )
					{
						HandleMultiTarget( pSocket, targetObj );
					}
					else if( targetObj.isItem )
					{
						HandleItemTarget( pSocket, targetObj );
					}
					else
					{
						HandleCharTarget( pSocket, targetObj );
					}
				}
				else if( targetObj != null && "username" in targetObj )
				{
					HandleAccountTarget( pSocket, targetObj );
				}
				else
				{
					HandleRegionTarget( pSocket, targetObj );
				}
			}
			break;
		default:
			break;
	}
}

// Close gump if already open
function CloseGump( pSocket, gumpID )
{
	// Create packet to force-close gump
	var pStream = new Packet;
	pStream.ReserveSize( 13 );
	pStream.WriteByte( 0, 0xBF ); // Command: Packet 0xBF - General Information Packet
	pStream.WriteShort( 1, 13 ); // Packet length
	pStream.WriteShort( 3, 0x04 ); // Subcommand 0x04 - Close Generic Gump
	pStream.WriteLong( 5, gumpID ); // dialogID - which gump to destroy
	pStream.WriteLong( 9, 0 );      // buttonID // response buttonID for packet 0xB1
	pSocket.Send( pStream );
	pStream.Free();
}

// List of AI types
const aiTypeName = {
	0:"AI_NONE",
	1:"AI_HEALER_G",
	2:"AI_EVIL",
	4:"AI_GUARD",
	5:"AI_FIGHTER",
	6:"AI_ANIMAL",
	7:"AI_DUMMY",
	8:"AI_BANKER",
	17:"AI_PLAYERVENDOR",
	32:"AI_PET_GUARD",
	88:"AI_CHAOTIC",
	666:"AI_HEALER_E",
}

// List of item layers
const layerName = {
	0:"None",
	1:"(Right Hand) Single-Hand item/weapon",
	2:"(Left Hand) Two-Hand item/weapon (including Shield)",
	3:"Footwear/Foot Covering/Armor",
	4:"Leg Covering (including Pants, Shorts, Bone/Chain/Ring legs)",
	5:"Chest Clothing/Female Chest Armor",
	6:"Head Covering/Helmet",
	7:"Hand Covering/Armor",
	8:"Ring",
	9:"Talisman",
	10:"Neck Covering/Armor",
	11:"Hair",
	12:"Waist (Half-Apron)",
	13:"Torso (inner)(Chest Armor)",
	14:"Bracelet",
	15:"Face",
	16:"Facial Hair",
	17:"Torso (Middle)(Surcoat, Tunic, Full Apron, Sash)",
	18:"Earrings",
	19:"Arm Covering/Armor",
	20:"Back (Cloak)",
	21:"BackPack",
	22:"Torso (outer)(Robe)",
	23:"Legs (outer)(Skirt/Kilt)",
	24:"Legs (inner)(Leg Armor)",
	25:"Mount (Horse, Ostard, etc.)",
	26:"(Sell container) NPC Buy Restock Container",
	27:"(Bought container) NPC Buy No Restock Container",
	28:"(Buy container) NPC Sell Container",
	29:"Bank Box"
}

// List of item types
const itemTypeNames = {
	0:"IT_NOTYPE",
	1:"IT_CONTAINER",
	2:"IT_CASTLEGATEOPENER",
	3:"IT_CASTLEGATE",
	6:"IT_TELEPORTITEM",
	7:"IT_KEY",
	8:"IT_LOCKEDCONTAINER",
	9:"IT_SPELLBOOK",
	10:"IT_MAP",
	11:"IT_BOOK",
	12:"IT_DOOR",
	13:"IT_LOCKEDDOOR",
	14:"IT_FOOD",
	15:"IT_MAGICWAND",
	16:"IT_RESURRECTOBJECT",
	17:"IT_CRYSTALBALL",
	18:"IT_POTION",
	19:"IT_TRADEWINDOW",
	35:"IT_TOWNSTONE",
	50:"IT_RECALLRUNE",
	51:"IT_GATE",
	60:"IT_OBJTELEPORTER",
	61:"IT_ITEMSPAWNER",
	62:"IT_NPCSPAWNER",
	63:"IT_SPAWNCONT",
	64:"IT_LOCKEDSPAWNCONT",
	65:"IT_UNLOCKABLESPAWNCONT",
	69:"IT_AREASPAWNER",
	80:"IT_ADVANCEGATE",
	81:"IT_MULTIADVANCEGATE",
	82:"IT_MONSTERGATE",
	83:"IT_RACEGATE",
	85:"IT_DAMAGEOBJECT",
	87:"IT_TRASHCONT",
	88:"IT_SOUNDOBJECT",
	89:"IT_MAPCHANGEOBJECT",
	90:"IT_WORLDCHANGEGATE",
	101:"IT_MORPHOBJECT",
	102:"IT_UNMORPHOBJECT",
	105:"IT_DRINK",
	106:"IT_STANDINGHARP",
	111:"IT_ZEROKILLSGATE",
	117:"IT_PLANK",
	118:"IT_FIREWORKSWAND",
	125:"IT_ESCORTNPCSPAWNER",
	186:"IT_RENAMEDEED",
	190:"IT_LEATHERREPAIRTOOL",
	191:"IT_BOWREPAIRTOOL",
	200:"IT_TILLER",
	201:"IT_HOUSEADDON",
	202:"IT_GUILDSTONE",
	203:"IT_HOUSESIGN",
	204:"IT_TINKERTOOL",
	205:"IT_METALREPAIRTOOL",
	207:"IT_FORGE",
	208:"IT_DYE",
	209:"IT_DYEVAT",
	210:"IT_MODELMULTI",
	211:"IT_ARCHERYBUTTE",
	212:"IT_DRUM",
	213:"IT_TAMBOURINE",
	214:"IT_HARP",
	215:"IT_LUTE",
	216:"IT_AXE",
	217:"IT_PLAYERVENDORDEED",
	218:"IT_SMITHYTOOL",
	219:"IT_CARPENTRYTOOL",
	220:"IT_MININGTOOL",
	221:"IT_EMPTYVIAL",
	222:"IT_UNSPUNFABRIC",
	223:"IT_UNCOOKEDFISH",
	224:"IT_UNCOOKEDMEAT",
	225:"IT_SPUNFABRIC",
	226:"IT_FLETCHINGTOOL",
	227:"IT_CANNONBALL",
	228:"IT_WATERPITCHER",
	229:"IT_UNCOOKEDDOUGH",
	230:"IT_SEWINGKIT",
	231:"IT_ORE",
	232:"IT_MESSAGEBOARD",
	233:"IT_SWORD",
	234:"IT_CAMPING",
	235:"IT_MAGICSTATUE",
	236:"IT_GUILLOTINE",
	238:"IT_FLOURSACK",
	239:"IT_OPENFLOURSACK",
	240:"IT_FISHINGPOLE",
	241:"IT_CLOCK",
	242:"IT_MORTAR",
	243:"IT_SCISSORS",
	244:"IT_BANDAGE",
	245:"IT_SEXTANT",
	246:"IT_HAIRDYE",
	247:"IT_LOCKPICK",
	248:"IT_COTTONPLANT",
	249:"IT_TINKERAXLE",
	250:"IT_TINKERAWG",
	251:"IT_TINKERCLOCK",
	252:"IT_TINKERSEXTANT",
	253:"IT_TRAININGDUMMY",
	255:"IT_COUNT"
}