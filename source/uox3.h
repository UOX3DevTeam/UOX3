//o-----------------------------------------------------------------------------------------------o
//| File		-	UOX3.h
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	This is the main UOX3 header file
//o-----------------------------------------------------------------------------------------------o
// ************************************************************************************************
// Disclaimer
//   Normally, one shouldn't need a "project" include, one should include the files one needs.
//   It helps in determining dependencies.  However, given this project "does" appear to have
//   a set of common dependencies, we can include them here, and then every file includes this one
// ************************************************************************************************
#ifndef __UOX3_H
#define __UOX3_H
#include "Prerequisites.h" // Pick up UOX3 symbols , this includes ConfigOS for OS symbols

#include "typedefs.h"     // UOX3 common typedefs

#include "enums.h"  // torn on this one, as we can/should forward declare.  But ok



#include "uoxstruct.h" // Same as above, but a lot was inline



#endif // __UOX3_H
