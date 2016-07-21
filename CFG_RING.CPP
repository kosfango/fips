// cfg_ring.cpp : implementation file
// IDD_CFG_INRINGS

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_ring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList income_times;
extern int	gIncomeDevicesHaveChanged;
static char DlgName[]="IDD_CFG_INRINGS";

// ========================================================================
	cfg_ring::cfg_ring(CWnd* pParent ) 	: CSAPrefsSubDlg(cfg_ring::IDD, pParent)
// ========================================================================
{
	//{{AFX_DATA_INIT(cfg_ring)
	m_nr_isdn = _T("");
	m_nr_m1 = _T("");
	m_nr_m2 = _T("");
	m_pause_isdn = _T("");
	m_pause_m1 = _T("");
	m_pause_m2 = _T("");
	m_from = _T("");
	m_to = _T("");
	m_for1 = FALSE;
	m_for2 = FALSE;
	//}}AFX_DATA_INIT
}

// ========================================================================
	void cfg_ring::DoDataExchange(CDataExchange* pDX)
// ========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_ring)
	DDX_Control(pDX, IDC_DISABLE, m_disable);
	DDX_Text(pDX, IDC_NR_ISDN, m_nr_isdn);
	DDX_Text(pDX, IDC_NR_S1, m_nr_m1);
	DDX_Text(pDX, IDC_NR_S2, m_nr_m2);
	DDX_Text(pDX, IDC_PAUSE_ISDN, m_pause_isdn);
	DDX_Text(pDX, IDC_PAUSE_S1, m_pause_m1);
	DDX_Text(pDX, IDC_PAUSE_S2, m_pause_m2);
	DDX_Text(pDX, IDC_FROM, m_from);
	DDX_Text(pDX, IDC_TO, m_to);
	DDX_Check(pDX, IDC_FOR1, m_for1);
	DDX_Check(pDX, IDC_FOR2, m_for2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_ring, CDialog)
	//{{AFX_MSG_MAP(cfg_ring)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_ring::GetName(void)	{return DlgName;}
// =====================================================================

// ========================================================================
	BOOL cfg_ring::OnInitDialog()
// ========================================================================
{
CString str;
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_DISABLE,
			IDC_STATIC6,
			IDC_STATIC7,
			IDC_STATIC9,
			IDC_STATIC11,
			IDC_STATIC8,
			IDC_STATIC10,
			IDC_FOR1,
			IDC_FOR2,
			IDC_STATIC12,
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	str=get_cfg(CFG_INCOMIN,"Modem1","3,10");
	get_token(str,0,m_nr_m1,',');
	get_token(str,1,m_pause_m1,',');

	str=get_cfg(CFG_INCOMIN,"Modem2","3,10");
	get_token(str,0,m_nr_m2,',');
	get_token(str,1,m_pause_m2,',');

	str=get_cfg(CFG_INCOMIN,"ISDN","3,10");
	get_token(str,0,m_nr_isdn,',');
	get_token(str,1,m_pause_isdn,',');
	
	m_disable.SetCheck(income_times.defaultindex);
	m_for1=income_times.reserved3 & 1;
	m_for2=income_times.reserved3 & 2;
	m_from.Format("%d",income_times.reserved4);
	m_to.Format("%d",income_times.reserved5);
	UpdateData(0);
	return TRUE;
}

// ========================================================================
	void cfg_ring::OnOK()
// ========================================================================
{
int		from=0,to=0;

	UpdateData(1);
	if (m_nr_m1.IsEmpty() || m_pause_m1.IsEmpty() || m_nr_m2.IsEmpty() ||
		m_pause_m2.IsEmpty() || m_nr_isdn.IsEmpty() || m_pause_isdn.IsEmpty())
			ERR_MSG_RET("E_PFUAE");
	sscanf(m_from,"%d",&from);
	sscanf(m_to,"%d",&to);
	if (m_for1 || m_for2)
	{
		if (from<1)	ERR_MSG_RET("E_WFSPCA");
		if (to<1 || to<from)	ERR_MSG_RET("E_WFSPCA");
	}

	set_cfg(CFG_INCOMIN,"Modem1",m_nr_m1+","+m_pause_m1);
	set_cfg(CFG_INCOMIN,"Modem2",m_nr_m2+","+m_pause_m2);
	set_cfg(CFG_INCOMIN,"ISDN",m_nr_isdn+","+m_pause_isdn);
	
	income_times.defaultindex=m_disable.GetCheck();
	income_times.reserved3=0;
	if (m_for1)	income_times.reserved3 |=1;
	if (m_for2)	income_times.reserved3 |=2;
	income_times.reserved4=from;
	income_times.reserved5=to;
	income_times.SaveToFile("incomet.cfg");
	gIncomeDevicesHaveChanged=1;
	
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ========================================================================
	void cfg_ring::OnHelp()
// ========================================================================
{
	WinHelp(VHELP_CFG_INCOME);
}

BOOL cfg_ring::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
