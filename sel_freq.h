// sel_freq.h : header file
//
#include "myeditfr.h"

/////////////////////////////////////////////////////////////////////////////
// sel_freq dialog

class sel_freq : public CDialog
{
// Construction
public:
	sel_freq(CWnd* pParent = NULL);   // standard constructor
	virtual ~sel_freq();
	void find_in_flist(int caseses);
	void DisplayFromLine(UINT startline);
	void key_top();
	void key_pgdown();	
	void key_pgup();	
	void key_bottom();	
	void key_up();	
	void key_down();
	char freq[1000];


// Dialog Data
	//{{AFX_DATA(sel_freq)
	enum { IDD = IDD_FILELIST_REQUEST };
	CButton	m_oem;
	CScrollBar	m_scroll;
	CEdit	m_gotoedit;
	CEdit	m_search;
	CEdit	m_list;
	myeditfr m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(sel_freq)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT	displines;	// number of lines in textbox
	double	norm;	// divider for scrollbar normalization
	UINT	*lineoffs;	// offsets of lines in file
	FILE 	*fpseek;

	// Generated message map functions
	//{{AFX_MSG(sel_freq)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnAdd();
	afx_msg void OnCaseinsens();
	afx_msg void OnCasesens();
	afx_msg void OnGoto();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTop();
	afx_msg void OnPgdown();
	afx_msg void OnPgup();
	afx_msg void OnBottom();
	afx_msg void OnHelp();
	afx_msg	void OnSize(UINT nType, int cx, int cy); 
	afx_msg void OnPaint(); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnAnsi();
	afx_msg void OnOem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
