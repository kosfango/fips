
#include "stdafx.h"
#include "iphdr.h"
#include "readcfg.h"
#include "iptools.h"

/*
 * Finds ASCIIZ address
 */
const char *get_hostname (struct sockaddr_in *addr, char *host, int len)
{
  struct hostent *hp;
  struct sockaddr_in s;

  memcpy(&s, addr, sizeof(s));
  hp = ((backresolv == 0) ? 0 :
	gethostbyaddr ((char *) &s.sin_addr,
		       sizeof s.sin_addr,
		       AF_INET));

  strnzcpy (host, hp ? hp->h_name : inet_ntoa (s.sin_addr), len);

  return host;
}

void copy_hostent(struct hostent *dest, struct hostent *src)
{
  int naddr;
  char **cp;

  memcpy(dest, src, sizeof(struct hostent));
  for (cp = src->h_addr_list, naddr = 0; cp && *cp; naddr++, cp++);
  dest->h_addr_list = (char **) malloc((naddr+1)*sizeof(dest->h_addr_list[0]));
  if (dest->h_addr_list)
  {
    dest->h_addr_list[0] = (char *) malloc(naddr*src->h_length);
    if (dest->h_addr_list[0])
    {
      for (cp = src->h_addr_list, naddr=0; cp && *cp; cp++, naddr++)
      {
        dest->h_addr_list[naddr] = dest->h_addr_list[0]+naddr*src->h_length;
        memcpy(dest->h_addr_list[naddr], *cp, src->h_length);
      }
      dest->h_addr_list[naddr] = NULL;
    }
  }
}

/*
 * Sets non-blocking mode for a given socket
 */
void setsockopts (SOCKET s)
{

#if defined(FIONBIO)
#if defined(UNIX) || defined(IBMTCPIP) || defined(AMIGA)
  int arg;

  arg = 1;
  if (ioctl (s, FIONBIO, (char *) &arg, sizeof arg) < 0)
    Log (1, "ioctl (FIONBIO): %s", TCPERR ());

#elif defined(WIN32)
  u_long arg;

  arg = 1;
  if (ioctlsocket (s, FIONBIO, &arg) < 0)
    Log (1, "ioctlsocket (FIONBIO): %s", TCPERR ());
#endif
#endif

}

/*
 * Find the port number (in the host byte order) by a port number string or
 * a service name. Find_port ("") will return binkp's port from
 * /etc/services or even (if there is no binkp entry) 24554.
 * Returns 0 on error.
 */
int find_port (char *s)
{
  struct servent *entry = getservbyname (*s ? s : PRTCLNAME, "tcp");

  if (entry)
    return ntohs (entry->s_port);
  if (*s == 0)
    return DEF_PORT;
  if (isdigit (*s))
    return atoi (s);

  Log (1, "%s: incorrect port", s);
  return DEF_PORT;
}
