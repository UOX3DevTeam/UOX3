#ifndef __CPACKETRECEIVE_H__
#define __CPACKETRECEIVE_H__

#include <cstdint>
#include <string>

#include "typedefs.h"

#include "network.h"

class CChar;
class CMultiObj;

class CPIFirstLogin : public CPInputBuffer {
protected:
    virtual void InternalReset();
    std::string userId;
    std::string password;
    std::uint8_t unknown;
    
public:
    virtual ~CPIFirstLogin() {}
    CPIFirstLogin();
    CPIFirstLogin(CSocket *s);
    virtual void Receive() override;
    virtual const std::string Name();
    virtual const std::string Pass();
    virtual std::uint8_t Unknown();
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIServerSelect : public CPInputBuffer {
protected:
    virtual void InternalReset();
    
public:
    virtual ~CPIServerSelect() {}
    CPIServerSelect();
    CPIServerSelect(CSocket *s);
    virtual void Receive() override;
    virtual std::int16_t ServerNum();
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPISecondLogin : public CPInputBuffer {
protected:
    virtual void InternalReset();
    std::uint32_t keyUsed;
    std::string sid;
    std::string password;
    
public:
    virtual ~CPISecondLogin() {}
    CPISecondLogin();
    CPISecondLogin(CSocket *s);
    virtual void Receive() override;
    virtual std::uint32_t Account();
    virtual const std::string Name();
    virtual const std::string Pass();
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIClientVersion : public CPInputBuffer {
protected:
    virtual void InternalReset();
    char *Offset();
    std::int16_t len;
    
public:
    virtual ~CPIClientVersion() {}
    CPIClientVersion();
    CPIClientVersion(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
    virtual void SetClientVersionShortAndType(CSocket *s, char *verString);
};

class CPINewClientVersion : public CPInputBuffer {
protected:
    virtual void InternalReset();
    // char *			Offset( void );
    // std::int16_t			len;
    std::uint32_t seed;
    std::uint32_t majorVersion;
    std::uint32_t minorVersion;
    std::uint32_t clientRevision;
    std::uint32_t clientPrototype;
    
public:
    virtual ~CPINewClientVersion() {}
    CPINewClientVersion();
    CPINewClientVersion(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIUpdateRangeChange : public CPInputBuffer {
protected:
    virtual void InternalReset();
    
public:
    virtual ~CPIUpdateRangeChange() {}
    CPIUpdateRangeChange();
    CPIUpdateRangeChange(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPILogoutStatus : public CPInputBuffer {
protected:
    virtual void InternalReset();
    
public:
    virtual ~CPILogoutStatus() {}
    CPILogoutStatus();
    CPILogoutStatus(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIKrriosClientSpecial : public CPInputBuffer {
    std::uint8_t type;
    std::uint16_t blockLen;
    bool locations;
    
protected:
    virtual void InternalReset();
    
public:
    virtual ~CPIKrriosClientSpecial() {}
    CPIKrriosClientSpecial();
    CPIKrriosClientSpecial(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPITips : public CPInputBuffer {
protected:
    virtual void InternalReset();
    
public:
    virtual ~CPITips() {}
    CPITips();
    CPITips(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIRename : public CPInputBuffer {
protected:
    virtual void InternalReset();
    
public:
    virtual ~CPIRename() {}
    CPIRename();
    CPIRename(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIKeepAlive : public CPInputBuffer {
public:
    virtual ~CPIKeepAlive() {}
    CPIKeepAlive();
    CPIKeepAlive(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIStatusRequest : public CPInputBuffer {
    std::uint32_t pattern;
    std::uint8_t getType;
    serial_t playerId;
    
public:
    virtual ~CPIStatusRequest() {}
    CPIStatusRequest();
    CPIStatusRequest(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPISpy : public CPInputBuffer {
public:
    virtual ~CPISpy() {}
    CPISpy();
    CPISpy(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIKRSeed : public CPInputBuffer {
public:
    virtual ~CPIKRSeed() {}
    CPIKRSeed();
    CPIKRSeed(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIKREncryptionVerification : public CPInputBuffer {
public:
    virtual ~CPIKREncryptionVerification() {}
    CPIKREncryptionVerification();
    CPIKREncryptionVerification(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIGodModeToggle : public CPInputBuffer {
public:
    virtual ~CPIGodModeToggle() {}
    CPIGodModeToggle();
    CPIGodModeToggle(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIDblClick : public CPInputBuffer {
protected:
    serial_t objectId;
    
public:
    virtual ~CPIDblClick() {}
    CPIDblClick();
    CPIDblClick(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPISingleClick : public CPInputBuffer {
protected:
    serial_t objectId;
    
public:
    virtual ~CPISingleClick() {}
    CPISingleClick();
    CPISingleClick(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIMoveRequest : public CPInputBuffer {
public:
    virtual ~CPIMoveRequest() {}
    CPIMoveRequest();
    CPIMoveRequest(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIResyncReq : public CPInputBuffer {
public:
    virtual ~CPIResyncReq() {}
    CPIResyncReq();
    CPIResyncReq(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIResMenuChoice : public CPInputBuffer {
public:
    virtual ~CPIResMenuChoice() {}
    CPIResMenuChoice();
    CPIResMenuChoice(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIAttack : public CPInputBuffer {
public:
    virtual ~CPIAttack() {}
    CPIAttack();
    CPIAttack(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPITargetCursor : public CPInputBuffer {
public:
    virtual ~CPITargetCursor() {}
    CPITargetCursor();
    CPITargetCursor(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader) override;
};

class CPIEquipItem : public CPInputBuffer {
public:
    virtual ~CPIEquipItem() {}
    CPIEquipItem();
    CPIEquipItem(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIGetItem : public CPInputBuffer {
public:
    virtual ~CPIGetItem() {}
    CPIGetItem();
    CPIGetItem(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIDropItem : public CPInputBuffer {
protected:
    bool uokrFlag;
    std::int16_t x, y;
    std::int8_t z, gridLoc;
    serial_t item, dest;
    
public:
    virtual ~CPIDropItem() {}
    CPIDropItem();
    CPIDropItem(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIGumpMenuSelect : public CPInputBuffer {
public:
    CPIGumpMenuSelect();
    CPIGumpMenuSelect(CSocket *s);
    virtual ~CPIGumpMenuSelect() {}
    virtual void Receive() override;
    virtual bool Handle() override;
    
    serial_t ButtonId() const;
    serial_t GumpId() const;
    serial_t ID() const;
    std::uint32_t SwitchCount() const;
    std::uint32_t TextCount() const;
    std::uint32_t SwitchValue(std::uint32_t index) const;
    
    std::uint16_t GetTextId(std::uint8_t number) const;
    std::uint16_t GetTextLength(std::uint8_t number) const;
    std::string GetTextString(std::uint8_t number) const;
    
protected:
    serial_t id, buttonId, gumpId;
    std::uint32_t switchCount, textCount;
    std::uint32_t textOffset;
    
    std::vector<std::uint16_t> textLocationOffsets;
    
    void BuildTextLocations();
};

class CPITalkRequest : public CPInputBuffer {
protected:
    colour_t textColour;
    std::uint16_t strLen;
    std::uint16_t fontUsed;
    char txtSaid[4096];
    speechtype_t typeUsed;
    bool isUnicode;
    virtual void InternalReset();
    bool HandleCommon();
    
public:
    CPITalkRequest();
    CPITalkRequest(CSocket *s);
    virtual ~CPITalkRequest() {}
    virtual void Receive() override = 0;
    virtual bool Handle() override;
    
    colour_t TextColour() const;
    std::uint16_t Length() const;
    std::uint16_t Font() const;
    speechtype_t Type() const;
    bool IsUnicode() const;
    virtual std::string TextString() const;
    virtual char *Text() const;
};

class CPITalkRequestAscii : public CPITalkRequest {
public:
    CPITalkRequestAscii();
    CPITalkRequestAscii(CSocket *s);
    virtual ~CPITalkRequestAscii() {}
    virtual void Receive() override;
};

class CPITalkRequestUnicode : public CPITalkRequest {
protected:
    char unicodeTxt[8192];
    char langCode[4];
    
public:
    CPITalkRequestUnicode();
    CPITalkRequestUnicode(CSocket *s);
    virtual ~CPITalkRequestUnicode() {}
    virtual void Receive() override;
    
    virtual std::string UnicodeTextString() const;
    virtual char *UnicodeText() const;
    char *Language() const;
};

class CPIAllNames3D : public CPInputBuffer {
public:
    virtual ~CPIAllNames3D() {}
    CPIAllNames3D();
    CPIAllNames3D(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIGumpChoice : public CPInputBuffer {
public:
    virtual ~CPIGumpChoice() {}
    CPIGumpChoice();
    CPIGumpChoice(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIBuyItem : public CPInputBuffer {
public:
    virtual ~CPIBuyItem() {}
    CPIBuyItem();
    CPIBuyItem(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPISellItem : public CPInputBuffer {
public:
    virtual ~CPISellItem() {}
    CPISellItem();
    CPISellItem(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIDeleteCharacter : public CPInputBuffer {
public:
    virtual ~CPIDeleteCharacter() {}
    CPIDeleteCharacter();
    CPIDeleteCharacter(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPICreateCharacter : public CPInputBuffer {
protected:
    std::uint32_t pattern1;
    std::uint32_t pattern2;
    std::uint8_t pattern3;
    char charName[30];
    char password[30];
    std::uint8_t sex;
    std::uint8_t str;
    std::uint8_t dex;
    std::uint8_t intel;
    std::uint8_t skill[4];
    std::uint8_t skillValue[4];
    colour_t skinColour;
    std::uint16_t hairStyle;
    colour_t hairColour;
    std::uint16_t facialHair;
    colour_t facialHairColour;
    std::uint16_t locationNumber;
    std::uint16_t unknown;
    std::uint16_t slot;
    std::uint32_t ipAddress;
    colour_t shirtColour;
    colour_t pantsColour;
    
    // extra for 3DCharCreate - 0x8D
    std::uint8_t packetSize;
    std::uint8_t profession;
    std::uint8_t clientFlags;
    std::uint8_t race;
    std::uint16_t unknown1;
    std::uint16_t unknown2;
    char unknown3[25];
    std::uint16_t unknown4;
    std::uint8_t unknown5;
    std::uint16_t unknown6;
    std::uint8_t unknown7;
    std::uint8_t unknown8;
    std::uint8_t unknown9;
    std::uint16_t shirtId;
    colour_t faceColour;
    std::uint16_t faceId;
    
    void NewbieItems(CChar *mChar);
    
public:
    virtual ~CPICreateCharacter() {}
    CPICreateCharacter();
    CPICreateCharacter(CSocket *s);
    virtual void Create2DCharacter();
    virtual void Create3DCharacter();
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void SetNewCharGenderAndRace(CChar *mChar);
    virtual void SetNewCharSkillsStats(CChar *mChar);
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIPlayCharacter : public CPInputBuffer {
protected:
    std::uint32_t pattern;
    char charName[30];
    char unknown[33];
    std::uint8_t slotChosen;
    std::uint32_t ipAddress;
    
public:
    virtual ~CPIPlayCharacter() {}
    CPIPlayCharacter();
    CPIPlayCharacter(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIGumpInput : public CPInputBuffer {
protected:
    std::uint32_t id;
    std::uint8_t type;
    std::uint8_t index;
    std::uint8_t unk[3];
    std::string reply;
    
    void HandleTownstoneText(std::uint8_t index);
    
public:
    virtual ~CPIGumpInput() {}
    CPIGumpInput();
    CPIGumpInput(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    
    std::uint32_t ID() const;
    std::uint8_t Type() const;
    std::uint8_t Index() const;
    std::uint8_t Unk(std::int32_t offset) const;
    const std::string Reply() const;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIHelpRequest : public CPInputBuffer {
protected:
    std::uint16_t menuNum;
    
public:
    virtual ~CPIHelpRequest() {}
    CPIHelpRequest(CSocket *s, std::uint16_t menuVal);
    CPIHelpRequest(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPITradeMessage : public CPInputBuffer {
public:
    virtual ~CPITradeMessage() {}
    CPITradeMessage();
    CPITradeMessage(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIDyeWindow : public CPInputBuffer {
protected:
    serial_t changing;
    colour_t newValue;
    std::uint16_t modelId;
    
public:
    virtual ~CPIDyeWindow() {}
    CPIDyeWindow();
    CPIDyeWindow(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIMsgBoardEvent : public CPInputBuffer {
public:
    virtual ~CPIMsgBoardEvent() {}
    CPIMsgBoardEvent();
    CPIMsgBoardEvent(CSocket *s);
    CPIMsgBoardEvent(CSocket *s, bool receive);
    
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPINewBookHeader : public CPInputBuffer {
public:
    virtual ~CPINewBookHeader() {}
    CPINewBookHeader();
    CPINewBookHeader(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIBookPage : public CPInputBuffer {
public:
    virtual ~CPIBookPage() {}
    CPIBookPage();
    CPIBookPage(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
};

class CPIMetrics : public CPInputBuffer {
public:
    virtual ~CPIMetrics() {}
    CPIMetrics();
    CPIMetrics(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPISubcommands : public CPInputBuffer {
protected:
    std::uint16_t subCmd;
    std::uint8_t subSubCmd;
    bool skipOver;
    CPInputBuffer *subPacket;
    
public:
    virtual ~CPISubcommands() {}
    CPISubcommands();
    CPISubcommands(CSocket *s);
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIPartyCommand : public CPInputBuffer {
public:
    virtual ~CPIPartyCommand() {}
    CPIPartyCommand();
    CPIPartyCommand(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPITrackingArrow : public CPInputBuffer {
public:
    virtual ~CPITrackingArrow() {}
    CPITrackingArrow();
    CPITrackingArrow(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIClientLanguage : public CPInputBuffer {
protected:
    unicodetypes_t newLang;
    
public:
    virtual ~CPIClientLanguage() {}
    CPIClientLanguage();
    CPIClientLanguage(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIUOTDActions : public CPInputBuffer {
protected:
    std::uint16_t action;
    
public:
    virtual ~CPIUOTDActions() {}
    CPIUOTDActions();
    CPIUOTDActions(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIToolTipRequestAoS : public CPInputBuffer {
protected:
    serial_t getSer;
    
public:
    virtual ~CPIToolTipRequestAoS() {}
    CPIToolTipRequestAoS();
    CPIToolTipRequestAoS(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIToolTipRequest : public CPInputBuffer {
protected:
    serial_t getSer;
    
public:
    virtual ~CPIToolTipRequest() {}
    CPIToolTipRequest();
    CPIToolTipRequest(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIPopupMenuRequest : public CPInputBuffer {
protected:
    serial_t mSer;
    
public:
    virtual ~CPIPopupMenuRequest() {}
    CPIPopupMenuRequest();
    CPIPopupMenuRequest(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIPopupMenuSelect : public CPInputBuffer {
protected:
    std::uint16_t popupEntry;
    CChar *targChar;
    
public:
    virtual ~CPIPopupMenuSelect() {}
    CPIPopupMenuSelect();
    CPIPopupMenuSelect(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIExtendedStats : public CPInputBuffer {
protected:
    std::uint8_t statToSet;
    std::uint8_t value;
    
public:
    virtual ~CPIExtendedStats() {}
    CPIExtendedStats();
    CPIExtendedStats(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIBandageMacro : public CPInputBuffer {
public:
    virtual ~CPIBandageMacro() {}
    CPIBandageMacro();
    CPIBandageMacro(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIClosedStatusGump : public CPInputBuffer {
public:
    virtual ~CPIClosedStatusGump() {}
    CPIClosedStatusGump();
    CPIClosedStatusGump(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIToggleFlying : public CPInputBuffer {
public:
    virtual ~CPIToggleFlying() {}
    CPIToggleFlying();
    CPIToggleFlying(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPISpellbookSelect : public CPInputBuffer {
public:
    virtual ~CPISpellbookSelect() {}
    CPISpellbookSelect();
    CPISpellbookSelect(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

class CPIAOSCommand : public CPInputBuffer {
public:
    virtual ~CPIAOSCommand() {}
    CPIAOSCommand();
    CPIAOSCommand(CSocket *s);
    
    virtual void Receive() override;
    virtual bool Handle() override;
    virtual void log(std::ostream &outStream, bool fullHeader = true) override;
};

#endif
