// dettoss.cpp : implementation file
// IDD_STAT_TOSS

#include "stdafx.h"
#include "light.h"
#include "lightdlg.h"
#include "dettoss.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CLightApp	FipsApp;
extern CLightDlg 	*gpMain;
extern _gconfig gc;

static _DlgItemsSize DlgItemsSize [] =
{
	0,					 {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST		  ,	 {0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDC_STATUS		  ,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_EDIT_ERRORS	  ,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_FR			  ,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_AREA_PROZ	  ,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDHELP			  ,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCLEAR			  ,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDSTART			  ,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL		  ,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
};
static char DlgName[]="IDD_STAT_TOSS";

// ==============================================================
	dettoss::dettoss(CWnd* pParent /*=NULL*/)
		: CDialog(dettoss::IDD, pParent)
// ==============================================================
{
	//{{AFX_DATA_INIT(dettoss)
	//}}AFX_DATA_INIT
}

// ==============================================================
	void dettoss::DoDataExchange(CDataExchange* pDX)
// ==============================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(dettoss)
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_AREA_PROZ, m_proz);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(dettoss, CDialog)
	//{{AFX_MSG_MAP(dettoss)
	ON_BN_CLICKED(IDCLEAR, OnClear)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDSTART, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// dettoss message handlers

// ==============================================================
	BOOL dettoss::OnInitDialog()
// ==============================================================
{
	CDialog::OnInitDialog();
	EXTENT_LB(IDC_LIST,1000);
	return TRUE;
}

// ===========================================
	void dettoss::SetLanguage	(void)
// ===========================================
{
	int  lng[]={
				IDSTART,
				IDCANCEL,
				IDCLEAR,
				IDHELP,
				};
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
}

// ==============================================================
	void dettoss::OnCancel()
// ==============================================================
{
  ShowWindow(SW_HIDE);
	CDialog::OnCancel();
}

// ==============================================================
	void dettoss::OnClear()
// ==============================================================
{
	m_list.ResetContent();
	m_status.SetWindowText(NULL);
    m_proz.SetPos(0);
}

// ==============================================================
	void dettoss::OnStart()
// ==============================================================
{
	if (!gc.tosser.running)	FipsApp.OnStarttosser();
}

// ==============================================================
	void dettoss::AddString(char *text)
// ==============================================================
{
char buf[200],buf2[200],*p;
int	 cnt;

	m_list.SetRedraw(0);
	p=strchr(text,'\n');
	if (p)	*p=0;
	_strtime(buf2);
	sprintf(buf,"%s  %s",buf2,text);
	m_list.AddString(buf);
	cnt=m_list.GetCount()-2;
	m_list.SetTopIndex(cnt<0 ? 0 : cnt);
	m_list.SetRedraw(1);
}

// ==============================================================
	void dettoss::UpdateValues(void)
// ==============================================================
{
char buf[300],siz[50];

	sprintf(buf,L("S_597",
		gc.tosser.m_packets,gc.tosser.m_mails,fmt_size(siz,gc.tosser.m_bytes),
		gc.tosser.m_netmails,gc.tosser.m_echomails,
		gc.tosser.m_carbons,gc.tosser.m_twits));	 
	m_status.SetWindowText(buf);
}

// ===============================================
	BOOL dettoss::PreTranslateMessage(MSG *pMsg)
// ===============================================
{
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam=='2' && 
		(GetKeyState(VK_CONTROL) & 0x80000000))
			OnCancel();
	if (handle_function_key(pMsg))	return 1;
	return CDialog::PreTranslateMessage(pMsg);
}

// ===============================================
	void dettoss::OnClose()
// ===============================================
{
	OnCancel();
}

// ============================================
	void dettoss::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void dettoss::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ===============================================
	void dettoss::SavePosition(void)
// ===============================================
{
  StoreInitDialogSize(DlgName,this);
}

// ===========================================
	void dettoss::OnShowWindow(BOOL bShow, UINT nStatus) 
// ===========================================
{
	CDialog::OnShowWindow(bShow, nStatus);
	(gpMain->GetMenu())->CheckMenuItem(ID_SWITCH_TOSSER, 
		MF_BYCOMMAND | (bShow ? MF_CHECKED : MF_UNCHECKED));
	if (!bShow)	gpMain->m_subjlist.SetFocus();
}

// ===========================================
	void dettoss::OnHelp()
// ===========================================
{
	WinHelp(VHELP_STAT_TOSS);
}

// ===========================================
	BOOL dettoss::OnHelpInfo(HELPINFO* pHelpInfo) 
// ===========================================
{
	OnHelp();
	return TRUE;
}
