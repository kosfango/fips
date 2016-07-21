// exbrow.cpp : implementation file
// IDD_START_BROWSER

#include "stdafx.h"
#include "exbrow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CStrList	gWebAddrList;
CString		browser_cmd;
static char DlgName[]="IDD_START_BROWSER";

// ==================================================================
	exbrow::exbrow(CWnd* pParent ) 	: CDialog(exbrow::IDD, pParent)
// ==================================================================
{
	//{{AFX_DATA_INIT(exbrow)
	m_startcmd = _T("");
	//}}AFX_DATA_INIT
}

// ==================================================================
	void exbrow::DoDataExchange(CDataExchange* pDX)
// ==================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(exbrow)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_STARTCMD, m_startcmd);
	DDV_MaxChars(pDX, m_startcmd, 260);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(exbrow, CDialog)
	//{{AFX_MSG_MAP(exbrow)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ==================================================================
	BOOL exbrow::OnInitDialog()
// ==================================================================
{
int	lng[]={
			IDOK,
			IDCANCEL,
			IDHELP,
			IDC_STATIC1,
			IDC_STATIC2
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	browser_cmd=get_cfg(CFG_COMMON,"BrowserCmd","start iexplore.exe %s");
	UPDATE_LB(gWebAddrList,IDC_LIST);
	return TRUE;
}

// ==================================================================
	void exbrow::OnOK()
// ==================================================================
{
CString str;

	UpdateData(1);
	if (m_startcmd.GetLength()<2)	return;
	if (m_startcmd.Find('-')>=0)
	{
		str=m_startcmd;
		str.MakeUpper();
		if (str.Find("NETSCAP")>=0)	err_out("W_NETSPP");
	}
	if (WinExec(m_startcmd,SW_SHOWNORMAL)<=31)
		ERR_MSG_RET("E_STEBFA");
	CDialog::OnOK();
}

// ==================================================================
	void exbrow::OnDblclkList()
// ==================================================================
{
	OnSelchangeList();
	OnOK();
}

// ==================================================================
	void exbrow::OnSelchangeList()
// ==================================================================
{
int  sel;

	GET_SELID(IDC_LIST);
	m_startcmd=browser_cmd;
	m_startcmd.Replace("%s",gWebAddrList.GetString(sel));
	UpdateData(0);
}

// ==================================================================
	void exbrow::OnHelp()
// ==================================================================
{
	WinHelp(VHELP_INTERNET_EXTERNAL);
}

BOOL exbrow::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
