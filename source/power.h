#ifndef __POWER_H__
#define __POWER_H__
#include <cstdint>
//o------------------------------------------------------------------------------------------------o
//|	Function	-	power()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Integer based version of the pow() function
//o------------------------------------------------------------------------------------------------o
inline std::uint32_t power( std::uint32_t base, std::uint32_t exponent )
{
	if( exponent == 0 )
		return 1;

	if( exponent > 0 )
	{
		auto total = base;
		for( std::uint32_t i = 1; i < exponent; ++i )
		{
			total *= base;
		}
		return total;
	}
	return 0;
}

#endif
