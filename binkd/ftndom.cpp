
#include "stdafx.h"
#include "ftndom.h"
#include "tools.h"

FTN_DOMAIN *pDomains = 0;

/*
 * 0 == domain not found
 */
FTN_DOMAIN *get_domain_info (char *domain_name)
{
  FTN_DOMAIN *curr;

  for (curr = pDomains; curr; curr = curr->next)
    if (!STRICMP (curr->name, domain_name))
      return curr;
  return 0;
}

/*
 * Returns the default domain
 */
FTN_DOMAIN *get_def_domain ()
{
  FTN_DOMAIN *curr;

  for (curr = pDomains; curr->next; curr = curr->next);
  return curr;
}
void free_domain ()
{
  FTN_DOMAIN *curr;
  FTN_DOMAIN *p;

  p=pDomains;
  for (curr = pDomains; p;curr=p) 
  {p=curr->next;
   free(curr);}
  pDomains=NULL;
  return ;
}
