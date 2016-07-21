// reqsel.cpp : implementation file
// IDD_FREQ_SELECT

#include "stdafx.h"
#include "reqsel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CString StoredFido;
extern CString StoredFile;
extern CStrList   FoundAddresses;
extern CStrList   found_filenames;

static char DlgName[]="IDD_FREQ_SELECT";

// ================================================================================
	reqsel::reqsel(CWnd* pParent ) : CDialog(reqsel::IDD, pParent)
// ================================================================================
{
	//{{AFX_DATA_INIT(reqsel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ================================================================================
	void reqsel::DoDataExchange(CDataExchange* pDX)
// ================================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(reqsel)
	DDX_Control(pDX, IDC_SYSTEMS, m_systems);
	DDX_Control(pDX, IDC_FILES, m_files);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(reqsel, CDialog)
	//{{AFX_MSG_MAP(reqsel)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ================================================================================
	BOOL reqsel::OnInitDialog()
// ================================================================================
{
int  lng[]={
			IDOK,
			IDCANCEL,
			IDHELP
			};

  CDialog::OnInitDialog();
  set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
  UPDATE_LB(FoundAddresses,IDC_SYSTEMS);
  UPDATE_LB(found_filenames,IDC_FILES);
	m_systems.SetCurSel(0);
	m_files.SetCurSel(0);
	return TRUE;
}

// ================================================================================
	void reqsel::OnOK()
// ================================================================================
{
int  selected[1000],ret,sel;
char buf[1000];
CString xx;

	StoredFido.Empty();
	StoredFile.Empty();

	sel=m_systems.GetCurSel();
	if (sel!=LB_ERR)
	   m_systems.GetText(sel,StoredFido);

	*buf=0;
    ret=m_files.GetSelItems(1000,selected);
	if (ret>0)
	{
		for (int n=0;n<ret;n++)
	   	{
		   m_files.GetText(selected[n],xx);
		   strcat(buf,xx);
		   if (n<(ret-1))
			   strcat(buf," ");
		}
	    StoredFile=buf;
	}
	CDialog::OnOK();
}

// ============================================
	void reqsel::OnCancel()
// ============================================
{
	CDialog::OnCancel();
}

// ================================================================================
	void reqsel::OnHelp()
// ================================================================================
{
	WinHelp(VHELP_FREQ_SELECT);
}

BOOL reqsel::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
