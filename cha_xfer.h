// cha_xfer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cha_xfer dialog

class cha_xfer : public CDialog
{
// Construction
public:
	cha_xfer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cha_xfer)
	enum { IDD = IDD_XFERFLS_EDIT };
	CString	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cha_xfer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cha_xfer)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
