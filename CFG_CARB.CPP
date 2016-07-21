// cfg_carb.cpp : implementation file
// IDD_CFG_CARBONS

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_carb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CStrList	carbons;
static char DlgName[]="IDD_CFG_CARBONS";

// ====================================================
cfg_carb::cfg_carb(CWnd* pParent ) : CSAPrefsSubDlg(cfg_carb::IDD, pParent)
// ====================================================
{
	//{{AFX_DATA_INIT(cfg_carb)
	m_cc_echo = FALSE;
	m_cc_netmail = FALSE;
	m_ign_case = FALSE;
	m_nsave_orig = FALSE;
	m_globalasave = FALSE;
	//}}AFX_DATA_INIT
}

// ====================================================
void cfg_carb::DoDataExchange(CDataExchange* pDX)
// ====================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_carb)
	DDX_Control(pDX, IDC_LIST1, m_listctl);
	DDX_Control(pDX, IDC_NOT_COPY_AREAS, m_do_not_copy);
	DDX_Control(pDX, IDC_DEST_AREA, m_destarea);
	DDX_Control(pDX, IDC_STRING, m_string);
	DDX_Check(pDX, IDC_CHECK_CC_ECHOMET, m_cc_echo);
	DDX_Check(pDX, IDC_CHECK_CC_NETMET, m_cc_netmail);
	DDX_Check(pDX, IDC_IGNORE_CASE, m_ign_case);
	DDX_Check(pDX, IDC_NSAVE_ORIGINAL, m_nsave_orig);
	DDX_Check(pDX, IDC_GLOBAL_AUTOSAVE, m_globalasave);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_carb, CDialog)
	//{{AFX_MSG_MAP(cfg_carb)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_carb::GetName(void)	{return DlgName;}
// =====================================================================

// ====================================================
	BOOL cfg_carb::OnInitDialog()
// ====================================================
{
CString	 val;
_listcol lc[]={ 120,"",20,"",20,"",160,"",160,""};
int		 lng[]={
				IDC_IGNORE_CASE,
				IDC_STATIC2,
				IDC_ADD,
				IDC_DELETE,
				IDC_STATIC1,
				IDC_STATIC3,
				IDC_STATIC5,
				IDC_CHECK_CC_NETMET,
				IDC_CHECK_CC_ECHOMET,
				IDC_STATIC4,
				IDC_CHANGE,
				IDC_NSAVE_ORIGINAL,
				IDC_GLOBAL_AUTOSAVE,
				};

	CDialog::OnInitDialog();
	strcpy(lc[0].hdr,L("S_398"));	// search pattern
	strcpy(lc[1].hdr,L("S_273"));	// ignore case
	strcpy(lc[3].hdr,L("S_141"));	// cc area
	strcpy(lc[4].hdr,L("S_407"));	// excluded areas
	strcpy(lc[2].hdr,L("S_348"));	// do not save original
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	carbons.LoadFromFile("carbons.cfg");
	m_cc_netmail=carbons.reserved3;
	m_cc_echo=carbons.reserved4;
	MakeColumns(m_listctl,lc,sizeof(lc)/sizeof(_listcol),DlgName,1);
	for (int i=0;i<carbons.GetCount();i++)
	{
		AddRow(m_listctl,carbons.GetString(i));
		val=m_listctl.GetItemText(i,4);
		m_listctl.SetItemText(i,4,m_listctl.GetItemText(i,3));
		m_listctl.SetItemText(i,3,m_listctl.GetItemText(i,2));
		m_listctl.SetItemText(i,2,val);
		val=m_listctl.GetItemText(i,1);
		if (val.GetLength()>0)
			m_listctl.SetItemText(i,1,(val[0]=='I' || val[0]=='+') ? "+" : " ");
		else
			m_listctl.SetItemText(i,1," ");
	}
	m_globalasave=get_cfg(CFG_COMMON,"GlobalASave",1);
	UpdateData(0);
	return TRUE;
}

// ====================================================
	void cfg_carb::OnOK()
// ====================================================
{
CString	str;

	UpdateData(1);
	carbons.RemoveAll();
	carbons.reserved3=m_cc_netmail;
	carbons.reserved4=m_cc_echo;
	for (int i=0;i<m_listctl.GetItemCount();i++)
	{
		str.Format("%s\t%s\t%s\t%s\t%s",m_listctl.GetItemText(i,0),
			m_listctl.GetItemText(i,1),m_listctl.GetItemText(i,3),
			m_listctl.GetItemText(i,4),m_listctl.GetItemText(i,2));
		carbons.AddTail(str);
	}
	carbons.SaveToFile("carbons.cfg");
	set_cfg(CFG_COMMON,"GlobalASave",m_globalasave);
	SaveColumnsWidth(m_listctl,DlgName,1);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ====================================================
	void cfg_carb::OnAdd()
// ====================================================
{
CString text,dest,nocopy,buf;

	UpdateData(1);
	m_string.GetWindowText(text);
	m_destarea.GetWindowText(dest);
	m_do_not_copy.GetWindowText(nocopy);

	buf.Format("%s\t%s\t%s\t%s\t%s",trim_all(text),m_ign_case ? "+" : " ",
		m_nsave_orig ? "+" : " ",trim_all(dest),trim_all(nocopy));
	AddRow(m_listctl,buf);
}

// ====================================================
	void cfg_carb::OnDelete()
// ====================================================
{
int sel;

	GET_SELECT(m_listctl);
	m_listctl.DeleteItem(sel);
}

// ====================================================
	void cfg_carb::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
// ====================================================
{
int		sel;

	GET_SELECT(m_listctl);
	m_string.SetWindowText(m_listctl.GetItemText(sel,0));
	m_ign_case=m_listctl.GetItemText(sel,1)=="+";
	m_nsave_orig=m_listctl.GetItemText(sel,2)=="+";
	m_destarea.SetWindowText(m_listctl.GetItemText(sel,3));
	m_do_not_copy.SetWindowText(m_listctl.GetItemText(sel,4));
	UpdateData(0);
	*pResult = 0;
}

// ====================================================
	void cfg_carb::OnHelp()
// ====================================================
{
	WinHelp(VHELP_CFG_CARBONCOPY);
}

BOOL cfg_carb::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
