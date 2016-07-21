// cfgexfr.cpp : implementation file
// IDD_CFG_EXTFREQ

#include "stdafx.h"
#include "cfgexfr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList exfrcfg;
static char DlgName[]="IDD_CFG_EXTFREQ";

// =================================================================
	cfgexfr::cfgexfr(CWnd* pParent ) : CDialog(cfgexfr::IDD, pParent)
// =================================================================
{
	//{{AFX_DATA_INIT(cfgexfr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =================================================================
	void cfgexfr::DoDataExchange(CDataExchange* pDX)
// =================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfgexfr)
	DDX_Control(pDX, IDC_CHECK_16BIT, m_check_16bit);
	DDX_Control(pDX, IDC_EDIT_PATH, m_edit_path);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfgexfr, CDialog)
	//{{AFX_MSG_MAP(cfgexfr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =================================================================
	BOOL cfgexfr::OnInitDialog()
// =================================================================
{
int  lng[]={
			IDC_STATIC2,
			IDC_CHECK_16BIT,
			IDCANCEL,
			IDOK
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	if (exfrcfg.GetCount()>1)
	{
		m_edit_path.SetWindowText(exfrcfg.GetString(0));
		m_check_16bit.SetCheck(exfrcfg.GetString(1)=="1" ? 1 : 0);
	}
	return TRUE;
}

// =================================================================
	void cfgexfr::OnCancel()
// =================================================================
{
	exfrcfg.LoadFromFile("exfrtool.cfg");
	CDialog::OnCancel();
}

// =================================================================
	void cfgexfr::OnOK()
// =================================================================
{
CString str;

	m_edit_path.GetWindowText(str);
	if (strchr(str,'\\'))
		ERR_MSG_RET("E_DMNCAP");

	if (strchr(str,':'))
		ERR_MSG_RET("E_DMNCAP");

	exfrcfg.RemoveAll();
	exfrcfg.AddTail(str);
	exfrcfg.AddTail(m_check_16bit.GetCheck() ? "1" : "0");
	exfrcfg.SaveToFile("exfrtool.cfg");
	CDialog::OnOK();
}
