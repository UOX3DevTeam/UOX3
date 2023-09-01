//
//  useful.h
//  uox3
//
//  Created by charles on 9/1/23.
//

#ifndef useful_h
#define useful_h
// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
#define _QUOTE( x ) # x
#define QUOTE( x ) _QUOTE( x )
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
#define NOTE( x )  message( x )
#define FILE_LINE  message( __FILE__LINE__ )
#define TODO( x )  message( __FILE__LINE__"\n"           \
"+------------------------------------------------\n" \
"|  TODO :   " #x "\n" \
"+-------------------------------------------------\n" )
#define FIXME( x )  message(  __FILE__LINE__"\n"           \
"+------------------------------------------------\n" \
"|  FIXME :  " #x "\n" \
"+-------------------------------------------------\n" )
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" )
#define fixme( x )  message( __FILE__LINE__" FIXME:   " #x "\n" )
#define note( x )  message( __FILE__LINE__" NOTE :   " #x "\n" )

#endif /* useful_h */
