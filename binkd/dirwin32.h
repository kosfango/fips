
#ifdef VISUALCPP

#ifndef _DIRWIN32_H_
#define _DIRWIN32_H_

#include <io.h>
#include <stdlib.h>

typedef struct dirent {
   // char        d_dta[ 21 ];            /* disk transfer area */
   // char        d_attr;                 /* file's attribute */
   // unsigned short int d_time;          /* file's time */
   // unsigned short int d_date;          /* file's date */
   // long        d_size;                 /* file's size */
    char        d_name[_MAX_PATH+1];  /* file's name */
   // unsigned short d_ino;               /* serial number (not used) */
   // char        d_first;                /* flag for 1st time */

   struct _finddata_t _dt;
   char               _mask[_MAX_PATH+1];
   long               _handle;
   char               _first_time;
} DIR;

DIR* opendir(const char*);
DIR* readdir(DIR*);
int  closedir(DIR*);

#endif /* _DIRWIN32_H_ */

#endif /* VISUALCPP */
