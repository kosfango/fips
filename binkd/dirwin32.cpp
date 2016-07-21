#include "stdafx.h"
#include "dirwin32.h"

DIR* opendir(const char* mask)
{
    DIR* dir;
    char *ch;

    if ((dir = (struct dirent *) malloc(sizeof(DIR))) == NULL) return NULL;
    dir->_first_time = 1;
    dir->_handle = -1;
    strcpy(dir->_mask,mask);
    ch = dir->_mask + strlen(dir->_mask) - 1;
    if (*ch=='\\' || *ch=='/') strcat(dir->_mask,"*");
                          else strcat(dir->_mask,"\\*");

    return dir;
}

DIR* readdir(DIR* dir)
{
    if (!dir) return NULL;

    if (dir->_first_time || dir->_handle==-1) {
      dir->_handle=_findfirst(dir->_mask,&(dir->_dt));
      if (dir->_handle==-1) return NULL;
      dir->_first_time=0;
      strcpy(dir->d_name,dir->_dt.name);
      }
    else {
      if (_findnext(dir->_handle,&(dir->_dt))==-1) return NULL;
      strcpy(dir->d_name,dir->_dt.name);
      }

    return dir; 
}


int  closedir(DIR* dir)
{
   int res;

   if (!dir) return 0;

   res = _findclose(dir->_handle);
   free(dir);

   return res==0;
}
