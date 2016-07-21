// emergenc.cpp : implementation file
// IDD_DIALBREAK

#include "stdafx.h"
#include "light.h"
#include "emergenc.h"
#include "mystrlst.h"
#include "structs.h"
#include "include.hh"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_DIALBREAK";
extern CStrList dialbrk;

// =========================================================
	emergenc::emergenc(CWnd* pParent ) 	: CDialog(emergenc::IDD, pParent)
// =========================================================
{
	//{{AFX_DATA_INIT(emergenc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =========================================================
	void emergenc::DoDataExchange(CDataExchange* pDX)
// =========================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(emergenc)
	DDX_Control(pDX, IDC_DELAY, m_edit_delay);
	DDX_Control(pDX, IDC_SECONDS, m_edit_seconds);
	DDX_Control(pDX, IDC_MAXIMUM, m_edit_maximum);
	DDX_Control(pDX, IDC_ENABLE, m_enabled);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(emergenc, CDialog)
	//{{AFX_MSG_MAP(emergenc)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDRESET, OnReset)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// =========================================================
	BOOL emergenc::OnInitDialog()
// =========================================================
{
int  lng[]={
			IDOK,
			IDCANCEL,
			IDC_STATIC1,
			IDC_STATIC22,
			IDC_STATIC55,
			IDC_STATIC33,
			IDC_STATIC66,
			IDC_ENABLE,
			IDHELP,
			IDRESET
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	if (dialbrk.GetCount()==3)
	{
		m_enabled.SetCheck(dialbrk.defaultindex);
		m_edit_maximum.SetWindowText(dialbrk.GetString(0));
		m_edit_seconds.SetWindowText(dialbrk.GetString(1));
		m_edit_delay.SetWindowText(dialbrk.GetString(2));
	}
	return TRUE;
}

// =========================================================
	void emergenc::OnCancel()
// =========================================================
{
	dialbrk.LoadFromFile("dialbrk.cfg");
	CDialog::OnCancel();
}

// =========================================================
	void emergenc::OnOK()
// =========================================================
{
CString text;

	dialbrk.RemoveAll();
	dialbrk.defaultindex=m_enabled.GetCheck();
    m_edit_maximum.GetWindowText(text); 
	trim_all(text); 
	dialbrk.AddTail(text);
    m_edit_seconds.GetWindowText(text); 
	trim_all(text); 
	dialbrk.AddTail(text);
    m_edit_delay.GetWindowText(text);   
	trim_all(text); 
	dialbrk.AddTail(text);
	dialbrk.SaveToFile("dialbrk.cfg");
	CDialog::OnOK();
}

// =========================================================
	void emergenc::OnReset()
// =========================================================
{
	reset_Notbremse();
	show_msg("Resetting","Перезапуск");
}

// =========================================================
	void emergenc::OnHelp()
// =========================================================
{
	WinHelp(VHELP_DIALBREAK);
}

BOOL emergenc::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void emergenc::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
