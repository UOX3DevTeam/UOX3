//Copyright © 2023 Charles Kerr. All rights reserved.

#include "netutil.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>

#if defined (_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stringapiset.h>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#define MALLOC( x ) HeapAlloc( GetProcessHeap(), 0, ( x ))
#define FREE( x ) HeapFree( GetProcessHeap(), 0, ( x ))

#else

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#if !defined(__linux__) && !defined(_WIN32)
#include <sys/event.h>
#include <time.h>
#endif
#if defined(__linux__)
#include <sys/epoll.h>
#endif

#include "neterror.hpp"
#include "ip4util.hpp"
#include "strutil.hpp"

using namespace std::string_literals ;

//======================================================================
namespace util {
    namespace net {
        // APIPA ip is form 169.254.*.*
        static constexpr  auto  apipaIP4 = 0xA9FE ;
        static constexpr auto loopback = 0x7F000001;
        
#if defined(_WIN32)
        static auto numberOfWSAStartups = 0 ;
#endif
        
        //=================================================================================
        // Intialize the network
        // For windows, this is initialize winsock2.
        // For everythig else, nothing
        auto startup() ->void {
#if defined(_WIN32)
            WSADATA wsa_data ;
            auto results = WSAStartup(MAKEWORD(2,2),&wsa_data);
            if (results != 0){
                results = WSAGetLastError();
                auto msg = errormsg(results);
                throw std::runtime_error("Error initializing windows network: "s + msg);
            }
            if ( LOBYTE(wsa_data.wVersion ) != 2 ||
                HIBYTE(wsa_data.wVersion ) != 2 ) {
                WSACleanup();
                throw std::runtime_error("Unable to obtain minimum WinSock version of 2.2 "s);
            }
            numberOfWSAStartups++ ;
            
#endif
            
        }
        //=================================================================================
        // shutdown the network
        // For windows, this is initialize winsock2.
        // For everythig else, nothing
        auto shutdown(bool complain) ->void {
#if defined(_WIN32)
            if (numberOfWSAStartups > 0){
                WSACleanup();
                numberOfWSAStartups-- ;
            }
            else {
                if (complain){
                    throw std::runtime_error("Requested shutdown exceeded startup requests"s);
                }
            }
#endif
            
        }
        //====================================================================
        // Get the text for error numbers
        auto errormsg(int error) -> std::string{
            auto value = std::string();
#if !defined(_WIN32)
            value = std::strerror(error);
#else
            LPTSTR  buffer = NULL;
            error = WSAGetLastError();
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&buffer), 0, NULL);
            if (buffer != NULL) {
#if defined(UNICODE)
                std::vector<char> tempBuffer;
                int size = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, NULL, 0, NULL, NULL);
                if (size > 0) {
                    tempBuffer.resize(size+1,0);
                    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, tempBuffer.data(), static_cast<int>(tempBuffer.size()), NULL, NULL);
                }
                value = tempBuffer.data();
                
#else
                value = *buffer;
#endif
                LocalFree(buffer);
            }
#endif
            return value;
            
        }
        
        //==================================================================================================================
        auto closeSocket(sockfd_t descrip) -> void {
#if defined(_WIN32)
            closesocket(descrip);
#else
            ::close(descrip) ;
#endif
        }
        //=================================================================================
        // Peer information
        auto peerInformation(sockfd_t descriptor) -> std::pair<std::string,std::string> {
            if (descriptor == BADSOCKET){
                throw SocketClose("Can not retrieve peer information, socket is closed");
            }
            struct sockaddr_in their_addr;
            socklen_t size = sizeof their_addr ;
            auto status = getpeername(descriptor,reinterpret_cast<sockaddr*>(&their_addr),&size) ;
            if (status == SOCKETERROR){
                auto error = 0 ;
#if defined(_WIN32)
                error = WSAGetLastError() ;
#else
                error = errno ;
#endif
                throw SocketError("Error getting peer information: "s+errormsg(error));
            }
            auto ip = ntohl(their_addr.sin_addr.s_addr) ;
            auto port = ntohs(their_addr.sin_port);
            return std::make_pair(IP4Entry::describeIP(ip), std::to_string(port));
        }
        
        //===================================================================
        auto availIP4() -> std::vector<ip4_t> {
            
            auto rvalue = std::vector<ip4_t>() ;
            
#if !defined(_WIN32)
            struct ifaddrs * ifAddrStruct = nullptr;
            struct ifaddrs * ifa = nullptr;
            void * tmpAddrPtr = nullptr;
            
            auto error = getifaddrs(&ifAddrStruct) ;
            if (error == 0){
                for( ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next ){
                    if( !ifa->ifa_addr ){
                        continue;
                    }
                    // check it is IP4
                    if( ifa->ifa_addr->sa_family == AF_INET ){
                        // is a valid IP4 Address
                        tmpAddrPtr = &(( struct sockaddr_in * )ifa->ifa_addr )->sin_addr;
                        auto holder = *reinterpret_cast<sockaddr_in *>( ifa->ifa_addr );
                        auto addr = ntohl( holder.sin_addr.s_addr );
                        //std::cout <<"IP4: " << ((addr&0xFF000000)>>24)<<"."<< ((addr&0xFF0000)>>16)<<"." << ((addr&0xFF00)>>8)<<"."<<(addr&0xFF) <<std::endl;
                        if( (((addr & 0xFFFF0000)>>16) != apipaIP4)  && (addr != loopback)){
                            //std::cout <<"IP4: " << ((addr&0xFF000000)>>24)<<"."<< ((addr&0xFF0000)>>16)<<"." << ((addr&0xFF00)>>8)<<"."<<(addr&0xFF) <<std::endl;
                            rvalue.push_back( static_cast<ip4_t>(addr) );
                        }
                    }
                }
                if( ifAddrStruct != NULL ){
                    freeifaddrs( ifAddrStruct );
                }
            }
#else
            constexpr auto WORKING_BUFFER_SIZE = 15000;
            constexpr auto MAX_TRIES = 3;
            
            
            /* Note: could also use malloc() and free() */
            
            std::string device;
            
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
            ULONG iterations = 0;
            
            PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
            PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
            PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
            PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
            IP_ADAPTER_DNS_SERVER_ADDRESS* pDnServer = NULL;
            IP_ADAPTER_PREFIX* pPrefix = NULL;
            
            // Allocate a 15 KB buffer to start with.
            outBufLen = WORKING_BUFFER_SIZE;
            
            do {
                pAddresses = (IP_ADAPTER_ADDRESSES*)MALLOC(outBufLen);
                if (pAddresses == nullptr){
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
                
                iterations++;
                
            } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (iterations < MAX_TRIES));
            
            if (dwRetVal == NO_ERROR) {
                // If successful, output some information from the data we received
                pCurrAddresses = pAddresses;
                while (pCurrAddresses) {
                    pUnicast = pCurrAddresses->FirstUnicastAddress;
                    if (pUnicast != nullptr){
                        if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                            for (i = 0; pUnicast != nullptr; i++) {
                                const int friendlen = 200;
                                
                                char friendly[friendlen];
                                std::memset(friendly, 0, friendlen);
                                sockaddr_in* sa_in = (sockaddr_in*)pUnicast->Address.lpSockaddr;
                                auto addr = ntohl(sa_in->sin_addr.S_un.S_addr);
                                
                                if ((((addr & 0xFFFF0000) >> 16) != apipaIP4) && (addr != loopback)) {
                                    BOOL conv = false;
                                    device = "";
                                    if (WideCharToMultiByte(CP_UTF8, 0, pCurrAddresses->FriendlyName, -1, friendly, friendlen, 0, &conv) > 0) {
                                        device = friendly;
                                    }
                                    if (!device.empty()) {
                                        // The device has a name, might be intersted
                                        if (device.find("(WSL)") == std::string::npos) {
                                            // we dont want a psuedo WSL device on windows
                                            rvalue.push_back(static_cast<ip4_t>(addr));
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
#endif
            
            return rvalue ;
        }
        
        
    }
}
