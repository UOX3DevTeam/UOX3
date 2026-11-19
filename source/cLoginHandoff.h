#ifndef __CLOGINHANDOFF_H
#define __CLOGINHANDOFF_H

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

class CSocket;

struct LoginHandoffTicket
{
	std::uint32_t token = 0;
	std::uint32_t clientIp = 0;
	std::uint32_t clientVersion = 0;
	std::uint32_t clientType = 0;
	std::uint32_t clientVersionShort = 0;
	std::uint64_t expires = 0;
	std::uint16_t shardId = 0;
	std::uint16_t accountFlags = 0;
	bool localAdmin = false;
	std::string username;
};

class cLoginHandoff
{
public:
	static constexpr std::uint32_t MAGIC = 0x554F5848; // "UOXH"
	static constexpr std::size_t PACKET_SIZE = 101;

	auto CreateTicket( const std::string &username, std::uint32_t clientIp, std::uint32_t clientVersion,
		std::uint32_t clientType, std::uint32_t clientVersionShort, std::uint16_t shardId,
		std::uint16_t accountFlags, std::uint16_t lifetimeSeconds, LoginHandoffTicket &ticket ) -> bool;
	auto CreateLocalAdminTicket( const std::string &username, std::uint32_t clientIp,
		std::uint32_t clientVersion, std::uint32_t clientType, std::uint32_t clientVersionShort,
		std::uint16_t shardId, std::uint16_t lifetimeSeconds, LoginHandoffTicket &ticket ) -> bool;
	auto RegisterWithGame( const std::string &host, std::uint16_t port, const LoginHandoffTicket &ticket,
		const std::string &secret, std::string &error ) -> bool;
	auto AcceptRegistration( CSocket *socket, const std::string &secret, std::uint16_t expectedShardId,
		std::uint16_t maximumLifetime, std::string &error ) -> bool;
	auto Consume( std::uint32_t token, const std::string &username, std::uint32_t clientIp,
		LoginHandoffTicket &ticket ) -> bool;

private:
	auto Store( const LoginHandoffTicket &ticket ) -> bool;
	void RemoveExpired();

	std::mutex pendingMutex;
	std::unordered_map<std::uint32_t, LoginHandoffTicket> pendingTickets;
};

extern cLoginHandoff LoginHandoff;

#endif
