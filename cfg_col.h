// cfg_col.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_col dialog

class cfg_col : public CSAPrefsSubDlg
{
protected: 
	CColorListBox m_samples_list;
	CColorListBoxSubject m_patterns_list;
	int GetColor(int index,CStrList &lst,int id);

// Construction
public:
	cfg_col(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName		(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_col)
	enum { IDD = IDD_CFG_COLOR };
	CRichEditCtrl	m_pattern;
	CButton	m_oem;
	CButton	m_ignorecase;
	CListBox	m_list;
	CSliderCtrl	m_patterns_range;
	CSliderCtrl	m_samples_range;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_col)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_col)
	virtual BOOL OnInitDialog();
	afx_msg void OnBasecolor();
	afx_msg void OnInformation();
	afx_msg void OnNormaltext();
	afx_msg void OnOddQuote();
	afx_msg void OnEvenQuote();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnOrigin();
	afx_msg void OnSetSamples();
	afx_msg void OnSetPatterns();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAddPattern();
	afx_msg void OnDeletePattern();
	afx_msg void OnChangePattern();
	afx_msg void OnBackgroundSubj();
	afx_msg void OnTextSubj();
	afx_msg void OnArrowSubj();
	afx_msg void OnColorSelect();
	afx_msg void OnBoldButton();
	afx_msg void OnTextconv();
	afx_msg void OnSubjlistFont();
	afx_msg void OnTextansiFont();
	afx_msg void OnTextoemFont();
	afx_msg void OnEditorFont();
	afx_msg void OnLabelsFont();
	afx_msg void OnFieldsFont();
	afx_msg void OnSelchangePatternsList();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
