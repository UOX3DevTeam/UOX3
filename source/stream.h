
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

/*#ifndef ios_base
	#define ios_base ios
	namespace std
	{
		inline ios& left( ios& i )
		{
			i.setf( ios::left, ios::adjustfield ); 
			return i;
		}
		inline ios& right( ios& i )
		{
			i.setf( ios::right, ios::adjustfield );
			return i;
		}
	}
#endif*/

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
