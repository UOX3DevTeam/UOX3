#ifndef __WEIGHT_H__
#define __WEIGHT_H__

#include <cstdint>
#include <string>

#include "typedefs.h"

class CItem;
class CChar;
class CBaseObject;

const std::int32_t MAX_WEIGHT =
6553500; // Max weight (This number / 100, thus max actual weight is 65535.00)

class CWeight {
private:
    bool CalcAddWeight(CItem *item, std::int32_t &totalWeight);
    bool CalcSubtractWeight(CItem *item, std::int32_t &totalWeight);
    bool IsWeightedContainer(CItem *toCheck);
    
public:
    std::int32_t CalcCharWeight(CChar *mChar);
    std::int32_t CalcWeight(CItem *pack);
    
    bool IsOverloaded(CChar *mChar) const;
    bool CheckPackWeight(CChar *ourChar, CItem *pack, CItem *item);
    bool CheckCharWeight(CChar *ourChar, CChar *mChar, CItem *item, std::uint16_t amount = 0);
    
    void AddItemWeight(CBaseObject *getObj, CItem *item);
    void AddItemWeight(CChar *mChar, CItem *item);
    void AddItemWeight(CItem *pack, CItem *item);
    
    void SubtractItemWeight(CBaseObject *getObj, CItem *item);
    void SubtractItemWeight(CChar *mChar, CItem *item);
    void SubtractItemWeight(CItem *pack, CItem *item);
};

extern CWeight *Weight;

#endif
