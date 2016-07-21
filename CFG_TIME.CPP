// cfg_time.cpp : implementation file
// IDD_TIMEOUTS

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_time.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList dialbrk;
CStrList gTimeouts;
static char DlgName[]="IDD_TIMEOUTS";

// ========================================================
	cfg_time::cfg_time(CWnd* pParent) : CSAPrefsSubDlg(cfg_time::IDD, pParent)
// ========================================================
	{
	//{{AFX_DATA_INIT(cfg_time)
	m_loopcounter = _T("");
	m_trycounter = _T("");
	m_waittime = _T("");
	m_maximum = _T("");
	m_seconds = _T("");
	m_delay = _T("");
	m_enabled = FALSE;
	//}}AFX_DATA_INIT
}

// ========================================================
	void cfg_time::DoDataExchange(CDataExchange* pDX)
// ========================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_time)
	DDX_Text(pDX, IDC_LOOPCOUNTER, m_loopcounter);
	DDV_MaxChars(pDX, m_loopcounter, 3);
	DDX_Text(pDX, IDC_TRYCOUNTER, m_trycounter);
	DDV_MaxChars(pDX, m_trycounter, 3);
	DDX_Text(pDX, IDC_WAITTIME, m_waittime);
	DDV_MaxChars(pDX, m_waittime, 6);
	DDX_Text(pDX, IDC_MAXIMUM, m_maximum);
	DDV_MaxChars(pDX, m_maximum, 3);
	DDX_Text(pDX, IDC_SECONDS, m_seconds);
	DDV_MaxChars(pDX, m_seconds, 3);
	DDX_Text(pDX, IDC_DELAY, m_delay);
	DDV_MaxChars(pDX, m_delay, 3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_time, CDialog)
	//{{AFX_MSG_MAP(cfg_time)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_time::GetName(void)	{return DlgName;}
// =====================================================================

// ========================================================
	BOOL cfg_time::OnInitDialog()
// ========================================================
{
int  lng[]={
			IDC_STATIC3,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC7,
			IDC_STATIC8,
			IDC_ENABLE,
			IDC_RESET,
			IDC_STATIC9,
			IDC_STATIC10,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	m_trycounter=get_cfg(CFG_TIMEOUT,"TryCounter","999");
	m_waittime=get_cfg(CFG_TIMEOUT,"WaitTime","3");
	m_loopcounter=get_cfg(CFG_TIMEOUT,"LoopCounter","100");
	if (dialbrk.GetCount()==3)
	{
		m_enabled=dialbrk.defaultindex;
		m_maximum=dialbrk.GetString(0);
		m_seconds=dialbrk.GetString(1);
		m_delay=dialbrk.GetString(2);
	}
	UpdateData(0);
	return TRUE;
}

// ========================================================
	void cfg_time::OnOK()
// ========================================================
{
	UpdateData(1);
	gTimeouts.RemoveAll();
	gTimeouts.AddTail(m_trycounter);
	gTimeouts.AddTail(m_waittime);
	gTimeouts.AddTail(m_loopcounter);
	set_cfg(CFG_TIMEOUT,"TryCounter",m_trycounter);
	set_cfg(CFG_TIMEOUT,"WaitTime",m_waittime);
	set_cfg(CFG_TIMEOUT,"LoopCounter",m_loopcounter);
	dialbrk.RemoveAll();
	dialbrk.defaultindex=m_enabled;
	trim_all(m_maximum); 
	dialbrk.AddTail(m_maximum);
	trim_all(m_seconds); 
	dialbrk.AddTail(m_seconds);
	trim_all(m_delay); 
	dialbrk.AddTail(m_delay);
	dialbrk.SaveToFile("dialbrk.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ========================================================
	void cfg_time::OnBnClickedReset()
// ========================================================
{
	reset_Notbremse();
	show_msg("Resetting","Перезапуск");
}

// ========================================================
	void cfg_time::OnHelp()
// ========================================================
{
	WinHelp(VHELP_TIMEOUTS);
}

BOOL cfg_time::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
