// search.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// search dialog

class search : public CDialog
{
// Construction
public:
	search(CWnd* pParent = NULL);   // standard constructor
	virtual void OnOK();

// Dialog Data
	//{{AFX_DATA(search)
	enum { IDD = IDD_SIMPLE_SEARCH };
	CEdit	m_status;
	int		m_direction;
	BOOL	m_convert;
	BOOL	m_header;
	BOOL	m_highlight;
	BOOL	m_ignorecase;
	BOOL	m_messagetext;
	CString	m_pattern;
	BOOL	m_multiselect;
	//}}AFX_DATA
protected:
	void show_result(char *st,struct mailheader *pmh,int ind,int offs);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(search)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(search)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnHelp();
	afx_msg void OnExtsearch();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
