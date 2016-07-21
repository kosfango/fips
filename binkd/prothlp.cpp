
#include "stdafx.h"
#include "srif.h"

int tfile_cmp (TFILE *a, char *netname, size_t size, time_t time)
{
  int rc = strcmp (a->netname, netname);

  if (rc == 0)
  {
    if (a->size != size)
      return a->size - size;
    if (a->time != time)
      return a->time - time;
    return 0;
  }
  else
    return rc;
}

/* Adds a file to killlist */
void add_to_killlist (KILLLIST **killlist, int *n_killlist, char *name, int cond)
{
  *killlist = (struct _KILLLIST *) xrealloc (*killlist, sizeof (KILLLIST) * (*n_killlist + 1));
  memset (*killlist + *n_killlist, 0, sizeof (KILLLIST));
  strnzcpy ((*killlist)[*n_killlist].name, name, MAXPATHLEN);
  (*killlist)[*n_killlist].cond = cond;
  ++*n_killlist;
}

void q_to_killlist (KILLLIST **killlist, int *n_killlist, FTNQ *q)
{
  FTNQ *curr;

  for (curr = q; curr; curr = curr->next)
  {
    if (curr->action == 'a')
    {
      curr->action = 0;
      add_to_killlist (killlist, n_killlist, curr->path, 0);
    }
  }
}

void free_killlist (KILLLIST **killlist, int *n_killlist)
{
  free (*killlist);
  *killlist = 0;
  *n_killlist = 0;
}

/*
 * Process killlist. flag == 's': successful session
 */
void process_killlist (KILLLIST *killlist, int n_killlist, int flag)
{
  int i;

  Log (6, "processing kill list");
  for (i = 0; i < n_killlist; ++i)
    if (killlist[i].cond != 's' || (flag == 's' && killlist[i].cond == 's'))
      deletefile (killlist[i].name);
}

/* Adds a file to rcvdlist */
void add_to_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist, char *name)
{
  *rcvdlist = (struct _RCVDLIST *) xrealloc (*rcvdlist, sizeof (RCVDLIST) * (*n_rcvdlist + 1));
  memset (*rcvdlist + *n_rcvdlist, 0, sizeof (RCVDLIST));
  strnzcpy ((*rcvdlist)[*n_rcvdlist].name, name, MAXPATHLEN);
  ++*n_rcvdlist;
}

void free_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist)
{
  free (*rcvdlist);
  *rcvdlist = 0;
  *n_rcvdlist = 0;
}

void netname (char *s, TFILE *q)
{
  static char *weekext[] = {"su", "mo", "tu", "we", "th", "fr", "sa"};

  if (q->type == 'm')
  {
    sprintf (s, "%08lx.pkt", rnd ());
  }
  else if (q->type == 'r')
  {
    sprintf (s, "%08lx.req", rnd ());
  }
  else
  {
    char *z = max (max (strrchr (q->path, '\\'), strrchr (q->path, '/')), strrchr (q->path, ':'));

    z = z ? z + 1 : q->path;
    strnzcpy (s, z, MAX_NETNAME);

    /* gul: bt+ arcshield */
    if ((z = strrchr (s, '.')) != NULL)
      if (strlen (++z) == 3)
	if ((*z >= '0') && (*z < '7') && (z[1] == '#'))
	  memcpy (z, weekext[*z - '0'], 2);

    z = strquote(s, SQ_CNTRL | SQ_SPACE);
    strnzcpy (s, z, MAX_NETNAME);
    free (z);
  }
}
