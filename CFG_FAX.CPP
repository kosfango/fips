// cfg_fax.cpp : implementation file
// IDD_CFG_FAX

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_fax.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList faxcfg;
static char DlgName[]="IDD_CFG_FAX";

// ==========================================================
	cfg_fax::cfg_fax(CWnd* pParent ) : CSAPrefsSubDlg(cfg_fax::IDD, pParent)
// ==========================================================
{
	//{{AFX_DATA_INIT(cfg_fax)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


// ==========================================================
	void cfg_fax::DoDataExchange(CDataExchange* pDX)
// ==========================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_fax)
	DDX_Control(pDX, IDC_FAXVIEWER, m_faxviewer);
	DDX_Control(pDX, IDC_FAXVIEWER_BGFAX, m_faxviewer_bgfax);
	DDX_Control(pDX, IDC_BASEPATH, m_basepath);
	DDX_Control(pDX, IDC_DONT_DELETE, m_dont_delete);
	DDX_Control(pDX, IDC_DONT_CONVERT, m_dont_convert);
	DDX_Control(pDX, IDC_CHECK_FALLBACK, m_fallback);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cfg_fax, CDialog)
	//{{AFX_MSG_MAP(cfg_fax)
	ON_BN_CLICKED(IDC_BROWSE_PATH, OnBrowsePath)
	ON_BN_CLICKED(IDC_BROWSE_VIEWER, OnBrowseViewer)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_BROWSE_VIEWER_BGFAX, OnBrowseViewerBgfax)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cfg_fax message handlers

// =====================================================================
LPCSTR cfg_fax::GetName(void)	{return DlgName;}
// =====================================================================

// ==========================================================
	BOOL cfg_fax::OnInitDialog()
// ==========================================================
{
CString help;
int  lng[]={
			IDC_DONT_CONVERT,
			IDC_DONT_DELETE,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_CHECK_FALLBACK
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	help=faxcfg.GetString(0);
	m_faxviewer.SetWindowText(help);
	help=faxcfg.GetString(3);
	m_basepath.SetWindowText(help);
	help=faxcfg.GetString(4);
	m_faxviewer_bgfax.SetWindowText(help);

	m_dont_delete.SetCheck(faxcfg.reserved3);
	m_dont_convert.SetCheck(faxcfg.reserved4);
	m_fallback.SetCheck(faxcfg.defaultindex);
	return TRUE;
}

// ==========================================================
	void cfg_fax::OnBrowsePath()
// ==========================================================
{
CString path;

	if (GetDirectory(path,m_hWnd))
		m_basepath.SetWindowText(path);
}

// ==========================================================
	void cfg_fax::OnBrowseViewer()
// ==========================================================
{
CString str;
int		ret;

	str.LoadString(IDS_EXECFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_faxviewer.SetWindowText(dlg.GetPathName());
}

// ==========================================================
	void cfg_fax::OnBrowseViewerBgfax()
// ==========================================================
{
CString str;
int		ret;

	str.LoadString(IDS_EXECFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_faxviewer_bgfax.SetWindowText(dlg.GetPathName());
}

// ==========================================================
	void cfg_fax::OnCancel()
// ==========================================================
{
	faxcfg.LoadFromFile("faxcfg.cfg");
	CDialog::OnCancel();
}

// ==========================================================
	void cfg_fax::OnOK()
// ==========================================================
{
CString help;

	faxcfg.RemoveAll();
    m_faxviewer.GetWindowText(help);
	faxcfg.AddTail(trim_all(help));
	help="-";
	faxcfg.AddTail(help);
	help="-";
	faxcfg.AddTail(help);
    m_basepath.GetWindowText(help);
	trim_all(help);
	if (help.GetLength()>1)
		create_path(help);
	else
		err_out("W_NOFBPDEF");

	faxcfg.AddTail(help);
    m_faxviewer_bgfax.GetWindowText(help);
	trim_all(help);
	faxcfg.AddTail(help);
	faxcfg.reserved3=m_dont_delete.GetCheck();
	faxcfg.reserved4=m_dont_convert.GetCheck();
	faxcfg.defaultindex=m_fallback.GetCheck();
	faxcfg.SaveToFile("faxcfg.cfg");

	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ==========================================================
	void cfg_fax::OnHelp()
// ==========================================================
{
	WinHelp(VHELP_FAX_SETUP);
}

BOOL cfg_fax::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
