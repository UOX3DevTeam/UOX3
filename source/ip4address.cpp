//
// Created on:  6/8/21

#include "ip4address.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#if defined(_WIN32)
// clang-format off
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stringapiset.h>
// clang-format on

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
constexpr auto WORKING_BUFFER_SIZE = 15000;
constexpr auto MAX_TRIES = 3;

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#else

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
using namespace std::string_literals;

// o------------------------------------------------------------------------------------------------o
//|	string manipulation, in case strutil is not available. Enables this to be standalone
// o------------------------------------------------------------------------------------------------o
auto trim(const std::string &value) -> std::string {
    auto rValue = std::string();
    auto startpos = value.find_first_not_of(" \t\r\n\f");
    if (startpos != std::string::npos) {
        rValue = value.substr(startpos);
        auto endpos = rValue.find_last_not_of(" \t\r\n\f");
        if (endpos != std::string::npos) {
            rValue = rValue.substr(0, endpos + 1);
        }
    }
    return rValue;
}
//==================================================================================================
auto parse(const std::string &value, const std::string &separator = ".")
    -> std::vector<std::string> {
    auto rValue = std::vector<std::string>();
    auto subject = trim(value);
    auto position = subject.find(separator);
    if (position == std::string::npos) {
        // It coulnd't find the separator
        rValue.push_back(subject);
    }
    else {
        while (position != std::string::npos) {
            auto parsed = trim(subject.substr(0, position));
            rValue.push_back(parsed);
            subject = subject.substr(position + separator.size());
            position = subject.find(separator);
        }
        subject = trim(subject);
        rValue.push_back(subject);
    }
    return rValue;
}
//==================================================================================================
auto strip(const std::string &value, const std::string &identifier = "//") -> std::string {
    auto position = value.find(identifier);
    return value.substr(0, position);
}
//==================================================================================================
auto split(const std::string &value, const std::string &identifier = "=")
    -> std::pair<std::string, std::string> {
    auto position = value.find(identifier);
    auto pos1 = trim(value.substr(0, position));
    auto pos2 = trim(value.substr(position + identifier.size()));
    return std::make_pair(pos1, pos2);
}

// o------------------------------------------------------------------------------------------------o
//|	IP4Addr
// o------------------------------------------------------------------------------------------------o
const std::vector<IP4Addr> IP4Addr::lanips{
    IP4Addr("10.*.*.*"s),   IP4Addr("192.168.*.*"s), IP4Addr("172.16.*.*"s),
    IP4Addr("172.17.*.*"s), IP4Addr("172.18.*.*"s),  IP4Addr("172.19.*.*"s),
    IP4Addr("172.20.*.*"s), IP4Addr("172.21.*.*"s),  IP4Addr("172.22.*.*"s),
    IP4Addr("172.23.*.*"s), IP4Addr("172.241.*.*"s), IP4Addr("172.25.*.*"s),
    IP4Addr("172.26.*.*"s), IP4Addr("172.27.*.*"s),  IP4Addr("172.28.*.*"s),
    IP4Addr("172.29.*.*"s), IP4Addr("172.30.*.*"s),  IP4Addr("172.31.*.*"s)};
//==================================================================================================
const std::vector<IP4Addr> IP4Addr::localips{IP4Addr("127.*.*.*")};
//==================================================================================================
const std::vector<IP4Addr> IP4Addr::apipaips{IP4Addr("169.254.*.*")};

auto IP4Addr::exact(const IP4Addr &value) const -> bool {
    auto rValue = true;
    for (auto i = 0; i < 4; ++i) {
        if (components[i] != value.components[i]) {
            rValue = false;
            break;
        }
    }
    return rValue;
}

//==================================================================================================
auto IP4Addr::match(std::uint32_t value, bool bigendian) const -> int {
    auto ptr = reinterpret_cast<std::uint8_t *>(&value);
    if (!bigendian) {
        std::reverse(ptr, ptr + 4);
    }
    auto match = 0;
    for (const auto &comp : components) {
        match += 1;
        if ((comp != "*") && !comp.empty()) {
            try {
                if (std::stoi(comp) != *ptr) {
                    match -= 1;
                    break;
                }
            } catch (...) {
                // the value couldn't be converted
                match -= 1;
                break;
            }
        }
    }
    return match;
}
//==================================================================================================
auto IP4Addr::match(const IP4Addr &value) const -> int {
    auto rValue = 0;
    for (auto i = 0; i < 4; ++i) {
        rValue = i + 1;
        if (!(((value.components[i] == "*") || value.components[i].empty()) ||
              ((components[i] == "*") || components[i].empty()))) {
            // Neither are wild, we have to match them
            if (value.components[i] != components[i]) {
                rValue = rValue - 1;
                break;
            }
        }
    }
    return rValue;
}
//==================================================================================================
IP4Addr::IP4Addr(const std::string &value) {
    components.fill("*"s);
    auto values = parse(value);
    switch (value.size()) {
    default:
    case 4:
        components[3] = values[3];
        [[fallthrough]];
    case 3:
        components[2] = values[2];
        [[fallthrough]];
    case 2:
        components[1] = values[1];
        [[fallthrough]];
    case 1:
        components[0] = values[0];
        [[fallthrough]];
    case 0:
        break;
    }
}
//==================================================================================================
IP4Addr::IP4Addr(std::uint32_t addr, bool bigendian) {
    components.fill("*"s);
    auto ptr = reinterpret_cast<std::uint8_t *>(&addr);
    if (!bigendian) {
        std::reverse(ptr, ptr + 4);
    }
    for (auto i = 0; i < 4; ++i) {
        components[i] = std::to_string(ptr[i]);
    }
}
//==================================================================================================
auto IP4Addr::ipaddr(bool bigendian) const -> std::uint32_t {
    auto rValue = std::uint32_t(0);
    auto ptr = reinterpret_cast<std::uint8_t *>(&rValue);
    for (auto i = 0; i < 4; ++i) {
        auto value = std::uint8_t(0);
        try {
            value = static_cast<std::uint8_t>(std::stoi(components[i]));
        } catch (...) {
            throw std::runtime_error("Error converting ip to a number"s);
        }
        ptr[i] = value;
    }
    if (!bigendian) {
        std::reverse(ptr, ptr + 4);
    }
    return rValue;
}
//==================================================================================================
// This assumes the value is in big endian
auto IP4Addr::operator==(std::uint32_t value) const -> bool { return match(value) == 4; }
//==================================================================================================
auto IP4Addr::operator==(const IP4Addr &value) const -> bool { return match(value) == 4; }
//==================================================================================================
auto IP4Addr::type() const -> ip4type_t {
    auto rValue = ip4type_t::wan;
    try {
        auto iter = std::find_if(lanips.begin(), lanips.end(),
                                 [this](const IP4Addr &ip) { return *this == ip; });
        if (iter != lanips.end()) {
            rValue = ip4type_t::lan;
        }
        else {
            auto iter = std::find_if(localips.begin(), localips.end(),
                                     [this](const IP4Addr &ip) { return *this == ip; });
            if (iter != localips.end()) {
                rValue = ip4type_t::local;
            }
            else {
                auto iter = std::find_if(apipaips.begin(), apipaips.end(),
                                         [this](const IP4Addr &ip) { return *this == ip; });
                if (iter != apipaips.end()) {
                    rValue = ip4type_t::apipa;
                }
            }
        }
    } catch (...) {
        rValue = ip4type_t::invalid;
    }
    return rValue;
}
//==================================================================================================
auto IP4Addr::description() const -> std::string {
    auto ip = std::string();
    for (auto &value : components) {
        ip += value + "."s;
    }
    return ip.substr(0, ip.size() - 1);
}

// o------------------------------------------------------------------------------------------------o
//|	ip4list_t
// o------------------------------------------------------------------------------------------------o

//==================================================================================================
ip4list_t::ip4list_t(const std::string &filename) {
    if (!filename.empty()) {
        load(filename);
    }
}

//==================================================================================================
auto ip4list_t::bestmatch(const IP4Addr &value) const -> std::pair<IP4Addr, int> {
    auto matches = std::vector<std::pair<IP4Addr, int>>();
    for (const auto &addr : ipaddresses) {
        auto comp = addr.match(value);
        matches.push_back(std::make_pair(addr, comp));
    }
    std::sort(matches.begin(), matches.end(), [](const std::pair<IP4Addr, int> &lhs, const std::pair<IP4Addr, int> &rhs) {
                  return lhs.second < rhs.second;
              });
    return *matches.rbegin();
}
//==================================================================================================
auto ip4list_t::bestmatch(std::uint32_t value, bool bigendian) const -> std::pair<IP4Addr, int> {
    auto ptr = reinterpret_cast<std::uint8_t *>(&value);
    if (!bigendian) {
        std::reverse(ptr, ptr + 4);
    }
    auto matches = std::vector<std::pair<IP4Addr, int>>();
    for (const auto &addr : ipaddresses) {
        auto comp = addr.match(value);
        matches.push_back(std::make_pair(addr, comp));
    }
    std::sort(matches.begin(), matches.end(),  [](const std::pair<IP4Addr, int> &lhs, const std::pair<IP4Addr, int> &rhs) {
                  return lhs.second < rhs.second;
              });
    return *matches.rbegin();
}
//==================================================================================================
auto ip4list_t::add(const IP4Addr &value) -> void { ipaddresses.push_back(value); }
//==================================================================================================
auto ip4list_t::remove(const IP4Addr &value) -> void {
    auto iter = std::find_if(ipaddresses.begin(), ipaddresses.end(), [value](const IP4Addr &entry) { return value.exact(entry); });
    if (iter != ipaddresses.end()) {
        ipaddresses.erase(iter);
    }
}
//==================================================================================================
auto ip4list_t::size() const -> size_t { return ipaddresses.size(); }
//==================================================================================================
auto ip4list_t::load(const std::string &filename) -> bool {
    auto rValue = false;
    enum state_t { section, startsection, data };
    ipaddresses.clear();
    auto input = std::ifstream(filename);
    if (input.is_open()) {
        rValue = true;
        char inputline[4096];
        auto state = state_t::section;
        while (input.good() && !input.eof()) {
            input.getline(inputline, 4095);
            if (input.gcount() > 0) {
                inputline[input.gcount()] = 0;
                auto line = trim(strip(std::string(inputline)));
                if (!line.empty()) {
                    // look for a section
                    switch (static_cast<int>(state)) {
                    case static_cast<int>(state_t::section): {
                        if (line[0] == '[') {
                            if (line[line.size() - 1] == ']') {
                                // it is a section!
                                line = trim(line.substr(1, line.find("]") - 1));
                                // apparently we dont look at this?
                                state = state_t::startsection;
                            }
                        }
                        break;
                    }

                    case static_cast<int>(state_t::startsection): {
                        if (line[0] == '{') {
                            state = state_t::data;
                        }
                        break;
                    }
                    case static_cast<int>(state_t::data): {
                        if (line[0] != '}') {
                            auto [key, value] = split(line, "=");
                            if ((key == "ip") || (key == "IP") || (key == "Ip") || (key == "iP")) {
                                ipaddresses.push_back(IP4Addr(value));
                            }
                        }
                        else {
                            state = state_t::section;
                        }
                    }
                    }
                }
            }
        }
    }
    return rValue;
}

//==================================================================================================
auto ip4list_t::ips() const -> const std::vector<IP4Addr> & { return ipaddresses; }
//==================================================================================================
auto ip4list_t::ips() -> std::vector<IP4Addr> & { return ipaddresses; }

// Unfortunately, the approach here for the unix/windows is almost totally
// different, so effectively, two completely different routines
#if defined(_WIN32)
//==================================================================================================
auto ip4list_t::available() -> ip4list_t {
    /* Note: could also use malloc() and free() */

    ip4list_t rValue;
    std::string device;
    IP4Addr device_address;

    /* Declare and initialize variables */
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_INET;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    IP_ADAPTER_PREFIX *pPrefix = NULL;

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
        if (pAddresses == nullptr) {
            throw std::runtime_error("Memory allocation files for IP_ADAPTER_ADDRESSES");
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        }
        else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != nullptr) {
                if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                    for (i = 0; pUnicast != nullptr; i++) {
                        const int friendlen = 200;

                        char friendly[friendlen];
                        std::memset(friendly, 0, friendlen);
                        sockaddr_in *sa_in = (sockaddr_in *)pUnicast->Address.lpSockaddr;
                        device_address = IP4Addr(sa_in->sin_addr.S_un.S_addr);

                        if (device_address.type() != IP4Addr::ip4type_t::apipa) {
                            // ourdevice.address = inet_ntop( AF_INET, &( sa_in->sin_addr ), buff,
                            // bufflen );
                            BOOL conv = false;
                            device = "";
                            if (WideCharToMultiByte(CP_UTF8, 0, pCurrAddresses->FriendlyName, -1,
                                                    friendly, friendlen, 0, &conv) > 0) {
                                device = friendly;
                            }
                            if (!device.empty()) {
                                // The device has a name, might be intersted
                                if (device.find("(WSL)") == std::string::npos) {
                                    // we dont want a psuedo WSL device on windows
                                    rValue.add(device_address);
                                }
                            }
                        }
                        pUnicast = pUnicast->Next;
                    }
                }
            }

            pCurrAddresses = pCurrAddresses->Next;
        }
    }
    else {
        if (dwRetVal != ERROR_NO_DATA) {
            if (pAddresses) {
                FREE(pAddresses);
            }

            throw std::runtime_error("Unable to get address info");
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }
    return rValue;
}
#else
auto ip4list_t::available() -> ip4list_t {
    ip4list_t rValue;
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    IP4Addr device_address;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) // check it is IP4
        {
            // is a valid IP4 Address
            auto holder = *reinterpret_cast<sockaddr_in *>(ifa->ifa_addr);
            auto addr = IP4Addr(holder.sin_addr.s_addr);
            if (addr.type() != IP4Addr::ip4type_t::apipa) {
                rValue.add(addr);
            }
        }
    }
    if (ifAddrStruct != NULL) {
        freeifaddrs(ifAddrStruct);
    }
    return rValue;
}

#endif
