#include "uox3.h"
#ifndef DEBUG_H
#define DEBUG_H


//---------------------------------------------------------------------------
//     Prototypes of functions
//---------------------------------------------------------------------------
void MessageReady(char *OutputMessage);
#ifdef WIN32
	void LogMessageF( UI08 Type, long Line, char *File, char *Message, ...);
#else
	void LogMessageF( UI08 Type, long Line, char *File, char *Message, ...);
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
