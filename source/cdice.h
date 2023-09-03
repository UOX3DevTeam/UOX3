#ifndef __CDICE__
#define __CDICE__
#include <cstdint>
#include <string>

#include "typedefs.h"
class cDice {
  private:
    std::int32_t dice;
    std::int32_t sides;
    std::int32_t addition;

    bool convStringToDice(std::string dieString);

  public:
    cDice();
    cDice(const std::string &dieString);
    cDice(std::int32_t d, std::int32_t s, std::int32_t a);
    ~cDice();

    std::int32_t RollDice(void);
    void SetDice(std::int32_t newDice);
    void SetSides(std::int32_t newSides);
    void SetAddition(std::int32_t newAddition);
};

#endif
