
//o---------------------------------------------------------------------------o
//| Stream.h
//o---------------------------------------------------------------------------o
//| The file will see if you have a compilier that supports the template based
//| String Stream library, and if you do, includes it. If not, it uses the
//| older based string stream library.
//o---------------------------------------------------------------------------o
//| Version History
//| 1.0		First version
//o---------------------------------------------------------------------------o

#ifndef _STREAM_H_
#define _STREAM_H_
#include <sstream>
#endif

/* This code is causing trouble. Remming it out temporarly.
#if !defined(STREAM_H)
  #define STREAM_H
  #if defined(__unix__)
    #if __GNUC__ == 3 || (__GNUC__ == 2 && __GNUC_MINOR__ > 95)
      #include <sstream>
    #else
      #include <strstream>
      #if !defined(stringstream)
        #define stringstream strstream
      #endif
    #endif
  #else
    #include <sstream>
  #endif
#endif
*/
