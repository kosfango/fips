// kludges.cpp : implementation file
// IDD_CFG_KLUDGES

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "kludges.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CFG_KLUDGES";

extern CStrList AddKludges;
extern CStrList HideKludges;
extern _gconfig gc;
/////////////////////////////////////////////////////////////////////////////
// kludges dialog


kludges::kludges(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(kludges::IDD, pParent)
{
	//{{AFX_DATA_INIT(kludges)
	m_mykludge = _T("");
	m_hiskludge = _T("");
	m_hidemode = -1;
	//}}AFX_DATA_INIT
}


void kludges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(kludges)
	DDX_Control(pDX, IDC_LIST2, m_hidkludges);
	DDX_Control(pDX, IDC_LIST1, m_addkludges);
	DDX_Text(pDX, IDC_MYKLUDGE, m_mykludge);
	DDX_Text(pDX, IDC_HISKLUDGE, m_hiskludge);
	DDX_Radio(pDX, IDC_HIDEALL, m_hidemode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(kludges, CDialog)
	//{{AFX_MSG_MAP(kludges)
	ON_BN_CLICKED(IDC_ADD1, OnAdd1)
	ON_BN_CLICKED(IDC_DELETE1, OnDelete1)
	ON_BN_CLICKED(IDC_DELETE2, OnDelete2)
	ON_BN_CLICKED(IDC_ADD2, OnAdd2)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
	ON_BN_CLICKED(IDC_CHANGE1, OnChange1)
	ON_BN_CLICKED(IDC_CHANGE2, OnChange2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// kludges message handlers

// =====================================================================
LPCSTR kludges::GetName(void)	{return DlgName;}
// =====================================================================

// =====================================================================
	BOOL kludges::OnInitDialog() 
// =====================================================================
{
CStrList lst;
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_ADD1,
			IDC_DELETE1,
			IDC_ADD2,
			IDC_DELETE2,
			IDC_HIDEALL,
			IDC_HIDESEL,
			IDC_CHANGE1,
			IDC_CHANGE2,
			IDC_HIDE,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	lst.LoadFromDelimString(get_cfg(CFG_EDIT,"ExtraKludges",""),';');
	UPDATE_LB(lst,IDC_LIST1);
	get_hiddenkludges(lst);
	m_hidemode=get_cfg(CFG_EDIT,"HiddenMode",0);
	UPDATE_LB(lst,IDC_LIST2);
	UpdateData(FALSE);
	return TRUE;
}

	void kludges::OnOK() 
{
CString str,tmp;
int		i;

	UpdateData(TRUE);
	str.Empty();
	for (i=0;i<m_addkludges.GetCount();i++)
	{
		if (!str.IsEmpty())	str+=";";
		m_addkludges.GetText(i,tmp);
		str+=tmp;
	}
	AddKludges.LoadFromDelimString(str,';');
	set_cfg(CFG_EDIT,"ExtraKludges",str);
	str.Empty();
	for (i=0;i<m_hidkludges.GetCount();i++)
	{
		if (!str.IsEmpty())	str+=";";
		m_hidkludges.GetText(i,tmp);
		str+=tmp;
	}
	HideKludges.LoadFromDelimString(str,';');
	gc.HiddenMode=m_hidemode;
	set_cfg(CFG_EDIT,"HiddenKludges",str);
	set_cfg(CFG_EDIT,"HiddenMode",m_hidemode);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

void kludges::OnAdd1() 
{
	UpdateData(1);
	trim_all(m_mykludge);
	if (!m_mykludge.IsEmpty() && m_addkludges.FindStringExact(-1,m_mykludge)==LB_ERR)
		m_addkludges.AddString(m_mykludge);
}

void kludges::OnDelete1() 
{
int sel;

	GET_SELID(IDC_LIST1);
	m_addkludges.DeleteString(sel);
}

void kludges::OnSelchangeList1() 
{
int	sel;
	
	GET_SELID(IDC_LIST1);
	m_addkludges.GetText(sel,m_mykludge);
	UpdateData(0);
}

void kludges::OnChange1() 
{
int	sel;
	
	UpdateData(1);
	GET_SELID(IDC_LIST1);
	trim_all(m_mykludge);
	if (!m_mykludge.IsEmpty() && m_addkludges.FindStringExact(-1,m_mykludge)==LB_ERR)
	{
		m_addkludges.DeleteString(sel);
		m_addkludges.InsertString(sel,m_mykludge);
		SET_SELID(IDC_LIST1,sel);
	}
}

void kludges::OnAdd2() 
{
	UpdateData(1);
	trim_all(m_hiskludge);
	if (!m_hiskludge.IsEmpty() && m_hidkludges.FindStringExact(-1,m_hiskludge)==LB_ERR)
		m_hidkludges.AddString(m_hiskludge);
}

void kludges::OnDelete2() 
{
int sel;

	GET_SELID(IDC_LIST2);
	m_hidkludges.DeleteString(sel);
}

void kludges::OnSelchangeList2() 
{
int	sel;
	
	GET_SELID(IDC_LIST2);
	m_hidkludges.GetText(sel,m_hiskludge);
	UpdateData(0);
}

void kludges::OnChange2() 
{
int	sel;
	
	UpdateData(1);
	GET_SELID(IDC_LIST2);
	trim_all(m_hiskludge);
	if (!m_hiskludge.IsEmpty() && m_hidkludges.FindStringExact(-1,m_hiskludge)==LB_ERR)
	{
		m_hidkludges.DeleteString(sel);
		m_hidkludges.InsertString(sel,m_hiskludge);
		SET_SELID(IDC_LIST2,sel);
	}
}
	void kludges::OnHelp() 
{
	WinHelp(VHELP_KLUDGES);
}

BOOL kludges::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
