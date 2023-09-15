//

#ifndef serverswitch_hpp
#define serverswitch_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <optional>
#include <ostream>
#include <vector>

//======================================================================
struct ServerSwitch{
    enum switch_t {
        UOG=0,                  FREESHARD,                  CONSOLELOG,                 NETWORKLOG,
        SPEECHLOG,              ANNOUNCESAVE,               ANNOUNCEJOINPART,           BACKUP,
        SHOOTONANIMALBACK,      NPCTRAINING,                CORPSELOOTDECAY,            GUARDSACTIVE,
        DEATHANIMATION,         AMBIENTFOOTSTEPS,           AUTOACCOUNT,                SHOWOFFLINEPCS,
        ROGUEENABLE,            SNOOPISCRIME,               PLAYERPERSECUTION,          SELLBYNAME,
        TRADESYSTEM,            RANKSYSTEM,                 CUTSCROLLREQ,               DISPLAYHITMSG,
        ESCORTS,                MONSTERSVSANIMALS,          PETHUNGEROFFLINE,           HIDEWHILEMOUNTED,
        OVERLOADPACKETS,        ARMORIMPACTSMANA,           ANIMALSGUARDED,             ADVANCEDPATHFINDING,
        LOOTINGISCRIME,         BASICTOOLTIPSONLY,          ITEMDECAY,                  SCRIPTITEMSDECAYABLE,
        BASEITEMSDECAYABLE,     INHOUSEDECAY,               DISPLAYGUILDBUTTON,         ATTACKSPEEDFROMSTAMINA,
        DISPLAYDAMAGENUMBERS,   EXTENDEDSTATS,              EXTENDEDSKILLS,             ASSISTANTNEGOTIATION,
        KICKONASSISTANTSILENCE, CUOMAPTRACKER,              PROTECTPRIVATEHOUSES,       TRACKHOUSESPERACCOUNT,
        OWNCOOWNHOUSE,          COWNHOUSEACCOUNT,           ITEMDETECTSPEECH,           FORECENEWANIMATIONPACKET,
        MAPDIFF,                ARMORCLASSBONUS,            ALCHEMYBONUS,               PETTHIRSTOFFLINE,
        UNICODEMESSAGE,         HUNGER,                     THIRST,                     TRAVELBOATKEY,
        TRAVELBURDEN,           RUNEMULTI,                  SPELLWORLDTRAVEL,           TRAVELAGRESSOR,
        CONTEXTMENU,            PETDIFFICULTY,              NPCTOOLTIPS,                INTERRUPTCASTING,
        STATIMPACTSKILL,        TOOLUSE,                    TOOLBREAK,                  REPAIRLOSS,
        MAGICSTATS,             COLORWEAPON,                SAFETELEPORT,               AWAKENPC,
        MAKERMARK,              OVERHEADTITLE,              DISPLAYINVUNERABLE,         PETCOMBATTRAINING,
        HIRELINGTRAINING,       NPCCOMBAT,                  DISPLAYRESISTSTATS,         DISPLAYDAMAGETYPE,
        DISPLAYRACE,            DISPLAYRACEPAPERDOLL,       SPELLMOVING,                DISPLAYREPUTATIONTOOLTIP,
        DISPLAYGUILDTOOLTIP,    COOWNERLOGOUT,              FRIENDLOGOUT,               GUESTLOGOUT,
        KEYLESSGUEST,           KEYLESSOWNER,               KEYLESSCOOWNER,             KEYLESSFRIEND,
        OFFERBODSFROMITEMSALES, OFFERBODSFROMCONTEXTMENU,   BODSFROMCRAFTEDITEMSONLY,   GUILDDISCOUNT,
        GUILDPREMIUM,           SNOOPAWARE,                 YOUNGPLAYER,                RANDOMSTART,
        HTMLSTAT
    };
    static const std::map<std::string, switch_t> NAMESWITCHMAP ;
    std::vector<bool> enabledSwitch ;
    
    ServerSwitch() ;
    auto reset() ->void ;
    auto size() const ->size_t ;
    auto operator[](switch_t setting) const ->bool ;
    auto setSetting(switch_t setting, bool state) ->void ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto save(std::ostream &output) const ->void;
    auto valueFor(const std::string &keyword) const ->std::optional<bool> ;
};

#endif /* serverswitch_hpp */
