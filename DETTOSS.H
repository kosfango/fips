// dettoss.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// dettoss dialog

class dettoss : public CDialog
{
// Construction
public:
	dettoss(CWnd* pParent = NULL);   // standard constructor
	void AddString		(char *text); 
	void UpdateValues	(void); 
	void SetLanguage	(void);
	void SavePosition	(void); 
	afx_msg void OnStart();


// Dialog Data
	//{{AFX_DATA(dettoss)
	enum { IDD = IDD_STAT_TOSS };
	CButton	m_ask;
	CEdit	m_status;
	CListBox	m_list;
	CProgressCtrl	m_proz;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(dettoss)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(dettoss)
	virtual void OnCancel();
	afx_msg void OnClear();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnClose();
	afx_msg	void OnSize  (UINT nType, int cx, int cy); 
	afx_msg void OnPaint (); 
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
