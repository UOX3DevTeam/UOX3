//------------------------------------------------------------------------
//  
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
// In addition to that license, if you are running this program or modified  *
// versions of it on a public system you HAVE TO make the complete source of *
// the version used by you available or provide people with a location to    *
// download it.                                                              *


#ifndef __DEFINES_H
#define	__DEFINES_H

//	System Includes


//	Namespace

//using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration

//	UOX3 includes


//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define NOTE( x )  message( x )
#define FILE_LINE  message( __FILE__LINE__ )

#define TODO( x )  message( __FILE__LINE__"\n""+------------------------------------------------\n""|  TODO :   " #x "\n""+-------------------------------------------------\n" )
#define FIXME( x )  message(  __FILE__LINE__"\n""+------------------------------------------------\n""|  FIXME :  " #x "\n""+-------------------------------------------------\n" )
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" ) 
#define fixme( x )  message( __FILE__LINE__" FIXME:   " #x "\n" ) 
#define note( x )  message( __FILE__LINE__" NOTE :   " #x "\n" ) 

// product info
#define VER " 0.70.03"
#define BUILD "21d"
#define SVER "1.0"
#define CVER "1.0"
#define IVER "1.0"
#define PRODUCT "Ultima Offline eXperiment 3"
#define TIMEZONE "GMT+10"
#define NAME "The Freelancers Team"
#define EMAIL "http://uox3.sourceforge.net/"
#define PROGRAMMERS "Freelancers"
//#ifdef _WIN32
//#define __NT__
//#define _MSVC
//#endif
//#ifdef __NT__
//#ifndef _WIN32
//#define _WIN32
//#endif
#endif
// remove PACKED for unix/linux because it going to cause bus errors - fur
//#if defined _WIN32 && (!defined(__MINGW32__))
#define PACK_NEEDED
//#else
//#define PACK_NEEDED
//#endif


#ifndef __linux__
#pragma warning(disable: 4786) //Gets rid of BAD stl warnings
#pragma warning(disable: 4503)
#endif

const int MapTileWidth = 768;
const int MapTileHeight = 512;
// use this value whereever you need to return an illegal z value
const signed char illegal_z = -128;	// reduced from -1280 to -128, to fit in with a valid signed char

//  Need for ILSHENAR
#define ILSHENAR 0 

#endif
