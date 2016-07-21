#include "stdafx.h"
#include "structs.h"
#include <direct.h>

void dbout		(void);

char dbuf[400];
int  gdebug;
char glogfile[400]; 

STARTUPINFO			startUpInfo;
PROCESS_INFORMATION	procInfo;

// =========================================================
	int main(int argc, char *argv[])
// =========================================================
// TOP: Der Aufruf erfolgt mit
//		exectool debug logfilepath startupdirectory Kommandozeile
//         0       1	   2			3				4

{
int  retval=RETVAL_UNDEFINED;
int  ret;
int  last;
char curpath[1000];
char errbuf[300];


	 if (argc!=5)
	 {
	     printf("F.I.P.S./32 Helper Executable\n");
	     printf("Theres no need to call direct, this tool is only used from FIPS.EXE\n");
	     printf("Copyright Johann Weinzierl & Oliver Weindl\n");
		 printf("Press a key ...\n");
		 getchar();
		 exit(RETVAL_PARAMERROR);
	 }

	 // Get Debug Level
	 gdebug=atoi(argv[1]);
	 strcpy(glogfile,argv[2]);

	 sprintf(dbuf,"\n\n\n");   dbout();
	 sprintf(dbuf,"----====[*F.I.P.S./32 Helper Executable]====----\n");
	 dbout();

	 GetCurrentDirectory(999,curpath);
	 sprintf(dbuf,"  *Current directory: <%s>\n",curpath);
	 dbout();

	 ret=SetCurrentDirectory(argv[3]);
	 if (ret)
	 {
	     sprintf(dbuf,"  *Changing to directory: <%s> :OK\n",argv[3]);
		 dbout();
	 }
	 else
	 {
	     sprintf(dbuf,"  !Changing to directory: <%s> :!FAILED!\n",argv[3]);
		 dbout();
		 retval=RETVAL_CHANGEDIRFAILED;
		 goto endit;
	 }

	 GetCurrentDirectory(999,curpath);
	 if (stricmp(argv[3],curpath))
	 {   
	   	 // Change Directory fehlgeschlagen ...
		 sprintf(dbuf,"  ! WIN Error on changing directory !!");
		 dbout();
		 retval=RETVAL_WINERROR;
		 goto endit;
	 }

	 sprintf(dbuf,"  *Executing: <%s>\n",argv[4]);
	 dbout();

	// Baue Startup Info
	GetStartupInfo						(&startUpInfo);
	startUpInfo.dwFlags 				=0;

	// Hier wird der neue Prozess generiert
    ret=CreateProcess(0,argv[4],0,0,0,0,0,0,&startUpInfo,&procInfo);
	if (!ret)
	{
		last=GetLastError();

		sprintf(errbuf,"%d",last);
		if (last==2)
		   strcpy(errbuf,"File not found");
		if (last==3)
		   strcpy(errbuf,"Path not found");

		sprintf(dbuf,"  !CreateProcess FAILED because of: %s\n",errbuf);  dbout();
		printf("  Please press CTRL-C to stop .. \n"); 
		while (1)
		{
		    printf("\007");
			Sleep(5000);
		}

		retval=2000000+last;
		if (last==2)
			retval=RETVAL_FILENOTFOUND;
		if (last==3)
			retval=RETVAL_PATH_NOTFOUND;
		goto endit;
	}
	else
		sprintf(dbuf,"  *CreateProcess OK!\n");  dbout();


    sprintf(dbuf,"----====[ External Tool ...             ]====----\n");
    dbout();
	ret=WaitForSingleObject(procInfo.hProcess,INFINITE);
	if (ret	!= WAIT_OBJECT_0)
	{
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		sprintf(dbuf,"  !WaitForSingleObject failed: %d\n",ret);  dbout();
		retval=RETVAL_WAITFORSINGLEOBJFAILED;
		goto endit;
	}

	// Hole den Rueckgabewert ...
	ret=GetExitCodeProcess(procInfo.hProcess,(unsigned long *)&retval);
	if (!ret)
	{
		sprintf(dbuf,"  !GetExitCodeProcess failed: %d\n",ret);  dbout();
		retval=RETVAL_GETEXITCODE;
		goto endit;
	}
	sprintf(dbuf,"  *Process returned: %d\n",retval);  dbout();
	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);

endit:
	 return (retval);
}


// =====================
	void dbout(void)
// =====================
{
char tmp[2000];
char mtime[100];
FILE  *fp;

	_strtime(mtime);

	sprintf(tmp,"%s: %s",mtime,dbuf);
	printf(dbuf);

	if (gdebug)
	{
		fp=fopen(glogfile,"at");
		if (!fp)
		{
		    printf("!!Cannot open logfile <%s>!!\n",glogfile);
		}
		else
		{
		    fprintf(fp,tmp);
			fclose(fp);
		}
	}
	strcpy(dbuf,"");
	return;
}
