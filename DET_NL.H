// det_nl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// det_nl dialog

class det_nl : public CDialog
{
// Construction
public:
	det_nl(CWnd* pParent = NULL);   // standard constructor
	void AddString					(char *text); 
	void SetLanguage				(void);
	void SavePosition (void); 
	afx_msg void OnStart();
	virtual void OnCancel();

// Dialog Data
	//{{AFX_DATA(det_nl)
	enum { IDD = IDD_STAT_NLCOMP };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(det_nl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(det_nl)
	afx_msg void OnClear();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnClose();
	afx_msg void OnCheck();
	afx_msg	void OnSize  (UINT nType, int cx, int cy); 
	afx_msg void OnPaint (); 
	afx_msg void OnMove(int x, int y);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
