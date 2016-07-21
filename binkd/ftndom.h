#ifndef _ftndomain_h
#define _ftndomain_h

#include "ftnaddr.h"

typedef struct _FTN_DOMAIN FTN_DOMAIN;
struct _FTN_DOMAIN
{
  char name[MAX_DOMAIN + 1];
  char *path;				    /* Outbound dir's path, ie
					     * "/var/spool/fido" */
  char *dir;				    /* Outbound dir's name, ie "outb" */
  int *z;
  FTN_DOMAIN *alias4;
  FTN_DOMAIN *next;
};

/*
 * 0 == domain not found
 */
FTN_DOMAIN *get_domain_info (char *domain_name);

/*
 * Returns the default domain
 */
FTN_DOMAIN *get_def_domain ();

//free domain
void free_domain ();
#endif
