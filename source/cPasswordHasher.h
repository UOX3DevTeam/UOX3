#ifndef __CPASSWORDHASHER_H
#define __CPASSWORDHASHER_H

#include <array>
#include <cstddef>
#include <string>

class cPasswordHasher
{
public:
	static bool IsPasswordHash( const std::string &storedPassword );
	static bool HashPassword( const std::string &password, std::string &passwordHash );
	static bool VerifyPassword( const std::string &storedPassword, const std::string &password );
	static bool GenerateTemporaryPassword( std::string &temporaryPassword );
	static bool FillRandom( unsigned char *buffer, size_t length );
	static std::array<unsigned char, 32> HmacSha256( const std::string &key, const unsigned char *data, size_t length );
	static bool ConstantTimeEquals( const unsigned char *left, const unsigned char *right, size_t comparisonLength,
		size_t leftLength, size_t rightLength );
};

#endif
