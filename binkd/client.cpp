#include "stdafx.h"

#include "readcfg.h"
#include "iphdr.h"
#include "iptools.h"
#include "ftnq.h"
#include "protocol.h"
#include "bsy.h"
#include "client.h"
#include "https.h"
#include "sem.h"

extern MUTEXSEM hostsem;
extern int BINKD_STATUS;

static int call (void *arg);

SOCKET sockfd = -1;
extern _gconfig gc;
int abort_bink(void);

int clientmgr (void *arg)
{
  extern int pidcmgr;
  extern int poll_flag;
  extern int server_flag;
  extern int client_flag;
  extern int checkcfg_flag;	       /* exit(3) on config change */

  
  int q_empty = 1;


  pidcmgr = 0;

 mailer_stat(L("S_620"));	//Session BinkP
 update_charging();
 modem_update();

  Log (4, "clientmgr started");

    FTN_NODE *r;

	if (abort_bink()) return 0;

    if (q_empty)
    {
      q_free (SCAN_LISTED);
      if (printq)
	Log (-1, "scan");
      q_scan (SCAN_LISTED);
      q_empty = !q_not_empty ();
      if (printq)
      {
	   q_list (stderr, SCAN_LISTED);
	   Log (-1, "idle");
      }
    }
      if ((r = q_next_node ()) != 0 &&
	  bsy_test (&r->fa, F_BSY) &&
	  bsy_test (&r->fa, F_CSY))
      {
         return call(r);
      }
    
  return 0;
}

static int call0 (FTN_NODE *node)
{
  int sockfd = INVALID_SOCKET;
  struct hostent he;
  struct hostent *hp;
  struct sockaddr_in sin;
  struct in_addr defaddr;
  char **cp;
  char szDestAddr[FTN_ADDR_SZ + 1];
  char *alist[2];
  int i, rc;

  char host[MAXHOSTNAMELEN + 1];       /* current host/port */
  unsigned short port;

  ftnaddress_to_str (szDestAddr, &node->fa);
  Log (2, "call to %s", szDestAddr);
  

  for (i = 1; sockfd == INVALID_SOCKET
       && (rc = get_host_and_port
	   (i, host, &port, node->hosts, &node->fa)) != -1; ++i)
  {
    if (rc == 0)
    {
      Log (1, "%s: %i: error parsing host list", node->hosts, i);
      continue;
    }

    if (!isdigit (host[0]) ||
	(defaddr.s_addr = inet_addr (host)) == INADDR_NONE)
    {
      /* If not a raw ip address, try nameserver */
      Log (5, "resolving `%s'...", host);
      LockSem(&hostsem);
      if ((hp = gethostbyname (host)) == NULL)
      {
	Log (1, "%s: unknown host", host);
	bad_try (&node->fa, "Cannot gethostbyname");
        ReleaseSem(&hostsem);
	continue;
      }
      copy_hostent(&he, hp);
      hp = &he;
      ReleaseSem(&hostsem);
    }
    else
    {
      /* Raw ip address, fake */
      hp = &he;
      hp->h_name = host;
      hp->h_aliases = 0;
      hp->h_addrtype = AF_INET;
      hp->h_length = sizeof (struct in_addr);
      hp->h_addr_list = alist;
      hp->h_addr_list[0] = (char *) &defaddr;
      hp->h_addr_list[1] = (char *) 0;
    }
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = htons (port);

    /* Trying... */

    for (cp = hp->h_addr_list; cp && *cp; cp++)
    {
      if ((sockfd = socket (hp->h_addrtype, SOCK_STREAM, 0)) == INVALID_SOCKET)
      {
	Log (1, "socket: %s", TCPERR ());
	return 0;
      }
      sin.sin_addr = *((struct in_addr *) * cp);
      LockSem(&hostsem);
      Log (4, port == DEF_PORT ? "trying %s..." : "trying %s, port %u...",
	   inet_ntoa (sin.sin_addr), (unsigned) port);
      ReleaseSem(&hostsem);
      if (h_connect (&sockfd, &sin) == 0)
      {
	 
	Log (4, "connected");
	break;
      }

      Log (1, "unable to connect: %s", TCPERR ());
      bad_try (&node->fa, TCPERR ());
      soclose (sockfd);
      sockfd = INVALID_SOCKET;
    }
    if (hp->h_addr_list != alist)
    {
      if (hp->h_addr_list && hp->h_addr_list[0])
        free(hp->h_addr_list[0]);
      if (hp->h_addr_list)
        free(hp->h_addr_list);
    }
  }

  if (sockfd == INVALID_SOCKET)
    return 0;

  protocol (sockfd, node);
  soclose (sockfd);
  return 1;
}

int call (void *arg)
{
  FTN_NODE *node = (FTN_NODE *) arg;

  if (bsy_add (&node->fa, F_CSY))
  {
    call0 (node);
    bsy_remove (&node->fa, F_CSY);
  }
  free (arg);
  return 0;
}
