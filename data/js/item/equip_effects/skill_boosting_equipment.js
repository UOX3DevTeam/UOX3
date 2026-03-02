/// <reference path="../../definitions.d.ts" />
// @ts-check
// Skill-boosting Equipment

// Original item rewards
// 		Leather gloves of mining
//			Increases mining skill by 1 point. Dyeable using a leather dyetub.
//
// 		Studded leather gloves of mining
//			Increases mining skill by 3point. Dyeable using a leather dyetub.
//
// 		Ringmail gloves of mining
//			Increases mining skill by 5 points.
//
// 		Ancient smithy hammer
//			Increase blacksmithing skill by 10, 15, 30 or 60 points when equipped

/** @type { ( equipper: Character, equipping: Item ) => boolean } */
function onEquip( pEquipper, iEquipped )
{
	var skillBonusID = iEquipped.GetTag( "skillBonusID" );
	var skillBonusVal = iEquipped.GetTag( "skillBonusVal" );

	switch( skillBonusID )
	{
		case 0: pEquipper.skills.alchemy += skillBonusVal; break;
		case 1: pEquipper.skills.anatomy += skillBonusVal; break;
		case 2: pEquipper.skills.animallore += skillBonusVal; break;
		case 3: pEquipper.skills.itemid += skillBonusVal; break;
		case 4: pEquipper.skills.armslore += skillBonusVal; break;
		case 5: pEquipper.skills.parrying += skillBonusVal; break;
		case 6: pEquipper.skills.begging += skillBonusVal; break;
		case 7: pEquipper.skills.blacksmithing += skillBonusVal; break;
		case 8: pEquipper.skills.bowcraft += skillBonusVal; break;
		case 9: pEquipper.skills.peacemaking += skillBonusVal; break;
		case 10: pEquipper.skills.camping += skillBonusVal; break;
		case 11: pEquipper.skills.carpentry += skillBonusVal; break;
		case 12: pEquipper.skills.cartography += skillBonusVal; break;
		case 13: pEquipper.skills.cooking += skillBonusVal; break;
		case 14: pEquipper.skills.detectinghidden += skillBonusVal; break;
		case 15: pEquipper.skills.enticement += skillBonusVal; break;
		case 16: pEquipper.skills.evaluatingintel += skillBonusVal; break;
		case 17: pEquipper.skills.healing += skillBonusVal; break;
		case 18: pEquipper.skills.fishing += skillBonusVal; break;
		case 19: pEquipper.skills.forensics += skillBonusVal; break;
		case 20: pEquipper.skills.herding += skillBonusVal; break;
		case 21: pEquipper.skills.hiding += skillBonusVal; break;
		case 22: pEquipper.skills.provocation += skillBonusVal; break;
		case 23: pEquipper.skills.inscription += skillBonusVal; break;
		case 24: pEquipper.skills.lockpicking += skillBonusVal; break;
		case 25: pEquipper.skills.magery += skillBonusVal; break;
		case 26: pEquipper.skills.magicresistance += skillBonusVal; break;
		case 27: pEquipper.skills.tactics += skillBonusVal; break;
		case 28: pEquipper.skills.snooping += skillBonusVal; break;
		case 29: pEquipper.skills.musicianship += skillBonusVal; break;
		case 30: pEquipper.skills.poisoning += skillBonusVal; break;
		case 31: pEquipper.skills.archery += skillBonusVal; break;
		case 32: pEquipper.skills.spiritspeak += skillBonusVal; break;
		case 33: pEquipper.skills.stealing += skillBonusVal; break;
		case 34: pEquipper.skills.tailoring += skillBonusVal; break;
		case 35: pEquipper.skills.taming += skillBonusVal; break;
		case 36: pEquipper.skills.tasteid += skillBonusVal; break;
		case 37: pEquipper.skills.tinkering += skillBonusVal; break;
		case 38: pEquipper.skills.tracking += skillBonusVal; break;
		case 39: pEquipper.skills.veterinary += skillBonusVal; break;
		case 40: pEquipper.skills.swordsmanship += skillBonusVal; break;
		case 41: pEquipper.skills.macefighting += skillBonusVal; break;
		case 42: pEquipper.skills.fencing += skillBonusVal; break;
		case 43: pEquipper.skills.wrestling += skillBonusVal; break;
		case 44: pEquipper.skills.lumberjacking += skillBonusVal; break;
		case 45: pEquipper.skills.mining += skillBonusVal; break;
		case 46: pEquipper.skills.meditation += skillBonusVal; break;
		case 47: pEquipper.skills.stealth += skillBonusVal; break;
		case 48: pEquipper.skills.removetrap += skillBonusVal; break;
		case 49: pEquipper.skills.necromancy += skillBonusVal; break;
		case 50: pEquipper.skills.focus += skillBonusVal; break;
		case 51: pEquipper.skills.chivalry += skillBonusVal; break;
		case 52: pEquipper.skills.bushido += skillBonusVal; break;
		case 53: pEquipper.skills.ninjitsu += skillBonusVal; break;
		case 54: pEquipper.skills.spellweaving += skillBonusVal; break;
		case 55: pEquipper.skills.mysticism += skillBonusVal; break;
		case 56: pEquipper.skills.imbuing += skillBonusVal; break;
	}
}

/** @type { ( equipper: Character, equipping: Item ) => boolean } */
function onUnequip( pUnequipper, iUnequipped )
{
	var skillBonusID = iUnequipped.GetTag( "skillBonusID" );
	var skillBonusVal = iUnequipped.GetTag( "skillBonusVal" );

	switch( skillBonusID )
	{
		case 0: pUnequipper.skills.alchemy -= skillBonusVal; break;
		case 1: pUnequipper.skills.anatomy -= skillBonusVal; break;
		case 2: pUnequipper.skills.animallore -= skillBonusVal; break;
		case 3: pUnequipper.skills.itemid -= skillBonusVal; break;
		case 4: pUnequipper.skills.armslore -= skillBonusVal; break;
		case 5: pUnequipper.skills.parrying -= skillBonusVal; break;
		case 6: pUnequipper.skills.begging -= skillBonusVal; break;
		case 7: pUnequipper.skills.blacksmithing -= skillBonusVal; break;
		case 8: pUnequipper.skills.bowcraft -= skillBonusVal; break;
		case 9: pUnequipper.skills.peacemaking -= skillBonusVal; break;
		case 10: pUnequipper.skills.camping -= skillBonusVal; break;
		case 11: pUnequipper.skills.carpentry -= skillBonusVal; break;
		case 12: pUnequipper.skills.cartography -= skillBonusVal; break;
		case 13: pUnequipper.skills.cooking -= skillBonusVal; break;
		case 14: pUnequipper.skills.detectinghidden -= skillBonusVal; break;
		case 15: pUnequipper.skills.enticement -= skillBonusVal; break;
		case 16: pUnequipper.skills.evaluatingintel -= skillBonusVal; break;
		case 17: pUnequipper.skills.healing -= skillBonusVal; break;
		case 18: pUnequipper.skills.fishing -= skillBonusVal; break;
		case 19: pUnequipper.skills.forensics -= skillBonusVal; break;
		case 20: pUnequipper.skills.herding -= skillBonusVal; break;
		case 21: pUnequipper.skills.hiding -= skillBonusVal; break;
		case 22: pUnequipper.skills.provocation -= skillBonusVal; break;
		case 23: pUnequipper.skills.inscription -= skillBonusVal; break;
		case 24: pUnequipper.skills.lockpicking -= skillBonusVal; break;
		case 25: pUnequipper.skills.magery -= skillBonusVal; break;
		case 26: pUnequipper.skills.magicresistance -= skillBonusVal; break;
		case 27: pUnequipper.skills.tactics -= skillBonusVal; break;
		case 28: pUnequipper.skills.snooping -= skillBonusVal; break;
		case 29: pUnequipper.skills.musicianship -= skillBonusVal; break;
		case 30: pUnequipper.skills.poisoning -= skillBonusVal; break;
		case 31: pUnequipper.skills.archery -= skillBonusVal; break;
		case 32: pUnequipper.skills.spiritspeak -= skillBonusVal; break;
		case 33: pUnequipper.skills.stealing -= skillBonusVal; break;
		case 34: pUnequipper.skills.tailoring -= skillBonusVal; break;
		case 35: pUnequipper.skills.taming -= skillBonusVal; break;
		case 36: pUnequipper.skills.tasteid -= skillBonusVal; break;
		case 37: pUnequipper.skills.tinkering -= skillBonusVal; break;
		case 38: pUnequipper.skills.tracking -= skillBonusVal; break;
		case 39: pUnequipper.skills.veterinary -= skillBonusVal; break;
		case 40: pUnequipper.skills.swordsmanship -= skillBonusVal; break;
		case 41: pUnequipper.skills.macefighting -= skillBonusVal; break;
		case 42: pUnequipper.skills.fencing -= skillBonusVal; break;
		case 43: pUnequipper.skills.wrestling -= skillBonusVal; break;
		case 44: pUnequipper.skills.lumberjacking -= skillBonusVal; break;
		case 45: pUnequipper.skills.mining -= skillBonusVal; break;
		case 46: pUnequipper.skills.meditation -= skillBonusVal; break;
		case 47: pUnequipper.skills.stealth -= skillBonusVal; break;
		case 48: pUnequipper.skills.removetrap -= skillBonusVal; break;
		case 49: pUnequipper.skills.necromancy -= skillBonusVal; break;
		case 50: pUnequipper.skills.focus -= skillBonusVal; break;
		case 51: pUnequipper.skills.chivalry -= skillBonusVal; break;
		case 52: pUnequipper.skills.bushido -= skillBonusVal; break;
		case 53: pUnequipper.skills.ninjitsu -= skillBonusVal; break;
		case 54: pUnequipper.skills.spellweaving -= skillBonusVal; break;
		case 55: pUnequipper.skills.mysticism -= skillBonusVal; break;
		case 56: pUnequipper.skills.imbuing -= skillBonusVal; break;
	}
}

// Display bonus skill in tooltip
/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip(myObj)
{
	var tooltipText = "";
	var skillBonusID = myObj.GetTag( "skillBonusID" );
	var skillBonusVal = myObj.GetTag( "skillBonusVal" );

	switch( skillBonusID )
	{
		case 0: tooltipText = "Alchemy: +" + (skillBonusVal / 10); break;
		case 1: tooltipText = "Anatomy: +" + (skillBonusVal / 10); break;
		case 2: tooltipText = "Animal Lore: +" + (skillBonusVal / 10); break;
		case 3: tooltipText = "Item Identification: +" + (skillBonusVal / 10); break;
		case 4: tooltipText = "Arms Lore: +" + (skillBonusVal / 10); break;
		case 5: tooltipText = "Parrying: +" + (skillBonusVal / 10); break;
		case 6: tooltipText = "Begging: +" + (skillBonusVal / 10); break;
		case 7: tooltipText = "Blacksmithing: +" + (skillBonusVal / 10); break;
		case 8: tooltipText = "Bowcraft/Fletching: +" + (skillBonusVal / 10); break;
		case 9: tooltipText = "Peacemaking: +" + (skillBonusVal / 10); break;
		case 10: tooltipText = "Camping: +" + (skillBonusVal / 10); break;
		case 11: tooltipText = "Carpentry: +" + (skillBonusVal / 10); break;
		case 12: tooltipText = "Cartography: +" + (skillBonusVal / 10); break;
		case 13: tooltipText = "Cooking: +" + (skillBonusVal / 10); break;
		case 14: tooltipText = "Detecting Hidden: +" + (skillBonusVal / 10); break;
		case 15: tooltipText = "Discordance: +" + (skillBonusVal / 10); break;
		case 16: tooltipText = "Evaluating Intelligence: +" + (skillBonusVal / 10); break;
		case 17: tooltipText = "Healing: +" + (skillBonusVal / 10); break;
		case 18: tooltipText = "Fishing: +" + (skillBonusVal / 10); break;
		case 19: tooltipText = "Forensic Evaluation: +" + (skillBonusVal / 10); break;
		case 20: tooltipText = "Herding: +" + (skillBonusVal / 10); break;
		case 21: tooltipText = "Hiding: +" + (skillBonusVal / 10); break;
		case 22: tooltipText = "Provocation: +" + (skillBonusVal / 10); break;
		case 23: tooltipText = "Inscription: +" + (skillBonusVal / 10); break;
		case 24: tooltipText = "Lockpicking: +" + (skillBonusVal / 10); break;
		case 25: tooltipText = "Magery: +" + (skillBonusVal / 10); break;
		case 26: tooltipText = "Resisting Spells: +" + (skillBonusVal / 10); break;
		case 27: tooltipText = "Tactics: +" + (skillBonusVal / 10); break;
		case 28: tooltipText = "Snooping: +" + (skillBonusVal / 10); break;
		case 29: tooltipText = "Musicianship: +" + (skillBonusVal / 10); break;
		case 30: tooltipText = "Poisoning: +" + (skillBonusVal / 10); break;
		case 31: tooltipText = "Archery: +" + (skillBonusVal / 10); break;
		case 32: tooltipText = "Spirit Speak: +" + (skillBonusVal / 10); break;
		case 33: tooltipText = "Stealing: +" + (skillBonusVal / 10); break;
		case 34: tooltipText = "Tailoring: +" + (skillBonusVal / 10); break;
		case 35: tooltipText = "Animal Taming: +" + (skillBonusVal / 10); break;
		case 36: tooltipText = "Taste Identification: +" + (skillBonusVal / 10); break;
		case 37: tooltipText = "Tinkering: +" + (skillBonusVal / 10); break;
		case 38: tooltipText = "Tracking: +" + (skillBonusVal / 10); break;
		case 39: tooltipText = "Veterinary: +" + (skillBonusVal / 10); break;
		case 40: tooltipText = "Swordsmanship: +" + (skillBonusVal / 10); break;
		case 41: tooltipText = "Mace Fighting: +" + (skillBonusVal / 10); break;
		case 42: tooltipText = "Fencing: +" + (skillBonusVal / 10); break;
		case 43: tooltipText = "Wrestling: +" + (skillBonusVal / 10); break;
		case 44: tooltipText = "Lumberjacking: +" + (skillBonusVal / 10); break;
		case 45: tooltipText = "Mining: +" + (skillBonusVal / 10); break;
		case 46: tooltipText = "Meditation: +" + (skillBonusVal / 10); break;
		case 47: tooltipText = "Stealth: +" + (skillBonusVal / 10); break;
		case 48: tooltipText = "Remove Trap: +" + (skillBonusVal / 10); break;
		case 49: tooltipText = "Necromancy: +" + (skillBonusVal / 10); break;
		case 50: tooltipText = "Focus: +" + (skillBonusVal / 10); break;
		case 51: tooltipText = "Chivalry: +" + (skillBonusVal / 10); break;
		case 52: tooltipText = "Bushido: +" + (skillBonusVal / 10); break;
		case 53: tooltipText = "Ninjitsu: +" + (skillBonusVal / 10); break;
		case 54: tooltipText = "Spellweaving: +" + (skillBonusVal / 10); break;
		case 55: tooltipText = "Mysticism: +" + (skillBonusVal / 10); break;
		case 56: tooltipText = "Imbuing: +" + (skillBonusVal / 10); break;
		default:
			tooltipText = "Skill " + skillBonusID + ": +" + (skillBonusVal / 10);
			break;
	}

	myObj.SetTempTag( "tooltipSortOrder", 46 );
	return tooltipText;
}
