// inouthis.cpp : implementation file
// IDD_INOUT_HIST

#include "stdafx.h"
#include <io.h>
#include "inouthis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;
static _DlgItemsSize DlgItemsSize [] =
{
	0,				{0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST1,		{0,0,0,0},{0,0,0,0},0,0,VE|HO,VE|HO,0,
	IDC_INCOME,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_OUTGOING,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
};
static char DlgName[]="IDD_INOUT_HIST";

// ======================================================================
	inouthis::inouthis(CWnd* pParent ) 	: CDialog(inouthis::IDD, pParent)
// ======================================================================
{
	//{{AFX_DATA_INIT(inouthis)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ======================================================================
	void inouthis::DoDataExchange(CDataExchange* pDX)
// ======================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(inouthis)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(inouthis, CDialog)
	//{{AFX_MSG_MAP(inouthis)
	ON_BN_CLICKED(IDC_INCOME, OnIncome)
	ON_BN_CLICKED(IDC_OUTGOING, OnOutgoing)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ======================================================================
	BOOL inouthis::OnInitDialog()
// ======================================================================
{
int tabs[]={70,150,230,320};
int  lng[]={
			IDC_INCOME,
			IDC_OUTGOING,
			IDOK,
			};
    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
	TABULATE_LB(IDC_LIST1);
	EXTENT_LB(IDC_LIST1,900);

	return TRUE;
}

// ======================================================================
	void inouthis::OnIncome()
// ======================================================================
{
	display_this_id(1);
}

// ======================================================================
	void inouthis::OnOutgoing()
// ======================================================================
{
	display_this_id(0);
}

// ======================================================================
	void inouthis::OnOK()
// ======================================================================
{
    StoreInitDialogSize (DlgName,this);
	CDialog::OnOK();
}

// ============================================
	void inouthis::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void inouthis::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ======================================================================
	void inouthis::display_this_id(int id)
// ======================================================================
{
int			ret;
char		fname[300];
CStrList	lst;

	if (id)
		make_path(fname,gc.BasePath,"inbound.hst");
	else
		make_path(fname,gc.BasePath,"outbound.hst");

	lst.LoadFromFile(fname);
	UPDATE_LB(lst,IDC_LIST1);

	ret=lst.GetCount()-1;
	if (ret>=0)
		m_list.SetCurSel(ret);
}

