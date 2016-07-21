// cfg_serv.cpp : implementation file
// IDD_CFG_SERVICES

#include "stdafx.h"
#include "cfg_serv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static char DlgName[]="IDD_CFG_SERVICES";

/////////////////////////////////////////////////////////////////////////////
// cfg_serv dialog

cfg_serv::cfg_serv(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(cfg_serv::IDD, pParent)
{
	//{{AFX_DATA_INIT(cfg_serv)
	m_autorescan = FALSE;
	m_autostart = FALSE;
	m_delseenby = FALSE;
	m_ignorerr = FALSE;
	m_noinfomails = FALSE;
	m_nonewarea = FALSE;
	m_notasaved = FALSE;
	m_packoutbound = FALSE;
	m_prochidden = FALSE;
	m_purgescanned = FALSE;
	m_startupevents = FALSE;
	m_akaroute = FALSE;
	m_noproctic = FALSE;
	m_delimp = FALSE;
	//}}AFX_DATA_INIT
}

void cfg_serv::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_serv)
	DDX_Check(pDX, IDC_AUTORESCAN, m_autorescan);
	DDX_Check(pDX, IDC_AUTOSTART_TOSSER, m_autostart);
	DDX_Check(pDX, IDC_DELETE_SEENBY, m_delseenby);
	DDX_Check(pDX, IDC_IGNORERR, m_ignorerr);
	DDX_Check(pDX, IDC_NO_INFOMAILS, m_noinfomails);
	DDX_Check(pDX, IDC_NO_NEWAREAINFO, m_nonewarea);
	DDX_Check(pDX, IDC_NOT_PROC_ASAVED, m_notasaved);
	DDX_Check(pDX, IDC_PACKOUTBOUND, m_packoutbound);
	DDX_Check(pDX, IDC_PROC_HIDDEN, m_prochidden);
	DDX_Check(pDX, IDC_PURGE_SCANNED, m_purgescanned);
	DDX_Check(pDX, IDC_STARTUP_EVENTS, m_startupevents);
	DDX_Check(pDX, IDC_AKAROUTE, m_akaroute);
	DDX_Check(pDX, IDC_NOPROC_TIC, m_noproctic);
	DDX_Check(pDX, IDC_DELIMP, m_delimp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_serv, CDialog)
	//{{AFX_MSG_MAP(cfg_serv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cfg_serv message handlers
// =====================================================================
LPCSTR cfg_serv::GetName(void)	{return DlgName;}
// =====================================================================

// ==============================================
BOOL cfg_serv::OnInitDialog()
// ==============================================
{
int lng[]={
		IDC_STARTUP_EVENTS,
		IDC_AUTORESCAN,
		IDC_PACKOUTBOUND,
		IDC_PURGE_SCANNED,
		IDC_NO_INFOMAILS,
		IDC_NO_NEWAREAINFO,
		IDC_AKAROUTE,
		IDC_AUTOSTART_TOSSER,
		IDC_IGNORERR,
		IDC_PROC_HIDDEN,
		IDC_DELIMP,
		IDC_NOPROC_TIC,
		IDC_TZ_NOT,
		IDC_TZ_TOSS,
		IDC_TZ_DISP,
		IDC_NOT_PROC_ASAVED,
		IDC_DELETE_SEENBY,
		IDC_MAILER_GRP,
		IDC_TOSSER_GRP,
		IDC_PURGER_GRP
	};
	
	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	
	m_autostart=get_cfg(CFG_COMMON,"TosserStart",1);
	m_autorescan=get_cfg(CFG_COMMON,"RescanStart",1);
	m_startupevents=get_cfg(CFG_COMMON,"EnableEvents",1);
	m_noinfomails=get_cfg(CFG_COMMON,"DontGenInfo",0);
	m_purgescanned=get_cfg(CFG_COMMON,"PurgeScanned",0);
	m_packoutbound=get_cfg(CFG_COMMON,"PackOutbound",1);
	m_nonewarea=get_cfg(CFG_COMMON,"NoInfoNewArea",0);
	m_akaroute=get_cfg(CFG_COMMON,"AKARoute",0);
	m_notasaved=get_cfg(CFG_PURGER,"NotProcAsaved",0);
	m_delseenby=get_cfg(CFG_PURGER,"DeleteSeenBy",0);
	m_ignorerr=get_cfg(CFG_TOSSER,"IgnoreErrors",0);
	m_prochidden=get_cfg(CFG_TOSSER,"TossHidden",0);
	m_noproctic=get_cfg(CFG_TOSSER,"NoProcTics",0);
	m_delimp=get_cfg(CFG_TOSSER,"DeleteImps",0);
	CheckRadioButton(IDC_TZ_NOT,IDC_TZ_DISP,IDC_TZ_NOT+get_cfg(CFG_TOSSER,"ProcTimezone",0));
	UpdateData(0);
	return TRUE;
}

void cfg_serv::OnOK() 
{
	UpdateData(1);
	set_cfg(CFG_COMMON,"TosserStart",m_autostart);
	set_cfg(CFG_COMMON,"RescanStart",m_autorescan);
	set_cfg(CFG_COMMON,"EnableEvents",m_startupevents);
	set_cfg(CFG_COMMON,"DontGenInfo",m_noinfomails);
	set_cfg(CFG_COMMON,"PurgeScanned",m_purgescanned);
	set_cfg(CFG_COMMON,"PackOutbound",m_packoutbound);
	set_cfg(CFG_COMMON,"NoInfoNewArea",m_nonewarea);
	set_cfg(CFG_COMMON,"AKARoute",m_akaroute);
	set_cfg(CFG_PURGER,"NotProcAsaved",m_notasaved);
	set_cfg(CFG_PURGER,"DeleteSeenBy",m_delseenby);
	set_cfg(CFG_TOSSER,"IgnoreErrors",m_ignorerr);
	set_cfg(CFG_TOSSER,"TossHidden",m_prochidden);
	set_cfg(CFG_TOSSER,"NoProcTics",m_noproctic);
	set_cfg(CFG_TOSSER,"DeleteImps",m_delimp);
	set_cfg(CFG_TOSSER,"ProcTimezone",GetCheckedRadioButton(IDC_TZ_NOT,IDC_TZ_DISP)-IDC_TZ_NOT);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}
