#include "stdafx.h"
#include <afxcmn.h>
#include <ASSERT.h>
#include <io.h>
#include <time.h>
#include "mystrlst.h"
#include "resource.h"
#include "lightdlg.h"
#include "direct.h"
#include <afxtempl.h>
#include "structs.h"

// ******************************
// Database exported prototypes
// ******************************

// global vars
pareadef	gAreadef=0;		   	// Pointer to array of area description structures
pareadef	gbackupAreadef=0;
long		num_of_areas=0;		// Number of valid area description structs
long		gbackup_numof_area_structs=-1;	   // var for  BACKUP and RESTORE

DWORD    dret;                  // Hilfsvariable fuer SET Test

extern _magicid magicid; // MAGIC ID struct
extern _gconfig gc;
//struct _mainconf mainconf;
extern CStrList	AreasList;
extern CStrList UserdefAreaList;
extern CStrList Akas;
extern int	gNewAreaFromExtSearch;
extern int	gSortCriteria;

CRITICAL_SECTION AreaLockCS;
CRITICAL_SECTION MailAccessCS;
CRITICAL_SECTION WWDAccessCS;

CStrList	Bosses;
BOOL		bPurgeScanned=FALSE;
int			area_search_amount=0;
int			DelayedAreas[100];	// locked areas

void db_sort_areas (void);

// ===============================================================
// builds aka-string, if extra==TRUE, includes all bosses aka's
	void db_build_akas(LPSTR szaka,BOOL extra)
// ===============================================================
{
CString aka;
char	bossinfo[MAX_BOSSLEN],mainaka[100];
int		i=0;

	strcpy(mainaka,szaka);
// append other bosses aka's
	if (extra || *mainaka==0)
	{
 		while (db_get_boss_by_index(i++,bossinfo))
		{
			get_token(bossinfo,MAINAKA,aka);
			if (aka.GetLength() && *mainaka && strcmp(mainaka,aka))
			{
				if (*szaka)
					strcat(szaka," ");
				strcat(szaka,aka);
			}
		}
	}
// append additional aka's
	for (i=0;i<Akas.GetCount();i++)
	{
		aka=Akas.GetString(i);
		if (aka.GetLength() && (extra || (*mainaka && strcmp(mainaka,aka))))
		{
			if (*szaka)
				strcat(szaka," ");
			strcat(szaka,aka);
		}
	}
}

// ===============================================================
//	returns BOSS and POINT address for area
	int  db_get_uplink_for_area(long hnd,char *boss,char *point)
// ===============================================================
{
char	buf[MAX_BOSSLEN];
int		i=0;

	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,BOSSADDR,boss);
		if (strcmp(boss,gAreadef[hnd].uplink)==0)
		{
			get_token(buf,MAINAKA,point);
			return DB_OK;
		}
	}
	*boss=*point=0;
	return DB_NO_SUCH_BOSS;
}

// ===============================================================
// sets flag of existing new mails for area
	int  db_set_area_state(long hnd,int flag)
// ===============================================================
{
BOOL mod;

	mod=gAreadef[hnd].status & DB_AREA_MODIFIED;
	EnterCriticalSection(&AreaLockCS);
	if (!strcmp(gAreadef[hnd].echotag,BBSMAIL))	return 1;
	if (flag && !mod)
	{
		gAreadef[hnd].status |= DB_AREA_MODIFIED;
		LeaveCriticalSection(&AreaLockCS);
		return flush_areabase_to_disk();
	}
	else if (!flag && mod)
	{
		gAreadef[hnd].status &= ~DB_AREA_MODIFIED;
		LeaveCriticalSection(&AreaLockCS);
		return flush_areabase_to_disk();
	}
	LeaveCriticalSection(&AreaLockCS);
	return 1;
}

// ===============================================================
// returns area modified state
	int  db_get_area_state(long hnd)
// ===============================================================
{
	return (gAreadef[hnd].status & DB_AREA_MODIFIED);
}

// ==========================================
// returns the number of areas
	int db_get_count_of_areas(int *count)
// ==========================================
{
	if (!gAreadef)	db_open_area_descriptions(0);
	*count=num_of_areas;
	return DB_OK;
}

// ==========================================
// set Lock-flag for area
	int db_lock_area(int ind)
// ==========================================
{
	EnterCriticalSection(&AreaLockCS);
	if (ind >= num_of_areas)
	{
		LeaveCriticalSection(&AreaLockCS);
		return DB_NO_SUCH_INDEX;
	}
	if (gAreadef[ind].status & DB_AREA_LOCKED)
	{
		LeaveCriticalSection(&AreaLockCS);
		return DB_AREA_ALREADY_LOCKED;
	}
	gAreadef[ind].status |= DB_AREA_LOCKED;
	LeaveCriticalSection(&AreaLockCS);
	return DB_OK;
}

// ==========================================
// clear Lock-flag for area
	int db_unlock_area(int ind)
// ==========================================
{
	EnterCriticalSection(&AreaLockCS);
	if (ind >= num_of_areas)
	{
		LeaveCriticalSection(&AreaLockCS);
		return DB_NO_SUCH_INDEX;
	}
	gAreadef[ind].status &= ~DB_AREA_LOCKED;
	LeaveCriticalSection(&AreaLockCS);
	return DB_OK;
}

// **********************************************************
// 				Bosses functions
// **********************************************************
// ==========================================
	void db_open_boss_descriptions(int force)
// ==========================================
{
	if (Bosses.GetCount()==0 || force)	load_bosslist(Bosses);
}

// ==========================================
	int db_get_boss_by_index(long ind, char *dst)
// ==========================================
{
	if (!Bosses.GetCount())	load_bosslist(Bosses);
	if (ind >= Bosses.GetCount())	return FALSE;
	strcpy(dst,Bosses.GetString(ind));
	return TRUE;
}

// **********************************************************
// 				Areas functions
// **********************************************************

// ==========================================
	 int db_create_default_areas(void)
// ==========================================
{
areadef ad;
long   dm;
int    ret,val;

	db_get_count_of_areas(&val);
	if (val > 0)	return DB_OK;

	memset(&ad,0,sizeof(areadef));
	strcpy(ad.echotag,"NETMAIL");
	ad.index=0;
	ad.structlen=sizeof(areadef);
	strcpy(ad.description,L("S_357"));	// private mails
	ret=db_append_new_area(&ad,&dm);
	if (ret != DB_OK)	return ret;

	memset(&ad,0,sizeof(areadef));
	strcpy(ad.echotag,"LOCALMAIL");
	ad.index=1;
	ad.localmail=1;
	ad.structlen=sizeof(areadef);
	strcpy(ad.description,L("S_301"));	// local mail
	ret=db_append_new_area(&ad,&dm);
	return ret;
}

// ==========================================
	 int db_open_area_descriptions(int create)
// ==========================================
{
WIN32_FIND_DATA	finddata;
HANDLE  hf;
FILE 	*fp;
areadef ad,*pa;
int     size,killcount=0,index=0;
char	fname[MAX_PATH],tmp[100];

	if (gAreadef)   return 1;  // if opened

	InitializeCriticalSection(&AreaLockCS);
	InitializeCriticalSection(&MailAccessCS);
	InitializeCriticalSection(&WWDAccessCS);

// measure the arealist
	make_path(fname,gc.MsgBasePath,AREADBF);
	hf = FindFirstFile(fname,&finddata);
	FindClose(hf);
	if (hf==INVALID_HANDLE_VALUE)
	{
		if (!create)	return (DB_AREAFILE_NOT_FOUND);
		fp=fopen(fname,"wb");
		if (!fp)		return (DB_AREAFILE_NOT_FOUND);
		fclose(fp);
		size=0;
		num_of_areas=0;
	}
	else
	{
		size=finddata.nFileSizeLow;
		num_of_areas=size/sizeof(areadef);	  // number of structs
	}

// allocate memory for complete structure
   gAreadef=(areadef*)malloc(size);
   if (!gAreadef)		return (DB_OUT_OF_MEMORY);

   fp=fopen(fname,"rb");
   pa=gAreadef;
   for (int i=0;i<num_of_areas;i++)
   {
	   fread(&ad,sizeof(areadef),1,fp);
	   ad.status &= ~DB_AREA_LOCKED;	// clear lock-flag
	   if (ad.delflag)     // skip deleted area
	   {
		   killcount++;
		   sprintf(tmp,"%s.*",ad.filename);
		   make_path(fname,gc.MsgBasePath,tmp);
		   SafeFileDelete(fname);
	   }
	   else
	   {
	       ad.index=index++;
		   memcpy(pa,&ad,sizeof(ad));
		   pa++;
	   }
   }
   fclose(fp);
   num_of_areas-=killcount;

// if there are killed areas, then realloc
   if (killcount>0)
	   gAreadef=(areadef*) realloc(gAreadef,sizeof(areadef)*num_of_areas);

   // before saving we make quick sort
   db_sort_areas();
   db_init_create_date();
   flush_areabase_to_disk();	// backup wwd

   // check correctness of areadef (version?)
   if (num_of_areas>0 && gAreadef->structlen != sizeof(areadef))
		return (DB_WRONG_AREADDEF_VERSION);

   // initialize file handles
   pa=gAreadef;
   //int i;
   for (int i=0;i<num_of_areas;i++)
   {
	  pa->hheader =INVALID_HANDLE_VALUE;
	  pa->hmessage=INVALID_HANDLE_VALUE;
	  pa++;
   }
   return (DB_OK);
}

// ==========================================
	void db_close_area_descriptions(void)
// ==========================================
{
	ASSERT(gAreadef);
	flush_areabase_to_disk();
    db_close_all_areas();
	free(gAreadef);
	gAreadef=NULL;
	num_of_areas=0;
    DeleteCriticalSection(&AreaLockCS);
    DeleteCriticalSection(&MailAccessCS);
    DeleteCriticalSection(&WWDAccessCS);
}

// ==========================================
	int db_get_area_by_index(long index,pareadef pa)
// ==========================================
{
pareadef pad;

	ASSERT(pa && index>=0 && gAreadef);
	if (index >= num_of_areas)	return DB_NO_SUCH_INDEX;
	pad=&gAreadef[index];
	ASSERT(index==pad->index);
	memcpy(pa,pad,sizeof(areadef));
	return DB_OK;
}

// ==========================================
	int db_get_area_by_name(LPCSTR areatag,pareadef pa)
// ==========================================
// pointer to areadef may be NULL
{
	ASSERT(areatag && gAreadef);

	for (int ind=0;ind<num_of_areas;ind++)
	   if (!stricmp(areatag,gAreadef[ind].echotag))
	   {
		   if (pa)
			  memcpy(pa,&gAreadef[ind],sizeof(areadef)); // *pa=gAreadef[ind];
		   return ind;
	   }
	return -1;
}

// ==========================================
	int db_append_new_area(pareadef pa,long *new_handl,LPCSTR uplink,BOOL from_cfg)
// ==========================================
{
pareadef pad;
FILE	 *fp;
char 	 buf[MAX_BOSSLEN],buf1[MAX_BOSSLEN],path1[MAX_PATH],path2[MAX_PATH],desc[150];
int      ret,i,new_index=-1;
CString	 lookfor,bossaddr,point,fido,aka,name,sender,subject,mailtext;

	EnterCriticalSection(&WWDAccessCS);
	pa->status &= ~DB_AREA_MODIFIED;	// clear modification flag
	pa->status |= DB_CONVERTED;			// set new-message-format flag

// realloc the memory
	gAreadef=(pareadef)realloc(gAreadef,(num_of_areas+1)*sizeof(areadef));
	if (!gAreadef)
	{
		LeaveCriticalSection(&WWDAccessCS);
		return (DB_OUT_OF_MEMORY);
	}
	new_index=num_of_areas;
	pad=gAreadef+num_of_areas;

// append new area definition to internal memory structure
   memcpy(pad,pa,sizeof(areadef));
   pad->hheader=INVALID_HANDLE_VALUE;
   pad->hmessage=INVALID_HANDLE_VALUE;

// search free filename
   for(i=0;;i++)
   {
	   sprintf(buf,"%08d." EXT_HEADER,i);
	   make_path(path1,gc.MsgBasePath,buf);
	   if (_access(path1,0))
	   {
		   sprintf(buf,"%08d." EXT_MESSAGE,i);
		   make_path(path2,gc.MsgBasePath,buf);
		   if (_access(path2,0))	
		   {
			   fp=fopen(path1,"wb");
			   if (!fp)	return (DB_AREA_CANT_CREATE);
			   fclose(fp);
			   fp=fopen(path2,"wb");
			   if (!fp)	return (DB_AREA_CANT_CREATE);
			   fclose(fp);
			   sprintf(buf,"%08d",i);
			   break;
		   }
	   }
   }

   strcpy(pad->filename,buf);
   pad->index=new_index;
   pad->structlen=sizeof(areadef);

   db_find_area_description(pad->echotag,desc);
   if (*desc==0)
   {
		if (!stricmp("NETMAIL",pad->echotag))
			strcpy(pad->description,L("S_358"));	// netmail
		else if (!stricmp("LOCALMAIL",pad->echotag))
			strcpy(pad->description,L("S_124"));	// localmail
		else
			strcpy(pad->description,L("S_342"));	// no description
   }
   else
   {
	  OemToChar(desc,desc);
      strcpy(pad->description,desc);
   }

   if (!from_cfg)	// set autoclear params for autocreated areas
   {
		//if (strcpy(pad->group,ASAVED_GROUP))	// normal group?
	   if (strcpy(pad->group,DEFAULT_GROUP))
	   {
			pad->purge_age_create=get_cfg(CFG_COMMON,"DefAgeCreate",30);
			pad->purge_age_recipt=get_cfg(CFG_COMMON,"DefAgeRecipt",30);
			pad->purge_max_num=get_cfg(CFG_COMMON,"DefMaxMails",1000);
			pad->additional_days=get_cfg(CFG_COMMON,"DefDaysNonread",10);
		}
		else	// asaved
		{
			pad->purge_age_create=0;
			pad->purge_age_recipt=0;
			pad->purge_max_num=0;
			pad->additional_days=0;
		}

/*		if (strncmp(pad->echotag,"ASAVED",6)==0)
		{
			strcpy(pad->group,"ASAVED");
			pad->localmail=1;
		}
		else
			strcpy(pad->group,get_cfg(CFG_COMMON,"DefAreaGroup","FIDO"));*/

		if (gNewAreaFromExtSearch)
		{
			strcpy(pad->group,"EX_SEARCH");
			strcpy(pad->description,L("S_196"));
		}
   }

// set uplink and corresponing aka
   if (uplink)
   {
		strcpy(pad->uplink,uplink);
		lookfor=uplink;
		i=0;
		while (db_get_boss_by_index(i++,buf1))
		{
			get_token(buf1,BOSSADDR,bossaddr);
			if (bossaddr==lookfor)
			{
				get_token(buf1,MAINAKA,point);
				strcpy(pad->aka,point);
				break;
			}
		}
	}
	else	// set boss as uplink
	{
 	   if (db_get_boss_by_index(0,buf)==DB_OK)
	   {
			get_token(buf,BOSSADDR,fido);
			strcpy(pad->uplink,fido);
			get_token(buf,MAINAKA,aka);
			strcpy(pad->aka,aka);
	   }
	}
	num_of_areas++;
	*new_handl=new_index;
	LeaveCriticalSection(&WWDAccessCS);
	ret=flush_areabase_to_disk();

	if (!get_cfg(CFG_COMMON,"NoInfoNewArea",0))	// generate infomail?
	{
		db_open_simple(0);
		get_fullname(name);
		sender=L("S_178");
		subject=L("S_302");
		mailtext=L("S_161",pad->echotag);
		build_routed_netmail(sender,"",name,"",subject,mailtext,0,0,1);
		db_refresh_area_info(0);
	}
	if (UserdefAreaList.GetCount())
	{
		UserdefAreaList.AddTail(pad->echotag);
		UserdefAreaList.SaveToFile("udefdef.cfg");
	}
	return ret;
}

/*
	char aka[21];					// AKA
	char uplink[31];				// Uplink
	char group[11];				    // Group
	int  purge_age_create;          // Purge info
	int  purge_age_recipt;          // Purge info
	int  purge_max_num;             // Purge info
	int  purge_flag_create;         // Purge info
	int  purge_flag_age_recipt;     // Purge info
	int  purge_flag_nrmails;       	// Purge info
*/

// ==========================================
	int db_change_area_properties(pareadef pa)
// ==========================================
{
pareadef	pad;
int   		ret;

   ASSERT(pa && gAreadef);
   EnterCriticalSection(&WWDAccessCS);
   pad=gAreadef;
   for (int i=0;i<num_of_areas;i++)
   {
	   if (pad->index==pa->index)
	   {
		   pa->hheader =pad->hheader;
		   pa->hmessage=pad->hmessage;
		   memcpy(pad,pa,sizeof(areadef));
		   ret=flush_areabase_to_disk();
		   LeaveCriticalSection(&WWDAccessCS);
		   return ret;
	   }
	   pad++;
   }
   LeaveCriticalSection(&WWDAccessCS);
   return DB_AREA_INDEX_NOT_FOUND;
}

// **********************************************************
// 				Nun kommt das Message graffel
// **********************************************************

// ================================================================
// returns area handle by echotag
	int db_get_area_handle(LPCSTR echotag,long *pha,int create_if_new,LPCSTR uplink)
// ================================================================
{
pareadef 	pa;
areadef		ad;
char 			buf[300],fname[100];
long      new_handl=-1;
int 		i,ret;

	for (pa=gAreadef,i=0;i<num_of_areas;i++,pa++)
		if (stricmp(pa->echotag,echotag)==0)	goto area_found;
	if (!create_if_new)	return DB_AREA_NAME_NF;

	memset(&ad,0,sizeof(areadef));
	strcpy(ad.description,echotag);
	strcat(ad.description,L("S_3"));
	strcpy(ad.echotag,echotag);

	ret=db_append_new_area(&ad,&new_handl,uplink);
	if (ret != DB_OK)	return ret;
	pa=&gAreadef[new_handl];

area_found:
  EnterCriticalSection(&WWDAccessCS);
  if (pa->hheader==INVALID_HANDLE_VALUE)
  {
	  sprintf(fname,"%s.%s",pa->filename,EXT_HEADER);
	  make_path(buf,gc.MsgBasePath,fname);
		if (gc.ReadOnly)
			pa->hheader=CreateFile(buf,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
		else
			pa->hheader=CreateFile(buf,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,
				OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	  if (pa->hheader==INVALID_HANDLE_VALUE)	goto open_failed;
	}

  if (pa->hmessage==INVALID_HANDLE_VALUE)
  {
	  sprintf(fname,"%s.%s",pa->filename,EXT_MESSAGE);
	  make_path(buf,gc.MsgBasePath,fname);
		if (gc.ReadOnly)
			pa->hmessage=CreateFile(buf,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
		else
			pa->hmessage=CreateFile(buf,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
				OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
	  if (pa->hmessage==INVALID_HANDLE_VALUE)	goto open_failed;
  }
  *pha=pa->index;
	LeaveCriticalSection(&WWDAccessCS);
  return DB_OK;
open_failed:
	LeaveCriticalSection(&WWDAccessCS);
	return DB_FILE_OPEN_ERROR;
}

// ================================================================
	void db_close_all_areas(void)
// ================================================================
{
pareadef 	pa;

   for (int i=0;i<num_of_areas; i++)
   {
		pa=&gAreadef[i];
		ASSERT(i==pa->index);

	    if (pa->hheader!=INVALID_HANDLE_VALUE)
		{
		    CloseHandle(pa->hheader);
		    pa->hheader=INVALID_HANDLE_VALUE;
		}

	    if (pa->hmessage!=INVALID_HANDLE_VALUE)
		{
		    CloseHandle(pa->hmessage);
		    pa->hmessage=INVALID_HANDLE_VALUE;
		}
		pa++;
   }
}

// ================================================================
// writes mailheader to hdr-file and mailtext to mes-file
	int db_append_new_message(long hnd,pmailheader pmh)
// ================================================================
{
pareadef pa=&gAreadef[hnd];
DWORD 	 cnt;
int   	 version=1;
char		 area[20];

	if (gc.ReadOnly)	return DB_OK;
	if (pmh->structlen != sizeof(mailheader))	return DB_WRONG_MAILHEADER_SIZE;
	pmh->unixtime=parse_time(pmh->datetime);
	strncpy(area,pa->echotag,19);
	area[19]=0;
	pmh->subject[71]=0;
	EnterCriticalSection(&MailAccessCS);
	pmh->offset=SetFilePointer(pa->hmessage,0,NULL,FILE_END);
	if (!WriteFile(pa->hmessage,&magicid,sizeof(magicid),&cnt,NULL) || cnt != sizeof(magicid))
		goto msg_failed;
	if (!WriteFile(pa->hmessage,&version,sizeof(int),&cnt,NULL) || cnt != sizeof(int))
		goto msg_failed;
	if (!WriteFile(pa->hmessage,&area,sizeof(area),&cnt,NULL) || cnt != sizeof(area))
		goto msg_failed;
	pmh->index=SetFilePointer(pa->hheader,0,NULL,FILE_END) / sizeof(mailheader);
	if (!WriteFile(pa->hmessage,pmh,sizeof(mailheader),&cnt,0) || cnt != sizeof(mailheader))
		goto msg_failed;
	if (!WriteFile(pa->hmessage,pmh->mail_text,pmh->text_length,&cnt,0) || cnt != pmh->text_length)
		goto msg_failed;
	if (!WriteFile(pa->hheader,pmh,sizeof(mailheader),&cnt,NULL) || cnt != sizeof(mailheader))
		goto hdr_failed;
	return DB_OK;
msg_failed:
	LeaveCriticalSection(&MailAccessCS);
	return DB_MESSAGE_WRITE_FAILED;
hdr_failed:
	LeaveCriticalSection(&MailAccessCS);
	return DB_HEADER_WRITE_FAILED;
}

// ================================================================
	int db_get_mailtext(long hndl,pmailheader pmh,char *buf,long maxlen)
// ================================================================
// TOP: Diese funktioenchen holt von einer bestimmten Datenbank fuer einen
// existierenden pmaiheader den Text der Message
{
pareadef 	pa;
int    		ret;
long 		read_len;
DWORD		have_read;

   ASSERT (pmh && buf && hndl>=0 && hndl<num_of_areas);
   ASSERT (pmh->structlen==sizeof(mailheader));
   pa=&gAreadef[hndl];
   ASSERT(pa->index==hndl);
   ASSERT(pa->hheader!=INVALID_HANDLE_VALUE && pa->hmessage!=INVALID_HANDLE_VALUE);

   EnterCriticalSection(&MailAccessCS);

   int addoff=sizeof(magicid)+4+20+sizeof(mailheader);
   dret=SetFilePointer(pa->hmessage,pmh->offset+addoff,NULL,FILE_BEGIN);
   ASSERT(dret==(DWORD)pmh->offset+addoff);

   if (pmh->text_length <=maxlen)
	   read_len=pmh->text_length;
   else
	   read_len=maxlen;

   ret=ReadFile(pa->hmessage,buf,read_len,&have_read,NULL);
   buf[read_len-1]=0;	// vk: add -1 to prevent buffer oversizing
   LeaveCriticalSection(&MailAccessCS);
   if (!ret || have_read != (DWORD)read_len)	return (DB_MESSAGE_WRITE_FAILED);
   return DB_OK;
}

// ================================================================
	int db_getfirsthdr	(long hndl,pmailheader pmh)
// ================================================================
{
mailheader mh;
pareadef    pa;
int    		ret;
DWORD		have_read;

   ASSERT (pmh && hndl >= 0 && hndl < num_of_areas);
   pa=&gAreadef[hndl];
   ASSERT(pa->index==hndl);
   ASSERT(pa->hheader !=INVALID_HANDLE_VALUE);
   ASSERT(pa->hmessage!=INVALID_HANDLE_VALUE);

   EnterCriticalSection(&MailAccessCS);
   dret=SetFilePointer(pa->hheader,0,NULL,FILE_BEGIN);
   ASSERT(dret==0);

   while (1)
   {
	   ret=ReadFile(pa->hheader,&mh,sizeof(mailheader),&have_read,NULL);
	   if (!ret || have_read!=sizeof(mailheader))
	   {
	      LeaveCriticalSection(&MailAccessCS);
	      return (DB_HEADERFILE_EMPTY);
	   }

	   ASSERT(mh.structlen==sizeof(mailheader));
//changed, show also DELETED MAILS
//	   if (!(mh.status & DB_DELETED))
//	   {
	   	   memcpy(pmh,&mh,sizeof(mailheader));
		   LeaveCriticalSection(&MailAccessCS);
		   return DB_OK;
//	   }

   }
   ASSERT(0);
}

// ================================================================
// returns all Mailheaders of one Area
// ATTENTION: allocated Pointer ppmh must be freed at the end
	int db_getallheaders(long hndl,struct mailheader **ppmh,int *count,BOOL tzc)
// ================================================================
{
pareadef   pa;
mailheader *p;
_tzone		tz;
int    		ret,cnt,tzm;
DWORD			have_read,filesize,dummy;

	tzm=get_cfg(CFG_COMMON,"ProcTimezone",0);
	ASSERT (ppmh && *ppmh==0 && hndl>=0 && hndl<num_of_areas);
	pa=&gAreadef[hndl];
	ASSERT(pa->index==hndl && pa->hheader!=INVALID_HANDLE_VALUE && pa->hmessage!=INVALID_HANDLE_VALUE);
	EnterCriticalSection(&MailAccessCS);
	dret=SetFilePointer(pa->hheader,0,NULL,FILE_BEGIN);
	ASSERT(dret==0);
	filesize=GetFileSize(pa->hheader,&dummy);
	cnt=(filesize/sizeof(mailheader));
	ASSERT(cnt*sizeof(mailheader)==filesize);

	*count=cnt;
	*ppmh=(mailheader *)malloc(filesize+100);
	if (*ppmh==0)
	{
		LeaveCriticalSection(&MailAccessCS);
		ERR_MSG_RET0("E_MFOFHR");
	}

	ret=ReadFile(pa->hheader,*ppmh,filesize,&have_read,NULL);
	LeaveCriticalSection(&MailAccessCS);
	if (!ret || have_read!=filesize)	ERR_MSG_RET0("DB_FULLHEADERRFAILED");
// timezone corrections
	if (tzc)
	{
		p=*ppmh;
		for (UINT i=0;i<have_read/sizeof(mailheader);i++)
		{
			if (tzm==2 && get_timezone_inf(p->srczone,p->srcnet,p->srcnode,p->srcpoint,tz))
				p->unixtime+=tz.summer*3600;	// TZMode==2 - add timezone while building chains
			p++;
		}
	}
	return DB_OK;
}

// ================================================================
	int db_set_all_headers(long hnd,mailheader *pmh,int count)
// ================================================================
{
pareadef pa=&gAreadef[hnd];
DWORD		 cnt;

	if (gc.ReadOnly)	return DB_OK;
	EnterCriticalSection(&MailAccessCS);
	SetFilePointer(pa->hheader,0,NULL,FILE_BEGIN);
	if (!WriteFile(pa->hheader,pmh,count*sizeof(mailheader),&cnt,0) || cnt != count*sizeof(mailheader))
	{
		LeaveCriticalSection(&MailAccessCS);
		return DB_HEADER_WRITE_FAILED;
	}
	LeaveCriticalSection(&MailAccessCS);
	return DB_OK;
}

// ================================================================
	int db_getnexthdr(long hnd,pmailheader pmh)
// ================================================================
{
mailheader mh;
pareadef	 pa=&gAreadef[hnd];
DWORD			 cnt;

	EnterCriticalSection(&MailAccessCS);
	if (!ReadFile(pa->hheader,&mh,sizeof(mailheader),&cnt,0) || (cnt != sizeof(mailheader)))
	{
		LeaveCriticalSection(&MailAccessCS);
		return DB_HEADERFILE_EMPTY;
	}
	memcpy(pmh,&mh,sizeof(mailheader));
	LeaveCriticalSection(&MailAccessCS);
	return DB_OK;
}

// ================================================================
	int db_gethdr_by_index(long hnd,long index,pmailheader pmh)
// ================================================================
{
mailheader mh;
pareadef   pa=&gAreadef[hnd];
DWORD			 cnt;

	EnterCriticalSection(&MailAccessCS);
	SetFilePointer(pa->hheader,index * sizeof(mailheader),0,FILE_BEGIN);
	if (!ReadFile(pa->hheader,&mh,sizeof(mailheader),&cnt,0) || cnt != sizeof(mailheader))
	{
		LeaveCriticalSection(&MailAccessCS);
		return DB_HEADER_READ_FAILED;
	}
	memcpy(pmh,&mh,sizeof(mailheader));
	LeaveCriticalSection(&MailAccessCS);
	return DB_OK;
}

// ================================================================
	int db_sethdr_by_index(long hnd,long index,pmailheader pmh,int dont_update_time)
// ================================================================
{
pareadef pa=&gAreadef[hnd];
DWORD		 cnt;

	if (gc.ReadOnly)	return DB_OK;
	if (!dont_update_time)	pmh->unixtime=parse_time(pmh->datetime);
	EnterCriticalSection(&MailAccessCS);
	SetFilePointer(pa->hheader,index * sizeof(mailheader),0,FILE_BEGIN);
	if (!WriteFile(pa->hheader,pmh,sizeof(mailheader),&cnt,0) || cnt != sizeof(mailheader))
	{
		LeaveCriticalSection(&MailAccessCS);
		return DB_HEADER_WRITE_FAILED;
	}
	LeaveCriticalSection(&MailAccessCS);
	return DB_OK;
}

// ================================================================
	int db_setnew_mailtext(long hnd,long index,pmailheader pmh)
// ================================================================
{
pareadef pa=&gAreadef[hnd];
DWORD		 cnt;
int			 version=1;

	if (gc.ReadOnly)	return DB_OK;
	EnterCriticalSection(&MailAccessCS);
	pa->echotag[19]=0;
	pmh->unixtime=parse_time(pmh->datetime);
	pmh->index=index;
	pmh->offset=SetFilePointer(pa->hmessage,0,NULL,FILE_END);
	if (!WriteFile(pa->hmessage,&magicid,sizeof(magicid),&cnt,0) || cnt != sizeof(magicid))   
		goto msg_failed;
	if (!WriteFile(pa->hmessage,&version,sizeof(int),&cnt,0) || cnt != sizeof(int))   
		goto msg_failed;
	if (!WriteFile(pa->hmessage,&pa->echotag,sizeof(pa->echotag),&cnt,0) || cnt != sizeof(pa->echotag))   
		goto msg_failed;
	if (!WriteFile(pa->hmessage,pmh,sizeof(mailheader),&cnt,0) || cnt != sizeof(mailheader))
		goto msg_failed;
	if (!WriteFile(pa->hmessage,pmh->mail_text,pmh->text_length,&cnt,0) || cnt != pmh->text_length)
		goto msg_failed;
	SetFilePointer(pa->hheader,index * sizeof(mailheader),NULL,FILE_BEGIN);
	if (!WriteFile(pa->hheader,pmh,sizeof(mailheader),&cnt,0) || cnt != sizeof(mailheader))
		goto hdr_failed;
	LeaveCriticalSection(&MailAccessCS);
	return DB_OK;
msg_failed:
	LeaveCriticalSection(&MailAccessCS);
	return DB_MESSAGE_WRITE_FAILED;
hdr_failed:
	LeaveCriticalSection(&MailAccessCS);
	return DB_HEADER_WRITE_FAILED;
}

// ================================================================
	int db_purge_area(long hnd)
// ================================================================
{
mailheader	mh;
pareadef 	pa=&gAreadef[hnd];
areadef		atmp;
FILE 		*fp2header,*fp2message;
CString		help;
char		fname[MAX_PATH],buf[300],*p,*pp;
long   		xhandle,index=0;
DWORD       have_read;
int    		ret,size,size1,already_removed,countmail=0;
BOOL		bDeleteSeenby;
ULONG		hdrsize=sizeof(mailheader);
ULONG		add2read=sizeof(magicid)+4+20+hdrsize;	// length of file header - offset to mailtext

	if (gc.ReadOnly)	return DB_OK;
	bDeleteSeenby=get_cfg(CFG_PURGER,"DeleteSeenBy",0);
	bPurgeScanned=get_cfg(CFG_COMMON,"PurgeScanned",0);

	if (pa->hheader==INVALID_HANDLE_VALUE)
	{
		db_get_area_by_index(hnd,&atmp);
		db_get_area_handle(atmp.echotag,&xhandle,0);
	}

	p=(char *)malloc(MAX_MSG+100+10000);   // for Add Info
	if (!p)	return DB_OUT_OF_MEMORY;

	make_path(fname,gc.MsgBasePath,TMP_HDR_FILE);	// create temp hdr-file
	fp2header=fopen(fname,"wb");
	if (!fp2header)
	{
		free(p);
		err_out("E_CANCRPTMPF",fname);
		return DB_CANNOT_CREATE_TEMP;
	}

	make_path(fname,gc.MsgBasePath,TMP_MSG_FILE);	// create temp msg-file
	fp2message=fopen(fname,"wb");
	if (!fp2message)
	{
		free(p);
		fclose(fp2header);
		err_out("E_CANCRPTMPF",fname);
		return DB_CANNOT_CREATE_TEMP;
	}

	SetFilePointer(pa->hheader,0,NULL,FILE_BEGIN);
	already_removed=0;
	size=GetFileSize(pa->hheader,0)/hdrsize;
	gc.purger.mails_total+=size;
	size1=GetFileSize(pa->hmessage,0)+GetFileSize(pa->hheader,0);
	gc.purger.bytes_total+=size1;

	countmail=0;
	while(ReadFile(pa->hheader,&mh,hdrsize,&have_read,NULL))
	{
		countmail++;
		if (have_read!=hdrsize)	break;
		if (should_save_this_message(&mh,pa,size,already_removed))
		{
			SetFilePointer(pa->hmessage,mh.offset,NULL,FILE_BEGIN);
			if (mh.text_length > MAX_MSG)	mh.text_length = MAX_MSG;
			ReadFile(pa->hmessage,p,add2read+mh.text_length,&have_read,NULL);
			if (bDeleteSeenby)
			{
				pp=strstr(p+add2read,"\rSEEN-BY: ");
				if (pp)
				{
					*(pp+1)=0;
					gc.purger.bytes_removed+=mh.text_length;
					mh.text_length=pp-(p+add2read)+2;
					gc.purger.bytes_removed-=mh.text_length;
				}
			}

			mh.offset=ftell(fp2message);	// Store new offset
			mh.index=index++;
			mh.subject[71]=0;
			mh.unixtime=parse_time(mh.datetime);

			char *pxxx;
			pxxx=p+sizeof(magicid)+4+20;
			memcpy(pxxx,&mh,hdrsize);

			if (fwrite(p,add2read+mh.text_length,1,fp2message) != 1)
			{
				free(p);
				fclose(fp2header);
				fclose(fp2message);
				err_out("E_WRTPTMPF");
				return DB_WRITE_ERROR_TEMP_FILE;
			}
			if (fwrite(&mh,hdrsize,1,fp2header) != 1)
			{
				free(p);
				fclose(fp2header);
				fclose(fp2message);
				err_out("E_WRTPTMPF");
				return DB_WRITE_ERROR_TEMP_FILE;
			}
		}
		else
		{
			gc.purger.mails_removed++;
			gc.purger.bytes_removed+=mh.text_length+hdrsize;
			already_removed++;
		}
  }
  free(p);
  fclose(fp2header);
  fclose(fp2message);
  CloseHandle(pa->hheader);
  CloseHandle(pa->hmessage);

// save header file
  sprintf(buf,"%s\\%s.%s",gc.MsgBasePath,pa->filename,EXT_HEADER);
  unlink(buf);
  if (!access(buf,0))
  {
		err_out("E_CANDELFL",buf);
		return DB_CANNOT_DELETE_HEADER;
  }
  make_path(fname,gc.MsgBasePath,TMP_HDR_FILE);
  if (ret=rename(fname,buf))
  {
	  err_out("E_CANREN",fname,buf,ret);
	  return DB_CANNOT_RENAME_HEADER;
  }
  pa->hheader=CreateFile(buf,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
  if (pa->hheader==INVALID_HANDLE_VALUE)	return DB_FILE_OPEN_ERROR;

// save message file
  sprintf(buf,"%s\\%s.%s",gc.MsgBasePath,pa->filename,EXT_MESSAGE);
  unlink(buf);
  if (access(buf,0)==0)
  {
		err_out("E_CANDELFL",buf);
		return (DB_CANNOT_DELETE_MESSAGE);
  }
  make_path(fname,gc.MsgBasePath,TMP_MSG_FILE);
  if (rename(fname,buf))
  {
		err_out("E_CANREN",fname,buf,ret);
		return (DB_CANNOT_RENAME_MESSAGE);
	}
	pa->hmessage=CreateFile(buf,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,NULL);

	if (pa->hmessage==INVALID_HANDLE_VALUE)
	{
		err_out("E_ECOTF",buf);
		return DB_FILE_OPEN_ERROR;
	}
	pa->status &= ~DB_MARKED_FOR_PURGE;
	return flush_areabase_to_disk();
}

// =============================================================================
	int should_save_this_message (mailheader *mh,areadef *pa,int total,int already_removed)
// =============================================================================
{
time_t ltime,mtime,secs;

	if (mh->status & DB_DELETED)			return 0;
	if (mh->status & DB_NEVER_DELETE)	return 1;
	if ((mh->status & DB_MAIL_CREATED) && !(mh->status & DB_MAIL_SCANNED))	return 1;
	if ((mh->status & DB_MAIL_SCANNED) && bPurgeScanned)		return 0;
	if (pa->purge_max_num>0 && (total - already_removed)>pa->purge_max_num)	return 0;
	if (pa->purge_age_create > 0)
	{
		time(&ltime);
		mtime=parse_time(mh->datetime);
		secs=pa->purge_age_create * 86400;
		if (!(mh->status & DB_MAIL_READ))	secs+=pa->additional_days * 86400;
		if (difftime(ltime,mtime) > secs)	return 0;
	}

	if (pa->purge_age_recipt > 0)
	{
		time(&ltime);
		secs=pa->purge_age_recipt * 86400;
		if (!(mh->status & DB_MAIL_READ))	secs+=pa->additional_days * 86400;
		if (difftime(ltime,mh->recipttime) > secs)	return 0;
	}
	return 1;
}

// ================================================================
	int db_delete_message(long hnd, pmailheader pmh)
// ================================================================
{
mailheader mh;
pareadef	 pa=&gAreadef[hnd];
DWORD      cnt;

	if (gc.ReadOnly)	return DB_OK;
  EnterCriticalSection(&MailAccessCS);
  SetFilePointer(pa->hheader,pmh->index*sizeof(mailheader),NULL,FILE_BEGIN);
  if (!ReadFile(pa->hheader,&mh,sizeof(mailheader),&cnt,NULL) || cnt != sizeof(mailheader))
  {
		LeaveCriticalSection(&MailAccessCS);
		return DB_HEADERR_ON_DELETE;
  }
  mh.status |= DB_DELETED;
  SetFilePointer(pa->hheader,pmh->index*sizeof(mailheader),NULL,FILE_BEGIN);
  if (!WriteFile(pa->hheader,&mh,sizeof(mailheader),&cnt,NULL) || cnt != sizeof(mailheader))
  {
		LeaveCriticalSection(&MailAccessCS);
		return DB_WRITE_ERROR_ON_HDR;
  }
  FlushFileBuffers(pa->hheader);
  LeaveCriticalSection(&MailAccessCS);
  if ((pa->status & DB_MARKED_FOR_PURGE)==0)
  {
	  pa->status |= DB_MARKED_FOR_PURGE;
	  return flush_areabase_to_disk();
  }
  return DB_OK;
}

// ======================================
	int flush_areabase_to_disk(void)
// ======================================
{
pareadef	pa;
char		path1[MAX_PATH],path2[MAX_PATH];
FILE		*fp;
int			ret;

	if (gc.ReadOnly)	return DB_OK;
	EnterCriticalSection(&WWDAccessCS);
	backup_wwd(AREADBF3,AREADBF4);
	backup_wwd(AREADBF2,AREADBF3);
	backup_wwd(AREADBF1,AREADBF2);
	make_path(path1,gc.MsgBasePath,AREADBF);
	make_path(path2,gc.MsgBasePath,AREABACKUP"\\" AREADBF1);
	CopyFile(path1,path2,0);
	fp=fopen(path1,"w+b");
	pa=gAreadef;
	for (int i=0;i<num_of_areas;i++)
	{
		pa->affe1=magicid.affe1;
		pa->affe2=magicid.affe2;
		pa->count1=magicid.count1;
		pa->count2=magicid.count2;
		pa++;
	}
	ret=fwrite(gAreadef,sizeof(areadef),num_of_areas,fp);
	fflush(fp);
	fclose(fp);
	LeaveCriticalSection(&WWDAccessCS);
	return (ret==num_of_areas ? DB_OK : DB_WRITE_AREA_FAILED);
}

// ======================================
// creates name for new area
	int create_new_areaname(LPSTR echotag)
// ======================================
{
pareadef pa;
char buf[10];
int  j=0;

	while (j<MAX_AREAS)
	{
		sprintf(buf,"%08d",j++);
		pa=gAreadef;
		for (int i=0;i<area_search_amount;i++)
		{
			if (!stricmp(buf,pa->filename))	goto nextname;
			pa++;
		}
		strcpy(echotag,buf);
		return 1;
nextname:;
	}
	return 0;
}

// =============================================================================
// counts of all messages in area and already read messages
	int db_get_area_counters(int hnd,int *allcnt,int *readcnt)
// =============================================================================
{
mailheader mh;
pareadef	 pa=&gAreadef[hnd];
DWORD			 cnt;
long			 oldpos;

	*allcnt=*readcnt=0;
  db_open_simple(hnd);
	oldpos=SetFilePointer(pa->hheader,0,0,FILE_CURRENT);
	SetFilePointer(pa->hheader,0,0,FILE_BEGIN);
	while (1)
	{
		if (!ReadFile(pa->hheader,&mh,sizeof(mh),&cnt,NULL) || cnt != sizeof(mh))	break;
		if (mh.status & DB_MAIL_READ)	(*readcnt)++;
		(*allcnt)++;
	}
	SetFilePointer(pa->hheader,oldpos,0,FILE_BEGIN);
	return DB_OK;
}

// =============================================================================
// refreshs amount and read mails counters in all areas
	int db_refresh_areas_info(void)
// =============================================================================
{
pareadef pa;
int		 areacnt,allcnt,readcnt;

	db_get_count_of_areas(&areacnt);
	for (int ind=0;ind<areacnt;ind++)
	{
		allcnt=readcnt=0;
		db_get_area_counters(ind,&allcnt,&readcnt);
		EnterCriticalSection(&AreaLockCS);
		pa=&gAreadef[ind];
		pa->number_of_mails=allcnt;
		pa->number_of_read=readcnt;
		LeaveCriticalSection(&AreaLockCS);
	}
	return flush_areabase_to_disk();
}

// =============================================================================
// refreshs amount and read mails counters in one area
	int db_refresh_area_info(long hnd)
// =============================================================================
{
int	allcnt,readcnt;

	allcnt=readcnt=0;
	db_get_area_counters(hnd,&allcnt,&readcnt);
	EnterCriticalSection(&AreaLockCS);
	gAreadef[hnd].number_of_mails=allcnt;
	gAreadef[hnd].number_of_read=readcnt;
	LeaveCriticalSection(&AreaLockCS);
	return flush_areabase_to_disk();
}

// =================================================================
	int delayed_purge_on_exit(void)
// =================================================================
{
int	allcnt,readcnt,da;

	for (int i=0;i<100;i++)
	{
		da=DelayedAreas[i];
		if (da == -1)
		{
		   flush_areabase_to_disk();
		   return 1;
		}
		db_purge_area(da);
		allcnt=readcnt=0;
		db_get_area_counters(da,&allcnt,&readcnt);
		gAreadef[da].number_of_mails=allcnt;
		gAreadef[da].number_of_read=readcnt;
	}
	return 0;
}

// ===============================================================
// increment counter of already read messages in area
	void  db_inc_read_counter(long hnd,int num)
// ===============================================================
{
   EnterCriticalSection(&AreaLockCS);
   gAreadef[hnd].number_of_read += num;
   LeaveCriticalSection(&AreaLockCS);
}

// ===============================================================
// mark area as all messages were read
	void db_set_area_to_read(long hnd)
// ===============================================================
{
   EnterCriticalSection(&AreaLockCS);
   gAreadef[hnd].number_of_read=gAreadef[hnd].number_of_mails;
   LeaveCriticalSection(&AreaLockCS);
}

// ===============================================================
// set localmail status flag for area
	void db_set_area_to_carboncopy(long hnd)
// ===============================================================
{
   EnterCriticalSection(&AreaLockCS);
   gAreadef[hnd].localmail=1;
   LeaveCriticalSection(&AreaLockCS);
}

// ===============================================================
  int db_open_simple(long hnd)
// ===============================================================
{
pareadef pa=&gAreadef[hnd];
char	 buf[MAX_PATH];

	EnterCriticalSection(&WWDAccessCS);
	if (pa->hheader==INVALID_HANDLE_VALUE)
	{
		sprintf(buf,"%s\\%s.%s",gc.MsgBasePath,pa->filename,EXT_HEADER);
		if (gc.ReadOnly)
			pa->hheader=CreateFile(buf,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
		else
			pa->hheader=CreateFile(buf,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,
				OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
		if (pa->hheader==INVALID_HANDLE_VALUE)
		{
			LeaveCriticalSection(&WWDAccessCS);
			return DB_FILE_OPEN_ERROR;
		}
	}
	if (pa->hmessage==INVALID_HANDLE_VALUE)
	{
		sprintf(buf,"%s\\%s.%s",gc.MsgBasePath,pa->filename,EXT_MESSAGE);
		if (gc.ReadOnly)
			pa->hmessage=CreateFile(buf,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,NULL);
		else
			pa->hmessage=CreateFile(buf,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,
				OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,NULL);
		if (pa->hmessage==INVALID_HANDLE_VALUE)
		{
			LeaveCriticalSection(&WWDAccessCS);
			return DB_FILE_OPEN_ERROR;
		}
	}
	LeaveCriticalSection(&WWDAccessCS);
	return DB_OK;
}

// ======================================================================================
// searches area description in echofiles (max length=128 chars)
	int db_find_area_description(LPCSTR echotag,LPSTR desc)
// ======================================================================================
{
CString fname;
FILE	*fp;
char	buf[MAX_BOSSLEN],line[300],tmp[300],fpath[MAX_PATH],*p,*t;
int		i=0;

	*desc=0;
	if (*echotag==0)	return 0;
	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,ECHOFILE,fname);
		make_path(fpath,gc.UtilPath,fname);
		if (fp=fopen(fpath,"rt"))
		{
		   while (fgets(line,299,fp))
		   {
				trim_all(line);
				p=strchr(line,',');
				if (!p)
					p=strchr(line,'\t');
				else
				{
					t=strchr(line,'\t');
					if (t && t<p)	p=t;
				}
				if (!p)
					p=strchr(line,' ');
				else
				{
					t=strchr(line,' ');
					if (t && t<p)	p=t;
				}
				if (p)
				{
					*p=0;
					if (!stricmp(line,echotag))
					{
						strcpy(tmp,p+1);
						i=strspn(tmp," .,\"\t");
						if (i)	strcpy(tmp,tmp+i);
						i=strcspn(tmp,"\"\r\n");
						if (i)	tmp[i]=0;
						strncpy(desc,tmp,128);
						desc[128]=0;
						fclose(fp);
						return 1;
					}
				}
		   }
		   fclose(fp);
		}
	}
	return 0;
}

// ======================================================================================
// sort Areas in alphabetical order
	void  db_sort_areas(void)
// ======================================================================================
// NETMAIL is first, LOCALMAIL is second, and ASAVED comes at the end
{
	if (num_of_areas>2)	// sort areas excluding NETMAIL & LOCALMAIL
		qsort((void*)(gAreadef+2),num_of_areas-2,sizeof(areadef),SortAreaCallback);

// reindex areas
	for (int i=0;i<num_of_areas;i++)
	{
	    gAreadef[i].index=i;
		gAreadef[i].udefindex=-1;
	}
}

//===================================================
// compares areatags, ASAVED areas are last in order
	int SortAreaCallback(const void *a1,const void *a2)
//===================================================
{
areadef *p1,*p2;
int    g1,g2;

	p1=(areadef *)a1;
	p2=(areadef *)a2;
	if (!stricmp(p1->group,p2->group))	// sort in one group
		return strcmp(p1->echotag,p2->echotag);
	g1=stricmp(p1->group,ASAVED_GROUP);
	g2=stricmp(p2->group,ASAVED_GROUP);
	if (g1==0)		// first is ASAVED
		return -1;
	else if (g2==0)	// first is ASAVED
		return 1;
	else			// none is ASAVED
		return stricmp(p1->group,p2->group);
}

// ======================================================================================
// initialize creation date
	void db_init_create_date(void)
// ======================================================================================
{
	for (int i=0;i<num_of_areas;i++)
	    if (gAreadef[i].area_creation_date==0)
		    gAreadef[i].area_creation_date=time(NULL);
}

// ======================================================================================
	int db_get_boss_flags(const char *addr,int *nopoll,int *nohydra,int *nozmod,int *tcpip)
// ======================================================================================
{
CString fido;
char	buf[MAX_BOSSLEN];
int		i=0;

	*nopoll=*nohydra=*nozmod=*tcpip=0;
 	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,BOSSADDR,fido);
		if (fido==addr)
		{
			*nopoll=get_token_int(buf,NOPOLL);
			*nohydra=get_token_int(buf,NOHYDRA);
			*nozmod=get_token_int(buf,NOZMODEM);
			*tcpip=get_token_int(buf,TCPIP);
			return 1;
		}
	}
	return 0;
}

// ==========================================
	void db_flush_area(void)
// ==========================================
{
   EnterCriticalSection(&WWDAccessCS);
   for (int i=0;i<num_of_areas;i++)
   {
		if (gAreadef[i].hheader != INVALID_HANDLE_VALUE)
			FlushFileBuffers(gAreadef[i].hheader);
		if (gAreadef[i].hmessage != INVALID_HANDLE_VALUE)
			FlushFileBuffers(gAreadef[i].hmessage);
   }
   LeaveCriticalSection(&WWDAccessCS);
}

// ==========================================
	void db_mark_all_areas_as_read(void)
// ==========================================
{
mailheader *mh,*mhw;
int			count=0;

	mh=0;
	EnterCriticalSection(&WWDAccessCS);
	for (int i=0;i<num_of_areas;i++)
	{
		db_open_simple (i);
		if (!db_getallheaders(i,&mh,&count))	continue;
		mhw=mh;
		for (int k=0;k<count;k++)
		{
			mhw->status |= DB_MAIL_READ;
			mhw++;
		}
		db_set_all_headers (i,mh,count);
		if (mh)
		{
			free(mh);
			mh=0;
		}
		db_set_area_to_read(i);
	}
	LeaveCriticalSection(&WWDAccessCS);
	db_flush_area();
}

// ===========================================================
	int  MailheaderSortFunc (const void *elem1,const void *elem2)
// ===========================================================
{
mailheader *m1=(mailheader *) elem1;
mailheader *m2=(mailheader *) elem2;
char   *p1=m1->subject;
char   *p2=m2->subject;
int    ret=1;

	if (gc.SortCriteria & SORT_RECIEVE)
		ret=m1->recipttime-m2->recipttime;
	else if (gc.SortCriteria & SORT_CREATE)
		ret=m1->unixtime-m2->unixtime;
	else if (gc.SortCriteria & SORT_FROM)
		ret=stricmp(m1->fromname,m2->fromname);
	else if (gc.SortCriteria & SORT_TO)
		ret=stricmp(m1->toname,m2->toname);
	else if (gc.SortCriteria & SORT_SUBJECT)
	{
		if (!strnicmp(p1,"Re:",3))	p1+=3;
		while (*p1 && *p1==' ')		p1++;
		if (!strnicmp(p2,"Re:",3))	p2+=3;
		while (*p2 && *p2==' ')		p2++;
		ret=stricmp(p1,p2);
	}
	else if (gc.SortCriteria & SORT_STATUS)
		ret=m1->status-m2->status;
	if (ret==0)	ret=m1->unixtime-m2->unixtime;	// additional sort for equal values
	if (gc.SortCriteria & SORT_ORDER)
		return ret<0 ? 1 : ret>0 ? -1 : 0;
	else
		return ret>0 ? 1 : ret<0 ? -1 : 0;
	return ret;
}

// ===========================================================
	void db_sort_area(int first,int last)
// ===========================================================
{
mailheader *mh;
int	   count=0;

	mh=0;
	EnterCriticalSection(&WWDAccessCS);
	for (int i=first;i<=last;i++)
	{
		db_open_simple(i);
		if (!db_getallheaders(i,&mh,&count))	continue;

		qsort((void *)mh,count,sizeof(mailheader),MailheaderSortFunc);
		for (int k=0;k<count;k++)
			(mh+k)->index = k;
		db_set_all_headers (i,mh,count);
		if (mh)
		{
			free(mh);
			mh=0;
		}
	}
	LeaveCriticalSection(&WWDAccessCS);
	db_flush_area();
}

// ===================================================
	void backup_wwd(LPCSTR nold,LPCSTR nnew,BOOL useroot)
// ===================================================
{
char fpath[MAX_PATH],fold[MAX_PATH],fnew[MAX_PATH];

	make_path(fpath,gc.MsgBasePath,AREABACKUP);
	_mkdir(fpath);
	make_path(fold,useroot ? gc.MsgBasePath : fpath,nold);
	make_path(fnew,fpath,nnew);
	CopyFile(fold,fnew,0);
}
