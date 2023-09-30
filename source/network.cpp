#include "network.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iomanip>
#include <memory>
#include "cchar.h"
#include "ceffects.h"
#include "cjsmapping.h"
#include "type/clientversion.hpp"
#include "subsystem/console.hpp"
#include "cmagic.h"
#include "cmultiobj.h"
#include "cpacketsend.h"
#include "cscript.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "cthreadqueue.h"
#include "dictionary.h"
#include "funcdecl.h"
#include "movement.h"
#include "utility/neterror.hpp"
#include "objectfactory.h"
#include "osunique.hpp"
#include "scriptc.h"
#include "configuration/serverconfig.hpp"
#include "skills.h"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"
#include "wholist.h"

extern CSkills worldSkill ;
extern CMagic worldMagic ;
extern CMovement worldMovement ;
extern CWhoList worldWhoList;
extern CWhoList worldOfflist;
extern CJSMapping worldJSMapping ;
extern cEffects worldEffect ;

using namespace std::string_literals;
// o------------------------------------------------------------------------------------------------o
//|	bytebuffer exceptions
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o

ByteBufferBounds::ByteBufferBounds(int offset, int amount, int size) : std::out_of_range(""), offset(offset), amount(amount), buffersize(size) {
    _msg = "Offset : "s + std::to_string(offset) + " Amount: "s + std::to_string(amount) +
    " exceeds buffer size of: "s + std::to_string(size);
}
//=========================================================
auto ByteBufferBounds::what() const noexcept -> const char * { return _msg.c_str(); }

// o------------------------------------------------------------------------------------------------o
//| ByteBuffer
// o------------------------------------------------------------------------------------------------o
//| strutil functions
// o------------------------------------------------------------------------------------------------o

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ByteBuffer::ntos
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Convert a bool to a string
//|					the true_value/false_value are returned based on the bool
// o------------------------------------------------------------------------------------------------o
auto ByteBuffer::ntos(bool value, const std::string &true_value, const std::string &false_value) -> std::string {
    return (value ? true_value : false_value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ByteBuffer::DumpByteBuffer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Formatted dump of a byte buffer
// o------------------------------------------------------------------------------------------------o
auto ByteBuffer::DumpByteBuffer(std::ostream &output, const std::uint8_t *buffer, std::size_t length, radix_t radix, std::size_t entries_line) -> void {
    // number of characters for entry
    auto entry_size = 3; // decimal and octal
    switch (static_cast<int>(radix)) {
        case static_cast<int>(radix_t::hex):
            entry_size = 2;
            break;
        case static_cast<int>(radix_t::bin):
            entry_size = 8;
            break;
        default:
            break;
    }
    auto num_rows = (length / entries_line) + (((length % entries_line) == 0) ? 0 : 1);
    // what is the largest number for the address ?
    auto max_address_chars = static_cast<int>((ntos(num_rows * entries_line)).size());
    
    // first write out the header
    output << std::setw(max_address_chars + 2) << "" << std::setw(1);
    for (std::size_t i = 0; i < entries_line; ++i) {
        output << ntos(i, radix_t::dec, false, entry_size, ' ') << " ";
    }
    output << "\n";
    
    // now we write out the values for each line
    std::string text(entries_line, ' ');
    
    for (std::size_t i = 0; i < length; ++i) {
        auto row = i / entries_line;
        if (((i % static_cast<size_t>(entries_line) == 0) &&
             (i >= static_cast<size_t>(entries_line))) ||
            (i == 0)) {
            // This is a new line!
            output << ntos(row * entries_line, radix_t::dec, false, max_address_chars, ' ') << ": ";
            text = std::string(entries_line, ' ');
        }
        output << ntos(buffer[i], radix, false, entry_size) << " ";
        // If it is an alpha, we want to write it
        if (std::isalpha(static_cast<int>(buffer[i])) != 0) {
            // we want to write this to the string
            text[(i % entries_line)] = buffer[i];
        }
        else {
            text[(i % entries_line)] = '.';
        }
        if (i % entries_line == entries_line - 1) {
            output << " " << text << "\n";
        }
    }
    // what if we had a partial last line, we need to figure that out
    auto last_line_entry = length % entries_line;
    if (last_line_entry != 0) {
        // we need to put the number of leading spaces
        output << std::setw(static_cast<int>((entries_line - last_line_entry) * (entry_size + 1)))
        << "" << std::setw(1) << " " << text << "\n";
    }
}

// o------------------------------------------------------------------------------------------------o
//| end strutil functions
// o------------------------------------------------------------------------------------------------o

//==================================================================================================
auto ByteBuffer::Exceeds(int offset, int bytelength) const -> bool {
    auto index = offset + bytelength;
    return static_cast<int>(_bytedata.size()) < index;
}
//==================================================================================================
auto ByteBuffer::Exceeds(int offset, int bytelength, bool expand) -> bool {
    auto rValue = Exceeds(offset, bytelength);
    if (expand && rValue) {
        this->size(offset + bytelength);
        rValue = false;
    }
    return rValue;
}
//==================================================================================================
ByteBuffer::ByteBuffer(int size, int reserve) : _index(0) {
    if ((reserve > 0) && (size <= 0)) {
        _bytedata.reserve(reserve);
    }
    if (size > 0) {
        _bytedata.resize(size);
    }
}

//==================================================================================================
auto ByteBuffer::size() const -> size_t { return _bytedata.size(); }
//==================================================================================================
auto ByteBuffer::size(int value, std::uint8_t fill) -> void { _bytedata.resize(value, fill); }
//==================================================================================================
auto ByteBuffer::index() const -> int { return _index; }
//==================================================================================================
auto ByteBuffer::index(int value) -> void { _index = value; }
//==================================================================================================
auto ByteBuffer::raw() const -> const std::uint8_t * { return _bytedata.data(); }
//==================================================================================================
auto ByteBuffer::raw() -> std::uint8_t * { return _bytedata.data(); }
//==================================================================================================
auto ByteBuffer::operator[](int index) const -> const std::uint8_t & { return _bytedata[index]; }
//==================================================================================================
auto ByteBuffer::operator[](int index) -> std::uint8_t & { return _bytedata[index]; }
//==================================================================================================
auto ByteBuffer::Fill(std::uint8_t value, int offset, int length) -> void {
    if (offset < 0) {
        offset = _index;
    }
    if (length < 0) {
        length = static_cast<int>(_bytedata.size()) - offset;
    }
    if (length > 0) {
        std::fill(_bytedata.begin() + offset, _bytedata.begin() + offset + length, value);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ByteBuffer::log()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Formatted dump of a byte buffer
// o------------------------------------------------------------------------------------------------o
auto ByteBuffer::LogByteBuffer(std::ostream &output, radix_t radix, int entries_line) const -> void {
    DumpByteBuffer(output, _bytedata.data(), _bytedata.size(), radix, entries_line);
}


#define _DEBUG_PACKET 0


void KillTrades(CChar *i);
void DoorMacro(CSocket *s);
void sysBroadcast(const std::string &txt);

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::ClearBuffers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends ALL buffered data
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::ClearBuffers() {
    std::for_each(connClients.begin(), connClients.end(), [](CSocket *sock) { sock->FlushBuffer(); });
    std::for_each(loggedInClients.begin(), loggedInClients.end(), [](CSocket *sock) { sock->FlushBuffer(); });
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::SetLastOn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the laston character member value to the current date/time
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::SetLastOn(CSocket *s) {
    assert(s != nullptr);
    
    time_t ltime;
    time(&ltime);
    char buffer[100];
    char *t = mctime(buffer, 100, &ltime);
    
    // some ctime()s like to stick \r\n on the end, we don't want that
    size_t mLen = strlen(t);
    for (size_t end = mLen; end-- > 0 && isspace(t[end]) && end < mLen;) {
        t[end] = '\0';
    }
    
    if (s->CurrcharObj() != nullptr) {
        s->CurrcharObj()->SetLastOn(t);
        s->CurrcharObj()->SetLastOnSecs(static_cast<std::uint32_t>(ltime));
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::Disconnect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces player to disconnect/instalog
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::Disconnect(uoxsocket_t s) {
    SetLastOn(connClients[s]);
    CChar *currChar = connClients[s]->CurrcharObj();
    Console::shared() << "Client " << static_cast<std::uint32_t>(s) << " disconnected. [Total:" << static_cast<std::int32_t>(worldMain.GetPlayersOnline() - 1) << "]" << myendl;
    
    if (connClients[s]->AcctNo() != AccountEntry::INVALID_ACCOUNT) {
        AccountEntry &actbAccount = connClients[s]->GetAccount();
        if (actbAccount.accountNumber != AccountEntry::INVALID_ACCOUNT) {
            actbAccount.flag.set(AccountEntry::attributeflag_t::ONLINE, false);
        }
    }
    // Instalog
    if (currChar != nullptr) {
        if (!currChar->IsFree() && connClients[s] != nullptr) {
            // October 6, 2002 - Support for the onLogout event.
            bool returnState = false;
            std::vector<std::uint16_t> scriptTriggers = currChar->GetScriptTriggers();
            for (auto i : scriptTriggers) {
                cScript *toExecute = worldJSMapping.GetScript(i);
                if (toExecute != nullptr) {
                    if (toExecute->OnLogout(connClients[s], currChar)) {
                        returnState = true;
                    }
                }
            }
            
            if (!returnState) {
                // No individual scripts handling OnLogout were found - let's check global script!
                cScript *toExecute = worldJSMapping.GetScript(static_cast<std::uint16_t>(0));
                if (toExecute != nullptr) {
                    toExecute->OnLogout(connClients[s], currChar);
                }
            }
            LogOut(connClients[s]);
        }
    }
    try {
        connClients[s]->AcctNo(AccountEntry::INVALID_ACCOUNT);
        connClients[s]->IdleTimeout(-1);
        connClients[s]->FlushBuffer();
        connClients[s]->ClearTimers();
        connClients[s]->CloseSocket();
        
        delete connClients[s];
        connClients.erase(connClients.begin() + s);
    } catch (...) {
        Console::shared() << "| CATCH: Invalid connClients[] encountered. Ignoring." << myendl;
    }
    worldMain.DecPlayersOnline();
    worldWhoList.FlagUpdate();
    worldOfflist.FlagUpdate();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::LogOut()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Process client logout, and process instalog settings
//|
//|	Changes		-	Date Unknown - Added multi checked to instalog processing
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::LogOut(CSocket *s) {
    // This would probably be a good place to do the disconnect
    CChar *p = s->CurrcharObj();
    bool valid = false;
    std::int16_t x = p->GetX(), y = p->GetY();
    std::uint8_t world = p->WorldNumber();
    std::uint8_t instanceId = p->GetInstanceId();
    
    KillTrades(p);
    
    if (p->GetCommandLevel() >= CL_CNS || p->GetAccount().accountNumber == 0 || (ServerConfig::shared().enabled(ServerSwitch::YOUNGPLAYER) && p->GetAccount().flag.test(AccountEntry::attributeflag_t::YOUNG) && ServerConfig::shared().ruleSets[Expansion::SHARD] >= Era(Era::UOR))) {
        valid = true;
    }
    else {
        auto logoutLocs = worldMain.logoutLocs;
        for (size_t a = 0; a < logoutLocs.size(); ++a) {
            if (logoutLocs[a].x1 <= x && logoutLocs[a].y1 <= y && logoutLocs[a].x2 >= x && logoutLocs[a].y2 >= y && logoutLocs[a].worldNum == world && logoutLocs[a].instanceId == instanceId) {
                valid = true;
                break;
            }
        }
    }
    if (!valid) {
        CMultiObj *multi = nullptr;
        if (!ValidateObject(p->GetMultiObj())) {
            multi = FindMulti(p);
        }
        else {
            multi = static_cast<CMultiObj *>(p->GetMultiObj());
        }
        if (ValidateObject(multi)) {
            // Check house script for onMultiLogout handling. If it returns true, instant logout is
            // allowed
            std::vector<std::uint16_t> scriptTriggers = multi->GetScriptTriggers();
            for (auto scriptTrig : scriptTriggers) {
                cScript *toExecute = worldJSMapping.GetScript(scriptTrig);
                if (toExecute != nullptr) {
                    if (toExecute->OnMultiLogout(multi, p) == 1) {
                        valid = true;
                        break;
                    }
                }
            }
        }
    }
    // If player is holding an item on the cursor, and it was picked up off the ground, drop it back
    // to the ground
    if (s->GetCursorItem()) {
        if (s->PickupSpot() == PL_GROUND) {
            CItem *i = s->GetCursorItem();
            i->SetCont(nullptr);
            i->SetLocation(p->GetX(), p->GetY(), p->GetZ());
        }
    }
    
    // Iterate through list of containers opened by player
    auto contsOpened = s->GetContsOpenedList();
    for (const auto &iCont : contsOpened->collection()) {
        if (ValidateObject(iCont)) {
            // Remove player from container's own list of players who have previously opened it
            iCont->GetContOpenedByList()->Remove(s);
        }
    }
    s->GetContsOpenedList()->Clear(); // Then clear the socket's list of opened containers
    
    AccountEntry &actbAccount = s->GetAccount();
    if (valid && !p->IsCriminal() && !p->HasStolen() && !p->IsAtWar() && !p->IsAggressor(false)) {
        if (actbAccount.accountNumber != AccountEntry::INVALID_ACCOUNT) {
            actbAccount.inGame = INVALIDSERIAL;
        }
        p->SetTimer(tPC_LOGOUT, 0);
        s->ClearTimers();
    }
    else {
        if (actbAccount.accountNumber != AccountEntry::INVALID_ACCOUNT) {
            actbAccount.inGame = p->GetSerial();
        }
        p->SetTimer(tPC_LOGOUT, BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::LOGINTIMEOUT]) ));
    }
    
    s->LoginComplete(false);
    actbAccount.flag.set(AccountEntry::attributeflag_t::ONLINE, false);
    p->SetSocket(nullptr);
}


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::SockClose()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes all sockets for shutdown
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::SockClose() {
    networkMgr.close() ;
    std::for_each(connClients.begin(), connClients.end(), [](CSocket *sock) { sock->CloseSocket(); });
}

#if !defined(_WIN32)
#ifdef FD_SETSIZE
#if FD_SETSIZE < 256
#undef FD_SETSIZE
#define FD_SETSIZE 256
#endif
#else
#define FD_SETSIZE 256
#endif
#endif

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::CheckConn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for connection requests
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::CheckConn() {
    
    auto newconnection = networkMgr.accept() ;
    if (newconnection.has_value()) {
        // sockfd_t,string clientip, string clientport, ip4_t relay
        auto value = newconnection.value();
        util::net::NetSocket client(std::get<0>(value),false);
        auto toMake = new CSocket(client, std::get<1>(value), std::get<2>(value), std::get<3>(value));
        messageLoop << util::format("Client %zu [%s] connected [Total:%lu]", worldMain.GetPlayersOnline(), std::get<1>(value).c_str(), worldMain.GetPlayersOnline() + 1);
        loggedInClients.push_back(toMake);
    }
}
//==============================================================================================================
CNetworkStuff::~CNetworkStuff() {
    for (auto iter = loggedInClients.begin();iter != loggedInClients.end();){
        (*iter)->FlushBuffer();
        (*iter)->CloseSocket();
        iter = loggedInClients.erase(iter) ;
    }
    for (auto iter = connClients.begin(); iter != connClients.end();){
        (*iter)->FlushBuffer();
        (*iter)->CloseSocket();
        iter = connClients.erase(iter) ;
    }
    
    loggedInClients.resize(0);
    connClients.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::CheckMessage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for messages from the clients
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::CheckMessage() {
    
    
    // The sockets are not blocking, so we could just peek, and if data, read it
    // Which is what we will do for now, but we can revist using kqueue/epoll(unix) and
    // WSASelect (windows) later
    std::vector<CSocket*> disconnect ;
    for (size_t i= 0 ; i < connClients.size();i++) {
        try {
            if (connClients.at(i)->netSocket.peek()){
                // It has some data!
                GetMsg(static_cast<uoxsocket_t>(i));
            }
        }
        catch(const std::exception &e){
            Console::shared() << e.what() << myendl;
            disconnect.push_back(connClients.at(i));
        }
    }
    // We now have to deal with any disconnects
    for (auto &entry:disconnect){
        auto iter = std::find_if(connClients.begin(),connClients.end(),[&entry](const CSocket *sock){
            return entry == sock ;
        });
        if (iter != connClients.end()){
            Disconnect(std::distance(connClients.begin(), iter)) ;
        }
    }
}

CNetworkStuff::CNetworkStuff() : peakConnectionCount(0) {} // Initialize sockets
auto CNetworkStuff::startup() -> void {
    LoadFirewallEntries();
    sockInit();
}
CSocket *CNetworkStuff::GetSockPtr(uoxsocket_t s) {
    if (static_cast<unsigned>(s) >= connClients.size())
        return nullptr;
    
    return connClients[s];
}

CPInputBuffer *WhichPacket(std::uint8_t packetId, CSocket *s);
CPInputBuffer *WhichLoginPacket(std::uint8_t packetId, CSocket *s);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::GetMsg()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Receive message from client
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::GetMsg(uoxsocket_t s) {
    if (static_cast<unsigned>(s) >= connClients.size())
        return;
    
    CSocket *mSock = connClients[s];
    
    if (mSock == nullptr)
        return;
    
    if (mSock->NewClient()) {
        std::int32_t count = mSock->Receive(4);
        if (mSock->Buffer()[0] == 0x21 && count < 4) { // UOMon
            std::int32_t ho, mi, se, total;
            total = (worldMain.GetUICurrentTime() - worldMain.GetStartTime()) / 1000;
            ho = total / 3600;
            total -= ho * 3600;
            mi = total / 60;
            total -= mi * 60;
            se = total;
            
            auto uoxmonitor = util::format("UOX LoginServer\r\nUOX3 Server: up for %ih %im %is\r\n", ho, mi, se);
            mSock->Send(uoxmonitor.c_str(), static_cast<std::int32_t>(uoxmonitor.size()));
            mSock->NewClient(false);
        }
        else {
            mSock->NewClient(false);
            if (mSock->GetDWord(0) == 0x12345678) {
                Disconnect(s);
            }
            mSock->FirstPacket(true);
        }
    }
    else {
        mSock->InLength(0);
        CChar *ourChar = mSock->CurrcharObj();
        std::uint8_t *buffer = mSock->Buffer();
        if (mSock->Receive(1, false) > 0) {
            std::int32_t book;
            std::uint8_t packetId = buffer[0];
            if (mSock->FirstPacket() && packetId != 0x80 && packetId != 0x91) {
                // April 5, 2004 - There is a great chance that alot of the times this will be UOG2
                // connecting to get information from the server
                if (ServerConfig::shared().enabled(ServerSwitch::UOG)) {
                    Console::shared() << "UOG Stats request completed. Disconnecting client. [" << mSock->clientIP << "]" << myendl;
                }
                else {
                    Console::shared() << "Encrypted client attempting to cut in, disconnecting them: IP " << mSock->clientIP << myendl;
                }
                Disconnect(s);
                return;
            }
#if _DEBUG_PACKET
            Console::shared().print(util::format("Packet ID: 0x%x\n", packetId));
#endif
            if (packetId != 0x73 && packetId != 0xA4 && packetId != 0x80 && packetId != 0x91) {
                if (!mSock->ForceOffline()) { // Don't refresh idle-timer if character is being forced
                    // offline
                    mSock->IdleTimeout(BuildTimeValue(ServerConfig::shared().timerSetting[TimerSetting::LOGINTIMEOUT] ));
                }
            }
            
            bool doSwitch = true;
            if (ServerConfig::shared().enabled(ServerSwitch::OVERLOADPACKETS)) {
                std::map<std::uint16_t, std::uint16_t>::const_iterator pFind = packetOverloads.find(packetId);
                if (pFind != packetOverloads.end()) {
                    cScript *pScript = worldJSMapping.GetScript(pFind->second);
                    if (pScript != nullptr) {
                        doSwitch = !pScript->OnPacketReceive(mSock, packetId);
                    }
                }
            }
            if (doSwitch) {
                CPInputBuffer *test = WhichPacket(packetId, mSock);
                if (test != nullptr) {
                    mSock->ReceiveLogging(test);
                    if (test->Handle()) {
                        doSwitch = false;
                    }
                    delete test;
                }
            }
            if (doSwitch) {
                size_t j = 0;
                switch (packetId) {
                    case 0x01: // Main Menu on the character select screen
                        Disconnect(s);
                        break;
                    case 0x72: { // Combat Mode
                        mSock->Receive(5);
                        
                        std::vector<std::uint16_t> scriptTriggers = ourChar->GetScriptTriggers();
                        for (auto scriptTrig : scriptTriggers) {
                            cScript *toExecute = worldJSMapping.GetScript(scriptTrig);
                            if (toExecute != nullptr) {
                                if (toExecute->OnWarModeToggle(ourChar) == 0) {
                                    return;
                                }
                            }
                        }
                        
                        // No individual scripts handling OnWarModeToggle returned true - let's check
                        // global script!
                        cScript *toExecute = worldJSMapping.GetScript(static_cast<std::uint16_t>(0));
                        if (toExecute != nullptr) {
                            if (toExecute->OnWarModeToggle(ourChar) == 0) {
                                return;
                            }
                        }
                        
                        ourChar->SetWar(buffer[1] != 0);
                        ourChar->SetPassive(buffer[1] == 0); // Set player as passive if they tab out of combat
                        
                        if (ourChar->GetTimer(tCHAR_TIMEOUT) <= worldMain.GetUICurrentTime()) {
                            ourChar->SetTimer(tCHAR_TIMEOUT, BuildTimeValue(1));
                        }
                        else {
                            ourChar->SetTimer(tCHAR_TIMEOUT, ourChar->GetTimer(tCHAR_TIMEOUT) + 1000);
                        }
                        mSock->Send(buffer, 5);
                        mSock->FlushBuffer();
                        worldMovement.CombatWalk(ourChar);
                        worldEffect.DoSocketMusic(mSock);
                        ourChar->BreakConcentration(mSock);
                        break;
                    }
                    case 0x12: // Ext. Command
                        mSock->Receive(3);
                        mSock->Receive(mSock->GetWord(1));
                        ourChar->BreakConcentration(mSock);
                        if (buffer[3] == 0xC7) { // Action
                            if (ourChar->IsOnHorse() || ourChar->IsFlying())
                                return; // can't bow or salute on horse or while flying
                            
                            if (std::string(reinterpret_cast<char *>(&buffer[4])) == "bow") {
                                if (ourChar->GetBodyType() == BT_GARGOYLE || (ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET) && (ourChar->GetSocket() == nullptr || ourChar->GetSocket()->clientType() >= ClientType::SA2D))) {
                                    // If gargoyle, human or elf, and new animation packet is enabled
                                    worldEffect.PlayNewCharacterAnimation(ourChar, N_ACT_EMOTE, S_ACT_EMOTE_BOW);
                                }
                                else if ((ourChar->GetBodyType() == BT_HUMAN || ourChar->GetBodyType() == BT_ELF) && !ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET)) {
                                    // If human or elf, and new animation packet is disabled
                                    worldEffect.PlayCharacterAnimation(ourChar, ACT_EMOTE_BOW, 0, 5); // 0x20
                                }
                                else {
                                    // If polymorphed to other creatures
                                    worldEffect.PlayCharacterAnimation(ourChar, 0x12, 0,  5); // Monster fidget 1
                                }
                            }
                            if (std::string(reinterpret_cast<char *>(&buffer[4])) == "salute") {
                                if (ourChar->GetBodyType() == BT_GARGOYLE || (ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET) && (ourChar->GetSocket() == nullptr || ourChar->GetSocket()->clientType() >= ClientType::SA2D))) {
                                    // If gargoyle, human or elf, and new animation packet is enabled
                                    worldEffect.PlayNewCharacterAnimation(ourChar, N_ACT_EMOTE, S_ACT_EMOTE_SALUTE);
                                }
                                else if ((ourChar->GetBodyType() == BT_HUMAN ||  ourChar->GetBodyType() == BT_ELF) && !ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET)) {
                                    // If human or elf, and new animation packet is disabled
                                    worldEffect.PlayCharacterAnimation(ourChar, ACT_EMOTE_SALUTE, 0,  5); // 0x21
                                }
                                else {
                                    // If polymorphed to other creatures
                                    worldEffect.PlayCharacterAnimation(ourChar, 0x11, 0, 5); // Monster fidget 2
                                }
                            }
                            break;
                        }
                        else if (buffer[3] == 0x58) {
                            DoorMacro(mSock);
                            break;
                        }
                        else if (buffer[3] == 0x24) { // Skill
                            j = 4;
                            while (buffer[j] != ' ') {
                                ++j;
                            }
                            buffer[j] = 0;
                            worldSkill.SkillUse(mSock, static_cast<std::uint8_t>(std::stoul(std::string((char *)&buffer[4]))));
                            break;
                        }
                        else if (buffer[3] == 0x27 || buffer[3] == 0x56) { // Spell
                            // This'll find our spellbook for us
                            CItem *sBook = FindItemOfType(ourChar, IT_SPELLBOOK);
                            CItem *p = ourChar->GetPackItem();
                            if (ValidateObject(sBook)) {
                                bool validLoc = false;
                                if (sBook->GetCont() == ourChar) {
                                    validLoc = true;
                                }
                                else if (ValidateObject(p) && sBook->GetCont() == p) {
                                    validLoc = true;
                                }
                                
                                if (validLoc) {
                                    book = buffer[4] - 0x30;
                                    if (buffer[5] > 0x20) {
                                        book = (book * 10) + (buffer[5] - 0x30);
                                    }
                                    if (worldMagic.CheckBook(((book - 1) / 8) + 1, (book - 1) % 8, sBook)) {
                                        if (ourChar->IsFrozen()) {
                                            if (ourChar->IsCasting()) {
                                                mSock->SysMessage(762); // You are already casting a spell.
                                            }
                                            else {
                                                mSock->SysMessage(763); // You cannot cast spells while frozen.
                                            }
                                        }
                                        else {
                                            mSock->CurrentSpellType(0);
                                            worldMagic.SelectSpell(mSock, book);
                                        }
                                    }
                                    else {
                                        mSock->SysMessage(764); // You do not have that spell.
                                    }
                                }
                                else {
                                    mSock->SysMessage(765); // To cast spells, your spellbook must be in your hands or
                                    // in the first layer of your pack.
                                }
                            }
                        }
                        else {
                            if (buffer[2] == 0x05 && buffer[3] == 0x43){ // Open spell book
                                worldMagic.SpellBook(mSock);
                            }
                            break;
                        }
                        break;
                        // client sends them out if the title and/or author gets changed on a writeable
                        // book its NOT send (anymore?) when a book is closed as many packet docus
                        // state. 7-dec 1999
                    case 0x93: // Old Book Packet, no longer sent by the client
                        mSock->Receive(99);
                        break;
                    case 0x69:             // Client text change
                        mSock->Receive(3); // What a STUPID message...  It would be useful if
                        mSock->Receive( mSock->GetWord(1)); // it included the color changed to, but it doesn't!
                        break;
                    case 0xB5: // Open Chat Window
                        mSock->Receive(64);
                        break;
                    case 0xB6: // T2A Popuphelp request
                        mSock->Receive(9);
                        break;
                    case 0xB8: // T2A Profile request
                        mSock->Receive(3);
                        mSock->Receive(mSock->GetWord(1)); // this is how it's really done
                        
                        if (buffer[3] == 1) { // Set profile, not feasible yet
                            mSock->SysMessage(768); // You cannot set your profile currently.
                        }
                        else {
                            serial_t targSerial = mSock->GetDWord(4);
                            CChar *player = CalcCharObjFromSer(targSerial);
                            if (!ValidateObject(player)) {
                                mSock->SysMessage(769); // This player no longer exists.
                            }
                            else {
                                // Still doesn't work
                                //	Request Char Profile (Variable # of bytes)
                                //	BYTE cmd
                                //	BYTE[2] blockSize
                                //	BYTE mode (CLIENT ONLY! Does not exist in server message.)
                                //	BYTE[4] id
                                //	If request, ends here.
                                //	If Update request
                                //	BYTE[2] cmdType (0x0001 – Update)
                                //	BYTE[2] msglen (# of unicode characters)
                                //	BYTE[msglen][2] new profile, in unicode, not null terminated.
                                //	Else If from server
                                //	BYTE[?] title (not unicode, null terminated.)
                                //	BYTE[?][2] static profile  ( unicode string can't  be edited )
                                //	BYTE[2] (0x0000) (static profile terminator)
                                //	BYTE[?][2] profile (in unicode, can be edited)
                                //	BYTE[2] (0x0000) (profile terminator)
                                
                                std::uint8_t tempBuffer[1024];
                                std::string ourMessage;
                                memset(tempBuffer, 0, sizeof(tempBuffer[0]) * 1024); // zero it out
                                tempBuffer[0] = 0xB8;
                                tempBuffer[3] = buffer[4];
                                tempBuffer[4] = buffer[5];
                                tempBuffer[5] = buffer[6];
                                tempBuffer[6] = buffer[7];
                                auto playerName =
                                player->GetNameRequest(mSock->CurrcharObj(), NRS_JOURNAL);
                                size_t plNameLen = playerName.size();
                                strncopy((char *)&tempBuffer[7], 30, playerName.c_str(), 30);
                                size_t mj = 8 + plNameLen;
                                auto playerTitle = player->GetTitle();
                                size_t plTitleLen = playerTitle.size() + 1;
                                for (size_t k = 0; k < plTitleLen; ++k) {
                                    tempBuffer[mj++] = 0;
                                    tempBuffer[mj++] = playerTitle.data()[k];
                                }
                                mj += 2;
                                ourMessage = "Test of Char Profile";
                                for (j = 0; j < ourMessage.size(); ++j) {
                                    tempBuffer[mj++] = 0;
                                    tempBuffer[mj++] = ourMessage[j];
                                }
                                mj += 2;
                                tempBuffer[2] = static_cast<std::uint8_t>(mj);
                                mSock->Send(tempBuffer, static_cast<std::int32_t>(mj));
                                mSock->FlushBuffer();
                            }
                        }
                        break;
                    case 0xBB:
                        //	Ultima Messenger (9 bytes)
                        //	BYTE cmd
                        //	BYTE[4] id1
                        //	BYTE[4] id2
                        //	Note: This is both a client and server message.
                        mSock->Receive(9);
                        break;
                    case 0x3A: // skills management packet
                        mSock->Receive(6); // it's always 6 so Westy tells me... let's not be intelligent =)
                        std::uint16_t skillNum;
                        skillNum = mSock->GetWord(3);
                        ourChar->SetSkillLock(static_cast<skilllock_t>(buffer[5]),static_cast<std::uint8_t>(skillNum));
                        break;
                    case 0x56:
                        Console::shared() << "'Plot Course' button on a map clicked." << myendl;
                        break;
                    case 0xD0:
                        // Configuration file
                        mSock->Receive(3);
                        mSock->Receive(mSock->GetWord(1));
                        break;
                    case 0xD9: // Spy On Client
                        break;
                    case 0xFB: // Update View Public House Contents
                        mSock->Receive(2);
                        break;
                    case 0xF0: // ClassicUO Map Tracker, handled by CPIKrriosClientSpecial in
                        // cpacketreceive.cpp
                        break;
                    default: {
                        try {
                            if (mSock->netSocket.peek()) {
                                mSock->Receive(MAXBUFFER);
                            }
                            Console::shared() << util::format("Unknown message from client: 0x%02X - Ignored", packetId) << myendl;
                        }
                        catch(const std::exception &e){
                            std::cerr << e.what()<< std::endl;
                        }
                        break;
                    }
                }
            }
        }
        else{ // Sortta error type thing, they disconnected already
            Disconnect(s);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::CheckLoginMessage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	 Check for messages from the clients
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::CheckLoginMessage() {
    
    auto disconnect = std::vector<CSocket*>() ;
    for (size_t i = 0; i< loggedInClients.size();i++){
        try {
            if (loggedInClients.at(i)->netSocket.peek()){
                GetLoginMsg(static_cast<uoxsocket_t>(i));
            }
        }
        catch (const std::exception &e){
            messageLoop << e.what() ;
            disconnect.push_back(loggedInClients.at(i));
        }
    }
    for (auto &entry:disconnect){
        auto iter = std::find_if(loggedInClients.begin(),loggedInClients.end(),[&entry](const CSocket* value){
            return entry == value ;
        });
        if (iter != loggedInClients.end()){
            LoginDisconnect(std::distance(loggedInClients.begin(), iter));
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::LoginDisconnect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Force disconnection of player //Instalog
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::LoginDisconnect(uoxsocket_t s) {
    messageLoop << util::format("LoginClient %u disconnected.", s);
    loggedInClients[s]->FlushBuffer();
    loggedInClients[s]->CloseSocket();
    
    // 9/17/01 : fix for clients disconnecting during login not being able to reconnect.
    if (loggedInClients[s]->AcctNo() != AccountEntry::INVALID_ACCOUNT) {
        AccountEntry &actbAccount = loggedInClients[s]->GetAccount();
        if (actbAccount.accountNumber != AccountEntry::INVALID_ACCOUNT) {
            actbAccount.flag.set(AccountEntry::attributeflag_t::ONLINE, false);
        }
    }
    
    delete loggedInClients[s];
    loggedInClients.erase(loggedInClients.begin() + s);
}

void CNetworkStuff::LoginDisconnect(CSocket *s) { // Force disconnection of player //Instalog
    uoxsocket_t i = FindLoginPtr(s);
    LoginDisconnect(i);
}

void CNetworkStuff::Disconnect(CSocket *s) { // Force disconnection of player //Instalog
    uoxsocket_t i = FindNetworkPtr(s);
    Disconnect(i);
}

uoxsocket_t CNetworkStuff::FindLoginPtr(CSocket *s) {
    for (uoxsocket_t i = 0; static_cast<unsigned>(i) < loggedInClients.size(); ++i) {
        if (loggedInClients[i] == s)
            return i;
    }
    return 0xFFFFFFFF;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::Transfer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Transfers from the logged in queue to the in world queue
//|
//|	Notes		-	Takes something out of the logging in queue and places it in the in
// world queue |					REQUIRES THREAD SAFETY
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::Transfer(CSocket *mSock) {
    uoxsocket_t s = FindLoginPtr(mSock);
    if (static_cast<unsigned>(s) >= loggedInClients.size())
        return;
    
    // std::scoped_lock lock(internallock);
    connClients.push_back(loggedInClients[s]);
    loggedInClients.erase(loggedInClients.begin() + s);
    worldMain.SetPlayersOnline(connClients.size());
    if (worldMain.GetPlayersOnline() > peakConnectionCount) {
        peakConnectionCount = worldMain.GetPlayersOnline();
    }
    return;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::GetLoginMsg()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets login message from socket
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::GetLoginMsg(uoxsocket_t s) {
    CSocket *mSock = loggedInClients[s];
    if (mSock == nullptr)
        return;
    
    if (mSock->NewClient()) {
        [[maybe_unused]] std::int32_t count;
        count = mSock->Receive(4);
        auto packetId = mSock->Buffer()[0];
        
        if (ServerConfig::shared().enabled(ServerSwitch::OVERLOADPACKETS)) {
            // Allow overloading initial connection packets
            std::map<std::uint16_t, std::uint16_t>::const_iterator pFind = packetOverloads.find(packetId);
            if (pFind != packetOverloads.end()) {
                cScript *pScript = worldJSMapping.GetScript(pFind->second);
                if (pScript != nullptr) {
                    pScript->OnPacketReceive(mSock, packetId);
                }
            }
        }
        
        if (packetId == 0xEF || packetId == 0xC0 || packetId == 0xFF) {
            WhichLoginPacket(packetId, mSock);
            // mSock->Receive( 21, false );
        }
        
        mSock->NewClient(false);
        if (mSock->GetDWord(0) == 0x12345678) {
            LoginDisconnect(s);
        }
        mSock->FirstPacket(true);
    }
    else {
        mSock->InLength(0);
        std::uint8_t *buffer = mSock->Buffer();
        if (mSock->Receive(1, false) > 0) {
            std::uint8_t packetId = buffer[0];
            if (mSock->FirstPacket() && packetId != 0x80 && packetId != 0x91 && packetId != 0xE4 && packetId != 0xF1) {
                // April 5, 2004 - Hmmm there are two of these ?
                if (ServerConfig::shared().enabled(ServerSwitch::UOG)) {
                    messageLoop << util::format( "UOG Stats Sent or Encrypted client detected. [%s]", mSock->clientIP.c_str());
                }
                else {
                    messageLoop << util::format("Encrypted client detected. [%s]", mSock->clientIP.c_str());
                }
                LoginDisconnect(s);
                return;
            }
            else if (mSock->FirstPacket() && packetId == 0) {
                messageLoop << util::format("Buffer is empty, no packets to read. Disconnecting client. [%s]", mSock->clientIP.c_str());
                LoginDisconnect(s);
                return;
            }
            bool doSwitch = true;
            if (ServerConfig::shared().enabled(ServerSwitch::OVERLOADPACKETS)) {
                std::map<std::uint16_t, std::uint16_t>::const_iterator pFind = packetOverloads.find(packetId);
                if (pFind != packetOverloads.end()) {
                    cScript *pScript = worldJSMapping.GetScript(pFind->second);
                    if (pScript != nullptr) {
                        doSwitch = !pScript->OnPacketReceive(mSock, packetId);
                    }
                }
            }
            if (doSwitch) {
                CPInputBuffer *test = nullptr;
                try {
                    test = WhichLoginPacket(packetId, mSock);
                }
                catch (PacketError &e) {
                    Console::shared().warning(e.what());
                    mSock->FlushIncoming();
                    return;
                }
                if (test != nullptr) {
#ifdef _DEBUG_PACKET
                    Console::shared().log(util::format("Logging packet ID: 0x%X", packetId), "loginthread.txt");
#endif
                    mSock->ReceiveLogging(test);
                    if (test->Handle()) {
                        doSwitch = false;
                    }
                    delete test;
                }
            }
            if (doSwitch) {
                switch (packetId) {
                    case 0x01:
                        // Disconnect Notification (5 bytes)
                        //	BYTE cmd
                        //	BYTE[4] pattern (0xFFFFFFFF)
                        // Note:  Client message
                        // Note:  Sent when the user chooses to return to the main menu from the
                        // character select menu. Note:  Since the character select menu no longer has a
                        // main menu button, this message is no longer sent. Note:  It’s send again in
                        // current 3.0.x clients. When player does "paper-doll logout"
                        LoginDisconnect(s);
                        break;
                    case 0xBB:
                        // Ultima Messenger  (9 bytes)
                        //	BYTE cmd
                        //	BYTE[4] id1
                        //	BYTE[4] id2
                        // Note: This is both a client and server message.
                        mSock->Receive(9);
                        break;
                    case 0xBF:
                        mSock->Receive(3);
                        mSock->Receive(mSock->GetWord(1));
                        break;
                    default: {
                        try {
                            if (mSock->netSocket.peek()){
                                mSock->Receive(MAXBUFFER);
                                messageLoop << util::format("Unknown message from client: 0x%02X - Ignored", packetId);
                            }
                        }
                        catch(const std::exception &e){
                            std::cerr <<e.what()<< std::endl;
                        }
                        break;
                    }
                        
                }
            }
        }
        else { // Sortta error type thing, they disconnected already
            LoginDisconnect(s);
        }
    }
}

uoxsocket_t CNetworkStuff::FindNetworkPtr(CSocket *toFind) {
    for (uoxsocket_t i = 0; static_cast<unsigned>(i) < connClients.size(); ++i) {
        if (connClients[i] == toFind)
            return i;
    }
    return 0xFFFFFFFF;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::LoadFirewallEntries()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads list of banned IPs from firewall list
// o------------------------------------------------------------------------------------------------o
auto CNetworkStuff::LoadFirewallEntries() -> void {
    // These should be config entries
    // But so where do we look?
    networkMgr.loadAccessFiles(ServerConfig::shared().directoryFor(dirlocation_t::SAVE)/std::filesystem::path("allow.cfg"),ServerConfig::shared().directoryFor(dirlocation_t::SAVE)/std::filesystem::path("deny.cfg"));
}

void CNetworkStuff::RegisterPacket(std::uint8_t packet, [[maybe_unused]] std::uint8_t subCmd, std::uint16_t scriptId) {
    // std::uint16_t packetId = static_cast<std::uint16_t>(( subCmd << 8 ) + packet ); // Registration of subCmd
    // disabled until it can be fully implemented
    std::uint16_t packetId = static_cast<std::uint16_t>(packet);
    packetOverloads[packetId] = scriptId;
}

void CNetworkStuff::CheckConnections() { CheckConn(); }

void CNetworkStuff::CheckMessages() {
    CheckLoginMessage();
    CheckMessage();
}

size_t CNetworkStuff::PeakConnectionCount() const { return peakConnectionCount; }

//============================================================================
auto CNetworkStuff::sockInit() ->void {
    networkMgr.listen(ServerConfig::shared().ushortValues[UShortValue::PORT], ServerConfig::shared().serverString[ServerString::PUBLICIP]);
}
