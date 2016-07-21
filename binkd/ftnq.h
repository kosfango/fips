#ifndef _ftnq_h
#define _ftnq_h

#if !defined(WIN32)
#include <sys/time.h>
#endif

#include <stdio.h>
#include "Config.h"
#include "ftnaddr.h"
#include "ftnnode.h"

typedef struct _FTNQ FTNQ;
struct _FTNQ
{
  FTNQ *next;
  FTNQ *prev;

  FTN_ADDR fa;
  char flvr;			       /* 'I', 'i', 'C', 'c', 'D', 'd', 'O',
				        * 'o', 'F', 'f', 'H', 'h' */
  char action;			       /* 'd'elete, 't'runcate, '\0' -- none,
				        * remove on 's'uccessful session,
				        * after 'a'ny session */
  char type;			       /* 'm'ail (.out), .f'l'o, '*' -- a
				        * file from .flo (just for stats, it
				        * will never be selected for sending
				        * right from the queue, it will be
				        * send when parsing its .flo instead,
				        * now it's obsolete), other -- a file
				        * to send. */
  char path[MAXPATHLEN + 1];
  unsigned long size;
  time_t time;			       /* this field seems to be used only in
				        * cmp_filebox_files(), when sorting
				        * files from a filebox before sending */

  int sent;			       /* == 1, if the file have been sent */
};

#define SCAN_LISTED ((FTNQ*)-1)

/*
 * Scans outbound. Return value must be q_free()'d.
 */
FTNQ *q_scan (FTNQ *q);
void q_free (FTNQ *q);

/*
 * Add a file to the queue.
 */
FTNQ *q_add_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type);

/*
 * Adds to the q all files for n akas stored in fa
 */
FTNQ *q_scan_addrs (FTNQ *q, FTN_ADDR *fa, int n);

/*
 * Scans fileboxes for n akas stored in fa
 */
FTNQ *q_scan_boxes (FTNQ *q, FTN_ADDR *fa, int n);

/*
 * 0 = the queue is empty.
 */
FTN_NODE *q_not_empty ();

/*
 * Selects a node to make the next call. (It's alost like
 * q_not_empty(), but it will never select a node twice)
 */
FTN_NODE *q_next_node ();

/*
 * Selects from q the next file for fa (Returns a pointer to a q element)
 */
FTNQ *select_next_file (FTNQ *q, FTN_ADDR *fa, int nAka);

/*
 * Just lists q, not more
 */
void q_list (FILE *out, FTNQ *q);

/*
 * Creates an empty .?lo
 */
int create_poll (FTN_ADDR *fa, int flvr);

/*
 * Set .hld for a node
 */
void hold_node (FTN_ADDR *fa, time_t hold_until);

/*
 * get size of files in the queue
 */
void q_get_sizes (FTNQ *q, unsigned long *netsize, unsigned long *filessize);

#define FQ_ZERO(x) (memset(x, 0, sizeof(*(x))), FA_ZERO(&((x)->fa)))
#define FQ_ISNULL(x) (FA_ISNULL(&((x)->fa)))


/*
 * Is F a good flavour letter?
 */
extern const char prio[];

#define isflvr(F) ((F) && strchr(prio,(F)))

/*
 * Compare flavours. `I' is the best, `\0' is the worst.
 */
#define MAXFLVR(a,b) ((strchr(prio, (a)) < strchr(prio, (b))) ? (a) : (b))

void bad_try (FTN_ADDR *fa, const char *error);
void good_try (FTN_ADDR *fa, char *comment);
void read_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad);
void write_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad, char *comment);

#endif
