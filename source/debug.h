//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  debug.h
//
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
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
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
 #include <stdarg.h>
 #include <windows.h>
#else
 #include <stdarg.h>
 #include <time.h>
 //#include <varargs.h>
#endif
          

//---------------------------------------------------------------------------
//     Prototypes of functions
//---------------------------------------------------------------------------
void MessageReady(char *OutputMessage);
#ifdef WIN32
	void LogMessageF(unsigned char Type, long Line, char *File, char *Message, ...);
#else
	void LogMessageF(unsigned char Type, long Line, char *File, char *Message, ...);
#endif
//---------------------------------------------------------------------------
//     Translator macros for functions
//---------------------------------------------------------------------------
#ifdef WIN32
  #define LogMessage(Message)	LogMessageF('M', __LINE__, DBGFILE, Message)
#else
  #define LogMessage(Message)	LogMessageF('M', __LINE__, DBGFILE, Message)
#endif
#define LogWarning(WarningMessage)	LogMessageF('W', __LINE__, DBGFILE, WarningMessage)
#define LogError(ErrorMessage)	LogMessageF('E', __LINE__, DBGFILE, ErrorMessage)
#define LogCritical(CriticalMessage)	LogMessageF('C', __LINE__, DBGFILE, CriticalMessage)

#define _ ,

#endif
