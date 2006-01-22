/*
-----------------------------------------------------------------------------
This file has been adapted from OgreConfig.h, but customised to our purposes
-----------------------------------------------------------------------------
*/
#ifndef __Config_H_
#define __Config_H_

/** Define this if you don't want the accounts block to have a copy constructor
	or assignment operator
*/
#define _NOACTCOPY_ 0

/** Define this if we want to have time stamps associated with our packet logs
*/
#define P_TIMESTAMP 1

// configure options
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#endif
