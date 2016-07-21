#ifndef _assert_h
#define _assert_h

#include "tools.h"

#if defined (NDEBUG)
  #define assert(exp) ((void)0)
#else
#if defined (EMX)
  #define assert(exp) if ((exp)==0) \
           { int hcore=open("c:\\binkd.core",O_BINARY|O_RDWR|O_CREAT,0600); \
             if (hcore!=-1) _core(hcore), close(hcore); \
             Log (0, "%s: %i: %s: assertion failed", __FILE__, __LINE__, #exp); \
             abort(); }
#else
  #define assert(exp) ((exp) ? (void)0 : \
          Log (0, "%s: %i: %s: assertion failed", __FILE__, __LINE__, #exp))
#endif
#endif

#endif

