// cfg_apno.cpp : implementation file
// IDD_CFG_PRIVNODES

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_apno.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CFG_PRIVNODES";

cfg_apno::cfg_apno(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(cfg_apno::IDD, pParent)
{
	//{{AFX_DATA_INIT(cfg_apno)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void cfg_apno::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_apno)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_FIDO, m_fido);
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_TELEPHONE_NUMBER, m_telephone_number);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_apno, CDialog)
	//{{AFX_MSG_MAP(cfg_apno)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_apno::GetName(void)	{return DlgName;}
// =====================================================================

// ================================================================
	BOOL cfg_apno::OnInitDialog()
// ================================================================
{
CStrList lst;
int tabs[]={100,180,260,400,1900};
int  lng[]={
			IDC_ADD,
			IDC_DELETE,
			IDC_CHANGE,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC2,
			IDC_STATIC5
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TABULATE_LB(IDC_LIST);
 	EXTENT_LB(IDC_LIST,500);
	lst.LoadFromFile("addnodes.cfg");
	lst.Sort(0);
	UPDATE_LB(lst,IDC_LIST);
	return TRUE;
}

// ================================================================
	void cfg_apno::OnAdd()
// ================================================================
{
CString adr,fido,telnr,desc,str;

	m_name.GetWindowText(adr);
	m_fido.GetWindowText(fido);
	m_telephone_number.GetWindowText(telnr);
	m_description.GetWindowText(desc);
	str=adr+"\t"+fido+"\t"+telnr+"\t"+desc+"\t\t\t";
	m_list.AddString(str);
}

// ================================================================
	void cfg_apno::OnDelete()
// ================================================================
{
CString actual;
int		sel;

	GET_SELID(IDC_LIST);
	m_list.DeleteString(sel);
	SET_SELID(IDC_LIST,sel);
}

// ================================================================
	void cfg_apno::OnOK()
// ================================================================
{
CStrList lst;

	lst=m_list;
	lst.SaveToFile("addnodes.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ================================================================
	void cfg_apno::OnChange()
// ================================================================
{
CString adr,fido,telnr,desc,str;
int		sel;

	GET_SELID(IDC_LIST);
	m_name.GetWindowText(adr);
	m_fido.GetWindowText(fido);
	m_telephone_number.GetWindowText(telnr);
	m_description.GetWindowText(desc);
	str=adr+"\t"+ fido+"\t"+telnr+"\t"+desc+"\t\t\t";
	m_list.DeleteString(sel);
	m_list.InsertString(sel,str);
	SET_SELID(IDC_LIST,sel);
}

// ================================================================
	void cfg_apno::OnSelchangeList()
// ================================================================
{
CString adr,fido,telnr,desc,str;
int		sel;

	GET_SELID(IDC_LIST);
	m_list.GetText(sel,str);
	get_token(str,0,adr);
	get_token(str,1,fido);
	get_token(str,2,telnr);
	get_token(str,3,desc);
	m_name.SetWindowText(adr);
	m_fido.SetWindowText(fido);
	m_telephone_number.SetWindowText(telnr);
	m_description.SetWindowText(desc);
}

// ================================================================
	void cfg_apno::OnHelp()
// ================================================================
{
	WinHelp(VHELP_PRIVATE_NODES);
}

BOOL cfg_apno::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
