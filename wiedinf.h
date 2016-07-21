// presinf.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// presinf dialog

class presinf : public CDialog
{
// Construction
public:
	presinf(CWnd* pParent = NULL);   // standard constructor
	void		populate_list(); 

// Dialog Data
	//{{AFX_DATA(presinf)
	enum { IDD = IDD_REPOST_FOLDER };
	CListCtrl	m_listctl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(presinf)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(presinf)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteRepost();
	afx_msg void OnHelp();
	afx_msg void OnRepostNow();
	afx_msg void OnSize	(UINT nType, int cx, int cy); 
	afx_msg void OnPaint(); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
