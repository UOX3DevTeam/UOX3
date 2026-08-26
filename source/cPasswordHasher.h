#ifndef __CPASSWORDHASHER_H
#define __CPASSWORDHASHER_H

#include <string>

class cPasswordHasher
{
public:
	static bool IsPasswordHash( const std::string &storedPassword );
	static bool HashPassword( const std::string &password, std::string &passwordHash );
	static bool VerifyPassword( const std::string &storedPassword, const std::string &password );
	static bool GenerateTemporaryPassword( std::string &temporaryPassword );

private:
	static bool FillRandom( unsigned char *buffer, size_t length );
	static bool ConstantTimeEquals( const unsigned char *left, const unsigned char *right, size_t comparisonLength,
		size_t leftLength, size_t rightLength );
};

#endif
