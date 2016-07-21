// costanal.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// costanal dialog

class costanal : public CDialog
{
// Construction
public:
	costanal(CWnd* pParent = NULL);   // standard constructor
	void		test_and_append			(struct _costinfo* ci);
	int			find_this_fido_system	(char	*tofind);


// Dialog Data
	//{{AFX_DATA(costanal)
	enum { IDD = IDD_COSTANAL };
	CListCtrl	m_list;
	CEdit	m_edit_startdate;
	CEdit	m_edit_enddate;
	CButton	m_check_supress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(costanal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(costanal)
	virtual BOOL OnInitDialog();
	afx_msg void OnDefault();
	afx_msg void OnDisplay();
	virtual void OnCancel();
	afx_msg void OnHelp();
	afx_msg void OnDisplayAll();
	afx_msg void OnClearInfo();
	afx_msg void OnDisplayToday();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint(); 
	afx_msg void OnWriteFile();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
