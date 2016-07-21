#include "stdafx.h"
#include "readcfg.h"
#include "client.h"
#include "bsy.h"
#include "binlog.h"
#include "sem.h"
#include "readflo.h"
#include "..\globals.h" 
#include "..\SUP\pr_xfer.h" 

MUTEXSEM hostsem = 0; 
/*
 * Global variables
 */
int pidcmgr = 0;		       /* pid for clientmgr */
int pidsmgr = 0;		       /* pid for server */
SOCKET inetd_socket = 0;

int BINKD_STATUS;
extern _gconfig gc;
void clear_mailstate	(void);


// =================================
	int abort_bink(void)
// =================================
{
	
	if (gc.mailer.keyboard_break ==1)
 	{
		mailer_stat(L("S_455"));	// User Abort
		modem_update();
		modem_add_listbox(L("S_68"));	// == Session aborted by user
		Sleep(1000);

		//abortflag=1;

		gc.mailer.keyboard_break=3;
		BINKD_STATUS=MODEM_ABORTED;
		return 1;
	}
    if (gc.mailer.keyboard_break ==3) return 1;

	return 0;
}



void usage ()
{
	char *s=NULL;
}

static void cfgfilename(char *source,char *dest)
{ unsigned int i,n;
  dest[0]='\"'; 
  for (i=0,n=1;i<strlen(source);i++,n++)
  {   dest[n]=source[i];
	  if (source[i]=='\\') {n++;dest[n]=source[i];}
  }
  dest[n]='\"';
  dest[n+1]=0;

 
}

/* Command line flags */
int inetd_flag = 0;		       /* Run from inetd (-i) */
int server_flag = 0;		       /* Run servermgr (-s) */
int client_flag = 0;		       /* Run clientmgr (-c) */
int poll_flag = 0;		       /* Run clientmgr, make all jobs, quit
				        * (-p) */
int quiet_flag = 0;		       /* Be quiet (-q) */
int verbose_flag = 0;		       /* Be verbose / print version (-v) */
int checkcfg_flag = 0;		       /* exit(3) on config change (-C) */
int no_MD5 = 0;			       /* disable MD5 flag (-m) */


extern int nNod;
extern int id_boss;
char *file_get_dos_name(char *buffer, const char *filename);

int binkd (CString line)
{
  CString fido; 
  CString tel; 
  CString pas; 
  CString str; 

  char buf[500];
  char param[500];
  char netnode[50];
  int zone,net,node,point;
  
  poll_flag = client_flag = 1;
  server_flag = 0;
  BINKD_STATUS=0;
  
  //Определение идентификатора босса
  char	bossinfo[MAX_BOSSLEN];
  CString  line1;
  CString  line2;

    id_boss=0;
 	while (db_get_boss_by_index(id_boss,bossinfo))
	{  get_token(line,1,line1);
	   get_token(bossinfo,1,line2); 
		if (!strcmp(line1,line2)) break;
		id_boss++;
	}

	//Очистка файлов статистики
	DeleteFile("mescomw.log");
	DeleteFile("mescomr.log");
	

  /* Init for ftnnode.c */
  nodes_init ();

  //Перенос части настроек из cfg файла в ини файл
    get_token(line,BOSSADDR,fido);
    parse_address(curmail.myaka,&zone,&net,&node,&point);

  //Выходная директория
  cfgfilename(curmail.basedir,param);
  sprintf(buf,"domain fidonet %s 2",param);
  read_domain_info (NULL, buf);
  read_domain_info (NULL, "domain fido alias-for fidonet");
  read_domain_info (NULL, "domain fidonet.org alias-for fidonet");
  
  //Адрес поинта
  sprintf(buf,"address %s",curmail.myaka);
  strcat(buf,"@fidonet");
  read_aka_list(NULL,buf);
  
  //Входная директория
  strcpy(inbound,gc.InboundPath);
  strcpy(inbound_nonsecure,gc.InboundPath);
  strcpy(param,gc.InboundPath);
  strcat(param,"\\Temp");
  strcpy(temp_inbound,param);
  
   //Станция поинта
  str=get_cfg(CFG_COMMON,"Location","");
  cfgfilename(str.GetBuffer(100),location);
  
  str=get_cfg(CFG_COMMON,"SystemName","");
  cfgfilename(str.GetBuffer(100),sysname);
  
  str=get_cfg(CFG_COMMON,"FirstName","");
  str+=" ";
  str+=get_cfg(CFG_COMMON,"SecondName","");
  cfgfilename(str.GetBuffer(100),sysop);
  
  

  
  

  readcfg ();
  
  //Нода
   get_token(line,4,tel);
  get_token(line,SESSPASW,pas);
  sprintf(buf,"node %d/%d -crypt %s %s",net,node,tel.GetBuffer(100),pas.GetBuffer(100));
  read_node_info (NULL, buf);

  percents = printq = 1;
  conlog = 4;

  
  if (sock_init ())
    Log (0, "sock_init: %s", TCPERR ());

  bsy_init ();
  BinLogInit ();
  rnd ();


  InitSem (&hostsem);

  poll_node (fido.GetBuffer(100));

  sprintf(netnode,"%0.4X%0.4X.dlo",net,node);

  clear_mailstate();
  modem_update();
  make_dlo_file (netnode,curmail.basedir);
  
  set_fips_priority(1);
	
  gc.mailer.keyboard_break=0;
  BINKD_STATUS=MODEM_OK;
  clientmgr (0);

  bsy_remove_all ();
  sock_deinit ();
  BinLogDeInit ();
  free_domain ();
  freenode ();
  strcpy(netnode+8,".?lo");freeoutbound(curmail.basedir,"*.?[Ll][Oo]");
  strcpy(netnode+8,".bsy");freeoutbound(curmail.basedir,"*.[Bb][Ss][Yy]");
  strcpy(netnode+8,".hld");freeoutbound(curmail.basedir,"*.[Hh][Ll][Dd]"); 
  strcpy(netnode+8,".csy");freeoutbound(curmail.basedir,"*.[Cc][Ss][Yy]"); 
  //strcpy(netnode+8,".req");freeoutbound(curmail.basedir,"*.[Rr][Ee][Qq]"); 
  //strcpy(netnode+8,".fls");freeoutbound(curmail.basedir,"*.[Ff][Ll][Ss]"); 
  

  //Удаление возможного атача и реквеста
  if (BINKD_STATUS==MODEM_OK)
  {
   freeoutbound(curmail.basedir,"*.[Ff][Ll][Ss]");
   freeoutbound(curmail.basedir,"*.[Rr][Ee][Qq]");
  }

  if (*pid_file && pidsmgr == (int) getpid ())
    deletefile (pid_file);
  clear_status(true);
  clear_status(false);
  return BINKD_STATUS;
}
