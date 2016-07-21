#include "stdafx.h"
#include <io.h>
#include "..\globals.h" 
#include "readflo.h"


RF_RULE *rf_rules = 0;

/*
 * Reads a line from a flo to dst[MAX_PATH], sets action
 * 1 -- ok
 * 0 -- EOF
 */
int read_flo_line (char *dst, int *action, FILE *flo)
{
  char buf[MAX_PATH + 1];
  int i;

  while (1)
  {
    if (!fgets (buf, MAX_PATH, flo))
      return 0;

    for (i = strlen (buf) - 1; i > 0 && isspace (buf[i]); --i)
      buf[i] = 0;

    switch (*buf)
      {
	case 0:
	case '~':
	  continue;
	case '^':
	  *action = 'd';
	  strcpy (dst, buf + 1);
	  break;
	case '#':
	  *action = 't';
	  strcpy (dst, buf + 1);
	  break;
	default:
	  *action = 0;
	  strcpy (dst, buf);
	  break;
      }
    break;
  }
  return 1;
}

/*
 * Translates a flo line using rf_rules.
 * Returns 0 if no rf_rules defined, otherwise returned value
 * should be free()'d
 */
char *trans_flo_line (char *s)
{
  RF_RULE *curr;
  char buf[MAX_PATH + 1];

  if (rf_rules)
  {
    char *w;

    strnzcpy (buf, s, MAX_PATH);
    for (curr = rf_rules; curr; curr = curr->next)
    {
      w = ed (buf, curr->from, curr->to, NULL);
      strnzcpy (buf, w, MAX_PATH);
      free (w);
    }
    return (char*) xstrdup (buf);
  }
  else
    return 0;
}

/*
 * Add a translation rule for trans_flo_line ()
 * (From and to are saved as pointers!)
 */
void rf_rule_add (char *from, char *to)
{
  static RF_RULE *last_rule = 0;
  RF_RULE *new_rule = (struct _RF_RULE *) xalloc (sizeof (RF_RULE));

  memset (new_rule, 0, sizeof (RF_RULE));
  new_rule->from = from;
  new_rule->to = to;
  if (last_rule)
    last_rule->next = new_rule;
  else
    rf_rules = new_rule;
  last_rule = new_rule;
}


void make_dlo_file (char *namefile,char* inbound)
{
		long hfile;
		_finddata_t se;
        FILE *fp;
		FILE *fp2;
		char buffer[500];

		char param[500];
		char nfile[500];
		char par1[500];
		char par2[500];
		char netnode[13];
		int zone,net,node,point;

		strcpy(nfile,inbound);
		strcat(nfile,"\\");
		strcat(nfile,namefile);
                fp=fopen(nfile,"wt"); 
		strcpy(param,inbound);
		strcat(param,"\\*.*");
		hfile = _findfirst(param,&se);
		if (hfile != -1L)
		{
		do
		{ 
		if (ispkt(se.name) || isarcmail(se.name) || istic(se.name))
		{ strcpy(par1,inbound);
		  strcat(par1,"\\");
		  strcat(par1,se.name);
          fprintf(fp,"%s \n",par1);
		}
		if (pmatch("*.[Rr][Ee][Qq]",se.name))
		{
		  strcpy(par1,inbound);
		  strcat(par1,"\\");
		  strcat(par1,se.name);
          strcpy(par2,inbound);
		  strcat(par2,"\\");
		  parse_address(curmail.myaka,&zone,&net,&node,&point);
		  sprintf(netnode,"%0.4X%0.4X.REQ",net,node);
		  strcat(par2,netnode);
		  CopyFile(par1,par2,NULL);
		  //fprintf(fp,"%s \n",par2);
		  
		
		}
			if (pmatch("*.[Ff][Ll][Ss]",se.name))
		{
		  strcpy(par1,inbound);
		  strcat(par1,"\\");
		  strcat(par1,se.name);
          
		  fp2=fopen(par1,"rt"); 
		  while (fgets(buffer,499,fp2))
		  {
		  fprintf(fp,"%s \n",buffer);
		  }
		  fclose(fp2);
		  
		
			}
		}
		while (_findnext(hfile,&se)==0);
		_findclose(hfile);
		}
		//Netmail
		strcpy(param,inbound);
		strcat(param,"\\NETMAIL\\*.*");
		hfile = _findfirst(param,&se);
		if (hfile != -1L)
		{
		do
		{
		if (ispkt(se.name) || isarcmail(se.name) || istic(se.name)) 
		{
		  strcpy(par1,inbound);
		  strcat(par1,"\\NETMAIL\\");
		  strcat(par1,se.name);
          fprintf(fp,"%s \n",par1);
		}
        }
		while (_findnext(hfile,&se)==0);
		_findclose(hfile);
		}
	fclose(fp);	
    return ;
}
