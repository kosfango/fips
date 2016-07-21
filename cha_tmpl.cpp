// cha_tmpl.cpp : implementation file
// IDD_CHANGE_TEMPLATE

#include "stdafx.h"
#include "cha_tmpl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList HeaderList;
extern CStrList FooterList;
extern _gconfig gc;

// ==========================================================================
	cha_tmpl::cha_tmpl(int t,CWnd* pParent /*=NULL*/)
		: CDialog(cha_tmpl::IDD, pParent)
// ==========================================================================
{
	//{{AFX_DATA_INIT(cha_tmpl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	type=t;
}

// ==========================================================================
	void cha_tmpl::DoDataExchange(CDataExchange* pDX)
// ==========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cha_tmpl)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cha_tmpl, CDialog)
	//{{AFX_MSG_MAP(cha_tmpl)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ==========================================================================
	BOOL cha_tmpl::OnInitDialog()
// ==========================================================================
{
int	lng[]={
			IDHELP,
			IDCANCEL,
			IDOK,
			};

	CDialog::OnInitDialog();
	if (type==1)
	{
		set_dlg_language(this,"IDD_CHANGE_HEADER",lng,sizeof(lng)/sizeof(int));
		gc.new_header_index=0;
		UPDATE_LB(HeaderList,IDC_LIST);
	}
	else
	{
		set_dlg_language(this,"IDD_CHANGE_FOOTER",lng,sizeof lng/sizeof(int));
		gc.new_footer_index=0;
		UPDATE_LB(FooterList,IDC_LIST);
	}
	return TRUE;
}

// ==========================================================================
	void cha_tmpl::OnOK()
// ==========================================================================
{
int sel;

	GET_SEL(m_list);
	if (type==1)
		gc.new_header_index=sel;
	else
		gc.new_footer_index=sel;
	CDialog::OnOK();
}

// ==========================================================================
	void cha_tmpl::OnDblclkList()
// ==========================================================================
{
	OnOK();
}

// ==========================================================================
	void cha_tmpl::OnHelp()
// ==========================================================================
{
	WinHelp(VHELP_MAIL_TEMPLATE);
}

BOOL cha_tmpl::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
