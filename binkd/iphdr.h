#ifndef _iphdrs_h
#define _iphdrs_h

#include <sys/types.h>
#include "sys.h"                            /* Get system i/o headers */

#if !defined(WIN32)
  #include <netinet/in.h>
  #ifndef __WATCOMC__
    #include <sys/param.h>		    /* One of these two should have
					     * MAXHOSTNAMELEN */
  #endif
  #include <netdb.h>			    /* One of these two should have
					     * MAXHOSTNAMELEN */
#endif

#ifdef HAVE_ARPA_INET_H
  #include <arpa/inet.h>
#endif

#ifndef MAXHOSTNAMELEN
  #define MAXHOSTNAMELEN 255		    /* max hostname size */
#endif

#define MAXSERVNAME 80                      /* max id len in /etc/services */

#ifdef IBMTCPIP
  #ifdef __WATCOMC__
    #define __IBMC__ 0
    #define __IBMCPP__ 0
  #endif
  #define BSD_SELECT
  #include <utils.h>
  #include <sys/select.h>
  #ifndef MAXSOCKETS
    #define MAXSOCKETS 2048
  #endif
#endif

#if !defined(WIN32)
  #include <sys/socket.h>
#else
  #include <winsock.h>
#endif

#if defined(IBMTCPIP)
const char *tcperr ();

  #define TCPERR() tcperr()
  #define TCPERRNO (sock_errno())
  #include <errno.h>
  #undef ENAMETOOLONG
  #undef ENOTEMPTY
  #include <nerrno.h>
  #define TCPERR_WOULDBLOCK EWOULDBLOCK
  #define TCPERR_AGAIN EAGAIN
  #define sock_deinit()
#elif defined(WIN32)
const char *tcperr ();

  #define TCPERR() tcperr()
  #define TCPERRNO (h_errno)
  #define TCPERR_WOULDBLOCK WSAEWOULDBLOCK
  #define TCPERR_AGAIN WSAEWOULDBLOCK
  #include "wsock.h"
  #define sock_init() WinsockIni()
  #define sock_deinit() WinsockClean()
  #define soclose(h) closesocket(h)
#else
  #include <errno.h>
  #define TCPERR() strerror(errno)
  #define TCPERRNO errno
  #define TCPERR_WOULDBLOCK EWOULDBLOCK
  #define TCPERR_AGAIN EAGAIN
  #define sock_init() 0
  #define sock_deinit()
  #define soclose(h) close(h)
#endif

#if !defined(WIN32)
typedef int SOCKET;

  #define INVALID_SOCKET (-1)
  #define SOCKET_ERROR (-1)
#endif

#ifndef INADDR_NONE                 
  #define INADDR_NONE -1
#endif                              

#endif
