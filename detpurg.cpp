// detpurg.cpp : implementation file
// IDD_STAT_PURGE

#include "stdafx.h"
#include "light.h"
#include "lightdlg.h"
#include "areasel.h"
#include "detpurg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CLightApp	FipsApp;
extern CLightDlg 	*gpMain;
extern _gconfig gc;

static _DlgItemsSize DlgItemsSize [] =
{
	0					,    {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST			,	 {0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDC_STATIC1			,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC2			,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC3			,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC4			,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC5			,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_MAILSREMOVED	,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATUS			,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_MAILSTOTAL		,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_BYTESSAVED		,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_BYTESTOTAL		,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_AREAS			,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_AREA_PROZ		,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_FR				,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDHELP				,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCLEAR				,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_PURGEONE		,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_PURGEALL		,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL			,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
};
static char DlgName[]="IDD_STAT_PURGE";

detpurg::detpurg(CWnd* pParent ) : CDialog(detpurg::IDD, pParent)
{
	//{{AFX_DATA_INIT(detpurg)
	//}}AFX_DATA_INIT
}

void detpurg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(detpurg)
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_AREA_PROZ, m_area_proz);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(detpurg, CDialog)
	//{{AFX_MSG_MAP(detpurg)
	ON_BN_CLICKED(IDCLEAR, OnClear)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_PURGEALL, OnPurgeAll)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PURGEONE, OnBnClickedPurgeone)
END_MESSAGE_MAP()

// ===========================================
	void detpurg::OnCancel()
// ===========================================
{
	ShowWindow(SW_HIDE);
	CDialog::OnCancel();
}

// ===========================================
	int detpurg::UpdateDetails()
// ===========================================
{
char	buf[100],buf2[100],siz[50];
int		cnt;

	if (!gc.purger.text_only)
	{
		fmt_size(buf2,gc.purger.bytes_total);
		fmt_size(siz,gc.purger.bytes_removed);
		sprintf(buf,L("S_598",gc.purger.mails_total,buf2,gc.purger.area_counter,
			gc.purger.number_of_areas,gc.purger.mails_removed,siz));	 
		m_status.SetWindowText(buf);
		m_area_proz.SetRange(0,gc.purger.number_of_areas);
		m_area_proz.SetPos(gc.purger.area_counter);
	}

	if (*m_message)
	{
		m_list.SetRedraw(0);
		_strtime(buf2);
		sprintf(buf,"%s  %s",buf2,m_message);
		m_list.AddString(buf);
		cnt=m_list.GetCount()-2;
		m_list.SetTopIndex(cnt<0 ? 0 : cnt);
		m_list.SetRedraw(1);
	}
	gc.purger.text_only=0;
	*m_message=0;
	return 1;
}

// ===========================================
	void detpurg::OnPurgeAll()
// ===========================================
{
	FipsApp.OnStartpurger();
}

// ===========================================
	void detpurg::OnClear()
// ===========================================
{
	m_list.ResetContent();
	m_status.SetWindowText(NULL);
	m_area_proz.SetPos(0);
}

// ===========================================
	void detpurg::AddMessage(LPCSTR text)
// ===========================================
{
	strcpy(m_message,text);
	writelog(LOG_PURGER,m_message);
	UpdateDetails();
}

// ===========================================
	void detpurg::AddString(LPCSTR text)
// ===========================================
{
char buf[200],buf2[200],*p;

	m_list.SetRedraw(0);
	p= (char *) strchr(text,'\n');
	if (p)
	   *p=0;

	_strtime(buf2);
	sprintf(buf,"%s  %s",buf2,text);
	m_list.AddString(buf);
	int cnt=m_list.GetCount()-2;
	m_list.SetTopIndex(cnt<0 ? 0 : cnt);
	m_list.SetRedraw(1);
}

// ===========================================
	void detpurg::SetLanguage(void)
// ===========================================
{
int  lng[]={
			IDC_PURGEALL,
			IDCANCEL,
			IDCLEAR,
			IDHELP,
			IDC_PURGEONE
			};
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
}

// ===========================================
	BOOL detpurg::PreTranslateMessage(MSG* pMsg)
// ===========================================
{
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam=='3' && (GetKeyState(VK_CONTROL) & 0x80000000))
		OnCancel();
	if (handle_function_key(pMsg))	return 1;
	return CDialog::PreTranslateMessage(pMsg);
}

// ============================================
	void detpurg::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void detpurg::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ===========================================
	void detpurg::SavePosition(void)
// ===========================================
{
    StoreInitDialogSize(DlgName,this);
}

// ===========================================
	void detpurg::OnShowWindow(BOOL bShow, UINT nStatus) 
// ===========================================
{
	CDialog::OnShowWindow(bShow, nStatus);
	(gpMain->GetMenu())->CheckMenuItem(ID_SWITCH_PURGER, 
		MF_BYCOMMAND | (bShow ? MF_CHECKED : MF_UNCHECKED));
	if (!bShow)	gpMain->m_subjlist.SetFocus();
}

// ===========================================
	void detpurg::OnHelp()
// ===========================================
{
	WinHelp(VHELP_STAT_PURGE);
}

// ===========================================
	BOOL detpurg::OnHelpInfo(HELPINFO* pHelpInfo) 
// ===========================================
{
	OnHelp();
	return TRUE;
}

// ===========================================
	void detpurg::OnBnClickedPurgeone()
// ===========================================
{
CAreasel areasel;
long	prev;

	//gc.AreaSelectionMode=1;
	prev=gustat.cur_area_handle;
	gc.asel_as_list=FALSE;
	areasel.DoModal();
	gustat.cur_area_handle=prev;
	//gc.AreaSelectionMode=0;
	if (gc.selected_area<0)	return;
	FipsApp.OnPurgeOne(gc.selected_area);
}
