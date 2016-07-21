// det_nl.cpp : implementation file
// IDD_STAT_NLCOMP

#include "stdafx.h"
#include "lightdlg.h"
#include "det_nl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CWinThread			*th_nl;
extern	CLightDlg 	*gpMain;
extern _gconfig gc;

static char DlgName[]="IDD_STAT_NLCOMP";

static _DlgItemsSize DlgItemsSize [] =
{
	0,					{0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDHELP,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCLEAR,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCHECK,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDSTART,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_LIST,			{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
};

// ====================================================================
	det_nl::det_nl(CWnd *pParent)	: CDialog(det_nl::IDD, pParent)
// ====================================================================
{
	//{{AFX_DATA_INIT(det_nl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ====================================================================
	void det_nl::DoDataExchange(CDataExchange* pDX)
// ====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(det_nl)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(det_nl, CDialog)
	//{{AFX_MSG_MAP(det_nl)
	ON_BN_CLICKED(IDCLEAR, OnClear)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCHECK, OnCheck)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDSTART, OnStart)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ====================================================================
	BOOL det_nl::OnInitDialog()
// ====================================================================
{
	CDialog::OnInitDialog();
	EXTENT_LB(IDC_LIST,400);
	return TRUE;
}

// ====================================================================
	void det_nl::OnStart()
// ====================================================================
{
DWORD dm;

	if (gc.detnl.running)	return;
	if (gc.detnl.checking)	ERR_MSG_RET("E_NLCCIR");

	gc.detnl.running=1;
	th_nl=AfxBeginThread(NlCompMainThread,&dm,THREAD_PRIORITY_LOWEST);
}

// ====================================================================
	void det_nl::AddString(char *text)
// ====================================================================
{
char	buf[200],buf2[200];
char	*p;
int		cnt;

	m_list.SetRedraw(0);
	p=strchr(text,'\n');
	if (p)
	   *p=0;

	_strtime(buf2);
	sprintf(buf,"%s  %s",buf2,text);
	if (strlen(text)==0)
		strcpy(buf," ");
	m_list.AddString(buf);
	cnt=m_list.GetCount()-2;
	m_list.SetTopIndex(cnt<0 ? 0 : cnt);
	m_list.SetRedraw(1);
}

// ====================================================================
	void det_nl::OnClear()
// ====================================================================
{
	m_list.ResetContent();
}

// ====================================================================
	void det_nl::OnCancel()
// ====================================================================
{
	ShowWindow(SW_HIDE);
	CDialog::OnCancel();
}

// ===========================================
	void det_nl::SetLanguage(void)
// ===========================================
{
int  lng[]={
			IDSTART,
			IDCANCEL,
			IDCLEAR,
			IDCHECK,
			IDHELP
			};

    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
}

// ===========================================
	BOOL det_nl::PreTranslateMessage(MSG *pMsg)
// ===========================================
{
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam=='4' && (GetKeyState(VK_CONTROL) & 0x80000000))
		OnCancel();
	if (handle_function_key(pMsg))	return 1;
	return CDialog::PreTranslateMessage(pMsg);
}

// ===========================================
	void det_nl::OnClose()
// ===========================================
{
	OnCancel();
}

// ===========================================
	void det_nl::OnCheck()
// ===========================================
{
	if (gc.detnl.running)	ERR_MSG_RET("E_NLCTR");
	DISABLEID(IDSTART);
	gc.detnl.checking=1;
	gpMain->BeginWaitCursor();
	check_nodelists();
	gc.detnl.checking=0;
	gpMain->EndWaitCursor();
	ENABLEID(IDSTART);
}

// ============================================
	void det_nl::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void det_nl::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ============================================
	void det_nl::SavePosition(void)
// ============================================
{
    StoreInitDialogSize(DlgName,this);
}

// ============================================
	void det_nl::OnMove(int x, int y)
// ============================================
{
	CDialog::OnMove(x, y);
}

void det_nl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	(gpMain->GetMenu())->CheckMenuItem(ID_SWITCH_NODELIST, 
		MF_BYCOMMAND | (bShow ? MF_CHECKED : MF_UNCHECKED));
	if (!bShow)	gpMain->m_subjlist.SetFocus();
}

// ===========================================
	void det_nl::OnHelp()
// ===========================================
{
	WinHelp(VHELP_STAT_NLCOMP);
}

BOOL det_nl::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
