// o------------------------------------------------------------------------------------------------o
//|	File			-	cBaseobject.cpp
//|	Date			-	7/26/2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Handles base object stuff shared between characters, items
// and multis
//|
//|	Version History	-
//|
//|							1.0		26th July, 2000
//|							Initial implementation.  Most, if not all,
//common between |							items and chars is now
//implemented here	Functions
// declared |							as const where possible
//|
//|							1.1		Unknown
//|							Significant fraction of things moved to CBO
//to support |							future functionality
//|
//|							1.2		August 27th, 2000
//|							Addition of basic script trigger stuff.
//Function documentation |							finished for all
// functions
// o------------------------------------------------------------------------------------------------o

#include "cbaseobject.h"
#include <bitset>

#include "cchar.h"
#include "citem.h"
#include "cjsmapping.h"
#include "cmultiobj.h"
#include "cscript.h"
#include "csocket.h"
#include "dictionary.h"
#include "funcdecl.h"
#include "network.h"
#include "objectfactory.h"
#include "power.h"
#include "subsystem/console.hpp"
#include "utility/strutil.hpp"
#include "weight.h"

const std::uint32_t BIT_FREE = 0;
const std::uint32_t BIT_DELETED = 1;
const std::uint32_t BIT_POSTLOADED = 2;
const std::uint32_t BIT_SPAWNED = 3;
const std::uint32_t BIT_SAVE = 4;
const std::uint32_t BIT_DISABLED = 5;
const std::uint32_t BIT_WIPEABLE = 6;
const std::uint32_t BIT_DAMAGEABLE = 7;

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject destructor
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This function is does basically what the name implies
// o------------------------------------------------------------------------------------------------o
CBaseObject::~CBaseObject() {
    if (multis != nullptr) {
        RemoveFromMulti(false);
    }
    
    // Delete all tags.
    tags.clear();
}

const auto DEFBASE_OBJTYPE = CBaseObject::OT_CBO;
const raceid_t DEFBASE_RACE = 0;
const std::int16_t DEFBASE_X = 100;
const std::int16_t DEFBASE_Y = 100;
const std::int8_t DEFBASE_Z = 0;
const std::uint16_t DEFBASE_ID = 1;
const std::uint16_t DEFBASE_COLOUR = 0;
const std::uint8_t DEFBASE_DIR = 0;
const serial_t DEFBASE_SERIAL = INVALIDSERIAL;
CMultiObj *DEFBASE_MULTIS = nullptr;
const serial_t DEFBASE_SPAWNSER = INVALIDSERIAL;
const serial_t DEFBASE_OWNER = INVALIDSERIAL;
const std::uint8_t DEFBASE_WORLD = 0;
const std::uint16_t DEFBASE_INSTANCEID = 0;
const std::uint16_t DEFBASE_SUBREGION = 0;
const std::int16_t DEFBASE_STR = 0;
const std::int16_t DEFBASE_DEX = 0;
const std::int16_t DEFBASE_INT = 0;
const std::int16_t DEFBASE_HP = 1;
const visibletypes_t DEFBASE_VISIBLE = VT_VISIBLE;
const std::int16_t DEFBASE_HIDAMAGE = 0;
const std::int16_t DEFBASE_LODAMAGE = 0;
const std::int32_t DEFBASE_WEIGHT = 0;
const std::int16_t DEFBASE_MANA = 1;
const std::int16_t DEFBASE_STAMINA = 1;
const std::uint16_t DEFBASE_SCPTRIG = 0;
const std::int16_t DEFBASE_STR2 = 0;
const std::int16_t DEFBASE_DEX2 = 0;
const std::int16_t DEFBASE_INT2 = 0;
const std::int32_t DEFBASE_FP = -1;
const std::uint8_t DEFBASE_POISONED = 0;
const std::int16_t DEFBASE_CARVE = 0;
const std::int16_t DEFBASE_KARMA = 0;
const std::int16_t DEFBASE_FAME = 0;
const std::int16_t DEFBASE_KILLS = 0;
const std::uint16_t DEFBASE_RESIST = 0;
const bool DEFBASE_NAMEREQUESTACTIVE = 0;
const expansionruleset_t DEFBASE_ORIGIN = ER_UO;

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject constructor
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This function basically does what the name implies
// o------------------------------------------------------------------------------------------------o
CBaseObject::CBaseObject(void)
: objType(DEFBASE_OBJTYPE), race(DEFBASE_RACE), x(DEFBASE_X), y(DEFBASE_Y), z(DEFBASE_Z),
id(DEFBASE_ID), colour(DEFBASE_COLOUR), dir(DEFBASE_DIR), serial(DEFBASE_SERIAL),
multis(DEFBASE_MULTIS), spawnSerial(DEFBASE_SPAWNSER), owner(DEFBASE_OWNER),
worldNumber(DEFBASE_WORLD), instanceId(DEFBASE_INSTANCEID), strength(DEFBASE_STR),
dexterity(DEFBASE_DEX), intelligence(DEFBASE_INT), hitpoints(DEFBASE_HP),
visible(DEFBASE_VISIBLE), hiDamage(DEFBASE_HIDAMAGE), loDamage(DEFBASE_LODAMAGE),
weight(DEFBASE_WEIGHT), mana(DEFBASE_MANA), stamina(DEFBASE_STAMINA),
scriptTrig(DEFBASE_SCPTRIG), st2(DEFBASE_STR2), dx2(DEFBASE_DEX2), in2(DEFBASE_INT2),
FilePosition(DEFBASE_FP), poisoned(DEFBASE_POISONED), carve(DEFBASE_CARVE), oldLocX(0),
oldLocY(0), oldLocZ(0), oldTargLocX(0), oldTargLocY(0), fame(DEFBASE_FAME),
karma(DEFBASE_KARMA), kills(DEFBASE_KILLS), subRegion(DEFBASE_SUBREGION),
nameRequestActive(DEFBASE_NAMEREQUESTACTIVE), origin(DEFBASE_ORIGIN) {
    multis = nullptr;
    tempMulti = INVALIDSERIAL;
    objSettings.reset();
    tempContainerSerial = INVALIDSERIAL;
    name.reserve(MAX_NAME);
    title.reserve(MAX_TITLE);
    // origin.reserve( MAX_ORIGIN );
    sectionId.reserve(MAX_NAME);
    if (cwmWorldState != nullptr && cwmWorldState->GetLoaded()) {
        SetPostLoaded(true);
    }
    ShouldSave(true);
    memset(&resistances[0], DEFBASE_RESIST, sizeof(std::uint16_t) * WEATHNUM);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetNumTags()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return the number of tags in an object's tag map
// o------------------------------------------------------------------------------------------------o
size_t CBaseObject::GetNumTags(void) const { return tags.size(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetTagMap()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return object's tag map
// o------------------------------------------------------------------------------------------------o
auto CBaseObject::GetTagMap() const -> const std::map<std::string, TagMap> { return tags; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetTempTagMap()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return object's temporary tag map
// o------------------------------------------------------------------------------------------------o
auto CBaseObject::GetTempTagMap() const -> const std::map<std::string, TagMap> { return tempTags; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetTag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetch custom tag with specified name from object's tag map
// o------------------------------------------------------------------------------------------------o
TagMap CBaseObject::GetTag(std::string tagname) const {
    TagMap localObject;
    localObject.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
    localObject.m_IntValue = 0;
    localObject.m_Destroy = false;
    localObject.m_StringValue = "";
    auto CI = tags.find(tagname);
    if (CI != tags.end()) {
        localObject = CI->second;
    }
    
    return localObject;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::SetTag()
//|	Date		-	Unknown / Feb 3, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Store custom string/int tag in an object's tag map
// o------------------------------------------------------------------------------------------------o
//| Changes		-	Updated the function to use the internal tagmap object instead of
// using some
//|					stored jsval in a context that may or may not change when
// reloaded.
// o------------------------------------------------------------------------------------------------o
void CBaseObject::SetTag(std::string tagname, TagMap tagval) {
    auto I = tags.find(tagname);
    if (I != tags.end()) {
        // Check to see if this object needs to be destroyed
        if (I->second.m_Destroy || tagval.m_Destroy) {
            tags.erase(I);
            if (CanBeObjType(OT_ITEM)) {
                (static_cast<CItem *>(this))->UpdateRegion();
            }
            else if (CanBeObjType(OT_CHAR)) {
                (static_cast<CChar *>(this))->UpdateRegion();
            }
            return;
        }
        // Change the tag's TagMap value. NOTE this will also change type should type be
        // changed
        else if (tagval.m_ObjectType == TagMap::TAGMAP_TYPE_STRING) {
            I->second.m_Destroy = false;
            I->second.m_ObjectType = tagval.m_ObjectType;
            I->second.m_StringValue = tagval.m_StringValue;
            // Just because it seemed like a waste to leave it unused. I put the length of the
            // string in the int member
            I->second.m_IntValue = static_cast<std::int32_t>(tagval.m_StringValue.length());
        }
        else {
            I->second.m_Destroy = false;
            I->second.m_ObjectType = tagval.m_ObjectType;
            I->second.m_StringValue = "";
            I->second.m_IntValue = tagval.m_IntValue;
        }
        
        if (CanBeObjType(OT_ITEM)) {
            (static_cast<CItem *>(this))->UpdateRegion();
        }
        else if (CanBeObjType(OT_CHAR)) {
            (static_cast<CChar *>(this))->UpdateRegion();
        }
    }
    else { // We need to create a TagMap and initialize and store into the tagmap
        if (!tagval.m_Destroy) {
            tags[tagname] = tagval;
            if (CanBeObjType(OT_ITEM)) {
                (static_cast<CItem *>(this))->UpdateRegion();
            }
            else if (CanBeObjType(OT_CHAR)) {
                (static_cast<CChar *>(this))->UpdateRegion();
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetTempTag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetch custom, temporary tag with specified name from object's
// temporary tag map
// o------------------------------------------------------------------------------------------------o
TagMap CBaseObject::GetTempTag(std::string tempTagName) const {
    TagMap localObject;
    localObject.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
    localObject.m_IntValue = 0;
    localObject.m_Destroy = false;
    localObject.m_StringValue = "";
    auto CI = tempTags.find(tempTagName);
    if (CI != tempTags.end()) {
        localObject = CI->second;
    }
    
    return localObject;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::SetTempTag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Store custom, temporary string/int tag in an object's temporary tag
// map
// o------------------------------------------------------------------------------------------------o
void CBaseObject::SetTempTag(std::string tempTagName, TagMap tagVal) {
    auto I = tempTags.find(tempTagName);
    if (I != tempTags.end()) {
        // Check to see if this object needs to be destroyed
        if (I->second.m_Destroy || tagVal.m_Destroy) {
            tempTags.erase(I);
            if (CanBeObjType(OT_ITEM)) {
                (static_cast<CItem *>(this))->UpdateRegion();
            }
            else if (CanBeObjType(OT_CHAR)) {
                (static_cast<CChar *>(this))->UpdateRegion();
            }
            return;
        }
        // Change the tag's TagMap value. NOTE this will also change type should type be
        // changed
        else if (tagVal.m_ObjectType == TagMap::TAGMAP_TYPE_STRING) {
            I->second.m_Destroy = false;
            I->second.m_ObjectType = tagVal.m_ObjectType;
            I->second.m_StringValue = tagVal.m_StringValue;
            // Just because it seemed like a waste to leave it unused. I put the length of the
            // string in the int member
            I->second.m_IntValue = static_cast<std::int32_t>(tagVal.m_StringValue.length());
        }
        else {
            I->second.m_Destroy = false;
            I->second.m_ObjectType = tagVal.m_ObjectType;
            I->second.m_StringValue = "";
            I->second.m_IntValue = tagVal.m_IntValue;
        }
        
        if (CanBeObjType(OT_ITEM)) {
            (static_cast<CItem *>(this))->UpdateRegion();
        }
        else if (CanBeObjType(OT_CHAR)) {
            (static_cast<CChar *>(this))->UpdateRegion();
        }
    }
    else { // We need to create a TagMap and initialize and store into the tagmap
        if (!tagVal.m_Destroy) {
            tempTags[tempTagName] = tagVal;
            if (CanBeObjType(OT_ITEM)) {
                (static_cast<CItem *>(this))->UpdateRegion();
            }
            else if (CanBeObjType(OT_CHAR)) {
                (static_cast<CChar *>(this))->UpdateRegion();
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetOldTargLocX()
//|					CBaseObject::SetOldTargLocX()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets old target X location for object - used in pathfinding
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetOldTargLocX(void) const { return oldTargLocX; }
void CBaseObject::SetOldTargLocX(std::int16_t newValue) { oldTargLocX = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetOldTargLocY()
//|					CBaseObject::SetOldTargLocY()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets old target Y location for object - used in pathfinding
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetOldTargLocY(void) const { return oldTargLocY; }
void CBaseObject::SetOldTargLocY(std::int16_t newValue) { oldTargLocY = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetX()
//|					CBaseObject::SetX()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets X location of object, but also stores old location
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetX(void) const { return x; }
void CBaseObject::SetX(std::int16_t newValue) {
    oldLocX = x;
    x = newValue;
    Dirty(UT_LOCATION);
}
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetY()
//|					CBaseObject::SetY()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Y location of object, but also stores old location
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetY(void) const { return y; }
void CBaseObject::SetY(std::int16_t newValue) {
    oldLocY = y;
    y = newValue;
    Dirty(UT_LOCATION);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetZ()
//|					CBaseObject::SetZ()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Z location of object, but also stores old location
// o------------------------------------------------------------------------------------------------o
std::int8_t CBaseObject::GetZ(void) const { return z; }
void CBaseObject::SetZ(std::int8_t newValue) {
    oldLocZ = z;
    z = newValue;
    Dirty(UT_LOCATION);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::WalkXY()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Update an object's X and Y location values
// o------------------------------------------------------------------------------------------------o
void CBaseObject::WalkXY(std::int16_t newX, std::int16_t newY) {
    oldLocX = x;
    oldLocY = y;
    x = newX;
    y = newY;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetResist()
//|					CBaseObject::SetResist()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets object's resistances versus different damage types
// o------------------------------------------------------------------------------------------------o
std::uint16_t CBaseObject::GetResist(weathertype_t damage) const { return resistances[damage]; }
void CBaseObject::SetResist(std::uint16_t newValue, weathertype_t damage) {
    resistances[damage] = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetId()
//|					CBaseObject::SetId()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the ID of the object
// o------------------------------------------------------------------------------------------------o
std::uint16_t CBaseObject::GetId(void) const { return id; }
void CBaseObject::SetId(std::uint16_t newValue) {
    CBaseObject *checkCont = nullptr;
    if (IsPostLoaded() && CanBeObjType(OT_ITEM)) {
        checkCont = (static_cast<CItem *>(this))->GetCont();
    }
    
    if (ValidateObject(checkCont)) {
        Weight->SubtractItemWeight(checkCont, static_cast<CItem *>(this));
    }
    
    id = newValue;
    
    if (ValidateObject(checkCont)) {
        Weight->AddItemWeight(checkCont, static_cast<CItem *>(this));
    }
    
    Dirty(UT_HIDE);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetId()
//|					CBaseObject::SetId()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets part of the ID
// o------------------------------------------------------------------------------------------------o
std::uint8_t CBaseObject::GetId(std::uint8_t part) const {
    std::uint8_t rValue = static_cast<std::uint8_t>(id >> 8);
    if (part == 2) {
        rValue = static_cast<std::uint8_t>(id % 256);
    }
    return rValue;
}
void CBaseObject::SetId(std::uint8_t newValue, std::uint8_t part) {
    if (part <= 2 && part > 0) {
        std::uint8_t parts[2];
        parts[0] = static_cast<std::uint8_t>(id >> 8);
        parts[1] = static_cast<std::uint8_t>(id % 256);
        parts[part - 1] = newValue;
        SetId(static_cast<std::uint16_t>((parts[0] << 8) + parts[1]));
    }
    else {
        Console::shared() << "Invalid part requested on SetId" << myendl;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetColour()
//|					CBaseObject::SetColour()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the colour of the object
// o------------------------------------------------------------------------------------------------o
std::uint16_t CBaseObject::GetColour(void) const { return colour; }
void CBaseObject::SetColour(std::uint16_t newValue) {
    colour = newValue;
    Dirty(UT_UPDATE);
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetColour()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns part of the colour
// o------------------------------------------------------------------------------------------------o
std::uint8_t CBaseObject::GetColour(std::uint8_t part) const {
    std::uint8_t rValue = static_cast<std::uint8_t>(colour >> 8);
    if (part == 2) {
        rValue = static_cast<std::uint8_t>(colour % 256);
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|   Function    -  CBaseObject::GetWeight()
// o------------------------------------------------------------------------------------------------o
//|   Purpose     -  Weight of the CHARACTER
// o------------------------------------------------------------------------------------------------o
std::int32_t CBaseObject::GetWeight(void) const { return weight; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetMultiObj()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the multi object that the object is inside
// o------------------------------------------------------------------------------------------------o
CMultiObj *CBaseObject::GetMultiObj(void) const { return multis; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetMulti()
//|					CBaseObject::SetMulti()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the serial of the multi the object is inside
// o------------------------------------------------------------------------------------------------o
serial_t CBaseObject::GetMulti(void) const {
    auto multiSer = INVALIDSERIAL;
    if (ValidateObject(multis)) {
        multiSer = multis->GetSerial();
    }
    
    return multiSer;
}
void CBaseObject::SetMulti(serial_t newSerial, bool fireTrigger) {
    RemoveFromMulti(fireTrigger);
    if (newSerial >= BASEITEMSERIAL) {
        CMultiObj *newMulti = CalcMultiFromSer(newSerial);
        if (ValidateObject(newMulti)) {
            multis = newMulti;
            AddToMulti(fireTrigger);
        }
        else {
            multis = nullptr;
        }
    }
}
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetSerial()
//|					CBaseObject::SetSerial()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sers serial of the object
// o------------------------------------------------------------------------------------------------o
serial_t CBaseObject::GetSerial(void) const { return serial; }
void CBaseObject::SetSerial(serial_t newSerial) {
    if (GetSerial() != INVALIDSERIAL) {
        ObjectFactory::shared().UnregisterObject(this);
    }
    serial = newSerial;
    if (newSerial != INVALIDSERIAL) {
        ObjectFactory::shared().RegisterObject(this, newSerial);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetSpawnObj()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns thing that spawned us - cannot be a character!
// o------------------------------------------------------------------------------------------------o
CSpawnItem *CBaseObject::GetSpawnObj(void) const {
    CSpawnItem *ourSpawner = static_cast<CSpawnItem *>(CalcItemObjFromSer(spawnSerial));
    if (ValidateObject(ourSpawner) && ourSpawner->GetObjType() == OT_SPAWNER) {
        return ourSpawner;
    }
    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetOwner()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns SERIAL of thing that owns us
// o------------------------------------------------------------------------------------------------o
serial_t CBaseObject::GetOwner(void) const { return owner; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetOwnerObj()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns thing that owns us
// o------------------------------------------------------------------------------------------------o
CChar *CBaseObject::GetOwnerObj(void) const { return CalcCharObjFromSer(owner); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::SetOwner()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the object's owner to newOwner
// o------------------------------------------------------------------------------------------------o
void CBaseObject::SetOwner(CChar *newOwner) {
    RemoveSelfFromOwner();
    if (ValidateObject(newOwner)) {
        owner = newOwner->GetSerial();
    }
    else {
        owner = INVALIDSERIAL;
    }
    AddSelfToOwner();
    
    if (newOwner != nullptr && CanBeObjType(OT_CHAR)) {
        CChar *thisChar = static_cast<CChar *>(this);
        std::uint8_t maxPetOwners = cwmWorldState->ServerData()->MaxPetOwners();
        if (!thisChar->IsDispellable() && maxPetOwners > 0 &&
            thisChar->GetPetOwnerList()->Num() < maxPetOwners) {
            // Add new owner to list of players who have owned character
            thisChar->AddPetOwnerToList(newOwner);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::DumpBody()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out body information of the object
//|					This is tag/data pairing information
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::DumpBody(std::ostream &outStream) const {
    std::int16_t temp_st2, temp_dx2, temp_in2;
    const char newLine = '\n';
    
    // Hexadecimal Values
    outStream << std::hex;
    outStream << "Serial=0x" << serial << newLine;
    outStream << "ID=0x" << id << newLine;
    outStream << "Colour=0x" << colour << newLine;
    outStream << "Direction=0x" << (std::int16_t)dir << newLine;
    
    if (ValidateObject(multis)) {
        outStream << "MultiID=0x";
        try {
            outStream << multis->GetSerial() << newLine;
        } catch (...) {
            outStream << "FFFFFFFF" << newLine;
            Console::shared() << "EXCEPTION: CBaseObject::DumpBody(" << name << "[" << serial
            << "]) - 'MultiID' points to invalid memory." << myendl;
        }
    }
    outStream << "SpawnerID=0x" << spawnSerial << newLine;
    outStream << "OwnerID=0x" << owner << newLine;
    
    // Decimal / String Values
    outStream << std::dec;
    
    std::string objName = name;
    if (CanBeObjType(OT_CHAR)) {
        if (objName == "#") {
            // If character name is #, use default name from dictionary files instead - using base
            // entry 3000 + character's ID
            objName = "#//" + Dictionary->GetEntry(3000 + id);
        }
    }
    
    outStream << "SectionID=" << sectionId << newLine;
    outStream << "Name=" << objName << newLine;
    std::string myLocation = "Location=" + std::to_string(x) + "," + std::to_string(y) + "," +
    std::to_string(z) + "," + std::to_string(worldNumber) + "," +
    std::to_string(instanceId) + newLine;
    outStream << myLocation;
    outStream << "Title=" << title << newLine;
    outStream << "Origin="
    << cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(origin))
    << newLine;
    
    //=========== BUG (= For Characters the dex+str+int malis get saved and get rebuilt on next
    // serverstartup = increasing malis)
    temp_st2 = st2;
    temp_dx2 = dx2;
    temp_in2 = in2;
    if (objType == OT_CHAR) {
        CChar *myChar = (CChar *)(this);
        
        // For every equipped item
        // We need to reduce Str2+Dex2+Int2
        for (CItem *myItem = myChar->FirstItem(); !myChar->FinishedItems();
             myItem = myChar->NextItem()) {
            if (ValidateObject(myItem)) {
                temp_st2 -= myItem->GetStrength2();
                temp_dx2 -= myItem->GetDexterity2();
                temp_in2 -= myItem->GetIntelligence2();
            }
        }
    }
    //=========== BUGFIX END
    outStream << "Weight=" + std::to_string(weight) + newLine;
    outStream << "Mana=" + std::to_string(mana) + newLine;
    outStream << "Stamina=" + std::to_string(stamina) + newLine;
    outStream << "Dexterity=" + std::to_string(dexterity) + "," + std::to_string(temp_dx2) +
    newLine;
    outStream << "Intelligence=" + std::to_string(intelligence) + "," + std::to_string(temp_in2) +
    newLine;
    outStream << "Strength=" + std::to_string(strength) + "," + std::to_string(temp_st2) + newLine;
    outStream << "HitPoints=" + std::to_string(hitpoints) + newLine;
    outStream << "Race=" + std::to_string(race) + newLine;
    outStream << "Visible=" + std::to_string(visible) + newLine;
    outStream << "Disabled=" << (IsDisabled() ? "1" : "0") << newLine;
    outStream << "Damage=" + std::to_string(loDamage) + "," + std::to_string(hiDamage) + newLine;
    outStream << "Poisoned=" + std::to_string(poisoned) + newLine;
    outStream << "Carve=" + std::to_string(GetCarve()) + newLine;
    outStream << "Damageable=" << (IsDamageable() ? "1" : "0") << newLine;
    
    outStream << "Defense=";
    for (std::uint8_t resist = 1; resist < WEATHNUM; ++resist) {
        outStream << GetResist(static_cast<weathertype_t>(resist)) << ",";
    }
    outStream << "[END]" << newLine;
    
    if (scriptTriggers.size() > 0) {
        for (auto scriptTrig : scriptTriggers) {
            outStream << "ScpTrig=" + std::to_string(scriptTrig) + newLine;
        }
    }
    else {
        outStream << "ScpTrig=" + std::to_string(0) + newLine;
    }
    outStream << "Reputation=" + std::to_string(GetFame()) + "," + std::to_string(GetKarma()) +
    "," + std::to_string(GetKills()) + newLine;
    
    // Spin the character tags to save make sure to dump them too
    
    for (auto CI = tags.begin(); CI != tags.end(); ++CI) {
        outStream << "TAGNAME=" << CI->first << newLine;
        if (CI->second.m_ObjectType == TagMap::TAGMAP_TYPE_STRING) {
            outStream << "TAGVALS=" << CI->second.m_StringValue << newLine;
        }
        else {
            outStream << "TAGVAL=" + std::to_string(CI->second.m_IntValue) + newLine;
        }
    }
    //====================================================================================
    // We can have exceptions, but return no errors ?
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetRace()
//|					CBaseObject::SetRace()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the race ID associaed with the object
// o------------------------------------------------------------------------------------------------o
raceid_t CBaseObject::GetRace(void) const { return race; }
void CBaseObject::SetRace(raceid_t newValue) {
    race = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetNameRequest()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the name of the object, but checks for presence of
// onNameRequest JS event
// o------------------------------------------------------------------------------------------------o
std::string CBaseObject::GetNameRequest(CChar *nameRequester, std::uint8_t requestSource) {
    std::vector<std::uint16_t> scriptTriggers = GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        cScript *toExecute = JSMapping->GetScript(scriptTrig);
        if (toExecute != nullptr) {
            std::string textFromScript =
            toExecute->OnNameRequest(this, nameRequester, requestSource);
            if (!textFromScript.empty()) {
                // If a string was returned from the event, return that as the object's name
                return textFromScript;
            }
        }
    }
    
    // Otherwise, just return the actual name of the object
    return GetName();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetName()
//|					CBaseObject::SetName()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the name of the object
// o------------------------------------------------------------------------------------------------o
std::string CBaseObject::GetName(void) const { return name; }
void CBaseObject::SetName(std::string newName) {
    name = newName.substr(0, MAX_NAME - 1);
    Dirty(UT_UPDATE);
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetSectionId()
//|					CBaseObject::SetSectionId()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets reference to Section ID/Header for object from DFNs
// o------------------------------------------------------------------------------------------------o
std::string CBaseObject::GetSectionId(void) const { return sectionId; }
void CBaseObject::SetSectionId(std::string newSectionId) {
    sectionId = newSectionId.substr(0, MAX_NAME - 1);
    Dirty(UT_UPDATE);
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetStrength()
//|					CBaseObject::SetStrength()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the strength of the object
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetStrength(void) const { return strength; }
void CBaseObject::SetStrength(std::int16_t newValue) {
    strength = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetDexterity()
//|					CBaseObject::SetDexterity()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the dexterity of the object
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetDexterity(void) const { return dexterity; }
void CBaseObject::SetDexterity(std::int16_t newValue) {
    dexterity = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetIntelligence()
//|					CBaseObject::SetIntelligence()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the intelligence of the object
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetIntelligence(void) const { return intelligence; }
void CBaseObject::SetIntelligence(std::int16_t newValue) {
    intelligence = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetHP()
//|					CBaseObject::SetHP()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the hitpoints of the object
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetHP(void) const { return hitpoints; }
void CBaseObject::SetHP(std::int16_t newValue) {
    hitpoints = newValue;
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IncHP()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the hitpoints of the object by the specified value
// o------------------------------------------------------------------------------------------------o
void CBaseObject::IncHP(std::int16_t amtToChange) { SetHP(hitpoints + amtToChange); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetDir()
//|					CBaseObject::SetDir()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the direction of the object
// o------------------------------------------------------------------------------------------------o
std::uint8_t CBaseObject::GetDir(void) const { return dir; }
void CBaseObject::SetDir(std::uint8_t newDir, bool sendUpdate) {
    dir = newDir;
    if (sendUpdate) {
        Dirty(UT_UPDATE);
    }
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetVisible()
//|					CBaseObject::SetVisible()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the visibility property of the object
//|	Notes		-	Generally it is
//|						0 = Visible
//|						1 = Temporary Hidden (Skill, Item visible to owner)
//|						2 = Invisible (Magic Invis)
//|						3 = Permanent Hidden (GM Hide)
// o------------------------------------------------------------------------------------------------o
visibletypes_t CBaseObject::GetVisible(void) const { return visible; }
void CBaseObject::SetVisible(visibletypes_t newValue) {
    visible = newValue;
    Dirty(UT_HIDE);
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetObjType()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns an CBaseObject::type_t that indicates the item's type
// o------------------------------------------------------------------------------------------------o
CBaseObject::type_t CBaseObject::GetObjType(void) const { return objType; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::CanBeObjType()
//|	Date		-	24 June, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::CanBeObjType(type_t toCompare) const {
    if (toCompare == OT_CBO) {
        return true;
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::RemoveFromMulti()
//|	Date		-	15 December, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes object from a multi, selectively firing the trigger
// o------------------------------------------------------------------------------------------------o
void CBaseObject::RemoveFromMulti(bool fireTrigger) {
    if (ValidateObject(multis)) {
        if (multis->CanBeObjType(OT_MULTI)) {
            multis->RemoveFromMulti(this);
            if (fireTrigger) {
                // First, trigger the OnLeaving event for the multi an object is removed from
                std::vector<std::uint16_t> scriptTriggers = multis->GetScriptTriggers();
                for (auto i : scriptTriggers) {
                    cScript *toExecute = JSMapping->GetScript(i);
                    if (toExecute != nullptr) {
                        // If script returns true/1, prevent other onLeaving events from triggering
                        if (toExecute->OnLeaving(multis, this) == 1) {
                            break;
                        }
                    }
                }
                // Clear scriptTriggers vector so we can re-use it below
                scriptTriggers.clear();
                
                // Then, trigger the same event for the object being removed
                scriptTriggers = GetScriptTriggers();
                for (auto i : scriptTriggers) {
                    cScript *toExecute = JSMapping->GetScript(i);
                    if (toExecute != nullptr) {
                        // If script returns true/1, prevent other onLeaving events from triggering
                        if (toExecute->OnLeaving(multis, this) == 1) {
                            break;
                        }
                    }
                }
            }
        }
        else {
            Console::shared().Error(
                                    util::format("Object of type %i with serial 0x%X has a bad multi setting of %i",
                                                 GetObjType(), serial, multis->GetSerial()));
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::AddToMulti()
//|	Date		-	15 December, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds object to multi
// o------------------------------------------------------------------------------------------------o
void CBaseObject::AddToMulti(bool fireTrigger) {
    if (CanBeObjType(OT_MULTI)) {
        multis = nullptr;
        return;
    }
    if (ValidateObject(multis)) {
        if (multis->CanBeObjType(OT_MULTI)) {
            multis->AddToMulti(this);
            if (fireTrigger) {
                // First, trigger the onEntrance script attached to the multi, if any
                std::vector<std::uint16_t> scriptTriggers = multis->GetScriptTriggers();
                for (auto i : scriptTriggers) {
                    cScript *toExecute = JSMapping->GetScript(i);
                    if (toExecute != nullptr) {
                        // If script returns true/1, prevent other onEntrance events from triggering
                        if (toExecute->OnEntrance(multis, this) == 1) {
                            break;
                        }
                    }
                }
                
                // Clear scriptTriggers vector so we can reuse it below
                scriptTriggers.clear();
                scriptTriggers.shrink_to_fit();
                
                // Then, trigger the onEntrance script attached to the object entering the multi
                scriptTriggers = GetScriptTriggers();
                for (auto i : scriptTriggers) {
                    cScript *toExecute = JSMapping->GetScript(i);
                    if (toExecute != nullptr) {
                        // If script returns true/1, prevent other onEntrance events from triggering
                        if (toExecute->OnEntrance(multis, this) == 1) {
                            break;
                        }
                    }
                }
            }
        }
        else {
            Console::shared().Error(
                                    util::format("Object of type %i with serial 0x%X has a bad multi setting of %X",
                                                 GetObjType(), serial, multis->GetSerial()));
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::SetMulti()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the object's multi to newMulti
// o------------------------------------------------------------------------------------------------o
void CBaseObject::SetMulti(CMultiObj *newMulti, bool fireTrigger) {
    RemoveFromMulti(fireTrigger);
    multis = newMulti;
    AddToMulti(fireTrigger);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetSpawn()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns part of the item's spawner serial
// o------------------------------------------------------------------------------------------------o
std::uint8_t CBaseObject::GetSpawn(std::uint8_t part) const {
    std::uint8_t rValue = 0;
    switch (part) {
        case 1:
            rValue = static_cast<std::uint8_t>(spawnSerial >> 24);
            break;
        case 2:
            rValue = static_cast<std::uint8_t>(spawnSerial >> 16);
            break;
        case 3:
            rValue = static_cast<std::uint8_t>(spawnSerial >> 8);
            break;
        case 4:
            rValue = static_cast<std::uint8_t>(spawnSerial % 256);
            break;
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetSpawn()
//|	Function	-	CBaseObject::SetSpawn()
//|	Date		-	26 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets SERIAL of thing that spawned it
// o------------------------------------------------------------------------------------------------o
serial_t CBaseObject::GetSpawn(void) const { return spawnSerial; }
void CBaseObject::SetSpawn(serial_t newSpawn) {
    CSpawnItem *ourSpawner = GetSpawnObj();
    if (ourSpawner != nullptr) {
        ourSpawner->spawnedList.Remove(this);
        ourSpawner->UpdateRegion();
    }
    spawnSerial = newSpawn;
    if (newSpawn != INVALIDSERIAL) {
        ourSpawner = GetSpawnObj();
        if (ourSpawner != nullptr) {
            ourSpawner->spawnedList.Add(this);
            ourSpawner->UpdateRegion();
        }
    }
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetSerial()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns part of a serial #
// o------------------------------------------------------------------------------------------------o
std::uint8_t CBaseObject::GetSerial(std::uint8_t part) const {
    switch (part) {
        case 1:
            return static_cast<std::uint8_t>(serial >> 24);
        case 2:
            return static_cast<std::uint8_t>(serial >> 16);
        case 3:
            return static_cast<std::uint8_t>(serial >> 8);
        case 4:
            return static_cast<std::uint8_t>(serial % 256);
    }
    return 0;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetHiDamage()
//|					CBaseObject::SetHiDamage()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's high damage value (for randomization
// purposes)
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetHiDamage(void) const { return hiDamage; }
void CBaseObject::SetHiDamage(std::int16_t newValue) {
    hiDamage = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetLoDamage()
//|					CBaseObject::SetLoDamage()
//|	Date		-	28 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's low damage value (for randomization purposes)
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetLoDamage(void) const { return loDamage; }
void CBaseObject::SetLoDamage(std::int16_t newValue) {
    loDamage = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetFilePosition()
//|					CBaseObject::SetFilePosition()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's file position
// o------------------------------------------------------------------------------------------------o
std::int32_t CBaseObject::GetFilePosition(void) const { return FilePosition; }
std::int32_t CBaseObject::SetFilePosition(std::int32_t filepos) {
    FilePosition = filepos;
    return FilePosition;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetStamina()
//|					CBaseObject::SetStamina()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's stamina
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetStamina(void) const { return stamina; }
void CBaseObject::SetStamina(std::int16_t stam) { stamina = stam; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetMana()
//|					CBaseObject::SetMana()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's mana
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetMana(void) const { return mana; }
void CBaseObject::SetMana(std::int16_t mn) { mana = mn; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetTitle()
//|					CBaseObject::SetTitle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's title
// o------------------------------------------------------------------------------------------------o
std::string CBaseObject::GetTitle(void) const { return title; }
void CBaseObject::SetTitle(std::string newtitle) { title = newtitle.substr(0, MAX_TITLE - 1); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetOrigin()
//|					CBaseObject::SetOrigin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's origin
// o------------------------------------------------------------------------------------------------o
/*std::string CBaseObject::GetOrigin( void ) const
 {
 return origin;
 }
 void CBaseObject::SetOrigin( std::string newOrigin )
 {
 origin = newOrigin.substr( 0, MAX_ORIGIN );
 }*/
auto CBaseObject::GetOrigin() const -> std::uint8_t { return origin; }
auto CBaseObject::SetOrigin(std::uint8_t setting) -> void {
    if (setting >= ER_COUNT) {
        setting = ER_COUNT - 1;
    }
    origin = setting;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetScriptTriggers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of script triggers on object
// o------------------------------------------------------------------------------------------------o
std::vector<std::uint16_t> CBaseObject::GetScriptTriggers(void) { return scriptTriggers; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::AddScriptTrigger()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a script trigger to object's list of script triggers
// o------------------------------------------------------------------------------------------------o
void CBaseObject::AddScriptTrigger(std::uint16_t newValue) {
    if (std::find(scriptTriggers.begin(), scriptTriggers.end(), newValue) == scriptTriggers.end()) {
        // Add scriptId to scriptTriggers if not already present
        scriptTriggers.push_back(newValue);
    }
    
    // Sort vector in ascending order, so order in which scripts are evaluated is predictable
    std::sort(scriptTriggers.begin(), scriptTriggers.end());
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::RemoveScriptTrigger()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a specific script trigger to object's list of script
// triggers
// o------------------------------------------------------------------------------------------------o
void CBaseObject::RemoveScriptTrigger(std::uint16_t newValue) {
    // Remove all elements containing specified script trigger from vector
    scriptTriggers.erase(std::remove(scriptTriggers.begin(), scriptTriggers.end(), newValue),
                         scriptTriggers.end());
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::HasScriptTrigger()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if object has specified script trigger in list of script
// triggers
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::HasScriptTrigger(std::uint16_t scriptTrigger) {
    if (std::find(scriptTriggers.begin(), scriptTriggers.end(), scriptTrigger) !=
        scriptTriggers.end()) {
        // ScriptTrigger found!
        return true;
    }
    
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::ClearScriptTriggers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears out all script triggers from object
// o------------------------------------------------------------------------------------------------o
void CBaseObject::ClearScriptTriggers(void) {
    scriptTriggers.clear();
    scriptTriggers.shrink_to_fit();
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetLocation()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a Point3_st structure pointing to the object's current
// location
// o------------------------------------------------------------------------------------------------o
Point3_st CBaseObject::GetLocation(void) const { return Point3_st(x, y, z); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetStrength2()
//|					CBaseObject::SetStrength2()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the second strength var associated with the object. For
// chars, it's the |					bonuses (via armour and such)
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetStrength2(void) const { return st2; }
void CBaseObject::SetStrength2(std::int16_t nVal) {
    st2 = nVal;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetDexterity2()
//|					CBaseObject::SetDexterity2()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the second dexterity var associated with the object. For
// chars, it's |					the bonuses (via armour and such)
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetDexterity2(void) const { return dx2; }
void CBaseObject::SetDexterity2(std::int16_t nVal) {
    dx2 = nVal;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetIntelligence2()
//|					CBaseObject::SetIntelligence2()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the second intelligence var associated with the object.
// For chars, |					it's the bonuses (via armour and such)
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetIntelligence2(void) const { return in2; }
void CBaseObject::SetIntelligence2(std::int16_t nVal) {
    in2 = nVal;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IncStrength()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the object's strength value
// o------------------------------------------------------------------------------------------------o
void CBaseObject::IncStrength(std::int16_t toInc) { SetStrength(strength + toInc); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IncDexterity()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the object's dexterity value
// o------------------------------------------------------------------------------------------------o
void CBaseObject::IncDexterity(std::int16_t toInc) { SetDexterity(dexterity + toInc); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IncIntelligence()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the object's intelligence value
// o------------------------------------------------------------------------------------------------o
void CBaseObject::IncIntelligence(std::int16_t toInc) { SetIntelligence(intelligence + toInc); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::DumpFooter()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out footer information so that a logical break between entries
// can |					be found without moving file pointers
//|						Mode 0 - Text
//|						Mode 1 - Binary
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::DumpFooter(std::ostream &outStream) const {
    outStream << "\no---o\n\n";
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::Load()
//|	Date		-	28 July, 2000
//|	Changes		-	(1/9/02) no longer needs mode
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads object from disk based on mode
// o------------------------------------------------------------------------------------------------o
void ReadWorldTagData(std::istream &inStream, std::string &tag, std::string &data);
bool CBaseObject::Load(std::istream &inStream) {
    std::string tag = "", data = "", UTag = "";
    while (tag != "o---o") {
        ReadWorldTagData(inStream, tag, data);
        if (tag != "o---o") {
            UTag = util::upper(tag);
            if (!HandleLine(UTag, data)) {
                Console::shared().Warning(util::format(
                                                       "Unknown world file tag %s with contents of %s", tag.c_str(), data.c_str()));
            }
        }
    }
    return LoadRemnants();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::HandleLine()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to handle world lines. Returns true if the tag is known. If
// known,
//|					internal information updated and load routine continues to
//next tag. |					Otherwise, passed up inheritance tree (if any)
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::HandleLine(std::string &UTag, std::string &data) {
    static std::string staticTagName = "";
    bool rValue = true;
    auto csecs = oldstrutil::sections(data, ",");
    
    switch ((UTag[0])) {
        case 'A':
            if (UTag == "ATT") {
                // For backwards compatibility with older UOX3 versions
                loDamage =
                static_cast<std::int16_t>(std::stoi(util::trim(util::strip(data, "//")), nullptr, 0));
                hiDamage =
                static_cast<std::int16_t>(std::stoi(util::trim(util::strip(data, "//")), nullptr, 0));
            }
            else {
                rValue = false;
            }
            break;
        case 'B':
            if (UTag == "BASEWEIGHT") {
                auto temp = static_cast<std::uint32_t>(std::stoul(data, nullptr, 0));
                (static_cast<CItem *>(this))->SetBaseWeight(temp);
            }
            else {
                rValue = false;
            }
            break;
        case 'C':
            if (UTag == "COLOUR" || UTag == "COLOR") {
                auto temp = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                colour = temp;
            }
            else if (UTag == "CARVE") {
                auto temp = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                carve = temp;
            }
            else {
                rValue = false;
            }
            break;
        case 'D':
            if (UTag == "DAMAGE") {
                if (csecs.size() >= 2) {
                    loDamage = static_cast<std::int16_t>(
                                                         std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                    hiDamage = static_cast<std::int16_t>(
                                                         std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                }
                else {
                    // If there's only one value, set both to the same
                    loDamage =
                    static_cast<std::int16_t>(std::stoi(util::trim(util::strip(data, "//")), nullptr, 0));
                    hiDamage = loDamage;
                }
            }
            else if (UTag == "DAMAGEABLE") {
                SetDamageable(util::ston<std::uint8_t>(data) == 1);
            }
            else if (UTag == "DIRECTION" || UTag == "DIR") {
                auto temp = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
                dir = temp;
            }
            else if (UTag == "DEXTERITY") {
                if (csecs.size() >= 2) {
                    dexterity = static_cast<std::int16_t>(
                                                          std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                    dx2 = static_cast<std::int16_t>(
                                                    std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                }
                else {
                    dexterity =
                    static_cast<std::int16_t>(std::stoi(util::trim(util::strip(data, "//")), nullptr, 0));
                }
            }
            else if (UTag == "DEXTERITY2") {
                dx2 = static_cast<std::int16_t>(std::stoi(util::trim(util::strip(data, "//")), nullptr, 0));
            }
            else if (UTag == "DEFENSE") {
                if (data.find(",") != std::string::npos) {
                    int count = 1;
                    for (auto &val : csecs) {
                        if (!val.empty()) {
                            auto temp = util::upper(util::trim(util::strip(val, "//")));
                            if (temp == "[END]") {
                                break;
                            }
                            auto value = static_cast<std::int16_t>(std::stoi(temp, nullptr, 0));
                            SetResist(value, static_cast<weathertype_t>(count));
                            count++;
                        }
                    }
                }
                else {
                    SetResist(
                              static_cast<std::int16_t>(std::stoi(util::trim(util::strip(data, "//")), nullptr, 0)),
                              PHYSICAL);
                }
            }
            else if (UTag == "DISABLED") {
                SetDisabled(util::ston<std::int16_t>(data) == 1);
            }
            else {
                rValue = false;
            }
            break;
        case 'F':
            if (UTag == "FAME") {
                SetFame(util::ston<std::int16_t>(data));
            }
            else {
                rValue = false;
            }
            break;
        case 'H':
            if (UTag == "HITPOINTS") {
                hitpoints = util::ston<std::int16_t>(data);
            }
            else if (UTag == "HIDAMAGE") {
                hiDamage = util::ston<std::int16_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        case 'I':
            if (UTag == "ID") {
                id = util::ston<std::int16_t>(data);
            }
            else if (UTag == "INTELLIGENCE") {
                if (data.find(",") != std::string::npos) {
                    intelligence = static_cast<std::int16_t>(
                                                             std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                    in2 = static_cast<std::int16_t>(
                                                    std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                }
                else {
                    intelligence = util::ston<std::int16_t>(data);
                }
            }
            else if (UTag == "INTELLIGENCE2") {
                in2 = util::ston<std::int16_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        case 'K':
            if (UTag == "KARMA") {
                SetKarma(util::ston<std::int16_t>(data));
            }
            else if (UTag == "KILLS") {
                SetKills(util::ston<std::int16_t>(data));
            }
            else {
                rValue = false;
            }
            break;
        case 'L':
            if (UTag == "LOCATION") {
                x = static_cast<std::int16_t>(std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                y = static_cast<std::int16_t>(std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                z = static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[2], "//")), nullptr, 0));
                worldNumber =
                static_cast<std::uint8_t>(std::stoi(util::trim(util::strip(csecs[3], "//")), nullptr, 0));
                
                // Backwards compatibility with pre-instanceId worldfiles
                if (csecs.size() >= 5) {
                    instanceId = static_cast<std::int16_t>(
                                                           std::stoi(util::trim(util::strip(csecs[4], "//")), nullptr, 0));
                }
                else {
                    instanceId = 0;
                }
            }
            else if (UTag == "LODAMAGE") {
                loDamage = util::ston<std::int16_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        case 'M':
            if (UTag == "MANA") {
                mana = util::ston<std::int16_t>(data);
            }
            else if (UTag == "MULTIID") {
                tempMulti = (util::ston<std::uint32_t>(data));
                multis = nullptr;
            }
            else {
                rValue = false;
            }
            break;
        case 'N':
            if (UTag == "NAME") {
                name = data.substr(0, MAX_NAME - 1);
            }
            else {
                rValue = false;
            }
            break;
        case 'O':
            if (UTag == "ORIGIN") {
                origin = cwmWorldState->ServerData()->EraStringToEnum(data.substr(0, MAX_ORIGIN - 1));
            }
            else if (UTag == "OWNERID") {
                owner = util::ston<std::uint32_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        case 'P':
            if (UTag == "POISONED") {
                poisoned = util::ston<std::uint8_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        case 'R':
            if (UTag == "RACE") {
                race = util::ston<std::uint16_t>(data);
            }
            else if (UTag == "REPUTATION") {
                if (csecs.size() == 3) {
                    SetFame(static_cast<std::int16_t>(
                                                      std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0)));
                    SetKarma(static_cast<std::int16_t>(
                                                       std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0)));
                    SetKills(static_cast<std::int16_t>(
                                                       std::stoi(util::trim(util::strip(csecs[2], "//")), nullptr, 0)));
                }
            }
            else {
                rValue = false;
            }
            break;
        case 'S':
            
            if (UTag == "SECTIONID") {
                sectionId = data.substr(0, MAX_NAME - 1);
            }
            else if (UTag == "STAMINA") {
                stamina = util::ston<std::int16_t>(data);
            }
            else if (UTag == "SPAWNERID") {
                spawnSerial = util::ston<std::uint32_t>(data);
            }
            else if (UTag == "SERIAL") {
                serial = util::ston<std::uint32_t>(data);
            }
            else if (UTag == "STRENGTH") {
                if (csecs.size() >= 2) {
                    strength = static_cast<std::int16_t>(
                                                         std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                    st2 = static_cast<std::int16_t>(
                                                    std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                }
                else {
                    strength = util::ston<std::int16_t>(data);
                }
            }
            else if (UTag == "STRENGTH2") {
                st2 = util::ston<std::int16_t>(data);
            }
            else if (UTag == "SCPTRIG") {
                // scriptTrig	= util::ston<std::uint16_t>(data);
                std::uint16_t scriptId = util::ston<std::uint16_t>(data);
                if (scriptId != 0 && scriptId != 65535) {
                    cScript *toExecute = JSMapping->GetScript(scriptId);
                    if (toExecute == nullptr) {
                        Console::shared().Warning(util::format(
                                                               "SCPTRIG tag found with invalid script ID (%s) while loading world data!",
                                                               data.c_str()));
                    }
                    else {
                        this->AddScriptTrigger(util::ston<std::uint16_t>(data));
                    }
                }
            }
            else {
                rValue = false;
            }
            break;
        case 'T':
            if (UTag == "TITLE") {
                title = data.substr(0, MAX_TITLE - 1);
            }
            else if (UTag == "TAGNAME") {
                staticTagName = data;
            }
            else if (UTag == "TAGVAL") {
                TagMap tagvalObject;
                tagvalObject.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
                tagvalObject.m_IntValue = std::stoi(util::trim(util::strip(data, "//")), nullptr, 0);
                tagvalObject.m_Destroy = false;
                tagvalObject.m_StringValue = "";
                SetTag(staticTagName, tagvalObject);
            }
            else if (UTag == "TAGVALS") {
                std::string localString = data;
                TagMap tagvalObject;
                tagvalObject.m_ObjectType = TagMap::TAGMAP_TYPE_STRING;
                tagvalObject.m_IntValue = static_cast<std::int32_t>(localString.size());
                tagvalObject.m_Destroy = false;
                tagvalObject.m_StringValue = localString;
                SetTag(staticTagName, tagvalObject);
            }
            else {
                rValue = false;
            }
            break;
        case 'V':
            if (UTag == "VISIBLE") {
                visible = static_cast<visibletypes_t>(std::stoul(util::trim(util::strip(data, "//"))));
            }
            else {
                rValue = false;
            }
            break;
        case 'W':
            if (UTag == "WEIGHT") {
                SetWeight(util::ston<std::int32_t>(data));
            }
            else if (UTag == "WIPE") {
                SetWipeable(util::ston<std::uint8_t>(data) == 1);
            }
            else if (UTag == "WORLDNUMBER") {
                worldNumber = util::ston<std::uint8_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        case 'X':
            if (UTag == "XYZ") {
                if (csecs.size() >= 1) {
                    x = static_cast<std::uint16_t>(
                                                   std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                }
                else {
                    x = 0;
                }
                if (csecs.size() >= 2) {
                    y = static_cast<std::uint16_t>(
                                                   std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                }
                else {
                    y = 0;
                }
                if (csecs.size() >= 3) {
                    z = static_cast<std::uint16_t>(
                                                   std::stoul(util::trim(util::strip(csecs[2], "//")), nullptr, 0));
                }
                else {
                    z = 0;
                }
            }
            else if (UTag == "X") // For backwards compatibility with older UOX3 versions
            {
                x = util::ston<std::uint16_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        case 'Y':
            if (UTag == "Y") // For backwards compatibility with older UOX3 versions
            {
                y = util::ston<std::uint16_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        case 'Z':
            if (UTag == "Z") // For backwards compatibility with older UOX3 versions
            {
                z = util::ston<std::uint16_t>(data);
            }
            else {
                rValue = false;
            }
            break;
        default:
            rValue = false;
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::PostLoadProcessing()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to setup any pointers that may need adjustment
//|					following the loading of the world
// o------------------------------------------------------------------------------------------------o
void CBaseObject::PostLoadProcessing(void) {
    auto tmpSerial = INVALIDSERIAL;
    if (multis != nullptr) {
        multis = nullptr;
        SetMulti(tempMulti, false);
    }
    else if (multis == nullptr && tempMulti != INVALIDSERIAL) {
        // Get locked down items reacquainted with multis that were loaded before them
        CItem *tempItem = static_cast<CItem *>(this);
        if (tempItem->GetMovable() == 3) {
            CMultiObj *iMulti = CalcMultiFromSer(tempMulti);
            if (ValidateObject(iMulti)) {
                iMulti->LockDownItem(static_cast<CItem *>(this));
            }
        }
    }
    if (spawnSerial != INVALIDSERIAL) {
        tmpSerial = spawnSerial;
        spawnSerial = INVALIDSERIAL;
        SetSpawn(tmpSerial);
    }
    if (owner != INVALIDSERIAL) // To repopulate the petlist of the owner
    {
        tmpSerial = owner;
        owner = INVALIDSERIAL;
        SetOwner(CalcCharObjFromSer(tmpSerial));
    }
    
    oldLocX = x;
    oldLocY = y;
    oldLocZ = z;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::WorldNumber()
//|	Date		-	26th September, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the world number that the object is in
// o------------------------------------------------------------------------------------------------o
std::uint8_t CBaseObject::WorldNumber(void) const { return worldNumber; }
void CBaseObject::WorldNumber(std::uint8_t value) {
    if (worldNumber != value && CanBeObjType(OT_CHAR)) {
        // WorldNumber has changed, check for onFacetChange JS event (characters only)
        std::vector<std::uint16_t> scriptTriggers = GetScriptTriggers();
        for (auto i : scriptTriggers) {
            cScript *tScript = JSMapping->GetScript(i);
            if (tScript != nullptr) {
                if (tScript->OnFacetChange(static_cast<CChar *>(this), worldNumber, value) == 0) {
                    // Script indicated facet change should not be allowed. Abort!
                    return;
                }
            }
        }
    }
    worldNumber = value;
    Dirty(UT_LOCATION);
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetInstanceId()
//|					CBaseObject::SetInstanceId()
//|	Date		-	24th June, 2020
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the instance ID the object is in
// o------------------------------------------------------------------------------------------------o
std::uint16_t CBaseObject::GetInstanceId(void) const { return instanceId; }
void CBaseObject::SetInstanceId(std::uint16_t value) {
    instanceId = value;
    Dirty(UT_LOCATION);
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetSubRegion()
//|					CBaseObject::SetSubRegion()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the subregion the character is in
// o------------------------------------------------------------------------------------------------o
std::uint16_t CBaseObject::GetSubRegion(void) const { return subRegion; }
void CBaseObject::SetSubRegion(std::uint16_t value) { subRegion = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetPoisoned()
//|					CBaseObject::SetPoisoned()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets an object's poisoned status
// o------------------------------------------------------------------------------------------------o
std::uint8_t CBaseObject::GetPoisoned(void) const { return poisoned; }
void CBaseObject::SetPoisoned(std::uint8_t newValue) {
    poisoned = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetCarve()
//|					CBaseObject::SetCarve()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets an object's carve ID from carve DFN
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetCarve(void) const { return carve; }
void CBaseObject::SetCarve(std::int16_t newValue) {
    carve = newValue;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IsFree()
//|					CBaseObject::SetFree()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether object is free(??)
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::IsFree(void) const { return objSettings.test(BIT_FREE); }
void CBaseObject::SetFree(bool newVal) { objSettings.set(BIT_FREE, newVal); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IsDeleted()
//|					CBaseObject::SetDeleted()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether object has been marked as deleted
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::IsDeleted(void) const { return objSettings.test(BIT_DELETED); }
void CBaseObject::SetDeleted(bool newVal) { objSettings.set(BIT_DELETED, newVal); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IsPostLoaded()
//|					CBaseObject::SetPostLoaded()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether object has finished loading
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::IsPostLoaded(void) const { return objSettings.test(BIT_POSTLOADED); }
void CBaseObject::SetPostLoaded(bool newVal) { objSettings.set(BIT_POSTLOADED, newVal); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IsSpawned()
//|					CBaseObject::SetSpawned()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether object was spawned
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::IsSpawned(void) const { return objSettings.test(BIT_SPAWNED); }
void CBaseObject::SetSpawned(bool newVal) { objSettings.set(BIT_SPAWNED, newVal); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::ShouldSave()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should save object
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::ShouldSave(void) const { return objSettings.test(BIT_SAVE); }
void CBaseObject::ShouldSave(bool newVal) { objSettings.set(BIT_SAVE, newVal); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IsDisabled()
//|					CBaseObject::SetDisabled()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the object is disabled
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::IsDisabled(void) const { return objSettings.test(BIT_DISABLED); }
void CBaseObject::SetDisabled(bool newVal) {
    objSettings.set(BIT_DISABLED, newVal);
    
    if (CanBeObjType(OT_ITEM))
        (static_cast<CItem *>(this))->UpdateRegion();
    else if (CanBeObjType(OT_CHAR))
        (static_cast<CChar *>(this))->UpdateRegion();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::Cleanup()
//|	Date		-	11/6/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Cleans up after the object
// o------------------------------------------------------------------------------------------------o
void CBaseObject::Cleanup(void) {
    SetX(7000);
    SetY(7000);
    SetZ(0);
    
    std::vector<std::uint16_t> scriptTriggers = GetScriptTriggers();
    for (auto i : scriptTriggers) {
        cScript *tScript = JSMapping->GetScript(i);
        if (tScript != nullptr) {
            tScript->OnDelete(this);
        }
    }
    
    auto toFind = cwmWorldState->refreshQueue.find(this);
    if (toFind != cwmWorldState->refreshQueue.end()) {
        cwmWorldState->refreshQueue.erase(toFind);
    }
    
    if (ValidateObject(multis)) {
        SetMulti(INVALIDSERIAL, false);
    }
    for (auto &iSock : Network->connClients) {
        if (iSock) {
            if (iSock->TempObj() != nullptr && iSock->TempObj() == this) {
                iSock->TempObj(nullptr);
            }
            if (iSock->TempObj2() != nullptr && iSock->TempObj2() == this) {
                iSock->TempObj2(nullptr);
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::Dirty()
//|	Date		-	25 July, 2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces the object onto the global refresh queue
// o------------------------------------------------------------------------------------------------o
void CBaseObject::Dirty([[maybe_unused]] updatetypes_t updateType) {
    if (IsDeleted()) {
        Console::shared().Error(util::format(
                                             "Attempt was made to add deleted item (name: %s, id: %i, serial: %i) to refreshQueue!",
                                             GetName().c_str(), GetId(), GetSerial()));
    }
    else if (IsPostLoaded()) {
        ++(cwmWorldState->refreshQueue[this]);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::RemoveFromRefreshQueue()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes the object from the global refresh queue
// o------------------------------------------------------------------------------------------------o
void CBaseObject::RemoveFromRefreshQueue() {
    auto toFind = cwmWorldState->refreshQueue.find(this);
    if (toFind != cwmWorldState->refreshQueue.end()) {
        cwmWorldState->refreshQueue.erase(toFind);
    }
}

void CBaseObject::CopyData(CBaseObject *target) {
    target->SetSectionId(GetSectionId());
    target->SetTitle(GetTitle());
    target->SetOrigin(GetOrigin());
    target->SetRace(GetRace());
    target->SetName(GetName());
    target->SetStrength(GetStrength());
    target->SetDexterity(GetDexterity());
    target->SetIntelligence(GetIntelligence());
    target->SetHP(GetHP());
    target->SetDir(GetDir());
    target->SetVisible(GetVisible());
    target->SetMana(GetMana());
    target->SetStamina(GetStamina());
    target->SetLocation(this);
    target->SetId(GetId());
    target->SetColour(GetColour());
    target->SetHiDamage(GetHiDamage());
    target->SetLoDamage(GetLoDamage());
    for (std::uint8_t resist = 0; resist < WEATHNUM; ++resist) {
        target->SetResist(GetResist(static_cast<weathertype_t>(resist)),
                          static_cast<weathertype_t>(resist));
    }
    target->SetStrength2(GetStrength2());
    target->SetDexterity2(GetDexterity2());
    target->SetIntelligence2(GetIntelligence2());
    target->SetPoisoned(GetPoisoned());
    target->SetWeight(GetWeight());
    
    target->SetKarma(karma);
    target->SetFame(fame);
    target->SetKills(kills);
    target->SetWipeable(IsWipeable());
    target->SetDamageable(IsDamageable());
}

Point3_st CBaseObject::GetOldLocation(void) { return Point3_st(oldLocX, oldLocY, oldLocZ); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetKarma()
//|					CBaseObject::SetKarma()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's karma
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetKarma(void) const { return karma; }
void CBaseObject::SetKarma(std::int16_t value) {
    karma = value;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetFame()
//|					CBaseObject::SetFame()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's fame
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetFame(void) const { return fame; }
void CBaseObject::SetFame(std::int16_t value) {
    fame = value;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetKills()
//|					CBaseObject::SetKills()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets an object's kill/murder count
// o------------------------------------------------------------------------------------------------o
std::int16_t CBaseObject::GetKills(void) const { return kills; }
void CBaseObject::SetKills(std::int16_t value) {
    kills = value;
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IsWipeable( )
//|					CBaseObject::SetWipeable()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether an item is affected by wipe command or not
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::IsWipeable(void) const { return objSettings.test(BIT_WIPEABLE); }
void CBaseObject::SetWipeable(bool newValue) { objSettings.set(BIT_WIPEABLE, newValue); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::IsDamageable()
//|					CBaseObject::SetDamageable()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's damageable state
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::IsDamageable(void) const { return objSettings.test(BIT_DAMAGEABLE); }
void CBaseObject::SetDamageable(bool newValue) {
    objSettings.set(BIT_DAMAGEABLE, newValue);
    
    if (CanBeObjType(OT_ITEM)) {
        (static_cast<CItem *>(this))->UpdateRegion();
    }
    else if (CanBeObjType(OT_CHAR)) {
        (static_cast<CChar *>(this))->UpdateRegion();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::NameRequestActive()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a name request is in process for an item (to
// prevent infinite loop)
// o------------------------------------------------------------------------------------------------o
bool CBaseObject::NameRequestActive(void) const { return nameRequestActive; }
void CBaseObject::NameRequestActive(bool newValue) { nameRequestActive = newValue; }
