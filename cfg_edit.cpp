// cfg_edit.cpp : implementation file
// IDD_CFG_EDITOR

#include "stdafx.h"
#include "lightdlg.h"
#include "light.h"
#include "editwnd.h"
#include "writmail.h"
#include "cfg_edit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CLightApp	FipsApp;
extern CLightDlg	*gpMain;
extern CEditWnd		*gEdit;
extern writmail		*gEditor;
extern _gconfig		gc;

static char DlgName[]="IDD_CFG_EDITOR";
COLORREF TextWndColors[NUM_EDIT_COLORS];

// ================================================
// cfg_edit dialog

CStrList edcfg;

cfg_edit::cfg_edit(CWnd* pParent /*=NULL*/)	: CSAPrefsSubDlg(cfg_edit::IDD, pParent)
{
	//{{AFX_DATA_INIT(cfg_edit)
	m_golded_comp = FALSE;
	m_hide_leading = FALSE;
	m_immed_update = FALSE;
	m_colored = FALSE;
	m_first_non = FALSE;
	m_autoscroll = FALSE;
	m_altcolored = FALSE;
	m_last_read = FALSE;
	m_clear_tearline = FALSE;
	m_netorigin = FALSE;
	m_structured = FALSE;
	m_area_last = FALSE;
	m_repquote = FALSE;
	m_multispace = FALSE;
	m_ndlprefer = FALSE;
	m_filter_re = FALSE;
	m_no_res = FALSE;
	m_no_detsearch = FALSE;
	m_nRMargin = 0;
	m_bAutoIndent = FALSE;
	m_bOemCharset = FALSE;
	m_bWordWrap = FALSE;
	m_nTabSize = 0;
	m_bEqualTabs = FALSE;
	m_bReplaceTabs = FALSE;
	m_bShowTabs = FALSE;
	m_nCurColor = 0;
	m_bColorMark = FALSE;
	//}}AFX_DATA_INIT
}

void cfg_edit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_edit)
	DDX_Control(pDX, IDC_EDITORWIDTH, m_editorwidth);
	DDX_Control(pDX, IDC_TEARLINE, m_tearline);
	DDX_Control(pDX, IDC_MAXUUE, m_maxuue);
	DDX_Check(pDX, IDC_GOLDED_COMP, m_golded_comp);
	DDX_Check(pDX, IDC_HIDE_BEG_EMP_LINES, m_hide_leading);
	DDX_Check(pDX, IDC_IMMEDIATE_UPDATE, m_immed_update);
	DDX_Check(pDX, IDC_COLORED, m_colored);
	DDX_Check(pDX, IDC_FIRST_NON, m_first_non);
	DDX_Check(pDX, IDC_AUTOSCROLL, m_autoscroll);
	DDX_Check(pDX, IDC_ALTCOLORED, m_altcolored);
	DDX_Check(pDX, IDC_LAST_READ, m_last_read);
	DDX_Check(pDX, IDC_LBL_TEARLINE, m_clear_tearline);
	DDX_Check(pDX, IDC_NETMAILORIG, m_netorigin);
	DDX_Check(pDX, IDC_STRUCT_SUBJECT, m_structured);
	DDX_Check(pDX, IDC_AREA_AFTER_LAST, m_area_last);
	DDX_Check(pDX, IDC_REPEAT_QUOTE, m_repquote);
	DDX_Check(pDX, IDC_MULTISPACE, m_multispace);
	DDX_Check(pDX, IDC_NDLPREFER, m_ndlprefer);
	DDX_Check(pDX, IDC_FILTER_RE, m_filter_re);
	DDX_Check(pDX, IDC_NORES, m_no_res);
	DDX_Check(pDX, IDC_NO_DETSEARCH, m_no_detsearch);
	DDX_Text(pDX, IDC_RMARGIN, m_nRMargin);
	DDX_Check(pDX, IDC_AUTOINDENT, m_bAutoIndent);
	DDX_Check(pDX, IDC_USEDOS, m_bOemCharset);
	DDX_Check(pDX, IDC_WORDWRAP, m_bWordWrap);
	DDX_Text(pDX, IDC_TABSIZE, m_nTabSize);
	DDX_Check(pDX, IDC_EQUALTABS, m_bEqualTabs);
	DDX_Check(pDX, IDC_TABTOSPACE, m_bReplaceTabs);
	DDX_Check(pDX, IDC_SHOWTABS, m_bShowTabs);
	DDX_Control(pDX, IDC_COLOR, m_Color);
	DDX_CBIndex(pDX, IDC_COLORS, m_nCurColor);
	DDX_Check(pDX, IDC_COLORMARK, m_bColorMark);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_edit, CDialog)
	//{{AFX_MSG_MAP(cfg_edit)
	ON_CBN_SELCHANGE(IDC_COLORS, OnCbnSelchangeColors)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EDITOR_FONT, OnBnClickedEditorFont)
END_MESSAGE_MAP()

const COLORREF defTextWndColors[] =
{
	//цвета редактора
	GetSysColor(COLOR_WINDOW),		//COLOR_EDIT_WHITESPACE
	GetSysColor(COLOR_WINDOWTEXT),	//COLOR_EDIT_NORMALTEXT
	GetSysColor(COLOR_HIGHLIGHT),		//COLOR_EDIT_SELBKGND
	GetSysColor(COLOR_HIGHLIGHTTEXT),	//COLOR_EDIT_SELTEXT
	RGB(128, 0, 128),			//COLOR_EDIT_QUOTE1
	RGB(110, 0, 0),				//COLOR_EDIT_QUOTE2
	RGB(0, 192, 0),				//COLOR_EDIT_TEARLINE
	RGB(0, 192, 192),			//COLOR_EDIT_TAGLINE
	RGB(0, 128, 0),				//COLOR_EDIT_ORIGIN

	RGB(255, 0, 0),				//COLOR_EDIT_KLUDGE
	RGB(0, 0, 0),				//COLOR_EDIT_MARK0, 0000
	RGB(255, 0, 0),				//0001
	RGB(128, 128, 128),			//0010
	RGB(0, 0, 128),				//0011
	RGB(0, 0, 255),				//0100
	RGB(128, 0, 0),				//0101
	RGB(0, 255, 0),				//0110
	RGB(0, 255, 0),				//0111
	RGB(128, 128, 128),			//1000
	RGB(255, 0, 255),			//1001
	RGB(0, 0, 128),				//1010
	RGB(0, 0, 255),				//1011
	RGB(255, 0, 0),				//1100
	RGB(128, 0, 0),				//1101
	RGB(0, 128, 0),				//1110
	RGB(0, 255, 0),				//COLOR_EDIT_MARK15, 1111
};

/////////////////////////////////////////////////////////////////////////////
// cfg_edit message handlers

// =====================================================================
LPCSTR cfg_edit::GetName(void)	{return DlgName;}
// =====================================================================

// ==============================================
	BOOL cfg_edit::OnInitDialog()
// ==============================================
{
int  lng[]={
			IDC_GOLDED_COMP,
			IDC_HIDE_BEG_EMP_LINES,
			IDC_IMMEDIATE_UPDATE,
			IDC_COLORED,
			IDC_AUTOSCROLL,
			IDC_FIRST_NON,
			IDC_LAST_READ,
			IDC_STRUCT_SUBJECT,
			IDC_STATIC2,
			IDC_ALTCOLORED,
			IDC_NETMAILORIG,
			IDC_LBL_MAXUUE,
			IDC_LBL_TEARLINE,
			IDC_LBL_EDITORWIDTH,
			IDC_AREA_AFTER_LAST,
			IDC_REPEAT_QUOTE,
			IDC_MULTISPACE,
			IDC_NDLPREFER,
			IDC_NO_DETSEARCH,
			IDC_FILTER_RE,
			IDC_NORES,
			IDC_STATIC1,
			IDC_EDITOR_FONT,
			IDC_AUTOINDENT,
			IDC_USEDOS,
			IDC_SHOWTABS,
			IDC_WORDWRAP,
			IDC_EQUALTABS,
			IDC_TABTOSPACE,
			IDC_COLORMARK
			};

	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	m_autoscroll	=get_cfg(CFG_EDIT,"Autoscroll",1);
	m_structured	=get_cfg(CFG_EDIT,"StructDisplay",0);
	m_first_non		=get_cfg(CFG_EDIT,"FirstNonRead",1);
	m_last_read		=m_first_non ? 0 : 1;
	m_golded_comp	=get_cfg(CFG_EDIT,"GoldedComp",1);
	m_hide_leading=get_cfg(CFG_EDIT,"HideLeading",1);
	m_immed_update=get_cfg(CFG_EDIT,"ImmUpdate",1);
	m_colored			=get_cfg(CFG_EDIT,"Colored",1);
	m_altcolored	=get_cfg(CFG_EDIT,"AltColored",1);
	m_clear_tearline=get_cfg(CFG_EDIT,"ClearTearline",0);
	m_netorigin		=get_cfg(CFG_EDIT,"NetmailOrigin",0);
	m_no_res			=get_cfg(CFG_COMMON,"DontGenREs",0);
	m_filter_re		=get_cfg(CFG_COMMON,"FilterREs",0);
	m_tearline.SetWindowText(get_cfg(CFG_EDIT,"Tearline",""));
	m_maxuue.SetWindowText(get_cfg(CFG_EDIT,"MaxUueLines","340"));
	m_editorwidth.SetWindowText(get_cfg(CFG_EDIT,"EditorWidth","80"));
	m_area_last		=get_cfg(CFG_COMMON,"LastAreaSelection",1);
	m_repquote		=get_cfg(CFG_EDIT,"RepeatQuote",1);
	m_multispace	=get_cfg(CFG_EDIT,"MultiSpace",0);
	m_no_detsearch=get_cfg(CFG_COMMON,"NoDetSearch",0);
	m_ndlprefer		=get_cfg(CFG_EDIT,"NdlPrefer",0);

	m_nRMargin		=get_cfg(CFG_EDIT, "RightMargin", 0);
	m_bAutoIndent =get_cfg(CFG_EDIT, "AutoIndent", 1);
	m_bOemCharset =get_cfg(CFG_EDIT, "OemCharset", 1);
	m_bWordWrap		=get_cfg(CFG_EDIT, "WordWrap", 1);
	m_nTabSize		=get_cfg(CFG_EDIT, "TabSize", 4);
	m_bEqualTabs	=get_cfg(CFG_EDIT, "EqualTabs", 1);
	m_bReplaceTabs=get_cfg(CFG_EDIT, "ReplaceTabs", 0);
	m_bShowTabs		=get_cfg(CFG_EDIT, "ViewTabs", 0);
	m_bColorMark	=get_cfg(CFG_EDIT, "ColorMark", 1);

	memcpy(m_aColors, TextWndColors, sizeof(COLORREF) * NUM_EDIT_COLORS);
	m_Color.currentcolor = m_aColors[m_nCurColor];
	m_nLastColor = m_nCurColor;

	UpdateData(0);
	return TRUE;
}

// ==============================================
	void cfg_edit::OnOK()
// ==============================================
{
CString	str;
int		m;

	UpdateData(1);
	set_cfg(CFG_EDIT,"Autoscroll",m_autoscroll);
	set_cfg(CFG_EDIT,"StructDisplay",m_structured);
	set_cfg(CFG_EDIT,"FirstNonRead",m_first_non);
	set_cfg(CFG_EDIT,"GoldedComp",m_golded_comp);
	set_cfg(CFG_EDIT,"HideLeading",m_hide_leading);
	set_cfg(CFG_EDIT,"ImmUpdate",m_immed_update);
	set_cfg(CFG_EDIT,"Colored",m_colored);
	set_cfg(CFG_EDIT,"AltColored",m_altcolored);
	set_cfg(CFG_EDIT,"ClearTearline",m_clear_tearline);
	set_cfg(CFG_EDIT,"NetmailOrigin",m_netorigin);
	set_cfg(CFG_COMMON,"DontGenREs",m_no_res);
	set_cfg(CFG_COMMON,"FilterREs",m_filter_re);
	set_cfg(CFG_COMMON,"LastAreaSelection",m_area_last);
	set_cfg(CFG_EDIT,"RepeatQuote",m_repquote);
	set_cfg(CFG_EDIT,"MultiSpace",m_multispace);
	set_cfg(CFG_COMMON,"NoDetSearch",m_no_detsearch);
	set_cfg(CFG_EDIT,"NdlPrefer",m_ndlprefer);
	gc.RepeatQuote=m_repquote;
	gc.MultiSpace=m_multispace;
	gc.NdlPrefer=m_ndlprefer;
	m_tearline.GetWindowText(str);
	set_cfg(CFG_EDIT,"Tearline",str);
	m_maxuue.GetWindowText(str);
	m=atoi(str);
	if (m<10 || m>1000)	m=340;
	set_cfg(CFG_EDIT,"MaxUueLines",m);
	m_editorwidth.GetWindowText(str);
	m=atoi(str);
	if (m<20 || m>100)	m=80;
	set_cfg(CFG_EDIT,"EditorWidth",m);

	set_cfg(CFG_EDIT, "RightMargin", m_nRMargin, FALSE);
	set_cfg(CFG_EDIT, "AutoIndent", m_bAutoIndent, FALSE);
	set_cfg(CFG_EDIT, "OemCharset", m_bOemCharset, FALSE);
	set_cfg(CFG_EDIT, "WordWrap", m_bWordWrap, FALSE);
	set_cfg(CFG_EDIT, "TabSize", m_nTabSize, FALSE);
	set_cfg(CFG_EDIT, "EqualTabs", m_bEqualTabs, FALSE);
	set_cfg(CFG_EDIT, "ReplaceTabs", m_bReplaceTabs, FALSE);
	set_cfg(CFG_EDIT, "ViewTabs", m_bShowTabs, FALSE);
	set_cfg(CFG_EDIT, "ColorMark", m_bColorMark);

	memcpy(TextWndColors, m_aColors, sizeof(COLORREF) * NUM_EDIT_COLORS);

	if (gEditor)	gEditor->ApplyEditOptions();

	load_editor_cfg();
	gpMain->m_show_attributes.SetPushed(gc.display_attributes);
	gpMain->m_show_hidden.SetPushed(gc.show_seenby || gc.show_tags);
	gpMain->m_show_deleted.SetPushed(!gc.hide_deleted);
	gc.structured_display=!gc.structured_display; // will be reversed in sub
	gpMain->OnToggleStruct();
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ==============================================
	void cfg_edit::OnHelp()
// ==============================================
{
	WinHelp(VHELP_CFG_EDITOR);
}

// ==============================================
	BOOL cfg_edit::OnHelpInfo(HELPINFO* pHelpInfo) 
// ==============================================
{
	OnHelp();
	return TRUE;
}

// ==============================================
	void cfg_edit::OnCbnSelchangeColors()
// ==============================================
{
	UpdateData(1);
	m_aColors[m_nLastColor] = m_Color.currentcolor;
	m_Color.currentcolor = m_aColors[m_nCurColor];
	m_Color.Invalidate();
	m_Color.UpdateWindow();
	m_nLastColor = m_nCurColor;
}

void load_textwnd_colors()
{
	char key[32];
	for (int i = 0; i < NUM_EDIT_COLORS; i++)
	{
		sprintf(key, "Color%d", i);
		TextWndColors[i] = get_cfg(CFG_EDIT, key, defTextWndColors[i]);
	}
}

void store_textwnd_colors()
{
	char key[32];
	for (int i = 0; i < NUM_EDIT_COLORS; i++)
	{
		sprintf(key, "Color%d", i);
		set_cfg(CFG_EDIT, key, TextWndColors[i]);
	}
}

void cfg_edit::OnBnClickedEditorFont()
{
	FipsApp.OnChooseFontEditor();
}
