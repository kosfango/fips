// detpurg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// detpurg dialog

class detpurg : public CDialog
{
// Construction
public:
	detpurg(CWnd* pParent = NULL);   // standard constructor
	int  UpdateDetails	(void);
	void AddString		(LPCSTR text); 
	void AddMessage		(LPCSTR text);
	void SetLanguage	(void);
	void SavePosition	(void); 

// Dialog Data
	//{{AFX_DATA(detpurg)
	enum { IDD = IDD_STAT_PURGE };
	CEdit	m_status;
	CListBox	m_list;
	CProgressCtrl	m_area_proz;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(detpurg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char	m_message[100];

	// Generated message map functions
	//{{AFX_MSG(detpurg)
	afx_msg void OnPurgeAll();
	virtual void OnCancel();
	afx_msg void OnClear();
	afx_msg void OnHelp();
	afx_msg	void OnSize  (UINT nType, int cx, int cy); 
	afx_msg void OnPaint (); 
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPurgeone();
};
