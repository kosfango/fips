 static const char rcsid[] =
      "$Id: getfree.c  0.01  1997/01/08 09:00:25 ufm $";

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <limits.h>

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "tools.h"
#include "config.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/


unsigned long getfree (char *path) {
char RPN[MAXPATHLEN];	// root path 
char *pRPN;             // Pointer to Root path
DWORD SPC;				// sectors per cluster 
DWORD BPS;				// bytes per sector 
DWORD FC;				// number of free clusters  
DWORD TNC;				// total number of clusters  
BOOL rc;

  pRPN = RPN;
  if (isalpha(path[0]) && path[1] == ':' ) {
	  // Drive letter
	  RPN[0] = path[0];
	  RPN[1] = ':';
	  RPN[2] = '\\';
	  RPN[3] = '\0';
  } else if (path[0] == '\\' && path[1] == '\\') {
	  // UNC path
	  int i;
      RPN[0] = '\\';
	  RPN[1] = '\\';
	  i = 2;
	  // copy server name....
      do {
		  RPN[i] = path[i];
	  } while (path[i++] != '\\');
      // .... and share name
      do {
		  RPN[i] = path[i];
	  } while (path[i++] != '\\');
      
      RPN[i] = '\0';

  } else {
	  // Current Drive
	  pRPN = NULL;
  }
  rc = GetDiskFreeSpace(pRPN,&SPC,&BPS,&FC,&TNC);
  if (rc != TRUE) {
    Log (1, "GetDiskFreeSpace error: return code = %lu", GetLastError());
    return ULONG_MAX;		    /* Assume enough disk space */
  } else {
    return (unsigned long) (BPS * SPC * FC);
  }
}
