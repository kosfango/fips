#include <stdafx2.h>

FILE *fpin=0;
FILE *fpout=0;


char linebuf[10000];
char areatag[10000];
char desc	[10000];
char tmp    [10000];
char *p;
char *p1;
char *p2;

int  inlinecount=0;
int  outlinecount=0;

int read_next_line						(void);
char form=' ';

#define PS (char *)(const char *)

// ============================================
	int main(int argc, char *argv[])
// ============================================
// TOP: Format:  echoconv FORMAT INPUTFILE OUTPUTFILE
//      example  echoconv A c:\tmp\input.eco d:\point\output.eco
{
int retval=1;
int ret;
int echos=0;

	 printf("F.I.P.S./32 ECHOLIST CONVERTER TOOL V1.1\n");
	 if (argc!=4)
	 {
		   printf("Usage:   echoconv FORMAT INPUTFILE OUTPUTFILE\n");
		   printf("example: echoconv A c:\\tmp\\input.eco d:\\point\\output.eco\n");
		   printf("Output will be in YUPPIE Format:\n\n");
		   printf("Supported Formats:\n");
		   printf("==================\n\n");
		   printf("Format 'A':\n");
		   printf("areatag ........ areadescription\n\n");
		   printf("Format 'B':\n");
		   printf("areatag\n");
		   printf(" areadescription\n\n");
		   printf("Format 'C':\n");
		   printf("areatag         \"areadescription\"\n\n");
		   printf("Format 'D':\n");
		   printf("\"areatag\",\"areadescription\"\n\n");
		   printf("Format 'E':\n");
		   printf("\"areatag\",areadescription\n\n");
		   printf("Format 'F':\n");
		   printf("[*] areatag ..... areadescription\n");
		   fcloseall();
		   exit(1);
	 }

	 if (!(stricmp(argv[1],"A")))
	 	form='A';

	 if (!(stricmp(argv[1],"B")))
	 	form='B';

	 if (!(stricmp(argv[1],"C")))
	 	form='C';

	 if (!(stricmp(argv[1],"D")))
	 	form='D';

	 if (!(stricmp(argv[1],"E")))
	 	form='E';

	 if (!(stricmp(argv[1],"F")))
	 	form='F';

	 if (form==' ')
	 {
		printf("Unknown Format specifier!\n");
		fcloseall();
		exit(2);
	 }

	 fpin=fopen(argv[2],"rt");
	 if (!fpin)
	 {
		printf("Cannot open input file <%s> for reading\n",argv[2]);
		fcloseall();
		exit(3);
	 }

	 fpout=fopen(argv[3],"wt");
	 if (!fpout)
	 {
		printf("Cannot open ouput file <%s> for writing\n",argv[3]);
		fcloseall();
		exit(4);
	 }

	 for (;;)
	 {
		 ret=read_next_line();
		 if (ret==-1)
		 {
			 printf("Format error in line %d\n",inlinecount);
			 printf("Conversion aborted!\n");
			 fcloseall();
			 exit(5);
		 }

		 if (ret==0)
		 {
			 printf("Conversion successfully\n");
			 printf("%d Areas written to Output File\n",echos);
			 fcloseall();
			 exit(0);
		 }

		 echos++;
		 // Ansonsten schreiben wir die Zeile im YUPPIE Format raus ...
		 fprintf(fpout,"%s,\"%s\"\n",areatag,desc);

	 }


	 fcloseall();
	 return (0);
}

// =========================================================================
	int read_next_line()
// =========================================================================
// TOP: Funktion liefert die naechste Zeile in areatag und desc	zurueck
// 1    Wenn alles gutgeht
// 0    keine weitere zeile mehr vorhanden
// -1   Format Error

//		   printf("Format 'A':\n");
//		   printf("areatag ........ areadescription\n\n");
//		   printf("Format 'B':\n");
//		   printf("areatag\n");
//		   printf(" areadescription\n\n");
{
char dummy[500];							// sscanf
char dummy2[500];							// sscanf
CString help;
CString help1;
char    *p;

   if (form=='A')
   {
nochmal:
		inlinecount++;
		if (!fgets(linebuf,999,fpin))
		   return 0;

		if (strlen(linebuf)<4)
		   goto nochmal;

		// teste auf leerzeilen
		dummy[0]=0;	sscanf(linebuf,"%s",dummy);
		if (strlen(dummy)<2)
		   goto nochmal;

        areatag[0]=0;
		desc   [0]=0;
		sscanf(linebuf,"%[^ ]%[ .]%[^\n]",areatag,dummy2,desc);
		if ((strlen(areatag)==0) || (strlen(desc)==0))
		   return -1;
		else
		{
			help=desc;
            trim_all(help);
			strcpy(desc,PS help);

			help=areatag;
            trim_all(help);
			strcpy(areatag,PS help);
		    return 1;
		}
   }

   if (form=='B')
   {
nochmal2:
		inlinecount++;
		if (!fgets(linebuf,999,fpin))
		   return 0;

		if (strlen(linebuf)<4)
		   goto nochmal2;

		if (linebuf[0]!=' ' && linebuf[0]!='\n'	&& linebuf[0]!='\r')
		{
			sscanf(linebuf,"%s",areatag);
			if (!fgets(linebuf,999,fpin))
			   return -1;

			// das erste Zeichen muss ein ' ' sein.
			if (linebuf[0]!=' ')
			 	return -1;

			p=strchr(linebuf,'\n');
			if (p)
			   *p=0;

			help=linebuf;
            trim_all(help);
			strcpy(desc,PS help);
			return 1;
		}
   }

   if (form=='C')
   {
nochmal3:
		inlinecount++;
		if (!fgets(linebuf,999,fpin))
		   return 0;

		if (strlen(linebuf)<4)
		   goto nochmal3;

		// teste auf leerzeilen
		dummy[0]=0;	sscanf(linebuf,"%s",dummy);
		if (strlen(dummy)<2)
		   goto nochmal;

        areatag[0]=0;
		desc   [0]=0;
		sscanf(linebuf,"%[^ ]%[^\n]",areatag,desc);
		if ((strlen(areatag)==0) || (strlen(desc)==0))
		   return -1;
		else
		{
			help=desc;
            trim_all(help);
			strcpy(desc,PS help);

			help=areatag;
            trim_all(help);
			strcpy(areatag,PS help);
		    return 1;
		}
   }
   if (form=='D')
   {
nochmal4:
		inlinecount++;
		if (!fgets(linebuf,999,fpin))
		   return 0;

		if (strlen(linebuf)<4)
		   goto nochmal4;

		// teste auf leerzeilen
		dummy[0]=0;	sscanf(linebuf,"%s",dummy);
		if (strlen(dummy)<2)
		   goto nochmal;

        areatag[0]=0;
		desc   [0]=0;

		help=linebuf;
        trim_all(help);
		strcpy(linebuf,PS help);

		if (linebuf[0]!='\"')
		   return -1;

		sscanf(&linebuf[1],"%[^\"]%[\"]%[^\"]%[\"]%[^\"]",areatag,dummy,dummy,dummy,desc);

		if ((strlen(areatag)==0) || (strlen(desc)==0))
		   return -1;
		else
		{
			help=desc;
            trim_all(help);
			strcpy(desc,PS help);

			help=areatag;
            trim_all(help);
			strcpy(areatag,PS help);
		    return 1;
		}
   }

   if (form=='E')
   {
nochmal5:
		inlinecount++;
		if (!fgets(linebuf,999,fpin))
		   return 0;

		if (strlen(linebuf)<4)
		   goto nochmal5;

		// teste auf leerzeilen
		dummy[0]=0;	sscanf(linebuf,"%s",dummy);
		if (strlen(dummy)<2)
		   goto nochmal;

        areatag[0]=0;
		desc   [0]=0;

		help=linebuf;
        trim_all(help);
		strcpy(linebuf,PS help);

		if (linebuf[0]!='\"')
		   return -1;

		sscanf(&linebuf[1],"%[^\"]%[\" ,\t]%[^\n]",areatag,dummy,desc);

		if ((strlen(areatag)==0) || (strlen(desc)==0))
		   return -1;
		else
		{
			help=desc;
            trim_all(help);
			strcpy(desc,PS help);

			help=areatag;
            trim_all(help);
			strcpy(areatag,PS help);
		    return 1;
		}
   }

   if (form=='F')
   {
nochmal6:
		inlinecount++;
		if (!fgets(linebuf,999,fpin))
		   return 0;

		if (strlen(linebuf)<4)
		   goto nochmal5;

		// teste auf leerzeilen
		dummy[0]=0;	sscanf(linebuf,"%s",dummy);
		if (strlen(dummy)<2)
		   goto nochmal6;

        areatag[0]=0;
		desc   [0]=0;

		help=linebuf;
        // trim_all(help);
		strcpy(linebuf,PS help);


		strcpy(desc,"Not available");
		sscanf(&linebuf[0],"%[ *\t]%s%[ .\t]%[^\n]",dummy,areatag,dummy,desc);

		help=desc;
        trim_all(help);
		strcpy(desc,PS help);

		help=areatag;
        trim_all(help);
		strcpy(areatag,PS help);
	    return 1;

   }

   ASSERT(0);
   return 1;
}


/*/ =====================================================================
    int trim_all(CString &text)
// =====================================================================
{
char buf[3000];
char buf1[3000];
char *p;
char *pend;

	 strcpy(buf,PS text);
	 if (buf[0]==0)
	    return 1;

	 p=buf;
	 while (*p==' ' && *p)
	     p++;
	 // So jetzt stehen wir auf dem ersten Character nach dem ' ' oder auf \0

	 pend=&buf[strlen(buf)];  // Auf der 0
	 pend--;

	 while (*pend==' ' & pend>p)
	      pend--;

	 pend++;
	 // Jetzt stehen wir entweder auf dem letzen Char oder auf p;
	 if((pend-p) >1)
	 {
		memcpy(buf1,p,pend-p);
		buf1[pend-p]=0;
		text=buf1;
	 }
	 else
		 text="";

	 return 1;
}

*/
