#include "cdice.h"

#include <algorithm>
#include <cstdlib>

#include "funcdecl.h"
#include "utility/random.hpp"
using Random = effolkronium::random_static ;
cDice::cDice() : dice(1), sides(1), addition(0) {}

cDice::cDice(const std::string &dieString) { convStringToDice(dieString); }

cDice::cDice(std::int32_t d, std::int32_t s, std::int32_t a) : dice(d), sides(s), addition(a) {}

cDice::~cDice() {}

void cDice::SetDice(std::int32_t newDice) { dice = newDice; }

void cDice::SetSides(std::int32_t newSides) { sides = newSides; }

void cDice::SetAddition(std::int32_t newAddition) { addition = newAddition; }
std::int32_t cDice::RollDice() {
    std::int32_t sum = 0;
    for (std::int32_t rolls = 0; rolls < dice; ++rolls) {
        sum += Random::get(1, sides);
    }
    sum += addition;
    return sum;
}

bool cDice::convStringToDice(std::string dieString) {
    dice = 1;
    sides = 1;
    addition = 0;
    
    auto const d_position = dieString.find("d");
    
    auto const is_invalid_d_position = bool{d_position >= dieString.size()};
    
    if (is_invalid_d_position)
        return false;
    
    auto const plus_position = std::clamp(dieString.find("+"), std::size_t{0}, dieString.size());
    
    auto const is_invalid_plus_position = bool{plus_position < d_position};
    
    if (is_invalid_plus_position) {
        return false;
    }
    
    auto const parsed_value = [&](std::int32_t position, std::int32_t count, std::int32_t value = 0) -> std::int32_t {
        try {
            return std::stoi(dieString.substr(position, count));
        }
        catch (...) {
            return value;
        }
    };
    
    auto const is_to_parse_size = bool{d_position > 0};
    
    if (is_to_parse_size) {
        auto const position = std::size_t{0};
        auto const count = d_position;
        dice = parsed_value(position, static_cast<std::int32_t>(count), 1);
    }
    
    auto const is_to_parse_sides = bool{plus_position > d_position + 1};
    
    if (is_to_parse_sides) {
        auto const position = d_position + 1;
        auto const count = plus_position - position;
        sides = parsed_value(static_cast<std::int32_t>(position), static_cast<std::int32_t>(count), 1);
    }
    
    auto const is_to_parse_plus = bool{dieString.size() > plus_position + 1};
    
    if (is_to_parse_plus) {
        auto const position = plus_position + 1;
        auto const count = dieString.size() - position;
        addition = parsed_value(static_cast<std::int32_t>(position), static_cast<std::int32_t>(count));
    }
    
    return true;
}
