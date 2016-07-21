// cfg_inco.cpp : implementation file
// IDD_CFG_INCOME

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_inco.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList income_times;
extern int	gIncomeDevicesHaveChanged;
static char DlgName[]="IDD_CFG_INCOME";

void SetCheckbox(CString &line,int id,CButton &ctl);

// =========================================================================
	cfg_inco::cfg_inco(CWnd* pParent ) : CSAPrefsSubDlg(cfg_inco::IDD, pParent)
// =========================================================================
{
	//{{AFX_DATA_INIT(cfg_inco)
	//}}AFX_DATA_INIT
}

// =========================================================================
	void cfg_inco::DoDataExchange(CDataExchange* pDX)
// =========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_inco)
	DDX_Control(pDX, IDC_SERVICE_FAX, m_service_fax);
	DDX_Control(pDX, IDC_SERVICE_MAILBOX, m_service_mailbox);
	DDX_Control(pDX, IDC_SERVICE_EMSI, m_service_emsi);
	DDX_Control(pDX, IDC_SECOND_MODEM, m_secondmodem);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_ISDN_LINE, m_isdnline);
	DDX_Control(pDX, IDC_FIRST_MODEM, m_firstmodem);
	DDX_Control(pDX, IDC_EDIT_TO, m_edit_to);
	DDX_Control(pDX, IDC_EDIT_FROM, m_edit_from);
	DDX_Control(pDX, IDC_CHECK_WEDNESDAY, m_we);
	DDX_Control(pDX, IDC_CHECK_THURSDAY, m_th);
	DDX_Control(pDX, IDC_CHECK_THUESDAY, m_tu);
	DDX_Control(pDX, IDC_CHECK_SUNDAY, m_su);
	DDX_Control(pDX, IDC_CHECK_SATURADAY, m_sa);
	DDX_Control(pDX, IDC_CHECK_MONDAY, m_mo);
	DDX_Control(pDX, IDC_CHECK_FRIDAY, m_fr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_inco, CDialog)
	//{{AFX_MSG_MAP(cfg_inco)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_inco::GetName(void)	{return DlgName;}
// =====================================================================

// =========================================================================
	BOOL cfg_inco::OnInitDialog()
// =========================================================================
{
int		tabs[]={50,300};
int		lng[]={
				IDC_STATIC1,
				IDC_STATIC2,
				IDC_STATIC3,
				IDC_STATIC4,
				IDC_STATIC5,
				IDC_STATIC6,
				IDC_STATIC7,
				IDC_FIRST_MODEM,
				IDC_SECOND_MODEM,
				IDC_ISDN_LINE,
				IDC_SERVICE_EMSI,
				IDC_SERVICE_MAILBOX,
				IDC_ADD,
				IDC_DELETE,
				IDC_CHANGE,
				IDC_STATIC12,
				IDC_STATIC13,
				IDC_STATIC11,
				IDC_STATIC10,
				IDC_STATIC9,
				IDC_STATIC14,
				IDC_STATIC8,
				IDC_STATIC15,
				};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_list.SetTabStops(sizeof(tabs)/sizeof(int),tabs);
	UPDATE_LB(income_times,IDC_LIST1);
	return TRUE;
}

// =========================================================================
	void cfg_inco::OnAdd()
// =========================================================================
{
CString str;
int     sel;

	if (!make_string(str))	return;
	m_list.AddString(str);
	sel=m_list.GetCount()-1;
	if (sel<0)	return;
	SET_SELID(IDC_LIST1,sel);
	OnSelchangeList1();
}

// =========================================================================
	void cfg_inco::OnDelete()
// =========================================================================
{
int		sel;

	GET_SELID(IDC_LIST1);
	m_list.DeleteString(sel);
	sel=m_list.GetCount()-1;
	if (sel<0)	return;
	SET_SELID(IDC_LIST1,sel);
	OnSelchangeList1();
}

// =========================================================================
	void cfg_inco::OnOK()
// =========================================================================
{
	income_times=m_list;
	income_times.SaveToFile("incomet.cfg");
	gIncomeDevicesHaveChanged=1;
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =========================================================================
	void cfg_inco::OnChange()
// =========================================================================
{
int sel;
CString str;

	GET_SELID(IDC_LIST1);
	make_string(str);
	m_list.DeleteString(sel);
    m_list.InsertString(sel,str);
	SET_SELID(IDC_LIST1,sel);
}

// =========================================================================
	int cfg_inco::make_string(CString &line)
// =========================================================================
{
CString str,tmp;
int     from,to;

	str="\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	m_edit_to.GetWindowText(tmp);
	set_tabbed_string(str,IDINCCm_edit_to,PS tmp);
	m_edit_from.GetWindowText(tmp);
	set_tabbed_string(str,IDINCCm_edit_from,PS tmp);
	set_tabbed_string(str,IDINCCm_firstmodem,m_firstmodem.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_secondmodem,m_secondmodem.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_isdnline,m_isdnline.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_service_mailbox,m_service_mailbox.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_service_emsi,m_service_emsi.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_service_fax,m_service_fax.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_mo,m_mo.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_tu,m_tu.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_we,m_we.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_th,m_th.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_fr,m_fr.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_sa,m_sa.GetCheck() ? "1" : "0");
	set_tabbed_string(str,IDINCCm_su,m_su.GetCheck() ? "1" : "0");

	line=str;

	if (!m_mo.GetCheck() && !m_tu.GetCheck() && !m_we.GetCheck() && !m_th.GetCheck() &&
		!m_fr.GetCheck() && !m_sa.GetCheck() && !m_su.GetCheck())
			ERR_MSG_RET0("E_NOTASWDD");

	if (!m_service_mailbox.GetCheck() && !m_service_emsi.GetCheck()	&& !m_service_fax.GetCheck())
		ERR_MSG_RET0("E_NOTACSV");

	if (!m_firstmodem.GetCheck() && !m_secondmodem.GetCheck() && !m_isdnline.GetCheck())
		ERR_MSG_RET0("E_NOTASMAD");

	m_edit_from.GetWindowText(str);
	if (sscanf(str,"%d:%d",&from,&to)!=2)	ERR_MSG_RET0("E_WFIFF");
	m_edit_to.GetWindowText(str);
	if (sscanf(str,"%d:%d",&from,&to)!=2)	ERR_MSG_RET0("E_WFITF");

	return 1;
}

// =========================================================================
	void cfg_inco::parse_string(CString &line)
// =========================================================================
{
CString str;

	get_token(line,IDINCCm_edit_to,str);
	m_edit_to.SetWindowText(str);
	get_token(line,IDINCCm_edit_from,str);
	m_edit_from.SetWindowText(str);

	SetCheckbox(line,IDINCCm_firstmodem,m_firstmodem);
	SetCheckbox(line,IDINCCm_secondmodem,m_secondmodem);
	SetCheckbox(line,IDINCCm_isdnline,m_isdnline);
	SetCheckbox(line,IDINCCm_service_mailbox,m_service_mailbox);
	SetCheckbox(line,IDINCCm_service_emsi,m_service_emsi);
	SetCheckbox(line,IDINCCm_service_fax,m_service_fax);
	SetCheckbox(line,IDINCCm_mo,m_mo);
	SetCheckbox(line,IDINCCm_tu,m_tu);
	SetCheckbox(line,IDINCCm_we,m_we);
	SetCheckbox(line,IDINCCm_th,m_th);
	SetCheckbox(line,IDINCCm_fr,m_fr);
	SetCheckbox(line,IDINCCm_sa,m_sa);
	SetCheckbox(line,IDINCCm_su,m_su);
}

// =========================================================================
	void cfg_inco::OnSelchangeList1()
// =========================================================================
{
CString str;
int		sel;

	GET_SELID(IDC_LIST1);
	m_list.GetText(sel,str);
	parse_string(str);
}

// =========================================================================
	void SetCheckbox(CString &line,int id,CButton &ctl)
// =========================================================================
{
CString str;

	get_token(line,id,str);
	ctl.SetCheck(str=="1");
}

// =========================================================================
	void cfg_inco::OnHelp()
// =========================================================================
{
	WinHelp(VHELP_CFG_INCOME);
}

BOOL cfg_inco::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
