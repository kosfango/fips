// cfg_gate.cpp : implementation file
// IDD_CFG_GATE

#include "stdafx.h"
#include "cfg_gate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList    gatecfg;
static char DlgName[]="IDD_CFG_GATE";

// gatecfg Format:
//	1 - m_edit_to
//	2 - m_edit_to_fido
//	3 - m_edit_subject
//	4 - m_edit_aka
//	5 - m_edit_mailcont

// ======================================================================0
	cfg_gate::cfg_gate(CWnd* pParent ) : CSAPrefsSubDlg(cfg_gate::IDD, pParent)
// ======================================================================0
{
	//{{AFX_DATA_INIT(cfg_gate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ======================================================================0
	void cfg_gate::DoDataExchange(CDataExchange* pDX)
// ======================================================================0
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_gate)
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_EDIT_AKA, m_edit_aka);
	DDX_Control(pDX, IDC_EDIT_TO_FIDO, m_edit_to_fido);
	DDX_Control(pDX, IDC_EDIT_TO, m_edit_to);
	DDX_Control(pDX, IDC_EDIT_SUBJECT, m_edit_subject);
	DDX_Control(pDX, IDC_EDIT_MAILCONT, m_edit_mailcont);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_gate, CDialog)
	//{{AFX_MSG_MAP(cfg_gate)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_gate::GetName(void)	{return DlgName;}
// =====================================================================

// ======================================================================0
	BOOL cfg_gate::OnInitDialog()
// ======================================================================0
{
CString help;
char buf[1000];
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC56,
			IDC_STATIC78,
			IDC_ENABLE,
			IDC_STATIC568,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	if (gatecfg.GetCount()>4)
	{
		help=gatecfg.GetString(0);
		m_edit_to.SetWindowText(help);
		help=gatecfg.GetString(1);
		m_edit_to_fido.SetWindowText(help);
		help=gatecfg.GetString(2);
		m_edit_subject.SetWindowText(help);
		help=gatecfg.GetString(3);
		m_edit_aka.SetWindowText(help);
		help=gatecfg.GetString(4);
		strcpy(buf,help);
		insert_LFCR(buf);
		m_edit_mailcont.SetWindowText(buf);
	}
	m_enable.SetCheck(gatecfg.reserved3);
	return TRUE;
}

// ======================================================================0
	void cfg_gate::OnOK()
// ======================================================================0
{
CString help;
char buf[1000];

	gatecfg.RemoveAll();
	m_edit_to.GetWindowText(help);
	gatecfg.AddTail(help);
	m_edit_to_fido.GetWindowText(help);
	gatecfg.AddTail(help);
	m_edit_subject.GetWindowText(help);
	gatecfg.AddTail(help);
	m_edit_aka.GetWindowText(help);
	gatecfg.AddTail(help);
	m_edit_mailcont.GetWindowText(help);
	strcpy(buf,help);
	remove_LFCR(buf);
	gatecfg.AddTail(buf);
	gatecfg.reserved3=m_enable.GetCheck();
	gatecfg.SaveToFile("ingate.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ======================================================================0
	void cfg_gate::OnHelp()
// ======================================================================0
{
	WinHelp(VHELP_CFG_GATE);
}

BOOL cfg_gate::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
