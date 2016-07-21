// logopti.cpp : implementation file
// IDD_CFG_LOGFILES

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "logopti.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CFG_LOGFILES";

void get_sizes(CEdit &edit1,CEdit &edit2,int ind);
BOOL check_sizes(CEdit &edit1,CEdit &edit2);
void set_sizes(CEdit &edit1,CEdit &edit2,int ind);

// ========================================================================
	logopti::logopti(CWnd* pParent ) : CSAPrefsSubDlg(logopti::IDD, pParent)
// ========================================================================
{
	//{{AFX_DATA_INIT(logopti)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ========================================================================
	void logopti::DoDataExchange(CDataExchange* pDX)
// ========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(logopti)
	DDX_Control(pDX, IDC_NODELIST_SHORT_TO, m_nodelist_shortto);
	DDX_Control(pDX, IDC_NODELIST_MAXSIZE, m_nodelist_maxsize);
	DDX_Control(pDX, IDC_EVENTS_SHORT_TO, m_events_shortto);
	DDX_Control(pDX, IDC_TOSSER_SHORT_TO, m_tosser_shortto);
	DDX_Control(pDX, IDC_PURGER_SHORT_TO, m_purger_shortto);
	DDX_Control(pDX, IDC_MAILER_SHORT_TO, m_mailer_shortto);
	DDX_Control(pDX, IDC_TOSSER_MAXSIZE, m_tosser_maxsize);
	DDX_Control(pDX, IDC_EVENTS_MAXSIZE, m_events_maxsize);
	DDX_Control(pDX, IDC_PURGER_MAXSIZE, m_purger_maxsize);
	DDX_Control(pDX, IDC_MAILER_MAXSIZE, m_mailer_maxsize);
	DDX_Control(pDX, IDC_LEVENTS, m_events);
	DDX_Control(pDX, IDC_LTOSSER, m_tosser);
	DDX_Control(pDX, IDC_LPURGER, m_purger);
	DDX_Control(pDX, IDC_LMAILER, m_mailer);
	DDX_Control(pDX, IDC_LNODELIST, m_nodelist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(logopti, CDialog)
	//{{AFX_MSG_MAP(logopti)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR logopti::GetName(void)	{return DlgName;}
// =====================================================================

// ========================================================================
	BOOL logopti::OnInitDialog()
// ========================================================================
{
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_LMAILER,
			IDC_LTOSSER,
			IDC_LPURGER,
			IDC_LEVENTS,
			IDC_LNODELIST,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_mailer.SetCheck(STOB(get_cfg(CFG_LOGS,"MailerLog","1")));
	m_tosser.SetCheck(STOB(get_cfg(CFG_LOGS,"TosserLog","1")));
	m_purger.SetCheck(STOB(get_cfg(CFG_LOGS,"PurgerLog","1")));
	m_events.SetCheck(STOB(get_cfg(CFG_LOGS,"EventsLog","1")));
	m_nodelist.SetCheck(STOB(get_cfg(CFG_LOGS,"NLCompLog","1")));
	get_sizes(m_mailer_maxsize,m_mailer_shortto,0);
	get_sizes(m_tosser_maxsize,m_tosser_shortto,1);
	get_sizes(m_purger_maxsize,m_purger_shortto,2);
	get_sizes(m_events_maxsize,m_events_shortto,3);
	get_sizes(m_nodelist_maxsize,m_nodelist_shortto,4);
	return TRUE;
}

// ========================================================================
	void logopti::OnOK()
// ========================================================================
{
	if (!check_sizes(m_mailer_maxsize,m_mailer_shortto) ||
		!check_sizes(m_tosser_maxsize,m_tosser_shortto) ||
		!check_sizes(m_purger_maxsize,m_purger_shortto) ||
		!check_sizes(m_events_maxsize,m_events_shortto) ||
		!check_sizes(m_nodelist_maxsize,m_nodelist_shortto))
			return;
	set_cfg(CFG_LOGS,"MailerLog",BTOS(m_mailer.GetCheck()));
	set_cfg(CFG_LOGS,"TosserLog",BTOS(m_tosser.GetCheck()));
	set_cfg(CFG_LOGS,"PurgerLog",BTOS(m_purger.GetCheck()));
	set_cfg(CFG_LOGS,"EventsLog",BTOS(m_events.GetCheck()));
	set_cfg(CFG_LOGS,"NLCompLog",BTOS(m_nodelist.GetCheck()));

	set_sizes(m_mailer_maxsize,m_mailer_shortto,0);
	set_sizes(m_tosser_maxsize,m_tosser_shortto,1);
	set_sizes(m_purger_maxsize,m_purger_shortto,2);
	set_sizes(m_events_maxsize,m_events_shortto,3);
	set_sizes(m_nodelist_maxsize,m_nodelist_shortto,4);

	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ========================================================================
	void get_sizes(CEdit &edit1,CEdit &edit2,int ind)
// ========================================================================
{
char str[20];
int	 mx,sh;
BOOL n;

	get_logoptions(ind,n,mx,sh);
	itoa(mx,str,10);
	edit1.SetWindowText(str);
	itoa(sh,str,10);
	edit2.SetWindowText(str);
}

// ========================================================================
	BOOL check_sizes(CEdit &edit1,CEdit &edit2)
// ========================================================================
{
CString	str1,str2;

	edit1.GetWindowText(str1);
	edit2.GetWindowText(str2);
	if (atoi(str2) >= atoi(str1))
		ERR_MSG_RET0("W_TSMBGTS");
	return 1;
}

// ========================================================================
	void set_sizes(CEdit &edit1,CEdit &edit2,int id)
// ========================================================================
{
CString	str,str1,str2;

	edit1.GetWindowText(str1);
	edit2.GetWindowText(str2);
	str=str1+","+str2;
	switch (id)
	{
		case LOG_MAILER:
			set_cfg(CFG_LOGS,"MailerLim",str);
			break;
		case LOG_TOSSER:
			set_cfg(CFG_LOGS,"TosserLim",str);
			break;
		case LOG_PURGER:
			set_cfg(CFG_LOGS,"PurgerLim",str);
			break;
		case LOG_EVENTS:
			set_cfg(CFG_LOGS,"EventsLim",str);
			break;
		case LOG_NL:
			set_cfg(CFG_LOGS,"NLCompLim",str);
			break;
	}
}

// ========================================================================
	void logopti::OnHelp()
// ========================================================================
{
	WinHelp(VHELP_CFG_LOGFILES);
}
	
BOOL logopti::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
