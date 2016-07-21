// cfg_aaka.cpp : implementation file
// IDD_CFG_PERSONAL

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_aaka.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList Akas;
extern CStrList	Aliases;
extern _gconfig gc;
static char DlgName[]="IDD_CFG_PERSONAL";

cfg_aaka::cfg_aaka(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(cfg_aaka::IDD, pParent) // CDialog(cfg_aaka::IDD, pParent)
	, m_password(_T(""))
{
	//{{AFX_DATA_INIT(cfg_aaka)
	m_prename = _T("");
	m_location = _T("");
	m_sysname = _T("");
	m_phone = _T("");
	m_aftername = _T("");
	//}}AFX_DATA_INIT
}

void cfg_aaka::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_aaka)
	DDX_Control(pDX, IDC_ALIASES, m_aliases);
	DDX_Control(pDX, IDC_AKAS, m_akas);
	DDX_Control(pDX, IDC_EDIT1, m_edit1);
	DDX_Control(pDX, IDC_EDIT2, m_edit2);
	DDX_Text(pDX, IDC_PRENAME, m_prename);
	DDX_Text(pDX, IDC_LOCATION, m_location);
	DDX_Text(pDX, IDC_SYSNAME, m_sysname);
	DDX_Text(pDX, IDC_PHONE, m_phone);
	DDX_Text(pDX, IDC_AFTERNAME, m_aftername);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 20);
}

BEGIN_MESSAGE_MAP(cfg_aaka, CDialog)
	//{{AFX_MSG_MAP(cfg_aaka)
	ON_BN_CLICKED(IDC_ADD1, OnAddAka)
	ON_BN_CLICKED(IDC_DELETE1, OnDeleteAka)
	ON_BN_CLICKED(IDC_ADD2, OnAddAlias)
	ON_BN_CLICKED(IDC_DELETE2, OnDeleteAlias)
	ON_WM_HELPINFO()
	ON_LBN_SELCHANGE(IDC_ALIASES, OnSelchangeAlias)
	ON_BN_CLICKED(IDC_CHANGE1, OnChangeAka)
	ON_BN_CLICKED(IDC_CHANGE2, OnChangeAlias)
	ON_LBN_SELCHANGE(IDC_AKAS, OnSelchangeAka)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SETPAS, OnBnClickedSetpas)
END_MESSAGE_MAP()

// =====================================================================
	LPCSTR cfg_aaka::GetName(void)	{return DlgName;}
// =====================================================================

// =============================================================
	BOOL cfg_aaka::OnInitDialog()
// =============================================================
{
CStrList lst;
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_DELETE1,
			IDC_CHANGE1,
			IDC_ADD1,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_DELETE2,
			IDC_CHANGE2,
			IDC_ADD2,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC7,
			IDC_STATIC8,
			IDC_STATIC9,
			IDC_STATIC10,
			IDC_STATIC11,
			IDC_SETPAS
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_prename=get_cfg(CFG_COMMON,"FirstName","");
	m_aftername=get_cfg(CFG_COMMON,"SecondName","");
	m_sysname=get_cfg(CFG_COMMON,"SystemName","");
	m_location=get_cfg(CFG_COMMON,"Location","");
	m_phone=get_cfg(CFG_COMMON,"Telephone","");
	split_string(get_cfg(CFG_COMMON,"Aliases",""),lst);
	UPDATE_LB(lst,IDC_ALIASES);
	split_string(get_cfg(CFG_COMMON,"Akas",""),lst);
	UPDATE_LB(lst,IDC_AKAS);
	UpdateData(0);
	return TRUE;
}

// =============================================================
	void cfg_aaka::OnAddAka()
// =============================================================
{
CString str;

	m_edit1.GetWindowText(str);
	if (str.IsEmpty())	return;
	if (!is_valid_address(str))	return;
	m_akas.AddString(str);
	m_edit1.SetWindowText(NULL);
	m_edit1.SetFocus();
}

// =============================================================
	void cfg_aaka::OnDeleteAka()
// =============================================================
{
int sel;

	GET_SELID(IDC_AKAS);
	m_akas.DeleteString(sel);
}

// =============================================================
	void cfg_aaka::OnAddAlias()
// =============================================================
{
CString str;

	m_edit2.GetWindowText(str);
	if (str.IsEmpty())	return;
	m_aliases.AddString(str);
	m_edit2.SetWindowText(NULL);
	m_edit2.SetFocus();
}

// =============================================================
	void cfg_aaka::OnDeleteAlias()
// =============================================================
{
int sel;

	GET_SELID(IDC_ALIASES);
	m_aliases.DeleteString(sel);
}

// =============================================================
	void cfg_aaka::OnOK()
// =============================================================
{
CString str;

	strncpy(gc.FirstName,trim_all(m_prename),99);
	gc.FirstName[99]=0;
	m_prename=gc.FirstName;
	strncpy(gc.SecondName,trim_all(m_aftername),99);
	gc.SecondName[99]=0;
	m_aftername=gc.SecondName;
	set_cfg(CFG_COMMON,"FirstName",m_prename);
	set_cfg(CFG_COMMON,"SecondName",m_aftername);
	set_cfg(CFG_COMMON,"SystemName",m_sysname);
	set_cfg(CFG_COMMON,"Location",m_location);
	set_cfg(CFG_COMMON,"Telephone",m_phone);
	collect_string(str,m_akas);
	Akas=m_akas;
	set_cfg(CFG_COMMON,"Akas",str);
	collect_string(str,m_aliases);
	Aliases=m_aliases;
	set_cfg(CFG_COMMON,"Aliases",str);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =============================================================
	void cfg_aaka::OnHelp()
// =============================================================
{
	WinHelp(VHELP_CFG_ADD_AKA);
}

BOOL cfg_aaka::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void cfg_aaka::OnSelchangeAlias() 
{
int sel;
CString str;
	
	GET_SELID(IDC_ALIASES);
	m_aliases.GetText(sel,str);
	m_edit2.SetWindowText(str);
}

void cfg_aaka::OnChangeAka() 
{
int sel;
CString str;

	GET_SELID(IDC_AKAS);
	m_edit1.GetWindowText(str);
	m_akas.DeleteString(sel);
	m_akas.InsertString(sel,str);
	m_akas.SetCurSel(sel);
}

void cfg_aaka::OnChangeAlias() 
{
int sel;
CString str;

	GET_SELID(IDC_ALIASES);
	m_edit2.GetWindowText(str);
	m_aliases.DeleteString(sel);
	m_aliases.InsertString(sel,str);
	m_aliases.SetCurSel(sel);
}

void cfg_aaka::OnSelchangeAka() 
{
int sel;
CString str;
	
	GET_SELID(IDC_AKAS);
	m_akas.GetText(sel,str);
	m_edit1.SetWindowText(str);
}

void cfg_aaka::OnBnClickedSetpas()
{
UINT m=0;

	UpdateData(1);
	if (!m_password.IsEmpty()) 
	{
		m_password+="Phoenix";
		for(int i=0;i<=m_password.GetLength();i++)	m=(m<<1)+m_password[i];
		set_cfg(CFG_COMMON,"Entry",m);
		ERR_MSG_RET("M_PASSET");
	}
	else
	{
		del_cfg(CFG_COMMON,"Entry");
		ERR_MSG_RET("M_PASRES");
	}
}
