//------------------------------------------------------------------------
//  debug.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
#ifndef DEBUG_H
#define DEBUG_H
#include <uox3.h>
#include <stdio.h>
#include <string.h>
#ifndef __linux__
 #include <stdarg.h>
 #include <windows.h>
#else
 #include <stdarg.h>
 #include <time.h>
#ifdef __MINGW32__ // knox, small failure in the mingw target headers, they define CLOCKS_PER_SEC as 1000.0
	#undef CLOCKS_PER_SEC
	#define CLOCKS_PER_SEC 1000
#endif
#endif
          
//---------------------------------------------------------------------------
//     Prototypes of functions
//---------------------------------------------------------------------------
void MessageReady(char *OutputMessage);
	void __cdecl LogMessageF(unsigned char Type, long Line, char *File, char *Message, ...);
//---------------------------------------------------------------------------
//     Translator macros for functions
//---------------------------------------------------------------------------
  #define LogMessage(Message)	LogMessageF('M', __LINE__, DBGFILE, Message)
#define LogWarning(WarningMessage)	LogMessageF('W', __LINE__, DBGFILE, WarningMessage)
#define LogError(ErrorMessage)	LogMessageF('E', __LINE__, DBGFILE, ErrorMessage)
#define LogCritical(CriticalMessage)	LogMessageF('C', __LINE__, DBGFILE, CriticalMessage)

#define _ ,

#endif
