// reqsel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// reqsel dialog

class reqsel : public CDialog
{
// Construction
public:
	reqsel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(reqsel)
	enum { IDD = IDD_FREQ_SELECT };
	CListBox	m_systems;
	CListBox	m_files;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(reqsel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(reqsel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	virtual void OnCancel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
