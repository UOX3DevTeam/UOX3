/*
  socket_interface.h
  local include file for sockets layer - helps with porting

  programmer  rmcinnis
  date        april 08, 1997
*/
#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H

#ifndef NEXTEVEN
#define  NEXTEVEN(x)                   (((x) & 0x1) ? (x)+1 : (x))
#endif

#ifndef SM_NETWORKORDER
#define  SM_NETWORKORDER               0x4e4e /*  'NN' */
#endif

#ifndef SM_HOSTORDER
#define  SM_HOSTORDER                  0x4848 /*  'HH' */
#endif


//#if defined __WIN32__
#if defined WIN32

#ifdef USE_WINSOCK1
#  include <winsock.h>
#else
#  include <winsock2.h>
#endif

#  define HTHREAD               HANDLE

#elif defined UNIX

#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <string.h>
#  include <stricmp.h>

#  define SOCKET                int
#  define INVALID_SOCKET        -1
#  define SOCKET_ERROR          -1
#  define SOCKADDR_IN           struct sockaddr_in
#if !defined( _HPUX_) 
# if !defined (LINUX)
#  if !defined (SUN)
#   define INADDR_NONE           -1
#   define HTHREAD               thread_t
#  endif
# endif
#else
#  define HTHREAD               pthread_t
#endif
//#  define DWORD                 unsigned long
#  define INVALID_HANDLE_VALUE  -1

typedef void *(*LPTHREAD_START_ROUTINE)( void * ) ;

#endif

#endif




