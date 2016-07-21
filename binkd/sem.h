
#ifndef _SEM_H_
#define _SEM_H_

#if defined(WIN32)

#include <windows.h>
typedef HANDLE MUTEXSEM;

#elif defined(OS2)

#ifdef __WATCOMC__
#define __IBMC__ 0
#define __IBMCPP__ 0
#endif

#define INCL_DOS
#include <os2.h>
typedef HMTX MUTEXSEM;

#elif defined(AMIGA)

#include <exec/exec.h>
typedef struct SignalSemaphore MUTEXSEM;

#endif


/*
 *    Initialise Semaphores.
 */

int _InitSem (void *);

#if defined(HAVE_THREADS) || defined(AMIGA)
#define InitSem(vpSem) _InitSem(vpSem)
#else
#define InitSem(vpSem)			    /* Do Nothing */
#endif

/*
 *    Clean Semaphores.
 */

int _CleanSem (void *);

#if defined(HAVE_THREADS) || defined(AMIGA)
#define CleanSem(vpSem) _CleanSem(vpSem)
#else
#define CleanSem(vpSem)			    /* Do Nothing */
#endif

/*
 *    Wait & lock semaphore
 */

int _LockSem (void *);

#if defined(HAVE_THREADS) || defined(AMIGA)
#define LockSem(vpSem) _LockSem(vpSem)
#else
#define LockSem(vpSem)			    /* Do Nothing */
#endif

/*
 *    Release Semaphore.
 */

int _ReleaseSem (void *);

#if defined(HAVE_THREADS) || defined(AMIGA)
#define ReleaseSem(vpSem) _ReleaseSem(vpSem)
#else
#define ReleaseSem(vpSem)		    /* Do Nothing */
#endif

#endif
