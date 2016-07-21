// manual.cpp : implementation file
// IDD_MANUAL_AREA_ORDER

#include "stdafx.h"
#include "manual.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList ManualAreaBestell;
static char DlgName[]="IDD_MANUAL_AREA_ORDER";

// ======================================================================0
	manual::manual(CWnd* pParent ) 	: CDialog(manual::IDD, pParent)
// ======================================================================0
{
	//{{AFX_DATA_INIT(manual)
	m_edit = _T("");
	//}}AFX_DATA_INIT
}

// ======================================================================0
	void manual::DoDataExchange(CDataExchange* pDX)
// ======================================================================0
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(manual)
	DDX_Text(pDX, IDC_EDIT, m_edit);
	DDV_MaxChars(pDX, m_edit, 9999);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(manual, CDialog)
	//{{AFX_MSG_MAP(manual)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ======================================================================0
	BOOL manual::OnInitDialog()
// ======================================================================0
{
int  lng[]={
			IDOK,
			IDHELP,
			IDCANCEL
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	return TRUE;
}

// ======================================================================0
	void manual::OnOK()
// ======================================================================0
{
CStrList lst;
char tmp[10000],buf[1000],*p;
int  i;

	UpdateData(1);
	strcpy(tmp,m_edit);
	strcat(tmp,"\r\n");
	ManualAreaBestell.SplitTabs(tmp,'\n');

	lst.RemoveAll();
	for (i=0;i<ManualAreaBestell.GetCount();i++)
	{
	   strcpy(buf,ManualAreaBestell.GetString(i));
	   p=strchr(buf,'\r'); if (p) *p=0;
	   p=strchr(buf,'\t'); if (p) *p=0;
	   if (strlen(buf)>0)
		   lst.AddTail(buf);
	}

	ManualAreaBestell.RemoveAll();
	for (i=0;i<lst.GetCount();i++)
	   ManualAreaBestell.AddTail(lst.GetString(i));

	CDialog::OnOK();
}

// ======================================================================0
	void manual::OnCancel()
// ======================================================================0
{
	ManualAreaBestell.RemoveAll();
	CDialog::OnCancel();
}

// ======================================================================0
	void manual::OnHelp()
// ======================================================================0
{
	WinHelp(VHELP_ECHO_ORDER);
}

// ======================================================================0
	BOOL manual::OnHelpInfo(HELPINFO* pHelpInfo) 
// ======================================================================0
{
	OnHelp();
	return TRUE;
}
