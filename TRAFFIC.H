// traffic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// traffic dialog

class traffic : public CDialog
{
// Construction
public:
	traffic(CWnd* pParent = NULL);   // standard constructor
	void 	insert_area			(LPCSTR area);
	int 	find_area			(LPCSTR area);
	int 	create_statistic	(void);
	int 	display_the_stuff	(void);

// Dialog Data
	//{{AFX_DATA(traffic)
	enum { IDD = IDD_TRAFFIC };
	CListCtrl	m_traffic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(traffic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(traffic)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClearStatistics();
	afx_msg void OnHelp();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint(); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
