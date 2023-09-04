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
    SERIAL stone;
    SERIAL master;
    std::vector<SERIAL> recruits;
    std::vector<SERIAL> members;
    std::map<guildid_t, GuildRelation> relationList;

    std::vector<SERIAL>::iterator recruitPtr;
    std::vector<SERIAL>::iterator memberPtr;

    std::map<guildid_t, GuildRelation>::iterator warPtr;
    std::map<guildid_t, GuildRelation>::iterator allyPtr;

  public:
    guildid_t FirstWar(void);
    guildid_t NextWar(void);
    bool FinishedWar(void);

    guildid_t FirstAlly(void);
    guildid_t NextAlly(void);
    bool FinishedAlly(void);

    CGuild();
    ~CGuild();
    const std::string Name() const;
    auto Abbreviation() const -> const std::string &;
    GuildType Type() const;
    const std::string Charter() const;
    const std::string Webpage() const;
    SERIAL Stone() const;
    SERIAL Master() const;
    SERIAL FirstRecruit();
    SERIAL NextRecruit();
    bool FinishedRecruits();
    SERIAL RecruitNumber(size_t rNum) const;
    SERIAL FirstMember();
    SERIAL NextMember();
    bool FinishedMember();
    SERIAL MemberNumber(size_t rNum) const;

    void Name(std::string txt);
    auto Abbreviation(const std::string &value) -> void;
    void Type(GuildType nType);
    void Charter(const std::string &txt);
    void Webpage(const std::string &txt);
    void Stone(SERIAL newStone);
    void Stone(CItem &newStone);
    void Master(SERIAL newMaster);
    void Master(CChar &newMaster);
    void NewRecruit(CChar &newRecruit);
    void NewRecruit(SERIAL newRecruit);
    void NewMember(CChar &newMember);
    void NewMember(SERIAL newMember);
    void RemoveRecruit(CChar &newRecruit);
    void RemoveRecruit(SERIAL newRecruit);
    void RemoveMember(CChar &newMember);
    void RemoveMember(SERIAL newMember);
    void RecruitToMember(CChar &newMember);
    void RecruitToMember(SERIAL newMember);

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

    bool IsRecruit(SERIAL toCheck) const;
    bool IsMember(SERIAL toCheck) const;
    bool IsRecruit(CChar &toCheck) const;
    bool IsMember(CChar &toCheck) const;
};

class CGuildCollection {
  private:
    std::map<guildid_t, CGuild *> gList;

    void ToggleAbbreviation(CSocket *s);
    void TransportGuildStone(CSocket *s, guildid_t guildId);
    void Erase(guildid_t toErase);
    guildid_t MaximumGuild(void);

  public:
    void Resign(CSocket *s);
    CGuildCollection() = default;
    size_t NumGuilds(void) const;
    guildid_t NewGuild(void);
    CGuild *Guild(guildid_t num) const;
    CGuild *operator[](guildid_t num);
    void Save();
    void Load();
    GuildRelation Compare(guildid_t srcGuild, guildid_t trgGuild) const;
    GuildRelation Compare(CChar *src, CChar *trg) const;
    void Menu(CSocket *s, std::int16_t menu, guildid_t trgGuild = -1, SERIAL plID = INVALIDSERIAL);
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
