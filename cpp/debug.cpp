//------------------------------------------------------------------------
//  debug.cpp
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
//Adapted by TANiS - Thanks to Armageddon for the original code.

#include <debug.h>

/********************************************************
 *                                                      *
 *  Function to be called when a string is ready to be  *
 *    written to the log.  Insert access to your log in *
 *    this function.                                    *
 *                                                      *
 ********************************************************/
void MessageReady(char *OutputMessage)
{
	ConOut(OutputMessage);
}

/********************************************************
 *                                                      *
 *  Rountine to process and stamp a message.            *
 *                                                      *
 ********************************************************/
#ifndef __linux__
void __cdecl LogMessageF(unsigned char Type, long Line, char *File, char *Message, ...)
{
	char fullMessage[512];
	char builtMessage[512];
	SYSTEMTIME currentTime;
	va_list argptr;

	va_start(argptr, Message);
	vsprintf(builtMessage, Message, argptr);
	va_end(argptr);

	GetSystemTime(&currentTime);

	sprintf(fullMessage, "%c:%s:%i[%2d:%02d:%02d] ", Type, File, Line, currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
	strcat(fullMessage, builtMessage);
	MessageReady(fullMessage);
}

#else

void LogMessageF(unsigned char Type, long Line, char *File, char *Message, ...)
{
	char fullMessage[512];
	char builtMessage[512];
	time_t currentTime;
	tm *ct;
	va_list argptr;

	va_start(argptr, Message);
	vsprintf(builtMessage, Message, argptr);
	va_end(argptr);

	time(&currentTime);

	ct = gmtime(&currentTime);
	
	sprintf(fullMessage, "%c:%s:%ld[%2d:%02d:%02d] ", Type, File, Line, ct->tm_hour, ct->tm_min, ct->tm_sec);
	strcat(fullMessage, builtMessage);
	MessageReady(fullMessage);
}

#endif
