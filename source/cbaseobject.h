// o------------------------------------------------------------------------------------------------o
//|	File		-	cbaseobject.h
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Defines the property specifications for the base object class
// o------------------------------------------------------------------------------------------------o
#ifndef __CBO_H__
#define __CBO_H__
#include <bitset>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "typedefs.h"
#include "uoxstruct.h"
class CMultiObj;
class CSpawnItem;
class CChar;
class CSocket;


struct TagMap {
    enum tagmapobject_t { TAGMAP_TYPE_INT = 0, TAGMAP_TYPE_STRING, TAGMAP_TYPE_BOOL };
    
    std::uint8_t m_ObjectType;
    std::int32_t m_IntValue;
    bool m_Destroy;
    std::string m_StringValue;
} ;


enum updatetypes_t {
    UT_UPDATE = 0,
    UT_LOCATION,
    UT_HITPOINTS,
    UT_STAMINA,
    UT_MANA,
    UT_HIDE,
    UT_STATWINDOW,
    UT_COUNT
};

// o------------------------------------------------------------------------------------------------o
//|	Class		-	class CBaseObject
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	BaseObject class with common, shared properties for all object types
// o------------------------------------------------------------------------------------------------o
class CBaseObject {
public:
    enum type_t {
        OT_CBO = 0,
        OT_CHAR,
        OT_ITEM,
        OT_MULTI,
        OT_BOAT,
        OT_SPAWNER,
    };
    
protected:
    std::map<std::string, TagMap> tags;
    std::map<std::string, TagMap> tempTags;
    std::string title;
    type_t objType;
    raceid_t race;
    std::int16_t x;
    std::int16_t y;
    std::int8_t z;
    std::uint16_t id;
    std::uint16_t colour;
    std::uint8_t dir;
    serial_t serial;
    CMultiObj *multis;
    serial_t spawnSerial;
    serial_t owner;
    std::uint8_t worldNumber;
    std::uint16_t instanceId;
    std::int16_t strength;
    std::int16_t dexterity;
    std::int16_t intelligence;
    std::int16_t hitpoints;
    visibletypes_t visible;
    std::int16_t hiDamage;
    std::int16_t loDamage;
    std::int32_t weight;
    std::int16_t mana;
    std::int16_t stamina;
    std::uint16_t scriptTrig;
    std::int16_t st2;
    std::int16_t dx2;
    std::int16_t in2;
    mutable std::int32_t FilePosition;
    serial_t tempMulti;
    std::string name;
    std::string sectionId;
    std::vector<std::uint16_t> scriptTriggers;
    std::uint8_t poisoned;
    std::int16_t carve; // Carve.dfn entry
    std::int16_t oldLocX;
    std::int16_t oldLocY;
    std::int8_t oldLocZ;
    std::int16_t oldTargLocX;
    std::int16_t oldTargLocY;
    std::int16_t fame;
    std::int16_t karma;
    std::int16_t kills;
    std::uint16_t subRegion;
    
    void RemoveFromMulti(bool fireTrigger = true);
    void AddToMulti(bool fireTrigger = true);
    
    std::bitset<8> objSettings;
    
    std::uint16_t resistances[WEATHNUM];
    
    serial_t tempContainerSerial;
    
    bool nameRequestActive;
    // std::string	origin;	// Stores expansion item originates from
    std::uint8_t origin; // Stores expansion item originates from
    
    void CopyData(CBaseObject *target);
    
public:
    
    Point3 GetOldLocation();
    
    size_t GetNumTags() const;
    auto GetTagMap() const -> const std::map<std::string, TagMap>;
    auto GetTempTagMap() const -> const std::map<std::string, TagMap>;
    
    TagMap GetTag(std::string tagname) const;
    void SetTag(std::string tagname, TagMap tagval);
    
    TagMap GetTempTag(std::string tempTagName) const;
    void SetTempTag(std::string tempTagName, TagMap tagVal);
    
    void SetResist(std::uint16_t newValue, weathertype_t damage);
    std::uint16_t GetResist(weathertype_t damage) const;
    
    void setTitle(std::string newtitle);
    std::string GetTitle() const;
    
    // void					SetOrigin( std::string newOrigin );
    // std::string				GetOrigin( void ) const;
    
    void SetOrigin(std::uint8_t value);
    std::uint8_t GetOrigin() const;
    
    virtual void SetMana(std::int16_t mn);
    std::int16_t GetMana() const;
    virtual void SetStamina(std::int16_t stam);
    std::int16_t GetStamina() const;
    std::int32_t GetFilePosition() const;
    std::int32_t SetFilePosition(std::int32_t filepos);
    
    virtual ~CBaseObject();
    CBaseObject();
    
    std::int16_t GetOldTargLocX() const;
    std::int16_t GetOldTargLocY() const;
    std::int16_t GetX() const;
    std::int16_t GetY() const;
    std::int8_t GetZ() const;
    Point3 GetLocation() const;
    
    void SetOldTargLocX(std::int16_t newvalue);
    void SetOldTargLocY(std::int16_t newvalue);
    void SetX(std::int16_t newValue);
    void SetY(std::int16_t newValue);
    void SetZ(std::int8_t newValue);
    void WalkXY(std::int16_t newX, std::int16_t newY);
    virtual void SetOldLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ) = 0;
    virtual void SetLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ, std::uint8_t world, std::uint16_t instanceId) = 0;
    virtual void SetLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ) = 0;
    virtual void SetLocation(const CBaseObject *toSet) = 0;
    
    std::uint16_t GetId() const;
    std::uint16_t GetColour() const;
    std::uint8_t GetId(std::uint8_t part) const;
    std::uint8_t GetColour(std::uint8_t part) const;
    
    void SetId(std::uint16_t newValue);
    void SetColour(std::uint16_t newValue);
    void SetId(std::uint8_t newValue, std::uint8_t part);
    
    std::int32_t GetWeight() const;
    virtual void SetWeight(std::int32_t newVal, bool doWeightUpdate = true) = 0;
    
    serial_t GetSerial() const;
    serial_t GetSpawn() const;
    serial_t GetOwner() const;
    serial_t GetMulti() const;
    CMultiObj *GetMultiObj() const;
    CSpawnItem *GetSpawnObj() const;
    CChar *GetOwnerObj() const;
    
    std::uint8_t GetSerial(std::uint8_t part) const;
    std::uint8_t GetSpawn(std::uint8_t part) const;
    
    void SetMulti(serial_t newSerial, bool fireTrigger = true);
    void SetMulti(CMultiObj *newMulti, bool fireTrigger = true);
    void SetSerial(serial_t newSerial);
    void SetSpawn(serial_t newSpawn);
    virtual void SetOwner(CChar *newOwner);
    
    virtual bool Save(std::ostream &outStream) = 0;
    virtual bool DumpHeader(std::ostream &outStream) const = 0;
    virtual bool DumpBody(std::ostream &outStream) const;
    bool DumpFooter(std::ostream &outStream) const;
    bool load(std::istream &inStream);
    
    virtual bool HandleLine(std::string &UTag, std::string &data);
    
    raceid_t GetRace() const;
    void SetRace(raceid_t newValue);
    
    std::string GetNameRequest(CChar *nameRequester, std::uint8_t requestSource);
    std::string GetName() const;
    void SetName(std::string newName);
    
    std::string GetSectionId() const;
    void SetSectionId(std::string newSectionID);
    
    virtual std::int16_t GetStrength() const;
    virtual std::int16_t GetDexterity() const;
    virtual std::int16_t GetIntelligence() const;
    std::int16_t GetHP() const;
    
    virtual void SetStrength(std::int16_t newValue);
    virtual void SetDexterity(std::int16_t newValue);
    virtual void SetIntelligence(std::int16_t newValue);
    virtual void SetHP(std::int16_t newValue);
    void IncHP(std::int16_t amtToChange);
    
    void SetDir(std::uint8_t newDir, bool sendUpdate = true);
    std::uint8_t GetDir() const;
    
    void SetVisible(visibletypes_t newValue);
    visibletypes_t GetVisible() const;
    
    type_t GetObjType() const;
    virtual bool CanBeObjType(type_t toCompare) const;
    
    std::int16_t GetHiDamage() const;
    std::int16_t GetLoDamage() const;
    
    void SetHiDamage(std::int16_t newValue);
    void SetLoDamage(std::int16_t newValue);
    
    std::vector<std::uint16_t> GetScriptTriggers();
    void AddScriptTrigger(std::uint16_t newValue);
    bool HasScriptTrigger(std::uint16_t newValue);
    void RemoveScriptTrigger(std::uint16_t newValue);
    void ClearScriptTriggers();
    
    std::int16_t GetStrength2() const;
    std::int16_t GetDexterity2() const;
    std::int16_t GetIntelligence2() const;
    
    virtual void SetStrength2(std::int16_t nVal);
    virtual void SetDexterity2(std::int16_t nVal);
    virtual void SetIntelligence2(std::int16_t nVal);
    
    void IncStrength(std::int16_t toInc = 1);
    void IncDexterity(std::int16_t toInc = 1);
    void IncIntelligence(std::int16_t toInc = 1);
    
    virtual void PostLoadProcessing();
    virtual bool LoadRemnants() = 0;
    
    std::uint8_t WorldNumber() const;
    void WorldNumber(std::uint8_t value);
    
    std::uint16_t GetInstanceId() const;
    void SetInstanceId(std::uint16_t value);
    
    std::uint16_t GetSubRegion() const;
    void SetSubRegion(std::uint16_t value);
    
    std::uint8_t GetPoisoned() const;
    virtual void SetPoisoned(std::uint8_t newValue);
    
    std::int16_t GetCarve() const;
    void SetCarve(std::int16_t newValue);
    
    virtual void Update(CSocket *mSock = nullptr, bool drawGamePlayer = false,
                        bool sendToSelf = true) = 0;
    virtual void SendToSocket(CSocket *mSock, bool drawGamePlayer = false) = 0;
    virtual void Dirty(updatetypes_t updateType);
    void RemoveFromRefreshQueue();
    
    virtual void Delete() = 0;
    virtual void Cleanup();
    
    virtual void RemoveSelfFromOwner() = 0;
    virtual void AddSelfToOwner() = 0;
    
    bool IsFree() const;
    bool IsDeleted() const;
    bool IsPostLoaded() const;
    bool IsSpawned() const;
    bool ShouldSave() const;
    bool IsDisabled() const;
    bool IsWipeable() const;
    bool IsDamageable() const;
    bool NameRequestActive() const;
    
    void SetFree(bool newVal);
    void SetDeleted(bool newVal);
    void SetPostLoaded(bool newVal);
    void SetSpawned(bool newVal);
    void ShouldSave(bool newVal);
    void SetDisabled(bool newVal);
    void SetWipeable(bool newValue);
    void SetDamageable(bool newValue);
    void NameRequestActive(bool newValue);
    
    std::int16_t GetFame() const;
    void SetFame(std::int16_t value);
    void SetKarma(std::int16_t value);
    std::int16_t GetKarma() const;
    void SetKills(std::int16_t value);
    std::int16_t GetKills() const;
};

#endif
