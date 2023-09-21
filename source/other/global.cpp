//

#include "global.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
//
//  We will keep our global classes here for now, until we figure out what to do.
//  UOX3 currently makes pointers for everything, and then externs those.  Not sure
//  the advanatage, other then being able to control the shutdown order, and the binary size.
//
//  For now, we will start migrating them to external globals (not pointers), and then
//  sort out, if should be global, singleton, or an actual pointer (and perhaps scoped in a better place?)
//
//======================================================================

WorldWeather worldWeather ;

