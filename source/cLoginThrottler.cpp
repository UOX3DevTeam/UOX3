#include "uox3.h"
#include "cLoginThrottler.h"
#include "cSocket.h"

#include <algorithm>

cLoginThrottler LoginThrottler;

UI32 cLoginThrottler::AddressKey( CSocket *socket ) const
{
	return CalcSerial( socket->ClientIP1(), socket->ClientIP2(), socket->ClientIP3(), socket->ClientIP4() );
}

bool cLoginThrottler::IsBlocked( CSocket *socket, const std::string &username, UI32 &retryAfterSeconds )
{
	retryAfterSeconds = 0;
	CServerData *serverData = cwmWorldState->ServerData();
	if( socket == nullptr || !serverData->LoginThrottleEnabled() )
	{
		return false;
	}

	const auto now = std::chrono::steady_clock::now();
	std::lock_guard<std::mutex> guard( entriesMutex );
	auto entry = entries.find( AddressKey( socket ));
	if( entry == entries.end() || entry->second.blockedUntil <= now )
	{
		return false;
	}
	if( !entry->second.oneTimeBypassUsername.empty() && entry->second.oneTimeBypassUsername == username )
	{
		entry->second.oneTimeBypassUsername.clear();
		return false;
	}

	retryAfterSeconds = static_cast<UI32>( std::chrono::duration_cast<std::chrono::seconds>( entry->second.blockedUntil - now ).count() ) + 1;
	return true;
}

void cLoginThrottler::RecordFailure( CSocket *socket )

{
	RecordAttempt( socket, "" );
}

void cLoginThrottler::RecordAccountCreation( CSocket *socket, const std::string &username )
{
	RecordAttempt( socket, username );
}

void cLoginThrottler::RecordAttempt( CSocket *socket, const std::string &oneTimeBypassUsername )
{
	CServerData *serverData = cwmWorldState->ServerData();
	if( socket == nullptr || !serverData->LoginThrottleEnabled() )
	{
		return;
	}

	const UI32 maxAttempts = std::max<UI32>( 1, serverData->LoginThrottleMaxAttempts() );
	const UI32 windowSeconds = std::max<UI32>( 1, serverData->LoginThrottleWindow() );
	const UI32 initialDelay = std::max<UI32>( 1, serverData->LoginThrottleInitialDelay() );
	const UI32 multiplier = std::max<UI32>( 1, serverData->LoginThrottleMultiplier() );
	const UI32 maxDelay = std::max<UI32>( initialDelay, serverData->LoginThrottleMaxDelay() );
	const UI32 entryTtl = std::max<UI32>( windowSeconds, serverData->LoginThrottleEntryTtl() );
	const auto now = std::chrono::steady_clock::now();
	UI32 delaySeconds = 0;

	{
		std::lock_guard<std::mutex> guard( entriesMutex );
		LoginThrottleEntry &entry = entries[AddressKey( socket )];
		if( entry.failureCount == 0 || now - entry.windowStarted >= std::chrono::seconds( windowSeconds ))
		{
			entry.failureCount = 0;
			entry.windowStarted = now;
		}

		entry.lastAccess = now;
		entry.oneTimeBypassUsername = oneTimeBypassUsername;
		++entry.failureCount;
		if( entry.failureCount >= maxAttempts )
		{
			delaySeconds = initialDelay;
			for( UI32 attempt = maxAttempts; attempt < entry.failureCount && delaySeconds < maxDelay; ++attempt )
			{
				if( multiplier > 1 && delaySeconds > maxDelay / multiplier )
				{
					delaySeconds = maxDelay;
					break;
				}
				delaySeconds = std::min( maxDelay, delaySeconds * multiplier );
			}
			entry.blockedUntil = now + std::chrono::seconds( delaySeconds );
		}

		if( ++operationsSinceCleanup >= 256 )
		{
			CleanupLocked( now, entryTtl );
			operationsSinceCleanup = 0;
		}
	}

	if( delaySeconds > 0 )
	{
		Console.Log( oldstrutil::format( "Login throttle activated for [%i.%i.%i.%i] for %u seconds.", socket->ClientIP4(), socket->ClientIP3(), socket->ClientIP2(), socket->ClientIP1(), delaySeconds ), "accounts.log" );
	}
}

void cLoginThrottler::CleanupLocked( const std::chrono::steady_clock::time_point &now, UI32 entryTtl )
{
	// Caller must hold entriesMutex while entries and operationsSinceCleanup are accessed.
	for( auto entry = entries.begin(); entry != entries.end(); )
	{
		if( entry->second.blockedUntil <= now && now - entry->second.lastAccess >= std::chrono::seconds( entryTtl ))
		{
			entry = entries.erase( entry );
		}
		else
		{
			++entry;
		}
	}
}
