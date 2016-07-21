// cfg_area.cpp : implementation file
// IDD_CFG_AREA

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_area.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern	_gconfig gc;
FL_NOTIFY Notify;

#define DEFC  FL_DEFCOLOR
#define RED   RGB(255,0,0)
#define BLUE  RGB(0,0,255)
#define GREEN RGB(0,150,0)
#define DEF_R 0	// default
#define NEW_R 1	// new
#define MOD_R 2	// modified
#define DEL_R 4	// deleted

static char DlgName[]="IDD_CFG_AREA";

/////////////////////////////////////////////////////////////////////////////
// cfg_area dialog

cfg_area::cfg_area(CWnd* pParent /*=NULL*/)
	: CDialog(cfg_area::IDD, pParent)
{
	//{{AFX_DATA_INIT(cfg_area)
	m_aka = _T("");
	m_areatag = _T("");
	m_description = _T("");
	m_group = _T("");
	m_ccarea = FALSE;
	m_adddays = _T("");
	m_number = _T("");
	m_create = _T("");
	m_recipt = _T("");
	m_maxmails = _T("");
	//}}AFX_DATA_INIT
}

void cfg_area::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_area)
	DDX_Control(pDX, IDC_UPLINK, m_uplink);
	DDX_Control(pDX, IDC_LIST, m_listctl);
	DDX_Text(pDX, IDC_AKA, m_aka);
	DDV_MaxChars(pDX, m_aka, 20);
	DDX_Text(pDX, IDC_AREATAG, m_areatag);
	DDV_MaxChars(pDX, m_areatag, 100);
	DDX_Text(pDX, IDC_DESCRIPTION, m_description);
	DDV_MaxChars(pDX, m_description, 128);
	DDX_Text(pDX, IDC_GROUP, m_group);
	DDV_MaxChars(pDX, m_group, 10);
	DDX_Check(pDX, IDC_CCAREA, m_ccarea);
	DDX_Text(pDX, IDC_ADDDAYS, m_adddays);
	DDV_MaxChars(pDX, m_adddays, 5);
	DDX_Text(pDX, IDC_NUMBER, m_number);
	DDV_MaxChars(pDX, m_number, 8);
	DDX_Text(pDX, IDC_CREATE, m_create);
	DDX_Text(pDX, IDC_RECIPT, m_recipt);
	DDX_Text(pDX, IDC_MAXMAILS, m_maxmails);
	DDX_Control(pDX, IDC_STATIC8, m_newrec);
	DDX_Control(pDX, IDC_STATIC9, m_modrec);
	DDX_Control(pDX, IDC_STATIC10, m_delrec);
	DDX_Control(pDX, IDC_STATIC11, m_totrec);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_area, CDialog)
	//{{AFX_MSG_MAP(cfg_area)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_DESCR, OnLookForDescription)
	ON_BN_CLICKED(IDC_SETGROUP, OnSetGroup)
	ON_NOTIFY(FLNM_SELECT, IDC_LIST, OnListEvent)
	ON_BN_CLICKED(IDC_RENUM, OnRenum)
	ON_CBN_SELENDOK(IDC_UPLINK, OnSelendokUplink)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_area::GetName(void)	{return DlgName;}
// =====================================================================

// ====================================================
	BOOL cfg_area::OnInitDialog()
// ====================================================
{
CString str;
HFROW hfr,hfc,tr;
int 	i;
char	buf[MAX_BOSSLEN];
areadef ad;
_flistcol lc[]={170,"tag",FL_LEFT,200,"desc",FL_LEFT,100,"AKA",FL_LEFT,
	80,"upl",FL_LEFT,40,"crt",FL_CENTER,40,"rcp",FL_CENTER,
	40,"max",FL_CENTER,40,"add",FL_CENTER,25,"cc",FL_CENTER,
	70,"file",FL_CENTER,0,"flg",0};
int  	lng[]={
				IDC_MAXTXT,
				IDC_CREATETXT,
				IDC_RECIPTTXT,
				IDC_ADD,
				IDC_DELETE,
				IDC_DEFAULT,
				IDC_STATIC1,
				IDC_STATIC2,
				IDC_STATIC3,
				IDC_STATIC4,
				IDC_STATIC6,
				IDC_STATIC5,
				IDC_DESCR,
				IDC_CCAREA,
				IDC_STATIC55,
				IDC_SETGROUP,
				IDC_RENUM,
				IDC_CHANGE,
				IDOK,
				IDCANCEL,
				IDHELP
				};

	CDialog::OnInitDialog();
	gc.areasetup_active=1;

	strcpy(lc[0].hdr,L("S_92"));	// area
	strcpy(lc[1].hdr,L("S_162"));	// description
	strcpy(lc[3].hdr,L("S_454"));	// uplink
	strcpy(lc[4].hdr,L("S_72"));	// creation date
	strcpy(lc[5].hdr,L("S_73"));	// reciption date
	strcpy(lc[6].hdr,L("S_314"));	// max mails
	strcpy(lc[7].hdr,L("S_53"));	// +days
	strcpy(lc[8].hdr,L("S_127"));	// cc
	strcpy(lc[9].hdr,L("S_349"));	// number
//	lc[10] - flag: 0-no changes, 1-added, 2-changed, 4-deleted
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	MakeColumns(lc,sizeof(lc)/sizeof(_listcol));
	m_listctl.SetTree(0);


    db_open_area_descriptions(1);	  // Make sure db is open
	i=0;
	hfr=NULL;
	while (db_get_area_by_index(i++,&ad)==DB_OK)
	{
		tr=m_listctl.FindIndex(FL_ROOT,0);
		if (tr)	hfr=m_listctl.FindFRow(tr,tr,0,ad.group);
		if (!tr || !hfr)
		{
			hfr=m_listctl.InsertFRow(FL_ROOT,FL_LAST,ad.group);
			m_listctl.SetFRowBold(hfr);
		}
		hfc=m_listctl.InsertFRow(hfr,FL_LAST,ad.echotag);
		m_listctl.SetFItemText(hfc,1,ad.description);
		m_listctl.SetFItemText(hfc,2,ad.aka);
		m_listctl.SetFItemText(hfc,3,ad.uplink);
		m_listctl.SetFItemNum(hfc,4,ad.purge_age_create);
		m_listctl.SetFItemNum(hfc,5,ad.purge_age_recipt);
		m_listctl.SetFItemNum(hfc,6,ad.purge_max_num);
		m_listctl.SetFItemNum(hfc,7,ad.additional_days);
		m_listctl.SetFItemNum(hfc,8,ad.localmail);
		m_listctl.SetFItemText(hfc,9,ad.filename);
		if (ad.delflag)
		{
			m_listctl.SetColor(FL_FROW,hfc,0,RED);
			m_listctl.SetFRowBold(hfc);
			m_listctl.SetFItemNum(hfc,10,DEL_R);
		}
		else
			m_listctl.SetFItemNum(hfc,10,DEF_R);
	}
// build UPLINK list
	m_uplink.ResetContent();
	i=0;
	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,BOSSADDR,str);
		m_uplink.AddString(str);
	}

	m_uplink.SetCurSel(0);
	hfr=m_listctl.FindIndex(FL_ROOT,0);
	m_listctl.Select(hfr);
	m_listctl.Collapse(hfr,FALSE);
	SetFields();
	CountRecs();
	m_listctl.SetFocus();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// cfg_area message handlers

// ====================================================
	void cfg_area::OnAdd()
// ====================================================
{
HFROW sel,tr;
BOOL fnew=FALSE;
	
	UpdateData(1);
	if (m_areatag.IsEmpty())	ERR_MSG_RET("E_INVALIDECHO");
	if (m_listctl.FindFRow(0,0,0,m_areatag,FALSE))	ERR_MSG_RET("E_INVALIDECHO");
	if (m_group.IsEmpty())	ERR_MSG_RET("E_INVALIDGROUP");
// find group
	tr=m_listctl.FindIndex(FL_ROOT,0);
	sel=m_listctl.FindFRow(tr,tr,0,m_group);
	if (!sel)
	{
		sel=m_listctl.InsertFRow(FL_ROOT,FL_LAST,m_group);
		m_listctl.SetColor(FL_FROW,sel,0,GREEN);
		m_listctl.SetFRowBold(sel);
		m_listctl.Collapse(sel,FALSE);
	}
	tr=m_listctl.InsertFRow(sel,FL_LAST,m_areatag);
	GetFields(tr,NEW_R);
	CountRecs();
}

// ====================================================
	void cfg_area::OnChange()
// ====================================================
{
CString oldg;
HFROW sel,tr,tp;

	oldg=m_group;
	UpdateData(1);
	if (m_areatag.IsEmpty())	ERR_MSG_RET("E_INVALIDECHO");
	sel=Notify.hFRow;
	if (sel) 
	{
		tr=m_listctl.FindFRow(NULL,NULL,0,m_areatag,FALSE);
		if (tr && tr!=sel)	ERR_MSG_RET("E_INVALIDECHO");
		if (m_group.IsEmpty())	ERR_MSG_RET("E_INVALIDGROUP");
		if (m_group!=oldg)	// group changed
		{
// look for group
			tr=m_listctl.FindIndex(FL_ROOT,0);
			tp=m_listctl.FindFRow(tr,tr,0,m_group);
// add new group
			if (!tp)
			{
				tp=m_listctl.InsertFRow(FL_ROOT,FL_LAST,m_group);
				m_listctl.SetColor(FL_FROW,tp,0,GREEN);
				m_listctl.SetFRowBold(tp);
				m_listctl.Collapse(tp,FALSE);
			}
// add changed row
			tr=m_listctl.InsertFRow(tp,FL_LAST,m_areatag);
			GetFields(tr,m_listctl.GetFItemNum(tr,10) | MOD_R);
// delete old row and may be group
			tp=m_listctl.GetNextFRow(sel,FL_PARENT);
			m_listctl.DeleteFRow(sel);
			if (m_listctl.GetFRowCount(tp)==0)
				m_listctl.DeleteFRow(tp);
		}
		else
			GetFields(sel,m_listctl.GetFItemNum(sel,10) | MOD_R);
	}
	CountRecs();
}

// ====================================================
	void cfg_area::OnDelete()
// ====================================================
{
CString str;
HFROW sel,tp;
int flag;

	sel=Notify.hFRow;
	if (sel) 
	{
		str=m_listctl.GetFItemText(sel,0);
		if (str=="NETMAIL" || str=="LOCALMAIL")	ERR_MSG_RET("E_CDALON");// netmail & localmail
		flag=m_listctl.GetFItemNum(sel,10);
		if (flag & NEW_R)
		{
			tp=m_listctl.GetNextFRow(sel,FL_PARENT);
			m_listctl.DeleteFRow(sel);
			if (m_listctl.GetFRowCount(tp)==0)
				m_listctl.DeleteFRow(tp);
			CountRecs();
			return;
		}
		flag^=DEL_R;
		GetFields(sel,flag);
	}
	CountRecs();
	m_listctl.SetFocus();
}

// ====================================================
	void cfg_area::OnBnClickedOk()
// ====================================================
{
CString grp;
HFROW	hp,hr;
areadef ad;
long	dm;
int		op;

	if (gc.tosser.running || gc.purger.running)	ERR_MSG_RET("E_PMNCWTR");
	UpdateData(1);
	db_open_area_descriptions (1);        // Make sure db is open
	for(hp=m_listctl.FindIndex(FL_ROOT,0);hp;hp=m_listctl.GetNextFRow(hp))
	{
		grp=m_listctl.GetFItemText(hp,0);
		hr=m_listctl.GetNextFRow(hp,FL_FIRST_CHILD);
		while(hr)
		{
			op=m_listctl.GetFItemNum(hr,10);
			if (op == DEF_R)	goto next;
			memset(&ad,0,sizeof(ad));
			if (op & (MOD_R | DEL_R))
			{
				db_get_area_by_name(m_listctl.GetFItemText(hr,0),&ad);
				if (op & DEL_R)
				{
					ad.delflag=1;
					db_change_area_properties(&ad);
					goto next;
				}
			}
			
			strcpy(ad.echotag,m_listctl.GetFItemText(hr,0));
			strcpy(ad.description,m_listctl.GetFItemText(hr,1));
			strcpy(ad.aka,m_listctl.GetFItemText(hr,2));
			strcpy(ad.uplink,m_listctl.GetFItemText(hr,3));
			strcpy(ad.group,grp);
			ad.purge_age_create=m_listctl.GetFItemNum(hr,4);
			ad.purge_age_recipt=m_listctl.GetFItemNum(hr,5);
			ad.purge_max_num=m_listctl.GetFItemNum(hr,6);
			ad.additional_days=m_listctl.GetFItemNum(hr,7);
			ad.localmail=m_listctl.GetFItemNum(hr,8);
			
			if (op & NEW_R)
				db_append_new_area(&ad,&dm,ad.uplink,1);
			else if (op & MOD_R)
				db_change_area_properties(&ad);
next:
			hr=m_listctl.GetNextFRow(hr);
		}
	}
	gc.areasetup_active=0;
//	if(m_bCloseOnOk)
	CDialog::OnOK();
}

// ====================================================
	void cfg_area::OnBnClickedCancel()
// ====================================================
{
	gc.areasetup_active=0;
	CDialog::OnCancel();
}

// ====================================================
	void cfg_area::OnLookForDescription()
// ====================================================
{
char	desc[300];
	
	UpdateData(1);
	if (!m_areatag.IsEmpty())
	{
		if (db_find_area_description(m_areatag,desc))
		{
			m_description=desc;
			m_description.OemToAnsi();
			UpdateData(0);
			return;
		}
	}
	show_msg(L("S_347"));	// not found
}

// ====================================================
	void cfg_area::OnRenum() 
// ====================================================
{
areadef ad;
CString	str;
int		i;

	UpdateData(1);
	str.Format("%08d",atoi(m_number));
	i=0;
	while (db_get_area_by_index(i++,&ad)==DB_OK)
	{
		if (!strcmp(ad.filename,str))
		{
			((CEdit*)GetDlgItem(IDC_NUMBER))->Undo();
			err_out("E_AREAEXISTS");
			GetDlgItem(IDC_NUMBER)->SetFocus();
			return;
		}
		if (db_lock_area(i)==DB_AREA_ALREADY_LOCKED)
			db_unlock_area(i);
	}
}

// ====================================================
	void cfg_area::OnDefault()
// ====================================================
{
	UpdateData(1);
	if (err_out("DY_SETDEFSET") != IDYES)	return;
	if (!m_group.IsEmpty())		set_cfg(CFG_COMMON,"DefAreaGroup",m_group);
	if (!m_create.IsEmpty())	set_cfg(CFG_COMMON,"DefAgeCreate",m_create);
	if (!m_recipt.IsEmpty())	set_cfg(CFG_COMMON,"DefAgeRecipt",m_recipt);
	if (!m_maxmails.IsEmpty())	set_cfg(CFG_COMMON,"DefMaxMails",m_maxmails);
	if (!m_adddays.IsEmpty())	set_cfg(CFG_COMMON,"DefDaysNonread",m_adddays);
}

// ==============================================
	void cfg_area::OnSetGroup()
// ==============================================
{
HFROW sel,tr;
CString str;

	UpdateData(1);
	if (err_out("DY_SETGROUP",m_group) != IDYES)	return;

	sel=Notify.hFRow;
	if (sel) 
	{
		tr=m_listctl.GetNextFRow(sel,FL_FIRST_CHILD);
		while (tr)
		{
			m_uplink.GetWindowText(str);
			if (!str.IsEmpty())	m_listctl.SetFItemText(tr,3,str);
			if (!m_aka.IsEmpty())	m_listctl.SetFItemText(tr,2,m_aka);
			if (!m_create.IsEmpty())	m_listctl.SetFItemNum(tr,4,atoi(m_create));
			if (!m_recipt.IsEmpty())	m_listctl.SetFItemNum(tr,5,atoi(m_recipt));
			if (!m_maxmails.IsEmpty())	m_listctl.SetFItemNum(tr,6,atoi(m_maxmails));
			if (!m_adddays.IsEmpty())	m_listctl.SetFItemNum(tr,7,atoi(m_adddays));
			m_listctl.SetFItemNum(tr,8,m_ccarea);
			m_listctl.SetFItemNum(tr,10,MOD_R);
			m_listctl.UpdateFRow(tr);
			m_listctl.SetColor(FL_FROW,tr,0,BLUE);
			m_listctl.SetFRowBold(tr);
			tr=m_listctl.GetNextFRow(tr);
		}
	}
	m_listctl.Invalidate();
	CountRecs();
}

// ============================================
	void cfg_area::OnListEvent(NMHDR* pNMHDR, LRESULT* pResult) 
// ============================================
{
	memcpy(&Notify,pNMHDR,sizeof(Notify));
	SetFields();
	*pResult = FL_OK;
}

// ============================================
	void cfg_area::OnBnClickedHelp()
// ============================================
{
	WinHelp(VHELP_CFG_AREA);
}

// ============================================
	BOOL cfg_area::OnHelpInfo(HELPINFO* pHelpInfo) 
// ============================================
{
	OnHelp();
	return TRUE;
}

// ====================================================
	void cfg_area::OnSelendokUplink() 
// ====================================================
{
char str[MAX_BOSSLEN];
	
	db_get_boss_by_index(m_uplink.GetCurSel(),str);
	get_token(str,MAINAKA,m_aka);
	GetDlgItem(IDC_AKA)->SetWindowText(m_aka);
}

// ============================================
	void cfg_area::MakeColumns(_flistcol *fl,int count)
// ============================================
{
int	al;
		
	for (int i=0;i<count;i++)
	{
		al=fl[i].align==0 ? FL_LEFT : fl[i].align;
		m_listctl.InsertFColumn(i,fl[i].hdr,fl[i].width,al);
	}
	m_listctl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

// ====================================================
	void cfg_area::SetFields(void)
// ====================================================
{
HFROW sel,tr;
	
	sel=Notify.hFRow;
	if (sel) 
	{
		tr=m_listctl.GetNextFRow(sel,FL_PARENT);
		if (tr==FL_ROOT)
		{
			m_group=m_listctl.GetFItemText(sel,0);
			m_areatag.Empty();
			m_description.Empty();
			m_aka.Empty();
			m_uplink.SetWindowText("");
			m_create=get_cfg(CFG_COMMON,"DefAgeCreate","30");
			m_recipt=get_cfg(CFG_COMMON,"DefAgeRecipt","30");
			m_maxmails=get_cfg(CFG_COMMON,"DefMaxMails","1000");
			m_adddays=get_cfg(CFG_COMMON,"DefDaysNonread","10");
			m_ccarea=0;
			m_number.Empty();
			DISABLEID(IDC_NUMBER);
			DISABLEID(IDC_DELETE);
			DISABLEID(IDC_CHANGE);
			DISABLEID(IDC_DESCR);
			DISABLEID(IDC_GROUP);
			ENABLEID(IDC_SETGROUP);
			ENABLEID(IDC_AREATAG);
		}
		else
		{
			m_group=m_listctl.GetFItemText(tr,0);
			m_areatag=m_listctl.GetFItemText(sel,0);
			m_description=m_listctl.GetFItemText(sel,1);
			m_aka=m_listctl.GetFItemText(sel,2);
			m_uplink.SetWindowText(m_listctl.GetFItemText(sel,3));
			m_create=m_listctl.GetFItemText(sel,4);
			m_recipt=m_listctl.GetFItemText(sel,5);
			m_maxmails=m_listctl.GetFItemText(sel,6);
			m_adddays=m_listctl.GetFItemText(sel,7);
			m_ccarea=m_listctl.GetFItemText(sel,8)=="1";
			m_number=m_listctl.GetFItemText(sel,9);
			ENABLEID(IDC_NUMBER);
			ENABLEID(IDC_DELETE);
			ENABLEID(IDC_CHANGE);
			ENABLEID(IDC_DESCR);
			ENABLEID(IDC_GROUP);
			DISABLEID(IDC_SETGROUP);
			if (m_areatag=="NETMAIL" || m_areatag=="LOCALMAIL")
				DISABLEID(IDC_AREATAG)
			else
				ENABLEID(IDC_AREATAG)
		}
		UpdateData(0);
	}
}
	
// ============================================
	void cfg_area::GetFields(HFROW sel,int flag)
// ============================================
{
CString str;

	m_listctl.SetFItemText(sel,1,m_description);
	m_listctl.SetFItemText(sel,2,m_aka);
	m_uplink.GetWindowText(str);
	m_listctl.SetFItemText(sel,3,str);
	m_listctl.SetFItemText(sel,4,m_create);
	m_listctl.SetFItemText(sel,5,m_recipt);
	m_listctl.SetFItemText(sel,6,m_maxmails);
	m_listctl.SetFItemText(sel,7,m_adddays);
	m_listctl.SetFItemNum(sel,8,m_ccarea);
	m_listctl.SetFItemText(sel,9,m_number);
	m_listctl.SetFItemNum(sel,10,flag);
	if (flag & DEL_R)
	{
		m_listctl.SetColor(FL_FROW,sel,0,RED);
		m_listctl.SetFRowBold(sel);
	}
	else if (flag & NEW_R)
	{
		m_listctl.SetColor(FL_FROW,sel,0,GREEN);
		m_listctl.SetFRowBold(sel);
	}
	else if (flag & MOD_R)
	{
		m_listctl.SetColor(FL_FROW,sel,0,BLUE);
		m_listctl.SetFRowBold(sel);
	}
	else
	{
		m_listctl.SetColor(FL_FROW,sel,0,DEFC);
		m_listctl.SetFRowBold(sel,FALSE);
	}
	m_listctl.Select(sel);
}

// ============================================
	void cfg_area::CountRecs(void)
// ============================================
{
HFROW tp,tr;
int	  fl;

	delrec=newrec=modrec=totrec=0;
	tp=m_listctl.FindIndex(FL_ROOT,0);
	while (tp)
	{
		tr=m_listctl.GetNextFRow(tp,FL_FIRST_CHILD);
		while (tr)
		{
			fl=m_listctl.GetFItemNum(tr,10);
			if (fl & DEL_R)		delrec++;
			else if (fl & NEW_R)	newrec++;
			else if (fl & MOD_R)	modrec++;
			totrec++;
			tr=m_listctl.GetNextFRow(tr);
		}
		tp=m_listctl.GetNextFRow(tp);
	}
	m_newrec.SetWindowText(L("S_610",newrec));
	m_modrec.SetWindowText(L("S_611",modrec));
	m_delrec.SetWindowText(L("S_612",delrec));
	m_totrec.SetWindowText(L("S_613",totrec));
}
