#ifndef __CLOGINTHROTTLER_H
#define __CLOGINTHROTTLER_H

#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

class CSocket;

struct LoginThrottleEntry
{
	UI32 failureCount = 0;
	std::chrono::steady_clock::time_point windowStarted;
	std::chrono::steady_clock::time_point lastAccess;
	std::chrono::steady_clock::time_point blockedUntil;
	std::string oneTimeBypassUsername;
};

class cLoginThrottler
{
public:
	bool IsBlocked( CSocket *socket, const std::string &username, UI32 &retryAfterSeconds );
	void RecordFailure( CSocket *socket, const std::string &username );
	void RecordAccountCreation( CSocket *socket, const std::string &username );

private:
	UI32 AddressKey( CSocket *socket ) const;
	void RecordAttempt( CSocket *socket, const std::string &username, const std::string &oneTimeBypassUsername );
	void CleanupLocked( const std::chrono::steady_clock::time_point &now, UI32 entryTtl );

	std::unordered_map<UI32, LoginThrottleEntry> entries;
	std::mutex entriesMutex;
	UI32 operationsSinceCleanup = 0;
};

extern cLoginThrottler LoginThrottler;

#endif
