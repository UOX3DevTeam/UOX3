#ifndef __CSOCKET_H__
#define __CSOCKET_H__

#include <cstdint>
#include <string>

#include "type/clientversion.hpp"
#include "genericlist.h"
#include "typedefs.h"
#include "utility/nettype.hpp"
#include "utility/netsocket.hpp"
class CPInputBuffer;
class CPUOXBuffer;
class CChar;
class cScript;
class CItem;
class CBaseObject;
struct AccountEntry;


enum cS_TID {
    // PC Timers
    tPC_SKILLDELAY = 0,
    tPC_OBJDELAY,
    tPC_SPIRITSPEAK,
    tPC_TRACKING,
    tPC_FISHING,
    tPC_MUTETIME,
    tPC_TRACKINGDISPLAY,
    tPC_TRAFFICWARDEN,
    tPC_COUNT
};

class CSocket {
public:
    // Account Related Member(s)
    AccountEntry &GetAccount();
    void SetAccount(AccountEntry &actbBlock);
    
    void InternalReset();
    
    util::net::ip4_t forwardIP ;
    util::net::NetSocket netSocket ; // client
    std::string clientIP ;
    std::string clientPort ;

    
private:
    std::vector<std::uint16_t> trigWords;
    std::vector<std::uint16_t>::iterator twIter;
    std::uint16_t accountNum;
    
    CChar *currCharObj;
    std::int32_t idleTimeout;
    bool wasIdleWarned;
    bool objDelayMsgShown;
    bool skillDelayMsgShown;
    std::int32_t negotiateTimeout;
    bool negotiatedWithAssistant;
    
    std::uint8_t buffer[MAXBUFFER];
    std::uint8_t outbuffer[MAXBUFFER];
    // These vectors are for dealing with packets that are larger than the buffer size
    // While admittedly not thread friendly, the number of times these buffers are used
    // should be very small and right now, is an implementation that will increase clieht
    // compatability
    std::vector<std::uint8_t> largeBuffer;
    std::vector<std::uint8_t> largePackBuffer;
    
    std::string xtext;
    std::string xtext2;
    std::int16_t clickx;
    std::int16_t clicky;
    std::int8_t clickz;
    std::uint8_t currentSpellType;
    std::int32_t outlength;
    std::int32_t inlength;
    bool logging;
    std::uint8_t range;
    bool cryptclient;
    std::int16_t walkSequence;
    size_t postAckCount;
    pickuplocations_t pSpot;
    serial_t pFrom;
    
    GenericList<CItem *> contsOpened;
    
    std::int16_t pX;
    std::int16_t pY;
    std::int8_t pZ;
    
    unicodetypes_t lang;
    ClientType cliType;
    std::uint32_t bytesReceived;
    std::uint32_t bytesSent;
    bool receivedVersion;
    //	Temporary variables (For targeting commands, etc)
    CBaseObject *tmpObj;
    CBaseObject *tmpObj2;
    std::int32_t tempint;
    std::int32_t tempint2;
    std::uint8_t dyeall;
    std::uint8_t addid[4];
    
    bool newClient;
    bool firstPacket;
    
    bool forceOffline;
    
    
    bool loginComplete;
    CItem *cursorItem; // pointer to item held on mouse cursor
    
    std::uint16_t bytesRecvWarningCount;
    std::uint16_t bytesSentWarningCount;
    
    bool targetok;
    
    std::vector<serial_t> postAcked;
    std::vector<serial_t>::iterator ackIter;
    
    std::uint32_t Pack(void *pvIn, void *pvOut, std::int32_t len);
    
    // Timer Vals moved here from CChar due to their inherently temporary nature and to reduce
    // wasted memory
    timerval_t pcTimers[tPC_COUNT];
    
public:
    CSocket(util::net::NetSocket &clientSocket,const std::string &clientIPAddress, const std::string &clientPort, util::net::ip4_t relayIPAddress);
    
    ~CSocket();
    
    ClientVersion clientVersion ;
    
    auto clientType() const -> const ClientType&;
    auto setClientType(ClientType::type_t type) ->void;
    
    void ClearPage();
    void AddTrigWord(std::uint16_t);
    // Accessors
    
    bool LoginComplete() const;
    
    CItem *GetCursorItem() const;
    void SetCursorItem(CItem *newValue);
    std::int16_t PickupX() const;
    std::int16_t PickupY() const;
    std::int8_t PickupZ() const;
    pickuplocations_t PickupSpot() const;
    serial_t PickupSerial() const;
    bool FirstPacket() const;
    std::int32_t IdleTimeout() const;
    bool WasIdleWarned() const;
    bool ObjDelayMsgShown() const;
    bool SkillDelayMsgShown() const;
    std::int32_t NegotiateTimeout() const;
    bool NegotiatedWithAssistant() const;
    std::uint8_t *Buffer();
    std::uint8_t *OutBuffer();
    std::int16_t WalkSequence() const;
    std::uint16_t AcctNo() const;
    std::string XText();
    std::string XText2();
    bool CryptClient() const;
    util::net::sockfd_t CliSocket() const;
    std::uint8_t CurrentSpellType() const;
    std::int32_t OutLength() const;
    std::int32_t InLength() const;
    bool Logging() const;
    CChar *CurrcharObj() const;
    bool NewClient() const;
    bool TargetOK() const;
    std::uint16_t FirstTrigWord();
    std::uint16_t NextTrigWord();
    bool FinishedTrigWords();
    void ClearTrigWords();
    bool ForceOffline() const;
    
    // Temporary Variables
    CBaseObject *TempObj() const;
    CBaseObject *TempObj2() const;
    std::int32_t TempInt() const;
    std::int32_t TempInt2() const;
    std::uint32_t AddId() const;
    std::uint8_t AddId1() const;
    std::uint8_t AddId2() const;
    std::uint8_t AddId3() const;
    std::uint8_t AddId4() const;
    std::uint8_t DyeAll() const;
    std::int8_t ClickZ() const;
    
    serial_t FirstPostAck();
    serial_t NextPostAck();
    bool FinishedPostAck();
    serial_t RemovePostAck();
    
    size_t PostCount() const;
    size_t PostAckCount() const;
    
    std::int16_t ClickX() const;
    std::int16_t ClickY() const;
    std::uint8_t Range() const;
    
    void CloseSocket();
    
    // Mutators
    
    void LoginComplete(bool newVal);
    
    void PickupLocation(std::int16_t x, std::int16_t y, std::int8_t z);
    void PickupX(std::int16_t x);
    void PickupY(std::int16_t y);
    void PickupZ(std::int8_t z);
    void PickupSpot(pickuplocations_t newValue);
    void PickupSerial(serial_t pickupSerial);
    void FirstPacket(bool newValue);
    void IdleTimeout(std::int32_t newValue);
    void WasIdleWarned(bool value);
    void ObjDelayMsgShown(bool value);
    void SkillDelayMsgShown(bool value);
    void NegotiateTimeout(std::int32_t newValue);
    void NegotiatedWithAssistant(bool value);
    void WalkSequence(std::int16_t newValue);
    void AcctNo(std::uint16_t newValue);
    void CryptClient(bool newValue);
    void setClient(util::net::NetSocket &client) ;
    void CurrentSpellType(std::uint8_t newValue);
    void OutLength(std::int32_t newValue);
    void InLength(std::int32_t newValue);
    void Logging(bool newValue);
    void CurrcharObj(CChar *newValue);
    void NewClient(bool newValue);
    void Range(std::uint8_t value);
    void ForceOffline(bool newValue);
    
    //	Temporary Variables
    
    // Under protest I add, NEVER NEVER Do this
    cScript *scriptForCallBack;
    // Get rid of above as soon as possible, horible.
    
    void TempObj(CBaseObject *newValue);
    void TempObj2(CBaseObject *newValue);
    void TempInt(std::int32_t newValue);
    void TempInt2(std::int32_t newValue);
    void AddId(std::uint32_t newValue);
    void AddId1(std::uint8_t newValue);
    void AddId2(std::uint8_t newValue);
    void AddId3(std::uint8_t newValue);
    void AddId4(std::uint8_t newValue);
    void DyeAll(std::uint8_t newValue);
    void ClickZ(std::int8_t newValue);
    
    bool FlushBuffer(bool doLog = true);
    bool FlushLargeBuffer(bool doLog = true);
    void FlushIncoming();
    void Send(const void *point, std::int32_t length);
    std::int32_t Receive(std::int32_t x, bool doLog = true);
    void ReceiveLogging(CPInputBuffer *toLog);
    
    std::uint32_t GetDWord(size_t offset);
    std::uint16_t GetWord(size_t offset);
    std::uint8_t GetByte(size_t offset);
    
    void SetDWord(size_t offset, std::uint32_t newValue);
    void SetWord(size_t offset, std::uint16_t newValue);
    void SetByte(size_t offset, std::uint8_t newValue);
    void TargetOK(bool newValue);
    void ClickX(std::int16_t newValue);
    void ClickY(std::int16_t newValue);
    void PostAcked(serial_t newValue);
    void PostAckCount(size_t newValue);
    void PostClear();
    void XText(const std::string &newValue);
    void XText2(const std::string &newValue);
    
    void Send(CPUOXBuffer *toSend);
    
    unicodetypes_t Language() const;
    void Language(unicodetypes_t newVal);
    
    void SysMessage(const std::string txt, ...);
    void SysMessageJS(const std::string &uformat, std::uint16_t txtColor, const std::string &data);
    void SysMessage(std::int32_t dictEntry, ...);
    void ObjMessage(const std::string &txt, CBaseObject *getObj, float secsFromNow = 0.0f, std::uint16_t Color = 0x03B2);
    void ObjMessage(std::int32_t dictEntry, CBaseObject *getObj, float secsFromNow = 0.0f, std::uint32_t Color = 0x03B2, ...);
    
    void ShowCharName(CChar *i, bool showSer);
    
    void SendTargetCursor(std::uint8_t targType, std::uint8_t targId, const std::string &txt, std::uint8_t cursorType = 0);
    void SendTargetCursor(std::uint8_t targType, std::uint8_t targId, std::uint8_t cursorType, std::int32_t dictEntry, ...);
    void mtarget(std::uint16_t itemId, std::int32_t dictEntry);
    
    void StatWindow(CBaseObject *i, bool updateParty = true);
    void UpdateSkill(std::uint8_t skillnum);
    void OpenPack(CItem *i, bool isPlayerVendor = false);
    void OpenBank(CChar *i);
    void OpenURL(const std::string &txt);
    
    auto GetContsOpenedList() -> GenericList<CItem *> *;
    
    bool ReceivedVersion() const;
    void ReceivedVersion(bool value);
    
    std::uint32_t BytesSent() const;
    void BytesSent(std::uint32_t newValue);
    std::uint32_t BytesReceived() const;
    void BytesReceived(std::uint32_t newValue);
    std::uint16_t BytesReceivedWarning() const;
    void BytesReceivedWarning(std::uint16_t newValue);
    std::uint16_t BytesSentWarning() const;
    void BytesSentWarning(std::uint16_t newValue);
    
    timerval_t GetTimer(cS_TID timerId) const;
    void SetTimer(cS_TID timerId, timerval_t value);
    void ClearTimers();
    colour_t GetFlagColour(CChar *src, CChar *trg);
    auto GetHtmlFlagColour(CChar *src, CChar *trg) -> std::string;
    
private:
};
#endif
