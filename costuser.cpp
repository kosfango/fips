// costuser.cpp : implementation file
// IDD_CFG_COST

#include "stdafx.h"
#include "costuser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList costuserlist;
extern CStrList costbaselist;

static char DlgName []="IDD_CFG_COST";

// ========================================================================
	costuser::costuser(CWnd* pParent ) 	: CSAPrefsSubDlg(costuser::IDD, pParent)
// ========================================================================
{
	//{{AFX_DATA_INIT(costuser)
	m_phone = _T("");
	m_delay = _T("");
	m_cost = _T("");
	m_name = _T("");
	m_workdays = _T("");
	m_unit = _T("");
	m_weekend = _T("");
	//}}AFX_DATA_INIT
}

void costuser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(costuser)
	DDX_Control(pDX, IDC_CHECK_ENABLE, m_enabled);
	DDX_Control(pDX, IDC_LISTB, m_listb);
	DDX_Control(pDX, IDC_LISTU, m_listu);
	DDX_Control(pDX, IDC_BASECOSTS, m_basecosts);
	DDX_Text(pDX, IDC_PHONE, m_phone);
	DDX_Text(pDX, IDC_DELAY, m_delay);
	DDX_Text(pDX, IDC_COST, m_cost);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_WORKDAYS, m_workdays);
	DDX_Text(pDX, IDC_CURRENCY, m_unit);
	DDX_Text(pDX, IDC_WEEKEND, m_weekend);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(costuser, CDialog)
	//{{AFX_MSG_MAP(costuser)
	ON_BN_CLICKED(IDC_ADDB, OnAddB)
	ON_BN_CLICKED(IDC_DELETEB, OnDeleteB)
	ON_BN_CLICKED(IDC_CHANGEB, OnChangeB)
	ON_BN_CLICKED(IDC_ADDU, OnAddU)
	ON_BN_CLICKED(IDC_DELETEU, OnDeleteU)
	ON_BN_CLICKED(IDC_CHANGEU, OnChangeU)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_LISTU, OnSelchangeListU)
	ON_LBN_SELCHANGE(IDC_LISTB, OnSelchangeListB)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR costuser::GetName(void)	{return DlgName;}
// =====================================================================

// ========================================================================
	BOOL costuser::OnInitDialog()
// ========================================================================
{
int tabsb[]={70,85,95,300,1900};
int tabsu[]={70,400};
int lng[]={
			IDC_ADDB,
			IDC_DELETEB,
			IDC_CHANGEB,
			IDC_ADDU,
			IDC_DELETEU,
			IDC_CHANGEU,
			IDC_CHECK_ENABLE,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC7,
			IDC_STATIC8,
			IDC_STATIC9,
			IDC_STATIC10,
			IDC_STATIC11,
			IDC_STATIC12,
			IDC_SAMPLE
			};

  CDialog::OnInitDialog();
  set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	costuserlist.Sort(0);
	m_delay.Format("%d",costuserlist.reserved3);
	m_enabled.SetCheck(costuserlist.defaultindex);

	m_listb.SetTabStops((sizeof(tabsb)/sizeof(int)),tabsb);
	EXTENT_LB(IDC_LISTB,1200);
	UPDATE_LB(costbaselist,IDC_LISTB);
	SET_SELID(IDC_LISTB,0);
	OnSelchangeListB();

	m_listu.SetTabStops((sizeof(tabsu)/sizeof(int)),tabsu);
	UPDATE_LB(costuserlist,IDC_LISTU);
	SET_SELID(IDC_LISTU,0);
	OnSelchangeListU();

	FillBaseCosts();
	UpdateData(0);
	return TRUE;
}

// ========================================================================
	void costuser::OnAddU()
// ========================================================================
{
CString str;

	Disp2StringU(str);
	if (!str.IsEmpty())	m_listu.AddString(str);
}

// ========================================================================
	void costuser::OnDeleteU()
// ========================================================================
{
int sel;

	GET_SELID(IDC_LISTU);
	if (sel==0)
		ERR_MSG_RET("W_DNDEZ");

	m_listu.DeleteString(sel);
}

// ========================================================================
	void costuser::OnChangeU()
// ========================================================================
{
CString str;
int sel;

	GET_SELID(IDC_LISTU);
	Disp2StringU(str);
	m_phone=sel ? str : L("S_152");	// default
	m_listu.DeleteString(sel);
	m_listu.InsertString(sel,str);
	SET_SELID(IDC_LISTU,sel);
}

// ================================================================
	void costuser::OnAddB()
// ================================================================
{
CString str;

	Disp2StringB(str);
	if (!str.IsEmpty())	m_listb.AddString(str);
	FillBaseCosts();
}

// ================================================================
void costuser::OnDeleteB()
// ================================================================
{
CString str,nm1,tmp,nm2;
int sel;

	GET_SELID(IDC_LISTB);
	m_listb.GetText(sel,str);
	get_token(str,0,nm1);
	for (int i=m_listu.GetCount()-1;i>=0;i--)
	{
		m_listu.GetText(i,tmp);
		get_token(tmp,1,nm2);
		if (nm1==nm2)	m_listu.DeleteString(i);
	}
	m_listb.DeleteString(sel);
	FillBaseCosts();
}

// ================================================================
	void costuser::OnChangeB()
// ================================================================
{
CString str,bno,tmp,bn,nm,bnn;
int sel;

	GET_SELID(IDC_LISTB);
	Disp2StringB(str);
	get_token(str,0,bnn);
	m_listb.GetText(sel,tmp);
	get_token(tmp,0,bno);
	for (int i=m_listu.GetCount()-1;i>=0;i--)
	{
		m_listu.GetText(i,tmp);
		get_token(tmp,1,bn);
		if (bno==bn)	
		{
			get_token(tmp,0,nm);
			tmp=nm+"\t"+bnn;
			m_listu.DeleteString(i);
			m_listu.InsertString(i,tmp);
		}
	}
	m_listb.DeleteString(sel);
	m_listb.InsertString(sel,str);
	SET_SELID(IDC_LISTB,sel);
	FillBaseCosts();
}

// ========================================================================
	void costuser::OnOK()
// ========================================================================
{
	UpdateData(1);
	costbaselist=m_listb;
	costuserlist=m_listu;
	costuserlist.defaultindex=m_enabled.GetCheck();
	sscanf(m_delay,"%d",&costuserlist.reserved3);
	costuserlist.SaveToFile("costuser.cfg");
	costbaselist.SaveToFile("costbase.cfg");
	if(m_bCloseOnOk)	CDialog::OnOK();
}

// ========================================================================
	void costuser::OnSelchangeListU()
// ========================================================================
{
CString str;
int		sel;

	GET_SELID(IDC_LISTU);
	m_listu.GetText(sel,str);
	String2DispU(str);
}

// ================================================================
	void costuser::OnSelchangeListB()
// ================================================================
{
CString str;
int		sel;

	GET_SELID(IDC_LISTB);
	m_listb.GetText(sel,str);
	String2DispB(str);
}

/*/ ========================================================================
	void costuser::OnCheckEnable()
// ========================================================================
{
	costuserlist.defaultindex=m_enabled.GetCheck();
}*/

// ========================================================================
	void costuser::FillBaseCosts(void)
// ========================================================================
{
CString str,tmp;

	m_basecosts.ResetContent();
	for (int i=0;i<m_listb.GetCount();i++)
	{
		m_listb.GetText(i,tmp);
		get_token(tmp,0,str);
		m_basecosts.AddString(str);
	}
}

// ========================================================================
	void costuser::String2DispU(LPCSTR str)
// ========================================================================
{
CString tmp;
int		ret;

	get_token(str,0,m_phone);
	get_token(str,1,tmp);
	ret=m_basecosts.FindStringExact(0,tmp);
	if (ret!=CB_ERR)
		m_basecosts.SetCurSel(ret);
	UpdateData(0);
}

// ================================================================
	void costuser::String2DispB(LPCSTR str)
// ================================================================
{
	get_token(str,0,m_name);
	get_token(str,1,m_cost);
	get_token(str,2,m_unit);
	get_token(str,3,m_workdays);
	get_token(str,4,m_weekend);
	UpdateData(0);
}

// ========================================================================
	void costuser::Disp2StringU(CString &str)
// ========================================================================
{
CString tmp;
int		sel;

	UpdateData(1);
	sel=m_basecosts.GetCurSel();
	if (sel==CB_ERR)
	{
		str.Empty();
		return;
	}
	m_basecosts.GetLBText(sel,tmp);
	str=m_phone+"\t"+tmp;
}

// ================================================================
	void costuser::Disp2StringB(CString &str)
// ================================================================
{
	UpdateData(1);
	trim_all(m_weekend);
	trim_all(m_unit);
	trim_all(m_cost);
	trim_all(m_name);
	trim_all(m_workdays);
	str=m_name+"\t"+m_cost+"\t"+m_unit+"\t"+m_workdays+"\t"+m_weekend;
}

// ========================================================================
	void costuser::OnHelp()
// ========================================================================
{
	WinHelp(VHELP_COSTANAL);
}

BOOL costuser::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
