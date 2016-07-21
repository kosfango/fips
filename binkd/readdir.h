#ifndef _readdir_h
#define _readdir_h

#if defined(__WATCOMC__) || defined(VISUALCPP)
#include <direct.h>
#include <sys/utime.h>
#else
#include <dirent.h>
#include <utime.h>
#endif

#if defined(VISUALCPP)
#include "dirwin32.h"
#endif

#endif
