#if !defined(cfg_mailed_h)
#define cfg_mailed_h

// cfg_mailed dialog

#include "SAPrefsSubDlg.h"
#include "colorbtn.h"
#include "textwnd.h"

void load_textwnd_colors();
void store_textwnd_colors();

class cfg_mailed : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(cfg_mailed)
	COLORREF m_aColors[NUM_EDIT_COLORS];
	int m_nLastColor;
	void FillFontsList();
public:
	cfg_mailed(CWnd* pParent = NULL);   // standard constructor
	virtual ~cfg_mailed();
	LPCSTR  GetName(void);

// Dialog Data
	enum { IDD = IDD_CFG_EDITOR };
		
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nRMargin;
	BOOL m_bAutoIndent;
	BOOL m_bOemCharset;
	BOOL m_bWordWrap;
	int m_nTabSize;
	BOOL m_bEqualTabs;
	BOOL m_bReplaceTabs;
	CComboBox m_FontsList;
	int m_nFontSize;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CString m_FontName;
	BOOL m_bShowTabs;
	CColorBtn m_Color;
	int m_nCurColor;
	afx_msg void OnCbnSelchangeColors();
	afx_msg void OnBnClickedUsedos();
	BOOL m_bColorMark;
	afx_msg void OnCbnSelchangeFontface();
};

class CEditWnd;
//void ApplyEditWndOptions(CEditWnd *wnd);
//void InsertTextToBuffer(const char *text, CTextBuffer *buf);
//void GetBufferText(CTextBuffer *buf, CString& text);
//void ReplaceSelection(CTextBuffer *buf, CTextWnd *wnd, const char *newtext);
//bool FindStringHelper(CTextBuffer *pBuf, bool casesens, bool rx, const char *what, CPoint& found, int& len);

#endif
