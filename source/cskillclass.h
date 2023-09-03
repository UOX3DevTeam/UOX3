#ifndef __CSKILLCLASS_H__
#define __CSKILLCLASS_H__

#include <algorithm>
#include <cstdint>
#include <string>

#include "typedefs.h"

class cSkillClass {
  public:
    std::uint8_t skill;
    std::uint16_t value;

    cSkillClass(std::uint8_t nskill, std::uint16_t nvalue) : skill(nskill), value(nvalue) {}
};

inline bool operator==(const cSkillClass &x, const cSkillClass &y) { return (x.value == y.value); }

inline bool operator<(const cSkillClass &x, const cSkillClass &y) {
    return ((x.value == y.value) ? (x.skill < y.skill) : (x.value < y.value));
}

inline bool operator>(const cSkillClass &x, const cSkillClass &y) {
    return ((x.value == y.value) ? (x.skill > y.skill) : (x.value > y.value));
}

#endif
