// again.cpp : implementation file
// IDD_REPOST

#include "stdafx.h"
#include "again.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;
static char DlgName[]="IDD_REPOST";

// ===================================================
	again::again(CWnd* pParent ) : CDialog(again::IDD, pParent)
// ===================================================
{
	period=-1;
	//{{AFX_DATA_INIT(again)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ===================================================
	void again::DoDataExchange(CDataExchange* pDX)
// ===================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(again)
	DDX_Control(pDX, IDC_EDIT_DAYS, m_edit_days);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(again, CDialog)
	//{{AFX_MSG_MAP(again)
	ON_BN_CLICKED(IDC_1DAY, On1day)
	ON_BN_CLICKED(IDC_3DAYS, On3days)
	ON_BN_CLICKED(IDC_WEEK, OnWeek)
	ON_BN_CLICKED(IDC_MONTH, OnMonth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ===================================================
	BOOL again::OnInitDialog()
// ===================================================
{
int lng[]={
			IDOK,
			IDCANCEL,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_1DAY,
			IDC_3DAYS,
			IDC_WEEK,
			IDC_MONTH,
			};

    CDialog::OnInitDialog();
	m_edit_days.SetLimitText(3);
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_edit_days.SetWindowText(get_cfg(CFG_COMMON,"DefRepost","7"));
	return TRUE;
}

// ===================================================
	void again::OnOK()
// ===================================================
{
CString str;
int  ret;

	m_edit_days.GetWindowText(str);
	ret=sscanf(str,"%d",&period);
	if (ret != 1)
		period=-1;
	else
		set_cfg(CFG_COMMON,"DefRepost",period);
	CDialog::OnOK();
}

// ===================================================
	void again::On1day()
// ===================================================
{
	m_edit_days.SetWindowText("1");	
	OnOK();
}
// ===================================================
	void again::On3days()
// ===================================================
{
	m_edit_days.SetWindowText("3");
	OnOK();
}
// ===================================================
	void again::OnWeek()
// ===================================================
{
	m_edit_days.SetWindowText("7");	
	OnOK();
}
// ===================================================
	void again::OnMonth()
// ===================================================
{
	m_edit_days.SetWindowText("30");	
	OnOK();
}
