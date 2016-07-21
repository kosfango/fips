#include "stdafx.h"
#include "iphdr.h"
#include "iptools.h"
#include "readcfg.h"
#include "protocol.h"
#include "server.h"
#include "sem.h"
extern MUTEXSEM hostsem;

extern int n_servers ;
extern int ext_rand ;
extern SOCKET sockfd;

void serv (void *arg)
{
  int h = *(int *) arg;
  extern int pidcmgr;

  pidcmgr = 0;
  protocol (h, 0);
  Log (5, "downing server...");
  soclose (h);
  free (arg);

  --n_servers;
}


void servmgr (void *arg)
{
  SOCKET new_sockfd;
  
  int client_addr_len;
  struct sockaddr_in serv_addr, client_addr;
  int opt = 1;
  n_servers = 0;
  ext_rand = 0;
  SOCKET sockfd = -1;
  
  srand(time(0));
  
  Log (4, "servmgr started");

  

  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    Log (0, "socket: %s", TCPERR ());

  if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR,
		  (char *) &opt, sizeof opt) == SOCKET_ERROR)
    Log (1, "setsockopt (SO_REUSEADDR): %s", TCPERR ());

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons ((unsigned short) iport);

  if (bind (sockfd, (struct sockaddr *) & serv_addr, sizeof (serv_addr)) != 0)
    Log (0, "bind: %s", TCPERR ());

  listen (sockfd, 5);
  
  for (;;)
  {
    struct timeval tv;
    fd_set r;

    FD_ZERO (&r);
    FD_SET (sockfd, &r);
    tv.tv_usec = 0;
    tv.tv_sec  = CHECKCFG_INTERVAL;
    switch (select(sockfd+1, &r, NULL, NULL, &tv))
    { case 0: /* timeout */
        /* Test config mtime */
        continue;
      case -1:
        if (TCPERRNO == EINTR)
          continue;
	Log (1, "select: %s", TCPERR ());
        goto accepterr;
    }

    
    client_addr_len = sizeof (client_addr);
    if ((new_sockfd = accept (sockfd, (struct sockaddr *) & client_addr,
			      &client_addr_len)) == INVALID_SOCKET)
    {
      if (TCPERRNO != EINVAL && TCPERRNO != EINTR)
      {
	Log (1, "accept: %s", TCPERR ());
	accepterr:
        return;
      }
    }
    else
    { char host[MAXHOSTNAMELEN + 1];

      ext_rand=rand();
      LockSem(&hostsem);
      Log (3, "incoming from %s (%s)",
	   get_hostname(&client_addr, host, sizeof(host)),
	   inet_ntoa (client_addr.sin_addr));
      ReleaseSem(&hostsem);

      ++n_servers;
      Log (5, "started server #%i", n_servers);
	  serv((void *) &new_sockfd);
     
    }
  }
}
