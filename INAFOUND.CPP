// inafound.cpp : implementation file
// IDD_INADR_FOUND

#include "stdafx.h"
#include "inafound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString	gUseThisURL;
extern CStrList EmailList;
static char DlgName[]="IDD_INADR_FOUND";

// ====================================================================
	inafound::inafound(CWnd* pParent ) : CDialog(inafound::IDD, pParent)
// ====================================================================
{
	//{{AFX_DATA_INIT(inafound)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ====================================================================
	void inafound::DoDataExchange(CDataExchange* pDX)
// ====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(inafound)
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(inafound, CDialog)
	//{{AFX_MSG_MAP(inafound)
	ON_BN_CLICKED(IDOK, OnUseThisAdress)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ====================================================================
	BOOL inafound::OnInitDialog()
// ====================================================================
{
int  lng[]={
			IDOK,
			IDCANCEL,
			IDC_STATIC1,
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	gUseThisURL.Empty();
	UPDATE_LB(EmailList,IDC_LIST);
	SET_SELID(IDC_LIST,0);
	OnSelchangeList1();
	return TRUE;
}

// ====================================================================
	void inafound::OnCancel()
// ====================================================================
{
	gUseThisURL.Empty();
	CDialog::OnCancel();
}

// ====================================================================
	void inafound::OnUseThisAdress()
// ====================================================================
{
CString str;

	gUseThisURL.Empty();
	m_edit.GetWindowText(str);
    trim_all(str);
	gUseThisURL=str;
	CDialog::OnCancel();
}

// ====================================================================
	void inafound::OnSelchangeList1()
// ====================================================================
{
int		sel;
CString	str;

	GET_SELID(IDC_LIST);
	str=EmailList.GetString(sel);
	m_edit.SetWindowText(str);
}

// ====================================================================
	void inafound::OnDblclkList1()
// ====================================================================
{
	OnSelchangeList1();
	OnUseThisAdress();
}
