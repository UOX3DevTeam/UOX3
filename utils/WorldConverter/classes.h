//---Classes code by Zippy Started 7/29/99---//

#ifndef __Classes_h
#define __Classes_h

#define MAXPOSTS 128
#include <queue>
using namespace std;
#include "cBaseObject.h"

class CChar;
class CItem;
class CMultiObj;

typedef vector< CChar * > CHARLIST;
typedef vector< CItem * > ITEMLIST;

#include "CChar.h"
#include "CItem.h"

// use this value whereever you need to return an illegal z value
const SI08 illegal_z = -128;	// reduced from -1280 to -128, to fit in with a valid signed char

#include "CMultiObj.h"
#include "handlers.h"

#endif