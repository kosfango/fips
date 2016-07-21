// *********************************************************************************
// *********************** Die einzelnen Optionen der Compilierten Versio **********
// *********************************************************************************

// [ *** Specific Debug Enable *** ]
// #define DEBSEARCH			1		  // DEBUG OUT SEARCH ENGINE STUFF
// #define DEBUDP			    1		  // DEBUG OUT UDP STUFF
// #define WATCHERROR			1		  // MIT WATCHERROR
// #define BNDCHECK				1		  // FUER BOUNDCHECKER 
// #define DEB_TRACE_ON			1
// #define DEB_TRACE_LINEINFO	1

// !! Falls neue Macros hinzgefuegt werden einen eventuellen Hinweis per  !!
// !! Pragma im areasel.cpp nicht vergessen 							  !!
// !! Diese File wurde gewaehlt, weil es das erste zu uebersetztende ist  !!
/*
#ifdef DEB_TRACE_ON	
	extern char gDebugTraceBuffer[3000]; 
	void		debwrite				(char *);
	void		debbin					(char *,int anzahl);
	void		deb_transfer_line_info	(char *text);
#endif


#ifdef FROMPROTOCOL
	#ifdef	DEB_TRACE_LINEINFO	
		extern char	G_DebWriteLineInfo[100];
		#define		SETDEBUGLINEINFO	sprintf(G_DebWriteLineInfo," %d, %s",__LINE__,__FILE__);deb_transfer_line_info(G_DebWriteLineInfo);
	#else
		#define		SETDEBUGLINEINFO	 
	#endif
#else
	#ifdef	DEB_TRACE_LINEINFO	
		extern char	G_DebWriteLineInfo[100];
		#define		SETDEBUGLINEINFO	sprintf(G_DebWriteLineInfo," %d, %s",__LINE__,__FILE__); 
	#else
		#define		SETDEBUGLINEINFO	 
	#endif
#endif
*/