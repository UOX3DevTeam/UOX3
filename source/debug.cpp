//Adapted by TANiS - Thanks to Armageddon for the original code.

#include "debug.h"
#ifndef va_start
	#include <cstdarg>
	using namespace std;
#endif
//*******************************************************
//                                                      *
//  Function to be called when a string is ready to be  *
//    written to the log.  Insert access to your log in *
//    this function.                                    *
//                                                      *
//*******************************************************
void MessageReady(char *OutputMessage)
{
	Console << OutputMessage;
}

//*******************************************************
//                                                      *
//  Routine to process and stamp a message.             *
//                                                      *
//*******************************************************
#ifdef WIN32
void LogMessageF( UI08 Type, long Line, char *File, char *Message, ...)
{
	char fullMessage[512];
	char builtMessage[512];
	SYSTEMTIME currentTime;
	va_list argptr;

#ifdef __NONANSI_VASTART__
	va_start( argptr );
#else
	va_start( argptr, Message );
#endif
	vsprintf(builtMessage, Message, argptr);
	va_end(argptr);

	GetSystemTime(&currentTime);

	sprintf(fullMessage, "%c:%s:%i[%2d:%02d:%02d] ", Type, File, Line, currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
	strcat(fullMessage, builtMessage);
	MessageReady(fullMessage);
}

#else

void LogMessageF( UI08 Type, long Line, char *File, char *Message, ...)
{
	char fullMessage[512];
	char builtMessage[512];
	time_t currentTime;
	tm *ct;
	va_list argptr;

#ifdef __NONANSI_VASTART__
	va_start( argptr );
#else
	va_start( argptr, Message );
#endif
	vsprintf(builtMessage, Message, argptr);
	va_end(argptr);

	time(&currentTime);

	ct = gmtime(&currentTime);
	
	sprintf(fullMessage, "%c:%s:%ld[%2d:%02d:%02d] ", Type, File, Line, ct->tm_hour, ct->tm_min, ct->tm_sec);
	strcat(fullMessage, builtMessage);
	MessageReady(fullMessage);
}

#endif
