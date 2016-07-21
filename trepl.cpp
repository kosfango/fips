// trepl.cpp : implementation file
// IDD_CFG_TEXTREPL

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "trepl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CFG_TEXTREPL";
extern CStrList trepls;
CStrList tr;

// ===================================================================
	trepl::trepl(CWnd* pParent ) : CSAPrefsSubDlg(trepl::IDD, pParent)
// ===================================================================
{
	//{{AFX_DATA_INIT(trepl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ===================================================================
	void trepl::DoDataExchange(CDataExchange* pDX)
// ===================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(trepl)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT_STRING, m_string);
	DDX_Control(pDX, IDC_EDIT_REPLACEMENT, m_replacement);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(trepl, CDialog)
	//{{AFX_MSG_MAP(trepl)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR trepl::GetName(void)	{return DlgName;}
// =====================================================================

// ===================================================================
	BOOL trepl::OnInitDialog()
// ===================================================================
{
int  tabs[]={100};
int  lng[]={
			IDC_ADD,
			IDC_DELETE,
			IDC_CHANGE,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TABULATE_LB(IDC_LIST);
 	EXTENT_LB(IDC_LIST,1000);
	UPDATE_LB(trepls,IDC_LIST);
	return TRUE;
}

// ===================================================================
	void trepl::OnAdd()
// ===================================================================
{
CString all,text,repl;
char    repp[1000];

	m_string.GetWindowText(text);
	m_replacement.GetWindowText(repl);
	if (text.IsEmpty() || repl.IsEmpty())	return;
	if (repl.GetLength()>999)	ERR_MSG_RET("E_MTRPSO");

	strcpy(repp,repl);
	remove_LFCR(repp);
	all=text+"\t"+repp;
	m_list.AddString(all);
}

// ===================================================================
	void trepl::OnDelete()
// ===================================================================
{
int sel;

	GET_SELID(IDC_LIST);
	m_list.DeleteString(sel);
}

// ===================================================================
	void trepl::OnChange()
// ===================================================================
{
int		sel;
CString	all,text,repl;
char    repp[1000];

	GET_SELID(IDC_LIST);
	m_string.GetWindowText(text);
	m_replacement.GetWindowText(repl);
	if (text.IsEmpty() || repl.IsEmpty())	return;
	if (repl.GetLength()>999)	ERR_MSG_RET("E_MTRPSO");
	strcpy(repp,repl);
	remove_LFCR(repp);
	all=text+"\t"+repp;
	m_list.DeleteString(sel);
	m_list.InsertString(sel,all);
	SET_SELID(IDC_LIST,sel);
}

// ===================================================================
	void trepl::OnOK()
// ===================================================================
{
	trepls=m_list;
	trepls.SaveToDelimText("TextRepl.def",0,'\t');
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ===================================================================
	void trepl::OnSelchangeList()
// ===================================================================
{
int sel;
CString line,text,repl;
char    repp[10000];

	GET_SELID(IDC_LIST);
	m_list.GetText(sel,line);
	get_token(line,0,text);
	get_token(line,1,repl);
	strcpy(repp,repl);
	insert_LFCR(repp);
	m_string.SetWindowText(text);
	m_replacement.SetWindowText(repp);
}

// ===================================================================
	void trepl::OnHelp()
// ===================================================================
{
	WinHelp(VHELP_TEXT_REPLACEMENT);
}

BOOL trepl::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
