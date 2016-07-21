#ifndef _ftnnode_h
#define _ftnnode_h

#include <stdio.h>
#include "Config.h"
#include "ftnaddr.h"
#include "iphdr.h"

#define MAXPWDLEN 40

typedef struct _FTN_NODE FTN_NODE;
struct _FTN_NODE
{
  char *hosts;			       /* "host1:port1,host2:port2,*" */

  FTN_ADDR fa;
  char pwd[MAXPWDLEN + 1];
  char obox_flvr;
  char *obox;
  char *ibox;
  int NR_flag;
  int ND_flag;
  int MD_flag;
  int restrictIP;
  int crypt_flag;

  time_t hold_until;
  int busy;			       /* 0=free, 'c'=.csy, other=.bsy */
  int mail_flvr;		       /* -1=no mail, other=it's flavour */
  int files_flvr;		       /* -1=no files, other=it's flavour */
#ifdef HTTPS
  unsigned long current_addr;
#endif
};

/*
 * Call this before all others functions from this file.
 */
void nodes_init ();

/*
 * Return up/downlink info by fidoaddress. 0 == node not found
 */
FTN_NODE *get_node_info (FTN_ADDR *fa);

/*
 * Compares too nodes. 0 == don't match
 */
int node_cmp (FTN_NODE *a, FTN_NODE *b);

/*
 * Add a new node, or edit old settings for a node
 *
 * 1 -- ok, 0 -- error;
 */
int add_node (FTN_ADDR *fa, char *hosts, char *pwd, char obox_flvr,
	      char *obox, char *ibox, int NR_flag, int ND_flag,
	      int crypt_flag, int MD_flag, int restrictIP);

#define CRYPT_ON	 1
#define CRYPT_OFF	 0
#define CRYPT_USE_OLD	-1		/* Use old value */


#define NR_ON       1
#define NR_OFF      0
#define NR_USE_OLD -1		       /* Use old value */

#define ND_ON       1
#define ND_OFF      0
#define ND_USE_OLD -1		       /* Use old value */

/*
 * Iterates through nodes while func() == 0.
 */
int foreach_node (int (*func) (FTN_NODE *, void *), void *);

/*
 * Dump node list
 */
void print_node_info (FILE *out);

/*
 * Create a poll for an address (in "z:n/n.p" format) (0 -- bad)
 */
#define POLL_NODE_FLAVOUR 'i'
int poll_node (char *s);
void freenode(void);
#endif
