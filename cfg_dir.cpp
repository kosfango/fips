// cfg_dir.cpp : implementation file
// IDD_CFG_DIRS

#include "stdafx.h"
#include "cfg_dir.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern _gconfig  gc;
static char DlgName[]="IDD_CFG_DIRS";

CString basepath;
void	remove_backslash(CString &text);

/////////////////////////////////////////////////////////////////////////////
// cfg_dir dialog

cfg_dir::cfg_dir(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(cfg_dir::IDD, pParent)
{
	//{{AFX_DATA_INIT(cfg_dir)
	m_basedir = _T("");
	m_inbound = _T("");
	m_messagebase = _T("");
	m_nodelist = _T("");
	m_outbound = _T("");
	m_utilities = _T("");
	m_multimedia = _T("");
	m_ticbasic = _T("");
	m_prtprog = _T("");
	m_useansi = FALSE;
	m_useroot = FALSE;
	m_browser = _T("");
	m_hidden = FALSE;
	//}}AFX_DATA_INIT
}


void cfg_dir::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_dir)
	DDX_Text(pDX, IDC_BASEDIR, m_basedir);
	DDX_Text(pDX, IDC_INBOUND, m_inbound);
	DDX_Text(pDX, IDC_MESSAGEBASE, m_messagebase);
	DDX_Text(pDX, IDC_NODELIST, m_nodelist);
	DDX_Text(pDX, IDC_OUTBOUND, m_outbound);
	DDX_Text(pDX, IDC_UTILITIES, m_utilities);
	DDX_Text(pDX, IDC_MULTIMEDIA, m_multimedia);
	DDX_Text(pDX, IDC_TICBASIC, m_ticbasic);
	DDX_Text(pDX, IDC_PRTPROG, m_prtprog);
	DDX_Check(pDX, IDC_USEANSI, m_useansi);
	DDX_Check(pDX, IDC_USEROOT, m_useroot);
	DDX_Text(pDX, IDC_BROWSER, m_browser);
	DDX_Check(pDX, IDC_HIDDEN, m_hidden);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_dir, CDialog)
	//{{AFX_MSG_MAP(cfg_dir)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_PATH1, OnPath1)
	ON_BN_CLICKED(IDC_PATH2, OnPath2)
	ON_BN_CLICKED(IDC_PATH3, OnPath3)
	ON_BN_CLICKED(IDC_PATH4, OnPath4)
	ON_BN_CLICKED(IDC_PATH5, OnPath5)
	ON_BN_CLICKED(IDC_PATH6, OnPath6)
	ON_BN_CLICKED(IDC_PATH7, OnPath7)
	ON_BN_CLICKED(IDC_PATH8, OnPath8)
	ON_EN_CHANGE(IDC_BASEDIR, OnChangeBasedir)
	ON_BN_CLICKED(IDC_PATH9, OnPath9)
	ON_BN_CLICKED(IDC_PATH10, OnPath10)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cfg_dir message handlers

// =====================================================================
	LPCSTR cfg_dir::GetName(void)	{return DlgName;}
// =====================================================================

// =====================================================================
	BOOL cfg_dir::OnInitDialog()
// =====================================================================
{
char path[MAX_PATH];
int  lng[]={
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC7,
			IDC_STATIC1,
			IDC_STATIC8,
			IDC_STATIC9,
			IDC_USEROOT,
			IDC_STATIC10,
			IDC_STATIC11,
			IDC_STATIC12,
			IDC_STATIC13,
			IDC_HIDDEN,
			IDC_USEANSI,
			};
	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	get_exepath(path);
	m_basedir=get_cfg(CFG_COMMON,"BasePath",path);
	m_messagebase=get_cfg(CFG_COMMON,"MsgBasePath",path);
	m_outbound=get_cfg(CFG_COMMON,"OutboundPath",path);
	m_inbound=get_cfg(CFG_COMMON,"InboundPath",path);
	m_nodelist=get_cfg(CFG_COMMON,"NodelistPath",path);
	m_multimedia=get_cfg(CFG_COMMON,"MultimedPath",path);
	m_utilities=get_cfg(CFG_COMMON,"UtilPath",path);
	m_ticbasic=get_cfg(CFG_COMMON,"TicbasicPath",path);
	m_browser=get_cfg(CFG_COMMON,"BrowserCmd","start iexplore.exe %s");
	m_prtprog=get_cfg(CFG_PRINT,"PrintCommand","start notepad.exe /p %s");
	m_hidden=get_cfg(CFG_PRINT,"BackgroundPrint",1);
	m_useansi=get_cfg(CFG_PRINT,"AlwaysAnsi",1);
	m_useroot=TRUE;
	UpdateData(0);
	return TRUE;
}

// =====================================================================
	void cfg_dir::OnOK()
// =====================================================================
{
	UpdateData(1);
	if (m_browser.GetLength()<2)	ERR_MSG_RET("E_YEBSIE");
	if (!strstr(m_browser,"%s"))	ERR_MSG_RET("W_NOPOSP");
	
	remove_backslash(m_basedir);
	remove_backslash(m_messagebase);
	remove_backslash(m_outbound);
	remove_backslash(m_inbound);
	remove_backslash(m_nodelist);
	remove_backslash(m_utilities);
	remove_backslash(m_multimedia);
	remove_backslash(m_ticbasic);

	create_path(m_basedir);
	create_path(m_messagebase);
	create_path(m_outbound);
	create_path(m_inbound);
	create_path(m_nodelist);
	create_path(m_utilities);
	create_path(m_multimedia);
	create_path(m_ticbasic);
// refresh global structs
	strncpy(gc.BasePath,m_basedir,MAX_PATH-1);
	gc.BasePath[MAX_PATH-1]=0;
	strncpy(gc.MsgBasePath,m_messagebase,MAX_PATH-1);
	gc.MsgBasePath[MAX_PATH-1]=0;
	strncpy(gc.OutboundPath,m_outbound,MAX_PATH-1);
	gc.OutboundPath[MAX_PATH-1]=0;
	strncpy(gc.InboundPath,m_inbound,MAX_PATH-1);
	gc.InboundPath[MAX_PATH-1]=0;
	strncpy(gc.MultimedPath,m_multimedia,MAX_PATH-1);
	gc.MultimedPath[MAX_PATH-1]=0;
	strncpy(gc.NodelistPath,m_nodelist,MAX_PATH-1);
	gc.NodelistPath[MAX_PATH-1]=0;
	strncpy(gc.UtilPath,m_utilities,MAX_PATH-1);
	gc.UtilPath[MAX_PATH-1]=0;
	if (m_prtprog.GetLength()==0)	m_prtprog="start notepad.exe /p %s";
// save configs
	set_cfg(CFG_COMMON,"UtilPath",gc.UtilPath);
	set_cfg(CFG_COMMON,"OutboundPath",gc.OutboundPath);
	set_cfg(CFG_COMMON,"NodelistPath",gc.NodelistPath);
	set_cfg(CFG_COMMON,"MsgBasePath",gc.MsgBasePath);
	set_cfg(CFG_COMMON,"InboundPath",gc.InboundPath);
	set_cfg(CFG_COMMON,"BasePath",gc.BasePath);
	set_cfg(CFG_COMMON,"MultimedPath",gc.MultimedPath);
	set_cfg(CFG_COMMON,"TicbasicPath",m_ticbasic);
	set_cfg(CFG_COMMON,"BrowserCmd",m_browser);
	set_cfg(CFG_PRINT,"PrintCommand",m_prtprog);
	set_cfg(CFG_PRINT,"BackgroundPrint",m_hidden);
	set_cfg(CFG_PRINT,"AlwaysAnsi",m_useansi);
	
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =================================================
	void build_path(CString &path,CString &base,BOOL root,LPCSTR deflt)
// =================================================
{
int	i;

	if (!path.GetLength())
		path=base+deflt;
	else if (root)
	{
		i=path.ReverseFind('\\');
		path=base+path.Mid(i<0 ? 0 : i);
	}
	path.MakeUpper();
}

// =================================================
	void cfg_dir::OnChangeBasedir() 
// =================================================
{
	UpdateData(1);
	build_path(m_utilities,m_basedir,m_useroot,"\\util");
	build_path(m_outbound,m_basedir,m_useroot,"\\outbound");
	build_path(m_nodelist,m_basedir,m_useroot,"\\nodelist");
	build_path(m_messagebase,m_basedir,m_useroot,"\\msgbase");
	build_path(m_inbound,m_basedir,m_useroot,"\\inbound");
	build_path(m_multimedia,m_basedir,m_useroot,"\\multimed");
	build_path(m_ticbasic,m_basedir,m_useroot,"\\ticbasic");
	UpdateData(0);
}

// =================================================
	void cfg_dir::OnPath1() 
// =================================================
{
	if (GetDirectory(m_basedir,m_hWnd))
		GetDlgItem(IDC_BASEDIR)->SetWindowText(m_basedir);

	OnChangeBasedir();
}

// =================================================
	void cfg_dir::OnPath2() 
// =================================================
{
	if (GetDirectory(m_messagebase,m_hWnd))
		UpdateData(0);
}

// =================================================
	void cfg_dir::OnPath3() 
// =================================================
{
	if (GetDirectory(m_utilities,m_hWnd))
		UpdateData(0);
}

// =================================================
	void cfg_dir::OnPath4() 
// =================================================
{
	if (GetDirectory(m_nodelist,m_hWnd))
		UpdateData(0);
}
// =================================================
	void cfg_dir::OnPath5() 
// =================================================
{
	if (GetDirectory(m_inbound,m_hWnd))
		UpdateData(0);
}
// =================================================
	void cfg_dir::OnPath6() 
// =================================================
{
	if (GetDirectory(m_outbound,m_hWnd))
		UpdateData(0);
}
// =================================================
	void cfg_dir::OnPath7() 
// =================================================
{
	if (GetDirectory(m_multimedia,m_hWnd))
		UpdateData(0);
}
// =================================================
	void cfg_dir::OnPath8() 
// =================================================
{
	if (GetDirectory(m_ticbasic,m_hWnd))
		UpdateData(0);
}

// =================================================
	void cfg_dir::OnHelp()
// =================================================
{
	WinHelp(VHELP_CFG_STATION);
}

// =================================================
	BOOL cfg_dir::OnHelpInfo(HELPINFO* pHelpInfo) 
// =================================================
{
	OnHelp();
	return TRUE;
}

// =================================================
	void remove_backslash(CString &text)
// =================================================
{
	if (text.IsEmpty())
	    return;

	if (text.Right(1)=="\\")
		text=text.Left(text.GetLength()-1);
}

// =================================
	void create_path(LPCSTR path)
// =================================
{
LPCSTR	p;
LPCSTR	plast;
char	buf[MAX_PATH];

	 p=plast=path+3;
	 while (p)
	 {
		 p=strchr(plast,'\\');
		 if (p)
		 {
		     plast=p+1;
			 strncpy(buf,path,(p-path));
			 buf[(p-path)]=0;
			 CreateDirectory(buf,NULL);
		 }
	 }
	CreateDirectory(path,NULL);
}

void cfg_dir::OnPath9() 
{
CString	str;
int		ret;
	
	str.LoadString(IDS_EXECFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_browser=dlg.GetPathName()+" %s";
	UpdateData(0);
}

void cfg_dir::OnPath10() 
{
CString	str;
int		ret;
	
	str.LoadString(IDS_EXECFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_prtprog=dlg.GetPathName();
	UpdateData(0);
}
