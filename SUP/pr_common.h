#ifndef _p_common_h
#define _p_common_h
 
class p_common
{ 
public:
	char    abortstr[20] ;
	char    pktprefix[10];
	char    autostr[10];
	int port;  
	char    message_error[255];
	int  xfer_logged;
	char xfer_pathname[256];
	char xfer_real[256];
	char xfer_temp[256];
	long xfer_fsize;
	long xfer_ftime;
	char    txpathname[300];
	long    txallsize;        /* all files length      */
	long    rxallsize;        /* all files length      */
	long    txtecsize;	    /* curent transfer       */
	long    rxtecsize;        /* curent transfer       */
	long    txoffset,       rxoffset;       /* offset in file we begun   */
	char    txfname[13];
	char    rxfname[13];    /* fname of current files    */
	long    txftime,        rxftime;        /* file timestamp (UNIX)     */
	long    txfsize,        rxfsize;        /* file length               */
	FILE  * txfd,           *rxfd;          /* file handles              */
	long    txstart,        rxstart;        /* time we started this file */
	char *ReqFile;
	char NameReqFile[500];
	char *XferFile;
	char NameXferFile[500];

private:  
	char temppath[300];
public:
	p_common(void);
	virtual ~p_common();
	void p_common::errcode(const int result,char *resultstring);
	void unique_name (char *pathname);
	int xfer_init(char *fname,int fsize,int ftime);
	char xfer_bad (void);
	char *xfer_okay (void);
	void xfer_del (void);
	void pr_badxfer (void);
	void next_txpathname(char *sendlist,int index);

	int IsReqFile(char *name);
	void DelReqFile(char *name);
	int CountReqFile(char *name);
	int IsXferFile(char *name);
	void DelXferFile(char *name);
};
#endif