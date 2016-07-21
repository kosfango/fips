#include "stdafx.h" 
#include <io.h>
#include <sys\stat.h>
#include "pr_xfer.h"                    
#include "pr_common.h"                    

extern _gconfig gc;

	p_common::p_common(void)
{  
	strcpy(pktprefix,"");
	strcpy(message_error,"OK");           
	ReqFile=0;
	*NameReqFile=0;
	XferFile=0;
	*NameXferFile=0;
TRACE0("init");
}

	p_common::~p_common()
{  
    if (ReqFile)	{ free(ReqFile);  ReqFile=0;  }
    if (XferFile)	{ free(XferFile); XferFile=0; }
}

	int p_common::IsReqFile(char *name)
{
char *p;

	p=ReqFile;
	while (p) 
	{ 
		if (!stricmp(p,name)) return 1;
		p=p+strlen(p)+1; 
	}
	return 0;  
}

	int p_common::IsXferFile(char *name)
{
char *p;

	p=XferFile;
	while (p) 
	{ 
		if (!stricmp(p,name)) return 1;
		p=p+strlen(p)+1; 
	}
	return 0;  
}

	void p_common::DelReqFile(char *name)
{
FILE *fpReq;
FILE *fpNewReq;
char buffer[500];
char newname[]="bad_req.re_";
  
	if (*NameReqFile && (fpReq = fopen(NameReqFile, "rt"))) 
	{
		if ((fpNewReq = fopen(newname, "wt"))) 
		{
			while (fgets(buffer,500,fpReq)) 
			{
				if (strcmp(buffer,name)) 
					fputs(buffer,fpNewReq);
				
			}
			fclose(fpReq);
			fclose(fpNewReq);
			unlink(NameReqFile);
			rename(newname,NameReqFile);
			
		}
		else
			fclose(fpReq);
	}
	return;  
}

	void p_common::DelXferFile(char *name)
{
FILE *fpReq;
FILE *fpNewReq;
char buffer[500];
char newname[]="bad_xfer.fl_";
  
	if (*NameReqFile && (fpReq=fopen(NameReqFile,"rt"))) 
	{
		if ((fpNewReq = fopen(newname, "wt"))) 
		{
			while (fgets(buffer,500,fpReq)) 
			{
				if (strcmp(buffer,name)) 
					fputs(buffer,fpNewReq);
				
			}
			fclose(fpReq);
			fclose(fpNewReq);
			unlink(NameReqFile);
			rename(newname,NameReqFile);
			
		}
		else
			fclose(fpReq);
	}
}

int p_common::CountReqFile(char *name)
{FILE *fpReq;
 char buffer[500];
 int n=0;
  
	if ((fpReq = fopen(name, "rt"))) 
	{
	  while (fgets(buffer,500,fpReq)) 
		if (strlen(buffer)>3) n++;
	  fclose(fpReq);
	}
  return n;  
}

void p_common::next_txpathname(char *sendlist,int index)
{
FILE *fpReq;
char buffer[500];
struct stat f;
CString str;
char drive[10],dir[300],fname[100],ext[100];

//Инициализация отправляемого файла
	txfd = NULL;
	*txfname=0;
	if (!sendlist || !sendlist[0]) return;

// извлекаем имя очередного файла
    if (!get_token(sendlist,index-1,str,' '))	return;
	str.TrimLeft(" +");
	str.TrimRight();
	strcpy(txpathname,str);
	stat(txpathname,&f);
	txfsize = f.st_size;
	txftime = f.st_mtime;

	if ((txfd = fopen(txpathname,"rb")) <= 0) 
	{
		message("-HSEND: Unable to open %s",txpathname);
		txfd=NULL; 
		return;
	}
	strupr(txpathname);
	_splitpath(txpathname,drive,dir,fname,ext);
	strcpy(txfname,fname);
	strcat(txfname,ext);
//Проверка на файл-запрос

	if (pmatch(txfname,"*.[Rr][Ee][Qq]"))
	{ 
		fpReq=fopen(txpathname,"rt");
		if (fpReq<=0) return;
		int Len;
		Len=0;
		while (!feof(fpReq)) 
		{ 
			fgets(buffer,sizeof(buffer),fpReq);
			Len=Len+strlen(buffer)+1;
		}
		ReqFile=(char *) malloc(Len+1);
		if (!ReqFile) return;
		Len=0;
		while (!feof(fpReq)) 
		{ 
			fgets(buffer,sizeof(buffer),fpReq);
			strcpy(ReqFile+Len,buffer);
			Len=Len+strlen(buffer)+1;
		}
		*(ReqFile+Len)=0;
		strcpy(NameReqFile,txpathname);
		fclose(fpReq);
TRACE0(txpathname);
	}
//Проверка на атач
	if (pmatch(txfname,"*.[Ff][Ll][Ss]"))
	{ 
		fpReq=fopen(txpathname,"rt");
		if (fpReq<=0) return;
		int Len;
		Len=0;
		while (!feof(fpReq)) 
		{ 
			fgets(buffer,sizeof(buffer),fpReq);
			Len=Len+strlen(buffer)+1;
		}
		XferFile=(char *) malloc(Len+1);
		if (!ReqFile) return;
		Len=0;
		while (!feof(fpReq)) 
		{ 
			fgets(buffer,sizeof(buffer),fpReq);
			strcpy(XferFile+Len,buffer);
			Len=Len+strlen(buffer)+1;
		}
		*(XferFile+Len)=0;
		strcpy(NameXferFile,txpathname);
		fclose(fpReq);
TRACE0(txpathname);
	}
}

void p_common::xfer_del (void)
{
char new_log[300];
char xfer_log[300];
char linebuf[255];
char bad_real[300];
char bad_temp[300];
long bad_fsize;
long bad_ftime;
FILE *fp, *new_fp;
int left;

	if (existfile(xfer_pathname))
		unlink(xfer_pathname);

	make_path(xfer_log,gc.BasePath,"BAD-XFER.LOG");
	if ((fp = fopen(xfer_log, "rt"))) 
	{
		make_path(new_log,gc.BasePath,"BAD-XFER.$$$");
		if ((new_fp = fopen(new_log, "wt"))) 
		{
			left = false;
			while (fgets(linebuf,255,fp)) 
			{
				sscanf(linebuf,"%s %s %ld %lo",bad_real,bad_temp,&bad_fsize,&bad_ftime);
				if (strcmp(xfer_real,bad_real) || strcmp(xfer_temp,bad_temp) || 
					xfer_fsize != bad_fsize || xfer_ftime != bad_ftime) 
				{
					fputs(linebuf,new_fp);
					left = true;
				}
			}
			fclose(fp);
			fclose(new_fp);
			unlink(xfer_log);
			if (left) 
				rename(new_log,xfer_log);
			else      
				unlink(new_log);
		}
		else
			fclose(fp);
	}
}
char p_common::xfer_bad (void)
{
	if (xfer_logged) 
		return (true);
	xfer_del();
	return (false); 
}

char *p_common::xfer_okay (void)
{
char new_pathname[MAX_PATH];
char *p;

	make_path(new_pathname,gc.InboundPath);
	p = new_pathname + strlen(new_pathname);
	strcat(new_pathname,xfer_real);
	unique_name(new_pathname);
	rename(xfer_pathname,new_pathname);

//Удаление файла из запроса
        if (IsReqFile(rxfname)) DelReqFile(rxfname);

	if (xfer_logged)    
		xfer_del();
	xfer_logged=FALSE;

//Если был пропуск файла, то он нам не нужен
	if (gc.mailer.skip_file)
	{
		DeleteFile(new_pathname);
		gc.mailer.skip_file=0;
	}

	return (strcmp(p,xfer_real) ? p : NULL);
}

int p_common::xfer_init(char *fname,int fsize,int ftime)
{
char xfer_log[300];
char bad_real[300];
char bad_temp[300];
char linebuf[255];
char *p;
int  bad_fsize;
int  bad_ftime;
FILE *fp;
int skip_file;
        
	gc.mailer.skip_file=0;
	make_path(temppath,gc.InboundPath,"TEMP");	// temp inbound folder
	make_path(xfer_pathname,gc.InboundPath,fname);	// path to file in inbound
        
	skip_file=0; 
	strupr(fname);
	strcpy(xfer_real,fname);
	xfer_fsize = fsize;
	xfer_ftime = ftime;
	xfer_logged=FALSE;

	if (!access(xfer_pathname,0))
	{
		struct stat f;
		stat(xfer_pathname,&f);
		if (xfer_fsize == f.st_size && xfer_ftime == f.st_mtime)
			return (0);   
	};

	make_path(xfer_log,gc.BasePath,"BAD-XFER.LOG");

	if ((fp = fopen(xfer_log,"rt"))) 
	{
		while (fgets(linebuf,255,fp)) 
		{
			sscanf(linebuf,"%s %s %ld %lo %ld",bad_real,bad_temp,&bad_fsize,&bad_ftime,&skip_file);
			if (!strcmp(xfer_real,bad_real) && xfer_fsize == bad_fsize && xfer_ftime == bad_ftime) 
			{ 
				make_path(xfer_pathname,temppath,bad_temp);
				if (!access(xfer_pathname,0)) 
				{ 
					fclose(fp); 
					strcpy(xfer_temp,bad_temp);
					if (skip_file == 2) return 2;
					if (skip_file == 1) return 1;
					xfer_logged = TRUE; 
					if (skip_file == 0) return 0;
				}
			}
		}
		fclose(fp);
	}

   	xfer_logged = TRUE; 
    make_path(xfer_pathname,temppath);
    p = xfer_pathname + strlen(xfer_pathname);
    strcat(xfer_pathname,"BAD-XFER.000");
    unique_name(xfer_pathname);
    strcpy(xfer_temp,p);
    fp = fopen(xfer_log,"at");
	if (fp)
	{
		fprintf(fp,"%s %s %ld %lo %ld\n",xfer_real,xfer_temp,xfer_fsize,xfer_ftime,0);
        fclose(fp);
	}

//dk:Пропуск всех новых: Значение 1-отказ;2-пропуск
    if (get_cfg(CFG_COMMON,"SkipNewFile",0)) return get_cfg(CFG_COMMON,"SkipNewFile",0);
	
	return (0);
}
void p_common::pr_badxfer (void)
{
//force closing after session
	if (rxfd>0)	fclose(rxfd);
//	rxfd=NULL;
	if (xfer_bad())
		message("+HRECV: Bad xfer recovery-info saved");
	else
		message("-HRECV: Bad xfer - file deleted");
}

	void p_common::unique_name (char *pathname)
{
static char *suffix = ".000";
char *p;
int   n;

	if (existfile(pathname)) 
	{
		p = pathname;
		while (*p && *p!='.') p++;
		for (n=0; n<4; n++) 
		{
			if (!*p) 
			{
				*p=suffix[n];*(++p) = '\0';
			} 
			else 
				p++;
		}

		while (existfile(pathname)) 
		{
			p = pathname + (int)strlen(pathname) - 1;
			if (!isdigit(*p)) *p = '0';
			else 
			{
				for (n=3; n--;) 
				{
					if (!isdigit(*p)) *p = '0';
					if (++(*p) <= '9') break;
					else *p-- = '0';
				}
			}
		}
	}
}

// =========================================================================
	void p_common::errcode(const int result,char *msg)
// =========================================================================
{
	switch(result)
	{
		case PRC_ERROR:
			strcpy(msg,L("S_126"));	// carrier lost
			break;
		case PRC_REMOTEABORTED:
			strcpy(msg,L("S_497"));	// Aborted by other side
			break;
		case PRC_LOCALABORTED:
			strcpy(msg,L("S_501"));	// Aborted by operator
			break;
		case PRC_CPSTOOLOW:
			strcpy(msg,L("S_502"));	// Aborted as low CPS
			break;
		case PRC_STOPTIME:
			strcpy(msg,L("S_637"));	// Aborted due to the time limits
			break;
		default:
		case PRC_NOERROR:
			strcpy(msg,"OK");
	}
}                                              
