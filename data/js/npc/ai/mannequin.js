/// <reference path="../../definitions.d.ts" />
// @ts-check

const eraType = EraStringToNum( GetServerSetting( "CoreShardEra" ));
const armorLayers = [0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x0A, 0x0D, 0x11, 0x13, 0x14, 0x16, 0x17, 0x18];
const resistLayers = [ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0C, 0x0D, 0x0E, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18];
// Wearable layers (skip hair/facial/backpack/mount/vendor/bank)
const equipLayers = [0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0C,0x0D,0x0E,0x0F,0x11,0x12,0x13,0x14,0x16,0x17,0x18];

/** @type { ( tSock: Socket, baseObj: BaseObject ) => boolean } */
function onContextMenuRequest( socket, targObj )
{
	if( socket == null )
		return false;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return false;

	if( pUser.InRange( targObj, 2 ) && targObj.owner == pUser)
	{
		var list = [
		{ id: 0x000A, text: 6123, flags: 0x0000, hue: 0x03E0 },       // open paperdoll
		{ id: 0x000D, text: 1151585, flags: 0x0000, hue: 0x03E0 },    // Customize Body
		{ id: 0x000C, text: 1159296, flags: 0x0000, hue: 0x03E0 },    // open stats gump
		//{ id: 0x000F, text: 1159295, flags: 0x0000, hue: 0x03E0 },  // CompareWithItemInSlot
		{ id: 0x0001, text: 1159411, flags: 0x0000, hue: 0x03E0 },	  // Add Description
		{ id: 0x000E, text: 1151606, flags: 0x0000, hue: 0x03E0 },    // Switch Clothes
		{ id: 0x000B, text: 1151586, flags: 0x0000, hue: 0x03E0 },    // rotate mannequin
		{ id: 0x0002, text: 1151601, flags: 0x0000, hue: 0x03E0 } ];  // redeed
	}
	else if( pUser.InRange( targObj, 4 ) && targObj.owner == pUser)
	{
		var list = [
		{ id: 0x000A, text: 6123, flags: 0x0000, hue: 0x03E0 },       // open paperdoll
		{ id: 0x000D, text: 1151585, flags: 0x0000, hue: 0x03E0 }  ];  // Customize Body
	}
	else
	{
		var list = [
		{ id: 0x000A, text: 6123, flags: 0x0000, hue: 0x03E0 } ];      // open paperdoll
	}

	var useKR = true; // force KR/EC layout so we can use large clilocs
	TriggerEvent( 18001, "modifyContextMenu", socket, targObj, list, useKR );
	return false;
}

/** @type { ( tSock: Socket, baseObj: BaseObject, popupEntry: number ) => boolean } */
function onContextMenuSelect( socket, targObj, popupEntry )
{
	if( socket == null )
		return false;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return false;

	switch( popupEntry )
	{
		case 0x000A: // paperdoll
			OpenPaperdoll( socket, targObj );
			break;
		case 0x000B: // rotate
			var dir = ( targObj.direction | 0 ) + 1;
			if( dir > 0x7 )
				dir = 0x0;
			targObj.direction = dir;
			socket.SysMessage( "You rotate the mannequin a little bit." );
			break;
		case 0x000C: // stats gump
			OpenMannequinStatsGump( socket, targObj, 0 );
			break;
		case 0x000D: // Customize Body
			CustomizeMannequinBody( socket );
			pUser.SetTempTag( "mannequinStats", targObj.serial );
			break;
		case 0x000E: // Switch Clothes
			pUser.SetTempTag( "mannequinStats", targObj.serial );
			SwitchMannequinClothing( pUser, targObj );
			break;
		//case 0x000F: // CompareWithItemInSlot
		//	pUser.SetTempTag("mannequinStats", targObj.serial);
		//	break;
		case 0x0001: // Add Description
			pUser.SetTempTag( "mannequinStats", targObj.serial );
			DescriptionGump( socket );
			break;
		case 0x0002: // Redeed
			MannequinGearToPlayerPack( targObj, pUser );
			CreateDFNItem( socket, pUser, "mannequindeed", 1, "ITEM", true );
			targObj.Delete();
			break;
	}
	return false;
}

/** @type { ( currChar: Character, targChar: Character ) => boolean } */
function onCharDoubleClick( pUser, targChar )
{
	if( !ValidateObject( pUser ) || !ValidateObject( targChar ))
		return false;

	if( pUser.socket == null )
		return false;

	OpenPaperdoll( pUser.socket, targChar );
	return false;
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	var tooltipText = "";
	var desc = myObj.GetTag("Description");
	if( desc != "" && desc != 0 )
	{
		tooltipText = "Description: " + desc;
	}

	myObj.SetTempTag( "clilocTooltip", 1159410 ); // Description: ~1_MESSAGE~
	return tooltipText;
}

/** @type { ( socket: Socket ) => boolean } */
function DescriptionGump( socket )
{
	if( socket == null )
		return false;

	var descriptionGump = new Gump;
	descriptionGump.AddBackground( 50, 50, 400, 300, 0xA28 );
	descriptionGump.AddPage( 0 );
	descriptionGump.AddXMFHTMLGumpColor( 50, 70, 400, 20, 1159409, false, false, 0x7FFF ); // <CENTER>Mannequin</CENTER>
	descriptionGump.AddXMFHTMLGumpColor( 75, 95, 350, 145, 1159408, true, true, 0x7FFF ); // Enter the description to add to the mannequin. Leave the text area blank to remove any existing text.
	descriptionGump.AddButton( 125, 300, 0x81A, 0x81B, 1, 0, 8 );
	descriptionGump.AddButton( 320, 300, 0x819, 0x818, 1, 0, 0 );
	descriptionGump.AddTiledGump( 75, 245, 350, 40, 0xDB0 );
	descriptionGump.AddTiledGump( 76, 245, 350, 2, 0x23C5 );
	descriptionGump.AddTiledGump( 75, 245, 2, 40, 0x23C3 );
	descriptionGump.AddTiledGump( 75, 285, 350, 2, 0x23C5 );
	descriptionGump.AddTiledGump( 425, 245, 2, 42, 0x23C3 );
	descriptionGump.AddTextEntryLimited( 78, 246, 343, 37, 0x4FF, 0, 2, " ", 44 );

	descriptionGump.Send( socket );
	descriptionGump.Free();
	return true;
}

/** @type { ( socket: Socket ) => boolean } */
function CustomizeMannequinBody( socket )
{
	if( socket == null )
		return false;

	var customizeMannyBodyGump = new Gump;
	customizeMannyBodyGump.AddPage( 0 );
	customizeMannyBodyGump.AddBackground( 0, 0, 300, 130, 0x13BE );
	customizeMannyBodyGump.AddTiledGump( 10, 10, 280, 20, 0xA40 );
	customizeMannyBodyGump.AddTiledGump( 10, 40, 280, 80, 0xA40 );
	customizeMannyBodyGump.AddCheckerTrans( 10, 10, 280, 110);
	customizeMannyBodyGump.AddXMFHTMLGumpColor( 10, 10, 200, 300, 1151582, false, false, 0x7FFF );// <center>CUSTOMIZE BODY</center>

	if( eraType >= EraStringToNum( "lbr" ))
	{
		customizeMannyBodyGump.AddXMFHTMLGumpColor( 45, 52, 180, 18, 1072255, false, false, 0x7FFF );// Human
		customizeMannyBodyGump.AddButton( 10, 50, 0xFA5, 1, 0, 1 );
	}

	if( eraType >= EraStringToNum( "ml" ))
	{
		customizeMannyBodyGump.AddXMFHTMLGumpColor( 45, 72, 180, 18, 1072256, false, false, 0x7FFF );// Elf
		customizeMannyBodyGump.AddButton(10, 70, 0xFA5, 1, 0, 2 );

		customizeMannyBodyGump.AddXMFHTMLGumpColor( 45, 92, 180, 18, 1029613, false, false, 0x7FFF );// Gargoyle
		customizeMannyBodyGump.AddButton(10, 90, 0xFA5,  1, 0, 3 );
	}

	customizeMannyBodyGump.AddXMFHTMLGumpColor( 205, 52, 180, 18, 1015327, false, false, 0x7FFF);// Male
	customizeMannyBodyGump.AddButton( 170, 50, 0xFA5, 1, 0, 4 );

	customizeMannyBodyGump.AddXMFHTMLGumpColor( 205, 72, 180, 18, 1015328, false, false, 0x7FFF );// Female
    customizeMannyBodyGump.AddButton( 170, 70, 0xFA5, 1, 0, 5 );

	customizeMannyBodyGump.Send( socket );
	customizeMannyBodyGump.Free();
	return true;
}

/** @type { ( sx: number, sy: number, w?: number, h?: number ) => { sx: number, sy: number, w: number, h: number } } */
function ABS( sx, sy, w, h )
{
	return { sx: sx | 0, sy: sy | 0, w: ( w || 30 ) | 0, h: ( h || 30 ) | 0 };
}

const Icons = {
	StrengthBonus:   ABS( 120, 30 ), DexterityBonus:  ABS(  90,  0 ), IntelligenceBonus: ABS( 240,  0 ),
	HitPointsInc:    ABS( 180,  0 ), StaminaInc:      ABS(  90, 30 ),

	ManaInc:         ABS(   0, 30 ), HPRegen:         ABS(  30, 60 ), StaminaRegen:     ABS( 210, 60 ),
	ManaRegen:       ABS( 120, 60 ),

	PhysResist:      ABS(  30, 30 ), FireResist:      ABS( 150,  0 ), ColdResist:       ABS(   0,  0 ),
	PoisonResist:    ABS(  60, 30 ), EnergyResist:    ABS( 120,  0 ),

	DamageEater:     ABS( 270, 60 ), KineticEater:    ABS(   0, 90 ), FireEater:        ABS(  30, 90 ),
	ColdEater:       ABS(  60, 90 ), PoisonEater:     ABS(  90, 90 ), EnergyEater:      ABS( 120, 90 ),

	DamageIncrease:  ABS(  60,  0 ), DCI:             ABS( 210, 30 ), HCI:              ABS( 210,  0 ),
	SSI:             ABS( 150, 30 ), LMC:             ABS(  60, 60 ), Medable:          ABS(   0,150 ),
	ReflectPhys:     ABS( 150, 60 ), EnhancePotions:  ABS( 240, 30 ),

	Luck:            ABS( 270,  0 ),

	PhysicalDamage:  ABS( 270,210 ), ColdDamage:      ABS( 210,210 ), FireDamage:       ABS( 180,210 ),
	PoisonDamage:    ABS(   0,240 ), EnergyDamage:    ABS( 240,210 ), ChaosDamage:      ABS(  30,240 ),

	FasterCasting:   ABS( 270, 30 ), fasterRecovery:  ABS(   0, 60 ), LowerRegentCost:  ABS( 180, 60 ),
	SpellDamage:     ABS(  90,600 )
};

const mannequinPage1 = [
	{
		header: 1049593, rowsLeft: [
			{ key: "StrengthBonus", text: 1079767, icon: Icons.StrengthBonus },
			{ key: "DexterityBonus", text: 1079732, icon: Icons.DexterityBonus },
			{ key: "IntelligenceBonus", text: 1079756, icon: Icons.IntelligenceBonus },
			{ key: "HitPointsInc", text: 1079404, icon: Icons.HitPointsInc, cap: "/25" },
			{ key: "StaminaInc", text: 1079405, icon: Icons.StaminaInc, cap: "/25" }
		], rowsRight: [
			{ key: "ManaInc", text: 1079406, icon: Icons.ManaInc, cap: "/25" },
			{ key: "HPRegen", text: 1075627, icon: Icons.HPRegen, cap: "/18" },
			{ key: "StaminaRegen", text: 1079411, icon: Icons.StaminaRegen, cap: "/24" },
			{ key: "ManaRegen", text: 1079410, icon: Icons.ManaRegen, cap: "/30" }
		]
	},
	{
		header: 1061645, rowsLeft: [
			{ key: "PhysResist", text: 1079764, icon: Icons.PhysResist, cap: "/70" },
			{ key: "FireResist", text: 1079763, icon: Icons.FireResist, cap: "/70" },
			{ key: "ColdResist", text: 1079761, icon: Icons.ColdResist, cap: "/70" },
			{ key: "PoisonResist", text: 1079765, icon: Icons.PoisonResist, cap: "/70" },
			{ key: "EnergyResist", text: 1079762, icon: Icons.EnergyResist, cap: "/70" }
		], rowsRight: [
			{ key: "DamageEater", text: 1154667, icon: Icons.DamageEater, cap: "/18" },
			{ key: "KineticEater", text: 1154666, icon: Icons.KineticEater, cap: "/30" },
			{ key: "FireEater", text: 1154662, icon: Icons.FireEater, cap: "/30" },
			{ key: "ColdEater", text: 1154663, icon: Icons.ColdEater, cap: "/30" },
			{ key: "PoisonEater", text: 1154664, icon: Icons.PoisonEater, cap: "/30" },
			{ key: "EnergyEater", text: 1154665, icon: Icons.EnergyEater, cap: "/30" }
		]
	},
	{
		header: 1077417, rowsLeft: [
			{ key: "DamageIncrease", text: 1079760, icon: Icons.DamageIncrease, cap: "/100" },
			{ key: "DCI", text: 1075620, icon: Icons.DCI, cap: "/45" },
			{ key: "HCI", text: 1075616, icon: Icons.HCI, cap: "/45" },
			{ key: "SSI", text: 1075629, icon: Icons.SSI, cap: "/60" },
			{ text: 1075621, icon: Icons.LMC, cap: "/40" },
			{ text: 1159280, icon: Icons.Medable, val: "Yes" },
			{ text: 1075626, icon: Icons.ReflectPhys },
			{ text: 1075624, icon: Icons.EnhancePotions }
		], rowsRight: [
			{ key: "Luck", text: 1061153, icon: Icons.Luck }
		]
	}
];

const mannequinPage2 = [
	{
		header: 1077417, rowsLeft: [
			{ key: "PhysDamage", text: 1151800, icon: Icons.PhysicalDamage },
			{ text: 1151802, icon: Icons.ColdDamage },
			{ text: 1151801, icon: Icons.FireDamage }
		], rowsRight: [
			{ text: 1151803, icon: Icons.PoisonDamage },
			{ text: 1151804, icon: Icons.EnergyDamage },
			{ text: 1151805, icon: Icons.ChaosDamage }
		]
	},
	{
		header: 1076209, rowsLeft: [
			{ text: 1075617, icon: Icons.FasterCasting, cap: "/4" },
			{ text: 1075618, icon: Icons.fasterRecovery, cap: "/6" }
		], rowsRight: [
			{ text: 1075625, icon: Icons.LowerRegentCost, cap: "/100" },
			{ text: 1075628, icon: Icons.SpellDamage }
		]
	}
];

/** @type { ( mannequin: Character ) => { lo: number, hi: number } } */
function BestWeaponDamage( mannequin )
{
	var best = { lo: 0, hi: 0 };
	if( !( mannequin && ValidateObject( mannequin )))
		return best;

	var it, lo, hi;

	it = mannequin.FindItemLayer( 0x01 );
	if( it && ValidateObject( it ))
	{
		lo = parseInt( it.lodamage, 10 ); lo = isNaN( lo ) ? 0 : (lo | 0);
		hi = parseInt( it.hidamage, 10 ); hi = isNaN( hi ) ? 0 : (hi | 0);
		if( hi >= lo && hi > 0 && hi > best.hi ) { best.lo = lo; best.hi = hi; }
	}

	it = mannequin.FindItemLayer( 0x02 );
	if( it && ValidateObject( it ))
	{
		lo = parseInt( it.lodamage, 10 ); lo = isNaN( lo ) ? 0 : (lo | 0);
		hi = parseInt( it.hidamage, 10 ); hi = isNaN( hi ) ? 0 : (hi | 0);
		if( hi >= lo && hi > 0 && hi > best.hi ) { best.lo = lo; best.hi = hi; }
	}

	return best;
}

/** @type { ( mannequin: Character, layers: number[], prop: string ) => number } */
function SumLayersProp( mannequin, layers, prop )
{
	if( !( mannequin && ValidateObject( mannequin )))
		return 0;

	var total = 0, it, v;
	for( var i = 0; i < layers.length; i++ )
	{
		it = mannequin.FindItemLayer( layers[i] );
		if( it && ValidateObject( it ))
		{
			v = parseInt( it[prop], 10 );
			total += isNaN( v ) ? 0 : (v | 0);
		}
	}
	return total | 0;
}

/** @type { ( mannequin: Character ) => number } */
function SumPhysResistFromArmor( mannequin )
{
	if( !( mannequin && ValidateObject( mannequin )))
		return 0;

	var total = 0, it, v;
	for( var i = 0; i < armorLayers.length; i++ )
	{
		it = mannequin.FindItemLayer( armorLayers[i] );
		if( it && ValidateObject( it ))
		{
			v = parseInt( it.def, 10 );
			total += isNaN( v ) ? 0 : (v | 0);
		}
	}
	return total | 0;
}

/** @type { ( mannequin: Character ) => {
	physResist: number,
	coldResist: number,
	fireResist: number,
	energyResist: number,
	poisonResist: number,
	hpRegenBonus: number,
	staminaRegenBonus: number,
	manaRegenBonus: number,
	hpInc: number,
	stamInc: number,
	manaInc: number,
	hci: number,
	dci: number,
	di: number,
	luck: number,
	ssi: number,
	physDmgLo: number,
	physDmgHi: number
} } */
function BuildGumpContext( mannequin )
{
	var ctx = {};

	ctx.physResist   = SumPhysResistFromArmor( mannequin );

	ctx.coldResist   = SumLayersProp( mannequin, resistLayers, "resistCold" );
	ctx.fireResist   = SumLayersProp( mannequin, resistLayers, "resistHeat" );
	ctx.energyResist = SumLayersProp( mannequin, resistLayers, "resistLightning" );
	ctx.poisonResist = SumLayersProp( mannequin, resistLayers, "resistPoison" );

	ctx.hpRegenBonus      = SumLayersProp( mannequin, resistLayers, "healthRegenBonus" );
	ctx.staminaRegenBonus = SumLayersProp( mannequin, resistLayers, "staminaRegenBonus" );
	ctx.manaRegenBonus    = SumLayersProp( mannequin, resistLayers, "manaRegenBonus" );

	ctx.hpInc   = SumLayersProp( mannequin, resistLayers, "healthBonus" );
	ctx.stamInc = SumLayersProp( mannequin, resistLayers, "staminaBonus" );
	ctx.manaInc = SumLayersProp( mannequin, resistLayers, "manaBonus" );

	ctx.hci  = SumLayersProp( mannequin, resistLayers, "hitChance" );
	ctx.dci  = SumLayersProp( mannequin, resistLayers, "defenseChance" );
	ctx.di   = SumLayersProp( mannequin, resistLayers, "damageIncrease" );
	ctx.luck = SumLayersProp( mannequin, resistLayers, "luck" );
	ctx.ssi  = SumLayersProp( mannequin, resistLayers, "swingSpeedIncrease" );

	var wd = BestWeaponDamage( mannequin );
	ctx.physDmgLo = wd.lo | 0;
	ctx.physDmgHi = wd.hi | 0;

	return ctx;
}

/** @type { ( sections: any[] ) => any[] } */
function FilterSectionsForEra( sections )
{
	var out = [];

	var eraLBR = EraStringToNum( "lbr" );
	var eraAOS = EraStringToNum( "aos" );
	var eraML  = EraStringToNum( "ml" );

	var isLBRonly = ( eraType >= eraLBR && eraType < eraAOS );
	var isAOSonly = ( eraType >= eraAOS && eraType < eraML );
	var isMLplus  = ( eraType >= eraML );

	function cloneRow( row )
	{
		var r = {}, k;
		row = row || {};
		for( k in row )
		{
			if( row.hasOwnProperty( k ))
				r[k] = row[k];
		}
		return r;
	}

	function includeByEraKey( k )
	{
		k = k || "";

		if( isMLplus )
			return true;

		if( isLBRonly )
		{
			return ( k === "StrengthBonus" || k === "DexterityBonus" || k === "IntelligenceBonus" ||
			         k === "PhysResist"    || k === "PhysDamage" );
		}

		if( isAOSonly )
		{
			return !( k === "DamageEater" || k === "KineticEater" || k === "FireEater" ||
			          k === "ColdEater"   || k === "PoisonEater"  || k === "EnergyEater" );
		}

		return true;
	}

	for( var s = 0; s < sections.length; s++ )
	{
		var src = sections[s] || {};
		var sec = { header: src.header, rowsLeft: [], rowsRight: [] };

		var L = src.rowsLeft  || [];
		var R = src.rowsRight || [];

		for( var i = 0; i < L.length; i++ )
		{
			if( isMLplus || includeByEraKey( L[i] && L[i].key ))
				sec.rowsLeft.push( cloneRow( L[i] ));
		}

		for( var j = 0; j < R.length; j++ )
		{
			if( isMLplus || includeByEraKey( R[j] && R[j].key ))
				sec.rowsRight.push( cloneRow( R[j] ));
		}

		if( sec.rowsLeft.length || sec.rowsRight.length )
			out.push( sec );
	}

	return out;
}

/** @type { ( socket: Socket, mannequin: Character, page: number ) => void } */
function OpenMannequinStatsGump( socket, mannequin, page )
{
	page = page | 0;

	var ctx = BuildGumpContext( mannequin );

	// key -> ctx field mapping (kills the giant switch)
	var MAP = {
		PhysResist:     "physResist",
		ColdResist:     "coldResist",
		FireResist:     "fireResist",
		EnergyResist:   "energyResist",
		PoisonResist:   "poisonResist",

		HPRegen:        "hpRegenBonus",
		StaminaRegen:   "staminaRegenBonus",
		ManaRegen:      "manaRegenBonus",

		HitPointsInc:   "hpInc",
		StaminaInc:     "stamInc",
		ManaInc:        "manaInc",

		HCI:            "hci",
		DCI:            "dci",
		DamageIncrease: "di",
		Luck:           "luck",
		SSI:            "ssi"
	};

	// local helpers (not global noise)
	function text( gump, x, y, w, h, v )
	{
		if( typeof v === "number" )
			gump.AddXMFHTMLTok( x, y, w, h, false, false, 0x560A, v);
		else
			gump.AddHTMLGump( x, y, w, h, false, false, v );
	}

	function valHtml( row )
	{
		var cap = ( row && row.cap ) ? row.cap : "";
		if( row && row.key === "PhysDamage" )
			return "<BASEFONT COLOR=#80BFFF>" + ( ctx.physDmgLo|0) + "-" + ( ctx.physDmgHi|0 ) + "</BASEFONT>";

		if( row && row.key && MAP[row.key] )
			return "<BASEFONT COLOR=#80BFFF>" + (( ctx[ MAP[row.key] ]|0 )) + cap + "</BASEFONT>";

		return "<BASEFONT COLOR=#80BFFF>0" + cap + "</BASEFONT>";
	}

	function row( gump, side, y, r )
	{
		var xIcon  = side ? 307 : 10;
		var xLabel = side ? 342 : 45;
		var xVal   = side ? 532 : 235;

		gump.AddPicInPic( xIcon, y - 5, 0x09D3B, r.icon.sx|0, r.icon.sy|0, r.icon.w|0, r.icon.h|0 );
		text( gump, xLabel, y, 220, 18, r.text );
		gump.AddHTMLGump( xVal, y, 110, 18, false, false, valHtml( r ) );
	}

	function section( gump, y, sec )
	{
		text( gump, 10, y, 200, 18, sec.header );
		gump.AddTiledGump( 10, y + 18, 584, 5, 0x06DC );
		y += 32;

		var L = sec.rowsLeft  || [];
		var R = sec.rowsRight || [];
		var max = ( L.length > R.length ? L.length : R.length );
		var baseY = y;

		for( var i = 0; i < max; i++ )
		{
			if( i < L.length ) row( gump, 0, baseY + (i * 30), L[i] );
			if( i < R.length ) row( gump, 1, baseY + (i * 30), R[i] );
		}

		return baseY + (max * 30) + 3;
	}

	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 604, 820, 0x06DB );
	gump.AddCheckerTrans( 0, 0, 604, 820 );
	gump.AddXMFHTMLTok( 10, 10, 584, 18, false, false, 0x43F7, 1114513, "#1159279" );

	// pager
	gump.AddButton( 554, 10, ( page > 0 ) ? 0x15E3 : 0x15E1, 1, 0, ( page > 0 ) ? 7 : 6 );

	var sections = FilterSectionsForEra(( page > 0 ) ? mannequinPage2 : mannequinPage1 );

	var y = 28;
	for( var s = 0; s < sections.length; s++ )
	{
		y = section( gump, y, sections[s] );
	}

	gump.Send( socket );
	gump.Free();
}

/** @type { ( mannequin: Character, pUser: Character ) => number } */
function MannequinGearToPlayerPack( mannequin, pUser )
{
    if( !ValidateObject( mannequin ) || !ValidateObject( pUser ))
		return 0;

	var pSocket = pUser.socket;
	if( pSocket== null )
		return 0;

    var isPack = pUser.pack;
    if( !ValidateObject( isPack ))
	{
        pSocket.SysMessage( "You need a backpack to take the mannequin's gear." );
        return 0;
    }

    var moved = 0;
    for( var i = 0; i < resistLayers.length; i++ )
    {
        var layer = resistLayers[i];
        var item = mannequin.FindItemLayer( layer );
        if( !ValidateObject( item ))
			continue;

        // hand off the item to the player's pack
        item.container = isPack;
        item.Refresh();
        moved++;
    }

    if( moved > 0 ) 
	{
        pSocket.SysMessage( "You retrieve " + moved + " item"+( moved===1 ? "" : "s" )+" from the mannequin." );
    }
    return moved;
}

/** @type { ( pUser: Character, item: Item ) => boolean } */
function CheckPack( pUser, item )
{
	if( !ValidateObject( pUser ) || !ValidateObject( item ))
		return false;

	var pack = pUser.pack;
	if( !ValidateObject( pack )) 
		return false;

	if( pack.totalItemCount >= pack.maxItems )
		return false;

	if(( pack.maxWeight|0 ) > 0 )
	{
		var addW = ( item.weight|0 );
		if(( pack.totalWeight|0) + addW > ( pack.maxWeight|0 ))
			return false;
	}
	return true;
}

/** @type { ( pUser: Character, item: Item ) => void } */
function DropAtFeet( pUser, item )
{
	if( !ValidateObject( pUser ) || !ValidateObject( item ))
		return;

	item.container = null; // world
	item.x = pUser.x;
	item.y = pUser.y;
	item.z = pUser.z;
	item.worldnumber = pUser.worldnumber;
	item.instanceID = pUser.instanceID;
	item.Refresh();
}

/** @type { ( pUser: Character, item: Item ) => { equipped: boolean, packed: boolean, dropped: boolean } } */
function PlaceInPackOrDrop( pUser, item )
{
	if( !ValidateObject( pUser ) || !ValidateObject( item ))
		return {equipped:false, packed:false, dropped:false};

	var isPack = pUser.pack;
	if( ValidateObject( isPack ) && CheckPack( pUser, item ))
	{
		item.container = isPack;
		item.PlaceInPack();
		item.Refresh();
		return {equipped:false, packed:true, dropped:false};
	}

	DropAtFeet( pUser, item );
	return {equipped:false, packed:false, dropped:true};
}

/** @type { ( char: Character, item: Item, ownerForFallback: Character ) => { equipped: boolean, packed: boolean, dropped: boolean } } */
function EquipOn( char, item, ownerForFallback )
{
	if( !ValidateObject( char ) || !ValidateObject( item ))
		return {equipped:false, packed:false, dropped:false};

	item.container = char;
	item.Refresh();

	var ok = ( item.container === char );
	if( ok )
	{
		var onThatLayer = char.FindItemLayer( item.layer|0 );
		ok = ( onThatLayer === item );
	}

	if( ok )
		return {equipped:true, packed:false, dropped:false};

	if( ValidateObject( ownerForFallback ))
		return PlaceInPackOrDrop( ownerForFallback, item );

	return {equipped:false, packed:false, dropped:false};
}

/** @type { ( res: { equipped: boolean, packed: boolean, dropped: boolean } ) => number } */
function SwapFail( res )
{
	return ( res && !res.equipped && ( res.packed || res.dropped )) ? 1 : 0;
}

/** @type { ( pUser: Character, mannequin: Character, layer: number ) => { moved: number, failed: number } } */
function SwapLayer( pUser, mannequin, layer )
{
	var u = pUser.FindItemLayer( layer );
	var m = mannequin.FindItemLayer( layer );

	var moved = 0, failed = 0;

	if( !ValidateObject( u ) && !ValidateObject( m ))
		return {moved:0, failed:0};

	if( ValidateObject( u ) && ValidateObject( m ))
	{
		PlaceInPackOrDrop( pUser, u );

		var r1 = EquipOn( pUser, m, pUser );
		failed += SwapFail( r1 );
		moved++;

		var r2 = EquipOn( mannequin, u, pUser );
		failed += SwapFail( r2 );
		moved++;

		return {moved:moved, failed:failed};
	}

	if( !ValidateObject( u ) && ValidateObject( m ))
	{
		var r3 = EquipOn( pUser, m, pUser );
		failed += SwapFail( r3 ); moved++;
		return {moved:moved, failed:failed};
	}

	if( ValidateObject( u ) && !ValidateObject( m ))
	{
		var r4 = EquipOn( mannequin, u, pUser );
		failed += SwapFail( r4 ); moved++;
		return {moved:moved, failed:failed};
	}

	return {moved:0, failed:failed};
}

/** @type { ( pUser: Character, mannequin: Character ) => { moved: number, failed: number } } */
function SwapHands( pUser, mannequin )
{
	var moved = 0, failed = 0;

	var userRight = pUser.FindItemLayer( 0x01 );
	var userLeft = pUser.FindItemLayer( 0x02 );
	var mannequinRight = mannequin.FindItemLayer( 0x01 );
	var mannequinLeft = mannequin.FindItemLayer( 0x02 );

	if( ValidateObject( userRight ))
	{
		PlaceInPackOrDrop( pUser, userRight );
		moved++;
	}

	if( ValidateObject( userLeft ))
	{ 
		PlaceInPackOrDrop( pUser, userLeft );
		moved++;
	}

	if( ValidateObject( mannequinRight ))
	{ 
		var a = EquipOn( pUser, mannequinRight, pUser );
		failed += SwapFail( a );
		moved++;
	}

	if( ValidateObject( mannequinLeft ))
	{
		var b = EquipOn( pUser, mannequinLeft, pUser );
		failed += SwapFail( b );
		moved++;
	}

	if( ValidateObject( userRight ))
	{
		var c = EquipOn( mannequin, userRight, pUser );
		failed += SwapFail( c );
	}

	if( ValidateObject( userLeft ))
	{
		var d = EquipOn( mannequin, userLeft, pUser );
		failed += SwapFail( d );
	}

	return {moved:moved, failed:failed};
}

/** @type { ( pUser: Character, mannequin: Character ) => number } */
function SwitchMannequinClothing( pUser, mannequin )
{
	if( !ValidateObject( pUser ) || !ValidateObject( mannequin ))
		return 0;

	var pSocket = pUser.socket;
	if( pSocket== null )
		return 0;

	if( !ValidateObject( pUser.pack ))
	{
		pSocket.SysMessage( "You need a backpack to switch clothes." );
		return 0;
	}

	var totalMoved = 0, totalFailed = 0;
	for( var i = 0; i < equipLayers.length; i++ )
	{
		var isLayer = equipLayers[i];
		if( isLayer === 0x01 || isLayer === 0x02 ) continue;
		var r = SwapLayer( pUser, mannequin, isLayer );
		totalMoved += r.moved;
		totalFailed += r.failed;
	}

	var hr = SwapHands( pUser, mannequin );
	totalMoved += hr.moved;
	totalFailed += hr.failed;

	if( totalMoved > 0 )
	{
		pSocket.SysMessage( "You quickly swap clothes with the mannequin." );
	}

	if( totalFailed > 0 )
	{
		pSocket.SysMessage( totalFailed + " item" + ( totalFailed===1?"":"s" ) + " could not be swapped between you and the mannequin. These items are now in your backpack, or on the floor at your feet if your backpack is too full to hold them." );
	}

	return totalMoved;
}

/** @type { ( mannequin: Character, race: number | null, gender: number | boolean | null, pUser: Character ) => void } */
function SeRaceGender(  mannequin, race, gender, pUser )
{
    race = ( race == null
        ? ( isFinite( mannequin.race ) ? ( mannequin.race - 0 )
            : ( mannequin.id==0x0190||mannequin.id==0x0191 )?0
            : ( mannequin.id==0x025D||mannequin.id==0x025E )?1
            : ( mannequin.id==0x029A||mannequin.id==0x029B )?2
            : 0 )
        : ( race - 0 ));

    gender = gender ? 1 : 0;

    if(( mannequin.race == 2 ) || ( race == 2 ))
        MannequinGearToPlayerPack( mannequin, pUser );

    mannequin.race = race;
    mannequin.gender = gender;
    mannequin.orgID = ( mannequin.id = ( [[0x0190,0x0191],[0x025D,0x025E],[0x029A,0x029B]] )[race][gender]);

	mannequin.Refresh();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, iButton, gumpData )
{
	var pUser = pSock.currentChar;
	var mannequinSer = pUser.GetTempTag( "mannequinStats" );
	var mannequin = CalcCharFromSer( mannequinSer );
	if( !ValidateObject( pUser )) 
	{
		return;
	}

	switch( iButton )
	{
		case 0: break;
		case 1:
			SeRaceGender( mannequin, 0, mannequin.gender, pUser );
			CustomizeMannequinBody( pSock );
			break; // Human
		case 2:
			SeRaceGender( mannequin, 1, mannequin.gender, pUser );
			CustomizeMannequinBody( pSock );
			break; // Elf
		case 3:
			SeRaceGender( mannequin, 2, mannequin.gender, pUser );
			CustomizeMannequinBody( pSock );
			break; // Gargoyle
		case 4:
			SeRaceGender( mannequin, null, 0, pUser );
			CustomizeMannequinBody( pSock );
			break;   // Male
		case 5:
			SeRaceGender( mannequin, null, 1, pUser );
			CustomizeMannequinBody( pSock );
			break;   // Female
		case 6:
		case 7:
			if( ValidateObject( mannequin ))
				OpenMannequinStatsGump( pSock, mannequin, ( iButton === 6 ? 1 : 0 ));
			break;
		case 8:
			{
				var raw = gumpData.getEdit( 0 );
				var res = CheckDescription( raw, true );

				if( res.cleared )
				{
					mannequin.SetTag( "Description", "" );
					mannequin.Refresh();
					if( pUser && pUser.socket )
						pSock.SysMessage( "Description cleared." );
					break
				}

				if( !res.ok )
				{
					if( pUser && pUser.socket )
						pSock.SysMessage( "Invalid description: " + res.reason + "." );
					break;
				}

				mannequin.SetTag( "Description", res.value );
				mannequin.Refresh();
				if( pUser && pUser.socket )
					pSock.SysMessage( "Description updated." );
				break;
			}
	}
}

/** @type { ( socket: Socket, targChar: Character ) => void } */
function OpenPaperdoll( socket, targChar )
{
    // Create packet to open a paperdoll
    var pStream = new Packet;
    pStream.ReserveSize( 66 );
    pStream.WriteByte( 0, 0x88 );
    pStream.WriteLong( 1, targChar.serial );
    pStream.WriteString( 5, targChar.name + ( targChar.title ? " " + targChar.title : "" ), targChar.name.length + ( targChar.title ? targChar.title.length + 1 : 0 ));
    pStream.WriteByte( 65, 0x2 );
    socket.Send( pStream );
    pStream.Free();
}

/** @type { ( raw: string, allowEmpty: boolean ) => { ok: boolean, value?: string, cleared?: boolean, reason?: string } } */
function CheckDescription( raw, allowEmpty )
{
	var DISALLOWED_PATTERNS = [
		/(https?:\/\/|http\b|www\.|\.[a-z]{2,})/i,   // links
		/\bdiscord\.gg\b/i,       // invites
		/<[^>]+>/,                // html-ish tags
		/[\x00-\x1F\x7F]/         // control chars
	];

	var text = ( raw == null ) ? "" : ("" + raw);

	// allow clearing
	if( allowEmpty && !text.replace(/\s/g,"") )
		return { ok:true, value:"", cleared:true };

	// disallow empty
	if( !text.replace(/\s/g,"") )
		return { ok:false, reason:"empty" };

	// cap length
	if( text.length > 200 )
		return { ok:false, reason:"too long" };

	// policy patterns
	for( var i = 0; i < DISALLOWED_PATTERNS.length; i++ )
	{
		if( DISALLOWED_PATTERNS[i].test( text ) )
			return { ok:false, reason:"disallowed content" };
	}

	// character sanity (inlined hasOnlyNormalChars)
	if( !/^[A-Za-z0-9 \t\r\n.,'"\-_:;!?()#&\/\[\]]+$/.test( text ) )
		return { ok:false, reason:"contains unusual characters" };

	// require at least one letter
	if( !/[A-Za-z]/.test( text ) )
		return { ok:false, reason:"needs letters" };

	// normalize whitespace (inlined trimSpaces)
	text = text.replace(/\s+/g, " ").replace(/^\s+|\s+$/g, "");

	return { ok:true, value:text };
}