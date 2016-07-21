// costbase.cpp : implementation file
// IDD_COST_BASE

#include "stdafx.h"
#include "light.h"
#include "costbase.h"
#include "structs.h"
#include "mystrlst.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList costbaselist;
static char DlgName[]="IDD_COST_BASE";

costbase::costbase(CWnd* pParent ) : CDialog(costbase::IDD, pParent)
{
	//{{AFX_DATA_INIT(costbase)
	m_cost = _T("");
	m_name = _T("");
	m_normal = _T("");
	m_unit = _T("");
	m_weekend = _T("");
	//}}AFX_DATA_INIT
}

void costbase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(costbase)
	DDX_Text(pDX, IDC_EDIT_COST, m_cost);
	DDX_Text(pDX, IDC_EDIT_NAME, m_name);
	DDX_Text(pDX, IDC_EDIT_NORMAL, m_normal);
	DDX_Text(pDX, IDC_EDIT_WAEHRUNG, m_unit);
	DDX_Text(pDX, IDC_EDIT_WEEKEND, m_weekend);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(costbase, CDialog)
	//{{AFX_MSG_MAP(costbase)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ================================================================
	BOOL costbase::OnInitDialog()
// ================================================================
{
int tabs[]={80,110,300,400,1900};
int lang_patches[]={
					   IDC_SET,
					   IDC_ADD,
					   IDC_DELETE,
					   IDOK,
					   IDCANCEL,
					   IDHELP,
					   STATIC1,
					   STATIC2,
					   STATIC3,
					   IDC_STATIC4,
					   IDC_STATIC66,
					   IDC_STATIC98,
					   IDC_STATIC49,
					};
    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lang_patches,sizeof lang_patches/sizeof(int));
	TABULATE_LB(IDC_LIST);
	EXTENT_LB(IDC_LIST,1200);
	UpdateData(0);
	UPDATE_LB(costbaselist,IDC_LIST);
	SET_SELID(IDC_LIST,0);
	OnSelchangeList();
	return TRUE;
}

// ================================================================
	void costbase::OnAdd()
// ================================================================
{
CString str;

	Disp2String(str);
	costbaselist.AddTail(str);
	UPDATE_LB(costbaselist,IDC_LIST);
}

// ================================================================
	void costbase::OnDelete()
// ================================================================
{
int sel;

	GET_SELID(IDC_LIST);
	costbaselist.Remove(sel);
	UPDATE_LB(costbaselist,IDC_LIST);
}

// ================================================================
	void costbase::OnSet()
// ================================================================
{
CString str;
int sel;

	GET_SELID(IDC_LIST);
	Disp2String(str);
	costbaselist.Replace(sel,str);
	UPDATE_LB(costbaselist,IDC_LIST);
	SET_SELID(IDC_LIST,sel);
}

// ================================================================
	void costbase::OnCancel()
// ================================================================
{
	costbaselist.LoadFromFile("costbase.cfg");
	CDialog::OnCancel();
}

// ================================================================
	void costbase::OnOK()
// ================================================================
{
CString str;
CString line;
CString name;
CString cost;
CString normal;
CString weekend;
CString unit;
CStrList lst;
int i;

	UpdateData(1);
	for (int i=0;i<costbaselist.GetCount();i++)
	{
		line=costbaselist.GetString(i);
		get_token(line,0,name);
		get_token(line,1,cost);
		get_token(line,2,normal);
		get_token(line,3,weekend);
		get_token(line,4,unit);
		str=name+"\t"+cost+"\t"+normal+"\t"+weekend+"\t"+unit+"\t\t\t\t\t";
		lst.AddTail(str);
	}

	costbaselist.RemoveAll();
	for (i=0;i<lst.GetCount();i++)
		costbaselist.AddTail(lst.GetString(i));

	costbaselist.SaveToFile("costbase.cfg");
	CDialog::OnOK();
}

// ================================================================
	void costbase::OnSelchangeList()
// ================================================================
{
CString str;
int		sel;

	GET_SELID(IDC_LIST);
	str=costbaselist.GetString(sel);
	String2Disp(str);
}

// ================================================================
	void costbase::String2Disp(CString &line)
// ================================================================
{
	get_token(line,0,m_name);
	get_token(line,1,m_cost);
	get_token(line,2,m_normal);
	get_token(line,3,m_weekend);
	get_token(line,4,m_unit);
	UpdateData(0);
}

// ================================================================
	void costbase::Disp2String(CString &line)
// ================================================================
// format in string   Name\tCosts\tdefinition_normal\tdefinition_weekend\tunit
{
	UpdateData(1);
    trim_all(m_weekend);
    trim_all(m_unit);
    trim_all(m_cost);
    trim_all(m_name);
    trim_all(m_normal);
	line=m_name+"\t"+m_cost+"\t"+m_normal+"\t"+m_weekend+"\t"+m_unit;
}

// ================================================================
	void costbase::OnHelp()
// ================================================================
{
	WinHelp(VHELP_COSTANAL);
}

BOOL costbase::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
