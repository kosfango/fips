#ifndef _sys_h
#define _sys_h

  #include <io.h>
  #include <windows.h>
  #include <winsock.h>
  #include <dos.h>
  #include <process.h>
  #define PID() ((int)(0xffff & GetCurrentThreadId()))

#ifndef F_OK
  #define F_OK 0
#endif

  #define RENAME(f,t) rename(f,t)
  #define sleep(a) Sleep(a*1000)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

/*
 * Get free space in a directory
 */
unsigned long getfree (char *path);

/*
 * Set up break handler, set up exit list if needed
 */
int set_break_handlers ();


#endif
