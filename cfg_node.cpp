// cfg_node.cpp : implementation file
// IDD_CFG_NODELIST

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_advn.h"
#include "cfg_apno.h"
#include "cfg_node.h"
#include "adrbook.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CFG_NODELIST";
extern CStrList  Nodelists;

// ===============================================================================
	cfg_node::cfg_node(CWnd* pParent /*=NULL*/): CSAPrefsSubDlg(cfg_node::IDD, pParent)
// ===============================================================================
{
	//{{AFX_DATA_INIT(cfg_node)
	m_nodediff_name = _T("");
	m_nodelist_name = _T("");
	m_nodelist_type = _T("");
	m_edit_point_zone = _T("");
	m_exclude = _T("");
	//}}AFX_DATA_INIT
}

// ===============================================================================
	void cfg_node::DoDataExchange(CDataExchange* pDX)
// ===============================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_node)
	DDX_Control(pDX, IDC_LIST1, m_listctl);
	DDX_Control(pDX, IDC_POINTLIST_ZONE, m_edit_control);
	DDX_Control(pDX, IDC_PNTCHECK, m_check_pointlist);
	DDX_Text(pDX, IDC_NODEDIFF_NAME, m_nodediff_name);
	DDV_MaxChars(pDX, m_nodediff_name, 30);
	DDX_Text(pDX, IDC_NODELIST_NAME, m_nodelist_name);
	DDV_MaxChars(pDX, m_nodelist_name, 30);
	DDX_Text(pDX, IDC_NODELIST_TYPE, m_nodelist_type);
	DDV_MaxChars(pDX, m_nodelist_type, 30);
	DDX_Text(pDX, IDC_POINTLIST_ZONE, m_edit_point_zone);
	DDV_MaxChars(pDX, m_edit_point_zone, 5);
	DDX_Text(pDX, IDC_EDIT_EXCLUDE, m_exclude);
	DDV_MaxChars(pDX, m_exclude, 200);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_node, CDialog)
	//{{AFX_MSG_MAP(cfg_node)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_WM_HELPINFO()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ADDRBOOK, OnBnClickedAddrbook)
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_node::GetName(void)	{return DlgName;}
// =====================================================================

// ===============================================================================
	BOOL cfg_node::OnInitDialog()
// ===============================================================================
{
_listcol lc[]={180,"",85,"",85,"",50,"",30,""};
CStrList lst;
CString	 val;
int		 lng[]={
				IDC_CHANGE,
				IDC_ADD,
				IDC_DELETE,
				IDC_STATIC6,
				IDC_STATIC8,
				IDC_STATIC7,
				IDC_PNTCHECK,
				IDC_STATIC10,
				IDC_STATIC5,
				IDC_STATIC9,
				IDC_ADDRBOOK
				};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	strcpy(lc[0].hdr,L("S_162"));	// description
	strcpy(lc[1].hdr,L("S_298"));	// list name
	strcpy(lc[2].hdr,L("S_165"));	// diff list
	strcpy(lc[3].hdr,L("S_477"));	// zone
	strcpy(lc[4].hdr,L("S_353"));	// point list
	MakeColumns(m_listctl,lc,sizeof(lc)/sizeof(_listcol),DlgName,1);

	lst.LoadFromFile("nodelist.cfg");
	for (int i=0;i<lst.GetCount();i++)
	{
		AddRow(m_listctl,lst.GetString(i));
		val=m_listctl.GetItemText(i,4);
		if (val.GetLength()>0)
			m_listctl.SetItemText(i,4,val[0]=='0' ? " " : "+");
		else
			m_listctl.SetItemText(i,4," ");
	}
	m_exclude=get_cfg(CFG_PREFIX,"Exclude","");
	UpdateData(0);
	return TRUE;
}

// ===============================================================================
	void cfg_node::OnAdd()
// ===============================================================================
{
LONG	dm;
CString str;

	UpdateData(1);
	str.Format("%s\t%s\t%s\t%s\t%d",m_nodelist_type,m_nodelist_name,
		m_nodediff_name,m_edit_point_zone,
		m_check_pointlist.GetCheck() ? "+" : " ");
	AddRow(m_listctl,str);
	SelectRow(m_listctl,m_listctl.GetItemCount()-1);
	OnItemchangedList1(NULL,&dm);
	m_listctl.SetFocus();
}

// ===============================================================================
	void cfg_node::OnChange()
// ===============================================================================
{
int		sel;
CString str;

	GET_SELECT(m_listctl);
	UpdateData(1);
	str.Format("%s\t%s\t%s\t%s\t%s",m_nodelist_type,m_nodelist_name,m_nodediff_name,
		m_edit_point_zone,m_check_pointlist.GetCheck() ? "+" : " ");
	ReplaceRow(m_listctl,sel,str);
	m_listctl.SetFocus();
}

// ===============================================================================
	void cfg_node::OnDelete()
// ===============================================================================
{
int		sel;
LONG	dm;

	GET_SELECT(m_listctl);
	m_listctl.DeleteItem(sel);
	SelectRow(m_listctl,sel ? sel-1 : 0);
	OnItemchangedList1(NULL,&dm);
	m_listctl.SetFocus();
}

// ===============================================================================
	void cfg_node::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
// ===============================================================================
{
int sel;

	GET_SELECT(m_listctl);
	m_nodelist_type=m_listctl.GetItemText(sel,0);
	m_nodelist_name=m_listctl.GetItemText(sel,1);
	m_nodediff_name=m_listctl.GetItemText(sel,2);
	m_edit_point_zone=m_listctl.GetItemText(sel,3);
	m_check_pointlist.SetCheck(m_listctl.GetItemText(sel,4)=="+");
	UpdateData(0);
	*pResult = 0;
}

// ===============================================================================
	void cfg_node::OnOK()
// ===============================================================================
{
	UpdateData(1);
	set_cfg(CFG_PREFIX,"Exclude",m_exclude);
	for (int i=0;i<m_listctl.GetItemCount();i++)
		m_listctl.SetItemText(i,4,m_listctl.GetItemText(i,4)=="+" ? "1" : "0");
	Nodelists=m_listctl;
	Nodelists.SaveToFile("nodelist.cfg");
	SaveColumnsWidth(m_listctl,DlgName,1);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ===============================================================================
	void cfg_node::OnPrivate()  // Private Nodelist
// ===============================================================================
{
	cfg_apno dlg;
	dlg.DoModal();
}

// ===============================================================================
	void cfg_node::OnBnClickedAddrbook()
// ===============================================================================
{
	adrbook dlg;
	dlg.DoModal();
}

// ===============================================================================
	void cfg_node::OnHelp()
// ===============================================================================
{
	WinHelp(VHELP_CFG_NODELIST);
}

BOOL cfg_node::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
