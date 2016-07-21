#ifndef _readflo_h
#define _readflo_h

#include <stdio.h>

typedef struct _RF_RULE RF_RULE;
struct _RF_RULE
{
  char *from, *to;
  RF_RULE *next;
};

/*
 * Add a translation rule for trans_flo_line ()
 * (From and to are saved as pointers!)
 */
void rf_rule_add (char *from, char *to);

/*
 * Reads a line from a flo to dst[MAXPATHLEN], sets action
 * 1 -- ok
 * 0 -- EOF
 */
int read_flo_line (char *dst, int *action, FILE *flo);

/*
 * Translates a flo line using rf_rules.
 * Returns 0 if no rf_rules defined, otherwise returned value
 * should be free()'d
 */
char *trans_flo_line (char *s);
void make_dlo_file (char *namefile,char* inbound);

#endif
