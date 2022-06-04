#ifndef __WEIGHT_H__
#define __WEIGHT_H__
#include "uox3.h"
// Forward declares
class CItem ;
class CChar ;
class CBaseObject;

constexpr auto MAX_WEIGHT = std::int32_t(6553500);	// Max weight (This number / 100, thus max actual weight is 65535.00)

class CWeight{
private:
	auto	calcAddWeight( CItem *item, SI32 &totalWeight ) ->bool ;
	auto	calcSubtractWeight( CItem *item, SI32 &totalWeight ) ->bool ;
	auto	IsWeightedContainer( CItem *toCheck ) ->bool ;

public:
	auto	calcCharWeight( CChar *mChar ) ->SI32 ;
	auto	calcWeight( CItem *pack ) ->SI32 ;

	auto	isOverloaded( CChar *mChar ) const ->bool ;
	auto	checkPackWeight( CChar *ourChar, CItem *pack, CItem *item ) ->bool ;
	auto	checkCharWeight( CChar *ourChar, CChar *mChar, CItem *item, UI16 amount = 0 ) ->bool ;

	auto	addItemWeight( CBaseObject *getObj, CItem *item ) ->void ;
	auto	addItemWeight( CChar *mChar, CItem *item ) ->void ;
	auto	addItemWeight( CItem *pack, CItem *item ) ->void ;

	auto	subtractItemWeight( CBaseObject *getObj, CItem *item ) ->void ;
	auto	subtractItemWeight( CChar *mChar, CItem *item ) ->void ;
	auto	subtractItemWeight( CItem *pack, CItem *item ) ->void ;
};

extern CWeight *Weight;

#endif

