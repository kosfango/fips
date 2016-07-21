// costbase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// costbase dialog

class costbase : public CDialog
{
// Construction
public:
	costbase(CWnd* pParent = NULL);   // standard constructor
	void	String2Disp(CString &line);
	void	Disp2String(CString &line);

// Dialog Data
	//{{AFX_DATA(costbase)
	enum { IDD = IDD_COST_BASE };
	CString	m_cost;
	CString	m_name;
	CString	m_normal;
	CString	m_unit;
	CString	m_weekend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(costbase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(costbase)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnSet();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
