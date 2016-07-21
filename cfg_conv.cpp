// cfg_conv.cpp : implementation file
// IDD_CHAR_CONVERSION

#include "stdafx.h"
#include "cfg_conv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList converts;

static char DlgName []="IDD_CHAR_CONVERSION";

// ========================================================
	cfg_conv::cfg_conv(CWnd* pParent )	 	: CDialog(cfg_conv::IDD, pParent)
// ========================================================
{
	//{{AFX_DATA_INIT(cfg_conv)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ========================================================
	void cfg_conv::DoDataExchange(CDataExchange* pDX)
// ========================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_conv)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_ENABLE_CHARACTER_CONVERSION, m_enabled);
	DDX_Control(pDX, IDC_EDIT_REPLACE, m_replace);
	DDX_Control(pDX, IDC_EDIT_CHAR, m_char);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_conv, CDialog)
	//{{AFX_MSG_MAP(cfg_conv)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ========================================================
	BOOL cfg_conv::OnInitDialog()
// ========================================================
{
int  lng[]={
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_ADD,
			IDC_DELETE,
			IDC_STATIC1,
			IDC_ENABLE_CHARACTER_CONVERSION,
			IDHELP,
			IDCANCEL,
			IDOK
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_list.SetTabStops(40);
	UPDATE_LB(converts,IDC_LIST);
	m_enabled.SetCheck(get_cfg(CFG_COMMON,"CharConversion",0));
	return TRUE;
}

// ========================================================
	void cfg_conv::OnAdd()
// ========================================================
{
CString ch;
CString replace;
CString all;

 	m_char.GetWindowText(ch);
	if (strlen(ch)>1)
		ERR_MSG_RET("E_PIOCO");

 	m_replace.GetWindowText(replace);
	if (strlen(replace)>10)
		ERR_MSG_RET("E_PINOMTENC");

	if (strlen(ch)==0 || strlen(replace)==0)
	{
	    MessageBeep(0);
		return;
	}

	all=ch+"\t"+replace;
	converts.AddTail(PS all);
	UPDATE_LB(converts,IDC_LIST);
}

// ========================================================
	void cfg_conv::OnDelete()
// ========================================================
{
int sel;

	GET_SELID(IDC_LIST);
	converts.Remove(sel);
	UPDATE_LB(converts,IDC_LIST);
}

// ========================================================
	void cfg_conv::OnCancel()
// ========================================================
{
	converts.LoadFromDelimText("Converts.def",0,'\t');
	CDialog::OnCancel();
}

// ========================================================
	void cfg_conv::OnOK()
// ========================================================
{
	set_cfg(CFG_COMMON,"CharConversion",m_enabled.GetCheck());
	converts.SaveToDelimText("Converts.def",0,'\t');
	CDialog::OnOK();
}

// ========================================================
	void cfg_conv::OnHelp()
// ========================================================
{
	WinHelp(VHELP_CHAR_CONVERSION);
}

BOOL cfg_conv::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
