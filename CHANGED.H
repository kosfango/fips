// changed.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// changed dialog

class changed : public CDialog
{
// Construction
public:
	changed(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(changed)
	enum { IDD = IDD_CHANGE_DEST };
	CEdit	m_phonecontrol;
	CString	m_password;
	CString	m_phone;
	CString	m_system;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(changed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(changed)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnOtherlines();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
