// cfgsound.cpp : implementation file
// IDD_CFG_SOUND

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfgsound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList   SoundLst;
static char DlgName[]="IDD_CFG_SOUND";

// ================================================================================
	cfgsound::cfgsound(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(cfgsound::IDD, pParent)
// ================================================================================
{
	//{{AFX_DATA_INIT(cfgsound)
	//}}AFX_DATA_INIT
}

// ================================================================================
	void cfgsound::DoDataExchange(CDataExchange* pDX)
// ================================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfgsound)
	DDX_Control(pDX, IDC_PATH, m_path);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfgsound, CDialog)
	//{{AFX_MSG_MAP(cfgsound)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDDEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDTESTSOUND, OnTestsound)
	ON_BN_CLICKED(IDSTOPSOUND, OnStopsound)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
	LPCSTR cfgsound::GetName(void)	{return DlgName;}
// =====================================================================

// ================================================================================
	BOOL cfgsound::OnInitDialog()
// ================================================================================
{
int tabs[]={100,300};
int lng[]={
			IDC_CHANGE,
			IDDEFAULT,
			IDC_STATIC1,
			IDTESTSOUND,
			IDSTOPSOUND
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TABULATE_LB(IDC_LIST);
	SoundLst.Sort(0);
	UPDATE_LB(SoundLst,IDC_LIST);
	return TRUE;
}

// ================================================================================
	void cfgsound::OnDefault()
// ================================================================================
{
CString str;
CStrList lst;

	str.LoadString(IDS_DEFSOUNDS);
	lst.LoadFromDelimString(str,'\n');
	UPDATE_LB(lst,IDC_LIST);
}

// ================================================================================
	void cfgsound::OnBrowse()
// ================================================================================
{
CString	str;
int		ret;

	str.LoadString(IDS_SOUNDFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_path.SetWindowText(dlg.GetPathName());
}

// ================================================================================
	void cfgsound::OnOK()
// ================================================================================
{
	SoundLst=m_list;
	save_soundlist(SoundLst);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ================================================================================
	void cfgsound::OnChange()
// ================================================================================
{
CString id,str,str1;
int		sel;

	GET_SELID(IDC_LIST);
	m_list.GetText(sel,id);
	get_token(id,0,str);
    m_path.GetWindowText(str1);
	if (str1.IsEmpty())	str1="NONE";
	m_list.DeleteString(sel);
	m_list.InsertString(sel,str+'\t'+str1);
	m_list.SetCurSel(sel);
}

// ================================================================================
	void cfgsound::OnSelchangeList()
// ================================================================================
{
CString id,str;
int		sel;

	GET_SELID(IDC_LIST);
	m_list.GetText(sel,id);
	get_token(id,1,str);
	m_path.SetWindowText(str);
}

// ==================================================
	void cfgsound::OnTestsound()
// ==================================================
{
CString str;

	m_path.GetWindowText(str);
	test_sound(str);
}

// ==================================================
	void cfgsound::OnStopsound()
// ==================================================
{
	stop_test_sound();
}

// ==================================================
	void cfgsound::OnHelp()
// ==================================================
{
	WinHelp(VHELP_CFG_SOUND);
}

BOOL cfgsound::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
