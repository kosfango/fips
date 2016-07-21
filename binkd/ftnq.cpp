
#include "stdafx.h"
#include <sys/stat.h>
#include "readcfg.h"
#include "readdir.h"
#include "ftnq.h"

const char prio[] = "IiCcDdOoFfHh";
static const char flo_flvrs[] = "icdfhICDFH";
static const char out_flvrs[] = "icdohICDOH";

static FTNQ *q_add_dir (FTNQ *q, char *dir, FTN_ADDR *fa1);
FTNQ *q_add_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type);

/*
 * q_free(): frees memory allocated by q_scan()
 */
static int qn_free (FTN_NODE *fn, void *arg)
{
  fn->hold_until = 0;
  fn->mail_flvr = fn->files_flvr = 0;
  fn->busy = 0;
  return 0;
}

void q_free (FTNQ *q)
{
  if (q != SCAN_LISTED)
  {
    FTNQ *last = 0;

    while (q)
    {
      last = q;
      q = q->next;
    }
    while (last)
    {
      if (!last->prev)
      {
	free (last);
	break;
      }
      else
      {
	last = last->prev;
	if (last->next)
	  free (last->next);
      }
    }
  }
  else
    foreach_node (qn_free, 0);
}

/*
 * q_scan: scans outbound. Return value must be q_free()'d.
 */
static int qn_scan (FTN_NODE *fn, void *arg)
{
  char buf[FTN_ADDR_SZ + 1];

  ftnaddress_to_str (buf, &fn->fa);
  *(FTNQ **) arg = q_scan_boxes (*(FTNQ **) arg, &fn->fa, 1);
  return 0;
}

FTNQ *q_scan (FTNQ *q)
{
  char *s;
  char buf[MAXPATHLEN + 1], outb_path[MAXPATHLEN + 1];

  extern FTN_DOMAIN *pDomains;
  FTN_DOMAIN *curr_domain;

  for (curr_domain = pDomains; curr_domain; curr_domain = curr_domain->next)
  {
    DIR *dp;
    struct dirent *de;
    int len;

    if (curr_domain->alias4 == 0)
    {
      strnzcpy (outb_path, curr_domain->path, sizeof (buf) - 1);
/* `FOO:/bar means FOO:\..\bar on Amiga */
#ifndef AMIGA
      if (outb_path[strlen (outb_path) - 1] == ':')
	strcat (outb_path, PATH_SEPARATOR);
#endif

      if ((dp = opendir (outb_path)) == 0)
      {
	Log (1, "cannot opendir %s: %s", outb_path, strerror (errno));
	continue;
      }

      len = strlen (curr_domain->dir);
      strnzcpy (buf, curr_domain->path, sizeof (buf));
      strnzcpy (buf + strlen (buf), PATH_SEPARATOR, sizeof (buf) - strlen (buf));
      s = buf + strlen (buf);

      while ((de = readdir (dp)) != 0)
      {
	if (!STRNICMP (de->d_name, curr_domain->dir, len) &&
	    (de->d_name[len] == 0 ||
	     (de->d_name[len] == '.' && isxdigit (de->d_name[len + 1]))))
	{
	  FTN_ADDR fa;

	  FA_ZERO (&fa);
	  fa.z = ((de->d_name[len] == '.') ?
		  strtol (de->d_name + len + 1, (char **) NULL, 16) :
		  curr_domain->z[0]);
	  if (de->d_name[len] == 0 || fa.z != curr_domain->z[0])
	  {
	    strcpy (fa.domain, curr_domain->name);
	    strnzcpy (buf + strlen (buf), de->d_name, sizeof (buf) - strlen (buf));
	    q = q_add_dir (q, buf, &fa);
	  }
	  *s = 0;
	}
      }
      closedir (dp);
    }
  }
  foreach_node (qn_scan, &q);
  return q;
}

/*
 * Adds to the q all files for n akas stored in fa
 */
FTNQ *q_scan_addrs (FTNQ *q, FTN_ADDR *fa, int n)
{
  char buf[MAXPATHLEN + 1];
  int i;
  char *s;

  for (i = 0; i < n; ++i)
  {
    ftnaddress_to_filename (buf, fa + i);
    if (*buf)
    {
      if ((s = max (strrchr (buf, '\\'), strrchr (buf, '/'))) != 0)
      {
	*s = 0;
	q = q_add_dir (q, buf, fa + i);
      }
    }
  }
  q = q_scan_boxes (q, fa, n);
  return q;
}

static int weight_by_name (char *s)
{
  if (ispkt (s))
    return 100;
  if (isarcmail (s))
    return 50;
  if (istic (s))
    return -100;
  return 0;
}

static int cmp_filebox_files (FTNQ *a, FTNQ *b)
{
  int w_a = weight_by_name (a->path);
  int w_b = weight_by_name (b->path);

  if (w_a - w_b == 0)
    return b->time - a->time;
  else
    return w_a - w_b;
}

#ifdef MAILBOX
static char to32(int N)
{
    if ((N >= 0) && (N <=9)) return '0'+N;
    if ((N > 9) && (N < 32)) return 'A'+ N - 10;
    return 'Z'; /* fake return */
};

static struct {
    char * ext;
    char   flv;
} brakeExt[] =
{
#ifdef UNIX
    {"immediate", 'i'},
    {"crash",     'c'},
    {"direct",    'd'},
    {"normal",    'f'},
    {"hold",      'h'},
#endif
    {"Immediate", 'i'},
    {"Crash",     'c'},
    {"Direct",    'd'},
    {"Normal",    'f'},
    {"Hold",      'h'}
};
#endif

static FTNQ *q_scan_box (FTNQ *q, FTN_ADDR *fa, char *boxpath, char flvr, int deleteempty)
{
  FTNQ *files = NULL;
  int n_files = 0, i;
  DIR *dp;
  char buf[MAXPATHLEN + 1], *s;
  struct dirent *de;
  struct stat sb;

  strnzcpy (buf, boxpath, sizeof (buf));
  strnzcat (buf, PATH_SEPARATOR, sizeof (buf));
  s = buf + strlen (buf);
  if ((dp = opendir (boxpath)) != NULL)
  {
    while ((de = readdir (dp)) != 0)
    {
      strnzcat (buf, de->d_name, sizeof (buf));
      if (stat (buf, &sb) == 0 && (sb.st_mode & S_IFDIR) == 0 &&
	  de->d_name[0] != '.')
      {
	files = (struct _FTNQ *) xrealloc (files, sizeof (FTNQ) * (n_files + 1));
	strcpy (files[n_files].path, buf);
	files[n_files].time = sb.st_mtime;
	memcpy (&files[n_files].fa, fa, sizeof (FTN_ADDR));
	files[n_files].flvr = flvr;
	++n_files;
      }
      *s = 0;
    }
    closedir (dp);
    if (n_files == 0 && deleteempty) {
      if (rmdir (boxpath) == 0)
        Log (3, "Empty filebox %s deleted", boxpath);
      else
        Log (1, "Cannot delete empty filebox %s: %s", boxpath, strerror (errno));
    }
  }
  if (files)
  {
    qsort (files, n_files, sizeof (FTNQ),
	   (int (*) (const void *, const void *)) cmp_filebox_files);
    for (i = 0; i < n_files; ++i)
      q = q_add_file (q, files[i].path, &files[i].fa, files[i].flvr, 'd', 0);
    free (files);
  }
  return q;
}

/*
 * Scans fileboxes for n akas stored in fa
 */
FTNQ *q_scan_boxes (FTNQ *q, FTN_ADDR *fa, int n)
{
  FTN_NODE *node;
  int i;
#ifdef MAILBOX
  char buf[MAXPATHLEN + 1];
  char *s;
  int j;
#endif

  for (i = 0; i < n; ++i)
  {
#ifndef MAILBOX
    if ((node = get_node_info (fa + i)) != NULL && node->obox != NULL)
    {
      q = q_scan_box (q, fa+i, node->obox, node->obox_flvr, 0);
    }
#else
    if ((node = get_node_info (fa + i)) != NULL && ((node->obox != NULL) || tfilebox[0] || bfilebox[0]))
    {
      if (node->obox)
        q = q_scan_box (q, fa+i, node->obox, node->obox_flvr, 0);
      if (bfilebox[0]) {
        strnzcpy (buf, bfilebox, sizeof (buf));
        strnzcat (buf, PATH_SEPARATOR, sizeof (buf));
#ifdef HAVE_SNPRINTF
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
#else
        sprintf(buf + strlen(buf),
#endif
                "%s.%u.%u.%u.%u.",
                node->fa.domain,
                node->fa.z,
                node->fa.net,
                node->fa.node,
                node->fa.p);
        s = buf + strlen(buf);
        for (j = 0; j < sizeof(brakeExt)/sizeof(brakeExt[0]); j++) {
          strnzcat (buf, brakeExt[j].ext, sizeof (buf));
          q = q_scan_box (q, fa+i, buf, brakeExt[j].flv, deleteablebox);
          *s = '\0';
        }
      }
        
      if (tfilebox[0]) {
        strnzcpy ( buf, tfilebox, sizeof (buf));
        strnzcat ( buf, PATH_SEPARATOR, sizeof (buf));
#ifdef HAVE_SNPRINTF
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
#else
        sprintf(buf + strlen(buf),
#endif
                "%u.%u.%u.%u",
                node->fa.z,
                node->fa.net,
                node->fa.node,
                node->fa.p);
        q = q_scan_box (q, fa+i, buf, 'f', deleteablebox);
        strnzcat ( buf, ".h", sizeof (buf));
        q = q_scan_box (q, fa+i, buf, 'h', deleteablebox);

        strnzcpy ( buf, tfilebox, sizeof (buf));
        strnzcat ( buf, PATH_SEPARATOR, sizeof (buf));
#ifdef HAVE_SNPRINTF
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
#else
        sprintf(buf + strlen(buf),
#endif
                "%c%c%c%c%c%c%c%c.%c%c",
                to32(node->fa.z/32),     to32(node->fa.z%32),
                to32(node->fa.net/1024), to32((node->fa.net/32)%32), to32(node->fa.net%32),
                to32(node->fa.node/1024),to32((node->fa.node/32)%32),to32(node->fa.node%32),
                to32(node->fa.p/32),     to32(node->fa.p%32));
        q = q_scan_box (q, fa+i, buf, 'f', deleteablebox);
        strnzcat (buf, "h", sizeof (buf));
        q = q_scan_box (q, fa+i, buf, 'h', deleteablebox);
      }
    }
#endif
  }

  return q;
}


void process_hld (FTN_ADDR *fa, char *path)
{
  FTN_NODE *node = get_node_info (fa);

  if (node)
  {
    FILE *f;

    if ((f = fopen (path, "r")) == NULL ||
	fscanf (f, "%ld", &node->hold_until) != 1)
    {
      node->hold_until = 0;
    }
    if (f)
      fclose (f);

    if (node->hold_until <= time (0))
    {
      node->hold_until = 0;
      deletefile (path);
    }
  }
}

void process_bsy (FTN_ADDR *fa, char *path)
{
  char *s = path + strlen (path) - 4;
  FTN_NODE *node;
  struct stat sb;

  if (stat (path, &sb) == 0 && kill_old_bsy != 0
      && time (0) - sb.st_mtime > kill_old_bsy)
  {
    char buf[FTN_ADDR_SZ + 1];

    ftnaddress_to_str (buf, fa);
    Log (2, "found old %s file for %s", s, buf);
    deletefile (path);
  }
  else if ((node = get_node_info (fa)) != 0 && node->busy != 'b' &&
	   (!STRICMP (s, ".bsy") || !STRICMP (s, ".csy")))
  {
    node->busy = tolower (s[1]);
  }
}

/*
 * Adds files from outbound directory _dir_ to _q_. _fa1_ is
 * the address wildcard for this outbound. E.g.
 *     c:\bbs\outbound\                     fa1 = 2:-1/-1.-1@fidonet
 * or
 *     c:\bbs\outbound\00030004.pnt\        fa1 = 2:3/4.-1@fidonet
 * or even
 *     c:\bbs\outbound\00030004.pnt\        fa1 = 2:3/4.5@fidonet
 */
static FTNQ *q_add_dir (FTNQ *q, char *dir, FTN_ADDR *fa1)
{
  DIR *dp;
  FTN_ADDR fa2;
  char buf[MAXPATHLEN + 1];
  int j;
  char *s;

  if ((dp = opendir (dir)) != 0)
  {
    struct dirent *de;

    while ((de = readdir (dp)) != 0)
    {
      s = de->d_name;

      for (j = 0; j < 8; ++j)
	if (!isxdigit (s[j]))
	  break;

      if (j == 8 && strchr (s, 0) - s == 12 &&
	  strchr (s, '.') - s == 8 && strrchr (s, '.') - s == 8)
      {
	/* fa2 will store dest.address for the current (de->d_name) file */
	memcpy (&fa2, fa1, sizeof (FTN_ADDR));

	if (fa1->node != -1 && fa1->p != 0)
	  sscanf (s, "%8x", &fa2.p);   /* We now in /xxxxyyyy.pnt */
	else
	  sscanf (s, "%4x%4x", &fa2.net, &fa2.node);

	/* add the file if wildcard (f1) match the address (fa2) */
	if (fa1->node == -1 || fa1->p == -1 || !ftnaddress_cmp (fa1, &fa2))
	{
	  strnzcpy (buf, dir, sizeof (buf));
	  strnzcpy (buf + strlen (buf), PATH_SEPARATOR,
		    sizeof (buf) - strlen (buf));
	  strnzcpy (buf + strlen (buf), s, sizeof (buf) - strlen (buf));

	  if (!STRICMP (s + 9, "pnt") && fa2.p == -1)
	  {
	    struct stat sb;

	    if (stat (buf, &sb) == 0 && sb.st_mode & S_IFDIR)
	      q = q_add_dir (q, buf, &fa2);
	  }
	  else
	  {
	    if (fa2.p == -1)
	      fa2.p = 0;

	    if (!STRICMP (s + 9, "bsy") || !STRICMP (s + 9, "csy"))
	      process_bsy (&fa2, buf);

	    if (get_node_info (&fa2) || is5D (fa1))
	    {
	      if (strchr (out_flvrs, s[9]) &&
		  tolower (s[10]) == 'u' && tolower (s[11]) == 't')
	      {
		/* Adding *.?ut */
		q = q_add_file (q, buf, &fa2, s[9], 'd', 'm');
	      }
	      else if (!STRICMP (s + 9, "req"))
	      {
		/* Adding *.req */
		q = q_add_file (q, buf, &fa2, 'h', 's', 'r');
	      }
	      else if (!STRICMP (s + 9, "hld"))
	      {
		process_hld (&fa2, buf);
	      }
	      else if (strchr (flo_flvrs, s[9]) &&
		       tolower (s[10]) == 'l' && tolower (s[11]) == 'o')
	      {
		/* Adding *.?lo */
		q = q_add_file (q, buf, &fa2, s[9], 'd', 'l');
	      }
	      else if (!STRICMP (s + 9, "stc"))
	      {
		/* Adding *.stc */
		q = q_add_file (q, buf, &fa2, 'h', 0, 's');
	      }
	    }
	  }
	}
      }
    }
    closedir (dp);
  }
  else
    Log (1, "cannot opendir %s: %s", dir, strerror (errno));
  return q;
}

/*
 * Add a file to the queue.
 */
FTNQ *q_add_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type)
{
  const int argc=3;
  char *argv[3];
  char str[MAXPATHLEN+1];

  if (q != SCAN_LISTED)
  {
    FTNQ *new_file;

    if (type == 's')
    { char *p;
      FILE *f;

      f = fopen(filename, "r");
      if (f == NULL)
      { Log(1, "Can't open %s: %s", filename, strerror(errno));
        return q;
      }
      if (!fgets(str, sizeof(str), f))
      { Log(1, "Incorrect status (can't fgets), ignored");
        fclose(f);
        return q;
      }
      fclose(f);
      if (*str && isspace(*str))
      { Log(1, "Incorrect status (space first), ignored");
        return q;
      }
      for (p=str+strlen(str)-1; isspace(*p); *p--='\0');
      Log(2, "Status is '%s'", str);
      if (!parse_args (argc, argv, str, "Status"))
      { Log(1, "Incorrect status, ignored");
        return q;
      }
    }

    new_file = (struct _FTNQ *) xalloc (sizeof (FTNQ));
    FQ_ZERO (new_file);

    new_file->next = q;
    if (q)
      q->prev = new_file;
    q = new_file;

    if (fa1)
      memcpy (&q->fa, fa1, sizeof (FTN_ADDR));

    q->flvr = flvr;
    q->action = action;
    q->type = type;
    q->sent = 0;

    if (type == 's')
    { q->size = atol(argv[1]);
      q->time = atol(argv[2]);
      strnzcpy (q->path, argv[0], MAXPATHLEN);
    }
    else
      strnzcpy (q->path, filename, MAXPATHLEN);
  }
  else
  {
    FTN_NODE *node = get_node_info (fa1);

    if (node)
    {
      if (type == 'm')
	node->mail_flvr = MAXFLVR (flvr, node->mail_flvr);
      else
	node->files_flvr = MAXFLVR (flvr, node->files_flvr);
    }
  }
  return q;
}

/*
 * q_list: just lists q, not more
 */

typedef struct
{
  int first_pass;
  FILE *out;
} qn_list_arg;

static int qn_list (FTN_NODE *fn, void *arg)
{
  char tmp[60], buf[FTN_ADDR_SZ + 1];
  qn_list_arg *a = (qn_list_arg *) arg;

  if (fn->mail_flvr || fn->files_flvr || fn->busy)
  {
    if (fn->hold_until > 0)
    {
      struct tm *tm;
#ifdef __WATCOMC__
      struct tm stm;
      tm = &stm;
      _localtime (&fn->hold_until, tm);
#else
      tm = localtime (&fn->hold_until);
#endif
      strftime (tmp, sizeof (tmp), " (hold until %Y/%m/%d %H:%M:%S)", tm);
    }
    else
      *tmp = 0;

    ftnaddress_to_str (buf, &fn->fa);
    fprintf (a->out, "%c %c%c %s%s%s\n",
	     a->first_pass ? '$' : ' ', fn->mail_flvr ? fn->mail_flvr : '-',
	     fn->files_flvr ? fn->files_flvr : '-', buf, tmp,
	     fn->busy ? (fn->busy == 'c' ? "\tcall" : "\tbusy") : "");
    if (a->first_pass)
      a->first_pass = 0;
  }
  return 0;
}

void q_list (FILE *out, FTNQ *q)
{
  char buf[FTN_ADDR_SZ + 1];

  if (q == SCAN_LISTED)
  {
    qn_list_arg qnla;

    qnla.first_pass = 1;
    qnla.out = out;

    foreach_node (qn_list, &qnla);
  }
  else
  {
    for (; q; q = q->next)
    {
      if (!q->sent)
      {
	ftnaddress_to_str (buf, &q->fa);
	fprintf (out, "%-20s %c%c%c %8lu %s\n",
		 buf, q->flvr, q->action ? q->action : '-',
		 q->type ? q->type : '-',
		 q->size, q->path);
      }
    }
  }
}

/*
 * Selects from q the next file for fa (Returns a pointer to a q element)
 */
FTNQ *select_next_file (FTNQ *q, FTN_ADDR *fa, int nAka)
{
  int j, k;
  FTNQ *curr;

  for (k = 0; k < nAka; ++k)
  {
    for (curr = q; curr; curr = curr->next)	/* Status first */
    {
      if (!curr->sent &&
	  (FA_ISNULL (&curr->fa) || !ftnaddress_cmp (&curr->fa, fa + k)) &&
	  curr->type == 's')
      {
	curr->sent = 1;
	return curr;
      }
    }
    for (j = 0; prio[j]; ++j)
    {
      for (curr = q; curr; curr = curr->next)	/* Netmail first */
      {
	if (!curr->sent &&
	    (FA_ISNULL (&curr->fa) || !ftnaddress_cmp (&curr->fa, fa + k)) &&
	    curr->flvr == prio[j] &&
	    curr->type == 'm')
	{
	  curr->sent = 1;
	  return curr;
	}
      }
    }
    for (j = 0; prio[j]; ++j)
    {
      for (curr = q; curr; curr = curr->next)	/* Then go files and .flo */
      {
	if (!curr->sent &&
	    (FA_ISNULL (&curr->fa) || !ftnaddress_cmp (&curr->fa, fa + k)) &&
	    curr->flvr == prio[j])
	{
	  curr->sent = 1;
	  return curr;
	}
      }
    }
  }
  return 0;
}

/*
 * get size of files in the queue
 */
void q_get_sizes (FTNQ *q, unsigned long *netsize, unsigned long *filessize)
{
  FTNQ *curr;
  struct stat st;
  char *p;

  *netsize = *filessize = 0;
  for (curr = q; curr; curr = curr->next)
  {
    if (curr->type == 'l')
    { FILE *f;
      char str[MAXPATHLEN+2];

      if ((f = fopen(curr->path, "r")) != NULL)
      {
        while (fgets (str, sizeof(str), f))
        {
          if (*str == '~' || *str == '$') continue;
          if ((p = strchr(str, '\n')) != NULL) *p='\0';
          p=str;
          if (*str == '#' || *str == '^') p++;
          if (stat(p, &st) == 0)
            *filessize += st.st_size;
        }
        fclose(f);
      }
    }
    else if (curr->type == 's')
      *filessize += curr->size;
    else if (stat(curr->path, &st) == 0)
      *(curr->type == 'm' ? netsize : filessize) += st.st_size;
  }
}

/*
 * q_not_empty () == 0: the queue is empty.
 */

typedef struct
{
  int maxflvr;
  FTN_NODE *fn;
} qn_not_empty_arg;

static int qn_not_empty (FTN_NODE *fn, void *arg)
{
  qn_not_empty_arg *a = (qn_not_empty_arg *) arg;

  if (!fn->busy && strcmp (fn->hosts, "-") && fn->hold_until < time (0))
  {
    if (a->maxflvr != MAXFLVR (fn->mail_flvr, MAXFLVR (fn->files_flvr, a->maxflvr)))
    {
      a->maxflvr = MAXFLVR (fn->mail_flvr, fn->files_flvr);
      a->fn = fn;
    }
  }
  return 0;
}

FTN_NODE *q_not_empty ()
{
  qn_not_empty_arg arg;

  arg.maxflvr = 0;
  arg.fn = 0;

  foreach_node (qn_not_empty, &arg);

  if (arg.maxflvr && tolower (arg.maxflvr) != 'h')
    return arg.fn;
  else
    return 0;
}

FTN_NODE *q_next_node ()
{
  FTN_NODE *fn = q_not_empty ();

  if (fn == 0)
    return 0;
  else
  {
    fn->mail_flvr = fn->files_flvr = 0;
    return fn;
  }
}

/*
 * Creates an empty .?lo
 */
int create_poll (FTN_ADDR *fa, int flvr)
{
  char buf[MAXPATHLEN + 1];
  char ext[5];
  int rc = 0;

  strcpy (ext, ".flo");
  if (flvr && strchr (flo_flvrs, flvr))
    ext[1] = tolower (flvr);
  ftnaddress_to_filename (buf, fa);
  if (*buf)
  {
    mkpath (buf);
    strnzcat (buf, ext, sizeof (buf));
    if ((rc = create_empty_sem_file (buf)) == 0)
      if (errno != EEXIST)
        Log (1, "cannot create %s: %s", buf, strerror (errno));
  }
  else
    Log (1, "%s: unknown domain", fa->domain);
  return rc;
}

/*
 * Set .hld for a node
 */
void hold_node (FTN_ADDR *fa, time_t hold_until)
{
  char buf[MAXPATHLEN + 1];
  char addr[FTN_ADDR_SZ + 1];
  char time[80];
  struct tm *tm;

#ifdef __WATCOMC__
  struct tm stm;
  tm = &stm;
  _localtime (&hold_until, tm);
#else
  tm = localtime (&hold_until);
#endif
  strftime (time, sizeof (time), "%Y/%m/%d %H:%M:%S", tm);
  ftnaddress_to_str (addr, fa);
  ftnaddress_to_filename (buf, fa);
  Log (2, "holding %s (%s)", addr, time);
  if (*buf)
  {
    FILE *f;
    FTN_NODE *fn = get_node_info (fa);

    strnzcat (buf, ".hld", sizeof (buf));
    if ((f = fopen (buf, "w")) != NULL)
    {
      fprintf (f, "%li", (long int) hold_until);
      if (fn)
	fn->hold_until = hold_until;
      fclose (f);
    }
    else
    {
      Log (1, "%s: %s", buf, strerror (errno));
    }
  }
}

void write_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad, char *comment)
{
  char buf[MAXPATHLEN + 1];

  if (tries > 0)
  {
    ftnaddress_to_filename (buf, fa);
    if (*buf)
    {
      FILE *f;

      strnzcat (buf, ".try", sizeof (buf));
      if ((f = fopen (buf, "wb")) != NULL)
      {
	fprintf (f, "%c%c%c%c%c%s",
		 *nok & 0xff, *nok >> 8,
		 *nbad & 0xff, *nbad >> 8,
		 (int) strlen (comment),
		 comment);
	fclose (f);
      }
    }
  }
}

void read_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad)
{
  char buf[MAXPATHLEN + 1];

  ftnaddress_to_filename (buf, fa);
  if (*buf)
  {
    FILE *f;
    unsigned char ch1, ch2, ch3, ch4;

    strnzcat (buf, ".try", sizeof (buf));
    if ((f = fopen (buf, "rb")) != NULL &&
	fscanf (f, "%c%c%c%c", &ch1, &ch2, &ch3, &ch4) == 4)
    {
      *nok = ch1 + (ch2 << 8);
      *nbad = ch3 + (ch4 << 8);
    }
    else
    {
      *nok = *nbad = 0;
    }
    if (f)
      fclose (f);
  }
}

void bad_try (FTN_ADDR *fa, const char *error)
{
  unsigned nok, nbad;

  read_try (fa, &nok, &nbad);
  if (tries > 0 && ++nbad >= (unsigned) tries)
  {
    nok = nbad = 0;
    hold_node (fa, time (0) + hold);
  }
  write_try (fa, &nok, &nbad, (char *) error);
}

void good_try (FTN_ADDR *fa, char *comment)
{
  unsigned nok, nbad;

  read_try (fa, &nok, &nbad);
  nbad = 0;
  ++nok;
  write_try (fa, &nok, &nbad, comment);
}
