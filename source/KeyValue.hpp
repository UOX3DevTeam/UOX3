// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/22/21

#ifndef KeyValue_hpp
#define KeyValue_hpp

#include <cstdint>
#include <string>
#include <map>
#include <limits>

/*
enum class KeyValueType {
	undefined=-1,numeric,text, uppertext,nodata,unknown,doublenumeric};

*/

struct KeyValue {
	enum KeyType {
		undefined = -1, armorclass = 0, addmenuitem, advanceobj, alchemy,
		ammo, ammofx, amount, anatomy, animallore, archery, armslore, attack,
		backpack, baserange, begging, blacksmithing, bowcraft, bushido, camping,
		carpentry, cartography, carve, chivalry, cold, color, colorlist,
		colormatchhair, cooking, corpse, creator, customstringtag, custominttag,
		damage, decay, defense, detecthidden, dexterity, dexterity2, direction,
		dispellable, disabled, doorflag, dye, dyebeard, dyehair, elementresist,
		emotecolor, enticement, equipitem, evaluteintel, fame, fencing, fishing,
		flag, flee, fleespeed, focus, forensics, fx1, fx2, fy1, fy2, fz1, food,
		get, glow, glowbc, glowtype, gold, good, haircolor, healing, heat,
		herding, high_damage, hiding, hitpoint, identification,
		imbuing, intaddition, intelligence, interval, inscription, itemid,
		itemlist, karma, killhair, killbeard, killpack, layer, light,
		lightning, lockpicking, low_damage, loot, lumberjacking, macefighting,
		magery, magicresistance, mana, maxmana, maxhitpoint, maxitems, maxrange,
		meditation, mining, movable, more, more2, morex, morey, morez,
		musicianship, mysticism, name, name2, namelist, necromancy, newbie,
		ninjitsu, notrain, npcai, npclist, npcwander, offspell, packitem, parrying,
		peacemaking, stackable, poisondamage, poisonstrength, poisoned, poisoning,
		poly, privilege, provocation, race, rain, rank, reattackat, removetraps,
		restock, resistfire, resistcold, resistlightening, resistpoison,
		rshopitem, runningspeed, runs, saycolor, script, sellitem,
		shopitem, shopkeeper, shoplist, skill_made, skill, skillvar, skin,
		skinlist, snooping, snow, spadelay, spattack, spawnobject, spawnobject_list,
		speed, spellweaving, spiritspeak, split, splitchance, stamina, maxstamina,
		strength, stradd, stealing, stealth, swordsmanship, tatics, tailoring,
		taming, tamedhunger, tasteid, throwing, tinkering, title, to_tame,
		to_prov, to_peace, tracking, type, worth, veterinary, isvisible,
		walkingspeed, weigth, maxweight, will_hunger, wipe, wrestling,location };
	
	static const std::map<std::string, KeyType> _keyenum_mapping ;
//	static const std::map<KeyType, KeyValueType> _keytype_mapping ;

	std::string fileorigin;
	int linenumber ;
	bool typecheck;
	std::string original_line ;
	
	std::string key ;
	std::string upper_key ;
	KeyType enum_key ;
	//KeyValueType value_type ;
	
	std::string text ;
	std::string upper_text;
	int first_number ;
	int second_number ;
	
	KeyValue();
	std::string description() const ;
	std::string origin() const ;
	static KeyType lookupKeyEnum(const std::string &key);
//	static KeyValueType lookupKeyType(KeyType keyenum);
};

#endif /* KeyValue_hpp */
