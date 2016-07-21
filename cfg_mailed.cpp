#include "stdafx.h"
#include "resource.h"
#include "cfg_mailed.h"
#include "structs.h"
#include "editwnd.h"
#include "regexp.h"
#include <ctype.h>

static char DlgName[]="IDD_CFG_MAILEDITOR";
// cfg_mailed dialog

extern COLORREF TextWndColors[NUM_EDIT_COLORS];

IMPLEMENT_DYNAMIC(cfg_mailed, CSAPrefsSubDlg)
cfg_mailed::cfg_mailed(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(cfg_mailed::IDD, pParent)
	, m_nRMargin(0)
	, m_bAutoIndent(FALSE)
	, m_bOemCharset(FALSE)
	, m_bWordWrap(FALSE)
	, m_nTabSize(0)
	, m_bEqualTabs(FALSE)
	, m_bReplaceTabs(FALSE)
	, m_nFontSize(0)
	, m_FontName(_T(""))
	, m_bShowTabs(FALSE)
	, m_nCurColor(0)
	, m_bColorMark(FALSE)
{
	m_nRMargin = get_cfg(CFG_MAILED, "RightMargin", 0);
	m_bAutoIndent = get_cfg(CFG_MAILED, "AutoIndent", 1);
	m_bOemCharset = get_cfg(CFG_MAILED, "OemCharset", 1);
	m_bWordWrap = get_cfg(CFG_MAILED, "WordWrap", 1);
	m_nTabSize = get_cfg(CFG_MAILED, "TabSize", 4);
	m_bEqualTabs = get_cfg(CFG_MAILED, "EqualTabs", 1);
	m_bReplaceTabs = get_cfg(CFG_MAILED, "ReplaceTabs", 0);
	m_nFontSize = get_cfg(CFG_MAILED, "FontSize", 10);
	m_bShowTabs = get_cfg(CFG_MAILED, "ViewTabs", 0);
	m_FontName = get_cfg(CFG_MAILED, "FontName", "Courier New");
	m_bColorMark = get_cfg(CFG_MAILED, "ColorMark", 1);
	
	memcpy(m_aColors, TextWndColors, sizeof(COLORREF) * NUM_EDIT_COLORS);
	m_Color.currentcolor = m_aColors[m_nCurColor];
	m_nLastColor = m_nCurColor;
}

void ApplyEditWndOptions(CEditWnd *wnd)
{
	wnd->SetScreenChars(get_cfg(CFG_MAILED, "RightMargin", 0));
	wnd->SetAutoIndent(get_cfg(CFG_MAILED, "AutoIndent", 1));
	wnd->SetOemCharset(get_cfg(CFG_MAILED, "OemCharset", 1));
	wnd->SetWordWrapping(get_cfg(CFG_MAILED, "WordWrap", 1));
	wnd->SetTabSize(get_cfg(CFG_MAILED, "TabSize", 4));
	wnd->SetEqualTabs(get_cfg(CFG_MAILED, "EqualTabs", 0));
	wnd->SetReplaceTabs(get_cfg(CFG_MAILED, "ReplaceTabs", 0));
	wnd->SetViewTabs(get_cfg(CFG_MAILED, "ViewTabs", 0));
	wnd->SetColorMark(get_cfg(CFG_MAILED, "ColorMark", 0));
	
	LOGFONT lf;
	wnd->GetFont(lf);
	strcpy(lf.lfFaceName, get_cfg(CFG_MAILED, "FontName", "Courier New"));
	HDC hDC = ::GetDC(wnd->m_hWnd);
	lf.lfHeight = -MulDiv(get_cfg(CFG_MAILED, "FontSize", 10), GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(wnd->m_hWnd, hDC);
	wnd->SetFont(lf);
}

cfg_mailed::~cfg_mailed()
{
}

void cfg_mailed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RMARGIN, m_nRMargin);
	DDX_Check(pDX, IDC_AUTOINDENT, m_bAutoIndent);
	DDX_Check(pDX, IDC_USEDOS, m_bOemCharset);
	DDX_Check(pDX, IDC_WORDWRAP, m_bWordWrap);
	DDX_Text(pDX, IDC_TABSIZE, m_nTabSize);
	DDX_Check(pDX, IDC_EQUALTABS, m_bEqualTabs);
	DDX_Check(pDX, IDC_TABTOSPACE, m_bReplaceTabs);
	DDX_Control(pDX, IDC_FONTFACE, m_FontsList);
	DDX_Text(pDX, IDC_EDITFONTSZ, m_nFontSize);
	DDX_CBString(pDX, IDC_FONTFACE, m_FontName);
	DDX_Check(pDX, IDC_SHOWTABS, m_bShowTabs);
	DDX_Control(pDX, IDC_COLOR, m_Color);
	DDX_CBIndex(pDX, IDC_COLORS, m_nCurColor);
	DDX_Check(pDX, IDC_COLORMARK, m_bColorMark);
}


BEGIN_MESSAGE_MAP(cfg_mailed, CSAPrefsSubDlg)
	ON_CBN_SELCHANGE(IDC_COLORS, OnCbnSelchangeColors)
	ON_BN_CLICKED(IDC_USEDOS, OnBnClickedUsedos)
	ON_CBN_SELCHANGE(IDC_FONTFACE, OnCbnSelchangeFontface)
END_MESSAGE_MAP()


// =====================================================================
LPCSTR cfg_mailed::GetName(void)	{return DlgName;}
// =====================================================================

// cfg_mailed message handlers

BOOL cfg_mailed::OnInitDialog()
{
int lng[]={
		IDC_STATIC1,
		IDC_STATIC2,
		IDC_STATIC3,
		IDC_STATIC4,
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

	FillFontsList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void cfg_mailed::OnOK()
{
	set_cfg(CFG_MAILED, "RightMargin", m_nRMargin, FALSE);
	set_cfg(CFG_MAILED, "AutoIndent", m_bAutoIndent, FALSE);
	set_cfg(CFG_MAILED, "OemCharset", m_bOemCharset, FALSE);
	set_cfg(CFG_MAILED, "WordWrap", m_bWordWrap, FALSE);
	set_cfg(CFG_MAILED, "TabSize", m_nTabSize, FALSE);
	set_cfg(CFG_MAILED, "EqualTabs", m_bEqualTabs, FALSE);
	set_cfg(CFG_MAILED, "ReplaceTabs", m_bReplaceTabs, FALSE);
	set_cfg(CFG_MAILED, "FontSize", m_nFontSize, FALSE);
	set_cfg(CFG_MAILED, "FontName", m_FontName, FALSE);
	set_cfg(CFG_MAILED, "ViewTabs", m_bShowTabs, FALSE);
	set_cfg(CFG_MAILED, "ColorMark", m_bColorMark);
	
	memcpy(TextWndColors, m_aColors, sizeof(COLORREF) * NUM_EDIT_COLORS);

	//extern CEditWnd *editptr;
	extern CEditWnd *gEdit;
	if (gEdit)
		ApplyEditWndOptions(gEdit);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}


void cfg_mailed::OnCbnSelchangeColors()
{
	UpdateData();
	m_aColors[m_nLastColor] = m_Color.currentcolor;
	m_Color.currentcolor = m_aColors[m_nCurColor];
	m_Color.Invalidate();
	m_Color.UpdateWindow();
	m_nLastColor = m_nCurColor;
}

int CALLBACK EnumFontFamProc(CONST LOGFONTA *lplf, CONST TEXTMETRICA *, DWORD, LPARAM lParam)
{
	CComboBox *cb = (CComboBox *)lParam;
	ASSERT(cb);
	if (cb->FindString(-1, lplf->lfFaceName) == CB_ERR)
		cb->AddString(lplf->lfFaceName);
		
	return 1;
}

void cfg_mailed::FillFontsList()
{
	UpdateData();

	LOGFONT lf;
	lf.lfCharSet = m_bOemCharset ? OEM_CHARSET : DEFAULT_CHARSET;
	lf.lfFaceName[0] = 0;
	lf.lfPitchAndFamily = 0;

	HDC hDC = ::GetDC(0);
	m_FontsList.ResetContent();
	EnumFontFamiliesEx(hDC, &lf, EnumFontFamProc, (LPARAM)&m_FontsList, 0);
	::ReleaseDC(0, hDC);
	
	m_FontsList.SelectString(-1, m_FontName);
}

void cfg_mailed::OnBnClickedUsedos()
{
	FillFontsList();
}

void cfg_mailed::OnCbnSelchangeFontface()
{
	UpdateData();
}

/*char *UndoDescriptions[] =
{
  "",
  "вставку",
  "удаление выделения",
  "вырезание",
  "ввод",
  "стирание последнего символа",
  "отступ",
  "перетаскивание",
  "замену",
  "удаление",
  "автоотступ",
  "вставку файла"
};

void InsertTextToBuffer(const char *text, CTextBuffer *buf)
{
	ASSERT(buf);
	int x = 0, y = 0, line = buf->GetLineCount() - 1;
	int len = buf->GetLineLength(line);
	if (len > 0 || line > 0)
		buf->DeleteText(0, 0, 0, line, len, CE_ACTION_UNKNOWN);
	buf->InsertText(0, 0, 0, text, y, x, CE_ACTION_UNKNOWN); 
}

void GetBufferText(CTextBuffer *buf, CString& text)
{
	ASSERT(buf);
	string str;
	int x = 0, y = 0, line = buf->GetLineCount() - 1;
	int len = buf->GetLineLength(line);
	if (len > 0 || line > 0)
		buf->GetText(0, 0, line, len, str);
	text = str.c_str();		
}

void ReplaceSelection(CTextBuffer *buf, CTextWnd *wnd, const char *newtext)
{
	ASSERT(buf);
	CPoint pt1, pt2;
	int y, x;
	wnd->GetSelection(pt1, pt2);
	if (pt2.y < buf->GetLineCount() && pt2.x < buf->GetLineLength(pt2.y))
		buf->DeleteText(wnd, pt1.y, pt1.x, pt2.y, pt2.x, CE_ACTION_UNKNOWN);
	buf->InsertText(wnd, pt1.y, pt1.x, newtext, y, x, CE_ACTION_UNKNOWN);
}

const char *str_str(const char *str1, const char *str2, bool useCase)
{
	int len1 = strlen(str1), len2 = strlen(str2);
	int len = min(len1, len2);
	char b1[2] = { 0, 0 }, b2[2] = { 0, 0 };
	for (int j = 0; j <= len1 - len2; j++)
	{
		for (int i = 0; i < len; i++)
		{
			if (useCase) if (str1[j + i] != str2[i]) break;
			if (!useCase)
			{
				b1[0] = toupper(str1[j + i]); b2[0] = toupper(str2[i]);
				if (b1[0] != b2[0]) break;
			}
		}
		if (i == len) return str1 + j;
	}
	return 0;
}

int HowManyStr(const char *s, const char *m)
{
  const char *p = s;
  int n = 0, l = _tcslen (m);
  while ((p = strstr(p, m)) != 0)
	{
		n++;
    p += l;
	}
  return n;
}

bool FindStringHelper(CTextBuffer *pBuf, bool casesens, bool useRx, const char *what, CPoint& found, int& len)
{
	CString text;
	RxMatchRes match_res;
	CPoint pt(0,0);
	int pos = -1, nEolns = 0, last_len = -1;
	len = 0;
	RxNode *rx = 0;

	if (useRx)
	{
		rx = RxCompile(what);
		nEolns = HowManyStr(what, "\\n");
	}

	text = pBuf->GetLineChars(pt.y) + pt.x, pBuf->GetLineLength(pt.y) - pt.x;
	
	while (pos == -1)
	{
		if (rx)
		{
			if (RxExec(rx, text, text.GetLength(), text, &match_res, casesens) != 0)
			{
				pos = match_res.Open[0];
				len = match_res.Close[0] - match_res.Open[0];
			}
		}
		else
		{
			char *match = (char*)str_str(text, what, casesens);
			if (match != 0) 
			{
				pos = match - text;
				len = strlen(what);
			}
		}
		if (pos == -1)
		{
			if (++pt.y >= pBuf->GetLineCount()) break;
			text += '\n';
			last_len = text.GetLength();
			text += CString(pBuf->GetLineChars(pt.y),	pBuf->GetLineLength(pt.y));
		}
	}
	if (pos >= 0)
	{
		found.y = pt.y - nEolns;
		if (nEolns == 0)
			if (last_len != -1) found.x = pos - last_len;
			else found.x = pos + pt.x;
		else
		{
			char *pos1 = strstr(what, "\\n");
			ASSERT(pos1 != 0);
			found.x = pBuf->GetLineLength(found.y) - (int)(pos1 - what);
		}
	}
	return pos >= 0;
}
*/