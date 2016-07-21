// This file contains the basic TWIT FILTER Stuff
#include "stdafx.h"

// ========================================================================================
	int is_thrue_that(char *text,int match, int casesens, char *list)
// ========================================================================================
// TOP: Hier kommen wir mit einem Nullterminierten Text, einer MATCHSPEC: MATCHEQUAL
//																		  MATCHCONTAINS
//																		  MATCHCONTAINSWORDS
// einer WoerterListe und einem casesens flag rein.
//
{
CStrList words;
int t;
int x;
char help[300];

	  ASSERT(text);
	  ASSERT((casesens==0) || (casesens==1));
	  ASSERT((match==MATCHEQUAL) || (match==MATCHCONTAINS) || (match==MATCHCONTAINSWORDS));
	  ASSERT (list);

	  switch (match)
	  {

		   case MATCHEQUAL:
							 // Pruefen auf exakt identisch
							 if (casesens)
							 {
							    x=strcmp(text,list);
							    if (!x)	return 1; else  return 0;
							 }
							 else
							 {
							    x=stricmp(text,list);
							    if (!x)	return 1; else  return 0;
							 }
		                     break;


		   case MATCHCONTAINS:
		                     // Pruefen auf String enthalten
							 return (find(text,casesens,list));
		                     break;

		   case MATCHCONTAINSWORDS:
							  words.SplitWords(list);
							  if (!words.GetCount())
							     return 1;                  // Nix is immer enthalten ... :-)

							 // Pruefen ob die Woerter enthalten sind
							 for (t=0;t<words.GetCount();t++)
							 {
								 strcpy(help,words.GetString(t));
								 if (!find(text,casesens,help))
								    return 0;
							 }
							 return 1;
		                     break;


	       default:
				             ASSERT(0);
							 break;

	  }
return 0;
}

// ========================================================================================
	int find(char *text,int casesens, char *search)
// ========================================================================================
{
char *ptext;
char *psearch;
char *x;

	// Case sensitive search
	if (casesens)
	{
	   if (strstr(text,search))
	      return 1;
	   else
	      return 0;
	}
	else	 // Case insensitive search
	{
		ptext	=(char *)malloc(strlen(text)+100);
		psearch	=(char *)malloc(strlen(search)+100);
		if (!ptext || !psearch)	{err_out("E_MEM_OUT"); return 0; }
		strcpy(ptext,text);
		strcpy(psearch,search);
		strupr(ptext);
		strupr(psearch);

	    x=strstr(ptext,psearch);
		free(ptext);
		free(psearch);
		if (x)
		   return 1;
		else
		   return 0;
	}
}



