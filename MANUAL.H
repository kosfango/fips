// manual.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// manual dialog

class manual : public CDialog
{
// Construction
public:
	manual(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(manual)
	enum { IDD = IDD_MANUAL_AREA_ORDER };
	CString	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(manual)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(manual)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
