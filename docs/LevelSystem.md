# UOX3 Level and Class System

The level system is optional. When enabled, it replaces normal skill gain with
level- and class-based skill values, replaces the OSI skill window with the
level progression gump, and prompts characters without a class to choose one.
Turning it off restores the normal UOX3 skill system; saved class and level data
is retained so it is available if the system is enabled again.

## Enabling the system

Set these values in `data/uox.ini`, then restart the server:

```ini
LEVELSYSTEMENABLED=1
LEVELCAP=50
LEVELBASEEXPERIENCE=1000
LEVELSTATPOINTSPERLEVEL=3
LEVELSYSTEMCRAFTING=1
MAXPRIMARYPROFESSIONS=2
ALLOWALLPROFESSIONS=0
PROFESSIONGAINRATE=1.0
```

- `LEVELCAP` is the highest attainable level.
- `LEVELBASEEXPERIENCE` controls the curve. Advancing from level `L` costs
  `LEVELBASEEXPERIENCE * L`; total XP for a level is
  `LEVELBASEEXPERIENCE * (L - 1) * L / 2`.
- `LEVELSTATPOINTSPERLEVEL` grants points that players spend on Strength,
  Dexterity, or Intelligence in the progression gump.
- `LEVELSYSTEMCRAFTING` enables profession-owned crafting and gathering skills
  while the level system is enabled. The setting name is retained for
  compatibility with existing shard configurations.
- `MAXPRIMARYPROFESSIONS` controls how many primary professions a character may
  learn. Secondary professions do not consume these slots.
- `ALLOWALLPROFESSIONS` ignores the slot limit when set to `1`.
- `PROFESSIONGAINRATE` multiplies profession experience awards. `1.0` is the
  default rate, `0.5` is half speed, and `2.0` is double speed.

All class, profession, crafting, and gathering overrides require
`LEVELSYSTEMENABLED=1`. With it set to `0`, `GetSkill` reads the original UO
skills, normal use-based skill gain runs, gathering never requires a profession,
and profession XP hooks become no-ops. This preserves the traditional UO rules
without requiring alternate gathering scripts.

## Defining classes

Classes are loaded from `data/dfndata/race/classes.dfn` by the race-definition
loader. Class IDs must begin at 1 and remain contiguous because the numeric ID
is saved on each character. Do not reorder existing classes on a live shard;
append new classes with the next ID instead.

The supplied file defines Warrior, Mage, Rogue, and Ranger. A class definition
uses this format:

```dfn
[CLASS 5]
{
NAME=Paladin
DESCRIPTION=Armored holy warrior with healing and chivalry.
HEALTHPERLEVEL=2
MANAPERLEVEL=1
STAMINAPERLEVEL=0
PRIMARYSKILLS=SWORDSMANSHIP,TACTICS,CHIVALRY
CORESKILLS=ANATOMY,HEALING,PARRYING,MAGICRESISTANCE
SUPPORTSKILLS=MEDITATION,FOCUS,ARMSLORE
PRIMARYSTART=300
PRIMARYCAP=100
CORESTART=200
CORECAP=85
SUPPORTSTART=100
SUPPORTCAP=60
OTHERSTART=0
OTHERCAP=25
ABILITY=5001
}
```

Skill names must match the names loaded by the UOX3 skill definitions.
`*START` values use UOX3 tenths, so `300` is 30.0 skill. `*CAP` values are a
percentage of `SKILLCAPSINGLE`. Between level 1 and `LEVELCAP`, each virtual
skill value grows linearly from its tier's start to its tier's cap. Skills not
listed in Primary, Core, or Support use the Other tier.

`HEALTHPERLEVEL`, `MANAPERLEVEL`, and `STAMINAPERLEVEL` add to the matching
maximum resource for every level gained after level 1.

## Class abilities

Each `ABILITY` value in a class references a `[CLASS ABILITY #]` section. The
numeric ability IDs do not need to be contiguous, but should be unique across
classes because they are also used as cooldown keys.

```dfn
[CLASS ABILITY 5001]
{
NAME=Lay on Hands
DESCRIPTION=Spend mana to recover 30 health.
LEVEL=10
ACTION=RESTOREHEALTH
VALUE=30
MANACOST=20
STAMINACOST=0
COOLDOWN=60
}
```

Players open the ability list from the progression gump. Locked abilities show
their required level; usable abilities show a button and their resource costs.
Cooldown expiration is stored persistently on the character, so reconnecting
does not reset an active cooldown.
The following actions are built in:

- `RESTOREHEALTH`, `RESTOREMANA`, and `RESTORESTAMINA` restore `VALUE` points.
- `SPELL` passes `VALUE` as a spell number through UOX3's normal player spell
  selection and casting system. Normal spell requirements still apply.
- `PASSIVE` displays an always-on ability with no Use button. Its effect should
  come from the class's normal stats or another attached script.
- `SCRIPT` calls the configured script and function for completely custom
  behavior.

A scripted ability uses these additional fields:

```dfn
[CLASS ABILITY 5002]
{
NAME=Divine Challenge
DESCRIPTION=Invoke custom Paladin behavior.
LEVEL=15
ACTION=SCRIPT
VALUE=50
SCRIPT=2515
FUNCTION=UseClassAbility
MANACOST=15
COOLDOWN=30
}
```

The function receives `(character, abilityId, value)` and must return `true`
when activation succeeds. Resource costs and cooldown are applied only after a
successful return. `MANACOST` and `STAMINACOST` are additional costs; for a
`SPELL`, the normal spell cost is still handled by the magic system.

## Crafting and gathering professions

The profession layer preserves UOX3's existing crafting and gathering systems.
Recipe
visibility, minimum requirements, success chance, resource consumption, item
rank, exceptional quality, repairs, and maker's marks continue to use the
normal recipe `SKILL` values. The difference is that a profession supplies the
virtual value for its owned skill instead of normal OSI skill gain.
Turning off `LEVELSYSTEMCRAFTING` returns those skills to the class-derived
level values. Turning off the complete level system restores the original UO
skills. Neither setting deletes saved profession progress.

Professions are loaded from `data/dfndata/race/professions.dfn`. IDs must begin
at 1 and remain contiguous. Do not reorder them on a live shard because their
numeric IDs are saved on characters.

```dfn
[PROFESSION 13]
{
NAME=Poisoncraft
DESCRIPTION=Prepare specialized toxins for custom shard recipes.
TYPE=CRAFTING
PRIMARY=1
SKILL=POISONING
STARTVALUE=0
MAXVALUE=1000
EXPERIENCEPERPOINT=100
}
```

Each profession must own a unique UOX3 skill. `STARTVALUE` and `MAXVALUE` use
tenths, so `1000` displays as 100.0. `EXPERIENCEPERPOINT` is the XP required to
gain 0.1 proficiency. `TYPE` is a display category such as `CRAFTING` or
`GATHERING`. `PRIMARY=1` consumes one primary-profession slot; `PRIMARY=0`
creates a secondary profession that may be learned even when primary slots are
full.

The supplied definitions cover Blacksmithing, Tailoring, Carpentry, Tinkering,
Bowcraft, Alchemy, Inscription, Cartography, Cooking, Mining, Lumberjacking,
and Fishing.
Tinkering and Inscription begin at 10.0 because their
lowest stock recipes require more than zero proficiency. Glassblowing continues
to use Alchemy, as it does in the existing crafting scripts. Mining and
Lumberjacking are primary gathering professions; Fishing is a secondary
profession and does not consume one of the two default primary slots.

A profession must be learned before any recipe using its skill can be crafted,
including recipes with a minimum skill of zero. Class-based supporting skills
remain active, so class and profession combinations still matter. For example,
a Warrior/Blacksmith receives the Warrior's Arms Lore proficiency when the
exceptional-item scripts calculate bonuses.

Successful crafts award progress based on the recipe's minimum/maximum skill
range. Recipes in the first, middle, and final thirds of the current difficulty
range award orange, yellow, and green progress respectively. A recipe awards no
progress once the character reaches its maximum skill requirement. This makes
players move through the existing recipe ladder instead of repeatedly crafting
a trivial item. Profession progress and selections are saved in the character
world data.

Successful Mining, Lumberjacking, and Fishing harvests use the same profession
progression. Empty nodes, failed skill checks, and failed item creation award no
XP. Shallow-water Fishing advances only to 75.0; deep-water Fishing is required
for later progression. All existing tools, resource depletion, ore selection,
special catches, yields, and regional resource regeneration remain in control
of the original gathering scripts.

## Class and level equipment rules

Class-wide item graphic restrictions use the same idea as race equipment
lists. Add references to a class and define the corresponding lists anywhere
in `classes.dfn`:

```dfn
[CLASS 2]
{
// Other Mage settings...
BANEQUIPLIST=10
}

[CLASS EQUIPLIST 10]
{
ITEMS=0x13B9,0x13BA
}
```

`BANEQUIPLIST` rejects the listed item graphic IDs. If a class has one or more
`ALLOWEQUIPLIST` entries, only graphic IDs in those allow lists may be equipped.
Allow lists take precedence over ban lists.

Individual item definitions can also use persistent integer tags:

```dfn
CUSTOMINTTAG=levelRequired 20
CUSTOMINTTAG=classRequired 1
CUSTOMINTTAG=levelClassMask 9
```

- `levelRequired` sets the minimum character level.
- `classRequired` permits one exact class ID.
- `levelClassMask` permits several of classes 1 through 31: bit 0 is class 1,
  bit 1 is class 2, and so on. The example value `9` permits classes 1 and 4.

These checks apply while the level system is enabled. GMs bypass them.

## Experience rewards and parties

Unowned, untamed NPCs award XP when killed. Pet and summon kills credit the
owning player. By default, reward XP is `max(1, NPC fame / 10)`. Override it in
an NPC definition with:

```dfn
CUSTOMINTTAG=levelXP 250
```

Set `levelXP` to `0` for NPCs that must never award XP. PvP and owned NPCs do
not award level XP.

### NPC levels and difficulty colors

While the level system is enabled, combat NPCs display their level as part of
their overhead name, for example `[Level 30] a daemon`. The complete line is
colored relative to the viewing player's level:

- Gray: at least 5 levels below the player.
- Green: 2 to 4 levels below.
- Yellow: 1 level below through 2 levels above.
- Orange: 3 or 4 levels above.
- Red: at least 5 levels above.

NPC levels default to `ceil(max(0, fame) / 300)`, clamped from 1 through the
configured player level cap. Set an exact level or hide the level on an
individual NPC with a persistent integer tag:

```dfn
CUSTOMINTTAG=npcLevel 35
CUSTOMINTTAG=levelXP 1200
```

Use `CUSTOMINTTAG=npcLevel 0` for an NPC that should not display a level.
Tamed creatures, summons, player vendors, guards, training dummies, and
invulnerable NPCs retain their normal names. With `LEVELSYSTEMENABLED=0`, NPC
levels and difficulty colors are entirely bypassed and normal notoriety hues
are used.

The bundled NPC definitions include explicit levels across the full 1-50
range. Ambient animals occupy levels 1-3, starter creatures levels 3-8,
dungeon enemies progress through the teens and twenties, dragons and major
daemons occupy the upper thirties and forties, and champion bosses are level
50. Civilian, vendor, and quest NPC definitions use level `0`. Expansion
variants carry matching tags so changing `NPCCORE` does not change a
creature's intended level.

`tools/apply_npc_levels.ps1` contains the curated tier table and validates the
complete NPC set. Run it without arguments for a dry run, or use `-Apply` to
reapply the table after adding NPC definitions. Existing NPCs need to be
respawned before newly added DFN tags are present on those instances.

Nearby, online party members in the same world and instance share the reward.
Each additional eligible member adds 10% group XP, up to a 50% bonus, before
the result is divided evenly. Only characters with a selected class can accept
XP.

## Player and GM commands

Players use `levelinfo` to open the progression gump, `abilities` to open their
class ability list directly, and `professions` to view or learn crafting
professions. Opening the normal skill window also opens the progression gump
while the level system is enabled.

GM-level commands are:

- `addxp <amount>` - target a player and grant XP.
- `setlevel <level>` - target a player and set progression to that level.
- `setclass <id|name|none>` - force a class change or clear the class.
- `resetlevel` - reset the target to level 1, zero XP, and zero unspent points.
- `resetlevel clearclass` - also clear the target's class and reopen selection.
- `setprofession <id|name> <0.0-100.0>` - learn a profession if needed and set
  the target's proficiency.
- `forgetprofession <id|name>` - remove one profession and all of its progress.
- `resetprofessions` - remove every profession and all profession progress.

Resetting progression intentionally does not subtract attributes already spent;
this avoids destructive character migration. A shard that wants full respecs
can add its own stat-reset policy to the GM command script.

## Script integration

Characters expose `level`, `experience`, `unspentStatPoints`, and the read-only
`levelClass` property. Relevant methods are `AddExperience`,
`SpendLevelStatPoint`, `SetLevelClass`, `SetLevelProgress`, and
`ResetLevelProgress`.

Scripts can enumerate definitions with `GetLevelClassCount()` and read them
with `GetLevelClassProperty(classId, propertyName)`. Supported property names
are `NAME`, `DESCRIPTION`, `PRIMARYSKILLS`, `CORESKILLS`, `SUPPORTSKILLS`,
`HEALTHPERLEVEL`, `MANAPERLEVEL`, `STAMINAPERLEVEL`, `PRIMARYCAP`, `CORECAP`,
`SUPPORTCAP`, and `OTHERCAP`.

Ability definitions are exposed through
`GetLevelClassAbilityCount(classId)` and
`GetLevelClassAbilityProperty(classId, abilityIndex, propertyName)`. Ability
indices are zero-based within the order referenced by the class. Supported
properties are `ID`, `NAME`, `DESCRIPTION`, `ACTION`, `VALUE`, `LEVEL`,
`MANACOST`, `STAMINACOST`, `COOLDOWN`, `SCRIPT`, and `FUNCTION`.

Profession definitions are exposed through `GetLevelProfessionCount()` and
`GetLevelProfessionProperty(professionId, propertyName)`. Supported properties
are `ID`, `NAME`, `DESCRIPTION`, `SKILL`, `SKILLID`, `STARTVALUE`, `MAXVALUE`,
`TYPE`/`CATEGORY`, `PRIMARY`/`COUNTSTOWARDLIMIT`, `EXPERIENCEPERPOINT`, and
`MAXEXPERIENCE`.

Character profession methods are `GetProfessionCount`, `GetProfessionId`,
`HasProfession`, `LearnProfession`, `ForgetProfession`, `ResetProfessions`,
`GetProfessionExperience`, `GetProfessionProficiency`, and
`SetProfessionProgress`. Gathering scripts can use
`CanUseProfessionSkill(skillId)` before starting an action and
`GainProfessionExperience(skillId, minimumSkill, maximumSkill)` after a
successful result. Profession slots use zero-based indices; profession
IDs begin at 1. Proficiency arguments and results use tenths.
