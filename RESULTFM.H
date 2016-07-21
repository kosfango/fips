// resultfm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// resultfm dialog

class resultfm : public CDialog
{
// Construction
public:
	resultfm(CWnd* pParent = NULL);   // standard constructor
	int		TestConsistency(void);

// Dialog Data
	//{{AFX_DATA(resultfm)
	enum { IDD = IDD_RESULT_FORMAT };
	CStatic	m_warning;
	CEdit	m_edit_tabs;
	CEdit	m_edit_format;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(resultfm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(resultfm)
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg void OnDefault();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnApply();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
