// cfg_advn.cpp : implementation file
// IDD_CFG_PHONES

#include "stdafx.h"
#include "resource.h"
#include "cfg_advn.h"
#include "structs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList teltrans;
static char DlgName[]="IDD_CFG_PHONES";

// ==========================================================================
	cfg_advn::cfg_advn(CWnd* pParent ) : CSAPrefsSubDlg(cfg_advn::IDD, pParent)
// ==========================================================================
{
	//{{AFX_DATA_INIT(cfg_advn)
	m_countrypf = _T("");
	m_citypf = _T("");
	m_officepf = _T("");
	m_translate = _T("");
	//}}AFX_DATA_INIT
}

// ==========================================================================
	void cfg_advn::DoDataExchange(CDataExchange* pDX)
// ==========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_advn)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT_REPLACE, m_edit_replace);
	DDX_Control(pDX, IDC_EDIT_NUMBER, m_edit_number);
	DDX_Text(pDX, IDC_COUNTRYPF, m_countrypf);
	DDX_Text(pDX, IDC_CITYPF, m_citypf);
	DDX_Text(pDX, IDC_OFFICEPF, m_officepf);
	DDX_Text(pDX, IDC_TRANSLATE, m_translate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_advn, CDialog)
	//{{AFX_MSG_MAP(cfg_advn)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_advn::GetName(void)	{return DlgName;}
// =====================================================================

// ==========================================================================
	BOOL cfg_advn::OnInitDialog()
// ==========================================================================
{
int  lng[]={
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_ADD,
			IDC_DELETE,
			IDC_CHANGE,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC1,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	m_countrypf=get_cfg(CFG_PREFIX,"CountryPref","");
	m_translate=get_cfg(CFG_PREFIX,"CountryRepl","");
	m_citypf=get_cfg(CFG_PREFIX,"CityPref","");
	m_officepf=get_cfg(CFG_PREFIX,"InternatPref","");
	m_list.SetTabStops(120);
	teltrans.Sort(0);
	UPDATE_LB(teltrans,IDC_LIST);
	UpdateData(0);
	return TRUE;
}

// ==========================================================================
	void cfg_advn::OnAdd()
// ==========================================================================
{
CString phone,replace;

 	m_edit_number.GetWindowText(phone);
 	m_edit_replace.GetWindowText(replace);

	if (phone.IsEmpty() || replace.IsEmpty())	return;
	phone+="\t"+replace;
	m_list.AddString(phone);
}

// ==========================================================================
	void cfg_advn::OnDelete()
// ==========================================================================
{
int sel;

	GET_SELID(IDC_LIST);
	m_list.DeleteString(sel);
}

// ==========================================================================
	void cfg_advn::OnOK()
// ==========================================================================
{
	UpdateData(1);
	set_cfg(CFG_PREFIX,"CountryPref",m_countrypf);
	set_cfg(CFG_PREFIX,"CountryRepl",m_translate);
	set_cfg(CFG_PREFIX,"CityPref",m_citypf);
	set_cfg(CFG_PREFIX,"InternatPref",m_officepf);
	teltrans=m_list;
	teltrans.SaveToFile("teltrans.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}
