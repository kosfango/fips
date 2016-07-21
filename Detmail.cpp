// detmail.cpp : implementation file
// IDD_STAT_MAILER

#include "stdafx.h"
#include "light.h"
#include "cfido.h"
#include <io.h>
#include "changed.h"
#include "lightdlg.h"
#include "events.h"
#include "gcomm.h"
#include "detmail.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
static char		DlgName[]="IDD_STAT_MAILER";

extern CLightApp	FipsApp;
extern detmail		gMailer;
extern CLightDlg	*gpMain;
extern _gconfig gc;
extern _gcomm	gcomm;
extern CStrList remote;
extern CStrList alt_phone;
CStrList        queue;
int	timer_was_here=0;

struct _grafstatus
{
	int posid;				// ID of Rectangle Element
	int *status;			// Zeiger auf die Pos wo der Status steht ...
	int dispstatus;			// Der gerade angezeigte Status
	int OnID;				// ID of image for ON state
	int OffID;				// ID of image for OFF state
};

void update_if_new(CEdit &ctl,const char *txt);
void update_if_new(CEdit &ctl,const int val);

struct _grafstatus GS[]=
{
	{IDC_MOD1IN,&gcomm.AcceptOn1Modem,-1,IDB_COM1_ON,IDB_COM1_OFF},
	{IDC_MOD2IN,&gcomm.AcceptOn2Modem,-1,IDB_COM2_ON,IDB_COM2_OFF},
	{IDC_ISDNIN,&gcomm.AcceptOnIsdn,-1,IDB_ISDN_ON,IDB_ISDN_OFF},
	{IDC_MOD1OUT,&gcomm.ConnectedOn1Modem,-1,IDB_COM1_ON,IDB_COM1_OFF},
	{IDC_MOD2OUT,&gcomm.ConnectedOn2Modem,-1,IDB_COM2_ON,IDB_COM2_OFF},
	{IDC_ISDNOUT,&gcomm.ConnectedOnIsdn,-1,IDB_ISDN_ON,IDB_ISDN_OFF}
};

static _DlgItemsSize DlgItemsSize [] =
{
	0,			     {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_MFNAME			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_MFNAME2			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_TRANSNAME		,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_TRANSNAME2		,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_MCPS			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_MCPS2			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_CPS				,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_CPS2			,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_MTIME			,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_MTIME2			,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_TIMEELAPSED		,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_TIMEELAPSED2	,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_MTOTAL			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_MTOTAL2			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_BYTESTOTAL		,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_BYTESTOTAL2		,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_MTRANSF			,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_MTRANSF2		,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_BYTESTRANSF		,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_BYTESTRANSF2	,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_MSTAT			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATUS			,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_PROZ			,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_PROZ2			,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_MSENDATA		,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_MRESDATA		,{0,0,0,0},{0,0,0,0},VE,VE,HV,HV,0,
	IDC_LIST			,{0,0,0,0},{0,0,0,0},0,0,HV,HV,0,
	IDC_MSESSINFO		,{0,0,0,0},{0,0,0,0},0,0,HV,HV,0,
	IDC_MOD1IN		    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_MOD2IN		    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_ISDNIN		    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_MOD1OUT		    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_MOD2OUT		    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_ISDNOUT		    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_MCALLS		    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_MCALLS2		    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_SHOW_QUEUE	    ,{0,0,0,0},{0,0,0,0},HO,HO,HO,HO,0,
	IDC_MSHOW		    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_MEVENTS		    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDHELP				,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDABORTSESSION		,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDSTART				,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDCANCEL			,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_START_EVENTS    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_STOP_EVENTS	    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_SETUP_EVENTS    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_MQUEUE		    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_REMOVECALL	    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_SKIPCALL	    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_SKIPFILE	    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_REMOVEFILE	    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_RESCAN		    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_MINFO		    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_SYSTEM		    ,{0,0,0,0},{0,0,0,0},HV,HV,HV,HV,0,
	IDC_STATIC8		    ,{0,0,0,0},{0,0,0,0},HO,HO,HV,HV,0,
	IDC_QUEUE		    ,{0,0,0,0},{0,0,0,0},HO,HO,HV,HV,0,
	IDC_EVENTLOG	    ,{0,0,0,0},{0,0,0,0},HO,HO,HV,HV,0,
};

// =====================================================
	detmail::detmail(CWnd* pParent ) : CDialog(detmail::IDD, pParent)
// =====================================================
{
	//{{AFX_DATA_INIT(detmail)
	//}}AFX_DATA_INIT
}

// =====================================================
	void detmail::DoDataExchange(CDataExchange* pDX)
// =====================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(detmail)
	DDX_Control(pDX, IDC_ISDNOUT, m_isdnout);
	DDX_Control(pDX, IDC_MOD2OUT, m_mod2out);
	DDX_Control(pDX, IDC_MOD1OUT, m_mod1out);
	DDX_Control(pDX, IDC_ISDNIN, m_isdnin);
	DDX_Control(pDX, IDC_MOD2IN, m_mod2in);
	DDX_Control(pDX, IDC_MOD1IN, m_mod1in);
	DDX_Control(pDX, IDC_TIMEELAPSED2, m_timeelapsed2);
	DDX_Control(pDX, IDC_BYTESTOTAL2, m_total2);
	DDX_Control(pDX, IDC_BYTESTRANSF2, m_transferred2);
	DDX_Control(pDX, IDC_CPS2, m_cps2);
	DDX_Control(pDX, IDC_TRANSNAME2, m_transname2);
	DDX_Control(pDX, IDC_PROZ2, m_proz2);
	DDX_Control(pDX, IDC_EVENTLOG, m_eventlog);
	DDX_Control(pDX, IDC_TRANSNAME, m_transname);
	DDX_Control(pDX, IDC_QUEUE, m_queue);
	DDX_Control(pDX, IDC_SYSTEM, m_system);
	DDX_Control(pDX, IDSTART, m_startstop);
	DDX_Control(pDX, IDC_TIMEELAPSED, m_timeelapsed);
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_PROZ, m_proz);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_CPS, m_cps);
	DDX_Control(pDX, IDC_BYTESTRANSF, m_transferred);
	DDX_Control(pDX, IDC_BYTESTOTAL, m_total);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(detmail, CDialog)
	//{{AFX_MSG_MAP(detmail)
	ON_LBN_DBLCLK(IDC_QUEUE, OnDblclkQueue)
	ON_LBN_SELCHANGE(IDC_QUEUE, OnSelchangeQueue)
	ON_EN_CHANGE(IDC_STATUS, OnChangeStatus)
	ON_EN_CHANGE(IDC_SYSTEM, OnChangeSystem)
	ON_BN_CLICKED(IDC_SKIPFILE, OnSkipfile)
	ON_BN_CLICKED(IDABORTSESSION, OnAbortsession)
	ON_BN_CLICKED(IDC_SHOW_QUEUE, OnShowQueue)
	ON_BN_CLICKED(IDC_START_EVENTS, OnStartEvents)
	ON_BN_CLICKED(IDC_STOP_EVENTS, OnStopEvents)
	ON_BN_CLICKED(IDSTART, OnStart)
	ON_BN_CLICKED(IDC_RESCAN, OnRescan)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_SETUP_EVENTS, OnSetupEvents)
	ON_BN_CLICKED(IDC_REMOVEFILE, OnRemovefile)
	ON_BN_CLICKED(IDCLEAR, OnClear)
	ON_BN_CLICKED(IDC_SKIPCALL, OnSkipItem)
	ON_BN_CLICKED(IDC_REMOVECALL, OnDelete)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// detmail message handlers

// =====================================================
	BOOL detmail::OnInitDialog()
// =====================================================
{
int tabs[]={15,30,60,95,300};

	CDialog::OnInitDialog();
	gc.mailer.thisptr=(void *)this;
	TABULATE_LB(IDC_QUEUE);
	EXTENT_LB(IDC_EVENTLOG,800);
	EXTENT_LB(IDC_LIST,500);
	set_transparent_bmp(&m_mod1in,IDB_COM1_OFF);
	set_transparent_bmp(&m_mod1out,IDB_COM1_OFF);
	set_transparent_bmp(&m_mod2in,IDB_COM2_OFF);
	set_transparent_bmp(&m_mod2out,IDB_COM2_OFF);
	set_transparent_bmp(&m_isdnin,IDB_ISDN_OFF);
	set_transparent_bmp(&m_isdnout,IDB_ISDN_OFF);
	m_proz.SetRange(0,100);
	m_proz2.SetRange(0,100);
/*	m_proz.SetPos(30);
	m_proz.SendMessage(PBM_SETBARCOLOR,0,(LPARAM)RGB(0,255,0));
	m_proz2.SetPos(50);
	m_proz2.SendMessage(PBM_SETBARCOLOR,0,(LPARAM)RGB(0,0,255));*/
	show_events=0;
	OnShowQueue();
	SetButtonsState();
// tooltips
	tip.Create(this);
	tip.SetMaxTipWidth(0xFFFFFF);
	tip.AddTool(GetDlgItem(IDSTART),"");
	tip.AddTool(GetDlgItem(IDABORTSESSION),"");
	tip.AddTool(GetDlgItem(IDC_PROZ),"");
	tip.AddTool(GetDlgItem(IDC_PROZ2),"");
	tip.AddTool(GetDlgItem(IDC_STATUS),"");
	tip.AddTool(GetDlgItem(IDC_BYTESTOTAL),"");
	tip.AddTool(GetDlgItem(IDC_BYTESTOTAL2),"");
	tip.AddTool(GetDlgItem(IDC_BYTESTRANSF),"");
	tip.AddTool(GetDlgItem(IDC_BYTESTRANSF2),"");
	tip.AddTool(GetDlgItem(IDC_CPS),"");
	tip.AddTool(GetDlgItem(IDC_CPS2),"");
	tip.AddTool(GetDlgItem(IDC_TIMEELAPSED),"");
	tip.AddTool(GetDlgItem(IDC_TIMEELAPSED2),"");
	tip.AddTool(GetDlgItem(IDC_TRANSNAME),"");
	tip.AddTool(GetDlgItem(IDC_TRANSNAME2),"");
	tip.AddTool(GetDlgItem(IDC_SHOW_QUEUE),"");
	tip.AddTool(GetDlgItem(IDC_SKIPCALL),"");
	tip.AddTool(GetDlgItem(IDC_REMOVECALL),"");
	tip.AddTool(GetDlgItem(IDC_SKIPFILE),"");
	tip.AddTool(GetDlgItem(IDC_REMOVEFILE),"");
	tip.AddTool(GetDlgItem(IDC_RESCAN),"");
	tip.AddTool(GetDlgItem(IDC_START_EVENTS),"");
	tip.AddTool(GetDlgItem(IDC_STOP_EVENTS),"");
	tip.AddTool(GetDlgItem(IDC_SETUP_EVENTS),"");
	tip.AddTool(GetDlgItem(IDC_SYSTEM),"");
	tip.Activate(!gc.NoTooltips);
	return TRUE;
}

// =====================================================
	void detmail::OnCancel()
// =====================================================
{
  ShowWindow(SW_HIDE);
	CDialog::OnCancel();
}

// =====================================================
	void detmail::OnClear()
// =====================================================
{
   m_list.ResetContent();
   m_proz.SetPos(0);
   m_proz2.SetPos(0);
}

// =============================================================
	void detmail::GraphicalStatusDisplay(int forceall)
// ============================================================
{
static	int lastninenine=0;
_udp_packet sendpacket;
_grafstatus	*gf;
int			one_was_shown=0;
int			dispid;

	gf=GS;
	memset(&sendpacket,0,sizeof(sendpacket));
	sendpacket.command=UDP_DEVICESTATUS;

	for (int i=0;i<(sizeof(GS)/sizeof(_grafstatus));i++)
	{
		// if status don't change - nothing to do
		if (!forceall && gf->dispstatus==*(gf->status) && *(gf->status)!=99)
		{
			gf++;
			continue;
		}

		// else show element
		if (*(gf->status)==99)
		{
			if (lastninenine==0)
			{
				dispid=gf->OnID;
				lastninenine=1;
			}
			else
			{
				dispid=gf->OffID;
				lastninenine=0;
			}
		}
		else
			dispid=*(gf->status) ? gf->OnID : gf->OffID;

		if (*(gf->status)==2)	dispid=gf->OffID;

		// for TCP/IP connections
		sendpacket.data[i]=(char)(dispid!=gf->OffID);
		one_was_shown=1;
		gf->dispstatus=*(gf->status);

		set_transparent_bmp((CStatic *)GetDlgItem(gf->posid),dispid);
		gf++;

		if (one_was_shown)	send_tcp_request(&sendpacket);
	}
}

// =====================================================
	int prepare_one_entry_for_dialing(int sel,_curmail *cm)
// =====================================================
{
CString addr,phone,pwd,aka;
CFido	cf;
char	buf[200],path[MAX_PATH];

	memset(cm,0,sizeof(_curmail));
	if (sel > gMailer.m_queue.GetCount())
	    return 0;
	gMailer.m_queue.GetText(sel,buf);

	get_token(buf,4,phone);
	get_token(buf,5,pwd);
	get_token(buf,1,addr);
	get_token(buf,6,aka);

	if (addr[0]=='>')
	   addr=addr.Mid(3);

	cf=PS addr;
	addr=cf.GetAsDir();

	strcpy(curmail.phone,phone);
	make_path(path,gc.OutboundPath,addr);
	strcpy(curmail.basedir,path);
	strcpy(curmail.passwd,pwd);
	strcpy(curmail.myaka,aka);

	return 1;
}

// =====================================================
	void detmail::OnStart()
// =====================================================
{
	if (gc.mailer.running)	return;
	if (gc.ineventaction)	ERR_MSG_RET("E_EVCIACT");
	if (m_queue.GetCount()==0)	OnRescan();
	if (m_queue.GetCount()==0)
	{
		update_queue();
		return;
	}
	alt_phone.RemoveAll();
	FipsApp.OnStartmailer();
}

// =====================================================
	void detmail::OnAbortsession()
// =====================================================
{
	if (gc.mailer.keyboard_break==1 || !gc.mailer.running)	return;
	gc.mailer.keyboard_break=1;
	m_status.SetWindowText(L("S_75"));	// abort pending
	SetTimer(2345,1000,0);
}

// =====================================================
// updates info fields in mailer dialog
	void detmail::OnUpdateValues(void)
// =====================================================
{
_mailer *pdm=&gc.mailer;
char	 buf[500];

	sprintf(buf,"%s\r\n%s\r\n%s",pdm->m_sysop,pdm->m_system,pdm->m_location);
	update_if_new(m_system,buf);

	sprintf(buf,"%02d:%02d:%02d %s",
		pdm->m_timeelapsed/3600,(pdm->m_timeelapsed%3600)/60,pdm->m_timeelapsed%60,pdm->m_charge);
	update_if_new(m_timeelapsed,buf);

	sprintf(buf,"%02d:%02d:%02d %s",
		pdm->m_timeelapsed2/3600,(pdm->m_timeelapsed2%3600)/60,pdm->m_timeelapsed2%60,pdm->m_charge);
	update_if_new(m_timeelapsed2,buf);
	update_if_new(m_status,pdm->m_status);
	update_if_new(m_cps,pdm->m_cps);
	update_if_new(m_cps2,pdm->m_cps2);
	update_if_new(m_transferred,pdm->m_transferred);
	update_if_new(m_transferred2,pdm->m_transferred2);
	update_if_new(m_total,pdm->m_total);
	update_if_new(m_total2,pdm->m_total2);
	update_if_new(m_transname,pdm->m_filename);
	update_if_new(m_transname2,pdm->m_filename2);

	m_proz.SetPos(pdm->m_proz);
	m_proz2.SetPos(pdm->m_proz2);
	update_left_side();
	update_right_side();
}

// =====================================================
// adds timed string to listbox
	void detmail::OnAddToListBox(char *text)
// =====================================================
{
char	buf[16384];
char	buf2[100];
int		ret;

	m_list.SetRedraw(0);
	_strtime(buf2);
	sprintf(buf,"%s  %s",buf2,text);
	m_list.AddString(buf);
	send_datagram(buf);
	ret=m_list.GetCount()-2;
	m_list.SetTopIndex(ret<0 ? 0 : ret);
	m_list.SetRedraw(1);
}

// =====================================================
	void detmail::OnRescan()
// =====================================================
{
	if (!gc.WeComeFromEventScript && gc.mailer.running)
		ERR_MSG_RET("M_STOPMFIRST");

	BeginWaitCursor();
  m_status.SetWindowText(L("S_382"));	// rescanning
	scan_database();
	fill_queuelist();
  m_status.SetWindowText(L("S_271"));	// idle
	save_current_mail_index();
	gpMain->show_subjects(gustat.cur_area_handle);
  restore_current_mail_index();
	EndWaitCursor();
	m_list.SetFocus();
}

// ============================================
	void detmail::fill_queuelist(void)
// ============================================
{
CStringList alst;
CStrList	lst;
CFido			cf;
CString		addr,type,name,pwd,phone,aka,mainaka,noadd,str;
char			x[500],x1[500],buf[MAX_BOSSLEN],path[MAX_PATH];
int				i;

// list of bosses
	queue.RemoveAll();
	i=0;
	while (db_get_boss_by_index(i,buf))
	{
		get_token(buf,NOTSEND,noadd);
		if (noadd != "1")
		{
			get_token(buf,BOSSSYST,name);
			get_token(buf,BOSSADDR,addr);
			get_token(buf,BOSSPHON,phone);
			get_token(buf,MAINAKA,aka);
			get_token(buf,SESSPASW,pwd);

			if (i==0)		  	// default-aka for first uplink
				mainaka = aka;

			cf=PS addr;
			make_path(path,gc.OutboundPath,cf.GetAsDir());
			strcat(path,"\\");
			build_flags_string(path,"BOSS",type);
			str="\t"+addr+"\t"+type+"\t"+name+"\t"+phone+"\t"+pwd+"\t"+aka;
			queue.AddTail(str);
			alst.AddTail(addr);
		}
		i++;
	}

// list of other nodes
	lst.FillWithDirs(gc.OutboundPath);
	lst.DeleteEmptySubDirs(gc.OutboundPath);
	lst.FillWithDirs(gc.OutboundPath);
	for (i=0;i<lst.GetCount();i++)
	{
		cf=PS lst.GetString(i);
		addr=cf.Get(x);
		if (alst.Find(addr) != NULL)	continue;	// skip duplicates
		make_path(path,gc.OutboundPath,lst.GetString(i));
		strcat(path,"\\");
		build_flags_string(path,"FIDO",type);
		cf.GetSystemName(x);
		name=x;
	  if (cf.GetPhoneAndPwd(x,x1) == 0)
		{
		   err_out("E_UNKNOWN_BOSS",addr);
		   continue;
		}
		str.Format("\t%s\t%s\t%s\t%s\t%s\t%s",addr,type,name,x,x1,mainaka);
    queue.AddTail(str);
		alst.AddTail(addr);
	}
	UPDATE_LB(queue,IDC_QUEUE);
 	update_queue();
}
	
// =====================================================
// makes outbound info string according to folder contents
	void detmail::build_flags_string(LPCSTR path,LPCSTR init,CString &type)
// =====================================================
{
WIN32_FIND_DATA wf;
HANDLE	hnd;
char		tmp[MAX_PATH];

	type=init;

	make_path(tmp,path,XFER_LIST);
	if (!access(tmp,0))	type+=",TF";

	make_path(tmp,path,FREQ_LIST);
	if (!access(tmp,0))	type+=",FR";

	make_path(tmp,path,"*.PKT");
	hnd=FindFirstFile(tmp,&wf);
	if(hnd != INVALID_HANDLE_VALUE) goto found;
	make_path(tmp,path,"*.SU*");
	hnd=FindFirstFile(tmp,&wf);
	if(hnd != INVALID_HANDLE_VALUE) goto found;
	make_path(tmp,path,"*.MO*");
	hnd=FindFirstFile(tmp,&wf);
	if(hnd != INVALID_HANDLE_VALUE) goto found;
	make_path(tmp,path,"*.TU*");
	hnd=FindFirstFile(tmp,&wf);
	if(hnd != INVALID_HANDLE_VALUE) goto found;
	make_path(tmp,path,"*.WE*");
	hnd=FindFirstFile(tmp,&wf);
	if(hnd != INVALID_HANDLE_VALUE) goto found;
	make_path(tmp,path,"*.TH*");
	hnd=FindFirstFile(tmp,&wf);
	if(hnd != INVALID_HANDLE_VALUE) goto found;
	make_path(tmp,path,"*.FR*");
	hnd=FindFirstFile(tmp,&wf);
	if(hnd != INVALID_HANDLE_VALUE) goto found;
	make_path(tmp,path,"*.SA*");
	hnd=FindFirstFile(tmp,&wf);
	if(hnd != INVALID_HANDLE_VALUE) goto found;

	return;
found:
	FindClose(hnd);
	type+=",MO";
}

// =====================================================
	void detmail::OnSkipItem()  // SKIP Button
// =====================================================
{
	if (!gc.mailer.running || gc.mailer.keyboard_break)	return;
  gc.mailer.skip_only=1;
	gc.mailer.keyboard_break=1;
}

// =====================================================
	void detmail::OnSkipfile() 
// =====================================================
{
	if (!gc.mailer.running || gc.mailer.keyboard_break)	return;
	if (gc.mailer.skip_file==0 && err_out("DY_SKIPFILE")==IDYES)	gc.mailer.skip_file=1;
}

// ===============================================
	void detmail::OnRemovefile() 
// ===============================================
{
	if (!gc.mailer.running || gc.mailer.keyboard_break)	return;
	if (gc.mailer.skip_file==0 && err_out("DY_REMOVEFILE")==IDYES)	gc.mailer.skip_file=-1;
}

// =====================================================
	void detmail::OnDelete()
// =====================================================
{
int  entry[2];

	if (gc.mailer.running)	ERR_MSG_RET("M_STOPMFIRST");
	if (!m_queue.GetSelItems(1,entry))	return;
	if (err_out("REALLY_DELETE")==IDYES)
	{
		queue.Remove(entry[0]);
		UPDATE_LB(queue,IDC_QUEUE);
	}
}

// =====================================================
	void detmail::OnTimer(UINT nIDEvent)
// =====================================================
{
	timer_was_here++;
// wait till keyboard_break acknowledge is received
	if (gc.mailer.keyboard_break==3 || gc.mailer.keyboard_break==0)
	{
		gc.mailer.keyboard_break=0;
		m_status.SetWindowText(L("S_76"));	// aborted
		KillTimer(2345);
		update_queue();
	}
	else if (timer_was_here >25)
	{
		KillTimer(2345);
		gc.mailer.keyboard_break=0;
		gc.mailer.skip_only=0;
		m_status.SetWindowText(L("S_74"));	// abort failed
		update_queue();
	}
	CDialog::OnTimer(nIDEvent);
}

// ===============================================
	void detmail::OnDblclkQueue()
// ===============================================
{
CString str;
int		sel;

	if (gc.mailer.running)	ERR_MSG_RET("M_STOPMFIRST");
	sel=m_queue.GetCurSel();
	LB_ERR_RET;
	m_queue.GetText(sel,str);
	strcpy(gc.changed,str);
	changed dlg;
	if (dlg.DoModal()==IDOK)
	{
		m_queue.DeleteString(sel);
		m_queue.InsertString(sel,gc.changed);
	}
}

// ===============================================
	void detmail::OnSelchangeQueue()
// ===============================================
{
	update_queue();
	if (gc.mailer.running)	ERR_MSG_RET("M_STOPMFIRST");
}

// ===========================================
	void detmail::SetLanguage(void)
// ===========================================
{
int  lng[]={
					IDCANCEL,
					IDSTART,
					IDABORTSESSION,
					IDC_MTOTAL,
					IDC_MTRANSF,
					IDC_MCPS,
					IDC_MTIME,
					IDC_MSTAT,
					IDC_MINFO,
					IDC_REMOVECALL,
					IDC_SKIPCALL,
					IDC_RESCAN,
					IDC_MFNAME,
					IDC_MSESSINFO,
					IDC_MEVENTS,
					IDC_START_EVENTS,
					IDC_STOP_EVENTS,
					IDC_SETUP_EVENTS,
					IDC_MCALLS,
					IDC_SHOW_QUEUE,
					IDC_MQUEUE,
					IDHELP,
					IDC_MTOTAL2,
					IDC_MTRANSF2,
					IDC_MCPS2,
					IDC_MTIME2,
					IDC_MFNAME2,
					IDC_MSENDATA,
					IDC_MRESDATA,
					IDC_MCALLS2,
					IDC_SKIPFILE,
					IDC_REMOVEFILE
	};
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
}

// ===============================================
	void detmail::OnShowQueue()
// ===============================================
{
	if (show_events)
	{
		m_queue.ShowWindow(SW_HIDE);
		m_eventlog.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_SHOW_QUEUE)->SetWindowText(L("S_629"));	// event log
	}
	else
	{
		m_queue.ShowWindow(SW_SHOWNORMAL);
		m_eventlog.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SHOW_QUEUE)->SetWindowText(L("S_309"));	// mailer queue
	}
	show_events=!show_events;
}

// ===============================================
	void detmail::OnStartEvents()
// ===============================================
{
	gc.eventsenabled=1;
	event_add_listbox(L("S_190"));	// enabling events
	show_events_status();
	SetButtonsState();
}

// ===============================================
	void detmail::OnStopEvents()
// ===============================================
{
	gc.eventsenabled=0;
	event_add_listbox(L("S_167"));	// disabling events
	SetButtonsState();
}

// ===============================================
	void detmail::OnSetupEvents()
// ===============================================
{
	events dlg;
	dlg.scheduled=0;
	dlg.DoModal();
}

// ===============================================
// switch enabling buttons
	void detmail::SetButtonsState()
// ===============================================
{
 	if (gc.eventsenabled)
	{
		DISABLEID(IDC_START_EVENTS);
		ENABLEID(IDC_STOP_EVENTS);
	}
	else
	{
		ENABLEID(IDC_START_EVENTS);
		DISABLEID(IDC_STOP_EVENTS);
	}
}

// ===============================================
	void detmail::update_left_side(void)
// ===============================================
{
_udp_packet sp;
CString fname,cps,time,total,transf,status,str;

	if (!remote.defaultindex)	return;
	m_transname.GetWindowText(fname);
	m_cps.GetWindowText(cps);
	m_timeelapsed.GetWindowText(time);
	m_total.GetWindowText(total);
	m_transferred.GetWindowText(transf);
	m_status.GetWindowText(status);
	str.Format("%s\t%s\t%s\t%s\t%s\t%s\t%d",fname,cps,time,total,transf,status,gc.mailer.m_proz);
	memset(&sp,0,sizeof(sp));
	sp.command=UDP_UPDATE_LEFT;
	strcpy(sp.data,str.Left(99));
	send_tcp_request(&sp);
}

// ===============================================
	void detmail::update_right_side(void)
// ===============================================
{
_udp_packet sp;
CString str;

	if (!remote.defaultindex)	return;
	m_system.GetWindowText(str);
	str.Replace("\r\n","\t");
	memset(&sp,0,sizeof(_udp_packet));
	sp.command=UDP_UPDATE_RIGHT;
	strcpy(sp.data,str.Left(99));
	send_tcp_request(&sp);
}

// ===============================================
	void detmail::update_queue(void)
// ===============================================
//#define		UDP_CLEAR_QUEUE			4
//#define		UDP_ADD_QUEUE			5
//#define		UDP_QUEUE_SEL			6
{
_udp_packet sp;
CString str;
int		entry[100],i;

	if (!remote.defaultindex)	return;
	memset(&sp,0,sizeof(sp));
	sp.command=UDP_CLEAR_QUEUE;
	send_tcp_request(&sp);

	for (i=0;i<m_queue.GetCount();i++)
	{
		m_queue.GetText(i,str);
		memset(&sp,0,sizeof(sp));
		sp.command=UDP_ADD_QUEUE;
		strcpy(sp.data,str);
		send_tcp_request(&sp);
	}
	for (i=0;i<m_queue.GetSelItems(99,entry);i++)
	{
		memset(&sp,0,sizeof(sp));
		sp.command=UDP_QUEUE_SEL;
		sp.subcommand=entry[i];
		send_tcp_request(&sp);
	}
}

// ===============================================
	void detmail::OnChangeStatus()
// ===============================================
{
	update_left_side();
}

// ===============================================
	void detmail::OnChangeSystem()
// ===============================================
{
	update_right_side();
}

// ===============================================
	BOOL detmail::PreTranslateMessage(MSG* pMsg)
// ===============================================
{
	if (tip)	tip.RelayEvent(pMsg);
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam=='1' && (GetKeyState(VK_CONTROL) & 0x80000000))
		OnCancel();
	if (handle_function_key(pMsg))	return 1;
	return CDialog::PreTranslateMessage(pMsg);
}

// ===============================================
	void detmail::OnMove(int x, int y)
// ===============================================
{
static bool count=true;

	CDialog::OnMove(x, y);

	if (count)
	{
		count=false;
		return;
	}

	if (IsWindow(m_hWnd))
		GraphicalStatusDisplay(1);
}

// ===============================================
	void detmail::OnPaint()
// ===============================================
{
	CPaintDC dc(this); // device context for painting
	GraphicalStatusDisplay(1);
	DrawRightBottomKnubble(this,dc);
	// Do not call CDialog::OnPaint() for painting messages
}

// ===============================================
	void detmail::OnClose()
// ===============================================
{
	OnCancel();
}

// ============================================
	void detmail::SavePosition(void)
// ============================================
{
    StoreInitDialogSize(DlgName,this);
}

// ============================================
	void detmail::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void detmail::RefreshTips(void) 
// ============================================
{
	tip.UpdateTipText(L("S_563"),GetDlgItem(IDSTART));
	tip.UpdateTipText(L("S_564"),GetDlgItem(IDABORTSESSION));
	tip.UpdateTipText(L("S_565"),GetDlgItem(IDC_PROZ));
	tip.UpdateTipText(L("S_566"),GetDlgItem(IDC_PROZ2));
	tip.UpdateTipText(L("S_567"),GetDlgItem(IDC_STATUS));
	tip.UpdateTipText(L("S_568"),GetDlgItem(IDC_BYTESTOTAL));
	tip.UpdateTipText(L("S_569"),GetDlgItem(IDC_BYTESTOTAL2));
	tip.UpdateTipText(L("S_570"),GetDlgItem(IDC_BYTESTRANSF));
	tip.UpdateTipText(L("S_571"),GetDlgItem(IDC_BYTESTRANSF2));
	tip.UpdateTipText(L("S_572"),GetDlgItem(IDC_CPS));
	tip.UpdateTipText(L("S_573"),GetDlgItem(IDC_CPS2));
	tip.UpdateTipText(L("S_574"),GetDlgItem(IDC_TIMEELAPSED));
	tip.UpdateTipText(L("S_575"),GetDlgItem(IDC_TIMEELAPSED2));
	tip.UpdateTipText(L("S_576"),GetDlgItem(IDC_TRANSNAME));
	tip.UpdateTipText(L("S_577"),GetDlgItem(IDC_TRANSNAME2));  
	tip.UpdateTipText(L("S_578"),GetDlgItem(IDC_SHOW_QUEUE));
	tip.UpdateTipText(L("S_579"),GetDlgItem(IDC_SKIPCALL));
	tip.UpdateTipText(L("S_580"),GetDlgItem(IDC_REMOVECALL));
	tip.UpdateTipText(L("S_581"),GetDlgItem(IDC_SKIPFILE));
	tip.UpdateTipText(L("S_582"),GetDlgItem(IDC_REMOVEFILE));
	tip.UpdateTipText(L("S_583"),GetDlgItem(IDC_RESCAN));
	tip.UpdateTipText(L("S_584"),GetDlgItem(IDC_START_EVENTS));
	tip.UpdateTipText(L("S_585"),GetDlgItem(IDC_STOP_EVENTS));
	tip.UpdateTipText(L("S_586"),GetDlgItem(IDC_SETUP_EVENTS));
	tip.UpdateTipText(L("S_587"),GetDlgItem(IDC_SYSTEM));
}

// ============================================
	void update_if_new(CEdit &ctl,LPCSTR txt)
// ============================================
{
CString str;

	ctl.GetWindowText(str);
	if (strcmp(str,txt))
		ctl.SetWindowText(txt);
}

// ============================================
	void update_if_new(CEdit &ctl,const int val)
// ============================================
{
char str[100],str1[100];

	sprintf(str1,"%d",val);
	ctl.GetWindowText(str,100);
	if (strcmp(str,str1))
		ctl.SetWindowText(str1);
}

// ============================================
	void detmail::OnShowWindow(BOOL bShow, UINT nStatus) 
// ============================================
{
	CDialog::OnShowWindow(bShow, nStatus);
	(gpMain->GetMenu())->CheckMenuItem(ID_SWITCH_MAILER, 
		MF_BYCOMMAND | (bShow ? MF_CHECKED : MF_UNCHECKED));
	if (!bShow)	gpMain->m_subjlist.SetFocus();
}

// ===============================================
	void detmail::OnHelp()
// ===============================================
{
	WinHelp(VHELP_STAT_MAILER);
}

// ===============================================
	BOOL detmail::OnHelpInfo(HELPINFO* pHelpInfo) 
// ===============================================
{
	OnHelp();
	return TRUE;
}
