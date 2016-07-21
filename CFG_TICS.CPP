// cfg_tics.cpp : implementation file
// IDD_CFG_TICS

#include "stdafx.h"
#include "resource.h"
#include "cfg_tics.h"
#include "structs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList tics;
static char DlgName[]="IDD_CFG_TICS";

// ======================================================
	cfg_tics::cfg_tics(CWnd* pParent ) : CDialog(cfg_tics::IDD, pParent)
// ======================================================
{
	//{{AFX_DATA_INIT(cfg_tics)
	//}}AFX_DATA_INIT
}

// ======================================================
	void cfg_tics::DoDataExchange(CDataExchange* pDX)
// ======================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_tics)
	DDX_Control(pDX, IDC_COMBO_UPLINK, m_uplink);
	DDX_Control(pDX, IDC_DONTDELETE, m_dontdelete);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT_PATH, m_path);
	DDX_Control(pDX, IDC_EDIT_ECHO, m_echo);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_description);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_tics, CDialog)
	//{{AFX_MSG_MAP(cfg_tics)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_tics::GetName(void)	{return DlgName;}
// =====================================================================

// ======================================================
	BOOL cfg_tics::OnInitDialog()
// ======================================================
{
int tabs[]={100,140,290,500};
int lng[]={
		   IDC_BROWSE,
		   IDC_CHANGE,
		   IDC_ADD,
		   IDC_DELETE,
		   IDC_STATIC1,
		   IDC_STATIC2,
		   IDC_STATIC3,
		   IDC_STATIC6,
		   IDC_STATIC4,
		   IDC_DONTDELETE,
		   IDC_STATIC5,
			 IDOK,
			 IDCANCEL,
			 IDHELP
		 };
  CDialog::OnInitDialog();
  set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	UPDATE_LB(tics,IDC_LIST);
	TABULATE_LB(IDC_LIST);
	EXTENT_LB(IDC_LIST,900);
	SET_SELID(IDC_LIST,0);
	fill_uplink_list();
	OnSelchangeList();
	return TRUE;
}

// ======================================================
	void cfg_tics::OnAdd()
// ======================================================
{
CString echo,desc,dirpath,uplink,str;
int     sel;

	m_echo.GetWindowText(echo);
	m_description.GetWindowText(desc);
	m_path.GetWindowText(dirpath);
	sel=m_uplink.GetCurSel();
	LB_ERR_RET;
	m_uplink.GetWindowText(uplink);

    trim_all(echo);
    trim_all(desc);
    trim_all(dirpath);

	if (echo.IsEmpty() || dirpath.IsEmpty())	return;

	str.Format("%s\t%s\t%s\t%s\t%d\t",echo,uplink,desc,dirpath,m_dontdelete.GetCheck());
	SET_SELID(IDC_LIST,m_list.AddString(str));
}

// ======================================================
	void cfg_tics::OnBrowse()
// ======================================================
{
CString path;

	if (GetDirectory(path,m_hWnd))
		m_path.SetWindowText(path);
}

// ======================================================
	void cfg_tics::OnDelete()
// ======================================================
{
int sel;

	GET_SELID(IDC_LIST);
	m_list.DeleteString(sel);
}

// ======================================================
	void cfg_tics::OnChange()
// ======================================================
{
CString echo,desc,dirpath,uplink,str;
int     sel;

	sel=m_uplink.GetCurSel();
	LB_ERR_RET;
	m_echo.GetWindowText(echo);
	m_path.GetWindowText(dirpath);
	trim_all(echo);
	trim_all(dirpath);
	if (echo.IsEmpty() || dirpath.IsEmpty())	return;
	m_description.GetWindowText(desc);
	m_uplink.GetWindowText(uplink);
    trim_all(desc);
	str.Format("%s\t%s\t%s\t%s\t%d\t",echo,uplink,desc,dirpath,m_dontdelete.GetCheck());
	GET_SELID(IDC_LIST);
	m_list.DeleteString(sel);
	m_list.InsertString(sel,str);
	SET_SELID(IDC_LIST,sel);
}

// ======================================================
	void cfg_tics::OnBnClickedOk()
// ======================================================
{
	tics=m_list;
	tics.SaveToFile("tics.cfg");
//	if(m_bCloseOnOk)
	CDialog::OnOK();
}

// ======================================================
	void cfg_tics::OnSelchangeList()
// ======================================================
{
CString str,echo,desc,dirpath,uplink;
int		sel,dd;

	GET_SELID(IDC_LIST);
	m_list.GetText(sel,str);
	get_token(str,0,echo);
	get_token(str,1,uplink);
	get_token(str,2,desc);
	get_token(str,3,dirpath);
	dd=get_token_int(str,4);

	m_dontdelete.SetCheck(dd);
	m_echo.SetWindowText(echo);
	m_description.SetWindowText(desc);
	m_path.SetWindowText(dirpath);

	sel=m_uplink.FindString(-1,uplink);
	if (sel!=LB_ERR)
		m_uplink.SetCurSel(sel);
}

// ======================================================
	void cfg_tics::fill_uplink_list()
// ======================================================
{
CString str;
char	buf[MAX_BOSSLEN];
int		i=0;

	m_uplink.ResetContent();
	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,BOSSADDR,str);
		m_uplink.AddString(str);
	}
	m_uplink.SetCurSel(0);
}

// ======================================================
	void cfg_tics::OnHelp()
// ======================================================
{
	WinHelp(VHELP_CFG_TICS);
}

// ======================================================
	BOOL cfg_tics::OnHelpInfo(HELPINFO* pHelpInfo) 
// ======================================================
{
	OnHelp();
	return TRUE;
}
