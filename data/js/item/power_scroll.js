/// <reference path="../definitions.d.ts" />
// @ts-check
//
// powerscrolls.js (script id 7504)
//
// New DFN storage style only (no tags):
//   morex = skills.dfn [SKILL X] index (example: 40 = Swordsmanship)
//   morey = bonus (5/10/15/20/25)

var powerRarityByBonus = {
	5:  "wonderous",
	10: "exalted",
	15: "mythical",
	20: "legendary",
	25: "ultimate"
};

var powerSkillDisplay = {
	swordsmanship:    "Swordsmanship",
	fencing:          "Fencing",
	macefighting:     "Mace Fighting",
	archery:          "Archery",
	wrestling:        "Wrestling",
	parrying:         "Parrying",
	tactics:          "Tactics",
	anatomy:          "Anatomy",
	healing:          "Healing",
	magery:           "Magery",
	meditation:       "Meditation",
	evaluatingintel:  "Evaluate Intelligence",
	magicresistance:  "Magic Resistance",
	stealing:         "Stealing",
	throwing:         "Throwing",
	stealth:          "Stealth",
	taming:           "Animal Taming",
	animallore:       "Animal Lore",
	veterinary:       "Veterinary",
	musicianship:     "Musicianship",
	provocation:      "Provocation",
	discordance:      "Discordance",
	peacemaking:      "Peacemaking",
	chivalry:         "Chivalry",
	focus:            "Focus",
	necromancy:       "Necromancy",
	bushido:          "Bushido",
	ninjitsu:         "Ninjitsu",
	spellweaving:     "Spellweaving",
	spiritspeak:      "Spirit Speak",
	mysticism:        "Mysticism",
	imbuing:          "Imbuing",
	alchemy:          "Alchemy",
	armslore:         "Arms Lore",
	itemid:           "Item Identification",
	blacksmithing:    "Blacksmithing",
	bowcraft:         "Bowcraft/Fletching",
	camping:          "Camping",
	carpentry:        "Carpentry",
	cartography:      "Cartography",
	cooking:          "Cooking",
	detectinghidden:  "Detecting Hidden",
	enticement:       "Enticement",
	forensics:        "Forensic Evaluation",
	herding:          "Herding",
	hiding:           "Hiding",
	inscription:      "Inscription",
	lockpicking:      "Lockpicking",
	mining:           "Mining",
	lumberjacking:    "Lumberjacking",
	poisoning:        "Poisoning",
	removetrap:       "Remove Trap",
	snooping:         "Snooping",
	tailoring:        "Tailoring",
	tasteid:          "Taste Identification",
	tinkering:        "Tinkering",
	tracking:         "Tracking",
	fishing:          "Fishing",
	begging:          "Begging"
};

var powerSkillById = {
	0:  "alchemy",
	1:  "anatomy",
	2:  "animallore",
	3:  "itemid",
	4:  "armslore",
	5:  "parrying",
	6:  "begging",
	7:  "blacksmithing",
	8:  "bowcraft",
	9:  "peacemaking",
	10: "camping",
	11: "carpentry",
	12: "cartography",
	13: "cooking",
	14: "detectinghidden",
	15: "enticement",
	16: "evaluatingintel",
	17: "healing",
	18: "fishing",
	19: "forensics",
	20: "herding",
	21: "hiding",
	22: "provocation",
	23: "inscription",
	24: "lockpicking",
	25: "magery",
	26: "magicresistance",
	27: "tactics",
	28: "snooping",
	29: "musicianship",
	30: "poisoning",
	31: "archery",
	32: "spiritspeak",
	33: "stealing",
	34: "tailoring",
	35: "taming",
	36: "tasteid",
	37: "tinkering",
	38: "tracking",
	39: "veterinary",
	40: "swordsmanship",
	41: "macefighting",
	42: "fencing",
	43: "wrestling",
	44: "lumberjacking",
	45: "mining",
	46: "meditation",
	47: "stealth",
	48: "removetrap",
	49: "necromancy",
	50: "focus",
	51: "chivalry",
	52: "bushido",
	53: "ninjitsu",
	54: "spellweaving",
	55: "mysticism",
	56: "imbuing",
	57: "throwing"
};


// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/** @type { () => number } */
function GlobalSingleSkillCap()
{
	// Always allow up to 120.0 with power scrolls
	return 1200;
}

/** @type { ( capValue: number ) => string } */
function SkillValue( capValue )
{
	var numValue = capValue / 10;
	return numValue.toFixed( 1 );
}

/** @type { ( skillProp: string ) => string } */
function PrettySkillName( skillProp )
{
	var s = powerSkillDisplay[skillProp];
	if( s )
		return s;

	if( !skillProp || skillProp.length <= 0 )
		return "";

	return skillProp.charAt( 0 ).toUpperCase() + skillProp.substr( 1 );
}

/** @type { ( iScroll: Item ) => { isValid: boolean, skillProp: string, bonus: number } } */
function ScrollDataFromMore( iScroll )
{
	var result = { isValid:false, skillProp:"", bonus:0 };

	if( !ValidateObject( iScroll ))
		return result;

	var skillId = iScroll.morex | 0;
	var bonus   = iScroll.morey | 0;

	if( skillId < 0 || bonus <= 0 )
		return result;

	var prop = powerSkillById[skillId];
	if( !prop )
		return result;

	// only allow known bonuses
	if( !powerRarityByBonus[bonus] )
		return result;

	result.isValid = true;
	result.skillProp = prop;
	result.bonus = bonus;
	return result;
}

/** @type { ( iScroll: Item ) => string } */
function BuildPowerScrollName( iScroll )
{
	var data = ScrollDataFromMore( iScroll );
	if( !data.isValid )
		return iScroll.name;

	var rarity = powerRarityByBonus[data.bonus];
	var skillPretty = PrettySkillName( data.skillProp );
	var maxValue = 100 + data.bonus;

	return "a " + rarity + " scroll of " + skillPretty + " [" + maxValue + "]";
}

/** @type { ( pUser: Character, iScroll: Item ) => number } */
function ApplyPowerScroll( pUser, iScroll )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iScroll ))
		return 0;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return 0;

	var data = ScrollDataFromMore( iScroll );
	if( !data.isValid )
	{
		pSocket.SysMessage( "This Scroll of Power is not configured correctly." );
		return 0;
	}

	var skillProp = data.skillProp;
	var bonus = data.bonus;

	var caps = pUser.skillCaps;
	if( !caps )
	{
		pSocket.SysMessage( "Your skill caps could not be read." );
		return 0;
	}

	if( typeof caps[skillProp] === "undefined" )
	{
		pSocket.SysMessage( "This scroll does not affect any skill you possess." );
		return 0;
	}

	var globalSingleCap = GlobalSingleSkillCap();

	var currentCap = caps[skillProp] | 0;
	if( currentCap <= 0 )
		currentCap = 1000; // default 100.0

	var desiredCap = 1000 + ( bonus * 10 );
	if( desiredCap > globalSingleCap )
		desiredCap = globalSingleCap;

	if( currentCap >= desiredCap )
	{
		var skillNamePrettyFail = PrettySkillName( skillProp );
		pSocket.SysMessage( "Your " + skillNamePrettyFail + " is too high for this power scroll." );
		return 0;
	}

	caps[skillProp] = desiredCap;

	var skillNamePretty = PrettySkillName( skillProp );
	var oldCapStr = SkillValue( currentCap );
	var newCapStr = SkillValue( desiredCap );

	pSocket.SysMessage( "You feel a surge of magic as the scroll enhances your " + skillNamePretty );
	pSocket.SysMessage( "Maximum " + skillNamePretty + " skill raised from " + oldCapStr + " to " + newCapStr + "." );

	pUser.SoundEffect( 0x243, true );
	DoMovingEffect( pUser, pUser.x - 6, pUser.y - 6, pUser.z + 15, 0x36D4, 0x07, 0x00, true, 0x497, 9502 );
	DoMovingEffect( pUser, pUser.x - 4, pUser.y - 6, pUser.z + 15, 0x36D4, 0x07, 0x00, true, 0x497, 9502 );
	DoMovingEffect( pUser, pUser.x - 6, pUser.y - 4, pUser.z + 15, 0x36D4, 0x07, 0x00, true, 0x497, 9502 );
	pUser.StaticEffect( 0x375A, 3, 19 );

	iScroll.Delete();
	return 1;
}


// ---------------------------------------------------------------------------
// DFN Create hook
// ---------------------------------------------------------------------------

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( iCreated, objType )
{
	if( !ValidateObject( iCreated ))
		return;

	var data = ScrollDataFromMore( /** @type {Item} */( iCreated ) );
	if( !data.isValid )
		return;

	iCreated.name = BuildPowerScrollName( /** @type {Item} */( iCreated ) );
}


// ---------------------------------------------------------------------------
// Use + Gump
// ---------------------------------------------------------------------------

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
		return false;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return false;

	pSocket.tempObj = iUsed;
	PowerScrollGump( pSocket, iUsed );
	return false;
}

/** @type { ( pSocket: Socket, iScroll: Item ) => void } */
function PowerScrollGump( pSocket, iScroll )
{
	var powerScrollGump = new Gump;
	powerScrollGump.AddPage( 0 );

	powerScrollGump.AddBackground( 25, 10, 420, 200, 5054 );

	powerScrollGump.AddTiledGump( 33, 20, 401, 181, 2624 );
	powerScrollGump.AddCheckerTrans( 33, 20, 401, 181 );

	powerScrollGump.AddXMFHTMLGump( 40, 48, 387, 100, 1049469, true, true );
	powerScrollGump.AddXMFHTMLGumpColor( 125, 148, 200, 20, 1049478, false, false, 0xFFFFFF ); // Do you wish to use this scroll?

	powerScrollGump.AddButton( 100, 172, 4005, 4007, 1, 0, 1 );
	powerScrollGump.AddXMFHTMLGumpColor( 135, 172, 120, 20, 1046362, false, false, 0xFFFFFF ); // Yes

	powerScrollGump.AddButton( 275, 172, 4005, 4007, 1, 0, 0 );
	powerScrollGump.AddXMFHTMLGumpColor( 310, 172, 120, 20, 1046363, false, false, 0xFFFFFF ); // No

	var data = ScrollDataFromMore( iScroll );
	if( !data.isValid )
	{
		// still show gump, but generic header
		powerScrollGump.AddXMFHTMLGumpColor( 40, 20, 350, 20, 1049463, false, false, 0xFFFFFF );
		powerScrollGump.Send( pSocket );
		powerScrollGump.Free();
		return;
	}

	var bonus = data.bonus;
	var skillProp = data.skillProp;

	// -----------------------------------------------------------------
	// Header: rarity + skill, using cliloc + ~1_type~ replacement
	// 1049463 Wonderous Scroll ( 105 Skill ):
	// 1049464 Exalted Scroll ( 110 Skill ):
	// 1049465 Mythical Scroll ( 115 Skill ):
	// 1049466 Legendary Scroll ( 120 Skill ):
	// 1049467 Ultimate Scroll ( 125 Skill ):
	// -----------------------------------------------------------------
	var headerCliloc = 1049463;
	switch( bonus )
	{
		case 5:  headerCliloc = 1049463; break;
		case 10: headerCliloc = 1049464; break;
		case 15: headerCliloc = 1049465; break;
		case 20: headerCliloc = 1049466; break;
		case 25: headerCliloc = 1049467; break;
		default: headerCliloc = 1049463; break;
	}

	var skillPretty = PrettySkillName( skillProp );

	// Use AddXMFHTMLTok to pass token arg (skill name)
	powerScrollGump.AddXMFHTMLTok( 40, 20, 350, 20, false, false, 0xFFFFFF, headerCliloc, skillPretty, " ", " " );

	powerScrollGump.Send( pSocket );
	powerScrollGump.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSocket, myButton, gumpData )
{
	if( pSocket == null )
		return;

	var pUser = pSocket.currentChar;
	var iUsed = pSocket.tempObj;

	switch( myButton )
	{
		case 0:
			break;

		case 1:
			ApplyPowerScroll( pUser, /** @type {Item} */( iUsed ) );
			break;
	}
}