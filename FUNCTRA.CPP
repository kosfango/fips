#include <stdio.h>
#include "version.h"



#ifdef DEB_FUNCTRACE

extern int gFuncLevel;

	// ============================================================
		CFunctionTrace::CFunctionTrace(char *name) 
	// ============================================================
	{
		printf("%d Entering %s\n",gFuncLevel++,name);
		if (strlen(name)>49)
			name[49]=0;
		strcpy(buf,name);
	}


	// ============================================================
		CFunctionTrace::~CFunctionTrace() 
	// ============================================================
	{
		printf("%d Leaving %s\n",--gFuncLevel,buf);
	}
#endif