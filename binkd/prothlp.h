#ifndef _prothlp_h
#define _prothlp_h

#include "ftnq.h"

/* A file in transfer */
typedef struct _TFILE TFILE;
struct _TFILE
{
  char path[MAXPATHLEN + 1];		    /* It's name */
  char flo[MAXPATHLEN + 1];		    /* It's .?lo */
  char netname[MAX_NETNAME + 1];	    /* It's "netname" */
  char action;				    /* 'd'elete, 't'runcate, '\0' -- * *
					     * none */
  char type;
  size_t size;
  time_t start;				    /* The transfer started at... */
  time_t time;				    /* Filetime */
  FILE *f;
};

#define TF_ZERO(a) (memset(a, 0, sizeof(*a)))

int tfile_cmp (TFILE *a, char *netname, size_t size, time_t time);

/* Files to kill _after_ session */
typedef struct _KILLLIST KILLLIST;
struct _KILLLIST
{
  char name[MAXPATHLEN + 1];		    /* file's name */
  char cond;				    /* after 's'uccessful session,
					     * '\0' -- in any case */
};

/* Adds a file to killlist */
void add_to_killlist (KILLLIST **killlist, int *n_killlist, char *name, int cond);
void q_to_killlist (KILLLIST **killlist, int *n_killlist, FTNQ *q);
void free_killlist (KILLLIST **killlist, int *n_killlist);
void process_killlist (KILLLIST *killlist, int n_killlist, int flag);

/* List of files received in the current batch */
typedef struct _RCVDLIST RCVDLIST;
struct _RCVDLIST
{
  char name[MAXPATHLEN + 1];		    /* file's name */
};

/* Adds a file to rcvdlist */
void add_to_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist, char *name);
void free_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist);

/* Creates a netname from a local name */
void netname (char *s, TFILE *q);

#endif
