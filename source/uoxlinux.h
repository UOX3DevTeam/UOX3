#ifndef __UOXLINUX_H__
#define __UOXLINUX_H__

#include <string>
#include <unistd.h> // for chdir and getcwd
#include <sys/stat.h> // for mkdir
#include <errno.h> // for errno
using namespace std;

//	  This header is kind of a patch for all the windows-specific things in
//	 uox3. --Sean

inline int stricmp( const char *s1, const char *s2 )
{
	return strcasecmp( s1, s2 );
}

inline int strnicmp( const char *s1, const char *s2, size_t n )
{
	return strncasecmp( s1, s2, n );
}

inline void GetCurrentDirectory( size_t size, char *buf )
{
	getcwd( buf, size );
}

inline int _chdir( const char *newDirectory )
{
	return chdir( newDirectory );
}

inline int _mkdir( const char *dirname )
{
	return mkdir( dirname, (mode_t)0777 );
}

// This function converts a windows line read from a file into
// Unix format by stripping a trailing '\r'
// Implemented for world loading so Windows/Unix can share the
// same world files.
inline void trimWindowsText(char *buffer)
{
        const UI32 lastchar = strlen(buffer) - 1;
	if ( buffer[lastchar] == '\r' )
	        buffer[lastchar] = '\0';
}
#endif
