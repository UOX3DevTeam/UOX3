const powerRarityByBonus = {
	5:  "wonderous",
	10: "exalted",
	15: "mythical",
	20: "legendary",
	25: "ultimate"
};

const powerSkillDisplay = {
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
	allskills:        "All Skills"
};

/*function globalSingleSkillCap(  )
{
	var cap = Number( GetServerSetting( "SKILLCAPSINGLE" ));
	if( cap <= 0 )
		cap = 1200; // default hard cap 120.0
	return cap;
}*/

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

/** @type { ( iScroll: Item ) => { isValid: boolean, skillProp: string, bonus: number } } */
function ScrollDataFromTags( iScroll )
{
	var result = {
		isValid: false,
		skillProp: "",
		bonus: 0
	};

	if( !ValidateObject( iScroll ))
		return result;

	var skillProp = iScroll.GetTag( "powerSkill" );
	var bonusTag = iScroll.GetTag( "powerBonus" );

	if( !skillProp || skillProp === "" )
		return result;

	var bonus = Number( bonusTag );
	if( bonus <= 0 )
		return result;

	result.isValid = true;
	result.skillProp = String( skillProp );
	result.bonus = bonus;
	return result;
}

/** @type { ( iScroll: Item ) => string } */
function BuildPowerScrollName( iScroll )
{
	var data = ScrollDataFromTags( iScroll );

	if( !data.isValid )
		return iScroll.name;

	var skillProp = data.skillProp;
	var bonus = data.bonus;

	var rarity = powerRarityByBonus[bonus];
	if( rarity === "" )
		return iScroll.name;

	// Pretty skill name
	var skillPretty = powerSkillDisplay[skillProp] || skillProp;

	// 100 + bonus -> [105], [110], [115], [120]
	var maxValue = 100 + bonus;

	// Example:
	// "a legendary scroll of Wrestling [120]"
	var newName = "a " + rarity + " scroll of " + skillPretty + " [" + maxValue + "]";

	return newName;
}

/** @type { ( pUser: Character, iScroll: Item ) => number } */
function ApplyPowerScroll( pUser, iScroll )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iScroll ))
		return 0;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return 0;

	var data = ScrollDataFromTags( iScroll );
	if( !data.isValid )
	{
		pSocket.SysMessage( "This Scroll of Power is not configured correctly." );
		return 0;
	}

	var skillProp = data.skillProp; // e.g. "magery"
	var bonus = data.bonus;         // 5, 10, 15, 20

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

	// Desired cap: 100.0 + bonus
	var desiredCap = 1000 + ( bonus * 10 );

	if( desiredCap > globalSingleCap )
		desiredCap = globalSingleCap;

	// OSI behavior: cannot use equal or lesser scroll if you already have one
	if( currentCap >= desiredCap )
	{
		var skillNamePretty = powerSkillDisplay[skillProp] || skillProp;
		pSocket.SysMessage( "Your " + skillNamePretty + " is too high for this power scroll." );
		return 0;
	}

	caps[skillProp] = desiredCap;

	// Pretty name, basic capitalization
	var skillNamePretty = skillProp;
	if( skillProp.length > 0 )
	{
		skillNamePretty = skillProp.charAt( 0 ).toUpperCase(  ) + skillProp.substr( 1 );
	}

	var oldCapStr = SkillValue( currentCap );
	var newCapStr = SkillValue( desiredCap );

	pSocket.SysMessage( "You feel a surge of magic as the scroll enhances your " + skillNamePretty );
	pSocket.SysMessage( "Maximum " + skillNamePretty + " skill raised from " + oldCapStr + " to " + newCapStr + "." );
	pUser.SoundEffect( 0x243, true );
	DoMovingEffect( pUser, pUser.x - 6, pUser.y - 6, pUser.z + 15, 0x36D4, 0x07, 0x00, true, 0x497, 9502 );
	DoMovingEffect( pUser, pUser.x - 4, pUser.y - 6, pUser.z + 15, 0x36D4, 0x07, 0x00, true, 0x497, 9502 );
	DoMovingEffect( pUser, pUser.x - 6, pUser.y - 4, pUser.z + 15, 0x36D4, 0x07, 0x00, true, 0x497, 9502 );
	pUser.StaticEffect(  0x375A, 3, 19  );
	iScroll.Delete();

	return 1;
}

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( iCreated, objType )
{
	if( !ValidateObject( iCreated ))
		return;

	// Check if this item actually has powerSkill/powerBonus
	var data = ScrollDataFromTags( iCreated );
	if( !data.isValid )
		return;

	// Set the OSI-style name
	iCreated.name = BuildPowerScrollName( iCreated );
}


// Event handlers -------------------------------------------------------------
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

	// -----------------------------------------------------------------
	// Header: rarity + skill, using cliloc + ~1_type~ replacement
	// 1049463 Wonderous Scroll ( 105 Skill ):
	// 1049464 Exalted Scroll ( 110 Skill ):
	// 1049465 Mythical Scroll ( 115 Skill ):
	// 1049466 Legendary Scroll ( 120 Skill ):
	// 1049467 Ultimate Scroll ( 125 Skill ):
	// -----------------------------------------------------------------
	var bonus = Number( iScroll.GetTag( "powerBonus" ));
	var headerCliloc = 1049463;

	switch ( bonus )
	{
		case 5:  headerCliloc = 1049463; break;
		case 10: headerCliloc = 1049464; break;
		case 15: headerCliloc = 1049465; break;
		case 20: headerCliloc = 1049466; break;
		case 25: headerCliloc = 1049467; break; // if you ever add 125 skill
		default: headerCliloc = 1049463; break;
	}

	// get skill name from tag
	var skillProp = iScroll.GetTag( "powerSkill" );
	var skillPretty = powerSkillDisplay[skillProp] || skillProp;

	// This is the important line:
	// Use AddXMFHTMLTok ( NOT AddXMFHTMLGumpColor ) to pass the skill name
	powerScrollGump.AddXMFHTMLTok( 40, 20, 350, 20, false, false, 0xFFFFFF, headerCliloc,skillPretty, " ", " "  );// clilocArg1 -> ~1_type~ or similar in the cliloc text

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
		case 0: break;
		case 1:	ApplyPowerScroll( pUser, iUsed );
	}
}
