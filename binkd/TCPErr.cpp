
 static const char rcsid[] =
      "$Id: tcperr.c  0.02  1996/12/15 18:58:09 ufm $";

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include "stdafx.h"
#include <winsock.h>
extern int BINKD_STATUS;
 void modem_add_listbox(const char* Str);
/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/
static const char *sockerrors[] =
{
  "Error 0",                                 // 0
  "Error 1",                                 // 1
  "Error 2",                                 // 2
  "Error 3",                                 // 3
  "The (blocking) call was canceled via WSACancelBlockingCall().", /* +4 */
  "Error 5",                                 // 5
  "No such device or address",		    /* SOCBASEERR+6 */
  "Error 7",                                 // 7
  "Error 8",                                 // 8 
  "Bad file number",			    /* SOCBASEERR+9 */
  "Error 10",                                // 10
  "Error 11",                                // 11
  "Error 12",                                // 12
  "Permission denied",			    /* SOCBASEERR+13 */
  "Bad address",			    /* SOCBASEERR+14 */
  "Error 15",
  "Error 16",
  "Error 17",
  "Error 18",
  "Error 19",
  "Error 20",
  "Error 21",
  "Invalid argument",			    /* SOCBASEERR+22 */
  "Error 23",
  "Too many open files",		    /* SOCBASEERR+24 */
  "Error 25",
  "Error 26",
  "Error 27",
  "Error 28",
  "Error 29",
  "Error 30",
  "Error 31",
  "Broken pipe",			    /* SOCBASEERR+32 */
  "Error 33",
  "Error 34",
  "Operation would block",		    /* SOCBASEERR+35 */
  "Operation now in progress",		    /* SOCBASEERR+36 */
  "Operation already in progress",	    /* SOCBASEERR+37 */
  "Socket operation on non-socket",	    /* SOCBASEERR+38 */
  "Destination address required",	    /* SOCBASEERR+39 */
  "Message too long",			    /* SOCBASEERR+40 */
  "Protocol wrong type for socket",	    /* SOCBASEERR+41 */
  "Protocol not available",		    /* SOCBASEERR+42 */
  "Protocol not supported",		    /* SOCBASEERR+43 */
  "Socket type not supported",		    /* SOCBASEERR+44 */
  "Operation not supported on socket",	    /* SOCBASEERR+45 */
  "Protocol family not supported",	    /* SOCBASEERR+46 */
  "Address family not supported by protocol family",	/* SOCBASEERR+47 */
  "Address already in use",		    /* SOCBASEERR+48 */
  "Can't assign requested address",	    /* SOCBASEERR+49 */
  "Network is down",			    /* SOCBASEERR+50 */
  "Network is unreachable",		    /* SOCBASEERR+51 */
  "Network dropped connection on reset",    /* SOCBASEERR+52 */
  "Software caused connection abort",	    /* SOCBASEERR+53 */
  "Connection reset by peer",		    /* SOCBASEERR+54 */
  "No buffer space available",		    /* SOCBASEERR+55 */
  "Socket is already connected",	    /* SOCBASEERR+56 */
  "Socket is not connected",		    /* SOCBASEERR+57 */
  "Can't send after socket shutdown",	    /* SOCBASEERR+58 */
  "Too many references: can't splice",	    /* SOCBASEERR+59 */
  "Connection timed out",		    /* SOCBASEERR+60 */
  "Connection refused",			    /* SOCBASEERR+61 */
  "Too many levels of symbolic links",	    /* SOCBASEERR+62 */
  "File name too long",			    /* SOCBASEERR+63 */
  "Host is down",			    /* SOCBASEERR+64 */
  "No route to host",			    /* SOCBASEERR+65 */
  "Directory not empty",		    /* SOCBASEERR+66 */
  "Error 67",
  "Error 68",
  "Error 69",
  "Error 70",
  "Error 71",

};

/*--------------------------------------------------------------------*/
/*                    Local functions prototypes                      */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*    int tcperr(void)                                                */
/*                                                                    */
/*    return string to winsock error.                                 */
/*--------------------------------------------------------------------*/

const char *tcperr (void) {
static char Str[512];
int err = h_errno - WSABASEERR;
BINKD_STATUS=MODEM_FAIL;
   if (err > (sizeof (sockerrors) / sizeof (char *))) 
   {   
       sprintf(Str,"TCP/IP error (%d)",err);modem_add_listbox(Str);
       return Str;
   } else 
   {
      sprintf(Str,"{%d} %s",err,sockerrors[err]);modem_add_listbox(Str);
      return Str;
   }
}
