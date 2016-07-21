// cfg_col.cpp : implementation file
// IDD_CFG_COLOR

#include "stdafx.h"
#include "light.h"
#include "lightdlg.h"
#include "owlist.h"
#include "sub_adj.h"
#include "cfg_col.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CLightApp FipsApp;
extern CLightDlg *gpMain;
extern _gconfig gc;
extern CFont    font_ANSI;
extern CFont    font_OEM;
extern CFont    font_mailtext;
extern CFont    font_subject;
extern long		gColors[20];		// global array

HBRUSH      hbackground=0;			// The Background Brush
HBRUSH      hbackground_subject=0;	// The Background Brush in subject
LOGFONT		ansi_lf,oem_lf;			// old mailtext font
CStrList	samples_list;			// samples strings
CStrList	patterns_list;			// patterns strings
int			colors_changed=0;		// Color's have changed
int			base_colors_changed=0;	// Color's have changed in subject
int			saved_display_mode;
void		load_sample_list(BOOL conv);

static char	DlgName[]="IDD_CFG_COLOR";

// ==========================================================================================
	cfg_col::cfg_col(CWnd* pParent /*=NULL*/)  : CSAPrefsSubDlg(cfg_col::IDD, pParent)
// ==========================================================================================
{
	//{{AFX_DATA_INIT(cfg_col)
	//}}AFX_DATA_INIT
}

// ==========================================================================================
	void cfg_col::DoDataExchange(CDataExchange* pDX)
// ==========================================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_col)
	DDX_Control(pDX, IDC_SAMPLE, m_pattern);
	DDX_Control(pDX, IDC_TEXTCONV, m_oem);
	DDX_Control(pDX, IDC_IGNORE_CASE, m_ignorecase);
	DDX_Control(pDX, IDC_RANGE_SUBJECT, m_patterns_range);
	DDX_Control(pDX, IDC_RANGE, m_samples_range);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_col, CDialog)
	//{{AFX_MSG_MAP(cfg_col)
	ON_BN_CLICKED(IDC_BASECOLOR, OnBasecolor)
	ON_BN_CLICKED(IDC_INFORMATION, OnInformation)
	ON_BN_CLICKED(IDC_NORMALTEXT, OnNormaltext)
	ON_BN_CLICKED(IDC_ODDQUOTE, OnOddQuote)
	ON_BN_CLICKED(IDC_EVENQUOTE, OnEvenQuote)
	ON_BN_CLICKED(IDC_ORIGIN, OnOrigin)
	ON_BN_CLICKED(IDC_SET, OnSetSamples)
	ON_BN_CLICKED(IDC_ADD, OnAddPattern)
	ON_BN_CLICKED(IDC_BACKGROUND_SUBJ, OnBackgroundSubj)
	ON_BN_CLICKED(IDC_TEXT_SUBJ, OnTextSubj)
	ON_BN_CLICKED(IDC_ARROW_SUBJ, OnArrowSubj)
	ON_BN_CLICKED(IDC_COLOR_SELECT, OnColorSelect)
	ON_BN_CLICKED(IDC_DELETE, OnDeletePattern)
	ON_BN_CLICKED(IDC_BOLD_BUTTON, OnBoldButton)
	ON_BN_CLICKED(IDC_TEXTCONV, OnTextconv)
	ON_BN_CLICKED(IDC_SUBJLIST_FONT, OnSubjlistFont)
	ON_BN_CLICKED(IDC_TEXTANSI_FONT, OnTextansiFont)
	ON_BN_CLICKED(IDC_TEXTOEM_FONT, OnTextoemFont)
	ON_BN_CLICKED(IDC_EDITOR_FONT, OnEditorFont)
	ON_BN_CLICKED(IDC_LABELS_FONT, OnLabelsFont)
	ON_BN_CLICKED(IDC_FIELDS_FONT, OnFieldsFont)
	ON_LBN_SELCHANGE(IDC_PATTERNS_LIST, OnSelchangePatternsList)
	ON_BN_CLICKED(IDC_CHANGE, OnChangePattern)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
	LPCSTR cfg_col::GetName(void)	{return DlgName;}
// =====================================================================

// ==========================================================================================
	BOOL cfg_col::OnInitDialog()
// ==========================================================================================
{
CHARFORMAT cf;
CString	str;
int  lng[]={
			IDC_BACKGROUND_SUBJ,
			IDC_TEXT_SUBJ,
			IDC_ARROW_SUBJ,
			IDC_COLOR_SELECT,
			IDC_IGNORE_CASE,
			IDC_ADD,
			IDC_DELETE,
			IDC_BASECOLOR,
			IDC_NORMALTEXT,
			IDC_ODDQUOTE,
			IDC_INFORMATION,
			IDC_ORIGIN,
			IDC_BOLD_BUTTON,
			IDC_STATIC8,
			IDC_STATIC1,
			IDC_EVENQUOTE,
			IDC_TEXTCONV,
			IDC_STATIC3,
			IDC_STATIC2,
			IDC_CHANGE,
			IDC_SUBJLIST_FONT,
			IDC_TEXTANSI_FONT,
			IDC_TEXTOEM_FONT,
			IDC_EDITOR_FONT,
			IDC_LABELS_FONT,
			IDC_FIELDS_FONT,
			};

	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	get_colors();
// backup fonts
	font_ANSI.GetLogFont(&ansi_lf);
	font_OEM.GetLogFont(&oem_lf);

	m_oem.SetCheck(gc.ViewOem);
	saved_display_mode=gc.display_attributes;
	gc.display_attributes=1;
	load_sample_list(gc.ViewOem);

	VERIFY(m_samples_list.SubclassDlgItem(IDC_SAMPLES_LIST, this));
	m_samples_list.SetFont(&font_mailtext);
	m_samples_list.type=0;

	VERIFY(m_patterns_list.SubclassDlgItem(IDC_PATTERNS_LIST, this));
	m_patterns_list.SetFont(&font_subject);
	m_pattern.SetFont(&font_subject);

	m_samples_range.SetRange(10,30,TRUE);
	m_samples_range.SetPos(gColors[TEXT_LINE_DISTANCE]);
	m_patterns_range.SetRange(10,30,TRUE);
	m_patterns_range.SetPos(gColors[SUBJ_LINE_DISTANCE]);

	OnSetSamples();
	OnSetPatterns();
// set color of sample richtextbox
	memset(&cf,0,sizeof(cf));
	cf.crTextColor=RGB(255,255,255);
	cf.dwMask=CFM_COLOR;
	m_pattern.SetDefaultCharFormat(cf);
	m_pattern.SetBackgroundColor(FALSE,gColors[SUBJ_BACKGR_COLOR]);
	SetTimer(2345,300,NULL);
	return TRUE;
}

// ==========================================================================================
	void cfg_col::OnBasecolor()  {GetColor(TEXT_BACKGR_COLOR,samples_list,IDC_SAMPLES_LIST);}
	void cfg_col::OnInformation(){GetColor(KLUDGE_COLOR,samples_list,IDC_SAMPLES_LIST);}
	void cfg_col::OnNormaltext() {GetColor(TEXT_COLOR,samples_list,IDC_SAMPLES_LIST);}
	void cfg_col::OnBoldButton() {GetColor(BOLD_COLOR,samples_list,IDC_SAMPLES_LIST);}
	void cfg_col::OnOddQuote()   {GetColor(ODD_QUOTE_COLOR,samples_list,IDC_SAMPLES_LIST);}
	void cfg_col::OnEvenQuote()  {GetColor(EVEN_QUOTE_COLOR,samples_list,IDC_SAMPLES_LIST);}
	void cfg_col::OnOrigin()     {GetColor(ORIGIN_COLOR,samples_list,IDC_SAMPLES_LIST);}

// ==========================================================================================
	void cfg_col::OnSetPatterns()
// ==========================================================================================
{
	m_patterns_list.itemhigh=m_patterns_range.GetPos();
	gColors[SUBJ_LINE_DISTANCE]=m_patterns_list.itemhigh ;
	UPDATE_LB(patterns_list,IDC_PATTERNS_LIST);
}

// ==========================================================================================
	void cfg_col::OnSetSamples()
// ==========================================================================================
{
	m_samples_list.itemhigh=m_samples_range.GetPos();
	gColors[TEXT_LINE_DISTANCE]=m_samples_list.itemhigh ;
	UPDATE_LB(samples_list,IDC_SAMPLES_LIST);
}

// ==========================================================================================
	void cfg_col::OnTextconv() 
// ==========================================================================================
{
	set_mailtext_font(m_oem.GetCheck() ? &oem_lf : &ansi_lf);
	m_samples_list.SetFont(&font_mailtext);
	load_sample_list(m_oem.GetCheck());
	OnSetSamples();
}

// ==========================================================================================
	void cfg_col::OnCancel()
// ==========================================================================================
{
	KillTimer(2345);
	gc.display_attributes=saved_display_mode;	// restore attr display mode
	get_colors();
//	colors_changed=1;
//	base_colors_changed=1;
//	gpMain->AdjustItemSizes();
//	patterns_list.LoadFromFile("subcol.cfg");
// restore fonts
 	set_mailtext_font(gc.ViewOem ? &oem_lf : &ansi_lf);
	CDialog::OnCancel();
}

// ==========================================================================================
	void cfg_col::OnOK()
// ==========================================================================================
{
RECT re;

	KillTimer(2345);
	gc.display_attributes=saved_display_mode;
	gc.ViewOem=m_oem.GetCheck();
	if (gc.ViewOem)
	{
		set_mailtext_font(&oem_lf);
		gpMain->m_statusbar.SetText("DOS",3,0);
	}
	else
	{
		set_mailtext_font(&ansi_lf);
		gpMain->m_statusbar.SetText("WIN",3,0);
	}
	set_cfg(CFG_COLORS,"ViewOem",gc.ViewOem);
	set_cfg(CFG_COLORS,"SubjBgrColor",gColors[SUBJ_BACKGR_COLOR]);
	set_cfg(CFG_COLORS,"TextBgrColor",gColors[TEXT_BACKGR_COLOR]);
	set_cfg(CFG_COLORS,"SubjColor",gColors[SUBJ_COLOR]);
	set_cfg(CFG_COLORS,"TextColor",gColors[TEXT_COLOR]);
	set_cfg(CFG_COLORS,"OddColor",gColors[ODD_QUOTE_COLOR]);
	set_cfg(CFG_COLORS,"EvenColor",gColors[EVEN_QUOTE_COLOR]);
	set_cfg(CFG_COLORS,"KludgeColor",gColors[KLUDGE_COLOR]);
	set_cfg(CFG_COLORS,"OriginColor",gColors[ORIGIN_COLOR]);
	set_cfg(CFG_COLORS,"BoldColor",gColors[BOLD_COLOR]);
	set_cfg(CFG_COLORS,"TextLineDist",gColors[TEXT_LINE_DISTANCE]);
	set_cfg(CFG_COLORS,"SubjLineDist",gColors[SUBJ_LINE_DISTANCE]);
	set_cfg(CFG_COLORS,"NestColor",gColors[NEST_COLOR]);
	set_cfg(CFG_COLORS,"WriteEditSize",gColors[WRITMAILSIZE]);

	gpMain->m_mailview.itemhigh=gColors[TEXT_LINE_DISTANCE ];
	gpMain->m_subjlist.itemhigh=gColors[SUBJ_LINE_DISTANCE ];
	gpMain->m_attrlist.itemhigh=gColors[SUBJ_LINE_DISTANCE ];
 	gpMain->AdjustItemSizes();

	gpMain->m_subjlist.SetItemHeight(0,gColors[SUBJ_LINE_DISTANCE]);
	gpMain->m_attrlist.SetItemHeight(0,gColors[SUBJ_LINE_DISTANCE]);
	gpMain->m_mailview.SetItemHeight(0,gColors[TEXT_LINE_DISTANCE]);

	gpMain->GetClientRect(&re);
	gpMain->InvalidateRect(&re);
	patterns_list.SaveToFile("subcol.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ==========================================================================================
	HBRUSH cfg_col::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// ==========================================================================================
{
HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd==GetDlgItem(IDC_SAMPLES_LIST))	// in samples list
	{
		if (colors_changed)
		{
			if (hbackground)
			{
			    DeleteObject(hbackground);
				hbackground=0;
			}
			colors_changed=0;
		}

		if (!hbackground)
		   hbackground=CreateSolidBrush(gColors[TEXT_BACKGR_COLOR]);

		if (CTLCOLOR_LISTBOX==nCtlColor)
		   return (hbackground);

	}
	else if (pWnd==GetDlgItem(IDC_PATTERNS_LIST)) // in subjects list
	{
		if (base_colors_changed)
		{
			if (hbackground_subject)
			{
			    DeleteObject(hbackground_subject);
				hbackground_subject=0;
			}
			base_colors_changed=0;
		}
		if (!hbackground_subject)
		   hbackground_subject=CreateSolidBrush(gColors[SUBJ_BACKGR_COLOR]);

		if (CTLCOLOR_LISTBOX==nCtlColor)
		   return (hbackground_subject);
	}
	return hbr;
}

// ==========================================================================================
	void cfg_col::OnTimer(UINT nIDEvent)
// ==========================================================================================
{
	if (m_samples_list.itemhigh!=m_samples_range.GetPos())	// line distance in EDIT was changed
		OnSetSamples();
	if (m_patterns_list.itemhigh!=m_patterns_range.GetPos())	// line distance in SUBJECT was changed
		OnSetPatterns();

	CDialog::OnTimer(nIDEvent);
}

// ==========================================================================================
	void cfg_col::OnColorSelect()
// ==========================================================================================
{
CHARFORMAT cf;
CColorDialog dlg(0);

	if (dlg.DoModal()==IDOK)
	{
		memset(&cf,0,sizeof(cf));
		cf.crTextColor=dlg.GetColor();
		cf.dwMask=CFM_COLOR;
		m_pattern.SetSel(0,-1);
		m_pattern.SetSelectionCharFormat(cf);
	}
}

// ==========================================================================================
	void cfg_col::OnSelchangePatternsList() 
// ==========================================================================================
{
CHARFORMAT cf;
CString str,txt;
int sel;

	GET_SELID(IDC_PATTERNS_LIST);
	if (sel<2)
	{
		m_pattern.SetWindowText("");
		m_ignorecase.SetCheck(0);
		SET_SELID(IDC_PATTERNS_LIST,-1);
		return;
	}
	str=(LPSTR)m_patterns_list.GetItemDataPtr(sel);
	memset(&cf,0,sizeof(cf));
	get_token(str,3,txt);
	sscanf(txt,"H%08x",&sel);
	cf.crTextColor=sel;
	cf.dwMask=CFM_COLOR;
	get_token(str,1,txt);
	m_pattern.SetWindowText(txt);
	m_pattern.SetSel(0,-1);
	m_pattern.SetSelectionCharFormat(cf);
	get_token(str,2,txt);
	m_ignorecase.SetCheck(txt==L("S_272"));
}

// ==========================================================================================
	void cfg_col::OnAddPattern()
// ==========================================================================================
{
CHARFORMAT cf;
CString txt,ign,col,str;
	
	m_pattern.GetWindowText(txt);
	if (txt.IsEmpty()) return;
	cf.dwMask=CFM_COLOR;
	m_pattern.GetSelectionCharFormat(cf);
	col.Format("H%08x",cf.crTextColor);
	if (m_ignorecase.GetCheck())	ign=L("S_272");
	str="\t"+txt+"\t"+ign+"\t"+col;
	patterns_list.AddTail(str);
	UPDATE_LB(patterns_list,IDC_PATTERNS_LIST);
	SET_SELID(IDC_PATTERNS_LIST,patterns_list.GetCount()-1);
}
	
// ==========================================================================================
	void cfg_col::OnChangePattern() 
// ==========================================================================================
{
CHARFORMAT cf;
CString txt,ign,col,str;
int sel;

	GET_SELID(IDC_PATTERNS_LIST);
	if (sel<2)	return;	// skip first 2 lines
	m_pattern.GetWindowText(txt);
	if (txt.IsEmpty()) return;
	cf.dwMask=CFM_COLOR;
	m_pattern.GetSelectionCharFormat(cf);
	col.Format("H%08x",cf.crTextColor);
	if (m_ignorecase.GetCheck())	ign=L("S_272");
	str="\t"+txt+"\t"+ign+"\t"+col;
	patterns_list.Replace(sel,str);
	UPDATE_LB(patterns_list,IDC_PATTERNS_LIST);
	SET_SELID(IDC_PATTERNS_LIST,sel);
}

// ==========================================================================================
	void cfg_col::OnDeletePattern()
// ==========================================================================================
{
int sel;

	GET_SELID(IDC_PATTERNS_LIST);
	if (sel<2)	return;	// skip first 2 lines
	patterns_list.Remove(sel);
	UPDATE_LB(patterns_list,IDC_PATTERNS_LIST);
}

// ==========================================================================================
	void cfg_col::OnTextSubj()
// ==========================================================================================
{
	GetColor(SUBJ_COLOR,patterns_list,IDC_PATTERNS_LIST);
}

// ==========================================================================================
	void cfg_col::OnBackgroundSubj()
// ==========================================================================================
{
	if (GetColor(SUBJ_BACKGR_COLOR,patterns_list,IDC_PATTERNS_LIST))
	{
		m_pattern.SetBackgroundColor(FALSE,gColors[SUBJ_BACKGR_COLOR]);
		base_colors_changed=1;
	}
}

// ==========================================================================================
	void cfg_col::OnArrowSubj()
// ==========================================================================================
{
	GetColor(NEST_COLOR,patterns_list,IDC_PATTERNS_LIST);
}

// ==========================================================================================
	int cfg_col::GetColor(int index,CStrList &lst,int id)
// ==========================================================================================
{
	CColorDialog dlg(gColors[index]);
	if (dlg.DoModal()==IDOK)
	{
		colors_changed=1;
		gColors[index]=dlg.GetColor();
		UPDATE_LB(lst,id);
		return 1;
	}
	return 0;
}
	
void cfg_col::OnSubjlistFont()	{ FipsApp.OnSetSubjectFont(); }
void cfg_col::OnTextansiFont()	{ FipsApp.OnSetMailtextFontANSI(); }
void cfg_col::OnTextoemFont()	{ FipsApp.OnSetMailtextFontOEM(); }
void cfg_col::OnEditorFont()	{ FipsApp.OnChooseFontEditor(); }
void cfg_col::OnLabelsFont()	{ FipsApp.OnFontMainLabels(); }
void cfg_col::OnFieldsFont()	{ FipsApp.OnFontMainFields(); }
	
// ==========================================================================================
	void cfg_col::OnHelp()
// ==========================================================================================
{
	WinHelp(VHELP_CFG_COLOR);
}

// ==========================================================================================
	BOOL cfg_col::OnHelpInfo(HELPINFO* pHelpInfo) 
// ==========================================================================================
{
	OnHelp();
	return TRUE;
}

// ==========================================================================================
   void init_empty_patterns(void)
// ==========================================================================================
{
CString	str;
char	buf[100];
int		i=0;

	patterns_list.RemoveAll();
	str.LoadString(IDS_EMPTY_PATTERNS);
	while (get_token(str,i++,buf,'\n'))
		patterns_list.AddTail(buf);

	sprintf(buf,"\t%s %s\t%s\tH00FF80FF\t",gc.FirstName,gc.SecondName,L("S_272"));
 	patterns_list.AddTail(buf);
}

// ==========================================================================================
   void load_sample_list(BOOL conv)
// ==========================================================================================
{
CString str,tmp;
int		i=0;

	samples_list.RemoveAll();
	str.LoadString(IDS_TEXT_SAMPLES);
	while (get_token(str,i++,tmp))
	{
		if (conv)	tmp.AnsiToOem();
		samples_list.AddTail(tmp);
	}
}

