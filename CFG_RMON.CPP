// cfg_rmon.cpp : implementation file
// IDD_CFG_REMOTE

#include "stdafx.h"
#include "cfg_rmon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList remote;
static char DlgName[]="IDD_CFG_REMOTE";

// ==================================================================
	cfg_rmon::cfg_rmon(CWnd* pParent ) 	: CDialog(cfg_rmon::IDD, pParent)
// ==================================================================
{
	//{{AFX_DATA_INIT(cfg_rmon)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ==================================================================
	void cfg_rmon::DoDataExchange(CDataExchange* pDX)
// ==================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_rmon)
	DDX_Control(pDX, IDC_CHECK_ENABLE, m_check);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_rmon, CDialog)
	//{{AFX_MSG_MAP(cfg_rmon)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ==================================================================
	BOOL cfg_rmon::OnInitDialog()
// ==================================================================
{
int  lng[]={
			IDOK,
			IDCANCEL,
			IDHELP,
			IDC_CHECK_ENABLE,
			IDC_STATIC1,
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_check.SetCheck(remote.defaultindex);
	return TRUE;
}

// ==================================================================
	void cfg_rmon::OnOK()
// ==================================================================
{
	remote.RemoveAll();
	remote.AddTail("dummy");
	remote.defaultindex=m_check.GetCheck();
	remote.SaveToFile("remote.cfg");

	if (remote.defaultindex)
	{
		if (!init_tcpip_stuff ())
			handle_tcpip_load_error();
	}
	else
		free_tcpip_stuff ();

	CDialog::OnOK();
}

// ==================================================================
	void cfg_rmon::OnCancel()
// ==================================================================
{
	remote.LoadFromFile("remote.cfg");
	CDialog::OnCancel();
}

// ==================================================================
	void cfg_rmon::OnHelp()
// ==================================================================
{
	WinHelp(VHELP_CFG_REMOTE);
}

BOOL cfg_rmon::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
