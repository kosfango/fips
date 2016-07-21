// logdisp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// logdisp dialog

class logdisp : public CDialog
{
// Construction
public:
	logdisp(CWnd* pParent = NULL);   // standard constructor
	void DisplayFile	(char *filename);

// Dialog Data
	//{{AFX_DATA(logdisp)
	enum { IDD = IDD_LOGFILE_DISPLAY };
	CEdit	m_type;
	CEdit	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(logdisp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(logdisp)
	afx_msg void OnMailerLogfile();
	afx_msg void OnPurgerLogfile();
	afx_msg void OnTosserLogfile();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnEventsLogfile();
	afx_msg void OnNodelistLogfile();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint(); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
