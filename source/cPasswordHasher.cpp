#include "uox3.h"
#include "cPasswordHasher.h"

#include <algorithm>
#include <array>
#include <cerrno>
#include <charconv>
#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>

#if PLATFORM == WINDOWS
#include <bcrypt.h>
#elif PLATFORM == LINUX
#include <sys/random.h>
#else
#include <cstdlib>
#endif

static constexpr UI32 PBKDF2_ITERATIONS = 600000;
static constexpr UI32 PBKDF2_MAX_VERIFY_ITERATIONS = 2000000;
static constexpr size_t PBKDF2_SALT_LENGTH = 16;
static constexpr size_t PBKDF2_HASH_LENGTH = 32;
static constexpr std::string_view PBKDF2_PREFIX = "$pbkdf2-sha256$";

static constexpr std::array<UI32, 64> SHA256_CONSTANTS = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

//o------------------------------------------------------------------------------------------------o
//| Function	- RotateRight()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Performs the 32-bit rotation operation required by SHA-256
//o------------------------------------------------------------------------------------------------o
static UI32 RotateRight( UI32 value, UI32 count )
{
	return ( value >> count ) | ( value << ( 32 - count ));
}

class SHA256Context
{
public:
	//o--------------------------------------------------------------------------------------------o
	//| Function	- SHA256Context::SHA256Context()
	//o--------------------------------------------------------------------------------------------o
	//| Purpose	- Initializes a SHA-256 context with the standard initial state
	//o--------------------------------------------------------------------------------------------o
	SHA256Context() : state{ 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 }, buffer{}, bufferLength( 0 ), totalLength( 0 )
	{
	}

	//o--------------------------------------------------------------------------------------------o
	//| Function	- SHA256Context::Update()
	//o--------------------------------------------------------------------------------------------o
	//| Purpose	- Adds input bytes to the SHA-256 calculation
	//o--------------------------------------------------------------------------------------------o
	void Update( const unsigned char *data, size_t length )
	{
		totalLength += length;
		while( length > 0 )
		{
			const size_t copyLength = std::min( length, buffer.size() - bufferLength );
			std::copy_n( data, copyLength, buffer.data() + bufferLength );
			bufferLength += copyLength;
			data += copyLength;
			length -= copyLength;
			if( bufferLength == buffer.size() )
			{
				Transform( buffer.data() );
				bufferLength = 0;
			}
		}
	}

	//o--------------------------------------------------------------------------------------------o
	//| Function	- SHA256Context::Final()
	//o--------------------------------------------------------------------------------------------o
	//| Purpose	- Finalizes the SHA-256 calculation and returns its digest
	//o--------------------------------------------------------------------------------------------o
	std::array<unsigned char, 32> Final()
	{
		const UI64 bitLength = static_cast<UI64>( totalLength ) * 8;
		buffer[bufferLength++] = 0x80;
		if( bufferLength > 56 )
		{
			std::fill( buffer.begin() + bufferLength, buffer.end(), 0 );
			Transform( buffer.data() );
			bufferLength = 0;
		}
		std::fill( buffer.begin() + bufferLength, buffer.begin() + 56, 0 );
		for( size_t index = 0; index < 8; ++index )
		{
			buffer[63 - index] = static_cast<unsigned char>( bitLength >> ( index * 8 ));
		}
		Transform( buffer.data() );

		std::array<unsigned char, 32> digest{};
		for( size_t index = 0; index < state.size(); ++index )
		{
			digest[index * 4] = static_cast<unsigned char>( state[index] >> 24 );
			digest[index * 4 + 1] = static_cast<unsigned char>( state[index] >> 16 );
			digest[index * 4 + 2] = static_cast<unsigned char>( state[index] >> 8 );
			digest[index * 4 + 3] = static_cast<unsigned char>( state[index] );
		}
		return digest;
	}

private:
	//o--------------------------------------------------------------------------------------------o
	//| Function	- SHA256Context::Transform()
	//o--------------------------------------------------------------------------------------------o
	//| Purpose	- Processes one 512-bit SHA-256 message block
	//o--------------------------------------------------------------------------------------------o
	void Transform( const unsigned char *block )
	{
		std::array<UI32, 64> words{};
		for( size_t index = 0; index < 16; ++index )
		{
			words[index] = ( static_cast<UI32>( block[index * 4] ) << 24 ) |
				( static_cast<UI32>( block[index * 4 + 1] ) << 16 ) |
				( static_cast<UI32>( block[index * 4 + 2] ) << 8 ) |
				static_cast<UI32>( block[index * 4 + 3] );
		}
		for( size_t index = 16; index < words.size(); ++index )
		{
			const UI32 s0 = RotateRight( words[index - 15], 7 ) ^ RotateRight( words[index - 15], 18 ) ^ ( words[index - 15] >> 3 );
			const UI32 s1 = RotateRight( words[index - 2], 17 ) ^ RotateRight( words[index - 2], 19 ) ^ ( words[index - 2] >> 10 );
			words[index] = words[index - 16] + s0 + words[index - 7] + s1;
		}

		UI32 a = state[0], b = state[1], c = state[2], d = state[3];
		UI32 e = state[4], f = state[5], g = state[6], h = state[7];
		for( size_t index = 0; index < words.size(); ++index )
		{
			const UI32 sum1 = RotateRight( e, 6 ) ^ RotateRight( e, 11 ) ^ RotateRight( e, 25 );
			const UI32 choice = ( e & f ) ^ ( ~e & g );
			const UI32 temporary1 = h + sum1 + choice + SHA256_CONSTANTS[index] + words[index];
			const UI32 sum0 = RotateRight( a, 2 ) ^ RotateRight( a, 13 ) ^ RotateRight( a, 22 );
			const UI32 majority = ( a & b ) ^ ( a & c ) ^ ( b & c );
			const UI32 temporary2 = sum0 + majority;
			h = g; g = f; f = e; e = d + temporary1;
			d = c; c = b; b = a; a = temporary1 + temporary2;
		}
		state[0] += a; state[1] += b; state[2] += c; state[3] += d;
		state[4] += e; state[5] += f; state[6] += g; state[7] += h;
	}

	std::array<UI32, 8> state;
	std::array<unsigned char, 64> buffer;
	size_t bufferLength;
	size_t totalLength;
};

class HMACSHA256
{
public:
	//o--------------------------------------------------------------------------------------------o
	//| Function	- HMACSHA256::HMACSHA256()
	//o--------------------------------------------------------------------------------------------o
	//| Purpose	- Prepares the reusable inner and outer SHA-256 states for an HMAC key
	//o--------------------------------------------------------------------------------------------o
	explicit HMACSHA256( const std::string &password ) : innerBase(), outerBase()
	{
		std::array<unsigned char, 64> key{};
		std::array<unsigned char, 64> innerPad{};
		std::array<unsigned char, 64> outerPad{};
		if( password.size() > key.size() )
		{
			SHA256Context context;
			context.Update( reinterpret_cast<const unsigned char *>( password.data() ), password.size() );
			const auto digest = context.Final();
			std::copy( digest.begin(), digest.end(), key.begin() );
		}
		else
		{
			std::copy( password.begin(), password.end(), key.begin() );
		}
		for( size_t index = 0; index < key.size(); ++index )
		{
			innerPad[index] = key[index] ^ 0x36;
			outerPad[index] = key[index] ^ 0x5c;
		}
		innerBase.Update( innerPad.data(), innerPad.size() );
		outerBase.Update( outerPad.data(), outerPad.size() );
	}

	//o--------------------------------------------------------------------------------------------o
	//| Function	- HMACSHA256::Calculate()
	//o--------------------------------------------------------------------------------------------o
	//| Purpose	- Calculates an HMAC-SHA256 value using the prepared key state
	//o--------------------------------------------------------------------------------------------o
	std::array<unsigned char, 32> Calculate( const unsigned char *data, size_t length ) const
	{
		SHA256Context inner = innerBase;
		inner.Update( data, length );
		const auto innerDigest = inner.Final();
		SHA256Context outer = outerBase;
		outer.Update( innerDigest.data(), innerDigest.size() );
		return outer.Final();
	}

private:
	SHA256Context innerBase;
	SHA256Context outerBase;
};

//o------------------------------------------------------------------------------------------------o
//| Function	- DerivePassword()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Derives a PBKDF2-HMAC-SHA256 password value using the supplied salt and work factor
//o------------------------------------------------------------------------------------------------o
static std::array<unsigned char, PBKDF2_HASH_LENGTH> DerivePassword(
	const std::string &password, const unsigned char *salt, size_t saltLength, UI32 iterations )
{
	HMACSHA256 hmac( password );
	std::vector<unsigned char> firstInput( salt, salt + saltLength );
	firstInput.insert( firstInput.end(), { 0, 0, 0, 1 } );
	auto current = hmac.Calculate( firstInput.data(), firstInput.size() );
	auto derived = current;
	for( UI32 iteration = 1; iteration < iterations; ++iteration )
	{
		current = hmac.Calculate( current.data(), current.size() );
		for( size_t index = 0; index < derived.size(); ++index )
		{
			derived[index] ^= current[index];
		}
	}
	return derived;
}

//o------------------------------------------------------------------------------------------------o
//| Function	- EncodeHex()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Encodes binary salt or hash data as lowercase hexadecimal text
//o------------------------------------------------------------------------------------------------o
static std::string EncodeHex( const unsigned char *data, size_t length )
{
	static constexpr char HEX[] = "0123456789abcdef";
	std::string encoded( length * 2, '0' );
	for( size_t index = 0; index < length; ++index )
	{
		encoded[index * 2] = HEX[data[index] >> 4];
		encoded[index * 2 + 1] = HEX[data[index] & 0x0f];
	}
	return encoded;
}

//o------------------------------------------------------------------------------------------------o
//| Function	- DecodeHex()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Decodes hexadecimal text into binary data while rejecting malformed input
//o------------------------------------------------------------------------------------------------o
static bool DecodeHex( std::string_view encoded, std::vector<unsigned char> &decoded )
{
	if( encoded.empty() || encoded.size() % 2 != 0 )
	{
		return false;
	}
	decoded.clear();
	decoded.reserve( encoded.size() / 2 );
	for( size_t index = 0; index < encoded.size(); index += 2 )
	{
		unsigned int value = 0;
		const auto result = std::from_chars( encoded.data() + index, encoded.data() + index + 2, value, 16 );
		if( result.ec != std::errc{} || result.ptr != encoded.data() + index + 2 )
		{
			return false;
		}
		decoded.push_back( static_cast<unsigned char>( value ));
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	- ParsePasswordHash()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Parses and validates a stored PBKDF2 password record
//o------------------------------------------------------------------------------------------------o
static bool ParsePasswordHash( const std::string &storedPassword, UI32 &iterations,
	std::vector<unsigned char> &salt, std::vector<unsigned char> &passwordHash )
{
	if( storedPassword.rfind( PBKDF2_PREFIX, 0 ) != 0 )
	{
		return false;
	}
	const size_t iterationsStart = PBKDF2_PREFIX.size();
	const size_t saltSeparator = storedPassword.find( '$', iterationsStart );
	const size_t hashSeparator = saltSeparator == std::string::npos ? std::string::npos : storedPassword.find( '$', saltSeparator + 1 );
	if( saltSeparator == std::string::npos || hashSeparator == std::string::npos || storedPassword.find( '$', hashSeparator + 1 ) != std::string::npos )
	{
		return false;
	}
	const auto iterationResult = std::from_chars(
		storedPassword.data() + iterationsStart, storedPassword.data() + saltSeparator, iterations );
	if( iterationResult.ec != std::errc{} || iterationResult.ptr != storedPassword.data() + saltSeparator ||
		iterations == 0 || iterations > PBKDF2_MAX_VERIFY_ITERATIONS )
	{
		return false;
	}
	return DecodeHex( std::string_view( storedPassword ).substr( saltSeparator + 1, hashSeparator - saltSeparator - 1 ), salt ) &&
		DecodeHex( std::string_view( storedPassword ).substr( hashSeparator + 1 ), passwordHash ) &&
		salt.size() == PBKDF2_SALT_LENGTH && passwordHash.size() == PBKDF2_HASH_LENGTH;
}

//o------------------------------------------------------------------------------------------------o
//| Function	- cPasswordHasher::IsPasswordHash()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Checks whether a stored password uses the PBKDF2 record prefix
//o------------------------------------------------------------------------------------------------o
bool cPasswordHasher::IsPasswordHash( const std::string &storedPassword )
{
	return storedPassword.rfind( PBKDF2_PREFIX, 0 ) == 0;
}

//o------------------------------------------------------------------------------------------------o
//| Function	- cPasswordHasher::HashPassword()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Creates a salted PBKDF2-HMAC-SHA256 record from a plaintext password
//o------------------------------------------------------------------------------------------------o
bool cPasswordHasher::HashPassword( const std::string &password, std::string &passwordHash )
{
	std::array<unsigned char, PBKDF2_SALT_LENGTH> salt{};
	if( password.empty() || !FillRandom( salt.data(), salt.size() ))
	{
		return false;
	}
	const auto derived = DerivePassword( password, salt.data(), salt.size(), PBKDF2_ITERATIONS );
	passwordHash = std::string( PBKDF2_PREFIX ) + std::to_string( PBKDF2_ITERATIONS ) + "$" +
		EncodeHex( salt.data(), salt.size() ) + "$" + EncodeHex( derived.data(), derived.size() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	- cPasswordHasher::VerifyPassword()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Verifies either a PBKDF2 record or a legacy plaintext password
//o------------------------------------------------------------------------------------------------o
bool cPasswordHasher::VerifyPassword( const std::string &storedPassword, const std::string &password )
{
	if( IsPasswordHash( storedPassword ))
	{
		UI32 iterations = 0;
		std::vector<unsigned char> salt;
		std::vector<unsigned char> expectedHash;
		if( !ParsePasswordHash( storedPassword, iterations, salt, expectedHash ))
		{
			return false;
		}
		const auto actualHash = DerivePassword( password, salt.data(), salt.size(), iterations );
		return ConstantTimeEquals( expectedHash.data(), actualHash.data(), actualHash.size(), expectedHash.size(), actualHash.size() );
	}
	return ConstantTimeEquals( reinterpret_cast<const unsigned char *>( storedPassword.data() ),
		reinterpret_cast<const unsigned char *>( password.data() ), std::max( storedPassword.size(), password.size() ),
		storedPassword.size(), password.size() );
}

//o------------------------------------------------------------------------------------------------o
//| Function	- cPasswordHasher::GenerateTemporaryPassword()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Generates a random temporary password suitable for account recovery
//o------------------------------------------------------------------------------------------------o
bool cPasswordHasher::GenerateTemporaryPassword( std::string &temporaryPassword )
{
	static constexpr char alphabet[] = "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz23456789";
	std::array<unsigned char, 12> randomBytes{};
	if( !FillRandom( randomBytes.data(), randomBytes.size() ))
	{
		return false;
	}
	temporaryPassword.clear();
	temporaryPassword.reserve( randomBytes.size() );
	for( const unsigned char randomByte : randomBytes )
	{
		temporaryPassword.push_back( alphabet[randomByte % ( sizeof( alphabet ) - 1 )] );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	- cPasswordHasher::FillRandom()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Fills a buffer using the operating system's cryptographically secure random source
//o------------------------------------------------------------------------------------------------o
bool cPasswordHasher::FillRandom( unsigned char *buffer, size_t length )
{
#if PLATFORM == WINDOWS
	if( length > std::numeric_limits<ULONG>::max() )
	{
		return false;
	}
	return BCryptGenRandom( nullptr, buffer, static_cast<ULONG>( length ), BCRYPT_USE_SYSTEM_PREFERRED_RNG ) == 0;
#elif PLATFORM == LINUX
	size_t generated = 0;
	while( generated < length )
	{
		const auto result = getrandom( buffer + generated, length - generated, 0 );
		if( result < 0 )
		{
			if( errno == EINTR )
			{
				continue;
			}
			return false;
		}
		generated += static_cast<size_t>( result );
	}
	return true;
#else
	arc4random_buf( buffer, length );
	return true;
#endif
}

//o------------------------------------------------------------------------------------------------o
//| Function	- cPasswordHasher::ConstantTimeEquals()
//o------------------------------------------------------------------------------------------------o
//| Purpose	- Compares credential data without returning early on the first differing byte
//o------------------------------------------------------------------------------------------------o
bool cPasswordHasher::ConstantTimeEquals( const unsigned char *left, const unsigned char *right, size_t comparisonLength,
	size_t leftLength, size_t rightLength )
{
	size_t difference = leftLength ^ rightLength;
	for( size_t index = 0; index < comparisonLength; ++index )
	{
		const unsigned char leftValue = index < leftLength ? left[index] : 0;
		const unsigned char rightValue = index < rightLength ? right[index] : 0;
		difference |= leftValue ^ rightValue;
	}
	return difference == 0;
}
