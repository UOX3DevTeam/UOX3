//o---------------------------------------------------------------------------o
//| UOX3.h
//o---------------------------------------------------------------------------o
//| This is the main UOX3 header file
//o---------------------------------------------------------------------------o
//| Version History
//| 1.5		Added comments and changed int variables into typefefs
//|         (Mr. Fixit 5. November 2001)
//| 1.6		Removed the warning promotions from level 3 to 4 off.
//|					(EviLDeD 022602)
//|	1.7		Most all global includes are now located here (rather than included in other global includes)
//|			Early class declarations to allow removal of most files from global scope
//|			Removed a majority of header files from global scope
//|			Removed some unneeded / duplicate external header includes
//|			Moved or Removed many global variables (many to cWorldMain class)
//|			( Zane(giwo) 4/01/2003 )
//|	1.8		Moved most class assignments out of global scope, they are now extern'd
//|			In their associated files (IE extern cBoat *Boats is in cBoat.h).
//|			Removed many early class declarations that were no longer necessary.
//o---------------------------------------------------------------------------o


//o---------------------------------------------------------------------------o
// Only run once
//o---------------------------------------------------------------------------o
#ifndef __UOX3_H
#define __UOX3_H

#include "Prerequisites.h"

//o---------------------------------------------------------------------------o
// Class Declarations
//o---------------------------------------------------------------------------o

#if !defined(_MSC_VER)
	// VC6 doesn't define std::min and std::max correctly, we're told to use _MIN and _MAX instead
	#define UOX_MIN (std::min)
	#define UOX_MAX (std::max)
#elif defined( _MSC_VER ) && _MSC_VER >= 1300	 
	#define UOX_MIN (std::min)		// without the brackets, VC7.1 kicks up a stink
	#define UOX_MAX (std::max)
#else
	#define UOX_MIN (std::_MIN)
	#define UOX_MAX (std::_MAX)
#endif

//o---------------------------------------------------------------------------o
// The UOX project includes
//o---------------------------------------------------------------------------o
#include "uoxstruct.h"
#include "CDataList.h"
#include "cAccountClass.h"
#include "ustring.h"
#include "cBaseObject.h"
#include "cChar.h"
#include "cItem.h"
#include "cMultiObj.h"
#include "cConsole.h"
#include "cSocket.h"
#include "cServerData.h"
#include "worldmain.h"
#include "funcdecl.h"


namespace UOX
{
//o---------------------------------------------------------------------------o
// JS Stuff
//o---------------------------------------------------------------------------o
extern JSRuntime *jsRuntime;
extern JSContext *jsContext;
extern JSObject *jsGlobal;
extern JSClass global_class;
extern JSClass uox_class;

//o---------------------------------------------------------------------------o
// Global Variables
//o---------------------------------------------------------------------------o
extern const char skillname[SKILLS+1][20];
extern QUEUEMAP							refreshQueue;
extern QUEUEMAP							deletionQueue;

inline UI32 BuildTimeValue( R32 timeFromNow ) 
{ 
	return static_cast<UI32>( cwmWorldState->GetUICurrentTime() + ( static_cast<R32>(1000) * timeFromNow ) );	
}

}

#endif // __UOX3_H
