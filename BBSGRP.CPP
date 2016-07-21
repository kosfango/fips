// bbsgrp.cpp : implementation file
// IDD_BBS_LEVELS

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "bbsgrp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CStrList bbsgrps;
static char DlgName[]="IDD_BBS_LEVELS";

// ==================================================================
	bbsgrp::bbsgrp(CWnd* pParent ) : CSAPrefsSubDlg(bbsgrp::IDD, pParent)
// ==================================================================
{
	//{{AFX_DATA_INIT(bbsgrp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ==================================================================
	void bbsgrp::DoDataExchange(CDataExchange* pDX)
// ==================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(bbsgrp)
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT5, m_edit_time);
	DDX_Control(pDX, IDC_EDIT2, m_edit_comment);
	DDX_Control(pDX, IDC_EDIT1, m_edit_level);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(bbsgrp, CDialog)
	//{{AFX_MSG_MAP(bbsgrp)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR bbsgrp::GetName(void)	{return DlgName;}
// =====================================================================

// ==================================================================
	BOOL bbsgrp::OnInitDialog()
// ==================================================================
{
int  tabs[]={50,100,150};
int  lng[]={
			IDC_ADD,
			IDC_CHANGE,
			IDC_DELETE,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			};
    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_list.SetTabStops((sizeof(tabs)/sizeof(int)),tabs);
	bbsgrps.LoadFromFile("bbsgrps.cfg");
	bbsgrps.Sort(0,1);
	UPDATE_LB(bbsgrps,IDC_LIST1);
	return TRUE;
}

// ==================================================================
	int bbsgrp::MakeString(CString &erg)
// ==================================================================
{
CString level,time,comment;
int     ti,le;

	m_edit_time.GetWindowText(time);
	m_edit_comment.GetWindowText(comment);
	m_edit_level.GetWindowText(level);

	le=atoi(level);
	ti=atoi(time);
	if (le==0 || ti==0)
		ERR_MSG_RET0("E_GDCCON");

	erg.Format("%d\t%d\t%s",le,ti,comment);
	return 1;
}

// ==================================================================
	void bbsgrp::OnAdd()
// ==================================================================
{
CString buf;

	if (!MakeString(buf))	return;
	bbsgrps.AddTail(buf);
	UPDATE_LB(bbsgrps,IDC_LIST1);
}

// ==================================================================
	void bbsgrp::OnDelete()
// ==================================================================
{
int sel;

	GET_SELID(IDC_LIST1);
	bbsgrps.Remove(sel);
	UPDATE_LB(bbsgrps,IDC_LIST1);
}

// ==================================================================
	void bbsgrp::OnChange()
// ==================================================================
{
CString buf;
int		sel;

	if (!MakeString(buf))	return;
	GET_SELID(IDC_LIST1);
    bbsgrps.Replace(sel,buf);
	UPDATE_LB(bbsgrps,IDC_LIST1);
	SET_SELID(IDC_LIST1,sel);
}

// ==================================================================
	void bbsgrp::OnOK()
// ==================================================================
{
	bbsgrps.SaveToFile("bbsgrps.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ==================================================================
	void bbsgrp::OnSelchangeList1()
// ==================================================================
{
CString level,time,comment,help;
int		sel;

	GET_SELID(IDC_LIST1);

	help=bbsgrps.GetString(sel);
	get_token(help,0,level);
	get_token(help,1,time);
	get_token(help,2,comment);

	m_edit_time.SetWindowText	(time);
	m_edit_comment.SetWindowText(comment);
	m_edit_level.SetWindowText	(level);
}

// ==================================================================
	void bbsgrp::OnHelp()
// ==================================================================
{
	WinHelp(VHELP_BBS_GROUPS);
}

BOOL bbsgrp::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
