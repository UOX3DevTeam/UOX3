#ifndef __CPACKETSEND_H__
#define __CPACKETSEND_H__

#include <cstdint>
#include <string>

#include "typedefs.h"

#include "cpacketreceive.h"

class CSpeechEntry;
class PhysicalServer;
class CChar;
class CItem;
struct AccountEntry;
class CBaseObject;
class CMultiObj;
class CTownRegion;

// Forward declare
struct __STARTLOCATIONDATA__;

class CPCharLocBody : public CPUOXBuffer {
  protected:
    virtual void CopyData(CChar &toCopy);
    virtual void InternalReset() override;

  public:
    CPCharLocBody();
    CPCharLocBody(CChar &toCopy);
    virtual ~CPCharLocBody() {}
    virtual void Flag(std::uint8_t toPut);
    virtual void HighlightColour(std::uint8_t color);
    CPCharLocBody &operator=(CChar &toCopy);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPacketSpeech : public CPUOXBuffer {
  protected:
    bool isUnicode;

    virtual void InternalReset() override;
    void CopyData(CSpeechEntry &toCopy);
    void CopyData(CPITalkRequest &toCopy);

  public:
    CPacketSpeech();
    CPacketSpeech(CSpeechEntry &toCopy);
    CPacketSpeech(CPITalkRequest &toCopy);
    virtual void SpeakerSerial(serial_t toPut);
    virtual void SpeakerModel(std::uint16_t toPut);
    virtual void Colour(colour_t toPut);
    virtual void Font(fonttype_t toPut);
    virtual void Language(unicodetypes_t toPut);
    virtual void Unicode(bool toPut);
    virtual void Type(speechtype_t toPut);
    virtual void Speech(const std::string &toPut);
    virtual void SpeakerName(const std::string &toPut);
    void GhostIt(std::uint8_t method);
    virtual ~CPacketSpeech() {}
    CPacketSpeech &operator=(CSpeechEntry &toCopy);
};

class CPWalkDeny : public CPUOXBuffer {
  public:
    CPWalkDeny();
    virtual ~CPWalkDeny() {}
    virtual void SequenceNumber(char newValue);
    virtual void X(std::int16_t newValue);
    virtual void Y(std::int16_t newValue);
    virtual void Z(std::int8_t newValue);
    virtual void Direction(char newValue);
};

class CPWalkOK : public CPUOXBuffer {
  public:
    CPWalkOK();
    virtual ~CPWalkOK() {}
    virtual void SequenceNumber(char newValue);
    virtual void FlagColour(std::uint8_t newValue);
};

class CPExtMove : public CPUOXBuffer {
  protected:
    void CopyData(CChar &toCopy);

  public:
    CPExtMove();
    virtual ~CPExtMove() {}
    CPExtMove(CChar &toCopy);
    virtual void FlagColour(std::uint8_t newValue);
    virtual void SetFlags(CChar &totoCopy);
    CPExtMove &operator=(CChar &toCopy);
};

class CPAttackOK : public CPUOXBuffer {
  protected:
    void CopyData(CChar &toCopy);

  public:
    CPAttackOK();
    virtual ~CPAttackOK() {}
    CPAttackOK(CChar &toCopy);
    virtual void Serial(serial_t newSerial);
    CPAttackOK &operator=(CChar &toCopy);
};

class CPRemoveItem : public CPUOXBuffer {
  protected:
    void CopyData(CBaseObject &toCopy);

  public:
    CPRemoveItem();
    virtual ~CPRemoveItem() {}
    CPRemoveItem(CBaseObject &toCopy);
    virtual void Serial(serial_t newSerial);
    CPRemoveItem &operator=(CBaseObject &toCopy);
};

class CPWorldChange : public CPUOXBuffer {
  public:
    CPWorldChange();
    virtual ~CPWorldChange() {}
    CPWorldChange(worldtype_t newSeason, std::uint8_t newCursor);
    virtual void Season(worldtype_t newSeason);
    virtual void Cursor(std::uint8_t newCursor);
};

class CPLightLevel : public CPUOXBuffer {
  public:
    CPLightLevel();
    virtual ~CPLightLevel() {}
    CPLightLevel(lightlevel_t level);
    virtual void Level(lightlevel_t level);
};

class CPUpdIndSkill : public CPUOXBuffer {
  protected:
    virtual void CopyData(CChar &i, std::uint8_t sNum);
    virtual void InternalReset() override;

  public:
    CPUpdIndSkill();
    virtual ~CPUpdIndSkill() {}
    CPUpdIndSkill(CChar &i, std::uint8_t sNum);
    virtual void Character(CChar &i, std::uint8_t sNum);
    virtual void SkillNum(std::uint8_t sNum);
    virtual void Skill(std::int16_t skillval);
    virtual void BaseSkill(std::int16_t skillval);
    virtual void Lock(skilllock_t lockVal);
    virtual void Cap(std::int16_t capVal);
};

class CPBuyItem : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CBaseObject &i);

  public:
    CPBuyItem();
    virtual ~CPBuyItem() {}
    CPBuyItem(CBaseObject &i);
    CPBuyItem &operator=(CBaseObject &toCopy);
    void Serial(serial_t toSet);
};

class CPRelay : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPRelay();
    virtual ~CPRelay() {}
    CPRelay(std::uint32_t newIP);
    CPRelay(std::uint32_t newIP, std::uint16_t newPort);
    virtual void ServerIP(std::uint32_t newIP);
    virtual void Port(std::uint16_t newPort);
    virtual void SeedIP(std::uint32_t newIP);
};

class CPWornItem : public CPUOXBuffer {
  protected:
    virtual void CopyData(CItem &toCopy);

  public:
    CPWornItem();
    virtual ~CPWornItem() {}
    CPWornItem(CItem &toCopy);
    virtual void ItemSerial(serial_t itemSer);
    virtual void Model(std::int16_t newModel);
    virtual void Layer(std::uint8_t layer);
    virtual void CharSerial(serial_t chSer);
    virtual void Colour(std::int16_t newColour);
    CPWornItem &operator=(CItem &toCopy);
};

class CPCharacterAnimation : public CPUOXBuffer {
  protected:
    virtual void CopyData(CChar &toCopy);
    virtual void InternalReset() override;

  public:
    CPCharacterAnimation();
    virtual ~CPCharacterAnimation() {}
    CPCharacterAnimation(CChar &toCopy);
    virtual void Serial(serial_t toSet);
    virtual void Action(std::uint16_t model);
    virtual void FrameCount(std::uint8_t frameCount);
    virtual void Repeat(std::int16_t repeatValue);
    virtual void DoBackwards(bool newValue);
    virtual void RepeatFlag(bool newValue);
    virtual void FrameDelay(std::uint8_t delay);
    CPCharacterAnimation &operator=(CChar &toCopy);
};

class CPNewCharacterAnimation : public CPUOXBuffer {
  protected:
    virtual void CopyData(CChar &toCopy);
    virtual void InternalReset() override;

  public:
    CPNewCharacterAnimation();
    virtual ~CPNewCharacterAnimation() {}
    CPNewCharacterAnimation(CChar &toCopy);
    virtual void Serial(serial_t toSet);
    virtual void Action(std::uint16_t action);
    virtual void SubAction(std::uint16_t subAction);
    virtual void SubSubAction(std::uint8_t subSubAction);
    CPNewCharacterAnimation &operator=(CChar &toCopy);
};

class CPDrawGamePlayer : public CPUOXBuffer {
  protected:
    virtual void CopyData(CChar &toCopy);
    virtual void InternalReset() override;

  public:
    CPDrawGamePlayer();
    virtual ~CPDrawGamePlayer() {}
    CPDrawGamePlayer(CChar &toCopy);
};

class CPPersonalLightLevel : public CPUOXBuffer {
  protected:
    virtual void CopyData(CChar &toCopy);
    virtual void InternalReset() override;

  public:
    CPPersonalLightLevel();
    virtual ~CPPersonalLightLevel() {}
    CPPersonalLightLevel(CChar &toCopy);
    virtual void Serial(serial_t toSet);
    virtual void Level(std::uint8_t lightLevel);
    CPPersonalLightLevel &operator=(CChar &toCopy);
};

class CPPlaySoundEffect : public CPUOXBuffer {
  protected:
    virtual void CopyData(CBaseObject &toCopy);
    virtual void InternalReset() override;

  public:
    CPPlaySoundEffect();
    virtual ~CPPlaySoundEffect() {}
    CPPlaySoundEffect(CBaseObject &toCopy);
    virtual void Mode(std::uint8_t mode);
    virtual void Model(std::uint16_t newModel);
    virtual void X(std::int16_t xLoc);
    virtual void Y(std::int16_t yLoc);
    virtual void Z(std::int16_t zLoc);
    CPPlaySoundEffect &operator=(CBaseObject &toCopy);
};

class CPPaperdoll : public CPUOXBuffer {
  protected:
    virtual void CopyData(CChar &toCopy);
    virtual void InternalReset() override;

  public:
    CPPaperdoll();
    virtual ~CPPaperdoll() {}
    CPPaperdoll(CChar &toCopy);
    virtual void Serial(serial_t tSerial);
    virtual void FlagByte(std::uint8_t fVal);
    virtual void Text(const std::string &toPut);
    CPPaperdoll &operator=(CChar &toCopy);
};

class CPWeather : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPWeather();
    virtual ~CPWeather() {}
    CPWeather(std::uint8_t nType);
    CPWeather(std::uint8_t nType, std::uint8_t nParts);
    CPWeather(std::uint8_t nType, std::uint8_t nParts, std::uint8_t nTemp);
    virtual void Type(std::uint8_t nType);
    virtual void Particles(std::uint8_t nParts);
    virtual void Temperature(std::uint8_t nTemp);
};

class CPGraphicalEffect : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPGraphicalEffect() {}
    CPGraphicalEffect(std::uint8_t effectType);
    CPGraphicalEffect(std::uint8_t effectType, CBaseObject &src, CBaseObject &trg);
    CPGraphicalEffect(std::uint8_t effectType, CBaseObject &src);
    virtual void Effect(std::uint8_t effectType);
    virtual void SourceSerial(CBaseObject &toSet);
    virtual void SourceSerial(serial_t toSet);
    virtual void TargetSerial(CBaseObject &toSet);
    virtual void TargetSerial(serial_t toSet);
    virtual void Model(std::int16_t nModel);
    virtual void X(std::int16_t nX);
    virtual void Y(std::int16_t nY);
    virtual void Z(std::int8_t nZ);
    virtual void XTrg(std::int16_t nX);
    virtual void YTrg(std::int16_t nY);
    virtual void ZTrg(std::int8_t nZ);
    virtual void Speed(std::uint8_t nSpeed);
    virtual void Duration(std::uint8_t nDuration);
    virtual void AdjustDir(bool nValue);
    virtual void ExplodeOnImpact(bool nValue);
    virtual void SourceLocation(CBaseObject &toSet);
    virtual void SourceLocation(std::int16_t x, std::int16_t y, std::int8_t z);
    virtual void TargetLocation(CBaseObject &toSet);
    virtual void TargetLocation(std::int16_t x, std::int16_t y, std::int8_t z);
};

class CPUpdateStat : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPUpdateStat() {}
    CPUpdateStat(CBaseObject &toUpdate, std::uint8_t statNum, bool normalizeStats);
    virtual void Serial(serial_t toSet);
    virtual void MaxVal(std::int16_t maxVal);
    virtual void CurVal(std::int16_t curVal);
};

class CPDeathAction : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPDeathAction(CChar &dying, CItem &corpse);
    CPDeathAction();
    virtual ~CPDeathAction() {}
    virtual void Player(serial_t toSet);
    virtual void Corpse(serial_t toSet);
    virtual void FallDirection(std::uint8_t toFall);
    CPDeathAction &operator=(CChar &dying);
    CPDeathAction &operator=(CItem &corpse);
};

class CPPlayMusic : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPPlayMusic(std::int16_t musicId);
    CPPlayMusic();
    virtual ~CPPlayMusic() {}
    virtual void MusicId(std::int16_t musicId);
};

class CPDrawContainer : public CPUOXBuffer {
  protected:
    virtual void CopyData(CItem &toCopy);
    virtual void InternalReset() override;

  public:
    CPDrawContainer();
    virtual ~CPDrawContainer() {}
    CPDrawContainer(CItem &toCopy);
    virtual void Model(std::uint16_t newModel);
    virtual void ContType(std::uint16_t contType);
    virtual void Serial(serial_t toSet);
    CPDrawContainer &operator=(CItem &toCopy);
};

//	0x7C Packet
//	Last Modified on Thursday, 03-Sep-2002
//	Open Dialog Box (Variable # of bytes)
//	BYTE cmd
//	BYTE[2] blockSize
//	BYTE[4] dialogId (echo'd back to the server in 7d)
//	BYTE[2] menuid (echo'd back to server in 7d)
//	BYTE length of question
//	BYTE[length of question] question text
//	BYTE # of responses
//	Then for each response:
//		BYTE[2] model id # of shown item (if grey menu -- then always 0x00 as msb)
//		BYTE[2] color of shown item
//		BYTE response text length
//		BYTE[response text length] response text

class CPOpenGump : public CPUOXBuffer {
  protected:
    virtual void CopyData(CChar &toCopy);
    virtual void InternalReset() override;
    size_t responseOffset;
    size_t responseBaseOffset;

  public:
    CPOpenGump();
    virtual ~CPOpenGump() {}
    CPOpenGump(CChar &toCopy);
    virtual void Length(std::int32_t TotalLines);
    virtual void GumpIndex(std::int32_t index);
    virtual void Serial(serial_t toSet);
    virtual void Question(std::string toAdd);
    virtual void AddResponse(std::uint16_t modelNum, std::uint16_t colour, std::string responseText);
    virtual void Finalize();
    CPOpenGump &operator=(CChar &toCopy);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPTargetCursor : public CPUOXBuffer {
  public:
    CPTargetCursor();
    virtual ~CPTargetCursor() {}
    virtual void Type(std::uint8_t nType);
    virtual void ID(serial_t toSet);
    virtual void CursorType(
        std::uint8_t nType); // There is more to this packet, but it's only valid when sent by the CLIENT
};

class CPStatWindow : public CPUOXBuffer {
  protected:
    bool extended3;
    bool extended4;
    bool extended5;
    bool extended6;
    size_t byteOffset;
    virtual void InternalReset() override;

  public:
    CPStatWindow();
    virtual ~CPStatWindow() {}
    CPStatWindow(CBaseObject &toCopy, CSocket &target);
    virtual void Serial(serial_t toSet);
    virtual void Name(const std::string &nName);
    virtual void CurrentHP(std::int16_t nValue);
    virtual void MaxHP(std::int16_t nValue);
    virtual void MaxWeight(std::uint16_t value);
    virtual void Race(std::uint8_t value);
    virtual void NameChange(bool nValue);
    virtual void Flag(std::uint8_t nValue);
    virtual void Sex(std::uint8_t nValue);
    virtual void Strength(std::int16_t nValue);
    virtual void Dexterity(std::int16_t nValue);
    virtual void Intelligence(std::int16_t nValue);
    virtual void Stamina(std::int16_t nValue);
    virtual void MaxStamina(std::int16_t nValue);
    virtual void Mana(std::int16_t nValue);
    virtual void MaxMana(std::int16_t nValue);
    virtual void Gold(std::uint32_t gValue);
    virtual void AC(std::uint16_t nValue);
    virtual void Weight(std::uint16_t nValue);
    virtual void SetCharacter(CChar &toCopy, CSocket &target);
    virtual void StatCap(std::uint16_t value);
    virtual void CurrentPets(std::uint8_t value);
    virtual void MaxPets(std::uint8_t value);
    virtual void FireResist(std::uint16_t value);
    virtual void ColdResist(std::uint16_t value);
    virtual void PoisonResist(std::uint16_t value);
    virtual void EnergyResist(std::uint16_t value);
    virtual void Luck(std::uint16_t value);
    virtual void DamageMax(std::uint16_t value);
    virtual void DamageMin(std::uint16_t value);
    virtual void TithingPoints(std::uint32_t value);
};

class CPIdleWarning : public CPUOXBuffer {
    /*
     0x53 Client Message:

     0x01=Char doesn't exist
     0x02=Char Already Exists
     0x03,0x04=Cannot Attach to server
     0x05=Char in world Msg
     0x06=Login server Syc Error
     0x07=Idle msg
     0x08 (and up?)=Can't Attach
     */
  protected:
    virtual void InternalReset() override;

  public:
    CPIdleWarning();
    virtual ~CPIdleWarning() {}
    CPIdleWarning(std::uint8_t errorNum);
    virtual void Error(std::uint8_t errorNum);
};

class CPTime : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPTime();
    virtual ~CPTime() {}
    CPTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t second);
    virtual void Hour(std::uint8_t hour);
    virtual void Minute(std::uint8_t minute);
    virtual void Second(std::uint8_t second);
};

class CPLoginComplete : public CPUOXBuffer {
  public:
    CPLoginComplete();
};

class CPTextEmoteColour : public CPUOXBuffer {
  public:
    virtual ~CPTextEmoteColour() {}
    CPTextEmoteColour();
    virtual void BlockSize(std::int16_t newValue);
    virtual void Unknown(std::int16_t newValue);
};

class CPWarMode : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPWarMode() {}
    CPWarMode();
    CPWarMode(std::uint8_t nFlag);
    virtual void Flag(std::uint8_t nFlag);
};

class CPPauseResume : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPPauseResume() {}
    CPPauseResume();
    CPPauseResume(std::uint8_t mode);
    virtual void Mode(std::uint8_t mode);
    virtual bool ClientCanReceive(CSocket *mSock) override;
};

class CPWebLaunch : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void SetSize(std::int16_t newSize);

  public:
    virtual ~CPWebLaunch() {}
    CPWebLaunch();
    CPWebLaunch(const std::string &txt);
    virtual void Text(const std::string &txt);
};

class CPTrackingArrow : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPTrackingArrow() {}
    CPTrackingArrow();
    CPTrackingArrow(std::int16_t x, std::int16_t y);
    CPTrackingArrow(CBaseObject &toCopy);
    virtual void Location(std::int16_t x, std::int16_t y);
    virtual void Active(std::uint8_t value);
    virtual void AddSerial(serial_t targetSerial);
    CPTrackingArrow &operator=(CBaseObject &toCopy);
};

class CPBounce : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPBounce() {}
    CPBounce();
    CPBounce(std::uint8_t mode);
    virtual void Mode(std::uint8_t mode);
};

class CPDyeVat : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CBaseObject &target);

  public:
    virtual ~CPDyeVat() {}
    CPDyeVat();
    CPDyeVat(CBaseObject &target);
    virtual void Serial(serial_t toSet);
    virtual void Model(std::int16_t toSet);
    CPDyeVat &operator=(CBaseObject &target);
};

class CPMultiPlacementView : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CItem &target);

  public:
    virtual ~CPMultiPlacementView() {}
    CPMultiPlacementView();
    CPMultiPlacementView(CItem &target);
    CPMultiPlacementView(serial_t toSet);
    virtual void RequestType(std::uint8_t rType);
    virtual void DeedSerial(serial_t toSet);
    virtual void MultiModel(std::int16_t toSet);
    virtual void SetHue(std::uint16_t hueValue);
    CPMultiPlacementView &operator=(CItem &target);
};

class CPEnableClientFeatures : public CPUOXBuffer {
  public:
    CPEnableClientFeatures(CSocket *mSock);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPNegotiateAssistantFeatures : public CPUOXBuffer {
  public:
    CPNegotiateAssistantFeatures(CSocket *mSock);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPKrriosClientSpecial : public CPUOXBuffer {
  public:
    CPKrriosClientSpecial(CSocket *mSock, CChar *mChar, std::uint8_t type, bool locations);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPLogoutResponse : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPLogoutResponse();
    CPLogoutResponse(std::uint8_t extraByte);
};

class CPAddItemToCont : public CPUOXBuffer {
  protected:
    bool uokrFlag;
    virtual void InternalReset() override;
    virtual void CopyData(CItem &toCopy);

  public:
    virtual ~CPAddItemToCont() {}
    CPAddItemToCont();
    CPAddItemToCont(CItem &toAdd);
    virtual void Serial(serial_t toSet);
    virtual void Model(std::int16_t toSet);
    virtual void NumItems(std::int16_t toSet);
    virtual void X(std::int16_t x);
    virtual void Y(std::int16_t y);
    virtual void GridLocation(std::int8_t gridLoc);
    virtual void Container(serial_t toAdd);
    virtual void Colour(std::int16_t toSet);
    void UOKRFlag(bool newVal);
    void Object(CItem &toAdd);
};

class CPKickPlayer : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CChar &toCopy);

  public:
    virtual ~CPKickPlayer() {}
    CPKickPlayer();
    CPKickPlayer(CChar &toCopy);
    virtual void Serial(serial_t toSet);
    CPKickPlayer &operator=(CChar &toCopy);
};

class CPResurrectMenu : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPResurrectMenu() {}
    CPResurrectMenu();
    CPResurrectMenu(std::uint8_t action);
    virtual void Action(std::uint8_t action);
};

class CPFightOccurring : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPFightOccurring() {}
    CPFightOccurring();
    CPFightOccurring(CChar &attacker, CChar &defender);
    virtual void Attacker(serial_t toSet);
    virtual void Attacker(CChar &attacker);
    virtual void Defender(serial_t toSet);
    virtual void Defender(CChar &defender);
};

class CPSkillsValues : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CChar &toCopy);
    virtual void BlockSize(std::int16_t newValue);
    virtual std::uint8_t NumSkills();

  public:
    virtual ~CPSkillsValues() {}
    CPSkillsValues();
    CPSkillsValues(CChar &toCopy);
    virtual void NumSkills(std::uint8_t numSkills);
    virtual void SetCharacter(CChar &toCopy);
    virtual void SkillEntry(std::int16_t skillId, std::int16_t skillVal, std::int16_t baseSkillVal, skilllock_t skillLock);
    CPSkillsValues &operator=(CChar &toCopy);
};

class CPMapMessage : public CPUOXBuffer {
  public:
    virtual ~CPMapMessage() {}
    CPMapMessage();
    virtual void UpperLeft(std::int16_t x, std::int16_t y);
    virtual void LowerRight(std::int16_t x, std::int16_t y);
    virtual void Dimensions(std::int16_t width, std::int16_t height);
    virtual void GumpArt(std::int16_t newArt);
    virtual void KeyUsed(std::uint32_t key);
};

class CPMapRelated : public CPUOXBuffer {
  public:
    virtual ~CPMapRelated() {}
    CPMapRelated();
    virtual void PlotState(std::uint8_t pState);
    virtual void Location(std::int16_t x, std::int16_t y);
    virtual void command(std::uint8_t cmd);
    virtual void ID(serial_t key);
};

class CPBookTitlePage : public CPUOXBuffer {
  public:
    virtual ~CPBookTitlePage() {}
    CPBookTitlePage();
    virtual void Serial(serial_t toSet);
    virtual void WriteFlag(std::uint8_t flag);
    virtual void NewFlag(std::uint8_t flag);
    virtual void Pages(std::int16_t pages);
    virtual void Title(const std::string &txt);
    virtual void Author(const std::string &txt);
};

class CPUltimaMessenger : public CPUOXBuffer {
  public:
    virtual ~CPUltimaMessenger() {}
    CPUltimaMessenger();
    virtual void ID1(serial_t toSet);
    virtual void ID2(serial_t toSet);
};

class CPGumpTextEntry : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void BlockSize(std::int16_t newVal);
    virtual std::int16_t CurrentSize();
    virtual std::int16_t Text1Len();
    virtual void Text1Len(std::int16_t newVal);
    virtual std::int16_t Text2Len();
    virtual void Text2Len(std::int16_t newVal);

  public:
    virtual ~CPGumpTextEntry() {}
    CPGumpTextEntry();
    CPGumpTextEntry(const std::string &text);
    CPGumpTextEntry(const std::string &text1, const std::string &text2);
    virtual void Serial(serial_t id);
    virtual void ParentId(std::uint8_t newVal);
    virtual void ButtonId(std::uint8_t newVal);
    virtual void Cancel(std::uint8_t newVal); // 0 = disable, 1 = enable
    virtual void Style(std::uint8_t newVal);  // 0 = disable, 1 = normal, 2 = numerical
    virtual void Format(serial_t id);   // if style 1, max text len, if style 2, max num len
    virtual void Text1(const std::string &txt);
    virtual void Text2(const std::string &txt);
};

class CPMapChange : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPMapChange();
    CPMapChange(std::uint8_t newMap);
    CPMapChange(CBaseObject *moving);
    virtual ~CPMapChange() {}
    virtual void SetMap(std::uint8_t newMap);
    CPMapChange &operator=(CBaseObject &moving);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPCloseGump : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    std::uint32_t _gumpId;
    std::uint32_t _buttonId;

  public:
    CPCloseGump(std::uint32_t dialogId, std::uint32_t buttonId);
    virtual ~CPCloseGump() {}
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPItemsInContainer : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CSocket *mSock, CItem &toCopy);
    bool isVendor;
    bool isPlayerVendor;
    bool isCorpse;
    serial_t vendorSerial;
    bool uokrFlag;

  public:
    virtual ~CPItemsInContainer() {}
    CPItemsInContainer();
    CPItemsInContainer(CSocket *mSock, CItem *container, std::uint8_t contType = 0x00,
                       bool isPVendor = false);
    virtual void NumberOfItems(std::uint16_t numItems);
    virtual std::uint16_t NumberOfItems() const;
    void Type(std::uint8_t contType);
    void UOKRFlag(bool value);
    void PlayerVendor(bool value);
    void VendorSerial(serial_t toSet);
    virtual void AddItem(CItem *toAdd, std::uint16_t itemNum, CSocket *mSock);
    void Add(std::uint16_t itemNum, serial_t toAdd, serial_t cont, std::uint8_t amount);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPCorpseClothing : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CItem &toCopy);

  public:
    virtual ~CPCorpseClothing() {}
    CPCorpseClothing();
    CPCorpseClothing(CItem *corpse);
    virtual void NumberOfItems(std::uint16_t numItems);
    virtual void AddItem(CItem *toAdd, std::uint16_t itemNum);
    CPCorpseClothing &operator=(CItem &corpse);
};

class CPOpenBuyWindow : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CItem &toCopy, CChar *vendorId, CPItemsInContainer &iic, CSocket *mSock);

  public:
    virtual ~CPOpenBuyWindow() {}
    CPOpenBuyWindow();
    CPOpenBuyWindow(CItem *container, CChar *vendorId, CPItemsInContainer &iic, CSocket *mSock);
    virtual void NumberOfItems(std::uint8_t numItems);
    virtual std::uint8_t NumberOfItems() const;
    virtual void AddItem(CItem *toAdd, CTownRegion *tReg, std::uint16_t &baseOffset);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPCharAndStartLoc : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(AccountEntry &toCopy);
    std::uint16_t packetSize;
    std::uint8_t numCharacters;

  public:
    virtual ~CPCharAndStartLoc() {}
    CPCharAndStartLoc();
    CPCharAndStartLoc(AccountEntry &account, std::uint8_t numCharacters, std::uint8_t numLocations, CSocket *mSock);
    virtual void NumberOfLocations(std::uint8_t numLocations, CSocket *mSock);
    virtual void AddCharacter(CChar *toAdd, std::uint8_t charOffset);
    virtual auto AddStartLocation(__STARTLOCATIONDATA__ *sLoc, std::uint8_t locOffset) -> void;
    virtual auto NewAddStartLocation(__STARTLOCATIONDATA__ *sLoc, std::uint8_t locOffset) -> void;
    CPCharAndStartLoc &operator=(AccountEntry &actbBlock);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPUpdScroll : public CPUOXBuffer {
  protected:
    char tipData[2048];
    virtual void InternalReset() override;
    void SetLength(std::uint16_t len);

  public:
    CPUpdScroll();
    CPUpdScroll(std::uint8_t tType);
    CPUpdScroll(std::uint8_t tType, std::uint8_t tNum);
    virtual ~CPUpdScroll() {}
    void AddString(const char *toAdd);
    void Finalize();
    void TipNumber(std::uint8_t tipNum);
    void TipType(std::uint8_t tType);
};

class CPGraphicalEffect2 : public CPGraphicalEffect {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPGraphicalEffect2() {}
    CPGraphicalEffect2(std::uint8_t effectType);
    CPGraphicalEffect2(std::uint8_t effectType, CBaseObject &src, CBaseObject &trg);
    CPGraphicalEffect2(std::uint8_t effectType, CBaseObject &src);
    virtual void Hue(std::uint32_t hue);
    virtual void RenderMode(std::uint32_t mode);
};

class CP3DGraphicalEffect : public CPGraphicalEffect2 {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CP3DGraphicalEffect() {}
    CP3DGraphicalEffect(std::uint8_t effectType);
    CP3DGraphicalEffect(std::uint8_t effectType, CBaseObject &src, CBaseObject &trg);
    CP3DGraphicalEffect(std::uint8_t effectType, CBaseObject &src);
    virtual void EffectId3D(std::uint16_t effectId3D);
    virtual void ExplodeEffectId(std::uint16_t explodeEffectId);
    virtual void MovingEffectId(std::uint16_t movingEffectId);
    virtual void TargetObjSerial(serial_t targetObjSerial);
    virtual void LayerId(std::uint8_t layerId);
    virtual void Unknown(std::uint16_t unknown);
};

class CPDrawObject : public CPUOXBuffer {
  protected:
    std::uint16_t curLen;
    virtual void InternalReset() override;
    void SetLength(std::uint16_t len);
    void CopyData(CChar &mChar);

  public:
    CPDrawObject();
    CPDrawObject(CChar &mChar);
    virtual ~CPDrawObject() {}
    void Finalize();
    void AddItem(CItem *toAdd, bool alwaysSendItemHue);
    void SetRepFlag(std::uint8_t value);
};

class CPObjectInfo : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    void CopyData(CItem &mItem, CChar &mChar);
    void CopyItemData(CItem &mItem, CChar &mChar);
    void CopyMultiData(CMultiObj &mObj, CChar &mChar);

  public:
    CPObjectInfo();
    CPObjectInfo(CItem &mItem, CChar &mChar);
    virtual ~CPObjectInfo() {}
    void Objects(CItem &mItem, CChar &mChar);
};

class CPNewObjectInfo : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    void CopyData(CItem &mItem, CChar &mChar);
    void CopyItemData(CItem &mItem, CChar &mChar);
    void CopyMultiData(CMultiObj &mObj, CChar &mChar);

  public:
    CPNewObjectInfo();
    CPNewObjectInfo(CItem &mItem, CChar &mChar);
    virtual ~CPNewObjectInfo() {}
    void Objects(CItem &mItem, CChar &mChar);
};

class CPGameServerList : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPGameServerList() {}
    CPGameServerList();
    CPGameServerList(std::uint16_t numServers);
    virtual void NumberOfServers(std::uint16_t numItems);
    virtual void AddServer(std::uint16_t servNum, PhysicalServer *data);
    virtual void addEntry(const std::string &name, std::uint32_t addressBig);
};

class CPSecureTrading : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    void CopyData(CBaseObject &mItem, serial_t mItem2, serial_t mItem3);

  public:
    CPSecureTrading();
    CPSecureTrading(CBaseObject &mItem);
    CPSecureTrading(CBaseObject &mItem, serial_t mItem2, serial_t mItem3);
    virtual ~CPSecureTrading() {}
    void Action(std::uint8_t value);
    void Name(const std::string &nameFollowing);
};

class CPGodModeToggle : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CSocket *s);

  public:
    virtual ~CPGodModeToggle() {}
    CPGodModeToggle();
    CPGodModeToggle(CSocket *s);
    CPGodModeToggle &operator=(CSocket *s);
    virtual void ToggleStatus(bool toSet);
};

enum logindenyreason_t {
    LDR_UNKNOWNUSER = 0,
    LDR_ACCOUNTINUSE,
    LDR_ACCOUNTDISABLED,
    LDR_BADPASSWORD,
    LDR_COMMSFAILURE,
    LDR_NODENY
};

class CPLoginDeny : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPLoginDeny() {}
    CPLoginDeny();
    CPLoginDeny(logindenyreason_t reason);
    virtual void DenyReason(logindenyreason_t reason);
};

class CPCharDeleteResult : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPCharDeleteResult() {}
    CPCharDeleteResult();
    CPCharDeleteResult(std::int8_t result);
    virtual void DeleteResult(std::int8_t result);
};

class CharacterListUpdate : public CPUOXBuffer {
  protected:
    std::uint8_t numChars;
    virtual void InternalReset() override;

  public:
    virtual ~CharacterListUpdate() {}
    CharacterListUpdate();
    CharacterListUpdate(std::uint8_t charCount);
    virtual void AddCharName(std::uint8_t charCount, std::string charName);
};

class CPKREncryptionRequest : public CPUOXBuffer {
  public:
    CPKREncryptionRequest(CSocket *mSock);
};

class CPClientVersion : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    virtual ~CPClientVersion() {}
    CPClientVersion();
};

// This packet is specific for Krrios' client
class CPKAccept : public CPUOXBuffer {
  public:
    CPKAccept(std::uint8_t Response);
};

class CPUnicodeSpeech : public CPUOXBuffer {
  protected:
    void CopyData(CBaseObject &toCopy);
    void CopyData(CPITalkRequestAscii &talking);
    void CopyData(CPITalkRequestUnicode &talking);
    void InternalReset() override;
    void SetLength(std::uint16_t value);

  public:
    CPUnicodeSpeech();
    virtual ~CPUnicodeSpeech() {}
    CPUnicodeSpeech(CBaseObject &toCopy);
    CPUnicodeSpeech(CPITalkRequestAscii &talking);
    CPUnicodeSpeech(CPITalkRequestUnicode &talking);
    void ID(std::uint16_t toSet);
    void Serial(serial_t toSet);
    void Object(CPITalkRequestAscii &tSaid);
    void Object(CPITalkRequestUnicode &tSaid);
    void Object(CBaseObject &toCopy);
    void Language(char *value);
    void Language(const char *value);
    void Lanaguge(const std::string &value);
    void Type(std::uint8_t value);
    void Colour(colour_t value);
    void Font(std::uint16_t value);
    void Name(std::string value);
    void Message(const char *value);
    void GhostIt(std::uint8_t method);
    CPUnicodeSpeech &operator=(CBaseObject &toCopy);
    CPUnicodeSpeech &operator=(CPITalkRequestAscii &talking);
    CPUnicodeSpeech &operator=(CPITalkRequestUnicode &talking);
};

class CPUnicodeMessage : public CPUOXBuffer {
  protected:
    void CopyData(CBaseObject &toCopy);
    void InternalReset() override;
    void SetLength(std::uint16_t value);

  public:
    CPUnicodeMessage();
    virtual ~CPUnicodeMessage() {}
    CPUnicodeMessage(CBaseObject &toCopy);
    void ID(std::uint16_t toSet);
    void Serial(serial_t toSet);
    void Object(CBaseObject &toCopy);
    void Language(char *value);
    void Language(const char *value);
    void Lanaguge(const std::string &value);
    void Type(std::uint8_t value);
    void Colour(colour_t value);
    void Font(std::uint16_t value);
    void Name(std::string value);
    void Message(const char *value);
    void Message(const std::string value);
    CPUnicodeMessage &operator=(CBaseObject &toCopy);
};

class CPAllNames3D : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CBaseObject &obj);

  public:
    virtual ~CPAllNames3D() {}
    CPAllNames3D();
    CPAllNames3D(CBaseObject &obj);
    void Object(CBaseObject &obj);
};

class CPBookPage : public CPUOXBuffer {
  protected:
    std::uint16_t bookLength;
    std::uint8_t pageCount;
    virtual void InternalReset() override;
    virtual void CopyData(CItem &obj);
    void IncLength(std::uint8_t amount);

  public:
    virtual ~CPBookPage() {}
    CPBookPage();
    CPBookPage(CItem &obj);
    void Object(CItem &obj);
    void Serial(serial_t value);
    void NewPage(std::int16_t pNum = -1);
    void NewPage(std::int16_t pNum, const std::vector<std::string> *lines);
    void AddLine(const std::string &line);
    void Finalize();
};

class CPSendGumpMenu : public CPUOXBuffer {
  protected:
    std::vector<std::string> commands, text;

  public:
    virtual ~CPSendGumpMenu() {}
    CPSendGumpMenu();
    void UserId(serial_t value);
    void GumpId(serial_t value);
    void X(std::uint32_t value);
    void Y(std::uint32_t value);

    void addCommand(const std::string &msg);
    void addText(const std::string &msg);

    void Finalize();
    virtual void Log(std::ostream &outStream, bool fullHeader) override;
};

class CPNewSpellBook : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CItem &obj);

  public:
    virtual ~CPNewSpellBook() {}
    CPNewSpellBook();
    CPNewSpellBook(CItem &obj);
    virtual bool ClientCanReceive(CSocket *mSock) override;
};

class CPDisplayDamage : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CChar &ourTarg, std::uint16_t ourDamage);

  public:
    virtual ~CPDisplayDamage() {}
    CPDisplayDamage();
    CPDisplayDamage(CChar &ourTarg, std::uint16_t ourDamage);
    virtual bool ClientCanReceive(CSocket *mSock) override;
};

class CPQueryToolTip : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CBaseObject &mObj);

  public:
    virtual ~CPQueryToolTip() {}
    CPQueryToolTip();
    CPQueryToolTip(CBaseObject &mObj);
    virtual bool ClientCanReceive(CSocket *mSock) override;
};

class CPToolTip : public CPUOXBuffer {
  protected:
    struct ToolTipEntry_st {
        std::string ourText;
        std::uint32_t stringNum;
        size_t stringLen;
    };
    std::vector<ToolTipEntry_st> ourEntries;

    struct ToolTipEntryWide_st {
        std::wstring ourText;
        std::uint32_t stringNum;
        size_t stringLen;
    };
    std::vector<ToolTipEntryWide_st> ourWideEntries;
    CSocket *tSock;
    virtual void InternalReset() override;
    virtual void CopyData(serial_t objSer, bool addAmount = true, bool playerVendor = false);
    void CopyItemData(CItem &cItem, size_t &totalStringLen, bool addAmount = true,
                      bool playerVendor = false);
    void CopyCharData(CChar &mChar, size_t &totalStringLen);
    void FinalizeData(ToolTipEntry_st tempEntry, size_t &totalStringLen);
    void FinalizeWideData(ToolTipEntryWide_st tempEntry, size_t &totalStringLen);

  public:
    virtual ~CPToolTip() {}
    CPToolTip();
    CPToolTip(serial_t objSer, CSocket *mSock = nullptr, bool addAmount = true,
              bool playerVendor = false);
};

class CPSellList : public CPUOXBuffer {
  protected:
    std::uint16_t numItems;

    virtual void InternalReset() override;
    virtual void CopyData(CChar &mChar, CChar &vendor);

  public:
    virtual ~CPSellList() {}
    CPSellList();
    void AddContainer(CTownRegion *tReg, CItem *spItem, CItem *ourPack, size_t &packetLen);
    void AddItem(CTownRegion *tReg, CItem *spItem, CItem *opItem, size_t &packetLen);
    bool CanSellItems(CChar &mChar, CChar &vendor);
};

class CPOpenMessageBoard : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    virtual void CopyData(CSocket *mSock);

  public:
    virtual ~CPOpenMessageBoard() {}
    CPOpenMessageBoard(CSocket *mSock);
};

struct MsgBoardPost_st;

class CPOpenMsgBoardPost : public CPUOXBuffer {
  protected:
    bool bFullPost;

    virtual void InternalReset() override;
    virtual void CopyData(CSocket *mSock, const MsgBoardPost_st &mbPost);

  public:
    virtual ~CPOpenMsgBoardPost() {}
    CPOpenMsgBoardPost(CSocket *mSock, const MsgBoardPost_st &mbPost, bool fullPost);
};

class CPSendMsgBoardPosts : public CPUOXBuffer {
  protected:
    std::uint16_t postCount;

    virtual void InternalReset() override;

  public:
    virtual ~CPSendMsgBoardPosts() {}
    virtual void CopyData(CSocket *msock, serial_t mSerial, std::uint8_t pToggle, serial_t oSerial);
    void Finalize();
    CPSendMsgBoardPosts();
};

class CPHealthBarStatus : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPHealthBarStatus();
    CPHealthBarStatus(CChar &mChar, CSocket &target, std::uint8_t healthBarColor);
    virtual ~CPHealthBarStatus() {}

    virtual void CopyData(CChar &mChar);
    virtual void SetHBStatusData(CChar &mChar, CSocket &target, std::uint8_t healthBarColor);
};

class CPExtendedStats : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPExtendedStats();
    CPExtendedStats(CChar &mChar);
    virtual ~CPExtendedStats() {}

    virtual void CopyData(CChar &mChar);
};

class CPEnableMapDiffs : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPEnableMapDiffs();
    virtual ~CPEnableMapDiffs() {}
    virtual void CopyData();
};

class CPNewBookHeader : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;
    std::string title;
    std::string author;

  public:
    CPNewBookHeader();
    virtual ~CPNewBookHeader() {}
    void Serial(std::uint32_t bookSer);
    void Flag1(std::uint8_t toSet);
    void Flag2(std::uint8_t toSet);
    void Pages(std::uint16_t numPages);
    void Author(const std::string &author);
    void Title(const std::string &title);
    void Finalize();
};

class CPPopupMenu : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPPopupMenu();
    CPPopupMenu(CChar &, CSocket &);
    virtual ~CPPopupMenu() {}
    virtual void CopyData(CChar &, CSocket &);
};

class CPClilocMessage : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPClilocMessage();
    CPClilocMessage(CBaseObject &toCopy);
    virtual ~CPClilocMessage() {}
    virtual void CopyData(CBaseObject &toCopy);

    void Serial(serial_t sourceSer);
    void Body(std::uint16_t toSet);
    void Type(std::uint8_t toSet);
    void Hue(std::uint16_t hueColor);
    void Font(std::uint16_t fontType);
    void Message(std::uint32_t messageNum);
    void Name(const std::string &name);
    void ArgumentString(const std::string &arguments);
};

class CPPartyMemberList : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPPartyMemberList();
    virtual ~CPPartyMemberList() {}
    void AddMember(CChar *member);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPPartyInvitation : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPPartyInvitation();
    virtual ~CPPartyInvitation() {}
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
    void Leader(CChar *leader);
};

class CPPartyMemberRemove : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPPartyMemberRemove(CChar *removed);
    virtual ~CPPartyMemberRemove() {}
    void AddMember(CChar *member);
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPPartyTell : public CPUOXBuffer {
  protected:
    virtual void InternalReset() override;

  public:
    CPPartyTell(CPIPartyCommand *told, CSocket *talker);
    virtual ~CPPartyTell() {}
    virtual void Log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPDropItemApproved : public CPUOXBuffer {
  public:
    CPDropItemApproved();
};

#endif
