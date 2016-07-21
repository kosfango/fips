
#include "stdafx.h"
#include "ftnnode.h"
#include "ftnq.h"
#include "sem.h"
#include "readcfg.h"

#if defined(HAVE_THREADS) || defined(AMIGA)
static MUTEXSEM LSem;
#endif

#ifdef HAVE_THREADS
extern MUTEXSEM hostsem;
#endif

int nNod = 0;
FTN_NODE *pNod = 0;
int nNodSorted = 0;
extern int havedefnode;

/*
 * Call this before all others functions from this file.
 */
void nodes_init ()
{
  InitSem (&LSem);
}

/*
 * Compares too nodes. 0 == don't match
 */
int node_cmp (FTN_NODE *a, FTN_NODE *b)
{
  return ftnaddress_cmp (&a->fa, &b->fa);
}

/*
 * Sorts pNod array. Must NOT be called if LSem is locked!
 */
void sort_nodes ()
{
  LockSem (&LSem);
  qsort (pNod, nNod, sizeof (FTN_NODE), (int (*) (const void *, const void *)) node_cmp);
  nNodSorted = 1;
  ReleaseSem (&LSem);
}

FTN_NODE *get_defnode_info(FTN_ADDR *fa)
{
  struct hostent *he;
  FTN_NODE n, *np;
  char host[MAXHOSTNAMELEN + 1];       /* current host/port */
  unsigned short port;
  int i;

  strcpy(n.fa.domain, "defnode");
  n.fa.z=n.fa.net=n.fa.node=n.fa.p=0;
  np = (FTN_NODE *) bsearch (&n, pNod, nNod, sizeof (FTN_NODE),
                 (int (*) (const void *, const void *)) node_cmp);

  if(!np) /* we don't have defnode info */
    return NULL;

  for(i=1;get_host_and_port(i, host, &port, np->hosts, fa)==1;i++)
  {
#ifdef HAVE_THREADS
    LockSem(&hostsem);
#endif
    he=gethostbyname(host);
#ifdef HAVE_THREADS
    ReleaseSem(&hostsem);
#endif
    if(!he) continue;
    sprintf(host+strlen(host), ":%d", port);
    i=0;
    break;
  }
  if(i) strcpy(host, "-");

  if(!add_node(fa, host, NULL, np->obox_flvr, np->obox, np->ibox, 
       np->NR_flag, np->ND_flag, np->crypt_flag, np->MD_flag, np->restrictIP)) return NULL;
  sort_nodes ();
  memcpy (&n.fa, fa, sizeof (FTN_ADDR));
  return (FTN_NODE *) bsearch (&n, pNod, nNod, sizeof (FTN_NODE),
                      (int (*) (const void *, const void *)) node_cmp);
}
/*
 * Return up/downlink info by fidoaddress. 0 == node not found
 */
FTN_NODE *get_node_info (FTN_ADDR *fa)
{
  FTN_NODE n, *np;

  if (!nNodSorted)
    sort_nodes ();
  LockSem (&LSem);
  memcpy (&n.fa, fa, sizeof (FTN_ADDR));
  np = (FTN_NODE *) bsearch (&n, pNod, nNod, sizeof (FTN_NODE),
			   (int (*) (const void *, const void *)) node_cmp);
  if((!np)&&(havedefnode)) np=get_defnode_info(fa);
  ReleaseSem (&LSem);
  return np;
}

/*
 * Add a new node, or edit old settings for a node
 *
 * 1 -- ok, 0 -- error;
 */
int add_node (FTN_ADDR *fa, char *hosts, char *pwd, char obox_flvr,
	      char *obox, char *ibox, int NR_flag, int ND_flag,
	      int crypt_flag, int MD_flag, int restrictIP)
{
  int cn;

  LockSem (&LSem);
  for (cn = 0; cn < nNod; ++cn)
  {
    if (!ftnaddress_cmp (&pNod[cn].fa, fa))
      break;
  }
  /* Node not found, create new entry */
  if (cn >= nNod)
  {
    cn = nNod;

    pNod = (struct _FTN_NODE *) xrealloc (pNod, sizeof (FTN_NODE) * ++nNod);
    memset (pNod + cn, 0, sizeof (FTN_NODE));
    memcpy (&pNod[cn].fa, fa, sizeof (FTN_ADDR));
    strcpy (pNod[cn].pwd, "-");
    pNod[cn].hosts = (char*) xstrdup ("-");
    pNod[cn].obox_flvr = 'f';
    pNod[cn].NR_flag = NR_OFF;
    pNod[cn].ND_flag = ND_OFF;
    pNod[cn].crypt_flag = CRYPT_OFF;
    pNod[cn].MD_flag = 0;
    pNod[cn].restrictIP = 0;

    /* We've broken the order... */
    nNodSorted = 0;
  }

  if(!pNod[cn].MD_flag) 
    pNod[cn].MD_flag = MD_flag;
  if(!pNod[cn].restrictIP) 
    pNod[cn].restrictIP = restrictIP;

  if (NR_flag != NR_USE_OLD)
    pNod[cn].NR_flag = NR_flag;
  if (ND_flag != ND_USE_OLD)
    pNod[cn].ND_flag = ND_flag;
  if (crypt_flag != CRYPT_USE_OLD)
    pNod[cn].crypt_flag = crypt_flag;

  if (hosts && *hosts)
  {
    free (pNod[cn].hosts);	       /* pNod[cn].hosts is never NULL */
    pNod[cn].hosts = (char*) xstrdup (hosts);
  }

  if (pwd && *pwd)
  {
    strnzcpy (pNod[cn].pwd, pwd, sizeof (pNod[cn].pwd));
  }

  if (obox_flvr != '-')
  {
    pNod[cn].obox_flvr = obox_flvr;
  }

  if (obox)
  {
    if (pNod[cn].obox)
      free (pNod[cn].obox);
    pNod[cn].obox = (char*) xstrdup (obox);
  }

  if (ibox)
  {
    if (pNod[cn].ibox)
      free (pNod[cn].ibox);
    pNod[cn].ibox = (char*) xstrdup (ibox);
  }

  ReleaseSem (&LSem);
  return 1;
}

/*
 * Iterates through nodes while func() == 0.
 */
int foreach_node (int (*func) (FTN_NODE *, void *), void *arg)
{
  int i, rc = 0;

  LockSem (&LSem);
  for (i = 0; i < nNod; ++i)
  {
    rc = func (pNod + i, arg);
    if (rc != 0)
      break;
  }
  ReleaseSem (&LSem);
  return rc;
}

static int print_node_info_1 (FTN_NODE *fn, void *arg)
{
  char szfa[FTN_ADDR_SZ + 1];

  ftnaddress_to_str (szfa, &fn->fa);
  fprintf ((FILE *) arg, "%-20.20s %s %s %c %s %s%s%s\n",
	   szfa, fn->hosts, fn->pwd,
	   fn->obox_flvr, fn->obox ? fn->obox : "-",
	   fn->ibox ? fn->ibox : "-",
	   (fn->NR_flag == NR_ON) ? " -NR" : "",
	   (fn->ND_flag == ND_ON) ? " -ND" : "");
  return 0;
}

void print_node_info (FILE *out)
{
  foreach_node (print_node_info_1, out);
}

/*
 * Create a poll for an address (in "z:n/n.p" format) (0 -- bad)
 */
int poll_node (char *s)
{
  FTN_ADDR target;

  if (!parse_ftnaddress (s, &target))
  {
    Log (1, "`%s' cannot be parsed as a Fido-style address\n", s);
    return 0;
  }
  else
  {
    char buf[FTN_ADDR_SZ + 1];

    exp_ftnaddress (&target);
    ftnaddress_to_str (buf, &target);
    Log (4, "creating a poll for %s (`%c' flavour)", buf, POLL_NODE_FLAVOUR);
    if (!get_node_info (&target))
      if (!add_node (&target, "*", 0, '-', 0, 0, 0, 0, 0, 0, 0))
	Log (1, "%s: add_node() failed", buf);
    return create_poll (&target, POLL_NODE_FLAVOUR);
  }
}

void freenode(void)
{
 int i;

 if (pNod==0) return;

 for (i=0;i<nNod;i++)
	{
	//if (pNod[i].obox) free (pNod[i].obox);
	//if (pNod[i].ibox) free (pNod[i].ibox);
	//if (pNod[i].hosts) free (pNod[i].hosts);
    }
 //if (pNod) free (pNod);
 nNod=0;  
 pNod=0;
}