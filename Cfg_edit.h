// cfg_edit.h : header file
//
#include "SAPrefsSubDlg.h"
#include "TextWnd.h"
#include "colorbtn.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_edit dialog

class cfg_edit : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_edit(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_edit)
	enum { IDD = IDD_CFG_EDITOR };
	CEdit	m_editorwidth;
	CEdit	m_tearline;
	CEdit	m_maxuue;
	BOOL	m_golded_comp;
	BOOL	m_hide_leading;
	BOOL	m_immed_update;
	BOOL	m_colored;
	BOOL	m_first_non;
	BOOL	m_autoscroll;
	BOOL	m_altcolored;
	BOOL	m_last_read;
	BOOL	m_clear_tearline;
	BOOL	m_netorigin;
	BOOL	m_structured;
	BOOL	m_area_last;
	BOOL	m_repquote;
	BOOL	m_multispace;
	BOOL	m_ndlprefer;
	BOOL	m_filter_re;
	BOOL	m_no_res;
	BOOL	m_no_detsearch;
	int		m_nRMargin;
	BOOL	m_bAutoIndent;
	BOOL	m_bOemCharset;
	BOOL	m_bWordWrap;
	int		m_nTabSize;
	BOOL	m_bEqualTabs;
	BOOL	m_bReplaceTabs;
	BOOL	m_bShowTabs;
	CColorBtn m_Color;
	int		m_nCurColor;
	BOOL	m_bColorMark;
	COLORREF m_aColors[NUM_EDIT_COLORS];
	int		m_nLastColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_edit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_edit)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeColors();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEditorFont();
};
