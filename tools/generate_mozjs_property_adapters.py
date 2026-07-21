import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SPECS = ROOT / "source" / "UOXJSPropertySpecs.h"
ENUMS = ROOT / "source" / "UOXJSPropertyEnums.h"
OUTPUT = ROOT / "source" / "UOXJSPropertyAdapters.cpp"

FAMILIES = {
    "CSpell": "CSpellProps",
    "CGlobalSkill": "CGlobalSkillProps",
    "CCreateEntry": "CCreateEntryProps",
    "CTimer": "CTimerProps",
    "CRace": "CRaceProps",
    "CRegion": "CRegionProps",
    "CSpawnRegion": "CSpawnRegionProps",
    "CGuild": "CGuildProps",
    "CCharacter": "CCharacterProps",
    "CItem": "CItemProps",
    "CSocket": "CSocketProps",
    "CSkills": "CSkillsProps",
    "CGumpData": "CGumpDataProps",
    "CAccount": "CAccountProps",
    "CConsole": "CConsoleProps",
    "CScriptSection": "CScriptSectionProps",
    "CResource": "CResourceProps",
    "CParty": "CPartyProps",
    "CScript": "CScriptProps",
}

INTEGER_ENUMS = {
    "CSpell": "CSpell_Properties",
    "CGlobalSkill": "CGlobalSkill_Properties",
    "CCreateEntry": "CCreateEntry_Properties",
    "CTimer": "CTimer_Properties",
    "CGumpData": "CGumpData_Properties",
	"CCharacter": "CC_Properties",
	"CItem": "CI_Properties",
	"CSocket": "CSocket_Properties",
}

ENUM_PREFIXES = {
    "CSpell": "CSP",
    "CGlobalSkill": "CGSKILL",
    "CCreateEntry": "CMAKE",
    "CTimer": "TIMER",
    "CGumpData": "CGumpData",
	"CCharacter": "CCP",
	"CItem": "CIP",
	"CSocket": "CSOCKP",
}

DIRECT_FAMILIES = {
    "CSpell",
    "CGlobalSkill",
    "CCreateEntry",
    "CTimer",
    "CGumpData",
    "CResource",
    "CConsole",
    "CScriptSection",
    "CScript",
    "CParty",
    "CRace",
    "CSkills",
    "CRegion",
    "CSpawnRegion",
    "CGuild",
    "CSocket",
    "CAccount",
}

DIRECT_PROPERTIES = {
    "CItem": {"sectionID", "name", "title", "x", "y", "z", "id", "colour", "color",
              "visible", "serial", "health", "worldnumber", "worldNumber", "instanceID", "amount", "type",
              "owner", "container", "scriptTrigger", "scriptTriggers",
              "movable", "layer", "decayable", "name2", "maxhp", "maxUses", "usesLeft", "rank",
              "creator", "poison", "poisonedBy", "poisonCharges", "dir", "wipable", "weight",
              "weightMax", "baseWeight", "maxItems", "corpse", "desc"},
}
DIRECT_PROPERTIES["CCharacter"] = {
    "sectionID", "name", "origName", "title", "x", "y", "z", "oldX", "oldY", "oldZ", "id",
    "colour", "color", "skin", "hue", "controlSlots", "controlSlotsUsed", "orneriness", "visible",
    "serial", "health", "worldnumber", "instanceID"
}
DIRECT_PROPERTIES["CCharacter"].update({
    "owner", "scripttrigger", "scriptTriggers", "target", "dexterity", "intelligence", "strength",
    "actualDexterity", "actualIntelligence", "actualStrength", "healthRegenBonus", "staminaRegenBonus",
    "manaRegenBonus", "mana", "stamina", "pack"
})
DIRECT_PROPERTIES["CCharacter"].update({
    "skills", "race", "region", "town", "guild", "baseskills", "skillsused", "socket", "guarding",
    "skillLock", "skillCaps", "partyLootable", "party", "multi", "account"
})
DIRECT_PROPERTIES["CCharacter"].update({
    "setPeace", "hungerRate", "thirstRate", "gender", "guildNumber", "ownerCount", "petCount",
    "followerCount", "ownedItemsCount", "isGM", "canBroadcast", "singClickSer", "noSkillTitles",
    "isGMPageable", "canSnoop", "isCounselor", "noNeedMana", "isDispellable", "noNeedReags",
    "orgID", "orgSkin", "isAnimal", "isPackAnimal", "isHuman", "isShop", "maxLoyalty", "loyalty",
    "loyaltyRate", "shouldSave", "origin", "accountNum", "createdOn", "playTime", "housesOwned",
    "housesCoOwned", "tithing", "lastOn", "lastOnSecs"
})
DIRECT_PROPERTIES["CCharacter"].update({
    "aitype", "split", "splitchance", "hireling", "trainer", "weight", "squelch", "isJailed",
    "magicReflect", "permanentMagicReflect", "hideFameKarmaTitle", "noCharCollide", "tamed",
    "tamedHungerRate", "tamedThirstRate", "hungerWildChance", "thirstWildChance", "foodList", "mounted",
    "stabled", "isUsingPotion", "stealth", "skillToTame", "skillToProv", "skillToPeace", "poisonStrength",
    "isPolymorphed", "isIncognito", "isDisguised", "canRun", "isMeditating"
})
DIRECT_PROPERTIES["CCharacter"].update({
    "lodamage", "hidamage", "flag", "atWar", "spellCast", "isCasting", "priv", "townPriv",
    "guildTitle", "hairStyle", "hairColour", "hairColor", "beardStyle", "beardColour", "beardColor",
    "fontType", "sayColour", "emoteColour", "attacker", "raceGate", "deaths", "cell", "allmove",
    "houseicons", "spattack", "spdelay", "swingSpeedIncrease", "luck", "damageIncrease", "hitChance",
    "defenseChance"
})
DIRECT_PROPERTIES["CCharacter"].update({
    "npcGuild", "spawnSerial", "maxhp", "maxstamina", "maxmana", "oldWandertype", "wandertype",
    "fx1", "fy1", "fx2", "fy2", "fz", "spawnX", "spawnY", "spawnZ", "pathTargX", "pathTargY",
    "nextAct"
})
DIRECT_PROPERTIES["CCharacter"].update({
    "dead", "npc", "isAwake", "online", "direction", "isRunning", "isChar", "isItem", "isSpawner",
    "isonhorse", "isFlying", "isGuarded", "tempdex", "tempint", "tempstr", "poison", "poisonedBy",
    "lightlevel", "vulnerable", "willhunger", "willthirst"
})
DIRECT_PROPERTIES["CCharacter"].update({
    "fame", "karma", "attack", "canAttack", "karmaLock", "fleeAt", "reAttackAt", "brkPeaceChance",
    "hunger", "thirst", "frozen", "commandlevel", "hasStolen", "criminal", "murderer", "innocent",
    "murdercount", "neutral", "npcFlag"
})

DIRECT_PROPERTIES["CItem"].update({
    "more", "more0", "more1", "more2", "morex", "morey", "morez", "lodamage", "hidamage", "ac", "def",
    "resistCold", "resistHeat", "resistLight", "resistLightning", "resistPoison", "resistRain", "resistSnow",
    "damageCold", "damageHeat", "damageLight", "damageLightning", "damagePoison", "damageRain", "damageSnow",
    "speed", "swingSpeedIncrease", "damageIncrease", "healthLeech", "staminaLeech", "manaLeech",
    "hitChance", "defenseChance", "luck", "healthBonus", "staminaBonus", "manaBonus", "artifactRarity",
    "durabilityHpBonus", "lowerStateReq"
})
DIRECT_PROPERTIES["CItem"].update({
    "att", "itemsinside", "totalItemCount", "healthRegenBonus", "staminaRegenBonus", "manaRegenBonus",
    "buyvalue", "sellvalue", "vendorPrice", "restock", "good", "divinelock", "strength", "dexterity",
    "intelligence", "event", "maxRange", "baseRange", "origin", "stealable", "moveType", "tithing"
})
DIRECT_PROPERTIES["CItem"].update({
    "tempLastTraded", "tempTimer", "isChar", "isItem", "isSpawner", "isFieldSpell", "isLockedDown",
    "isShieldType", "isMetalType", "isLeatherType", "canBeLockedDown", "isContType", "carveSection",
    "ammoID", "ammoHue", "ammoFX", "ammoFXHue", "ammoFXRender", "isItemHeld"
})
DIRECT_PROPERTIES["CItem"].update({
    "shouldSave", "isNewbie", "isDispellable", "madeWith", "entryMadeFrom", "isPileable",
    "isMarkedByMaker", "isDyeable", "isDamageable", "isWipeable", "isGuarded", "isDoorOpen"
})
DIRECT_PROPERTIES["CItem"].update({
    "oldX", "oldY", "oldZ", "skin", "hue", "scripttrigger", "decaytime", "race", "multi",
    "region", "spawnSerial"
})
DIRECT_PROPERTIES["CItem"].update({
    "spawnsection", "sectionalist", "mininterval", "maxinterval"
})
DIRECT_PROPERTIES["CItem"].update({
    "lockdowns", "maxLockdowns", "trashContainers", "maxTrashContainers", "secureContainers",
    "maxSecureContainers", "friends", "maxFriends", "guests", "maxGuests", "owners", "maxOwners",
    "bans", "maxBans", "vendors", "maxVendors", "deed", "isPublic", "buildTimestamp",
    "tradeTimestamp", "banX", "banY"
})

ALIASES = {
	"color": "colour",
	"skin": "colour",
	"hue": "colour",
	"pack": "charpack",
	"skillsused": "skilluse",
	"brkPeaceChance": "brkpeace",
	"isAwake": "awake",
	"tempdex": "tdexterity",
	"tempint": "tintelligence",
	"tempstr": "tstrength",
	"willhunger": "hungerstatus",
	"willthirst": "thirststatus",
	"permanentMagicReflect": "permmagicreflect",
	"isUsingPotion": "usingpotion",
	"lowerStateReq": "lowerstatreq",
	"lockdowns": "lockddowns",
}


def normalized(name):
    return re.sub(r"[^a-z0-9]", "", name.lower())


def enum_values(text, enum_name):
    match = re.search(rf"enum\s+{enum_name}\s*\{{(.*?)\}};", text, re.S)
    if not match:
        raise RuntimeError(f"Missing enum {enum_name}")
    result = {}
    value = -1
    for entry in match.group(1).split(","):
        entry = re.sub(r"//.*", "", entry).strip()
        if not entry:
            continue
        token, _, explicit = entry.partition("=")
        token = token.strip()
        value = int(explicit.strip(), 0) if explicit else value + 1
        result[normalized(token)] = value
    return result


def main():
    specs = SPECS.read_text(encoding="utf-8")
    enums = ENUMS.read_text(encoding="utf-8")
    table_text = specs[specs.index("// Property table") :]
    properties = re.findall(r"UX_PSG(S?)\(\s*(\w+)\s*,\s*(\w+)\s*,", table_text)

    enum_maps = {
        main_name: enum_values(enums, enum_name)
        for main_name, enum_name in INTEGER_ENUMS.items()
    }
    skill_ids = {
        normalized(attr): index
        for index, (_, main_name, attr) in enumerate(
            prop for prop in properties if prop[1] == "CSkills"
        )
    }

    lines = [
        "// Generated by tools/generate_mozjs_property_adapters.py.",
        '#include "uox3.h"',
        '#include "UOXJSPropertyFuncs.h"',
        "",
        "using LegacyGetter = bool (*)( JSContext *, JSObject *, jsid, JS::Value * );",
        "using LegacySetter = bool (*)( JSContext *, JSObject *, jsid, bool, JS::Value * );",
        "",
        "static bool LegacyGet( JSContext *cx, unsigned argc, JS::Value *vp, LegacyGetter getter, const char *name )",
        "{",
        "\tauto args = JS::CallArgsFromVp( argc, vp );",
        "\tJS::RootedObject obj( cx );",
        "\tif( !args.computeThis( cx, &obj )) return false;",
        "\tJS::RootedString property( cx, JS_NewStringCopyZ( cx, name ));",
        "\tif( !property ) return false;",
        "\tJS::RootedId id( cx );",
        "\tif( !JS_StringToId( cx, property, &id )) return false;",
        "\treturn getter( cx, obj, id, args.rval().address() );",
        "}",
        "",
        "static bool LegacyGet( JSContext *cx, unsigned argc, JS::Value *vp, LegacyGetter getter, int32_t propertyId )",
        "{",
        "\tauto args = JS::CallArgsFromVp( argc, vp );",
        "\tJS::RootedObject obj( cx );",
        "\tif( !args.computeThis( cx, &obj )) return false;",
        "\treturn getter( cx, obj, JS::PropertyKey::Int( propertyId ), args.rval().address() );",
        "}",
        "",
        "static bool LegacySet( JSContext *cx, unsigned argc, JS::Value *vp, LegacySetter setter, const char *name )",
        "{",
        "\tauto args = JS::CallArgsFromVp( argc, vp );",
        "\tJS::RootedObject obj( cx );",
        "\tif( !args.computeThis( cx, &obj ) || !args.requireAtLeast( cx, name, 1 )) return false;",
        "\tJS::RootedString property( cx, JS_NewStringCopyZ( cx, name ));",
        "\tif( !property ) return false;",
        "\tJS::RootedId id( cx );",
        "\tif( !JS_StringToId( cx, property, &id )) return false;",
        "\tJS::RootedValue value( cx, args.get( 0 ));",
        "\targs.rval().setUndefined();",
        "\treturn setter( cx, obj, id, false, value.address() );",
        "}",
        "",
        "static bool LegacySet( JSContext *cx, unsigned argc, JS::Value *vp, LegacySetter setter, int32_t propertyId )",
        "{",
        "\tauto args = JS::CallArgsFromVp( argc, vp );",
        "\tJS::RootedObject obj( cx );",
        "\tif( !args.computeThis( cx, &obj ) || !args.requireAtLeast( cx, \"property setter\", 1 )) return false;",
        "\tJS::RootedValue value( cx, args.get( 0 ));",
        "\targs.rval().setUndefined();",
        "\treturn setter( cx, obj, JS::PropertyKey::Int( propertyId ), false, value.address() );",
        "}",
        "",
    ]

    seen = set()
    for has_setter, main_name, attr in properties:
        if main_name in DIRECT_FAMILIES or attr in DIRECT_PROPERTIES.get(main_name, set()):
            continue
        key = (main_name, attr)
        if key in seen:
            continue
        seen.add(key)
        family = FAMILIES[main_name]
        if main_name in enum_maps:
            candidates = enum_maps[main_name]
            canonical_attr = ALIASES.get(attr, attr)
            wanted = normalized(ENUM_PREFIXES[main_name] + canonical_attr)
            matches = [(token, value) for token, value in candidates.items() if token == wanted]
            if not matches and "color" in wanted:
                wanted = wanted.replace("color", "colour")
                matches = [(token, value) for token, value in candidates.items() if token == wanted]
            if not matches and attr.endswith("s"):
                wanted = normalized(ENUM_PREFIXES[main_name] + attr[:-1])
                matches = [(token, value) for token, value in candidates.items() if token == wanted]
            if len(matches) != 1:
                raise RuntimeError(f"Cannot map {main_name}.{attr}: {matches}")
            property_arg = str(matches[0][1])
        elif main_name == "CSkills":
            property_arg = str(skill_ids[normalized(attr)])
        else:
            property_arg = f'"{attr}"'

        lines.append(
            f"bool JS{main_name}_get_{attr}( JSContext *cx, unsigned argc, JS::Value *vp ) "
            f"{{ return LegacyGet( cx, argc, vp, {family}_getProperty, {property_arg} ); }}"
        )
        if has_setter:
            lines.append(
                f"bool JS{main_name}_set_{attr}( JSContext *cx, unsigned argc, JS::Value *vp ) "
                f"{{ return LegacySet( cx, argc, vp, {family}_setProperty, {property_arg} ); }}"
            )

    OUTPUT.write_text("\n".join(lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
