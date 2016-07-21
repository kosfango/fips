#include "stdafx.h"
#include "..\structs.h"
#include <sys/stat.h>
#include <fcntl.h>
#include "assert.h"
#include "readdir.h"		       // for [sys/]utime.h
#include "readcfg.h"
#include "sem.h"
#include "tools.h"



extern _gconfig gc;

/*
 * Lowercase the string
 */
char *strlower (char *s)
{
  for (int i = 0; s[i]; ++i)
    s[i] = tolower (s[i]);
  return s;
}

int mkpath0 (const char *path0)
{
  struct stat sb;

  if (stat (path0, &sb) == -1)
  {
    char *path, *s;
    int retval;

    if ((path = strdup (path0)) == NULL)
    {
      errno = ENOMEM;
      return -1;
    }
    if ((s = max (strrchr (path, '\\'), strrchr (path, '/'))) != 0)
    {
      *(s++) = 0;
      if (*path && mkpath0 (path) == -1)
      {
				free (path);
				return -1;
      }
    }
    else
      s = path;

    if (path0[strlen (path0) - 1] == ':')
      retval = 0;		       /* we found the device part of the
				        * path */
    else
      retval = MKDIR (path0);

    free (path);
    return retval;
  }
  else if (sb.st_mode & S_IFDIR)
  {
    return 0;
  }
  else
  {
    errno = ENOTDIR;
    return -1;
  }
}

int mkpath (char *s)
{
  char path[MAX_PATH];

  strnzcpy (path, s, MAX_PATH);
  if ((s = max (strrchr (path, '\\'), strrchr (path, '/'))) == 0)
    return 0;
  *s = 0;
  return mkpath0 (path);
}

unsigned long rnd ()
{
  static int i;

  if (!i)
  {
    i = 1;
    srand (time (0));
  }
  return time (0) + rand ();
}

/*
 * 1 -- created, 0 -- already busy
 */
int create_empty_sem_file (char *name)
{
  int h;

  if ((h = open (name, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1)
    return 0;
  close (h);
  return 1;
}

int create_sem_file (char *name)
{
  int h, i;
  char buf[10];

  if ((h = open (name, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1)
  { Log (5, "Can't create %s: %s", name, strerror(errno));
    return 0;
  }
  sprintf (buf, "%i\n", (int) getpid ());
  if ((i = write(h, buf, strlen(buf))) != (int)strlen(buf))
  { if (i == -1)
      Log (2, "Can't write to %s (handle %d): %s", name, h, strerror(errno));
    else
      Log (2, "Can't write %d bytes to %s, wrote only %d", strlen(buf), name, i);
  }
  close (h);
  Log (5, "created %s", name);
  return 1;
}

#if defined(EMX) || defined(__WATCOMC__)
#include <malloc.h>		       /* for _heapchk() */
#endif

void Log (int lev, char *s,...)
{
#if defined(HAVE_THREADS) || defined(AMIGA)

  static MUTEXSEM LSem;

#endif
#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)

  extern int syslog_facility;

#endif

  static int first_time = 1;
  char timebuf[60];
  time_t t;
  struct tm *tm;
  va_list ap;
  static const char *marks = "!?+-";
  char ch = (0 <= lev && lev < (int) strlen (marks)) ? marks[lev] : ' ';

  if (first_time == 1)
  {
    InitSem (&LSem);
    first_time = 2;
  }

  time (&t);
  tm = localtime (&t);

  if (lev <= conlog)
  {
    strftime (timebuf, sizeof (timebuf), "%H:%M", tm);
    LockSem (&LSem);
    fprintf (stderr, "%30.30s\r%c %s [%i] ", " ", ch, timebuf, (int) PID ());
    va_start (ap, s);
    vfprintf (stderr, s, ap);
    va_end (ap);
    if (lev >= 0)
      fputc ('\n', stderr);
    ReleaseSem (&LSem);
    if (lev < 0)
      return;
  }

  if (lev <= loglevel && *logpath)
  {
    FILE *logfile = 0;
    int i;

    LockSem (&LSem);
    for (i = 0; logfile == 0 && i < 10; ++i)
    {
      logfile = fopen (logpath, "a");
    }
    if (logfile)
    {
      if (first_time)
      {
	fputc ('\n', logfile);
	first_time = 0;
      }
      strftime (timebuf, sizeof (timebuf), "%d %b %H:%M:%S", tm);
      fprintf (logfile, "%c %s [%i] ", ch, timebuf, (int) PID ());
      va_start (ap, s);
      vfprintf (logfile, s, ap);
      va_end (ap);
      fputc ('\n', logfile);
      fclose (logfile);
    }
    else
      fprintf (stderr, "Cannot open %s: %s!\n", logpath, strerror (errno));
    ReleaseSem (&LSem);
  }

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
  if (lev <= loglevel && syslog_facility >= 0)
  {
    static int opened = 0;
    static int log_levels[] =
    {
      /* Correspondence between binkd's loglevel and syslog's priority */
      LOG_ERR,			       /* 0 */
      LOG_WARNING,		       /* 1 */
      LOG_NOTICE,		       /* 2 */
      LOG_INFO,			       /* 3 */
      LOG_INFO,			       /* 4 */
      LOG_INFO,			       /* 5 */
      LOG_INFO,			       /* 6 */
      LOG_DEBUG			       /* other */
    };

    if (!opened)
    {
      opened = 1;
      openlog ("binkd", LOG_PID, syslog_facility);
    }

    if (lev < 0 || lev >= sizeof log_levels / sizeof (int))
      lev = sizeof log_levels / sizeof (int) - 1;

    va_start (ap, s);
    vsyslog (log_levels[lev], s, ap);
    va_end (ap);

  }
#endif

  if (lev == 0)
    exit (1);

#if defined(EMX) || defined(__WATCOMC__)
/*
  assert (_heapchk () == _HEAPOK || _heapchk () == _HEAPEMPTY);
*/
#endif
}

int o_memicmp (const void *s1, const void *s2, size_t n)
{
  int i;

  for (i = 0; i < (int) n; ++i)
    if (tolower (((char *) s1)[i]) != tolower (((char *) s2)[i]))
      return (tolower (((char *) s1)[i]) - tolower (((char *) s2)[i]));

  return 0;
}

int o_stricmp (const char *s1, const char *s2)
{
  int i;

  for (i = 0;; ++i)
  {
    if (tolower (s1[i]) != tolower (s2[i]))
      return (tolower (s1[i]) - tolower (s2[i]));
    if (!s1[i])
      return 0;
  }
}

int o_strnicmp (const char *s1, const char *s2, size_t n)
{
  int i;

  for (i = 0; i < (int) n; ++i)
  {
    if (tolower (s1[i]) != tolower (s2[i]))
      return (tolower (s1[i]) - tolower (s2[i]));
    if (!s1[i])
      return 0;
  }
  return 0;
}

/*
 * Quotes all special chars. free() it's retval!
 */
char *strquote (char *s, int flags)
{
  char *r = (char *) xalloc (strlen (s) * 4 + 1);
  int i;

  for (i = 0; *s; ++s)
  {
    if (((flags & SQ_CNTRL) && iscntrl (*s)) ||
	((flags & SQ_SPACE) && isspace (*s)))
    {
      sprintf (r + i, "\\x%02x", *(unsigned char *) s);
      i += 4;
    }
    else
      r[i++] = *s;
  }
  r[i] = 0;
  return r;
}

/*
 * Reverse for strquote(), free it's return value!
 */
char *strdequote (char *s)
{
  char *r = (char *) xstrdup (s);
  int i = 0;

  while (*s)
  {
    if (s[0] == '\\' && s[1] == 'x' && isxdigit (s[2]) && isxdigit (s[3]))
    {
#define XD(x) (isdigit(x) ? ((x)-'0') : (tolower(x)-'a'+10))
      r[i++] = XD (s[2]) * 16 + XD (s[3]);
      s += 4;
#undef XD
    }
    else
      r[i++] = *(s++);
  }
  r[i] = 0;
  return r;
}

/*
 * Makes file system-safe names by wiping suspicious chars with '_'
 */
char *strwipe (char *s)
{
  for (int i = 0; s[i]; ++i)
    if (iscntrl (s[i]) || s[i] == '\\' || s[i] == '/' || s[i] == ':')
      s[i] = '_';
  return s;
}

/*
 * Copies not more than len chars from src into dst, but, unlike strncpy(),
 * it appends '\0' even if src is longer than len.
 */
char *strnzcpy (char *dst, const char *src, size_t len)
{
  dst[len - 1] = 0;
  return strncpy (dst, src, len - 1);
}

char *strnzcat (char *dst, const char *src, size_t len)
{
  int x = strlen (dst);

  return strnzcpy (dst + x, src, len - x);
}

/*
 * Splits args ASCIIZ string onto argc separate words,
 * saves them as argv[0]...argv[argc-1]. Logs error
 * "ID: cannot parse args", if args containes less than argc words.
 */
int parse_args (int argc, char *argv[], char *src, char *ID)
{
  int i = 0;

  while (i < argc)
  {
    while (*src && isspace (*src))
      ++src;
    if (!*src)
      break;
    argv[i] = src;
    while (*src && !isspace (*src))
      ++src;
    ++i;
    if (!*src)
      break;
    src++[0] = 0;
  }
  if (i < argc)
  {
    Log (1, "%s: cannot parse args", ID, src);
    return 0;
  }
  else
    return 1;
}

/*
 * Set times for a file, 0 == success, -1 == error
 */
int touch (char *file, time_t t)
{
#ifndef OS2
  struct utimbuf utb;

  utb.actime = utb.modtime = t;
  return utime (file, &utb);
#else /* FastEcho deletes *.bsy by ctime :-( */
  APIRET r;
  FILESTATUS3 buf;
  struct stat st;

  if ((r = stat(file, &st)) == 0)
  {
    struct tm *tm;
#ifdef __WATCOMC__
    struct tm stm;
    tm = &stm;
    _localtime(&t, tm);
#else
    tm=localtime(&t);
#endif
    buf.fdateCreation.day=buf.fdateLastAccess.day=buf.fdateLastWrite.day=
        tm->tm_mday;
    buf.fdateCreation.month=buf.fdateLastAccess.month=buf.fdateLastWrite.month=
        tm->tm_mon+1;
    buf.fdateCreation.year=buf.fdateLastAccess.year=buf.fdateLastWrite.year=
        tm->tm_year-80;
    buf.ftimeCreation.twosecs=buf.ftimeLastAccess.twosecs=buf.ftimeLastWrite.twosecs=
        tm->tm_sec/2;
    buf.ftimeCreation.minutes=buf.ftimeLastAccess.minutes=buf.ftimeLastWrite.minutes=
        tm->tm_min;
    buf.ftimeCreation.hours=buf.ftimeLastAccess.hours=buf.ftimeLastWrite.hours=
        tm->tm_hour;
    buf.cbFile = buf.cbFileAlloc = st.st_size;
    buf.attrFile = FILE_ARCHIVED | FILE_NORMAL;
    r=DosSetPathInfo(file, FIL_STANDARD, &buf, sizeof(buf), 0);
    if (r == 32)
      r = 0; /* Can't touch opened *.bsy */
    if (r)
      Log (1, "touch: DosSetPathInfo(%s) retcode %d", file, r);
  }
  return (r!=0);
#endif
}

/*
 * Replaces all entries of a in s for b, returns edited line.
 * Returned value must be free()'d. Ignores case.
 * size parameter can be used only if "s" was created by malloc()
 */
char *ed (char *s, char *a, char *b, unsigned int *size)
{
   int i, j;
   unsigned int sz;
   int len_a=a?strlen(a):0;
   int len_b=b?strlen(b):0;
   char *r=s;
   
   if((!len_a)||(!s)) return r;
   if(!size) 
   {
     sz=strlen(s)+1;
     r=(char *) xstrdup(s);
   }
   else sz=*size;
   for(i=j=0;i<(int)strlen(r);i++)
   {
     if(tolower(r[i])!=tolower(a[j++]))
     {
       j=0;
       continue;
     }
     if(a[j]) continue;
     if(strlen(r)-len_a+len_b>sz)
     {
       if(len_b<64) sz+=64;
       else sz+=len_b;
       r=(char *) xrealloc(r, sz);
     }
     i-=len_a-1;
     memmove(r+i+len_b, r+i+len_a, strlen(r+i+len_a)+1);
     if(len_b)
       memcpy(r+i, b, len_b);
   }
   if(size) *size=sz;
   return r;
}

/*
 * Remove/trucate a file, log this
 */
int deletefile (char *path)
{
  int rc;

  if ((rc = unlink (path)) != 0)
    Log (1, "error unlinking `%s': %s", path, strerror (errno));
  else
    Log (5, "unlinked `%s'", path);

  return rc;
}

int trunc (char *path)
{
  int h;

  if ((h = open (path, O_WRONLY | O_TRUNC)) == -1)
  {
    Log (1, "cannot truncate `%s': %s", path, strerror (errno));
    return -1;
  }
  else
  {
    Log (4, "truncated %s", path);
    close (h);
    return 0;
  }
}

/*
 * Get the string with OS name/version
 */
#ifdef HAVE_UNAME
#include <sys/utsname.h>
#endif

char *get_os_string ()
{
  static char os[80];

  strcpy (os, "/");

#ifdef HAVE_UNAME
  {
    struct utsname name;

    if (uname (&name) == 0)
    {
      strnzcat (os, name.sysname, sizeof (os));
      return os;
    }
  }
#endif

#ifdef OS
  strnzcat (os, OS, sizeof (os));
#else
  *os = 0;
#endif

  return os;
}

/*
 * Test netnames against some wildcards
 */
int ispkt (char *s)
{
  return pmatch ("*.[Pp][Kk][Tt]", s);
}

int istic (char *s)
{
  return pmatch ("*.?[Ii][Cc]", s);
}

int isarcmail (char *s)
{
  /* *.su? *.mo? *.tu? *.we? *.th? *.fr? *.sa? */
  return (pmatch ("*.[Ss][Uu]?", s) ||
	  pmatch ("*.[Mm][Oo]?", s) ||
	  pmatch ("*.[Tt][Uu]?", s) ||
	  pmatch ("*.[Ww][Ee]?", s) ||
	  pmatch ("*.[Tt][Hh]?", s) ||
	  pmatch ("*.[Ff][Rr]?", s) ||
	  pmatch ("*.[Ss][Aa]?", s));
}

/*
 * Formats and prints argv into buf (for logging purposes)
 */
void print_args (char *buf, size_t sizeof_buf, int argc, char *argv[])
{
  int i, j, quote;

  assert (sizeof_buf > 5);
  *buf = 0;
  for (i = 0; i < argc; ++i)
  {
    quote = 0;
    if (argv[i][0] == 0)
      quote = 1;
    else
      for (j = 0; argv[i][j]; ++j)
	if (argv[i][j] <= ' ')
	{
	  quote = 1;
	  break;
	}

    strnzcat (buf, " ", sizeof_buf);
    if (quote)
      strnzcat (buf, "\"", sizeof_buf);
    strnzcat (buf, argv[i], sizeof_buf);
    if (quote)
      strnzcat (buf, "\"", sizeof_buf);
  }
}

/*
 * Dup argv
 */
char **mkargv (int argc, char **argv)
{
  int i;
  char **p;

  p = (char **) xalloc ((argc + 1) * sizeof (p));

  for (i = 0; i < argc; i++)
    p[i] = (char *) xstrdup (argv[i]);

  p[i] = NULL;

  return p;
}
int _cdecl recv_socet(unsigned int s,char *buffer,int size,int nom)
{
  FILE *pf;

  int n;
  n=recv(s,buffer,size,nom);
  if (gc.displaylevel==3) 
  {
  pf=fopen("mescomr.log","a+b");
  fwrite(buffer,size,1,pf); fclose(pf);
  }
return n;
}

int _cdecl send_socet(unsigned int s,char *buffer,int size,int nom)
{
  FILE       *pf;
  if (gc.displaylevel==3) 
  {
  pf=fopen("mescomw.log","a+b");
  fwrite(buffer,size,1,pf); fclose(pf);
  }
return send(s,buffer,size,nom);
}
void freeoutbound(char* outbound,char* ext)
{
 char buf[500];
 char buf2[500];
 long hfile;
 _finddata_t se;

 strcpy(buf,outbound); strcat(buf,"\\");
 strcpy(buf2,buf); strcat(buf2,"*.*");
 hfile = _findfirst(buf2,&se);
 if (hfile != -1L)
 {
  do
 {  strcpy(buf2,buf); 
    strcat(buf2,se.name);
	if (pmatch (ext, se.name)) deletefile(buf2);
   }
 while (_findnext(hfile,&se)==0);
 _findclose(hfile);
 }

}
// ===============================================
 int get_cfg_bink(int id_sect,LPCSTR key,int defval)
// ===============================================
 // 1-параметр - Номер секции босса
 // 2-параметр - считываемый ключ
 // 3-параметр - значение по умолчанию
 {
  char bosses[20]="Boss";
  char path[500];
  strcpy(path,gc.BasePath);
  strcat(path,"\\bink.ini");
  
  sprintf(bosses+4,"%d",id_sect);	
  return GetPrivateProfileInt(bosses,key,defval,path);
  
 }
// ====================================================
CString get_cfg_bink(int id_sect,LPCSTR key,LPCSTR defval)
// ====================================================
// 1-параметр - Номер секции босса
 // 2-параметр - считываемый ключ
 // 3-параметр - значение по умолчанию
{ char bosses[20]="Boss";
  char path[500];
  char buffer[500];
  strcpy(path,gc.BasePath);
  strcat(path,"\\bink.ini");
  
  sprintf(bosses+4,"%d",id_sect);	
  GetPrivateProfileString(bosses,key,defval,buffer,499,path);
  return buffer;
}