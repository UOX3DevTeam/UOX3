#ifndef __CGUILD__
#define __CGUILD__

#include <cstdint>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "cbaseobject.h"
#include "typedefs.h"

class CPIGumpInput;
class CScriptSection;
class CItem;

enum GuildType { GT_STANDARD = 0, GT_ORDER, GT_CHAOS, GT_UNKNOWN, GT_COUNT };

enum GuildRelation { GR_NEUTRAL = 0, GR_WAR, GR_ALLY, GR_UNKNOWN, GR_SAME, GR_COUNT };

const std::string GTypeNames[GT_COUNT] = {"Standard", "Order", "Chaos", "Unknown"};
const std::string GRelationNames[GR_COUNT] = {"Neutral", "War", "Ally", "Unknown", "Same"};
const std::int16_t BasePage = 8000;


class CGuild {
private:
    std::string name;
    std::string abbreviation;
    GuildType gType;
    std::string charter;
    std::string webpage;
    serial_t stone;
    serial_t master;
    std::vector<serial_t> recruits;
    std::vector<serial_t> members;
    std::map<guildid_t, GuildRelation> relationList;
    
    std::vector<serial_t>::iterator recruitPtr;
    std::vector<serial_t>::iterator memberPtr;
    
    std::map<guildid_t, GuildRelation>::iterator warPtr;
    std::map<guildid_t, GuildRelation>::iterator allyPtr;
    
public:
    guildid_t FirstWar();
    guildid_t NextWar();
    bool FinishedWar();
    
    guildid_t FirstAlly();
    guildid_t NextAlly();
    bool FinishedAlly();
    
    CGuild();
    ~CGuild();
    const std::string Name() const;
    auto Abbreviation() const -> const std::string &;
    GuildType Type() const;
    const std::string Charter() const;
    const std::string Webpage() const;
    serial_t Stone() const;
    serial_t Master() const;
    serial_t FirstRecruit();
    serial_t NextRecruit();
    bool FinishedRecruits();
    serial_t RecruitNumber(size_t rNum) const;
    serial_t FirstMember();
    serial_t NextMember();
    bool FinishedMember();
    serial_t MemberNumber(size_t rNum) const;
    
    void Name(std::string txt);
    auto Abbreviation(const std::string &value) -> void;
    void Type(GuildType nType);
    void Charter(const std::string &txt);
    void Webpage(const std::string &txt);
    void Stone(serial_t newStone);
    void Stone(CItem &newStone);
    void Master(serial_t newMaster);
    void Master(CChar &newMaster);
    void NewRecruit(CChar &newRecruit);
    void NewRecruit(serial_t newRecruit);
    void NewMember(CChar &newMember);
    void NewMember(serial_t newMember);
    void RemoveRecruit(CChar &newRecruit);
    void RemoveRecruit(serial_t newRecruit);
    void RemoveMember(CChar &newMember);
    void RemoveMember(serial_t newMember);
    void RecruitToMember(CChar &newMember);
    void RecruitToMember(serial_t newMember);
    
    size_t NumMembers() const;
    size_t NumRecruits() const;
    
    GuildRelation RelatedToGuild(guildid_t otherGuild) const;
    bool IsAtWar(guildid_t otherGuild) const;
    bool IsNeutral(guildid_t otherGuild) const;
    bool IsAlly(guildid_t otherGuild) const;
    bool IsAtPeace() const;
    
    void SetGuildFaction(GuildType newFaction);
    void SetGuildRelation(guildid_t otherGuild, GuildRelation toSet);
    void Save(std::ostream &toSave, guildid_t gNum);
    void Load(CScriptSection *toRead);
    
    std::map<guildid_t, GuildRelation>* GuildRelationList(); // NOTE: This is aimed ONLY at menu stuff
    
    void CalcMaster();
    
    void TellMembers(std::int32_t dictEntry, ...);
    
    const std::string TypeName();
    
    bool IsRecruit(serial_t toCheck) const;
    bool IsMember(serial_t toCheck) const;
    bool IsRecruit(CChar &toCheck) const;
    bool IsMember(CChar &toCheck) const;
};

class CGuildCollection {
private:
    std::map<guildid_t, CGuild *> gList;
    
    void ToggleAbbreviation(CSocket *s);
    void TransportGuildStone(CSocket *s, guildid_t guildId);
    void Erase(guildid_t toErase);
    guildid_t MaximumGuild();
    
public:
    void Resign(CSocket *s);
    CGuildCollection() = default;
    size_t NumGuilds() const;
    guildid_t NewGuild();
    CGuild *Guild(guildid_t num) const;
    CGuild *operator[](guildid_t num);
    void Save();
    void Load();
    GuildRelation Compare(guildid_t srcGuild, guildid_t trgGuild) const;
    GuildRelation Compare(CChar *src, CChar *trg) const;
    void Menu(CSocket *s, std::int16_t menu, guildid_t trgGuild = -1, serial_t plID = INVALIDSERIAL);
    void GumpInput(CPIGumpInput *gi);
    void GumpChoice(CSocket *s);
    void PlaceStone(CSocket *s, CItem *deed);
    bool ResultInCriminal(guildid_t srcGuild, guildid_t trgGuild) const;
    bool ResultInCriminal(CChar *src, CChar *trg) const;
    void DisplayTitle(CSocket *s, CChar *src) const;
    ~CGuildCollection();
};

extern CGuildCollection *GuildSys;

#endif
