#include "stdafx.h"

void *xalloc (size_t size)
{
  void *p = malloc (size);

  if (!p)
    Log (0, "Not enough memory (failed to allocate %lu byte(s))",
	 (unsigned long) size);
  return p;
}

void *xrealloc (void *ptr, size_t size)
{
  if ((ptr = realloc (ptr, size)) == NULL)
    Log (0, "Not enough memory (failed to realloc %p to %lu byte(s))",
	 ptr, (unsigned long) size);
  return ptr;
}

void *xstrdup (const char *str)
{
  void *p = strdup (str);

  if (!p)
    Log (0, "Not enough memory (failed to strdup %lu byte(s) at %p)",
	 (unsigned long) strlen (str), str);
  return p;
}
