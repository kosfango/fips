// floating.cpp : implementation file
// IDD_FLOATING_BAR

#include "stdafx.h"
#include "resource.h"
#include "lightdlg.h"
#include "floating.h"
#include "structs.h"
#include "cfg_icon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static _DlgItemsSize DlgItemsSize [] ={0,{0,0,0,0},{0,0,0,0},0,0,0,0,0};
static char		DlgName[]="IDD_FLOATING_BAR";

static int now_flag=0;

extern CLightDlg *gpMain;
extern int IconOrder[];
extern _gconfig gc;

// ==================================================================
	floating::floating(CWnd* pParent ) 	: CDialog(floating::IDD, pParent)
// ==================================================================
{
	//{{AFX_DATA_INIT(floating)
	//}}AFX_DATA_INIT
}

// ==================================================================
	void floating::DoDataExchange(CDataExchange* pDX)
// ==================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(floating)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(floating, CDialog)
	//{{AFX_MSG_MAP(floating)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_FILT, OnFilter)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_ASAVE,  OnAsave)
	ON_BN_CLICKED(IDC_FIND,   OnFind)
	ON_BN_CLICKED(IDC_NEW,    OnNew)
	ON_BN_CLICKED(IDC_QUOTE,  OnQuote)
	ON_BN_CLICKED(IDC_QUOTEA, OnQuoteA)
	ON_BN_CLICKED(IDC_FREQ,   OnFreq)
	ON_BN_CLICKED(IDC_MACRO1, OnMacro1)
	ON_BN_CLICKED(IDC_MACRO2, OnMacro2)
	ON_BN_CLICKED(IDC_MACRO3, OnMacro3)
	ON_BN_CLICKED(IDC_MACRO4, OnMacro4)
	ON_BN_CLICKED(IDC_MACRO5, OnMacro5)
	ON_BN_CLICKED(IDC_EXIT,   OnExit)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ==================================================================
	BOOL floating::OnInitDialog()
// ==================================================================
{
	CDialog::OnInitDialog();
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));

	create_button(m_autosave,IDC_ASAVE,IDI_ASAVE);
	create_button(m_find,IDC_FIND,IDI_FIND);
	create_button(m_newmail,IDC_NEW,IDI_NEW);
	create_button(m_quote,IDC_QUOTE,IDI_QUOTE);
	create_button(m_quotea,IDC_QUOTEA,IDI_QUOTEA);
	create_button(m_freq,IDC_FREQ,IDI_FREQ);
	create_button(m_udef1,IDC_MACRO1,IDI_MACRO1);
	create_button(m_udef2,IDC_MACRO2,IDI_MACRO2);
	create_button(m_udef3,IDC_MACRO3,IDI_MACRO3);
	create_button(m_udef4,IDC_MACRO4,IDI_MACRO4);
	create_button(m_udef5,IDC_MACRO5,IDI_MACRO5);
	create_button(m_exit,IDC_EXIT,IDI_EXIT);
	create_button(m_abortscript,IDC_ABORT,IDI_ABORT);
	create_button(m_filter,IDC_FILT,IDI_FILT);
	m_filter.SetStyleToggle(TRUE);
// tooltips
	tip.Create(this);
	tip.SetMaxTipWidth(0xFFFFFF);
	tip.AddTool(&m_autosave,"");
	tip.AddTool(&m_find,"");
	tip.AddTool(&m_newmail,"");
	tip.AddTool(&m_quote,"");
	tip.AddTool(&m_quotea,"");
	tip.AddTool(&m_freq,"");
	tip.AddTool(&m_exit,"");
	tip.AddTool(&m_abortscript,"");
	tip.AddTool(&m_filter,"");
	tip.AddTool(&m_udef1,"");
	tip.AddTool(&m_udef2,"");
	tip.AddTool(&m_udef3,"");
	tip.AddTool(&m_udef4,"");
	tip.AddTool(&m_udef5,"");
	tip.Activate(!gc.NoTooltips);
	EnableToolTips(!gc.NoTooltips);
	
	now_flag=1;
	RepositionAllBitmaps();
	return TRUE;
}

// ==================================================================
	void floating::OnSize(UINT nType, int cx, int cy)
// ==================================================================
{
RECT re;

	CDialog::OnSize(nType, cx, cy);
	GetClientRect(&re);
	InvalidateRect(&re);
	RepositionAllBitmaps();
	GetClientRect(&re);
	InvalidateRect(&re);
}

// ============================================
	void floating::SavePosition(void)
// ============================================
{
    StoreInitDialogSize(DlgName,this);
}

// ============================================
	void floating::RepositionAllBitmaps(void)
// ============================================
{
RECT	re;
int		columns,width,i,cur_column=0,cur_row=0;

	if (!now_flag)	return;

	GetClientRect(&re);
	width=re.right-re.left;

	columns=(width / BARIW)-1;
	if (width % BARIW)
	{
		if ((width % BARIW) > (BARIW / 2))
			columns++;
	}

	cur_column=cur_row=0;
	re.top=0;
	re.bottom=0;
	re.left=0;
	re.right=0;

	m_filter.MoveWindow(&re);
	m_autosave.MoveWindow(&re);
	m_find.MoveWindow(&re);
	m_newmail.MoveWindow(&re);
	m_quote.MoveWindow(&re);
	m_quotea.MoveWindow(&re);
	m_freq.MoveWindow(&re);
	m_udef1.MoveWindow(&re);
	m_udef2.MoveWindow(&re);
	m_udef3.MoveWindow(&re);
	m_udef4.MoveWindow(&re);
	m_udef5.MoveWindow(&re);
	m_exit.MoveWindow(&re);

	for (i=0;i<MAX_ICONS;i++)
	{
		if (IconOrder[i]==0)	break;
		re.top		=cur_row*BARIH;
		re.bottom	=re.top+BARIH;
		re.left		=cur_column*BARIW;
		re.right	=re.left+BARIW;
		switch (IconOrder[i])
		{
		case IDI_FILT:
			m_filter.MoveWindow(&re);
			break;
		case IDI_ASAVE:
			m_autosave.MoveWindow(&re);
			break;
		case IDI_FIND:
			m_find.MoveWindow(&re);
			break;
		case IDI_NEW:
			m_newmail.MoveWindow(&re);
			break;
		case IDI_QUOTE:
			m_quote.MoveWindow(&re);
			break;
		case IDI_QUOTEA:
			m_quotea.MoveWindow(&re);
			break;
		case IDI_FREQ:
			m_freq.MoveWindow(&re);
			break;
		case IDI_MACRO1:
			m_udef1.MoveWindow(&re);
			break;
		case IDI_MACRO2:
			m_udef2.MoveWindow(&re);
			break;
		case IDI_MACRO3:
			m_udef3.MoveWindow(&re);
			break;
		case IDI_MACRO4:
			m_udef4.MoveWindow(&re);
			break;
		case IDI_MACRO5:
			m_udef5.MoveWindow(&re);
			break;
		case IDI_EXIT:
			m_exit.MoveWindow(&re);
			break;
		default:
		case IDI_EMPTY:
			break;
		}
		if (++cur_column>columns)
		{
			cur_row++;
			cur_column=0;
		}
	}
}

// ===================================================================================
	BOOL floating::PreTranslateMessage(MSG* pMsg)
// ===================================================================================
{
	if (tip)	tip.RelayEvent(pMsg);
	if (pMsg->message==WM_RBUTTONDOWN && HandleRightButton(pMsg))	return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

// ===================================================================================
	void floating::OnFilter()	
// ===================================================================================
{ 
	gpMain->m_filter.SetPushed(m_filter.IsPushed());
	gpMain->OnFilter();	
	gpMain->m_subjlist.SetFocus(); 
}
	void floating::OnAsave()	{ gpMain->OnAsave();	gpMain->m_subjlist.SetFocus(); }
	void floating::OnFind()		{ gpMain->OnFind(); }
	void floating::OnNew()		{ gpMain->OnNew();	gpMain->m_subjlist.SetFocus(); }
	void floating::OnQuote()	{ gpMain->OnQuote();	gpMain->m_subjlist.SetFocus(); }
	void floating::OnQuoteA()	{ gpMain->OnQuoteA(); gpMain->m_subjlist.SetFocus(); }
	void floating::OnFreq()		{ gpMain->OnFreq();	gpMain->m_subjlist.SetFocus(); }
	void floating::OnMacro1()	{ gpMain->OnMacro1(); gpMain->m_subjlist.SetFocus(); }
	void floating::OnMacro2()	{ gpMain->OnMacro2(); gpMain->m_subjlist.SetFocus(); }
	void floating::OnMacro3()	{ gpMain->OnMacro3(); gpMain->m_subjlist.SetFocus(); }
	void floating::OnMacro4()	{ gpMain->OnMacro4();	gpMain->m_subjlist.SetFocus(); }
	void floating::OnMacro5()	{ gpMain->OnMacro5();	gpMain->m_subjlist.SetFocus(); }
	void floating::OnExit()		{ gpMain->OnExit(); }
// ===================================================================================

/*/ ============================================
	void floating::OnRButtonDown(UINT nFlags, CPoint point)
// ============================================
{
	cfg_icon tmp;
	tmp.DoModal();
	gpMain->m_subjlist.SetFocus();
	CDialog::OnRButtonDown(nFlags, point);
}
*/
// ============================================
	void floating::OnMove(int x, int y)
// ============================================
{
	CDialog::OnMove(x,y);
}

// ============================================
	void floating::OnClose()
// ============================================
{
	(gpMain->GetMenu())->CheckMenuItem(ID_HSFLOATING, MF_BYCOMMAND | MF_UNCHECKED);
	CDialog::OnClose();
}

// ============================================
	void floating::create_button(SButton &ctl,int id,int resid)
// ============================================
{
	ctl.SubclassWindow(GetDlgItem(id)->GetSafeHwnd());
	ctl.SetImages(resid);
	ctl.SetStyleFlat(TRUE);
	ctl.SetFrameWidth(SButton::THICK_FRAME);
}

// ============================================
	void floating::OnShowWindow(BOOL bShow, UINT nStatus) 
// ============================================
{
	CDialog::OnShowWindow(bShow, nStatus);
	(gpMain->GetMenu())->CheckMenuItem(ID_HSFLOATING, MF_BYCOMMAND | (bShow ? MF_CHECKED:MF_UNCHECKED));
}

// ============================================
	void floating::RefreshTips(void) 
// ============================================
{
	tip.UpdateTipText(L("S_518"),&m_autosave);
	tip.UpdateTipText(L("S_519"),&m_find);
	tip.UpdateTipText(L("S_515"),&m_newmail);
	tip.UpdateTipText(L("S_516"),&m_quote);
	tip.UpdateTipText(L("S_517"),&m_quotea);
	tip.UpdateTipText(L("S_520"),&m_freq);
	tip.UpdateTipText(L("S_521"),&m_exit);
	tip.UpdateTipText(L("S_588"),&m_abortscript);
	tip.UpdateTipText(L("S_530"),&m_filter);
	tip.UpdateTipText(get_script_desc(1),&m_udef1);
	tip.UpdateTipText(get_script_desc(2),&m_udef2);
	tip.UpdateTipText(get_script_desc(3),&m_udef3);
	tip.UpdateTipText(get_script_desc(4),&m_udef4);
	tip.UpdateTipText(get_script_desc(5),&m_udef5);
}

// ============================================
	void floating::set_images(void)
// ============================================
{
	m_autosave.SetImages(IDI_ASAVE);
	m_find.SetImages(IDI_FIND);
	m_newmail.SetImages(IDI_NEW);
	m_quote.SetImages(IDI_QUOTE);
	m_quotea.SetImages(IDI_QUOTEA);
	m_freq.SetImages(IDI_FREQ);
	m_udef1.SetImages(IDI_MACRO1);
	m_udef2.SetImages(IDI_MACRO2);
	m_udef3.SetImages(IDI_MACRO3);
	m_udef4.SetImages(IDI_MACRO4);
	m_udef5.SetImages(IDI_MACRO5);
	m_exit.SetImages(IDI_EXIT);
	m_abortscript.SetImages(IDI_ABORT);
	m_filter.SetImages(IDI_FILT);
	Invalidate();
}
