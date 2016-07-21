// bbsmain.cpp : implementation file
// IDD_BBS_MAIN

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "bbsmain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CStrList bbs_main;
static char DlgName[]="IDD_BBS_MAIN";

extern int bbs_language_list(CStrList &);

// =====================================================================
	bbsmain::bbsmain(CWnd* pParent ) : CSAPrefsSubDlg(bbsmain::IDD, pParent)
// =====================================================================
{
	//{{AFX_DATA_INIT(bbsmain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =====================================================================
	void bbsmain::DoDataExchange(CDataExchange* pDX)
// =====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(bbsmain)
	DDX_Control(pDX, IDC_CHECK_MINI, m_check_mini);
	DDX_Control(pDX, IDC_EDIT_SOUND, m_edit_sound);
	DDX_Control(pDX, IDC_DD_LANGUAGE, m_dd_language);
	DDX_Control(pDX, IDC_DD_DEFGROUP, m_dd_defgroup);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(bbsmain, CDialog)
	//{{AFX_MSG_MAP(bbsmain)
	ON_BN_CLICKED(IDC_SOUND_CHOICE, OnSoundChoice)
	ON_BN_CLICKED(IDC_TEST_SOUND, OnTestSound)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// bbsmain message handlers

// =====================================================================
LPCSTR bbsmain::GetName(void)	{return DlgName;}
// =====================================================================

// =====================================================================
	BOOL bbsmain::OnInitDialog()
// =====================================================================
{
int  lng[]={
			IDC_TEST_SOUND,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_SOUND_CHOICE,
			};
    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	write_bbs_setup();
	bbs_main.LoadFromFile("bbsmain.cfg");
	m_edit_sound.SetWindowText(bbs_main.GetString(3));	// sound
	m_check_mini.SetCheck(bbs_main.defaultindex);
	FillLanguageDropDown();
	FillLevelDropDown();
	SelectLanguage(bbs_main.GetString(2));	// language
	SelectLevel(bbs_main.GetString(4));		// level
	return TRUE;
}

// =====================================================================
	void bbsmain::FillLanguageDropDown(void)
// =====================================================================
{
CStrList	lst;

	bbs_language_list(lst);
	m_dd_language.ResetContent();
	for (int i=0;i<lst.GetCount();i++)
		m_dd_language.AddString(lst.GetString(i));
}

// =====================================================================
	void bbsmain::SelectLanguage(LPCSTR str)
// =====================================================================
{
	m_dd_language.SetCurSel(0);
	m_dd_language.SelectString(-1,str);
}

// =====================================================================
	void bbsmain::FillLevelDropDown(void)
// =====================================================================
{
CStrList	lst;
CString		str;

	lst.LoadFromFile("bbsgrps.cfg");
	m_dd_defgroup.ResetContent();
	for (int i=0;i<lst.GetCount();i++)
	{
		get_token(lst.GetString(i),0,str);
		m_dd_defgroup.AddString(str);
	}
}

// =====================================================================
	void bbsmain::SelectLevel(LPCSTR str)
// =====================================================================
{
	m_dd_defgroup.SetCurSel(0);
	m_dd_defgroup.SelectString(-1,str);
}

// =====================================================================
	void bbsmain::OnSoundChoice()
// =====================================================================
{
CString str;
int		ret;

	str.LoadString(IDS_SOUNDFILT);
	CFileDialog	dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_edit_sound.SetWindowText(dlg.GetPathName());
}

// =====================================================================
	void bbsmain::OnTestSound()
// =====================================================================
{
CString str;

	m_edit_sound.GetWindowText(str);
	test_sound(str);
}

// =====================================================================
	void bbsmain::OnOK()
// =====================================================================
{
CStrList	lst;
CString		str;
int			ret;

	lst.AddTail("");
	lst.AddTail("");
	lst.defaultindex=m_check_mini.GetCheck();
	ret=m_dd_language.GetCurSel();
	if (ret==LB_ERR)
	{
		MessageBeep(0);
		return;
	}
	m_dd_language.GetLBText(ret,str);
	lst.AddTail(str);

	m_edit_sound.GetWindowText(str);
	lst.AddTail(str);

	ret=m_dd_defgroup.GetCurSel();
	if (ret==LB_ERR)
	{
		MessageBeep(0);
		return;
	}
	m_dd_defgroup.GetLBText(ret,str);
	lst.AddTail(str);
	lst.SaveToFile("bbsmain.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =====================================================================
	void bbsmain::OnHelp()
// =====================================================================
{
	WinHelp(VHELP_BBS_MAIN);
}

BOOL bbsmain::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
