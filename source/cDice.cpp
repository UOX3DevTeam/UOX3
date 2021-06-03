#include "cdice.h"
#include <algorithm>
#include <random>
#include <stdlib.h>
#include <string>

#include "funcdecl.h"

cDice::cDice() : dice( 1 ), sides( 1 ), addition( 0 )
{
}

cDice::cDice( const std::string &dieString )
{
	convStringToDice( dieString );
}

cDice::cDice( SI32 d, SI32 s, SI32 a ) : dice( d ), sides( s ), addition( a )
{
}

cDice::~cDice()
{
}

void cDice::setDice( SI32 newDice )
{
	dice = newDice;
}

void cDice::setSides( SI32 newSides )
{
	sides = newSides;
}

void cDice::setAddition( SI32 newAddition )
{
	addition = newAddition;
}
SI32 cDice::roll( void )
{
	SI32 sum = 0;
	for( SI32 rolls = 0; rolls < dice; ++rolls )
	{
		sum += RandomNum( 1, sides );
	}
	sum += addition;
	return sum;
}

bool cDice::convStringToDice( std::string dieString )
{
	dice = 1;
	sides = 1;
	addition = 0;

	auto const d_position = dieString.find("d");

	auto const is_invalid_d_position = bool{d_position >=  dieString.size()};

	if (is_invalid_d_position) {
		return false;
	}

	auto const plus_position = std::clamp(
										  dieString.find("+"), std::size_t{0}, dieString.size()
										  );

	auto const is_invalid_plus_position = bool{plus_position < d_position};

	if (is_invalid_plus_position) {
		return false;
	}

	auto const parsed_value = [&] (
								   SI32 position, SI32 count, SI32 value = 0
								   ) -> SI32 {
		try {
			return std::stoi(dieString.substr(position, count));
		} catch (...) {
			return value;
		}
	};

	auto const is_to_parse_size = bool{d_position > 0};

	if(is_to_parse_size) {
		auto const position = std::size_t{0};
		auto const count = d_position;
		dice = parsed_value(position, static_cast<SI32>(count), 1);
	}

	auto const is_to_parse_sides = bool{plus_position > d_position + 1};

	if(is_to_parse_sides) {
		auto const position = d_position + 1;
		auto const count = plus_position - position;
		sides = parsed_value(static_cast<SI32>(position), static_cast<SI32>(count), 1);
	}

	auto const is_to_parse_plus = bool{dieString.size() > plus_position + 1};

	if(is_to_parse_plus) {
		auto const position = plus_position + 1;
		auto const count = dieString.size() - position;
		addition = parsed_value(static_cast<SI32>(position), static_cast<SI32>(count));
	}

	return true;
}
