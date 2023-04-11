
## Description  
This is a collection of potentially useful c++ routines and objects for some common needs.
The library should compile on macOS, Linux, and Windows. 

Contents:  
 - buffer.hpp/cpp  
 	- Provides a means to read/write standard integral and string types to a memory buffer  
 - color.hpp/cpp  
 	- A generic color object, primarily intended to be used with image.hpp/cpp  
 - console.hpp/cpp  
 	- Routines to generate the standard escape sequences for console manipulation (colors, cursor position,etc)  
 - filemap.hpp/cpp  
 	- Memory maps files into the address space
 - image.hpp/cpp  
 	- A generic bitmap object and routine to save/import from non compressed bmp files.  
 - netutil.hpp/cpp  
 	- Generic object/routines for network access: initialize/shutdown network(Windows only), errormessage (used by sock.hpp/cpp)  
 - numinc.hpp  
 	- A simple number generator for one up counts.  Allows for a max value.  Useful for serial number generation of objects.  
 - random.hpp  
 	- This is a third party (not developed by myself) template for generating random numbers.  It has its own license.  
 - sock.hpp/cpp  
 	- Is a generic socket class for network operations: send,receive,connect,etc. 
 - strutil.hpp  
 	- Provides multitude of methods to operate on strings: lower,upper,trim,parse,split,dump,etc.  
 - timer.hpp/cpp  
 	- Simple timer object
 
## Building  
Ensure cmake is installed. From the top level directory of the source tree:  
 - `mkdir build`  
 - `cd build`  
 - `cmake ..  -DCMAKE_BUILD_TYPE=Release -G"NMake Makefiles"` (Windows)  
 - `cmake ..  -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"` (all other OS)  
 - `cmake --build . --config Release`    
 	