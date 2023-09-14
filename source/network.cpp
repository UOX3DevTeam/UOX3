#include "network.h"

#include <cassert>
#include <cctype>
#include <iomanip>
#include <memory>
#if !defined(_WIN32)
#include <sys/ioctl.h>
#endif

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
#include "mapstuff.h"
#include "movement.h"
#include "objectfactory.h"
#include "osunique.hpp"
#include "scriptc.h"
#include "configuration/serverconfig.hpp"
#include "skills.h"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"
#include "wholist.h"

using namespace std::string_literals;
// o------------------------------------------------------------------------------------------------o
//|	bytebuffer exceptions
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o

ByteBufferBounds::ByteBufferBounds(int offset, int amount, int size)
: std::out_of_range(""), offset(offset), amount(amount), buffersize(size) {
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
auto ByteBuffer::ntos(bool value, const std::string &true_value, const std::string &false_value)
-> std::string {
    return (value ? true_value : false_value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ByteBuffer::DumpByteBuffer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Formatted dump of a byte buffer
// o------------------------------------------------------------------------------------------------o
auto ByteBuffer::DumpByteBuffer(std::ostream &output, const std::uint8_t *buffer,
                                std::size_t length, radix_t radix, std::size_t entries_line)
-> void {
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
auto ByteBuffer::LogByteBuffer(std::ostream &output, radix_t radix, int entries_line) const
-> void {
    DumpByteBuffer(output, _bytedata.data(), _bytedata.size(), radix, entries_line);
}

CNetworkStuff *Network = nullptr;

#define _DEBUG_PACKET 0

fd_set conn;
fd_set all;
fd_set errsock;

void KillTrades(CChar *i);
void DoorMacro(CSocket *s);
void sysBroadcast(const std::string &txt);

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::ClearBuffers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends ALL buffered data
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::ClearBuffers() {
    std::for_each(connClients.begin(), connClients.end(),
                  [](CSocket *sock) { sock->FlushBuffer(); });
    std::for_each(loggedInClients.begin(), loggedInClients.end(),
                  [](CSocket *sock) { sock->FlushBuffer(); });
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
    Console::shared() << "Client " << static_cast<std::uint32_t>(s) << " disconnected. [Total:"
    << static_cast<std::int32_t>(cwmWorldState->GetPlayersOnline() - 1) << "]" << myendl;
    
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
                cScript *toExecute = JSMapping->GetScript(i);
                if (toExecute != nullptr) {
                    if (toExecute->OnLogout(connClients[s], currChar)) {
                        returnState = true;
                    }
                }
            }
            
            if (!returnState) {
                // No individual scripts handling OnLogout were found - let's check global script!
                cScript *toExecute = JSMapping->GetScript(static_cast<std::uint16_t>(0));
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
    cwmWorldState->DecPlayersOnline();
    WhoList->FlagUpdate();
    OffList->FlagUpdate();
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
        auto logoutLocs = cwmWorldState->logoutLocs;
        for (size_t a = 0; a < logoutLocs.size(); ++a) {
            if (logoutLocs[a].x1 <= x && logoutLocs[a].y1 <= y && logoutLocs[a].x2 >= x &&
                logoutLocs[a].y2 >= y && logoutLocs[a].worldNum == world &&
                logoutLocs[a].instanceId == instanceId) {
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
                cScript *toExecute = JSMapping->GetScript(scriptTrig);
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
        p->SetTimer(tPC_LOGOUT,
                    BuildTimeValue(cwmWorldState->ServerData()->SystemTimer(tSERVER_LOGINTIMEOUT)));
    }
    
    s->LoginComplete(false);
    actbAccount.flag.set(AccountEntry::attributeflag_t::ONLINE, false);
    p->SetSocket(nullptr);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::sockInit()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes and binds sockets during startup
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::sockInit() {
    std::int32_t bcode;
    
    cwmWorldState->SetKeepRun(true);
    cwmWorldState->SetError(false);
    
#if !defined(_WIN32)
    std::int32_t on = 1;
#endif
    
    a_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (a_socket < 0) {
        Console::shared().error(" Unable to create socket");
#if defined(_WIN32)
        Console::shared().error(util::format(" Code %i", WSAGetLastError()));
#else
        Console::shared() << myendl;
#endif
        cwmWorldState->SetKeepRun(false);
        cwmWorldState->SetError(true);
        Shutdown(FATAL_UOX3_ALLOC_NETWORK);
        return;
    }
#if !defined(_WIN32)
    [[maybe_unused]] int result = setsockopt(a_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif
    
    std::uint32_t len_connection_addr = sizeof(struct sockaddr_in);
    sockaddr_in connection;
    memset((char *)&connection, 0, len_connection_addr);
    connection.sin_family = AF_INET;
    connection.sin_addr.s_addr = htonl(INADDR_ANY);
    connection.sin_port = htons(cwmWorldState->ServerData()->ServerPort());
    bcode = bind(a_socket, (struct sockaddr *)&connection, len_connection_addr);
    
    if (bcode < 0) {
        Console::shared().error(util::format(" Unable to bind socket 1 - Error code: %i", bcode));
        cwmWorldState->SetKeepRun(false);
        cwmWorldState->SetError(true);
        Shutdown(FATAL_UOX3_ALLOC_NETWORK);
        return;
    }
    
    unsigned long mode = 1;
    // set the socket to nonblocking
    ioctlsocket(a_socket, FIONBIO, &mode);
    
    listen(a_socket, 42);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::SockClose()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes all sockets for shutdown
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::SockClose() {
    closesocket(a_socket);
    std::for_each(connClients.begin(), connClients.end(),
                  [](CSocket *sock) { sock->CloseSocket(); });
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
    FD_ZERO(&conn);
    FD_SET(a_socket, &conn);
    std::int32_t nfds = a_socket + 1;
    std::int32_t s = select(nfds, &conn, nullptr, nullptr, &cwmWorldState->uoxTimeout);
    if (s > 0) {
        std::int32_t len = sizeof(struct sockaddr_in);
        SOCKET newClient;
#if defined(_WIN32)
        newClient = accept(a_socket, (struct sockaddr *)&client_addr, &len);
#else
        newClient = accept(a_socket, (struct sockaddr *)&client_addr, (socklen_t *)&len);
        if (newClient >= FD_SETSIZE) {
            Console::shared().error("accept() returning unselectable fd!");
            closesocket(static_cast<uoxsocket_t>(newClient));
            return;
        }
#endif
        CSocket *toMake = new CSocket(newClient);
        // set the ip address of the client;
        toMake->ipaddress = IP4Addr(client_addr.sin_addr.s_addr);
        
        if (newClient < 0) {
#if defined(_WIN32)
            std::int32_t errorCode = WSAGetLastError();
            if (errorCode == WSAEWOULDBLOCK)
#else
                std::int32_t errorCode = errno;
            if (errorCode == EWOULDBLOCK)
#endif
            {
                delete toMake;
                return;
            }
            Console::shared().error("Error at client connection!");
            cwmWorldState->SetKeepRun(true);
            cwmWorldState->SetError(true);
            delete toMake;
            return;
        }
        //	April 5, 2000
        //	Due to an attack on the shard, and the true inability to determine who did what
        //	I am implementing a ShitList, any IP on this list will be immediatly dropped.
        //	This is a light weight firewall attempt to stop some people that are attacking
        //	a shard that they can easily create new accounts automatically on.
        //	ListFormat: x.x.x.x --> * means any for that domain class
        //   _  _  _
        std::uint8_t part[4];
        part[0] = static_cast<std::uint8_t>(client_addr.sin_addr.s_addr & 0xFF);
        part[1] = static_cast<std::uint8_t>((client_addr.sin_addr.s_addr & 0xFF00) >> 8);
        part[2] = static_cast<std::uint8_t>((client_addr.sin_addr.s_addr & 0xFF0000) >> 16);
        part[3] = static_cast<std::uint8_t>((client_addr.sin_addr.s_addr & 0xFF000000) >> 24);
        
        if (IsFirewallBlocked(part)) {
            messageLoop << util::format("FIREWALL: Blocking address %i.%i.%i.%i --> Blocked!",
                                        part[0], part[1], part[2], part[3]);
            closesocket(static_cast<uoxsocket_t>(newClient));
            delete toMake;
            return;
        }
        // Firewall-messages are really only needed when firewall blocks, not when it lets someone
        // through. Leads to information overload in console. Commenting out.
        
        messageLoop << util::format("Client %zu [%i.%i.%i.%i] connected [Total:%lu]",
                                    cwmWorldState->GetPlayersOnline(), part[0], part[1], part[2],
                                    part[3], cwmWorldState->GetPlayersOnline() + 1);
        loggedInClients.push_back(toMake);
        toMake->ClientIP(client_addr.sin_addr.s_addr);
        return;
    }
    if (s < 0) {
        Console::shared().error(" Select (Conn) failed!");
        cwmWorldState->SetKeepRun(false);
        cwmWorldState->SetError(true);
        return;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::IsFirewallBlocked()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if IP of connecting client is blocked by firewall
// o------------------------------------------------------------------------------------------------o
bool CNetworkStuff::IsFirewallBlocked(std::uint8_t part[4]) {
    bool match[4];
    for (size_t i = 0; i < slEntries.size(); ++i) {
        for (std::uint8_t k = 0; k < 4; ++k) {
            if (slEntries[i].b[k] == -1) {
                match[k] = true;
            }
            else {
                match[k] = (slEntries[i].b[k] == part[k]);
            }
        }
        if (match[0] && match[1] && match[2] && match[3])
            return true;
    }
    return false;
}
CNetworkStuff::~CNetworkStuff() {
    size_t i = 0;
    size_t s = 0;
    for (i = 0; i < loggedInClients.size(); ++i) {
        loggedInClients[i]->FlushBuffer();
        loggedInClients[i]->CloseSocket();
        delete loggedInClients[i];
    }
    for (i = 0; i < connClients.size(); ++i) {
        connClients[i]->FlushBuffer();
        connClients[i]->CloseSocket();
        s = std::max(s, connClients[i]->CliSocket() + 1);
        delete connClients[i];
    }
    
    loggedInClients.resize(0);
    connClients.resize(0);
    closesocket(static_cast<uoxsocket_t>(s));
#if defined(_WIN32)
    WSACleanup();
#endif
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CNetworkStuff::CheckMessage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for messages from the clients
// o------------------------------------------------------------------------------------------------o
void CNetworkStuff::CheckMessage() {
    FD_ZERO(&all);
    FD_ZERO(&errsock);
    std::int32_t nfds = 0;
    for (auto &tSock : connClients) {
        auto clientSock = static_cast<uoxsocket_t>(tSock->CliSocket());
        FD_SET(clientSock, &all);
        FD_SET(clientSock, &errsock);
        if (static_cast<int>(clientSock) + 1 > nfds) {
            nfds = clientSock + 1;
        }
    }
    std::int32_t s = select(nfds, &all, nullptr, &errsock, &cwmWorldState->uoxTimeout);
    if (s > 0) {
        size_t oldnow = cwmWorldState->GetPlayersOnline();
        for (size_t i = 0; i < cwmWorldState->GetPlayersOnline(); ++i) {
            if (FD_ISSET(connClients[i]->CliSocket(), &errsock)) {
                Disconnect(static_cast<uoxsocket_t>(i));
            }
            if ((FD_ISSET(connClients[i]->CliSocket(), &all)) &&
                (oldnow == cwmWorldState->GetPlayersOnline())) {
                try {
                    GetMsg(static_cast<uoxsocket_t>(i));
                } catch (socket_error &blah) {
#if !defined(_WIN32)
                    Console::shared() << "Client disconnected" << myendl;
#else
                    if (blah.ErrorNumber() == WSAECONNRESET) {
                        Console::shared() << "Client disconnected" << myendl;
                    }
                    else if (blah.ErrorNumber() != -1) {
                        Console::shared()
                        << "Socket error: " << static_cast<std::int32_t>(blah.ErrorNumber()) << myendl;
                    }
#endif
                    Disconnect(static_cast<uoxsocket_t>(i)); // abnormal error
                }
            }
        }
    }
    else if (s == -1) {
    }
}

CNetworkStuff::CNetworkStuff()
: peakConnectionCount(0) // Initialize sockets
{}
auto CNetworkStuff::startup() -> void {
    FD_ZERO(&conn);
    sockInit();
    LoadFirewallEntries();
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
        if (mSock->Buffer()[0] == 0x21 && count < 4) // UOMon
        {
            std::int32_t ho, mi, se, total;
            total = (cwmWorldState->GetUICurrentTime() - cwmWorldState->GetStartTime()) / 1000;
            ho = total / 3600;
            total -= ho * 3600;
            mi = total / 60;
            total -= mi * 60;
            se = total;
            
            auto uoxmonitor =
            util::format("UOX LoginServer\r\nUOX3 Server: up for %ih %im %is\r\n", ho, mi, se);
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
                    Console::shared() << "UOG Stats request completed. Disconnecting client. [" << static_cast<std::int32_t>(mSock->ClientIP4()) << "." << static_cast<std::int32_t>(mSock->ClientIP3()) << "." << static_cast<std::int32_t>(mSock->ClientIP2()) << "." << static_cast<std::int32_t>(mSock->ClientIP1()) << "]" << myendl;
                }
                else {
                    Console::shared()
                    << "Encrypted client attempting to cut in, disconnecting them: IP " << static_cast<std::int32_t>(mSock->ClientIP1()) << "." << static_cast<std::int32_t>(mSock->ClientIP2()) << "." << static_cast<std::int32_t>(mSock->ClientIP3()) << "." << static_cast<std::int32_t>(mSock->ClientIP4()) << myendl;
                }
                Disconnect(s);
                return;
            }
#if _DEBUG_PACKET
            Console::shared().print(util::format("Packet ID: 0x%x\n", packetId));
#endif
            if (packetId != 0x73 && packetId != 0xA4 && packetId != 0x80 && packetId != 0x91) {
                if (!mSock->ForceOffline()) // Don't refresh idle-timer if character is being forced
                    // offline
                {
                    mSock->IdleTimeout(
                                       cwmWorldState->ServerData()->BuildSystemTimeValue(tSERVER_LOGINTIMEOUT));
                }
            }
            
            bool doSwitch = true;
            if (ServerConfig::shared().enabled(ServerSwitch::OVERLOADPACKETS)) {
                std::map<std::uint16_t, std::uint16_t>::const_iterator pFind = packetOverloads.find(packetId);
                if (pFind != packetOverloads.end()) {
                    cScript *pScript = JSMapping->GetScript(pFind->second);
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
                    case 0x72: // Combat Mode
                    {
                        mSock->Receive(5);
                        
                        std::vector<std::uint16_t> scriptTriggers = ourChar->GetScriptTriggers();
                        for (auto scriptTrig : scriptTriggers) {
                            cScript *toExecute = JSMapping->GetScript(scriptTrig);
                            if (toExecute != nullptr) {
                                if (toExecute->OnWarModeToggle(ourChar) == 0) {
                                    return;
                                }
                            }
                        }
                        
                        // No individual scripts handling OnWarModeToggle returned true - let's check
                        // global script!
                        cScript *toExecute = JSMapping->GetScript(static_cast<std::uint16_t>(0));
                        if (toExecute != nullptr) {
                            if (toExecute->OnWarModeToggle(ourChar) == 0) {
                                return;
                            }
                        }
                        
                        ourChar->SetWar(buffer[1] != 0);
                        ourChar->SetPassive(buffer[1] ==
                                            0); // Set player as passive if they tab out of combat
                        
                        if (ourChar->GetTimer(tCHAR_TIMEOUT) <= cwmWorldState->GetUICurrentTime()) {
                            ourChar->SetTimer(tCHAR_TIMEOUT, BuildTimeValue(1));
                        }
                        else {
                            ourChar->SetTimer(tCHAR_TIMEOUT, ourChar->GetTimer(tCHAR_TIMEOUT) + 1000);
                        }
                        mSock->Send(buffer, 5);
                        mSock->FlushBuffer();
                        Movement->CombatWalk(ourChar);
                        Effects->DoSocketMusic(mSock);
                        ourChar->BreakConcentration(mSock);
                        break;
                    }
                    case 0x12: // Ext. Command
                        mSock->Receive(3);
                        mSock->Receive(mSock->GetWord(1));
                        ourChar->BreakConcentration(mSock);
                        if (buffer[3] == 0xC7) // Action
                        {
                            if (ourChar->IsOnHorse() || ourChar->IsFlying())
                                return; // can't bow or salute on horse or while flying
                            
                            if (std::string(reinterpret_cast<char *>(&buffer[4])) == "bow") {
                                if (ourChar->GetBodyType() == BT_GARGOYLE || (ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET) && (ourChar->GetSocket() == nullptr || ourChar->GetSocket()->clientType() >= ClientType::SA2D))) {
                                    // If gargoyle, human or elf, and new animation packet is enabled
                                    Effects->PlayNewCharacterAnimation(ourChar, N_ACT_EMOTE, S_ACT_EMOTE_BOW);
                                }
                                else if ((ourChar->GetBodyType() == BT_HUMAN || ourChar->GetBodyType() == BT_ELF) && !ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET)) {
                                    // If human or elf, and new animation packet is disabled
                                    Effects->PlayCharacterAnimation(ourChar, ACT_EMOTE_BOW, 0, 5); // 0x20
                                }
                                else {
                                    // If polymorphed to other creatures
                                    Effects->PlayCharacterAnimation(ourChar, 0x12, 0,  5); // Monster fidget 1
                                }
                            }
                            if (std::string(reinterpret_cast<char *>(&buffer[4])) == "salute") {
                                if (ourChar->GetBodyType() == BT_GARGOYLE || (ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET) && (ourChar->GetSocket() == nullptr || ourChar->GetSocket()->clientType() >= ClientType::SA2D))) {
                                    // If gargoyle, human or elf, and new animation packet is enabled
                                    Effects->PlayNewCharacterAnimation(ourChar, N_ACT_EMOTE, S_ACT_EMOTE_SALUTE);
                                }
                                else if ((ourChar->GetBodyType() == BT_HUMAN ||  ourChar->GetBodyType() == BT_ELF) && !ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET)) {
                                    // If human or elf, and new animation packet is disabled
                                    Effects->PlayCharacterAnimation(ourChar, ACT_EMOTE_SALUTE, 0,  5); // 0x21
                                }
                                else {
                                    // If polymorphed to other creatures
                                    Effects->PlayCharacterAnimation(ourChar, 0x11, 0, 5); // Monster fidget 2
                                }
                            }
                            break;
                        }
                        else if (buffer[3] == 0x58) {
                            DoorMacro(mSock);
                            break;
                        }
                        else if (buffer[3] == 0x24) // Skill
                        {
                            j = 4;
                            while (buffer[j] != ' ') {
                                ++j;
                            }
                            buffer[j] = 0;
                            Skills->SkillUse(
                                             mSock, static_cast<std::uint8_t>(std::stoul(std::string((char *)&buffer[4]))));
                            break;
                        }
                        else if (buffer[3] == 0x27 || buffer[3] == 0x56) // Spell
                        {
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
                                    if (Magic->CheckBook(((book - 1) / 8) + 1, (book - 1) % 8, sBook)) {
                                        if (ourChar->IsFrozen()) {
                                            if (ourChar->IsCasting()) {
                                                mSock->SysMessage(
                                                                  762); // You are already casting a spell.
                                            }
                                            else {
                                                mSock->SysMessage(
                                                                  763); // You cannot cast spells while frozen.
                                            }
                                        }
                                        else {
                                            mSock->CurrentSpellType(0);
                                            Magic->SelectSpell(mSock, book);
                                        }
                                    }
                                    else {
                                        mSock->SysMessage(764); // You do not have that spell.
                                    }
                                }
                                else {
                                    mSock->SysMessage(
                                                      765); // To cast spells, your spellbook must be in your hands or
                                    // in the first layer of your pack.
                                }
                            }
                        }
                        else {
                            if (buffer[2] == 0x05 && buffer[3] == 0x43) // Open spell book
                            {
                                Magic->SpellBook(mSock);
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
                        mSock->Receive(
                                       mSock->GetWord(1)); // it included the color changed to, but it doesn't!
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
                        
                        if (buffer[3] == 1) // Set profile, not feasible yet
                        {
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
                        mSock->Receive(
                                       6); // it's always 6 so Westy tells me... let's not be intelligent =)
                        std::uint16_t skillNum;
                        skillNum = mSock->GetWord(3);
                        ourChar->SetSkillLock(static_cast<skilllock_t>(buffer[5]),
                                              static_cast<std::uint8_t>(skillNum));
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
                    default:
                        FD_ZERO(&all);
                        FD_SET(mSock->CliSocket(), &all);
                        std::int32_t nfds;
                        nfds = static_cast<int>(mSock->CliSocket()) + 1;
                        if (select(nfds, &all, nullptr, nullptr, &cwmWorldState->uoxTimeout) > 0) {
                            mSock->Receive(MAXBUFFER);
                        }
                        
                        Console::shared()
                        << util::format("Unknown message from client: 0x%02X - Ignored", packetId)
                        << myendl;
                        break;
                }
            }
        }
        else // Sortta error type thing, they disconnected already
        {
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
    // fd_set all; // This is already defined globally?
    // fd_set errsock; // This is already defined globally?
    size_t i;
    
    // cwmWorldState->uoxTimeout.tv_sec = 0;
    // cwmWorldState->uoxTimeout.tv_usec = 1; // This causes Windows to wait 1 extra milliseconds
    // per loop, while Linux waits 1 microseconds, causing performance difference
    
    FD_ZERO(&all);
    FD_ZERO(&errsock);
    
    std::int32_t nfds = 0;
    for (auto &tSock : loggedInClients) {
        auto clientSock = static_cast<uoxsocket_t>(tSock->CliSocket());
        FD_SET(clientSock, &all);
        FD_SET(clientSock, &errsock);
        if (static_cast<int>(clientSock) + 1 > nfds) {
            nfds = clientSock + 1;
        }
    }
    
    std::int32_t s = select(nfds, &all, nullptr, &errsock, &cwmWorldState->uoxTimeout);
    if (s > 0) {
        size_t oldnow = loggedInClients.size();
        for (i = 0; i < loggedInClients.size(); ++i) {
            if (FD_ISSET(loggedInClients[i]->CliSocket(), &errsock)) {
                LoginDisconnect(static_cast<uoxsocket_t>(i));
                continue;
            }
            if ((FD_ISSET(loggedInClients[i]->CliSocket(), &all)) &&
                (oldnow == loggedInClients.size())) {
                try {
                    GetLoginMsg(static_cast<uoxsocket_t>(i));
                } catch (socket_error &blah) {
#if !defined(_WIN32)
                    messageLoop << "Client disconnected";
#else
                    if (blah.ErrorNumber() == WSAECONNRESET) {
                        messageLoop << "Client disconnected";
                    }
                    else if (blah.ErrorNumber() != -1) {
                        messageLoop << util::format("Socket error: %i", blah.ErrorNumber());
                    }
#endif
                    LoginDisconnect(static_cast<uoxsocket_t>(i)); // abnormal error
                }
            }
        }
    }
    else if (s == -1) {
#if defined(_WIN32)
        std::int32_t errorCode = WSAGetLastError();
        if (errorCode != 10022) {
            Console::shared() << static_cast<std::int32_t>(errorCode) << myendl;
        }
#endif
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

void CNetworkStuff::LoginDisconnect(CSocket *s) // Force disconnection of player //Instalog
{
    uoxsocket_t i = FindLoginPtr(s);
    LoginDisconnect(i);
}

void CNetworkStuff::Disconnect(CSocket *s) // Force disconnection of player //Instalog
{
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
    cwmWorldState->SetPlayersOnline(connClients.size());
    if (cwmWorldState->GetPlayersOnline() > peakConnectionCount) {
        peakConnectionCount = cwmWorldState->GetPlayersOnline();
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
                cScript *pScript = JSMapping->GetScript(pFind->second);
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
            if (mSock->FirstPacket() && packetId != 0x80 && packetId != 0x91 && packetId != 0xE4 &&
                packetId != 0xF1) {
                // April 5, 2004 - Hmmm there are two of these ?
                if (ServerConfig::shared().enabled(ServerSwitch::UOG)) {
                    messageLoop << util::format( "UOG Stats Sent or Encrypted client detected. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1());
                }
                else {
                    messageLoop << util::format("Encrypted client detected. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1());
                }
                LoginDisconnect(s);
                return;
            }
            else if (mSock->FirstPacket() && packetId == 0) {
                messageLoop << util::format("Buffer is empty, no packets to read. Disconnecting client. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1());
                LoginDisconnect(s);
                return;
            }
            bool doSwitch = true;
            if (ServerConfig::shared().enabled(ServerSwitch::OVERLOADPACKETS)) {
                std::map<std::uint16_t, std::uint16_t>::const_iterator pFind = packetOverloads.find(packetId);
                if (pFind != packetOverloads.end()) {
                    cScript *pScript = JSMapping->GetScript(pFind->second);
                    if (pScript != nullptr) {
                        doSwitch = !pScript->OnPacketReceive(mSock, packetId);
                    }
                }
            }
            if (doSwitch) {
                CPInputBuffer *test = nullptr;
                try {
                    test = WhichLoginPacket(packetId, mSock);
                } catch (socket_error &) {
                    Console::shared().warning(util::format("Bad packet request thrown! [packet ID: 0x%x]", packetId));
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
                    default:
                        std::int32_t nfds;
                        fd_set all;
                        FD_ZERO(&all);
                        FD_SET(mSock->CliSocket(), &all);
                        nfds = static_cast<uoxsocket_t>(mSock->CliSocket()) + 1;
                        if (select(nfds, &all, nullptr, nullptr, &cwmWorldState->uoxTimeout) > 0) {
                            mSock->Receive(MAXBUFFER);
                        }
                        messageLoop << util::format("Unknown message from client: 0x%02X - Ignored", packetId);
                        break;
                }
            }
        }
        else // Sortta error type thing, they disconnected already
        {
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
    std::string token;
    auto fileToUse = std::filesystem::path("banlist.ini");
    if (!std::filesystem::exists(fileToUse)) {
        fileToUse = std::filesystem::path("firewall.ini");
        if (!std::filesystem::exists(fileToUse)) {
            fileToUse =  std::filesystem::path();
        }
    }
    if (!fileToUse.empty()) {
        auto firewallData = std::make_unique<Script>(fileToUse, NUM_DEFS, false);
        if (firewallData) {
            std::int16_t p[4];
            for (const auto &[secName, firewallSect] : firewallData->collection()) {
                if (firewallSect) {
                    for (const auto &sec : firewallSect->collection()) {
                        auto tag = sec->tag;
                        auto data = sec->data;
                        if (util::upper(tag) == "IP") {
                            data = util::trim(util::strip(data, "//"));
                            if (!data.empty()) {
                                auto psecs = oldstrutil::sections(data, ".");
                                if (psecs.size() == 4) { // Wellformed IP address
                                    for (std::uint8_t i = 0; i < 4; ++i) {
                                        token = psecs[i];
                                        if (token == "*") {
                                            p[i] = -1;
                                        }
                                        else {
                                            p[i] = static_cast<std::int16_t>(std::stoi(token, nullptr, 0));
                                        }
                                    }
                                    slEntries.push_back(FirewallEntry_st(p[0], p[1], p[2], p[3]));
                                }
                                else if (data != "\n" && data != "\r") {
                                    Console::shared().error(util::format( "Malformed IP address in banlist.ini (line: %s)", data.c_str()));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
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
