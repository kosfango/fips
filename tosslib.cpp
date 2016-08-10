#include "stdafx.h"			// loadlibrary
#include <ASSERT.h>
#include "structs.h"
#include <direct.h>
#include <io.h>
#include <locale.h>
#include "supercom.h"
#include "cfido.h"
#include "resource.h"
#include "lightdlg.h"
#include "gcomm.h"
#include "dettoss.h"
#include "sup\pr_xfer.h"
#include "sup\pr_hydra.h"
#include "sup\pr_zmod.h"

#define LOCAL_COPY 1
#define CARBON_COPY 2
#define DONT_SAVE 4

#define PKT_EOL 1	// end of line
#define PKT_EOM 2	// end of mail
#define PKT_EOF 3	// end of pkt

typedef int	(_stdcall *UNPACKPROC)(char *,char *,char *,char *,char *,char *);

struct bossarray
{
	char fido[50],mypoint[50];
	char pwd[80];
	int zone,net,node,point;
	int pzone,pnet,pnode,ppoint;
	int nzone,nnet,nnode,npoint;
	char routing[300];
	char dir[300],packer[300],archivename[300];
};

extern CLightDlg *gpMain;
extern CStrList gAkaList;
extern CStrList isdnlist;
extern CStrList dialbrk;
extern CStrList Aliases;
extern CStrList costuserlist;
extern CStrList income_times;
extern dettoss  gTosser;
extern CStrList magics;
extern CStrList rpathes;
extern CStrList tics;
extern CStrList faxcfg;
extern _gcomm	gcomm;
extern HINSTANCE hUni,hRar,hCab,hAce;

CStrList alt_phone;
CStrList Twits;
CStrList Carbons;
CString  gLastCalled;
CString	 name1,name2;
int		 gFirstCRCSupressed=0;

_sessiondata sessiondata; // only used in tosslib for inbound and outbound history

extern "C"
{
	int SupHandleTestForIncomingISDNCall	(int dummy); // Parameter zur Zeit dummy ...
	int Listen								(int cip);
}

extern CRITICAL_SECTION GlobalLogFileAccess;

static OLDPKTHDR    po;   // groesse jeweils 58 bytes
static NEWPKTHDR    *pn;
static MEDPKTHDR    *pm;
static MEDPKTHDR2   *pl;
static _msghdr      mh;

// just some vars for the stats
time_t startsecs,finishsecs;

// command strings for invoking the decompression programms

extern _detincome gincome;	// Holds all the global stuff for INCOME handling
extern _gconfig gc;
extern _packer	Packers[];
static char mybuf[2048];
static long oldperc=0;
char	rdbuf[MAX_RDBUF];	// buffer for file reading
int		abortflag=0,AlreadyConnected=0;
int		bFilterRe,processed=0,msgpos=0;	// bAnsiToOem,

static int mailcount[MAX_AREAS];	// statistic counters
static int maxstatnum;
static int allpackets;
static float charged_summ;
static int   charging_intervall;
static int   charging_active;
static char sbuf[500];				// Global buffer to use with sprintf
static FILE *emsi=NULL;				// filepointer for lastemsi.dat file
static time_t timeelapsed=0;		// elapsed time for session
static int intermail_zmodem=0;
static char requestmail[MAX_REQ_MAIL];
char	msgcc[MAX_MSG+3000],lwrbuf[MAX_MSG+3000],*msgbuf,*textbuf;

int	 cc_asaved_netmail_handle=-1;
int	 cc_asaved_echomail_handle=-1;
int	 g_lastcrc_error=-1;
int	 state=0;

void clear_mailstate	(void);
char *gen_pktname		(char *str,LPCSTR path);
char *add_to_sendlist	(LPCSTR path,LPCSTR name);
int  collect_bossinfo	(struct bossarray *ba,int maxboss,char *outpath);
void close_packets		(FILE **fp,FILE **fpn,int maxuplink,int wasnetmail);
void build_subj_attaches(char *path,char *subject);
void build_uplink_file	(struct bossarray &ba,MEDPKTHDR2 *pl,FILE **fp,FILE **fpn,int otherboss,int curarea,int *wasnetmail);
void rename_to_bad		(LPCSTR fname);
int  filter_re			(char *src,char *dst);
LPCSTR rest_line		(LPCSTR line);
int  toss_all_packets	(void);
int  toss_packet		(char *fname);
int	 packertype			(LPCSTR fname);
void unpack_packets		(void);
void send_command		(char *text);
int  write_statistic	(char *type);
BOOL bad_field			(char *field,char *fname);
void ClearSession		(BYTE Com);
void check_file			(LPCSTR fname,LPSTR unppath=NULL);
void make_emptypoll		(LPCSTR dir,LPCSTR mainaka,LPCSTR pwd,time_t *secs);
void check_outbound		(LPCSTR dir);
int	 append_info_dialing(void);

UNPACKPROC UnPackPrc;

// ================================ Prototypes ======================================

BOOL is_delimiter_char  (char c,LPCSTR delim);
char *find_nst_delimiter(int num,char *text,char *delim);
int	 capi_abort			(int val);
int	 init_capi_callback	(void);
void update_history		(int histid,_sessiondata *session);
char *get_currency		(void);
char *get_zone			(void);
// ### tosser utils ###
int	 get_line_from_file	(FILE *fp,LPSTR str,int *pcnt);
BOOL parse_kludges		(mailheader *hdr,LPSTR str);
void parse_subject		(mailheader *pmh,LPSTR str);
BOOL parse_origin		(mailheader *pmh,LPCSTR str);
BOOL check_twits		(mailheader *pmh,LPSTR area,LPSTR buf);
int	 check_carbons		(pmailheader pmh,LPCSTR area,LPSTR ccarea);
void process_extra_parts(FILE *fp,long hnd,long cchnd,mailheader *pmh,int state);

// ==================================================================================
// adds backslash to the end of line, if there is none
	void addbackslash(LPSTR path)
// ==================================================================================
{
	if (path && path[strlen(path)-1] != '\\')	strcat(path,"\\");
}

// ==============================================================================
	char *extractstr(int num,char *text,char *delim,char *result, int maxlen)
// ==============================================================================
//   int   num             = word to extract
//   char *text            = text to search in
//   char *delim       = 	string of delimiters,
//                            	even if not included, tabs and newlines are also
// 								seen as delim
//   char *result          = place to store result
//   int  maxlen           = maximal len of result buffer
//
//   return:
//
//   char *p                 pointer to start of found word, or NULL if not found
//
{
char *pbeg,*pend;
int  len;

   ASSERT(maxlen>0);

   // First cut leading delimiters
   while (*text)
   {
    if (is_delimiter_char(*text,delim))
       text++;
    else
        break;
   }
   if (!*text)   // No text, return 0;
      return 0;

   // now find start delimter
	if (num>1)
	{
		pbeg=find_nst_delimiter(num-1,text,delim);
		if (!pbeg)
			return 0;
		pbeg++;
		if (!pbeg)
			return 0;
	}
	else
		pbeg=text;


   // Find real start of string
   while (*pbeg)
   {
    if (is_delimiter_char(*pbeg,delim))
       pbeg++;
    else
        break;
   }

   if (!*pbeg)
       return 0;

   // Now find the next delimter
   pend=find_nst_delimiter(1,pbeg,delim);
   if (!pend)
      return 0;

   len=pend-pbeg;
   if (len > (maxlen-1))
      len=(maxlen-1);

   strncpy(result,pbeg,len);
   result[len]=0;
   return pbeg;
}

// ==============================================================================
// subfunc, used in extractword
  char *find_nst_delimiter(int num,char *text,char *delim)
// ==============================================================================
{
int  count=0;
int  last=0;

	while (*text)
	{
		if (is_delimiter_char(*text,delim))
		{
			if (last==1)
			{
				text++;
				continue;
			}
			count++;
			last=1;
			if (count==num)
				return text;

			text++;
		}
		else
		{
			last=0;
			text++;
		}
	}
	return text;
}

// ==============================================================================
// subfunc, used in extractword
	BOOL is_delimiter_char(char c,LPCSTR delim)
// ==============================================================================
{
	return (strchr(delim,c) || c=='\t' || c=='\n' || c=='\r');
}

// ==============================================================
	int set_file_time(char *filename,int day,int mon,int year,int hour,int min,int sec)
// ==============================================================
{
HANDLE   hnd;
FILETIME LastWrite; 	// address of last write time
int 		 wDosDate,wDosTime,ret,mday,mmon,myear;
char     today[50];

	_strdate(today);
	sscanf(today,"%d/%d/%d",&mmon,&mday,&myear);

  if (year > 1900)	year -= 1900;
	if (myear < 80)		myear+= 100;
	if (year < 80)		year += 100;
	if (year >= myear && mon >= mmon && day > mday)	day = mday;
	if (year >= myear && mon > mmon)	mon = mmon;
	if (year > myear)	year = myear;

	wDosDate = (WORD)((year - 80)<< 9  |  mon <<5 | day );
	wDosTime = (WORD)((sec / 2)        |  min <<5 | hour <<11);

	if (!DosDateTimeToFileTime (wDosDate, wDosTime, &LastWrite))	return 0;
	if (gc.ReadOnly)
		hnd = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,0,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	else
		hnd = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
			0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if (hnd==INVALID_HANDLE_VALUE)	return 0;
	ret=SetFileTime (hnd,&LastWrite,&LastWrite,&LastWrite);
	CloseHandle(hnd);
	return ret;
}

// ==========================
// TOSSER CODE
// ==========================

// =============================================================
    int analyse_pkt(char* fname,CStrList &out)
// =============================================================
// to_name /t from_name /t subject /t date_time /t text_offset /t mail_offset /t mail_length
{
FILE	*fp;
char	buf[1024],subj[1024],datetime[100];
int		nread,offset,c,startmail,maillength;

	out.RemoveAll();

	fp=fopen(fname,"rb");	
	if (!fp)	
		return 0;
	nread=fread(buf,1,58,fp);	
	if (nread != 58)	
	{ 
		fclose(fp); 
		return 0;	
	}
	offset=0;

	for (;;)
	{
		strcpy(subj,"");
		startmail=ftell(fp);
		maillength=0;
		nread=fread(buf,1,14,fp); // msg-hdr
		if (nread<14)	
			break;

		maillength+=nread;
		nread=fread(datetime,20,1,fp); // datetime
		maillength+=20;
		c=0;
		for (;;)
		{
			nread=fread(&buf[c],1,1,fp);  // toname
			maillength++;
			if (buf[c]==0)	
				break;
			c++;
			if (!nread) 
				return 0;
		}

		strcat(subj,buf);
		strcat(subj,"\t");
		c=0;
		for (;;)
		{
			nread=fread(&buf[c],1,1,fp);  // fromname
			maillength++;
			if (buf[c]==0)	
				break;
			c++;
			if (!nread) 
				return 0;
		}

		strcat(subj,buf);
		strcat(subj,"\t");
		c=0;
		for (;;)
		{
			nread=fread(&buf[c],1,1,fp);  // subject
			maillength++;
			if (buf[c]==0)	
				break;
			c++;
			if (!nread) 
				return 0;
		}

		strcat(subj,buf);
		strcat(subj,"\t");
		strcat(subj,datetime);
		strcat(subj,"\t");
		offset=ftell(fp); // offset to msg-text
		sprintf(datetime,"%d",offset);
		strcat(subj,datetime);
		strcat(subj,"\t");

		do 
		{
			nread=fread(buf,1,1,fp); // bis zum naechsten nuller lesen
			maillength++;
		} while (buf[0] !=0 && nread != 0);

		sprintf(datetime,"%d",startmail);
		strcat(subj,datetime);
		strcat(subj,"\t");
		sprintf(datetime,"%d",maillength);
		strcat(subj,datetime);
		out.AddTail(subj);
	}
	fclose(fp);
	return 1;
}

// =============================================================
	int toss_all_packets()
// =============================================================
{
_finddata_t se;
long	hf;
DWORD	sttime;
char	pktpath[MAX_PATH],tossbat[MAX_PATH],buf[MAX_PATH],str[MAX_PATH];
int		secs,mode,ret,retx;

//	bAnsiToOem=get_cfg(CFG_COMMON,"ConvertLatin",0);
	bFilterRe=get_cfg(CFG_COMMON,"FilterREs",0);
	Carbons.LoadFromFile("carbons.cfg");
	Twits.LoadFromFile("twitfilt.cfg");
	make_path(tossbat,gc.BasePath,"toss.bat");
	if (!access(tossbat,0))       // check existance for toss.bat
	{
		tosser_add_listbox(L("S_356"));	// pretossing
		mode=get_cfg(CFG_COMMON,"HideCmdWin",0) ? SW_HIDE : SW_SHOWMINNOACTIVE;
		retx=system_command(tossbat,"PreTossing",gc.BasePath,mode,300000,&ret);
		if (retx !=1 || ret !=0)
			tosser_add_listbox(L("S_199",retx,ret));	// error
	}

	make_path(pktpath,gc.InboundPath,"*.pkt");
	hf = _findfirst(pktpath,&se);
	if (hf != -1L)
	{
		do	allpackets+=se.size;
		while (_findnext(hf,&se)==0);
		_findclose(hf);
	}

	writelog(LOG_TOSSER,L("S_412"));	// begin tossing
	memset(mailcount,0,sizeof(mailcount));
	sprintf(buf,"TOSSER_PROZ %d %d",0,allpackets);	// init progressbar
	send_command(buf);
	gTosser.UpdateValues();
	maxstatnum=-1;
	sttime=time(NULL);

	if ((hf= _findfirst(pktpath,&se)) != -1L)	// pkt found
	{
		do
		{
			tosser_add_listbox(L("S_359",se.name));	// proccessing file
			make_path(pktpath,gc.InboundPath,se.name);
			toss_packet(pktpath);
		}while (!_findnext(hf,&se));
		_findclose(hf);
	}

	sprintf(buf,"TOSSER_PROZ %d %d",gc.tosser.m_bytes,allpackets);
	send_command(buf);
	gTosser.UpdateValues();

	time(&finishsecs);
	secs=time(NULL)-sttime;
	if (secs==0)
		secs=1;

	writelog(LOG_TOSSER,L("S_430",secs));	// ending
	tosser_add_listbox(L("S_430",secs));	// ending

	if (gc.tosser.m_mails)
	{
		writelog(LOG_TOSSER,"+=+=+=+=+=+=+=+=+=+=+=+=+");
		strcpy(str,L("S_310",gc.tosser.m_mails/secs));
		tosser_add_listbox(str);
		sprintf(buf,"|  %s",str);
		writelog(LOG_TOSSER,buf);
		strcpy(str,L("S_323",gc.tosser.m_netmails));
		tosser_add_listbox(str);
		sprintf(buf,"|  %s",str);
		writelog(LOG_TOSSER,buf);
		strcpy(str,L("S_177",gc.tosser.m_echomails));
		tosser_add_listbox(str);
		sprintf(buf,"|  %s",str);
		writelog(LOG_TOSSER,buf);
		strcpy(str,L("S_125",gc.tosser.m_carbons));
		tosser_add_listbox(str);
		sprintf(buf,"|  %s",str);
		writelog(LOG_TOSSER,buf);
		strcpy(str,L("S_434",gc.tosser.m_twits));
		tosser_add_listbox(str);
		sprintf(buf,"|  %s",str);
		writelog(LOG_TOSSER,buf);
		writelog(LOG_TOSSER,"+=+=+=+=+=+=+=+=+=+=+=+=+");
	}

// write statistic
	write_statistic("I"); // I for Import

	if (gc.tosser.m_netmails)
		system_sound("NEW_NETMAIL",1);
	else if (gc.tosser.m_carbons)
		system_sound("NEW_LOCALMAIL",1);

	return 0;
}

// =============================================================
	int toss_packet(char *fname)
// =============================================================
{
mailheader hdr;
CString	str;
FILE 	*fp;
time_t	oldtime=0,newtime;
long	hnd,cchnd,nread,filelen;
char	defuplink[300],buf[300],pkttype[300],tmp[1024];
char	ccarea[300],areatag[300],*p,*s;
int		defzone,defaultzone,defaultnet,defaultnode;
int		ld,lt,lf,ls,twit,longwarn,len,rdstate,lfcnt;
BOOL	kscan;

	memset(rdbuf,0,sizeof(rdbuf));
	gc.tosser.m_packets++;
	sprintf(mybuf,"TOSSER_PROZ %d",gc.tosser.m_bytes);
	send_command(mybuf);
	gTosser.UpdateValues();

	get_fullname(name1);
	get_fullname(name2,TRUE);
// assign pointers to valid struct
	pn = (NEWPKTHDR *)&po;
	pl = (MEDPKTHDR2 *)&po;
	pm = (MEDPKTHDR *)&po;

	if (!(fp=fopen(fname,"rb")))	return -1;
	fseek(fp,0,SEEK_END);   // get filelen
	filelen=ftell(fp);
	rewind(fp);

// read pkt-header
	nread=fread(&po,1,sizeof(OLDPKTHDR),fp);	// 58 bytes
	processed+=nread;
	if(nread < sizeof(OLDPKTHDR))		// not a 2.x packet
	{
		fclose(fp);
		SafeFileDelete(fname);			// delete an empty packet
		return TOSS_NO_VALID_PACKET;
	}

	if(po.rate == 2)	// FSC-0045 (version 2.2) packet
	{
		strcpy(pkttype,"FSC-0045 Type 2.2");
		defzone = pn->ozone;
		defaultzone = pn->ozone;
		defaultnet  = pn->onet;
		defaultnode = pn->onode;
	}
	else if(pm->capword && pm->capword==(~pm->capword2))// FSC-0039 packet
	{
		strcpy(pkttype,"FSC-0039");
		defzone = pm->orig_zone;
		defaultzone = pm->orig_zone;
		defaultnet  = pm->orig_net;
		defaultnode = pm->orig_node;
	}
	else if(pl->capword && pl->capword == pl->capword2)// FSC-0048 packet
	{
		strcpy(pkttype,"FSC-0048");
		defzone = pl->orig_zone;
		defaultzone = pl->orig_zone;
		defaultnet  = pl->orig_net;
		defaultnode = pl->orig_node;
	}
	else	// Stoneage type 2
	{
		strcpy(pkttype,"Type 2");
		defzone=po.orig_zone;
		defaultzone = po.orig_zone;
		defaultnet  = po.orig_net;
		defaultnode = po.orig_node;
	}

	writelog(LOG_TOSSER,L("S_428",fname,pkttype));	// tossing file
	send_command(L("S_428",fname,pkttype));
	make_address(defuplink,defaultzone,defaultnet,defaultnode);

	for(;;)
	{
		msgpos=0;						// msg position counter
		memset(&hdr,0,sizeof(hdr));
		nread=fread(&mh,1,sizeof(mh),fp);	// get first 7 ints
		if (nread < sizeof(_msghdr))	break;
		processed+=nread;
		hdr.structlen=sizeof(mailheader);
		time(&hdr.recipttime);       	// set reception time
		hdr.srczone =defzone;			// default zone in pkt-hdr
		hdr.dstzone =defzone;
		hdr.srcnet  =mh.srcnet;
		hdr.dstnet  =mh.dstnet;
		hdr.srcnode =mh.srcnode;
		hdr.dstnode =mh.dstnode;
		hdr.attrib   =mh.attrib;

		memset(buf,0,sizeof(buf));
		ld=sizeof(hdr.datetime);	// =20
		lt=sizeof(hdr.toname);		// =36
		lf=sizeof(hdr.fromname);	// =36
		ls=sizeof(hdr.subject);		// =72
// read mail header
		nread=fread(&buf,1,ld+lt+lf+ls,fp);
		buf[ld+lt+lf+ls-1]=0;
// get date
		p=buf;
		s=(LPSTR)memchr(p,0,sizeof(buf));
		if(s-p>=ld)	tosser_add_listbox(L("S_99"));	// bad DATE
		strncpy(hdr.datetime,p,ld-1);
		hdr.unixtime=parse_time(hdr.datetime);
// TODO: make timezone correction
// get to-name
		p=s+1;
		s=(LPSTR)memchr(p,0,sizeof(buf));
		if(s-p>=lt && bad_field(L("S_425"),fname))	// bad TO
		{
			fclose(fp);
			rename_to_bad(fname);
			return 0;
		}
		strncpy(hdr.toname,p,lt-1);
// get from-name
		p=s+1;
		s=(LPSTR)memchr(p,0,sizeof(buf));
		if(s-p>=lf && bad_field(L("S_266"),fname))	// bad FROM
		{
			fclose(fp);
			rename_to_bad(fname);
			return 0;
		}
		strncpy(hdr.fromname,p,lf-1);
// get subject
		p=s+1;
		s=(LPSTR)memchr(p,0,sizeof(buf));
		if(s-p>=ls)	tosser_add_listbox(L("S_416"));	// bad SUBJECT
		strncpy(buf,p,ls-1);
		buf[ls-1]=0;
		parse_subject(&hdr,buf);			// process RE in subject
		nread-=(s-buf)+1;
		fseek(fp,-nread,SEEK_CUR);	// move pointer to begining of mailtext
		processed+=ld+lt+lf+ls-nread;
// process mail body
		get_line_from_file(fp,buf,&lfcnt);
// get areatag
		if (strnicmp(buf,"AREA:",5))	// netmail
		{
            strcpy(areatag,"NETMAIL");
			if (maxstatnum<0)	maxstatnum=0;
			mailcount[0]++;
			gc.tosser.m_mails++;
			gc.tosser.m_netmails++;
			hnd=0;
		}
		else							// echomail
		{
			sscanf(buf+5,"%s",areatag);
			if (db_get_area_handle(areatag,&hnd,1,defuplink) != DB_OK)
				err_out("E_DBGETAHANDLE",buf);
			else if (hnd>=0 && hnd<2048)
			{
				mailcount[hnd]++;
				if (hnd > maxstatnum)	maxstatnum=hnd;
			}
			gc.tosser.m_mails++;
			gc.tosser.m_echomails++;
			processed+=strlen(buf);
		}
// presave cc-prefix
		sprintf(msgcc,langstr(CC_ENG"%s",CC_RUS"%s"),areatag);
		strcat(msgcc,"\r");
		CharToOem(msgcc,msgcc);
		msgbuf=msgcc+strlen(msgcc);
		textbuf=msgbuf;
		msgpos=0;
		longwarn=0;
		kscan=TRUE;
		textbuf=msgbuf;
// read one mail in buffer
		do
		{
			rdstate=get_line_from_file(fp,buf,&lfcnt);
			len=strlen(buf);
			if (len==0)	break;
// analyzing kludges
			if (kscan && parse_kludges(&hdr,buf))
				textbuf+=len;	// points to begining of mail text
			else
				kscan=FALSE;	// stop collecting kludges
			strcpy(msgbuf+msgpos,buf);
			msgpos+=len;
			processed+=len+lfcnt;
			if (msgpos>=MAX_MSG-1000)
			{
				tosser_add_listbox(L("S_306"));					// too long
				sprintf(msgbuf+msgpos,"\r\r%s\r",L("S_36"));	// splitted
				CharToOem(msgbuf+msgpos,msgbuf+msgpos);
				msgpos+=strlen(msgbuf+msgpos);
				strnzcpy(buf,hdr.subject,67);
				sprintf(hdr.subject,"%s [01]",buf);
				longwarn=1;
				break;
			}
		} while(rdstate != PKT_EOF && rdstate != PKT_EOM);
		strcpy(lwrbuf,msgbuf);
		strlwr(lwrbuf);
// store mail in msgbase --------------------------------
		if (rdstate==PKT_EOM)
			gc.tosser.m_bytes+=processed+1; // processed bytes (+ 1 null byte)
		else
			gc.tosser.m_bytes+=processed+3; // processed bytes (+ 3 null bytes)
		processed = 0;
		time(&newtime);
		if (oldtime != newtime) // update tosser window each second
		{
			sprintf(mybuf,"TOSSER_PROZ %d",gc.tosser.m_bytes);
			send_command(mybuf);
			gTosser.UpdateValues();
			time(&oldtime);
		}
// TwitFilter checking
		if (twit=check_twits(&hdr,areatag,buf)) 
		{
			strcpy(tmp,L("S_429",areatag,twit,buf));	// twit found
			tosser_add_listbox(tmp);
			writelog(LOG_TOSSER,tmp);
			gc.tosser.m_twits++;
			continue;
		}
// CarbonCopy checking
		cchnd=0;
		state=check_carbons(&hdr,areatag,ccarea);
// save to carbon area
		if (hnd && (state & (CARBON_COPY | LOCAL_COPY)))	// isn't netmail
		{
			if (state & LOCAL_COPY || *ccarea==0)
				strcpy(ccarea,"LOCALMAIL");

			if (db_get_area_handle(ccarea,&cchnd,TRUE,defuplink)!=DB_OK)
				ERR_MSG2_RET0("E_DBGETAHANDLE",ccarea)

			sprintf(buf,langstr(CC_ENG"%s",CC_RUS"%s"),areatag);
			tosser_add_listbox(buf);

			db_set_area_to_carboncopy(cchnd);
			hdr.mail_text=msgcc;
			hdr.text_length=strlen(msgcc);
			if (db_append_new_message(cchnd,&hdr)!=DB_OK)	err_out("E_APPMFAILED"); // CC
			gc.tosser.m_carbons++;
		}
		if (!(state & DONT_SAVE))	// write original message
		{
			hdr.mail_text=msgbuf;
			hdr.text_length=strlen(msgbuf);
			if (db_append_new_message(hnd,&hdr)!=DB_OK)		err_out("E_APPMFAILED");
		}
		if (longwarn)	// read & save extra parts
			process_extra_parts(fp,hnd,cchnd,&hdr,state);
	}

	tosser_add_listbox(L("S_432",gc.tosser.m_bytes));	// processed ... bytes
	fclose(fp);
	SafeFileDelete(fname);
	return TOSS_OK;
}

// =============================================================
// read and save extra parts of long mails
	void process_extra_parts(FILE *fp,long hnd,long cchnd,mailheader *pmh,int state)
// =============================================================
{
int  numsubj=2,msgpos,headpos,len,rdstate,lfcnt;
char subj[72],buf[300],*p;

	strcpy(subj,pmh->subject);
	p=strrchr(subj,'[');
	*p=0;
	sprintf(textbuf,"\r%s\r\r",L("S_604"));	// continuation
	CharToOem(textbuf,textbuf);
	headpos=strlen(textbuf);
	msgpos=headpos;
// save middle parts
	do
	{
		rdstate=get_line_from_file(fp,buf,&lfcnt);
		len=strlen(buf);
		if (len==0)	break;
		strcpy(textbuf+msgpos,buf);
		msgpos+=len;
		processed+=len+lfcnt;
		if (msgpos>=MAX_MSG-3000)
		{
			sprintf(textbuf+msgpos,"\r\r%s\r",L("S_36"));		// to be continued
			CharToOem(textbuf+msgpos,textbuf+msgpos);
			sprintf(pmh->subject,"%s[%02d]",subj,numsubj);
			if (cchnd && (state & (CARBON_COPY | LOCAL_COPY)))	// save copy
			{
				pmh->mail_text=msgcc;
				pmh->text_length=strlen(msgcc);
				if (db_append_new_message(cchnd,pmh) != DB_OK)	err_out("E_APPMFAILED");
			}
			if (!(state & DONT_SAVE))	// save original
			{
				pmh->mail_text=msgbuf;
				pmh->text_length=strlen(msgbuf);
				if (db_append_new_message(hnd,pmh) != DB_OK)	err_out("E_APPMFAILED");
			}
			msgpos=headpos;
			numsubj++;
		}
	}while(rdstate != PKT_EOF && rdstate != PKT_EOM);
// save end of message
	if (msgpos>headpos)
	{
		sprintf(pmh->subject,"%s[%02d]",subj,numsubj);
		if (cchnd && (state & (CARBON_COPY | LOCAL_COPY)))	// save copy
		{
			pmh->mail_text=msgcc;
			pmh->text_length=strlen(msgcc);
			if (db_append_new_message(cchnd,pmh) != DB_OK)	err_out("E_APPMFAILED");
		}
		if (!(state & DONT_SAVE))	// save original
		{
			pmh->mail_text=msgbuf;
			pmh->text_length=strlen(msgbuf);
			if (db_append_new_message(hnd,pmh) != DB_OK)	err_out("E_APPMFAILED");
		}
	}
}

// =============================================================
// reads one line from file (up to CR or \0), returns status and count of LF's
	int get_line_from_file(FILE *fp,LPSTR str,int *pcnt)
// =============================================================
{
char	chr[2],*p;
int		cnt=0;

	p=str;
	*p=0;
	*pcnt=0;
	if (feof(fp))	return PKT_EOF;
	while (fread(chr,1,1,fp) && cnt<299)
	{
		if (*chr=='\n')
		{
			(*pcnt)++;
			continue;	// skip LF
		}
		*p++=*chr;
		cnt++;
		if (*chr=='\r')
		{
			*p=0;
			return PKT_EOL;			// end of line
		}
		if (*chr=='\0')
		{
			if (fread(chr,1,2,fp)<2 || !memcmp(chr,"\0\0",2))
				return PKT_EOF;		// end of pkt
			else
			{
				fseek(fp,-2,SEEK_CUR);
				return PKT_EOM;		// end of mail
			}
		}
	}
	*p=0;
	return 0;	// splitted line
}

// =============================================================
// returns TRUE for kludges, FALSE otherwise
	BOOL parse_kludges(mailheader *pmh,LPSTR str)
// =============================================================
{
char s1[300],s2[300],s3[300],*p;

	if (*str != '\01')	return FALSE;
	sscanf(str,"\01%s %s %s",s1,s2,s3);
	if (strcmpi(s1,"INTL")==0)		// INTL kludge
	{
		parse_address(s2,&pmh->dstzone,&pmh->dstnet,&pmh->dstnode);
		parse_address(s3,&pmh->srczone,&pmh->srcnet,&pmh->srcnode);
	}
	else if (!stricmp(s1,"TOPT"))	// ToPoint kludge
		sscanf(s2,"%hu",&pmh->dstpoint);
	else if (!stricmp(s1,"FMPT"))	// FromPoint kludge
		sscanf(s2,"%hu",&pmh->srcpoint);
	else if (!stricmp(s1,"REPLY:"))	// ReplyID kludge
		sscanf(s3,"%x",&pmh->replyid);
	else if (!stricmp(s1,"MSGID:"))	// MessageID kludge
	{
		sscanf(s3,"%x",&pmh->mailid);
		p=strchr(s2,'@'); if (p) *p=0;
		parse_address(s2,&pmh->srczone,&pmh->srcnet,&pmh->srcnode,&pmh->srcpoint);
	}
/*		else if (!stricmp(s1,"CHRS:") && !stricmp(s2,"LATIN-1") && bAnsiToOem)
		{
			isAnsi = TRUE;				// Charset kludge
			return TRUE;
		}*/
	return TRUE;
}

// =============================================================
// returns TRUE for origin, FALSE otherwise
	BOOL parse_origin(mailheader *pmh,LPCSTR str)
// =============================================================
{
const char *p,*s;

	if (!strncmp(" * Origin:",str,10))	// Origin
	{
		p=strrchr(str,')');
		if (p)
		{
			s=strrchr(p,'(');
			if (s && s<p)
			{
				parse_address(s+1,&pmh->srczone,&pmh->srcnet,
					&pmh->srcnode,&pmh->srcpoint);
				return TRUE;
			}
		}
	}
	return FALSE;
}
	
// =============================================================
// parsing RE in subject: filtering or combining re:,re^n:,re[n]
	void parse_subject(mailheader *pmh,char *str)
// =============================================================
{
char dst[300],*p;
int  cnt=0,num=0,n=0;

	memset(&(pmh->subject),0,sizeof(pmh->subject));
	p=str;
next:
	while (*p && *p==' ')	p++;	// skip forwarding spaces
	if (*p==0)	return;
	if (!strnicmp(p,"re",2))
	{
		switch (*(p+2))
		{
		case ':':		// 'Re:'
//		case ' ':		// 'Re '
			p+=3;
			cnt++;
			goto next;
		case '^':		// 'Re^n:'
			if (sscanf(p+2,"^%d:%n",&num,&n))
			{
				p+=2+n;
				cnt+=num;
				goto next;
			}
			break;
		case '[':		// 'Re[n]:'
			if (sscanf(p+2,"[%d]:%n",&num,&n))
			{
				p+=2+n;
				cnt+=num;
				goto next;
			}
			break;
		}
	}
	if (bFilterRe || cnt==0)
		strcpy(pmh->subject,p);
	else
	{
		if (cnt==1)
			strcpy(pmh->subject,"Re: ");
		else
			sprintf(pmh->subject,"Re[%d]: ",cnt);
		strncat(pmh->subject,dst,71-strlen(pmh->subject));
		pmh->subject[71]=0;
	}
}

// =============================================================
// returns number of twit in twits-list or 0 if twit not found
	int check_twits(mailheader *pmh,LPSTR area,LPSTR buf)
// =============================================================
{
char s1[36],s2[36],s3[72],s4[20],line[300],tmp[300];
int  flags;

	*buf=0;
	setlocale(LC_ALL,".866");
	strcpy(s1,pmh->fromname);
	strlwr(s1);
	strcpy(s2,pmh->toname);
	strlwr(s2);
	strcpy(s3,pmh->subject);
	strlwr(s3);
	make_address(s4,pmh->srczone,pmh->srcnet,pmh->srcnode,pmh->srcpoint);
	for (int i=0;i<Twits.GetCount();i++)
	{
		strcpy(line,Twits.GetString(i));
		get_token(line,2,buf);
		trim_all(buf);
		strupr(buf);
		if (pmatch(buf,area))
		{
			flags=get_token_int(line,3);
			get_token(line,0,buf);
			strcpy(tmp,"*");
			strcat(tmp,buf);
			strcat(tmp,"*");
			CharToOem(tmp,tmp);
//			strlwr(buf);
			
			if ((flags & 1) && pmatch(tmp,s1)) // from name
			{
				OemToChar(pmh->fromname,buf);
				return i+1;
			}
			if ((flags & 2) && pmatch(tmp,s2)) // to name
			{
				OemToChar(pmh->toname,buf);
				return i+1;
			}
			if ((flags & 4) && pmatch(tmp,s3)) // subject
			{
				OemToChar(pmh->subject,buf);
				return i+1;
			}
			if ((flags & 8) && pmatch(tmp,s4)) // from address
			{
				strcpy(buf,s4);
				return i+1;
			}
		}
	}
	return 0;
}
	
// =============================================================
// returns TRUE if cc-text was found in FROM, SUBJECT or MAILTEXT
	int check_carbons(pmailheader pmh,LPCSTR area,LPSTR ccarea)
// =============================================================
{
char s1[36],s2[72],pat[200],line[1000],buf[1000],*p;
int	 ret=0,st=0,k;

	strcpy(s1,pmh->fromname);
	strcpy(s2,pmh->subject);
	strlwr(s1);
	strlwr(s2);
	*ccarea=0;
	for (k=0;k<Carbons.GetCount();k++)
	{
		strcpy(line,Carbons.GetString(k));
		get_token(line,3,buf);		// excluded areas list
		if (is_excluded_area(buf,area))	continue;
// check carbons in FROM, SUBJECT, MAILTEXT
		get_token(line,0,pat);		// search string
		CharToOem(pat,pat);
		get_token(line,1,buf);		// case sensitivity
		if (*buf=='I' || *buf=='+')	// case-insensitive
		{
			strlwr(pat);
			ret=strstr(s1,pat) || strstr(s2,pat) || strstr(lwrbuf,pat);
		}
		else						// case-sensitive
		{		
			ret=strstr(pmh->fromname,pat) || strstr(pmh->subject,pat) || strstr(msgbuf,pat);
		}
		if (ret)
		{
			st |= CARBON_COPY;
			get_token(line,2,ccarea);	// carboncopy area
			get_token(line,4,buf);		// do not save original
			st |= *buf=='+' ? DONT_SAVE : 0;
			break;
		}
	}
// check aliases in TO (case sensitive!)
	if (!name1.Compare(pmh->toname) || !name2.Compare(pmh->toname))
		st |= LOCAL_COPY;
	else
		for (k=0;k<Aliases.GetCount();k++)
		{
			strcpy(buf,Aliases.GetString(k));
			p=strchr(buf,'(');	if (p)	*p=0;
			trim_all(buf);
			if (!strcmp(pmh->toname,buf))	// SEE: case sensitive?
			{
				st |= LOCAL_COPY;
				break;
			}
		}
	return st;
}

// =============================================================
	int packertype(LPCSTR fname)
// =============================================================
{
FILE  *fp;
UCHAR buf[10];
int	  i;

	if (!(fp=fopen(fname,"rb")))
		return TOSS_FILE_NOT_FOUND;

	fread(buf,10,1,fp);
	fclose(fp);
	i=0;
	while (Packers[i].name[0])
	{
		for (int k=0;k<10;k++)
		{
			if (Packers[i].sign[k]>=0 && Packers[i].sign[k] != buf[k])
				goto nextarc;
		}
		return i; // index of packertype
nextarc:
		i++;
	}
	return TOSS_UNKNOWN_PACKER; // otherwise no valid packer
}

// =============================================================
	int unpack_file(char *fullname)
// =============================================================
{
CString	str;
char	result[300],fname[50];
int		ret,retval=0,mode;

// first trying UniPack call
	if (get_cfg(CFG_TOSSER,"EnableBuiltinArc",0) && hUni)
	{
		writelog(LOG_TOSSER,L("S_449",fullname));
		get_filename(fullname,fname);
		tosser_add_listbox(L("S_448",fname,"UniPack"));
		UnPackPrc=(UNPACKPROC)GetProcAddress(hUni,"UnPack");
		if (!UnPackPrc)
		{
			str.Format(IDS_SYSERROR,GetLastError(),"UnPack");
			AfxMessageBox(str);
			goto extpack;
		}
		ret=UnPackPrc(fullname,gc.InboundPath,NULL,NULL,"*","O");
		if (ret==0)
		{
			SafeFileDelete(fullname);
			return 0;
		}
		else
		{
			err_out("E_DEPFAULED",L("S_446",ret));
			goto extpack;
		}
	}
// external packers call
extpack:
	ret = packertype(fullname);            // get archive-type
	if (ret==TOSS_UNKNOWN_PACKER || ret==TOSS_FILE_NOT_FOUND)
		return 1;
	
	expand_external_cmd(Packers[ret].upck_cmd,gc.InboundPath,fullname,result) ;
	writelog(LOG_TOSSER,L("S_234",result));
	get_filename(fullname,fname);
	tosser_add_listbox(L("S_448",fname,Packers[ret].name));

	mode=get_cfg(CFG_COMMON,"HideCmdWin",0) ? SW_HIDE : SW_SHOWMINNOACTIVE;
	ret=system_command(result,L("S_450"),gc.InboundPath,mode,300000,&retval);

 	if (!ret || retval)
	{
		err_out("E_DEPFAULED",result,retval);
		return 1;
	}

	SafeFileDelete(fullname);
	return 0;
}

// ===========================================================
	void tossing_proc(void)
// ===========================================================
{
char path[MAX_PATH];

	make_path(path,gc.BasePath,"IMPORT");
	mkdir(path);
	unpack_packets();
	toss_all_packets();
	check_import(path);
	check_repost();
}

// =============================================================
	void unpack_packets(void)
// =============================================================
{
char	packetname[7][6] = {"*.SU*","*.MO*","*.TU*","*.WE*","*.TH*","*.FR*","*.SA*"};
long	hf;
_finddata_t se;
char	fullname[MAX_PATH];
char	fname[MAX_PATH];
char	fnew[MAX_PATH];
int		hid;
char	*p;

	hid=get_cfg(CFG_TOSSER,"TossHidden",0);
	for (int t=0;t<7;t++)
	{
		make_path(fullname,gc.InboundPath,packetname[t]);
		hf = _findfirst(fullname,&se);
		if (hf != -1L)             // packet-archive found
		{
			do
			{
				if (!(se.attrib & _A_SUBDIR) && (hid || !(se.attrib & _A_HIDDEN)))
				{
					make_path(fullname,gc.InboundPath,se.name);
					if (unpack_file(fullname))	// if error, rename to .BAD
					{
						get_filename(fullname,fname);
						tosser_add_listbox(L("S_204",fname));
						strcpy(fnew,fullname);
						if (p=strrchr(fnew,'.'))
							*p=0;

						strcat(fnew,".BAD");
						rename(fullname,fnew);
						get_filename(fnew,fname);
						tosser_add_listbox(L("S_91",fname));
					}
				}
			}while (_findnext(hf,&se) == 0);
			_findclose(hf);
		}
	}
}

// =============================================================
	int write_statistic(char * typ)
// =============================================================
{
FILE	*fp;
int		t,ret;
areadef adef;

	fp=fopen("traffic.log","at");
	if (fp)
	{
		for (t=0;t<=maxstatnum;t++)
		{
			ret=db_get_area_by_index(t,&adef);
			ASSERT(ret==DB_OK);
			if (mailcount[t]>0)
				fprintf(fp,"%s %x %s %d\n",typ,time(NULL),adef.echotag,mailcount[t]);
		}
		fclose(fp);
	}
	return 1;
}

// ==========================================
	void mailer_stat(LPCSTR text)
// ==========================================
{
	strcpy(gc.mailer.m_status,text);
}

// ==========================================
	void send_command(LPSTR text)
// ==========================================
{
	gpMain->MailSlotAlternateWrite(text);
}

// ==========================================
	void modem_add_listbox(LPCSTR text)
// ==========================================
{
char	tmp[16384];
LPCSTR	p;
int		writelogfile=0;	// in log and window

   p = text;
   if (*p == '\01')			// in logfile
   {
	   writelogfile = 1;
	   p++;
   }
   else if (*p == '\02')	// in window
   {
	   writelogfile = 2;
	   p++;
   }

   if (writelogfile == 1 || writelogfile == 0)	writelog(LOG_MAILER,p);
   if (emsi && gc.displaylevel>0)	fprintf(emsi,"LOGFILE %s\n",text);
   if (writelogfile == 2 || writelogfile == 0)
   {
			sprintf(tmp,"MAILER_ADD_LISTBOX %s",p);
			send_command(tmp);
   }
}

// ==========================================
	void tosser_add_listbox(LPCSTR text)
// ==========================================
{
char tmp[300];

   sprintf(tmp,"TOSSER_ADD_LISTBOX %s",text);
   send_command(tmp);
}

// ==========================================
	void nl_add_listbox(char *text)
// ==========================================
{
char tmp[300];

   writelog(LOG_NL,text);
   sprintf(tmp,"NL_ADD_LISTBOX %s",text);
   send_command(tmp);
}


// ==========================================
	void modem_update(void)
// ==========================================
{
   send_command("UPDATE_MAILER");
}

// ************************************************************************************
// ************************************************************************************
// ************************* MAILER STUFF  ********************************************
// ************************************************************************************
// ************************************************************************************
// ************************************************************************************


#define EMSI_REQ 1
#define EMSI_DAT 2
#define EMSI_NAK 3
#define EMSI_ACK 4
#define EMSI_INQ 5

BYTE Com;
UCHAR ch;

_emsi_dat_host host;
char   	sendlist[8192];
char   	akaarr[MAX_AKAS_EMSI][MAX_AKAS_EMSI];
int 	akacount=0;

_modemcfg gModemCfg;                      // Used for communication with LIGHT

int abort_session(void);

// =======================================
	int send_emsi_command(char * packet)
// =======================================
{
USHORT  CRC;
char    buf[20],sendbuf[8192];
int     t;

	CRC=0;
	RS_GetCRC_CCITT(packet,(WORD)strlen(packet),(short *)&CRC);
	sprintf(buf,"%04X",CRC);
	sendbuf[0]=0;
	strcpy(sendbuf,"**");
	strcat(sendbuf,packet);
	strcat(sendbuf,buf);

	if (emsi)	fprintf(emsi,"** Sending to Remote : %s\n",sendbuf);
	buf[0]=13;
	buf[1]=0;
	strcat(sendbuf,buf);

	if(Com==ISDNPORT)
	//	RS_TXPInTime(Com,sendbuf,(unsigned short)strlen(sendbuf),SEC_10,&result);
		capi_send_data(sendbuf,(unsigned short)strlen(sendbuf));
	else
		for (t=0;t<(int)strlen(sendbuf);t++)
		{
			RS_TXOnLine(Com,sendbuf[t]);
			blog_write(sendbuf[t]);
		}
	return 1;
}

// ==============================================
// Call a system by phonenumber
	int dial_system(char *phone)
// ==============================================
{
time_t	newsecs,oldsecs,lastsec=0;
char	dialbuf[800],buf[20],command[200],baudrate[400],ch;
int		ende,t,echoactive;

	oldsecs=0;
	strcpy(dialbuf,gModemCfg.m_dial);
	strcat(dialbuf,phone);

	sprintf(sbuf,">> %s",dialbuf);
	modem_add_listbox(sbuf);
	strcat(dialbuf,"\r");

	echoactive=1;

	for (t=0;t<(USHORT)strlen(dialbuf);t++)
	{
		RS_TXInTime(Com,dialbuf[t],SEC0_1);
		if (echoactive && !RS_RXInTime(Com,&ch,SEC0_5))
  			echoactive=0;
	}

	Sleep(1000);

	if (echoactive)
		do
		{
			RS_RXInTime(Com,(char *)&ch,SEC0_1);//if (==TRUE);
		} while (ch != 13);

	time(&oldsecs);
	time(&newsecs);
	ende =0;
	strcpy(dialbuf,"");

	do
	{
		if (RS_RXInTime(Com,buf,SEC0_3)==TRUE)
		{
			buf[1]=0;
			if (buf[0] == 10 && strlen(dialbuf) > 3)
			{
				strupr(dialbuf);

				if (strstr(dialbuf,"RING") != NULL) // ring bzw. ringing rausfiltern
				{
					strcpy(dialbuf,"");
					modem_add_listbox("<< RINGING");
				}
				else
    				ende = 1;
			}
			else
			{
				if (buf[0] != 13 && buf[0] != 10)
					strcat(dialbuf,buf);
			}
		}
		else
		{
			Sleep(200);
			time(&newsecs);

			if ((newsecs-oldsecs) > 60) ende = -1;

			if (lastsec!=newsecs)
			{
			   gc.mailer.m_timeelapsed=newsecs-timeelapsed; // in timeelapsed steht die startzeit
			   modem_update();
			   lastsec=newsecs;
			}
		}

		if (abort_session())
		{
			ComReset(Com);
			return 0;
		}

	} while (ende ==0);

	sscanf(dialbuf,"%s %s",command,baudrate);

	for (t=0;t<30;t++)
	{
		if ( RS_Carrier(Com) != 0)	break;
		Sleep(100);
	}

	if (RS_Carrier(Com))
	{
		// for changing bitmap indicator
		if (gcomm.DialingViaSecondModem)
			gcomm.ConnectedOn2Modem=1;
		else
			gcomm.ConnectedOn1Modem=1;
		modem_add_listbox(L("S_55"));	// Carrier detected
		sprintf(sbuf,"<< %s",dialbuf);
		modem_add_listbox(sbuf);
	}
	else
	{
		if (ende == -1)
			modem_add_listbox(L("S_62"));	// Modem timeout
		else
		{
			sprintf(sbuf,"<< %s",dialbuf);
			modem_add_listbox(sbuf);
		}
		return 0;
	}
	return 1;
}

// =============================================
	int emsi_detect (char *buf)
// =============================================
// look for emsi-sequence in incoming buffer
{
char *b;

	b=strstr(buf,"**EMSI_");
	while (b) 
	{
		if (!strncmp(b,"**EMSI_DAT",10))  return EMSI_DAT;
		if (!strncmp(b,"**EMSI_ACK",10))  return EMSI_ACK;
		if (!strncmp(b,"**EMSI_NAK",10))  return EMSI_NAK;
		if (!strncmp(b,"**EMSI_REQ",10))  return EMSI_REQ;
		if (!strncmp(b,"**EMSI_INQ",10))  return EMSI_INQ;
		b = strstr(b+7,"**EMSI_");
	}
	return 0;
}

// =======================================
	void analyse_site(char *buf)
// =======================================
{
int		len=0,bcount=0,inbracket=0,tpos=0;
char	temp[4096];

	len = strlen(buf);
	do
	{
		if (buf[0] == '[' && buf[1] != '[' && inbracket==0)
		{
			bcount++;
			inbracket=1;
			memset(temp,0,sizeof(temp));
			tpos=0;
		}
		else if (buf[0] == ']' && buf[1] != ']')
		{

			inbracket=0;

			switch (bcount)
			 {

			   case 1: strcpy(host.bbs,temp);
					   break;
			   case 2: strcpy(host.site,temp);
					   break;
			   case 3: strcpy(host.sysop,temp);
				   break;
			   case 4: strcpy(host.phone,temp);
			   default:
				   break;
			 }
		}
		else
		{
			temp[tpos]=buf[0];
			tpos++;
		}

	buf++;
	} while (len-->0);
}

// =======================================
	void analyse_emsi_dat(char *buf)
// =======================================
// analyse incoming EMSI_DAT package
{
int		len=0,bcount=0,inbracket=0,tpos=0,identfound=0,i=0;
char	temp[1024];

	len = strlen(buf);
//modem_add_listbox(buf);
	do
	{
		if (buf[0] == '{' && buf[1] != '{')
		{
			inbracket=1;
			memset(temp,0,sizeof(temp));
			tpos=0;
		}
		else if (buf[0] == '}' && buf[1] != '}')
		{
			bcount++;
			inbracket=1;

			if (identfound>0)
				identfound++;

			if (identfound==2)
				analyse_site(temp);

			if (strcmpi(temp,"IDENT")==0)
				identfound=1;

			switch (bcount)
			{
			   case 2:
					{
			   		  char tmpaka[8192];
					  int t,num;
					  strcpy(tmpaka,temp);

			   		  strcpy(host.adr_list,temp);
					  i=strcspn(temp,"@ ");
					  temp[i]=0;
					  parse_address(temp,&host.zone,&host.net,&host.node,&host.point);

					  num = count_tokens(tmpaka);
					  akacount=0;

					  if (num>MAX_AKAS_EMSI) num=MAX_AKAS_EMSI;

					  if (num>0)
						for (t=0;t<num;t++)
						{
							int zone,net,node,point;
							extractstr(t+1,tmpaka," ",akaarr[akacount],80);
							akacount++;
							i=strcspn(akaarr[t],"@ ");// strip domain 2:2494/17@fidonet.org
							akaarr[t][i] = 0;
							parse_address(akaarr[t],&zone,&net,&node,&point);
							sprintf(akaarr[t],"%d:%d/%d.%d",zone,net,node,point);
						}
					}
					break;
			   case 3:
					strcpy(host.pwd,temp);
					break;
			   case 5:
					strcpy(gc.mailer.protocol,temp);
					break;
			   case 7:
					strcpy(host.mailer,temp);
					break;
			   case 8:
					strcat(host.mailer," ");
					strcat(host.mailer,temp);
					break;
			   case 9:
					strcat(host.mailer," ");
					strcat(host.mailer,temp);
					break;
			   default:
					break;
			 }

		}
		else
		{
		   temp[tpos]=buf[0];
		   tpos++;
		}

		buf++;
	} while (len-->0);
}

// ==========================================
// translates stop-bytes
	void translate_stop_chars(CString &str)
// ==========================================
{
	str.Replace("]","]]");
	str.Replace("}","}}");
}

// ==========================================
// prepares EMSI-string for transferring
	void make_emsi_dat(LPSTR pak,LPCSTR pwd,LPCSTR mainaka,int ISDN,int nohydra,int nozmod)
// ==========================================
{
CString tmp,s1,s2,s3,s4,s5;
char akabuf[2048],buf[4096],buf1[5120];
int  i;

	strcpy(akabuf,mainaka);
	db_build_akas(akabuf,FALSE);		// make aka's string (only for current boss)
	rtrim(akabuf);
	sprintf(buf,"{EMSI}{%s}{%s}{8N1,PUA}{",akabuf,pwd);                                                // sessionpassword
    if (!nohydra) strcat(buf,"HYD,");
    if (nohydra || !nozmod) strcat(buf,"ZAP,");
	strcat(buf,"ARC,FNC}{0110}{");
	strcat(buf,get_versioninfo(buf1,1));	// application name
	strcat(buf,"}{");
	strcat(buf,get_versioninfo(buf1,2));	// build number
	strcat(buf,"}{Win32}{IDENT}");
	ASSERT (strlen(buf)<sizeof(buf));
	
	s1=get_cfg(CFG_COMMON,"SystemName","");
	translate_stop_chars(s1);
	s2=get_cfg(CFG_COMMON,"Location","");
	translate_stop_chars(s1);
	s3=gc.FirstName;
	translate_stop_chars(s1);
	s4=gc.SecondName;
	translate_stop_chars(s1);
	s5=get_cfg(CFG_COMMON,"Telephone","");
	translate_stop_chars(s1);
	tmp.Format("{[%s][%s][%s %s][%s][9600][MO]}",s1,s2,s3,s4,s5);
	ASSERT (strlen(buf)+tmp.GetLength()<sizeof(buf));
	strcat(buf,tmp);
// check highascii's and convert them
	i=0;
	*buf1=0;
	for (size_t t=0;t<strlen(buf);t++)
	{
		if ((UCHAR)buf[t] <=127)
			buf1[i++]=buf[t];
		else
		{
			sprintf(buf1+i,"\\%02x",(UCHAR)buf[t]);
			i+=3;
		}
	}
	buf1[i]=0;
	strcpy(pak,"EMSI_DAT");
	sprintf(buf,"%04X",(UCHAR)strlen(buf1));
	strcat(pak,buf);
	strcat(pak,buf1);
	ASSERT (strlen(pak)<2000);
}

//===================================================
	int check_oldfilelist(_tmpfile *list,char *name, int *count, char *path)
//===================================================
{
time_t akttime=0;
char tmp[300];
int t,found;

	for (t=0;t<*count;t++)
	{
		if (!strcmpi(list[t].filename,name))
			return 1; // file is in list
	}

	// datei nicht in liste, also aufnehmen, gegebenenfalls einen eintrag loeschen

	if (*count<9) // liste noch nicht voll
	{
		(*count)++;
		time(&akttime);
		strcpy(list[*count].filename,name);
		list[*count].timestamp=akttime;
		return 1;
	}

	// liste ist voll,also den aeltesten eintrag loeschen

	found=-1;
	akttime=0xffffffff;

	for (t=0;t<=*count;t++) // alteste datei ermitteln
	{
		if ((UINT)list[t].timestamp <= (UINT)akttime)
		{
			akttime=list[t].timestamp;
			found=t;
		}
	}

	if (found==-1)
		return 0;

	// jetzt die alte datei loeschen
	strcpy(tmp,path);
	strcat(tmp,list[found].filename);
	_unlink(tmp);

	if (strlen(list[found].filename) > 0)
	{
		sprintf(tmp,langstr(
			"Erasing broken file %s","Удаление поврежденного файла %s"),list[found].filename);
		modem_add_listbox(tmp);
	}

	// aeltestes element ersetzen
	time(&akttime);
	list[found].timestamp = akttime;
	strcpy(list[found].filename,name);
	return 1;
}

//==================================================
	int clear_temporary_inbound(void)
//==================================================
// save 10 actual fls for crash-recovery in inbound
{
CString	tempin;
char	tmpfile[300];
char	tmp[300];
FILE	*fp;
int		count;
struct	_finddata_t se;
long	hf;
struct	_tmpfile oldfilelist[11];

	memset(oldfilelist,0,sizeof(struct _tmpfile)*11);

	tempin.Format("%s\\TEMP\\",gc.InboundPath);
	strcpy(tmpfile,tempin);			// filename of "old file"
	strcat(tmpfile,"$TMPFILE.$$$");

	count=0;
	fp=fopen(tmpfile,"rb");
	if (fp)
	{
		count=fread(oldfilelist,sizeof(struct _tmpfile),10,fp);
		fclose(fp);
	}

	// check existance of new fls
	strcpy(tmp,tempin);
	strcat(tmp,"*.*");

	hf = _findfirst(tmp,&se);
	if (hf != -1L)
	{
		do
		{
			if (!(se.attrib & _A_SUBDIR) && strcmp(se.name,"$TMPFILE.$$$") != 0)
				check_oldfilelist(oldfilelist,se.name,&count,PS tempin);

		} while (!_findnext(hf,&se));

		 _findclose(hf);
	}

	fp=fopen(tmpfile,"wb"); // rewrite list
	if (fp)
	{
		fwrite(oldfilelist,sizeof(struct _tmpfile),count,fp);
		fclose(fp);
	}
	return 1;
}

//=======================================================================================================
	int find_a_new_name(char *name)
//=======================================================================================================
{
	strupr(name);
	if (name[strlen(name)-1] == 'T' && // PKT ?
		name[strlen(name)-2] == 'K' &&
		name[strlen(name)-3] == 'P')
	{
		char tmp[300];
		int counter=0;

		while (name[strlen(name)-1] != '\\') // filename weg vom pfad
			name[strlen(name)-1]=0;

		sprintf(tmp,"%s%08X.PKT",name,counter);

		while (!access(tmp,0))
		{
			counter++;
			sprintf(tmp,"%s%08X.PKT",name,counter);
		}
		strcpy(name,tmp); // jetzt haben wir was gefunden, also zurueckgeben
	}
	else	// normal file
	{
		int counter=0;
		int firsttime=0;

		while (!access(name,0))
		{
			char *p;
			counter++;

			if (strlen(name) < 1)
				return 0;

			name[strlen(name)-1] = name[strlen(name)-1]+1;

			p = &name[strlen(name)-1];

			if (*p < '0' || *p > 'Z')
				*p = '0';
			else
				if (*p > '9' && *p < 'A')
					*p='A';

			if (counter > 40)
			{
				if (strlen(name) > 1)
				{
					if (!firsttime)
					{
						name[strlen(name)-2] = '0';
						firsttime=1;
					}
					else
						name[strlen(name)-2] = name[strlen(name)-2]+1;

					name[strlen(name)-1] = '0';
				}
				counter=0;
			}
		}
	}

return 1;
}

//=======================================================================================================
// wird von COMMAPI Info (unterhalb) benoetigt
	int check_existing_file(char *path,long *startbyte,PFDATA pFD,int makenewname)
//=======================================================================================================
{
int		returnvalue=0;
_finddata_t se;
char	tmp[300];
static unsigned short y,mo,d,h,mi,s; // filedate/time on harddisk !
long	hf;

	startagain:

	strcpy(tmp,path);
	strcat(tmp,pFD->FileName);

	if (_access(tmp,0)==0) // gibt's die datei schon
	{

		hf = _findfirst(tmp,&se);
		if (hf != -1L)
		{
			RS_FileDateToDos(se.time_write+RS_GetTimeZone(),&y,&mo,&d,&h,&mi,&s);
			mo++; // monate fangen mit 0 an !

			if (gc.displaylevel > 1)
			{
				sprintf(sbuf,"ZM %02d.%02d.%02d %02d:%02d:%02d",pFD->FileDate.Day,pFD->FileDate.Month,pFD->FileDate.Year,pFD->FileDate.Hour,pFD->FileDate.Min,pFD->FileDate.Sec);
				modem_add_listbox(sbuf);
				sprintf(sbuf,"FL %02d.%02d.%02d %02d:%02d:%02d",d,mo,y,h,mi,s);
				modem_add_listbox(sbuf);
			}

			if ((pFD->FileDate.Year == (int)y 	) &&
				(pFD->FileDate.Month== (int)mo 	) &&
				(pFD->FileDate.Day 	== (int)d 	) &&
				//(pFD->FileDate.Hour== (int)h 	) &&
				(pFD->FileDate.Min 	== (int)mi 	) &&
				(pFD->FileDate.Sec 	== (int)s 	))
			{											// zeit stimmt

				if (pFD->FileSize == (int)se.size) // file liegt schon da
				{
					returnvalue = 2;						// skipfile !
					sprintf(sbuf,"File exists, skipping !");
					modem_add_listbox(sbuf);
				}
				else
					if ((int)pFD->FileSize >= (int)se.size)
					{
						unsigned long tempval;

						tempval=se.size;
						*startbyte=0;
						*startbyte=(long)tempval;
							 									//	crash-recovery aufrufen
						returnvalue = 1;
					}

			}
			else // neuen dateinamen festlegen
			{
				if (makenewname) // wenn die neue datei umbenannt werden soll
				{
					char tmp[300];
					char *p;

					strcpy(tmp,path);
					strcat(tmp,pFD->FileName); // war fehler mit endlosschleife
					find_a_new_name(tmp);
					p = tmp+strlen(tmp);

					while (p>tmp && *p != '\\')
						p--;
					p++;

					strcpy(pFD->FileName,p);
					//pFD->FileName[strlen(pFD->FileName)-1]++;
					sprintf(sbuf,"File exists, changing to %s",pFD->FileName);
					modem_add_listbox(sbuf);
					strcpy(gc.mailer.m_filename,pFD->FileName);
					modem_update();
					goto startagain;
				}
				else
					returnvalue = 3;  // alte datei muss umbenannt werden !
			}
			_findclose(hf);
		}
	}
	return(returnvalue);
}

//=======================================================================================================
// updates charging if active
	void update_charging(void)
//=======================================================================================================
{
	if (!charging_active)	return;
	charged_summ+=get_charging_sum(&charging_intervall);
	sprintf(gc.mailer.m_charge,"(%2.2f%s)",//, %d%s)",
		charged_summ,get_currency());//,charging_intervall+1,langstr("s","с"));
	modem_update();
}

//=======================================================================================================
	int COMMAPI Info(BYTE Com, BYTE Action, DWORD Data, PFDATA pFD)
// ======================================================================================================
{
static	time_t startsec=0;
time_t	aktsec;
static	time_t oldaktsec=0;
static int active=0;
static unsigned long filesize;
char	cps[10]={"0"};
long	diff;
long	crashrecovery=0;
static long startbyte;
static int lastcrcpos=-1;
static int dupecrccount=0;
static unsigned short y,mo,d,h,mi,s; // filedate/time on harddisk !
static char fname[300];
static int oldremain=0;
static int same_crc_counter=0;
FILE	*fpx;
int		t;
char	tmp[300];
char	str[300];
char	sbuf[200];
CString	basedir;

	if (pFD!=NULL)
		get_filename(pFD->FileName,tmp); //wir kommen nicht aus der capi_abort

	update_charging();

	if (Action== acUSERBREAK)	  // User breaked
		return infBREAK;

	if (Action== acRXING)	  // filedatetime sets
	{
		char tmp[300];

		if (gc.displaylevel>0)
		{
			sprintf(sbuf,"FileDate %d.%d.%d %d:%d:%d",pFD->FileDate.Day,pFD->FileDate.Month,
				pFD->FileDate.Year,pFD->FileDate.Hour,pFD->FileDate.Min,pFD->FileDate.Sec);
			modem_add_listbox(sbuf);
		}
		make_path(tmp,gc.InboundPath,"temp\\");
		strcat(tmp,pFD->FileName);
		set_file_time(tmp,pFD->FileDate.Day,pFD->FileDate.Month,pFD->FileDate.Year,pFD->FileDate.Hour,
	   			pFD->FileDate.Min,pFD->FileDate.Sec);
	}

	if (Action==acRETRY)
	{
	    if (gFirstCRCSupressed==0)
			gFirstCRCSupressed=1;
		else
		{
			if (lastcrcpos==(int)Data)
			   dupecrccount++;

			lastcrcpos=Data;

			// Wir wollen hier bei gleichen CRC Stellen nur jeden 4'ten anzeigen ...
			if (g_lastcrc_error==lastcrcpos)
				same_crc_counter++;
			else
				same_crc_counter=0;

			if (g_lastcrc_error!=lastcrcpos || same_crc_counter>3)
			{
				dupecrccount=0;
				sprintf(sbuf,langstr("... retry at position %d","... повтор с позиции %d"),Data);
				modem_add_listbox(sbuf);
			}
			g_lastcrc_error=lastcrcpos;
		}
	}

	if (Action== acRESEND) // gegenueber forderte erneutes senden eines blockes an
	{              // other side requested to resend block
		sprintf(sbuf,langstr("... resending from position %d","... перепосылка с позиции %d"),Data);
		modem_add_listbox(sbuf);
	}

	if (Action==acFILENAME || Action==acFILEDATA) // filetransfer begins
	{
		startbyte=0;

		if (Action==acFILEDATA) // beim senden einer datei werden hier die params angefordert
		{
		   // filesize und filedate werden von mir in der supercom gesetzt.
		}

		if (Action==acFILENAME)
   		{
		 	CString	basepath;
		 	CString	temppath;
			int		xret;

 			basepath.Format("%s\\",gc.InboundPath);

			temppath=basepath;
			temppath+="TEMP\\";

			if (strchr(pFD->FileName,'*') || strchr(pFD->FileName,'?'))
			{
				char *p;

				p = pFD->FileName;
				while (*p)
				{
					if (*p == '*') // * to #
						*p='#';

					if (*p == '?') // ? to !
						*p='!';

					// fix 1.0
					// wenn asc(*p) > 127 auf ungueltiges zeichen setzen

					if (*p > 127) // >127 nach @
						*p='@';

					p++;
				}
			}

			// jetzt nachsehen ob eine die gleiche datei im inbond liegt
			// dann muessen wir ein skip-file schicken oder die Datei umbenennnen !

			// 3 = file changed
			// 2 = skip file
			// 1 = crash-recover
			// 0 = nothing

			xret = check_existing_file(PS basepath,&startbyte,pFD,0);

			// wenn im basepath-check ein skipfile kommt, dann muessen wir
			// das an das zmodem weitergeben
			if (xret==2)
				crashrecovery = 2;

			if (xret==1) // neue Datei muss umbenannt werden
			{
				char newfile[300];
				char oldfile[300];

				strcpy(newfile,basepath);
				strcat(newfile,pFD->FileName);
				strcpy(oldfile,newfile);
				find_a_new_name(newfile);
				get_filename(newfile,pFD->FileName);

				sprintf(sbuf,langstr("File exists, renaming to %s","Файл существует, переименован в %s"),
					_strupr(newfile));
				modem_add_listbox(sbuf);
			}

			if (xret==3)
			{
				//lediglich die "alte datei" im inbound muss umbenannt werden
				char oldfile[300];
				char newoldfile[300];

				strcpy(oldfile,basepath);
				strcat(oldfile,pFD->FileName);
				strcpy(newoldfile,oldfile);
				find_a_new_name(newoldfile);

				sprintf(sbuf,langstr("Renaming File %s to %s","Файл %s переименован в %s"),
					_strupr(oldfile),_strupr(newoldfile));
				modem_add_listbox(sbuf);

				rename(oldfile,newoldfile);
			}
			// jetzt nachsehen ob eine "halbe" datei im temp-inbond liegt
			// dann muessen wir ein zmodem-resume ausfuehren, evtl. die alte loeschen
			xret = check_existing_file(PS temppath,&startbyte,pFD,1);
			// wenn crash-recover, dann an zmodem zurueckgeben
			if (xret==1)
			{
				modem_add_listbox(langstr("ZModem Crash Recovery","Восстановление ZModem после сбоя"));
				sprintf(sbuf,langstr("Resuming from Byte %d","Продолжение с байта %d"),startbyte);
				modem_add_listbox(sbuf);
				crashrecovery = 1;
			}
			// ein skip-file im temp-inbound gibts nicht
			// resume-file schreiben
			if (Action==acFILENAME)
			{
				make_path(str,gc.BasePath,"fips.zcr");
				fpx=fopen(str,"wb");
				if (fpx)
				{
					fwrite(pFD,1,sizeof(TFDATA),fpx);
					fclose(fpx);
				}
			}
		}

		active=1;
		time(&startsec);
		filesize = pFD->FileSize;
		get_filename(pFD->FileName,tmp);

		if (strlen(pFD->FileName)>0)
		{
			if (filesize < 10240)
	   			sprintf(sbuf,langstr("%s %1.0f Bytes","%s  %1.0f Байт"),tmp,filesize);
   			else if (filesize<(1024*1024))
				sprintf(sbuf,langstr("%s %8.1f kB","%s  %8.1f КБайт"),tmp,(float)filesize/1024);
			else
				sprintf(sbuf,langstr("%s %4.2f MB","%s  %4.2f МБайт"),tmp,(float)filesize/1024/1024);

			strcpy(str,langstr("File ","Файл "));
			strcat(str,sbuf);
			modem_add_listbox(str);
			strcpy(gc.mailer.m_filename,sbuf);
			strcpy(fname,sbuf);
		}
	}

	if (Action==acRESUME) // file resume from remote
	{
		startbyte= Data;
		if (startbyte != 0) // sequence also sent when starting a new file
		{
   			sprintf(sbuf,langstr("Resuming from %d","Восстановление с позиции %d"),startbyte);
			modem_add_listbox(sbuf);
		}
	}

	if (Action==acDELETEFILE)   // file will be erased by zmodem
	{
		if (gc.displaylevel>0)
		{
			sprintf(sbuf,langstr("Deleting File %s","Удаление файла %s"),tmp);
			modem_add_listbox(sbuf);
		}
	}

	if (Action==acENDFILEXFER)  // file was completely transmitted
	{
		active=0;
		if (gc.displaylevel>0)
		{
			sprintf(sbuf,langstr("File transfer of %s complete !","Передача файла %s завершена"),tmp);
			modem_add_listbox(sbuf);
		}
		strcpy(gc.mailer.m_filename,"");
		g_lastcrc_error=-1;
	}

	if (Action==acRXFIN || Action == acTXFIN )  // file, m_cps
	{
 		CString basepath;
 		CString temppath;
 		char tmp[300];

		if (Action==acRXFIN)
		{
			basepath.Format("%s\\",gc.InboundPath);
			strcpy(tmp,basepath);
			strcat(tmp,pFD->FileName);
			set_file_time(tmp,pFD->FileDate.Day,pFD->FileDate.Month,
							 pFD->FileDate.Year,pFD->FileDate.Hour,
							 pFD->FileDate.Min,pFD->FileDate.Sec);

	   		strcpy(gc.mailer.m_filename,""); // file-feld leer machen
		}
		else
		  get_filename(pFD->FileName,tmp);

	   // ohne pfad ins mailer-window
	   sprintf(sbuf,"\02%s, %d cps",tmp,gc.mailer.m_cps);
	   modem_add_listbox(sbuf);

	   // mit pfad ins logfile
	   sprintf(sbuf,"\01%s, %d cps",pFD->FileName,gc.mailer.m_cps);
	   modem_add_listbox(sbuf);

	   // jetzt muessen wir nach dem erfolgreichen transfer die datei vom temp-inbound
	   // ins echte inbound renamen

	   if (Action==acRXFIN) // nur beim receiven !
	   {
			basepath.Format("%s\\",gc.InboundPath);
			temppath=basepath+"TEMP\\"+pFD->FileName;
			basepath+=pFD->FileName;

			if (_access(temppath,0)==0)
			if (rename(temppath,basepath))
			{
				sprintf(sbuf,langstr(
					"Cannot move %s to the INBOUND dir !",
					"Невозможно перенести %s в папку INBOUND"),temppath);
				modem_add_listbox(sbuf);
			}
			else
			{
	   			set_file_time(PS basepath,pFD->FileDate.Day,pFD->FileDate.Month,
	   							 pFD->FileDate.Year,pFD->FileDate.Hour,
	   							 pFD->FileDate.Min,pFD->FileDate.Sec);
			}
	   }
  }

	time(&aktsec);
	if (pFD!= NULL)
	{
		diff=aktsec-startsec;

		if (diff > 0 )
			diff = (Data-startbyte)/diff;
		else
			diff =0;

		if (active==1)
			gc.mailer.m_transferred=Data;

		gc.mailer.m_total=pFD->FileSize;

		if (filesize>0)
			gc.mailer.m_proz=(Data*100)/filesize;
		else
			gc.mailer.m_proz=0;

		gc.mailer.m_cps=diff;
		oldaktsec=0;

		if (diff != 0)
		{
			t = (int)((filesize-Data)/diff);

			if (t > 0 && t != oldremain && aktsec != timeelapsed) // remaining zeit hat sich geandert
			{
				sprintf(sbuf,"%s (%02d:%02d:%02d)",fname,t/3600,(t%3600)/60,t%60);
		   		strcpy(gc.mailer.m_filename,sbuf);
			}
		}
	}

	gc.mailer.m_timeelapsed=aktsec-timeelapsed;

	if (oldaktsec != aktsec)
		modem_update();

	oldaktsec=aktsec;

	if (abort_session())	// check for userabort
		return infBREAK;

	if (crashrecovery==1)	// if crashrecovery detected
	{
		crashrecovery=0;
		return infZRECOVERY;
	}
	else if (crashrecovery==2)	// if skipfile detected
	{
		crashrecovery=0;
		return infSKIP;
	}

	return infCONTINUE;
}

//=========================================
	int UserBreak(int i)
//=========================================
// Diese Funktion wird mittels userbreak_set_callbackfunc(USERBREAKCALLBACK funcptr)
// gesetzt und von der supercom regelmaessig aufegrufen !
// return 1 simuliert userbreak in supercom
{
static long last_tick=0;
long cur_tick=0;

	if (gc.mailer.keyboard_break ==1)
		return 1;

	return 0;

	if (Com != ISDNPORT)
	{

		// Damit wir nur max. 5 mal pro Sekunde diesen daemlichen RS_Carrier aufrufen ..
		cur_tick=GetTickCount();
		if (last_tick && ((cur_tick-last_tick)<200))
		{
			last_tick=cur_tick;
			return 0;
		}

		if (!RS_Carrier(Com) && AlreadyConnected==1)
		{
			mailer_stat(langstr("Carrier Lost !","Потеря несущей !"));
			modem_update();
			modem_add_listbox(langstr("== Modem Lost Carrier !","== Модем потерял несущую !"));
			abortflag=1;
			return 1;
		}
	}
	else // check isdn for carrier-lost
	{
		if (!capi_getconnectstate()&& (AlreadyConnected==1))
		{
			modem_add_listbox(langstr("Disconnect from ISDN detected !",
				"Обнаружено отключение от сети ISDN !"));

			// hier kommen wir rein wenn die andere Seite mehr oder weniger lustlos aufgelegt hat.
			// Also rufen wir von uns aus den hangup auf, der sollte auch mit einer
			// abgebrochenen Verbindung klarkommen ...
//			capi_hangup();
			abortflag=1;

			// Im einen hoffentlich neutralen Zustand zu erreichen  ...
//			init_capi_stuff();
			return 1;
		}

	}
	return 0;
}

// =================================
	void add_file_to_sendlist(char *name,char *complete)
// =================================
{
FILE	*pf;
char	buf[256];

	if (strcmpi(name,FREQ_LIST) && strcmpi(name,XFER_LIST))	// not FILES.REQ and XFER.FLS
	{
		strcat(sendlist,"+");
		strcat(sendlist,complete);
		strcat(sendlist," ");
	}
	else if (!strcmpi(name,XFER_LIST))
	{
		pf = fopen(complete,"rt");
		if (!pf) 
			return;

		memset(buf,0,sizeof(buf));
		while (fgets(buf,sizeof(buf),pf))
		{
			rtrim(buf);
			if (strlen(buf) > 0)
			{
				strcat(sendlist,buf);
				strcat(sendlist," ");
			}
			memset(buf,0,sizeof(buf));
		}
		fclose(pf);
	}
}

// ======================================================
	int set_comport_options(UCHAR lcom,_modemcfg *modemcfg,int baudrate)
// ======================================================
{
int  rate;
//data=7,8
//stop=1,2
//parity='O','E','N'
//flow=SIGNAL_CTS,SIGNAL_XON;
	rate=baudrate ? baudrate : gModemCfg.m_baud;
	ComSetState(lcom,rate,gModemCfg.m_data,gModemCfg.m_stop,gModemCfg.m_parity,gModemCfg.m_flow);
	return 1;
}

// =================================
	int abort_session(void)
// =================================
{
	if (gc.mailer.keyboard_break ==1)
 	{
		mailer_stat(langstr("User Abort !","Прервано оператором !"));
		modem_update();
		modem_add_listbox(langstr("== Session aborted by user","== Сессия прервана оператором"));
		Sleep(1000);

		abortflag=1;

		gc.mailer.keyboard_break=3;

		if (Com == ISDNPORT)
		{
			if (capi_getconnectstate())
			{
				Sleep(200);
				if (capi_hangup())
					modem_add_listbox(langstr("== Disconnect successfull","== Успешное рассоединение"));
				else
					modem_add_listbox(langstr("== Disconnect failed","== Ошибка рассоединения"));
		 	}
			 Sleep(800);

//			 StopCapiWorkerThread();

		//	 free_capi_stuff();
		}
		return 1;
	}

	if (Com != ISDNPORT)
	{
		if (RS_Carrier(Com)==0 && AlreadyConnected==1)
		{
			mailer_stat(langstr("Carrier Lost !","Потеря несущей !"));
			modem_update();
			modem_add_listbox(langstr("== Modem Lost Carrier","== Модем потерял несущую"));
			abortflag=1;
			return 1;
		}
	}
	else // check isdn for carrier-lost
	{
		if (!capi_getconnectstate() && AlreadyConnected==1)
		{
			modem_add_listbox(langstr("== Disconnect from ISDN detected",
				"== Обнаружено рассоединение с сетью ISDN"));
			capi_hangup();
			abortflag=1;
			init_capi_stuff();
			return 1;
		}
	}
	return 0;
}

// =========================================================================
	void zm_errcode(int result,char *resultstring)
// =========================================================================
{
	strcpy(resultstring,"OK");

	if (result==-9999)
		strcpy(resultstring,"ZModem TIMEOUT");
	if (result==-9998)
		strcpy(resultstring,"ZModem CANCELED");
	if (result==-9997)
		strcpy(resultstring,"ZModem BAD LINE STATUS");
	if (result==-9996)
		strcpy(resultstring,"ZModem GOT CANCELED");
	if (result==-9995)
		strcpy(resultstring,"ZModem ERROR NO HEADER");
	if (result==-9994)
		strcpy(resultstring,"ZModem ERROR BAD STRUCTURE");
}


// =========================================================================
	int append_file(char *from, char *to)
// =========================================================================
{
FILE *fp1;
FILE *fp2;
char tmp[300];

	if (_access(to,0))
		fp1=fopen(to,"wt");
	else
		fp1=fopen(to,"at");

	if (!fp1)
		return 0;

	fp2=fopen(from,"rt");
	if (!fp1)
	{
		fclose(fp1);
		return 0;
	}

	while (fgets(tmp,sizeof(tmp),fp2)!=NULL)
		fprintf(fp1,"%s\n",tmp);

	fclose(fp1);
	fclose(fp2);
	return 1;
}

// =========================================================================
	int check_for_packets(LPCSTR aka,LPCSTR mainpath,int process_requests=1)
// =========================================================================
{
_finddata_t se;
char	outbpath[MAX_PATH],path[MAX_PATH],maindir[MAX_PATH],tdir[MAX_PATH],tmp[300];
int		zone,net,node,point;
long	hf;

	make_path(maindir,mainpath);
	mkdir(maindir);
	parse_address(aka,&zone,&net,&node,&point);
	make_fidodir(path,zone,net,node,point);
	make_path(outbpath,gc.OutboundPath,path);
	if (!access(outbpath,0) && strcmpi(outbpath,mainpath))	// check for fls to be sent
	{
// dir exists, now check for files to be sent

		sprintf(sbuf,langstr("Checking OUTBOUND for %s","Проверка OUTBOUND для %s"),aka);
		modem_add_listbox(sbuf);

		make_path(path,outbpath,"*.*");
		if ((hf = _findfirst(path,&se)) != -1L)
		{
			do 	
			{
				  make_path(tdir,outbpath,se.name);
				  if (!(se.attrib & _A_SUBDIR))
				  {
						if (!strcmpi(se.name,FREQ_LIST))
						{
							if (process_requests)
							{
								make_path(path,maindir,FREQ_LIST);
								sprintf(sbuf,langstr("Appending file requests for %s",
									"Добавление запроса файлов для %s"),aka);
								modem_add_listbox(sbuf);

								if (!append_file(tdir,path))
								{
									sprintf(sbuf,langstr("Unable to append file requests!",
										"Невозможно добавить запрос файла!"));
									modem_add_listbox(sbuf);
								}
								SafeFileDelete(tdir); // deleting files.req
							} // end of process-requests
						}
						else
						{
							if (!strcmpi(se.name,XFER_LIST))
							{
								FILE *fp;

								make_path(path,maindir,XFER_LIST);

								if (access(tdir,0)==0)
								{
									sprintf(sbuf,langstr("Appending file transfers for %s",
										"Присоединение файла для %s"),aka);
									modem_add_listbox(sbuf);

									if (!append_file(tdir,path))
									{
										sprintf(sbuf,langstr("Unable to append all file transfers",
											"Невозможно присоединить все файлы"));
										modem_add_listbox(sbuf);
									}
									else
									{
										fp = fopen(tdir,"rt");
										if (fp)
										{
											while (fgets(tmp,sizeof(tmp),fp))
											{
												strcat(sendlist,tmp);
												strcat(sendlist," ");
											}
											fclose(fp);
										}
										SafeFileDelete(tdir);
									}
								}
							}
							else
								if (se.name[0] != '.')
									add_file_to_sendlist(se.name,tdir); // otherwise editing
						}
					}
			} while (_findnext(hf,&se) == 0);
			_findclose(hf);
		}
	}
	return 1;
}

// ######################################################################################
// ######################################################################################
// ###########################  Modem Stuff  ############################################
// ######################################################################################
// ######################################################################################

// =========================================================================
	int modem_answer_line(int modem_id,int *port,int allowed_flags)
// =========================================================================
{
char	tmp[3000],incom[3000],faxconnect[300];
int		ende=0,timeout=60;
time_t	curtime,newtime,oldsecs;
unsigned short result;

	strcpy(faxconnect,faxcfg.GetString(1));
	if (strlen(faxconnect)==0)
		strcpy(faxconnect,"+FCON");

	clear_mailstate();
	mailer_stat(L("S_319"));	// modem INIT
	modem_update();
	load_modem_cfg(modem_id-1);
	if (modem_id != ISDNPORT)
	{
		Com = gModemCfg.m_comport-1;  // Supercom begins with 0
		*port=Com;
		ComInit(Com);
		set_comport_options(Com,&gModemCfg);
		system_sound("INCOMING_SERIAL");
	}
	else
	{
		Com=ISDNPORT;
		*port=Com;
		system_sound("INCOMING_CAPI");
		goto ISDNgoon;		// und ohne modem-answer-zeug weiter...
	}

	AlreadyConnected=0;

	mailer_stat(L("S_87"));	// answer incoming
	modem_update();
	modem_add_listbox(L("S_88"));

	strcpy(tmp,gModemCfg.m_answer);
	strcat(tmp,"\r");

	Sleep(1000);

	ComBufClear(Com,DIR_OUT);
	ComBufClear(Com,DIR_INC);

	sprintf(sbuf,"Modem : %s",gModemCfg.m_answer);
	modem_add_listbox(sbuf);

	RS_TXPInTime(Com,tmp,(unsigned short)strlen(tmp),SEC_1,&result);
	if (modem_id==1)
		gcomm.ConnectedOn1Modem=1;
	else
		gcomm.ConnectedOn1Modem=2;

	if (result < strlen(tmp))
	{
		modem_add_listbox(langstr("Unable to send answer string","Ошибка ответа модема"));
		return 0;
	}

	Sleep(1000);
	ComBufClear(Com,DIR_INC);

ISDNgoon:

	time(&curtime);
	oldsecs=0;
	strcpy(incom,"");
	memset(incom,0,sizeof(incom));

	// init outbound-session info

	time(&sessiondata.starttime);
	sessiondata.endtime=0;
	sessiondata.cost=(float)0.0;

	if (Com==ISDNPORT)
	{
		modem_add_listbox("ISDN CONNECT 64k");
		return 1;
	}

	do 
	{
		time(&newtime);

		if (abort_session())
			break;

		if (newtime != oldsecs)
		{
			gc.mailer.m_timeelapsed=newtime-curtime;
			oldsecs=newtime;
			modem_update();

			timeout--; // is set by definition to 60 seconds

			if (timeout<1)
				ende=1;
		}

		if (RS_RXInTime(Com,tmp,SEC0_1))
		{
			tmp[1]=0;

			if (*tmp != '\n' && *tmp != '\r')
			{
				*tmp=toupper(*tmp);

				if (strlen(incom) > 1024)
				{
					char temp[4000];
					strcpy(temp,incom);
					strcpy(incom,&temp[512]);
				}

				strcat(incom,tmp);

				if (gc.displaylevel>2)
					modem_add_listbox(incom);

				tmp[0]=0;
			}

			if (tmp[0]==13 && strlen(incom)>0)
			{
				Sleep(250);
				if (strstr(incom,faxconnect) != NULL)
				{
					if (allowed_flags &4) // Ist Fax Empfang erlaubt ?
					{
						if (faxcfg.defaultindex)
						{
							set_comport_options(Com,&gModemCfg,19200); // drop to 19200 baud
							modem_add_listbox("Dropping Baudrate to 19200 Baud");
						}
						faxreceive(Com);

						if (faxcfg.defaultindex)
						{
							set_comport_options(Com,&gModemCfg);
							modem_add_listbox("Restoring originate Baudrate");
						}
					}
					else
					{
						modem_add_listbox("Incoming FAX denied because of settings!");
						return 0;
					}
					return 2;
				}


				if(!RS_Carrier(Com)) // No Carrier
				{
					sprintf(tmp,"<< %s",incom);
					modem_add_listbox(tmp);

					if (strstr(incom,"NO CARRIER"))
						ende = 1;

					if (!strstr(incom,"CONNECT") &&	!strstr(incom,"CARRIER") &&	!strstr(incom,faxconnect))
						strcpy(incom,""); // clearing buffer
				}
			}
			else if (tmp[0]=='\r')
				strcpy(incom,"");
		}

		if(gc.exitincomingemsi)
		{
			modem_add_listbox(L("S_65"));
			init_modem(Com,&gModemCfg);	//.m_init1,gModemCfg.m_init2,gModemCfg.m_init3);
			ComReset(Com);
			return 0;
		}

		// check
		if (RS_Carrier(Com) &&
			(strstr(incom,"CONNECT") || strstr(incom,"CARRIER") || strstr(incom,faxconnect)) &&
			tmp[0]=='\r')
				ende=1;

	} while ( !ende );

	Sleep(1000);
	if (RS_Carrier(Com) != 0)
	{
		modem_add_listbox(L("S_55"));
		modem_add_listbox(incom);
	}
	else
	{
		if (!abort_session())
			modem_add_listbox(L("S_62"));

		strcpy(tmp,"\r");
		RS_TXPInTime(Com,tmp,(USHORT)strlen(tmp),SEC_1,&result);
		mailer_stat(L("S_383"));
		modem_update();

		Sleep(1500);
		ComBufClear(Com,DIR_OUT);
		ComBufClear(Com,DIR_INC);
		Sleep(500);

		ComReset(Com);
		ComInit(Com);
		set_comport_options(Com,&gModemCfg);
		Sleep(3000);
		mailer_stat(L("S_319"));
		modem_update();

		if (!init_modem(Com,&gModemCfg))	//.m_init1,gModemCfg.m_init2,gModemCfg.m_init3))
			modem_add_listbox(L("S_69"));

		mailer_stat(L("S_271"));
		modem_update();
		init_modem(Com,&gModemCfg);	//.m_init1,gModemCfg.m_init2,gModemCfg.m_init3);
		ComReset(Com);
		return 0;
	}
	mailer_stat(L("S_139"));
	modem_update();

	if (strstr(incom,faxconnect))
		return 2; // return 2 if a fax-connewct is reported !

	if (strstr(incom,"FAX") )
		return 3; // return 2 if a fax-connewct is reported !

	return 1;
}

// =========================================================================
	int make_init_sequence(char *initsequence, int flags)
// =========================================================================
{
	strcpy(initsequence,"**EMSI_REQA77E");
	strcat(initsequence,"\r");

	if ((flags&2)==2)
		strcat(initsequence,"press ESC twice for mailbox ....\r");
	else
		strcat(initsequence,"No human callers allowed, please hang up your modem ...\r");

	return 1;
}

// =========================================================================
	int get_description_from_files_bbs(char * fname, char * retlist)
// =========================================================================
{
FILE	*fp;
char	fbbsname[300];
char	tfile[300];
char	*p;
char	line[2048];
char	tmp[2048];
int		found=0;

	strcpy(fbbsname,fname);			// build name of fls.bbs file

	while (fbbsname[strlen(fbbsname)-1]!=92 && strlen(fbbsname)>0)
		fbbsname[strlen(fbbsname)-1]=0;

	strcat(fbbsname,"FILES.BBS");	//=============================

	fp = fopen(fbbsname,"rt");
	if (!fp)
	{
		sprintf(retlist,"%-13s*No description available!*\r",fname);
		return 0;
	}

	p = fname+strlen(fname);		// filename nach dem gesucht wird

	while (p>fname && *p != 92)
		p--;

	p++;

	strcpy(tfile,p);				// ================================
	strupr(tfile);

	while (1)
	{

		p = fgets(line,sizeof(line)-1,fp);

		if (!p)
			break;

		p = line;

		while (*p != 0 && *p != ' ')
		{
			*p = toupper(*p);
			p++;
		}

		if (strncmp(tfile,line,strlen(tfile))==0) // file gefunden
		{
			found=1;
			strcat(retlist,line);
			strcat(retlist,"\r");

			while (1)
			{
				p = fgets(line,sizeof(line)-1,fp);

				if (!p || line[0] != ' ') // wenn kein leerzeichen, dann sind wir schon bei einem neuen file
					break;

				p = line;

				while (*p != 0 && (*p == ' ' || *p == '+'))
					p++;

				memset(tmp,32,14); // 14 leerzeichen vorne dran
				tmp[13]=0;
				strcat(tmp,p);
				strcat(retlist,tmp);
				strcat(retlist,"\r");
			}
			break; // schleife verlassen
		}
	}

	fclose(fp);

	if (!found)
	{
		strcpy(retlist,tfile);
		while (strlen(retlist)<13)
			strcat(retlist," ");

		strcat(retlist,"*No description available!*\r");
	}
	return 1;
}

// =========================================================================
	int search_request_paths(char * sfile,char * mpwd,char * retlist)
// =========================================================================
{
int			maxfilecount=0; // wie viele fls maximal gefunden werden duerfen
_finddata_t se;
long		hf;
int			t;
char		fname[300],sbuf[200];
char		foundname[300];
char		searchpath[300];
char		pwd[300];
int			foundone=0;
char		filedesc[8192];

	if (magics.GetCount()<1)
		magics.LoadFromFile("magics.cfg");

	if (rpathes.GetCount()<1)
		rpathes.LoadFromFile("rpathes.cfg");

	if (sfile == NULL && retlist == NULL) // init der requestpfade
		return 1;

	strupr(mpwd);
	strcpy(fname,strupr(sfile));
	retlist[0]=0; // returnliste leer machen

	sprintf(sbuf,"Requested %s",sfile);
	modem_add_listbox(sbuf);

	for (t=0;t<magics.GetCount();t++) // search magics
	{
		strcpy(foundname,magics.GetFromTable(t,0,'\t'));
		strcpy(searchpath,magics.GetFromTable(t,1,'\t'));
		strcpy(pwd,magics.GetFromTable(t,2,'\t'));
		strupr(foundname);
		strupr(pwd);

		if (strcmp(foundname,fname)==0 && strcmp(mpwd,pwd) == 0) // magic passt
		{

		char		sbuf[200];
		char		path[300];
		char		fname[300];
		char		*p;
		_finddata_t mase;
		long		hanfile;

			strcpy(path,searchpath);
			p = path+strlen(path);

			while (*p != '\\' && p > path)
				p--;

			p++; // erster buchstabe des filenamen
			strcpy(fname,p);
			*p = 0;

			hanfile = _findfirst(searchpath,&mase);
			if (hanfile != -1L)             // packet-archiv found
			{
				do
				{
//was:					if (se.attrib != _A_SUBDIR)
					if (mase.attrib != _A_SUBDIR)
					{
						sprintf(sbuf,"=> MAGIC %s%s",path,mase.name);
						modem_add_listbox(sbuf);

						if (strlen(retlist)>0)
							strcat(retlist," ");

						strcat(retlist,path);
						strcat(retlist,mase.name);
						strcpy(sbuf,path);
						strcat(sbuf,mase.name);
						strcpy(filedesc,"");
						get_description_from_files_bbs(sbuf,filedesc);
						strcat(requestmail,filedesc);
						foundone=1;
					}

				} while (_findnext(hanfile,&mase) == 0);

				_findclose(hanfile);
			}
		}
	}

	if (foundone) // found a magic
		return 1;

	for (t=0;t<rpathes.GetCount();t++) // search filepaths if no magic was found
	{
		maxfilecount=0;
		strcpy(searchpath ,rpathes.GetFromTable(t,0,'\t'));
		strcpy(pwd        ,rpathes.GetFromTable(t,1,'\t'));
		strupr(pwd);
		strcat(searchpath,"\\");
		strcat(searchpath,sfile);

		hf = _findfirst(searchpath,&se);
		if (hf != -1L)             // packet-archiv found
		{
			do
			{
				if (!(se.attrib & _A_SUBDIR) &&	strcmpi(se.name,"FILES.BBS") && strcmpi(se.name,"FILES.BAK"))
				{
					char tmp[300];
					if (maxfilecount <10 && strcmp(mpwd,pwd)==0)
					{
						maxfilecount++;
						sprintf(sbuf,"=> FILE %s",se.name);
						modem_add_listbox(sbuf);
						foundone=1;
						make_path(tmp,rpathes.GetFromTable(t,0,'\t'),se.name);

						if (strlen(retlist)>0)
							strcat(retlist," ");

						strcat(retlist,tmp);
						get_description_from_files_bbs(tmp,filedesc);
						strcat(requestmail,filedesc);
					}
					else if (maxfilecount >=10)
					{
						sprintf(tmp,"\r_Maximum requestcounter for %s exceeded !_\r\r",sfile);
						strcat(requestmail,tmp);
					}
				}
			}
			while (_findnext(hf,&se) == 0);
		}
	}
	return foundone;
}

// =========================================================================
// returns zone,net,node,point for aka of first boss
	int get_first_aka(int *pzone, int *pnet, int *pnode, int *ppoint)
// =========================================================================
{
CStrList bosses;
CString  buf;

	load_bosslist(bosses);
	if (bosses.GetCount()==0)
		return 0;

	get_token(bosses.GetString(0),MAINAKA,buf);
	parse_address(buf,pzone,pnet,pnode,ppoint);
	return 1;
}

// =========================================================================
	int make_request_pkt(char *aka,char *sendlist)
// =========================================================================
{
FILE	*fp;
CString pktname;
time_t akttime;
int zone,net,node,point,dzone,dnet,dnode,dpoint,t;
char remoteaka[300],datetime[30],tmp[300],tmp2[300];
char termchar[2] = {0,0};

	make_path(tmp,gc.BasePath,"REQUEST.IN");
	mkdir(tmp);
	akttime=time(NULL);
	sprintf(tmp,"\\%08X.PKT",akttime);
	pktname=tmp;

	get_first_aka(&zone,&net,&node,&point);

	strcpy(remoteaka,"");
	t=0;

	while (strlen(remoteaka) == 0 && t < gAkaList.GetCount())
	{
		strcpy(remoteaka,gAkaList.GetString(t));
		t++;
	}

	parse_address(remoteaka,&dzone,&dnet,&dnode,&dpoint);

	pl = (MEDPKTHDR2 *)&po;			//

	memset(pl,0,sizeof(MEDPKTHDR2));

	pl->orig_zone 	= zone;
	pl->qm_orig_zone= zone;
	pl->orig_net 	= net;
	pl->orig_node 	= node;
	pl->orig_point 	= point;
	pl->dest_zone 	= dzone;
	pl->qm_dest_zone= dzone;
	pl->dest_net 	= dnet;
	pl->dest_node 	= dnode;
	pl->dest_point 	= dpoint;
	pl->capword  = 1;
	pl->capword2 = 256; // byte swapped
	pl->ver= 2;
	pl->product = 0x10; // fips product code
	pl->product2= 0x01;

	_strdate(tmp);
	sscanf(tmp,"%hu/%hu/%hu",&pl->month,&pl->day,&pl->year);
	pl->year += pl->year>40 ? 1900 : 2000;
	_strtime(tmp);
	sscanf(tmp,"%hu:%hu:%hu",&pl->hour,&pl->minute,&pl->second);

	get_versioninfo(tmp,6);
	pl->rev_lev =tmp[0];	// MAIN_BUILD
	get_versioninfo(tmp,7);
	pl->rev_lev2=tmp[0];	// SUB_BUILD

	// message-hdr basteln

	mh.filler  =2;					// filler 2 wichtig ! sonst geht's ned.
	mh.srcnode=node;
	mh.srcnet =net;
	mh.dstnet =dnet;
	mh.dstnode=dnode;
	mh.attrib  =0;
	mh.cost	   =0;

	fp = fopen(pktname,"wb");
	if (!fp)
		return 0;

	fwrite(pl,sizeof(MEDPKTHDR2),1,fp);
	fwrite(&mh,sizeof(_msghdr),1,fp);

	build_fido_time(datetime);

	while (strlen(datetime) < 19)
		strcat(datetime," ");

	fwrite(datetime,20,1,fp); // strlen = 19 + 1 null-byte
	fwrite(&host.sysop,strlen(host.sysop)+1,1,fp);
	fwrite("FIPS Request Processor",23,1,fp);
	fwrite("Request result notification",28,1,fp);

	sprintf(tmp,"\01INTL %d:%d/%d %d:%d/%d\r",dzone,dnet,dnode,zone,net,node);

	if (dpoint)
	{
		sprintf(tmp2,"\01TOPT %d\r",dpoint);
		strcat(tmp,tmp2);
	}

	if (point)
	{
		sprintf(tmp2,"\01FMPT %d\r",point);
		strcat(tmp,tmp2);
	}

	time(&akttime);
	sprintf(tmp2,"\01MSGID: %d:%d/%d.%d %08X\r",zone,net,node,point,(int)akttime);

	strcat(tmp,tmp2);
	strcat(tmp,"\01PID: ");
	strcat(tmp,get_versioninfo(tmp2,0));
	strcat(tmp,"\r");

	fwrite(tmp,strlen(tmp),1,fp); // ohne null !
	fwrite(requestmail,strlen(requestmail)+1,1,fp);
	fwrite(termchar,1,2,fp); // endbytes schreiben
	fclose(fp);

	if (strlen(sendlist) >0) // und jetzt das pkt in die sendliste mitaufnehmen
		strcat(sendlist," ");

	strcat(sendlist,"+");
	strcat(sendlist,pktname);

	return 1;
}

// =========================================================================
	int build_request_list(char *path, char *list)
// =========================================================================
{

	_finddata_t se;			// fillconsoleoutputattribute
	long hf;
	char tmp[300];
	char fname[300];
	FILE *fp;
	int filesfound=0;

	strcpy(tmp,path);
	strcat(tmp,"\\*.REQ");

	memset(fname,0,sizeof(fname));

	memset(requestmail,0,sizeof(requestmail));

	strcpy(requestmail,"Hello *");

	strcat(requestmail,host.sysop);
	strcat(requestmail,"* !");

	strcat(requestmail,"\r\r");
	strcat(requestmail,"This is the result of your request:");
	strcat(requestmail,"\r\r");

	if ((hf = _findfirst(tmp,&se)) != -1L)
	{
		strcpy(fname,path);
		strcat(fname,"\\");
		strcat(fname,se.name);

		_findclose(hf);
	}


	if (strlen(fname)<1)
		return 0;

	// proceed request-list

	fp=fopen(fname,"rt");
	if (!fp)
		return 0;

	modem_add_listbox(langstr("Processing requests ...","Обрабатываются запросы..."));

	search_request_paths(NULL,NULL,NULL); // init

	while (fgets(tmp,200,fp) != NULL)
	{
		char temp[8192];
		rtrim(tmp);

		if (strlen(tmp)>0)
		{
			// in tmp steht das zu suchende file, passwort steht evtl. auch drinnen.
			char fname[300],pwd[300];
			char *p;

			memset(temp,0,sizeof(temp));
			memset(pwd,sizeof(pwd),0);
			memset(fname,sizeof(pwd),0);

			sscanf(tmp,"%s %s",fname,pwd);

			p = pwd;
			if (*p != '!')
				*p=0;		// wrong password
			else
				p++;		// skip '!'

			if (search_request_paths(fname,p,temp)) // in temp are list of founded fls
				filesfound=1;
			else
			{
				strcat(requestmail,fname);

				for (UINT t=strlen(tmp);t<14;t++)
					strcat(requestmail," ");

				strcat(requestmail,"*File not found !*\r");
			}

			if (strlen(temp) > 0)
			{
				if (strlen(list) >0)	// leerzeichen vorne dran, falls liste nicht leer
					strcat(list," ");

				strcat(list,temp);		// gefundenes file (oder mehrere !) an die liste
			}
		}
	}

	fclose(fp);
	_unlink(fname);

	strcat(requestmail,"\r--- FIPS Request Processor");

	return 1;
}

// =========================================================================
// searches for a pwd : gAkaList lookup in bosses-config
	int search_for_pwd(char * pwd)
// =========================================================================
{
CStrList bosses;
int		rz,rne,rno,rp;
int		z,no,ne,p;

	load_bosslist(bosses);
	strcpy(pwd,""); // no pwd by default

	for (int t=0;t<gAkaList.GetCount();t++)
	{
		parse_address(gAkaList.GetString(t),&z,&ne,&no,&p);
		if (z && ne && no)
		{
			// now search bosses-config
			for (int i=0;i<bosses.GetCount();i++)
			{
				parse_address(bosses.GetFromTable(i,1,'\t'),&rz,&rne,&rno,&rp);

				if (rz==z && rne == ne && rno==no && p==rp)
				{
					strcpy(pwd,bosses.GetFromTable(i,5,'\t'));
					return 1;
				}
			}
		}
	}
	return 0; // no pwd found;
}

// =========================================================================
	int init_emsi_session(int port, int flags)
// =========================================================================
{
char initsequence[1024],inbuf[8192],in[300];
USHORT result;
time_t oldsecs,newsecs;
int t,responsecode=0,whattodo=0,timeoutcount=0,lastesc=0;

	sprintf(initsequence,"\n\r%s\n\r(c) 1995-2003 by Deutch & Russian teams\n\r\n",
		get_versioninfo(in,0));
	RS_TXPInTime(port,initsequence,strlen(initsequence),SEC_1,&result);
	make_init_sequence(initsequence,flags);
	RS_TXPInTime(port,initsequence,strlen(initsequence),SEC_1,&result);

	time(&oldsecs);
	newsecs=oldsecs;

	Sleep(500);
	ComBufClear(Com,DIR_INC);
	strcpy(inbuf,"");

	while (1)
	{
		time(&newsecs);

		if (abort_session())
			return 0;

		if ((newsecs-oldsecs) > 1)
		{

			(gc.mailer.m_timeelapsed)++;
			modem_update();

			RS_TXPInTime(port,initsequence,strlen(initsequence),SEC_1,&result);
			oldsecs=newsecs;
			timeoutcount++;

			if (timeoutcount>15)
			{
				if (flags & 2)
				{
					modem_add_listbox(langstr("No EMSI response, assuming HUMAN CALLER !",
						"Не получена EMSI-посылка, предполагается ручной вызов"));
					return 2;
				}
				else if (timeoutcount >20)
				{
					modem_add_listbox(langstr("No response from REMOTE ! Mailbox disabled.",
						"Удаленный модем не ответил! Прием почты отключен."));
					return 0;
				}
			}
		}

		if (RS_RXInTime(port,in,SEC0_1))
		{
			in[1]=0;			// zeichen empfangen
			strcat(inbuf,in);
			responsecode = emsi_detect(inbuf);

			if (responsecode != EMSI_DAT && responsecode != 0) // alles ausser emsi_dat wegwerfen
			{
				strcpy(inbuf,"");
				responsecode=0;
			}

			if (responsecode == EMSI_DAT)
			{
				whattodo=1;
				modem_add_listbox(langstr("Incoming EMSI-Session","Входящая EMSI-сессия"));
				break;
			}

			if (in[0] == 27)
				lastesc++;
			else
				lastesc=0;
		}

		if (lastesc>1 && (flags & 2))	// came second ESC
		{
			modem_add_listbox(langstr("Incoming HUMAN CALLER","Входящий ручной вызов"));
			return 2;	// mailbox-session disable
		}

		if (abort_session())
			return 0;
	}

	timeoutcount=0;
	if (whattodo) // we have emsi-session or mailbox-caller
	{
		time(&oldsecs);
		while (1)
		{
			if (timeoutcount>2 || abort_session())
				return 0;

			time(&newsecs);
			RS_RXPInTime(Com,(char *)inbuf,4,SEC_2,&result); // read length word in hex

			if (result = 4)
			{
				sscanf((char *)inbuf,"%x",&t);
				strcpy((char *)inbuf,"");

				RS_RXPInTime(Com,(char *)inbuf,t,SEC_5,&result); // read emsi-dat-package

				if (result==t)
				{
					char buf[8192];
					char pwd[300];
					char tmpbuf[1024];

					RS_RXPInTime(Com,(char *)tmpbuf,4,SEC_5,&result); // read emsi-crc16
					send_emsi_command("EMSI_ACK");
					analyse_emsi_dat((char *)inbuf);
					sprintf(sbuf,"Connected to : %s",host.bbs);
					modem_add_listbox(sbuf);

					gAkaList.RemoveAll();
					for (t=0;t<akacount;t++)
					{
						gAkaList.AddTail(akaarr[t]);
						sprintf(sbuf,"  AKA : %s",akaarr[t]);
						modem_add_listbox(sbuf);
					}

					strcpy(tmpbuf,host.mailer);
					_strupr(tmpbuf);

					sprintf(sbuf,"| %s",host.sysop);
					modem_add_listbox(sbuf);
					sprintf(sbuf,"| %s",host.site);
					modem_add_listbox(sbuf);
					sprintf(sbuf,"| %s",host.phone);
					modem_add_listbox(sbuf);
					sprintf(sbuf,"| %s",host.mailer);
					modem_add_listbox(sbuf);
					
					strcpy(gc.mailer.m_system,host.bbs);
					sprintf(gc.mailer.m_sysop,"%s ( %d:%d/%d.%d )",host.sysop,host.zone,host.net,host.node,host.point);
					sprintf(gc.mailer.m_location,"%s ( %s )",host.site,host.phone);
					modem_update();

					sprintf(sessiondata.system,"%d:%d/%d.%d %s (%s)",
						host.zone,host.net,host.node,host.point,host.sysop,host.phone);
					sprintf(sessiondata.user,"%s",host.site);

					// now check for pwd for other system

					search_for_pwd(pwd);
					make_emsi_dat(buf,pwd,"",port==ISDNPORT,0,0);
					if (gc.displaylevel>0)
						modem_add_listbox(langstr("Sending EMSI_DAT","Посылка EMSI_DAT"));

		   			strcpy(in,""); // clear inboundbuffer, send EMSI_DAT
		   			mailer_stat(langstr("Sending EMSI_DAT","Посылка EMSI_DAT"));
					modem_update();
					send_emsi_command(buf);
					return 1; // successfull emsi-session
				}
				else
				{
					modem_add_listbox(langstr(
						"Incomplete EMSI_DAT received","Получен незавершенный EMSI_DAT"));

					if (gc.displaylevel >1)
					{
						char buf[300];

						sprintf(buf,langstr(
							"Read %d, expected %d Bytes","Считано %d, ожидалось %d байтов"),
							result,t);
						modem_add_listbox(buf);
					}

					send_emsi_command("EMSI_NAK");
					send_emsi_command("EMSI_REQ");
					timeoutcount++;
					time(&newsecs);
					oldsecs=newsecs;
				}
			}
			else
				modem_add_listbox(langstr("Incomplete EMSI_DAT received",
					"Получен незавершенный EMSI_DAT"));

			if (abs(newsecs-oldsecs)>20)
			{
				modem_add_listbox(langstr("EMSI TIMEOUT","Таймаут EMSI"));
				timeoutcount++;
				time(&newsecs);
				oldsecs=newsecs;
			}
		}
	}
	return 1; // emsi-session einleiten
}


// =========================================================================
	int move_files_from_to(LPCSTR srcpath,LPCSTR destpath)
// =========================================================================
// move all fls from srcpath to destpath
{
_finddata_t se;
int retval =0;
long hf;
char infile[400];
char outfile[400];
char tmp[400];

	if ((hf = _findfirst(tmp,&se)) != -1L)
	{
		do
		{
			if (!(se.attrib & _A_SUBDIR))
			{
				sprintf(infile,"%s\\%s",srcpath,se.name);
				sprintf(outfile,"%s\\%s",destpath,se.name);
				rename(infile,outfile);
			}

		} while (_findnext(hf,&se) == 0);

		_findclose(hf);
	}
	return retval;
}

// =========================================================================
	void look_for_mail(void)
// =========================================================================
{
CString	aka;
char	tmp[MAX_PATH],tmp1[MAX_PATH];
int		zone,net,node,point;

	for (int i=0;i<gAkaList.GetCount();i++)
	{
		aka=gAkaList.GetString(i);
		if (parse_address(aka,&zone,&net,&node,&point)<3)
			continue;

		make_fidodir(tmp,zone,net,node,point);
		make_path(tmp1,gc.OutboundPath,tmp);

		if (!access(tmp1,0))	// dir exists
			check_for_packets(aka,gc.OutboundPath,0);
	}
}

// =========================================================================
	int kill_all_req_files(char *path)
// =========================================================================
{
_finddata_t se;
long	hf;
char	fname[MAX_PATH];
char	sbuf[300];

	make_path(fname,path,"*.req");
	if ((hf = _findfirst(fname,&se)) != -1L)
	{
		do
		{
			make_path(fname,path,se.name);
			if (unlink(fname))
				sprintf(sbuf,langstr("Cannot delete old REQ-File '%s' !",
					"Ошибка удаления старого REQ-файла '%s' !"),fname);
			else
				sprintf(sbuf,langstr("Deleted old REQUEST-File %s",
					"Старый REQ-файла '%s' удален"),fname);

			modem_add_listbox(sbuf);
			
		} while (_findnext(hf,&se) == 0);

		_findclose(hf);
	}
	return 1;
}

// =========================================================================
	int incoming_emsi_session(int modem_id,int flags)
// =========================================================================
{
int t,zresult,result,retval,returnvalue=1,inboundmode=0;
CString basepath,inpath,path;
int  port=-1;

	gc.mailer.running = 1;
	gpMain->m_animail.Play(0,-1,-1);
	time(&timeelapsed);
	zmodem_compatibility_proc(000000);
	intermail_zmodem=0;
	inboundmode=modem_answer_line(modem_id,&port,flags);

	if (inboundmode==1)
	{
		result=init_emsi_session(port,flags);
		AlreadyConnected =1;
		if (result ==1)
		{
			char dummy[300];
			set_burst_mode(1);
			dummy[0]=0;

			basepath=gc.InboundPath;
			inpath=basepath;
			basepath+="\\TEMP";	// basepath for ZModem
			mkdir(basepath);

			// look for all old req-fls from previous requests and delete them Fix for 1.0

			kill_all_req_files(PS basepath); // in \TEMP
			kill_all_req_files(PS inpath);   // normal inbound

			// Receive Zmodem ########################################################

		   	mailer_stat(langstr("== Receiving ZModem","== Прием ZModem"));
			modem_update();
			modem_add_listbox(langstr("== Receiving ZModem","== Прием ZModem"));

			if (intermail_zmodem == 1)
				zmodem_compatibility_proc(000001);
			
			RS_RXFile_ZMODEM(Com,dummy,&zresult,(TINFOPROC)Info,PS basepath);
			set_burst_mode(0);
			zm_errcode(zresult,dummy);
			
			sprintf(sbuf,langstr("== Xfer status: [%s]","== Результат: [%s]"),dummy);
			modem_add_listbox(sbuf);

			// Process received fls ################################################

			memset(sendlist,0,sizeof(sendlist)); // clear sendlist

			if (build_request_list(PS inpath,sendlist)) // now build the request-pkt
				make_request_pkt(PS gAkaList.GetString(0),sendlist); // for first aka

			look_for_mail();

			// Send Zmodem
			set_burst_mode(1);

			ComBufClear(Com,DIR_OUT);
			ComBufClear(Com,DIR_INC);

		   	mailer_stat(L("S_405"));	// sending ZModem
			modem_update();
			modem_add_listbox(L("S_67"));	// == sending ZModem
			
			RS_TXFile_ZMODEM(Com,sendlist,&zresult,(TINFOPROC)Info,0);
			zm_errcode(zresult,dummy);

			modem_add_listbox(L("S_70",dummy));	// == Xfer status: %s
			set_burst_mode(0);

			// Rename all files in emsi_in and clear buffers

			path=gc.InboundPath;
			retval=move_files_from_to(basepath,path); // move all fls

			// retval sagt die aktion die nun zu folgen hat
			// 0 nichts
			// 1 tossen anwerfen falls gewuenscht

			rmdir(basepath);
			modem_add_listbox(L("S_60"));	// == End of Inbound Session
	   	mailer_stat(L("S_274"));	// Inbound Session complete

			clear_mailstate();
			modem_update();

			if (get_cfg(CFG_COMMON,"TosserStart",1))
			{
				check_inbound(); // automatically check inbound after an inbound-call
				gTosser.OnStart();
			}
		}
		else if (result ==2)
		{
			// mailbox-session
			gc.mailer.m_timeelapsed=0;
			modem_update();
			run_bbs(Com);
		}

		if (Com != ISDNPORT)
		{
			ComReset(Com);
			Sleep(500);
			ComInit(Com);
			Sleep(500);
			ComReset(Com);
		}

		returnvalue=1;
		goto INBOUNDEMSIDONE;
	}
	else if (inboundmode==3) // fax receive ueber CONNECT FAX !! not supported !!
	{
		modem_add_listbox("-----------------------------------------");
		modem_add_listbox("!! Incompatible FAX settings    !!");
		modem_add_listbox("!! refer to the online-help for !!");
		modem_add_listbox("!! a correct FaxClass-2 setup   !!");
		modem_add_listbox("-----------------------------------------");
		Sleep(4000);
		ComReset(port);
		returnvalue=1;
		goto INBOUNDEMSIDONE;
	}
	
	if (inboundmode==2) // fax receive ueber fcon, fax muesste schon da sein
	{
		if (Com != ISDNPORT)
			ComReset(port);

		returnvalue=1;
		goto INBOUNDEMSIDONE;
	}
	else // inboundmode==0
	{
		modem_add_listbox(L("S_329"));	// No caller detected

		if (Com != ISDNPORT)
			ComReset(port);
		else
			capi_hangup();

		returnvalue=1;
		goto INBOUNDEMSIDONE;
	}

INBOUNDEMSIDONE:

	gcomm.ConnectedOn1Modem=gcomm.ConnectedOn2Modem=0;
	time(&sessiondata.endtime);
	update_history(1,&sessiondata);

	if (Com != ISDNPORT)
	{
		load_modem_cfg(modem_id-1);
		Com = gModemCfg.m_comport-1;				  // Supercom begins with 0
	}

	if (Com != ISDNPORT)
	{
		ComInit(Com);
		set_comport_options(Com,&gModemCfg);
		t=0;
		while (RS_Carrier(Com)==1 && t < 30)
		{
			Sleep(100);
			t++;
		}

		Sleep(1500);
		AlreadyConnected=0;

		if (!init_modem(Com,&gModemCfg))	//.m_init1,gModemCfg.m_init2,gModemCfg.m_init3))
			modem_add_listbox(langstr("Unable to initialize modem !","Невозможно инициализировать модем !"));

		ComReset(Com);
	}
	else
	{
		if (gcomm.ConnectedOnIsdn)
			modem_add_listbox(L("S_174"));	// Disconnecting from ISDN
		else
			modem_add_listbox(L("S_173"));	// Disconnected from ISDN
	}

	gpMain->m_animail.Stop();
	gpMain->m_animail.Seek(0);
	gc.mailer.running=0;
	returnvalue=0;

	return returnvalue;
}

//=====================================================
	void write_charging_info(void)
//=====================================================
{
_costinfo cost;
char   buf[300];

	if (!charging_active)	return;
	modem_add_listbox(L("S_131",charged_summ,get_currency()));	// Charged money: %2.2f %s
	gc.mailer.m_charge[0]=0; // reset
	modem_update();
	memset(&cost,0,sizeof(cost));
	cost.time=time(NULL);
	cost.cost=charged_summ;
	sessiondata.cost = cost.cost; // save for outboundhistory
	cost.type=1;
	strcpy(buf,gLastCalled);
	buf[19]=0;
	strcpy(cost.fido,buf);
  append_to_cost_file(&cost);
}

// =========================================================================
	void check_send_list(Char *sendlist)
// =========================================================================
{
CStrList tmp;
int		t,i;
char	tst[350];

	tmp.RemoveAll();
	i = count_tokens(sendlist);
	if (i==0)	return;

	for (t=0;t<i;t++) // no tic-files
	{
		extractstr(t+1,sendlist," ",tst,300);
		strupr(tst);

		if (strstr(tst,".TIC") ==0)
		{
			char tmp2[300];

			strcpy(tmp2,tst);
			if (strlen(tmp2)>0)
			{
				if (tmp2[0] == '+')
					strcpy(tmp2,&tst[1]);
			}

			if (existfile(tmp2))
				tmp.AddTail(tst);
			else
				modem_add_listbox(L("S_245",tmp2));	// File %s not found
		}
	}

	for (t=0;t<i;t++) // there are tic-files
	{
		extractstr(t+1,sendlist," ",tst,300);
		strupr(tst);
		if (strstr(tst,".TIC") !=0)
		{
			char tmp2[300];

			strcpy(tmp2,tst);
			if (strlen(tmp2)>0 && tmp2[0] == '+')
					strcpy(tmp2,&tst[1]);

			if (existfile(tmp2))
				tmp.AddTail(tst);
			else
				modem_add_listbox(L("S_245",tmp2));	// File %s not found
		}
	}

	strcpy(sendlist,"");
	for (t=0;t<tmp.GetCount();t++)
	{
		if (strlen(sendlist)>0)
			strcat(sendlist," ");

		strcat(sendlist,tmp.GetString(t));
	}
}

// =========================================================================
	int outgoing_emsi_session(LPCSTR argphone,LPCSTR pwd,LPSTR dir,LPCSTR mainaka,int nopoll,int nohydra,int nozmod)
// =========================================================================
{
_finddata_t se;
USHORT	result;
CString	stbuf,help;
char	buf[8192],in[8192],inbuf[8192],newdir[300],tdir[300],tmp[300];
char	phone[300],req_fname[300];
time_t	oldsecs,newsecs,inqsecs;
int		emsifailcount=0,responsecode=0,emsistate=1,t1=0,t2=0,reqcount=0,notwait=30;
int		inqcount,t,zresult,n,fhandle;//tries,

	//Очистка файлов статистики
	DeleteFile("mescomw.log");
	DeleteFile("mescomr.log");
	DeleteFile("session.log");
	
	strcpy(phone,argphone);
	handle_tel_nr_rotation(phone);
	gFirstCRCSupressed=0;
	set_fips_priority(1);
	charging_active=0;		// no charging-info by default
	gcomm.DialingViaSecondModem=0;

	*req_fname=0;	// stub filename because it will be deleted after all

	if (gincome.OllisTurn)	// block outgoing if we have incomming caller
	{
		ClearSession(Com);
		return MODEM_CANNOT_INIT;
	}

	if (gincome.WaitingForMin1Device)
	{
		gincome.MailerNeedsModem=1;
		gincome.InModemEmsiSession=1;
		PulseEvent(gincome.EventToKillWaitCommEvent);
		Sleep(500);
	}

	if (!test_Notbremse())
	{
		help=dialbrk.GetString(2);
		sscanf(help,"%d",&notwait);
		modem_add_listbox(L("S_153",notwait));
		notwait=notwait*100;

		gc.NotBremseActive=1;
		for (n=0;n<notwait;n++)
		{
	 		if (gc.ExitEventThread || gc.mailer.keyboard_break || gc.mailer.skip_only)
			{
				ClearSession(Com);
				return MODEM_FAIL;
			}
			Sleep(10);
		}
	}

	intermail_zmodem=0; // default
	make_path(tmp,gc.InboundPath,"TEMP");
	_mkdir(tmp);
	clear_temporary_inbound();

	gc.NotBremseActive=0;
	append_info_dialing();

	abortflag=0;
	AlreadyConnected=0;

	memset(sendlist,0,sizeof(sendlist));

	load_modem_cfg(1);			// Load first Modem cfg
// reset detmail struct, but dont kill this ptr and running!
	clear_mailstate();
	modem_update();

// get files to send
	add_to_sendlist(dir,"*.*");	// add arcmail
	make_path(newdir,dir,"NETMAIL");
	add_to_sendlist(newdir,"*.*");	// add netmail

	Com = gModemCfg.m_comport-1;
// ISDN dial
	if (*phone=='$')// ISDN
	{
		if (isdnlist.defaultindex)
		{
			load_modem_cfg(2);
			Com = gModemCfg.m_comport-1;
			modem_add_listbox(L("S_616"));	// using second modem
			gcomm.DialingViaSecondModem=1;
			stbuf=phone;
			strcpy(phone,stbuf.Mid(1));
			goto dial_via_modem;
		}

		Com=ISDNPORT;
		modem_add_listbox("Try to register ISDN-CAPI");

		init_capi_callback();
		if (init_capi_stuff())
			modem_add_listbox("CAPI registered");
		else
		{
			modem_add_listbox("Register CAPI failed");
			ClearSession(Com);
			return 0;
		}
	}
	else
	{
// MODEM dial
dial_via_modem:

		ComInit(Com);
		set_comport_options(Com,&gModemCfg);
		if (ComEnableFIFO(Com,THRESHOLD08) == FALSE)
			sprintf(sbuf,"Cannot enable FIFO!");

		strcpy(in,"");

		t1 = 60; // global timeout

		Sleep(500);

		mailer_stat(L("S_319"));	// modem init
		modem_update();

		if (!init_modem(Com,&gModemCfg))	//.m_init1,gModemCfg.m_init2,gModemCfg.m_init3))
		{
			modem_add_listbox(L("S_444"));	// error init modem
			ClearSession(Com);
			return MODEM_CANNOT_INIT;
		}
	}

	if (abort_session())
	{
		ClearSession(Com);
		return MODEM_ABORTED;
	}

	if (Com==ISDNPORT)
	{
		mailer_stat(L("S_138"));	// isdn connection...
		t1 = 60; // timeout
	}
	else
		mailer_stat(L("S_164"));	// calling...

	modem_update();

	time(&timeelapsed);
	if (Com==ISDNPORT)
// isdn
	{
		sprintf(sbuf,">> %s",&phone[1]);
		modem_add_listbox(sbuf);
		char *p=sbuf;
		for (char *q=phone;*q;q++)	// remove non-digit chars
			if (isdigit(*q))
				*p++=*q;
		*p=0;

		if (gc.displaylevel>0)
		{
			char mybuf[300];
			sprintf(mybuf,"Dialing number %s",sbuf);
			modem_add_listbox(mybuf);
		}

		system_sound("ISDN_DIALING");
		broesel_msns();
		t=capi_dial_system(sbuf,gcomm.Nr1,&gc.mailer.keyboard_break);
		if (t==2 || t==0)
		{
			modem_add_listbox("ISDN reports BUSY or NO ANSWER !");
			system_sound("ISDN_FAILED");
			t=0;
		}
	}
	else
// modem
		t = dial_system(phone);
	
	if (t)	// we are connected
	{
		charged_summ=0;
		charging_intervall=0;
		charging_active=costuserlist.defaultindex;
		if (charging_active)
		{
			init_charging_for_one_call(phone);
			modem_add_listbox(L("S_132",get_zone()));	// base cost %s
			charging_intervall-=costuserlist.reserved3;
		}

		time(&sessiondata.starttime);
		sessiondata.starttime-=costuserlist.reserved3;
		sessiondata.endtime=0;
		sessiondata.cost=0;

		if (Com==ISDNPORT)
		{
			system_sound("ISDN_CONNECT");
			modem_add_listbox(L("S_417"));
			mailer_stat(L("S_284"));
			modem_update();
			Sleep(25);
		}
		
		AlreadyConnected=1;	// flag for detect carrier-lost
		emsi=NULL;
		if (gc.displaylevel>0)	emsi=fopen("lastemsi.out","wt");
		emsistate = 0;
		mailer_stat(L("S_463"));
		modem_update();
		Sleep(1000);
		strcpy(in,"");
		inqsecs=0;
		oldsecs=40;
		t=0;
		do 
		{
			if (abort_session())  // check for user abort
			{
				if (emsi)	{ fclose(emsi); emsi=0; }	// close log file
				ClearSession(Com);
				return MODEM_ABORTED;
			}
			time(&newsecs);
			if (inqsecs!=newsecs)
			{
				update_charging();
				gc.mailer.m_timeelapsed=newsecs-timeelapsed;
				modem_update();

				t++;
				if ((t % 5)==0 || t>20)	
					send_emsi_command("EMSI_INQ");
				else
				{
					strcpy(inbuf," \r");
					RS_TXInTime(Com,inbuf[0],SEC0_1); // initiate emsi-session
					RS_TXInTime(Com,inbuf[1],SEC0_1);
					blog_write(inbuf[0]);
					blog_write(inbuf[1]);
					strcpy(inbuf,"");
				}
				inqsecs=newsecs;
				oldsecs--;
				if (oldsecs < 1)
				{
					modem_add_listbox(L("S_351"));
					modem_add_listbox(L("S_175"));

					if (Com==ISDNPORT)
					{
						modem_add_listbox(L("S_174"));
						mailer_stat(L("S_174"));
						modem_update();
						Sleep(200);
						if (capi_hangup())
							modem_add_listbox(L("S_171"));
						else
							modem_add_listbox(L("S_168"));
						Sleep(800);
					}
					else
						ComReset(Com);
					ClearSession(Com);
					return MODEM_OK;
				}
			}

			if (RS_RXInTime(Com,inbuf,SEC0_1)==TRUE)
			{
				blog_read(inbuf[0]);
				inbuf[1]=0;
				if (inbuf[0]=='\n')
					strcat(in,"[0x0A]");
				else if (inbuf[0]=='\r')
					strcat(in,"[0x0D]");
				else
					strcat(in,inbuf);

				if (emsi)	fprintf(emsi,"Received from Remote : (INIT) %s\n",in);

				if (strlen(in) > 3800) // truncate buffer if recieved more than needed
				{
					char tmp [3800];
					if (emsi)	fprintf(emsi,"Too much Garbage, truncating Buffer...\n");
					strcpy(tmp,&in[3000]);
					strcpy(in,tmp);
				}
			}
	   } while (strstr(in,"EMSI_REQ") == NULL);

		mailer_stat(L("S_207"));
		modem_update();
		inqcount=30;
		oldsecs=0;
		do 
		{
			time(&newsecs);
			if (newsecs!=oldsecs)
	   		{
				 RS_TXInTime(Com,13,SEC0_5);
				 blog_write(13);
				 oldsecs=newsecs;
				 update_charging();
			}

			if (abort_session())  // check for user abort
			{
				if (emsi)	{ fclose(emsi); emsi=0; }
				ClearSession(Com);
				return MODEM_ABORTED;
			}

			time(&newsecs);

			if (RS_RXInTime(Com,inbuf,SEC0_1)==TRUE)
			{
				blog_read(inbuf[0]);
				inbuf[1]=0;
				if (*inbuf && *inbuf!='\r' && *inbuf!='\n')
					strcat(in,inbuf);

				if (emsi)
				{
					char tstr[100];
					_strtime(tstr);
					fprintf(emsi,"%s Received from Remote : (%d) %s\n",tstr,emsistate,in);
				}

				if (strlen(in)==(sizeof(in)-1))
				{
					strcpy(in,"");
					modem_add_listbox(L("S_426"));
					modem_add_listbox(L("S_135"));
				}
				ASSERT (strlen(in) < sizeof(in));
			}

			if (strstr(in,"**EMSI_REQ"))
			{
				responsecode=emsi_detect(in);
				strcpy(in,"");
			}

			if (strlen(in) >0)
				responsecode=emsi_detect(in);

			if (responsecode)
				strcpy(in,"");

		if (emsistate==0 && responsecode==EMSI_REQ)
		{
			if (gc.displaylevel>0)	modem_add_listbox(L("S_186"));
			emsistate = 1;
			mailer_stat(L("S_186"));
			modem_update();
		}

		switch(emsistate)
		{
			case 1:
				t1=60; // init emsi-loop
				emsistate=2;
				break;
			case 2:
				send_emsi_command("EMSI_INQ"); // new
				make_emsi_dat(buf,pwd,mainaka,Com==ISDNPORT ? 1 : 0,nohydra,nozmod);
				if (gc.displaylevel>0)	modem_add_listbox(L("S_404"));
				memset(in,0,sizeof(in));	// clear inbound
				mailer_stat(L("S_404"));
				modem_update();
				Sleep(750);					// waiting for remote
				memset(in,0,sizeof(in));	// clear inbound
				send_emsi_command(buf);		// send emsi_dat
				emsistate=3;
				break;
			case 3:
				t2=20;
				emsistate=4;
				reqcount=0;
				break;
			case 4:
				if (t2<0)
					goto emsireq;
				switch (responsecode)
				{
				case EMSI_ACK:
					if (gc.displaylevel>0)	modem_add_listbox(L("S_181"));
					mailer_stat(L("S_181"));
					modem_update();
					emsistate=5;
					responsecode=0;
					break;
				case EMSI_DAT: // if emsi_ack was not send
					modem_add_listbox(L("S_183"));
					emsistate=5;
					goto goto_emsi_dat_direct;
					break;
				case EMSI_REQ:
emsireq:
					if (reqcount>5)
					{
						if (gc.displaylevel>0 && t2>0)	modem_add_listbox(L("S_187"));
						if (t2>0)
						{
							mailer_stat(L("S_186"));
							modem_update();
						}
						else
							modem_add_listbox(L("S_188"));	// timeout

						emsistate=2;
						responsecode=0;
					}
					else
						reqcount++;
					break;
				case EMSI_NAK:
					mailer_stat(L("S_184"));
					modem_update();
					if (gc.displaylevel>0)	modem_add_listbox(L("S_185"));
					modem_add_listbox(L("S_377"));	// refused
					emsifailcount++;
					emsistate=2;
					responsecode=0;
				}
				break;

goto_emsi_dat_direct: // no emsi_ack recieved
			case 5:
				if (responsecode==EMSI_DAT)  // remote emsi-dat recieved
				{
					mailer_stat(L("S_182"));
					modem_update();
					if (gc.displaylevel>0)	modem_add_listbox(L("S_182"));
					RS_RXPInTime(Com,inbuf,4,SEC_1,&result); // read length word in hex
					blog_readstr((PUCHAR)inbuf,4);
					if (result = 4)
					{
						char crc[10];
						USHORT result2;

						strcat(inbuf,""); // get length of emsi_dat package
						ASSERT (strlen(inbuf)<sizeof(inbuf));
						sscanf(inbuf,"%x",&t);
						strcpy(inbuf,"");
						memset(inbuf,0,sizeof(inbuf));  // empty inbuf, so RXPInTime
						RS_RXPInTime(Com,inbuf,(USHORT)t,SEC_20,&result); // read emsi_dat
						blog_readstr((PUCHAR)inbuf,t);
						if (result==t) // successful reading ?
						{
							char tmpbuf[300];

							RS_RXPInTime(Com,crc,4,SEC_2,&result2); // read 4 bytes crc16
							blog_readstr((PUCHAR)&crc,4);
							if (emsi)	fprintf(emsi,">> Received EMSI_DAT : %s\n",inbuf);

							if (gc.displaylevel>0)
								modem_add_listbox(L("S_182"));

							strcat(inbuf,"");
							analyse_emsi_dat(inbuf);
							modem_add_listbox(L("S_56",host.bbs));
							modem_add_listbox(L("S_43",host.zone,host.net,host.node,host.point));

							for (t=0;t<akacount;t++)
							{
								gAkaList.AddTail(akaarr[t]);
								sprintf(sbuf,"|      AKA : %s",akaarr[t]);
								modem_add_listbox(sbuf);
							}

							sprintf(sbuf,"| %s",host.sysop);
							modem_add_listbox(sbuf);
							sprintf(sbuf,"| %s",host.site);
							modem_add_listbox(sbuf);
							sprintf(sbuf,"| %s",host.phone);
							modem_add_listbox(sbuf);
							sprintf(sbuf,"| %s",host.mailer);
							modem_add_listbox(sbuf);

							sprintf(sessiondata.system,"%d:%d/%d.%d %s (%s)",host.zone,host.net,host.node,host.point,host.sysop,host.phone);
							sprintf(sessiondata.user,"%s",host.site);

							strcpy(tmpbuf,host.mailer);
							strupr(tmpbuf); // in uppercase
							strcpy(gc.mailer.m_system,host.bbs);
							sprintf(gc.mailer.m_sysop,"%s ( %d:%d/%d.%d )",host.sysop,host.zone,host.net,host.node,host.point);
							sprintf(gc.mailer.m_location,"%s ( %s )",host.site,host.phone);
							modem_update();
							Sleep(250);
							send_emsi_command("EMSI_ACK");
							Sleep(100);
							send_emsi_command("EMSI_ACK");
							emsistate=6;
							goto emsicomplete;
						} 
						else
						{
					   		if (emsi)	fprintf(emsi,"EMSI_DAT incomplete: %s\n",inbuf);
							modem_add_listbox(L("S_278",result,t));
					   		send_emsi_command("EMSI_NAK");
						}
					} 
					else
					{
						modem_add_listbox(L("S_142"));
				  		send_emsi_command("EMSI_NAK");
					}
				}
				break;
			}

emsicomplete:
			time(&newsecs);          // check timeouts
			if (newsecs != oldsecs)
			{
				t1--; 
				t2--;
				oldsecs=newsecs;
			}

			if (emsistate==6) 
				t1 = 99;

			if (abort_session())
			{
				if (emsi)	{ fclose(emsi); emsi=0; }
				write_charging_info();
				time(&sessiondata.endtime);
				update_history(0,&sessiondata);
				ClearSession(Com);
				return MODEM_ABORTED;
			}

		} while (emsistate>=0 && emsistate<=5 && t1>=0 && emsifailcount<4);

		if (emsi)	{ fclose(emsi);	emsi=0; }
		if (emsifailcount>3 && emsistate<5) // emsi error
		{
			mailer_stat(L("S_189"));
			modem_update();
			write_charging_info();
			time(&sessiondata.endtime);
			update_history(0,&sessiondata);
			ClearSession(Com);
			return MODEM_EMSI_FAILURE;
		}

		mailer_stat(L("S_433"));
		modem_update();
// ===================================================================
		check_outbound(dir);	// make sendlist
// ===================================================================
// check FILES.REQ in outbound and rename it to net+node.REQ

		make_path(newdir,dir,FREQ_LIST);
//	make_path(tdir,dir);
		fhandle=_findfirst(newdir,&se);
		if (fhandle != -1L)
		{
			sprintf(buf,"%0.4X%0.4X.REQ",host.net,host.node);
			make_path(tdir,dir,buf);
			if (rename(newdir,tdir)==0) // returns 0 if successulf
			{
				strcpy(req_fname,tdir);
				strcat(sendlist,tdir);
			}
			_findclose(fhandle);
		}

// if no files in outbound and not nopoll, create dummy-pkt
		if (!nopoll && strlen(sendlist)==0)
			make_emptypoll(dir,mainaka,pwd,&oldsecs);

		strupr(sendlist);
		check_send_list(sendlist);
		Sleep(500);
		ComBufClear(Com,DIR_OUT);
		ComBufClear(Com,DIR_INC);

// start sending
		if (gc.displaylevel>0)
		{
			sprintf(sbuf,"EMSI_DAT: {%s}",gc.mailer.protocol);
			modem_add_listbox(sbuf);
		}
//Dk:protocol string
p_hydra hyd;
p_zmod  zmod;

// Hydra
		if (strstr(gc.mailer.protocol,"HYD"))
		{
			zresult=hyd.hydra_batch(sendlist,inbuf);
			hyd.errcode(zresult,buf);
		}
// Zmodem
		else if (strstr(gc.mailer.protocol,"ZAP"))
		{
			zresult=zmod.zmod_batch(sendlist,inbuf);
			zmod.errcode(zresult,buf);
		}
		else // if (strstr(gc.mailer.protocol,"PUA")) 
		{
			zresult=PRC_ERROR;
			modem_add_listbox(L("S_594"));
			zmod.errcode(zresult,buf);
		}
		
		if (abortflag)
		{
			write_charging_info();
			time(&sessiondata.endtime);
			update_history(0,&sessiondata);
			ClearSession(Com);
			return MODEM_ABORTED;
		}

		if (*req_fname)
		{
			if (!strcmp(buf,"OK")) // status==OK
				unlink(req_fname); // delete REQ-file
			else
			{
				char tmp[300],*p;
				strcpy(tmp,req_fname);
				p=strrchr(tmp,'\\');
				if (!p)	p=strrchr(tmp,':');
				*(p+1)=0;
				strcat(tmp,FREQ_LIST);
				rename(req_fname,tmp);
				modem_add_listbox(L("S_394"));	// save freq list
			}
		}
	} // end if (t) // of dial_system
	else // there was no connect
	{
		mailer_stat(L("S_330"));
		modem_update();
		if (emsi)	{ fclose(emsi);	emsi=0; }
		ClearSession(Com);
		return MODEM_TIMEOUT;
	}

	if (Com==ISDNPORT)
	{
		modem_add_listbox(L("S_174"));
		mailer_stat(L("S_174"));
		modem_update();
		Sleep(200);
		if (capi_hangup())
			modem_add_listbox(L("S_171"));
		else
			modem_add_listbox(L("S_168"));

		Sleep(800);
	}
	
	gpMain->m_animail.Stop();
	gpMain->m_animail.Seek(0);
	modem_add_listbox(L("S_406"));
	mailer_stat(L("S_271"));
	clear_mailstate();
	modem_update();

	write_charging_info();
	time(&sessiondata.endtime);
	update_history(0,&sessiondata);
	ClearSession(Com);
// post-session commands
/*	if (!init_modem(Com,"AT%S0","","",TRUE))//gModemCfg.m_init1,gModemCfg.m_init2,gModemCfg.m_init3))
	{
		modem_add_listbox(L("S_444"));
		ClearSession(Com);
		return MODEM_ERROR;
	}
	init_modem(Com,"ATI3","","",TRUE);
	ClearSession(Com);*/
	// session is OK if no errors and no operators abort
	return 	(zresult==PRC_NOERROR || zresult==PRC_LOCALABORTED ? MODEM_OK : MODEM_FAIL);
}

// ===================================================================
	void make_emptypoll(LPCSTR dir,LPCSTR mainaka,LPCSTR pwd,time_t *secs)
// ===================================================================
{
FILE *fp;
char buf[20],buf1[300],dm[]="\0\0";
int	 zone,net,node,point;

	parse_address(mainaka,&zone,&net,&node,&point);
	pn = (NEWPKTHDR *)&po;
	memset(pn,0,sizeof(NEWPKTHDR));

	pn->product	= 0;
	pn->rev_lev	= 0;
	pn->subver	= 2;
	pn->version	= 2;
	strcpy(pn->password,pwd);
	pn->ozone 	= zone;
	pn->onet 	= net;
	pn->onode 	= node;
	pn->opoint 	= point;
  	pn->dzone 	= host.zone;
	pn->dnet 	= host.net;
	pn->dnode 	= host.node;
	pn->dpoint 	= host.point;

	time(secs);
	sprintf(buf,"%08X.PKT",*secs);
	_mkdir(dir);
	make_path(buf1,dir,buf);
	fp=fopen(buf1,"wb");
	if (!fp)
		err_out("E_CANTCREATEDUMMY",buf1);
	else
	{
		fwrite(pn,sizeof(NEWPKTHDR),1,fp);
		fwrite(dm,2,1,fp); // two zero as package end
		fclose(fp);
	}
	strcpy(sendlist,"+");
	strcat(sendlist,buf1);
}

// ===================================================================
	void check_outbound(LPCSTR dir)
// ===================================================================
{
_finddata_t se;
CString tmp,tmp2;
long	hf;
int		zone,net,node,point;

	_mkdir(gc.OutboundPath);
	for (int t=0;t<akacount;t++)
	{
		if (parse_address(akaarr[t],&zone,&net,&node,&point)==4)
			check_for_packets(akaarr[t],dir);
		else									// check all points of system
		{
			tmp.Format("%s\\%d_%d_%d*",gc.OutboundPath,zone,net,node);
			hf=_findfirst(tmp,&se);
			if (hf!=-1L)
			{
				do 
				{
					if ((se.attrib & _A_SUBDIR) && se.name[0] != '.')	// is dir?
					{
						char tmp3[100];

						point=0;
						split_fidodir(se.name,&zone,&net,&node,&point);
						sprintf(tmp3,"%d:%d/%d.%d",zone,net,node,point);
						check_for_packets(tmp3,dir);
					}
				} while (_findnext(hf,&se)==0);

				_findclose(hf);
			}
		}
	}
}

// ===================================================================
	int init_modem(BYTE Com,_modemcfg *mc)	// char *init1,char *init2,char *init3,BOOL logit)
// ===================================================================
{
char	tmp[300],buf[300],in[5];
time_t	oldsecs;
int		response=0;
char	*p;
BOOL	use;

    if (ComValid(Com)==0) return 0;
	RS_Carrier(Com);
	for (int t=0;t<3;t++)
	{
		switch (t)
		{
			case 0:
				strcpy(tmp,mc->m_init1);
				use=mc->m_useinit1;
				break;
			case 1:
				strcpy(tmp,mc->m_init2);
				use=mc->m_useinit2;
				break;
			case 2:
				strcpy(tmp,mc->m_init3);
				use=mc->m_useinit3;
				break;
		}

		if (*tmp && use)
		{
			sprintf(sbuf,">> %s",tmp);
			modem_add_listbox(sbuf);
			strcat(tmp,"\r");
			ComBufClear(Com,DIR_OUT);
			ComBufClear(Com,DIR_INC);

			p=tmp;
			while (*p)
			{
				if (*p != '~')
					RS_TXInTime(Com,*p,SEC0_05);
				else
					Sleep(250);
				p++;
			}
			memset(buf,0,sizeof(buf));
			response=0;
			time(&oldsecs);
			while (1)
			{
				if (abort_session())	{ ComReset(Com); return 0; } // user break
				if (time(NULL)-oldsecs>5)	break;	// make a timeout of 5 seconds

				in[0]=0;
				in[1]=0;

				if (RS_RXInTime(Com,in,SEC0_5))
				{
					if (*in != '\n' && *in != '\r')
					{
						strcat(buf,in);
						if (gc.displaylevel>1)	// debug modem answer
						{
							sprintf(sbuf,"[%s]",buf);
							modem_add_listbox(sbuf);
						}
						if (strlen(buf) > 200)	// truncate buffer
							strcpy(buf,buf+100);
					}
					else
					{
						if (*buf)
						{
							//if (logit)	TRACE0(buf);
							//	writelog(LOG_MAILER,buf);
							if (!strcmpi(buf,"OK"))
							{
								modem_add_listbox("<< OK");
								response=1;
								break;
							}

							if (!strcmpi(buf,"ERROR"))
							{
								modem_add_listbox("<< ERROR");
								return 0;
							}
						}
						*buf=0;
					}
				}
			}

			if (response==0)
			{
				modem_add_listbox(L("S_318"));
				return 0;
			}
		}
	}
	return 1;
}

// ************************************************************************************
// ************************* Scanner STUFF  *******************************************
// ************************************************************************************

// =============================================================== 
// compare address with mask (using wildcards * and ?)
	int checkdestrouting(LPCSTR dest,LPCSTR routing)
// ===============================================================
{
int zone,point,net,node;
char mdest[300],mrout[300];

	parse_address(dest,&zone,&net,&node,&point);
	sprintf(mdest,"%d:%d/%d.%d",zone,net,node,point);	// expand address
	strcpy(mrout,routing);

	if (!strchr(mrout,'.')) // if point is absent, add wildcard for all points
		 strcat(mrout,".*");

	for (int t=0;t<(int)strlen(mdest);t++)
	{
		if (mrout[t]=='*')
			return 1;

		if (mdest[t] != mrout[t] && mrout[t] != '?')
			return 0;
	}
	return 1;
}

// =============================================================== // checks routing in rotuing list
	int checkrouting(LPCSTR dest,LPCSTR route)
// ===============================================================
{
char tmp[1000];
int	t;

	t = count_tokens(route);
	if (t==0)  // no routes
		return 0;

	for (int i=0;i<t;i++)
	{
		get_token(route,i,tmp,' ');
		if (checkdestrouting(dest,tmp)) // routing found
			return 1;
	}
	return 0; // no routing
}

// ===============================================================
// splits the routing line and checks the routing
	int checkdestrouting_with_complete_line(LPCSTR dest,LPCSTR routing)
// ===============================================================
{
char routeto[1024],noroute[1024];
char *p;

	strcpy(routeto,routing);
	*noroute=0;

	if (p=strchr(routeto,',')) // split line
	{
		*p=0;
		strcpy(noroute,p+1);
	}

	if (checkrouting(dest,noroute)==1) // system isn't in routing list
		return 0;

	if (checkrouting(dest,routeto)==1) // system is in routing list
		return 1;

	return 0; // system not found
}

// ===============================================================
// returns uplink for given address
	int get_uplink_address(LPCSTR addr,LPSTR uplink)
// ===============================================================
//
{
CString buf;
char	bossinfo[MAX_BOSSLEN];
int		i=0;

	if (!strchr(addr,':'))
		return 0;

	 while (db_get_boss_by_index(i++,bossinfo))
	 {
		get_token(bossinfo,ROUTING,buf);
		if (checkdestrouting_with_complete_line(addr,buf))
		{
			get_token(bossinfo,MAINAKA,buf);
			strcpy(uplink,buf);
			return 1;
		}
	 }
	 return 0;
}

// ===============================================================
// scans for new outbound mails
	int scan_database(void)
// ===============================================================
{
bossarray ba[MAX_UPLINKS];
_fidonumber ftn;
mailheader hdr;
areadef pa;
int		curarea=0,oldstatus=0,oldpacketname=0,dum=0;
int		maxboss,k,i,maxuplink,otherboss;
long	ahandle;
char	routeto[2048],noroute[2048],tmp[2048],outpath[MAX_PATH];
FILE	*fp[MAX_UPLINKS];	// echomail packages
FILE	*fpn[MAX_UPLINKS];	// netmail packages
FILE	*fpc;
CFido	fidodir;
CStrList bosses;
CString str;
BOOL	wasnetmail=FALSE,akaroute=FALSE;

	cc_asaved_netmail_handle=-1;
	cc_asaved_echomail_handle=-1;
	make_path(outpath,gc.OutboundPath);
	mkdir(gc.OutboundPath);
	modem_add_listbox(L("S_66"));	// scanning outbound

	load_bosslist(bosses);
	maxboss=bosses.GetCount();
	akaroute=get_cfg(CFG_COMMON,"AKARoute",0);
	memset(mailcount,0,sizeof(mailcount));
	maxstatnum=-1;

	for (k=0;k<MAX_UPLINKS;k++)
		fp[k]=fpn[k]=NULL;

	maxuplink=collect_bossinfo(ba,maxboss,outpath);
// scan areas
	while (db_get_area_by_index(curarea,&pa)==DB_OK)
	{
		maxstatnum=curarea;
		k=db_get_area_handle(pa.echotag, &ahandle,0);

		if (pa.localmail != 1)	// localmail ?
			if (db_get_area_state(ahandle))
			{
				//int ret;
				int b=false;
				/*ret=*/db_set_area_state(ahandle,0);
				//ASSERT(ret==DB_OK);
				k = db_getfirsthdr(ahandle,&hdr);
				while (k==DB_OK)
				{
					otherboss=0;
					if ((hdr.status & DB_MAIL_CREATED) && !(hdr.status & DB_NOTSCAN))
					{
						if (!b)
						{
							modem_add_listbox(L("S_2",pa.echotag));	// New mail in %s
							b=true;
						}
						oldstatus = hdr.status;
						hdr.status |= DB_MAIL_SCANNED | DB_MAIL_READ;

						CString fr=hdr.fromname;
						CString to=hdr.toname;
						fr.OemToAnsi();
						to.OemToAnsi();
						modem_add_listbox(L("S_305",fr,to));	// Mail %s -> %s

						if (hdr.attrib & MSGKILL)
						{
							modem_add_listbox(L("S_158"));	// Deleting after sent
							hdr.status |= DB_DELETED;
						}

						if (db_sethdr_by_index(ahandle,hdr.index,&hdr)!=DB_OK)
							 ASSERT(0);

// find matching uplink
// check clear state of direct-flag for curarea==0 (netmails)
// if not then mail must be directed to recipient and we set additionally MSGCRASH in hdr.status

					dum=-1;

					if (curarea==0 && (hdr.attrib & MSGCRASH))
					{
						char buf2[300];
						int t;
						int foundaboss=-1;
 // check if we have quasi-bosses for this node
						for (t=0;t<maxuplink;t++)
						{
							if (ba[t].zone==hdr.dstzone && ba[t].net==hdr.dstnet  && 
								ba[t].node==hdr.dstnode && ba[t].point==hdr.dstpoint)
							{
								foundaboss=t;
								break;
							}
						}

						if (foundaboss==-1)
						{
							dum=maxuplink;	// crash-node set as quasi-uplink
							maxuplink++;
						}
						else
							dum = foundaboss; // boss found

						mailcount[0]++; // netmail-counter
						modem_add_listbox(L("S_166",hdr.dstzone,hdr.dstnet,hdr.dstnode,hdr.dstpoint));	// DirectMail to %d:%d/%d.%d
// check point in poinlist
						if (!nl_get_fido_by_number(
							hdr.dstzone,hdr.dstnet,hdr.dstnode,hdr.dstpoint,&ftn))
						{ // point is not in pointlist
							if (hdr.status & DB_MAIL_ROUTED)
							{
								hdr.dstpoint=0;
								modem_add_listbox(L("S_423"));	// this mail will be sent to his BOSS

								if (hdr.zone_net || hdr.node) // route to boss
								{
									ba[dum].nnode = hdr.node;
									ba[dum].nnet  = hdr.zone_net & 0x0000ffff;
									ba[dum].nzone = (hdr.zone_net >> 16) & 0x0000ffff;
									ba[dum].npoint=0;
									otherboss=1;
									modem_add_listbox(L("S_86",ba[dum].nzone,ba[dum].nnet,ba[dum].nnode));	// Alternate address is %d:%d/%d
								}
							}
							else
							{
								if (hdr.dstpoint)
								{
									modem_add_listbox(L("S_447",hdr.dstzone,hdr.dstnet,hdr.dstnode,hdr.dstpoint));	// Unlisted Point %d:%d/%d.%d
									modem_add_listbox(L("S_423"));	// mail will be sent to points BOSS
									hdr.dstpoint=0;
								}
							}
						}
						else
						{ // point is in pointlist, so check its bosses
							if (hdr.dstpoint && (hdr.status & DB_MAIL_ROUTED))
							{
								hdr.dstpoint=0;
								modem_add_listbox(L("S_423"));	// mail will be sent to points BOSS
								if (hdr.zone_net || hdr.node) // route to bosses
								{
									ba[dum].nnode = hdr.node;
									ba[dum].nnet  = hdr.zone_net & 0x0000ffff;
									ba[dum].nzone = (hdr.zone_net >> 16) & 0x0000ffff;
									ba[dum].npoint=0;
									otherboss=1;
									modem_add_listbox(L("S_86",ba[dum].nzone,ba[dum].nnet,ba[dum].nnode));	// Alternate address is %d:%d/%d
								}
							}
						}

						memset(ba[dum].pwd,0,20);

						ba[dum].zone  = hdr.dstzone;
						ba[dum].net   = hdr.dstnet;
						ba[dum].node  = hdr.dstnode;
						ba[dum].point = hdr.dstpoint;

						ba[dum].pzone  = hdr.srczone;
						ba[dum].pnet   = hdr.srcnet;
						ba[dum].pnode  = hdr.srcnode;
						ba[dum].ppoint = hdr.srcpoint;

						sprintf(ba[dum].fido,"%d:%d/%d.%d",
							hdr.dstzone,hdr.dstnet,hdr.dstnode,hdr.dstpoint);
						sprintf(ba[dum].mypoint,"%d:%d/%d.%d",
							hdr.srczone,hdr.srcnet,hdr.srcnode,hdr.srcpoint);

						strcpy(ba[dum].dir,outpath);

						if (!otherboss)
							fidodir.Set(ba[dum].zone,ba[dum].net,ba[dum].node,ba[dum].point);
						else // route mail to other bosses (after 2 ints in mailheader)
							fidodir.Set(ba[dum].nzone,ba[dum].nnet,ba[dum].nnode,ba[dum].npoint);

						fidodir.GetAsDir(buf2);
						strcat(ba[dum].dir,buf2);
						strcpy(ba[dum].packer,"");		  // no packer, crashmail is not .PKT
						strcpy(ba[dum].archivename,"");
						strcpy(ba[dum].routing,ba[dum].fido); // routing to "new" uplink
					}
					else
					{
						for (i=0;i<maxuplink;i++)
						{
							int zone,net,node,point;
							parse_address(pa.uplink,&zone,&net,&node,&point);
							if (ba[i].zone==zone && ba[i].net==net && ba[i].node==node && ba[i].point==point)
							{
								dum=i;
								break;
							}
						}
					}
					if (curarea==0 && !(hdr.attrib & MSGCRASH))
					{
						char srcadr[100],dstadr[100];

						mailcount[0]++; // netmail-counter +1
						hdr.dstpoint=0;
						sprintf(dstadr,"%d:%d/%d.%d",hdr.dstzone,hdr.dstnet,hdr.dstnode,0);
						sprintf(srcadr,"%d:%d/%d",hdr.srczone,hdr.srcnet,hdr.srcnode);
						for (i=0;i<maxuplink;i++)
						{
							*routeto=*noroute=0;
							get_token(ba[i].routing,0,routeto,',');
							get_token(ba[i].routing,1,noroute,',');

							if (gc.displaylevel>1) // routing debug
							{
								modem_add_listbox(L("S_107",ba[i].fido));	// Boss-Routing for %s
								modem_add_listbox(L("S_390",routeto));	// RouteTo '%s'
								modem_add_listbox(L("S_229",noroute));	// Except  '%s'
							}
							
							sscanf(routeto,"%s",tmp);
							if (akaroute && !strcmp(tmp,"*"))
							{
								for (int j=0;j<maxuplink;j++)
								{
									if (!strcmp(ba[j].fido,srcadr))
									{
										modem_add_listbox(L("S_391",dstadr,ba[j].fido));	// Routing to %s through %s
										dum=j;
										goto bossfound;
									}
								}
							}
							else
							{
								if (checkrouting(dstadr,noroute)) // exception ?
									continue;

								if (checkrouting(dstadr,routeto)) // then route it
								{
									modem_add_listbox(L("S_391",dstadr,ba[i].fido));	// Routing to %s through %s
									dum=i;
									break;
								}
							}
						}
bossfound:;
					}

					if (dum==-1)// no defined uplink
					{
						modem_add_listbox(L("S_335",pa.echotag));	// No uplink defined for area %s
						hdr.status = oldstatus; // undoing changes in status
						db_sethdr_by_index(ahandle,hdr.index,&hdr);
						break;
					}
					else
					{
						// check file existance for this uplink, if none, create it
						if ((curarea && !fp[dum]) || (!curarea && !fpn[dum]))
							build_uplink_file(ba[dum],pl,&fp[dum],&fpn[dum],otherboss,curarea,&wasnetmail);
						// check file-attach
						if (curarea==0 && (hdr.attrib & MSGFILE))
							build_subj_attaches(ba[dum].dir,hdr.subject);

						mh.filler  =2;					// filler 2 important !
						mh.srcnode=ba[dum].pnode;
						mh.srcnet =ba[dum].pnet;
						mh.attrib=0;
						mh.cost=0;

						if (curarea)  // echomail
						{
						// if is not a netmail, then addressing to bossnode
							mh.dstnet =ba[dum].net;
							mh.dstnode=ba[dum].node;
						// echomail has no default attributes
							mh.attrib = hdr.attrib & MSGPRIVATE; // private echomail
							fpc=fp[dum];
						}
						else    // netmail
						{
						// direct addressing to recipient
							mh.dstnet =hdr.dstnet;
							mh.dstnode=hdr.dstnode;
						// set attributes for mail
							mh.attrib |= MSGPRIVATE;			 // netmails are always private
							mh.attrib |= MSGKILL;				 // kill/sent flag (for bosses-system)
							mh.attrib |= hdr.attrib & MSGFILE;// file-attach-flag
							mh.attrib |= hdr.attrib & MSGRRQ; // return-receipt flag
							mh.attrib |= hdr.attrib & MSGCPT; // is a return receipt
							fpc=fpn[dum];
						}

						if (ahandle != 0)	  // netmail?
							mailcount[ahandle]++;
// write pkt
						fwrite(&mh,sizeof(_msghdr),1,fpc);	// write mailheader

						while (strlen(hdr.datetime) < 19)
							strcat(hdr.datetime," ");

						fwrite(&hdr.datetime,20,1,fpc); // strlen = 19 + 1 null-byte
						fwrite(&hdr.toname,strlen(hdr.toname)+1,1,fpc); // with null-bytes
						fwrite(&hdr.fromname,strlen(hdr.fromname)+1,1,fpc);
						fwrite(&hdr.subject,strlen(hdr.subject)+1,1,fpc);
						{
							char tmpbuf[MAX_MSG+1000],tmp[100],*mbuf;
							long hnd;
							mailheader tmphdr;
							memcpy(&tmphdr,&hdr,sizeof(hdr));

							mbuf=tmpbuf;
							*tmp=0;
							if (curarea)  // add "AREA:" hdr to echomail
							{
								if (Carbons.reserved4)	// add CC-hdr
								{
									sprintf(tmp,"%s\r",L("S_130",pa.echotag));	// CarbonCopy from area %s
									CharToOem(tmp,mbuf);
									mbuf+=strlen(tmp);
									*mbuf=0;
								}
								sprintf(tmp,"AREA:%s\r",pa.echotag);
								strcpy(mbuf,tmp);
							}
							
							db_get_mailtext(ahandle,&hdr,mbuf+strlen(tmp),MAX_MSG);
							tmphdr.mail_text = tmpbuf;
							tmphdr.text_length=strlen(tmpbuf);
							// copy mail in asaved areas
							if (!curarea && Carbons.reserved3)	// netmail
							{
								if (db_get_area_handle(ASAVED_NETMAIL,&hnd,1,"")==DB_OK &&
									db_append_new_message(hnd,&tmphdr)==DB_OK)
								{
									modem_add_listbox(L("S_128",ASAVED_NETMAIL));	// CC -> %s
									cc_asaved_netmail_handle=hnd;
								}
							}
							else if (curarea && Carbons.reserved4)	// echomail
							{
								if (db_get_area_handle(ASAVED_ECHO,&hnd,1,"")==DB_OK &&
									db_append_new_message(hnd,&tmphdr)==DB_OK)
								{
									modem_add_listbox(L("S_128",ASAVED_ECHO));	// CC -> %s
									cc_asaved_echomail_handle=hnd;
								}
							}

							char *p=tmpbuf+strlen(tmpbuf)-1;
							while (p>tmpbuf && (*p=='\r' || *p=='\n'))
								*p--=0;	// delete extra blank lines

							p++;
							*p++='\r';	// last line ended with Return
							*p=0;

							if (curarea)	// echomail only
							{
								// add SEEN-BY of bosses
								sprintf(tmp,"SEEN-BY: %d/%d\r",ba[dum].net,ba[dum].node);
								strcat(tmpbuf,tmp);
								// add PATH kludge for nodes
								if (hdr.srcpoint==0)
								{
									sprintf(tmp,"\001PATH: %hu/%hu\r",hdr.srcnet,hdr.srcnode);
									strcat(tmpbuf,tmp);
								}
							}

							fwrite(mbuf,strlen(mbuf)+1,1,fpc);
						}
					}
				}
				k = db_getnexthdr(ahandle,&hdr);
			}
		}
		curarea++;
	}

	close_packets(fp,fpn,maxuplink,wasnetmail);

// write area stat
	write_statistic("E"); // E for export
	modem_add_listbox(L("S_63"));	// == Outbound mail scan complete

	if (cc_asaved_netmail_handle!=-1)
		db_refresh_area_info(cc_asaved_netmail_handle);

	if (cc_asaved_echomail_handle!=-1)
		db_refresh_area_info(cc_asaved_echomail_handle);

	if (get_cfg(CFG_COMMON,"PackOutbound",1))
	{
		char buf[300],arcmail[300],packname[300];
		int	 idx;
		for (k=0;k<bosses.GetCount();k++) // check all bosses-system and pack mail
		{
			get_arcmail_name(ba[k].dir,arcmail); // get arcmail-name
			make_path(buf,ba[k].dir,"*.pkt");
			idx=get_bossindex(ba[k].fido);
			if (get_boss_packer(idx,packname))
				pack_file(arcmail,buf,ba[k].packer);
		}
	}
	return 1;
}

// ************************************************************************************
// ************************* NODELIST STUFF  ******************************************
// ************************************************************************************

static _fidonumber user;
static _fidonumber fido;
static _fidouser   *usertemp;
static _fidonumber *fidotemp;

FILE *cachefp;

struct _fu_tree  {
				  _fidonumber user;
				  _fu_tree  *left;
				  _fu_tree  *right;
				 } *fu;

struct _fn_tree  {
				  _fidonumber number;
				  _fn_tree    *left;
				  _fn_tree    *right;
				 } *fn;

// ===========================================
	int add_user_to_tree(struct _fu_tree ** root, struct _fidonumber * user)
// ===========================================
{
	if (*root == NULL)
	{
		*root = (_fu_tree*)malloc(sizeof(_fu_tree));

		if (*root==NULL) // malloc ging schief
			return 0;

		(*root)->left=NULL;
		(*root)->right=NULL;

		memcpy(&(*root)->user,user,sizeof(_fidonumber));
		return 1;
	}

	if (strcmpi((*root)->user.user,user->user) < 0)
		return add_user_to_tree(&(*root)->right,user);
	else
		return add_user_to_tree(&(*root)->left,user);
}

// ===========================================
	int add_number_to_tree(_fidonumber *number)
// ===========================================
{
static int cachecount=0;
_fidonumber cache[1000];

	if (number!=NULL)
	{
		memcpy(&cache[cachecount],number,sizeof(_fidonumber));
		cachecount++;
	}

	if (cachecount>990 || number == NULL)
	{
		int retval;
		retval=((int)fwrite(cache,sizeof(_fidonumber),cachecount,cachefp)==(int)cachecount);
		cachecount=0;
		return retval;
	}
	return 1;
}

// ===========================================
	int write_user_tree(FILE * fp,_fu_tree * root)
// ===========================================
{
	if(root==NULL)
		return 1;

	if(root->left != NULL)				// alles kleinere schreiben
		write_user_tree(fp,root->left);

	fwrite(&root->user,sizeof(_fidouser),1,fp); // eigenen eintrag schreiben

	if(root->right != NULL)             // alles groessere schreiben
		write_user_tree(fp,root->right);

    free(root); // aktuellen knoten freigeben;
	return 1;
}

//======================================== 
// get newest nodelist by _findfirst etc...
	void get_newest_nodelist(char *path,char *nl)
//========================================
{
_finddata_t se;
char	temp[MAX_PATH],retpath[MAX_PATH];
long	hf;
long	oldtime=0;
int		i;
char	*p;

	make_path(temp,path,nl);
	strcat(temp,".*");

	*retpath=0;

	if ((hf = _findfirst(temp,&se)) != -1L)
	{
		do
		{
			if (!(se.attrib & _A_SUBDIR))
			{
				p=strstr(se.name,".");
				i=1;
				if (p)
				{
					while (*(++p))
					{
						if (!isdigit(*p))
						{
							i=0;
							break;
						}
					}
				 }
				 else
					 i=0;

				if (se.time_write > oldtime && i)
				{
					make_path(retpath,path,se.name);
					oldtime = se.time_write;
				}
			}
		}	while (!_findnext(hf,&se));
	}

	_findclose(hf);
	strcpy(nl,retpath);
}

//===================================================
	int comparefidonumber(const void *aa1,const void *aa2)
//===================================================
{
_fidonumber *a1;
_fidonumber *a2;

	a1=(_fidonumber *)aa1;
	a2=(_fidonumber *)aa2;

	if (a1->zone < a2->zone) return -1;
	if (a1->zone > a2->zone) return 1;

	if (a1->net < a2->net) return -1;
	if (a1->net > a2->net) return 1;

	if (a1->node < a2->node) return -1;
	if (a1->node > a2->node) return 1;

	if (a1->point < a2->point) return -1;
	if (a1->point > a2->point) return 1;

	return 0;
}

//===================================================
	int comparefidouser(const void *aa1,const void * aa2)
//===================================================
{
_fidouser *a1;
_fidouser *a2;

	a1=(_fidouser *)aa1;
	a2=(_fidouser *)aa2;

	return strcmpi(a1->user,a2->user);
}

//===================================================
	int check_for_nodediffs(void)					
//===================================================
// This functions checks for valid Nodediffs in the inbound dir
// returns 1 if a nodediff is found, 0 if none
{
_finddata_t se;
CStrList NL;
char	fname[MAX_PATH];
char	nodediff[100];
long	hf;

	NL.LoadFromFile("nodelist.cfg");
	for (int t=0;t<NL.GetCount();t++)
	{
		strcpy(nodediff,NL.GetFromTable(t,2,'\t')); // get nodediffname from setup
		if (*nodediff)
		{
			make_path(fname,gc.InboundPath,nodediff);
			strcat(fname,".*");
			hf = _findfirst(fname,&se);
			_findclose(hf);
			if (hf != -1L)           	// nodediff found, return 1, else - next file
				return 1;
		}
	}
	return 0;
}

//===================================================
	int get_diff_name(char *name, int number)
//===================================================
// This functions returns 1 if name contains a valid diff-name, otherwise zero
// the oldest nodediffname will be returned !!!!
{
char	fname[MAX_PATH];
char	nodediff[100];
int		found=0;
long	hf;
ULONG	newtime=0xffffffff; // newest time possible
struct _finddata_t se;
CStrList NL;

	NL.LoadFromFile("nodelist.cfg");
	*name=0;
	strcpy(nodediff,NL.GetFromTable(number,2,'\t')); // get nodediffname from setup

	if (strlen(nodediff)>0)
	{
		make_path(fname,gc.InboundPath,nodediff);
		strcat(fname,".*");			// nodediff-fname

		hf = _findfirst(fname,&se);
		if (hf != -1L)
			do	
			{
				if ((ULONG)se.time_write < newtime) // is there an older diff ?
				{
					 found=1;
					 strcpy(name,se.name);
					 newtime = se.time_write;
				}
			}
			while (!_findnext(hf,&se));

		return found;
	}
	return 0;
}

//===================================================
// extracts a packed nodediff in the nodelist-dir
	int extract_nodediff(char *name)
//===================================================
{
char	tmp[MAX_PATH],buf[MAX_PATH];
int		ptype,ret,retval=0,mode;

	ptype = packertype(name);
	if (ptype!=TOSS_UNKNOWN_PACKER && ptype!=TOSS_FILE_NOT_FOUND)	// unpack file
	{
		expand_external_cmd(Packers[ptype].upck_cmd,gc.NodelistPath,name,tmp);
		sprintf(buf,langstr("Execute: %s","Выполнение: %s"),tmp);
		nl_add_listbox(buf);

		mode=get_cfg(CFG_COMMON,"HideCmdWin",0) ? SW_HIDE : SW_SHOWMINNOACTIVE;
	 	ret=system_command(tmp,langstr("Decrunching Nodediff","Распаковка листа изменений"),
			gc.NodelistPath,mode,300000,&retval);

 		if (!ret)
			return 0;

		if (retval)// unpacking failed
		{
			err_out("E_DEPFAULED",tmp,retval);
			return 0;
		}

		while(unlink(name))
		{
			sprintf(buf,langstr("Cannot delete %s","Невозможно удалить %s"),name);
			nl_add_listbox(buf);
			sprintf(buf,langstr("File may be locked by other task...",
				"Файл возможно заблокирован другой программой..."));
			nl_add_listbox(buf);
			Sleep(2000);
		}
		return 1;
	}

	if (ptype == TOSS_UNKNOWN_PACKER)	// copiing file
	{
		get_filename(name,tmp);
		make_path(buf,gc.NodelistPath,tmp);
		if (CopyFile(name,buf,FALSE))
		{
			_unlink(name);
			return 1;
		}
	}
	return 0;
}

//===================================================
//	This functions extracts a packed file into destpath
	int extract_file(LPCSTR name,LPSTR destpath)	
//===================================================
{
char cmd[MAX_PATH],tmp[MAX_PATH];
int	 ptype,ret,retval=0,mode;

	strcpy(tmp,destpath);
	ptype = packertype(name);
	if (ptype!=TOSS_UNKNOWN_PACKER && ptype!=TOSS_FILE_NOT_FOUND)	// unpack file
	{
		expand_external_cmd(Packers[ptype].upck_cmd,destpath,name,cmd);
		mode=get_cfg(CFG_COMMON,"HideCmdWin",0) ? SW_HIDE : SW_SHOWMINNOACTIVE;
	 	ret=system_command(cmd,langstr("Decrunching File","Распаковка файла"),
			tmp,mode,300000,&retval);
 		if (!ret)
			return 0;

		if (retval)
		{
			err_out("E_DEPFAULED",cmd,retval);
			return 0;
		}
		return 1;
	}
	if (ptype==TOSS_UNKNOWN_PACKER)
		ERR_MSG2_RET0("E_UNKNOWNPACKER",name);
	
	return 0;
}

// ================================================ // returns 145 bei "filename.145"
	void get_ext_number(char *name,int *number)
// ================================================
{
int t;

	*number = -1;
	t = strlen(name)-1;

	if (t>=1) // nur wenn name nicht leer ist
	{
		while (name[t] != '.' && t > 0)
			t--;

		t++;
		*number = atoi(&name[t]);
	}
}

// =====================================================
// merges diff to nl, makes new nl with extension of diff and deletes the old nl
	int execute_nodediff_merge(char *nl,char *diff)
// =====================================================
{

char newnl[300];
char buf[1024];
int  t,ext;
int	 count=0;

FILE *fpdiff;
FILE *fpnl;
FILE *fpnew;

	strcpy(newnl,nl);

	t=strlen(newnl);
	t--;
	while ( newnl[t] != '.' && t > 0)
		t--;
	t++;
	newnl[t]=0;

	get_ext_number(diff,&ext);
	sprintf(buf,"%03d",ext);

	strcat(newnl,buf);   // built new nodelistname

	fpdiff 	= fopen(diff,"rt");
	 if (!fpdiff)
		 return 0;

	fpnl 	= fopen(nl,"rt");
	if (!fpnl)
 	{
		 fclose(fpdiff);
		 return 0;
	}

	fpnew=fopen(newnl,"wt");
	if (!fpnew)
 	{
		 fclose(fpnl);
		 fclose(fpdiff);
		 return 0;
	}

	while (fgets(buf,sizeof(buf),fpdiff) != NULL)
	{
		buf[0] = toupper(buf[0]);

		if (buf[0] == 'C' || buf[0] == 'A' || buf[0] == 'D')
		{
			char tempval[50];
			strcpy(tempval,&buf[1]);
			count = atoi(tempval);
		}

		switch (buf[0])
	 	{
	 		case 'C': 
				for (t=0;t<count;t++)              // copy lines from nl to newnl
				{
					fgets(buf,sizeof(buf),fpnl);
					fprintf(fpnew,"%s",buf);
				}
				break;
			case 'A': 
				for (t=0;t<count;t++)				 // add lines from diff to newnl
				{
					fgets(buf,sizeof(buf),fpdiff);
					fprintf(fpnew,"%s",buf);
				}
				break;
			case 'D': 
				for (t=0;t<count;t++)				 // skip lines in nl
					fgets(buf,sizeof(buf),fpnl);
				break;
		 }
	}
 // copy the rest of the old nodelist to the new nodelist
	while (fgets(buf,sizeof(buf),fpnl)!= NULL)
		 fprintf(fpnew,"%s",buf);

	fclose(fpdiff);
	fclose(fpnew);
	fclose(fpnl);

	get_filename(newnl,buf);
	nl_add_listbox(L("S_146",buf));	// Created nodelist %s

	get_filename(diff,buf);
	nl_add_listbox(L("S_154",buf));	// Deleted nodediff %s
	unlink(diff);

	get_filename(nl,buf);
	nl_add_listbox(L("S_155",buf));	// Deleted old nodelist %s
	unlink(nl);

	add_to_nl_stringlist(nl);
	return 1;
}

//===================================================
// 	This functions merges a nodediff to	an already existing nodelist
	int nodediff_merge()
//===================================================
{
CStrList NL;
CString	str;
char	path[MAX_PATH],tmppath[MAX_PATH],sbuf[MAX_PATH],nlname[MAX_PATH],diffname[MAX_PATH];
int		ende=1;
int		ret,t,foundatleastone=0;

	if (!check_for_nodediffs()) // return if no nodediff is found
		return 1;

	NL.LoadFromFile("nodelist.cfg");
	foundatleastone=0;
	for (t=0;t<NL.GetCount();t++)
	{
		if (get_diff_name(diffname,t))
		{
			ende=0;
			strupr(diffname);
			while (ende==0)
			{
				make_path(path,gc.InboundPath,diffname);
				nl_add_listbox(L("S_361",diffname));	// Processing Nodediff %s
// trying UniPack
				if (get_cfg(CFG_TOSSER,"EnableBuiltinArc",0) && hUni)
				{
					UnPackPrc=(UNPACKPROC)GetProcAddress(hUni,"UnPack");
					if (!UnPackPrc)
					{
						str.Format(IDS_SYSERROR,GetLastError(),"UnPack");
						AfxMessageBox(str);
						goto extpack;
					}
					ret=UnPackPrc(path,gc.NodelistPath,NULL,NULL,"*","O");
					if (ret==0)
					{
						SafeFileDelete(path);
						goto CHECKNEXT;
					}
				}
// using external packer
extpack:
				if (packertype(path)!=TOSS_UNKNOWN_PACKER)
				{
					if (!extract_nodediff(path)) // unpack nodediff to NL folder
						ERR_MSG2_RET0("E_UNKNOWNPACKER",diffname);
				}
				else // nodediff already unpacked, so moved to NL folder
				{
					nl_add_listbox(L("S_333",diffname));	// No packer defined for %s

					ret=show_msg_t("TI_CPYDIFF",1,10,L("S_286"),L("S_322"));	// Yes, No
					if (ret!=1)
						return 0;
					
					make_path(tmppath,gc.NodelistPath,diffname);
					nl_add_listbox(L("S_321",gc.NodelistPath));	// Moving file to %s

					if (CopyFile(path,tmppath,FALSE)==TRUE) // FALSE == Overwrite !
						 unlink(path);
					else
					{
						nl_add_listbox(L("S_201",GetLastError()));	// Error moving: %d
						return 0;
					}
				}
CHECKNEXT:
				if (!get_diff_name(diffname,t)) // check next nodelist entry
					ende=1;
			}
		}
	}

// now find the matching nodediffs and nodelists and merge them ...
LOOKNEXT:

	foundatleastone=0; // until here we haven't found a _matching_ diff to merge
	t=0;

	for (t=0;t<NL.GetCount();t++)
	{
		int extnumber=0;
		char foundstr[MAX_PATH];

		strcpy(foundstr,"");
		strcpy(nlname,NL.GetFromTable(t,1,'\t')); // get nodelist from setup
		strcpy(diffname,NL.GetFromTable(t,2,'\t')); // get nodediff from setup
	 	get_newest_nodelist(gc.NodelistPath,nlname);
		get_ext_number(nlname,&extnumber);

		if (extnumber!=-1)
		{
			extnumber+=6;	// first test NL+6
			if (extnumber>365)	extnumber-=365;
			sprintf(sbuf,"%s.%03d",diffname,extnumber);
			make_path(path,gc.NodelistPath,sbuf);

			if (!access(path,0))
				 strcpy(foundstr,path);
			else
			{					// second test NL+7
				extnumber++;
				if (extnumber>365)	extnumber-=365;
				sprintf(sbuf,"%s.%03d",diffname,extnumber);
				make_path(path,gc.NodelistPath,sbuf);

				if (!access(path,0))
					strcpy(foundstr,path);
				else
				{
					extnumber++;// third test NL+8
					if (extnumber>365)	extnumber-=365;
					sprintf(sbuf,"%s.%03d",diffname,extnumber);
					make_path(path,gc.NodelistPath,sbuf);

					if (!access(path,0))
						strcpy(foundstr,path);
				}
			}

		} // end of extnumber != -1

		if (strlen(foundstr)>2)
		{
			char temp1[80];
			char temp2[80];

			get_filename(nlname,temp1);
			get_filename(foundstr,temp2);
			nl_add_listbox(L("S_315",temp1,temp2));	// Merging %s and %s
			foundatleastone=1;
			execute_nodediff_merge(nlname,foundstr); // merge two files
			break;
		}
		else
		{
			get_filename(nlname,path);
			if (strlen(nlname)>1)
				nl_add_listbox(L("S_336",path));	// No valid Nodediff found for %s

			foundatleastone=0;
		}
	}

	if (foundatleastone) // try to find another nodediff to merge
		goto LOOKNEXT;

	return 1;
}

// ===================================================
	int check_inbound(void)
// ===================================================
{
_finddata_t se;
CStrList bosses,MM,NL;
CString	fipsdir;
char	flistp[300],flistu[300],newlistp[300],newlistu[300],echolist[300];
char	path[MAX_PATH],destpath[MAX_PATH],tmp[MAX_PATH],tmp1[MAX_PATH];
int		node,net,zone,retval,ret,t,mode;
long	hf;

	fipsdir=gc.BasePath;
	strcpy(tmp1,fipsdir);
	strcpy(destpath,gc.UtilPath);
	_mkdir(destpath);

	fipsdir+="\\checkin.bat";
	if (!access(fipsdir,0))       // check autoexec-batch
	{
		modem_add_listbox(L("S_235",fipsdir));	// Executing %s
		mode=get_cfg(CFG_COMMON,"HideCmdWin",0) ? SW_HIDE : SW_SHOWMINNOACTIVE;
	 	ret=system_command(PS fipsdir,"Inbound Batch",tmp1,mode,300000,&retval);
		if (ret!=1 || retval!=0)
			modem_add_listbox(L("S_198",ret,retval));	// Error executing inbound-batch: ret=%d error=%d
	}
// proccess tic-files
	tic_inbound();

// check for predefined files
	load_bosslist(bosses);
	for (t=0;t<bosses.GetCount();t++)
	{
		strcpy(echolist,bosses.GetFromTable(t,ECHOFILE));	// get echolist
		strcpy(flistp,bosses.GetFromTable(t,FLISTARC)); 	// get filelist packed
		strcpy(flistu,bosses.GetFromTable(t,FLISTUNA)); 	// get filelist unpacked
		strcpy(newlistp,bosses.GetFromTable(t,NFLSTARC));	// get newfilelist packed
		strcpy(newlistu,bosses.GetFromTable(t,NFLSTUNA));	// get newfilelist unpacked
		strcpy(tmp,bosses.GetFromTable(t,BOSSADDR)); 			// get bosses-aka
		parse_address(tmp,&zone,&net,&node);
// check for multimedia-packets
		make_path(path,gc.InboundPath,"FMMP*.*");
		MM.FillWithFiles(path);
		for (int t=0;t<MM.GetCount();t++)
		{
			make_path(path,gc.InboundPath,MM.GetString(t));
			modem_add_listbox(L("S_451",MM.GetString(t)));
			if (extract_file(path,gc.MultimedPath))
			{
				SafeFileDelete(path);
				modem_add_listbox(L("S_317",MM.GetString(t)));
			}
			else
				err_out("E_EXTFAD",path);
		}
// check for echolists
		check_file(echolist);
// check for packed filelists
		check_file(flistp,destpath);

		sprintf(tmp,"%d_%d_%d.filelist",zone,net,node);
		make_path(path,gc.UtilPath,tmp);		// new unpacked listname
		make_path(tmp1,gc.UtilPath,flistu);	// old unpacked listname

		if (*flistu && !access(tmp1,0))
		{
			if (CopyFile(tmp1,path,FALSE)==TRUE)
				unlink(tmp1);
			else
				err_out("E_CANTCREATEDUMMY",path);
		}
// check for packed newfilelists
		check_file(newlistp,destpath);
// check for unpacked newfilelists
		check_file(newlistu);

		sprintf(tmp,"%d_%d_%d.newflist",zone,net,node);
		make_path(path,gc.UtilPath,tmp);			// new newlistname
		make_path(tmp1,gc.UtilPath,newlistu);	// old newlistname

		if (*newlistu && !access(tmp1,0))
		{
			if (CopyFile(tmp1,path,FALSE)==TRUE)
				unlink(tmp1);
			else
				err_out("E_CANTCREATEDUMMY",path);
		}
	}

// check packed nodelists in inbound and unpack them into NL-folder
	NL.LoadFromFile("nodelist.cfg");
	for (t=0;t<NL.GetCount();t++)
	{
		make_path(path,gc.InboundPath,NL.GetFromTable(t,1,'\t'));// get nlname
		strcat(path,".*");
		if ((hf=_findfirst(path,&se))!=-1L)
		{
			make_path(path,gc.InboundPath,se.name);
			if (packertype(path)!=TOSS_UNKNOWN_PACKER)
			{
				if (extract_file(path,gc.NodelistPath))	// unpack nodelist
				{
					modem_add_listbox(L("S_263",se.name));	// Found nodelist, unpacking to NODELIST folder
					DeleteFile(path);
				}
			}
			else // already unpacked nodelist
			{
				make_path(tmp,gc.NodelistPath,se.name);
				if(!MoveFileEx(path,tmp,MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
					err_out("E_CANTCREATEDUMMY",tmp);
				else
					modem_add_listbox(L("S_257",se.name));	// Found %s, moving to NODELIST folder
			}
			_findclose(hf);
		}
	}
// now kill zero-length files
	make_path(path,gc.InboundPath,"*.*");
	hf =_findfirst(path,&se);
	if (hf != -1L)
	{
		do
		{
			if (!(se.attrib & _A_SUBDIR) && se.size==0)
			{
				 make_path(path,gc.InboundPath,se.name);
				 DeleteFile(path);
			}
		} while (_findnext(hf,&se)==0);
		_findclose(hf);
	 }
	return 1;
}

// =====================================================
	int systemcall(char *command,int &retval)
// =====================================================
{
CString ex;
char	retbuf[300],exe[300],*p;
int		ret,found;

	retval=0;

	if (strlen(command)<2)
	{
	    retval=-1;
	    return 0;
	}
	ret=sscanf(command,"%s",exe);
	ASSERT(ret==1);

	found=0;
	p=strchr(exe,'.');
	if (p)
	    found=SearchPath(NULL,exe,NULL,299,retbuf,&p);
	else
	    found=is_exe_available(exe);

	if (found==0)
	{
	   retval=-2;
	   return 0;
	}

	retval=1;
	ret=system(command);
	return (ret);
}


// ************************************************************************************
// ************************* LOGFILE STUFF  *******************************************
// ************************************************************************************

// ==================================================================================
	void get_logoptions(int id,BOOL &en,int &maxsize,int &shorten)
// ==================================================================================
{
CString tmp;

	switch (id)
	{
		case LOG_MAILER:
			en=get_cfg(CFG_LOGS,"MailerLog",1);
			tmp=get_cfg(CFG_LOGS,"MailerLim","100,50");
			break;
		case LOG_TOSSER:
			en=get_cfg(CFG_LOGS,"TosserLog",1);
			tmp=get_cfg(CFG_LOGS,"TosserLim","100,50");
			break;
		case LOG_PURGER:
			en=get_cfg(CFG_LOGS,"PurgerLog",1);
			tmp=get_cfg(CFG_LOGS,"PurgerLim","100,50");
			break;
		case LOG_EVENTS:
			en=get_cfg(CFG_LOGS,"EventsLog",1);
			tmp=get_cfg(CFG_LOGS,"EventsLim","100,50");
			break;
		case LOG_NL:
			en=get_cfg(CFG_LOGS,"NLCompLog",1);
			tmp=get_cfg(CFG_LOGS,"NLCompLim","100,50");
			break;
	}
	sscanf(tmp,"%d,%d",&maxsize,&shorten);
}

// ==================================================================================
	void writelog(int idd, LPCSTR text)
// ==================================================================================
{
static int 	ml_first=0;
static int 	ts_first=0;
static int 	pr_first=0;
static int 	ev_first=0;
static int 	nl_first=0;

	EnterCriticalSection(&GlobalLogFileAccess);
	switch (idd)
	{
		case LOG_MAILER:
			log_to_file(LOG_MAILER,"fipsmail.log","MAILER",text,ml_first);
			break;
		case LOG_TOSSER:
			log_to_file(LOG_TOSSER,"fipstoss.log","TOSSER",text,ts_first);
			break;
		case LOG_PURGER:
			log_to_file(LOG_PURGER,"fipspurg.log","PURGER",text,pr_first);
			break;
		case LOG_EVENTS:
			log_to_file(LOG_EVENTS,"fipsevnt.log","EVENTLOG",text,ev_first);
			break;
		case LOG_NL:
			log_to_file(LOG_NL,"fipsnodl.log","NL-COMPILER",text,pr_first);
	}
	LeaveCriticalSection(&GlobalLogFileAccess);
}

// ==================================================================
// expands template command string
	void expand_external_cmd(LPSTR templ,LPSTR flist,LPCSTR outfile,LPSTR outcmd)
// ==================================================================
// macroses in template:   %f - filename, %p - destination path
{
FILE *fp;
CString out;
char tmp[MAX_PATH],tmpfile[MAX_PATH],dir[MAX_PATH],destpath[MAX_PATH],filename[MAX_PATH];
char *p;

	if (strchr(flist,'*'))
	{
		get_filename(flist,tmpfile);
		strcpy(dir,flist);
		p=dir+strlen(dir);
		while(p>dir && *p!='\\')	p--;
		*p=0;
		GetShortPathName(dir,tmp,sizeof(tmp));
		make_path(destpath,tmp,tmpfile);
	}
	else
		GetShortPathName(flist,destpath,sizeof(destpath));	   // getshortfilename


	if (access(outfile,0))	// for getshortpathname an existing file is needed !
	{
		fp=fopen(outfile,"wt");
		fprintf(fp,"dummy");
		fclose(fp);
		GetShortPathName(outfile,filename,sizeof(filename));
		_unlink(outfile);
	}
	else
		GetShortPathName(outfile,filename,sizeof(filename));

	out=templ;
	out.Replace("%p",destpath);
	out.Replace("%f",filename);

	strcpy(outcmd,out);
}

// ===============================================================
	int capi_abort(int val)
// ===============================================================
// TOP: Diese Funktion wird von der supercom aus beim CAPI Zeuch aufgerufen ...
{
	return (Info(ISDNPORT,0,0,NULL)==infBREAK);
}

// ===============================================================
	int init_capi_callback(void)
// ===============================================================
// TOP: Hier sagen wir der Supercom welche Funktion bei dem ganzen CAPI Zeuchs
// als Callback aufgerufen werden kann ...
{
	capi_set_callbackfunc((CAPICALLBACK)capi_abort);
	return 1;
}

//###################################################################################
//								TICER STUFF
//###################################################################################

// ================================================================================
	int get_tic_filename(char *fname, char *path)
// ================================================================================
//
// returns a valid name for a new tic-file in path
{
char	tempname[300];
time_t	counter =0;

	time(&counter); // init mit aktueller zeit

	do
	{
		sprintf(tempname,"%s%08X.TIC",path,counter);
		counter++;
	}	while (access(tempname,0) == 0); // bis file nicht existiert

	counter--;
	strcpy(fname,tempname); // wert zurueckgeben.
	return 1;
}

// ================================================================================
	unsigned long crc32_of_file(char *filename)
// ================================================================================
{
FILE	*fp;
char	buffer[32762];
int		charsread;
unsigned long crc = 0xFFFFFFFF;

	fp = fopen(filename,"rb");
	if (!fp)
		return 0;

	while (1)
	{
		charsread=fread(buffer,1,sizeof(buffer),fp);

	//	for (t=0;t<charsread;t++)
	//		Do_CRC32(&crc,buffer[t]);

		RS_GetCRC_CCITT32 (buffer, charsread,&crc);
		if (charsread < sizeof(buffer))
			break;
	}
	fclose(fp);
	return (0xFFFFFFFF-crc); // so gehts, warum weiss ich nicht...
}

// ================================================================================
	int make_tic_file(int zone, int net, int node, int point,
					  int myzone, int mynet, int mynode, int mypoint,
					  LPCSTR fname,LPCSTR pass,LPCSTR desc,LPCSTR area,
					  int deletefile,LPCSTR replace,LPCSTR magic=NULL)
// ================================================================================
// creates a new ticfile int the outbound-dir for this bosses
{
CString dm;
FILE	*fp;
ULONG	crcvalue=0;
char	tic[300],outpath[300],tmp[300],datum[100];
char	areatag[300],pwd[300],filename[300];

	strcpy(areatag,area);
	_strupr(areatag);
	strcpy(pwd,pass);
	_strupr(pwd);
	strcpy(filename,fname);
	_strupr(filename);
	_strdate(datum);
	strcat(datum," ");
	_strtime(tmp);
	strcat(datum,tmp);

	make_fidodir(tmp,zone,net,node,point);
	make_path(outpath,gc.OutboundPath,tmp); // outbound-dir for this bosses
	mkdir(outpath);
	strcat(outpath,"\\");
	get_tic_filename(tic,outpath); // get new filename for new tic file
	if (access(filename,0))	return 0; // file not found
	crcvalue = crc32_of_file(filename);
	fp = fopen(tic,"wt");
	if (!fp)
		ERR_MSG2_RET0("E_CNEWTIC",tic);

	fprintf(fp,"Area %s\n",areatag);
	fprintf(fp,"Origin %d:%d/%d.%d\n",myzone,mynet,mynode,mypoint);
	fprintf(fp,"From %d:%d/%d.%d\n",myzone,mynet,mynode,mypoint);
	fprintf(fp,"To %d:%d/%d\n",zone,net,node);

	if (strlen(replace) >0)
	{
		dm=replace;
		dm.MakeUpper();
		dm.AnsiToOem();
		fprintf(fp,"Replaces %s\n",dm);
	}

	if (strlen(magic) >0)
	{
		dm=magic;
		dm.AnsiToOem();
		fprintf(fp,"Magic %s\n",dm);
	}

	get_filename(filename,tmp);
	fprintf(fp,"File %s\n",tmp);

	if (strlen(desc) >0)
	{
		dm=desc;
		dm.AnsiToOem();
	}
	if (!strstr(dm,"\r"))
		fprintf(fp,"Desc %s\n",dm);
	else
	{
		for (int t=0;t<count_tokens(dm);t++)
		{
			extractstr(t+1,PS dm,"\r",tmp,sizeof(tmp));
			trim_all(tmp);
			if (t==0)
				fprintf(fp,"Desc %s\n",tmp);

			fprintf(fp,"LDesc %s\n",tmp);
		}
	}
	fprintf(fp,"Crc %08X\n",crcvalue);
	fprintf(fp,"Created by %s\n",get_versioninfo(tmp,0));
	fprintf(fp,"Path %d:%d/%d.%d %s\n",myzone,mynet,mynode,mypoint,datum);
	fprintf(fp,"Seenby %d:%d/%d.%d\n",myzone,mynet,mynode,mypoint);
	fprintf(fp,"Pw %s\n",pwd);
	fclose(fp);

	// tic-file wurde erstellt, jetzt die datei ins xfer.fls dazuhaengen

	make_path(tic,outpath,XFER_LIST);
	fp = fopen(tic,"at");
	if (!fp)
		ERR_MSG2_RET0("E_CNAF",filename);

	if (deletefile)
		fprintf(fp,"+");
	
	fprintf(fp,"%s\n",filename);
	fclose(fp);
	return 1;
}

// ================================================================================
	void process_tic(char *fname,char *inbound,char *ticmail,char *newmail)
// ================================================================================
{
FILE	*fp;
char	line[1024],linecopy[1024],tmp[300],areadesc[300];
int		ldescfound=0;
int		t,found;
long	lastvisited=0;
char	*p;

// variables for tic-file

char	areatag[300],pwd[100],ticfile[300],ticupl[300];

CStrList desc;
CString	str;

// vars for tic-setup

char	tarea[100],tupl[100],tdesc[300],tpath[300];
int		tcopy=0;

	desc.RemoveAll();
	memset(areatag,0,sizeof(areatag));
	memset(pwd,0,sizeof(pwd));
	memset(ticfile,0,sizeof(ticfile));

	strcpy(areadesc,L("S_95"));	// autoadded file-echo

	fp = fopen(fname,"rt");
	if (!fp)
	{
		modem_add_listbox(L("S_197"));	// error accessing tic
		return;
	}

	while (fgets(line,sizeof(line),fp))
	{
		p=line+strlen(line)-1;
		while (p>=line && ISSPACE(*p)) // strip ending whitespaces
			*p--=0;
		strcpy(linecopy,line);
		strupr(line);
		sscanf(line,"%s",tmp);

		if (!strcmp(tmp,"AREA"))				// areatag
			sscanf(line,"%*s %s",areatag);
		else if (!strcmp(tmp,"FIRSTTIMECHECK")) // timecheck
			sscanf(line,"%*s %x",&lastvisited);
		else if (!strcmp(tmp,"PW"))				// password
			sscanf(line,"%*s %s",pwd);
		else if (!strcmp(tmp,"FILE"))			// tic-file
//			sscanf(line,"%*s %s",ticfile);
			strcpy(ticfile,rest_line(linecopy));
		else if (!strcmp(tmp,"FROM"))			// from uplink
			sscanf(line,"%*s %s",ticupl);
		else if (!strcmp(tmp,"AREADESC"))		// description
		{
			strcpy(areadesc,rest_line(linecopy));
			OemToChar(areadesc,areadesc);
		}
		else if (!strcmp(tmp,"DESC") && desc.GetCount()==0) // Single-line description
			desc.AddTail(rest_line(linecopy));
		else if (!strcmp(tmp,"LDESC")) // Multi-line description
		{
			if (!ldescfound && desc.GetCount() >0)
				desc.RemoveAll();

			ldescfound=1;
			desc.AddTail(rest_line(linecopy));
		}
	}

	fclose(fp);

	found=0;
	for (t=0;t<tics.GetCount();t++)
	{
		get_token(tics.GetString(t),0,str);
		strcpy(tarea,str);
		strupr(tarea);
		get_token(tics.GetString(t),1,str);
		strcpy(tupl,str);
		get_token(tics.GetString(t),2,str);
		strcpy(tdesc,str);
		get_token(tics.GetString(t),3,str);
		strcpy(tpath,str);
		strupr(tpath);
		tcopy=get_token_int(tics.GetString(t),4);

		if (!strcmp(areatag,tarea))
		{
			found=1;
			break;
		}
	}

	str=get_cfg(CFG_COMMON,"TicbasicPath",""); // get basic tic path
	if (!found && str.GetLength()>0)
	{
		CString temp;
		int retval;

		make_path(tmp,str,areatag);
		mkdir(tmp);
		retval = access(tmp,0); // check access to dir
		addbackslash(tmp);
		strupr(tmp);

		temp.Format("%s\t%s\t%s\t%s\t0",areatag,ticupl,areadesc,tmp);
		tics.AddTail(temp);

		strcpy(tarea,areatag);
		strcpy(tupl,ticupl);
		strcpy(tdesc,areadesc);
		strcpy(tpath,tmp);
		strupr(tpath);

		strcat(newmail,langstr("\r/Area:/ ","\r/Область:/ "));
		strcat(newmail,areatag);
		strcat(newmail," (");
		strcat(newmail,areadesc);
		strcat(newmail,langstr(")\r/Path:/ ",")\r/Путь   :/ "));
		strcat(newmail,tmp);
		strcat(newmail,langstr("\r/From:/ ","\r/Аплинк :/ "));
		strcat(newmail,ticupl);
		strcat(newmail,"\r");

		if (retval==0)
		{
			found = 1;
			tics.SaveToFile("tics.cfg");
		}
		else
		{
			temp.Format(langstr("Cannot create dir %s","Невозможно создать папку %s"),tmp);
			modem_add_listbox(temp);
		}
	}

	if (found) // area found
	{
		char fromfile[MAX_PATH];
		char tofile[MAX_PATH];

		make_path(fromfile,inbound,ticfile);
		addbackslash(tpath);
		strupr(tpath);
		make_path(tofile,tpath,ticfile);

		if (!access(fromfile,0))
		{
			if (strlen(ticmail) < (MAX_REQ_MAIL-1024))
			{
				strcat(ticmail,"\r/");
				strcat(ticmail,langstr("Area       :/ *","Область   :/ *"));
				strcat(ticmail,tarea);
				strcat(ticmail,"*\r/");
				strcat(ticmail,langstr("File       :/ *","Файл      :/ *"));
				strcat(ticmail,ticfile);
				strcat(ticmail,"*\r/");
				if (!tcopy)
					strcat(ticmail,langstr("Moved      :/ *","Переписан :/ *"));
				else
					strcat(ticmail,langstr("Copied     :/ *","Скопирован:/ *"));
				strcat(ticmail,tpath);
				strcat(ticmail,"*\r/");
				strcat(ticmail,langstr("Description:/ ","Описание  :/ "));
			}

			if (CopyFile(fromfile,tofile,FALSE))
			{
				if (!tcopy)
				{
					SafeFileDelete(fromfile);
					sprintf(tmp,langstr("Moving file %s to %s","перенос %s -> %s"),ticfile,tpath);
				}
				else
					sprintf(tmp,langstr("Copying file %s to %s","копия %s -> %s"),ticfile,tpath);

				modem_add_listbox(tmp);

				make_path(tofile,tpath,"FILES.BBS");
				if (desc.GetCount()>0)
				{
					FILE *fp;
					int t;

					fp = fopen(tofile,"at");
					if (fp)
					{
						fprintf(fp,"%-13s%s\n",ticfile,desc.GetString(0));
						if (strlen(ticmail) < (MAX_REQ_MAIL-1024))
						{
							str=desc.GetString(0);
							str.OemToAnsi();// to prevent conversion in build_routed_mail
							strcat(ticmail,str);
							strcat(ticmail,"\r");
						}

						t = 1;
						while (t < desc.GetCount())
						{
							fprintf(fp,"            +%s\n",desc.GetString(t));
							if (strlen(ticmail) < (MAX_REQ_MAIL-1024))
							{
								strcat(ticmail,"     ");
								str=desc.GetString(t);
								str.OemToAnsi();// to prevent conversion in build_routed_mail
								strcat(ticmail,str);
								strcat(ticmail,"\r");
							}
							t++;
						}
						fclose(fp);
					}
					else
						modem_add_listbox(L("S_487",tofile));	// cannot open %s
				}
				strcat(ticmail,"\r");
				SafeFileDelete(fname);
			}
			else
				modem_add_listbox(L("S_488"));	// cannot copy
		}
		else // tic-file is present, file is absent
		{
			if (!lastvisited)
			{
				FILE *fp;
				time_t dummy;

				time(&dummy);
				if (strlen(ticmail) < (MAX_REQ_MAIL-1024))
					strcat(ticmail,L("S_489",ticfile,tarea));	// file area tic delayed

				fp = fopen(fname,"at");
				if (fp)
				{
					fprintf(fp,"FirstTimeCheck %X\n",dummy);
					fclose(fp);
					modem_add_listbox(L("S_492"));	// tic delayed
				}
				else
					modem_add_listbox(L("S_493",fname));	// cannot access tic
			}
			else	// tic-file is older than 2 days, it should be deleted
			{
				time_t dummy;
				time(&dummy);
				dummy -= (3600*24*2);

				if (dummy > lastvisited)
				{
					get_filename(fname,tofile);
					modem_add_listbox(L("S_494",tofile));	// deleting tic
					SafeFileDelete(fname);
					if (strlen(ticmail) < (MAX_REQ_MAIL-1024))
						strcat(ticmail,L("S_495",ticfile,tarea));	// file area old tic deleted
				}
			}
		}
	}
	else
	{
		modem_add_listbox(L("S_498",areatag));	// no tic area
		modem_add_listbox(L("S_499"));	// file left in inbound
		if (strlen(ticmail) < (MAX_REQ_MAIL-1024))
			strcat(ticmail,L("S_500",ticfile,areatag));
	}
}

// ================================================================================
	void tic_inbound(void)
// ================================================================================
// proccessing all tics in inbound folder
{
_finddata_t se;
long	hf;
char	path[MAX_PATH],ticmail[MAX_REQ_MAIL],newmail[MAX_REQ_MAIL],tmp[100];

	if (get_cfg(CFG_COMMON,"NoTics",0))	return;

	memset(ticmail,0,sizeof(ticmail));
	make_path(path,gc.InboundPath,"*.TIC");
	hf = _findfirst(path,&se);
	if (hf != -1L)             // tic-file found
	{
		do
		{
			sprintf(tmp,L("S_503",se.name));	// processing %s
			modem_add_listbox(tmp);
			make_path(path,gc.InboundPath,se.name);
			process_tic(path,gc.InboundPath,ticmail,newmail);
		}
		while (_findnext(hf,&se) == 0);
	}

	if (strlen(ticmail)>0 || strlen(newmail)>0)
	{
		CString sender,subject,mailtext,name;

		get_fullname(name);
		sender=langstr("TIC-manager","TIC-менеджер");
		subject=langstr("Local Notification","Локальное уведомление");
		
		if (strlen(newmail)>0)
		{
			mailtext.Format(langstr(
				"\rThese TIC-areas were automatically created by FIPS:\r\r"
				"*Please check your TIC-area setup to validate the default settings*\r%s\r",
				"\rСледующие TIC-области были автоматически созданы FIPS:\r\r"
				"*проверьте установленные по умолчанию настройки TIC-области*\r%s\r"),
				newmail);
			build_routed_netmail(sender,"",name,"",subject,mailtext,0,0,1);
		}

		if (*ticmail>0)
		{
			Sleep(1000);	// for new MSGID
			mailtext.Format(langstr(
				"\rThese TIC-files were received from your uplinks:\r%s\r",
				"\rСледующие TIC-файлы были получены от Вашего аплинка:\r%s\r"),ticmail);
			build_routed_netmail(sender,"",name,"",subject,mailtext,0,0,1);
		}

		db_refresh_area_info(0);
	}
}

// ================================================================================
	void update_history(int histid,struct _sessiondata *session)
// ================================================================================
// update history fls: histid==0 for outbound, ==1 for inbound
{
CStrList lst;
CString tmp;
char	fname[300];
char	str[300];
int		secs;
int		mins;
int		hours;

	ASSERT (session);

	if (histid)
		make_path(fname,gc.BasePath,"inbound.hst");
	else
		make_path(fname,gc.BasePath,"outbound.hst");

	lst.LoadFromFile(fname);
	if (lst.GetCount()>500)
		lst.RemoveHead();

	secs=session->endtime-session->starttime;
	hours=(int)(secs/3600);
	mins=(int)(secs/60)%60;
	secs = secs % 60;

	session->system[45]=0;
	session->user[25]=0;

	tmp.Format("%s %s\t%s\t%s\t%02d:%02d:%02d",_strdate(str),_strtime(str),
		session->system,session->user,hours,mins,secs);

	if (!histid)
	{
		sprintf(str,"\t(=%2.2f)",session->cost);
		tmp += str;
	}

	lst.AddTail(tmp);
	lst.SaveToFile(fname);
}

// ============================================
	int ISDNCallAcceptionOnOff(int on)
// ============================================
// TOP: Diese Funktion wird aufgerufen wenn der Status der ISDN Anrufannahme
// bestimmt wird.
// Achtung: Sie wird mehrfach aufgerufen, deshalb als erstes der Check auf
// gueltigen Status.

{
static int IsOn=0;
int		ret;

	if ((IsOn && on) || (!IsOn && !on))
		return 0;			// In diesen Faellen muessen wir nix machen ...

	switch (on)
	{
		case 0:				// Ausschalten
			ret=Listen(0);
			free_capi_stuff();
			gcomm.CapiIsRegistered=0;
			IsOn=0;
			break;
		case 1:						// Einschalten
			// Jetzt muessen wir die Annahme MSN's in die NR1 bis Nr3 aufbroeseln ...
			if (!broesel_msns())
			{
				gcomm.AcceptOn1Modem=gcomm.AcceptOn2Modem=gcomm.AcceptOnIsdn=0;
				income_times.defaultindex=1;		// Disable all further INCOMES
				ERR_MSG_RET0("E_MSNSETUWR");
			}

			init_capi_callback(); // capi-callback func
			ret=init_capi_stuff();
			if (!ret)
			{
				modem_add_listbox("Call Acception: Cannot register CAPI!");
				gcomm.CapiIsRegistered=0;
				return 0;
			}
			gcomm.CapiIsRegistered=1;

			ret=Listen(0x0004);		// Zur Zeit nur Daten ...
			IsOn=1;
			break;
		default:
			ASSERT(0);
			break;
	}
	return 1;
}

// ============================================
	int HandleTestForIncomingISDNCall(void)
// ============================================
// TOP: Diese Funktion wird bei eigeschalteter ISDN Anrufannahme
// alle halbe Sekunde aufgerufen.
// Rueckgabewerte: 0 kein Anruf da oder abgelehnt
//                 1 Anruf angenommen

// Die komplette Annahme muss in dieser Funktion erledigt werden ...
// Zu dieser Zeit lauft noch kein ISDN Worker Tread ...
{
int ret;
int dummy=0;

	if (!gcomm.CapiIsRegistered)
		return 0;

	gcomm.CallAcceptActive=1;
	gcomm.IncomingSignalled=0;
	ret=SupHandleTestForIncomingISDNCall(0);		// Parameter zur Zeit dummy ...
	gcomm.CallAcceptActive=0;
	if (ret==1)
		dummy++;
	return ret;
}

// ============================================
	void handle_tel_nr_rotation(char *phone)
// ============================================
{
int		i;
int		wc;
CString line;
CString line2;
int		index=0;

	wc=count_tokens(phone,ALT_TEL_DELIM"\t\r\n");
	if (wc<=1)
		return;

	for (i=0;i<alt_phone.GetCount();i++)
	{
		line=alt_phone.GetString(i);
		get_token(line,0,line2);
		if (!strcmp(phone,line2))
		{
			index=get_token_int(line,1);
			if (wc > index)
			    index++;
			else
				index=1;

			line.Format("%s\t%d",line2,index);
			alt_phone.Insert(i,line);
			alt_phone.Remove(i);

			memset(phone,0,200);
			extractstr(index,PS line,ALT_TEL_DELIM,phone,299);
			return;
		}

	}
	line.Format("%s\t%d",phone,1);
	memset(phone,0,200);
	extractstr(1,PS line,ALT_TEL_DELIM,phone,299);
	alt_phone.AddTail(line);
}

// ============================================
	void clear_mailstate(void)
// ============================================
{
	gc.mailer.m_timeelapsed=0;
	gc.mailer.m_timeelapsed2=0;
	gc.mailer.m_cps=0;
	gc.mailer.m_cps2=0;
	gc.mailer.m_transferred=0;
	gc.mailer.m_transferred2=0;
	gc.mailer.m_total=0;
	gc.mailer.m_total2=0;
	gc.mailer.m_proz=0;
	gc.mailer.m_proz2=0;
	gc.mailer.m_filename[0]=0;
	gc.mailer.m_filename2[0]=0;
	gc.mailer.m_status[0]=0;
	gc.mailer.m_system[0]=0;
	gc.mailer.m_sysop[0]=0;
	gc.mailer.m_location[0]=0;
}

// ============================================
// generates unique pkt-name
	char *gen_pktname(char *str,LPCSTR path)
// ============================================
{
static long oldtime=time(NULL);
time_t	curtime;
char	tmp[MAX_PATH];

nextname:
	do{time(&curtime);} while (oldtime==curtime);	// generate unique time
	Sleep(1000);
	oldtime=curtime;
	sprintf(str,"%08X.pkt",curtime);
	make_path(tmp,path,str);
	if (!access(tmp,0))
		goto nextname;
	return str;
}

// ============================================
// adds all fls with wildcard 'path\\name' to sendlist
	char *add_to_sendlist(LPCSTR path,LPCSTR name)
// ============================================
{
struct _finddata_t se;
long	hf;
char	tmp[MAX_PATH];

	make_path(tmp,path,name);
	if ((hf = _findfirst(tmp,&se)) != -1L)
	{
		do
		{
			if (!(se.attrib & _A_SUBDIR))
			{
				make_path(tmp,path,se.name);
				add_file_to_sendlist(se.name,tmp);
			}

		} while(_findnext(hf,&se)==0);
		_findclose(hf);
	}
	return sendlist;
}

// ============================================
	int collect_bossinfo(bossarray *ba,int maxboss,char *outpath)
// ============================================
{
CString str;
CFido	fidodir;
char	buf[300];
char	bossinfo[MAX_BOSSLEN];
int		i=0;

	for (int t=0;t<maxboss;t++)	// build bosses info
	{
		memset(&ba[t],0,sizeof(struct bossarray));
		if (db_get_boss_by_index(t,bossinfo))
		{
			strcpy(ba[i].routing,"");
			get_token(bossinfo,MAINAKA,str);	// point address
			strcpy(ba[i].mypoint,str.GetBuffer(strlen(ba[i].mypoint)));
			get_token(bossinfo,BOSSADDR,str);	// bosses address
			strcpy(ba[i].fido,str.GetBuffer(strlen(ba[i].fido)));
			get_token(bossinfo,ROUTING,str);	// routing
			strcpy(ba[i].routing,str.GetBuffer(strlen(ba[i].routing)));
			get_token(bossinfo,ARCMPASW,str);	// arcmail pwd
			strcpy(ba[i].pwd,str.GetBuffer(strlen(ba[i].pwd)));
			parse_address(ba[i].fido,&ba[i].zone,&ba[i].net,&ba[i].node,&ba[i].point);
			parse_address(ba[i].mypoint,&ba[i].pzone,&ba[i].pnet,&ba[i].pnode,&ba[i].ppoint);
			get_token(bossinfo,ARCHIVER,str);	// packer
			strcpy(ba[i].packer,str.GetBuffer(strlen(ba[i].fido)));
			fidodir.Set(ba[i].zone,ba[i].net,ba[i].node,ba[i].point);
			fidodir.GetAsDir(buf);
			strcpy(ba[i].dir,outpath);
			strcat(ba[i].dir,buf);
			i++;
		}
	}
	return i;
}

// ============================================
	void close_packets(FILE **fp,FILE **fpn,int maxuplink,int wasnetmail)
// ============================================
{
char termchar[2]={0,0};

	for (int k=0;k<maxuplink;k++) // close all open packets
	{
		if (fp[k])
		{
			fwrite(termchar,1,2,fp[k]); // endbytes
			fclose(fp[k]);
		}
		if (fpn[k])
		{
			if (wasnetmail)
				fwrite(termchar,1,2,fpn[k]);
			fclose(fpn[k]);
		}
	}
}

// ============================================
	void build_subj_attaches(char *path,char *subject)
// ============================================
{
CStrList xferlist;
FILE *fp;
char fls[80][80],buf[100],tmp[80];
int	 found;

	modem_add_listbox(langstr("     Attached fls:","     Прикрепленные файлы:"));
	memset(fls,0,sizeof(fls));
	OemToChar(subject,subject);
	GetShortPathName(subject,subject,80);
#ifdef BNDCHECK
	strcpy((char*) &fls[0],subject);
#else
	sscanf(subject,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
		&fls[0],&fls[1],&fls[2],&fls[3],&fls[4],&fls[5],&fls[6],
		&fls[7],&fls[8],&fls[9],&fls[10],&fls[11],&fls[12],&fls[13],
		&fls[14],&fls[15],&fls[16],&fls[17],&fls[18],&fls[19]);
#endif
	*subject=0; // prepare subject to fill with filenames
	make_path(buf,path,XFER_LIST);
	xferlist.FillAsEdit(buf);
	fp=fopen(buf,"at");
	for (int i=0;i<20;i++)
	{
		if (strlen(fls[i])>0)
		{
			found=0;
			for (int j=0;j<xferlist.GetCount();j++)
			{
				if (strstr(xferlist.GetString(j),fls[i]))
				{
					found=1;
					break;
				}
			}

			get_filename(fls[i],tmp);
			strcat(subject,tmp);
			strcat(subject," ");

			if(found)
				sprintf(buf,langstr("File %s already attached","файл %s уже прикреплен"),fls[i]);
			else
			{
				fprintf(fp,"%s\n",fls[i]);
				sprintf(buf,"[%d] %s",i+1,fls[i]);
			}
			modem_add_listbox(buf);
		}
	}
	rtrim(subject); // kill ending spaces
	fclose(fp);
}

// ============================================
	void build_uplink_file(struct bossarray &ba,MEDPKTHDR2 *pl,FILE **fp,FILE **fpn,int otherboss,int curarea,int *wasnetmail)
// ============================================
{
CFido fidodir;
char outbpath[300],pktpath[300],buf[300];
time_t akttime=0;

	if (otherboss)
		fidodir.Set(ba.nzone,ba.nnet,ba.nnode,ba.npoint);
	else
		fidodir.Set(ba.zone,ba.net,ba.node,ba.point);

	fidodir.GetAsDir(buf);
	make_path(outbpath,gc.OutboundPath,buf);
	_mkdir(outbpath);	// outbound path for this bosses
	make_path(pktpath,outbpath,"NETMAIL");
	_mkdir(pktpath);	// pktpath for this bosses

	if (curarea)
	{
		gen_pktname(buf,outbpath);
		make_path(ba.archivename,outbpath,buf);
		*fp = fopen(ba.archivename,"wb");	// arc-file
	}
	else
	{
		gen_pktname(buf,pktpath);
		make_path(ba.archivename,pktpath,buf);
		*fpn = fopen(ba.archivename,"wb");	// pkt-file
	}

	pl = (MEDPKTHDR2 *)&po;	// PKT-HDR 2.2, geht nicht mit squish !
	memset(pl,0,sizeof(MEDPKTHDR2));
	pl->orig_zone 	= ba.pzone;
	pl->qm_orig_zone= ba.pzone;
	pl->orig_net 	= ba.pnet;
	pl->orig_node 	= ba.pnode;
	pl->orig_point 	= ba.ppoint;
	pl->dest_zone 	= ba.zone;
	pl->qm_dest_zone= ba.zone;
	pl->dest_net 	= ba.net;
	pl->dest_node 	= ba.node;
	pl->dest_point 	= ba.point;
	pl->capword		= 1;
	pl->capword2	= 256; // byte swapped
	pl->ver			= 2;
	pl->product		= 0x10; // fips product code
	pl->product2	= 0x01;
	get_versioninfo(buf,6);
	pl->rev_lev =buf[0];	// MAIN_BUILD
	get_versioninfo(buf,7);
	pl->rev_lev2=buf[0];	// SUB_BUILD
	
	// refresh time and date in pkt-hdr
	_strdate(buf);
	sscanf(buf,"%hu/%hu/%hu",&pl->month,&pl->day,&pl->year);
	pl->year += pl->year>40 ? 1900 : 2000;
	_strtime(buf);
	sscanf(buf,"%hu:%hu:%hu",&pl->hour,&pl->minute,&pl->second);

	ba.pwd[8]=0; // strip pwd to a max of 8 chars
	memset(pl->password,0,8);
	if(strlen(ba.pwd)>0)
		memcpy(pl->password,ba.pwd,8);
	// now write pkt-hdr
	if (curarea)
		fwrite(pl,sizeof(MEDPKTHDR2),1,*fp);
	else
	{
		*wasnetmail=true;
		fwrite(pl,sizeof(MEDPKTHDR2),1,*fpn);
	}
}

// ============================================
	void rename_to_bad(LPCSTR fname)
// ============================================
{
char newname[MAX_PATH],*p;

	strcpy(newname,fname);
	p=strrchr(newname,'.');
	if (p)	*p=0;
	strcat(newname,".BAD");
	rename(fname,newname);
	tosser_add_listbox(L("S_511",newname));
}

// ============================================
	int filter_re(char *src,char *dst)
// ============================================
{
char *p,*t;
int  ret,n;
int	 k=0,nr=0;

	strnzcpy(dst,src,80);
	p=src;
nextsearch:
	while (*p && *p==' ')	p++;	// skip forwarding spaces
	if (*p==0)
	{
		*dst=0;
		return 0;
	}
	t=p;
	if (!strnicmp(p,"re",2))
	{
		p+=2;
//		while (*p && *p==' ')	p++;	// skip spaces
		switch (*p)
		{
			case ':':          // 'Re:'
			case ' ':          // 'Re '
				p++;
				k++;
				goto nextsearch;
			case '^':          // 'Re^n:'
				p++;
				ret=sscanf(p,"%d%n",&nr,&n);
				if (ret)
				{
					p+=n;
					if (*p==':')
					{
						p++;
						k+=nr;
						goto nextsearch;
					}
				}
				break;
			case '[':          // 'Re^n:'
				p++;
				ret=sscanf(p,"%d%n",&nr,&n);
				if (ret)
				{
					p+=n;
					if (!strncmp(p,"]:",2))
					{
						p+=2;
						k+=nr;
						goto nextsearch;
					}
				}
				break;
		}
	}
	strcpy(dst,t);
	return k;
}

// ============================================
	LPCSTR rest_line(LPCSTR line)
// ============================================
{
LPCSTR p;

	p = line;
	while (*p && *p != ' ')
		p++;
	while (*p && *p == ' ')
		p++;

	return p;
}

// ============================================
	BOOL bad_field(char *fld,char *fname)
// ============================================
{
	tosser_add_listbox(L("S_512",fld,fname));	// bad data
	if (!get_cfg(CFG_TOSSER,"IgnoreErrors",0))	return TRUE;
	return FALSE;
}

// ============================================
	void ClearSession(BYTE Com)
// ============================================
{
	if (Com==ISDNPORT)
		capi_hangup();
	else
		ComReset(Com);
	gincome.InModemEmsiSession=gincome.MailerNeedsModem=0;
	set_fips_priority(0);
}

// ============================================
// move file from inbound to utils, optional - unpack
	void check_file(LPCSTR fname,LPSTR unppath)
// ============================================
{
char path[MAX_PATH],dest[MAX_PATH];

	make_path(path,gc.InboundPath,fname);
	if (*fname && !access(path,0))
	{
		modem_add_listbox(L("S_513",fname));
		if (unppath && packertype(path) != TOSS_UNKNOWN_PACKER)
		{
			if (extract_file(path,unppath))
				unlink(path);
			else
				err_out("E_EXTFAD",path);
		}
		else // file not packed
		{
			make_path(dest,gc.UtilPath,fname);
			if (CopyFile(path,dest,FALSE)==TRUE) // FALSE == Overwrite !
			  	unlink(path);
		}
	}
}

// ===================================================================
	int append_info_dialing	(void)
// ===================================================================
{
long jetzt;

	if (!notbremse)
	{
		notbremse=(ULONG *)malloc(1000*sizeof(long));
		notbremscounter=0;
		if (!notbremse)	ERR_MSG_RET0("E_MEM_OUT");
	}
	if (notbremscounter>0 && !((notbremscounter+1)%1000))
	{
		notbremse=(ULONG *)realloc(notbremse,(notbremscounter+1001)*sizeof(long));
		if (!notbremse)	ERR_MSG_RET0("E_MEM_OUT");
	}

	jetzt=time(NULL);
	*(notbremse+notbremscounter)=jetzt;
	notbremscounter++;
	return 1;
}

