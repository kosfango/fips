 static const char rcsid[] =
      "$Id: WSock.c  0.01  1996/12/03 10:57:05 ufm $";

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include "stdafx.h"
#include <windows.h>
#include <winsock.h>


/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "tools.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                    Local functions prototypes                      */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*    int WinsockIni(void)                                            */
/*                                                                    */
/*    Initialise Winsock.                                             */
/*--------------------------------------------------------------------*/

int WinsockIni(void) 
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int Err;

    wVersionRequested = MAKEWORD( 1, 1 );

    Err = WSAStartup(wVersionRequested, &wsaData);
    if (Err != 0) {
       Log (0, "Cannot initialise WinSock");
       return (-1);
    }
    /*----------------------------------------------------------------*/
    /* than 1.1 in addition to 1.1, it will still return 1.1 in       */
    /* wVersion since that is the version we requested                */
    /*----------------------------------------------------------------*/

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
       Log (0, "WinSock %d.%d detected. Required version 1.1",LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
       WSACleanup( );
       return (-1);
    }
    return 0;
}

/*--------------------------------------------------------------------*/
/*    int WinsockClean(void)                                          */
/*                                                                    */
/*    Initialise Winsock.                                             */
/*--------------------------------------------------------------------*/

int WinsockClean(void) 
{
    WSACleanup();
    return 0;
}