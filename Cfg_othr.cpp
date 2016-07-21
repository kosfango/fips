// cfg_othr.cpp : implementation file
// IDD_CFG_OTHERS

#include "stdafx.h"
#include "resource.h"
#include "cfg_othr.h"
#include "lightdlg.h"
#include "structs.h"
#include "detmail.h"
#include "floating.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern LangID	 LANG_ID;
extern CLightDlg *gpMain;
extern detmail   gMailer;
extern floating  gFloating;
extern _gconfig  gc;

static char DlgName[]="IDD_CFG_OTHERS";

// ==============================================
	cfg_oth::cfg_oth(CWnd* pParent ) 	: CSAPrefsSubDlg(cfg_oth::IDD, pParent)
// ==============================================
, m_mintotray(FALSE)
	{
	//{{AFX_DATA_INIT(cfg_oth)
	m_convert_latin = FALSE;
	m_hidecmd = FALSE;
	m_no_reminder = FALSE;
	m_notooltips = FALSE;
//	m_blink_scroll = FALSE;
	//}}AFX_DATA_INIT
}

// ==============================================
void cfg_oth::DoDataExchange(CDataExchange* pDX)
// ==============================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_oth)
	DDX_Check(pDX, IDC_CONVERT_LATIN, m_convert_latin);
	DDX_Check(pDX, IDC_HIDE_CMD_WINDOW, m_hidecmd);
	DDX_Check(pDX, IDC_NO_NL_REMINDER, m_no_reminder);
	DDX_Check(pDX, IDC_NO_TOOLTIPS, m_notooltips);
	//	DDX_Check(pDX, IDC_ROLLEN_LIGHT, m_blink_scroll);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_MINTOTRAY, m_mintotray);
}

BEGIN_MESSAGE_MAP(cfg_oth, CDialog)
	//{{AFX_MSG_MAP(cfg_oth)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_oth::GetName(void)	{return DlgName;}
// =====================================================================

// ==============================================
	BOOL cfg_oth::OnInitDialog()
// ==============================================
{
int lng[]={
			IDC_NO_TOOLTIPS,
			IDC_ROLLEN_LIGHT,
			IDC_CONVERT_LATIN,
			IDC_NO_NL_REMINDER,
			IDC_HIDE_CMD_WINDOW,
			IDC_MINTOTRAY
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	m_notooltips=get_cfg(CFG_COMMON,"NoTooltips",0);
	m_blink_scroll=get_cfg(CFG_COMMON,"NoBlinkScrollLock",0);
	m_no_reminder=get_cfg(CFG_COMMON,"NoNLReminder",0);
	m_hidecmd=get_cfg(CFG_COMMON,"HideCmdWin",0);
	m_mintotray=get_cfg(CFG_COMMON,"MinToTray",0);
	if (LANG_ID==LANG_GER)	// only for Germans
	{
		m_convert_latin=get_cfg(CFG_COMMON,"ConvertLatin",0);
		GetDlgItem(IDC_CONVERT_LATIN)->ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		m_convert_latin=0;
		GetDlgItem(IDC_CONVERT_LATIN)->ShowWindow(SW_HIDE);
	}
	UpdateData(0);
	return TRUE;
}

// ==============================================
	void cfg_oth::OnOK()
// ==============================================
{
	UpdateData(1);
	gc.NoBlinkScrollLock=m_blink_scroll;
	set_cfg(CFG_COMMON,"NoBlinkScrollLock",gc.NoBlinkScrollLock);
	gc.NoTooltips=m_notooltips;
	set_cfg(CFG_COMMON,"NoTooltips",gc.NoTooltips);
	set_cfg(CFG_COMMON,"ConvertLatin",m_convert_latin);
	set_cfg(CFG_COMMON,"NoNLReminder",m_no_reminder);
	set_cfg(CFG_COMMON,"HideCmdWin",m_hidecmd);
	gc.MinToTray=m_mintotray;
	set_cfg(CFG_COMMON,"MinToTray",gc.MinToTray);

	gpMain->EnableToolTips();
	gpMain->tip.Activate(!gc.NoTooltips);
	gFloating.EnableToolTips(!gc.NoTooltips);
	gFloating.tip.Activate(!gc.NoTooltips);
	gMailer.tip.Activate(!gc.NoTooltips);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ==============================================
	void cfg_oth::OnHelp()
// ==============================================
{
	WinHelp(VHELP_CFG_OTHERS);
}

// ==============================================
	BOOL cfg_oth::OnHelpInfo(HELPINFO* pHelpInfo) 
// ==============================================
{
	OnHelp();
	return TRUE;
}
