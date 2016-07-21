#ifndef _tools_h
#define _tools_h

#include <stdarg.h>
#include <sys/types.h>
#ifdef VISUALCPP
#include <malloc.h>  /* for size_t */
#endif

#include "getw.h"

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif

void Log (int lev, char *s,...);

#define LOGINT(v) Log(6, "%s=%i\n", #v, (int)(v))

/*
 * (xalloc.c) [Re]allocate memory or log error
 */
void *xalloc (size_t size);
void *xrealloc (void *ptr, size_t size);
void *xstrdup (const char *str);

/*
 * Compare strings ignoring case
 */
int o_stricmp (const char *s1, const char *s2);
int o_memicmp (const void *s1, const void *s2, size_t n);
int o_strnicmp (const char *s1, const char *s2, size_t n);

#define STRICMP(a,b)    o_stricmp(a,b)
#define MEMICMP(a,b,c)  o_memicmp(a,b,c)
#define STRNICMP(a,b,c) o_strnicmp(a,b,c)

/*
 * Lowercase the string
 */
char *strlower (char *);

/*
 * Copyes not more than len chars from src into dst, but, unlike strncpy(),
 * it appends '\0' even if src is longer than len.
 */
char *strnzcpy (char *dst, const char *src, size_t len);
char *strnzcat (char *dst, const char *src, size_t len);

/*
 * Quotes all special chars. free() it's retval!
 */
#define SQ_CNTRL 1
#define SQ_SPACE 2
char *strquote (char *s, int flags);

/*
 * Reverse for strquote(), free it's return value!
 */
char *strdequote (char *s);

/*
 * Makes file system-safe names by wiping suspicious chars with '_'
 */
char *strwipe (char *s);

/*
 * 1 -- created, 0 -- already busy
 */
int create_sem_file (char *s);
int create_empty_sem_file (char *s);

/*
 */
unsigned long rnd ();

/*
 * Makes all dirs in the path
 */
int mkpath (char *s);

/*
 * Splits args ASCIIZ string onto argc separate words,
 * saves them as argv[0]...argv[argc-1]. Logs error
 * "ID: cannot parse args", if args containes less than argc words.
 */
int parse_args (int argc, char *argv[], char *src, char *ID);

/*
 * (pmatch.c) Returns true if the pattern matches the string.
 */
int pmatch (LPCSTR pattern,LPCSTR string);

/*
 * Set times for a file, 0 == success, -1 == error
 */
int touch (char *file, time_t t);

/*
 * Replaces all entries of a in s for b, returns edited line.
 * Returned value must be free()'d. Ignores case.
 */
char *ed (char *src, char *a, char *b, unsigned int *size);

/*
 * Remove/trucate a file, log this
 */
int deletefile (char *);
int trunc (char *);

/*
 * Get the string with OS name/version
 */
char *get_os_string ();

/*
 * Test netnames against some wildcards
 */
int ispkt (char *s);
int isarcmail (char *s);
int istic (char *s);

/*
 * Formats and prints argv into buf (for logging purposes)
 */
void print_args (char *buf, size_t sizeof_buf, int argc, char *argv[]);

/*
 * Dup argv
 */
char **mkargv (int argc, char **argv);

int _cdecl recv_socet(unsigned int s,char *buffer,int size,int nom);
int _cdecl send_socet(unsigned int s,char *buffer,int size,int nom);
void freeoutbound(char* outbound,char* ext);
int get_cfg_bink(int id_sect,LPCSTR key,int defval);
CString get_cfg_bink(int id_sect,LPCSTR key,LPCSTR defval);

#endif
