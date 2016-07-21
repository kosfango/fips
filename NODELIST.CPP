//
//############ The whole Nodelist Stuff ########################
//

#include "stdafx.h"
#include <io.h>
#include "supercom.h"

// NLType Synatx:	0	Normal Nodelist
//					1	Points24 Pointlist
//					2	Points4D Pointlist
//					3	Yucknet

#define NODELIST	0
#define	POINTS24	1
#define	POINTS4D	2
#define	YUCKNET		3

//-------------------------------------------
//				Prototypes
//-------------------------------------------

int check_exclude		(int zone, int net, int node, int point);
int analyse_nodelist	(char *,struct _nl_idx *);
int get_newest_file		(char *nl,char *filename);
void parse_nodelist_line(char *input,char *w1,char *w2,char *w3,char *w4,char *w5,char *w6);

int compare_FN			(const void *aa1,const void *aa2);	// werden fuer qsort gebraucht
int compare_FU			(const void *aa1,const void * aa2);
int compare_nl_rec_temp	(const void *aa1,const void *aa2);

void NLGetAccess		(void);	// critical section ein/aus
void NLFreeAccess		(void);
void NLErrormsg			(char *);

int expand_from_fn		(struct _fn_rec, struct _fidonumber*); // holt sich aus der nodeliste die zeile
int expand_from_fu		(struct _fu_rec, struct _fidonumber*); // holt sich aus der nodeliste die zeile

int add_node_points24	(char*); // je nach nodelistentyp wird die zeile verarbeitet
int add_node_points4d	(char*);
int add_node_yucknet	(char*);
int add_node_nodelist	(char*);

//-------------------------------------------
//				Global Variables
//-------------------------------------------

static CStrList	nodel;          // Nodelist-Config
static char		mybuf[2048];	// tmp-buffer
static char		input[1024];

static _nl_idx	nlidx[MAX_NODELISTS];		// List der zu bearbeitenden Nodelisten
static int		maxnlidx=0;		// counter of Nodelist entries in List
static int		retval;			// return-value

extern CRITICAL_SECTION NodeListAcces;
extern _gconfig gc;

static int	NLInitialized=0;

static char	t1[100]; // beim nl-compile stehen hier die worte einer zeile
static char	t2[100];
static char	t3[100];
static char	t4[100];
static char	t5[100];
static char	t6[100];

static _fn_rec fn;		// globale recs fuer jeden eintrag
static _fu_rec fu;
static int act_offset;	// zeigt auf den anfang jeder Zeile
static CStrList nl_list;

//====================================================================
void do_crc16(char p,unsigned short *crc)
//====================================================================
{
//unsigned short divisor = 0x1021;
unsigned short temp;
int t;

	temp = *crc;
	for (t=0;t<8;t++)
	{
		if ((temp & 0x8000) == 0x8000)
			temp = (temp << 1) ^ 0x1021; // divisor;
		else
			temp = temp << 1;

		temp = temp ^ p;
	}
	*crc = temp;
}

//====================================================================
	int check_crc(char *fname)
//====================================================================
{
USHORT	crc16;
int		t,i,k,filesize,nlcrc;
FILE	*fp;
char	*firstline,*p,*s,*buf,ext[4];

	fp = fopen(fname,"rb");
	if (!fp)
	{
		nl_add_listbox(L("S_203",fname));	// Error opening file %s
		return 0;
	}
	filesize=_filelength(_fileno(fp));
	p=strrchr(fname,'.');
	strcpy(ext,p+1);
	nl_add_listbox(L("S_250",fname));	// File: %s

	buf=(char*)malloc(filesize+128);
	if (!buf)
	{
		nl_add_listbox(L("S_345"));	// Not enough memory for this operation
		fclose(fp);
		return 0;
	}

	t=fread(buf,1,filesize,fp);
	fclose(fp);
	firstline=buf;
	p=buf;
	i=0;

	while (*p && *p != '\r')
	{
		p++;
		i++;
	}
	*p=0;

	p++;
	i++;

	while (*p && (*p == '\n' || *p == '\r'  ))
	{
		p++;
		i++;
	}

	t = filesize-i-1;
	if (t>0 && p[t] == 0x1a)
			t--; // strip ending EOF

	crc16=0;

	for (k=0;k<=t;k++)
		Do_CRC((short*)&crc16,p[k]);

	nl_add_listbox(L("S_109",crc16,crc16));	// Calculated CRC16: %d (0x%X)

	nlcrc=0;
	s=strstr(firstline,ext);
	if (s)
	{
		p=strchr(s,':');
		if (p)
		{
			p++;
			sscanf(p,"%d",&nlcrc);
		}
	}
	free(buf);
	if (nlcrc==0)
	{
		nl_add_listbox(L("S_120"));	// Cannot get CRC16 checksum from nodelist file
		nl_add_listbox("");
		return 0;
	}

	if ((unsigned short)nlcrc != crc16)
	{
		nl_add_listbox(L("S_327",nlcrc,nlcrc));	// NL-CRC16: %d (0x%X)
		nl_add_listbox(L("S_143"));	// CRC16 check failed
		nl_add_listbox("");
		return 0;
	}
	else
	{
		nl_add_listbox(L("S_144"));	// CRC16 check successfull
		nl_add_listbox("");
	}
	return 1;
}


//====================================================================
	int check_nodelists(void)
//====================================================================
{
CString nldesc,nname,dname;
char	sbuf[MAX_PATH],fname[MAX_PATH];

	nodel.LoadFromFile("NODELIST.CFG");
	if (nodel.GetCount()<1)
	{
		nl_add_listbox(L("S_71"));	// >> No Nodelists defined
		return 0;
	}

	for (int t=0;t<nodel.GetCount();t++)
	{
		get_token(nodel.GetString(t),0,nldesc);
		get_token(nodel.GetString(t),1,nname);
		get_token(nodel.GetString(t),2,dname);

		nl_add_listbox(L("S_133",nldesc));	// Checking %s
		make_path(sbuf,gc.NodelistPath,nname);

		if (!get_newest_file(sbuf,fname))
			nl_add_listbox(L("S_332",sbuf));	// No file found matching %s.*
		else // check this nodelist
			check_crc(fname);
	}
	return 1;
}

//====================================================================
	int nl_compile(void)
//====================================================================
{
int		t;
FILE	*fp;
_fn_rec *fnr;
_fu_rec *fur;
int		aktnl=0;
CStrList addnodes;

_fidonumber rec; // for additional nodes
FILE	*afp;
char	tmp[300],tmp2[300],tmp3[300];

	check_exclude(0,0,0,0); // init exclude list

	nl_add_listbox(L("S_413"));	// Starting Nodelist-Compile
	nl_add_listbox("=======================================");

	nl_list.RemoveAll();
	nodediff_merge();

	fur=(_fu_rec*)malloc(sizeof(_fu_rec)*MAX_NL_ENTRIES);
	if (!fur)
		ERR_MSG_RET0("E_MEM_NLOUT");

	fnr=(_fn_rec*)malloc(sizeof(_fn_rec)*MAX_NL_ENTRIES);
	if (!fnr)
	{
		if (fur)
		{
			free(fur);
			fur=0;
		}
		ERR_MSG_RET0("E_MEM_NLOUT");
	}

	memset(fur,0,sizeof(_fu_rec)*MAX_NL_ENTRIES);
	memset(fnr,0,sizeof(_fn_rec)*MAX_NL_ENTRIES);
	nodel.LoadFromFile("nodelist.cfg");
	if (nodel.GetCount()==0)
		ERR_MSG_RET1("E_NONODELISTS");

	strcpy(nlidx[0].filename,"FIPS Nodelist Index\n\n\n\0x1b");
	nlidx[0].nltype=NL_VERSION;
	maxnlidx=1;

	for (t=0;t<nodel.GetCount();t++)
	{
		strcpy(tmp,nodel.GetString(t));
		if(analyse_nodelist(tmp,&nlidx[maxnlidx]))
		{
			strcpy(tmp3,strupr(nlidx[maxnlidx].filename));
			nl_add_listbox(L("S_256",strrchr(tmp3,'\\')+1,nlidx[maxnlidx].nltype));	// Found %s  (type %d)
			maxnlidx++;
		}
	}
	
	nl_add_listbox("=======================================");
	for (t=1;t<maxnlidx;t++)
	{
		act_offset=0;
		strcpy(tmp3,strupr(nlidx[t].filename));
		nl_add_listbox(L("S_360",nlidx[t].nltype==NODELIST ? L("S_338") : L("S_355"),
			strrchr(tmp3,'\\')+1));	// Processing %s %s
		fn.nl_idx=fu.nl_idx=(UCHAR)t; // nodelist-index sichern
		fn.zone = nlidx[t].pointzone; // default zone

		if (nlidx[t].pointzone !=0)
		{
			fn.zone = nlidx[t].pointzone;
			nl_add_listbox(L("S_364",fn.zone));	// Processing specified Zone %d
		}

		fp=fopen(nlidx[t].filename,"rt");
		if (fp)
		{
			act_offset=ftell(fp);

			while(fgets(input,1024,fp)!=NULL)
			{
				if (input[0] != ';')
				{
					switch (nlidx[t].nltype)
					{
						case POINTS24:
							retval=add_node_points24(input);
							break;
						case POINTS4D:
							retval=add_node_points4d(input);
							break;
						case YUCKNET:
							retval=add_node_yucknet(input);
							break;
						default: // Nodelisten-Typ
							retval=add_node_nodelist(input);
					}

				// jetzt muss der ermittelte Node in die Datenbank mitaufgenommen werden !

					if(retval && aktnl < MAX_NL_ENTRIES &&
						!check_exclude(fn.zone,fn.net,fn.node,fn.point))
					{
						fu.filepos=fn.filepos=act_offset;
						fu.zone	=fn.zone;
						fu.net	=fn.net;
						fu.node	=fn.node;
						fu.point=fn.point;
						memcpy(&fur[aktnl],&fu,sizeof(_fu_rec));
						memcpy(&fnr[aktnl],&fn,sizeof(_fn_rec));
						aktnl++;
					}
				}
				act_offset=ftell(fp);
			} // of while
			fclose(fp);
		}
	}
	addnodes.LoadFromFile("addnodes.cfg");
	if (addnodes.GetCount()!=0)
		nl_add_listbox(L("S_362"));	// Processing Private Nodes

	make_path(tmp3,gc.NodelistPath,"ADDTMP.$$$");
	afp = fopen(tmp3,"wb");

	for (t=0;t<addnodes.GetCount();t++)
	{
		char tmp[300];
		CString tmp2;

		get_token(addnodes.GetString(t),0,tmp2); // name
		strcpy(tmp,tmp2);
		tmp[39] =0;
		strcpy(rec.user,tmp);
		get_token(addnodes.GetString(t),1,tmp2); // nodenumber
		strcpy(tmp,tmp2);
		rec.zone=rec.node=rec.net=rec.point=0;
		sscanf(tmp,"%hu:%hu/%hu.%hu",&rec.zone,&rec.net,&rec.node,&rec.point);
		get_token(addnodes.GetString(t),2,tmp2);	// telephone
		strcpy(tmp,tmp2);
		tmp[39] =0;
		strcpy(rec.phone,tmp);
		get_token(addnodes.GetString(t),3,tmp2);	// flags
		strcpy(tmp,tmp2);
		tmp[75] =0;
		strcpy(rec.flags,tmp);
		strcpy(rec.bbsname,L("S_617"));	// private Node
		strcpy(rec.location,rec.bbsname);

		retval=fwrite(&rec,sizeof(_fidonumber),1,afp);
		if (retval!=1)
		{
			fclose(afp);
			ERR_MSG_RET0("E_CWWCN");
		}

		fu.filepos=fn.filepos=t;
		fn.zone	=rec.zone;
		fn.net	=rec.net;
		fn.node	=rec.node;
		fn.point=rec.point;
		fu.zone	=fn.zone;
		fu.net	=fn.net;
		fu.node	=fn.node;
		fu.point=fn.point;
		fu.nl_idx=fn.nl_idx = PRIVATE_IDX; // index number for private nodes

		strcpy(fu.name,rec.user);
		sprintf(mybuf,L("S_77",rec.user,rec.zone,rec.net,rec.node,rec.point));	// Adding private node
		nl_add_listbox(mybuf);
		memcpy(&fur[aktnl],&fu,sizeof(_fu_rec));
		memcpy(&fnr[aktnl],&fn,sizeof(_fn_rec));
		aktnl++;
	}

	fclose(afp);
	nl_add_listbox(L("S_9",aktnl));	// %d Users in Address lists

	nl_add_listbox(L("S_410"));	// Sorting by numbers
	qsort((void*)fnr,aktnl,sizeof(_fn_rec),compare_FN);

	nl_add_listbox(L("S_409"));	// Sorting by names
	qsort((void*)fur,aktnl,sizeof(_fu_rec),compare_FU);

	// writing index by numbers-----------------------------------------------
	nl_add_listbox(L("S_470"));	// Writing Index by Numbers
	make_path(tmp,gc.NodelistPath,nlidxtmpname);
	fp = fopen(tmp,"wb");
	if (!fp)
	{
		NLErrormsg(tmp);
		free(fnr);
		free(fur);
		return 0;
	}

	retval=fwrite(fnr,sizeof(_fn_rec),aktnl,fp);
	if (retval!=aktnl)
	{
		fclose(fp);
		ERR_MSG_RET0("E_CWWCN");
	}
	fclose(fp);

	// writing index by names-----------------------------------------------
	nl_add_listbox(L("S_469"));	// Writing Index by Names
	make_path(tmp,gc.NodelistPath,nludxtmpname);
	fp = fopen(tmp,"wb");
	if (!fp)
	{
		NLErrormsg(tmp);
		free(fnr);
		free(fur);
		return 0;
	}

	retval=fwrite(fur,sizeof(_fu_rec),aktnl,fp);
	if (retval!=aktnl)
	{
		fclose(fp);
		ERR_MSG_RET0("E_CWWCN");
	}

	fclose(fp);
	free(fnr);
	free(fur);
	NLGetAccess();

	// writing nodelist-index-----------------------------------------------
	nl_add_listbox(L("S_471"));	// Writing Nodelist-Index
	make_path(tmp,gc.NodelistPath,IDX_NAME);
	fp = fopen(tmp,"wb");
	if (!fp)
	{
		NLErrormsg(tmp);
		return 0;
	}

	retval=fwrite(nlidx,sizeof(_nl_idx),maxnlidx,fp);
	if (retval!=maxnlidx)
	{
		NLFreeAccess();
		fclose(fp);
		ERR_MSG_RET0("E_CWWCN");
	}
	fclose(fp);

	make_path(tmp,gc.NodelistPath,nludxtmpname);	// user-index
	make_path(tmp2,gc.NodelistPath,nludxname);
	if (access(tmp2,0)==0 && unlink(tmp2))
	{
		nl_add_listbox(L("S_118"));	// Cannot delete old User-Index
		NLFreeAccess();
		return 0;
	}

	rename(tmp,tmp2);
	make_path(tmp,gc.NodelistPath,nlidxtmpname);	// nodenumber -index
	make_path(tmp2,gc.NodelistPath,nlidxname);
	if (access(tmp2,0)==0 && unlink(tmp2))
	{
		nl_add_listbox(L("S_115"));	// Cannot delete old FIDO-Index
		NLFreeAccess();
		return 0;
	}

	rename(tmp,tmp2);
	make_path(tmp,gc.NodelistPath,"addtmp.$$$");	// private-node-index
	make_path(tmp2,gc.NodelistPath,"addnode.idx");
	if (access(tmp2,0)==0 && unlink(tmp2))
	{
		nl_add_listbox(L("S_116"));	// Cannot delete old PrivateNode-Index
		NLFreeAccess();
		return 0;
	}

	rename(tmp,tmp2);
	//----------------------------------------------------------------------
	kill_nl_stringlist();
	nl_add_listbox("=========================");
	nl_add_listbox(L("S_341"));	// Nodelist-Compile complete
	NLFreeAccess();
	return 1;
}

// ===========================================
	int check_exclude(int zone,int net,int node,int point)
// ===========================================
//  checkt ob die Zone in der Exclude-Liste steht
//	wenn ja, dann return 1 ansonsten return 0
//
{

struct _excludestruct
{
	 int zone;
	 int net;
	 int node;
	 int point;
};

static int done=0;
static int lastzone=0,lastnet=0,lastnode=0,lastpoint=0,lastreturn=0;
static int maxex=-1,t;
static struct _excludestruct ex[100];
char	excludelist[2000];
char	dummy[300];
int		i;

	if (done && lastzone==zone)
		return lastreturn;

	if (zone==0 && node==0 && net==0)
		 done=0;

	if (!done) // init exclude-list
	{
		strcpy(excludelist,get_cfg(CFG_PREFIX,"Exclude",""));
		if (strlen(excludelist))
			nl_add_listbox(L("S_230",excludelist));	// ExcludeList %s

 		i=count_tokens(excludelist,",\t\r\n");
		if (i>100)
			i=100;

		for (t=0;t<i;t++)
		{
    		extractstr(t+1,excludelist,",",dummy, sizeof(dummy)-1);
			maxex++;
			sscanf(dummy,"%d",&ex[maxex].zone);
			sprintf(mybuf,L("S_231",ex[maxex].zone));	// Excluding Zones %d
			nl_add_listbox(mybuf);
		}
		done=1;
	}

	lastreturn=0;
	for (t=0;t<=maxex;t++)
	{
		if (ex[t].zone==zone)
		{
			lastreturn = 1;
			return lastreturn;
		}
	}
	return lastreturn;
}

//=============================================================
	int analyse_nodelist(char *line,_nl_idx *curnl)
//=============================================================
{
FILE	*fp;
CString nodelist,help;
char	nlname[MAX_PATH],fname[MAX_PATH],buf2[MAX_PATH];
int		pointcount,bosscount,ispointlist=0,count=0;

	get_token(line,1,nodelist);
	get_token(line,3,help);				// zone
	ispointlist=get_token_int(line,4);  // pointlist
	make_path(nlname,gc.NodelistPath,nodelist);
	*fname=0;
	if (!get_newest_file(nlname,fname))
	{
		strcpy(buf2,nodelist);
		nl_add_listbox(L("S_337",buf2));	// No valid nodelist found for %s.*
		return 0;
	}

	strcpy(curnl->filename,fname);

	if (ispointlist)	// PointList
		curnl->pointzone = atoi(help);
	else
	{									// Nodelist
		curnl->pointzone=atoi(help);
		curnl->nltype=NODELIST;
		return 1;
	}

	fp = fopen(curnl->filename,"rt");
	if (!fp)	return 0;

	count=0;
	pointcount=bosscount=0;
	while (fgets(mybuf,250,fp) && count<600)
	{
		count++;
		extractstr(1,mybuf,",",buf2,sizeof(buf2)-1);
		strupr(buf2);
		if (strcmpi("POINT",buf2)==0)
			pointcount++;
		if (strcmpi("BOSS",buf2)==0)
			bosscount++;
	}
	fclose(fp);

	curnl->nltype=POINTS24; // default for pointlist
	if (pointcount>10)	curnl->nltype=POINTS4D;
	if (bosscount>3)		curnl->nltype=YUCKNET;
	return 1;
}

//======================================== // get newest file
	int get_newest_file(char *nl,char *filename)
//========================================
{
char temp[300],returnvalue[300],tmp[100];
_finddata_t se;
long hfile,oldtime=0;
int t,i;
char *p;

strcpy(temp,nl);
strcat(temp,".*");

returnvalue[0]=0;

if ((hfile = _findfirst((char *)temp,&se)) != -1L)
{
	do
	{
		if ((se.attrib & _A_SUBDIR) != _A_SUBDIR)
		{
			p=strstr(se.name,".");
			i=1;

			if (p != NULL)
			{
				p++;
				strcpy(tmp,p);
				for (t=0;t<(int)strlen(tmp);t++)
					if (isdigit(tmp[t])==0)	i=0;
			 }
			 else 
				 i=0;

			if (se.time_write > oldtime && i != 0)
			{
				 strcpy(returnvalue,nl);
				 while (returnvalue[strlen(returnvalue)-1] != 92)
					returnvalue[strlen(returnvalue)-1] =0;
				 strcat(returnvalue,se.name);
				 oldtime = se.time_write;
			}
		}
	}	while (_findnext(hfile,&se) == 0);

}

_findclose(hfile);
strcpy(filename,returnvalue);
if (oldtime!=0)
	return 1;
else 
	return 0;
}


// ==============================================================================
	int add_node_points24	(char*line)			// POINTS24
// ==============================================================================
{
	parse_nodelist_line(line,t1,t2,t3,t4,t5,t6);
	if (strcmp(t1,"REGION")==0)	return 0;
	if (strcmp(t1,"HOST")==0)
	{
		sscanf(t3,"%hu/%hu",&fn.net,&fn.node);
		return 0;
	}
	if (!strcmp(t1,"DOWN"))
	{
		fn.point = atoi(t2);
		strcpy(fu.name,t5);
	}
	else if (!strcmp(t1,"PVT"))
	{
		fn.point = atoi(t2);
		strcpy(fu.name,t5);
	}
	else if (line[0] != ',')
	{
		fn.point = atoi(t2);
		strcpy(fu.name,t5);
	}
	else
	{
		fn.point = atoi(t1);
		strcpy(fu.name,t4);
	}
	return 1;
}

// ==============================================================================
	int add_node_points4d	(char*line)			// POINTS4D
// ==============================================================================
{
	parse_nodelist_line(line,t1,t2,t3,t4,t5,t6);

	if (!strcmp(t1,"REGION"))	return 0;
	if (!strcmp(t1,"HOST"))
	{
		sscanf(t2,"%hu",&fn.net);
		return 0;
	}
	if (!strcmp(t1,"HUB"))
	{
		sscanf(t2,"%hu",&fn.node);
		return 0;
	}
	if (!strcmp(t1,"POINT"))
	{
		fn.point = atoi(t2);
		strcpy(fu.name,t5);
	}
	else
		fn.node = atoi(t1);

	return 1;
}

// ==============================================================================
	int add_node_yucknet	(char*line)			// YUCKNET
// ==============================================================================
{
	parse_nodelist_line(line,t1,t2,t3,t4,t5,t6);
	if (!strcmp(t1,"BOSS"))
	{
		sscanf(t2,"%hu:%hu/%hu",&fn.zone,&fn.net,&fn.node);
		return 0;
	}
	if (line[0] != ',')
	{
		sscanf(t2,"%hu",&fn.point);
		strcpy(fu.name,t5);
	}
	else
	{
		sscanf(t1,"%hu",&fn.point);
		strcpy(fu.name,t4);

	}

	return 1;
}

// ==============================================================================
	int add_node_nodelist(char*line)			// NODELIST FIDO
// ==============================================================================
{
	parse_nodelist_line(line,t1,t2,t3,t4,t5,t6);
	fn.point=0;

	if (!strcmp(t1,"ZONE"))
	{
		fn.zone = atoi(t2);
		fn.net  = fn.zone;
		fn.node = 0;
		strcpy(fu.name,t5);
		nl_add_listbox(L("S_365",fn.zone));	// Processing Zone %d
	}
	else if (!strcmp(t1,"REGION"))
	{
		fn.net  = atoi(t2);
		fn.node = 0;
		strcpy(fu.name,t5);
	}
	else if (!strcmp(t1,"HOST"))
	{
		fn.net  = atoi(t2);
		fn.node = 0;
		strcpy(fu.name,t5);
	}
	else if (!strcmp(t1,"HUB"))
	{
		fn.node = atoi(t2);
		strcpy(fu.name,t5);
	}
	else if (!strcmp(t1,"DOWN"))
	{
		fn.node = atoi(t2);
		strcpy(fu.name,t5);
	}
	else if (!strcmp(t1,"POINT")) // for mixed nodelist/pointlist
	{
		fn.point = atoi(t2);
		strcpy(fu.name,t5);
	}
	else if (line[0] != ',')
	{
		fn.node = atoi(t2);
		strcpy(fu.name,t5);
	}
	else
	{
		fn.node = atoi(t1);
		strcpy(fu.name,t4);
	}
	return 1;
}

//===============================
// replace underlines with spaces
	void killunder(char *str)
//===============================
{
char *t=str;

	while (*t)
	{
		if (*t=='_')	*t=' ';
		t++;
	}
}

// ==============================================================================
	void parse_nodelist_line(char *input,char *w1,char *w2,char *w3,char *w4,char *w5,char *w6)
// ==============================================================================
{
char s1[300],s2[300],s3[300],s4[300],s5[300],s6[300];

	extractstr(1,input,",",s1,sizeof(s1)-1);
	extractstr(2,input,",",s2,sizeof(s2)-1);
	extractstr(3,input,",",s3,sizeof(s3)-1);
	extractstr(4,input,",",s4,sizeof(s4)-1);
	extractstr(5,input,",",s5,sizeof(s5)-1);
	extractstr(6,input,",",s6,sizeof(s6)-1);

	s1[35]=0;
	s2[35]=0;
	s3[35]=0;
	s4[35]=0;
	s5[35]=0;
	s6[35]=0;

	killunder(s1);
	killunder(s2);
	killunder(s3);
	killunder(s4);
	killunder(s5);
	killunder(s6);

	strcpy(w1,strupr(s1));

	strcpy(w2,s2);
	strcpy(w3,s3);
	strcpy(w4,s4);
	strcpy(w5,s5);
	strcpy(w6,s6);
}


//===================================================
	int compare_FN(const void *aa1,const void *aa2)
//===================================================
{
_fn_rec *a1;
_fn_rec *a2;

	a1=(_fn_rec *)aa1;
	a2=(_fn_rec *)aa2;


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
	int compare_nl_rec_temp(const void *aa1,const void *aa2)
//===================================================
{
_nl_rec_temp *a1;
_nl_rec_temp *a2;

	a1=(_nl_rec_temp *)aa1;
	a2=(_nl_rec_temp *)aa2;

	if (a1->nlidx > a2->nlidx)			return 1;
	if (a1->nlidx < a2->nlidx)			return -1;
	if (a1->filepos > a2->filepos)	return 1;
	if (a1->filepos < a2->filepos)	return -1;

	return 0;
}


//===================================================
	int compareFidoNumber(const void *aa1,const void *aa2)
//===================================================
{
_fidonumber *a1;
_fidonumber *a2;

	a1=(_fidonumber *)aa1;
	a2=(_fidonumber *)aa2;

	return strcmpi(a1->user,a2->user);
}

//===================================================
	int compare_FU(const void *aa1,const void * aa2)
//===================================================
{
_fu_rec *a1;
_fu_rec *a2;

	a1=(_fu_rec *)aa1;
	a2=(_fu_rec *)aa2;

	return strcmpi(a1->name,a2->name);
}


//===================================================
	int expand_from_fn(_fn_rec fn,_fidonumber *res)
//===================================================
{

FILE *fp;
char tmp[1024],tmp2[1024],path[MAX_PATH];
int  t;
char *p;

	memset(res,0,sizeof(_fidonumber));
	if (fn.nl_idx==PRIVATE_IDX)
	{
		make_path(path,gc.NodelistPath,"ADDNODE.IDX");
		fp = fopen(path,"rb");
		if (!fp)	return 0;

		fseek(fp,fn.filepos*(sizeof(_fidonumber)),0);
		fread(res,sizeof(_fidonumber),1,fp);
		fclose(fp);
		return 1;
	}

	fp=fopen(nlidx[fn.nl_idx].filename,"rt");
	if (!fp)	return 0;

	fseek(fp,fn.filepos,0);
	if (!fgets(tmp,1000,fp))
	{
		fclose(fp);
		return 0;
	}

	res->zone	=fn.zone;
	res->net	=fn.net;
	res->node	=fn.node;
	res->point	=fn.point;

	parse_nodelist_line(tmp,t1,t2,t3,t4,t5,t6);
	t=0;
	if (tmp[0]==',') // normale zeile
	{
		strcpy(res->bbsname,t2);
		strcpy(res->location,t3);
		strcpy(res->user,t4);
		strcpy(res->phone,t5);

		p = tmp;
		while ( t < 6 && *p != 0)
		{
			if (*p==',')	t++;
			p++;
		}

		strcpy(tmp2,p);
		tmp2[75]=0;
		strcpy(res->flags,tmp2);
	}
	else
	{
		strcpy(res->bbsname,t3);
		strcpy(res->location,t4);
		strcpy(res->user,t5);
		strcpy(res->phone,t6);

		p = tmp;
		while ( t < 7 && *p != 0)
		{
			if (*p==',')	t++;
			p++;
		}

		strcpy(tmp2,p);
		tmp2[75]=0;
		strcpy(res->flags,tmp2);
	}
	fclose(fp);
	return 1;
}

//===================================================
int expand_from_fu(_fu_rec fu,_fidonumber *res)
//===================================================
{
FILE * fp;
char tmp[1024],tmp2[1024],path[MAX_PATH];
int t;
char*p;

	ASSERT(res);
	memset(res,0,sizeof(_fidonumber));
	if (fu.nl_idx==ABOOK_IDX)
	{
		res->zone=fu.zone;
		res->net=fu.net;
		res->node=fu.node;
		res->point=fu.point;
		strcpy(res->phone,L("S_618"));	// AdressBook
		strcpy(res->user,fu.name);
    strcpy(res->flags,"");
		return 1;
	}

	if (fu.nl_idx==PRIVATE_IDX)
	{
		make_path(path,gc.NodelistPath,"ADDNODE.IDX");
		fp = fopen(path,"rb");
		if (!fp)	return 0;

		fseek(fp,fu.filepos*(sizeof(_fidonumber)),0);
		fread(res,sizeof(_fidonumber),1,fp);
		fclose(fp);
		return 1;
	}

	fp=fopen(nlidx[fu.nl_idx].filename,"rt");
	if (!fp)	return 0;

	fseek(fp,fu.filepos,0);
	if (!fgets(tmp,1000,fp))
	{
		fclose(fp);
		return 0;
	}

	res->zone	=fu.zone;
	res->net		=fu.net;
	res->node	=fu.node;
	res->point	=fu.point;

	parse_nodelist_line(tmp,t1,t2,t3,t4,t5,t6);
	t=0;
	if (tmp[0]==',') // normal line
	{
		strcpy(res->bbsname,t2);
		strcpy(res->location,t3);
		strcpy(res->user,t4);
		strcpy(res->phone,t5);

		p = tmp;
		while ( t < 6 && *p != 0)
		{
			if (*p==',')	t++;
			p++;
		}
		strcpy(tmp2,p);
		tmp2[35]=0;
		strcpy(res->flags,tmp2);
	}
	else
	{
		strcpy(res->bbsname,t3);
		strcpy(res->location,t4);
		strcpy(res->user,t5);
		strcpy(res->phone,t6);

		p = tmp;
		while ( t < 7 && *p != 0)
		{
			if (*p==',')	t++;
			p++;
		}
		strcpy(tmp2,p);
		tmp2[35]=0;
		strcpy(res->flags,tmp2);
	}
	fclose(fp);
	return 1;
}

//===================================================
// loads idx's and verify their presence
	int check_nodelist_idx(void)
//===================================================
{
static int done=0;
FILE	*fp;
char	path[MAX_PATH];

	if (!done)
	{
		make_path(path,gc.NodelistPath,IDX_NAME);
		fp=fopen(path,"rb");
		if (!fp)	return 0;
		maxnlidx = fread(nlidx,sizeof(_nl_idx),MAX_NODELISTS,fp);
		fclose(fp);
		done=1;
	}

	for (int t=1;t<maxnlidx;t++)
	{
		if (access(nlidx[t].filename,0) == -1)
			return 0;
	}
	return 1;
}

//===============================================================================
// returns 0 if fidonumber doesn't exist
	int nl_get_fido_by_number(int zone,int net,int node,int point,_fidonumber *res)
//===============================================================================
{
CStrList	adrbook;
FILE 		*fp;
_fn_rec		dm;
_fu_rec		du;
char		fbuf[25],nbuf[25],buf[100],fname[MAX_PATH];
int 		max=0,maxdum,current,ende=0,maxcount=0,dumcount=0;
int			lborder,rborder,t,tz,tne,tno,tp;

	memset(res,0,sizeof(_fidonumber));
	if (!check_nodelist_idx())
	{
		if (!get_cfg(CFG_COMMON,"NoNLReminder",0))	err_out("E_WRONGNODEL"); // need to update
		return 0;
	}
	NLGetAccess();
	sprintf(fbuf,"%04x:%04x/%04x.%04x",zone,net,node,point);
	make_path(fname,gc.NodelistPath,nlidxname);
	fp=fopen(fname,"rb");
	if (!fp)
	{
		NLFreeAccess();
	    return 0;
	}
	fseek(fp,0,SEEK_END);
	max = ftell(fp);
	rewind(fp);

	if (max % sizeof(_fn_rec))
	{
		NLFreeAccess();
		fclose(fp);
		fp=0;
		if (!get_cfg(CFG_COMMON,"NoNLReminder",0))	err_out("E_WRONGNODEL"); // need to update
		return 0;
	}

	max = max/sizeof(_fn_rec);
	current = max/2;
	rborder=max;
	lborder=0;

	while (ende==0)
	{
		fseek(fp,current*sizeof(_fn_rec),SEEK_SET);
		fread(&dm,sizeof(_fn_rec),1,fp);
		sprintf(nbuf,"%04x:%04x/%04x.%04x",dm.zone,dm.net,dm.node,dm.point);

		if (strcmp(nbuf,fbuf)==0)
		{
			expand_from_fn(dm,res);
			fclose(fp);
			fp=0;
			NLFreeAccess();
			return 1;
		}
		else if (strcmp(nbuf,fbuf)<0)
		{
			lborder=current;
			maxdum = (rborder-current)/2;
			current+=maxdum;
			if (current>max || maxdum == 0)
			{
				fclose(fp);
				fp=0;
				NLFreeAccess();
				goto searchadr;
			}
		}
		else
		{
			rborder=current;
			maxdum =(current-lborder)/2;
			current-=maxdum;
			if (current<0 || maxdum == 0)
			{
				fclose(fp);
				fp=0;
				NLFreeAccess();
				goto searchadr;
			}
		}
	}
	fclose(fp);
	fp=0;
	NLFreeAccess();

// search in addressbook
searchadr:

	adrbook.LoadFromFile("Adrbook.cfg");
	for (t=0;t<adrbook.GetCount();t++)
	{
		get_token(adrbook.GetString(t),1,buf);
		parse_address(buf,&tz,&tne,&tno,&tp);
		dm.zone=tz; dm.net=tne; dm.node=tno; dm.point=tp;
		if (tz==zone && tne==net && tno==node && tp==point)
		{
			memset(&du,0,sizeof(du));
			get_token(adrbook.GetString(t),0,buf);
			strcpy(du.name,buf);
			du.filepos=(char)0;
			du.nl_idx=ABOOK_IDX; // adrbook identifier
			expand_from_fu(du,res);
			strcpy(res->flags,L("S_618"));	// AdressBook
			return 1;
		}
	}
	return 0;
}

//=============================================================================
	int nl_get_fido_by_name(char *name,int *count,struct _fidonumber res[])
//=============================================================================
// searches a fidonumber by name
// returns 0 if no name is found, 1 if at least one is found
// int *count contains the maximum number to search for and after the search the
// number of found names in the nodelists.
{
CStrList	adrbook;
char	fname[300];
int		max=0,maxdum,current,ende=0,maxcount=0,dumcount=0;
_fu_rec	dm;
FILE	*fp;
int		lborder,rborder,t,tz,tne,tno,tp;
int		size_fu=sizeof(_fu_rec);

	maxcount=*count;
	*count	=-1;

	if (!check_nodelist_idx())
	{
		if (!get_cfg(CFG_COMMON,"NoNLReminder",0))
			err_out("E_WRONGNODEL"); // nodelist need to update
		return 0;
	}

	ASSERT(name && count);
	adrbook.LoadFromFile("Adrbook.cfg");
	for (t=0;t<adrbook.GetCount();t++)
	{
		CString tmp;

		get_token(adrbook.GetString(t),0,tmp);

		if (!strnicmp(name,tmp,strlen(name))) // found in adrbook and fill dm record
		{
			memset(&dm,0,sizeof(dm));
			strcpy(dm.name,tmp);
			get_token(adrbook.GetString(t),1,tmp);
			parse_address(tmp,&tz,&tne,&tno,&tp);
			dm.zone=tz; dm.net=tne; dm.node=tno; dm.point=tp;
			dm.filepos=0;
			dm.nl_idx=ABOOK_IDX; // adrbook identifier
			*count=*count+1;
			expand_from_fu(dm,&res[*count]);
		}
	}
	NLGetAccess();
	make_path(fname,gc.NodelistPath,nludxname);
	fp=fopen(fname,"rb");
	if (!fp)
	{
		NLFreeAccess();
		return 0;
	}
	fseek(fp,0,SEEK_END);
	max = ftell(fp);
	rewind(fp);

	if (max % size_fu)
	{
		NLFreeAccess();
		fclose(fp);
		if (!get_cfg(CFG_COMMON,"NoNLReminder",0))	err_out("E_WRONGNODEL"); // nodelist need to update
		return 0;
	}

	max = max/size_fu;
	current = max/2;
	rborder=max;
	lborder=0;

	while (ende==0)
	{
		fseek(fp,current*size_fu,SEEK_SET);
		fread(&dm,size_fu,1,fp);

		if (_strnicmp(name,dm.name,strlen(name))==0)
		{
			while (current>0 && _strnicmp(name,dm.name,strlen(name))==0) // find first matching name
			{
				current--;
				fseek(fp,current*size_fu,SEEK_SET);
				fread(&dm,size_fu,1,fp);
			}

			if (_strnicmp(name,dm.name,strlen(name))!=0)
			{
				current++;
				fseek(fp,current*size_fu,SEEK_SET);
				fread(&dm,size_fu,1,fp);
			}

			while (_strnicmp(name,dm.name,strlen(name))==0 && *count < maxcount-1 && current < max)
			{
				fseek(fp,current*size_fu,SEEK_SET);
				fread(&dm,size_fu,1,fp);

				if (_strnicmp(name,dm.name,strlen(name))==0)
				{
					*count=*count+1;
					expand_from_fu(dm,&res[*count]);
				}

				//memcpy(&res[*count],&dm,size_fu);
				current++;
			}

			// sort the found list of usernames acoording to their nodenumber
			if (*count>0)
				qsort(res,(size_t)(*count)+1,sizeof(_fidonumber),compareFidoNumber);

			*count=*count+1;
			NLFreeAccess();
			fclose(fp);
			return 1;
		}

		if (_strnicmp(name,dm.name,strlen(name))>0)
		{
			lborder=current;
			maxdum = rborder-current;
			maxdum = maxdum/2;
			current+=maxdum;
			if (current>max || maxdum == 0)
			{
				if(fclose(fp) && !get_cfg(CFG_COMMON,"NoNLReminder",0))
			 		err_out("E_WRONGNODEL");

				NLFreeAccess();
				if (*count <0)	return 0;
				*count=*count+1;
				return 1;
			}
		}
		else
		{
			maxdum = current-lborder;
			rborder=current;
			maxdum = maxdum/2;
			current-=maxdum;
			if (current<0 || maxdum == 0)
			{

				if(fclose(fp) && !get_cfg(CFG_COMMON,"NoNLReminder",0))
			 		err_out("E_WRONGNODEL");

				NLFreeAccess();
				if (*count <0)	return 0;
				*count=*count+1;
				return 1;
			}
		}
	}

	if(fclose(fp) && !get_cfg(CFG_COMMON,"NoNLReminder",0))
		err_out("E_WRONGNODEL"); // nodelist need to update

	NLFreeAccess();
	return(1);
}

//=============================================================================
	int nl_get_fido_by_partname(char *namesearch,int *count,struct _fidonumber res[])
//=============================================================================
// searches a fidonumber by a part of a name
// returns 0 if no name is found, 1 if at least one is found
// count - contains the maximum number to search for and the number of found names in the nodelists.
{
char	fname[MAX_PATH];
int		max=0,ende=0,maxcount=0;
_fu_rec dm;
FILE	*fp;
int		dumcount=0,t,tz,tne,tno,tp;
char    name[80];
CStrList adrbook;
int		size_fu=sizeof(struct _fu_rec);

	strcpy(name,namesearch);
	strupr(name);
	maxcount=*count;
	*count	=0;

	if (!check_nodelist_idx())	return 0;

	ASSERT(name && count);

	adrbook.LoadFromFile("Adrbook.cfg");

	for (t=0;t<adrbook.GetCount();t++)
	{
		CString tmp;

		get_token(adrbook.GetString(t),0,tmp);
		tmp.MakeUpper();

		if (strstr(tmp,name))	// found in adrbook
		{ 
			memset(&dm,0,sizeof(dm));
			get_token(adrbook.GetString(t),0,tmp);
			strcpy(dm.name,tmp);
			get_token(adrbook.GetString(t),1,tmp);
			parse_address(tmp,&tz,&tne,&tno,&tp);
			dm.zone=tz; dm.net=tne; dm.node=tno; dm.point =tp;

			dm.filepos=(char)0;
			dm.nl_idx=ABOOK_IDX; // adrbook identifier

			expand_from_fu(dm,&res[*count]);
			*count=*count+1;
		}

	}

	NLGetAccess();
	make_path(fname,gc.NodelistPath,nludxname);
	fp=fopen(fname,"rb");
	if (!fp)
	{
		NLFreeAccess();
		return 0;
	}

	fseek(fp,0,SEEK_END);
	max = ftell(fp);
	rewind(fp);

	if (max % size_fu)
	{
		NLFreeAccess();
		fclose(fp);
		if (!get_cfg(CFG_COMMON,"NoNLReminder",0))
			err_out("E_WRONGNODEL"); // nodelist need to update
		return 0;
	}

	max = max/size_fu;
	for (t=0;t<max;t++)
	{
		char tmp[80];
		fread(&dm,size_fu,1,fp); // read a set
		strcpy(tmp,_strupr(dm.name));

		if (strstr(tmp,name)) // found
		{
			expand_from_fu(dm,&res[*count]);
			*count=*count+1;
		}

		if (*count > maxcount)
			break;

	}
	fclose(fp);
	NLFreeAccess();

	if (*count>0)
	{
		qsort(res,*count,sizeof(_fidonumber),compareFidoNumber);
		return 1;
	}
	else
		return 0;
}

//=============================================================================
	int check_searchp(CString tmp,CStrList &searchp)
//=============================================================================
{
	for (int t=0;t<searchp.GetCount();t++)
		if (!strstr(tmp,searchp.GetString(t)))
			return 0;

	return 1;
}

//=============================================================================
// searches a fidonumber by a part of a name , searches textfile -> could last very long
	int nl_get_fido_by_partname_ext(char *namesearch,int *count,struct _fidonumber res[])
//=============================================================================
{

#define MAX_FT_SEARCH	2000
#define	MAX_DUMMY		200
struct _foundrec
{
	int nlidx;
	int filepos;
};

FILE	*fp;
CStrList adrbook;
CStrList searchp;
_fu_rec dm;
_fu_rec dummy2[MAX_DUMMY];
_foundrec ft[MAX_FT_SEARCH];
_nl_rec_temp * nt;

char	fname[300],name[300],tmp[1024];
int		ntcount,maxsearch,max,counter,aktsearch=0,found=0;
int		t,i,tz,tne,tno,tp;
int		size_fu=sizeof(_fu_rec);

	maxsearch=*count;
	*count=0;

	if (maxsearch>2000)
		maxsearch=2000;

	strcpy(name,namesearch);
	strupr(name);

	i = count_tokens(name);
	searchp.RemoveAll();

	// build list of strings to search for
	for (t=0;t<i;t++)
	{
		char dm[300];
    extractstr(t+1,name," ",dm, sizeof(dm)-1);
		searchp.AddTail(dm);
	}

	// first search adressbook
	adrbook.LoadFromFile("Adrbook.cfg");

	for (t=0;t<adrbook.GetCount();t++)
	{
		CString tmp;

		get_token(adrbook.GetString(t),0,tmp);

		tmp.MakeUpper();

		if (check_searchp(tmp,searchp))
		{ // found in adrbook
			//now fill dm record

			memset(&dm,0,sizeof(dm));
			get_token(adrbook.GetString(t),0,tmp);
			strcpy(dm.name,tmp);
			get_token(adrbook.GetString(t),1,tmp);
			parse_address(tmp,&tz,&tne,&tno,&tp);
			dm.zone=tz; dm.net=tne; dm.node=tno; dm.point =tp;

			dm.filepos=(char)0;
			dm.nl_idx=ABOOK_IDX; // adrbook identifier

			expand_from_fu(dm,&res[*count]);
			*count=*count+1;
			maxsearch--; // we already found something !
			found=1;
		}
	}

	if (!check_nodelist_idx())
		return (*count >= 1);

	NLGetAccess();
	for (t=0;t<maxnlidx;t++) // search all ascii-nodelists
	{
		FILE *fp;
		int filepos;

		fp=fopen(nlidx[t].filename,"rt");	// nodeliste oeffnen
		if (fp)
		{
			filepos=ftell(fp);

			while (fgets(tmp,sizeof(tmp)-1,fp)!=0) // read a line
			{
				tmp[1000]=0; // terminieren zur sicherheit

				if (tmp[0] != ';') // no comment - lines
				{
					char *p;
					p=tmp;

					while (*p != 0) // underlines ersetzen durch spaces
					{
						if (*p == '_')
							*p = ' ';
						p++;
					}

					_strupr(tmp);

					if ((check_searchp(tmp,searchp)) && (aktsearch < maxsearch))
					{
						ft[aktsearch].nlidx = t;
						ft[aktsearch].filepos = filepos;
						aktsearch++;
					}
				}
				filepos=ftell(fp);
			}
			fclose(fp);
		}
	}


	if (aktsearch >0)
	{
		make_path(fname,gc.NodelistPath,nludxname);
		fp=fopen(fname,"rb");
		if (!fp)
		{
			NLFreeAccess();
			return 0;
		}

		fseek(fp,0,SEEK_END);
		max = ftell(fp);
		rewind(fp);

		if ((max % size_fu))
		{
			NLFreeAccess();
			fclose(fp);
			if (!get_cfg(CFG_COMMON,"NoNLReminder",0))
				err_out("E_WRONGNODEL"); // nodelist need to update
			return 0;
		}

		counter=0;
		ntcount=0;
		nt = (_nl_rec_temp*)malloc(MAX_NL_ENTRIES * sizeof(_nl_rec_temp));

		if (!nt)
		{
			NLFreeAccess();
			fclose(fp);
			ERR_MSG_RET0("E_MEM_OUT");
		}

		while (1)
		{
			int i=fread(&dummy2,size_fu,MAX_DUMMY,fp); // read a set

			for (int k = 0; k < i; k++)
			{
				nt[ntcount].recnumber=ntcount;
				nt[ntcount].filepos = dummy2[k].filepos;
				nt[ntcount].nlidx = dummy2[k].nl_idx;
				ntcount++;
			}

			if (i < MAX_DUMMY) // last record
				break;

		}

		// sort the nl-idx
		qsort((void*)nt,(size_t)ntcount,(size_t)sizeof(_nl_rec_temp),compare_nl_rec_temp);

		// nl-idx is sorted
		for (t=0;t<aktsearch;t++)
		{
			int rborder,lborder,aktpos=0,ende=0;
			_nl_rec_temp dum;

			dum.filepos = ft[t].filepos;
			dum.nlidx   = ft[t].nlidx;

			rborder=ntcount;
			lborder=0;

			while (!ende)
			{
				int x = compare_nl_rec_temp(&nt[aktpos],&dum);
				if (x==0)
				{
					// found the right entry

					fseek(fp,(nt[aktpos].recnumber*size_fu),SEEK_SET);
					fread(&dm,size_fu,1,fp); // read a set

					expand_from_fu(dm,&res[*count]);
					*count=*count+1;
					counter++;
					found=1;

					break;
				}

				if (x > 0)
				{
					// less than
					rborder=aktpos;
					aktpos = aktpos - (int)((rborder-lborder)/2);

					if (aktpos==rborder || aktpos < 0) // not found
						break;

				}
				else
				{
					// greater than
					lborder=aktpos;
					aktpos = aktpos + (int)((rborder-lborder) / 2);

					if (aktpos==lborder || aktpos>ntcount) // not found
						break;

				}
			}
		}

	free(nt);
	}

	NLFreeAccess();

	if (found)
		return 1;
	else
		return 0;
}



// ##################################################################################
// Die folgenden Funktionen werden fuer das nodeliste-kompilieren bzw. nodediff-merge
// gebraucht. nach dem mergen der diffs werden die alten nodelisten nicht sofort
// geloescht, sondern in einer stringliste "gemerkt". erst nach dem vollstaendigen
// kompilieren der nodeliste werden die alten nodelisten geloescht.
// ##################################################################################

// ==============================================================================
	void add_to_nl_stringlist(char *filename )
// ==============================================================================
{
	nl_list.AddTail(filename);
}

// ==============================================================================
	void check_old_files(char *fname, char *path)
// ==============================================================================
{
char	temp[MAX_PATH],tmp[MAX_PATH];
_finddata_t se,last;
long	hfile,oldtime=0;
int		validname,found=0;
char	*p;

strcpy(temp,fname);
strcat(temp,".*"); // .* an dateinamen anhaengen

if ((hfile = _findfirst(temp,&se)) != -1L)
{
	do {
		if (!found)
		{
			validname=2; // check
			p = strchr(se.name,'.');
			if (p && *p)
			{
				p++;
				while (*p)
				{
					if (isdigit(*p))
						validname--;

					p++;
				}
			}

			if (validname < 1)
			{
				memcpy(&last,&se,sizeof(_finddata_t));
				found=1;
			}
		}
		else
		{
// check if nodelistname has at least two digits in extension, otherwise it is no nodelist-file
			validname=2;
			p = strchr(se.name,'.');
			if (p && *p)
			{
				p++;
				while (*p)
				{
					if (isdigit(*p))
						validname--;
					p++;
				}
			}
// we have found at least 2 digits
			if (validname < 1)
			{
				if (last.time_write < se.time_write)
				{
					unlink(make_path(tmp,path,last.name));
					strcpy(temp,last.name);
					memcpy(&last,&se,sizeof(_finddata_t));
				}
				else
				{
					unlink(make_path(tmp,path,se.name));
					strcpy(temp,se.name);
				}
				nl_add_listbox(L("S_160",temp));	// Deleting old Nodelist %s
			}
		}
	} while (_findnext(hfile,&se)==0);

	_findclose(hfile);
	}
}

// ==============================================================================
	void kill_nl_stringlist(void)
// ==============================================================================
{
int		t=0;
char	fname[MAX_PATH],path[MAX_PATH];
CString str;

	for (t=0;t<nl_list.GetCount();t++)
	{
		strcpy(path,nl_list.GetString(t));
		if (!access(path,0))
		{
			unlink(path);
			get_filename(path,fname);
			nl_add_listbox(L("S_157",fname));	// Deleting %s
		}
	}
	nodel.LoadFromFile("NODELIST.CFG");
	for (t=0;t<nodel.GetCount();t++)
	{
		get_token(nodel.GetString(t),1,str);
		make_path(fname,gc.NodelistPath,str);
		check_old_files(fname,gc.NodelistPath);
	}
}

//############## Critical Section fuer NodelistAccess #################

// ==============================================================================
	void NLGetAccess(void)
// ==============================================================================
{
	EnterCriticalSection(&NodeListAcces);
}
// ==============================================================================
	void NLFreeAccess(void)
// ==============================================================================
{
	LeaveCriticalSection(&NodeListAcces);
}

// ==============================================================================
	void NLErrormsg(char *tmp)
// ==============================================================================
{
	nl_add_listbox(L("S_113",tmp));	// Cannot create file %s
	nl_add_listbox(L("S_339"));	// Nodelist compile not possible
	NLFreeAccess();
}