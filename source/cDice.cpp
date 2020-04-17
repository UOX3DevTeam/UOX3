#include "cdice.h"
#include <algorithm>
#include <random>
#include <stdlib.h>
#include <string>

namespace UOX
{

cDice::cDice() : dice( 1 ), sides( 1 ), addition( 0 )
{
}

cDice::cDice( const std::string &dieString )
{
	convStringToDice( dieString );
}

cDice::cDice( int d, int s, int a ) : dice( d ), sides( s ), addition( a )
{
}

cDice::~cDice()
{
}

void cDice::setDice( int newDice )
{
	dice = newDice;
}

void cDice::setSides( int newSides )
{
	sides = newSides;
}

void cDice::setAddition( int newAddition )
{
	addition = newAddition;
}

namespace {

auto random_device = std::random_device{};

auto generator = std::mt19937{random_device()};

auto RandomNum(
    std::size_t lower_bound, std::size_t upper_bound
) -> std::size_t {
    auto distribution = std::uniform_int_distribution<std::size_t>{
        lower_bound, upper_bound
    };

    return distribution(generator);
}

}//namespace

int cDice::roll( void )
{
	int sum = 0;
	for( int rolls = 0; rolls < dice; ++rolls )
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
        std::size_t position,
        std::size_t count,
        std::size_t value = 0
    ) -> std::size_t {
        try {
            constexpr auto is_32bit = bool{ sizeof(std::size_t) == 4 };

            if constexpr(is_32bit) {
                return std::stoul(dieString.substr(position, count));
            } else {
                return std::stoull(dieString.substr(position, count));
            }
        } catch (...) {
            return value;
        }
    };

    auto const is_to_parse_size = bool{d_position > 0};

    if(is_to_parse_size) {
        auto const position = std::size_t{0};
        auto const count = d_position;
        dice = parsed_value(position, count, 1);
    }

    auto const is_to_parse_sides = bool{plus_position > d_position + 1};

    if(is_to_parse_sides) {
        auto const position = d_position + 1;
        auto const count = plus_position - position;
        sides = parsed_value(position, count, 1);
    }

    auto const is_to_parse_plus = bool{dieString.size() > plus_position + 1};

    if(is_to_parse_plus) {
        auto const position = plus_position + 1;
        auto const count = dieString.size() - position;
        addition = parsed_value(position, count);
    }

    return true;
}

}
