var Era_Type = EraStringToNum(GetServerSetting("CoreShardEra"));
let ARMOR_LAYERS = [0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x0A, 0x0D, 0x11, 0x13, 0x14, 0x16, 0x17, 0x18];
let RESIST_LAYERS = [ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0C, 0x0D, 0x0E, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18];
// Wearable layers (skip hair/facial/backpack/mount/vendor/bank)
let EQUIP_LAYERS = [0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0C,0x0D,0x0E,0x0F,0x11,0x12,0x13,0x14,0x16,0x17,0x18];

/** @type { ( tSock: Socket, baseObj: BaseObject ) => boolean } */
function onContextMenuRequest( socket, targObj )
{
	var pUser = socket.currentChar;

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
	var pUser = socket.currentChar;
	switch( popupEntry )
	{
		case 0x000A: // paperdoll
			TriggerEvent( 18000, "OpenPaperdoll", socket, targObj )
			break;
		case 0x000B: // rotate
			var dir = ( targObj.direction | 0 ) + 1;
			if( dir > 0x7 )
				dir = 0x0;
			targObj.direction = dir;
			socket.SysMessage( "You rotate the mannequin a little bit." );
			break;
		case 0x000C: // stats gump
			OpenMannequinStatsGump( pUser.socket, targObj, 0 );
			break;
		case 0x000D: // Customize Body
			CustomizeMannequinBody( pUser.socket );
			pUser.SetTempTag( "MANN_STATS_MANN", "" + targObj.serial );
			break;
		case 0x000E: // Switch Clothes
			pUser.SetTempTag( "MANN_STATS_MANN", "" + targObj.serial );
			switchMannequinClothing( pUser, targObj );
			break;
		//case 0x000F: // CompareWithItemInSlot
		//	pUser.SetTempTag("MANN_STATS_MANN", "" + targObj.serial);
		//	break;
		case 0x0001: // Add Description
			pUser.SetTempTag( "MANN_STATS_MANN", "" + targObj.serial );
			DescriptionGump( socket );
			break;
		case 0x0002: // Redeed
			moveMannequinGearToPlayerPack( targObj, pUser );
			CreateDFNItem( pUser.socket, pUser, "mannequindeed", 1, "ITEM", true );
			targObj.Delete();
			break;
	}
	return false;
}

/** @type { ( currChar: Character, targChar: Character ) => boolean } */
function onCharDoubleClick( pUser, targChar )
{
	TriggerEvent( 18000, "OpenPaperdoll", pUser.socket, targChar )
	return false;
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	var tooltipText = "";
	var desc = myObj.GetTag("Description");
	if(desc != "" && desc != 0)
	{
		tooltipText = "Description: " + desc;
	}

	myObj.SetTempTag( "clilocTooltip", 1159410 ); // Description: ~1_MESSAGE~
	return tooltipText;
}

/** @type { ( speech: string, personTalking: Character, talkingTo: BaseObject ) => null | undefined | number | boolean } */
function onSpeech( strSaid, pTalking, pTalkingTo )
{
	return 1;
}

function DescriptionGump( socket )
{
	var gump = new Gump;
	gump.AddBackground(50, 50, 400, 300, 0xA28);
	gump.AddPage(0);
	gump.AddXMFHTMLTok(50, 70, 400, 20, false, false,  0x7FFF ,1159409, "", "", "" ); // <CENTER>Mannequin</CENTER>
	gump.AddXMFHTMLTok(75, 95, 350, 145, true, true, 0x7FFF, 1159408, "", "", ""); // Enter the description to add to the mannequin. Leave the text area blank to remove any existing text.
	gump.AddButton(125, 300, 0x81A, 0x81B, 1, 0, 8);
	gump.AddButton(320, 300, 0x819, 0x818, 1, 0, 0);
	gump.AddTiledGump(75, 245, 350, 40, 0xDB0);
	gump.AddTiledGump(76, 245, 350, 2, 0x23C5);
	gump.AddTiledGump(75, 245, 2, 40, 0x23C3);
	gump.AddTiledGump(75, 285, 350, 2, 0x23C5);
	gump.AddTiledGump(425, 245, 2, 42, 0x23C3);
	gump.AddTextEntryLimited(78, 246, 343, 37, 0x4FF, 0, 2, " ", 44);

	gump.Send( socket );
	gump.Free();
}

function CustomizeMannequinBody( socket )
{
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 300, 130, 0x13BE );
	gump.AddTiledGump( 10, 10, 280, 20, 0xA40 );
	gump.AddTiledGump( 10, 40, 280, 80, 0xA40 );
	gump.AddCheckerTrans( 10, 10, 280, 110);
	gump.AddXMFHTMLTok( 10, 12, 280, 18, false, false, 0x7FFF, 1151582, "", "", "" );// <center>CUSTOMIZE BODY</center>

	if( Era_Type >= EraStringToNum( "lbr" ))
	{
		gump.AddXMFHTMLTok( 45, 52, 180, 18, false, false, 0x7FFF, 1072255, "", "", "" );// Human
		gump.AddButton( 10, 50, 0xFA5, 1, 0, 1 );
	}

	if( Era_Type >= EraStringToNum( "ml" ))
	{
		gump.AddXMFHTMLTok(45, 72, 180, 18, false, false, 0x7FFF, 1072256, "", "", "" );// Elf
		gump.AddButton(10, 70, 0xFA5, 1, 0, 2 );

		gump.AddXMFHTMLTok(45, 92, 180, 18, false, false, 0x7FFF, 1029613, "", "", "" );// Gargoyle
		gump.AddButton(10, 90, 0xFA5,  1, 0, 3 );
	}

	gump.AddXMFHTMLTok( 205, 52, 180, 18, false, false, 0x7FFF, 1015327, "", "", "" );// Male
	gump.AddButton( 170, 50, 0xFA5, 1, 0, 4 );

	gump.AddXMFHTMLTok( 205, 72, 180, 18, false, false, 0x7FFF, 1015328, "", "", "" );// Female
    gump.AddButton( 170, 70, 0xFA5, 1, 0, 5 );

	gump.Send( socket );
	gump.Free();
}

var Icons = {
	StrengthBonus:	ABS( 120, 30 ), DexterityBonus:	ABS( 90, 0),		IntelligenceBonus:	ABS( 240, 0 ),
	HitPointsInc:	ABS( 180, 0 ),	StaminaInc:		ABS( 90,30),

	ManaInc:		ABS( 0, 30 ),	HPRegen:		ABS( 30, 60 ),		StaminaRegen:		ABS( 210, 60 ), 
	ManaRegen:		ABS( 120, 60 ),

	PhysResist:		ABS( 30, 30),	FireResist:		ABS( 150, 0 ),		ColdResist:			ABS( 0, 0 ),
	PoisonResist:	ABS( 60, 30),	EnergyResist:	ABS( 120, 0 ),

	DamageEater:	ABS( 270, 60 ), KineticEater:	ABS(  0,90 ),		FireEater:			ABS( 30, 90 ),
	ColdEater:		ABS( 60, 90 ),	PoisonEater:	ABS( 90, 90 ),		EnergyEater:		ABS( 120, 90 ),

	DamageIncrease:	ABS( 60, 0 ),	DCI:			ABS( 210, 30 ),		HCI:				ABS( 210, 0 ),
	SSI:			ABS( 150, 30 ),	LMC:			ABS( 60, 60 ),		Medable:			ABS( 0, 150 ),
	ReflectPhys:	ABS( 150, 60 ), EnhancePotions:	ABS( 240, 30 ),

	Luck:			ABS( 270, 0 ),

	PhysicalDamage:	ABS( 270,210 ),	ColdDamage:		ABS( 210, 210 ),	FireDamage:			ABS(180,210),
	PoisonDamage:	ABS( 0,240 ),	EnergyDamage:	ABS( 240, 210 ),	ChaosDamage:		ABS( 30,240),

	FasterCasting:	ABS( 270, 30 ),	fasterRecovery:	ABS( 0,60 ),		LowerRegentCost:	ABS(180,60),
	SpellDamage:	ABS( 90, 600 )
};

// ---- UI model --------------------------------------------------------------
var PAGE1 = [
	{
		header: 1049593, rowsLeft: [
			{ key: "StrengthBonus", text: 1079767, icon: Icons.StrengthBonus },
			{ key: "DexterityBonus", text: 1079732, icon: Icons.DexterityBonus },
			{ key: "IntelligenceBonus", text: 1079756, icon: Icons.IntelligenceBonus },
			{ key: "HitPointsInc", text: 1079404, icon: Icons.HitPointsInc, cap: "/25" },
			{ key: "StaminaInc", text: 1079405, icon: Icons.StaminaInc, cap: "/25" }
		], rowsRight: [
			{ key: "ManaInc", text: 1079406, icon: Icons.ManaInc, cap: "/25" },
			{ key: "ManaInc", text: 1079406, icon: Icons.ManaInc },
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

var PAGE2 = [
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

function getItemLo( item )
{
	return ( !item || !ValidateObject( item )) ? 0 : ( parseInt( item.lodamage, 10 ) || 0 );
}

function getItemHi( item )
{
	return ( !item || !ValidateObject( item )) ? 0 : ( parseInt( item.hidamage, 10 ) || 0 );
}

function getWeaponDamage( mannequin )
{
	var best = { lo: 0, hi: 0 };
	if(!ValidateObject( mannequin ))
		return best;

	var cand = [];
	var right = mannequin.FindItemLayer( 0x01 );
	var left = mannequin.FindItemLayer( 0x02 );

	if( ValidateObject( right ))
		cand.push( right );

	if( ValidateObject( left ))
		cand.push( left );

	for( var i = 0; i < cand.length; i++ )
	{
		var item = cand[i];
		var lo = getItemLo( item );
		var hi = getItemHi( item );
		if( hi >= lo && hi > 0 )
		{
			if( hi > best.hi ) 
			{
				best.lo = lo; best.hi = hi;
			}
		}
	}
	return best;
}

function getItemDef( item )
{
	if( !ValidateObject( item ))
		return 0;
	var value = parseInt( item.def, 10 );
	return isNaN( value ) ? 0 : value;
}

function sumPhysResistFromLayers( mannequin )
{
	if( !ValidateObject( mannequin ))
		return 0;

	var total = 0, item;
	for( var idx = 0; idx < ARMOR_LAYERS.length; idx++ )
	{
		item = mannequin.FindItemLayer( ARMOR_LAYERS[idx] );
		total += getItemDef( item );
	}
	return total;
}

function safeInt( value )
{ 
	value = parseInt( value, 10 );
	return isNaN( value ) ? 0 : value;
}

function sumProp( mannequin, propName, raw )
{
	if( !ValidateObject( mannequin )) 
		return 0;

	var total = 0;
	for( var k = 0; k < RESIST_LAYERS.length; k++)
	{
		var it = mannequin.FindItemLayer( RESIST_LAYERS[k] );
		total += ( !ValidateObject( it )) ? 0 : safeInt( it[propName] );
	}
	return raw === true ? total : (total | 0);
}

function buildContext( mannequin )
{
	var ctx = { physResist: sumPhysResistFromLayers( mannequin ) | 0 };

	var coldRaw = sumProp( mannequin, "resistCold", true );
	var fireRaw = sumProp( mannequin, "resistHeat", true );
	var energyRaw = sumProp( mannequin, "resistLightning", true );
	var poisonRaw = sumProp( mannequin, "resistPoison", true );

	ctx.coldResist   = coldRaw   | 0;
	ctx.fireResist   = fireRaw   | 0;
	ctx.energyResist = energyRaw | 0;
	ctx.poisonResist = poisonRaw | 0;

	ctx.hpRegenBonus = sumProp( mannequin, "healthRegenBonus" );
	ctx.staminaRegenBonus = sumProp( mannequin, "staminaRegenBonus" );
	ctx.manaRegenBonus = sumProp( mannequin, "manaRegenBonus" );

	ctx.hpInc = sumProp( mannequin, "healthBonus" );
	ctx.stamInc = sumProp( mannequin, "staminaBonus" );
	ctx.manaInc = sumProp( mannequin, "manaBonus" );

	ctx.hci = sumProp( mannequin, "hitChance" );
	ctx.dci = sumProp( mannequin, "defenseChance" );
	ctx.di = sumProp( mannequin, "damageIncrease" );
	ctx.luck = sumProp( mannequin, "luck");
	ctx.ssi = sumProp( mannequin, "swingSpeedIncrease" );

	var wd = getWeaponDamage( mannequin );
	ctx.physDmgLo = wd.lo | 0;
	ctx.physDmgHi = wd.hi | 0;

	ctx._raw = { cold: coldRaw, fire: fireRaw, energy: energyRaw, poison: poisonRaw };
	return ctx;
}

function filterSectionsForEra( sections )
{
    var out = [];
    var eraLBR = EraStringToNum( "lbr" );
    var eraAOS = EraStringToNum( "aos" );
    var eraML  = EraStringToNum( "ml" );

    var isLBRonly = ( Era_Type >= eraLBR && Era_Type < eraAOS );
    var isAOSonly = ( Era_Type >= eraAOS && Era_Type < eraML );
    var isMLplus  = ( Era_Type >= eraML );

    function cloneRow( row )
	{
        var r = {};
        for( var k in row )
			if( row.hasOwnProperty( k ))
				r[k] = row[k];
        return r;
    }

    if( isMLplus )
    {
        for( var s = 0; s < sections.length; s++ )
        {
            var src = sections[s] || {};
            var sec = { header: src.header, rowsLeft: [], rowsRight: [] };

            var leftHand = src.rowsLeft  || [];
            var rightHand = src.rowsRight || [];

            for( var i = 0; i < leftHand.length; i++ )
				sec.rowsLeft.push( cloneRow( leftHand[i]||{} ) );

            for(var j = 0; j < rightHand.length; j++ )
				sec.rowsRight.push( cloneRow( rightHand[j]||{} ) );

            if( sec.rowsLeft.length || sec.rowsRight.length )
				out.push(sec);
        }
        return out;
    }

    function includeByEraKey(k)
	{
        k = k || "";
        if( isLBRonly )
        {
            // LBR: only these
            return ( k === "StrengthBonus" || k === "DexterityBonus" || k === "IntelligenceBonus" ||
                    k === "PhysResist"    || k === "PhysDamage" );
        }
        if( isAOSonly )
        {
            // AoS: everything except eaters
            return !( k === "DamageEater" || k === "KineticEater" || k === "FireEater" ||
                     k === "ColdEater"   || k === "PoisonEater"  || k === "EnergyEater" );
        }
        // pre-LBR fallback
        return true;
    }

    for( var s2 = 0; s2 < sections.length; s2++ )
    {
        var src2 = sections[s2] || {};
        var sec2 = { header: src2.header, rowsLeft: [], rowsRight: [] };

        var L2 = src2.rowsLeft  || [];
        var R2 = src2.rowsRight || [];

        for( var i2 = 0; i2 < L2.length; i2++ )
        {
            var rightHand = L2[i2] || {};
            if( includeByEraKey( rightHand.key ))
				sec2.rowsLeft.push( cloneRow( rightHand ));
        }
        for( var j2 = 0; j2 < R2.length; j2++ )
        {
            var r2 = R2[j2] || {};
            if( includeByEraKey( r2.key ))
				sec2.rowsRight.push( cloneRow(r2 ));
        }

        if( sec2.rowsLeft.length || sec2.rowsRight.length ) out.push( sec2 );
    }
    return out;
}

var MANN_CTX = null;

function fmtVal( row )
{
	var base = 0;

	if( MANN_CTX && row && row.key )
	{
		switch( row.key )
		{
			case "PhysResist": base = MANN_CTX.physResist | 0; break;
			case "ColdResist": base = MANN_CTX.coldResist | 0; break;
			case "FireResist": base = MANN_CTX.fireResist | 0; break;
			case "EnergyResist": base = MANN_CTX.energyResist | 0; break;
			case "PoisonResist": base = MANN_CTX.poisonResist | 0; break;

			case "HPRegen": base = MANN_CTX.hpRegenBonus | 0; break;
			case "StaminaRegen": base = MANN_CTX.staminaRegenBonus | 0; break;
			case "ManaRegen": base = MANN_CTX.manaRegenBonus | 0; break;

			case "HitPointsInc": base = MANN_CTX.hpInc | 0; break;
			case "StaminaInc": base = MANN_CTX.stamInc | 0; break;
			case "ManaInc": base = MANN_CTX.manaInc | 0; break;

			case "HCI": base = MANN_CTX.hci | 0; break;
			case "DCI": base = MANN_CTX.dci | 0; break;
			case "DamageIncrease": base = MANN_CTX.di | 0; break;
			case "Luck": base = MANN_CTX.luck | 0; break;
			case "SSI": base = MANN_CTX.ssi | 0; break;

			case "PhysDamage":
				var lo = MANN_CTX.physDmgLo | 0, hi = MANN_CTX.physDmgHi | 0;
				return "<BASEFONT COLOR=#80BFFF>" + lo + "-" + hi + "</BASEFONT>";
		}
	}

	var cap = ( row && row.cap ) ? row.cap : "";
	return "<BASEFONT COLOR=#80BFFF>" + base + cap + "</BASEFONT>";
}

function ABS( sx, sy, w, h )
{
	return { sx: sx | 0, sy: sy | 0, w: w || 30, h: h || 30 };
}

function addHeader( gump, y, header )
{
	if( typeof header === "number" )
		gump.AddXMFHTMLTok( 10, y, 200, 18, false, false, 0x560A, header, "", "", "" );
	else
		gump.AddHTMLGump( 10, y, 200, 18, false, false, header );
}

function addRowLabel( gump, left, top, label )
{
	if( typeof label === "number" )
		gump.AddXMFHTMLTok( left, top, 220, 18, false, false, 0x560A, label, "", "", "" );
	else
		gump.AddHTMLGump( left, top, 220, 18, false, false, label );
}

function drawRowLeft( gump, y, row )
{
	gump.AddPicInPic( 10, y - 5, 0x09D3B, row.icon.sx | 0, row.icon.sy | 0, 30 || 30, 30 || 30 );
	addRowLabel( gump, 45, y, row.text );
	gump.AddHTMLGump(235, y, 110, 18, false, false, fmtVal( row ));
	return y + 30;
}

function drawRowRight( gump, y, row )
{
	gump.AddPicInPic( 307, y - 5, 0x09D3B, row.icon.sx | 0, row.icon.sy | 0, 30 || 30, 30 || 30 );
	addRowLabel( gump, 342, y, row.text );
	gump.AddHTMLGump( 532, y, 110, 18, false, false, fmtVal( row ));
	return y + 30;
}

function drawSection( gump, y, section )
{
	addHeader( gump, y, section.header );
	gump.AddTiledGump( 10, y + 18, 584, 5, 0x06DC );
	y += 32;

	var L = section.rowsLeft || [];
	var R = section.rowsRight || [];
	var max = ( L.length > R.length ? L.length : R.length );

	var baseY = y;
	for( var i = 0; i < max; i++ )
	{
		if( i < L.length )
		{
			y = drawRowLeft( gump, y, L[i] );
		}
		else
		{
			y += 30;
		}

		var ry = baseY + ( i * 30 );
		if( i < R.length )
			drawRowRight( gump, ry, R[i] );
	}
	return y + 3;
}

function OpenMannequinStatsGump( socket, mannequin, page )
{
	page = page | 0;

	MANN_CTX = buildContext( mannequin );

	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 604, 820, 0x06DB );
	gump.AddCheckerTrans( 0, 0, 604, 820 );
	gump.AddXMFHTMLTok( 10, 10, 584, 18, false, false, 0x43F7, 1114513, "#1159279", "#1159279", "#1159279" );

	// pager
	if( page > 0 )
	{
		gump.AddButton( 554, 10, 0x15E3, 1, 0, 7 );
	}
	else
	{
		gump.AddButton( 554, 10, 0x15E1, 1, 0, 6 );
	}

	var sections = ( page > 0 ? PAGE2 : PAGE1 );
	sections = filterSectionsForEra( sections );

	var y = 28;
	for( var s = 0; s < sections.length; s++ )
	{
		y = drawSection( gump, y, sections[s] );
	}

	gump.Send( socket );
	gump.Free();

	MANN_CTX = null;
}

function moveMannequinGearToPlayerPack( mannequin, pUser )
{
    if(!ValidateObject( mannequin ) || !ValidateObject( pUser ))
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
    for( var i = 0; i < RESIST_LAYERS.length; i++ )
    {
        var layer = RESIST_LAYERS[i];
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

function packCanFit( pUser, item )
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

function dropAtFeet( pUser, item )
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

function placeInPackOrDrop( pUser, item )
{
	if( !ValidateObject( pUser ) || !ValidateObject( item ))
		return {packed:false, dropped:false};

	var isPack = pUser.pack;
	if( ValidateObject( isPack ) && packCanFit( pUser, item ))
	{
		item.container = isPack;
		item.PlaceInPack();
		item.Refresh();
		return {packed:true, dropped:false};
	}

	dropAtFeet( pUser, item );
	return {packed:false, dropped:true};
}

function equipOn( char, item, ownerForFallback )
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
		return placeInPackOrDrop( ownerForFallback, item );

	return {equipped:false, packed:false, dropped:false};
}

function isSwapFail( res )
{
	return ( res && !res.equipped && ( res.packed || res.dropped )) ? 1 : 0;
}

function swapLayer( pUser, mannequin, layer )
{
	var u = pUser.FindItemLayer( layer );
	var m = mannequin.FindItemLayer( layer );

	var moved = 0, failed = 0;

	if( !ValidateObject( u ) && !ValidateObject( m ))
		return {moved:0, failed:0};

	if( ValidateObject( u ) && ValidateObject( m ))
	{
		placeInPackOrDrop( pUser, u );

		var r1 = equipOn( pUser, m, pUser );
		failed += isSwapFail( r1 );
		moved++;

		var r2 = equipOn( mannequin, u, pUser );
		failed += isSwapFail( r2 );
		moved++;

		return {moved:moved, failed:failed};
	}

	if( !ValidateObject( u ) && ValidateObject( m ))
	{
		var r3 = equipOn( pUser, m, pUser );
		failed += isSwapFail( r3 ); moved++;
		return {moved:moved, failed:failed};
	}

	if( ValidateObject( u ) && !ValidateObject( m ))
	{
		var r4 = equipOn( mannequin, u, pUser );
		failed += isSwapFail( r4 ); moved++;
		return {moved:moved, failed:failed};
	}

	return {moved:0, failed:failed};
}

function swapHands( pUser, mannequin )
{
	var moved = 0, failed = 0;

	var userRight = pUser.FindItemLayer( 0x01 );
	var userLeft = pUser.FindItemLayer( 0x02 );
	var mannequinRight = mannequin.FindItemLayer( 0x01 );
	var mannequinLeft = mannequin.FindItemLayer( 0x02 );

	if( ValidateObject( userRight ))
	{
		placeInPackOrDrop( pUser, userRight );
		moved++;
	}

	if( ValidateObject( userLeft ))
	{ 
		placeInPackOrDrop( pUser, userLeft );
		moved++;
	}

	if( ValidateObject( mannequinRight ))
	{ 
		var a = equipOn( pUser, mannequinRight, pUser );
		failed += isSwapFail( a );
		moved++;
	}

	if( ValidateObject( mannequinLeft ))
	{
		var b = equipOn( pUser, mannequinLeft, pUser );
		failed += isSwapFail( b );
		moved++;
	}

	if( ValidateObject( userRight ))
	{
		var c = equipOn( mannequin, userRight, pUser );
		failed += isSwapFail( c );
	}

	if( ValidateObject( userLeft ))
	{
		var d = equipOn( mannequin, userLeft, pUser );
		failed += isSwapFail( d );
	}

	return {moved:moved, failed:failed};
}

function switchMannequinClothing( pUser, mannequin )
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
	for( var i = 0; i < EQUIP_LAYERS.length; i++ )
	{
		var isLayer = EQUIP_LAYERS[i];
		if( isLayer === 0x01 || isLayer === 0x02 ) continue;
		var r = swapLayer( pUser, mannequin, isLayer );
		totalMoved += r.moved;
		totalFailed += r.failed;
	}

	var hr = swapHands( pUser, mannequin );
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

function setRaceGender(  mannequin, race, gender, pUser )
{
    race = ( race == null
        ? ( isFinite( mannequin.race ) ? ( mannequin.race - 0 )
            : ( mannequin.id===0x0190||mannequin.id===0x0191 )?0
            : ( mannequin.id===0x025D||mannequin.id===0x025E )?1
            : ( mannequin.id===0x029A||mannequin.id===0x029B )?2
            : 0 )
        : ( race - 0 ));

    gender = gender ? 1 : 0;

    if(( mannequin.race === 2 ) || ( race === 2 ))
        moveMannequinGearToPlayerPack( mannequin, pUser );

    mannequin.race = race;
    mannequin.gender = gender;
    mannequin.orgID = ( mannequin.id = ( [[0x0190,0x0191],[0x025D,0x025E],[0x029A,0x029B]] )[race][gender]);

	mannequin.Refresh();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, iButton, gumpData )
{
	var pUser = pSock.currentChar;
	var mannequinSer = pUser.GetTempTag( "MANN_STATS_MANN" );
	var mannequin = CalcCharFromSer( mannequinSer );
	if( !ValidateObject( pUser )) 
	{
		return;
	}

	switch( iButton )
	{
		case 0: break;
		case 1: setRaceGender( mannequin, 0, mannequin.gender, pUser ); break; // Human
		case 2: setRaceGender( mannequin, 1, mannequin.gender, pUser ); break; // Elf
		case 3: setRaceGender( mannequin, 2, mannequin.gender, pUser ); break; // Gargoyle
		case 4: setRaceGender( mannequin, null, 0, pUser ); break;   // Male
		case 5: setRaceGender( mannequin, null, 1, pUser ); break;   // Female
		case 6:
		case 7:
			if( ValidateObject( mannequin ))
				OpenMannequinStatsGump( pSock, mannequin, ( iButton === 6 ? 1 : 0 ));
			break;
		case 8:
			{
				var raw = gumpData.getEdit( 0 );
				var res = TriggerEvent( 18002, "validateDescription", raw, true );

				if( res.cleared )
				{
					mannequin.SetTag( "Description", "" );   // or mann.DelTag("Description") if you prefer removing it
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
