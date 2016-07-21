// emergenc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// emergenc dialog

class emergenc : public CDialog
{
// Construction
public:
	emergenc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(emergenc)
	enum { IDD = IDDIALBREAK };
	CEdit	m_edit_delay;
	CEdit	m_edit_seconds;
	CEdit	m_edit_maximum;
	CButton	m_enabled;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(emergenc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(emergenc)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnReset();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
