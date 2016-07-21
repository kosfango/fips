// ecolst.cpp : implementation file
// IDD_ECHO_FILELIST

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "io.h"
#include "cfido.h"
#include "cfg_boss.h"
#include "cfg_area.h"
#include "manual.h"
#include "ecolst.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CStrList bosses;
CStrList areas;
CStrList difflist;
CStrList areas_org;
CStrList avechotags;
CStrList ManualAreaBestell;
CString selecteduplink;
extern _gconfig gc;

static char DlgName[]="IDD_ECHO_FILELIST";

// ===========================================================================
	ecolst::ecolst(CWnd* pParent ) : CDialog(ecolst::IDD, pParent)
// ===========================================================================
{
	//{{AFX_DATA_INIT(ecolst)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ===========================================================================
void ecolst::DoDataExchange(CDataExchange* pDX)
// ===========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ecolst)
	DDX_Control(pDX, IDC_AREALIST, m_arealist);
	DDX_Control(pDX, IDC_BOSS, m_bosslist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ecolst, CDialog)
	//{{AFX_MSG_MAP(ecolst)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(IDC_BUTTON1, OnRequestEchoList)
	ON_LBN_SELCHANGE(IDC_BOSS, OnSelchangeBoss)
	ON_LBN_DBLCLK(IDC_AREALIST, OnDblclkarealist)
	ON_BN_CLICKED(IDC_SEND_COMPLETE, OnSendComplete)
	ON_BN_CLICKED(IDC_BOSSCFG, OnBosscfg)
	ON_BN_CLICKED(IDC_AREACFG, OnAreacfg)
	ON_BN_CLICKED(IDC_MANUAL_ORDER, OnManualOrder)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ===========================================================================
BOOL ecolst::OnInitDialog()
// ===========================================================================
{
int tabs[]={100,180}; //,260,400,1900};
int tabs2[]={110,120}; //,180,260,400,1900};
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_BUTTON1,
			IDC_SEND,
			IDC_STATIC3,
			IDC_SEND_COMPLETE,
			IDC_BOSSCFG,
			IDC_AREACFG,
			IDC_MANUAL_ORDER,
			IDHELP,
			IDOK
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	m_bosslist.SetTabStops((sizeof(tabs)/sizeof(int)),tabs);
	m_arealist.SetTabStops((sizeof(tabs)/sizeof(int)),tabs2);
	m_arealist.SetHorizontalExtent(1000);

	display_boss_configs();
	build_available_echotags();

	m_bosslist.SetCurSel(0);
	OnSelchangeBoss();
	return TRUE;
}

// ===========================================================================
	void ecolst::build_diff_list(void)
// ===========================================================================
{
char buf1[300],buf2[300];

	difflist.RemoveAll();
	for (int i=0;i<areas.GetCount();i++)
	{
	   	strcpy(buf1,areas.GetString(i));
	   	strcpy(buf2,areas_org.GetString(i));
		if (strcmp(buf1,buf2))
		   difflist.AddTail(buf1);
	}
}

// ===========================================================================
	void ecolst::OnSend() 			  // Differences only
// ===========================================================================
{
	if (err_out("DY_SENDDIFF") != IDYES)	return;
	build_diff_list();
	build_afix_netmail(0);
}

// ===========================================================================
	void ecolst::OnBosscfg()
// ===========================================================================
{
	cfg_boss dlg;
	dlg.DoModal();
	OnSelchangeBoss();
}

// ===========================================================================
	void ecolst::OnAreacfg()
// ===========================================================================
{
	cfg_area dlg;
	dlg.DoModal();
	OnSelchangeBoss();
}

// ===========================================================================
	void ecolst::OnSendComplete() 	 // Compete List only
// ===========================================================================
{
	if (err_out("DY_SENDFULL") != IDYES)	return;
	build_afix_netmail(1); 		// Send complete List
}

// ===========================================================================
	void ecolst::OnRequestEchoList()
// ===========================================================================
// TOP: Diese Funktion setzt fuer den selektierten Boss einen FileRequest rein
// und bestellt die entsprechende EchoFileListe
//
{
FILE    *fp;
CFido	cfid;
CString	fido,dir,echolistname;
char	fname[MAX_PATH],buf[MAX_BOSSLEN];
int		sel,ret;

	sel=m_bosslist.GetCurSel();
	LB_ERR_RET;

	ret=db_get_boss_by_index(sel,buf);
	ASSERT(ret==DB_OK);

	get_token(buf,BOSSADDR,fido);
	get_token(buf,ECHOFILE,echolistname);
	if (echolistname.IsEmpty())
		ERR_MSG_RET("E_NOELCONF");

	cfid=fido;
	dir=cfid.GetAsDir();

	make_path(fname,gc.OutboundPath,dir);
	if (access(fname,0))
	   CreateDirectory(fname,0);

	strcat(fname,"\\" FREQ_LIST);
	fp=fopen(fname,"at");
	if (fp)
	{
		fprintf(fp,"%s\n",echolistname);
		fclose(fp);
	}
	else
		ERR_MSG_RET("E_FRCFAIL");

	err_out("M_ADDEDECFR");
}

/*/ ===========================================================================
	void ecolst::OnOK()
// ===========================================================================
{
	CDialog::OnOK();
}
*/
// ===========================================================================
	void ecolst::OnSelchangeBoss()
// ===========================================================================
{
	m_arealist.ResetContent();
	display_the_area_stuff_for_actual_boss();
}

// ===========================================================================
	void ecolst::OnDblclkarealist()
// ===========================================================================
{
int		sel;
int		top;
CString line;
char	buf[300];
char	*p;

	sel=m_arealist.GetCurSel();
	LB_ERR_RET;

	top=m_arealist.GetTopIndex();
	strcpy(buf,areas.GetString(sel));
	p=strchr(buf,'\t');
	ASSERT(p);
	p++;

	if (*p=='X')
		ERR_MSG_RET("W_TAICOFAB");

	if (*p=='O')
	   *p=' ';
	else
	   *p='O';

    areas.Insert(sel,buf);
	areas.Remove(sel);
	UPDATE_LB(areas,IDC_AREALIST);
	m_arealist.SetRedraw(0);
	m_arealist.SetCurSel(sel);
	m_arealist.SetRedraw(1);
	m_arealist.SetTopIndex(top);
}

// ===========================================================================
	void ecolst::handle_uplink_remove_for_this_area(char *area)
// ===========================================================================
// Diese Funktion traegt nach Rueckfrage beim Benutzer aus der entsprechenden Area den UPLINK
// aus.
// Gefragt wird nur wenn ueberhaupt ein UPLINK eingetragen ist.
{
areadef	ad;
int		i,count;

	db_get_count_of_areas(&count);
	for (i=0;i<count;i++)
	{
		memset(&ad,0,sizeof(ad));
		db_get_area_by_index(i,&ad);
		if (!stricmp(ad.echotag,area))
		{
			if (err_out("DN_READTAR",PS area) != IDYES)	return;
			if (strlen(ad.uplink)>0)
			{
				strcpy(ad.uplink,"");
				db_change_area_properties(&ad);
			}
		}
	}
}

/*/ ===========================================================================
	void ecolst::OnDblclkBoss()
// ===========================================================================
{

}*/

// ===========================================================================
	void ecolst::display_the_area_stuff_for_actual_boss()
// ===========================================================================
// TOP Hier werden die Bosse und deren Echolisten geholt, ind die CStrList geschoben
// und dargestellt.
{
CString act;
CString filename;
CString echofile;
char echopath[MAX_PATH];
CString all;
CString entry;
CString ar;
CString desc;
CString status;
int		sel,sub;
FILE	*fp;
char	line[1000],tmp[1000],buf[1000],*p,*p2,*p3,*t;

	selecteduplink.Empty();
	sel=m_bosslist.GetCurSel();
	LB_ERR_RET;

	m_bosslist.GetText(sel,act);
	get_token(act,1,echofile);
	make_path(echopath,gc.UtilPath,echofile);

	areas.RemoveAll();
	fp=fopen(echopath,"rt");
	if (access(echopath,0))
		return;

	get_token(act,2,selecteduplink);

	ASSERT(fp);
	if (fp)
	{
	   while (fgets(line,299,fp))
	   {
			p=strchr(line,'\n');
			if (p)
			   *p=0;
			if (strlen(line)<3)
			   continue;

			p=strchr(line,'"');
			if (p)		  // Format Nr 1		TALK.NB,"Niederbayrisches BlaBla"
			{
				p=strchr(line,',');
				if (p)
				{
					strncpy(buf,line,p-line);
					buf[p-line]=0;
					ar=buf;

					p2=strchr(p+1,'"');
					if (p2)
					{
						p3=strchr(p2+1,'"');
						if (p3)
						   *p3=0;

						desc=(p2+1);
						sub=do_we_have_this_area_subscribed(PS ar);
						switch (sub)
						{

							default:
						    case 0: 	     // Area not ordered
							        status=" ";
									break;
							case 1:			 // Area from this Boss
							        status="O";
									break;

							case 2:			 // Area from other Boss
							        status="X";
									break;
						}
						desc.OemToAnsi();
						entry=ar+'\t'+status+'\t'+desc;
						areas.AddTail(PS entry);
					}
				}
			}
			else   // Format Nr 2  ECHOTAG   Beschreibung bla bla bla
			{
				sscanf(line,"%s",tmp);  
				ar=tmp;
				t=&line[ar.GetLength()];
				while (*t)
				{
					if (*t==' ' || *t=='\t')
						t++;
					else
						break;
				}
				desc=t;
                trim_all(desc);

				sub=do_we_have_this_area_subscribed(PS ar);
				switch (sub)
				{

					default:
				    case 0:
					        status=" ";
							break;
					case 1:
					        status="O";
							break;
					case 2:
					        status="X";
							break;
				}
				desc.OemToAnsi();
				entry=ar+'\t'+status+'\t'+desc;
				areas.AddTail(entry);
			}
	   }
	   fclose(fp);
	}

	areas_org.RemoveAll();
	for (int k=0;k<areas.GetCount();k++)
	    areas_org.AddTail(areas.GetString(k));

	UPDATE_LB(areas,IDC_AREALIST);
}

// ===========================================================================
	void ecolst::display_boss_configs()
// ===========================================================================
// TOP Hier werden die Bosse und deren Echolisten geholt, ind die CStrList geschoben
// und dargestellt.
{
CString echofile,bossname,all,uplink;
char	echopath[MAX_PATH],buf[MAX_BOSSLEN];
int		i;

	bosses.RemoveAll();
	i=0;
	while (db_get_boss_by_index(i++,buf))
	{
	    get_token(buf,BOSSSYST,bossname);
	    get_token(buf,BOSSADDR,uplink);
	    get_token(buf,ECHOFILE,echofile);
		if (echofile.GetLength()<3)
			echofile=L("S_343");
		else
		{
			make_path(echopath,gc.UtilPath,echofile);
			if (access(echopath,0)!=0)
				echofile=L("S_346");
		}
		all=bossname+'\t'+echofile+'\t'+uplink;
		bosses.AddTail(all);
	}
	UPDATE_LB(bosses,IDC_BOSS);
}

// ===========================================================================
	void ecolst::build_available_echotags()
// ===========================================================================
// Erstelle eine Liste aller Echo's die wir haben
{
struct	areadef adef;
CString help;
int		count;
int		ret;
int		i;

	 avechotags.RemoveAll();
	 ret=db_get_count_of_areas(&count);
	 ASSERT(ret==DB_OK);
     for (i=0;i<count;i++)
	 {
         ret=db_get_area_by_index(i,&adef);
		 ASSERT(ret==DB_OK);
		 help.Format("%s\t%s\t\t",adef.echotag,adef.uplink);
		 avechotags.AddTail(help);
 	 }
}

// ===========================================================================
	int ecolst::do_we_have_this_area_subscribed(char *area)
// ===========================================================================
{
CString loc;
struct	areadef pa;
int		i;

   i=0;
   while (db_get_area_by_index(i++,&pa)==DB_OK)
   {
	  if (!stricmp(pa.echotag,area))
	  {
		  if (strlen(pa.uplink) >1)
		  {
			   if(!strcmp(pa.uplink,selecteduplink))
			       return 1;
			   else
			       return 2;
		  }
		  else
			 return 0;
	  }
   }
   return 0;
}

// ===========================================================================
	int ecolst::build_afix_netmail(int mode)
// ===========================================================================
{
mailheader mh;
CStrList *plst;
CString  bosscfg,from,to,frompt,topt,subject,INTL,tmp,FMPT,MSGID,POINT,PID;
CString  tearline,help,help2,addplus,mailout;
char	 buf[MAX_BOSSLEN],tmp2[100];
int      sel,i,ret;
char 	 *p;

	memset(&mh,0,sizeof(mailheader));
	sel=m_bosslist.GetCurSel();
	if (sel==LB_ERR)  return 0;
	BeginWaitCursor();
	ret=db_get_boss_by_index(sel,buf);
	ASSERT(ret==DB_OK);
	bosscfg=buf;

	// 0    char subject [72];				// Subject line
	get_token(bosscfg,AFIXPASW,subject); 
	strcpy(mh.subject,subject);

	get_token(bosscfg,MAINAKA,POINT);
	// 1	char datetime[20]; 				// Date
	build_fido_time(buf);	
	strcpy(mh.datetime,buf);
	// 2	char toname  [36];				// Receiver	Name
	get_token(bosscfg,AREAFIX,to); 
	strcpy(mh.toname,to);
	// 3	char fromname[36];				// Sender Name
	get_fullname(from);
	CharToOem(from,mh.fromname);
	// 4
	mh.structlen=sizeof(mailheader);
	// 5
	mh.status=DB_MAIL_CREATED;
	// 6,7
	mh.recipttime=mh.mailid=time(NULL);
	// 8 OFFSET: ignore
	// 9 kopiert an den Schluss
	// 10 INDEX :ignore
	// 11 FILLER:ignore
	// 12 ATTRIB: ignore
	// 13 COST:ignore
	// 14	srczone;
	// 15	srcnet;
	// 16	srcnode;
	// 17	srcpoint;
	get_token(bosscfg,MAINAKA,help);
	parse_address(help,&mh.srczone,&mh.srcnet,&mh.srcnode,&mh.srcpoint);
	// 18	unsigned short dstzone;		//
	// 19	unsigned short dstnet;			//
	// 20	unsigned short dstnode;		//
	// 21	unsigned short dstpoint;		//
	get_token(bosscfg,BOSSADDR,help);
	parse_address(help,&mh.dstzone,&mh.dstnet,&mh.dstnode,&mh.dstpoint);


	INTL.Format	("\001INTL %d:%d/%d %d:%d/%d\r",
				mh.dstzone,mh.dstnet,mh.dstnode,
				mh.srczone,mh.srcnet,mh.srcnode);
	FMPT.Format	("\001FMPT %d\r",mh.srcpoint);
	MSGID.Format("\001MSGID: %s %x\r",POINT,mh.mailid);
    PID.Format	("\001PID: %s\r",get_versioninfo(tmp2,0));

	// RemoveAll Mailtext
	mailout=INTL+FMPT+MSGID+PID+"\r\r";
	get_token(bosscfg,AFIXPLUS,addplus);

	if (mode==0 || mode==3)   // Differences only
		plst=&difflist;
	else
		plst=&areas;

	if (mode==3)
	{
		for (i=0;i<difflist.GetCount();i++)
		{
			char tmp1[300];
			tmp=difflist.GetString(i);
            trim_all(tmp);
			if (tmp[0]=='-')
			{
				strcpy(tmp1,tmp);
				tmp=&tmp1[1];

                trim_all(tmp);
				handle_uplink_remove_for_this_area(PS tmp);
			}
			mailout=mailout+difflist.GetString(i)+"\r";
		}
	}
	else
	{
		for (i=0;i< (*plst).GetCount();i++)
		{
			(*plst).GetString(i,help);
			get_token(help,0,help2);
			p= (char *) strchr(help,'\t');
			ASSERT(p);
			p++;

			if (*p=='X')	continue;
			if (*p=='O')
			{
				if (addplus[0]=='1')
					mailout+="+";
				mailout+=help2+"\r";
			}
			else if (*help2 != '%' && *help2 != '-' && *help2 != '*')
			{
				mailout+="-"+help2+"\r";
				handle_uplink_remove_for_this_area(PS help2);
			}
		}
	}
	mailout+="\r\r"+get_tearline(tearline,TRUE)+"\r";
	expand_tabs(mailout);
	mh.text_length=mailout.GetLength()+1;
	mh.mail_text=PS mailout;

	if (db_append_new_message(0,&mh) != DB_OK)
	{
	   EndWaitCursor();
	   ERR_MSG_RET0("E_APPMFAILED");
	}

	if (db_set_area_state(0,1) != DB_OK)
	{
		EndWaitCursor();
		ERR_MSG_RET0("E_MODFLAGSET");
	}
	EndWaitCursor();
	err_out("M_ECHOSEND");
	return 1;
 }

// =====================================
	void ecolst::OnManualOrder()
// =====================================
{
int k,sel;

   sel=m_bosslist.GetCurSel();
   if (sel==LB_ERR)
		ERR_MSG_RET("E_PSELABOS");

   ManualAreaBestell.RemoveAll();
   manual dlg;
   dlg.DoModal();
   if (ManualAreaBestell.GetCount()>0)
   {
	   err_out("M_SENDMANUAL");
	   difflist.RemoveAll();
	   for (k=0;k<ManualAreaBestell.GetCount();k++)
		   difflist.AddTail(ManualAreaBestell.GetString(k));
	   build_afix_netmail(3); 				// Manual stuff
   }
}

// =====================================
	void ecolst::OnHelp()
// =====================================
{
	WinHelp(VHELP_ECHO_ORDER);
}

BOOL ecolst::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
