// sub_adj.cpp : implementation file
// IDD_ADJUST_SUBJ_LIST

#include "stdafx.h"
#include "lightdlg.h"
#include "sub_adj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int		SubjPos[3];
extern CLightDlg 	 *gpMain;
static char DlgName[]="IDD_ADJUST_SUBJ_LIST";

// =================================================================
	sub_adj::sub_adj(CWnd* pParent ) : CDialog(sub_adj::IDD, pParent)
// =================================================================
{
	//{{AFX_DATA_INIT(sub_adj)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =================================================================
	void sub_adj::DoDataExchange(CDataExchange* pDX)
// =================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(sub_adj)
	DDX_Control(pDX, IDC_SLIDER3, m_slider3);
	DDX_Control(pDX, IDC_SLIDER2, m_slider2);
	DDX_Control(pDX, IDC_SLIDER1, m_slider1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(sub_adj, CDialog)
	//{{AFX_MSG_MAP(sub_adj)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_STATIC2, &sub_adj::OnStnClickedStatic2)
	ON_STN_CLICKED(IDC_STATIC3, &sub_adj::OnStnClickedStatic3)
	ON_STN_CLICKED(IDC_STATIC4, &sub_adj::OnStnClickedStatic4)
END_MESSAGE_MAP()

// =================================================================
	BOOL sub_adj::OnInitDialog()
// =================================================================
{
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDCANCEL,
			IDOK
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	m_slider1.SetRange(0,1024,TRUE);
	m_slider1.SetTicFreq(32);
	m_slider1.SetPos(SubjPos[0]);

	m_slider2.SetRange(0,1024,TRUE);
	m_slider2.SetTicFreq(32);
	m_slider2.SetPos(SubjPos[1]);

	m_slider3.SetRange(0,1024,TRUE);
	m_slider3.SetTicFreq(32);
	m_slider3.SetPos(SubjPos[2]);

	SetTimer(2345,300,NULL);
	return TRUE;
}

// =================================================================
	void sub_adj::OnCancel()
// =================================================================
{
	KillTimer(2345);
	SubjPos[0]=get_cfg(CFG_COLORS,"RecipientPos",200);
	SubjPos[1]=get_cfg(CFG_COLORS,"SubjectPos",400);
	SubjPos[2]=get_cfg(CFG_COLORS,"SizePos",600);
	ForceRedraw();
	CDialog::OnCancel();
}

// =================================================================
	void sub_adj::OnOK()
// =================================================================
{
	KillTimer(2345);
	set_cfg(CFG_COLORS,"RecipientPos",SubjPos[0]);
	set_cfg(CFG_COLORS,"SubjectPos",SubjPos[1]);
	set_cfg(CFG_COLORS,"SizePos",SubjPos[2]);
	CDialog::OnOK();
}

// =================================================================
	void sub_adj::OnTimer(UINT nIDEvent)
// =================================================================
{
int redraw=0;
int ret;

	ret=m_slider1.GetPos();
	if (ret!=SubjPos[0])
	{
		redraw=1;
		SubjPos[0]=ret;
	}
	ret=m_slider2.GetPos();
	if (ret!=SubjPos[1])
	{
		redraw=1;
		SubjPos[1]=ret;
	}
	ret=m_slider3.GetPos();
	if (ret!=SubjPos[2])
	{
		redraw=1;
		SubjPos[2]=ret;
	}
	if (redraw)
		ForceRedraw();

	CDialog::OnTimer(nIDEvent);
}

// =================================================================
	void sub_adj::ForceRedraw()
// =================================================================
{
RECT re;

	gpMain->AdjustItemSizes();
	gpMain->m_subjlist.GetClientRect(&re);
	gpMain->m_subjlist.InvalidateRect(&re,1);
}


	void sub_adj::OnStnClickedStatic2()
	{
		// TODO: добавьте свой код обработчика уведомлений
	}


	void sub_adj::OnStnClickedStatic3()
	{
		// TODO: добавьте свой код обработчика уведомлений
	}


	void sub_adj::OnStnClickedStatic4()
	{
		// TODO: добавьте свой код обработчика уведомлений
	}
