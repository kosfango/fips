// exbrow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// exbrow dialog

class exbrow : public CDialog
{
// Construction
public:
	exbrow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(exbrow)
	enum { IDD = IDD_START_BROWSER };
	CListBox	m_list;
	CString	m_startcmd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(exbrow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(exbrow)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnDblclkList();
	afx_msg void OnSelchangeList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
