
#include "stdafx.h"
#include <sys/stat.h>
#include "readcfg.h"
#include "readdir.h"
#include "inbound.h"

/* Removes both xxxxx.hr and it's xxxxx.dt */
static void remove_hr (char *path)
{
  int rc;

  strcpy (strrchr (path, '.'), ".dt");
  rc = deletefile (path);
  strcpy (strrchr (path, '.'), ".hr");
  if (rc == 0)
    deletefile (path);
}

static int creat_tmp_name (char *s, char *file, size_t size,
			    time_t time, FTN_ADDR *from, char *inbound)
{
  FILE *f;
  char tmp[20];
  char *t;
  char node[FTN_ADDR_SZ + 1];

  strnzcpy (s, inbound, MAXPATHLEN);
  strnzcat (s, PATH_SEPARATOR, MAXPATHLEN);
  t = s + strlen (s);
  while (1)
  {
    sprintf (tmp, "%08lx.hr", rnd ());
    strnzcat (s, tmp, MAXPATHLEN);
    if (create_empty_sem_file (s))
    {
      if ((f = fopen (s, "w")) == 0)
      {
	Log (1, "%s: %s", s, strerror (errno));
	return 0;
      }
      ftnaddress_to_str (node, from);
      fprintf (f, "%s %li %li %s\n",
	       file, (long int) size, (long int) time, node);
      fclose (f);
      break;
    }
    *t = 0;
  }
  return 1;
}

static int to_be_deleted (char *tmp_name, char *netname)
{
  struct stat sb;

  strcpy (strrchr (tmp_name, '.'), ".dt");
  if (stat (tmp_name, &sb) == 0 && kill_old_partial_files != 0 &&
      time (0) - sb.st_mtime > kill_old_partial_files)
  {
    Log (4, "found old .dt/.hr files for %s", netname);
    return 1;
  }
  strcpy (strrchr (tmp_name, '.'), ".hr");
  return 0;
}

/*
 * Searches for the ``file'' in the inbound and returns it's tmp name in s.
 * S must have MAXPATHLEN chars. Returns 0 on error, 1=found, 2=created.
 */
int find_tmp_name (char *s, char *file, size_t size,
		    time_t time, FTN_ADDR *from, int nfa, char *inbound)
{
  char buf[MAXPATHLEN + 80];
  DIR *dp;
  struct dirent *de;
  FILE *f;
  int i, found = 0;
  char *t;

  if (temp_inbound[0])
    inbound = temp_inbound;

  if ((dp = opendir (inbound)) == 0)
  {
    Log (1, "cannot opendir %s: %s", inbound, strerror (errno));
    return 0;
  }

  strnzcpy (s, inbound, MAXPATHLEN);
  strnzcat (s, PATH_SEPARATOR, MAXPATHLEN);
  t = s + strlen (s);
  while ((de = readdir (dp)) != 0)
  {
    for (i = 0; i < 8; ++i)
      if (!isxdigit (de->d_name[i]))
	break;
    if (i < 8 || STRICMP (de->d_name + 8, ".hr"))
      continue;
    strnzcat (s, de->d_name, MAXPATHLEN);
    if ((f = fopen (s, "r")) == 0 || !fgets (buf, sizeof (buf), f))
    {
      Log (1, "find_tmp_name: %s: %s", de->d_name, strerror (errno));
      if (f)
	fclose (f);
    }
    else
    {
      char *w[4];
      FTN_ADDR fa;

      fclose (f);
      FA_ZERO (&fa);
      for (i = 0; i < 4; ++i)
	w[i] = getwordx (buf, i + 1, GWX_NOESC);

      if (!strcmp (w[0], file) && parse_ftnaddress (w[3], &fa))
      {
	for (i = 0; i < nfa; i++)
	  if (!ftnaddress_cmp (&fa, from + i))
	    break;
	if (size == (size_t) atol (w[1]) &&
	    (time & ~1) == (atol (w[2]) & ~1) &&
	    i < nfa)
	{
	  found = 1;
	}
	else if (kill_dup_partial_files && i < nfa)
	{
	  Log (4, "dup partial file (%s):", w[0]);
	  remove_hr (s);
	}
      }
      else if (to_be_deleted (s, w[0]))
      {
	remove_hr (s);
      }

      for (i = 0; i < 4; ++i)
	if (w[i])
	  free (w[i]);
    }
    if (found)
      break;
    else
      *t = 0;
  }
  closedir (dp);

  /* New file */
  if (!found)
  {
    Log (5, "file not found, trying to create a tmpname");
    if (creat_tmp_name (s, file, size, time, from, inbound))
      found = 2;
    else
      return 0;
  }

  /* Replacing .hr with .dt */
  strcpy (strrchr (s, '.'), ".dt");
  return found;
}

FILE *inb_fopen (char *netname, size_t size,
		  time_t time, FTN_ADDR *from, int nfa, char *inbound)
{
  char buf[MAXPATHLEN + 1];
  struct stat sb;
  FILE *f;

  if (!find_tmp_name (buf, netname, size, time, from, nfa, inbound))
    return 0;

  if ((f = fopen (buf, "ab")) == 0)
    Log (1, "%s: %s", buf, strerror (errno));
  fseek (f, 0, SEEK_END);	       /* Work-around MSVC bug */

#if defined(OS2)
  DosSetFHState(fileno(f), OPEN_FLAGS_NOINHERIT);
#elif defined(EMX)
  fcntl(fileno(f), F_SETFD, FD_CLOEXEC);
#endif

  /* Checking for free space */
  if (fstat (fileno (f), &sb) == 0)
  {
    /* Free space req-d (Kbytes) */
    int req_free = (strcmp (inbound, inbound_nonsecure) ?
		    minfree : minfree_nonsecure);

    if (req_free >= 0 &&
	getfree (inbound) < size - sb.st_size + req_free * 1024)
    {
      Log (1, "no enough free space in %s (%li, req-d %li)",
	   inbound,
	   (long) getfree (inbound),
	   (long) size - sb.st_size + req_free * 1024l);
      fclose (f);
      return 0;
    }
  }
  else
    Log (1, "%s: fstat: %s", netname, strerror (errno));

  return f;
}

int inb_reject (char *netname, size_t size,
		 time_t time, FTN_ADDR *from, int nfa, char *inbound)
{
  char tmp_name[MAXPATHLEN + 1];

  if (find_tmp_name (tmp_name, netname, size, time, from, nfa, inbound) != 1)
  {
    Log (1, "missing tmp file for %s!", netname);
    return 0;
  }
  else
  {
    Log (2, "rejecting %s", netname);
    /* Replacing .dt with .hr and removing temp. file */
    strcpy (strrchr (tmp_name, '.'), ".hr");
    remove_hr (tmp_name);
    return 1;
  }
}

/*
 * File is complete, rename it to it's realname. 1=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_done (char *netname, size_t size, time_t time,
	      FTN_ADDR *from, int nfa, char *inbound, char *real_name)
{
  char tmp_name[MAXPATHLEN + 1];
  char *s, *u;

  *real_name = 0;

  if (find_tmp_name (tmp_name, netname, size, time, from, nfa, inbound) != 1)
  {
    Log (1, "missing tmp file for %s!", netname);
    return 0;
  }

  strnzcpy (real_name, inbound, MAXPATHLEN);
  strnzcat (real_name, PATH_SEPARATOR, MAXPATHLEN);
  s = real_name + strlen (real_name);
  strnzcat (real_name, u = strdequote (netname), MAXPATHLEN);
  free (u);
  strwipe (s);
  s = real_name + strlen (real_name) - 1;

  /* gul: for *.pkt and *.?ic (tic, zic etc.) change name but not extension */
  /* ditto for arcmail -- mff */
  if (ispkt (netname) || istic (netname) || isarcmail (netname))
    s -= 4;

  if (touch (tmp_name, time) != 0)
    Log (1, "touch %s: %s", tmp_name, strerror (errno));

  while (1)
  {
    if (!RENAME (tmp_name, real_name))
    {
      Log (2, "%s -> %s", netname, real_name);
      break;
    }
    else
    {
      if (errno != EEXIST && errno != EACCES && errno != EAGAIN)
      {
        Log (1, "cannot rename %s to it's realname: %s! (data stored in %s)",
             netname, strerror (errno), tmp_name);
        *real_name = 0;
        return 0;
      }
      Log (2, "error renaming `%s' to `%s': %s",
	   netname, real_name, strerror (errno));
    }

    if (isalpha (*s) && toupper (*s) != 'Z')
      ++*s;
    else if (isdigit (*s) && toupper (*s) != '9')
      ++*s;
    else if (*s == '9')
      *s = 'a';
    else if (*--s == '.' || *s == '\\' || *s == '/')
    {
      Log (1, "cannot rename %s to it's realname! (data stored in %s)",
	   netname, tmp_name);
      *real_name = 0;
      return 0;
    }
  }

  /* Replacing .dt with .hr and removing temp. file */
  strcpy (strrchr (tmp_name, '.'), ".hr");
  deletefile (tmp_name);
  return 1;
}

/*
 * Checks if the file already exists in our inbound. !0=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_test (char *filename, size_t size, time_t t,
	       char *inbound, char fp[])
{
  char *s, *u;
  struct stat sb;

  strnzcpy (fp, inbound, MAXPATHLEN);
  strnzcat (fp, PATH_SEPARATOR, MAXPATHLEN);
  s = fp + strlen (fp);
  strnzcat (fp, u = strdequote (filename), MAXPATHLEN);
  free (u);
  strwipe (s);

  return stat (fp, &sb) == 0 && (size_t) sb.st_size == size &&
    (sb.st_mtime & ~1) == (t & ~1);
}
