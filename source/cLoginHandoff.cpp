#include "uox3.h"
#include "cLoginHandoff.h"
#include "cPasswordHasher.h"
#include "network.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <limits>

#if PLATFORM != WINDOWS
#include <fcntl.h>
#include <unistd.h>
#else
#include <ws2tcpip.h>
#endif

namespace
{
	constexpr std::uint8_t HANDOFF_VERSION = 1;
	constexpr std::size_t SIGNED_LENGTH = 69;
	constexpr std::size_t USERNAME_OFFSET = 39;
	constexpr std::size_t USERNAME_LENGTH = 30;
	constexpr std::size_t MAC_OFFSET = 69;
	constexpr std::array<unsigned char, 2> HANDOFF_ACK = { 'O', 'K' };

	void Put16( unsigned char *output, std::size_t offset, std::uint16_t value )
	{
		output[offset] = static_cast<unsigned char>( value >> 8 );
		output[offset + 1] = static_cast<unsigned char>( value );
	}

	void Put32( unsigned char *output, std::size_t offset, std::uint32_t value )
	{
		for( std::size_t index = 0; index < 4; ++index )
		{
			output[offset + index] = static_cast<unsigned char>( value >> (( 3 - index ) * 8 ));
		}
	}

	void Put64( unsigned char *output, std::size_t offset, std::uint64_t value )
	{
		for( std::size_t index = 0; index < 8; ++index )
		{
			output[offset + index] = static_cast<unsigned char>( value >> (( 7 - index ) * 8 ));
		}
	}

	std::uint16_t Get16( const unsigned char *input, std::size_t offset )
	{
		return static_cast<std::uint16_t>(( static_cast<std::uint16_t>( input[offset] ) << 8 ) | input[offset + 1] );
	}

	std::uint32_t Get32( const unsigned char *input, std::size_t offset )
	{
		return ( static_cast<std::uint32_t>( input[offset] ) << 24 ) |
			( static_cast<std::uint32_t>( input[offset + 1] ) << 16 ) |
			( static_cast<std::uint32_t>( input[offset + 2] ) << 8 ) |
			static_cast<std::uint32_t>( input[offset + 3] );
	}

	std::uint64_t Get64( const unsigned char *input, std::size_t offset )
	{
		auto value = std::uint64_t{ 0 };
		for( std::size_t index = 0; index < 8; ++index )
		{
			value = ( value << 8 ) | input[offset + index];
		}
		return value;
	}

	std::uint64_t UnixTime()
	{
		return static_cast<std::uint64_t>( std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch() ).count() );
	}

	bool ValidSecret( const std::string &secret )
	{
		return secret.size() >= 16 && oldstrutil::upper( secret ) != "NONE";
	}

	std::array<unsigned char, cLoginHandoff::PACKET_SIZE> Serialize(
		const LoginHandoffTicket &ticket, const std::string &secret )
	{
		std::array<unsigned char, cLoginHandoff::PACKET_SIZE> packet{};
		Put32( packet.data(), 0, cLoginHandoff::MAGIC );
		packet[4] = HANDOFF_VERSION;
		Put16( packet.data(), 5, static_cast<std::uint16_t>( packet.size() ));
		Put16( packet.data(), 7, ticket.shardId );
		Put32( packet.data(), 9, ticket.token );
		Put32( packet.data(), 13, ticket.clientIp );
		Put32( packet.data(), 17, ticket.clientVersion );
		Put32( packet.data(), 21, ticket.clientType );
		Put32( packet.data(), 25, ticket.clientVersionShort );
		Put64( packet.data(), 29, ticket.expires );
		Put16( packet.data(), 37, ticket.accountFlags );
		std::copy_n( ticket.username.begin(), std::min( ticket.username.size(), USERNAME_LENGTH ),
			packet.begin() + USERNAME_OFFSET );
		const auto mac = cPasswordHasher::HmacSha256( secret, packet.data(), SIGNED_LENGTH );
		std::copy( mac.begin(), mac.end(), packet.begin() + MAC_OFFSET );
		return packet;
	}

#if PLATFORM == WINDOWS
	using NativeSocket = SOCKET;
	using NativeAddressLength = int;
	constexpr NativeSocket INVALID_NATIVE_SOCKET = INVALID_SOCKET;
#else
	using NativeSocket = int;
	using NativeAddressLength = socklen_t;
	constexpr NativeSocket INVALID_NATIVE_SOCKET = -1;
#endif

	void CloseNativeSocket( NativeSocket socket )
	{
		if( socket != INVALID_NATIVE_SOCKET )
		{
			closesocket( socket );
		}
	}

	bool SetNonBlocking( NativeSocket socket, bool enabled )
	{
#if PLATFORM == WINDOWS
		u_long mode = enabled ? 1 : 0;
		return ioctlsocket( socket, FIONBIO, &mode ) == 0;
#else
		const auto flags = fcntl( socket, F_GETFL, 0 );
		return flags >= 0 && fcntl( socket, F_SETFL, enabled ? flags | O_NONBLOCK : flags & ~O_NONBLOCK ) == 0;
#endif
	}

	bool WaitForSocket( NativeSocket socket, bool write, int seconds )
	{
		fd_set descriptors;
		FD_ZERO( &descriptors );
		FD_SET( socket, &descriptors );
		timeval timeout{ seconds, 0 };
		return select( static_cast<int>( socket ) + 1, write ? nullptr : &descriptors,
			write ? &descriptors : nullptr, nullptr, &timeout ) > 0;
	}

	bool ConnectWithTimeout( NativeSocket socket, const sockaddr *address, NativeAddressLength addressLength )
	{
		if( !SetNonBlocking( socket, true ))
		{
			return false;
		}
		const auto result = connect( socket, address, addressLength );
		if( result != 0 )
		{
#if PLATFORM == WINDOWS
			const auto error = WSAGetLastError();
			if( error != WSAEWOULDBLOCK && error != WSAEINPROGRESS )
#else
			if( errno != EINPROGRESS )
#endif
			{
				return false;
			}
			if( !WaitForSocket( socket, true, 3 ))
			{
				return false;
			}
			int socketError = 0;
		NativeAddressLength errorLength = sizeof( socketError );
			if( getsockopt( socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>( &socketError ), &errorLength ) != 0 || socketError != 0 )
			{
				return false;
			}
		}
		return SetNonBlocking( socket, false );
	}

	bool SendAll( NativeSocket socket, const unsigned char *data, std::size_t length )
	{
		std::size_t sent = 0;
		while( sent < length && WaitForSocket( socket, true, 3 ))
		{
			const auto result = send( socket, reinterpret_cast<const char *>( data + sent ),
				static_cast<int>( length - sent ), 0 );
			if( result <= 0 )
			{
				return false;
			}
			sent += static_cast<std::size_t>( result );
		}
		return sent == length;
	}

	bool ReceiveAll( NativeSocket socket, unsigned char *data, std::size_t length )
	{
		std::size_t received = 0;
		while( received < length && WaitForSocket( socket, false, 3 ))
		{
			const auto result = recv( socket, reinterpret_cast<char *>( data + received ),
				static_cast<int>( length - received ), 0 );
			if( result <= 0 )
			{
				return false;
			}
			received += static_cast<std::size_t>( result );
		}
		return received == length;
	}
}

cLoginHandoff LoginHandoff;

auto cLoginHandoff::CreateTicket( const std::string &username, std::uint32_t clientIp,
	std::uint32_t clientVersion, std::uint32_t clientType, std::uint32_t clientVersionShort,
	std::uint16_t shardId, std::uint16_t accountFlags, std::uint16_t lifetimeSeconds,
	LoginHandoffTicket &ticket ) -> bool
{
	if( username.empty() || username.size() > USERNAME_LENGTH || shardId == 0 )
	{
		return false;
	}
	std::uint32_t token = 0;
	do
	{
		if( !cPasswordHasher::FillRandom( reinterpret_cast<unsigned char *>( &token ), sizeof( token )))
		{
			return false;
		}
	} while( token == 0 || token == MAGIC || token == 0x12345678 || token == 0xFFFFFFFF );

	ticket.token = token;
	ticket.clientIp = clientIp;
	ticket.clientVersion = clientVersion;
	ticket.clientType = clientType;
	ticket.clientVersionShort = clientVersionShort;
	ticket.expires = UnixTime() + std::max<std::uint16_t>( lifetimeSeconds, 1 );
	ticket.shardId = shardId;
	ticket.accountFlags = accountFlags;
	ticket.username = username;
	return true;
}

auto cLoginHandoff::CreateLocalAdminTicket( const std::string &username, std::uint32_t clientIp,
	std::uint32_t clientVersion, std::uint32_t clientType, std::uint32_t clientVersionShort,
	std::uint16_t shardId, std::uint16_t lifetimeSeconds, LoginHandoffTicket &ticket ) -> bool
{
	if( !CreateTicket( username, clientIp, clientVersion, clientType, clientVersionShort,
		shardId, 0, lifetimeSeconds, ticket ))
	{
		return false;
	}
	ticket.localAdmin = true;
	return Store( ticket );
}

auto cLoginHandoff::RegisterWithGame( const std::string &host, std::uint16_t port,
	const LoginHandoffTicket &ticket, const std::string &secret, std::string &error ) -> bool
{
	if( !ValidSecret( secret ))
	{
		error = "LOGINHANDOFFSECRET must contain at least 16 characters";
		return false;
	}

	addrinfo hints{};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	addrinfo *addresses = nullptr;
	const auto portText = std::to_string( port );
	if( getaddrinfo( host.c_str(), portText.c_str(), &hints, &addresses ) != 0 )
	{
		error = "unable to resolve shard handoff address";
		return false;
	}

	const auto packet = Serialize( ticket, secret );
	auto registered = false;
	for( auto address = addresses; address != nullptr && !registered; address = address->ai_next )
	{
		auto socket = ::socket( address->ai_family, address->ai_socktype, address->ai_protocol );
		if( socket == INVALID_NATIVE_SOCKET )
		{
			continue;
		}
		if( ConnectWithTimeout( socket, address->ai_addr, static_cast<NativeAddressLength>( address->ai_addrlen )) &&
			SendAll( socket, packet.data(), packet.size() ))
		{
			auto acknowledgement = std::array<unsigned char, HANDOFF_ACK.size()>{};
			registered = ReceiveAll( socket, acknowledgement.data(), acknowledgement.size() ) && acknowledgement == HANDOFF_ACK;
		}
		CloseNativeSocket( socket );
	}
	freeaddrinfo( addresses );
	if( !registered )
	{
		error = "shard did not accept the login handoff";
	}
	return registered;
}

auto cLoginHandoff::AcceptRegistration( CSocket *socket, const std::string &secret,
	std::uint16_t expectedShardId, std::uint16_t maximumLifetime, std::string &error ) -> bool
{
	if( socket == nullptr || !ValidSecret( secret ))
	{
		error = "invalid handoff configuration";
		return false;
	}
	try
	{
		if( socket->Receive( static_cast<SI32>( PACKET_SIZE ), false ) <= 0 ||
			static_cast<std::size_t>( socket->InLength() ) != PACKET_SIZE )
		{
			error = "incomplete handoff registration";
			return false;
		}
	}
	catch( const socket_error & )
	{
		error = "handoff registration receive failed";
		return false;
	}

	const auto *packet = socket->Buffer();
	if( Get32( packet, 0 ) != MAGIC || packet[4] != HANDOFF_VERSION || Get16( packet, 5 ) != PACKET_SIZE )
	{
		error = "unsupported handoff registration";
		return false;
	}
	const auto expectedMac = cPasswordHasher::HmacSha256( secret, packet, SIGNED_LENGTH );
	if( !cPasswordHasher::ConstantTimeEquals( packet + MAC_OFFSET, expectedMac.data(), expectedMac.size(),
		expectedMac.size(), expectedMac.size() ))
	{
		error = "handoff authentication failed";
		return false;
	}

	LoginHandoffTicket ticket;
	ticket.shardId = Get16( packet, 7 );
	ticket.token = Get32( packet, 9 );
	ticket.clientIp = Get32( packet, 13 );
	ticket.clientVersion = Get32( packet, 17 );
	ticket.clientType = Get32( packet, 21 );
	ticket.clientVersionShort = Get32( packet, 25 );
	ticket.expires = Get64( packet, 29 );
	ticket.accountFlags = Get16( packet, 37 );
	const auto usernameEnd = std::find( packet + USERNAME_OFFSET, packet + USERNAME_OFFSET + USERNAME_LENGTH, 0 );
	ticket.username.assign( reinterpret_cast<const char *>( packet + USERNAME_OFFSET ),
		static_cast<std::size_t>( usernameEnd - ( packet + USERNAME_OFFSET )));

	const auto now = UnixTime();
	if( ticket.shardId != expectedShardId || ticket.token == 0 || ticket.username.empty() ||
		ticket.expires < now || ticket.expires > now + std::max<std::uint16_t>( maximumLifetime, 1 ) + 5 )
	{
		error = "handoff registration is expired or for another shard";
		return false;
	}
	if( !Store( ticket ))
	{
		error = "duplicate handoff token";
		return false;
	}
	socket->Send( HANDOFF_ACK.data(), static_cast<SI32>( HANDOFF_ACK.size() ));
	socket->FlushBuffer( false );
	return true;
}

auto cLoginHandoff::Store( const LoginHandoffTicket &ticket ) -> bool
{
	std::lock_guard<std::mutex> guard( pendingMutex );
	const auto now = UnixTime();
	for( auto entry = pendingTickets.begin(); entry != pendingTickets.end(); )
	{
		entry = entry->second.expires < now ? pendingTickets.erase( entry ) : std::next( entry );
	}
	return pendingTickets.emplace( ticket.token, ticket ).second;
}

void cLoginHandoff::RemoveExpired()
{
	const auto now = UnixTime();
	for( auto entry = pendingTickets.begin(); entry != pendingTickets.end(); )
	{
		entry = entry->second.expires < now ? pendingTickets.erase( entry ) : std::next( entry );
	}
}

auto cLoginHandoff::Consume( std::uint32_t token, const std::string &username,
	std::uint32_t clientIp, LoginHandoffTicket &ticket ) -> bool
{
	std::lock_guard<std::mutex> guard( pendingMutex );
	RemoveExpired();
	const auto found = pendingTickets.find( token );
	if( found == pendingTickets.end() )
	{
		return false;
	}
	ticket = found->second;
	pendingTickets.erase( found );
	return ticket.username == username && ticket.clientIp == clientIp && ticket.expires >= UnixTime();
}
