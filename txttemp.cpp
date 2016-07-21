// txttemp.cpp : implementation file
// IDD_CFG_TEXTIMP

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "trepl.h"
#include "txttemp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CFG_TEXTIMP";

// ==============================================================================
	txttemp::txttemp(CWnd* pParent /*=NULL*/) 	: CSAPrefsSubDlg(txttemp::IDD, pParent)
// ==============================================================================
{
	//{{AFX_DATA_INIT(txttemp)
	m_0 = _T("");
	m_1 = _T("");
	m_2 = _T("");
	m_3 = _T("");
	m_4 = _T("");
	m_5 = _T("");
	m_6 = _T("");
	m_7 = _T("");
	m_8 = _T("");
	m_9 = _T("");
	m_cod1 = -1;
	m_cod2 = -1;
	m_cod3 = -1;
	m_cod4 = -1;
	m_cod5 = -1;
	m_cod6 = -1;
	m_cod7 = -1;
	m_cod8 = -1;
	m_cod9 = -1;
	m_cod0 = -1;
	//}}AFX_DATA_INIT
}

// ==============================================================================
	void txttemp::DoDataExchange(CDataExchange* pDX)
// ==============================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(txttemp)
	DDX_Control(pDX, IDC_BROWSE9, m_br9);
	DDX_Control(pDX, IDC_BROWSE8, m_br8);
	DDX_Control(pDX, IDC_BROWSE7, m_br7);
	DDX_Control(pDX, IDC_BROWSE6, m_br6);
	DDX_Control(pDX, IDC_BROWSE5, m_br5);
	DDX_Control(pDX, IDC_BROWSE4, m_br4);
	DDX_Control(pDX, IDC_BROWSE3, m_br3);
	DDX_Control(pDX, IDC_BROWSE2, m_br2);
	DDX_Control(pDX, IDC_BROWSE10, m_br0);
	DDX_Control(pDX, IDC_BROWSE, m_br1);
	DDX_Text(pDX, IDC_0, m_0);
	DDX_Text(pDX, IDC_1, m_1);
	DDX_Text(pDX, IDC_2, m_2);
	DDX_Text(pDX, IDC_3, m_3);
	DDX_Text(pDX, IDC_4, m_4);
	DDX_Text(pDX, IDC_5, m_5);
	DDX_Text(pDX, IDC_6, m_6);
	DDX_Text(pDX, IDC_7, m_7);
	DDX_Text(pDX, IDC_8, m_8);
	DDX_Text(pDX, IDC_9, m_9);
	DDX_Radio(pDX, IDC_DOS1, m_cod1);
	DDX_Radio(pDX, IDC_DOS2, m_cod2);
	DDX_Radio(pDX, IDC_DOS3, m_cod3);
	DDX_Radio(pDX, IDC_DOS4, m_cod4);
	DDX_Radio(pDX, IDC_DOS5, m_cod5);
	DDX_Radio(pDX, IDC_DOS6, m_cod6);
	DDX_Radio(pDX, IDC_DOS7, m_cod7);
	DDX_Radio(pDX, IDC_DOS8, m_cod8);
	DDX_Radio(pDX, IDC_DOS9, m_cod9);
	DDX_Radio(pDX, IDC_DOS0, m_cod0);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(txttemp, CDialog)
	//{{AFX_MSG_MAP(txttemp)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_BROWSE, OnBr1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBr2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBr3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBr4)
	ON_BN_CLICKED(IDC_BROWSE5, OnBr5)
	ON_BN_CLICKED(IDC_BROWSE6, OnBr6)
	ON_BN_CLICKED(IDC_BROWSE7, OnBr7)
	ON_BN_CLICKED(IDC_BROWSE8, OnBr8)
	ON_BN_CLICKED(IDC_BROWSE9, OnBr9)
	ON_BN_CLICKED(IDC_BROWSE10, OnBr0)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR txttemp::GetName(void)	{return DlgName;}
// =====================================================================

// ==============================================================================
	BOOL txttemp::OnInitDialog()
// ==============================================================================
{
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	parse_textimp_str("Alt0",m_0,m_cod0);
	parse_textimp_str("Alt1",m_1,m_cod1);
	parse_textimp_str("Alt2",m_2,m_cod2);
	parse_textimp_str("Alt3",m_3,m_cod3);
	parse_textimp_str("Alt4",m_4,m_cod4);
	parse_textimp_str("Alt5",m_5,m_cod5);
	parse_textimp_str("Alt6",m_6,m_cod6);
	parse_textimp_str("Alt7",m_7,m_cod7);
	parse_textimp_str("Alt8",m_8,m_cod8);
	parse_textimp_str("Alt9",m_9,m_cod9);
	UpdateData(0);
	return TRUE;
}

// ==============================================================================
	void txttemp::OnOK()
// ==============================================================================
{
CString str;

	UpdateData(1);
	str.Format("%s,%d",m_0,m_cod0);
	set_cfg(CFG_TEXTIMP,"Alt0",str);
	str.Format("%s,%d",m_1,m_cod1);
	set_cfg(CFG_TEXTIMP,"Alt1",str);
	str.Format("%s,%d",m_2,m_cod2);
	set_cfg(CFG_TEXTIMP,"Alt2",str);
	str.Format("%s,%d",m_3,m_cod3);
	set_cfg(CFG_TEXTIMP,"Alt3",str);
	str.Format("%s,%d",m_4,m_cod4);
	set_cfg(CFG_TEXTIMP,"Alt4",str);
	str.Format("%s,%d",m_5,m_cod5);
	set_cfg(CFG_TEXTIMP,"Alt5",str);
	str.Format("%s,%d",m_6,m_cod6);
	set_cfg(CFG_TEXTIMP,"Alt6",str);
	str.Format("%s,%d",m_7,m_cod7);
	set_cfg(CFG_TEXTIMP,"Alt7",str);
	str.Format("%s,%d",m_8,m_cod8);
	set_cfg(CFG_TEXTIMP,"Alt8",str);
	str.Format("%s,%d",m_9,m_cod9);
	set_cfg(CFG_TEXTIMP,"Alt9",str);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ==============================================================================
	void txttemp::OnBr1() 
{
	GetFile(m_1);
	UpdateData(0);
}
	void txttemp::OnBr2() 
{
	GetFile(m_2);
	UpdateData(0);
}
	void txttemp::OnBr3() 
{
	GetFile(m_3);
	UpdateData(0);
}
	void txttemp::OnBr4() 
{
	GetFile(m_4);
	UpdateData(0);
}
	void txttemp::OnBr5() 
{
	GetFile(m_5);
	UpdateData(0);
}
	void txttemp::OnBr6() 
{
	GetFile(m_6);
	UpdateData(0);
}
	void txttemp::OnBr7() 
{
	GetFile(m_7);
	UpdateData(0);
}
	void txttemp::OnBr8() 
{
	GetFile(m_8);
	UpdateData(0);
}
	void txttemp::OnBr9() 
{
	GetFile(m_9);
	UpdateData(0);
}
	void txttemp::OnBr0() 
{
	GetFile(m_0);
	UpdateData(0);
}

// ==============================================================================
	BOOL txttemp::GetFile(CString &path)
// ==============================================================================
{
CString	str;
int		ret;

	str.LoadString(IDS_TEXTFILT);
	CFileDialog	dlg(TRUE,NULL,path,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return FALSE;
	path=dlg.GetPathName();
	return TRUE;
}

// ==============================================================================
	void txttemp::OnHelp()
// ==============================================================================
{
	WinHelp(VHELP_TEXT_IMPORT);
}

// ==============================================================================
	BOOL txttemp::OnHelpInfo(HELPINFO* pHelpInfo) 
// ==============================================================================
{
	OnHelp();
	return TRUE;
}
