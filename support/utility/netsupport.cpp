//Copyright © 2022 Charles Kerr. All rights reserved.

#include "netsupport.hpp"
#include "netexcept.hpp"

#include <iostream>
#if defined (_WIN32)
// Windows specific
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif
using namespace std::string_literals;
//=========================================================

//=========================================================
// auto startNetwork()->void
//=========================================================
//===================================================================
auto startNetwork()->void {
	// Mainly for windows, as we dont need to do anything for the unixes
	// Some like to disable sigpipe on the unixes, but we have taken care of that
	// on the options/flags
#if defined(_WIN32)
	WORD wVersionRequested;
	WSADATA wsaData;
	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);
	
	auto err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		throw socket_exp(err, "WSAStartup failed!"s);
	}
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup();
		throw socket_exp(0, "Could not find a WINSOCK 2.2!"s);
	}
	
#endif
	
}
//=========================================================
// auto stopNetwork()->void
//=========================================================
//===================================================================
auto stopNetwork()->void {
#if defined(_WIN32)
	WSACleanup();
#endif
}

